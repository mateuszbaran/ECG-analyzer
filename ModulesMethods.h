#pragma once

//This file is for enums for selecting module method.

/**
 * ECG_BASALINE methods.
 */
enum ECG_BASELINE_METHOD
{
  //example method
  BUTTERWORTH
};

/**
 * R_PEAKS methods.
 */
enum R_PEAKS_DETECTION_METHOD
{
	PAN_TOMPKINS,
	HILBERT
};