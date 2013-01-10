#pragma once

//This file is for enums for selecting module method.

/**
 * ECG_BASALINE methods.
 */
enum BASELINE_REMOVAL_METHOD
{
  MOVING_AVERAGE, BUTTERWORTH_2D
};

/**
 * R_PEAKS methods.
 */
enum R_PEAKS_DETECTION_METHOD
{
	PAN_TOMPKINS,
	HILBERT
};