/* options.h
 * (C) Stephan Beyer, 2004, GPL */
#ifndef OPTIONS_H
#define OPTIONS_H

/* global-needed options */
typedef struct {
	unsigned calibrate : 1;	 /* -c */
	unsigned show : 1;	 /* -s */
} Opt;
Opt opt;

/* prototypes */
void HandleOptions(int, char **);

/* default values */
#define DEFAULT_DISPLAY	":0"
#define DEFAULT_WIDTH	50
#define DEFAULT_HEIGHT	50
#define DEFAULT_MINWIDTH	10
#define DEFAULT_MINHEIGHT	10

/* macros */
#define MAX(x,y)	((x > y) ? x : y)

#endif
