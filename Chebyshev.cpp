#include "Chebyshev.h"

Chebyshev::Chebyshev(){}
Chebyshev::~Chebyshev(){}

/*
	filterDesign for the time being returns precalculated
	values for Chebyshev 3rd order highpass filter,
	given the sample frequency of source signal
*/
std::vector<std::vector<double>> Chebyshev::filterDesign(int order, double ripple, double cutoffFreq, int sampleFreq, int type)
{
	std::vector<std::vector<double>> cbCoefficients(2, std::vector<double>(4,0));

	switch(sampleFreq){
		case 200:
			cbCoefficients[0][0] = 0,9953;
			cbCoefficients[0][1] = -2.9858;
			cbCoefficients[0][2] = 2,9858;
			cbCoefficients[0][3] = -0,9953;
			cbCoefficients[1][0] = 1.0000;
			cbCoefficients[1][1] = -2,9904;
			cbCoefficients[1][2] = 2,9811;
			cbCoefficients[1][3] = -0,9906;
			break;
		case 250:
			cbCoefficients[0][0] = 0,9963;
			cbCoefficients[0][1] = -2,9887;
			cbCoefficients[0][2] = 2,9887;
			cbCoefficients[0][3] = -0,9963;
			cbCoefficients[1][0] = 1.0000;
			cbCoefficients[1][1] = -2,9924;
			cbCoefficients[1][2] = 2,9850;
			cbCoefficients[1][3] = -0,9926;
			break;
		case 300:
			cbCoefficients[0][0] = 0.9969;
			cbCoefficients[0][1] = -2.9906;
			cbCoefficients[0][2] = 2.9906;
			cbCoefficients[0][3] = -0.9969;
			cbCoefficients[1][0] = 1.0000;
			cbCoefficients[1][1] = -2.9937;
			cbCoefficients[1][2] = 2.9875;
			cbCoefficients[1][3] = -0.9938;
			break;
		case 360:
			cbCoefficients[0][0] = 0,9974;
			cbCoefficients[0][1] = -2,9921;
			cbCoefficients[0][2] = 2,9921;
			cbCoefficients[0][3] = -0,9974;
			cbCoefficients[1][0] = 1.0000;
			cbCoefficients[1][1] = -2.9947;
			cbCoefficients[1][2] = 2.9895;
			cbCoefficients[1][3] = -0.9948;
			break;
		case 500:
			cbCoefficients[0][0] = 0.9981;
			cbCoefficients[0][1] = -2.9944;
			cbCoefficients[0][2] = 2.9944;
			cbCoefficients[0][3] = -0.9969;
			cbCoefficients[1][0] = 1.0000;
			cbCoefficients[1][1] = -2.9962;
			cbCoefficients[1][2] = 2.9925;
			cbCoefficients[1][3] = -0.9938;
			break;
	}

	return cbCoefficients;
}