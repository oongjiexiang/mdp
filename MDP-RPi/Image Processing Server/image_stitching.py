from PIL import Image
import os

class image_stitching:
    def __init__(self):
        self.directory_path = os.path.dirname(os.path.realpath(__file__)) + "\processed_images"
        self.images_list = os.listdir(self.directory_path)
        self.images = []

    def stitching(self):
        for i,img in enumerate(self.images_list):
            image = Image.open(f"{self.directory_path}\{self.images_list[i]}")
            self.images.append(image)

        image_num=0
        row = None
        stop = False
        new_image = "";
        x=0
        y=0        
        
        if(len(self.images) == 0):
            print('No images in folder')
        else:
            if (len(self.images) == 1):
                new_image = Image.new('RGB', (self.images[0].size[0], self.images[0].size[0]), (250,250,250))
                row = 1
            else: 
                if(len(self.images) == 2):
                    row = 2
                elif(len(self.images)%2 == 0):
                    row = int(len(self.images)/2)
                else:
                    row = int(len(self.images)/2)+1

                new_image = Image.new('RGB', (row*self.images[1].size[0], 2*self.images[1].size[1]), (250,250,250))

            #Paste the object detected images 
            for i in range(row):
                for j in range(row):
                    if(image_num>=len(self.images)):
                        stop = True
                        break

                    new_image.paste(self.images[image_num],(x,y))
                    x += self.images[0].size[0]
                    image_num += 1
                x=0
                y+=self.images[j].size[1]
                if(stop == True):
                    break
            
            new_image.show()
            new_image.save(self.directory_path+ "_stitched_final.png")