#include<iostream>
#include<string>
#include<vector>
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
bool boundingBox(std::vector<Obstacle> obstacleList, double obstacleLength, Robot robot, double robotBoundary){
    double hOABLength = 25;//half of the obstacle and boundary length
    for(int i=0;i<obstacleLength;i++){
        //following the bounding box intersect equation from the slides
        if((robot.x_right>=obstacleList[i].x_center-hOABLength) && (obstacleList[i].x_center+hOABLength>=robot.x_left) && (robot.y_high>=obstacleList[i].y_center-hOABLength) && (obstacleList[i].y_center+hOABLength>=robot.y_low)){
            return true;
        }
    }
    return false;
}
//checks if 2 lines intersect (obstacle boundary line and path traveled line), returns true if line straddles
bool linesStraddle(std::vector<Obstacle> obstacleList, double obstacleLength, double xStart, double xEnd, double yStart, double yEnd){
    double x1, x2, x3, y1, y2, y3,eq1, eq2;
    int i;
    //p2,p3 -> obstacle lines
    //p0,p1 -> robot path
    //p0 = x1, y1
    //p1 = x2, y2
    //p2 or p3 = x3, y3

    x1 = xStart;
    y1 = yStart;
    x2 = xEnd;
    y2 = yEnd;
    for(i=0;i<obstacleLength;i++){
        //5 is the length of an obstacle from the center
        //this is the bot left point of the obstacle
        x3 = obstacleList[i].x_center-5;
        y3 = obstacleList[i].y_center-5;
        eq1 = ((x3-x1)*(y2-y1)) - ((y3-y1)*(x2-x1));
        //this is the top right point of the obstacle
        x3 = obstacleList[i].x_center+5;
        y3 = obstacleList[i].y_center+5;
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
    }
    return false;
}
//if robot is going to crash into a virtual obstacle, reverse and continue with previous movement(according to the A* search)
//reason: the only possibility that the robot will hit a virtual obstacle is during turning(robot should not be commanded to move forward into an obstacle due to the restrictions on A*)
//returns a message sent to STM/ANDROID for movement
std::string avoidObstacle(Robot robot, std::string oldCommand,std::vector<Obstacle> obstacleList, double obstacleLength, double robotBoundary){
    std::string newCommand;
    std::string forwardInstr = "FORWARD";
    std::string reverseInstr = "REVERSE";
    newCommand = oldCommand;
    if(boundingBox(obstacleList,obstacleLength,robot,robotBoundary)){
        newCommand = reverseInstr;
    }
    //call A* again here? not too sure
    return newCommand;
}

//define a set pattern that the robot will travel in if the image deteced is a bullseye image and check that the image is on the target obstacle, returns instructions to move the robot
std::string aroundObstacleTillDetect(int imageDetected, Vertex targetVertex, Robot robot){
    std::string instructions;
    int facesChecked, bullsEyeId;
    bullsEyeId=31;//change later
    facesChecked = 0;
    std::string reverseInstr = "REVERSE";
    std::string turnLeft = "TURN90";
    std::string turnRight = "TURN-90";
    std::string forwardInstr = "FORWARD";
    //if image is not found, eg. robot did not detect an image even though it is at an obstacle
    if(imageDetected==-1){
        instructions = reverseInstr+"10";
        //if still not detected go to another obstacle
    }
    //check the 4 faces of an obstacle and return to original position
    //move to check the image on the right until the robot is back to it's original position
    else if(imageDetected==bullsEyeId){
        while(facesChecked!=4){

        //move robot to the right face
        //may need to change this function depending on the distance traveled when turning
        instructions = reverseInstr+"10|"+turnRight+"|"+turnLeft+"|"+reverseInstr+"30|"+turnLeft;

        facesChecked++;
        }
    }
    //image found, reverse robot slightly
    else{
        instructions = reverseInstr+"10";
    }
    return instructions;
}
