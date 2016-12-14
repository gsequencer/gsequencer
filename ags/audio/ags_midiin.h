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

#ifndef __AGS_MIDIIN_H__
#define __AGS_MIDIIN_H__

#include <glib.h>
#include <glib-object.h>

#include <sys/types.h>

#include <pthread.h>

#include <alsa/asoundlib.h>

#define AGS_TYPE_MIDIIN                (ags_midiin_get_type())
#define AGS_MIDIIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIDIIN, AgsMidiin))
#define AGS_MIDIIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_MIDIIN, AgsMidiin))
#define AGS_IS_MIDIIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MIDIIN))
#define AGS_IS_MIDIIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MIDIIN))
#define AGS_MIDIIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_MIDIIN, AgsMidiinClass))

#define AGS_MIDIIN_DEFAULT_ALSA_DEVICE "hw:0,0\0"
#define AGS_MIDIIN_DEFAULT_OSS_DEVICE "/dev/midi00\0"
#define AGS_MIDIIN_DEFAULT_BUFFER_SIZE (256)

typedef struct _AgsMidiin AgsMidiin;
typedef struct _AgsMidiinClass AgsMidiinClass;

typedef enum
{
  AGS_MIDIIN_BUFFER0                        = 1,
  AGS_MIDIIN_BUFFER1                        = 1 << 1,
  AGS_MIDIIN_BUFFER2                        = 1 << 2,
  AGS_MIDIIN_BUFFER3                        = 1 << 3,
  AGS_MIDIIN_ATTACK_FIRST                   = 1 << 4,

  AGS_MIDIIN_RECORD                         = 1 << 5,

  AGS_MIDIIN_OSS                            = 1 << 6,
  AGS_MIDIIN_ALSA                           = 1 << 7,

  AGS_MIDIIN_SHUTDOWN                       = 1 << 8,
  AGS_MIDIIN_START_RECORD                   = 1 << 9,

  AGS_MIDIIN_NONBLOCKING                    = 1 << 10,
  AGS_MIDIIN_INITIALIZED                    = 1 << 11,
}AgsMidiinFlags;

typedef enum{
  AGS_MIDIIN_PASS_THROUGH                   = 1,
  AGS_MIDIIN_INITIAL_POLL                   = 1 <<  1,
  AGS_MIDIIN_POLL_WAIT                      = 1 <<  2,
  AGS_MIDIIN_POLL_DONE                      = 1 <<  3,
  AGS_MIDIIN_POLL_FINISH_WAIT               = 1 <<  4,
  AGS_MIDIIN_POLL_FINISH_DONE               = 1 <<  5,
  AGS_MIDIIN_POLL_SWITCH_BUFFER             = 1 <<  6,
}AgsMidiinSyncFlags;

#define AGS_MIDIIN_ERROR (ags_midiin_error_quark())

typedef enum{
  AGS_MIDIIN_ERROR_LOCKED_SEQUENCER,
}AgsMidiinError;

struct _AgsMidiin
{
  GObject object;

  guint flags;
  volatile guint sync_flags;
  
  char **ring_buffer;
  guint ring_buffer_size[2];
  
  char **buffer;
  guint buffer_size[4];

  double bpm; // beats per minute

  gdouble delay;
  gdouble delay_factor;
  guint latency;
  
  gdouble tact_counter;
  gdouble delay_counter; // next time attack changeing when delay_counter == delay
  guint tic_counter; // in the range of default period

  guint note_offset;
  
  union{
    struct _AgsOssMidi{
      int device_fd;
      char *device;
    }oss;
    struct _AgsAlsaMidi{
      char *device;
      int rc;
      snd_rawmidi_t *handle;
    }alsa;
  }in;

  pthread_mutex_t *poll_mutex;
  pthread_cond_t *poll_cond;

  pthread_mutex_t *poll_finish_mutex;
  pthread_cond_t *poll_finish_cond;

  GObject *application_context;
  pthread_mutex_t *application_mutex;
  
  GList *audio;
};

struct _AgsMidiinClass
{
  GObjectClass object;
};

GType ags_midiin_get_type();

GQuark ags_midiin_error_quark();

AgsMidiin* ags_midiin_new(GObject *application_context);

#endif /*__AGS_MIDIIN_H__*/
