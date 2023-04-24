/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AGS_AUDIO_BUFFER_UTIL_H__
#define __AGS_AUDIO_BUFFER_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#ifdef __APPLE__
#include <AudioToolbox/AudioToolbox.h>
#endif

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_BUFFER_UTIL         (ags_audio_buffer_util_get_type())

typedef struct _AgsAudioBufferUtil AgsAudioBufferUtil;

struct _AgsAudioBufferUtil
{
  //empty
};

GType ags_audio_buffer_util_get_type(void);

#define AGS_AUDIO_BUFFER_S8(ptr) ((gint8 *)(ptr))
#define AGS_AUDIO_BUFFER_S16(ptr) ((gint16 *)(ptr))
#define AGS_AUDIO_BUFFER_S24(ptr) ((gint32 *)(ptr))
#define AGS_AUDIO_BUFFER_S32(ptr) ((gint32 *)(ptr))
#define AGS_AUDIO_BUFFER_S64(ptr) ((gint64 *)(ptr))
#define AGS_AUDIO_BUFFER_FLOAT(ptr) ((gfloat *)(ptr))
#define AGS_AUDIO_BUFFER_DOUBLE(ptr) ((gdouble *)(ptr))
#define AGS_AUDIO_BUFFER_COMPLEX(ptr) ((AgsComplex *)(ptr))

#define AGS_AUDIO_BUFFER_UTIL_S8_TO_COMPLEX(x_val, x_retval) { double complex l_z; \
    l_z = ((double) x_val / (double) G_MAXINT8) + I * 0;					\
    ags_complex_set(x_retval[0], l_z); }
#define AGS_AUDIO_BUFFER_UTIL_S16_TO_COMPLEX(x_val, x_retval) { double complex l_z; \
    l_z = ((double) x_val / (double) G_MAXINT16) + I * 0;			\
    ags_complex_set(x_retval[0], l_z); }
#define AGS_AUDIO_BUFFER_UTIL_S24_TO_COMPLEX(x_val, x_retval) { double complex l_z; \
    l_z = ((double) x_val / (double) (0xffffff - 1)) + I * 0;				\
    ags_complex_set(x_retval[0], l_z); }
#define AGS_AUDIO_BUFFER_UTIL_S32_TO_COMPLEX(x_val, x_retval) { double complex l_z; \
    l_z = ((double) x_val / (double) G_MAXINT32) + I * 0;				\
    ags_complex_set(x_retval[0], l_z); }
#define AGS_AUDIO_BUFFER_UTIL_S64_TO_COMPLEX(x_val, x_retval) { double complex l_z; \
    l_z = ((double) x_val / (double) G_MAXINT64) + I * 0;				\
    ags_complex_set(x_retval[0], l_z); }
#define AGS_AUDIO_BUFFER_UTIL_FLOAT_TO_COMPLEX(x_val, x_retval) { double complex l_z; \
    l_z = (x_val) + I * 0;						\
    ags_complex_set(x_retval[0], l_z); }
#define AGS_AUDIO_BUFFER_UTIL_DOUBLE_TO_COMPLEX(x_val, x_retval) { double complex l_z; \
    l_z = (x_val) + I * 0;						\
    ags_complex_set(x_retval[0], l_z); }

#define AGS_AUDIO_BUFFER_UTIL_COMPLEX_TO_S8(x_val, x_retval) { double complex l_z; \
    l_z = ags_complex_get(x_val);					\
    x_retval[0] = (gint8) ((double) G_MAXINT8 * creal(l_z)); }
#define AGS_AUDIO_BUFFER_UTIL_COMPLEX_TO_S16(x_val, x_retval) { double complex l_z; \
    l_z = ags_complex_get(x_val);					\
    x_retval[0] = (gint16) ((double) G_MAXINT16 * creal(l_z)); }
#define AGS_AUDIO_BUFFER_UTIL_COMPLEX_TO_S24(x_val, x_retval) { double complex l_z; \
    l_z = ags_complex_get(x_val);					\
    x_retval[0] = (gint32) ((double) (0xffffff - 1) * creal(l_z)); }
#define AGS_AUDIO_BUFFER_UTIL_COMPLEX_TO_S32(x_val, x_retval) { double complex l_z; \
    l_z = ags_complex_get(x_val);					\
    x_retval[0] = (gint32) ((double) G_MAXINT32 * creal(l_z)); }
#define AGS_AUDIO_BUFFER_UTIL_COMPLEX_TO_S64(x_val, x_retval) { double complex l_z; \
    l_z = ags_complex_get(x_val);					\
    x_retval[0] = (gint64) ((double) G_MAXINT64 * creal(l_z)); }
#define AGS_AUDIO_BUFFER_UTIL_COMPLEX_TO_FLOAT(x_val, x_retval) { double complex l_z; \
    l_z = ags_complex_get(x_val);					\
    x_retval[0] = creal(l_z); }
#define AGS_AUDIO_BUFFER_UTIL_COMPLEX_TO_DOUBLE(x_val, x_retval) { double complex l_z; \
    l_z = ags_complex_get(x_val);					\
    x_retval[0] = creal(l_z); }

/**
 * AgsAudioBufferUtilFormat:
 * @AGS_AUDIO_BUFFER_UTIL_S8: signed 8 bit raw pcm
 * @AGS_AUDIO_BUFFER_UTIL_S16: signed 16 bit raw pcm
 * @AGS_AUDIO_BUFFER_UTIL_S24: signed 24 bit raw pcm
 * @AGS_AUDIO_BUFFER_UTIL_S32: signed 32 bit raw pcm
 * @AGS_AUDIO_BUFFER_UTIL_S64: signed 64 bit raw pcm
 * @AGS_AUDIO_BUFFER_UTIL_FLOAT: signed float raw pcm
 * @AGS_AUDIO_BUFFER_UTIL_DOUBLE: signed double raw pcm
 * @AGS_AUDIO_BUFFER_UTIL_FLOAT32: signed float32 raw pcm
 * @AGS_AUDIO_BUFFER_UTIL_COMPLEX: complex data type
 * 
 * #AgsAudioBufferUtilFormat specifies the audio data representation.
 */
typedef enum{
  AGS_AUDIO_BUFFER_UTIL_S8,
  AGS_AUDIO_BUFFER_UTIL_S16,
  AGS_AUDIO_BUFFER_UTIL_S24,
  AGS_AUDIO_BUFFER_UTIL_S32,
  AGS_AUDIO_BUFFER_UTIL_S64,
  AGS_AUDIO_BUFFER_UTIL_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_DOUBLE,
  AGS_AUDIO_BUFFER_UTIL_FLOAT32,
  AGS_AUDIO_BUFFER_UTIL_COMPLEX,
}AgsAudioBufferUtilFormat;

/**
 * AgsAudioBufferUtilSamplerate:
 * @AGS_AUDIO_BUFFER_UTIL_8000HZ: 8000 Hz
 * @AGS_AUDIO_BUFFER_UTIL_44100HZ: 44100 Hz
 * @AGS_AUDIO_BUFFER_UTIL_48000HZ: 48000 Hz
 * @AGS_AUDIO_BUFFER_UTIL_96000HZ: 96000 Hz
 * @AGS_AUDIO_BUFFER_UTIL_176400HZ: 176400 Hz
 * @AGS_AUDIO_BUFFER_UTIL_192000HZ: 192000 Hz
 * @AGS_AUDIO_BUFFER_UTIL_352800HZ: 352800 Hz
 * @AGS_AUDIO_BUFFER_UTIL_2822400HZ: 2822400 Hz
 * @AGS_AUDIO_BUFFER_UTIL_5644800HZ: 5644800 Hz
 * 
 * Some common samplerates in Hertz.
 */
typedef enum{
  AGS_AUDIO_BUFFER_UTIL_8000HZ,
  AGS_AUDIO_BUFFER_UTIL_44100HZ,
  AGS_AUDIO_BUFFER_UTIL_48000HZ,
  AGS_AUDIO_BUFFER_UTIL_96000HZ,
  AGS_AUDIO_BUFFER_UTIL_176400HZ,
  AGS_AUDIO_BUFFER_UTIL_192000HZ,
  AGS_AUDIO_BUFFER_UTIL_352800HZ,
  AGS_AUDIO_BUFFER_UTIL_2822400HZ,
  AGS_AUDIO_BUFFER_UTIL_5644800HZ,
}AgsAudioBufferUtilSamplerate;

/**
 * AgsAudioBufferUtilCopyMode:
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S8: copy AGS_AUDIO_BUFFER_UTIL_S8 to AGS_AUDIO_BUFFER_UTIL_S8
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S16: copy AGS_AUDIO_BUFFER_UTIL_S8 to AGS_AUDIO_BUFFER_UTIL_S16
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S24: copy AGS_AUDIO_BUFFER_UTIL_S8 to AGS_AUDIO_BUFFER_UTIL_S24
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S32: copy AGS_AUDIO_BUFFER_UTIL_S8 to AGS_AUDIO_BUFFER_UTIL_S32
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S64: copy AGS_AUDIO_BUFFER_UTIL_S8 to AGS_AUDIO_BUFFER_UTIL_S64
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT: copy AGS_AUDIO_BUFFER_UTIL_S8 to AGS_AUDIO_BUFFER_UTIL_FLOAT
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_DOUBLE: copy AGS_AUDIO_BUFFER_UTIL_S8 to AGS_AUDIO_BUFFER_UTIL_DOUBLE
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S8: copy AGS_AUDIO_BUFFER_UTIL_S16 to AGS_AUDIO_BUFFER_UTIL_S8
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16: copy AGS_AUDIO_BUFFER_UTIL_S16 to AGS_AUDIO_BUFFER_UTIL_S16
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S24: copy AGS_AUDIO_BUFFER_UTIL_S16 to AGS_AUDIO_BUFFER_UTIL_S24
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S32: copy AGS_AUDIO_BUFFER_UTIL_S16 to AGS_AUDIO_BUFFER_UTIL_S32
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S64: copy AGS_AUDIO_BUFFER_UTIL_S16 to AGS_AUDIO_BUFFER_UTIL_S64
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT: copy AGS_AUDIO_BUFFER_UTIL_S16 to AGS_AUDIO_BUFFER_UTIL_FLOAT
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_DOUBLE: copy AGS_AUDIO_BUFFER_UTIL_S16 to AGS_AUDIO_BUFFER_UTIL_DOUBLE
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S8: copy AGS_AUDIO_BUFFER_UTIL_S24 to AGS_AUDIO_BUFFER_UTIL_S8
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S16: copy AGS_AUDIO_BUFFER_UTIL_S24 to AGS_AUDIO_BUFFER_UTIL_S16
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S24: copy AGS_AUDIO_BUFFER_UTIL_S24 to AGS_AUDIO_BUFFER_UTIL_S24
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S32: copy AGS_AUDIO_BUFFER_UTIL_S24 to AGS_AUDIO_BUFFER_UTIL_S32
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S64: copy AGS_AUDIO_BUFFER_UTIL_S24 to AGS_AUDIO_BUFFER_UTIL_S64
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT: copy AGS_AUDIO_BUFFER_UTIL_S24 to AGS_AUDIO_BUFFER_UTIL_FLOAT
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_DOUBLE: copy AGS_AUDIO_BUFFER_UTIL_S24 to AGS_AUDIO_BUFFER_UTIL_DOUBLE
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S8: copy AGS_AUDIO_BUFFER_UTIL_S32 to AGS_AUDIO_BUFFER_UTIL_S8
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S16: copy AGS_AUDIO_BUFFER_UTIL_S32 to AGS_AUDIO_BUFFER_UTIL_S16
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S24: copy AGS_AUDIO_BUFFER_UTIL_S32 to AGS_AUDIO_BUFFER_UTIL_S24
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32: copy AGS_AUDIO_BUFFER_UTIL_S32 to AGS_AUDIO_BUFFER_UTIL_S32
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S64: copy AGS_AUDIO_BUFFER_UTIL_S32 to AGS_AUDIO_BUFFER_UTIL_S64
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT: copy AGS_AUDIO_BUFFER_UTIL_S32 to AGS_AUDIO_BUFFER_UTIL_FLOAT
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_DOUBLE: copy AGS_AUDIO_BUFFER_UTIL_S32 to AGS_AUDIO_BUFFER_UTIL_DOUBLE
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S8: copy AGS_AUDIO_BUFFER_UTIL_S64 to AGS_AUDIO_BUFFER_UTIL_S8
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S16: copy AGS_AUDIO_BUFFER_UTIL_S64 to AGS_AUDIO_BUFFER_UTIL_S16
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S24: copy AGS_AUDIO_BUFFER_UTIL_S64 to AGS_AUDIO_BUFFER_UTIL_S24
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S32: copy AGS_AUDIO_BUFFER_UTIL_S64 to AGS_AUDIO_BUFFER_UTIL_S32
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S64: copy AGS_AUDIO_BUFFER_UTIL_S64 to AGS_AUDIO_BUFFER_UTIL_S64
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT: copy AGS_AUDIO_BUFFER_UTIL_S64 to AGS_AUDIO_BUFFER_UTIL_FLOAT
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_DOUBLE: copy AGS_AUDIO_BUFFER_UTIL_S64 to AGS_AUDIO_BUFFER_UTIL_DOUBLE
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S8: copy AGS_AUDIO_BUFFER_UTIL_FLOAT to AGS_AUDIO_BUFFER_UTIL_S8
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S16: copy AGS_AUDIO_BUFFER_UTIL_FLOAT to AGS_AUDIO_BUFFER_UTIL_S16
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S24: copy AGS_AUDIO_BUFFER_UTIL_FLOAT to AGS_AUDIO_BUFFER_UTIL_S24
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S32: copy AGS_AUDIO_BUFFER_UTIL_FLOAT to AGS_AUDIO_BUFFER_UTIL_S32
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S64: copy AGS_AUDIO_BUFFER_UTIL_FLOAT to AGS_AUDIO_BUFFER_UTIL_S64
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT: copy AGS_AUDIO_BUFFER_UTIL_FLOAT to AGS_AUDIO_BUFFER_UTIL_FLOAT
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_DOUBLE: copy AGS_AUDIO_BUFFER_UTIL_FLOAT to AGS_AUDIO_BUFFER_UTIL_DOUBLE
 * @AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S8: copy AGS_AUDIO_BUFFER_UTIL_DOUBLE to AGS_AUDIO_BUFFER_UTIL_S8
 * @AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S16: copy AGS_AUDIO_BUFFER_UTIL_DOUBLE to AGS_AUDIO_BUFFER_UTIL_S16
 * @AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S24: copy AGS_AUDIO_BUFFER_UTIL_DOUBLE to AGS_AUDIO_BUFFER_UTIL_S24
 * @AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S32: copy AGS_AUDIO_BUFFER_UTIL_DOUBLE to AGS_AUDIO_BUFFER_UTIL_S32
 * @AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S64: copy AGS_AUDIO_BUFFER_UTIL_DOUBLE to AGS_AUDIO_BUFFER_UTIL_S64
 * @AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_FLOAT: copy AGS_AUDIO_BUFFER_UTIL_DOUBLE to AGS_AUDIO_BUFFER_UTIL_FLOAT
 * @AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE: copy AGS_AUDIO_BUFFER_UTIL_DOUBLE to AGS_AUDIO_BUFFER_UTIL_DOUBLE
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT32: copy AGS_AUDIO_BUFFER_UTIL_S8 to AGS_AUDIO_BUFFER_UTIL_FLOAT32
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT32: copy AGS_AUDIO_BUFFER_UTIL_S16 to AGS_AUDIO_BUFFER_UTIL_FLOAT32
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT32: copy AGS_AUDIO_BUFFER_UTIL_S24 to AGS_AUDIO_BUFFER_UTIL_FLOAT32
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT32: copy AGS_AUDIO_BUFFER_UTIL_S32 to AGS_AUDIO_BUFFER_UTIL_FLOAT32
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT32: copy AGS_AUDIO_BUFFER_UTIL_S64 to AGS_AUDIO_BUFFER_UTIL_FLOAT32
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT32: copy AGS_AUDIO_BUFFER_UTIL_FLOAT to AGS_AUDIO_BUFFER_UTIL_FLOAT
 * @AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_FLOAT32: copy AGS_AUDIO_BUFFER_UTIL_DOUBLE to AGS_AUDIO_BUFFER_UTIL_DOUBLE
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_FLOAT32: copy AGS_AUDIO_BUFFER_UTIL_FLOAT32 to AGS_AUDIO_BUFFER_UTIL_FLOAT32
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S8: copy AGS_AUDIO_BUFFER_UTIL_FLOAT32 to AGS_AUDIO_BUFFER_UTIL_S8
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S16: copy AGS_AUDIO_BUFFER_UTIL_FLOAT32 to AGS_AUDIO_BUFFER_UTIL_S16
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S24: copy AGS_AUDIO_BUFFER_UTIL_FLOAT32 to AGS_AUDIO_BUFFER_UTIL_S24
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S32: copy AGS_AUDIO_BUFFER_UTIL_FLOAT32 to AGS_AUDIO_BUFFER_UTIL_S32
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S64: copy AGS_AUDIO_BUFFER_UTIL_FLOAT32 to AGS_AUDIO_BUFFER_UTIL_S64
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_FLOAT: copy AGS_AUDIO_BUFFER_UTIL_FLOAT32 to AGS_AUDIO_BUFFER_UTIL_FLOAT
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_DOUBLE: copy AGS_AUDIO_BUFFER_UTIL_FLOAT32 to AGS_AUDIO_BUFFER_UTIL_DOUBLE
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_COMPLEX: copy AGS_AUDIO_BUFFER_UTIL_S8 to AGS_AUDIO_BUFFER_UTIL_COMPLEX
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_COMPLEX: copy AGS_AUDIO_BUFFER_UTIL_S16 to AGS_AUDIO_BUFFER_UTIL_COMPLEX
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_COMPLEX: copy AGS_AUDIO_BUFFER_UTIL_S24 to AGS_AUDIO_BUFFER_UTIL_COMPLEX
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_COMPLEX: copy AGS_AUDIO_BUFFER_UTIL_S32 to AGS_AUDIO_BUFFER_UTIL_COMPLEX
 * @AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_COMPLEX: copy AGS_AUDIO_BUFFER_UTIL_S64 to AGS_AUDIO_BUFFER_UTIL_COMPLEX
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_COMPLEX: copy AGS_AUDIO_BUFFER_UTIL_FLOAT to AGS_AUDIO_BUFFER_UTIL_COMPLEX
 * @AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_COMPLEX: copy AGS_AUDIO_BUFFER_UTIL_DOUBLE to AGS_AUDIO_BUFFER_UTIL_COMPLEX
 * @AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_COMPLEX: copy AGS_AUDIO_BUFFER_UTIL_FLOAT32 to AGS_AUDIO_BUFFER_UTIL_COMPLEX
 * @AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_COMPLEX: copy AGS_AUDIO_BUFFER_UTIL_COMPLEX to AGS_AUDIO_BUFFER_UTIL_COMPLEX
 * @AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S8: copy AGS_AUDIO_BUFFER_UTIL_COMPLEX to AGS_AUDIO_BUFFER_UTIL_S8
 * @AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S16: copy AGS_AUDIO_BUFFER_UTIL_COMPLEX to AGS_AUDIO_BUFFER_UTIL_S16
 * @AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S24: copy AGS_AUDIO_BUFFER_UTIL_COMPLEX to AGS_AUDIO_BUFFER_UTIL_S24
 * @AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S32: copy AGS_AUDIO_BUFFER_UTIL_COMPLEX to AGS_AUDIO_BUFFER_UTIL_S32
 * @AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S64: copy AGS_AUDIO_BUFFER_UTIL_COMPLEX to AGS_AUDIO_BUFFER_UTIL_S64
 * @AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_FLOAT: copy AGS_AUDIO_BUFFER_UTIL_COMPLEX to AGS_AUDIO_BUFFER_UTIL_FLOAT
 * @AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_DOUBLE: copy AGS_AUDIO_BUFFER_UTIL_COMPLEX to AGS_AUDIO_BUFFER_UTIL_DOUBLE
 * @AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_FLOAT32: copy AGS_AUDIO_BUFFER_UTIL_COMPLEX to AGS_AUDIO_BUFFER_UTIL_FLOAT32
 * 
 * Copy modes.
 */
typedef enum{
  /* signed 8 bit source*/
  AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S8,
  AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S16,
  AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S24,
  AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S32,
  AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S64,
  AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_DOUBLE,

  /* signed 16 bit source */  
  AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S8,
  AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16,
  AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S24,
  AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S32,
  AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S64,
  AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_DOUBLE,

  /* signed 24 bit source */
  AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S8,
  AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S16,
  AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S24,
  AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S32,
  AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S64,
  AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_DOUBLE,

  /* signed 32 bit source */
  AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S8,
  AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S16,
  AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S24,
  AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32,
  AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S64,
  AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_DOUBLE,

  /* signed 64 bit source */
  AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S8,
  AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S16,
  AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S24,
  AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S32,
  AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S64,
  AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_DOUBLE,

  /* float source */
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S8,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S16,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S24,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S32,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S64,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_DOUBLE,

  /* double source */
  AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S8,
  AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S16,
  AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S24,
  AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S32,
  AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S64,
  AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE,

  /* Float32 */
  AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT32,
  AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT32,
  AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT32,
  AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT32,
  AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT32,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT32,
  AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_FLOAT32,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_FLOAT32,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S8,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S16,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S24,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S32,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_S64,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_DOUBLE,

  /* AgsComplex */
  AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_COMPLEX,
  AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_COMPLEX,
  AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_COMPLEX,
  AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_COMPLEX,
  AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_COMPLEX,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_COMPLEX,
  AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_COMPLEX,
  AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT32_TO_COMPLEX,
  AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_COMPLEX,
  AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S8,
  AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S16,
  AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S24,
  AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S32,
  AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_S64,
  AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_FLOAT,
  AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_DOUBLE,
  AGS_AUDIO_BUFFER_UTIL_COPY_COMPLEX_TO_FLOAT32,

}AgsAudioBufferUtilCopyMode;

typedef gint8 ags_v8s8 __attribute__ ((vector_size(8 * sizeof(gint8))));
typedef gint16 ags_v8s16 __attribute__ ((vector_size(8 * sizeof(gint16))));
typedef gint32 ags_v8s32 __attribute__ ((vector_size(8 * sizeof(gint32))));
typedef gint64 ags_v8s64 __attribute__ ((vector_size(8 * sizeof(gint64))));
typedef gfloat ags_v8float __attribute__ ((vector_size(8 * sizeof(gfloat))));
typedef gdouble ags_v8double __attribute__ ((vector_size(8 * sizeof(gdouble))));

AgsAudioBufferUtilFormat ags_audio_buffer_util_format_from_soundcard(AgsSoundcardFormat soundcard_format);
AgsAudioBufferUtilCopyMode ags_audio_buffer_util_get_copy_mode(AgsAudioBufferUtilFormat destination_format,
							       AgsAudioBufferUtilFormat source_format);

/* clear */
void ags_audio_buffer_util_clear_float(gfloat *buffer, guint channels,
				       guint count);

#ifdef __APPLE__
void ags_audio_buffer_util_clear_float32(Float32 *buffer, guint channels,
					 guint count);
#endif

void ags_audio_buffer_util_clear_double(gdouble *buffer, guint channels,
					guint count);
void ags_audio_buffer_util_clear_complex(AgsComplex *buffer, guint channels,
					 guint count);
void ags_audio_buffer_util_clear_buffer(void *buffer, guint channels,
					guint count, guint format);

/* pong */
void ags_audio_buffer_util_pong_s8(gint8 *destination, guint dchannels,
				   gint8 *source, guint schannels,
				   guint count);
void ags_audio_buffer_util_pong_s16(gint16 *destination, guint dchannels,
				    gint16 *source, guint schannels,
				    guint count);
void ags_audio_buffer_util_pong_s24(gint32 *destination, guint dchannels,
				    gint32 *source, guint schannels,
				    guint count);
void ags_audio_buffer_util_pong_s32(gint32 *destination, guint dchannels,
				    gint32 *source, guint schannels,
				    guint count);
void ags_audio_buffer_util_pong_s64(gint64 *destination, guint dchannels,
				    gint64 *source, guint schannels,
				    guint count);
void ags_audio_buffer_util_pong_float(gfloat *destination, guint dchannels,
				      gfloat *source, guint schannels,
				      guint count);
void ags_audio_buffer_util_pong_double(gdouble *destination, guint dchannels,
				       gdouble *source, guint schannels,
				       guint count);
void ags_audio_buffer_util_pong_complex(AgsComplex *destination, guint dchannels,
					AgsComplex *source, guint schannels,
					guint count);

void ags_audio_buffer_util_pong(void *destination, guint dchannels,
				void *source, guint schannels,
				guint count, guint format);

/* copy 8 bit */
void ags_audio_buffer_util_copy_s8_to_s8(gint8 *destination, guint dchannels,
					 gint8 *source, guint schannels,
					 guint count);
void ags_audio_buffer_util_copy_s8_to_s16(gint16 *destination, guint dchannels,
					  gint8 *source, guint schannels,
					  guint count);
void ags_audio_buffer_util_copy_s8_to_s24(gint32 *destination, guint dchannels,
					  gint8 *source, guint schannels,
					  guint count);
void ags_audio_buffer_util_copy_s8_to_s32(gint32 *destination, guint dchannels,
					  gint8 *source, guint schannels,
					  guint count);
void ags_audio_buffer_util_copy_s8_to_s64(gint64 *destination, guint dchannels,
					  gint8 *source, guint schannels,
					  guint count);
void ags_audio_buffer_util_copy_s8_to_float(gfloat *destination, guint dchannels,
					    gint8 *source, guint schannels,
					    guint count);
void ags_audio_buffer_util_copy_s8_to_double(gdouble *destination, guint dchannels,
					     gint8 *source, guint schannels,
					     guint count);

/* copy 16 bit */
void ags_audio_buffer_util_copy_s16_to_s8(gint8 *destination, guint dchannels,
					  gint16 *source, guint schannels,
					  guint count);
void ags_audio_buffer_util_copy_s16_to_s16(gint16 *destination, guint dchannels,
					   gint16 *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s16_to_s24(gint32 *destination, guint dchannels,
					   gint16 *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s16_to_s32(gint32 *destination, guint dchannels,
					   gint16 *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s16_to_s64(gint64 *destination, guint dchannels,
					   gint16 *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s16_to_float(gfloat *destination, guint dchannels,
					     gint16 *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_s16_to_double(gdouble *destination, guint dchannels,
					      gint16 *source, guint schannels,
					      guint count);

/* copy 24 bit */
void ags_audio_buffer_util_copy_s24_to_s8(gint8 *destination, guint dchannels,
					  gint32 *source, guint schannels,
					  guint count);
void ags_audio_buffer_util_copy_s24_to_s16(gint16 *destination, guint dchannels,
					   gint32 *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s24_to_s24(gint32 *destination, guint dchannels,
					   gint32 *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s24_to_s32(gint32 *destination, guint dchannels,
					   gint32 *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s24_to_s64(gint64 *destination, guint dchannels,
					   gint32 *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s24_to_float(gfloat *destination, guint dchannels,
					     gint32 *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_s24_to_double(gdouble *destination, guint dchannels,
					      gint32 *source, guint schannels,
					      guint count);

/* copy 32 bit */
void ags_audio_buffer_util_copy_s32_to_s8(gint8 *destination, guint dchannels,
					  gint32 *source, guint schannels,
					  guint count);
void ags_audio_buffer_util_copy_s32_to_s16(gint16 *destination, guint dchannels,
					   gint32 *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s32_to_s24(gint32 *destination, guint dchannels,
					   gint32 *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s32_to_s32(gint32 *destination, guint dchannels,
					   gint32 *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s32_to_s64(gint64 *destination, guint dchannels,
					   gint32 *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s32_to_float(gfloat *destination, guint dchannels,
					     gint32 *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_s32_to_double(gdouble *destination, guint dchannels,
					      gint32 *source, guint schannels,
					      guint count);

/* copy 64 bit */
void ags_audio_buffer_util_copy_s64_to_s8(gint8 *destination, guint dchannels,
					  gint64 *source, guint schannels,
					  guint count);
void ags_audio_buffer_util_copy_s64_to_s16(gint16 *destination, guint dchannels,
					   gint64 *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s64_to_s24(gint32 *destination, guint dchannels,
					   gint64 *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s64_to_s32(gint32 *destination, guint dchannels,
					   gint64 *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s64_to_s64(gint64 *destination, guint dchannels,
					   gint64 *source, guint schannels,
					   guint count);
void ags_audio_buffer_util_copy_s64_to_float(gfloat *destination, guint dchannels,
					     gint64 *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_s64_to_double(gdouble *destination, guint dchannels,
					      gint64 *source, guint schannels,
					      guint count);

/* copy float */
void ags_audio_buffer_util_copy_float_to_s8(gint8 *destination, guint dchannels,
					    gfloat *source, guint schannels,
					    guint count);
void ags_audio_buffer_util_copy_float_to_s16(gint16 *destination, guint dchannels,
					     gfloat *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_float_to_s24(gint32 *destination, guint dchannels,
					     gfloat *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_float_to_s32(gint32 *destination, guint dchannels,
					     gfloat *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_float_to_s64(gint64 *destination, guint dchannels,
					     gfloat *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_float_to_float(gfloat *destination, guint dchannels,
					       gfloat *source, guint schannels,
					       guint count);
void ags_audio_buffer_util_copy_float_to_double(gdouble *destination, guint dchannels,
						gfloat *source, guint schannels,
						guint count);

/* copy double */
void ags_audio_buffer_util_copy_double_to_s8(gint8 *destination, guint dchannels,
					     gdouble *source, guint schannels,
					     guint count);
void ags_audio_buffer_util_copy_double_to_s16(gint16 *destination, guint dchannels,
					      gdouble *source, guint schannels,
					      guint count);
void ags_audio_buffer_util_copy_double_to_s24(gint32 *destination, guint dchannels,
					      gdouble *source, guint schannels,
					      guint count);
void ags_audio_buffer_util_copy_double_to_s32(gint32 *destination, guint dchannels,
					      gdouble *source, guint schannels,
					      guint count);
void ags_audio_buffer_util_copy_double_to_s64(gint64 *destination, guint dchannels,
					      gdouble *source, guint schannels,
					      guint count);
void ags_audio_buffer_util_copy_double_to_float(gfloat *destination, guint dchannels,
						gdouble *source, guint schannels,
						guint count);
void ags_audio_buffer_util_copy_double_to_double(gdouble *destination, guint dchannels,
						 gdouble *source, guint schannels,
						 guint count);

#ifdef __APPLE__
/* Float32 */
void ags_audio_buffer_util_copy_s8_to_float32(Float32 *destination, guint dchannels,
					      gint8 *source, guint schannels,
					      guint count);

void ags_audio_buffer_util_copy_s16_to_float32(Float32 *destination, guint dchannels,
					       gint16 *source, guint schannels,
					       guint count);

void ags_audio_buffer_util_copy_s24_to_float32(Float32 *destination, guint dchannels,
					       gint32 *source, guint schannels,
					       guint count);

void ags_audio_buffer_util_copy_s32_to_float32(Float32 *destination, guint dchannels,
					       gint32 *source, guint schannels,
					       guint count);

void ags_audio_buffer_util_copy_s64_to_float32(Float32 *destination, guint dchannels,
					       gint64 *source, guint schannels,
					       guint count);

void ags_audio_buffer_util_copy_float_to_float32(Float32 *destination, guint dchannels,
						 float *source, guint schannels,
						 guint count);

void ags_audio_buffer_util_copy_double_to_float32(Float32 *destination, guint dchannels,
						  gdouble *source, guint schannels,
						  guint count);

void ags_audio_buffer_util_copy_float32_to_float32(Float32 *destination, guint dchannels,
						   Float32 *source, guint schannels,
						   guint count);

void ags_audio_buffer_util_copy_float32_to_s8(gint8 *destination, guint dchannels,
					      Float32 *source, guint schannels,
					      guint count);
void ags_audio_buffer_util_copy_float32_to_s16(gint16 *destination, guint dchannels,
					       Float32 *source, guint schannels,
					       guint count);
void ags_audio_buffer_util_copy_float32_to_s24(gint32 *destination, guint dchannels,
					       Float32 *source, guint schannels,
					       guint count);
void ags_audio_buffer_util_copy_float32_to_s32(gint32 *destination, guint dchannels,
					       Float32 *source, guint schannels,
					       guint count);
void ags_audio_buffer_util_copy_float32_to_s64(gint64 *destination, guint dchannels,
					       Float32 *source, guint schannels,
					       guint count);
void ags_audio_buffer_util_copy_float32_to_float(gfloat *destination, guint dchannels,
						 Float32 *source, guint schannels,
						 guint count);
void ags_audio_buffer_util_copy_float32_to_double(gdouble *destination, guint dchannels,
						  Float32 *source, guint schannels,
						  guint count);
#endif

/* AgsComplex */
void ags_audio_buffer_util_copy_s8_to_complex(AgsComplex *destination, guint dchannels,
					      gint8 *source, guint schannels,
					      guint count);

void ags_audio_buffer_util_copy_s16_to_complex(AgsComplex *destination, guint dchannels,
					       gint16 *source, guint schannels,
					       guint count);

void ags_audio_buffer_util_copy_s24_to_complex(AgsComplex *destination, guint dchannels,
					       gint32 *source, guint schannels,
					       guint count);

void ags_audio_buffer_util_copy_s32_to_complex(AgsComplex *destination, guint dchannels,
					       gint32 *source, guint schannels,
					       guint count);

void ags_audio_buffer_util_copy_s64_to_complex(AgsComplex *destination, guint dchannels,
					       gint64 *source, guint schannels,
					       guint count);

void ags_audio_buffer_util_copy_float_to_complex(AgsComplex *destination, guint dchannels,
						 gfloat *source, guint schannels,
						 guint count);

void ags_audio_buffer_util_copy_double_to_complex(AgsComplex *destination, guint dchannels,
						  gdouble *source, guint schannels,
						  guint count);

#ifdef __APPLE__
void ags_audio_buffer_util_copy_float32_to_complex(AgsComplex *destination, guint dchannels,
						   Float32 *source, guint schannels,
						   guint count);
#endif

void ags_audio_buffer_util_copy_complex_to_complex(AgsComplex *destination, guint dchannels,
						   AgsComplex *source, guint schannels,
						   guint count);

void ags_audio_buffer_util_copy_complex_to_s8(gint8 *destination, guint dchannels,
					      AgsComplex *source, guint schannels,
					      guint count);
void ags_audio_buffer_util_copy_complex_to_s16(gint16 *destination, guint dchannels,
					       AgsComplex *source, guint schannels,
					       guint count);
void ags_audio_buffer_util_copy_complex_to_s24(gint32 *destination, guint dchannels,
					       AgsComplex *source, guint schannels,
					       guint count);
void ags_audio_buffer_util_copy_complex_to_s32(gint32 *destination, guint dchannels,
					       AgsComplex *source, guint schannels,
					       guint count);
void ags_audio_buffer_util_copy_complex_to_s64(gint64 *destination, guint dchannels,
					       AgsComplex *source, guint schannels,
					       guint count);
void ags_audio_buffer_util_copy_complex_to_float(gfloat *destination, guint dchannels,
						 AgsComplex *source, guint schannels,
						 guint count);
void ags_audio_buffer_util_copy_complex_to_double(gdouble *destination, guint dchannels,
						  AgsComplex *source, guint schannels,
						  guint count);
#ifdef __APPLE__
void ags_audio_buffer_util_copy_complex_to_float32(Float32 *destination, guint dchannels,
						   AgsComplex *source, guint schannels,
						   guint count);
#endif

/* copy */
void ags_audio_buffer_util_copy_buffer_to_buffer(void *destination, guint dchannels, guint doffset,
						 void *source, guint schannels, guint soffset,
						 guint count, AgsAudioBufferUtilCopyMode mode);

G_END_DECLS

#endif /*__AGS_AUDIO_BUFFER_UTIL_H__*/
