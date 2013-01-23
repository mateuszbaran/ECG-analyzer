project(kissfft)
cmake_minimum_required(VERSION 2.6)

set(kissfft_SOURCES ${kissfft_SOURCES}
kiss_fft.c
kiss_fftr.c
  )

set(kissfft_HEADERS ${kissfft_HEADERS}
_kiss_fft_guts.h
kiss_fft.h
kiss_fftr.h
kissfft.hh
  )

add_library(kissfft ${kissfft_SOURCES})
