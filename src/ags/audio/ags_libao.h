/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_LIBAO_H__
#define __AGS_LIBAO_H__

#include <pthread.h>

#include <glib.h>
#include <glib-object.h>

#include <sys/types.h>

#include <alsa/asoundlib.h>

#include <ao/ao.h>

#include <ags/audio/ags_recall_id.h>

#define AGS_TYPE_LIBAO                (ags_libao_get_type())
#define AGS_LIBAO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LIBAO, AgsLibao))
#define AGS_LIBAO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_LIBAO, AgsLibao))
#define AGS_IS_LIBAO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LIBAO))
#define AGS_IS_LIBAO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LIBAO))
#define AGS_LIBAO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_LIBAO, AgsLibaoClass))

typedef struct _AgsLibao AgsLibao;
typedef struct _AgsLibaoClass AgsLibaoClass;

typedef enum{
  AGS_LIBAO_BUFFER0                        = 1,
  AGS_LIBAO_BUFFER1                        = 1 << 1,
  AGS_LIBAO_BUFFER2                        = 1 << 2,
  AGS_LIBAO_BUFFER3                        = 1 << 3,

  AGS_LIBAO_ATTACK_FIRST                   = 1 << 4,

  AGS_LIBAO_PLAY                           = 1 << 5,

  AGS_LIBAO_LIBAO                          = 1 << 6,
  AGS_LIBAO_OSS                            = 1 << 7,
  AGS_LIBAO_ALSA                           = 1 << 8,

  AGS_LIBAO_SHUTDOWN                       = 1 << 9,
  AGS_LIBAO_START_PLAY                     = 1 << 10,

  AGS_LIBAO_NONBLOCKING                    = 1 << 11,

  AGS_LIBAO_TIMING_SET_0                   = 1 << 12,
  AGS_LIBAO_TIMING_SET_1                   = 1 << 13,
}AgsLibaoFlags;

#define AGS_LIBAO_ERROR (ags_libao_error_quark())

typedef enum{
  AGS_LIBAO_ERROR_LOCKED_SOUNDCARD,
}AgsLibaoError;

struct _AgsLibao
{
  GObject object;

  guint flags;

  guint dsp_channels;
  guint pcm_channels;
  guint bits;
  guint buffer_size;
  guint frequency; // sample_rate

  signed short** buffer;

  double bpm; // beats per minute

  gdouble *delay; // count of tics within buffer size
  guint *attack; // where currently tic resides in the stream's offset, measured in 1/64 of bpm

  gdouble delay_counter; // next time attack changeing when delay_counter == delay
  guint tic_counter;

  union{
    struct _AgsAO{
      ao_device *device;
      ao_sample_format *format;
      int driver_ao;
    }ao;
  }out;

  GObject *application_context;
  
  GList *audio;
};

struct _AgsLibaoClass
{
  GObjectClass object;
};

GType ags_libao_get_type();

GQuark ags_libao_error_quark();

AgsLibao* ags_libao_new(GObject *application_context);

#endif /*__AGS_LIBAO_H__*/

