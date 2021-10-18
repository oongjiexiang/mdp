import os
from numpy import random
from datetime import datetime
import imagezmq
import shutil
import cv2
import time
from pathlib import Path
import torch

from models.experimental import attempt_load
from utils.datasets import LoadImages
from utils.general import check_img_size, non_max_suppression, scale_coords, set_logging
from utils.plots import plot_one_box
from utils.torch_utils import select_device, time_sync
from image_stitching import image_stitching

#Image Stuff
image_size = 640
image_encoding = '.png'

image_hub = imagezmq.ImageHub()

conf_thres=0.75  # confidence threshold
iou_thres=0.45  # NMS IOU threshold
results = {} #Results to store processed_image_id
device='0' # cuda device, i.e. 0 or 0,1,2,3 or cpu

#Raw Image File Path
raw_image_filepath = os.path.dirname(os.path.realpath(__file__)) + "/raw_images/"
processed_raw_image_filepath = os.path.dirname(os.path.realpath(__file__)) + "/raw_images/processed_raw_images/"

#Processed Image FIle Path
processed_image_filepath = os.path.dirname(os.path.realpath(__file__)) + "/processed_images/"

# Location of the models
#models = 'models/best_yolov5m_included_bulleyes.pt' # yolov5m bulleye included
#models = 'models/best_yolov5s_included_bulleyes.pt' # yolov5s bulleye included
#models = 'models/best_yolov5m_no_bulleyes.pt' #v5m model, no bulleye included
models = 'models/best_yolov5m_no_bulleyes_latest.pt'#v5m latest model, no bulleye included

 # Initialize
set_logging()
device = select_device(device)

# Load model
print('[IPS Setup] Loading model...')
model = attempt_load(models, map_location=device)
stride = int(model.stride.max())
imgsz = check_img_size(image_size, s=stride)

print('\n*************Started Image Processing Server*************\n')
while True: 
    message = '';
    img_id = 0;
    detected_img = False

    print('Waiting for image from RPI......', flush=True)
    rpi_hostname, image = image_hub.recv_image()
    # form image file path for saving
    raw_image_name = rpi_hostname.replace(':', '') + ' ' + str(datetime.now().strftime('%d-%b_%H-%M-%S')) + image_encoding
    raw_image_path = os.path.join(raw_image_filepath, raw_image_name)
    
    #Save Raw Image
    save_success = cv2.imwrite(raw_image_path, image)

    rpi_hostname_list = list(rpi_hostname.split('_'))
    save_dir = Path(processed_image_filepath)
    save_dir.mkdir(parents=True, exist_ok=True)

    processed_image_id = [] #List to store [0] id, [1] conf, [2] filepath
    dataset = LoadImages(raw_image_filepath, img_size=imgsz, stride=stride)

    # Get names and colors
    names = model.module.names if hasattr(model, 'module') else model.names
    colors = [[random.randint(0, 255) for _ in range(3)] for _ in names]
    model(torch.zeros(1, 3, imgsz, imgsz).to(device).type_as(next(model.parameters())))

    t0 = time.time()
    for path, img, im0s, vid_cap in dataset:
        img = torch.from_numpy(img).to(device)
        img = img.float()
        img /= 255.0
        if img.ndimension() == 3:
            img = img.unsqueeze(0)

        # Inference
        t1 = time_sync()
        pred = model(img)[0]
        # NMS
        pred = non_max_suppression(pred, conf_thres, iou_thres, max_det = 1)
        t2 = time_sync()

        # Process detections
        for i, det in enumerate(pred):  # detections per image
            p, s, im0, frame = path, '', im0s, getattr(dataset, 'frame', 0)

            p = Path(p) # to Path
            save_path = str(save_dir / p.name) # img.jpg
            s += '%gx%g ' % img.shape[2:] # print string
            gn = torch.tensor(im0.shape)[[1, 0, 1, 0]] # normalization gain whwh
            if len(det):
                # Rescale boxes from img_size to im0 size
                det[:, :4] = scale_coords(img.shape[2:], det[:, :4], im0.shape).round()

                # Print results
                for c in det[:, -1].unique():
                    n = (det[:, -1] == c).sum()  # detections per class
                    s += f"{n} {names[int(c)]}{'s' * (n > 1)}, "  # add to string

                # Write results
                for *xyxy, conf, cls in reversed(det):
                    # Add bbox to image
                    if names[int(cls)]:
                        object_id = names[int(cls)]
                    else: object_id = ''

                    label = f'{object_id}, {conf:.2f}'
                    plot_one_box(xyxy, im0, label=label, color=colors[int(cls)], line_thickness=2)
                    img_conf = "{:.2f}".format(conf.item())
                    print('\nImage Detected: %s (Conf: %s)\n' % (str(object_id),str(img_conf)))
                    #append the value into the list
                    processed_image_id.append((object_id, img_conf, processed_image_filepath + p.name)) 
                    img_id = object_id
                    detected_img = True

            # Print (inference + NMS) time 
            print(f'{s}Done: {t2 - t1:.3f}s')

            # Save image with detection
            if dataset.mode == 'image':
                cv2.imwrite(save_path, im0)

                # Check if there is object detected in the image file
                if detected_img:
                    for i in processed_image_id:
                        id = i[0] #Image ID
                        conf = i[1] #Confidence
                        filepath = i[2] #Filepath
                        if id in results: # Compare the confidence of the same image id 
                            print('ID has been detected before')
                            if float(conf) > float(results[id][1]):
                                print('Replacing existing image as confidence higher')
                                os.remove(results[id][2]) #Remove the image file from the folder
                                del results[id] #remove existing result from dict
                                results[id] = i #add new result to dict.
                            else:
                                print('Keeping existing image and removing new image as confidence lower')
                                os.remove(filepath) #Remove the image file from the folder
                                pass
                        else:
                            print('New ID. Saving to results dict.') #Save new image id to dict.
                            results[id] = i
                else:
                    os.remove(processed_image_filepath + p.name) #Remove no object detected image file from the folder

            #Move the iamge file to processed raw image folder
            shutil.move(raw_image_filepath + p.name, processed_raw_image_filepath + p.name) 

    print(f"Results saved to {save_dir}")

    print(f'Time to process ({time.time() - t0:.3f}s)')

    #Always display the stitched images whenever processed_images folder got image
    print("*************Start stitching images*************")
    stitch_images_program = image_stitching()
    stitch_images_program.stitching()
    print("*************Finished stitching images*************")
    
    #Messages will be sent back to RPI
    if str(img_id) == "31" or str(img_id) == "0":
        message = "n"
    else:
        message = str(img_id);

    cv2.waitKey(1)
    print("*****Message sending: " + message)
    #Sending messages back to RPI
    image_hub.send_reply(message.encode('utf-8'))
