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
#include <X11/xpm.h> /* XpmCreatePixmapFromData()... */

#include "calibrate.h"
#include "common.h"
#include "main.h"
#include "morse.h"
#include "options.h"
#include "errors.h"
#include "xcommon.h"

/* main routine */
int main(int argc, char **argv)
{
	Atom wm_delete_window; /* WM_DELETE_WINDOW atom */
	XKeyboardState keyboard; /* keyboard configuration */

	/* introduction message */
	printf("%s                                   version %s\n",
							TITLE, VERSION);
	puts("GPL, (C) 2004, Stephan Beyer, s-beyer@gmx.net\n");

	AllocHints();

	/* handle options and open display */
	HandleOptions(argc, argv);

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

	/* Set WM_PROTOCOLS property: WM_DELETE_WINDOW */
	wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
	XSetWMProtocols(display, window, &wm_delete_window,  1);

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

	/* select events for input */
	XSelectInput(display, window,
			KeyPressMask | KeyReleaseMask | FocusChangeMask 
			/*| StructureNotifyMask*/ | ExposureMask);

	MainProgram();
	
	/* cleanup */
	if(keyboard.global_auto_repeat == AutoRepeatModeOn)
		XAutoRepeatOn(display);

	XFreeGC(display, gc);

	XUnmapWindow(display, window);
	XDestroyWindow(display, window);

	FreeHints();

	XCloseDisplay(display);
	return 0;
}
