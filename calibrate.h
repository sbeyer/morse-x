/* calibrate.h
 * (C) Stephan Beyer, 2004, GPL */
#ifndef CALIBRATE_H
#define CALIBRATE_H

#define RC_FILE ".morse-x"

int Calibrate(void);
void WriteCalibrateFile(int);
int ReadCalibrateFile(void);

#endif
