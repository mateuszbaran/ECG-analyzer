#include "ECGST.h"
#include <cstdlib>
#include <cstdio>

ECGST::ECGST (void) { }

ECGST::~ECGST (void) { }

double ECGST::Interval::thresh = 0.05;

ECGST::Interval::Interval(int j, int st, double sl, double of) :
  jpoint(j), stpoint(st), slope(sl), offset(of)
{ }

bool ECGST::Interval::higher() const
{
  return offset < -1*ECGST::Interval::thresh;
}

bool ECGST::Interval::lower() const
{
  return offset > ECGST::Interval::thresh;
}

bool ECGST::Interval::normal() const
{
  return !(lower() || higher());
}

void ECGST::addInterval(ECGST::Interval interval)
{
  //printf("ST interval info: J: %d, ST %d, slope: %f offset: %f\n", interval.jpoint, interval.stpoint, interval.slope, interval.offset);
  intervals.push_back(interval);
}

ECGST::Episode::Episode(int s, int e):
  start(s), end(e)
{

}


void ECGST::episodeAnalysis(int freq)
{
  int _60s_in_samples = freq*60;
  int _30s_in_samples = freq*30;
  
  bool episode = false;
  int start = 0;
  
  for(auto it = intervals.begin(); it != intervals.end(); ++it) {
    if (episode && it->normal()) {
      printf("BACK TO NORMAL %d (%d)\n", it->stpoint, it->stpoint - start);
      if (it->stpoint - start > _60s_in_samples) {
        printf("FOUND EPISODE! %d - %d\n", start, it->jpoint);
        episodes.push_back(Episode(start, it->stpoint));
      }
      episode = false;
    }
    
    if (!episode && ! it->normal()) {
      printf("STH wrong! %d %F\n", it->jpoint, it->offset);
      start = it->jpoint;
      episode = true;
    } 
  }
}



