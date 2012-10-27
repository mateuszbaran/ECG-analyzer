#pragma once

#include "ECGClasses.h"
#include "ECGController.h"
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
  virtual void setParams (ParametersTypes &) = 0;
};

class ECGBaselineModule : public GlobalInterface
{
public:
  virtual void runModule (const ECGSignal &, ECGSignal &) = 0;
};

class RPeaksModule : public GlobalInterface
{
public:
  virtual void runModule (const ECGSignal &, ECGRs &) = 0;
};

class WavesModule : public GlobalInterface
{
public:
  virtual void runModule (const ECGSignal &, const ECGRs &, ECGWaves &) = 0;
};

class HRV1Module : public GlobalInterface
{
public:
  virtual void runModule (const ECGRs &, ECGHRV1 &) = 0;
};

class HRV2Module : public GlobalInterface
{
public:
  virtual void runModule (const ECGRs &, ECGHRV2 &) = 0;
};

class HRVDFAModule : public GlobalInterface
{
public:
  virtual void runModule (const ECGRs &, ECGHRVDFA &) = 0;
};

class QRSClassModule : public GlobalInterface
{
public:
  virtual void runModule (const ECGWaves &, const ECGSignal &, ECGClasses &) = 0;
};

class STIntervalModule : public GlobalInterface
{
public:
  virtual void runModule (const ECGWaves &, const ECGSignal &, ECGST &) = 0;
};

class TWaveAltModule : public GlobalInterface
{
public:
  virtual void runModule (const ECGWaves &, const ECGSignal &, ECGTWave &) = 0;
};

class HRT : public GlobalInterface
{
public:
  virtual void runModule (const ECGWaves &, const ECGRs &, const ECGSignal &, ECGHRT &) = 0;
};