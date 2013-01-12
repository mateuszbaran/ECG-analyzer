#pragma once
#include <vector>

class STAnalysis;

class ECGST
{
public:
  class Interval;
  
  ECGST (void);
  ~ECGST (void);
  void addInterval(ECGST::Interval interval);
  
  class Interval {
  public:
    Interval(int j = 0, int st = 0, double sl = 0.0, double of = 0.0);
    int jpoint;
    int stpoint;
    double slope;
    double offset;
    static double thresh;
    inline bool lower() const;
    inline bool higher() const;
    inline bool normal() const;
  };
  
  class Episode {
  public:
    Episode (int s = 0, int e = 0);
    int start, end;
  };
  
private:
  std::vector<Interval> intervals;
  std::vector<Episode> episodes;
  void episodeAnalysis(int freq);
  
    
  friend class STAnalysis;
};

