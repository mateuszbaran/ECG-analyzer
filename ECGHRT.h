#pragma once
#ifndef HRTDATAX
#define HRTDATAX exists

#include<QPointF>
#include<QVector>
#include<QLine>
#include <iostream>
using namespace std;

class ECGHRT
{
public:
  ECGHRT (void);
  ~ECGHRT (void);

  //Dane do wykresu
  int offset;
  QVector<QPointF> rr;
  QLine ts;

  //Dane do wyświetlania
  int vpcCounter;		//  ilość znalezionych VPC,
  int isCorrect; // W razie jeśli wykryto zbyt mało VPC, lub w przypadku wystąpienia innych wad sygnału (np. zbyt krótki zapis) - ustawiana jest ta flag
				   // czyli, GOŚCIU OD WYKRESÓW, JEŚLI WIDZISZ FLAGĘ == 0, WYŚWIETL KOMUNIKAT O BŁĘDZIE 
  double TO;			// wartość Turbulence Onset - średnia z pojedynczych TO
  //double TO_2;		//wartość Turbulence Onset - obliczone na AvgTach
  double TS;			// wartość Turbulence Slope
  
 //Pozostałe	
	const int SLENGTH; // Każdy z poniższych sygnałów będzie miał indeksy od 0 DO SLENGTH
	double *avgSignal;			// uśredniony sygnał
	int ALL_SIGNALS_LENGTH;
	double **allSignals;		// wszystkie wycięte z sygnału fragmenty zawierające VPC
	double *straightSignal; //  punkty z najlepszej prostej TS wyznaczone dla argumentów
	
	const double x1_to; // współrzędne odcinka Turbulence Onset metoda 1 // początek i koniec
	double y1_to;
	double length_to; // ujemna długość -> malowanie w górę, dodatnia malowanie w dół


	void setAllSignalsSize(int size);
	double getMaxValue();

};

#endif