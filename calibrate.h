/* calibrate.h
 * (C) Stephan Beyer, 2004, GPL */
#ifndef CALIBRATE_H
#define CALIBRATE_H

#define RC_FILE ".morse-x"

int Calibrate(Display *, Window, GC);
int CalibrateLoop(Display *, Window, GC);
int ReadCalibrateFile(void);
void WriteCalibrateFile(int);

#endif
