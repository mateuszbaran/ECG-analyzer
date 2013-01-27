#include "HRTAnalyzer.h"
#include <QDebug>

HRTAnalyzer::HRTAnalyzer() { }

HRTAnalyzer::~HRTAnalyzer() { }

void HRTAnalyzer::runModule(const ECGRs & ecgrs, const QRSClass & qrsclass, const ECGInfo & ecginfo, ECGHRT & hrt_data) {
	#ifdef USE_MOCKED_INTERVALS_SIGNAL
		int N = 380; 
		QRSClass qrsclassification;
		#ifdef USE_MOCKED_QRS_CLASSIFICATION
			IntSignal tmpSig;
			tmpSig = IntSignal(new WrappedVectorInt);
		    tmpSig->signal = gsl_vector_int_alloc(N+1);
			for(int i=0; i<N+1; i++)	{
				tmpSig->set(i, VENTRICULUS);
			}
			qrsclassification.setQrsMorphology(tmpSig);
		#else
			qrsclassification = qrsclass;
		#endif
		calculateHrtParams(RRtest,N,qrsclassification,hrt_data);
	#else
		int N = ecgrs.count()-1;
		int f = ecginfo.channel_one.frequecy;
		RRs = new double[N];
		for(int i = 0; i<N; i++)	{
			RRs[i] = (ecgrs.GetRs()->get(i+1) - ecgrs.GetRs()->get(i))*1000/f;
			int a = RRs[i];
		}
		QRSClass qrsclassification;
		#ifdef USE_MOCKED_QRS_CLASSIFICATION
			IntSignal tmpSig;
			tmpSig = IntSignal(new WrappedVectorInt);
		    tmpSig->signal = gsl_vector_int_alloc(N+1);
			for(int i=0; i<N+1; i++)	{
				tmpSig->set(i, VENTRICULUS);
			}
			qrsclassification.setQrsMorphology(tmpSig);
		#else
			qrsclassification = qrsclass;
		#endif
		calculateHrtParams(RRs, qrsclassification, N,hrt_data);

		delete [] RRs;
	#endif

	hrt_data.offset=5;
	for(int i=0; i<26; i++)	{
		hrt_data.rr.push_back(QPointF(double(i), hrt_data.avgSignal[i]));
	}
	hrt_data.ts.setLine(7.0,hrt_data.straightSignal[7],25.0,hrt_data.straightSignal[25]);
}

void HRTAnalyzer::setParams(ParametersTypes &parameterTypes) { }

// G³ówna funkcja, zwraca obiekt z danymi do wizualizacji
void HRTAnalyzer::calculateHrtParams(double *signal,  const QRSClass & qrsclass, int size, ECGHRT & hrt_data)
{
	vector<int> vpc_list = findVpcOnsets(signal, qrsclass, size);
	hrt_data.setAllSignalsSize(vpc_list.size());
	hrt_data.vpcCounter = vpc_list.size();

	if(vpc_list.size() < 5)
	{
		hrt_data.isCorrect = 0;
		#ifdef DEBUG
			qDebug() << "Niestety, w zarejestrowanym sygnale nie uda³o siê znaleŸæ niezbêdnych 5 przedwczesnych pobudzeñ komorowych";
		#endif
	} 
	else	{
		hrt_data.isCorrect = 1;
		double* avgTach = calculateAvgTach(signal, vpc_list);

		double to = calculateTO(signal, size, vpc_list);
		calculateTS(signal, size, vpc_list, avgTach, to, hrt_data);
	}
}


vector<int> HRTAnalyzer::findVpcOnsets(double *signal,  const QRSClass & qrsclass, int size)
{
	vector<int> vpc_list;
	for(int i = 6; i < size-19; i++)
	{
		//Sprawdzenie czy modu³ QRS uzna³ QRS zwi¹zany z danym interwa³em RR za komorowy
		if(qrsclass.GetQRS_morphology()->get(i+1) == VENTRICULUS)	{
				// Interwa³ referencyjny
				double mean5before = (signal[i-5] + signal[i-4] + signal[i-3] + signal[i-2] + signal[i-1])/5;
		 
				// Sprawdzenie czy interwa³ mo¿e byæ kandydatem na VPC
				if((0.8 * mean5before < signal[i]) || (signal[i+1] < 1.1 * mean5before) || abs(signal[i-1] - signal[i-2]) > 200)
				{
				continue;
				}
			
				// Sprawdzenie czy d³ugoœæ jest >300 ms i <2000 ms
				int param = 0; 
				for(int j = i - 5; j <= i + 19; j++)
				{
				if(j == i - 1 || j == i || j == i+1)
				{
					continue;
				}

				if(signal[j] > 2000 || signal[j] < 300)
				{
					param = 1;
					break;
				}
				}
    
				if(param == 1)
				{
					continue;
				}

				// Sprawdzenie czy 2 s¹siednie zwyk³e interwa³y maj¹ ró¿nicê mniejsz¹ ni¿ o 200 ms
				// oraz czy ka¿dy ze zwyk³ych interwa³ów ró¿ni siê o mniej ni¿ 20% od interwa³u referencyjnego.
				for(int j = i-5; j <= i + 19; j++)
				{
					if(j == i-1 || j == i || j == i+1) 
					{
						continue;
					}

					if(abs(signal[j] - signal[j+1]) > 200 || (abs(signal[j] - mean5before) > 0.2 * mean5before))
					{
						param = 1;
						break;
					}
				}
    
				if(param == 1)
				{
				continue;
				}
			 
				vpc_list.push_back(i-5);
		}
	}

	return vpc_list;
}

double* HRTAnalyzer::calculateAvgTach(double *signal, vector<int> vpc_list)	{
	double *avgTach= new double[26];

	for(int i = 0; i < 26; i++)
	{
		avgTach[i] = 0.0;
	}

	/* Obliczanie uœrednionego tachogramu  */
	
	for(int j = 0; j < vpc_list.size(); j++)
	{
		for(int i=0; i < 26; i++)
		{
			avgTach[i] = avgTach[i] + signal[vpc_list[j] + i];
		}
	}

	for(int i = 0; i < 26; i++)
	{
		avgTach[i] = avgTach[i]/vpc_list.size();
	}

	return avgTach;
}

// wyliczenie TO
// Wzi¹³em nadal tê metodê, w której najpierw liczy siê wszystkie TO, a nastêpnie ich œredni¹
double HRTAnalyzer::calculateTO(double * signal, int size, vector<int> vpc_list)
{
	double to = 0.0;
	double sumto = 0.0;

	for(int i = 0; i < vpc_list.size(); i++)
	{
		to = 100*((signal[vpc_list[i] + 7] + signal[vpc_list[i] + 8]) - (signal[vpc_list[i] + 3] + signal[vpc_list[i] + 4])) / (signal[vpc_list[i] + 3] + signal[vpc_list[i] + 4]);   
		sumto += to;
	}

	to = sumto/vpc_list.size();
	return to;
}

/*
double HRTAnalyzer::calculateTO_2(double * signal, int size, double* avgTach)
{
	double to = 0.0;
	to = 100*((avgTach[7] + avgTach[8]) - (avgTach[3] + avgTach[4])) / (avgTach[3] + avgTach[4]);   

	return to;
}
*/


void HRTAnalyzer::calculateTS(double * signal, int size, vector<int> vpc_list, double* avgTach, double to, ECGHRT & hrt_data)
{
    vector<double> A;
    vector<double> B;

    for (int i = 0; i < 15; i++)
	{
		double *x = new double[5];
		for(int k = 0; k < 5; k++)
			x[k] = k + i + 7;
			
		Parameters res = Matrix::getLinearEquationParameters(x, avgTach+i+7 , 5);
 
        A.push_back(res.A);
        B.push_back(res.B);
	}
        
		
    // Wybór najwiêkszego A
	double maxA = A[0];
	for(int i = 1; i < A.size(); i++)
	{
		if(maxA < A[i]) maxA = A[i];
	}

    double parB = B[0];
	for(int i = 0; i < A.size(); i++)
	{
		if(A[i] == maxA)
		{
				
			parB = B[i];
		}
	}

	hrt_data.TO = to;

	//====================USTAWIANIE OBIEKTU Z DANYMI DO WYŒWIETLENIA=======================
	// Ustawienie œredniego sygna³u tachogramu oraz
	// Ustawienie parametrów prostej ilustrujacej Turbulence Slope
	for(int i = 0; i < hrt_data.SLENGTH; i++)
	{
		hrt_data.avgSignal[i] = avgTach[i];
		hrt_data.straightSignal[i] = i*maxA + parB;
	}

	// Ustawienie wszystkich mo¿liwych sygna³ów
	for(int i = 0; i < hrt_data.ALL_SIGNALS_LENGTH; i++)
	{
		for(int j = 0; j < hrt_data.SLENGTH; j++)
		{
			double a = vpc_list[i];
			hrt_data.allSignals[i][j] = signal[vpc_list[i] + j];
		}
	}
	hrt_data.y1_to = hrt_data.avgSignal[7];

	hrt_data.length_to = hrt_data.avgSignal[7]*hrt_data.TO/100;
	hrt_data.TS=maxA;
	delete [] avgTach;
}
