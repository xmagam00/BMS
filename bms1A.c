/* 
 * File:   bms1A.cpp
 */

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "sndfile.h"


#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0 / SAMPLE_RATE)

/*
 * 
 */
enum ERRORS
{
  ERR_ZERO=1,			//na vstupe bola zadana 0
  ERR_NDIG,			//chyba pri citani znakov, znak nie je cislo
  ERR_PARAM,			//chyba pri citani parametrov z konzole
  NO_ERR,			//prebehlo bez chyby
  ERR_OF ,			//doslo k preteceniu vstupu
  NUL_INP,			//navratova hodnota funkcie read_variable v pripade, ze sa zada prazdni vstup
  NO_ACT,			//pociatocna inicializacna hodnota pre strukturu funckiu iden_input
  //chybny pocet parametrov na terminale
};

PARAM_STRUCT iden_input (int pocet_par, char **prep);

FILE *open_matrix (char *nazov_suboru, unsigned int *chyba);

unsigned int close_matrix (FILE * fw);


int main(int argc, char** argv) {
	int sample_rate = 0;
  char *file_name = (char*) malloc(50);
  char *result = iden_input (argc, argv);	//pom. premenna pre funkciu iden_input
  
  if (result == NULL) {
  	fprintf (stderr, "Chyba v parametroch. Program ocakava ako parameter len nazov suboru");
  }
  char *e = strchr(string, '.');
  index = (int)(e - result);
  strncpy(file_name, result, index);

  FILE *fw = open_file(result);
  
  if (fw == NULL) {
   fprintf (stderr, "Chyba pri otvarani suboru");	
  }

int znak;
 int hodnota = 0;
 int bits_length = 0;
 int *data = (int *)malloc(1200);
  while ((znak = getc(fw)) != EOF)
  {
   if (!fscanf (fw, "%d", &hodnota)) {
   	free(file_name);
   	fprintf(stderr, "Chybne data vo vstupnom subore");
   	return EXIT_ERROR;
   }
   if (hodnota != 0 || hodnota != 1) {
   	free(file_name);
   	fprintf(stderr, "Chybne data vo vstupnom subore");
   	return EXIT_ERROR;
   }
   data[bits_length] = hodnota;
   bits_length++;
  }
sample_rate = bits_length / 2;s


    SndfileHandle outputFile;
    int *buffer = (int *)malloc(sample_rate);
    int frequency = 1000.0 / sample_rate;
    
    for (int i = 0; i < sample_rate; i+2) {
    	int shift = 0;
    	
    	if (data[i] == 0 && data[i + 1] == 0) {
    		shift = 1;
    	} else if (data[i] == 0 && data[i + 1] == 1) {
    		shift = 2;
    	} else if (data[i] == 1 && data[i + 1] == 0) {
    		shift = 3;
    	} else if (data[i] == 1 && data[i + 1] == 1) {
    		shift = 4;
    	}

        buffer [i] = AMPLITUDE * sin(frequency * 2 * i * M_PI + shift);
	
	}
    
    outputFile = SndfileHandle(file_name, SFM_WRITE, FORMAT, CHANELS, sample_rate);

    
    outputFile.write(buffer, sample_rate);
    outputFile.close();

    delete [] buffer;
    return EXIT_SUCCESS;
}

char *file_name iden_input (int pocet_par, char **prep)
{
  

  //najprv testujeme, ci mame spravny pocet parametrov
  if (pocet_par == 2)
    {
      return prep[1];
    } 
return NULL;
}

FILE *open_file (char *file_name)
{
  FILE *fw = fopen (file_name, "r");
  if (fw == NULL)
  {
    return NULL;
  }
  return fw;
}

unsigned int close_file(FILE * fw)
{
  if (fclose (fw) == EOF)
    return ERR_CLOSE;
  else
    return NO_ERR;
}
