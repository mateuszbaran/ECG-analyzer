#include "DFAAnalyzer.h"

DFAAnalyzer::DFAAnalyzer()
{

}

DFAAnalyzer::~DFAAnalyzer()
{

}

void DFAAnalyzer::runModule (const ECGRs &ecgRs, ECGHRVDFA &ecghrvdfa)
{
		this->rsPositions = ecgRs;
		this->ecghrvdfa = ecghrvdfa;
		this->calcDFA();
	
}
void DFAAnalyzer::setParams(ParametersTypes &parameterTypes)
{

}
void DFAAnalyzer::calcDFA()
{
	IntSignal rs = rsPositions.GetRs();
	int sigSize = rs->signal->size-1;


	double *rr_signal = new double[sigSize-1];
	double rr_sum = 0; //sum of all RR intervals
	for(int i = 0; i < sigSize; i++)
	{
		rr_signal[i] = (gsl_vector_int_get(rs->signal, i+1) - gsl_vector_int_get(rs->signal, i))/360.0;
		rr_sum += rr_signal[i];
	}
	double rr_avg = rr_sum/sigSize; //RR interval mean

	double* rr_integrated = new double[sigSize];
	rr_integrated[0] = rr_signal[0] - rr_avg;
	for(int i = 1; i<sigSize; i++)
	{
		rr_integrated[i] = rr_integrated[i-1] + rr_signal[i] - rr_avg; //integration
	}

	trend_coefs** trends = calc_trends(rr_integrated, sigSize);
	double* fluctuations = new double[sigSize - 1];
	for(int i = 0; i<sigSize - 1; i++)
	{
        int index = 0;
           for(int j = 0; j<sigSize; j+=i)
	       {
                double temp2 = 0;
                for(int k = 0; k < i + 2 && j+k<sigSize; j++)
	            {
                    double temp1 = rr_integrated[j + k] - (trends[i][index].a * (j + k) + trends[i][index].b);
                    temp2 += (temp1*temp1)/(sigSize);
                }
                fluctuations[i] = sqrt(temp2);
           }
	}
	double alpha = log((double)(fluctuations[sigSize - 2] - fluctuations[0]))/log((double)(sigSize - 1));
    delete fluctuations;
	delete rr_integrated;
	for(int i = 0; i < sigSize - 1; i++)
	{
		delete trends[i];
	}
	delete trends;
	ecghrvdfa.setDFA_scaling_factor(alpha);
}

trend_coefs** DFAAnalyzer::calc_trends(double* rr_integrated, int length)
{ 
    double a = 0.0;
    double b = 0.0;
    trend_coefs** trends = new trend_coefs*[length - 1];
	for(int i = 0; i < length - 1; i++)
	{
		trends[i] = new trend_coefs[(int)ceil(length/(float)(i + 2))];
	}
	for(int i = 2; i < length + 1; i++)//window length
	{
        
		for(int j = 0, k = 0; j < length; j += i, k++)
		{
            int w_length;
			if(j + i < length)
			{
				w_length = i;
			}
			else
			{
				w_length = length - i;
			}
            trend_coefs* t = calc_trend(rr_integrated, j, w_length);
		    trends[i-2][k].a = t->a;
		    trends[i-2][k].b = t->b;
		    
		}
	}
    return trends;
}

trend_coefs* DFAAnalyzer::calc_trend(double* rr_integrated, int start, int w_length)
{
    trend_coefs* t = new trend_coefs;
    double mean_x = (w_length + 1)/1;
    double mean_y = 0.0;
    int i;
    for(i = start; i < start + w_length; i++)
    {
        mean_y += rr_integrated[i] / w_length;
    }
    double ss_xx = 0.0, ss_xy = 0.0;
    for(i = start; i < start + w_length; i++)
    {
        ss_xx += i*i;
        ss_xy += i*rr_integrated[i];
    }
    ss_xx -= w_length*mean_x*mean_x;
    ss_xy -= w_length*mean_x*mean_y;
    t->b = ss_xy/ss_xx;
    t->a = mean_y - (mean_x*t->b);
    return t;
}

