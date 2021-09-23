#ifndef UtilityFunctions_h
#define UtilityFunctions_h

#include<iostream>
#include<string>
#include<vector>
#include "component.h"
Vertex findTargetLocation(double obstacleX, double obstacleY, double obstacleFacingDirection, double* retValue);
bool boundingBox(double pX0, double pX1, double pY0, double pY1, double oX0, double oX1, double oY0, double oY1);
bool linesStraddle(double pX0, double pX1, double pY0, double pY1, double oX0, double oX1, double oY0, double oY1);
bool checkIntersect(Vertex vertex, double initPathX, double initPathY, double endPathX, double endPathY);
std::string avoidObstacle(Robot robot, std::string oldCommand, std::vector<Obstacle> obstacleList, double obstacleLength, double robotBoundary);
std::string aroundObstacleTillDetect(int imageDetected, Vertex targetVertex, Robot robot);
bool curveIntersects(double robotCenterX, double robotCenterY, double oX0, double oX1, double oY0, double oY1, double turningRadius, double turningAngle, double facingDirection, std::string turnDirection);
#endif
