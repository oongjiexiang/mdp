#include <iostream>
#include "component.h"
#include "UtilityFunctions.h"

using namespace std;

int main()
{
    //------test functions------
    Vertex vertex(0,1);
    printf("vertex%.0f\n",vertex.x_left);
    printf("vertex%.0f\n",vertex.x_right);
    printf("vertex%.0f\n",vertex.y_low);
    printf("vertex%.0f\n",vertex.y_high);
    bool checker = false;
    checker = checkIntersect(vertex, 8,8,8,20); //true
    if(checker){
    cout << "line1 intersect!" << endl;
    }
    checker = checkIntersect(vertex, 13,8,14,13); //false
    if(checker){
    cout << "line2 intersect!" << endl;
    }
    checker = checkIntersect(vertex, 11,16,15,25); //false
    if(checker){
    cout << "line3 intersect!" << endl;
    }
    checker = checkIntersect(vertex, 8,9,15,19); //true
    if(checker){
    cout << "line4 intersect!" << endl;
    }
    checker = curveIntersects(10,20,5,25,50,25,21,90,0,"left"); //true
    if(checker){
    cout << "circle1 intersect!" << endl;
    }
    checker = curveIntersects(10,20,5,25,50,25,21,90,0,"right"); //false
    if(checker){
    cout << "circle2 intersect!" << endl;
    }
    checker = curveIntersects(10,20,5,25,50,25,21,10,0,"left");//false
    if(checker){
    cout << "circle3 intersect!" << endl;
    }
    return 0;
}
