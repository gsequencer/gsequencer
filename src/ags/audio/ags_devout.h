/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_DEVOUT_H__
#define __AGS_DEVOUT_H__

#include <glib.h>
#include <glib-object.h>

#include <sys/types.h>

#include <pthread.h>

#include <alsa/asoundlib.h>

#define AGS_TYPE_DEVOUT                (ags_devout_get_type())
#define AGS_DEVOUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DEVOUT, AgsDevout))
#define AGS_DEVOUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_DEVOUT, AgsDevout))
#define AGS_IS_DEVOUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_DEVOUT))
#define AGS_IS_DEVOUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_DEVOUT))
#define AGS_DEVOUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_DEVOUT, AgsDevoutClass))

typedef struct _AgsDevout AgsDevout;
typedef struct _AgsDevoutClass AgsDevoutClass;
typedef struct _AgsDevoutPlayDomain AgsDevoutPlayDomain;

typedef enum
{
  AGS_DEVOUT_BUFFER0                        = 1,
  AGS_DEVOUT_BUFFER1                        = 1 << 1,
  AGS_DEVOUT_BUFFER2                        = 1 << 2,
  AGS_DEVOUT_BUFFER3                        = 1 << 3,

  AGS_DEVOUT_ATTACK_FIRST                   = 1 << 4,

  AGS_DEVOUT_PLAY                           = 1 << 5,

  AGS_DEVOUT_OSS                            = 1 << 6,
  AGS_DEVOUT_ALSA                           = 1 << 7,

  AGS_DEVOUT_SHUTDOWN                       = 1 << 8,
  AGS_DEVOUT_START_PLAY                     = 1 << 9,

  AGS_DEVOUT_NONBLOCKING                    = 1 << 10,
}AgsDevoutFlags;

#define AGS_DEVOUT_ERROR (ags_devout_error_quark())

typedef enum{
  AGS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
}AgsDevoutError;

struct _AgsDevout
{
  GObject object;

  guint flags;

  guint dsp_channels;
  guint pcm_channels;
  guint format;
  guint buffer_size;
  guint samplerate; // sample_rate

  signed short** buffer;

  gdouble bpm; // beats per minute

  gdouble *delay; // count of tics within buffer size
  guint *attack; // where currently tic resides in the stream's offset, measured in 1/64 of bpm

  gdouble delay_counter; // next time attack changeing when delay_counter == delay
  guint tic_counter; // in the range of default period

  guint note_offset;
  
  union{
    struct _AgsOss{
      int device_fd;
      char *device;
    }oss;
    struct _AgsAlsa{
      char *device;
      int rc;
      snd_pcm_t *handle;
      snd_async_handler_t *ahandler;
      snd_pcm_hw_params_t *params;
    }alsa;
  }out;

  GObject *application_context;
  pthread_mutex_t *application_mutex;
  
  GList *audio;
};

struct _AgsDevoutClass
{
  GObjectClass object;
};

GType ags_devout_get_type();

GQuark ags_devout_error_quark();

AgsDevout* ags_devout_new(GObject *application_context);

#endif /*__AGS_DEVOUT_H__*/
