#include <stdio.h>
#include <math.h>
#include <fstream>
using namespace std;
#include <iostream>

void mPrint(float message); //mosquitto
void mPrint(string message);

void processQuaternionsAfterSwitch(float q0, float q1, float q2, float q3);
float roll; float pitch; float yaw;

void processQuaternions(float q0, float q1, float q2, float q3, int timer)
{
processQuaternionsAfterSwitch(q0, q1, q2, q3);
}

void QuaToEuler(float q0, float q1, float q2, float q3);

void switchAroundResultingAngles();

void processQuaternionsAfterSwitch(float q0, float q1, float q2, float q3)
{
/*	
	printf("%.6f\n", q0);
	printf("%.6f\n", q1);
	printf("%.6f\n", q2);
	printf("%.6f\n", q3);
*/

//	writeToTextFile(q0, q1, q2, q3, roll, pitch, yaw); //<what it should have been
//	writeToTextFile(q0, q1, q2, q3, yaw, pitch, roll);
	
	QuaToEuler(q0, q1, q2, q3);   //2,1,3,0
//	QuaToEuler(q3, q1, q0, q2);   //2,1,3,0
/*
	printf("%.6f\n", roll/3.14159265f*180.0f);
	printf("%.6f\n", pitch/3.14159265f*180.0f);
	printf("%.6f\n", yaw/3.14159265f*180.0f);
	printf("Hello World!\n");
*/
	
	switchAroundResultingAngles();

	mPrint("R");
	mPrint(roll);
	mPrint("P");
	mPrint(pitch);
	mPrint("Y");
	mPrint(yaw);

	ofstream someFile;
	someFile.open("datalogResults.txt", fstream::in | fstream::out | fstream::app);
	someFile << roll;
	someFile << " ";
	someFile << pitch;
	someFile << " ";
	someFile << yaw;
	someFile << " ";
	someFile<< ".\n";
	someFile.close();

}


void QuaToEuler(float q3, float q1, float q0, float q2)  //takes Quaternion in the form Real, i, j, k
{
//followed http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
//quaternion has to be normalized

float rollTemp = roll;float pitchTemp = pitch; float yawTemp = yaw;


float checkSingularities = q0*q1+q2*q3;

if(checkSingularities > 0.4999f) //north pole
{
	roll = 2.0f * atan2(q0,q3);
	pitch = 3.1415926535f/2.0f;
	yaw = 0;
//	cout<<"LOL!N"<<endl;
	return;
}

if(checkSingularities < -0.4999f) //south pole
{
	roll = -2.0f * atan2(q0,q3);
	pitch = -3.1415926535f/2.0f;
	yaw = 0;
//	cout<<"LOL!S"<<endl;
	return;
}

float sq0 = q0*q0; float sq1 = q1*q1; float sq2 = q2*q2;

roll = atan2(2.0f*q1*q3-2.0f*q0*q2, 1.0f - 2.0f*sq1 - 2.0f*sq2);
pitch = asin(2.0f*checkSingularities);
yaw = atan2(2.0f*q0*q3-2.0f*q1*q2, 1.0f - 2.0f*sq0 - 2.0f*sq2);

if((rollTemp-roll) > 6.1086523819f)
roll = roll+6.28318530718f;
if((rollTemp-roll) < - 6.1086523819f)
roll = roll - 6.28318530718f;

if((pitchTemp-pitch) > 6.1086523819f)
pitch = pitch+6.28318530718f;
if((pitchTemp-pitch) < - 6.1086523819f)
pitch = pitch - 6.28318530718f;

if((yawTemp-yaw) > 6.1086523819f)
yaw = yaw+6.28318530718f;
if((yawTemp-yaw) < - 6.1086523819f)
yaw = yaw - 6.28318530718f;

//printf("%.6f\n", roll);
//printf("%.6f\n", pitch);
//printf("%.6f\n", yaw);
//3.14159265f*180.0f

}

void switchAroundResultingAngles(){
float rollTemp = roll;
float pitchTemp = pitch;
float yawTemp = yaw;
pitch = yawTemp;
roll = -rollTemp;
yaw = pitchTemp;
//yaw = -pitchTemp;
}