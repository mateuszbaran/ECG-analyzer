#pragma once
#include <vector>

class STAnalysis;

class ECGST
{
public:
  class Interval;
  class Episode;
  
  ECGST (void);
  ~ECGST (void);
  const std::vector< Interval > & getIntervals() const;
  const std::vector< Episode >& getEpisodes() const;
  std::pair<int,int> getIntervalBeginAndEnd(int) const;
  std::pair<int,int> getEpisodeBeginAndEnd(int) const;
  
  class Interval {
  public:
    Interval(int iso = 0, int j = 0, int st = 0, double sl = 0.0, double of = 0.0);
    int isopoint;
    int jpoint;
    int stpoint;
    double slope;
    double offset;
    static double thresh;
    inline bool lower(const double& thresh = ECGST::Interval::thresh) const;
    inline bool higher(const double& thresh = ECGST::Interval::thresh) const;
    inline bool normal(const double& thresh = ECGST::Interval::thresh) const;
  };
  
  class Episode {
  public:
    Episode (int s = 0, int e = 0);
    int start, end;
  };
  
private:
  void addInterval(ECGST::Interval interval);
  
  std::vector<Interval> intervals;
  std::vector<Episode> episodes;
  void episodeAnalysis(int freq, double thresh);
  
    
  friend class STAnalysis;
};

