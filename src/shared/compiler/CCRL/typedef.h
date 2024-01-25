/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : typedef.h
* Version      : 1.00
* Description  : GSCE Standard Typedef Header file
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

#ifndef _TYPEDEF_H
#define _TYPEDEF_H

/******************************************************************************
Preprocess statement
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/
#ifndef __TYPEDEF__
typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef signed short        int16_t;
typedef unsigned short      uint16_t;
typedef signed long         int32_t;
typedef unsigned long       uint32_t;
#define __TYPEDEF__
#endif /* __TYPEDEF__ */

#ifndef __64BITS_INT_TYPEDEF__
typedef signed long long    int64_t;
typedef unsigned long long  uint64_t;
#define __64BITS_INT_TYPEDEF__
#endif

#ifndef __FLOAT_TYPEDEF__
typedef unsigned long long  double64_t;
typedef float               float32_t;
#define __FLOAT_TYPEDEF__
#endif /* __FLOAT_TYPEDEF__ */



/******************************************************************************
Macro definitions
******************************************************************************/
/* NULL pointer */
#ifndef NULL
#define NULL                ((void *)(0))
#endif

/* TRUE & FALSE */
#ifndef TRUE
#define TRUE            (1)
#endif
#ifndef FALSE
#define FALSE           (0)
#endif

#ifdef __cplusplus
#define EXTERN_C	extern "C"
#else
#define EXTERN_C
#endif

/* Macro concatenation */
#ifndef JOIN_AGAIN
#define JOIN_AGAIN(x,y)     x ## y
#endif
#ifndef JOIN
#define JOIN(x,y)           JOIN_AGAIN(x,y)
#endif

/* Stringification */
#ifndef str
#define str(x)              #x
#endif
#ifndef xstr
#define xstr(x)             str(x)
#endif

/* Static asserting */
#ifndef STATIC_ASSERT
#define STATIC_ASSERT(e)    enum { JOIN(assert_line_, __LINE__) = 1/(int)(!!(e)) }
#endif

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/

#endif /* _TYPEDEF_H */


