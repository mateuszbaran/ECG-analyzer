// HRTAnalysator.cpp : main project file.

/*
WYKORZYSTYWANE DO WYLICZENIA PROSTEJ REGRESJI
 */

#include "Matrix.h"
using namespace std;
	
int Matrix::getSizeX()
{ 
	return sizeX; 
}

int Matrix::getSizeY()
{ 
	return sizeY; 
}

Matrix::Matrix()
{
	sizeX = 0;
	sizeY = 0;
}

Matrix::Matrix(double *temp, int x, int y)
{
	sizeX = x;
	sizeY = y;
	tab = new double*[x];
	for(int i = 0; i < x; i++ )
	{
		tab[i] = new double[y];
	}

	for(int i = 0; i < x; i++ )
	{
		for(int j = 0; j < y; j++ )
		{
			tab[i][j] = temp[i];
		}
	}
}

Matrix::Matrix(double **temp, int x, int y)
{
	sizeX = x;
	sizeY = y;
	tab = new double*[x];
	for(int i = 0; i < x; i++ )
	{
		tab[i] = new double[y];
	}

	for(int i = 0; i < x; i++ )
	{
		for(int j = 0; j < y; j++ )
		{
			tab[i][j] = temp[i][j];
		}
	}
}

// pusta macierz
Matrix::Matrix(int x, int y) 
{
	sizeX = x;
	sizeY = y;
	tab = new double*[x];
	for(int i = 0; i < x; i++ )
	{
		tab[i] = new double[y];
	}

	for(int i = 0; i < x; i++ )
	{
		for(int j = 0; j < y; j++ )
		{
			tab[i][j] = 0.0;
		}
	}
}

// Transpozycja macierzy M2x3 -> M3x2
Matrix Matrix::invert()
{
	Matrix *mm = new Matrix(sizeY, sizeX);
	for(int i = 0; i < sizeX; i++)
	{
		for(int j = 0; j < sizeY; j++)
		{
			mm->set(j, i, tab[i][j]);
		}
	}
	
	return *mm;
}

Matrix Matrix::operator=(Matrix m)
{
	Matrix *mm = new Matrix(sizeX, sizeY);
	for(int i = 0; i < sizeX; i++)
	{
		for(int j = 0; j < sizeY; j++)
		{
			mm->set(i, j, tab[i][j]);
		}
	}
	
	return *mm;
}

// mno¿enie dwóch macierzy (zwyk³e mno¿enie macierzowe)
Matrix Matrix::operator*(Matrix m)
{
	if(sizeX != m.getSizeY())
	{
		Matrix *mm = new Matrix();
		return *mm;
	}
	else
	{
		double **stab = new double*[sizeY];
		for(int i = 0; i < sizeY; i++)
		stab[i] = new double[m.sizeX];
		
		for(int i = 0; i < sizeY; i++)
		{
			for(int j = 0; j < m.sizeX; j++)
			{
				stab[j][i] = 0;
				for(int k=0; k < sizeX; k++)
				{
					stab[j][i] += m(j,k) * tab[k][i];				
				}
			}
		}
		Matrix *mm = new Matrix(stab, m.sizeX, sizeY);
		return *mm;
	}		
}

// dodawanie liczby do ka¿dego elementu macierzy
Matrix Matrix::operator+(int val)
{
	double **stab = new double*[sizeX];
	for(int i = 0; i < sizeX; i++)
	stab[i] = new double[sizeY];
		
	for(int i = 0; i < sizeX; i++)
	{
		for(int j = 0; j < sizeY; j++)
		{
			stab[i][j] = 0;
			stab[i][j] += tab[i][j] + val;  
		}
	}

	Matrix *mm = new Matrix(stab, sizeX, sizeY);
	return *mm;
}

// mno¿enie ka¿dego elementu macierzy przez liczbê	
Matrix Matrix::operator*(int val)
{
	double **stab = new double*[sizeX];
	for(int i = 0; i < sizeX; i++)
	stab[i] = new double[sizeY];
		
	for(int i = 0; i < sizeX; i++)
	{
		for(int j = 0; j < sizeY; j++)
		{
			stab[i][j] = 0;
			stab[i][j] += tab[i][j] * val;  
		}
	}

	Matrix *mm = new Matrix(stab, sizeX, sizeY);
	return *mm;
}

// ³¹czenie dwóch macierzy poziomo
Matrix Matrix::join(Matrix b)
{
	if(sizeY == b.getSizeY())
	{
		double **stab = new double*[sizeX + b.getSizeX()];
		for(int i = 0; i < sizeX + b.getSizeX(); i++)
		stab[i] = new double[sizeY];
		
		for(int i = 0; i < sizeX + b.getSizeX(); i++)
			for(int j = 0; j < sizeY; j++)
			{
				if(i < sizeX)
				stab[i][j] = tab[i][j];
				else
				stab[i][j] = b(i - sizeX,j);
			}
			Matrix * mm = new Matrix(stab, sizeX + b.getSizeX(), sizeY);
			return *mm;
	}
	else 
	{
		cout<<"pudlo";
		Matrix * mm = new Matrix();
		return *mm;
	}
}

// ustawianie wartoœci elementu macierzy
void Matrix::set(int x, int y, double value)
{
	if(x >= 0 && x < sizeX && y >= 0 && y < sizeY)
	{
		tab[x][y] = value;
	}
}

//  pobranie wartoœci z macierzy o wsp x i y
double Matrix::operator()(int x, int y)
{
	if(x >= 0 && x < sizeX && y >= 0 && y < sizeY)
	{
		return tab[x][y];
	}
	double xs = 0.0;
	return xs;
}
	
// Rozwi¹zywanie prostego uk³adu równañ
Parameters Matrix::calculate(double A1, double B1, double V1, double A2, double B2,  double V2)
{
	Parameters res;
	res.isError=0;
	if((A1==0.0 && A2==0.0) || (B1==0.0 && B2==0.0) ||(A1*B2-B1*A2==0.0))
	{
		res.isError=1;
		return res;
	}
	else
	{
		if(A1 != 0.0 && A2 != 0.0 && B1 != 0.0 && B2 != 0.0 )
		{
			double Ax1 = A1*A2/A1;
			double Bx1 = B1*A2/A1;
			double Vx1 = V1*A2/A1;
			A2-=Ax1;
			B2-=Bx1;
			V2-=Vx1;
		}
		if(A1==0 && B2==0)
		{
			res.B=V1/B1;
			res.A=V2/A2;
		}
		else if(A2==0 && B1==0)
		{
			res.B=V2/B2;
			res.A=V1/A1;
		}
		else if(A1==0)
		{
			res.B=V1/B1;
			res.A=(V2-B2*res.B)/A2;
		}
		else if(A2==0)
		{
			res.B=V2/B2;
			res.A=(V1-B1*res.B)/A1;
		}
		else if(B1==0)
		{
			res.A = V1/A1;
			res.B = (V2-A2*res.B)/B2;
		}
		else if(B2==0)
		{
			res.A = V2/A2;
			res.B = (V1-A1*res.B)/B1;
		}
		else
		{
			res.isError = 1;
		}
	}

	return res;
}

// Obliczanie parametrów prostej regresji dla 5 punktów!! (w naszym przypadku)
// Testowa³em TYLKO dla piêciopunktowych!!
Parameters Matrix::getLinearEquationParameters(double x[], double y[], int size)
{
	Matrix *m = new Matrix(x,5,1);
	Matrix *m2 = new Matrix(y,5,1);

	Matrix X= (*m).invert();
	Matrix Y= (*m2).invert();

	int sizex = X.getSizeY();
	double tab[]= {1.0,1.0,1.0,1.0,1.0};

	Matrix *ones = new Matrix(tab,1,5);
	Matrix Aa = X.join(*ones);
	
	Matrix LEFT = (Aa.invert())*Aa;
	Matrix RIGHT = (Aa.invert())*Y;
	
	Parameters ss;
	if(LEFT.getSizeX() != 2 || LEFT.getSizeY() != 2 || RIGHT.getSizeX() != 1 || RIGHT.getSizeY() != 2)
	{
		ss.isError=1;
		return ss;
	}

	Parameters ss2 = Matrix::calculate(LEFT(0,0), LEFT(1,0), RIGHT(0,0), LEFT(0,1), LEFT(1,1), RIGHT(0,1));
			
	return ss2;
}


std::ostream& operator<<(std::ostream& str,  Matrix& m)
{
	str<<endl;
	for(int i = 0; i < m.sizeY; i++)
	{
		for(int j = 0; j < m.sizeX; j++)
		{
			str<<m.tab[j][i]<<" ";
		}
		str<<endl;
	}
	return str;
}
    
//
//int main(array<System::String ^> ^args)
//{
//	double x[] = {1, 2, 3, 4, 5};
//	double y[] = {1, 3, 3, 3, 5}; 
//
//	// Parameters result = calculate(1.0, 15.0, 53.0, 15.0, 5.0, 15.0);
//	// if(result.isError) cout<<"NIE MOZNA WYLICZYC!!"<<endl;
//	// else
//	// {
//	//	cout<<"A: "<<result.A<<endl;
//	//	cout<<"B: "<<result.B<<endl;
//	// }
//	Parameters result = Matrix::getLinearEquationParameters(x,y,5);
//	if(result.isError)
//	{
//		cout<<"BLEDNE WARTOSCI"<<endl;
//	}
//	else
//	{
//		cout<< "A: " << result.A << endl;
//		cout<< "B: " << result.B << endl;
//	}
//	/*
//	double **stab = new double*[4]; // zawsze najpierw podajemy Y!!
//
//	stab[0] = new double[3];
//	stab[1] = new double[3];
//	stab[2] = new double[3];
//	stab[3] = new double[3];
//
//	stab[0][0] = 2;
//	stab[0][1] = 1;
//
//	Matrix *m = new Matrix(stab,1,2);
//
//	stab[0][0] = 1;
//	stab[0][1] = 1;
//	stab[1][0] = 1;
//	stab[1][1] = 2;
//	
//	Matrix *m2 = new Matrix(stab,2,2);
//	
//	cout<<(*m);
//	cout<<(*m2);
//	cout<<(*m2)*(*m);
//	*/
//	/*
//		Matrix *m2 = new Matrix(stab,3,2);
//		cout<<(*m);
//		cout<<(*m2);
//
//		cout<<(*m).join(*m2);
//	
//		cout<<(*m) + 5;
//		cout<<(*m) * 5;
//		cout<<(*m).invert();
//		cout<<(*m2)*(*m);
//	*/
//
//	system("pause");
//    return 0;
//}
//