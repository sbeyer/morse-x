/* main.h
 * (C) Stephan Beyer, 2004, GPL */
#ifndef MAIN_H
#define MAIN_H

#define TITLE	"Morse"
#define VERSION	"20040414"

/* X11 icon (wmhints) */
#include "icons/morse4.xpm"

#define DEFAULT_DISPLAY	":0"
#define DEFAULT_WIDTH	50
#define DEFAULT_HEIGHT	50
#define DEFAULT_MINWIDTH	10
#define DEFAULT_MINHEIGHT	10

/* the next value defines the max X duration of a dot (.) */
/*#define DURATIONUNIT 75*/ /* deprecated */

/* global-needed options */
typedef struct {
	unsigned calibrate : 1;	 /* -c */
	unsigned show : 1;	 /* -s */
	unsigned firststart : 1; /* not an argument, true if first start */
} Opt;

/* macros */
#define MAX(x,y)	((x > y) ? x : y)

#endif
