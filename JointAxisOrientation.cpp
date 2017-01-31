#include <iostream>
#include <math.h>
using namespace std;

void mPrint(double message); //mosquitto
void mPrint(string message);

double gyroxValues[2000];
double gyroyValues[2000];
double gyrozValues[2000];
double gyrox2Values[2000];
double gyroy2Values[2000];
double gyroz2Values[2000];

int counter = -500;

float secondSensorTempStationary(int fromWhichSet, int loc); //temporary method here, replace all these with second sensor eventually



double findMinimi(double w, double x, double y, double z, int gyroSize);
void normalizeGradient();
void gradientOf(double w, double x, double y, double z, int gyroSize);
double functionOf(double w, double x, double y, double z, int gyroSize);
double gradientDx = 0.0001;
double gradient[4];
double oldGradient[4];
double wxyz[4];    //stands for phi1, theta1, phi2, theta2
double step[4];
int stepPower[4] = { 3, 3, 3, 3 };
double mOfC = 0.0001;

double rangeW = 3.14159265354 / 2.0; //phi1
double rangeX = 3.14159265354 * 2.0; //theta1
double rangeY = 3.14159265354 / 2.0; //phi2
double rangeZ = 3.14159265354 * 2.0; //theta2

int maxOrMin = -1; //enter -1 here if looking for minimum of function, 1 if for maximum

double crossMultiply(double a1[], double a2[]);




double errorMin; //toDo remove this later?


void collectForAxisCalculation(float gxRad, float gyRad, float gzRad){
if(counter<0)
counter++;
if(counter>=0)
if(counter<2000){
gyroxValues[counter] = (double) gxRad;
gyroyValues[counter] = (double) gxRad;
gyrozValues[counter] = (double) gxRad;
gyrox2Values[counter] = secondSensorTempStationary(0, counter);
gyroy2Values[counter] = secondSensorTempStationary(1, counter);
gyroz2Values[counter] = secondSensorTempStationary(2, counter);
counter++;
}
if(counter==499){
	double theta1 = 0.02; double phi1 = 0.02;
	double theta2 = 0.02; double phi2 = 0.02;

	wxyz[0] = phi1; wxyz[1] = theta1; wxyz[2] = phi2; wxyz[3] = theta2;
	errorMin = findMinimi(wxyz[0], wxyz[1], wxyz[2], wxyz[3], 500);
//	double errorMin = findMinimi(wxyz[0], wxyz[1], wxyz[2], wxyz[3], 500);

	mPrint("A");
	mPrint(wxyz[0]);
	mPrint(wxyz[1]);
	mPrint(wxyz[2]);
	mPrint(wxyz[3]);


	cout<<"newCalibrationInfo!!!:"<<endl;
	cout<<wxyz[0]<<endl;
	cout<<wxyz[1]<<endl;
	cout<<wxyz[2]<<endl;
	cout<<wxyz[3]<<endl;
	
	cout<<"ERROR!!!!:)"<<endl;
	cout<<errorMin<<endl;


}
if(counter==999){
	double theta1 = 0.02; double phi1 = 0.02;
	double theta2 = 0.02; double phi2 = 0.02;

	wxyz[0] = phi1; wxyz[1] = theta1; wxyz[2] = phi2; wxyz[3] = theta2;
	errorMin = findMinimi(wxyz[0], wxyz[1], wxyz[2], wxyz[3], 1000);
//	double errorMin = findMinimi(wxyz[0], wxyz[1], wxyz[2], wxyz[3], 1000);

	mPrint("A");
	mPrint(wxyz[0]);
	mPrint(wxyz[1]);
	mPrint(wxyz[2]);
	mPrint(wxyz[3]);


	cout<<"newCalibrationInfo!!!:"<<endl;
	cout<<wxyz[0]<<endl;
	cout<<wxyz[1]<<endl;
	cout<<wxyz[2]<<endl;
	cout<<wxyz[3]<<endl;
	
	cout<<"ERROR!!!!:)"<<endl;
	cout<<errorMin<<endl;


}
if(counter==1499){
	double theta1 = 0.02; double phi1 = 0.02;
	double theta2 = 0.02; double phi2 = 0.02;

	wxyz[0] = phi1; wxyz[1] = theta1; wxyz[2] = phi2; wxyz[3] = theta2;
	errorMin = findMinimi(wxyz[0], wxyz[1], wxyz[2], wxyz[3], 1500);
//	double errorMin = findMinimi(wxyz[0], wxyz[1], wxyz[2], wxyz[3], 1500);

	mPrint("A");
	mPrint(wxyz[0]);
	mPrint(wxyz[1]);
	mPrint(wxyz[2]);
	mPrint(wxyz[3]);


	cout<<"newCalibrationInfo!!!:"<<endl;
	cout<<wxyz[0]<<endl;
	cout<<wxyz[1]<<endl;
	cout<<wxyz[2]<<endl;
	cout<<wxyz[3]<<endl;
	
	cout<<"ERROR!!!!:)"<<endl;
	cout<<errorMin<<endl;

}
if(counter==1999){
	double theta1 = 0.02; double phi1 = 0.02;
	double theta2 = 0.02; double phi2 = 0.02;

	wxyz[0] = phi1; wxyz[1] = theta1; wxyz[2] = phi2; wxyz[3] = theta2;
	errorMin = findMinimi(wxyz[0], wxyz[1], wxyz[2], wxyz[3], 2000);
//	double errorMin = findMinimi(wxyz[0], wxyz[1], wxyz[2], wxyz[3], 2000);

	mPrint("A");
	mPrint(wxyz[0]);
	mPrint(wxyz[1]);
	mPrint(wxyz[2]);
	mPrint(wxyz[3]);

	cout<<"newCalibrationInfo!!!:"<<endl;
	cout<<wxyz[0]<<endl;
	cout<<wxyz[1]<<endl;
	cout<<wxyz[2]<<endl;
	cout<<wxyz[3]<<endl;
	
	cout<<"ERROR!!!!:)"<<endl;
	cout<<errorMin<<endl;


}
	

}



















double findMinimi(double w, double x, double y, double z, int gyroSize) {
	//returns the value of the function at the minimum
//	cout << "Hello World!" << endl;
	//initialize step matrix with large steps initially
	step[0] = rangeW / 3;
	step[1] = rangeX / 3;
	step[2] = rangeY / 3;
	step[3] = rangeZ / 3;

	//find initial point gradient
	gradientOf(wxyz[0], wxyz[1], wxyz[2], wxyz[3], gyroSize);
	normalizeGradient();
	step[0] = step[0] * gradient[0];
	step[1] = step[1] * gradient[1];
	step[2] = step[2] * gradient[2];
	step[3] = step[3] * gradient[3];
	oldGradient[0] = gradient[0];
	oldGradient[1] = gradient[1];
	oldGradient[2] = gradient[2];
	oldGradient[3] = gradient[3];

//	cout << gradient[0] << " " << gradient[1] << " " << gradient[2] << " "	<< gradient[3] << endl;

	while (fabs(step[0]) > mOfC || fabs(step[1]) > mOfC || fabs(step[2]) > mOfC
			|| fabs(step[3]) > mOfC) {
		wxyz[0] = wxyz[0] + step[0];
		wxyz[1] = wxyz[1] + step[1];
		wxyz[2] = wxyz[2] + step[2];
		wxyz[3] = wxyz[3] + step[3];
		gradientOf(wxyz[0], wxyz[1], wxyz[2], wxyz[3], gyroSize);
		normalizeGradient();
		if (oldGradient[0] * gradient[0] < 0)
			stepPower[0] = stepPower[0] * 3;
		if (oldGradient[1] * gradient[1] < 0)
			stepPower[1] = stepPower[1] * 3;
		if (oldGradient[2] * gradient[2] < 0)
			stepPower[2] = stepPower[2] * 3;
		if (oldGradient[3] * gradient[3] < 0)
			stepPower[3] = stepPower[3] * 3;

		step[0] = rangeW * gradient[0] / stepPower[0];
		step[1] = rangeX * gradient[1] / stepPower[1];
		step[2] = rangeY * gradient[2] / stepPower[2];
		step[3] = rangeZ * gradient[3] / stepPower[3];

		oldGradient[0] = gradient[0];
		oldGradient[1] = gradient[1];
		oldGradient[2] = gradient[2];
		oldGradient[3] = gradient[3];

//		cout << "wxyz:" << endl;
//		cout << wxyz[0] << " " << wxyz[1] << " " << wxyz[2] << " " << wxyz[3]
//				<< endl;
//		cout << "gradient:" << endl;
//		cout << gradient[0] << " " << gradient[1] << " " << gradient[2] << " " << gradient[3] << endl;
//
	}

//	cout << "MINIMI RESULTS: {" << wxyz[0] << " " << wxyz[1] << " " << wxyz[2] << " " << wxyz[3] << "}" << endl;
	return functionOf(wxyz[0], wxyz[1], wxyz[2], wxyz[3], gyroSize);
}

double crossMultiply(double a1[], double a2[]) {
	double answer = 0.0;
	double a = a1[1] * a2[2] - a1[2] * a2[1];
	double b = a1[2] * a2[0] - a1[0] * a2[2];
	double c = a1[0] * a2[1] - a1[1] * a2[0];
	double squared = a * a + b * b + c * c;
	answer = sqrt(squared);
	return answer;
}

void normalizeGradient() {
	double magnitude = sqrt(
			gradient[0] * gradient[0] + gradient[1] * gradient[1]
					+ gradient[2] * gradient[2] + gradient[3] * gradient[3]);
	gradient[0] = maxOrMin * gradient[0] / magnitude;
	gradient[1] = maxOrMin * gradient[1] / magnitude;
	gradient[2] = maxOrMin * gradient[2] / magnitude;
	gradient[3] = maxOrMin * gradient[3] / magnitude;
}

void gradientOf(double w, double x, double y, double z, int gyroSize) {
	double fDeltaW = functionOf(w + gradientDx, x, y, z, gyroSize);
	double dw = (fDeltaW - functionOf(w, x, y, z, gyroSize)) / gradientDx;

	double fDeltaX = functionOf(w, x + gradientDx, y, z, gyroSize);
	double dx = (fDeltaX - functionOf(w, x, y, z, gyroSize)) / gradientDx;

	double fDeltaY = functionOf(w, x, y + gradientDx, z, gyroSize);
	double dy = (fDeltaY - functionOf(w, x, y, z, gyroSize)) / gradientDx;

	double fDeltaZ = functionOf(w, x, y, z + gradientDx, gyroSize);
	double dz = (fDeltaZ - functionOf(w, x, y, z, gyroSize)) / gradientDx;

	gradient[0] = dw;
	gradient[1] = dx;
	gradient[2] = dy;
	gradient[3] = dz;
}

double functionOf(double w, double x, double y, double z, int gyroSize) {

	double phi1 = w;
	double theta1 = x;
	double phi2 = y;
	double theta2 = z;

	double error = 0.0;
	for (int errorSumN = 0; errorSumN < gyroSize; errorSumN++) {
		double gyro1[3] = { gyroxValues[errorSumN], gyroyValues[errorSumN],
				gyrozValues[errorSumN] };
		double gyro2[3] = { gyrox2Values[errorSumN], gyroy2Values[errorSumN],
				gyroz2Values[errorSumN] };
		double j1[3] = { cos(phi1) * cos(theta1), cos(phi1) * sin(theta1), sin(
				phi1) };
		double j2[3] = { cos(phi2) * cos(theta2), cos(phi2) * sin(theta2), sin(
				phi2) };
		double errN = crossMultiply(gyro1, j1) - crossMultiply(gyro2, j2);
		error = error + errN * errN;
		//cout<<error<<endl;
	}
	//error is actually sum of error squares here
	return error;

}





