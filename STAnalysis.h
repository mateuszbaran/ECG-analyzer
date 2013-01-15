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
  class AbstractAnalizator {
  public:
    virtual void analyse(const int it, const ECGRs& rpeaks, const ECGWaves& waves, const ECGSignalChannel& signal, const ECGChannelInfo& info, ECGST&) = 0;
    virtual void setParams(ParametersTypes&);
    virtual ~AbstractAnalizator() {};
  protected:
    ParametersTypes params;
  };
  
  class SimpleAnalizator: public AbstractAnalizator {
  public:
    SimpleAnalizator();
    void analyse(const int it, const ECGRs& rpeaks, const ECGWaves& waves, const ECGSignalChannel& signal, const ECGChannelInfo& info, ECGST& output);
    virtual void setParams(ParametersTypes& );
  private:
    double thresh;
    int start;
    bool during_episode;
  };
  
  class ComplexAnalizator: public AbstractAnalizator {
  public:
    ComplexAnalizator();
    void analyse(const int it, const ECGRs& rpeaks, const ECGWaves& waves, const ECGSignalChannel& signal, const ECGChannelInfo& info, ECGST& output);
    virtual void setParams(ParametersTypes& );
  private:
    std::pair<int, double> maxDistanceSample(OtherSignal);
    double thresh;
    int start;
    bool during_episode;
  };
  
  AbstractAnalizator * analizator;
  double dt;
  double thresh;
  void setAnalizator(STAnalysis::AbstractAnalizator* a = nullptr);
  void setAnalizator(AlgorithmType atype = AlgorithmType::Simple);
  
  AlgorithmType algorithmTypeFromInt(int value = 0) const;
  
  ECGRs read_normal_r_peaks(std::string path, std::string filename);
};

#endif // STANALYSIS_H
