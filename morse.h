/* morse.h
 * (C) Stephan Beyer, 2004, GPL */
#ifndef MORSE_H
#define MORSE_H

/* the morse code */
/* How I did it:
 *	I use an array as a binary tree, that means, if the array index
 *	i is the root, so the index 2*i is the left, and 2*i + 1 is the
 *	right knot. I decided that 'dit' ('.') is accessable with 2*i, and
 *	'daw' ('-') is 2*i+1, in general.
 *	Because array indeces begin with 0 in C, and because one root
 *	element is missing, you have to substract 2 for the right index, 
 *	when - and ONLY when - accessing.
 *	Summarized in pseudo code:
 *		i = 1;
 *		...
 *		if DIT then
 *			i = 2*i;
 *			... -> morse[i-2];
 *		if DAW then
 *			i = 2*i+1;
 *			... -> morse[i-2]; */
const char* morse[] = { /* this tree contains ISO-8859-1 characters */
	/* . */ "E",
	/* - */ "T",
	/* .. */ "I",
	/* .- */ "A",
	/* -. */ "N",
	/* -- */ "M",
	/* ... */ "S",
	/* ..- */ "U",
	/* .-. */ "R",
	/* .-- */ "W",
	/* -.. */ "D",
	/* -.- */ "K",
	/* --. */ "G",
	/* --- */ "O",
	/* .... */ "H",
	/* ...- */ "V",
	/* ..-. */ "F",
	/* ..-- */ "Ь",
	/* .-.. */ "L",
	/* .-.- */ "Д",
	/* .--. */ "P",
	/* .--- */ "J",
	/* -... */ "B",
	/* -..- */ "X",
	/* -.-. */ "C",
	/* -.-- */ "Y",
	/* --.. */ "Z",
	/* --.- */ "Q",
	/* ---. */ "Ц",
	/* ---- */ "CH",
	/* ..... */ "5",
	/* ....- */ "4",
	/* ...-. */ " SN ",
	/* ...-- */ "3",
	/* ..-.. */ "Й",
	/* ..-.- */ NULL,
	/* ..--. */ "!",
	/* ..--- */ "2",
	/* .-... */ " AS ",
	/* .-..- */ NULL,
	/* .-.-. */ " AR ", /* often "+" */
	/* .-.-- */ NULL,
	/* .--.. */ NULL,
	/* .--.- */ "(бе)",
	/* .---. */ NULL,
	/* .---- */ "1",
	/* -.... */ "6",
	/* -...- */ " BT ", /* separator, often "=" */
	/* -..-. */ "/", /* (fraction bar) */
	/* -..-- */ NULL,
	/* -.-.. */ NULL,
	/* -.-.- */ " KA ",
	/* -.--. */ "(",
	/* -.--- */ NULL,
	/* --... */ "7",
	/* --..- */ NULL,
	/* --.-. */ NULL,
	/* --.-- */ "С",
	/* ---.. */ "8",
	/* ---.- */ NULL,
	/* ----. */ "9",
	/* ----- */ "0",
	/* ...... */ NULL,
	/* .....- */ NULL,
	/* ....-. */ NULL,
	/* ....-- */ NULL,
	/* ...-.. */ NULL,
	/* ...-.- */ " SK ",
	/* ...--. */ NULL,
	/* ...--- */ NULL,
	/* ..-... */ NULL,
	/* ..-..- */ NULL,
	/* ..-.-. */ NULL,
	/* ..-.-- */ NULL,
	/* ..--.. */ "?",
	/* ..--.- */ NULL,
	/* ..---. */ NULL,
	/* ..---- */ NULL,
	/* .-.... */ NULL,
	/* .-...- */ NULL,
	/* .-..-. */ "\"",
	/* .-..-- */ NULL,
	/* .-.-.. */ NULL,
	/* .-.-.- */ ".",
	/* .-.--. */ NULL,
	/* .-.--- */ NULL,
	/* .--... */ NULL,
	/* .--..- */ NULL,
	/* .--.-. */ "@",
	/* .--.-- */ NULL,
	/* .---.. */ NULL,
	/* .---.- */ NULL,
	/* .----. */ "\\",
	/* .----- */ NULL,
	/* -..... */ NULL,
	/* -....- */ "-",
	/* -...-. */ NULL,
	/* -...-- */ NULL,
	/* -..-.. */ NULL,
	/* -..-.- */ NULL,
	/* -..--. */ NULL,
	/* -..--- */ NULL,
	/* -.-... */ NULL,
	/* -.-..- */ NULL,
	/* -.-.-. */ NULL,
	/* -.-.-- */ NULL,
	/* -.--.. */ NULL,
	/* -.--.- */ ")",
	/* -.---. */ NULL,
	/* -.---- */ NULL,
	/* --.... */ NULL,
	/* --...- */ NULL,
	/* --..-. */ NULL,
	/* --..-- */ ",",
	/* --.-.. */ NULL,
	/* --.-.- */ NULL,
	/* --.--. */ NULL,
	/* --.--- */ NULL,
	/* ---... */ ":",
	/* ---..- */ NULL,
	/* ---.-. */ NULL,
	/* ---.-- */ NULL,
	/* ----.. */ NULL,
	/* ----.- */ NULL,
	/* -----. */ NULL,
	/* ------ */ NULL
};
#if 0
	/* usw...: */
	/* ....... */ " ERROR ",
	/* ...--.. */ "Я",
	/* ...---... */ " SOS "
#endif

#endif
