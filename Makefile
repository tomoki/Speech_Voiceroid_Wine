# C++ does not work well on Wine.
CXX=i686-w64-mingw32-gcc
PROG=speech_voiceroid.exe
CPPF=speech_voiceroid.c
CFLAGS=-O2 -Wall -std=c99

all: $(PROG)

$(PROG): $(CPPF)
	$(CXX) $(CPPF) $(CFLAGS) -o $(PROG)

PHONY: check-syntax
check-syntax:
	$(CXX) -fsyntax-only -fno-color-diagnostics $(CFLAGS) $(CHK_SOURCES)
