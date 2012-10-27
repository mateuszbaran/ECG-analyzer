#pragma once

#include <vector\gsl_vector.h>

/**
 * @class Class for parameters created in HRV1 module
 */
class ECGHRV1
{
public:
  ECGHRV1 (void);
  ~ECGHRV1 (void);

  double GetHF () const
  {
    return HF;
  }

  double GetLF () const
  {
    return LF;
  }

  double GetLFHF () const
  {
    return LFHF;
  }

  double GetpNN50 () const
  {
    return pNN50;
  }

  double GetRMSSD () const
  {
    return RMSSD;
  }

  double GetRR_avg () const
  {
    return RR_avg;
  }

  double GetRR_stddev () const
  {
    return RR_stddev;
  }

  double GetSDANN () const
  {
    return SDANN;
  }

  double GetSDANN_index () const
  {
    return SDANN_index;
  }

  double GetSDSD () const
  {
    return SDSD;
  }

  double GetTP () const
  {
    return TP;
  }

  double GetULF () const
  {
    return ULF;
  }

  double GetVLF () const
  {
    return VLF;
  }

  gsl_vector* GetFreqency () const
  {
    return freqency;
  }

  double GetNN50 () const
  {
    return NN50;
  }

  gsl_vector* GetPower () const
  {
    return power;
  }

  void SetHF (double HF)
  {
    this->HF = HF;
  }

  void SetLF (double LF)
  {
    this->LF = LF;
  }

  void SetLFHF (double LFHF)
  {
    this->LFHF = LFHF;
  }

  void SetpNN50 (double NN50)
  {
    this->pNN50 = NN50;
  }

  void SetRMSSD (double RMSSD)
  {
    this->RMSSD = RMSSD;
  }

  void SetRR_avg (double RR_avg)
  {
    this->RR_avg = RR_avg;
  }

  void SetRR_stddev (double RR_stddev)
  {
    this->RR_stddev = RR_stddev;
  }

  void SetSDANN (double SDANN)
  {
    this->SDANN = SDANN;
  }

  void SetSDANN_index (double SDANN_index)
  {
    this->SDANN_index = SDANN_index;
  }

  void SetSDSD (double SDSD)
  {
    this->SDSD = SDSD;
  }

  void SetTP (double TP)
  {
    this->TP = TP;
  }

  void SetULF (double ULF)
  {
    this->ULF = ULF;
  }

  void SetVLF (double VLF)
  {
    this->VLF = VLF;
  }

  void SetNN50 (double NN50)
  {
    this->NN50 = NN50;
  }


private:
  //time domain parameters
  double RR_avg;
  double RR_stddev;
  double RMSSD;
  double NN50;
  double pNN50;
  double SDANN;
  double SDANN_index;
  double SDSD;
  //frequency domain parameters
  double TP;
  double HF;
  double LF;
  double VLF;
  double ULF;
  double LFHF;
  //vectors are needed to plot frequency figure
  gsl_vector * freqency;
  gsl_vector * power;
};

