/* xcommon.h
 * (C) Stephan Beyer, 2004, GPL */
#ifndef XCOMMON_H
#define XCOMMON_H

#include <X11/Xlib.h>
#include <X11/X.h>

/* X globals */
Display *display; /* the display itself */
Window window; /* the window itself */
GC gc; /* a graphic context */

/* hints */
XSizeHints *hints; /* size hints for the window */
XWMHints *wmhints; /* window manager hints */
XClassHint *classhint; /* resource name and class of the window */

#endif
