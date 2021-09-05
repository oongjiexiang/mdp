#include <stdio.h>
#include <string>
#include <vector>
#include "tripPlan.h"

using namespace std;

const int MESSAGE_LENGTH = 64;
string Move::moveForward(double length){
    char s[MESSAGE_LENGTH];
    sprintf(s, "FORWARD %.3f", length);
    return s;
}
string Move::turn(double degree){
    char s[MESSAGE_LENGTH];
    sprintf(s, "TURN %.3f", degree);
    return s;
}
string Move::reverse(double length){    // in cm
    char s[MESSAGE_LENGTH];
    sprintf(s, "REVERSE %.3f", length);
    return s;
}
string Move::reverseTurn(double degree){
    char s[MESSAGE_LENGTH];
    sprintf(s, "REVERSE_TURN %.3f", degree);
    return s;
}

vector<string> Network::cleanAndroid(vector<string> movementRaw){
    // process the raw movements once
}
vector<string> Network::cleanSTM(vector<string> movementPartialRaw){
    // process the movements from cleanAndroid() again
}
void Network::sendSTM(){
    // network to STM
}
void Network::sendAndroid(){

}
void Network::sendRPI(){

}