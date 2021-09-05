#ifndef TRIP_PLAN_H
#define TRIP_PLAN_H

#include <vector>
#include <string>

using namespace std;

const int MESSAGE_LENGTH = 64;
class Move{
    public:
        string moveForward(double length);
        string turn(double degree);
        string reverse(double length);
        string reverseTurn(double degree);
};

class Network{
    private:
        vector<string> cleanAndroid(vector<string> movementRaw);
        vector<string> cleanSTM(vector<string> movementPartialRaw);
    public:
        void sendSTM();
        void sendAndroid();
        void sendRPI();
};
#endif