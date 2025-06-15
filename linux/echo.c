#include "stdlib.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

#define WANT_HEX_ESCAPES 1
# define ALIGN1 __attribute__((aligned(1)))
# define ALIGN2 __attribute__((aligned(2)))
# define ALIGN4 __attribute__((aligned(4)))
/* Usual "this only works for ascii compatible encodings" disclaimer. */
#undef _tolower
#define _tolower(X) ((X)|((char) 0x20))

char bb_process_escape_sequence(const char **ptr)
{
	const char *q;
	unsigned num_digits;
	unsigned n;
	unsigned base;

	num_digits = n = 0;
	base = 8;
	q = *ptr;

	if (WANT_HEX_ESCAPES && *q == 'x') {
		++q;
		base = 16;
		++num_digits;
	}

	/* bash requires leading 0 in octal escapes:
	 * \02 works, \2 does not (prints \ and 2).
	 * We treat \2 as a valid octal escape sequence. */
	do {
		unsigned r;
		unsigned d = (unsigned char)(*q) - '0';
#if WANT_HEX_ESCAPES
		if (d >= 10) {
			d = (unsigned char)_tolower(*q) - 'a';
			//d += 10;
			/* The above would map 'A'-'F' and 'a'-'f' to 10-15,
			 * however, some chars like '@' would map to 9 < base.
			 * Do not allow that, map invalid chars to N > base:
			 */
			if ((int)d >= 0)
				d += 10;
		}
#endif
		if (d >= base) {
			if (WANT_HEX_ESCAPES && base == 16) {
				--num_digits;
				if (num_digits == 0) {
					/* \x<bad_char>: return '\',
					 * leave ptr pointing to x */
					return '\\';
				}
			}
			break;
		}

		r = n * base + d;
		if (r > UCHAR_MAX) {
			break;
		}

		n = r;
		++q;
	} while (++num_digits < 3);

	if (num_digits == 0) {
		/* Not octal or hex escape sequence.
		 * Is it one-letter one? */

		/* bash builtin "echo -e '\ec'" interprets \e as ESC,
		 * but coreutils "/bin/echo -e '\ec'" does not.
		 * Manpages tend to support coreutils way.
		 * Update: coreutils added support for \e on 28 Oct 2009. */
		static const char charmap[] ALIGN1 = {
			'a',  'b', 'e', 'f',  'n',  'r',  't',  'v',  '\\', '\0',
			'\a', '\b', 27, '\f', '\n', '\r', '\t', '\v', '\\', '\\',
		};
		const char *p = charmap;
		do {
			if (*p == *q) {
				q++;
				break;
			}
		} while (*++p != '\0');
		/* p points to found escape char or NUL,
		 * advance it and find what it translates to.
		 * Note that \NUL and unrecognized sequence \z return '\'
		 * and leave ptr pointing to NUL or z. */
		n = p[sizeof(charmap) / 2];
	}

	*ptr = q;

	return (char) n;
}


int main(int argc, char **argv)
{
	char **pp;
	const char *arg;
	char *out;
	char *buffer;
	unsigned buflen;
	int err;

	char nflag = 1;
	char eflag = 0;

	while ((arg = *++argv) != NULL) {
		char n, e;

		if (arg[0] != '-')
			break; /* not an option arg, echo it */

		/* If it appears that we are handling options, then make sure
		 * that all of the options specified are actually valid.
		 * Otherwise, the string should just be echoed.
		 */
		arg++;
		n = nflag;
		e = eflag;
		do {
			if (*arg == 'n')
				n = 0;
			else if (*arg == 'e')
				e = '\\';
			else if (*arg != 'E') {
				/* "-ccc" arg with one of c's invalid, echo it */
				/* arg consisting from just "-" also handled here */
				goto just_echo;
			}
		} while (*++arg);
		nflag = n;
		eflag = e;
	}
 just_echo:

	buflen = 0;
	pp = argv;
	while ((arg = *pp) != NULL) {
		buflen += strlen(arg) + 1;
		pp++;
	}
	out = buffer = malloc(buflen + 1); /* +1 is needed for "no args" case */

	while ((arg = *argv) != NULL) {
		int c;

		if (!eflag) {
			/* optimization for very common case */
			out = stpcpy(out, arg);
		} else
		while ((c = *arg++) != '\0') {
			if (c == eflag) {
				/* This is an "\x" sequence */

				if (*arg == 'c') {
					/* "\c" means cancel newline and
					 * ignore all subsequent chars. */
					goto do_write;
				}
				/* Since SUSv3 mandates a first digit of 0, 4-digit octals
				* of the form \0### are accepted. */
				if (*arg == '0') {
					if ((unsigned char)(arg[1] - '0') < 8) {
						/* 2nd char is 0..7: skip leading '0' */
						arg++;
					}
				}
				/* bb_process_escape_sequence handles NUL correctly
				 * ("...\" case). */
				{
					/* optimization: don't force arg to be on-stack,
					 * use another variable for that. ~30 bytes win */
					const char *z = arg;
					c = bb_process_escape_sequence(&z);
					arg = z;
				}
			}
			*out++ = c;
		}

		if (!*++argv)
			break;
		*out++ = ' ';
	}

	if (nflag) {
		*out++ = '\n';
	}

 do_write:
	/* Careful to error out on partial writes too (think ENOSPC!) */
	errno = 0;
	err = write(STDOUT_FILENO, buffer, out - buffer) != out - buffer;
	if (err) {
		// bb_simple_perror_msg(bb_msg_write_error);
	}
	free(buffer);
	return err;
}