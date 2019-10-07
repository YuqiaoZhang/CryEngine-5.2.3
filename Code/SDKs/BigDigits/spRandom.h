/* $Id: spRandom.h $ */

/******************** SHORT COPYRIGHT NOTICE**************************
This source code is part of the BigDigits multiple-precision
arithmetic library Version 2.1 originally written by David Ireland,
copyright (c) 2001-6 D.I. Management Services Pty Limited, all rights
reserved. It is provided "as is" with no warranties. You may use
this software under the terms of the full copyright notice
"bigdigitsCopyright.txt" that should have been included with this
library or can be obtained from <www.di-mgt.com.au/bigdigits.html>.
This notice must always be retained in any copy.
******************* END OF COPYRIGHT NOTICE***************************/
/*
	Last updated:
	$Date: 2006-08-23 11:13:00 $
	$Revision: 2.1.0 $
	$Author: dai $
*/

/* Interface for spBetterRand function */

#ifndef SPRANDOM_H_
#define SPRANDOM_H_ 1

#include "bigdigits.h"

#ifdef __cplusplus
extern "C" {
#endif

DIGIT_T spBetterRand(void);
	/*	Returns a "better" pseudo-random digit. */

#ifdef __cplusplus
}
#endif

#endif /* BDRAND_H_ */
