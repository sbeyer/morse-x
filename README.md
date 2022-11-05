# morse-x

*morse-x* is a morse practicing tool.

```
morse-x(1)                         hamradio                         morse-x(1)

NAME
       morse-x - morse practicing tool

SYNTAX
       morse-x [options]

DESCRIPTION
       In morse-x you can use any key on your keyboard as morse key (except q,
       which exits) and then it displays your morsed stuff as readable text on
       stdout. (You should execute morse-x in an X terminal.)

OPTIONS
       Of  course,  morse-x  takes  some options, some with arguments. See the
       following list. A preceding dash (-) is optional.

       c      Tries to find out your number of words per minute. (In fact,  it
              is like the calibration function in former versions, except that
              it does not save the result, it just tells you your  the  number
              of your words per minute.)

       f frequency
              Set sound frequency.

       g      Show dits as red dots and daws as red lines in the main window.

       h      Print usage information, the same you can read here. Hopefully.

       m      Mute audio. No audio device will be opened.

       r samplerate
              Set audio sample rate.

       s      Show dits as dots and daws as dashes on stdout.

       t      Show  used morse code table and exit. If you have suggestions on
              that table, please let me know.

       v volume
              Set sound volume. This must be a value between 0 and 1.

       w wpm  Set words per minute. This must be an integer between 1 and 60.

       Note that older versions (2004*) were written  using  Xlib  instead  of
       libSDL  and  thus  had  some other options and no audio output.  By the
       way, the .morse-x file of earlier versions is no longer needed. You may
       remove it.

BUGS
       None so far. Please report some.

COPYRIGHT
       Copyright © 2004, 2006 Stephan Beyer <s-beyer@gmx.net>

       This  is  free  software.   You may redistribute copies of it under the
       terms  of  the  GNU  General  Public  License   <http://www.gnu.org/li‐
       censes/gpl.html>.   There  is  NO  WARRANTY, to the extent permitted by
       law.

Stephan Beyer                     0.20060903                        morse-x(1)
```

Requirements:
  * C compiler
  * SDL 1.2

Compilation:
  * `make`
