#pragma once

#include "TWaveAltDetector.h"

TWaveAltDetector::TWaveAltDetector(void) {
    
}

TWaveAltDetector::~TWaveAltDetector(void) {

}
void TWaveAltDetector::setParams(ParametersTypes &parameterTypes){}


//(const ECGWaves &, const ECGSignalChannel &, ECGInfo &, ECGTWave &) = 0;
void TWaveAltDetector::runModule(const ECGWaves &ecgWaves, const ECGSignalChannel &ecgSignal, ECGInfo &ecgInfo, ECGTWave &ecgTWave) {	
	try{
		this->filteredSignal = ecgSignal;
                this->wavesData = ecgWaves;
                this->tWaveAltData = ecgTWave;
                
		#ifdef USE_MOCKED_SIGNAL
			this->filteredSignal = getMockedSignal();
		#endif
		
		detectTWaveAlt();
		

	}catch (...) {
		#ifdef DEBUG
			cout << "modules TWAveAlt failed" << endl;
		#endif
	}
}

bool TWaveAltDetector::detectTWaveAlt() { 

	#ifdef DEBUG
		cout << "Detecting TWaveAlt start" << endl;
	#endif
    
    int signalSize = wavesData.GetT_end()->signal->size;
	int sS = wavesData.GetT_end()->signal->size;
		
    int ilosc = 0;
    int A1, A2, A3, A4;
    double ECG_A1, ECG_A2, ECG_A3, ECG_A4;
    int k0 = 0, k1 = 0, k2 = 0, k3 = 0;
    
    bool *tmp_detect_tab = new bool[signalSize];

	#ifdef DEBUG
		cout << "Preparing singals to be detected" << endl;
	#endif

    for (int i = 0; i + 3 < signalSize; i++) {
        ilosc++;

        A1 = gsl_vector_int_get(wavesData.GetT_end()->signal, i);
        A2 = gsl_vector_int_get(wavesData.GetT_end()->signal, i + 1);
        A3 = gsl_vector_int_get(wavesData.GetT_end()->signal, i + 2);
        A4 = gsl_vector_int_get(wavesData.GetT_end()->signal, i + 3);

		ECG_A1 = gsl_vector_get(filteredSignal->signal, A1);
        ECG_A2 = gsl_vector_get(filteredSignal->signal, A2);
        ECG_A3 = gsl_vector_get(filteredSignal->signal, A3);
        ECG_A4 = gsl_vector_get(filteredSignal->signal, A4);

        if (fabs(ECG_A1 - ECG_A3) < fabs(ECG_A1 - ECG_A2)) {
            k1++;
            if (fabs(ECG_A2 - ECG_A4) < fabs(ECG_A3 - ECG_A4)) {
                k2++;
                if (fabs(ECG_A2 - ECG_A4) < fabs(ECG_A1 - ECG_A2)) {
                    k3++;
                    if (fabs(ECG_A1 - ECG_A3) < fabs(ECG_A3 - ECG_A4)) {
                        k0++;
                        tmp_detect_tab[i] = true;
                        tmp_detect_tab[i+1] = true;
                        tmp_detect_tab[i+2] = true;
                        tmp_detect_tab[i+3] = true;
                        
						#ifdef DEBUG
							cout << "T_WaveAlt detected" << endl;
						#endif
                        // TWaveAlt detected in window
                    }
                }
            }
        }
    }


	// setting number of detected peaks/windows
    int num_of_det_pic = 0;
    for(int i = 0 ; i < signalSize ; i++) {
        if(tmp_detect_tab[i] == true) num_of_det_pic++;
    }
    
    IntSignal tmp_twa = IntSignal(new WrappedVectorInt);
    tmp_twa->signal = gsl_vector_int_alloc(num_of_det_pic);    
    
	// creating vector to be shown
    int tmp_i = 0;
    for(int i = 0 ; i < signalSize ; i++) {
        if(tmp_detect_tab[i] == true) {
			gsl_vector_int_set(tmp_twa->signal, tmp_i, i);
            tmp_i++;
        }
    }
    
/*
    int position = 0;
    for (int i = 0; i + 3 < signalSize; i++) {
        ilosc++;

        A1 = gsl_vector_int_get(ecg_waves.GetT_end()->signal, i);
        A2 = gsl_vector_int_get(ecg_waves.GetT_end()->signal, i + 1);
        A3 = gsl_vector_int_get(ecg_waves.GetT_end()->signal, i + 2);
        A4 = gsl_vector_int_get(ecg_waves.GetT_end()->signal, i + 3);

        ECG_A1 = gsl_vector_get(ecg_signal, A1);
        ECG_A2 = gsl_vector_get(ecg_signal, A2);
        ECG_A3 = gsl_vector_get(ecg_signal, A3);
        ECG_A4 = gsl_vector_get(ecg_signal, A4);

        if (fabs(ECG_A1 - ECG_A3) < fabs(ECG_A1 - ECG_A2)) {
            if (fabs(ECG_A2 - ECG_A4) < fabs(ECG_A3 - ECG_A4)) {
                if (fabs(ECG_A2 - ECG_A4) < fabs(ECG_A1 - ECG_A2)) {
                    if (fabs(ECG_A1 - ECG_A3) < fabs(ECG_A3 - ECG_A4)) {
                        gsl_vector_int_set(tmp_twa, position, i);
                    }
                }
            }
        }
    }
*/
	// analysis of TWaveAlt
    tWaveAltData.numberOfWinDetected = (double) (num_of_det_pic);
    tWaveAltData.percentageOfWinDetected = ((double) (num_of_det_pic)) / ((double) (ilosc));
    tWaveAltData.setTWaveAlt(tmp_twa);

	// checking if whole signal has alternans
    if (tWaveAltData.percentageOfWinDetected >= 0.05)
        return true;
    else
        return false;
}