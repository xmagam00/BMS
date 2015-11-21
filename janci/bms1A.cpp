/*
 * File:   bms1A.cpp
 * xmagam00
 * BMS proj 1
 * modulator pomocou QPSK
 */

#include <map>
#include <cstdlib>
#include "sndfile.hh"
#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <math.h>

#define SAMPLE_RATE 18000
#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0 / SAMPLE_RATE)

using namespace std;

string process_input(int argc, char** argv);

string read_file_input(string file_name);

int main(int argc, char** argv) {

	//baud speed
	int bauds = 30;

	//angle codes
	const string first_angle  = "00";
	const string second_angle = "01";
	const string third_angle  = "10";
	const string four_angle   = "11";

	//angle values for each code
	const double first_angle_value = 3 * M_PI / 4.0;
	const double second_angle_value = M_PI / 4.0;
	const double third_angle_value = 5 * M_PI / 4.0;
	const double four_angle_value = 7 * M_PI / 4.0;

	//angle table
	map< string, double> angles;
	
	//angle table assigning
	angles[first_angle] = first_angle_value;
	angles[second_angle] = second_angle_value;
	angles[third_angle] = third_angle_value;
	angles[four_angle] = four_angle_value;

	//process input program arguments
	string file_name = process_input(argc, argv);
	
	//test if user set correct program arguments
	if (file_name == "error") {
		cerr << "Chyba. Ako parameter zadajte nazov vstupneho suboru" << endl;
		return EXIT_FAILURE;		
	}

	//read data from input file
	string code = read_file_input(file_name);

	if (code == "error") {
		cerr << "Chyba: Skontrolujte vstupny subor" << endl;
		return EXIT_FAILURE;
	}

	int code_length = code.length() / 2;

	int buffer_size = bauds * code_length;

    int *buffer = new int[buffer_size];
    int counter = 0;
    const int size = 2;

   unsigned code_lenth_iterate = code.length() - 2;
   
   //partial optimized phrase for increasing speed computation
   double partial_period_phrase = FREQ * 2 * M_PI;
   	
   	for(unsigned i = 0; i < code_lenth_iterate; i = i + 2) 
   	{ 
   		string code_substr = code.substr(i, size);
   	    double phase = angles[code_substr];
   	    for (int j = 0; j < bauds; j++) 
   	    { 
   	        buffer[counter] = AMPLITUDE * sin(counter * partial_period_phrase + phase);
   	        counter++;
   	    }
   	}

   	//method replace extesion of file
    string output_file_name = file_name.replace(file_name.end() - 3, file_name.end(), "wav");
    
    //open file
    SndfileHandle output_file = SndfileHandle(output_file_name, SFM_WRITE, FORMAT, CHANELS, SAMPLE_RATE);

    //write data to file
    output_file.write(buffer, counter);

    delete [] buffer;
    
    return EXIT_SUCCESS;
}


/*
 * Method process input arguments
 */ 
string process_input(int argc, char** argv) {

	if (2 != argc) {
		return "error";
	}	

	return argv[1];
}

/*
 * Method read input data from file
 */
string read_file_input(string file_name) {
	
	//open file
	ifstream input(file_name);

	//test if file was opened
	if (!input.is_open()) {
		return "error";
	}

	//read input bites from file
    string code((istreambuf_iterator<char>(input)), istreambuf_iterator<char>());
    code = "00110011" + code;
    //pre-append sync code
    return code;

}
