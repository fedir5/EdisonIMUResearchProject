//by Fedir
//example publishing with Mosquitto Code that I got to work
//added methods that automatically convert doubles, ints,
//      and floats into the needed format for publishing
//compile with: 
//g++ mosquittoPubReadMe.cpp outputMosquitto.cpp -lmosquittopp -o mosquittoPubReadMe

#include <unistd.h> //for sleep
#include <sstream> //for string
using namespace std; //for string

void mPrint(string message);
void mPrint(int message);
void mPrint(float message);
void mPrint(double message);
void mosquittoMake();
void mosquittoClose();

int main()
{
mosquittoMake();
mPrint("Hello!1");
usleep(1000000);
mPrint(2);
usleep(1000000);
mPrint(2.056f);
usleep(1000000);
mPrint(2.056685);
usleep(1000000);
mosquittoClose();

return 0;
}

