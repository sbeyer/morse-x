/* main.h
 * (C) Stephan Beyer, 2004, 2006 GPL */
#ifndef MAIN_H
#define MAIN_H

#define TITLE	"Morse"
#define VERSION	"0.20060903"

#define DEFAULT_RATE	44100
#define DEFAULT_FREQ	441
#define DEFAULT_VOLUME	0.7
#define DEFAULT_WPM	15

int KeyPressLoop(unsigned int *);

/* global-needed options */
typedef struct {
	unsigned checkwpm : 1;  /* -c */
	unsigned showgfx : 1;	/* -g */
	unsigned show : 1;	/* -s */
	unsigned mute : 1;      /* -m */
	int rate;               /* -r rate */
	int freq;               /* -f freq */
	float vol;              /* -v vol */
	int wpm;                /* -w wpm */
} Opt;
Opt opt;

void paint(char *);

#endif
