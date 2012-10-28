#pragma once

#include "ECGClasses.h"
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
  GlobalInterface() : runned(false) {}
  virtual void setParams (ParametersTypes &) = 0;
  bool runned;
};

class ECGBaselineModule : public GlobalInterface
{
public:
  ECGBaselineModule() : GlobalInterface() {}
  virtual void runModule (const ECGSignal &, ECGSignal &) = 0;
};

class RPeaksModule : public GlobalInterface
{
public:
  RPeaksModule() : GlobalInterface() {}
  virtual void runModule (const ECGSignal &, ECGRs &) = 0;
};

class WavesModule : public GlobalInterface
{
public:
  WavesModule() : GlobalInterface() {}
  virtual void runModule (const ECGSignal &, const ECGRs &, ECGWaves &) = 0;
};

class HRV1Module : public GlobalInterface
{
public:
  HRV1Module() : GlobalInterface() {}
  virtual void runModule (const ECGRs &, ECGHRV1 &) = 0;
};

class HRV2Module : public GlobalInterface
{
public:
  HRV2Module() : GlobalInterface() {}
  virtual void runModule (const ECGRs &, ECGHRV2 &) = 0;
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
  virtual void runModule (const ECGWaves &, const ECGSignal &, ECGClasses &) = 0;
};

class STIntervalModule : public GlobalInterface
{
public:
  STIntervalModule() : GlobalInterface() {}
  virtual void runModule (const ECGWaves &, const ECGSignal &, ECGST &) = 0;
};

class TWaveAltModule : public GlobalInterface
{
public:
  TWaveAltModule() : GlobalInterface() {}
  virtual void runModule (const ECGWaves &, const ECGSignal &, ECGTWave &) = 0;
};

class HRTModule : public GlobalInterface
{
public:
  HRTModule() : GlobalInterface() {}
  virtual void runModule (const ECGWaves &, const ECGRs &, const ECGSignal &, ECGHRT &) = 0;
};