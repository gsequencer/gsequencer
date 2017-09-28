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

#ifndef __AGS_CORE_AUDIO_DEVOUT_H__
#define __AGS_CORE_AUDIO_DEVOUT_H__

#include <glib.h>
#include <glib-object.h>

#include <pthread.h>

#define AGS_TYPE_CORE_AUDIO_DEVOUT                (ags_core_audio_devout_get_type())
#define AGS_CORE_AUDIO_DEVOUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CORE_AUDIO_DEVOUT, AgsCoreAudioDevout))
#define AGS_CORE_AUDIO_DEVOUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_CORE_AUDIO_DEVOUT, AgsCoreAudioDevout))
#define AGS_IS_CORE_AUDIO_DEVOUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CORE_AUDIO_DEVOUT))
#define AGS_IS_CORE_AUDIO_DEVOUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CORE_AUDIO_DEVOUT))
#define AGS_CORE_AUDIO_DEVOUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_CORE_AUDIO_DEVOUT, AgsCoreAudioDevoutClass))

typedef struct _AgsCoreAudioDevout AgsCoreAudioDevout;
typedef struct _AgsCoreAudioDevoutClass AgsCoreAudioDevoutClass;

typedef enum
{
  AGS_CORE_AUDIO_DEVOUT_BUFFER0                        = 1,
  AGS_CORE_AUDIO_DEVOUT_BUFFER1                        = 1 <<  1,
  AGS_CORE_AUDIO_DEVOUT_BUFFER2                        = 1 <<  2,
  AGS_CORE_AUDIO_DEVOUT_BUFFER3                        = 1 <<  3,
  AGS_CORE_AUDIO_DEVOUT_BUFFER4                        = 1 <<  4,
  AGS_CORE_AUDIO_DEVOUT_BUFFER5                        = 1 <<  5,
  AGS_CORE_AUDIO_DEVOUT_BUFFER6                        = 1 <<  6,
  AGS_CORE_AUDIO_DEVOUT_BUFFER7                        = 1 <<  7,

  AGS_CORE_AUDIO_DEVOUT_ATTACK_FIRST                   = 1 <<  8,

  AGS_CORE_AUDIO_DEVOUT_PLAY                           = 1 <<  9,
  AGS_CORE_AUDIO_DEVOUT_SHUTDOWN                       = 1 << 10,
  AGS_CORE_AUDIO_DEVOUT_START_PLAY                     = 1 << 11,

  AGS_CORE_AUDIO_DEVOUT_NONBLOCKING                    = 1 << 12,
  AGS_CORE_AUDIO_DEVOUT_INITIALIZED                    = 1 << 13,
}AgsCoreAudioDevoutFlags;

typedef enum{
  AGS_CORE_AUDIO_DEVOUT_PASS_THROUGH                   = 1,
  AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK               = 1 <<  1,
  AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT                  = 1 <<  2,
  AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE                  = 1 <<  3,
  AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_WAIT           = 1 <<  4,
  AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_DONE           = 1 <<  5,
}AgsCoreAudioDevoutSyncFlags;

#define AGS_CORE_AUDIO_DEVOUT_ERROR (ags_core_audio_devout_error_quark())

typedef enum{
  AGS_CORE_AUDIO_DEVOUT_ERROR_LOCKED_SOUNDCARD,
}AgsCoreAudioDevoutError;

struct _AgsCoreAudioDevout
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
  GObject *core_audio_client;

  gchar **port_name;
  GList *core_audio_port;

  pthread_mutex_t *callback_mutex;
  pthread_cond_t *callback_cond;

  pthread_mutex_t *callback_finish_mutex;
  pthread_cond_t *callback_finish_cond;

  GObject *application_context;
  pthread_mutex_t *application_mutex;

  GObject *notify_soundcard;

  GList *audio;
};

struct _AgsCoreAudioDevoutClass
{
  GObjectClass object;
};

GType ags_core_audio_devout_get_type();

GQuark ags_core_audio_devout_error_quark();

void ags_core_audio_devout_switch_buffer_flag(AgsCoreAudioDevout *core_audio_devout);

AgsCoreAudioDevout* ags_core_audio_devout_new(GObject *application_context);

#endif /*__AGS_CORE_AUDIO_DEVOUT_H__*/
