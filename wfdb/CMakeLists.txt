project(wfdb)
cmake_minimum_required(VERSION 2.6)

set(wfdb_SOURCES ${wfdb_SOURCES}
annot.c
calib.c
signal.c
wfdbinit.c
wfdbio.c
  )

set(wfdb_HEADERS ${wfdb_HEADERS}
ecgcodes.h
ecgmap.h
wfdb.h
wfdblib.h
  )


add_library(wfdb ${wfdb_SOURCES})
