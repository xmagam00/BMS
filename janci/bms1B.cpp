/* 
 * bms1B
 * xmagam00
 * Martin Maga
 * BMS proj 1
 * QPSK demodulator
 */


#define AMPLITUDE (1.0 * 0x7F000000)


#include <vector>
#include "sndfile.hh"
#include <iostream>
#include <math.h>
#include <fstream>
#include <cstdlib>

using namespace std;

double add_normalize_angle(double angle);

double normalize_angle(double angle_value);

double normalize_delta(double *delta, double angle);

string process_input(int argc, char** argv);

int *read_file(string file_name, int *frames_length);

int get_samples_baud(int change_first, int change_second, int change_third);

int main(int argc, char** argv) {

    //set local variable
    int frames = 0;
    double first_ang = 0.0;
    double delta_angle = 0.0;
    string output_code = "";
    int data_changes = 0;
    int change_first = 0;
    int change_second = 0;
    int change_third = 0;

    //angle period values
    const double first_angle_value = 3 * M_PI / 4.0;
    const double second_angle_value = M_PI / 4.0;
    const double third_angle_value = 5 * M_PI / 4.0;
    const double four_angle_value = 7 * M_PI / 4.0;
    const string first_angle_str_value  = "00";
    const string second_angle_str_value = "01";
    const string third_angle_str_value  = "10";
    const string four_angle_str_value   = "11";

    //set angles
    double first_angle = add_normalize_angle(first_angle_value);
    double second_angle = add_normalize_angle(second_angle_value);
    double third_angle = add_normalize_angle(third_angle_value);
    double four_angle = add_normalize_angle(four_angle_value);
    
    //get file name from program arguments
    string file_name = process_input(argc, argv);

    //test if user set correct program arguments
    if (file_name == "error") {
        cerr << "Chyba v parametroch. Zadajte 1 parameter a to nazov vstupneho suboru" << endl;        
        return EXIT_FAILURE;
    }
    
    //read data from input file
    int *buffer = read_file(file_name, &frames);

    first_ang = asin( ( (double) buffer[0] ) / AMPLITUDE);
    delta_angle = asin( ( (double) buffer[1] ) / AMPLITUDE) - asin(((double)buffer[0]) / AMPLITUDE);

    double actual_angle = first_ang;
    double delt1 = delta_angle;
    double delt4 = delta_angle;
    
    double first_angle_norm = normalize_angle(first_angle);
    double four_angle_norm = normalize_angle(four_angle);
    double *angle_exp = &first_angle_norm;
    const int data_shift = 3;
    const double M_PI_20 = M_PI / 20;
    
    while (data_changes < data_shift) {

        bool test1 = actual_angle > *angle_exp - (M_PI_20);
        bool test2 = actual_angle < *angle_exp + (M_PI_20);

        if (!((test1 && test2))) 
        {
            if (data_changes == 1) {
                angle_exp = &first_angle_norm;
            } else if (data_changes == 0) {
                angle_exp = &four_angle_norm;
            }
            data_changes = data_changes + 1;
        }
        
        if (data_changes < 3) {
            change_third += 1;
        }
        
        if (data_changes < 1) {
            change_first += 1;
        }
        
        if (data_changes < 2) {
            change_second += 1;
        }
        
        double value = ((double) buffer[change_third]) / AMPLITUDE;
       
        actual_angle = asin(value);
        four_angle_norm = normalize_delta(&delt4, four_angle_norm);
        first_angle_norm = normalize_delta(&delt1, first_angle_norm);
    }

    change_second = change_second / 2;
    change_third = change_third / 3;

    int samples_baud = get_samples_baud(change_first, change_second, change_third);
    
    double diff1 = 0;
    double diff2 = 0;
    double diff3 = 0;
    double diff4 = 0;
    int frames_1 = frames - 1;
 
    for (int j = 0, samples = samples_baud; j < frames; j++, samples--) {
        
        diff1 = diff1 + fabs(buffer[j] - sin(first_angle));
        diff2 = diff2 + fabs(buffer[j] - sin(second_angle));
        diff3 = diff3 + fabs(buffer[j] - sin(third_angle));
        diff4 = diff4 + fabs(buffer[j] - sin(four_angle));
        
        if (j == frames_1 || samples == 0) {
            
            samples = samples_baud;
            
            int diff12 = 0;
            int diff34 = 0;
            
            //test difference beetween sins
            if (diff1 < diff2) {
                diff12 = diff1;
                diff12 = 1;
            } else {
                diff12 = diff2;
                diff12 = 2;
            }
            if (diff3 < diff4) {
                diff34 = diff3;
                diff34 = 3;
            } else {
                diff34 = diff4;
                diff34 = 4;
            }
            int diff = 0;
            
            if (diff12 < diff34) {
                diff = diff12;
            } else {
                diff = diff34;
            }

            //get get changes to get output bits
            if (diff == 1) {
                output_code = output_code + first_angle_str_value;
            } else if (diff == 2) {
                output_code = output_code + second_angle_str_value;
            } else if (diff == 3) {
                output_code = output_code + third_angle_str_value;
            } else if (diff == 4) {
                output_code = output_code + four_angle_str_value;
            }
            
            //reincialize diff
            diff1 = 0;
            diff2 = 0;
            diff3 = 0;
            diff4 = 0;
        }

        //normalize angle
        four_angle   = add_normalize_angle (four_angle   - delta_angle);
        third_angle  = add_normalize_angle (third_angle  - delta_angle);
        second_angle = add_normalize_angle (second_angle - delta_angle);
        first_angle  = add_normalize_angle (first_angle  - delta_angle);
        
    }

    //get TXT version of file name
    string output_file_name = file_name.replace(file_name.end() - 3, file_name.end(), "txt");
    
    //open output file
    ofstream outFile(output_file_name);
    
    //test if file is open 
    if (! outFile.is_open()) {
        cerr << "Subor nemoze byt otvoreny" << endl;
        return EXIT_FAILURE;
    }

    //remove first 8 bits from input data
    string output_code_substr =  output_code.substr(8);
    
    //write data
    outFile << output_code_substr;
    
    //close file
    outFile.close();
    
    delete [] buffer;
    return EXIT_SUCCESS;
}

int get_samples_baud(int change_first, int change_second, int change_third) {
    int samples_baud = 0;
    if (change_first == change_second) {
        samples_baud = change_first;
    } else if (change_second == change_third) {
        samples_baud = change_second;
    } else if (change_first == change_third) {
        samples_baud = change_first;
    }
    return samples_baud;
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
int* read_file(string file_name, int *frames_length) {
    
    //file handler
    SndfileHandle inputFile;
    
    //read file from input WAV file
    inputFile = SndfileHandle(file_name);
    
    //get number of frames
    int frames = inputFile.frames();
    
    //set input buffer
    int *buffer;
 
    buffer = new int[frames];
    
    //read data from input file
    inputFile.read(buffer, frames);
    *frames_length = frames;
 
    return buffer;
}


/**
 * Normalize angle according arcsin.
 * */
double normalize_delta(double *delta, double angle) {
    
    const double half_pi = M_PI / 2;
    
    angle = angle + *delta;
    
    if (half_pi < angle) {
        *delta = -*delta;
        angle = half_pi - (angle - half_pi);
    } else if (-half_pi > angle) {
        *delta = -*delta;
        angle = -(half_pi) - (angle + half_pi);
    }
    return angle;
}

/**
 * Normalize angle and test if run over M_PI/2
 * */
double normalize_angle(double angle_value) {
    
    const double half_pi = M_PI / 2;
   
    if ( half_pi < angle_value) {
        angle_value = half_pi - (angle_value - half_pi);
    } else if ( -half_pi > angle_value) {
        angle_value = -(half_pi) - (angle_value + half_pi);
    }
    return angle_value;
}

/**
 * Normalize to angle 0, 2 * M_PI
 * */
double add_normalize_angle(double angle) {
    int norm_angle = 0.0;
    double double_MPI = 2 * M_PI;

    norm_angle = angle / double_MPI;
    
    angle = angle - double_MPI  * norm_angle;
    if (0 > angle) {
        angle = angle + double_MPI;
    }
    return angle;
}