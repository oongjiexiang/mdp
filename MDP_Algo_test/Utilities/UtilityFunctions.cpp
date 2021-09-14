#include<iostream>
#include<string>
#include<vector>
#include<math.h>
#include "component.h"


//find the location where the robot should move to in order to search for image
double* findTargetLocation(double obstacleX, double obstacleY, std::string obstacleFacingDirection, double* retValue){
    if(obstacleFacingDirection.compare("UP")==0){
         retValue[0]=obstacleX;
        retValue[1]=obstacleY+30;
    }
    else if(obstacleFacingDirection.compare("DOWN")==0){
        retValue[0]=obstacleX;
        retValue[1]=obstacleY-30;
    }
    else if(obstacleFacingDirection.compare("LEFT")==0){
        retValue[0]=obstacleX-30;
        retValue[1]=obstacleY;
    }
    else if(obstacleFacingDirection.compare("RIGHT")==0){
        retValue[0]=obstacleX+30;
        retValue[1]=obstacleY;
    }
    else{
            printf("error in obstacle facing direction");
            retValue[0]=obstacleX;
            retValue[1]=obstacleY;
    }
    return retValue;
}
//run both functions below when checking if there is an obstacle in the path
//function to check if robot's bounding box at any location will intersect with an obstacle bounding bo
bool boundingBox(double x1, double y1, double objectSize1, double x2, double y2, double objectSize2){
    double halfObstacleSize1;
    double halfObstacleSize2;
    if(objectSize1==0 || objectSize2==0){
        printf("object size must be greater than 0");
        return true;
    }
    halfObstacleSize1 = objectSize1/2;
    halfObstacleSize2 = objectSize2/2;
        //following the bounding box intersect equation from the slides
    if((x2+halfObstacleSize2>=x1-halfObstacleSize1) && (x1+halfObstacleSize1>=x2-halfObstacleSize2) && (y2+halfObstacleSize2>=y1-halfObstacleSize1) && (y1+halfObstacleSize1>=y2-halfObstacleSize2)){
            return true;
    }
    return false;
}
//checks if 2 lines intersect (obstacle boundary line and path traveled line), returns true if line straddles
bool linesStraddle(double object_X, double object_Y, double obstacleLength, double xStart, double xEnd, double yStart, double yEnd){
    double x1, x2, x3, y1, y2, y3,eq1, eq2;
    //p2,p3 -> obstacle lines
    //p0,p1 -> robot path
    //p0 = x1, y1
    //p1 = x2, y2
    //p2 or p3 = x3, y3

    x1 = xStart;
    y1 = yStart;
    x2 = xEnd;
    y2 = yEnd;
    //5 is the length of an obstacle from the center
    //this is the bot left point of the obstacle
    x3 = object_X-5;
    y3 = object_Y-5;
    eq1 = ((x3-x1)*(y2-y1)) - ((y3-y1)*(x2-x1));
    //this is the top right point of the obstacle
    x3 = object_X+5;
    y3 = object_Y+5;
    eq2 = ((x3-x1)*(y2-y1)) - ((y3-y1)*(x2-x1));

    if(eq1==0 && eq2==0){
        //on the same line
        return true;
    }
    if(eq1>0 && eq2>0){
        //line straddles
        return true;
    }
    else if(eq1<0 && eq2<0){
        return true;
    }

    return false;
}

//receives a message from STM
std::string avoidObstacle(std::string sensorMessage){
    std::string newCommand;
    if(sensorMessage.compare("b")==0){ //assuming message 'b' means 20 cm from obstacle
        //reverse 10cm, turn right45,turn left45, turn left45, turn right 45
        newCommand = "f|h|g|g|h";
    }
    //call A* again here to find a new path maybe
    return newCommand;
}

//define a set pattern that the robot will travel in if the image deteced is a bullseye image and check that the image is on the target obstacle, returns instructions to move the robot
//change image detected into string corresponding to the message that RPi sends us
//***REFER TO COMMAND LIST TO SEE WHAT THE INSTRUCTIONS MEAN
std::string aroundObstacleTillDetect(char imageDetected, Vertex targetVertex, Robot robot){
    std::string instructions;
    int facesChecked;
    facesChecked = 0;
    //if image is not found, eg. robot did not detect an image even though it is at an obstacle
    if(imageDetected == 'A'){
        instructions = "f";
        //if still not detected go to another obstacle
    }
    //check the 4 faces of an obstacle and return to original position
    //move to check the image on the right until the robot is back to it's original position
    else if(imageDetected == 'B'){
        while(facesChecked!=4){
        //perhaps instead of hardcoding the reaction, perform A* search given the new target location

        //move robot to the right face
        //reverse10,right90,reverse10x2,left90,reverse10x3,left90,forward10
        instructions = "f|h|f|f|i|f|f|f|i|b";

        facesChecked++;
        }
    }
    //image found, reverse robot slightly
    else if(imageDetected == 'C'){
        instructions = "e";
    }
    return instructions;
}

//The samplingPoints is the number of points that you check on the circle.
//(Eg. if sampling points = 10 and turning angle = 90, you will sample 10 points on the circle to check if they intersect with the vertex).
//This is to let you choose if you want to sample all angles from 0 to 90 for accuracy or sample half of the points for speed. (More samples = longer runtime).
bool gridOnCurve(Vertex vertex, int initRow, int initCol, int targetRow, int targetCol, double turnRadius, double turnAngle, int samplingPoints){
    //assuming turn radius = 34
    double angleSection;
    double x,y,startX,startY;
    startX = 0;
    startY = 0;
    angleSection = turnAngle/samplingPoints;
    //if initCol==targetCol startX =0
    if(initCol>targetCol){
        startX = startX + turnRadius;
    }
    else if(initCol<targetCol){
        startX = startX - turnRadius;
    }

    if(initRow>targetRow){
        startY = startY - turnRadius;
    }
    else if(initRow<targetRow){
        startY = startY + turnRadius;
    }
    for(int i =0;i<=samplingPoints;i++){
        x = turnRadius*cos(angleSection*i)+startX; //10 is the grid size, can be adjusted
        y = turnRadius*sin(angleSection*i)+startY;
        if(vertex.x_left<x && vertex.x_right>x && vertex.y_high>y && vertex.y_low<y){
            return true;
        }
    }
    return false;
}
