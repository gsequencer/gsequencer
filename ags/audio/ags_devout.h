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

#ifndef __AGS_DEVOUT_H__
#define __AGS_DEVOUT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_time.h>

#include <sys/types.h>

#include <pthread.h>

#include <ags/config.h>

#ifdef AGS_WITH_ALSA
#include <alsa/asoundlib.h>
#endif

#define AGS_TYPE_DEVOUT                (ags_devout_get_type())
#define AGS_DEVOUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DEVOUT, AgsDevout))
#define AGS_DEVOUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_DEVOUT, AgsDevout))
#define AGS_IS_DEVOUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_DEVOUT))
#define AGS_IS_DEVOUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_DEVOUT))
#define AGS_DEVOUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_DEVOUT, AgsDevoutClass))

#define AGS_DEVOUT_DEFAULT_ALSA_DEVICE "hw:0,0\0"
#define AGS_DEVOUT_DEFAULT_OSS_DEVICE "/dev/dsp\0"

typedef struct _AgsDevout AgsDevout;
typedef struct _AgsDevoutClass AgsDevoutClass;

typedef enum
{
  AGS_DEVOUT_BUFFER0                        = 1,
  AGS_DEVOUT_BUFFER1                        = 1 <<  1,
  AGS_DEVOUT_BUFFER2                        = 1 <<  2,
  AGS_DEVOUT_BUFFER3                        = 1 <<  3,

  AGS_DEVOUT_ATTACK_FIRST                   = 1 <<  4,

  AGS_DEVOUT_PLAY                           = 1 <<  5,

  AGS_DEVOUT_OSS                            = 1 <<  6,
  AGS_DEVOUT_ALSA                           = 1 <<  7,

  AGS_DEVOUT_SHUTDOWN                       = 1 <<  8,
  AGS_DEVOUT_START_PLAY                     = 1 <<  9,

  AGS_DEVOUT_NONBLOCKING                    = 1 << 10,
  AGS_DEVOUT_INITIALIZED                    = 1 << 11,
}AgsDevoutFlags;

#define AGS_DEVOUT_ERROR (ags_devout_error_quark())

typedef enum{
  AGS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
  AGS_DEVOUT_ERROR_BROKEN_CONFIGURATION,
  AGS_DEVOUT_ERROR_ACCESS_TYPE_NOT_AVAILABLE,
  AGS_DEVOUT_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
  AGS_DEVOUT_ERROR_CHANNELS_NOT_AVAILABLE,
  AGS_DEVOUT_ERROR_SAMPLERATE_NOT_AVAILABLE,
  AGS_DEVOUT_ERROR_BUFFER_SIZE_NOT_AVAILABLE,
  AGS_DEVOUT_ERROR_HW_PARAMETERS_NOT_AVAILABLE,
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

  void** buffer;
  unsigned char **ring_buffer;

  double bpm; // beats per minute
  gdouble delay_factor;
  
  gdouble *delay; // count of tics within buffer size
  guint *attack; // where currently tic resides in the stream's offset, measured in 1/64 of bpm

  gdouble tact_counter;
  gdouble delay_counter; // next time attack changeing when delay_counter == delay
  guint tic_counter; // in the range of default period

  guint note_offset;

  guint loop_left;
  guint loop_right;
  gboolean do_loop;
  
  guint loop_offset;
  
  union{
    struct _AgsOss{
      int device_fd;
      char *device;
    }oss;
#ifdef AGS_WITH_ALSA
    struct _AgsAlsa{
      char *device;
      int rc;
      snd_pcm_t *handle;
      snd_async_handler_t *ahandler;
      snd_pcm_hw_params_t *params;
    }alsa;
#else
    struct _AgsAlsaDummy{
      char *device;
      int rc;
      void *handle;
      void *ahandler;
      void *params;
    }alsa;
#endif
  }out;

  GObject *application_context;
  pthread_mutex_t *application_mutex;

  GList *poll_fd;
  
  GList *audio;
};

struct _AgsDevoutClass
{
  GObjectClass object;
};

GType ags_devout_get_type();

GQuark ags_devout_error_quark();

void ags_devout_switch_buffer_flag(AgsDevout *devout);

void ags_devout_adjust_delay_and_attack(AgsDevout *devout);
void ags_devout_realloc_buffer(AgsDevout *devout);

AgsDevout* ags_devout_new(GObject *application_context);

#endif /*__AGS_DEVOUT_H__*/
