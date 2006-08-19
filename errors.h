/* errors.h
 * (C) Stephan Beyer, 2003-2006 GPL */
#ifndef ERRORS_H
#define ERRORS_H

#define ERROCCMSG	"an error occured"
#define DoAndErr(COND,STR,ERR)	if (COND) HandleError(#STR, __FILE__, __LINE__, ERR);

enum errorcodes
{
	errNO = 0,
	errMEMORY,
	errARGS,
	errFILE,
	errSDL,
	errSDLvideo,
	errSDLaudio,
	errUNDEFINED
};

void ErrorMsg(signed int nr, const char *msg);
void HandleError(const char *def, char *file, unsigned int line, signed int nr);

#endif
