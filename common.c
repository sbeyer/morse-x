/* common.c
******************************************************************************
   This file is part of morse-x!
   Author: Stephan Beyer
   
   Description of this file:
       common functions

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

#include <X11/Xlib.h> /* XOpenDisplay(), XCreateWindow(), XEvent ... */
#include <X11/X.h> /* event masks ... */
#include <X11/Xutil.h> /* XValue, YValue, WidthValue, HeightValue */

#include "common.h"
#include "xcommon.h"
#include "errors.h"

/* get width and height of a window */
void GetWidthHeight(unsigned int *width, unsigned int *height)
{
	unsigned int null;
	XGetGeometry(display, window, (Window*)&null, (int *)&null, 
			(int *)&null, width, height, &null, &null);
}

/* allocation of hints */
void AllocHints(void)
{
	hints = XAllocSizeHints();
	DoAndErr(!hints, "XAllocSizeHints", errMEMORY);
	wmhints = XAllocWMHints();
	DoAndErr(!wmhints, "XAllocWMHints", errMEMORY);
	classhint = XAllocClassHint();
	DoAndErr(!classhint, "XAllocClassHint", errMEMORY);
}

/* ... and free them */
void FreeHints(void)
{
	XFree(hints);
	XFree(classhint);
	XFree(wmhints);
}

/* fills gc with black (== 0) or white (!= 1) to the given size */
void Paint(int blackwhite, unsigned int width, unsigned int height)
{
	XSetForeground(display, gc,
		(blackwhite
		? WhitePixel(display, DefaultScreen(display))
		: BlackPixel(display, DefaultScreen(display))));
	XFillRectangle(display, window, gc, 0, 0, width, height);
}

/* wait for press and release of one key 
 *  - return pressing duration
 *  - return -1 to exit (on q or DestroyEvent)
 *  - return KeyRelease time (argument) if adress is non-NULL */
int KeyPressLoop(unsigned long *rtime)
{
	unsigned int width, height;
	unsigned int keypressed = 0;
	int dur = 0;
	unsigned long time = 0;

	/* get width and height of window */
	GetWidthHeight(&width, &height);

	while(!dur)
	{
		XEvent event;
		XKeyEvent *keyevent;
		XNextEvent(display, &event); /* wait for next event */

		switch(event.type)
		{
			case KeyPress: /* key pressed? */
				keyevent = (XKeyEvent *)&event;
				/* pressed 'q'? */
				if (keyevent->keycode == 
						XKeysymToKeycode(display, 0x71))
					return -1;

				if (!keypressed) /* if no key pressed */
				{
					/* save keycode and time of KeyPress */
					keypressed = keyevent->keycode;
					time = keyevent->time;
					
					/* fill with color */
					Paint(1, width, height);
				}
				break;
			case FocusOut: /* no more focus? */
				/*if (keypressed)
					Paint(0, width, height);
				break;
				*/
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
					dur = keyevent->time - time;
					/* return KeyRelease time, if nonNULL */
					if (rtime)
						*rtime = keyevent->time;
					/* fill with color */
					Paint(0, width, height);
				}
				break;
			case Expose: /* resized? get new geometry */
				GetWidthHeight(&width, &height);
				break;
			case ClientMessage: /* WM_DELETE_WINDOW */
				return -1;
#if 0
			default:
				printf ("DEBUG Event: %d\n", event.type);
#endif
		}
	}
	return dur;
}
