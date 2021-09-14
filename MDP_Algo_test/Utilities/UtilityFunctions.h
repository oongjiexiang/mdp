#ifndef UtilityFunctions_h
#define UtilityFunctions_h

#include<iostream>
#include<string>
#include<vector>
#include "component.h"
//find the location where the robot should move to in order to search for image
Vertex findTargetLocation(double obstacleX, double obstacleY, double obstacleFacingDirection, double* retValue);
//run both functions below when checking if there is an obstacle in the path
//function to check if 2 bounding boxes intersect
bool boundingBox(vector<Obstacle> obstacleList, double obstacleLength, Robot robot, double robotBoundary);
//checks if 2 lines intersect (obstacle boundary line and path traveled line)
bool linesStraddle(vector<Obstacle> obstacleList, double obstacleLength, double xStart, double xEnd, double yStart, double yEnd);
//function to ask the robot to move away from the obstacle, either left or right at a fixed distance and turning rate, before returning to the original path
//check if there is available space on either left or right before issuing instructions
//returns a message sent to STM/ANDROID for movement
std::string avoidObstacle(Robot robot, std::string oldCommand, std::vector<Obstacle> obstacleList, double obstacleLength, double robotBoundary);
//define a set pattern that the robot will travel in if the image deteced is a bullseye image and check that the image is on the target obstacle
std::string aroundObstacleTillDetect(int imageDetected, Vertex targetVertex, Robot robot);
std::string imageNotFound();
bool gridOnCurve(int initRow, int initCol, int targetRow, int targetCol, double turnRadius, double turnAngle, int samplingPoints);
#endif
