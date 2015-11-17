/* 
 * File:   bms1B.cpp
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "sndfile.h"

/*
 * 
 */
char *file_name iden_input (int argc, char **argv);

int main(int argc, char** argv) {
    
    SndfileHandle inputFile;
    int sampleRate;
    int *buffer;
    

    char *file_name = iden_input (argc, argv);	
    if (file_name == NULL) {
    	fprintf(stderr, "Chyba v parametroch");
    }

    SF_INFO sndInfo;
   	SNDFILE *sndFile = sf_open(result, SFM_READ, &sndInfo);
   if (sndFile == NULL) {
      fprintf(stderr, "Error reading source file '%s': %s\n", file_name, sf_strerror(sndFile));
      return EXIT_ERROR;
   }
   
   if (sndInfo.format != (SF_FORMAT_WAV | SF_FORMAT_PCM_24)) {
      fprintf(stderr, "Zly vstupny format udajovs\n");
      sf_close(sndFile);
      return EXIT_ERROR;
   }

   // Check channels - mono
   if (sndInfo.channels != 1) {
      fprintf(stderr, "Zly pocet kananov\n");
      sf_close(sndFile);
      return EXIT_ERROR;
   };
    
    sampleRate = sndFile.samplerate();
    
    buffer = (int *)malloc(sampleRate);

    inputFile.read(buffer, sampleRate);
    
    
    sf_close(sndfile);
    free(buffer);
    return EXIT_SUCCESS;
}

char *file_name iden_input (int argc, char **argv)
{
  //najprv testujeme, ci mame spravny pocet parametrov
  if (pocet_par == 2)
    {
      return prep[1];
    } 
return NULL;
}

