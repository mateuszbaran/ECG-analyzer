#include "Butterworth.h"

Butterworth::Butterworth(){}
Butterworth::~Butterworth(){}

/*
	filterDesign for the time being returns precalculated
	values for Butterworth 2nd order highpass filter,
	given the sample frequency of source signal
*/
std::vector<std::vector<double>> Butterworth::filterDesign(int order, double attenuation, double cutoffFreq, int sampleFreq, int type)
{
	std::vector<std::vector<double>> baCoefficients(2, std::vector<double>(3,0));

	switch(sampleFreq){
		case 200:
			baCoefficients[0][0] = 0.8949;
			baCoefficients[0][1] = -1.7897;
			baCoefficients[0][2] = 0.8949;
			baCoefficients[1][0] = 1.0000;
			baCoefficients[1][1] = -1.7786;
			baCoefficients[1][2] = 0.8008;
			break;
		case 250:
			baCoefficients[0][0] = 0.9150;
			baCoefficients[0][1] = -1.8299;
			baCoefficients[0][2] = 0.9150;
			baCoefficients[1][0] = 1.0000;
			baCoefficients[1][1] = -1.8227;
			baCoefficients[1][2] = 0.8372;
			break;
		case 300:
			baCoefficients[0][0] = 0.9286;
			baCoefficients[0][1] = -1.8572;
			baCoefficients[0][2] = 0.9286;
			baCoefficients[1][0] = 1.0000;
			baCoefficients[1][1] = -1.8521;
			baCoefficients[1][2] = 0.8623;
			break;
		case 360:
			baCoefficients[0][0] = 0.9402;
			baCoefficients[0][1] = -1.8803;
			baCoefficients[0][2] = 0.9402;
			baCoefficients[1][0] = 1.0000;
			baCoefficients[1][1] = -1.8767;
			baCoefficients[1][2] = 0.8839;
			break;
		case 500:
			baCoefficients[0][0] = 0.9565;
			baCoefficients[0][1] = -1.9131;
			baCoefficients[0][2] = 0.9565;
			baCoefficients[1][0] = 1.0000;
			baCoefficients[1][1] = -1.9112;
			baCoefficients[1][2] = 0.9150;
			break;
	}

	return baCoefficients;
}