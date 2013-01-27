#include "RPeaksDetector.h"
#include <QDebug>
#include "tri_logger.hpp"
#define LOG_END TRI_LOG_STR("END: " << __FUNCTION__);

RPeaksDetector::RPeaksDetector()
{
    this->detectionMethod = HILBERT;
	this->panTompkinsMovinghWindowLenght = 0;
	this->panTompkinsThershold = 0;
	this->customParameters = false;
}

RPeaksDetector::~RPeaksDetector()
{
	#ifdef USE_MOCKED_SIGNAL
		gsl_vector_free(mockedSignal->signal);
	#endif
}

void RPeaksDetector::runModule(const ECGSignalChannel &filteredSignal, const ECGInfo & ecgi, ECGRs &ecgRs)
{
	#ifdef DEBUG
		qDebug() << "R peaks module started";
	#endif
	try{
		this->filteredSignal = filteredSignal;
		this->signalFrequency = ecgi.channel_one.frequecy;
		#ifdef USE_MOCKED_SIGNAL
			qDebug() << "Use mocked signal for R-peaks module.";
			this->filteredSignal = getMockedSignal();
		#endif
		this->rsPositions = &ecgRs;
	
		bool success = this->detectRPeaks();
		if(!success)
		{
			#ifdef DEBUG
				qDebug() << "R peaks detection module fails";
			#endif
			throw new RPeaksDetectionException("R peaks detection method fails");
		}
	} catch (...) {
		#ifdef DEBUG
			qDebug() << "R peaks detection module fails";
		#endif
		throw new RPeaksDetectionException("Unknown exception during execution R peaks detection method");
	}
}

void RPeaksDetector::setParams(ParametersTypes &parameterTypes)
{
	TRI_LOG_STR(__FUNCTION__);
	if(parameterTypes.find("detection_method") != parameterTypes.end())
	{
		int method = (int)parameterTypes["detection_method"];
		if(method == 0)
		{
			this->detectionMethod = HILBERT;
		}
		else if(method == 1)
		{
            this->detectionMethod = PAN_TOMPKINS;
		}
		else
		{
			#ifdef DEBUG
				qDebug() << "Unknown detection method";
			#endif
			TRI_LOG_STR("Unknown detection method.");
			throw new RPeaksDetectionException("Unknown detection method.");
		}
	}
	else {
		#ifdef DEBUG
			qDebug() << "Parameter: detection method not found";
		#endif
		TRI_LOG_STR("Parameter: detection method not found");
		throw new RPeaksDetectionException("Parameter: detection method not found");
	}
	
	if(parameterTypes.find("window_size") != parameterTypes.end())
	{
		this->panTompkinsMovinghWindowLenght = (int)parameterTypes["window_size"];
	}
	else {
		#ifdef DEBUG
			qDebug() << "Window size not found, use automatic calculated value";
		#endif
	}

	if(parameterTypes.find("threshold_size") != parameterTypes.end())
	{
		panTompkinsThershold = parameterTypes["threshold_size"];
	}
	else {
		#ifdef DEBUG
			qDebug() << "Thersold size not found, use automatic calculated value";
		#endif
	}
	this->customParameters = true;

	#ifdef DEBUG
			cout << "Input parameters for R peaks module:" << endl;
			if(this->detectionMethod == PAN_TOMPKINS)
			{
				qDebug()  << "Detection method: PanTompkins" << endl
						  << "Moving window size: " << this->panTompkinsMovinghWindowLenght << endl
						  << "Thersold size: " << panTompkinsThershold;
			} 
			else if (this->detectionMethod == HILBERT)
			{
				qDebug() << "Detection method: Hilbert";
			}
			else
			{
				qDebug() << "Unknown detection method";
			}
	#endif
	LOG_END;
}

bool RPeaksDetector::detectRPeaks()
{
	TRI_LOG_STR(__FUNCTION__);
	#ifdef DEBUG
		if(this->customParameters)
		{
			qDebug() << "Running module with custom parameters";
		}
		else
		{
			qDebug() << "Running module with default parameters";
		}
	#endif

	if(detectionMethod == PAN_TOMPKINS)
	{
		return panTompkinsRPeaksDetection(&filteredSignal);
	}
	else if (detectionMethod == HILBERT)
	{
		return hilbertRPeaksDetection(&filteredSignal);
	}
	else
	LOG_END;
	return false;
}

bool RPeaksDetector::panTompkinsRPeaksDetection(ECGSignalChannel *signal)
{
	TRI_LOG_STR(__FUNCTION__);
	ECGSignalChannel sig;
	sig = *signal;
	int sigSize = 0;
	if(sig->signal->size < 1)
	{
		#ifdef DEBUG
			qDebug() << "Input signal size is 0";
		#endif
		TRI_LOG_STR("Input signal size is 0");
		return false;
	}
	else
	{
		sigSize = sig->signal->size;
		#ifdef DEBUG_SIGNAL
			qDebug() << "Input signal";	
			for(int i = 0; i < sigSize; i++)
			{
				double inputValue = gsl_vector_get (sig->signal, i);	
				qDebug() << inputValue;	
			}
		#endif
	}

	//Convolution [-0.125 -0.25 0.25 0.125] (Here we lose 4 signal samples)	
	#ifdef DEBUG
		qDebug() << "Convolution [-0.125 -0.25 0.25 0.125]" << endl << "Orginal signal size: " << sigSize;
	#endif
	int newSigSize = 0;
	ECGSignalChannel diffSig;
	diffSig = ECGSignalChannel(new WrappedVector);
	diffSig->signal = gsl_vector_alloc(sigSize);
	double filter[] = {-0.125, -0.25, 0.25, 0.125};
	int filterSize = 4;
	for(int i = 0; i < sigSize - filterSize; i++)
	{
		double tmpSum = 0;
		for(int j = 0; j < filterSize; j++)
		{
			double inputValue = gsl_vector_get (sig->signal, i + j);			
			tmpSum += inputValue * filter[j];
			#ifdef DEBUG_SIGNAL_DETAILS
				qDebug() << "Signal: " << inputValue << " Filter: " << filter[j] << " Sum: " << tmpSum;
			#endif
		}
		#ifdef DEBUG_SIGNAL
			qDebug() << "Final val: " << tmpSum << " at index: " << i;
		#endif
		gsl_vector_set(diffSig->signal, i, tmpSum);
		newSigSize++;
	}
	
	//Exponentiation
	sigSize = newSigSize;
	#ifdef DEBUG
		qDebug() << "Exponentiation ^2" << endl << "Signal size after convolution: " << sigSize;
	#endif
	ECGSignalChannel powSig;
	powSig = ECGSignalChannel(new WrappedVector);
	powSig->signal = gsl_vector_alloc(sigSize);
	for(int i = 0; i < sigSize; i++)
	{
		double inputValue = gsl_vector_get (diffSig->signal, i);			
		double powVal = pow(inputValue, 2);
		gsl_vector_set(powSig->signal, i, powVal);
		#ifdef DEBUG_SIGNAL
				qDebug() << " Pow: "<< powVal << " at index: " << i;
		#endif
	}

	//Calculae moving window lenght or use custom value
	// N=30 for f=200Hz - from literature
	// N=24 for f=360Hz - from literature and tests
	// Linear function for calculating window lenght
	// wl = -0.0375 * fs + 37.5
	if(panTompkinsMovinghWindowLenght == 0)
	{
		panTompkinsMovinghWindowLenght = -0.0375 * signalFrequency + 37.5;
	}

	//Moving window integration (Here we lose "movinghWindowLenght" signal samples)
	#ifdef DEBUG
		qDebug() << "Moving window integration" << endl << "Window size: " << panTompkinsMovinghWindowLenght << endl
				  << "Signal size after exponentiation: " << sigSize;
	#endif
	ECGSignalChannel integrSig;
	integrSig = ECGSignalChannel(new WrappedVector);
	integrSig->signal = gsl_vector_alloc(sigSize);
	newSigSize = 0;
	int movinghWindowLenght = panTompkinsMovinghWindowLenght;
	double tmpSum = 0;

	for(int i = movinghWindowLenght; i < sigSize; i++)
	{
		for(int j = movinghWindowLenght - 1; j >= 0 ; j--)
		{
			double inputValue = gsl_vector_get (powSig->signal, i - j);			
			tmpSum += inputValue;
			#ifdef DEBUG_SIGNAL_DETAILS
				qDebug() << "Signal: " << inputValue << " Sum: " << tmpSum;
			#endif
		}
		int index = i - movinghWindowLenght;
		// TODO Why this is not working? (To small values and all are save as zero)
		//double mwico = (1/movinghWindowLenght) * tmpSum;
		double mwico =  tmpSum;
		#ifdef DEBUG_SIGNAL
			qDebug() << "Final val: " << mwico << " at index: " << index;
		#endif
		gsl_vector_set(integrSig->signal, index, mwico);
		tmpSum = 0;
		newSigSize++;
	}

	//Calculating detection threshold
	//TODO (Not important now) Try to find another way to calcutale threshold position, maybe dynamic threshold?
	sigSize = newSigSize;
	#ifdef DEBUG
	qDebug() << "Calculating detection threshold" << endl << "After moving window integration signal size: " << sigSize;
	#endif
	double sigMaxVal = 0;
	double meanVal = 0;

	for(int i = 0; i < sigSize; i++)
	{
		double inputValue = gsl_vector_get (integrSig->signal, i);					
		if(inputValue > sigMaxVal)
		{
			sigMaxVal = inputValue;
			#ifdef DEBUG_SIGNAL
				qDebug() << "New max signal value: " << inputValue;
			#endif
		}
		meanVal += inputValue;
	}
	meanVal = meanVal / sigSize;

	#ifdef DEBUG
		qDebug() << "Final max value for channel one: " << sigMaxVal << endl
	         << "Final mean value: " << meanVal << endl;

	#endif

	// Select automatic or manual thersold
	double threshold = 0;
	if( this->panTompkinsThershold == 0)
	{
		threshold = meanVal + (sigMaxVal * 0.5);
	}
	else
	{
		threshold = this->panTompkinsThershold;
	}

	//Looking for points over thersold
	#ifdef DEBUG
		qDebug() << "Current thresold value: " << threshold << endl << "Looking for points over thersold";
	#endif
	ECGSignalChannel overThersold;
	overThersold = ECGSignalChannel(new WrappedVector);
	overThersold->signal = gsl_vector_alloc(sigSize);
	for(int i = 0; i < sigSize; i++)
	{
		double inputValue = gsl_vector_get (integrSig->signal, i);			
		if(inputValue > threshold * sigMaxVal)
		{
			gsl_vector_set(overThersold->signal, i, 1);
			#ifdef DEBUG_SIGNAL
				qDebug() << "Value over thersold at index: " << i;
			#endif
		}
		else
		{
			gsl_vector_set(overThersold->signal, i, 0);
		}
	}
	#ifdef DEBUG_SIGNAL
		qDebug() << "Signal with points over thersold";
		for(int i = 0; i < sigSize; i++)
		{
			qDebug() << gsl_vector_get(overThersold->signal, i);
		}
	#endif
	#ifdef DEBUG
		qDebug() << "Detect begin and end of QRS complex";
	#endif
	ECGSignalChannel leftPoints;
	ECGSignalChannel tmpRightPoints;
	leftPoints = ECGSignalChannel(new WrappedVector);
	tmpRightPoints = ECGSignalChannel(new WrappedVector);
	leftPoints->signal = gsl_vector_alloc(sigSize);
	tmpRightPoints->signal = gsl_vector_alloc(sigSize);
	int leftPointsCount = 0;
	int rightPointsCount = 0;

	gsl_vector* copiedSig = gsl_vector_calloc(sigSize);
	gsl_vector_memcpy(copiedSig, overThersold->signal);

	// Boundary values
	if(gsl_vector_get (copiedSig, 0) == 1)
	{
		gsl_vector_set(leftPoints->signal, leftPointsCount, 0);
		leftPointsCount++;
		#ifdef DEBUG_SIGNAL
			qDebug() << "QRS complex left point at index: " << 0;
		#endif
	}
	
	if(gsl_vector_get (copiedSig, sigSize - 1) == 1)
	{
		gsl_vector_set(tmpRightPoints->signal, rightPointsCount, sigSize - 1);
		rightPointsCount++;
		#ifdef DEBUG_SIGNAL
			qDebug() << "QRS complex right point at index: " << sigSize - 1;
		#endif
	}

	// Left points of QRS complex
	for(int i = 0; i < sigSize - 1; i++)
	{
		double inputValue = gsl_vector_get (copiedSig, i);
		double inputValueIndexPlus = gsl_vector_get (copiedSig, i + 1);
		if((inputValueIndexPlus - inputValue) == 1)
		{
			gsl_vector_set(leftPoints->signal, leftPointsCount, i);
			leftPointsCount++;
			#ifdef DEBUG_SIGNAL
				qDebug() << "QRS complex left point at index: " << i;
			#endif
		}
	}

	// Rights points of QRS complex
	for(int i = sigSize - 1; i > 0; i--)
	{
		double reversedInput = gsl_vector_get(copiedSig, i);
		double reversedInputIndexMinus = gsl_vector_get (copiedSig, i - 1);
		if((reversedInputIndexMinus - reversedInput) == 1)
		{
			gsl_vector_set(tmpRightPoints->signal, rightPointsCount, i);
			rightPointsCount++;
			#ifdef DEBUG_SIGNAL
				qDebug() << "QRS complex right at index: " << i;
			#endif
		}
	}

	#ifdef DEBUG_SIGNAL
		cout << "Vector with left points:" << endl;
		for(int i = 0; i < leftPointsCount; i++)
		{
			qDebug() << gsl_vector_get(leftPoints->signal, i);
		}
		qDebug() << endl << "Vector with right points:";
		for(int i = 0; i < rightPointsCount; i++)
		{
			qDebug() << gsl_vector_get(tmpRightPoints->signal, i);
		}
		cout << endl;
	#endif
	// Invert vector with rightPoints
	ECGSignalChannel rightPoints;
	rightPoints = ECGSignalChannel(new WrappedVector);
	rightPoints->signal = gsl_vector_alloc(sigSize);
	for(int i = 0; i < rightPointsCount; i++)
	{
		double tmp = gsl_vector_get(tmpRightPoints->signal, rightPointsCount - i - 1);
		gsl_vector_set(rightPoints->signal, i, tmp );
	}
	for(int i = 0; i < rightPointsCount; i++)
	{
		double tmp = gsl_vector_get(tmpRightPoints->signal, rightPointsCount - i - 1);
		gsl_vector_set(rightPoints->signal, i, tmp );
	}
	#ifdef DEBUG_SIGNAL
		qDebug() << "After vector invertion" << endl;
		qDebug() << "Vector with left points:" << endl;
		for(int i = 0; i < leftPointsCount; i++)
		{
			qDebug() << gsl_vector_get(leftPoints->signal, i);
		}
		qDebug() << endl << "Vector with right points:" << endl;
		for(int i = 0; i < rightPointsCount; i++)
		{
			qDebug() << gsl_vector_get(rightPoints->signal, i);
		}
	#endif
	#ifdef DEBUG
		qDebug() << "Number of left points: " << leftPointsCount << endl
			     << "Number of right points: " << rightPointsCount;
	#endif

	//Final R peaks detection
	#ifdef DEBUG
		qDebug() << "Final R peaks detection";
	#endif

	int partLength;
	IntSignal rs;
	int numberRs = 0;

	if(leftPointsCount > 0 )
	{
		rs = IntSignal(new WrappedVectorInt);
		rs->signal = gsl_vector_int_alloc(leftPointsCount);
		for(int i = 0; i < leftPointsCount; i++)
		{		
			partLength = gsl_vector_get (rightPoints->signal, i) - gsl_vector_get(leftPoints->signal, i);
			double tmpMax = 0;
			int tmpMaxIndex = 0;
			for(int j = 0; j < partLength; j++)
			{
				int sigIndex = gsl_vector_get (leftPoints->signal, i) + j;
				double sigVal = gsl_vector_get(sig->signal, sigIndex);
				if(sigVal > tmpMax)
				{
					tmpMax = sigVal;
					tmpMaxIndex = sigIndex;
				}

			}
			gsl_vector_int_set(rs->signal, i, tmpMaxIndex);
			numberRs++;
			#ifdef DEBUG_SIGNAL
				qDebug() << "R point at index: " << tmpMaxIndex 
					     << " signal value: " << gsl_vector_get(sig->signal, tmpMaxIndex);
			#endif
		}
		rsPositions->setRs(rs);
		#ifdef DEBUG
			qDebug() << "Number of detected R-peaks:" << numberRs;
		#endif
	}
	else
	{
		#ifdef DEBUG
			qDebug() << "R peaks not detected. Check input signal.";
		#endif
		TRI_LOG_STR("R peaks not detected. Check input signal.");
		return false;
	}
	gsl_vector_free(copiedSig);

	rsDetected = true;
	#ifdef DEBUG
		qDebug() << "Done";
	#endif
	LOG_END;
	return true;
}

bool RPeaksDetector::hilbertRPeaksDetection(ECGSignalChannel *signal)
{
	TRI_LOG_STR(__FUNCTION__);

	ECGSignalChannel sig = *signal;

    if(sig->signal->size < 1)
	{
		#ifdef DEBUG
				qDebug() << "Input signal size is 0";
		#endif
		TRI_LOG_STR("Input signal size is 0");
		return false;
	}
	int n = sig->signal->size;

	std::vector<double> sygnal(n);
	int i = 0;

	for ( ; i < n ; ++i)
		sygnal[i] = gsl_vector_get (sig->signal, i);

	signalAbs(sygnal);

	std::vector<int> pozycje;
	int czestotliwosc = 360;

	hilbertDetection(sygnal, czestotliwosc, pozycje);

	IntSignal rs;
	rs = IntSignal(new WrappedVectorInt);

	n = pozycje.size();

	rs->signal = gsl_vector_int_alloc(n);

	for (i = 0; i < n; ++i)
		gsl_vector_int_set(rs->signal, i, pozycje.at(i));

	rsPositions->setRs(rs);

	#ifdef DEBUG
		qDebug() << "Number of detected R-peaks:" << n;
	#endif

	rsDetected = true;
	LOG_END;
	return true;
}

ECGSignalChannel RPeaksDetector::getMockedSignal()
{
	TRI_LOG_STR(__FUNCTION__);

	int length = 600000; // MAX 650000
	mockedSignal = ECGSignalChannel(new WrappedVector);
	mockedSignal->signal = gsl_vector_alloc(length);

	string line = "";
	ifstream myfile;
	myfile.open("mocks/FilteredSignal.txt");
	if (myfile.is_open())
    {
		int i = 0;
		while (!myfile.eof())
		{
			getline(myfile, line);
			std::istringstream stm;
			stm.str(line);
			double d;
			stm >>d;
			gsl_vector_set(mockedSignal->signal, i, d);
			i++;
			if(i == length)
				break;
		}
		myfile.close();
    }
	else
	{
		#ifdef DEBUG
			cout << "Cannot read mocked signal." << endl;
		#endif
		TRI_LOG_STR("Cannot read mocked signal.");
	}
	LOG_END;
	return mockedSignal;
}  

namespace hilbertParameters {

	using namespace std;
	// rozmiar odcinka dla ktorego przeprowadzona jest
	// transformacja hilberta (w artykule 1024), bierz potege 2
	const unsigned int rozmiar_okna = 1024; // 1024;
	// promien sasiedztwa (w milisekundach) do przezukiwania w
	// okolicy punktu podejrzanego (w artykule 1/36)
	const double sasiedztwo = 2.0/36.0;
	// odleglosc czasowa miedzy punktami w ktorych wykryto R-peaki,
	// ktore uznajemy za nieprawdopodobne i powoduje ona, z
	// zdecyujemy sie na jeden tylko z tych punktow
	const double  za_bliskie = 1.0/5.0;
	// jezeli mamy dwa punkty R, ktore sa za bliskie, to mozemy
	// chciec wybrac ten, ktory lepiej pasuje do srednich odleglosci
	// "dotychczasowych", ale do tego trzeba jakiejs sensownej
	// liczby poprzednio wykrytych punktow; ponizsza liczba to
	// wlasnie taka liczba
	const int min_liczba_do_sr_odl = 10; // absolutnie powyzej 3, ale 
	// lepiej wiecej, moze 10
	// jezeli mamy dwa zbyt bliskie sobie punkty R, i chcemy wybrac
	// ten wlasciwy, to o ile to mozliwe) obliczamy srednia z
	// odleglosci pomedzy juz odkrtymi punktami R i badamy jaka
	// bylaby roznica miedzy odglegloscia pierwszego z tych spornych
	// punktpw do ostatniego "zatwierdzonego" a ta srednia. Oznaczmy
	// ta wielkosc R1. Analogicznie obliczymy R2. Teraz, jezeli R1
	// stanowi ustalony procent R2 (wlasnie procent ustalony
	// ponizej) to za wlasciwy przyjmuje punkt pierwszy. Jezeli
	// natomiast R2 stanowi ustalony procent R1, to za wlasciwy
	// przymujemy punkt drugi. W pozostalych przypadkach roztrzyga
	// to, w ktorym punkcie wartosc sygnalu goruje. No, chyba, ze
	// wartosc ta bedzie identyczna. Wtedy wracamy do porownywania
	// odleglosci (juz bez zabawy w procenty). Jezeli i tu bedzie
	// identycznie - przyjmujemy pierwszy. 	
	const double procent_mniejszej_odleglosci = 0.8;
	// jeeeli dla danego odcinka oblicze transformate hilberta, to
	// rozwazam stosunek RMS ciagu do MAX tego ciagu. Jezeli ten
	// stounek jest ponizej tego progu - ciag ten ma maly szumm. W
	// ARTYKULE byla to wielkosc 0.18
	const double prog_stosunku_rms_do_max = 0.18;
	// w niektorych sytuacjach prog do filtrowania wyznacza sie
	// w proporcji do MAX. ponizsza stala ustala te proporcje
	const double stosunek_prog_do_max = 0.39;
	// w niektorych sytuacjach prog do filtrowania wyznacza sie
	// w proporcji do RMS. ponizsza stala ustala te proporcje
	const double stosunek_prog_do_rms = 2.6;
	// gdy porownuje ze soba dwa zbyt bliskie piki (bliskie w sensie
	// czasowym) to najpierw sprawdze, czy sa mniej wiecej rowne). A
	// mniej wiecej rewne bedzie znaczylo, ze roznica miedzy nim
	// stanowi malutki procent ich srednije wartosci, ponizszy
	// parametr ten wlasnie procent definiuje
	const double zaniedbywalna_roznica = 0.02;
};

// **********************************************************************
//     do wektora "wykryte" dopisuje elementy listy "propozycje"
// **********************************************************************
void RPeaksDetector::addToRecognized( const std::list<int> &propozycje,
	std::vector<int> &wykryte) {    
		for (std::list<int>::const_iterator i = propozycje.begin(); i!= propozycje.end(); i++) {
			wykryte.push_back( *i );
		}
};

// *************************************************************
// funkcja do wektora "propozycje" wpisze numery
// tych elementow z wektora h, ktore sa wieksze niz "threshold"
// przy dopisywaniu powieksza dopisywany numer o liczbe "zalatwione"
// *************************************************************
void RPeaksDetector::filterPropositions(int czestotliwosc, const std::vector<double> &h, 
	double threshold, 
	std::list<int> & propozycje,
	int przesuniecie) {   
		propozycje.resize(0);
		const int za_bliskie = round( hilbertParameters::za_bliskie * czestotliwosc );
		for (int i=0+za_bliskie; i<h.size()-za_bliskie; i++)
			if (h[i] > threshold)
				propozycje.push_back(i+przesuniecie);
}

// czestotliwosc - liczba probek w sekundzie
// sygnal - sygnal dla ktorego wyszukujemy R-peakow
// wykryte - wektor numerow wykrytych maksimow, ktore pasuja do punktow
// wskazanych przez duze wartosci transformaty hilberta
// R - wektro do ktorego wpiszemy wszystkie z wykrytych, no chyba, ze
// beda ktores zbyt blisko siebie, wtey (z tych bliskich) wybierzemy
// tylko ten "wlasciwy"
void RPeaksDetector::removeTooClose(int czestotliwosc, const std::vector<double> & sygnal,
	const std::vector<int> &wykryte, 
	std::vector<int> & R) { 
		R.resize(0);
		if (wykryte.size() == 0)
			return;
		R.push_back(wykryte[0]);
		ASSERT(removeTooClose, wykryte[0] < sygnal.size() );

		const int za_bliskie = 
			round(hilbertParameters::za_bliskie * czestotliwosc);

		for (int i=1; i<wykryte.size(); i++) {
			ASSERT(removeTooClose, wykryte[i] < sygnal.size() );
			if ((sygnal.size()-wykryte[i]) <= za_bliskie)
				break;
			if (wykryte[i] - R.back() <= za_bliskie ) {
				// sa zbyt blisko;
				bool rozstrzygniete = false;
				if (almostEqual(sygnal[R.back()],sygnal[wykryte[i]])) {
					if (R.size() >= hilbertParameters::min_liczba_do_sr_odl) {
						double srednia = (R.back() - R.front())/
							double(R.size() - 1);
						int odl1 = R[R.size()-1]-R[R.size()-2];
						int odl2 = wykryte[i] - R[R.size()-2];

						double roz1 = fabs(odl1 - srednia);
						double roz2 = fabs(odl2 - srednia);
						if (roz1 < hilbertParameters::procent_mniejszej_odleglosci*roz2)  {
							// bierzemy 1;
							rozstrzygniete = true;
						}

						if (roz2 < hilbertParameters::procent_mniejszej_odleglosci*roz1)  {
							// bierzemy 2;
							rozstrzygniete = true;
							R.back() = wykryte[i];
						}
						if (!rozstrzygniete) {
							if (sygnal[R.back()] > sygnal[wykryte[i]]){
                            }
							if (sygnal[R.back()] < sygnal[wykryte[i]])
								// drugi
								R.back() = wykryte[i];
							if (sygnal[R.back()] == sygnal[wykryte[i]]) {
								// jednak wg
								// bliskosci
								if (roz2 < roz1)
									R.back() = wykryte[i];
							};
							rozstrzygniete = true;
						}
					};
				};
				if (!rozstrzygniete) {
					// sa za blisko, ale nie mamy jeszcze
					// srednich odleglosci
					if (sygnal[R.back()] < sygnal[wykryte[i]])
						R.back() = wykryte[i];
				}
			} else {
				R.push_back(wykryte[i]);
			}
		}
}	

// sygnal - to przefiltrowany, gotowy do dalszej obrobki sygnal
// y - zrozniczkowany sygnal
// czestootliwosc - ile probek odpowiada jednej sekundzie
// wykryte - wekto numerow probek oznaczajacych miejsca wykrycia probki
// *************************************************************
// funkcja przetwarza jedno okno zrozniczkowanego sygnalu
// *************************************************************
void RPeaksDetector::balanceWindow(const std::vector<double> &sygnal,
	int start, int rozmiar, 
	const std::vector<double> &y,
	int czestotliwosc,	
	std::vector<int> &wykryte) {

		static double poprzedni_max_abs = -1;
		// wartosc max|y| dla poprzedniego segmentu, przechowywana z
		// wywolania na wywolanie 

		std::vector<double> odcinek( y.begin()+start,
			y.begin()+start+rozmiar );
		std::vector<double> h;
		hilbert(odcinek, h);

		double rms = rmsCalculate(h);
		double max_abs = maxFromAbs(h);
		double threshold;

		if (rms >= hilbertParameters::prog_stosunku_rms_do_max*max_abs)
			threshold = hilbertParameters::stosunek_prog_do_max*max_abs;
		else
			threshold = hilbertParameters::stosunek_prog_do_rms*rms;
		if (poprzedni_max_abs != -1 ) // by?o ju? poprzednie okno
			if (max_abs > 2*poprzedni_max_abs)
				threshold = std::max(hilbertParameters::stosunek_prog_do_max*poprzedni_max_abs, threshold);
		poprzedni_max_abs = max_abs;

		std::list<int> propozycje;
		filterPropositions(czestotliwosc,h, threshold, propozycje, start);
		realMax(czestotliwosc, sygnal, propozycje);
		addToRecognized(propozycje, wykryte);
};

// sygnal - to przefiltrowany, gotowy do dalszej obrobki sygnal
// czestootliwosc - ile probek odpowiada jednej sekundzie
// wykryte - wekto numerow probek oznaczajacych miejsca wykrycia probki
// *************************************************************
// funkcja organizuje caly algorytm detekcji R
// *************************************************************
void RPeaksDetector::hilbertDetection(const std::vector<double> & sygnal,
	int czestotliwosc,
	std::vector<int> & numery_R) { 
		const int za_bliskie = round(hilbertParameters::za_bliskie * czestotliwosc);
		std::vector<int> wykryte;
        int zalatwione = 0;
        std::vector<double> y;
		differentiates(sygnal, czestotliwosc, y);	
		while (zalatwione < sygnal.size() ) {
			zalatwione = std::max(0,zalatwione-za_bliskie);
            const unsigned int size = sygnal.size() - zalatwione;
            int rozmiar = std::min(hilbertParameters::rozmiar_okna, size);
			int stara_liczba_wykrytych = wykryte.size();
            balanceWindow(sygnal, zalatwione, rozmiar, y,
				czestotliwosc, wykryte);
			bool znalezione = wykryte.size() > stara_liczba_wykrytych;
			// czy zosta? wykryty za?amek R
			// w ostatniej pr?bce?
			if ((rozmiar < hilbertParameters::rozmiar_okna) || !znalezione) 
				zalatwione += rozmiar;
			else 
				zalatwione = wykryte.back();
		}
		//sort(wykryte.begin(), wykryte.end());
		removeTooClose(czestotliwosc, sygnal, wykryte, numery_R);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RPeaksDetector::double2kiss(const std::vector<double> &f, 
	kiss_fft_cpx * g) {
		for (int i=0; i<f.size(); i++) {
			g[i].r = f[i];
			g[i].i = 0.0;
		};
}

void RPeaksDetector::kiss2double(int n, kiss_fft_cpx * f, std::vector<double> &g) {
	g.resize(n);
	for (int i=0; i<n; i++) {
		g[i] = f[i].r;
	};
}

void RPeaksDetector::fft(const std::vector<double> & f, kiss_fft_cpx * g) {
	// z rzeczywistego wektora f utworzy jego transformate g
	kiss_fft_cpx * dane_dla_kiss = new kiss_fft_cpx[f.size()];
	double2kiss(f, dane_dla_kiss);
	kiss_fft_cfg cfg = kiss_fft_alloc(f.size(), false, NULL, 0);
	kiss_fft( cfg, dane_dla_kiss, g);
	kiss_fft_free(cfg);
	kiss_fft_cleanup();
	delete dane_dla_kiss;
};

void RPeaksDetector::ifft(int n, kiss_fft_cpx * f, std::vector<double> & g) {
	// z zespolonego wektora f utworzy jego transofrmate odwrotna, i
	// jej czesc rzeczywista wpisze do g
	kiss_fft_cpx * wyniki_kiss = new kiss_fft_cpx[n];
	kiss_fft_cfg cfg = kiss_fft_alloc(n, true, NULL, 0);
	kiss_fft( cfg, f, wyniki_kiss);
	kiss_fft_free(cfg);
	kiss_fft_cleanup();
	kiss2double(n,wyniki_kiss,g);
	delete wyniki_kiss;
};

void RPeaksDetector::multiplyBy1ppixTransformation( int n, kiss_fft_cpx * f ) {
	// pierwsza polowka przez j, druga polowka przez -j, srodek
	// przez 0
	f[ 0 ].r = 0; f[ 0 ].i = 0;
	f[ n/2 ].r = 0; f[ n/2 ].i = 0;
	for (int i=1; i< n/2; i++) {
		kiss_fft_cpx nowy;
		nowy.r = f[i].i;
		nowy.i = -f[i].r;
		f[i] = nowy;
	}
	for (int i=n/2+1; i< n; i++) {
		kiss_fft_cpx nowy;
		nowy.r = -f[i].i;
		nowy.i = f[i].r;
		f[i] = nowy;
	}
};

void RPeaksDetector::hilbert(const std::vector<double> & f, std::vector<double> & g) {
	const int n = f.size();
	kiss_fft_cpx * fft_f = new kiss_fft_cpx[n];
	fft ( f, fft_f );
	multiplyBy1ppixTransformation(n,  fft_f );
	ifft ( n, fft_f, g);
	delete fft_f;
	for (int i=0; i<n;i++)
		g[i] /= n; 
}

//***************************************************************
// ** stwierdza czy liczby sa "prawie rowne" (przy porownywaniu
// ** podejrzanie bliskich szczytow
//***************************************************************
bool RPeaksDetector::almostEqual(double x, double y) {
	double s = (x+y)/2;
	double blad = fabs(x-y)/s;
	// [TODO] Check that blad is infinite number
	// bool isInfinite = std::numeric_limits<double>::infinity() == blad;
	// ASSERT(almostEqual, isInfinite);
	return blad<hilbertParameters::zaniedbywalna_roznica;
}

double RPeaksDetector::round( double liczba)
{
	if( liczba - floor(liczba) < 0.5 )
	{
		return floor(liczba);
	} else
	{
		return ceil(liczba);
	}
}

// wyszukuje nr najwiekszego sygnalu w otoczeniu miejsca "nr", probka o
// czestotliwosci "czestotliwosc" (do wyznaczania sasiedztwa)
int RPeaksDetector::findMax( int nr, const std::vector<double> &sygnal, int czestotliwosc) { 
	ASSERT(findMax, (nr<sygnal.size()) );
	ASSERT(findMax, (nr >=0 ));
	int sasiedztwo = round(czestotliwosc * hilbertParameters::sasiedztwo);
	int start = std::max(0, nr-sasiedztwo);
	int stop = std::min((int)sygnal.size()-1, nr+sasiedztwo);
	int max_nr = start;
	for (int i=start+1; i<= stop; i++)
		if (sygnal[i] > sygnal[max_nr])
			max_nr = i;
	return max_nr;
};

// czestotliwosc 	- czestotliwosc probki
// sygnal		- badany sygnal
// propozycje		- lisa punktow podejrzanych o bycie R punktami
// ********************************************************************
//    funkcja funkcja dla kazdego z punktow sprawdza maksimum w jego
//    okolicy i zatepuje go przez prawdziwe maksimum; ewentualne punkty
//    z tego samego sasiedztwa na tej liscie -- usuwa
// ********************************************************************
void RPeaksDetector::realMax(int czestotliwosc, const std::vector<double> &sygnal, 
	std::list<int> &propozycje) {  
		for (std::list<int>::iterator p=propozycje.begin();
			p != propozycje.end(); p++) {
				int nr = findMax( *p, sygnal , czestotliwosc);
				*p = nr;
		}
		propozycje.unique();	
}

double RPeaksDetector::rmsCalculate(const std::vector< double >& w)
{ 
	ASSERT( rmsCalculate, w.size() > 0);
	double s=0;
	for (int i=0; i<w.size(); i++)
		s += w[i]*w[i];
	return sqrt(s/w.size());
}       

double RPeaksDetector::maxFromAbs(const std::vector< double >& w)
{ 
	double mx=w[1];
	for (int i=1; i<w.size(); i++) {
		double wart = fabs(w[i]);
		if (wart > mx)
			mx = wart;
	}
	return mx;
}       

void RPeaksDetector::differentiates(const std::vector<double> &sygnal, int czestotliwosc, 
	std::vector<double> &y) { 
		y.resize( sygnal.size() );
		double s = (sygnal[0] + sygnal[ sygnal.size() - 1 ])/2;
		double dt = 2.0/czestotliwosc;

		y[0] = (sygnal[1]-s)/dt;
		y[ y.size()-1 ] = (s- sygnal[ y.size() - 2]) /dt;

		for (int i=1; i< y.size() - 1; i++)
			y[i] = (sygnal[i+1] - sygnal[i-1]) / dt;
};

double RPeaksDetector::average(const std::vector<double> & w) {
	double suma = 0.0;
	for (int i=0; i< w.size(); i++)
		suma += w[i];

	return suma / w.size();
}

void RPeaksDetector::signalAbs(std::vector<double> &s) {
	double srednia = average(s);
	for (int i=0; i<s.size(); i++)
		s[i] = fabs(s[i] - srednia);
}

