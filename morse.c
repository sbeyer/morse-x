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
#include <stdlib.h> /* malloc(), free(), ... */

#include "main.h"

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
	/* .--.- */ "{бе}",
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

#define LENGTH	(sizeof(morse) / sizeof(char *))

/* prints the character(s) of the given morse tree index i */
void
PrintChar(int i)
{
	if(opt.show)
		putchar(' ');
	if((i-2 < LENGTH) && morse[i-2])
		printf("%s", morse[i-2]);
	else
		printf("%%not defined%%");
	if(opt.show)
		putchar(' ');
}

/* returns ".-"-like C string for num */
char *
ditdaw_str(int num)
{
	char *ret;
	int retlen = 0;
	int t;

	num += 2;
	t = num;

	do
	{
		retlen++;
		t >>= 1;
	} while(t);
	ret = malloc(retlen);

	for(t = 0; t < retlen; t++)
		ret[t] = ((num>>(retlen-2-t))&1 ? '-' : '.');
	ret[t-1] = 0;
	return ret;	
}


/* for -t */
void
showTable(void)
{
	int i;
	for(i = 0; i < LENGTH; i++)
	{
		if(morse[i])
		{
			char *x = ditdaw_str(i);
			printf("%s\t%s\n",x,morse[i]);
			free(x);
		}
	}
}

/* for -c, returns new durunit or -1 if an exit was suggested */
int
checkWPM(void)
{
	int dur = 0; /* the average duration */
	int n; /* duration of one loop */

	/* info */
	puts("WPM Check\n---------\n\nNow please morse:\n");

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

	puts("\nFinished.\n");

	return dur;
}

void
MainProgram(void)
{
	int dur = 0; /* keypress duration */
	int i = 1; /* morse tree index, see intro comment */
	unsigned int time = 0;
	int durunit = 1200/opt.wpm; /* duration of a 'dit' */

	/* checkwpm */
	if (opt.checkwpm || (durunit < 0))
		durunit = checkWPM();

	dur = durunit; /* just for exit check */
	if(dur >= 0)
	{
		printf("Using %d words per minute.\n\n"
		       "Press 'q' in the main window to exit. Any other key morses.\n"
		       "Text:\n", 1200/durunit); /* should equal opt.wpm */
	}

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
