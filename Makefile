CXX=x86_64-w64-mingw32-g++
PROG=speech_voiceroid.exe
CPPF=speech_voiceroid.cpp
CFLAGS=-O2 -Wall -std=c++11

all: $(PROG)

$(PROG): $(CPPF)
	$(CXX) $(CPPF) $(CFLAGS) -o $(PROG)

PHONY: check-syntax
check-syntax:
	$(CXX) -fsyntax-only -fno-color-diagnostics $(CFLAGS) $(CHK_SOURCES)
