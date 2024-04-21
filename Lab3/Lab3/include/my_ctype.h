/* Copyright (C) 1991-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */
/*
 *	ISO C99 Standard 7.4: Character handling	<ctype.h>
 */
#ifndef _CTYPE_H
#define _CTYPE_H 1
#include <bits/types.h>
#include <features.h>
__BEGIN_DECLS
#ifndef _ISbit
/* These are all the characteristics of characters.
   If there get to be more than 16 distinct characteristics,
   many things must be changed that use `unsigned short int's.
   The characteristics are stored always in network byte order (big
   endian).  We define the bit value interpretations here dependent on the
   machine's byte order.  */
#include <bits/endian.h>
#if __BYTE_ORDER == __BIG_ENDIAN
#define _ISbit(bit) (1 << (bit))
#else /* __BYTE_ORDER == __LITTLE_ENDIAN */
#define _ISbit(bit) ((bit) < 8 ? ((1 << (bit)) << 8) : ((1 << (bit)) >> 8))
#endif
enum {
  _ISupper = _ISbit(0),  /* UPPERCASE.  */
  _ISlower = _ISbit(1),  /* lowercase.  */
  _ISalpha = _ISbit(2),  /* Alphabetic.  */
  _ISdigit = _ISbit(3),  /* Numeric.  */
  _ISxdigit = _ISbit(4), /* Hexadecimal numeric.  */
  _ISspace = _ISbit(5),  /* Whitespace.  */
  _ISprint = _ISbit(6),  /* Printing.  */
  _ISgraph = _ISbit(7),  /* Graphical.  */
  _ISblank = _ISbit(8),  /* Blank (usually SPC and TAB).  */
  _IScntrl = _ISbit(9),  /* Control character.  */
  _ISpunct = _ISbit(10), /* Punctuation.  */
  _ISalnum = _ISbit(11)  /* Alphanumeric.  */
};
#endif /* ! _ISbit  */
/* These are defined in ctype-info.c.
   The declarations here must match those in localeinfo.h.
   In the thread-specific locale model (see `uselocale' in <locale.h>)
   we cannot use global variables for these as was done in the past.
   Instead, the following accessor functions return the address of
   each variable, which is local to the current thread if multithreaded.
   These point into arrays of 384, so they can be indexed by any `unsigned
   char' value [0,255]; by EOF (-1); or by any `signed char' value
   [-128,-1).  ISO C requires that the ctype functions work for `unsigned
   char' values and for EOF; we also support negative `signed char' values
   for broken old programs.  The case conversion arrays are of `int's
   rather than `unsigned char's because tolower (EOF) must be EOF, which
   doesn't fit into an `unsigned char'.  But today more important is that
   the arrays are also used for multi-byte character sets.  */
extern const unsigned short int **__ctype_b_loc(void) __THROW
    __attribute__((__const__));
extern const __int32_t **__ctype_tolower_loc(void) __THROW
    __attribute__((__const__));
extern const __int32_t **__ctype_toupper_loc(void) __THROW
    __attribute__((__const__));
#ifndef __cplusplus
#define __isctype(c, type)                                                     \
  ((*__ctype_b_loc())[(int)(c)] & (unsigned short int)type)
#elif defined __USE_EXTERN_INLINES
#define __isctype_f(type)                                                      \
  __extern_inline int is##type(int __c) __THROW {                              \
    return (*__ctype_b_loc())[(int)(__c)] & (unsigned short int)_IS##type;     \
  }
#endif
#define __isascii(c) (((c) & ~0x7f) == 0) /* If C is a 7 bit value.  */
#define __toascii(c) ((c) & 0x7f)         /* Mask off high bits.  */
#define __exctype(name) extern int name(int) __THROW
/* The following names are all functions:
     int isCHARACTERISTIC(int c);
   which return nonzero iff C has CHARACTERISTIC.
   For the meaning of the characteristic names, see the `enum' above.  */
__exctype(isalnum);
__exctype(isalpha);
__exctype(iscntrl);
__exctype(isdigit);
__exctype(islower);
__exctype(isgraph);
__exctype(isprint);
__exctype(ispunct);
__exctype(isspace);
__exctype(isupper);
__exctype(isxdigit);
/* Return the lowercase version of C.  */
extern int tolower(int __c) __THROW;
/* Return the uppercase version of C.  */
extern int toupper(int __c) __THROW;
/* ISO C99 introduced one new function.  */
#ifdef __USE_ISOC99
__exctype(isblank);
#endif
#ifdef __USE_GNU
/* Test C for a set of character classes according to MASK.  */
extern int isctype(int __c, int __mask) __THROW;
#endif
#if defined __USE_MISC || defined __USE_XOPEN
/* Return nonzero iff C is in the ASCII set
   (i.e., is no more than 7 bits wide).  */
extern int isascii(int __c) __THROW;
/* Return the part of C that is in the ASCII set
   (i.e., the low-order 7 bits of C).  */
extern int toascii(int __c) __THROW;
/* These are the same as `toupper' and `tolower' except that they do not
   check the argument for being in the range of a `char'.  */
__exctype(_toupper);
__exctype(_tolower);
#endif /* Use X/Open or use misc.  */
/* This code is needed for the optimized mapping functions.  */
#define __tobody(c, f, a, args)                                                \
  (__extension__({                                                             \
    int __res;                                                                 \
    if (sizeof(c) > 1) {                                                       \
      if (__builtin_constant_p(c)) {                                           \
        int __c = (c);                                                         \
        __res = __c < -128 || __c > 255 ? __c : (a)[__c];                      \
      } else                                                                   \
        __res = f args;                                                        \
    } else                                                                     \
      __res = (a)[(int)(c)];                                                   \
    __res;                                                                     \
  }))
#if !defined __NO_CTYPE
#ifdef __isctype_f
__isctype_f(alnum) __isctype_f(alpha) __isctype_f(cntrl) __isctype_f(digit)
    __isctype_f(lower) __isctype_f(graph) __isctype_f(print) __isctype_f(punct)
        __isctype_f(space) __isctype_f(upper) __isctype_f(xdigit)
#ifdef __USE_ISOC99
            __isctype_f(blank)
#endif
#elif defined __isctype
#define isalnum(c) __isctype((c), _ISalnum)
#define isalpha(c) __isctype((c), _ISalpha)
#define iscntrl(c) __isctype((c), _IScntrl)
#define isdigit(c) __isctype((c), _ISdigit)
#define islower(c) __isctype((c), _ISlower)
#define isgraph(c) __isctype((c), _ISgraph)
#define isprint(c) __isctype((c), _ISprint)
#define ispunct(c) __isctype((c), _ISpunct)
#define isspace(c) __isctype((c), _ISspace)
#define isupper(c) __isctype((c), _ISupper)
#define isxdigit(c) __isctype((c), _ISxdigit)
#ifdef __USE_ISOC99
#define isblank(c) __isctype((c), _ISblank)
#endif
#endif
#ifdef __USE_EXTERN_INLINES
                __extern_inline int __NTH(tolower(int __c)) {
  return __c >= -128 && __c < 256 ? (*__ctype_tolower_loc())[__c] : __c;
}
__extern_inline int __NTH(toupper(int __c)) {
  return __c >= -128 && __c < 256 ? (*__ctype_toupper_loc())[__c] : __c;
}
#endif
#if __GNUC__ >= 2 && defined __OPTIMIZE__ && !defined __cplusplus
#define tolower(c) __tobody(c, tolower, *__ctype_tolower_loc(), (c))
#define toupper(c) __tobody(c, toupper, *__ctype_toupper_loc(), (c))
#endif /* Optimizing gcc */
#if defined __USE_MISC || defined __USE_XOPEN
#define isascii(c) __isascii(c)
#define toascii(c) __toascii(c)
#define _tolower(c) ((int)(*__ctype_tolower_loc())[(int)(c)])
#define _toupper(c) ((int)(*__ctype_toupper_loc())[(int)(c)])
#endif
#endif /* Not __NO_CTYPE.  */
#ifdef __USE_XOPEN2K8
/* POSIX.1-2008 extended locale interface (see locale.h).  */
#include <bits/types/locale_t.h>
/* These definitions are similar to the ones above but all functions
   take as an argument a handle for the locale which shall be used.  */
#define __isctype_l(c, type, locale)                                           \
  ((locale)->__ctype_b[(int)(c)] & (unsigned short int)type)
#define __exctype_l(name)                                                      \
  extern int name(int, locale_t)                                               \
  __THROW
/* The following names are all functions:
     int isCHARACTERISTIC(int c, locale_t *locale);
   which return nonzero iff C has CHARACTERISTIC.
   For the meaning of the characteristic names, see the `enum' above.  */
__exctype_l(isalnum_l);
__exctype_l(isalpha_l);
__exctype_l(iscntrl_l);
__exctype_l(isdigit_l);
__exctype_l(islower_l);
__exctype_l(isgraph_l);
__exctype_l(isprint_l);
__exctype_l(ispunct_l);
__exctype_l(isspace_l);
__exctype_l(isupper_l);
__exctype_l(isxdigit_l);
__exctype_l(isblank_l);

short unsigned int ctype_b_values[] = {
    0x0002, //
    0x0002, //
    0x0002, //
    0x0002, //
    0x0002, //
    0x0002, //
    0x0002, //
    0x0002, //
    0x0002, //
    0x2003, //
    0x2002, //
    0x2002, //
    0x2002, //
    0x2002, //
    0x0002, // 
    0x0002, // 
    0x0002, // 
    0x0002, //
    0x0002, //
    0x0002, // 
    0x0002, //
    0x0002, //
    0x0002, //
    0x0002, //
    0x0002, 0x0002, 0x0002,
    0x0002, //
    0x0002, //
    0x0002, //
    0x0002, //
    0x0002, //
    0x6001, //
    0xc004, //!
    0xc004, //"
    0xc004, // #
    0xc004, //$
    0xc004, //%
    0xc004, //&
    0xc004, //'
    0xc004, //(
    0xc004, //)
    0xc004, //*
    0xc004, //+
    0xc004, //,
    0xc004, //-
    0xc004, //.
    0xc004, ///
    0xd808, // 0
    0xd808, // 1
    0xd808, // 2
    0xd808, // 3
    0xd808, // 4
    0xd808, // 5
    0xd808, // 6
    0xd808, // 7
    0xd808, // 8
    0xd808, // 9
    0xc004, //:
    0xc004, //;
    0xc004, //<
    0xc004, //=
    0xc004, //>
    0xc004, //?
    0xc004, //@
    0xd508, // A
    0xd508, // B
    0xd508, // C
    0xd508, // D
    0xd508, // E
    0xd508, // F
    0xc508, // G
    0xc508, // H
    0xc508, // I
    0xc508, // J
    0xc508, // K
    0xc508, // L
    0xc508, // M
    0xc508, // N
    0xc508, // O
    0xc508, // P
    0xc508, // Q
    0xc508, // R
    0xc508, // S
    0xc508, // T
    0xc508, // U
    0xc508, // V
    0xc508, // W
    0xc508, // X
    0xc508, // Y
    0xc508, // Z
    0xc004, //[
    0xc004, //
    0xc004, //]
    0xc004, //^
    0xc004, //_
    0xc004, //`
    0xd608, // a
    0xd608, // b
    0xd608, // c
    0xd608, // d
    0xd608, // e
    0xd608, // f
    0xc608, // g
    0xc608, // h
    0xc608, // i
    0xc608, // j
    0xc608, // k
    0xc608, // l
    0xc608, // m
    0xc608, // n
    0xc608, // o
    0xc608, // p
    0xc608, // q
    0xc608, // r
    0xc608, // s
    0xc608, // t
    0xc608, // u
    0xc608, // v
    0xc608, // w
    0xc608, // x
    0xc608, // y
    0xc608, // z
    0xc004, //{
    0xc004, //|
    0xc004, //}
    0xc004, //~
    0x0002, //
    0x0000, // €
    0x0000, //
    0x0000, // ‚
    0x0000, // ƒ
    0x0000, // „
    0x0000, // …
    0x0000, // †
    0x0000, // ‡
    0x0000, // ˆ
    0x0000, // ‰
    0x0000, // Š
    0x0000, // ‹
    0x0000, // Œ
    0x0000, //
    0x0000, // Ž
    0x0000, //
    0x0000, //
    0x0000, // ‘
    0x0000, // ’
    0x0000, // “
    0x0000, // ”
    0x0000, // •
    0x0000, // –
    0x0000, // —
    0x0000, // ˜
    0x0000, // ™
    0x0000, // š
    0x0000, // ›
    0x0000, // œ
    0x0000, //
    0x0000, // ž
    0x0000, // Ÿ
    0x0000, //
    0x0000, // ¡
    0x0000, // ¢
    0x0000, // £
    0x0000, // ¤
    0x0000, // ¥
    0x0000, // ¦
    0x0000, // §
    0x0000, // ¨
    0x0000, // ©
    0x0000, // ª
    0x0000, // «
    0x0000, // ¬
    0x0000, // ­
    0x0000, // ®
    0x0000, // ¯
    0x0000, // °
    0x0000, // ±
    0x0000, // ²
    0x0000, // ³
    0x0000, // ´
    0x0000, // µ
    0x0000, // ¶
    0x0000, // ·
    0x0000, // ¸
    0x0000, // ¹
    0x0000, // º
    0x0000, // »
    0x0000, // ¼
    0x0000, // ½
    0x0000, // ¾
    0x0000, // ¿
    0x0000, // À
    0x0000, // Á
    0x0000, // Â
    0x0000, // Ã
    0x0000, // Ä
    0x0000, // Å
    0x0000, // Æ
    0x0000, // Ç
    0x0000, // È
    0x0000, // É
    0x0000, // Ê
    0x0000, // Ë
    0x0000, // Ì
    0x0000, // Í
    0x0000, // Î
    0x0000, // Ï
    0x0000, // Ð
    0x0000, // Ñ
    0x0000, // Ò
    0x0000, // Ó
    0x0000, // Ô
    0x0000, // Õ
    0x0000, // Ö
    0x0000, // ×
    0x0000, // Ø
    0x0000, // Ù
    0x0000, // Ú
    0x0000, // Û
    0x0000, // Ü
    0x0000, // Ý
    0x0000, // Þ
    0x0000, // ß
    0x0000, // à
    0x0000, // á
    0x0000, // â
    0x0000, // ã
    0x0000, // ä
    0x0000, // å
    0x0000, // æ
    0x0000, // ç
    0x0000, // è
    0x0000, // é
    0x0000, // ê
    0x0000, // ë
    0x0000, // ì
    0x0000, // í
    0x0000, // î
    0x0000, // ï
    0x0000, // ð
    0x0000, // ñ
    0x0000, // ò
    0x0000, // ó
    0x0000, // ô
    0x0000, // õ
    0x0000, // ö
    0x0000, // ÷
    0x0000, // ø
    0x0000, // ù
    0x0000, // ú
    0x0000, // û
    0x0000, // ü
    0x0000, // ý
    0x0000, // þ
    0x0000, // ÿ
    0x0020, //
    0x0000, //
    0x0000, //
    0x0000, //
    0x0000, //
    0x0000, //
    0x0028, //
    0x0000, //
    0x0043, //
    0x0000, //
    0x0029, //
    0x0000, //
    0x0000, //
    0x0000,
    0x0000, // 
    0x0000, // 
    0x003c, // 
    0x0000, //
    0x003c, //
    0x0000, // 
    0x0000, //
    0x0000, //
    0x0000, //
    0x0000, 0x002d, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, //
    0x0028, //
    0x0000, //
    0x0052, //
    0x0000, //!
    0x0029, //"
    0x0000, // #
    0x0000, //$
    0x0000, //%
    0x0000, //&
    0x0000, //'
    0x0075, //(
    0x0000, //)
    0x0000, //*
    0x0000, //+
    0x0000, //,
    0x0000, //-
    0x002c, //.
    0x0000, ///
    0x0000, // 0
    0x0000, // 1
    0x0000, // 2
    0x0000, // 3
    0x003e, // 4
    0x0000, // 5
    0x003e, // 6
    0x0000, // 7
    0x0000, // 8
    0x0000, // 9
    0x0000, //:
    0x0000, //;
    0x0020, //<
    0x0000, //=
    0x0031, //>
    0x0000, //?
    0x002f, //@
    0x0000, // A
    0x0034, // B
    0x0000, // C
    0x0020, // D
    0x0000, // E
    0x0000, // F
    0x0000, // G
    0x0000, // H
    0x0000, // I
    0x0020, // J
    0x0000, // K
    0x0031, // L
    0x0000, // M
    0x002f, // N
    0x0000, // O
    0x0032, // P
    0x0000, // Q
    0x0020, // R
    0x0000, // S
    0x0000, // T
    0x0000, // U
    0x0000, // V
    0x0000, // W
    0x0020, // X
    0x0000, // Y
    0x0033, // Z
    0x0000, //[
    0x002f,
    0x0000, //]
    0x0034, //^
    0x0000, //_
    0x0020, //`
    0x0000, // a
    0x0000, // b
    0x0000, // c
    0x0000, // d
    0x0000, // e
    0x0041, // f
    0x0000, // g
    0x0045, // h
    0x0000, // i
    0x0000, // j
    0x0000, // k
    0x0000, // l
    0x0000, // m
    0x0078, // n
    0x0000, // o
    0x0000, // p
    0x0000, // q
    0x0000, // r
    0x0000, // s
    0x0073, // t
    0x0000, // u
    0x0073, // v
    0x0000, // w
    0x0000, // x
    0x0000, // y
    0x0000, // z
    0x0000, //{
    0x0061, //|
    0x0000, //}
    0x0065, //~
    0x0000, //
};

/* Return the lowercase version of C in locale L.  */
extern int __tolower_l(int __c, locale_t __l) __THROW;
extern int tolower_l(int __c, locale_t __l) __THROW;
/* Return the uppercase version of C.  */
extern int __toupper_l(int __c, locale_t __l) __THROW;
extern int toupper_l(int __c, locale_t __l) __THROW;
#if __GNUC__ >= 2 && defined __OPTIMIZE__ && !defined __cplusplus
#define __tolower_l(c, locale)                                                 \
  __tobody(c, __tolower_l, (locale)->__ctype_tolower, (c, locale))
#define __toupper_l(c, locale)                                                 \
  __tobody(c, __toupper_l, (locale)->__ctype_toupper, (c, locale))
#define tolower_l(c, locale) __tolower_l((c), (locale))
#define toupper_l(c, locale) __toupper_l((c), (locale))
#endif /* Optimizing gcc */
#ifndef __NO_CTYPE
#define __isalnum_l(c, l) __isctype_l((c), _ISalnum, (l))
#define __isalpha_l(c, l) __isctype_l((c), _ISalpha, (l))
#define __iscntrl_l(c, l) __isctype_l((c), _IScntrl, (l))
#define __isdigit_l(c, l) __isctype_l((c), _ISdigit, (l))
#define __islower_l(c, l) __isctype_l((c), _ISlower, (l))
#define __isgraph_l(c, l) __isctype_l((c), _ISgraph, (l))
#define __isprint_l(c, l) __isctype_l((c), _ISprint, (l))
#define __ispunct_l(c, l) __isctype_l((c), _ISpunct, (l))
#define __isspace_l(c, l) __isctype_l((c), _ISspace, (l))
#define __isupper_l(c, l) __isctype_l((c), _ISupper, (l))
#define __isxdigit_l(c, l) __isctype_l((c), _ISxdigit, (l))
#define __isblank_l(c, l) __isctype_l((c), _ISblank, (l))
#ifdef __USE_MISC
#define __isascii_l(c, l) ((l), __isascii(c))
#define __toascii_l(c, l) ((l), __toascii(c))
#endif
#define isalnum_l(c, l) __isalnum_l((c), (l))
#define isalpha_l(c, l) __isalpha_l((c), (l))
#define iscntrl_l(c, l) __iscntrl_l((c), (l))
#define isdigit_l(c, l) __isdigit_l((c), (l))
#define islower_l(c, l) __islower_l((c), (l))
#define isgraph_l(c, l) __isgraph_l((c), (l))
#define isprint_l(c, l) __isprint_l((c), (l))
#define ispunct_l(c, l) __ispunct_l((c), (l))
#define isspace_l(c, l) __isspace_l((c), (l))
#define isupper_l(c, l) __isupper_l((c), (l))
#define isxdigit_l(c, l) __isxdigit_l((c), (l))
#define isblank_l(c, l) __isblank_l((c), (l))
#ifdef __USE_MISC
#define isascii_l(c, l) __isascii_l((c), (l))
#define toascii_l(c, l) __toascii_l((c), (l))
#endif
#endif /* Not __NO_CTYPE.  */
#endif /* Use POSIX 2008.  */
__END_DECLS

#define __DECONST(type, var) ((type)(uintptr_t)(const void *)(var))

#endif /* ctype.h  */