/* options.c
******************************************************************************
   This file is part of morse-x!
   Author: Stephan Beyer
   
   Description of this file:
       option handling

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

#include <X11/Xlib.h> /* XOpenDisplay(), XCreateWindow(), XEvent ... */
#include <X11/X.h> /* event masks ... */
#include <X11/Xutil.h> /* XValue, YValue, WidthValue, HeightValue */

#include "options.h"
#include "errors.h"
#include "xcommon.h"

/* print information about possible options */
void Usage(char *binary)
{
	printf("Usage:  %s [options]\n\n", binary);
	puts("Possible options are:\n"
	"\t-c       calibrate!\n"
	"\t-d disp  use <disp> as X display\n"
	"\t-g geom  set geometry to <geom> (-g for help)\n"
	"\t-h       print this usage information\n"
	"\t-r n c   sets WM_CLASS name to <n> and class to <c>\n"
	"\t-s       show dits (as '.') and daws (as '-')\n"
	"The '-' prefix to options is optional.");
}

/* handle options and open display */
void HandleOptions(int argc, char **argv)
{
	char **i; /* help pointer to process arguments */
	char *displayname = NULL; /* the string specifying the display */
	int gp = NoValue; /* result of XGeometryParsed */

	/* option defaults */
	opt.calibrate = 0;
	opt.show = 0;

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
}
