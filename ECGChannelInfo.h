#pragma once

#include <string>

struct ECGChannelInfo
{
  std::string filename;
  std::string description;
};

struct ECGInfo
{
public:
  ECGChannelInfo channel_one;
  ECGChannelInfo channel_two;
};