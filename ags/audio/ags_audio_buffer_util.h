/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/audio/ags_sound_types.h>

#if defined(__APPLE__)
#include <AudioToolbox/AudioToolbox.h>
#endif

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_BUFFER_UTIL         (ags_audio_buffer_util_get_type())
#define AGS_AUDIO_BUFFER_UTIL(ptr) ((AgsAudioBufferUtil *)(ptr))

#define AGS_AUDIO_BUFFER_UTIL_DEFAULT_FORMAT (AGS_AUDIO_BUFFER_UTIL_S16)

typedef struct _AgsAudioBufferUtil AgsAudioBufferUtil;

#define AGS_AUDIO_BUFFER_S8(ptr) ((gint8 *)(ptr))
#define AGS_AUDIO_BUFFER_S16(ptr) ((gint16 *)(ptr))
#define AGS_AUDIO_BUFFER_S24(ptr) ((gint32 *)(ptr))
#define AGS_AUDIO_BUFFER_S32(ptr) ((gint32 *)(ptr))
#define AGS_AUDIO_BUFFER_S64(ptr) ((gint64 *)(ptr))
#define AGS_AUDIO_BUFFER_FLOAT(ptr) ((gfloat *)(ptr))
#define AGS_AUDIO_BUFFER_DOUBLE(ptr) ((gdouble *)(ptr))
#define AGS_AUDIO_BUFFER_COMPLEX(ptr) ((AgsComplex *)(ptr))

#define AGS_AUDIO_BUFFER_UTIL_S8_TO_COMPLEX(x_val, x_retval) { double complex l_z; \
    l_z = ((double) x_val / (double) G_MAXINT8) + I * 0;		\
    ags_complex_set(x_retval[0], l_z); }
#define AGS_AUDIO_BUFFER_UTIL_S16_TO_COMPLEX(x_val, x_retval) { double complex l_z; \
    l_z = ((double) x_val / (double) G_MAXINT16) + I * 0;		\
    ags_complex_set(x_retval[0], l_z); }
#define AGS_AUDIO_BUFFER_UTIL_S24_TO_COMPLEX(x_val, x_retval) { double complex l_z; \
    l_z = ((double) x_val / (double) (0x7fffff)) + I * 0;		\
    ags_complex_set(x_retval[0], l_z); }
#define AGS_AUDIO_BUFFER_UTIL_S32_TO_COMPLEX(x_val, x_retval) { double complex l_z; \
    l_z = ((double) x_val / (double) G_MAXINT32) + I * 0;		\
    ags_complex_set(x_retval[0], l_z); }
#define AGS_AUDIO_BUFFER_UTIL_S64_TO_COMPLEX(x_val, x_retval) { double complex l_z; \
    l_z = ((double) x_val / (double) G_MAXINT64) + I * 0;		\
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
    x_retval[0] = (gint32) ((double) (0x7fffff) * creal(l_z)); }
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

struct _AgsAudioBufferUtil
{
  gpointer destination;
  AgsSoundcardFormat destination_format;
  guint destination_stride;

  gpointer source;
  AgsSoundcardFormat source_format;
  guint source_stride;

  AgsAudioBufferUtilCopyMode copy_mode;
  
  gpointer converted_source;
  
  guint buffer_length;
};

GType ags_audio_buffer_util_get_type(void);

AgsAudioBufferUtil* ags_audio_buffer_util_alloc();

gpointer ags_audio_buffer_util_copy(AgsAudioBufferUtil *ptr);
void ags_audio_buffer_util_free(AgsAudioBufferUtil *ptr);

/* getter/setter */
gpointer ags_audio_buffer_util_get_destination(AgsAudioBufferUtil *audio_buffer_util);
void ags_audio_buffer_util_set_destination(AgsAudioBufferUtil *audio_buffer_util,
					   gpointer destination);

AgsSoundcardFormat ags_audio_buffer_util_get_destination_format(AgsAudioBufferUtil *audio_buffer_util);
void ags_audio_buffer_util_set_destination_format(AgsAudioBufferUtil *audio_buffer_util,
						  AgsSoundcardFormat destination_format);

guint ags_audio_buffer_util_get_destination_stride(AgsAudioBufferUtil *audio_buffer_util);
void ags_audio_buffer_util_set_destination_stride(AgsAudioBufferUtil *audio_buffer_util,
						  guint destination_stride);

gpointer ags_audio_buffer_util_get_source(AgsAudioBufferUtil *audio_buffer_util);
void ags_audio_buffer_util_set_source(AgsAudioBufferUtil *audio_buffer_util,
				      gpointer source);

AgsSoundcardFormat ags_audio_buffer_util_get_source_format(AgsAudioBufferUtil *audio_buffer_util);
void ags_audio_buffer_util_set_source_format(AgsAudioBufferUtil *audio_buffer_util,
					     AgsSoundcardFormat source_format);

guint ags_audio_buffer_util_get_source_stride(AgsAudioBufferUtil *audio_buffer_util);
void ags_audio_buffer_util_set_source_stride(AgsAudioBufferUtil *audio_buffer_util,
					     guint source_stride);

AgsAudioBufferUtilCopyMode ags_audio_buffer_util_get_copy_mode(AgsAudioBufferUtil *audio_buffer_util);
void ags_audio_buffer_util_set_copy_mode(AgsAudioBufferUtil *audio_buffer_util,
					 AgsAudioBufferUtilCopyMode copy_mode);

guint ags_audio_buffer_util_get_buffer_length(AgsAudioBufferUtil *audio_buffer_util);
void ags_audio_buffer_util_set_buffer_length(AgsAudioBufferUtil *audio_buffer_util,
					     guint buffer_length);

/* utility */
AgsAudioBufferUtilFormat ags_audio_buffer_util_format_from_soundcard(AgsAudioBufferUtil *audio_buffer_util,
								     AgsSoundcardFormat soundcard_format);
AgsAudioBufferUtilCopyMode ags_audio_buffer_util_get_copy_mode_from_format(AgsAudioBufferUtil *audio_buffer_util,
									   AgsAudioBufferUtilFormat destination_format,
									   AgsAudioBufferUtilFormat source_format);

/* clear */
void ags_audio_buffer_util_clear_float(AgsAudioBufferUtil *audio_buffer_util,
				       gfloat *buffer, guint channels,
				       guint count);

#ifdef __APPLE__
void ags_audio_buffer_util_clear_float32(AgsAudioBufferUtil *audio_buffer_util,
					 Float32 *buffer, guint channels,
					 guint count);
#endif

void ags_audio_buffer_util_clear_double(AgsAudioBufferUtil *audio_buffer_util,
					gdouble *buffer, guint channels,
					guint count);
void ags_audio_buffer_util_clear_complex(AgsAudioBufferUtil *audio_buffer_util,
					 AgsComplex *buffer, guint channels,
					 guint count);
void ags_audio_buffer_util_clear_buffer(AgsAudioBufferUtil *audio_buffer_util,
					void *buffer, guint channels,
					guint count, guint format);

/* pong */
void ags_audio_buffer_util_pong_s8(AgsAudioBufferUtil *audio_buffer_util,
				   gint8 *destination, guint destination_stride,
				   gint8 *source, guint source_stride,
				   guint count);
void ags_audio_buffer_util_pong_s16(AgsAudioBufferUtil *audio_buffer_util,
				    gint16 *destination, guint destination_stride,
				    gint16 *source, guint source_stride,
				    guint count);
void ags_audio_buffer_util_pong_s24(AgsAudioBufferUtil *audio_buffer_util,
				    gint32 *destination, guint destination_stride,
				    gint32 *source, guint source_stride,
				    guint count);
void ags_audio_buffer_util_pong_s32(AgsAudioBufferUtil *audio_buffer_util,
				    gint32 *destination, guint destination_stride,
				    gint32 *source, guint source_stride,
				    guint count);
void ags_audio_buffer_util_pong_s64(AgsAudioBufferUtil *audio_buffer_util,
				    gint64 *destination, guint destination_stride,
				    gint64 *source, guint source_stride,
				    guint count);
void ags_audio_buffer_util_pong_float(AgsAudioBufferUtil *audio_buffer_util,
				      gfloat *destination, guint destination_stride,
				      gfloat *source, guint source_stride,
				      guint count);
void ags_audio_buffer_util_pong_double(AgsAudioBufferUtil *audio_buffer_util,
				       gdouble *destination, guint destination_stride,
				       gdouble *source, guint source_stride,
				       guint count);
void ags_audio_buffer_util_pong_complex(AgsAudioBufferUtil *audio_buffer_util,
					AgsComplex *destination, guint destination_stride,
					AgsComplex *source, guint source_stride,
					guint count);

void ags_audio_buffer_util_pong(AgsAudioBufferUtil *audio_buffer_util,
				void *destination, guint destination_stride,
				void *source, guint source_stride,
				guint count, guint format);

/* copy 8 bit */
void ags_audio_buffer_util_copy_s8_to_s8(AgsAudioBufferUtil *audio_buffer_util,
					 gint8 *destination, guint destination_stride,
					 gint8 *source, guint source_stride,
					 guint count);
void ags_audio_buffer_util_copy_s8_to_s16(AgsAudioBufferUtil *audio_buffer_util,
					  gint16 *destination, guint destination_stride,
					  gint8 *source, guint source_stride,
					  guint count);
void ags_audio_buffer_util_copy_s8_to_s24(AgsAudioBufferUtil *audio_buffer_util,
					  gint32 *destination, guint destination_stride,
					  gint8 *source, guint source_stride,
					  guint count);
void ags_audio_buffer_util_copy_s8_to_s32(AgsAudioBufferUtil *audio_buffer_util,
					  gint32 *destination, guint destination_stride,
					  gint8 *source, guint source_stride,
					  guint count);
void ags_audio_buffer_util_copy_s8_to_s64(AgsAudioBufferUtil *audio_buffer_util,
					  gint64 *destination, guint destination_stride,
					  gint8 *source, guint source_stride,
					  guint count);
void ags_audio_buffer_util_copy_s8_to_float(AgsAudioBufferUtil *audio_buffer_util,
					    gfloat *destination, guint destination_stride,
					    gint8 *source, guint source_stride,
					    guint count);
void ags_audio_buffer_util_copy_s8_to_double(AgsAudioBufferUtil *audio_buffer_util,
					     gdouble *destination, guint destination_stride,
					     gint8 *source, guint source_stride,
					     guint count);

/* copy 16 bit */
void ags_audio_buffer_util_copy_s16_to_s8(AgsAudioBufferUtil *audio_buffer_util,
					  gint8 *destination, guint destination_stride,
					  gint16 *source, guint source_stride,
					  guint count);
void ags_audio_buffer_util_copy_s16_to_s16(AgsAudioBufferUtil *audio_buffer_util,
					   gint16 *destination, guint destination_stride,
					   gint16 *source, guint source_stride,
					   guint count);
void ags_audio_buffer_util_copy_s16_to_s24(AgsAudioBufferUtil *audio_buffer_util,
					   gint32 *destination, guint destination_stride,
					   gint16 *source, guint source_stride,
					   guint count);
void ags_audio_buffer_util_copy_s16_to_s32(AgsAudioBufferUtil *audio_buffer_util,
					   gint32 *destination, guint destination_stride,
					   gint16 *source, guint source_stride,
					   guint count);
void ags_audio_buffer_util_copy_s16_to_s64(AgsAudioBufferUtil *audio_buffer_util,
					   gint64 *destination, guint destination_stride,
					   gint16 *source, guint source_stride,
					   guint count);
void ags_audio_buffer_util_copy_s16_to_float(AgsAudioBufferUtil *audio_buffer_util,
					     gfloat *destination, guint destination_stride,
					     gint16 *source, guint source_stride,
					     guint count);
void ags_audio_buffer_util_copy_s16_to_double(AgsAudioBufferUtil *audio_buffer_util,
					      gdouble *destination, guint destination_stride,
					      gint16 *source, guint source_stride,
					      guint count);

/* copy 24 bit */
void ags_audio_buffer_util_copy_s24_to_s8(AgsAudioBufferUtil *audio_buffer_util,
					  gint8 *destination, guint destination_stride,
					  gint32 *source, guint source_stride,
					  guint count);
void ags_audio_buffer_util_copy_s24_to_s16(AgsAudioBufferUtil *audio_buffer_util,
					   gint16 *destination, guint destination_stride,
					   gint32 *source, guint source_stride,
					   guint count);
void ags_audio_buffer_util_copy_s24_to_s24(AgsAudioBufferUtil *audio_buffer_util,
					   gint32 *destination, guint destination_stride,
					   gint32 *source, guint source_stride,
					   guint count);
void ags_audio_buffer_util_copy_s24_to_s32(AgsAudioBufferUtil *audio_buffer_util,
					   gint32 *destination, guint destination_stride,
					   gint32 *source, guint source_stride,
					   guint count);
void ags_audio_buffer_util_copy_s24_to_s64(AgsAudioBufferUtil *audio_buffer_util,
					   gint64 *destination, guint destination_stride,
					   gint32 *source, guint source_stride,
					   guint count);
void ags_audio_buffer_util_copy_s24_to_float(AgsAudioBufferUtil *audio_buffer_util,
					     gfloat *destination, guint destination_stride,
					     gint32 *source, guint source_stride,
					     guint count);
void ags_audio_buffer_util_copy_s24_to_double(AgsAudioBufferUtil *audio_buffer_util,
					      gdouble *destination, guint destination_stride,
					      gint32 *source, guint source_stride,
					      guint count);

/* copy 32 bit */
void ags_audio_buffer_util_copy_s32_to_s8(AgsAudioBufferUtil *audio_buffer_util,
					  gint8 *destination, guint destination_stride,
					  gint32 *source, guint source_stride,
					  guint count);
void ags_audio_buffer_util_copy_s32_to_s16(AgsAudioBufferUtil *audio_buffer_util,
					   gint16 *destination, guint destination_stride,
					   gint32 *source, guint source_stride,
					   guint count);
void ags_audio_buffer_util_copy_s32_to_s24(AgsAudioBufferUtil *audio_buffer_util,
					   gint32 *destination, guint destination_stride,
					   gint32 *source, guint source_stride,
					   guint count);
void ags_audio_buffer_util_copy_s32_to_s32(AgsAudioBufferUtil *audio_buffer_util,
					   gint32 *destination, guint destination_stride,
					   gint32 *source, guint source_stride,
					   guint count);
void ags_audio_buffer_util_copy_s32_to_s64(AgsAudioBufferUtil *audio_buffer_util,
					   gint64 *destination, guint destination_stride,
					   gint32 *source, guint source_stride,
					   guint count);
void ags_audio_buffer_util_copy_s32_to_float(AgsAudioBufferUtil *audio_buffer_util,
					     gfloat *destination, guint destination_stride,
					     gint32 *source, guint source_stride,
					     guint count);
void ags_audio_buffer_util_copy_s32_to_double(AgsAudioBufferUtil *audio_buffer_util,
					      gdouble *destination, guint destination_stride,
					      gint32 *source, guint source_stride,
					      guint count);

/* copy 64 bit */
void ags_audio_buffer_util_copy_s64_to_s8(AgsAudioBufferUtil *audio_buffer_util,
					  gint8 *destination, guint destination_stride,
					  gint64 *source, guint source_stride,
					  guint count);
void ags_audio_buffer_util_copy_s64_to_s16(AgsAudioBufferUtil *audio_buffer_util,
					   gint16 *destination, guint destination_stride,
					   gint64 *source, guint source_stride,
					   guint count);
void ags_audio_buffer_util_copy_s64_to_s24(AgsAudioBufferUtil *audio_buffer_util,
					   gint32 *destination, guint destination_stride,
					   gint64 *source, guint source_stride,
					   guint count);
void ags_audio_buffer_util_copy_s64_to_s32(AgsAudioBufferUtil *audio_buffer_util,
					   gint32 *destination, guint destination_stride,
					   gint64 *source, guint source_stride,
					   guint count);
void ags_audio_buffer_util_copy_s64_to_s64(AgsAudioBufferUtil *audio_buffer_util,
					   gint64 *destination, guint destination_stride,
					   gint64 *source, guint source_stride,
					   guint count);
void ags_audio_buffer_util_copy_s64_to_float(AgsAudioBufferUtil *audio_buffer_util,
					     gfloat *destination, guint destination_stride,
					     gint64 *source, guint source_stride,
					     guint count);
void ags_audio_buffer_util_copy_s64_to_double(AgsAudioBufferUtil *audio_buffer_util,
					      gdouble *destination, guint destination_stride,
					      gint64 *source, guint source_stride,
					      guint count);

/* copy float */
void ags_audio_buffer_util_copy_float_to_s8(AgsAudioBufferUtil *audio_buffer_util,
					    gint8 *destination, guint destination_stride,
					    gfloat *source, guint source_stride,
					    guint count);
void ags_audio_buffer_util_copy_float_to_s16(AgsAudioBufferUtil *audio_buffer_util,
					     gint16 *destination, guint destination_stride,
					     gfloat *source, guint source_stride,
					     guint count);
void ags_audio_buffer_util_copy_float_to_s24(AgsAudioBufferUtil *audio_buffer_util,
					     gint32 *destination, guint destination_stride,
					     gfloat *source, guint source_stride,
					     guint count);
void ags_audio_buffer_util_copy_float_to_s32(AgsAudioBufferUtil *audio_buffer_util,
					     gint32 *destination, guint destination_stride,
					     gfloat *source, guint source_stride,
					     guint count);
void ags_audio_buffer_util_copy_float_to_s64(AgsAudioBufferUtil *audio_buffer_util,
					     gint64 *destination, guint destination_stride,
					     gfloat *source, guint source_stride,
					     guint count);
void ags_audio_buffer_util_copy_float_to_float(AgsAudioBufferUtil *audio_buffer_util,
					       gfloat *destination, guint destination_stride,
					       gfloat *source, guint source_stride,
					       guint count);
void ags_audio_buffer_util_copy_float_to_double(AgsAudioBufferUtil *audio_buffer_util,
						gdouble *destination, guint destination_stride,
						gfloat *source, guint source_stride,
						guint count);

/* copy double */
void ags_audio_buffer_util_copy_double_to_s8(AgsAudioBufferUtil *audio_buffer_util,
					     gint8 *destination, guint destination_stride,
					     gdouble *source, guint source_stride,
					     guint count);
void ags_audio_buffer_util_copy_double_to_s16(AgsAudioBufferUtil *audio_buffer_util,
					      gint16 *destination, guint destination_stride,
					      gdouble *source, guint source_stride,
					      guint count);
void ags_audio_buffer_util_copy_double_to_s24(AgsAudioBufferUtil *audio_buffer_util,
					      gint32 *destination, guint destination_stride,
					      gdouble *source, guint source_stride,
					      guint count);
void ags_audio_buffer_util_copy_double_to_s32(AgsAudioBufferUtil *audio_buffer_util,
					      gint32 *destination, guint destination_stride,
					      gdouble *source, guint source_stride,
					      guint count);
void ags_audio_buffer_util_copy_double_to_s64(AgsAudioBufferUtil *audio_buffer_util,
					      gint64 *destination, guint destination_stride,
					      gdouble *source, guint source_stride,
					      guint count);
void ags_audio_buffer_util_copy_double_to_float(AgsAudioBufferUtil *audio_buffer_util,
						gfloat *destination, guint destination_stride,
						gdouble *source, guint source_stride,
						guint count);
void ags_audio_buffer_util_copy_double_to_double(AgsAudioBufferUtil *audio_buffer_util,
						 gdouble *destination, guint destination_stride,
						 gdouble *source, guint source_stride,
						 guint count);

#ifdef __APPLE__
/* Float32 */
void ags_audio_buffer_util_copy_s8_to_float32(AgsAudioBufferUtil *audio_buffer_util,
					      Float32 *destination, guint destination_stride,
					      gint8 *source, guint source_stride,
					      guint count);

void ags_audio_buffer_util_copy_s16_to_float32(AgsAudioBufferUtil *audio_buffer_util,
					       Float32 *destination, guint destination_stride,
					       gint16 *source, guint source_stride,
					       guint count);

void ags_audio_buffer_util_copy_s24_to_float32(AgsAudioBufferUtil *audio_buffer_util,
					       Float32 *destination, guint destination_stride,
					       gint32 *source, guint source_stride,
					       guint count);

void ags_audio_buffer_util_copy_s32_to_float32(AgsAudioBufferUtil *audio_buffer_util,
					       Float32 *destination, guint destination_stride,
					       gint32 *source, guint source_stride,
					       guint count);

void ags_audio_buffer_util_copy_s64_to_float32(AgsAudioBufferUtil *audio_buffer_util,
					       Float32 *destination, guint destination_stride,
					       gint64 *source, guint source_stride,
					       guint count);

void ags_audio_buffer_util_copy_float_to_float32(AgsAudioBufferUtil *audio_buffer_util,
						 Float32 *destination, guint destination_stride,
						 float *source, guint source_stride,
						 guint count);

void ags_audio_buffer_util_copy_double_to_float32(AgsAudioBufferUtil *audio_buffer_util,
						  Float32 *destination, guint destination_stride,
						  gdouble *source, guint source_stride,
						  guint count);

void ags_audio_buffer_util_copy_float32_to_float32(AgsAudioBufferUtil *audio_buffer_util,
						   Float32 *destination, guint destination_stride,
						   Float32 *source, guint source_stride,
						   guint count);

void ags_audio_buffer_util_copy_float32_to_s8(AgsAudioBufferUtil *audio_buffer_util,
					      gint8 *destination, guint destination_stride,
					      Float32 *source, guint source_stride,
					      guint count);
void ags_audio_buffer_util_copy_float32_to_s16(AgsAudioBufferUtil *audio_buffer_util,
					       gint16 *destination, guint destination_stride,
					       Float32 *source, guint source_stride,
					       guint count);
void ags_audio_buffer_util_copy_float32_to_s24(AgsAudioBufferUtil *audio_buffer_util,
					       gint32 *destination, guint destination_stride,
					       Float32 *source, guint source_stride,
					       guint count);
void ags_audio_buffer_util_copy_float32_to_s32(AgsAudioBufferUtil *audio_buffer_util,
					       gint32 *destination, guint destination_stride,
					       Float32 *source, guint source_stride,
					       guint count);
void ags_audio_buffer_util_copy_float32_to_s64(AgsAudioBufferUtil *audio_buffer_util,
					       gint64 *destination, guint destination_stride,
					       Float32 *source, guint source_stride,
					       guint count);
void ags_audio_buffer_util_copy_float32_to_float(AgsAudioBufferUtil *audio_buffer_util,
						 gfloat *destination, guint destination_stride,
						 Float32 *source, guint source_stride,
						 guint count);
void ags_audio_buffer_util_copy_float32_to_double(AgsAudioBufferUtil *audio_buffer_util,
						  gdouble *destination, guint destination_stride,
						  Float32 *source, guint source_stride,
						  guint count);
#endif

/* AgsComplex */
void ags_audio_buffer_util_copy_s8_to_complex(AgsAudioBufferUtil *audio_buffer_util,
					      AgsComplex *destination, guint destination_stride,
					      gint8 *source, guint source_stride,
					      guint count);

void ags_audio_buffer_util_copy_s16_to_complex(AgsAudioBufferUtil *audio_buffer_util,
					       AgsComplex *destination, guint destination_stride,
					       gint16 *source, guint source_stride,
					       guint count);

void ags_audio_buffer_util_copy_s24_to_complex(AgsAudioBufferUtil *audio_buffer_util,
					       AgsComplex *destination, guint destination_stride,
					       gint32 *source, guint source_stride,
					       guint count);

void ags_audio_buffer_util_copy_s32_to_complex(AgsAudioBufferUtil *audio_buffer_util,
					       AgsComplex *destination, guint destination_stride,
					       gint32 *source, guint source_stride,
					       guint count);

void ags_audio_buffer_util_copy_s64_to_complex(AgsAudioBufferUtil *audio_buffer_util,
					       AgsComplex *destination, guint destination_stride,
					       gint64 *source, guint source_stride,
					       guint count);

void ags_audio_buffer_util_copy_float_to_complex(AgsAudioBufferUtil *audio_buffer_util,
						 AgsComplex *destination, guint destination_stride,
						 gfloat *source, guint source_stride,
						 guint count);

void ags_audio_buffer_util_copy_double_to_complex(AgsAudioBufferUtil *audio_buffer_util,
						  AgsComplex *destination, guint destination_stride,
						  gdouble *source, guint source_stride,
						  guint count);

#ifdef __APPLE__
void ags_audio_buffer_util_copy_float32_to_complex(AgsAudioBufferUtil *audio_buffer_util,
						   AgsComplex *destination, guint destination_stride,
						   Float32 *source, guint source_stride,
						   guint count);
#endif

void ags_audio_buffer_util_copy_complex_to_complex(AgsAudioBufferUtil *audio_buffer_util,
						   AgsComplex *destination, guint destination_stride,
						   AgsComplex *source, guint source_stride,
						   guint count);

void ags_audio_buffer_util_copy_complex_to_s8(AgsAudioBufferUtil *audio_buffer_util,
					      gint8 *destination, guint destination_stride,
					      AgsComplex *source, guint source_stride,
					      guint count);
void ags_audio_buffer_util_copy_complex_to_s16(AgsAudioBufferUtil *audio_buffer_util,
					       gint16 *destination, guint destination_stride,
					       AgsComplex *source, guint source_stride,
					       guint count);
void ags_audio_buffer_util_copy_complex_to_s24(AgsAudioBufferUtil *audio_buffer_util,
					       gint32 *destination, guint destination_stride,
					       AgsComplex *source, guint source_stride,
					       guint count);
void ags_audio_buffer_util_copy_complex_to_s32(AgsAudioBufferUtil *audio_buffer_util,
					       gint32 *destination, guint destination_stride,
					       AgsComplex *source, guint source_stride,
					       guint count);
void ags_audio_buffer_util_copy_complex_to_s64(AgsAudioBufferUtil *audio_buffer_util,
					       gint64 *destination, guint destination_stride,
					       AgsComplex *source, guint source_stride,
					       guint count);
void ags_audio_buffer_util_copy_complex_to_float(AgsAudioBufferUtil *audio_buffer_util,
						 gfloat *destination, guint destination_stride,
						 AgsComplex *source, guint source_stride,
						 guint count);
void ags_audio_buffer_util_copy_complex_to_double(AgsAudioBufferUtil *audio_buffer_util,
						  gdouble *destination, guint destination_stride,
						  AgsComplex *source, guint source_stride,
						  guint count);
#ifdef __APPLE__
void ags_audio_buffer_util_copy_complex_to_float32(AgsAudioBufferUtil *audio_buffer_util,
						   Float32 *destination, guint destination_stride,
						   AgsComplex *source, guint source_stride,
						   guint count);
#endif

/* vectors */
void ags_audio_buffer_util_fill_v8s8(AgsAudioBufferUtil *audio_buffer_util,
				     ags_v8s8 *destination, guint destination_stride,
				     gint8 *source, guint source_stride,
				     guint count);
void ags_audio_buffer_util_fill_v8s16(AgsAudioBufferUtil *audio_buffer_util,
				      ags_v8s16 *destination, guint destination_stride,
				      gint16 *source, guint source_stride,
				      guint count);
void ags_audio_buffer_util_fill_v8s32(AgsAudioBufferUtil *audio_buffer_util,
				      ags_v8s32 *destination, guint destination_stride,
				      gint32 *source, guint source_stride,
				      guint count);
void ags_audio_buffer_util_fill_v8s64(AgsAudioBufferUtil *audio_buffer_util,
				      ags_v8s64 *destination, guint destination_stride,
				      gint64 *source, guint source_stride,
				      guint count);
void ags_audio_buffer_util_fill_v8float(AgsAudioBufferUtil *audio_buffer_util,
					ags_v8float *destination, guint destination_stride,
					gfloat *source, guint source_stride,
					guint count);
void ags_audio_buffer_util_fill_v8double(AgsAudioBufferUtil *audio_buffer_util,
					 ags_v8double *destination, guint destination_stride,
					 gdouble *source, guint source_stride,
					 guint count);

void ags_audio_buffer_util_fetch_v8s8(AgsAudioBufferUtil *audio_buffer_util,
				      gint8 *destination, guint destination_stride,
				      ags_v8s8 *source, guint source_stride,
				      guint count);
void ags_audio_buffer_util_fetch_v8s16(AgsAudioBufferUtil *audio_buffer_util,
				       gint16 *destination, guint destination_stride,
				       ags_v8s16 *source, guint source_stride,
				       guint count);
void ags_audio_buffer_util_fetch_v8s32(AgsAudioBufferUtil *audio_buffer_util,
				       gint32 *destination, guint destination_stride,
				       ags_v8s32 *source, guint source_stride,
				       guint count);
void ags_audio_buffer_util_fetch_v8s64(AgsAudioBufferUtil *audio_buffer_util,
				       gint64 *destination, guint destination_stride,
				       ags_v8s64 *source, guint source_stride,
				       guint count);
void ags_audio_buffer_util_fetch_v8float(AgsAudioBufferUtil *audio_buffer_util,
					 gfloat *destination, guint destination_stride,
					 ags_v8float *source, guint source_stride,
					 guint count);
void ags_audio_buffer_util_fetch_v8double(AgsAudioBufferUtil *audio_buffer_util,
					  gdouble *destination, guint destination_stride,
					  ags_v8double *source, guint source_stride,
					  guint count);

/* double vectors */
void ags_audio_buffer_util_fill_v8double_from_s8(AgsAudioBufferUtil *audio_buffer_util,
						 ags_v8double *destination, guint destination_stride,
						 gint8 *source, guint source_stride,
						 guint count);
void ags_audio_buffer_util_fill_v8double_from_s16(AgsAudioBufferUtil *audio_buffer_util,
						  ags_v8double *destination, guint destination_stride,
						  gint16 *source, guint source_stride,
						  guint count);
void ags_audio_buffer_util_fill_v8double_from_s32(AgsAudioBufferUtil *audio_buffer_util,
						  ags_v8double *destination, guint destination_stride,
						  gint32 *source, guint source_stride,
						  guint count);
void ags_audio_buffer_util_fill_v8double_from_s64(AgsAudioBufferUtil *audio_buffer_util,
						  ags_v8double *destination, guint destination_stride,
						  gint64 *source, guint source_stride,
						  guint count);
void ags_audio_buffer_util_fill_v8double_from_float(AgsAudioBufferUtil *audio_buffer_util,
						    ags_v8double *destination, guint destination_stride,
						    gfloat *source, guint source_stride,
						    guint count);

void ags_audio_buffer_util_fetch_v8double_as_s8(AgsAudioBufferUtil *audio_buffer_util,
						gint8 *destination, guint destination_stride,
						ags_v8double *source, guint source_stride,
						guint count);
void ags_audio_buffer_util_fetch_v8double_as_s16(AgsAudioBufferUtil *audio_buffer_util,
						 gint16 *destination, guint destination_stride,
						 ags_v8double *source, guint source_stride,
						 guint count);
void ags_audio_buffer_util_fetch_v8double_as_s32(AgsAudioBufferUtil *audio_buffer_util,
						 gint32 *destination, guint destination_stride,
						 ags_v8double *source, guint source_stride,
						 guint count);
void ags_audio_buffer_util_fetch_v8double_as_s64(AgsAudioBufferUtil *audio_buffer_util,
						 gint64 *destination, guint destination_stride,
						 ags_v8double *source, guint source_stride,
						 guint count);
void ags_audio_buffer_util_fetch_v8double_as_float(AgsAudioBufferUtil *audio_buffer_util,
						   gfloat *destination, guint destination_stride,
						   ags_v8double *source, guint source_stride,
						   guint count);

/* copy */
void ags_audio_buffer_util_copy_buffer_to_buffer(AgsAudioBufferUtil *audio_buffer_util,
						 void *destination, guint destination_stride, guint doffset,
						 void *source, guint source_stride, guint soffset,
						 guint count, AgsAudioBufferUtilCopyMode mode);

G_END_DECLS

#endif /*__AGS_AUDIO_BUFFER_UTIL_H__*/
