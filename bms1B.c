/*
 * File:   bms1B.cpp
 */

#include <stdlib.h>
#include <stdio.h>
#include "sndfile.h"


#define PI 3.14159265358979323846
 #define AMPLITUDE (1.0 * 0x7F000000)

/*
 *
 */
double addNormalize(double angle, double deltaAngle);

double addAngle(double angle, double deltaAngle);

double addNormalizeChangeDelta(double angle, double *deltaAngle);


char* process_input(int argc, char **argv);

FILE *open_file (char *file_name);

unsigned int close_file(FILE * fw);

double addNormalizeChangeDelta(double angle, double *deltaAngle);

int main(int argc, char** argv) {

       if (argc != 2) {
        printHelp();
        return EXIT_FAILURE;
    }

    SndfileHandle inputFile;
    
    inputFile = SndfileHandle(argv[1]);
    
    int framesCount = inputFile.frames();
    int *buffer;
    buffer = new int[framesCount];
    inputFile.read(buffer, inputFile.frames());

    #ifdef DEBUG
    cerr<<"Samples size: "<<framesCount<<endl;
    #endif

    double firstAngle;
    double deltaAngle;
    string decoded = "";

    /* finding angle of a sin between two samples in the input signal */
    firstAngle = asin(((double)buffer[0]) / AMPLITUDE);
    deltaAngle = asin(((double)buffer[1]) / AMPLITUDE) - firstAngle;
    #ifdef DEBUG
    cerr<<"deltaAngle: "<<deltaAngle<<" = "<<deltaAngle*180/PI<<"°"<<endl;
    #endif
    double actAngle = firstAngle;
    double deltaX1 = deltaAngle;
    double deltaX4 = deltaAngle;

    /* QPSK bauds mapping values */
    double expectedAngle1 = addAngle(0, 3*PI/4.0);
    double expectedAngle2 = addAngle(0, PI/4.0);
    double expectedAngle3 = addAngle(0, 5*PI/4.0);
    double expectedAngle4 = addAngle(0, 7*PI/4.0);

    int changes = 0;
    int firstChange = 0;
    int secondChange = 0;
    int thirdChange = 0;
    double expectedAngle1b = addNormalize(0, expectedAngle1);
    double expectedAngle4b = addNormalize(0, expectedAngle4);
    double *expectedAngle = &expectedAngle1b;
    /* while 3 changes in sync part of the signal signal */
    while (changes < 3) {
        #ifdef DEBUG
        cerr<<"loop: "<<thirdChange<<endl;
        cerr<<"actAngle: "<<actAngle<<" = "<<actAngle*180/PI<<"° "<<sin(actAngle)<<endl;
        cerr<<"expected: "<<*expectedAngle<<" = "<<*expectedAngle*180/PI<<"° "<<sin(*expectedAngle)<<endl;
        #endif
        /* we have a change in a baud */
        if (!((actAngle > *expectedAngle - (PI/20))&&(actAngle < *expectedAngle + (PI/20)))) {
            if (changes == 0) {
                expectedAngle = &expectedAngle4b;
            } else if (changes == 1) {
                expectedAngle = &expectedAngle1b;
            }
            changes++;
        }
        /* counting samples till first change */
        if (changes < 1) {
            firstChange++;
        }
        /* counting samples till second change */
        if (changes < 2) {
            secondChange++;
        }
        /* counting samples till third change */
        if (changes < 3) {
            thirdChange++;
        }
        /* thirdChange is also a counter for this loop */
        actAngle = asin(((double)buffer[thirdChange]) / AMPLITUDE);
        expectedAngle1b = addNormalizeChangeDelta(expectedAngle1b, &deltaX1);
        expectedAngle4b = addNormalizeChangeDelta(expectedAngle4b, &deltaX4);
    }
    /* preparing values for voting */
    secondChange /= 2;
    thirdChange /=3;
    int samplesPerBaud = 0;
    /* voting for samles per baud... we should have at least 2 same
     * values to claim it as a samples per baud */
    if (firstChange == secondChange) {
        samplesPerBaud = firstChange;
    } else if (secondChange == thirdChange) {
        samplesPerBaud = secondChange;
    } else if (firstChange == thirdChange) {
        samplesPerBaud = firstChange;
    }

    #ifdef DEBUG
    cerr<<"SPB: "<<samplesPerBaud<< " 1st: "<<firstChange<<" 2nd: "<<secondChange<<" 3rd: "<<thirdChange<<endl;
    #endif
    
    double res1 = 0;
    double res2 = 0;
    double res3 = 0;
    double res4 = 0;

    /* Iteraes through the all samples. For each baud we are looking
     * for the sin values that differs the least from actual baud */
    for (int i = 0, s = samplesPerBaud; i < framesCount; i++, s--) {

        /* counting differences for each sin */
        res1 += fabs(buffer[i] - sin(expectedAngle1));
        res2 += fabs(buffer[i] - sin(expectedAngle2));
        res3 += fabs(buffer[i] - sin(expectedAngle3));
        res4 += fabs(buffer[i] - sin(expectedAngle4));

        /* we have processed a baud and we are looking for the
         * least change */
        if (s == 0 || i == framesCount-1) {
            s = samplesPerBaud;
            double res12;
            double res34;
            int resId12 = 0;
            int resId34 = 0;
            if (res1 < res2) {
                res12 = res1;
                resId12 = 1;
            } else {
                res12 = res2;
                resId12 = 2;
            }
            if (res3 < res4) {
                res34 = res3;
                resId34 = 3;
            } else {
                res34 = res4;
                resId34 = 4;
            }
            int resId;
            if (res12 < res34) {
                resId = resId12;
            } else {
                resId = resId34;
            }
            switch(resId) {
                case 1: decoded += "00"; break;
                case 2: decoded += "01"; break;
                case 3: decoded += "10"; break;
                case 4: decoded += "11"; break;
                default: break;
            }

            res1 = 0;
            res2 = 0;
            res3 = 0;
            res4 = 0;
        }
        expectedAngle1 = addAngle(expectedAngle1, -deltaAngle);
        expectedAngle2 = addAngle(expectedAngle2, -deltaAngle);
        expectedAngle3 = addAngle(expectedAngle3, -deltaAngle);
        expectedAngle4 = addAngle(expectedAngle4, -deltaAngle);
    }
    #ifdef DEBUG
    cerr<<decoded<<endl;
    cerr<<decoded.substr(8);
    #endif

    /* writting to an output file */
    string outName = string(argv[1]);
    outName = outName.replace(outName.end()-3, outName.end(), "txt");
    ofstream outFile(outName);
    if (!outFile.is_open()) {
        cerr<<"Can not open output file\n";
        return EXIT_FAILURE;
    }
    outFile<<decoded.substr(8);
    outFile.close();
    delete [] buffer;
    return EXIT_SUCCESS;
}


char* process_input(int argc, char **argv)
{


    //najprv testujeme, ci mame spravny pocet parametrov
    if (argc == 2)
    {
        return argv[1];
    }
    return NULL;
}

FILE* open_file (char *file_name)
{
    FILE *fw = fopen (file_name, "r");
    if (fw == NULL)
    {
        return NULL;
    }
    return fw;
}

void close_file(FILE * fw)
{
    fclose (fw);
}



double addNormalizeChangeDelta(double angle, double *deltaAngle) {
    angle += *deltaAngle;
    if (angle > PI/2) {
        *deltaAngle = -*deltaAngle;
        angle = PI/2 - (angle - PI/2);
    } else if (angle < -PI/2) {
        *deltaAngle = -*deltaAngle;
        angle = -PI/2 - (angle + PI/2);
    }
    return angle;
}

/**
 * Adding deltaAngle value to angle and normalizes it to arcsin definition scope.
 * If the angle runs over the upper bound (PI/2).
 * */
double addNormalize(double angle, double deltaAngle) {
    angle += deltaAngle;
    if (angle > PI/2) {
        angle = PI/2 - (angle - PI/2);
    } else if (angle < -PI/2) {
        angle = -PI/2 - (angle + PI/2);
    }
    return angle;
}

/**
 * Adds deltaAngle to the angle and normalizes it to <0, 2*PI> range
 * */
double addAngle(double angle, double deltaAngle) {
    angle += deltaAngle;
    int norm = angle/(2*PI);
    angle -= (2*PI)*norm;
    if (angle < 0) {
        angle += 2*PI;
    }
    return angle;
}