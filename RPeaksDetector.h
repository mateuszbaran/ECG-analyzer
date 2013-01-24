#pragma once
#include "fft/kiss_fft.h"
#include "fft/kiss_fftr.h"
#include "ModulesInterfaces.h"
#include "ModulesMethods.h"
#include <fstream>
#include <sstream>

#define ASSERT(where,what);
//#define DEBUG
//#define DEBUG_SIGNAL
//#define DEBUG_SIGNAL_DETAILS
//#define USE_MOCKED_SIGNAL

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
  
  ///////// Hilbert methods /////////
  /**
  *  Organizes hilbert R peaks detection
  */
  void hilbertDetection(const std::vector<double> & sygnal, int czestotliwosc, std::vector<int> & numery_R);
  
  /**
  *  Add new proposition to recognized R peaks
  */
  void addToRecognized( const std::list<int> &propozycje, std::vector<int> &wykryte);

  /**
  *  Leaves point sonly over threshold
  */
  void filterPropositions(int czestotliwosc, const std::vector<double> &h, double threshold, 
	  std::list<int> & propozycje, int przesuniecie);
  
  /**
  *  
  */
  void hilbert(const std::vector<double> & f, std::vector<double> & g);

  /**
  *  Process one differentiated signal window
  */
  void balanceWindow(const std::vector<double> &sygnal, int start, int rozmiar, 
	  const std::vector<double> &y, int czestotliwosc, std::vector<int> &wykryte);

  /**
  *  Remove to close points
  */
  void removeTooClose(int czestotliwosc, const std::vector<double> & sygnal,
	  const std::vector<int> &wykryte, std::vector<int> & R) ;

  ///////// HELPERS /////////
  /**
  *  Calculate RMS
  */
  double rmsCalculate (const std::vector<double> & w);

  /**
  *  Find max from abs vector
  */
  double maxFromAbs (const std::vector<double> & w);

  /**
  *  Differentiates for given frequency
  */
  void differentiates(const std::vector<double> &sygnal, int czestotliwosc, std::vector<double> &y);

  /**
  *  Make signal abs
  */
  void signalAbs(std::vector<double> &s);
  
  /**
  *  Chceks if numbers are almoust equals
  */
  bool almostEqual(double x, double y);

  /**
  *  Finds max value for given vector
  */
  int findMax( int nr, const std::vector<double> &sygnal, int czestotliwosc);
  
  /**
  *  
  */
  void realMax(int czestotliwosc, const std::vector<double> &sygnal, std::list<int> &propozycje) ;
  
  /**
  *  
  */
  void double2kiss(const std::vector<double> &f, kiss_fft_cpx * g);
  
  /**
  *  
  */
  void kiss2double(int n, kiss_fft_cpx * f, std::vector<double> &g);
  
  /**
  *  
  */
  void fft(const std::vector<double> & f, kiss_fft_cpx * g);
  
  /**
  *  
  */
  void ifft(int n, kiss_fft_cpx * f, std::vector<double> & g);
  
  /**
  *  
  */
  void multiplyBy1ppixTransformation( int n, kiss_fft_cpx * f );

  /**
  *  
  */
  double average(const std::vector<double> & w);

  /**
  *  
  */
  double round(double liczba);

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
