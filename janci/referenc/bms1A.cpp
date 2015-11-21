/*
 * File:   bms1A.cpp
 */

#include <cstdlib>
#include <math.h>
#include <cstdio>
#include <fstream>
#include <string>
#include <iostream>
#include <map>

#include "sndfile.hh"

#define SAMPLE_RATE 18000
#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0 / SAMPLE_RATE)

#define SAMPLES_IN_BAUD 30

using namespace std;

void printHelp() {
	cerr<<"Zadajte ako jediný parameter vstupný súbor"<<endl;
}


/*
 * min 500 baud /s => 1 baud max 36 vzoriek
 */
int main(int argc, char** argv) {

	if (argc != 2) {
		printHelp();
		return EXIT_FAILURE;
	}

	/**
	 * QPSK angle offsets table
	 * */
	map<string, double> table;
	table["00"] = 3*M_PI/4.0;
	table["01"] = M_PI/4.0;
	table["10"] = 5*M_PI/4.0;
	table["11"] = 7*M_PI/4.0;

	ifstream inFile(argv[1]);
	if (!inFile.is_open()) {
		cerr<< "Nemožno otvoriť súbor "<<argv[1]<<endl;
		return EXIT_FAILURE;
	}

    string inputCode((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inputCode = "00110011" + inputCode;

    int *buffer = new int[SAMPLES_IN_BAUD*(inputCode.length()/2)];
    int k = 0;
   	for(unsigned idx = 0; idx < inputCode.length()-2; idx+=2) { // iterate through all bauds
   	    double codeAngle = table[inputCode.substr(idx, 2)];
   	    for (int i = 0; i < SAMPLES_IN_BAUD; i++) { // iterate through all sampls in a baud
   	        buffer[k] = AMPLITUDE * sin(FREQ * 2 * k * M_PI + codeAngle);
   	        k++;
   	    }
   	}

    SndfileHandle outputFile;
    string outName = string(argv[1]);
    outName = outName.replace(outName.end()-3, outName.end(), "wav");
    outputFile = SndfileHandle(outName, SFM_WRITE, FORMAT, CHANELS, SAMPLE_RATE);

    outputFile.write(buffer, k);

    delete [] buffer;
    return EXIT_SUCCESS;
}

