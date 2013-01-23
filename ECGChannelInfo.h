#pragma once

#include <string>


struct ECGChannelInfo
{
public:
  std::string filename;
  std::string description;
  int signal_resolution;
  int zero_signal;
  double gain;
  int nr_samples;
  //this is not present in WFDB Siginfo - value from description mit-bih
  int frequecy;
  /* this is not present in WFDB Siginfo - value from description mit-bih, value in mV
  */
  int range; 

  /* in msec */
  unsigned int signal_duration;

  static std::string sampleToTime(int sample, int frequency);
  std::string sampleToTime(int sample) const;
};

struct ECGInfo
{
public:
  ECGChannelInfo channel_one;
  ECGChannelInfo channel_two;
};
