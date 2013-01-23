#ifndef STANALYSIS_H
#define STANALYSIS_H

#include <ModulesInterfaces.h>


class STAnalysis : public STIntervalModule
{

public:
  enum AlgorithmType {
    Simple,
    Complex
  };
  
  STAnalysis();
  
  virtual void runModule(const ECGRs&, const ECGWaves&, const ECGSignalChannel&, const ECGInfo&, ECGST&);
  void setParams(ParametersTypes& );
  
  virtual ~STAnalysis();
  
private:
  class AbstractAnalizer {
  public:
    virtual void analyse(const int it, const ECGRs& rpeaks, const ECGWaves& waves, const ECGSignalChannel& signal, const ECGChannelInfo& info, ECGST&) = 0;
    virtual void setParams(ParametersTypes&);
    virtual ~AbstractAnalizer() {};
  protected:
    ParametersTypes params;
  };
  
  class SimpleAnalizer: public AbstractAnalizer {
  public:
    SimpleAnalizer();
    void analyse(const int it, const ECGRs& rpeaks, const ECGWaves& waves, const ECGSignalChannel& signal, const ECGChannelInfo& info, ECGST& output);
    virtual void setParams(ParametersTypes& );
  private:
    double thresh;
    int start;
    bool during_episode;
  };
  
  class ComplexAnalizer: public AbstractAnalizer {
  public:
    ComplexAnalizer();
    void analyse(const int it, const ECGRs& rpeaks, const ECGWaves& waves, const ECGSignalChannel& signal, const ECGChannelInfo& info, ECGST& output);
    virtual void setParams(ParametersTypes& );
  private:
    std::pair<int, double> maxDistanceSample(const OtherSignal&, int, int);
    std::pair<int, int> overBelowSamples(const OtherSignal&, int, int);
    int getTPeak(const OtherSignal&, int, int);
    double thresh, type_thresh, slope_thresh;
    int start;
    bool during_episode;
  };
  
  AbstractAnalizer * analizator;
  void setAnalizer(STAnalysis::AbstractAnalizer* a);
  void setAnalizer(AlgorithmType atype = AlgorithmType::Simple);
  
#ifdef DEVELOPMENT
  ECGRs read_normal_r_peaks(std::string path, std::string filename);
#endif
};

#endif // STANALYSIS_H
