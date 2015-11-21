/*
 * File:   bms1A.cpp
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "sndfile.h"

#define SAMPLE_RATE 18000
#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0 / SAMPLE_RATE)
#define BAUDS 30
#define PI 3.141592653589793

char* process_input(int argc, char **argv);

FILE *open_file (char *file_name);

void close_file(FILE * fw);

double get_phase(char x1, char x2);

int main(int argc, char** argv)
{
    char *file_name = (char*) malloc(50);
    char *result = process_input(argc, argv);	//pom. premenna pre funkciu iden_input

    if (result == NULL)
    {
        fprintf (stderr, "Chyba v parametroch. Program ocakava ako len 1 parameter a to nazov suboru\n");
        return EXIT_FAILURE;
    }
    char *e = strchr(result, '.');
    int index = (int)(e - result);
    strncpy(file_name, result, index);

    FILE *fw = open_file(result);

    if (fw == NULL)
    {
        fprintf (stderr, "Chyba pri otvarani suboru\n");
        return EXIT_FAILURE;
    }


    int znak;

    int bits_length = 0;
    char *data = (char *)malloc(1200);

    while ((znak = getc(fw)) != EOF)
    {

        if (znak == 48 || znak == 49)
        {
            data[bits_length] = znak;
            bits_length++;
        }
        else if (znak != 10)
        {
            fprintf(stderr, "Chybne data v subore\n");
            return EXIT_FAILURE;
        }


    }
    if (bits_length  == 0)
    {
        fprintf(stderr, "Chyba: prazdny subor\n");
        return EXIT_FAILURE;
    }


    if (bits_length % 2 == 1)
    {
        fprintf(stderr, "Chyba: neparny pocet vstupnych bitov\n");
        return EXIT_FAILURE;
    }
    data[bits_length] = '0';
    data[bits_length++] = '0';
    data[bits_length++] = '1';
    data[bits_length++] = '1';
    data[bits_length++] = '0';
    data[bits_length++] = '0';
    data[bits_length++] = '1';
    data[bits_length++] = '1';





    int *buffer = (int *)malloc(BAUDS * (bits_length / 2));

    int k = 0;
    //printf("%d", bits_length);

    for(int idx = 0; idx < bits_length; idx+=2)
    {
        double codeAngle = get_phase(data[idx], data[idx+1]);
        for (int i = 0; i < BAUDS; i++)   // iterate through all sampls in a baud
        {
            buffer[k] = AMPLITUDE * sin(FREQ * 2 * k * PI + codeAngle);
            k++;
        }

    }

    SF_INFO sfinfo;

    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;
    sfinfo.channels = 1;
    sfinfo.samplerate = SAMPLE_RATE;

    SNDFILE* outputFile = sf_open(file_name, SFM_WRITE, &sfinfo);

    sf_write_raw(outputFile, buffer, k);
    free(buffer);
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

//funkcia otvori subor a vrati handle
FILE *open_file (char *file_name)
{
    FILE *fw = fopen (file_name, "r");
    if (fw == NULL)
    {
        return NULL;
    }
    return fw;
}

//funkcia zavrie subor
void close_file(FILE * fw)
{
    fclose (fw);
}


double get_phase(char x1, char x2)
{
    const int first_angle = 3 * PI / 4.0;
    const int second_angle = PI / 4.0;
    const int three_angle = 5 * PI / 4.0;
    const int four_angle = 7 * PI / 4.0;
    double shift = 0;

    if (x1 == 0 && x2 == 0)
    {
        shift = first_angle;
    }
    else if (x1 == 0 && x2 == 1)
    {
        shift = second_angle;
    }
    else if (x1 == 1 && x2 == 0)
    {
        shift = three_angle;
    }
    else if (x1== 1 && x2 == 1)
    {
        shift = four_angle;
    }
    return shift;

}


