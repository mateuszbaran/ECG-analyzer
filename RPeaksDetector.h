#pragma once
// Main includes
#include "ModulesInterfaces.h"
#include "ModulesMethods.h"
// PanTompkins includes
#include <fstream>
#include <sstream>
// Hilbert includes
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <list>
#include <cmath>
#include <climits>
#include <vector>



// *************************************************************
// funkcja oblicza RMS ci�gu "w"
// *************************************************************
double oblicz_rms (const std::vector<double> & w);

// *************************************************************
// funkcja oblicza maksimum z abs(w)
// *************************************************************
double oblicz_max_abs (const std::vector<double> & w);


// sygnal - dany sygnal
// czestotliwosc - jego czetotliwosc
// y - wektor w kt�rym umieszczone s� wyniki dzia�ania funkcji
// *************************************************************
// funkcja oblicza pochodn� sygna�u o zadanej cz�totliwo�ci
// *************************************************************
void rozniczkuj(const std::vector<double> &sygnal, int czestotliwosc, 
		std::vector<double> &y);

// sygnal - zmieniany sygnal
// *****************************************************************
//   fukcja odejmuje od sygnalu jego sredni� warto�� a nast�pnie 
//   ka�dy element wektoa zamienia na jego warto�� bezwzgl�dn�.
//   Po co to? Ano po to, by p�nejsza naliza transformacji pochodnej 
//   bra�a pod uwag� tak�e "dolne" R-peaki
// *****************************************************************
void zmien_sygnal_na_abs(std::vector<double> &s);

//---------------------------------------------------------------------------------------------------

void detekcja_r_hilbert(const std::vector<double> & sygnal,
	int czestotliwosc,
	std::vector<int> & numery_R);

	//--------------------------------------------------------------------------------------------------------


// Test definitions
//#define DEBUG
//#define DEBUG_SIGNAL
//#define DEBUG_SIGNAL_DETAILS
#define USE_MOCKED_SIGNAL

using namespace std;

/**
 * Class RPeaksDetector provides set of method to detect R peaks in ECG signal.
 * @class RPeaksDetector
 */
class RPeaksDetector : public RPeaksModule
{
public:

	RPeaksDetector();
	~RPeaksDetector();

	void runModule(const ECGSignalChannel &, const ECGInfo &, ECGRs &);
	void setParams(ParametersTypes &);

  /**
  *  Execute R peaks detection
  *  @return false if detection cannot be executed
  *  @return true if detection is complete
  */
  bool detectRPeaks();

private:
  /**
  *  Information about detection status
  */
  bool rsDetected;

  /**
  *  Information about parameters
  */
  bool customParameters;

  /**
  *  Filtered signal from 'ECG_BASALINE'
  */
  ECGSignalChannel filteredSignal;
  
  /**
  * Signal frequency
  */
  int signalFrequency;
  /**
  *  R peaks vector
  */
  ECGRs * rsPositions;

  /**
  *  R peaks detection method
  */
  R_PEAKS_DETECTION_METHOD detectionMethod;
  
  /**
  *  PanTompkins movingh window lenght
  */
  int panTompkinsMovinghWindowLenght;

  /**
  *  PanTompkins thershold size
  */
  double panTompkinsThershold;

  /**
  *  PanTompkins R peaks method detection
  *  @param pointer to ECG signal
  */
  bool panTompkinsRPeaksDetection(ECGSignalChannel *signal);
  
  /**
  *  Hilbert R peaks method detection
  *  @param pointer to ECG signal
  */
  bool hilbertRPeaksDetection(ECGSignalChannel *signal);


  /**
  *  Filtered mocked signal
  */
  ECGSignalChannel mockedSignal;

  /*
  * Returns a part of filtered signal
  * This function is used only for tests!
  */
  ECGSignalChannel getMockedSignal();

};

/**
 * Implementation RPeaksDetectionException
 * This exception will be thrown if any problems occour during R peaks detection
 * @class RPeaksDetector
 */
class RPeaksDetectionException
{
public:
  RPeaksDetectionException(string cause)
  {
    this->cause = cause;
  }

private:
	string cause;

  virtual const string what() const throw()
  {
    return "Error during execution R preaks module cause: " + cause;
  }
};
