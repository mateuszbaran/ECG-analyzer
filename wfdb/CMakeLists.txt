project(ecg-analyzer)
cmake_minimum_required(VERSION 2.6)

set(ecg-analyzer_SOURCES ${ecg-analyzer_SOURCES}
annot.c
calib.c
signal.c
wfdbinit.c
wfdbio.c
  )

set(ecg-analyzer_HEADERS ${ecg-analyzer_HEADERS}
ecgcodes.h
ecgmap.h
wfdb.h
wfdblib.h
  )
