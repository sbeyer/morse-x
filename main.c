/* main.c
******************************************************************************

   Morse

   Author: Stephan Beyer
   
   Copyright (C) 2004  Stephan Beyer - s-beyer@gmx.net
  
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

#include <X11/Xlib.h> /* XOpenDisplay(), XCreateWindow(), XEvent ... */
#include <X11/X.h> /* event masks ... */
#include <X11/Xutil.h> /* XValue, YValue, WidthValue, HeightValue */
#include <X11/xpm.h> /* XpmCreatePixmapFromData()... */

#include "main.h"
#include "morse.h"
#include "calibrate.h"
#include "errors.h"

/* globals */
Opt opt;

/* the main loop, waiting for events */
void EventLoop(int durationunit, Display *display, Window window, GC gc)
{
	/* long eventmask = NoEventMask; */
	unsigned int width, height, null;
	unsigned int keypressed = 0;
	unsigned long keyptime = 0;
	unsigned long keypduration;
	int end = 0; /* end loop? */
	int i = 1; /* morse tree index, see morse.h */

	/* select events for input */
	XSelectInput(display, window,
			KeyPressMask | KeyReleaseMask | FocusChangeMask 
			/*| StructureNotifyMask*/ | ExposureMask);
	
	/* get width and height of window */
	XGetGeometry(display, window, (Window*)&null, (int *)&null, 
			(int *)&null, &width, &height, &null, &null);

	while(!end)
	{
		XEvent event;
		XKeyEvent *keyevent;
		XNextEvent(display, &event);

		switch(event.type)
		{
			case KeyPress: /* key pressed? */
				keyevent = (XKeyEvent *)&event;
				/* pressed 'q'? */
				if (keyevent->keycode == 
						XKeysymToKeycode(display, 0x71))
					end = 1;
				
				if (!keypressed)
				{
					if (keyptime > 0)
					{
						/* new morse character? */
						if (keyevent->time - keyptime >=
								3*durationunit)
						{
							if (opt.show)
								putchar(' ');
							if ((i-2 < (sizeof(morse) / sizeof(char *))) && morse[i-2])
								printf("%s",
									morse[i-2]);
							else
								printf("%%not defined%%");
							if (opt.show)
								putchar(' ');
							fflush(stdout);
							i = 1; /* reset index */
						}
						/* new word? */
						if (keyevent->time - keyptime >=
								7*durationunit)
						{
							if (opt.show)
								putchar('\n');
							else
								putchar(' ');
						}
					}
					
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
			case FocusOut: /* no more focus? */
				/* nasty solution */
				keyevent = (XKeyEvent *)&event;
				if (keypressed)
					keypressed = keyevent->keycode;
				else
					/* just initialize keycode if no
					   key pressed */
					keyevent->keycode = 1;
			case KeyRelease: /* key released? */
				keyevent = (XKeyEvent *)&event;
				/* releasing the same key pressed? */
				if (keypressed == keyevent->keycode)
				{
					/* calculate KeyPressed duration */
					keypduration = keyevent->time
								- keyptime;

					if (keypduration <= 3*durationunit)
					{
						if (opt.show)
							putchar('.');
						i *= 2;
					}
					else
					{
						if (opt.show)
							putchar('-');
						i = 2*i + 1;
					}
					fflush(stdout);
					/* get KeyRelease time (overwrite
					   keypress time) */
					keyptime = keyevent->time;
					
					/* fill with color */
					XSetForeground(display, gc,
						BlackPixel(display,
						DefaultScreen(display)));
					XFillRectangle(display, window, gc,
						0, 0, width, height);

					/* no pressed key */
					keypressed = 0;
					/* I hope 0 isn't reserved for a
					   X keycode, otherwise we need an
					   extra variable, or it's a feature
					   instead of a bug, then :) */
				}
				break;
			case Expose: /* resized? get new geometry */
				XGetGeometry(display, window, (Window*)&null, 
						(int *)&null, (int *)&null,
						&width, &height, &null, &null);
				break;

			case DestroyNotify:
				end = 1;
#if 0
			default:
				printf ("%d\n", event.type);
#endif
		}
	}
	puts("");
}


/* print usage information (arguments) */
void Usage(char *binary)
{
	printf("Usage:  %s [options]\n\n", binary);
	puts("Possible options are:");
	puts("\t-c       calibrate!");
	puts("\t-d disp  use <disp> as X display");
	puts("\t-g geom  set geometry to <geom> (-g for help)");
	puts("\t-h       print this usage information");
	puts("\t-r n c   sets WM_CLASS name to <n> and class to <c>");
	puts("\t-s       show dits (as '.') and daws (as '-')");
	puts("The '-' prefix to options is optional.");
}

/* main routine */
int main(int argc, char **argv)
{
	char **i; /* help pointer to process arguments */
	char *displayname = NULL; /* the string specifying the display */
	Display *display; /* the display itself */
	Window window; /* the window itself */
	XSizeHints *hints; /* size hints for the window */
	XWMHints *wmhints; /* window manager hints */
	XClassHint *classhint; /* resource name and class of the window */
	XKeyboardState keyboard; /* keyboard configuration */
	GC gc; /* a graphic context */
	int gp = NoValue; /* result of XGeometryParsed */
	int dur = ReadCalibrateFile(); /* duration of a 'dit' */

	/* introduction message */
	printf("%s                                   version %s\n",
							TITLE, VERSION);
	printf("GPL, (C) 2004, Stephan Beyer, s-beyer@gmx.net\n\n");

	/* allocation of hints */
	hints = XAllocSizeHints();
	DoAndErr(!hints, "XAllocSizeHints", errMEMORY);
	wmhints = XAllocWMHints();
	DoAndErr(!wmhints, "XAllocWMHints", errMEMORY);
	classhint = XAllocClassHint();
	DoAndErr(!classhint, "XAllocClassHint", errMEMORY);

	/* option defaults */
	opt.calibrate = 0;
	opt.show = 0;
	if (dur < 0)
		opt.firststart = 1;
	else
		opt.firststart = 0;

	/* go through the arguments */
	i = argv+1;
	while (*i)
	{
		if (**i == '-') /* optional '-' */
			(*i)++;
		switch(**i)
		{
			case 'c':
				opt.calibrate = 1;
				break;
			case 'd':
				i++;
				if (*i)
					displayname = *i;
				else
					ErrorMsg(errARGS, "-d option needs display argument, e.g. '-d anyhost:1'");
				break;
			case 'g':
				i++;
				if (*i)
				{
					unsigned int width, height;
					gp = XParseGeometry(*i, 
							&hints->x, &hints->y, 
							&width, &height);
					hints->base_width = width;
					hints->base_height = height;
				}
				else
					ErrorMsg(errARGS, "-g option needs X geometry argument, e.g. '-g 50x50+200-100'\nX geometry format: [=][<width>{xX}<height>][{+-}<xoffset>{+-}<yoffset>]");
				break;
			case 'h':
				Usage(*argv);
				exit(0);
			case 'r':
				i++;
				if (*i)
				{
					classhint->res_name = *i;
					i++;
					if (*i)
						classhint->res_class = *i;
					else
						ErrorMsg(errARGS, "-r option wants a second argument, the WM_CLASS class.");
				} else
					ErrorMsg(errARGS, "-r option wants two arguments:\na WM_CLASS name, and WM_CLASS class, e.g. '-r morse morse'.");
				break;
			case 's':
				opt.show = 1;
				break;
			default:
				ErrorMsg(errARGS, "Wrong arguments. Use -h option for help!");
		}
		i++;
	}
	

	/* set the display */
	if (!displayname) /* if no display set with -d option */
		displayname = getenv("DISPLAY"); /* check for DISPLAY env */
	if (!displayname) /* if no DISPLAY in environment */
		displayname = DEFAULT_DISPLAY; /* use default, set in main.h */
	/* (still unsure about char pointers pointing to constant strings) */

	/* open the chosen display */
	display = XOpenDisplay(displayname);
	DoAndErr(!display,  "XOpenDisplay", errX11);

	/* set default window size hint values */
	hints->min_width = DEFAULT_MINWIDTH; /* minimal size */
	hints->min_height = DEFAULT_MINHEIGHT;
	hints->width_inc = 1; /* steps for resizing */
	hints->height_inc = 1;
	if (gp & WidthValue) /* if width was set with -g option */
		/* width at startup mustn't be below min_width */
		hints->base_width = MAX(hints->base_width, hints->min_width);
	else
		hints->base_width = DEFAULT_WIDTH; /* if not, set default value */
	if (gp & HeightValue) /* if height was set with -g option */
		/* height at startup mustn't be below min_height */
		hints->base_height = MAX(hints->base_height, hints->min_height);
	else
		hints->base_height = DEFAULT_HEIGHT; /* otherwise use default */
	if (gp & XValue) /* if x position was set */
	{
		if (gp & XNegative) /* hints->x < 0 */
			hints->x = DisplayWidth(display, DefaultScreen(display))
					+ hints->x - hints->base_width;
	} else /* otherwise use center of screen */
		hints->x = (DisplayWidth(display, DefaultScreen(display))
					- hints->base_width) / 2;
	if (gp & YValue) /* if y position was set */
	{
		if (gp & YNegative) /* hints->y < 0 */
			hints->y = DisplayHeight(display,DefaultScreen(display))
					+ hints->y - hints->base_height;
	} else /* otherwise use center of screen */
		hints->y = (DisplayHeight(display, DefaultScreen(display))
					- hints->base_height) / 2;
	/* If the user uses Xinerama, the center of the screen will
	   be the center of the Xinerama screens by default. */
	hints->flags |= PBaseSize | PMinSize | PResizeInc | PPosition
		| USSize | USPosition;
	/* The XClassHint isn't touched. Only the user can specify name and
	   class by using the -r option. */

	/* create the window */
	window = XCreateSimpleWindow(display, XDefaultRootWindow(display), 
			hints->x, hints->y, hints->base_width,
			hints->base_height, 0, 0, BlackPixel(display,
						DefaultScreen(display)));
	/* Now set some XWMHints: */
	DoAndErr(XpmCreatePixmapFromData(display, window, icon_xpm, 
			&wmhints->icon_pixmap, &wmhints->icon_mask, NULL)
		!= XpmSuccess, "XpmCreatePixmapFromData()", errNO);
	/* ...doesn't that work? */
	wmhints->flags |= IconPixmapHint | IconMaskHint;
	/* configure window (set title and apply hints) */
	XmbSetWMProperties(display, window, TITLE, TITLE, argv, argc, hints, 
				wmhints, classhint);

	/* map, wait until ready and flush window */
	XSelectInput(display, window, StructureNotifyMask);
	XMapWindow(display, window);
	for (;;)
	{
		XEvent event;
		XNextEvent(display, &event);
		if (event.type == MapNotify)
			break; /* ready */
	}
	XFlush(display);
	
	/* create graphic context */
	gc = XCreateGC(display, window, 0, NULL);
	
	/* save old keyboard AutoRepeatMode configuration */
	XGetKeyboardControl(display, &keyboard);
	/* adjust keyboard */
	if(keyboard.global_auto_repeat == AutoRepeatModeOn)
		XAutoRepeatOff(display);

	/* first start? */
	if (opt.firststart)
	{
		puts("No calibration file found. I guess it's the first start.\n");
		puts("Some general usage information:\n"
		" (1) There is a black Morse window on your X screen.\n"
		" (2) Pressing any key in that window will paint it white.\n"
		"     A short pressing will be recognized as a 'dit', and if you hold the\n"
		"     key for a (short) while, it will be recognized as a 'daw'.\n"
		" (3) The morsed results will only be displayed on this terminal.\n"
		" (4) Have fun.\n");
		puts("Please 'dit' or 'daw' _once_ to go next.\n");

		/* select events for input */
		XSelectInput(display, window,
			KeyPressMask | KeyReleaseMask | ExposureMask);

		CalibrateLoop(display, window, gc);
	}
	
	/* now real main stuff */
	if (opt.calibrate || opt.firststart) /* calibrate! */
		dur = Calibrate(display, window, gc);
	if (opt.firststart)
		puts("Pressing 'q' quits.");
	EventLoop(dur, display, window, gc); /* main loop */
	
	/* cleanup */
	XFreeGC(display, gc);

	XUnmapWindow(display, window);
	XDestroyWindow(display, window);

	XFree(hints);
	XFree(classhint);
	XFree(wmhints);

	if(keyboard.global_auto_repeat == AutoRepeatModeOn)
		XAutoRepeatOn(display);
	XCloseDisplay(display);
	return 0;
}
