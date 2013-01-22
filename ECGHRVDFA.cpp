#include "ECGHRVDFA.h"

ECGHRVDFA::ECGHRVDFA (void)
{}

ECGHRVDFA::~ECGHRVDFA (void)
{}

void ECGHRVDFA::setDFA_scaling_factor(double DFA_sf)
{
	DFA_scaling_factor = DFA_sf;
}

void ECGHRVDFA::setDFA_x(OtherSignal dfa_x)
{
	this -> dfa_x = dfa_x;
}

void ECGHRVDFA::setDFA_y(OtherSignal dfa_y)
{
	this -> dfa_y = dfa_y;
}

void ECGHRVDFA::setRR_integrated(OtherSignal rr_integrated)
{
	this -> rr_integrated = rr_integrated;
}
