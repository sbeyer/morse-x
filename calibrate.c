/* calibrate.c
******************************************************************************
   This file is part of morse-x!
   Author: Stephan Beyer
   
   Description of this file:
       calibration functions

   Copyright (C) GPL, 2004 Stephan Beyer - s-beyer@gmx.net
  
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

#include <X11/Xlib.h> /* XOpenDisplay(), XCreateWindow(), XEvent ... */
#include <X11/X.h> /* event masks ... */
#include <X11/Xutil.h> /* XValue, YValue, WidthValue, HeightValue */

#include "calibrate.h"
#include "common.h"
#include "errors.h"

/* returns a pointer to a string contining the filename of 
 * the calibrate file */
char *CalibrateFilename(void)
{
	char *ret;
	char *home = getenv("HOME");
	if (home)
	{
		ret = malloc(strlen(home)+strlen(RC_FILE)+2);
		DoAndErr(!ret, "malloc", errMEMORY);
		sprintf(ret, "%s/%s", home, RC_FILE);
	}
	else
	{
		ret = malloc(strlen(RC_FILE)+1);
		DoAndErr(!ret, "malloc", errMEMORY);
		strcpy(ret, RC_FILE);
		/*ret = strdup(RC_FILE);
		DoAndErr(!ret, "strdup", errMEMORY);*/
	}
	return ret;
}

/* read calibrate file; returns 'dit' duration or -1 on failure */
int ReadCalibrateFile(void)
{
	int dur;
	char *filename = CalibrateFilename();
	FILE *cf;
	cf = fopen(filename, "r");
	if (!cf)
		return -1;
	fread(&dur, sizeof(dur), 1, cf);
	fclose(cf);
	free(filename);
	return dur;
}

/* writes 'dit' duration to calibrate file */
void WriteCalibrateFile(int dur)
{
	FILE *cf;
	char *filename = CalibrateFilename();
	int foo;
	cf = fopen(filename, "w");
	free(filename);
	DoAndErr(!cf, "fopen", errFILE);
	foo = fwrite(&dur, sizeof(dur), 1, cf);
	DoAndErr(!foo, "fwrite", errNO);
	fclose(cf);
}

/* main calibration function: print info, get duration average, write it
 * to file and return it; returns -1 if an exit was suggested */
int Calibrate(void)
{
	int dur; /* the average duration */
	int n; /* duration of one loop */

	/* info */
	puts("Speed Calibration\n\n"
	"What? Why?\n"
	"Imagine you 'dit'. How should morse-x know if you have meant a 'dit'\n"
	"or a 'daw'? Yes, it needs a given duration for comparison. By calibrating\n"
	"morse-x you'll tell it that duration.\n");
	puts(
	"morse-x is very unflexible regarding to your morse speed. It does not\n"
	"save the duration of a 'daw' or of the gaps between characters and\n"
	"words. It only saves the duration of a 'dit' and then expects 'daw's\n"
	"to last at least 3 'dit's. A gap between characters is as long as one\n"
	"'dit' and a gap between words lasts 7 'dit's.\n\n"
	"Now please morse:\n");

	/* orders */
	puts("\t* 'A' ('dit daw' / .-)");
	n = KeyPressLoop(NULL); /* . */
	if (n < 0)
		return n;
	dur = n * 3;
	n = KeyPressLoop(NULL); /* - */
	if (n < 0)
		return n;
	dur += n;

	puts("\t* 'N' ('daw dit' / -.)");
	n = KeyPressLoop(NULL); /* - */
	if (n < 0)
		return n;
	dur += n;
	n = KeyPressLoop(NULL); /* . */
	if (n < 0)
		return n;
	dur += n * 3;

	puts("\t* 'S' ('dit dit dit' / ...)");

	n = KeyPressLoop(NULL); /* . */
	if (n < 0)
		return n;
	dur += n * 3;
	n = KeyPressLoop(NULL); /* . */
	if (n < 0)
		return n;
	dur += n * 3;
	n = KeyPressLoop(NULL); /* . */
	if (n < 0)
		return n;
	dur += n * 3;

	puts("\t* 'O' ('daw daw daw' / ---)");
	n = KeyPressLoop(NULL); /* - */
	if (n < 0)
		return n;
	dur += n;
	n = KeyPressLoop(NULL); /* - */
	if (n < 0)
		return n;
	dur += n;
	n = KeyPressLoop(NULL); /* - */
	if (n < 0)
		return n;
	dur += n;

	dur /= 30;
	WriteCalibrateFile(dur);
	puts("\nThanks! That's all!\nYou may morse now.\n");

	return dur;
}
