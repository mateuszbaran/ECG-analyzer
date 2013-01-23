#include "ECGHRV2.h"

ECGHRV2::ECGHRV2 (void)
	:  SD1(0), SD2(0), TINN(0), M(0), N(0), HRVTriangularIndex(0), Y(0), X(0), HistogramBinLength(7.8125)
{}

ECGHRV2::~ECGHRV2 (void)
{}


IntSignal ECGHRV2::GetHistogram_x () 
{
	return histogram_x;
}

IntSignal ECGHRV2::GetHistogram_y () 
{
	return histogram_y;
}

IntSignal ECGHRV2::GetPoincare_x () 
{
	return poincare_x;
}

IntSignal ECGHRV2::GetPoincare_y () 
{
	return poincare_y;
}

double ECGHRV2::GetSD1()
{
	return SD1;
}

double ECGHRV2::GetSD2()
{
	return SD2;
}

double ECGHRV2::GetTINN()
{
	return TINN;
}

double ECGHRV2::GetM()
{
	return M;
}

double ECGHRV2::GetN()
{
	return N;
}

double ECGHRV2::GetHRVTriangularIndex()
{
	return HRVTriangularIndex;
}

double ECGHRV2::GetY()
{
	return Y;
}

double ECGHRV2::GetX()
{
	return X;
}

double ECGHRV2::GetHistogramBinLength()
{
	return HistogramBinLength;
}

void ECGHRV2::SetHistogram_x(IntSignal histogram_x) 
{
	this->histogram_x=histogram_x;
}

void ECGHRV2::SetHistogram_y(IntSignal histogram_y) 
{
	this->histogram_y=histogram_y;
}

void ECGHRV2::SetPoincare_x(IntSignal poincare_x) 
{
	this->poincare_x=poincare_x;
}

void ECGHRV2::SetPoincare_y(IntSignal poincare_y) 
{
	this->poincare_y=poincare_y;
}

void ECGHRV2::SetSD1(double SD1)
{
	this->SD1=SD1;
}

void ECGHRV2::SetSD2(double SD2)
{
	this->SD2=SD2;
}

void ECGHRV2::SetTINN(double TINN)
{
	this->TINN=TINN;
}

void ECGHRV2::SetM(double M)
{
	this->M=M;
}

void ECGHRV2::SetN(double N)
{
	this->N=N;
}

void ECGHRV2::SetHRVTriangularIndex(double HRVTriangularIndex)
{
	this->HRVTriangularIndex=HRVTriangularIndex;
}

void ECGHRV2::SetY(double Y)
{
	this->Y=Y;
}

void ECGHRV2::SetX(double X)
{
	this->X=X;
}

void ECGHRV2::SetHistogramBinLength(double HistogramBinLength)
{
	this->HistogramBinLength=HistogramBinLength;
}