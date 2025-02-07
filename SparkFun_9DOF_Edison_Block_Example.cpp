/******************************************************************************
SparkFun_9DOF_Edison_Block_Example.cpp
Example code for the 9DOF Edison Block
14 Jul 2015 by Mike Hord
https://github.com/sparkfun/SparkFun_9DOF_Block_for_Edison_CPP_Library
Demonstrates the major functionality of the SparkFun 9DOF block for Edison.
** Supports only I2C connection! **
Development environment specifics:
  Code developed in Intel's Eclipse IOT-DK
  This code requires the Intel mraa library to function; for more
  information see https://github.com/intel-iot-devkit/mraa
This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!
Distributed as-is; no warranty is given.
******************************************************************************/

#include "mraa.hpp"

#include <iostream>
#include <unistd.h>
#include "SFE_LSM9DS0.h"
using namespace std;

#include <time.h>
float timeNow; float timeOfSensorReading; float timeDif;
//testing clock above - Fedir

#define Freq 10; //Freq is outdated

//change freq both here and in Madgwick
void extra(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
void mosquittoMake();
int timer;
//extra added by Fedir to link this program to the Madgwick or other programs

void collectForAxisCalculation(float gxRad, float gyRad, float gzRad);
//joint axis calculation part begins with above method^^^

#include <fstream>
#include <sys/time.h>
#include <inttypes.h>


timespec diff(timespec start, timespec end);
timespec time0, time1, time2;


int main()
{	
	clock_gettime(CLOCK_MONOTONIC, &time0);
	clock_gettime(CLOCK_MONOTONIC, &time1);

	mosquittoMake(); //starts Mosquitto for publishing
	timer = 0; //delete timer from all eventually
//	const clock_t beginT = clock(); //Fedir....... from stackOverflow answers example
	timeDif = 0.02f;	

  LSM9DS0 *imu;
  imu = new LSM9DS0(0x6B, 0x1D);
  // The begin() function sets up some basic parameters and turns the device
  //  on; you may not need to do more than call it. It also returns the "whoami"
  //  registers from the chip. If all is good, the return value here should be
  //  0x49d4. Here are the initial settings from this function:
  //  Gyro scale:        245 deg/sec max
  //  Xl scale:          4g max
  //  Mag scale:         2 Gauss max
  //  Gyro sample rate:  95Hz
  //  Xl sample rate:    100Hz
  //  Mag sample rate:   100Hz
  // These can be changed either by calling appropriate functions or by
  //  pasing parameters to the begin() function. There are named constants in
  //  the .h file for all scales and data rates; I won't reproduce them here.
  //  Here's the list of fuctions to set the rates/scale:
  //  setMagScale(mag_scale mScl)      setMagODR(mag_odr mRate)
  //  setGyroScale(gyro_scale gScl)    setGyroODR(gyro_odr gRate)
  //  setAccelScale(accel_scale aScl)  setGyroODR(accel_odr aRate)
  // If you want to make these changes at the point of calling begin, here's
  //  the prototype for that function showing the order to pass things:
  //  begin(gyro_scale gScl, accel_scale aScl, mag_scale mScl, 
	//				gyro_odr gODR, accel_odr aODR, mag_odr mODR)
  uint16_t imuResult = imu->begin();
  //cout<<hex<<"Chip ID: 0x"<<imuResult<<dec<<" (should be 0x49d4)"<<endl;

  bool newAccelData = false;
  bool newMagData = false;
  bool newGyroData = false;
  bool overflow = false;

  // Loop and report data
  while (1)
  {
    // First, let's make sure we're collecting up-to-date information. The
    //  sensors are sampling at 100Hz (for the accelerometer, magnetometer, and
    //  temp) and 95Hz (for the gyro), and we could easily do a bunch of
    //  crap within that ~10ms sampling period.
    while ((newGyroData & newAccelData & newMagData) != true)
    {
      if (newAccelData != true)
      {
        newAccelData = imu->newXData();
      }
      if (newGyroData != true)
      {
        newGyroData = imu->newGData();
      }
      if (newMagData != true)
      {
        newMagData = imu->newMData(); // Temp data is collected at the same
                                      //  rate as magnetometer data.
      } 
    }

    newAccelData = false;
    newMagData = false;
    newGyroData = false;

    // Of course, we may care if an overflow occurred; we can check that
    //  easily enough from an internal register on the part. There are functions
    //  to check for overflow per device.
    overflow = imu->xDataOverflow() | 
               imu->gDataOverflow() | 
               imu->mDataOverflow();

    if (overflow)
    {
//      cout<<"WARNING: DATA OVERFLOW!!!"<<endl;
    }

    // Calling these functions causes the data to be read from the IMU into
    //  10 16-bit signed integer public variables, as seen below. There is no
    //  automated check on whether the data is new; you need to do that
    //  manually as above. Also, there's no check on overflow, so you may miss
    //  a sample and not know it.
    imu->readAccel();
    imu->readMag();
    imu->readGyro();
    imu->readTemp();


//part below slightly modified from original by Fedir

	float gx = imu->gx;
	float gy = imu->gy;
	float gz = imu->gz;
	float ax = imu->ax;
	float ay = imu->ay;
	float az = imu->az;
	float mx = imu->mx;
	float my = imu->my;
	float mz = imu->mz;
	float gxDeg = imu->calcGyro(gx);
	float gyDeg = imu->calcGyro(gy);
	float gzDeg = imu->calcGyro(gz);

	clock_gettime(CLOCK_MONOTONIC, &time2);
	timeDif = (float) diff(time1,time2).tv_sec + diff(time1,time2).tv_nsec / 1000000000.0f;
	clock_gettime(CLOCK_MONOTONIC, &time1);
//	cout<<timeDif<<endl;
	timeOfSensorReading = (float) diff(time0,time2).tv_sec + diff(time0,time2).tv_nsec / 1000000000.0f;
//	cout<<timeOfSensorReading<<endl;

	// timeDif = (float(clock()) - timeOfSensorReading) * 1.25f / CLOCKS_PER_SEC;
//	timeOfSensorReading = float(clock());
//	cout<<"timeDif:"<<endl;	cout<<timeDif<<endl; cout<<"timeDif:"<<endl;
/*
    // Print the unscaled 16-bit signed values.
 //   cout<<"-------------------------------------"<<endl;
    cout<<"Gyro x: "<<gx<<endl;
    cout<<"Gyro y: "<<gy<<endl;
    cout<<"Gyro z: "<<gz<<endl;
    cout<<"Accel x: "<<ax<<endl;
    cout<<"Accel y: "<<ay<<endl;
    cout<<"Accel z: "<<az<<endl;
    cout<<"Mag x: "<<mx<<endl;
    cout<<"Mag y: "<<my<<endl;
    cout<<"Mag z: "<<mz<<endl;
    cout<<"Temp: "<<imu->temperature<<endl;
    cout<<"-------------------------------------"<<endl;

    // Print the "real" values in more human comprehensible units.
    cout<<"-------------------------------------"<<endl;
    cout<<"Gyro x: "<<gxDeg<<" deg/s"<<endl;
    cout<<"Gyro y: "<<gyDeg<<" deg/s"<<endl;
    cout<<"Gyro z: "<<gzDeg<<" deg/s"<<endl;
    cout<<"Accel x: "<<imu->calcAccel(imu->ax)<<" g"<<endl;
    cout<<"Accel y: "<<imu->calcAccel(imu->ay)<<" g"<<endl;
    cout<<"Accel z: "<<imu->calcAccel(imu->az)<<" g"<<endl;
    cout<<"Mag x: "<<imu->calcMag(imu->mx)<<" Gauss"<<endl;
    cout<<"Mag y: "<<imu->calcMag(imu->my)<<" Gauss"<<endl;
    cout<<"Mag z: "<<imu->calcMag(imu->mz)<<" Gauss"<<endl;
*/
    // Temp conversion is left as an example to the reader, as it requires a
    //  good deal of device- and system-specific calibration. The on-board
    //  temp sensor is probably best not used if local temp data is required!
//    cout<<"-------------------------------------"<<endl;
	

	extra(gxDeg, gyDeg, gzDeg, ax, ay, az, mx, my, mz);
//	cout<<"CLOCK:"<<endl;
//	std::cout << float( clock() - beginT ) / CLOCKS_PER_SEC * 1.08f<<endl;
	//for some reason the clock is a bit off from reality, so multiplied it
//	cout<<"CLOCK:"<<endl;
//	timeNow = float(clock()) / CLOCKS_PER_SEC;
/*	while((float(clock())/ CLOCKS_PER_SEC - timeNow) * 1.08f <= 0.02f) //wait for 0.1 sec
	{
		wait();
	}
*/
//	int sleepFor = 1000000/Freq;
//	usleep(sleepFor);
  } 

	return MRAA_SUCCESS;
}

void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, int timer, float timeDif);

void datalog(float gxRad,float gyRad,float gzRad,float ax,float ay,float az,float mx,float my,float mz,float timeDif,float timeNow); 

void extra(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz)
{
	float gxRad = gx*3.1415926535/180.0f;
	float gyRad = gy*3.1415926535/180.0f;
	float gzRad = gz*3.1415926535/180.0f;
//	MadgwickAHRSupdate(gxRad, gyRad, gzRad, ax, ay, az, mx, my, mz, timer);
//	MadgwickAHRSupdate(gxRad, gxRad, gzRad, ax, ay, az, mx, my, mz, timer);
	
//	gzRad+=0.07855793f; gyRad-=0.00763317f; gxRad-=0.00430633f; //attempting to zero stationary Gyro readings
	gxRad+=.0118;gyRad+=.0013;gzRad+=0;
	ax+=1900; ay+=30; az+=0;

	gxRad = -gxRad; gzRad = - gzRad; //flip gx,gz axes of sensors??? :!!!!!
	ax = -ax; az = - az; //flip ax, az axes of sensors??? :!!!!!

//	cout<<"XYZ"<<endl;
//	cout<<gxRad<<endl;
//	cout<<gyRad<<endl;
//	cout<<gzRad<<endl;
	
//	cout<<"RAD"<<endl; cout<<gxRad<<endl; cout<<gyRad<<endl; cout<<gzRad<<endl; cout<<"RAD"<<endl;

//	MadgwickAHRSupdate(gxRad, gyRad, gzRad, ax, ay, az, 0.0f, 0.0f, 0.0f, timer);
//	MadgwickAHRSupdate(gxRad, gyRad, gzRad, ax, ay, az, mx, my, mz, timer);
//	MadgwickAHRSupdate(0.0, 0.0, 0.0, ax, ay, az, 0.0f, 0.0f, 0.0f, timer, timeDif);

//	MadgwickAHRSupdate(gxRad, gyRad, gzRad, ax, ay, az, -my, -mx, mz, timer, timeDif);
//	MadgwickAHRSupdate(gyRad, gxRad, gzRad, ay, ax, az, 0.0f, 0.0f, 0.0f, timer, timeDif); //looks best so far
//	MadgwickAHRSupdate(gyRad, gxRad, gzRad, ay, ax, az, 0.0f, 0.0f, 0.0f, timer, timeDif); //looks best so far
	MadgwickAHRSupdate(gxRad, gyRad, gzRad, ax, ay, az, mx, my, mz, timer, timeDif); //looks best so far
//	MadgwickAHRSupdate(gyRad, gxRad, gzRad, ay, ax, az, mx, my, mz, timer, timeDif);


datalog(gxRad, gyRad, gzRad, ax, ay, az, mx, my, mz, timeDif, timeNow);

	timer++;
//	cout<<timer<<endl;
	if(timer>=12000) timer = 0;

collectForAxisCalculation(gxRad, gyRad, gzRad);


}


void datalog(float gxRad,float gyRad,float gzRad,float ax,float ay,float az,float mx,float my,float mz,float timeDif,float timeNow)
{
/*
cout<<gxRad<<endl;
cout<<gyRad<<endl;
cout<<gzRad<<endl;
cout<<ax<<endl;
cout<<ay<<endl;
cout<<az<<endl;
cout<<mx<<endl;
cout<<my<<endl;
cout<<mz<<endl;
cout<<timeDif<<endl;
cout<<timeNow<<endl;
*/

ofstream someFile;
someFile.open("datalog.txt", fstream::in | fstream::out | fstream::app);
someFile << gxRad;
someFile << " ";
someFile << gyRad;
someFile << " ";
someFile << gzRad;
someFile << " ";
someFile << ax;
someFile << " ";
someFile << ay;
someFile << " ";
someFile << az;
someFile << " ";
someFile << mx;
someFile << " ";
someFile << my;
someFile << " ";
someFile << mz;
someFile << " ";
someFile << timeDif;
someFile << " ";
someFile << timeNow;
someFile<< ".\n";
someFile.close();

}


timespec diff(timespec start, timespec end);
 
//directly copied and pasted from:
//https://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
timespec diff(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

