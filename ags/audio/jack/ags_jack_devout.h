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

#ifndef __AGS_JACK_DEVOUT_H__
#define __AGS_JACK_DEVOUT_H__

#include <glib.h>
#include <glib-object.h>

#include <pthread.h>

#include <jack/jack.h>

#define AGS_TYPE_JACK_DEVOUT                (ags_jack_devout_get_type())
#define AGS_JACK_DEVOUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_JACK_DEVOUT, AgsJackDevout))
#define AGS_JACK_DEVOUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_JACK_DEVOUT, AgsJackDevout))
#define AGS_IS_JACK_DEVOUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_JACK_DEVOUT))
#define AGS_IS_JACK_DEVOUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_JACK_DEVOUT))
#define AGS_JACK_DEVOUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_JACK_DEVOUT, AgsJackDevoutClass))

typedef struct _AgsJackDevout AgsJackDevout;
typedef struct _AgsJackDevoutClass AgsJackDevoutClass;

typedef enum
{
  AGS_JACK_DEVOUT_BUFFER0                        = 1,
  AGS_JACK_DEVOUT_BUFFER1                        = 1 <<  1,
  AGS_JACK_DEVOUT_BUFFER2                        = 1 <<  2,
  AGS_JACK_DEVOUT_BUFFER3                        = 1 <<  3,

  AGS_JACK_DEVOUT_ATTACK_FIRST                   = 1 <<  4,

  AGS_JACK_DEVOUT_PLAY                           = 1 <<  5,
  AGS_JACK_DEVOUT_SHUTDOWN                       = 1 <<  6,
  AGS_JACK_DEVOUT_START_PLAY                     = 1 <<  7,

  AGS_JACK_DEVOUT_NONBLOCKING                    = 1 <<  8,
  AGS_JACK_DEVOUT_INITIALIZED                    = 1 <<  9,

  AGS_JACK_DEVOUT_DUMMY                          = 1 << 10,
  AGS_JACK_DEVOUT_OSS                            = 1 << 11,
  AGS_JACK_DEVOUT_ALSA                           = 1 << 12,
  AGS_JACK_DEVOUT_FREEBOB                        = 1 << 13,
  AGS_JACK_DEVOUT_FIREWIRE                       = 1 << 14,
  AGS_JACK_DEVOUT_NET                            = 1 << 15,
  AGS_JACK_DEVOUT_SUN                            = 1 << 16,
  AGS_JACK_DEVOUT_PORTAUDIO                      = 1 << 17,
}AgsJackDevoutFlags;

typedef enum{
  AGS_JACK_DEVOUT_PASS_THROUGH                   = 1,
  AGS_JACK_DEVOUT_CALLBACK_WAIT                  = 1 <<  1,
  AGS_JACK_DEVOUT_CALLBACK_DONE                  = 1 <<  2,
}AgsJackDevoutSyncFlags;

#define AGS_JACK_DEVOUT_ERROR (ags_jack_devout_error_quark())

typedef enum{
  AGS_JACK_DEVOUT_ERROR_LOCKED_SOUNDCARD,
}AgsJackDevoutError;

struct _AgsJackDevout
{
  GObject object;

  guint flags;
  volatile guint sync_flags;
  
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

  guint loop_left;
  guint loop_right;
  gboolean do_loop;
  
  guint loop_offset;

  gchar *card_uri;
  GObject *jack_port;

  pthread_mutex_t *callback_mutex;
  pthread_cond_t *callback_cond;
  
  GObject *application_context;
  pthread_mutex_t *application_mutex;
  
  GList *audio;
};

struct _AgsJackDevoutClass
{
  GObjectClass object;
};

GType ags_jack_devout_get_type();

GQuark ags_jack_devout_error_quark();

int ags_jack_devout_process_callback(jack_nframes_t nframes, void *ptr);

void ags_jack_devout_adjust_delay_and_attack(AgsJackDevout *jack_devout);
void ags_jack_devout_realloc_buffer(AgsJackDevout *jack_devout);

AgsJackDevout* ags_jack_devout_new(GObject *application_context);

#endif /*__AGS_JACK_DEVOUT_H__*/
