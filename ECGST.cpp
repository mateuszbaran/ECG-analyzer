#include "ECGST.h"
#include <cstdlib>
#include <cstdio>

ECGST::ECGST (void) { }

ECGST::~ECGST (void) { }

double ECGST::Interval::thresh = 0.05;

ECGST::Interval::Interval(int iso, int j, int st, double sl, double of) :
  isopoint(iso), jpoint(j), stpoint(st), slope(sl), offset(of)
{ }

bool ECGST::Interval::higher(const double &thresh) const
{
  return offset < -1*thresh;
}

bool ECGST::Interval::lower(const double &thresh) const
{
  return offset > thresh;
}

bool ECGST::Interval::normal(const double &thresh) const
{
  return !(lower(thresh) || higher(thresh));
}

void ECGST::addInterval(ECGST::Interval interval)
{
  //printf("ST interval info: J: %d, ST %d, slope: %f offset: %f\n", interval.jpoint, interval.stpoint, interval.slope, interval.offset);
  intervals.push_back(interval);
}

const std::vector< ECGST::Interval >& ECGST::getIntervals() const
{
 return intervals;
}

const std::vector< ECGST::Episode >& ECGST::getEpisodes() const
{
  return episodes;
}



ECGST::Episode::Episode(int s, int e):
  start(s), end(e)
{

}

std::pair< int, int > ECGST::getIntervalBeginAndEnd(int i) const
{
  auto interval = intervals.at(i);
  return std::make_pair(interval.jpoint - 50, interval.stpoint+20);
}

std::pair< int, int > ECGST::getEpisodeBeginAndEnd(int i) const
{
  auto ep = episodes.at(i);
  return std::make_pair(ep.start-40, ep.end+10);
}


void ECGST::episodeAnalysis(int freq, double thresh)
{
  int _60s_in_samples = freq*60;
  
  bool episode = false;
  int start = 0;
  
  for(auto it = intervals.begin(); it != intervals.end(); ++it) {
    if (episode && it->normal(thresh)) {
      //printf("BACK TO NORMAL %d (%d)\n", it->stpoint, it->stpoint - start);
      if (it->stpoint - start > _60s_in_samples) {
        //printf("FOUND EPISODE! %d - %d\n", start, it->jpoint);
        episodes.push_back(Episode(start, it->stpoint));
      }
      episode = false;
    }
    
    if (!episode && ! it->normal()) {
      //printf("STH wrong! %d %F\n", it->jpoint, it->offset);
      start = it->jpoint;
      episode = true;
    } 
  }
}



