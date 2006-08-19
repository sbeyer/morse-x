/* morse.c
******************************************************************************
   This file is part of morse-x!
   Author: Stephan Beyer
   
   Description of this file:
       morse code functions

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

#include <stdio.h> /* putchar(), printf()... */

#include "main.h"
#include "calibrate.h"

/* the morse code */
/* How I did it:
 *	I use an array as a binary tree, that means, if the array index
 *	i is the root, so the index 2*i is the left, and 2*i + 1 is the
 *	right knot. I decided that 'dit' ('.') is accessable with 2*i, and
 *	'daw' ('-') is 2*i+1, in general.
 *	Because array indeces begin with 0 in C, and because one root
 *	element is missing, you have to substract 2 for the right index, 
 *	when - and ONLY when - accessing.
 *	Summarized in pseudo code:
 *		i = 1;
 *		...
 *		if DIT then
 *			i = 2*i;
 *			... -> morse[i-2];
 *		if DAW then
 *			i = 2*i+1;
 *			... -> morse[i-2]; */
static const char* morse[] = { /* this tree contains ISO-8859-1 characters */
	/* . */ "E",
	/* - */ "T",
	/* .. */ "I",
	/* .- */ "A",
	/* -. */ "N",
	/* -- */ "M",
	/* ... */ "S",
	/* ..- */ "U",
	/* .-. */ "R",
	/* .-- */ "W",
	/* -.. */ "D",
	/* -.- */ "K",
	/* --. */ "G",
	/* --- */ "O",
	/* .... */ "H",
	/* ...- */ "V",
	/* ..-. */ "F",
	/* ..-- */ "Ь",
	/* .-.. */ "L",
	/* .-.- */ "Д",
	/* .--. */ "P",
	/* .--- */ "J",
	/* -... */ "B",
	/* -..- */ "X",
	/* -.-. */ "C",
	/* -.-- */ "Y",
	/* --.. */ "Z",
	/* --.- */ "Q",
	/* ---. */ "Ц",
	/* ---- */ "CH",
	/* ..... */ "5",
	/* ....- */ "4",
	/* ...-. */ " SN ",
	/* ...-- */ "3",
	/* ..-.. */ "Й",
	/* ..-.- */ NULL,
	/* ..--. */ "!",
	/* ..--- */ "2",
	/* .-... */ " AS ",
	/* .-..- */ NULL,
	/* .-.-. */ " AR ", /* often "+" */
	/* .-.-- */ NULL,
	/* .--.. */ NULL,
	/* .--.- */ "(бе)",
	/* .---. */ NULL,
	/* .---- */ "1",
	/* -.... */ "6",
	/* -...- */ " BT ", /* separator, often "=" */
	/* -..-. */ "/", /* (fraction bar) */
	/* -..-- */ NULL,
	/* -.-.. */ NULL,
	/* -.-.- */ " KA ",
	/* -.--. */ "(",
	/* -.--- */ "#",
	/* --... */ "7",
	/* --..- */ NULL,
	/* --.-. */ NULL,
	/* --.-- */ "С",
	/* ---.. */ "8",
	/* ---.- */ NULL,
	/* ----. */ "9",
	/* ----- */ "0",
	/* ...... */ NULL,
	/* .....- */ NULL,
	/* ....-. */ NULL,
	/* ....-- */ NULL,
	/* ...-.. */ "|",
	/* ...-.- */ " SK ",
	/* ...--. */ NULL,
	/* ...--- */ NULL,
	/* ..-... */ NULL,
	/* ..-..- */ NULL,
	/* ..-.-. */ NULL,
	/* ..-.-- */ NULL,
	/* ..--.. */ "?",
	/* ..--.- */ "_",
	/* ..---. */ NULL,
	/* ..---- */ NULL,
	/* .-.... */ NULL,
	/* .-...- */ NULL,
	/* .-..-. */ "\"",
	/* .-..-- */ NULL,
	/* .-.-.. */ NULL,
	/* .-.-.- */ ".",
	/* .-.--. */ NULL,
	/* .-.--- */ NULL,
	/* .--... */ NULL,
	/* .--..- */ NULL,
	/* .--.-. */ "@",
	/* .--.-- */ NULL,
	/* .---.. */ NULL,
	/* .---.- */ NULL,
	/* .----. */ "'",
	/* .----- */ NULL,
	/* -..... */ "\\",
	/* -....- */ "-",
	/* -...-. */ NULL,
	/* -...-- */ NULL,
	/* -..-.. */ NULL,
	/* -..-.- */ NULL,
	/* -..--. */ NULL,
	/* -..--- */ NULL,
	/* -.-... */ NULL,
	/* -.-..- */ NULL,
	/* -.-.-. */ NULL,
	/* -.-.-- */ NULL,
	/* -.--.. */ NULL,
	/* -.--.- */ ")",
	/* -.---. */ NULL,
	/* -.---- */ NULL,
	/* --.... */ NULL,
	/* --...- */ NULL,
	/* --..-. */ NULL,
	/* --..-- */ ",",
	/* --.-.. */ NULL,
	/* --.-.- */ NULL,
	/* --.--. */ NULL,
	/* --.--- */ NULL,
	/* ---... */ ":",
	/* ---..- */ NULL,
	/* ---.-. */ NULL,
	/* ---.-- */ NULL,
	/* ----.. */ NULL,
	/* ----.- */ NULL,
	/* -----. */ "*",
	/* ------ */ NULL
};
#if 0
	/* etc...: */
	/* ....... */ " ERROR ",
	/* ...--.. */ "Я",
	/* ...---... */ " SOS "
#endif


/* prints the character(s) of the given morse tree index i */
void
PrintChar(int i)
{
	if (opt.show)
		putchar(' ');
	if ((i-2 < (sizeof(morse) / sizeof(char *))) && morse[i-2])
		printf("%s", morse[i-2]);
	else
		printf("%%not defined%%");
	if (opt.show)
		putchar(' ');
}

void
MainProgram(void)
{
	int dur = 0; /* keypress duration */
	int i = 1; /* morse tree index, see intro comment */
	unsigned int time = 0;
	int durunit = ReadCalibrateFile(); /* duration of a 'dit' */

	/* first start? */
	if (durunit < 0)
	{
		puts("No calibration file found. I guess it's the first start.\n");
		puts("Some general usage information:\n"
		" (1) There is a small black Morse window on your X screen.\n"
		" (2) Pressing 'q' in that window will quit.\n"
		" (3) Pressing any other key in that window will paint it white.\n"
		"     A short press will be recognized as a 'dit', and if you hold the\n"
		"     key for a (short) while, it will be recognized as a 'daw'.\n"
		" (3) The morsed results will only be displayed on this terminal.\n"
		" (4) Also take a look at the manual page, please.\n"
		" (5) Have fun!\n");
		puts("Please 'dit' or 'daw' _once_ to go next.\n");

		dur = KeyPressLoop(NULL);
		/* dur is only an exit-check here */
	}
	
	/* calibrate! */
	if ((dur >= 0) && (opt.calibrate || (durunit < 0)))
		durunit = Calibrate();

	dur = durunit; /* just for exit check */
	if(dur >= 0)
		puts("Press 'q' in the main window to exit. Any other key morses.\nText:");
	while (dur >= 0) /* main loop */
	{
		unsigned int oldtime = time; /* time of previous KeyRelease */

		dur = KeyPressLoop(&time);
		if (oldtime && (dur >= 0))
		{
			/* new morse character? (3-dit-pause?) */
			if (time - dur - oldtime >= 3*durunit)
			{
				PrintChar(i);
				fflush(stdout);
				i = 1; /* reset index */
			}
			if (time - dur - oldtime >= 7*durunit)
			{
				if (opt.show)
					putchar('\n');
				else
					putchar(' ');
			}
		}
		
		if (dur >= 0) /* not -1 */
		{
			/* duration of 3 dits? => daw */
			if (dur >= durunit * 3)
			{
				if(opt.showgfx)
					paint("-"); /* paint */
				if(opt.show)
					putchar('-');
				i = 2*i + 1;
				
			} else { /* a dit */
				if(opt.showgfx)
					paint("."); /* paint */
				if(opt.show)
					putchar('.');
				i *= 2;
			}
			fflush(stdout);
		}
	}
	if (i > 1)
		PrintChar(i);
	puts("\nBye!");
}
