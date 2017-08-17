/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_PULSE_DEVOUT_H__
#define __AGS_PULSE_DEVOUT_H__

#include <glib.h>
#include <glib-object.h>

#include <pthread.h>

#include <pulse/pulseaudio.h>
#include <pulse/stream.h>
#include <pulse/error.h>

#define AGS_TYPE_PULSE_DEVOUT                (ags_pulse_devout_get_type())
#define AGS_PULSE_DEVOUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PULSE_DEVOUT, AgsPulseDevout))
#define AGS_PULSE_DEVOUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PULSE_DEVOUT, AgsPulseDevout))
#define AGS_IS_PULSE_DEVOUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PULSE_DEVOUT))
#define AGS_IS_PULSE_DEVOUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PULSE_DEVOUT))
#define AGS_PULSE_DEVOUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_PULSE_DEVOUT, AgsPulseDevoutClass))

typedef struct _AgsPulseDevout AgsPulseDevout;
typedef struct _AgsPulseDevoutClass AgsPulseDevoutClass;

typedef enum
{
  AGS_PULSE_DEVOUT_BUFFER0                        = 1,
  AGS_PULSE_DEVOUT_BUFFER1                        = 1 <<  1,
  AGS_PULSE_DEVOUT_BUFFER2                        = 1 <<  2,
  AGS_PULSE_DEVOUT_BUFFER3                        = 1 <<  3,

  AGS_PULSE_DEVOUT_ATTACK_FIRST                   = 1 <<  4,

  AGS_PULSE_DEVOUT_PLAY                           = 1 <<  5,
  AGS_PULSE_DEVOUT_SHUTDOWN                       = 1 <<  6,
  AGS_PULSE_DEVOUT_START_PLAY                     = 1 <<  7,

  AGS_PULSE_DEVOUT_NONBLOCKING                    = 1 <<  8,
  AGS_PULSE_DEVOUT_INITIALIZED                    = 1 <<  9,
}AgsPulseDevoutFlags;

typedef enum{
  AGS_PULSE_DEVOUT_PASS_THROUGH                   = 1,
  AGS_PULSE_DEVOUT_INITIAL_CALLBACK               = 1 <<  1,
  AGS_PULSE_DEVOUT_CALLBACK_WAIT                  = 1 <<  2,
  AGS_PULSE_DEVOUT_CALLBACK_DONE                  = 1 <<  3,
  AGS_PULSE_DEVOUT_CALLBACK_FINISH_WAIT           = 1 <<  4,
  AGS_PULSE_DEVOUT_CALLBACK_FINISH_DONE           = 1 <<  5,
}AgsPulseDevoutSyncFlags;

#define AGS_PULSE_DEVOUT_ERROR (ags_pulse_devout_error_quark())

typedef enum{
  AGS_PULSE_DEVOUT_ERROR_LOCKED_SOUNDCARD,
}AgsPulseDevoutError;

struct _AgsPulseDevout
{
  GObject object;

  guint flags;
  volatile guint sync_flags;
  
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *mutexattr;

  guint dsp_channels;
  guint pcm_channels;
  guint format;
  guint buffer_size;
  guint samplerate;

  void** buffer;

  double bpm; // beats per minute
  gdouble delay_factor;
  
  gdouble *delay; // count of tics within buffer size
  guint *attack; // where currently tic resides in the stream's offset, measured in 1/64 of bpm

  gdouble tact_counter;
  gdouble delay_counter; // next time attack changeing when delay_counter == delay
  guint tic_counter; // in the range of default period

  guint note_offset;
  guint note_offset_absolute;
  
  guint loop_left;
  guint loop_right;
  gboolean do_loop;
  
  guint loop_offset;

  gchar *card_uri;
  GObject *pulse_client;

  gchar **port_name;
  GList *pulse_port;

  pthread_mutex_t *callback_mutex;
  pthread_cond_t *callback_cond;

  pthread_mutex_t *callback_finish_mutex;
  pthread_cond_t *callback_finish_cond;

  GObject *application_context;
  pthread_mutex_t *application_mutex;

  GObject *notify_soundcard;

  GList *audio;
};

struct _AgsPulseDevoutClass
{
  GObjectClass object;
};

GType ags_pulse_devout_get_type();

GQuark ags_pulse_devout_error_quark();

void ags_pulse_devout_switch_buffer_flag(AgsPulseDevout *pulse_devout);

AgsPulseDevout* ags_pulse_devout_new(GObject *application_context);

#endif /*__AGS_PULSE_DEVOUT_H__*/
