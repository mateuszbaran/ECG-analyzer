#include "DFAAnalyzer.h"
#include "ECGSignal.h"
#include<QDebug>
#include "tri_logger.hpp"
//#define MYDEBUG
#define LOG_END TRI_LOG_STR("END: " << __FUNCTION__);

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
	TRI_LOG_STR(__FUNCTION__);
	IntSignal rs = rsPositions.GetRs();
	int sigSize = rs->signal->size - 1;
	#ifdef MYDEBUG
		qDebug() << "Signal size: " << sigSize << endl;
	#endif

	double *rr_signal = new double[sigSize-1];
	double rr_sum = 0; //sum of all RR intervals
	for(int i = 0; i < sigSize; i++)
	{
		rr_signal[i] = (gsl_vector_int_get(rs->signal, i+1) - gsl_vector_int_get(rs->signal, i))/360.0;
		#ifdef MYDEBUG
			qDebug() << "rr_signal[" << i << "]" << rr_signal[i] << endl;
		#endif
		rr_sum += rr_signal[i];
	}
	double rr_avg = rr_sum/sigSize; //RR interval mean
	#ifdef MYDEBUG
		qDebug() << "RR interval mean: " << rr_avg << endl;
	#endif

			
	double* rr_integrated = new double[sigSize];
	OtherSignal rr_integrated_sig =  OtherSignal(new WrappedVector);
	rr_integrated_sig->signal = gsl_vector_alloc(sigSize);

	rr_integrated[0] = rr_signal[0] - rr_avg;
	gsl_vector_set(rr_integrated_sig->signal, 0, rr_integrated[0]);

	for(int i = 1; i<sigSize; i++)
	{
		rr_integrated[i] = rr_integrated[i-1] + rr_signal[i] - rr_avg; //integration
		gsl_vector_set(rr_integrated_sig->signal, i, rr_integrated[i]);
		#ifdef MYDEBUG
			qDebug() << "rr_integrated[" << i << "]" << rr_integrated[i] << endl;
		#endif
	}

	ecghrvdfa.setDFA_y(rr_integrated_sig);

	trend_coefs** trends = calc_all_trends(rr_integrated, sigSize);
	double* fluctuations = new double[sigSize - 1];

	OtherSignal dfa_y, dfa_x;

	dfa_y = OtherSignal(new WrappedVector);
	dfa_y->signal = gsl_vector_alloc(sigSize - 1);

	dfa_x = OtherSignal(new WrappedVector);
	dfa_x->signal = gsl_vector_alloc(sigSize - 1);

	for(int i = 0; i<sigSize - 2; i++)
	{
           for(int j = 0; j<sigSize; j+=i)
	       {
                double temp2 = 0;
                for(int k = 0; k < i + 2 && j+k<sigSize; j++)
	            {
                    double temp1 = rr_integrated[j + k] - (trends[i][0].a * (j + k) + trends[i][0].b);
                    temp2 += (temp1*temp1)/(sigSize);
                }
                fluctuations[i] = sqrt(temp2);
				gsl_vector_set(dfa_y->signal, i, fluctuations[i]);
				gsl_vector_set(dfa_x->signal, i, i+2);
				#ifdef MYDEBUG
					qDebug() << "fluctuations[" << i << "]" << fluctuations[i] << endl;
				#endif
           }
	}
	#ifdef MYDEBUG
		qDebug() << "fluctuations[sigSize - 3]: " << fluctuations[sigSize - 3]  << endl;
		qDebug() << "fluctuations[0]: " << fluctuations[0]  << endl;
		qDebug() << "log((double)(fluctuations[sigSize - 3] - fluctuations[0])): " << log((double)(fluctuations[sigSize - 3] - fluctuations[0]))  << endl;
		qDebug() << "log((double)(sigSize - 1)): " << log((double)(sigSize - 1))  << endl;
	#endif
	double alpha = log((double)(fluctuations[sigSize - 3] - fluctuations[0]))/log((double)(sigSize - 1));
	TRI_LOG(alpha);
	#ifdef DEBUG
		qDebug() << "Alpha: " << alpha  << endl;
	#endif

	ecghrvdfa.setDFA_scaling_factor(alpha);
	ecghrvdfa.setDFA_x(dfa_x);
	ecghrvdfa.setDFA_y(dfa_y);

    delete fluctuations;
	delete rr_integrated;
	for(int i = 0; i < sigSize - 1; i++)
	{
		delete trends[i];
	}
	delete trends;

	LOG_END;
}

trend_coefs* DFAAnalyzer::calc_trends_for_window_length(double* rr_integrated, int length, int window_length)
{ 
	//TRI_LOG_STR(__FUNCTION__);
    double a = 0.0;
    double b = 0.0;
    trend_coefs* trends = new trend_coefs[(int)ceil(length/(float)(window_length))];


		for(int j = 0, k = 0; j < length; j += window_length, k++)
		{
            int w_length;
			if(j + window_length < length)
			{
				w_length = window_length;
			}
			else
			{
				w_length = length - window_length;
			}
            trend_coefs* t = calc_trend(rr_integrated, j, w_length);
		    trends[k].a = t->a;
		    trends[k].b = t->b;
		    
		}
	//LOG_END;
    return trends;
}

trend_coefs** DFAAnalyzer::calc_all_trends(double* rr_integrated, int length)
{ 
	//TRI_LOG_STR(__FUNCTION__);
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
	//LOG_END;
    return trends;
}

trend_coefs* DFAAnalyzer::calc_trend(double* rr_integrated, int start, int w_length)
{
	//TRI_LOG_STR(__FUNCTION__);
    trend_coefs* t = new trend_coefs;

    double mean_x = (w_length + 1)/1;
    double mean_y = 0.0;
    int x;
    for(x = start; x < start + w_length; x++)
    {
        mean_y += rr_integrated[x] / w_length;
    }
    double ss_xx = 0.0, ss_xy = 0.0;
    for(x = start; x < start + w_length; x++)
    {
        ss_xx += x*x;
        ss_xy += x*rr_integrated[x];
    }
    ss_xx -= w_length*mean_x*mean_x;
    ss_xy -= w_length*mean_x*mean_y;
    t->b = ss_xy/ss_xx;
    t->a = mean_y - (mean_x*t->b);
	/*double S, Sx, Sy, Sxx, Sxy;
	int x;
	S = w_length;
	Sx = 0.0;
	Sy = 0.0;
	Sxx = 0.0;
	Sxy = 0.0;

    for(x = start; x < start + w_length; x++)
    {
        Sx += x;
        Sy += rr_integrated[x];
		Sxx += x*x;
		Sxy += x*rr_integrated[x];
    }
	t->a = (S*Sxy - Sx*Sy)/(S*Sxx-Sx*Sx);
	t->b = (Sxx*Sy - Sx*Sxy)/(S*Sxx-Sx*Sx);*/
	#ifdef MYDEBUG
		qDebug() << "start: " << start  << endl;
		qDebug() << "w_length: " << w_length  << endl;
		qDebug() << "t->a: " << t->a  << endl;
		qDebug() << "t->b: " << t->b  << endl;
	#endif
	//LOG_END;
    return t;
}

