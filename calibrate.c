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

/* waits for one key press + release and then returns the duration */
int CalibrateLoop(Display *display, Window window, GC gc)
{
	unsigned int width, height, null;
	unsigned int keypressed = 0;
	unsigned long keyptime = 0;
	int keypduration = 0;

	/* get width and height of window */
	XGetGeometry(display, window, (Window*)&null, (int *)&null, 
			(int *)&null, &width, &height, &null, &null);

	while(!keypduration)
	{
		XEvent event;
		XKeyEvent *keyevent;
		XNextEvent(display, &event);

		switch(event.type)
		{
			case KeyPress: /* key pressed? */
				keyevent = (XKeyEvent *)&event;
				if (!keypressed)
				{
					/* save keycode and time of KeyPress */
					keypressed = keyevent->keycode;
					keyptime = keyevent->time;
					
					/* fill with color */
					XSetForeground(display, gc, 
						WhitePixel(display, 
						DefaultScreen(display)));
					XFillRectangle(display, window, gc, 
						0, 0, width, height);
				}
				break;
			case KeyRelease: /* key released? */
				keyevent = (XKeyEvent *)&event;
				/* releasing the same key pressed? */
				if (keypressed == keyevent->keycode)
				{
					/* calculate KeyPressed duration */
					keypduration = keyevent->time
								- keyptime;
					/* fill with color */
					XSetForeground(display, gc,
						BlackPixel(display,
						DefaultScreen(display)));
					XFillRectangle(display, window, gc,
						0, 0, width, height);
				}
				break;
			case Expose: /* resized? get new geometry */
				XGetGeometry(display, window, (Window*)&null, 
						(int *)&null, (int *)&null,
						&width, &height, &null, &null);
				break;
		}
	}
	return keypduration;
}

/* main calibration function: print info, get duration average, write it
 * to file and return it */
int Calibrate(Display *display, Window window, GC gc)
{
	int dur; /* the average duration */

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

	/* select events for input */
	XSelectInput(display, window,
		KeyPressMask | KeyReleaseMask | ExposureMask);

	/* orders */
	puts("\t* 'A' ('dit daw' / .-)");
	dur = CalibrateLoop(display, window, gc); /* . */
	dur += CalibrateLoop(display, window, gc) / 3; /* - */
	puts("\t* 'N' ('daw dit' / -.)");
	dur += CalibrateLoop(display, window, gc) / 3; /* - */
	dur += CalibrateLoop(display, window, gc); /* . */
	puts("\t* 'S' ('dit dit dit' / ...)");
	dur +=	CalibrateLoop(display, window, gc) +
		CalibrateLoop(display, window, gc) +
		CalibrateLoop(display, window, gc); /* ... */
	puts("\t* 'O' ('daw daw daw' / ---)");
	dur += (CalibrateLoop(display, window, gc) +
		CalibrateLoop(display, window, gc) +
		CalibrateLoop(display, window, gc)) / 3; /* --- */
	dur /= 10;

	WriteCalibrateFile(dur);
	puts("\nThanks! That's all!\nYou may morse now.\n");

	return dur;
}
