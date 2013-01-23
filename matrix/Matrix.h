#include <iostream>

#ifndef MATRIXX
#define MATRIXX exists
// HRTAnalysator.cpp : main project file.

// Klasy wykorzystywane przy obliczeniach macierzowych
class Parameters //linear equation parameters
{
	public:
	int isError;
	double A;
	double B;
};

class Matrix
{
	private:
	
	int sizeX;

	int sizeY;

	double** tab;

	public:

	int getSizeX();

	int getSizeY();

	Matrix();

	Matrix(double *temp, int x, int y);

	Matrix(double **temp, int x, int y);

	Matrix(int x, int y);

	Matrix invert();

	Matrix operator=(Matrix m);

	Matrix operator*(Matrix m);	

	Matrix operator+(int val);
	
	Matrix operator*(int val);

	Matrix join(Matrix b);

	void set(int x, int y, double value);

	double operator()(int x, int y);
	
	static Parameters calculate(double A1, double B1, double V1, double A2, double B2,  double V2);

	static Parameters getLinearEquationParameters(double x[], double y[], int size);

	friend std::ostream& operator<<(std::ostream&, Matrix&);
	
	static void test();
};

std::ostream& operator<<(std::ostream& str,  Matrix& m); // tylko tymczasowo, wiêc nie ma sensu dawaæ gdzie indziej

#endif