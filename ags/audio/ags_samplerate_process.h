/*
** Copyright (c) 2002-2021, Erik de Castro Lopo <erikd@mega-nerd.com>
** All rights reserved.
**
** This code is released under 2-clause BSD license. Please see the
** file at : https://github.com/libsndfile/libsamplerate/blob/master/COPYING
*/

#ifndef __AGS_SAMPLERATE_PROCESS_H__
#define __AGS_SAMPLERATE_PROCESS_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_resample_util.h>

#include <assert.h>

G_BEGIN_DECLS

#define	ARRAY_LEN(x)		((int) (sizeof (x) / sizeof ((x) [0])))
#define OFFSETOF(type,member)	((int) (&((type*) 0)->member))

#define MAKE_INCREMENT_T(x) 	((increment_t) (x))

#define	SHIFT_BITS				12
#define	FP_ONE					((gdouble) (((increment_t) 1) << SHIFT_BITS))
#define	INV_FP_ONE				(1.0 / FP_ONE)

#define	SRC_MAX_RATIO			256
#define	SRC_MAX_RATIO_STR		"256"

#define	SRC_MIN_RATIO_DIFF		(1e-20)

#ifndef MAX
#define	MAX(a,b)	(((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define	MIN(a,b)	(((a) < (b)) ? (a) : (b))
#endif

typedef int32_t increment_t;

static inline int psf_lrint(double x)
{
  return lrint(x);
} /* psf_lrint */

/*----------------------------------------------------------
**	Common static inline functions.
*/

static inline double
fmod_one(double x)
{
  double res ;

  res = x - psf_lrint (x) ;

  if(res < 0.0){
    return res + 1.0 ;
  }
  
  return res ;
} /* fmod_one */

static inline int
is_bad_src_ratio (double ratio)
{	return (ratio < (1.0 / SRC_MAX_RATIO) || ratio > (1.0 * SRC_MAX_RATIO)) ;
} /* is_bad_src_ratio */

/* double_to_fp */
static inline increment_t
double_to_fp (double x)
{
  return (increment_t) (psf_lrint ((x) * FP_ONE)) ;
}

static inline increment_t
int_to_fp (int x)
{	return (((increment_t) (x)) << SHIFT_BITS) ;
} /* int_to_fp */

static inline int
fp_to_int (increment_t x)
{	return (((x) >> SHIFT_BITS)) ;
} /* fp_to_int */

static inline increment_t
fp_fraction_part (increment_t x)
{	return ((x) & ((((increment_t) 1) << SHIFT_BITS) - 1)) ;
} /* fp_fraction_part */

static inline double
fp_to_double (increment_t x)
{	return fp_fraction_part (x) * INV_FP_ONE ;
} /* fp_to_double */

static inline int
int_div_ceil (int divident, int divisor) /* == (int) ceil ((float) divident / divisor) */
{	assert (divident >= 0 && divisor > 0) ; /* For positive numbers only */
  return (divident + (divisor - 1)) / divisor ;
}

int ags_samplerate_process_resample_util(AgsResampleUtil *resample_util);

G_END_DECLS

#endif /*__AGS_SAMPLERATE_PROCESS_H__*/
