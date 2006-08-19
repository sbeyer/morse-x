/* audio.c
******************************************************************************
   This file is part of morse-x!
   Author: Stephan Beyer
   
   Description of this file:
       audio functions, using libSDL

   Copyright (C) GPL, 2006 Stephan Beyer - s-beyer@gmx.net
  
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

#include <string.h> /* memcpy() */
#include <SDL.h>
#include <math.h> /* M_PI, sinf */

#include "main.h" /* options */
#include "errors.h"
#include "audio.h"

#ifndef _GNU_SOURCE
# define mempcpy(A,B,C)	((Uint8*)memcpy(A,B,C)+C)
#endif

/* types */
typedef struct _sndData
{

	void *pstream; /* prepared stream data */
	Uint32 len; /* len of prepared stream data */
	Uint32 pos; /* last position in stream */
} sndData;

/* globals */
int play; /* boolean if playing or not */

/* sound callback */
void
sndcb(void *userdata, Uint8 *stream, int len)
{
	sndData *base = userdata;
	Uint8 *pstream = base->pstream;
	Uint32 plen = base->len - base->pos;

	pstream += base->pos;

	if(play)
		/* play tone */
		while(len > 0)
		{
			if(len > plen)
			{
				stream = mempcpy(stream,pstream,plen);
				
				len -= plen;
				base->pos = 0;
				pstream = base->pstream;
				plen = base->len;
			} else {
				stream = mempcpy(stream,pstream,len);

				base->pos += len;
				len = 0;

			}
		}
	else
		/* generate silence without snapping */
		if(base->pos)
		{
			if(len > plen)
			{
				stream = mempcpy(stream,pstream,plen);
				base->pos = 0;
			} else {
				stream = mempcpy(stream,pstream,len);
				base->pos += len;
			}
		}
}

SDL_AudioSpec *
specNew(int srate, Uint8 channels, void *userdata)
{
	SDL_AudioSpec *ret;
	ret = malloc(sizeof(SDL_AudioSpec));

	ret->freq = srate;
	ret->format = AUDIO_S16SYS;
	ret->channels = channels;
	ret->silence = 0; /* I really don't know what this is here ;) */
	/* the following seemed to be good values: */
	ret->samples = (srate > 80000 ? 2048 : (srate > 40000 ? 1024 :
			(srate > 20000 ? 512 : (srate > 10000 ? 256 :
			(srate > 5000 ? 128 : 64)))));
	ret->size = ret->samples*channels*2;
	ret->callback = sndcb;
	ret->userdata = userdata;

	return ret;
}

/* opens the audio device with given options */
SDL_AudioSpec *
openAudio(int srate, Uint8 channels, void *userdata)
{
	SDL_AudioSpec	*obtained = malloc(sizeof(SDL_AudioSpec)),
			*desired = specNew(srate,channels,userdata);
	DoAndErr(SDL_OpenAudio(desired,obtained), SDL_OpenAudio, errSDLaudio);
	if(!obtained)
		return desired;
	free(desired);
	return obtained;
}

/* creates a wave stream and write base->pstream and base->len */
void
createWave(sndData *base, SDL_AudioSpec *spec)
{
	Uint32 samples = spec->freq / opt.freq;
	Uint32 len = samples * spec->channels;
	Uint32 i;
	Uint16 amp = 32767;
	Uint8 *stream;

	printf("Morse sound frequency set to %d Hz.\n", spec->freq / samples);

	if(spec->format == AUDIO_U8 || spec->format == AUDIO_S8)
		amp >>= 8;
	else
		len *= 2;
	/* this works as long as 32 bit audio format is not supported by SDL */

	stream = malloc(len);
	base->pstream = stream;
	for(i = 0; i < samples; i++)
	{
		Uint8 c = 0;
		float x = amp*(opt.vol*sinf(2*i*M_PI/samples)+1);
		for(c = 0; c < spec->channels; c++)
		{
			Sint32 t = x;
			switch(spec->format)
			{
			case AUDIO_U8:
				*stream = (Uint8)x;
				break;
			case AUDIO_S8:
				*stream = (Sint8)x - amp;
				break;
			case AUDIO_S16LSB:
				t -= amp;
			case AUDIO_U16LSB:
				*stream = (Uint8)t;
				stream++;
				*stream = (Uint8)(t>>8);
				break;
			case AUDIO_S16MSB:
				t -= amp;
			case AUDIO_U16MSB:
				*stream = (Uint8)(t>>8);
				stream++;
				*stream = (Uint8)t;
				break;
			default:
				ErrorMsg(errSDLaudio,"Audio format not supported!");
			}
			stream++;
		}
	}
	base->len = len;
}

void
initAudio(int srate, int channels)
{
	sndData *base = malloc(sizeof(sndData));
	SDL_AudioSpec *sndspec = openAudio(srate,channels,base);
	/* note: we'll never free() this manually */
	printf("Opened audio device using\n\t%i Hz sample rate,\n\t%d channel%s,\n\t%s format.\n",
	       sndspec->freq, sndspec->channels, (sndspec->channels==2?"s":""),
	       (sndspec->format==AUDIO_S16LSB ? "signed 16-bit little-endian" : 
	       (sndspec->format==AUDIO_S16MSB ? "signed 16-bit big-endian" :
	       (sndspec->format==AUDIO_U16LSB ? "unsigned 16-bit little-endian" : 
	       (sndspec->format==AUDIO_U16MSB ? "unsigned 16-bit big-endian" : 
	       (sndspec->format==AUDIO_U8 ? "unsigned 8-bit" : 
	       (sndspec->format==AUDIO_S8 ? "signed 8-bit" : "unknown")))))));

	play = 0;
	base->pos = 0;
	createWave(base,sndspec);
	puts("");
}

void
playAudio(int yes)
{
	play = yes;
}
