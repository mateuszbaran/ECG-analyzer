#pragma once

#include "ECGSignal.h"
#include "ModulesMethods.h"

/**
 * Class ECGRs provides set of method to detect R peaks in ECG signal.
 * @class R peaks.
 */
class ECGRs
{
public:
  /**
  *  Default ctor
  *  Default detection method 'PanTompkins'
  *  NOTE: It is necessary to set filtered signal before start R peaks detection
  */
  ECGRs ();
  
  /**
  *  ctor
  *  Default detection method 'PanTompkins'
  *  Ctor starts R peaks detection automaticaly
  *  @param filtered signal
  */
  ECGRs (IntSignal filteredSignal);

  /**
  *  ctor
  *  Ctor starts R peaks detection automaticaly
  *  @param filteredSignal filtered signal from 'ECG_BASALINE'
  *  @param R_PEAKS_DETECTION_METHOD detection method
  */
  ECGRs (IntSignal filteredSignal, R_PEAKS_DETECTION_METHOD detectionMethod);

  /**
  *  Default destructor
  */
  ~ECGRs (void);

  /**
  *  @return vector with R peaks position
  */
  IntSignal GetRs () const
  {
    return rsPosition;
  }
  /**
  *  Check R peaks detection status
  *  @return true if detection is finisched
  */
  bool areRsDetected();

  /**
  *  Set new filtered signal
  *  @param filteredSignal filtered signal
  */
  void setFilteredSignal(IntSignal filteredSignal);

  /**
  *  Get current filtered signal
  *  @return filtered signal
  */
  IntSignal getFilteredSignal();

  /**
  *  Set new filtered method
  *  @param detectionMethod detection method
  */
  void setDetectionMethod(R_PEAKS_DETECTION_METHOD detectionMethod);

  /**
  *  Get current filtered method
  *  @return filtered method
  */
  R_PEAKS_DETECTION_METHOD getDetectionMethod();

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
  *  Vector with R peaks position
  */
  IntSignal rsPosition;

  /**
  *  Filtered signal from 'ECG_BASALINE'
  */
  IntSignal filteredSignal;

  /**
  *  R peaks detection method
  */
  R_PEAKS_DETECTION_METHOD detectionMethod;

  /**
  *  PanTompkins R peaks method detection
  *  @param pointer to ECG signal
  */
  void panTompkinsRPeaksDetection(IntSignal *signal);
  
  /**
  *  Hilbert R peaks method detection
  *  @param pointer to ECG signal
  */
  void hilbertRPeaksDetection(IntSignal *signal);
};

