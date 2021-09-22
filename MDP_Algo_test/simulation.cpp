#include "component.h"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <windows.h>

using namespace std;

const int SIMULATION_CELL_LENGTH = 3;
const int SIMULATION_BOUNDARY_LENGTH = SIMULATION_CELL_LENGTH*ROW_COUNT;
const int OBS_VIRTUAL = -1;
const int OBS_CENTRE = -2;
const int ROBOT_CENTRE = -3;
const int FACE_UP = -4;
const int FACE_DOWN = -5;
const int FACE_LEFT = -6;
const int FACE_RIGHT = -7;
const int EMPTY = 0;

const short COLOR_BLACK = 0;
const short COLOR_GREY = 8;
const short COLOR_RED = 4;
const short COLOR_WHITE = 7;
const short COLOR_YELLOW = 6;
const short COLOR_PURPLE = 5;
const short COLOR_GREEN = 10;
const short COLOR_AQUA = 11;
const short COLOR_LIGHT_BLUE = 9;

// To change colour of console. Only changeable on Windows
HANDLE hConsole; 

void printBoundary(){
    
    SetConsoleTextAttribute(hConsole, COLOR_BLACK);
    for(int i = 0; i < SIMULATION_BOUNDARY_LENGTH; i++){
        cout << "-";
    }
    cout << endl;
}

void preprocessMap(vector<vector<int>>& grids, vector<Obstacle>& obstacles, Robot robot){

    // update map with obstacles' info
    for(int i = 0; i < obstacles.size(); i++){ 
        Obstacle o = obstacles[i];
        for(int j = -1; j <= 1; j++){
            for(int k = -1; k <= 1; k++){
                int nRow = o.row + j;
                int nCol = o.column + k;
                if(nRow >= 0 && nCol >= 0 && nRow < ROW_COUNT && nCol < COLUMN_COUNT)
                    grids[o.row + j][o.column + k] = OBS_VIRTUAL;
            }
        }
        grids[o.row][o.column] = obstacles[i].id;
    }

    // update map with robot's info
    grids[robot.row][robot.column] = ROBOT_CENTRE;
}

void printMap(vector<vector<int>> grids, vector<Obstacle> obstacles, Robot robot){
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    preprocessMap(grids, obstacles, robot);
    for(int i = grids.size() - 1; i >= 0; i--){
        printBoundary();
        for(int j = 0; j < COLUMN_COUNT; j++){
            cout << " ";
            switch(grids[i][j]){
                case EMPTY:
                    cout << " ";
                break;
                case OBS_VIRTUAL:
                    SetConsoleTextAttribute(hConsole, COLOR_GREY);
                    cout << "X";
                break;
                case ROBOT_CENTRE:
                    SetConsoleTextAttribute(hConsole, COLOR_PURPLE);
                    cout << "O";
                break;
                case FACE_UP:
                    SetConsoleTextAttribute(hConsole, COLOR_RED);
                    cout << "^";
                break;
                case FACE_DOWN:
                    SetConsoleTextAttribute(hConsole, COLOR_RED);
                    cout << "v";
                break;
                case FACE_LEFT:
                    SetConsoleTextAttribute(hConsole, COLOR_RED);
                    cout << "<";
                break;
                case FACE_RIGHT:
                    SetConsoleTextAttribute(hConsole, COLOR_RED);
                    cout << ">";
                break;
                default:
                    cout << grids[i][j];
            }
            cout << " ";
        }
        cout << endl;
    }
    SetConsoleTextAttribute(hConsole, COLOR_WHITE);
}

int main(){
    vector<vector<int>> grids(ROW_COUNT, vector<int>(COLUMN_COUNT));
    vector<Obstacle> obstacles;

    Obstacle o1(1, 3, 1, 90); obstacles.push_back(o1); 
    Obstacle o2(2, 15, 10, 90); obstacles.push_back(o2); 
    Obstacle o3(3, 7, 25, -90); obstacles.push_back(o3); 
    Obstacle o4(4, 19, 19, 0); obstacles.push_back(o4); 
    
    Robot robot(1, 1, 90);

    printMap(grids, obstacles, robot);
    printBoundary();
}