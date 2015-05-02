#ifndef _INTEGER

/* modified by Martin Thomas to avoid redefiniton
   for BYTE, WORD and DWORD */

/* These types are assumed as 16-bit or larger integer */
typedef signed int		INT;
typedef unsigned int	UINT;

/* These types are assumed as 8-bit integer */
typedef signed char		CHAR;
typedef unsigned char	UCHAR;
#ifndef BYTE_TYPEDEF
#define BYTE_TYPEDEFED
typedef unsigned char	BYTE;
#endif

/* These types are assumed as 16-bit integer */
typedef signed short	SHORT;
typedef unsigned short	USHORT;
#ifndef WORD_TYPEDEF
#define WORD_TYPEDEF
typedef unsigned short	WORD;
#endif

/* These types are assumed as 32-bit integer */
typedef signed long		LONG;
typedef unsigned long	ULONG;
#ifndef DWORD_TYPEDEF
#define DWORD_TYPEDEF
typedef unsigned long	DWORD;
#endif

#if 1
// mthomas
#ifndef FALSE
typedef enum { FALSE = 0, TRUE } BOOL;
#endif
#else
/* Boolean type */
typedef enum { FALSE = 0, TRUE } BOOL;
#endif

#define _INTEGER
#endif
