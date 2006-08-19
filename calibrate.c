/* calibrate.c
******************************************************************************
   This file is part of morse-x!
   Author: Stephan Beyer
   
   Description of this file:
       calibration functions

   Copyright (C) GPL, 2004, 2006 Stephan Beyer - s-beyer@gmx.net
  
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
   THE AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************************
*/

#include <stdlib.h> /* getenv() ... */
#include <stdio.h> /* puts(), printf()... */
#include <string.h> /* strlen() */

#include "main.h"
#include "calibrate.h"
#include "errors.h"

/* returns a pointer to a string containing the filename of 
 * the calibrate file */
char *
CalibrateFilename(void)
{
	char *ret;
	char *home = getenv("HOME");
	if(home)
	{
		ret = malloc(strlen(home)+strlen(RC_FILE)+2);
		DoAndErr(!ret, "malloc", errMEMORY);
		sprintf(ret, "%s/%s", home, RC_FILE);
	} else {
		ret = malloc(strlen(RC_FILE)+1);
		DoAndErr(!ret, "malloc", errMEMORY);
		strcpy(ret, RC_FILE);
		/*ret = strdup(RC_FILE);
		DoAndErr(!ret, "strdup", errMEMORY);*/
	}
	return ret;
}

/* read calibrate file; returns 'dit' duration or -1 on failure */
int
ReadCalibrateFile(void)
{
	int dur;
	char *filename = CalibrateFilename();
	FILE *cf;
	cf = fopen(filename, "r");
	free(filename);
	if (!cf)
		return -1;
	if(!fread(&dur, sizeof(dur), 1, cf))
		return -1;
	fclose(cf);
	return dur;
}

/* writes 'dit' duration to calibrate file */
void
WriteCalibrateFile(int dur)
{
	FILE *cf;
	char *filename = CalibrateFilename();
	cf = fopen(filename, "w");
	free(filename);
	DoAndErr(!cf, "fopen", errFILE);
	DoAndErr(!fwrite(&dur, sizeof(dur), 1, cf), "fwrite", errNO);
	fclose(cf);
}

/* main calibration function: print info, get duration average, write it
 * to file and return it; returns -1 if an exit was suggested */
int
Calibrate(void)
{
	int dur = 0; /* the average duration */
	int n; /* duration of one loop */

	/* info */
	puts("Speed Calibration\n-----------------\n\n"
	"Imagine you 'dit'. How should morse-x know if you have meant a 'dit'\n"
	"or a 'daw'? So it needs a given duration for comparison. Calibrating\n"
	"means telling morse-x that duration.\n");
	puts(
	"morse-x is very unflexible regarding to your morse speed. It does neither\n"
	"save the duration of a 'daw' nor of the gaps between characters and\n"
	"words. It only saves the duration of a 'dit' and then expects 'daw's\n"
	"to last at least three 'dit' times. A gap between characters is as long\n"
	"as one 'dit' and a gap between words lasts seven 'dit' times. Alright?\n\n"
	"Now please morse:\n");

#define PRESS	n = KeyPressLoop(NULL); \
		if (n < 0) \
			return n;
#define DIT	PRESS dur += n*3;
#define DAW	PRESS dur += n;

	/* orders */
	puts("\t* 'A' ('dit daw' / .-)");
	paint(".-");
	DIT
	paint("-");
	DAW

	puts("\t* 'N' ('daw dit' / -.)");
	paint("-.");
	DAW
	paint(".");
	DIT

	puts("\t* 'S' ('dit dit dit' / ...)");
	paint("...");
	DIT
	paint("..");
	DIT
	paint(".");
	DIT

	puts("\t* 'O' ('daw daw daw' / ---)");
	paint("---");
	DAW
	paint("--");
	DAW
	paint("-");
	DAW

	dur /= 30;
	WriteCalibrateFile(dur);
	puts("\nThanks! That's all!\nYou may morse now.\n");

	return dur;
}
