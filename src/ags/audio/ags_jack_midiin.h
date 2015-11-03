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

#ifndef __AGS_JACK_MIDIIN_H__
#define __AGS_JACK_MIDIIN_H__

#include <glib.h>
#include <glib-object.h>

#include <sys/types.h>

#include <pthread.h>

#define AGS_TYPE_JACK_MIDIIN                (ags_jack_midiin_get_type())
#define AGS_JACK_MIDIIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_JACK_MIDIIN, AgsJackMidiin))
#define AGS_JACK_MIDIIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_JACK_MIDIIN, AgsJackMidiin))
#define AGS_IS_JACK_MIDIIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_JACK_MIDIIN))
#define AGS_IS_JACK_MIDIIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_JACK_MIDIIN))
#define AGS_JACK_MIDIIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_JACK_MIDIIN, AgsJackMidiinClass))

#define AGS_JACK_MIDIIN_DEFAULT_BUFFER_SIZE (256)

typedef struct _AgsJackMidiin AgsJackMidiin;
typedef struct _AgsJackMidiinClass AgsJackMidiinClass;

typedef enum
{
  AGS_JACK_MIDIIN_BUFFER0                        = 1,
  AGS_JACK_MIDIIN_BUFFER1                        = 1 <<  1,
  AGS_JACK_MIDIIN_BUFFER2                        = 1 <<  2,
  AGS_JACK_MIDIIN_BUFFER3                        = 1 <<  3,
  AGS_JACK_MIDIIN_ATTACK_FIRST                   = 1 <<  4,

  AGS_JACK_MIDIIN_RECORD                         = 1 <<  5,

  AGS_JACK_MIDIIN_SHUTDOWN                       = 1 <<  6,
  AGS_JACK_MIDIIN_START_RECORD                   = 1 <<  7,

  AGS_JACK_MIDIIN_NONBLOCKING                    = 1 <<  8,
  AGS_JACK_MIDIIN_INITIALIZED                    = 1 <<  9,

  AGS_JACK_MIDIIN_DUMMY                          = 1 << 10,
  AGS_JACK_MIDIIN_OSS                            = 1 << 11,
  AGS_JACK_MIDIIN_ALSA                           = 1 << 12,
  AGS_JACK_MIDIIN_FREEBOB                        = 1 << 13,
  AGS_JACK_MIDIIN_FIREWIRE                       = 1 << 14,
  AGS_JACK_MIDIIN_NET                            = 1 << 15,
  AGS_JACK_MIDIIN_SUN                            = 1 << 16,
  AGS_JACK_MIDIIN_PORTAUDIO                      = 1 << 17,
}AgsJackMidiinFlags;

#define AGS_JACK_MIDIIN_ERROR (ags_jack_midiin_error_quark())

typedef enum{
  AGS_JACK_MIDIIN_ERROR_LOCKED_SOUNDCARD,
}AgsJackMidiinError;

struct _AgsJackMidiin
{
  GObject object;

  guint flags;

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

  gchar *card_uri;

  GObject *application_context;
  pthread_mutex_t *application_mutex;
  
  GList *audio;
};

struct _AgsJackMidiinClass
{
  GObjectClass object;
};

GType ags_jack_midiin_get_type();

GQuark ags_jack_midiin_error_quark();

AgsJackMidiin* ags_jack_midiin_new(GObject *application_context);

#endif /*__AGS_JACK_MIDIIN_H__*/
