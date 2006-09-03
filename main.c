/* main.c
******************************************************************************

   Morse

   Author: Stephan Beyer
   
   Copyright (C) 2004,2006  Stephan Beyer - s-beyer@gmx.net
  
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

#include <stdlib.h> /* atoi(), atof() ... */
#include <stdio.h> /* puts(), printf()... */

#include <SDL/SDL.h>

#include "audio.h"
#include "main.h"
#include "morse.h"
#include "errors.h"

/* global variables and constants */
SDL_Surface *screen;

const Uint16 dawpx[] = {
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00 };
const Uint16 ditpx[] = {
	0x7F,0xFF,0x7F,
	0xFF,0xFF,0xFF,
	0x7F,0xFF,0x7F };
SDL_Surface *daw, *dit;

/* print information about possible options */
void
Usage(char *binary)
{
	printf(	"Usage:  %s [options]\n\n"
		"Possible options are:\n"
		"\t-c       try to check sending wpm\n"
		"\t-f freq  set sound frequency [%d]\n"
		"\t-g       like -s, but in main window\n"
		"\t-h       print this usage information\n"
		"\t-m       mute, no sounds are played\n"
		"\t-r rate  set audio sample rate [%d]\n"
		"\t-s       show dits (as '.') and daws (as '-')\n"
		"\t-t       show morse code table and exit\n"
		"\t-v vol   sound volume between 0 and 1 [%.1f]\n"
		"\t-w wpm   set words per minute [%d]\n"
		"The '-' prefix to options is optional.\n",
		binary, DEFAULT_FREQ, DEFAULT_RATE, DEFAULT_VOLUME, 
		DEFAULT_WPM);
}

/* handle options and open display */
void
HandleOptions(int argc, char **argv)
{
	char **i; /* help pointer to process arguments */

	/* option defaults */
	opt.checkwpm = 0;
	opt.showgfx = 0;
	opt.show = 0;
	opt.mute = 0;
	opt.freq = DEFAULT_FREQ;
	opt.rate = DEFAULT_RATE;
	opt.vol = DEFAULT_VOLUME;
	opt.wpm = DEFAULT_WPM;

	/* go through the arguments */
	i = argv+1;
	while (*i)
	{
		if (**i == '-') /* optional '-' */
			(*i)++;
		switch(**i)
		{
			case 'c':
				opt.checkwpm = 1;
				break;
			case 'f':
				i++;
				if(*i)
				{
					opt.freq = atoi(*i);
					if(opt.freq < 200 || opt.freq > 2000)
						ErrorMsg(errARGS, "Frequency must be a value between 200 and 2000 Hz!");
				} else
					ErrorMsg(errARGS,"-f option needs frequency!");
				break;
			case 'g':
				opt.showgfx = 1;
				break;
			case 'h':
				Usage(*argv);
				exit(0);
			case 'm':
				opt.mute = 1;
				break;
			case 'r':
				i++;
				if(*i)
				{
					opt.rate = atoi(*i);
					if(opt.rate < 4000)
						ErrorMsg(errARGS, "Sample rate must be at least 4000 Hz!");
				} else
					ErrorMsg(errARGS,"-r option needs sample rate!");
				break;
			case 's':
				opt.show = 1;
				break;
			case 't':
				showTable();
				exit(0);
			case 'v':
				i++;
				if(*i)
				{
					opt.vol = atof(*i);
					if(opt.vol < 0 || opt.vol > 1)
						ErrorMsg(errARGS, "Volume must be a value between 0 and 1!");
				} else
					ErrorMsg(errARGS,"-v option needs volume!");
				break;
			case 'w':
				i++;
				if(*i)
				{
					opt.wpm = atoi(*i);
					if(opt.wpm < 1 || opt.wpm > 60)
						ErrorMsg(errARGS, "WPM must be an integer between 1 and 60!");
				} else
					ErrorMsg(errARGS,"-w option needs number of words per minute!");
				break;
			default:
				ErrorMsg(errARGS, "Wrong arguments. Use -h option for help!");
		}
		i++;
	}
}

/* paint a "daw" ;) */
void
paintDaw(SDL_Surface *s, int x, int y)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	dest.w = 9;
	dest.h = 3;
	SDL_BlitSurface(daw,NULL,s,&dest);
}

/* ...and a "dit" */
void
paintDit(SDL_Surface *s, int x, int y)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	dest.w = 3;
	dest.h = 3;
	SDL_BlitSurface(dit,NULL,s,&dest);
}

void
paint(char *txt)
{
	char *t;
	int x, width = 0, height = screen->h/2-1;
	/* first: get total width */
	for(t=txt; *t; t++)
	{
		if(*t=='-' || *t==' ')
			width += 11;
		else
		if(*t=='.')
			width += 5;
	}

	/* second: paint dits and daws into rectangle */
	x = 1+(screen->w-width)/2;
	for(t=txt; *t; t++)
		if(*t=='-')
		{
			paintDaw(screen, x, height);
			x += 11;
		}
		else
		if(*t==' ')
			x += 11;
		else
		if(*t=='.')
		{
			paintDit(screen, x, height);
			x += 5;
		}
	SDL_Flip(screen);
}

void
initDitDaw(void)
{
	daw = SDL_CreateRGBSurfaceFrom(&dawpx, 9, 3, 16, 18 /* = width*bpp/8 */,
	                               255,0,0,0);
	dit = SDL_CreateRGBSurfaceFrom(&ditpx, 3, 3, 16, 6 /* = width*bpp/8 */,
	                               255,0,0,0);
	/* never SDL_FreeSurface() them, because of dawpx and ditpx */
	/* we could free() them, but we could also let the operating system
	 * do this at program exit ;P */
}

void
SetWMStuff(void)
{
	SDL_Surface *icon = SDL_CreateRGBSurface(SDL_SWSURFACE, 32, 32, 8,
	                                         255, 0, 0, 0);
	Uint8 mask[] = { 0, 15, 224, 0, 0, 127, 252, 0, 3, 255, 255, 128, 7, 255, 255, 192, 7, 255, 255, 192, 15, 255, 255, 224, 127, 255, 255, 252, 255, 255, 255, 254, 127, 255, 255, 252, 127, 255, 255, 252, 127, 255, 255, 252, 127, 255, 255, 252, 255, 255, 255, 254, 255, 255, 255, 254, 255, 255, 255, 254, 255, 255, 255, 254, 255, 255, 255, 254, 255, 255, 255, 254, 255, 255, 255, 254, 127, 255, 255, 252, 127, 255, 255, 252, 127, 255, 255, 252, 63, 255, 255, 248, 63, 255, 255, 248, 31, 255, 255, 240, 15, 255, 255, 224, 127, 255, 255, 252, 255, 255, 255, 254, 127, 255, 255, 252, 0, 127, 252, 0, 0, 15, 224, 0, 0, 0, 0, 0 };
		/* this was: (converted to array using some regexps and Ruby)
		 * 00000000 00001111 11100000 00000000
		 * 00000000 01111111 11111100 00000000
		 * 00000011 11111111 11111111 10000000
		 * 00000111 11111111 11111111 11000000
		 * 00000111 11111111 11111111 11000000
		 * 00001111 11111111 11111111 11100000
		 * 01111111 11111111 11111111 11111100
		 * 11111111 11111111 11111111 11111110
		 * 01111111 11111111 11111111 11111100
		 * 01111111 11111111 11111111 11111100
		 * 01111111 11111111 11111111 11111100
		 * 01111111 11111111 11111111 11111100
		 * 11111111 11111111 11111111 11111110
		 * 11111111 11111111 11111111 11111110
		 * 11111111 11111111 11111111 11111110
		 * 11111111 11111111 11111111 11111110
		 * 11111111 11111111 11111111 11111110
		 * 11111111 11111111 11111111 11111110
		 * 11111111 11111111 11111111 11111110
		 * 01111111 11111111 11111111 11111100
		 * 01111111 11111111 11111111 11111100
		 * 01111111 11111111 11111111 11111100
		 * 00111111 11111111 11111111 11111000
		 * 00111111 11111111 11111111 11111000
		 * 00011111 11111111 11111111 11110000
		 * 00001111 11111111 11111111 11100000
		 * 01111111 11111111 11111111 11111100
		 * 11111111 11111111 11111111 11111110
		 * 01111111 11111111 11111111 11111100
		 * 00000000 01111111 11111100 00000000
		 * 00000000 00001111 11100000 00000000
		 * 00000000 00000000 00000000 00000000 */
	/* Creating icon, I'm insane!
	 * Hardcode icon like this:
	 * First: everything black */
	SDL_FillRect(icon,NULL,0);
	/* Second: a daw is:  a dit is:
	 *    011111110   212            0=black (0,0,0)
	 *    111111111   111            1=red	(255,0,0)
	 *    011111110   212            2=darkred (128,0,0) */
	initDitDaw();
	/*  Put daws at: */
	paintDaw(icon,5,2);
	paintDaw(icon,18,2);
	paintDaw(icon,0,6);
	paintDaw(icon,11,6);
	paintDaw(icon,22,6);
	paintDaw(icon,11,10);
	paintDaw(icon,0,26);
	paintDaw(icon,22,26);
	/*  And dits at: */
	paintDit(icon,5,10);
	paintDit(icon,23,10);
	paintDit(icon,8,14);
	paintDit(icon,14,14);
	paintDit(icon,20,14);
	paintDit(icon,14,18);
	paintDit(icon,11,26);
	paintDit(icon,17,26);
	/* That's it ;) */

	SDL_WM_SetIcon(icon, mask);
	SDL_WM_SetCaption(TITLE,TITLE);
}

/* fills screen with black (== 0) or white (== 1) */
void
Fill(int white)
{
	SDL_FillRect(screen, NULL,
	             (white ? -1 : 0 ));
	SDL_Flip(screen);
}

/* wait for press and release of one key 
 *  - return pressing duration
 *  - return -1 to exit (on q or DestroyEvent)
 *  - return KeyRelease time (argument) if adress is non-NULL */
int
KeyPressLoop(unsigned int *rtime)
{
	unsigned int keypressed = 0;
	int dur = 0;
	Uint32 time = 0;
	Uint32 eventtime;

	while(!dur)
	{
		SDL_Event event;

		SDL_WaitEvent(&event);
		eventtime = SDL_GetTicks();
		switch(event.type)
		{
		case SDL_QUIT:
			return -1; /* ? */
		case SDL_KEYDOWN: /* key pressed? */
			/* exit if it is 'q' */
			if(event.key.keysym.sym==SDLK_q)
				return -1;
			keypressed++;
			/* if no other key currently pressed */
			if(keypressed==1)
			{
				time = eventtime;
				/* fill with white and play tone */
				Fill(1);
				if(!opt.mute)
					playAudio(1);
			}
			break;
		case SDL_KEYUP: /* key released? */
			if(!keypressed)
				break;
			keypressed--;

			/* are all pressed keys released? */
			if(!keypressed)
			{
				/* calculate KeyPressed duration */
				dur = eventtime - time;
				/* return KeyRelease time, if nonNULL */
				if (rtime)
					*rtime = eventtime;
				/* fill with black and stop tone */
				Fill(0);
				if(!opt.mute)
					playAudio(0);
			}
			break;
		case SDL_VIDEORESIZE: /* window resized? */
			SDL_SetVideoMode(event.resize.w, event.resize.h,
			                 screen->format->BitsPerPixel, screen->flags);
			break;
		}
	}
	return dur;
}

/* main routine */
int
main(int argc, char **argv)
{
	/* introduction message */
	printf("%s                                   version %s\n",
							TITLE, VERSION);
	puts("(C) 2004, 2006, Stephan Beyer, s-beyer@gmx.net\tGPL\n");

	/* handle options and open display */
	HandleOptions(argc, argv);

	/* init SDL */
	DoAndErr(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER), SDL_Init, errSDL);
	atexit(SDL_Quit);
	
	if(!opt.mute)
		/* init audio */
		initAudio(opt.rate,1);
	
	/* set wm stuff */
	SetWMStuff();

	/* init video */
	screen = SDL_SetVideoMode(32, 32, 24, SDL_HWSURFACE | SDL_RESIZABLE);
	DoAndErr(!screen, SDL_SetVideoMode, errSDLvideo);

	SDL_PauseAudio(0);

	MainProgram();

	SDL_Quit();
	return 0;
}
