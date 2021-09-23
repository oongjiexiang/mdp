#include<iostream>
#include<string>
#include<vector>
#include<math.h>
#include "component.h"
#include "action.h"


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
//use this before using lines straddle
//checks if the bounding boxes of the lines intersect, quick rejection test
bool boundingBox(double pX0, double pX1, double pY0, double pY1, double oX0, double oX1, double oY0, double oY1){
    if((pX1 >= oX0) &&  (oX1 >= pX0) && (pY1 >= oY0) && (oY1 >= pY0)){
        return true;
    }
    return false;
}

//call this twice and swap the lines
bool linesStraddle(double x0, double x1, double x2, double x3, double y0, double y1, double y2, double y3){
    double equation1, equation2;
    equation1 = ((x2 - x0) * (y1-y0)) - ((y2 - y0) * (x1-x0));
    equation2 = ((x3 - x0) * (y1-y0)) - ((y3 - y0) * (x1-x0));

    //if the 2 equations have different signs, it means the 2 points of the second line are on two different sides of the first line
    if(equation1 > 0 && equation2 < 0){
        return true;
    }
    if(equation1 < 0 && equation2 > 0){
        return true;
    }
    //if the 2 lines are collinear
    if(equation1 == 0 && equation2 ==0){
        //we have checked that their bounding boxes intersect, so we know the 2 collinear lines are touching each other, thus return true
        return true;
    }
    return false;
}
bool checkIntersect(Vertex vertex, double initPathX, double initPathY, double endPathX, double endPathY){
    double vX0, vX1, vY0, vY1;
    bool intersect = true; // initialisation value does not matter
    vX0 = vertex.x_left;
    vX1 = vertex.x_right;
    vY0 = vertex.y_low;
    vY1 = vertex.y_high;
    intersect = boundingBox(initPathX, endPathX, initPathY, endPathY, vX0, vX1, vY0, vY1);
    //if bounding box does not intersect, return false
    if(!intersect){
        return false;
    }
    //test if vertex border intersects with path
    intersect = linesStraddle(initPathX, endPathX, vertex.x_left, vertex.x_right, initPathY, endPathY, vertex.y_low, vertex.y_low);
    if(intersect){
        return true;
    }
    //test if path intersects with border
    intersect = linesStraddle(vertex.x_left, vertex.x_right, initPathX, endPathX, vertex.y_low, vertex.y_low, initPathY, endPathY);
    if(intersect){
        return true;
    }
        //test if vertex border intersects with path
    intersect = linesStraddle(initPathX, endPathX, vertex.x_left, vertex.x_right, initPathY, endPathY, vertex.y_high, vertex.y_high);
    if(intersect){
        return true;
    }
    //test if path intersects with border
    intersect = linesStraddle(vertex.x_left, vertex.x_right, initPathX, endPathX, vertex.y_high, vertex.y_high, initPathY, endPathY);
    if(intersect){
        return true;
    }
    intersect = linesStraddle(initPathX, endPathX, vertex.x_right, vertex.x_right, initPathY, endPathY, vertex.y_low, vertex.y_high);
    if(intersect){
        return true;
    }
    //test if path intersects with border
    intersect = linesStraddle(vertex.x_right, vertex.x_right, initPathX, endPathX, vertex.y_low, vertex.y_high, initPathY, endPathY);
    if(intersect){
        return true;
    }
    intersect = linesStraddle(initPathX, endPathX, vertex.x_left, vertex.x_left, initPathY, endPathY, vertex.y_low, vertex.y_high);
    if(intersect){
        return true;
    }
    //test if path intersects with border
    intersect = linesStraddle(vertex.x_left, vertex.x_left, initPathX, endPathX, vertex.y_low, vertex.y_high, initPathY, endPathY);
    if(intersect){
        return true;
    }
    return false;
}

//only allow turning angles of 30, 60, 90
//turnDirection can only be left or right
//assume turningRadius is ~22
//facing direction between 0 to 359
//
bool curveIntersects(double robotCenterX, double robotCenterY, double oX0, double oX1, double oY0, double oY1, double turningRadius, double turningAngle, double facingDirection, std::string turnDirection){
    //determine circle center using affine transformation
    //draw part of the circle using x=rcos(theta), y=rsin(theta) in radians for theta = 0 to turning angle r. turning radius = 21 * turningAngle/90
    //check for each x,y position, does it fall in the bounding box of any obstacles
    double circleCenterX, circleCenterY, tempX, tempY, a, b, c, discriminant, sqrtdisc, root1, root2, startOfValidAngle, endOfValidAngle,checkAngle;
    //if keep all angles within 0-359 range
    if(facingDirection < 0){
        facingDirection = facingDirection + 359;
    }
    if(facingDirection>359){
        facingDirection = facingDirection - 359;
    }
    if(turningAngle < 0){
        turningAngle = turningAngle + 359;
    }
    if(turningAngle >359){
        turningAngle = turningAngle - 359;
    }

    //convert facing direction and turning angle to radian
    facingDirection = facingDirection/180 * M_PI;
    turningAngle = turningAngle/180 * M_PI;
    startOfValidAngle = facingDirection;

    //find circle center
    //offset circle then rotate to facing direction
    if(turnDirection.compare("right")==0){
        //circle should be offset to the right of the robot to simulate right turn path
        circleCenterX = robotCenterX+turningRadius;
        circleCenterY = 0;

        startOfValidAngle = facingDirection+(0.5*M_PI); //offset angle from facing direction by positive 90 degrees to start from the left side of the circle
        endOfValidAngle = startOfValidAngle-turningAngle;
    }
    else{
        //circle should be offset to the left of the robot to simulate left turn path
        circleCenterX = robotCenterY-turningRadius;
        circleCenterY = 0;
        startOfValidAngle = facingDirection-(0.5*M_PI); //offset angle from facing direction by negative 90 degrees to start from the right side of the cirle
        endOfValidAngle = startOfValidAngle+turningAngle;
    }

    //apply rotation of facingDirection
    tempX = circleCenterX*cos(facingDirection) - circleCenterY*sin(facingDirection);
    tempY = circleCenterX*sin(facingDirection) + circleCenterY*cos(facingDirection);
    //offset the circle to location of the robot to find the center point of the circle to be drawn
    circleCenterX = tempX+robotCenterX;
    circleCenterY = tempY+robotCenterY;

    //check if the line given will intersect with the circle
    //to do this more easily we move the circle back to the origin, and offset the line by the same amount
    oX0 = oX0 - circleCenterX;
    oX1 = oX1 - circleCenterX;
    oY0 = oY0 - circleCenterY;
    oY1 = oY1 - circleCenterY;

    //since we want to check for intersection, we equate the equation of the line, with the equation of the circle
    //given the circle has an equation of x^2 + y^2 =r^2,
    //the line has an equation of x = Ax + t(Bx-Ax) and y = Ay + t(By-Ay) , where A = start, B = end, 0<t<1
    //then we sub in the x and y values and simplify to get
    //Ax^2 + Ay^2 - R2 + 2*t*(Ax + ((Bx-Ax))^2 + Ay + ((By-Ay))) + ((Bx+Ax)^2 + (By+Ay)^2)^2
    //this is now a quadratic equation, use the discriminant to find if two real values of t exist to see if lines intersect
    //split that into 3 parts a, b, c to find the
    a = pow((oX1 - oX0),2) + pow((oY1 - oY0),2);
    b = 2*(oX0*(oX1 - oX0) + oY0*(oY1 - oY0));
    c = pow(oX0,2) + pow(oX1,2) - turningRadius;

    discriminant = pow(b,2) - 4*a*c;

    if(discriminant <=0){
        return false; //no real roots, does not intersect
    }
    sqrtdisc = sqrt(discriminant);

    root1 = (-b +sqrtdisc)/(2*a);
    root2 = (-b -sqrtdisc)/(2*a);
    //if the roots are within the valid root range
    if(root1 <1 && root1 >0){
        //check if the intersects are within the turning angle
        tempX = oX0 + root1*(oX1 - oX0);
        tempY = oY0 + root1*(oY1 - oY0);
        if(tempX == 0){
            return false; //the starting location of X will not have any obstacles
        }
        checkAngle = tan(tempY/tempX);
        if(startOfValidAngle <= checkAngle && endOfValidAngle>=checkAngle){
            return true;//intersection occurs within the turning path
        }
    }
    if(root2 <1 && root2 >0){
    //check if the intersects are within the turning angle
        tempX = oX0 + root2*(oX1 - oX0);
        tempY = oY0 + root2*(oY1 - oY0);
        if(tempX == 0){
            return false; //the starting location of X will not have any obstacles
        }
        checkAngle = tan(tempY/tempX);
        if(startOfValidAngle <= checkAngle && endOfValidAngle>=checkAngle){
            return true;//intersection occurs within the turning path
        }
    }
    return false;
    //determine circle center using affine transformation
    //draw the whole circle
    //check if line intersects with circle
    //if yes, find the points of intersection
    //check if points of intersection are within turning angle
    //if right turn, angle to check: 180 to turningAngle to 180 + rotation(facingDirection)
    //if left turn, angle to check: 0 to turningAngle +rotation(facingDirection)
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

/*
std::string translateAction(Action currentAction){
    //converts the action class stored in the action array into a string message to be sent
    //encode 1 action followed by the target subteam, either 1:AND, 2:STM, 3:RPI
    std::string message="";
    ActionTurn actionTurn;
    ActionStraight actionStraight;
    ActionDetect actionDetect;
    if(dynamic_cast<actionTurn*>(currentAction) != nullptr){
        if(currentAction.getTravel > 0){
            message = "i2";
        }
        if(currentAction.getTravel < 0){
            message = "j2";
        }
        if(currentAction.getTravel == 0){
            return "ERROR";
        }
        //assuming negative turn angle == right
        //get turn angle
        //if turn angle positive msg = "i2"(turn left 90), send to STM
        //if turn angle negative msg = "j2"(turn right 90), send to STM

    }
    if(dynamic_cast<actionStraight*>(currentAction) != nullptr){
        if(currentAction.getTravel > 0){
            message = "b2";
        }
        if(currentAction.getTravel < 0){
            message = "f2";
        }
        if(currentAction.getTravel == 0){
            return "ERROR";
        }
        //assuming positive == forward
        //get travel distance
        //if travel distance positive msg = "b2"(forward 10cm)
        //if travel distance negative msg = "f2"(reverse 10cm)
    }
    if(dynamic_cast<actionDetect*>(currentAction) != nullptr){
        message = "k3"; //detect, RPI
    }

    return message;
}
*/


//----------------- maybe can delete--------------------
/*
//run both functions below when checking if there is an obstacle in the path
//function to check if robot's bounding box at any location will intersect with an obstacle bounding bo
bool boundingBox2(double x1, double y1, double objectSize1, double x2, double y2, double objectSize2){
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
*/
