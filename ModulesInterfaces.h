#pragma once

#include "ECGClasses.h"
#include "ECGChannelInfo.h"
#include "ECGHRT.h"
#include "ECGHRV1.h"
#include "ECGHRV2.h"
#include "ECGHRVDFA.h"
#include "ECGRs.h"
#include "ECGSignal.h"
#include "ECGST.h"
#include "ECGTWave.h"
#include "ECGWaves.h"

#include <map>
#include <string>

typedef std::map<std::string, double> ParametersTypes;

class GlobalInterface
{
public:
  GlobalInterface() : run_(false) {}
  virtual void setParams (ParametersTypes &) = 0;
private:
  bool run_;
  friend class ECGController;
};

class ECGBaselineModule : public GlobalInterface
{
public:
  ECGBaselineModule() : GlobalInterface() {}
  virtual void runModule (const ECGSignal &, const ECGInfo &, ECGSignalChannel &) = 0;
};

class RPeaksModule : public GlobalInterface
{
public:
  RPeaksModule() : GlobalInterface() {}
  virtual void runModule (const ECGSignalChannel &, const ECGInfo &, ECGRs &) = 0;
};

class WavesModule : public GlobalInterface
{
public:
  WavesModule() : GlobalInterface() {}
  virtual void runModule (const ECGSignalChannel &, const ECGInfo &, const ECGRs &, ECGWaves &) = 0;
};

class HRV1Module : public GlobalInterface
{
public:
  HRV1Module() : GlobalInterface() {}
  virtual void runModule (const ECGInfo &, const ECGRs &, ECGHRV1 &) = 0;
};

class HRV2Module : public GlobalInterface
{
public:
  HRV2Module() : GlobalInterface() {}
  virtual void runModule (const ECGInfo &info, const ECGRs &, ECGHRV2 &) = 0;
};

class HRVDFAModule : public GlobalInterface
{
public:
  HRVDFAModule() : GlobalInterface() {}
  virtual void runModule (const ECGRs &, ECGHRVDFA &) = 0;
};

class QRSClassModule : public GlobalInterface
{
public:
  QRSClassModule() : GlobalInterface() {}
  virtual void runModule (ECGWaves &, const ECGSignalChannel &, const ECGRs &, const ECGInfo &, ECGClasses &) = 0;
};

class STIntervalModule : public GlobalInterface
{
public:
  STIntervalModule() : GlobalInterface() {}
  virtual void runModule (const ECGRs &, const ECGWaves &, const ECGSignalChannel &, const ECGInfo &, ECGST &) = 0;
};

class TWaveAltModule : public GlobalInterface
{
public:
  TWaveAltModule() : GlobalInterface() {}
  virtual void runModule (const ECGWaves &, const ECGSignalChannel &, ECGInfo &, ECGTWave &) = 0;
};

class HRTModule : public GlobalInterface
{
public:
  HRTModule() : GlobalInterface() {}
  virtual void runModule (const ECGRs &, const QRSClass &, const ECGInfo &, ECGHRT &) = 0;
};
