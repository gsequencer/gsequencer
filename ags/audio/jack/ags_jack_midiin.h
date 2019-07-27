/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/libags.h>

#define AGS_TYPE_JACK_MIDIIN                (ags_jack_midiin_get_type())
#define AGS_JACK_MIDIIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_JACK_MIDIIN, AgsJackMidiin))
#define AGS_JACK_MIDIIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_JACK_MIDIIN, AgsJackMidiin))
#define AGS_IS_JACK_MIDIIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_JACK_MIDIIN))
#define AGS_IS_JACK_MIDIIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_JACK_MIDIIN))
#define AGS_JACK_MIDIIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_JACK_MIDIIN, AgsJackMidiinClass))

#define AGS_JACK_MIDIIN_GET_OBJ_MUTEX(obj) (((AgsJackMidiin *) obj)->obj_mutex)

#define AGS_JACK_MIDIIN_DEFAULT_BUFFER_SIZE (256)

typedef struct _AgsJackMidiin AgsJackMidiin;
typedef struct _AgsJackMidiinClass AgsJackMidiinClass;

/**
 * AgsJackMidiinFlags:
 * @AGS_JACK_MIDIIN_ADDED_TO_REGISTRY: the JACK midiin was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_JACK_MIDIIN_CONNECTED: indicates the JACK midiin was connected by calling #AgsConnectable::connect()
 * @AGS_JACK_MIDIIN_BUFFER0: ring-buffer 0
 * @AGS_JACK_MIDIIN_BUFFER1: ring-buffer 1
 * @AGS_JACK_MIDIIN_BUFFER2: ring-buffer 2
 * @AGS_JACK_MIDIIN_BUFFER3: ring-buffer 3
 * @AGS_JACK_MIDIIN_ATTACK_FIRST: use first attack, instead of second one
 * @AGS_JACK_MIDIIN_RECORD: is recording
 * @AGS_JACK_MIDIIN_SHUTDOWN: stop recording
 * @AGS_JACK_MIDIIN_START_RECORD: just started recording
 * @AGS_JACK_MIDIIN_NONBLOCKING: do non-blocking calls
 * @AGS_JACK_MIDIIN_INITIALIZED: recording is initialized
 *
 * Enum values to control the behavior or indicate internal state of #AgsJackMidiin by
 * enable/disable as flags.
 */
typedef enum
{
  AGS_JACK_MIDIIN_ADDED_TO_REGISTRY  = 1,
  AGS_JACK_MIDIIN_CONNECTED          = 1 <<  1,

  AGS_JACK_MIDIIN_BUFFER0            = 1 <<  2,
  AGS_JACK_MIDIIN_BUFFER1            = 1 <<  3,
  AGS_JACK_MIDIIN_BUFFER2            = 1 <<  4,
  AGS_JACK_MIDIIN_BUFFER3            = 1 <<  5,

  AGS_JACK_MIDIIN_ATTACK_FIRST       = 1 <<  6,

  AGS_JACK_MIDIIN_RECORD             = 1 <<  7,

  AGS_JACK_MIDIIN_SHUTDOWN           = 1 <<  8,
  AGS_JACK_MIDIIN_START_RECORD       = 1 <<  9,

  AGS_JACK_MIDIIN_NONBLOCKING        = 1 << 10,
  AGS_JACK_MIDIIN_INITIALIZED        = 1 << 11,
}AgsJackMidiinFlags;

/**
 * AgsJackMidiinSyncFlags:
 * @AGS_JACK_MIDIIN_PASS_THROUGH: do not sync
 * @AGS_JACK_MIDIIN_INITIAL_CALLBACK: initial callback
 * @AGS_JACK_MIDIIN_CALLBACK_WAIT: sync wait, sequencer conditional lock
 * @AGS_JACK_MIDIIN_CALLBACK_DONE: sync done, sequencer conditional lock
 * @AGS_JACK_MIDIIN_CALLBACK_FINISH_WAIT: sync wait, client conditional lock
 * @AGS_JACK_MIDIIN_CALLBACK_FINISH_DONE: sync done, client conditional lock
 * 
 * Enum values to control the synchronization between sequencer and client.
 */
typedef enum{
  AGS_JACK_MIDIIN_PASS_THROUGH                   = 1,
  AGS_JACK_MIDIIN_INITIAL_CALLBACK               = 1 <<  1,
  AGS_JACK_MIDIIN_CALLBACK_WAIT                  = 1 <<  2,
  AGS_JACK_MIDIIN_CALLBACK_DONE                  = 1 <<  3,
  AGS_JACK_MIDIIN_CALLBACK_FINISH_WAIT           = 1 <<  4,
  AGS_JACK_MIDIIN_CALLBACK_FINISH_DONE           = 1 <<  5,
}AgsJackMidiinSyncFlags;

#define AGS_JACK_MIDIIN_ERROR (ags_jack_midiin_error_quark())

typedef enum{
  AGS_JACK_MIDIIN_ERROR_LOCKED_SOUNDCARD,
}AgsJackMidiinError;

struct _AgsJackMidiin
{
  GObject gobject;

  guint flags;
  volatile guint sync_flags;
  
  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  GObject *application_context;
  
  AgsUUID *uuid;

  pthread_mutex_t **buffer_mutex;
  char **buffer;
  guint buffer_size[4];

  double bpm; // beats per minute

  gdouble delay;
  gdouble delay_factor;
  guint latency;
  
  gdouble tact_counter;
  gdouble delay_counter; // next time attack changeing when delay_counter == delay
  guint tic_counter; // in the range of default period

  guint start_note_offset;
  guint note_offset;
  guint note_offset_absolute;

  gchar *card_uri;
  GObject *jack_client;

  gchar **port_name;
  GList *jack_port;

  pthread_mutex_t *callback_mutex;
  pthread_cond_t *callback_cond;

  pthread_mutex_t *callback_finish_mutex;
  pthread_cond_t *callback_finish_cond;    
};

struct _AgsJackMidiinClass
{
  GObjectClass gobject;
};

GType ags_jack_midiin_get_type();

GQuark ags_jack_midiin_error_quark();

pthread_mutex_t* ags_jack_midiin_get_class_mutex();

gboolean ags_jack_midiin_test_flags(AgsJackMidiin *jack_midiin, guint flags);
void ags_jack_midiin_set_flags(AgsJackMidiin *jack_midiin, guint flags);
void ags_jack_midiin_unset_flags(AgsJackMidiin *jack_midiin, guint flags);

void ags_jack_midiin_switch_buffer_flag(AgsJackMidiin *jack_midiin);

void ags_jack_midiin_switch_buffer_flag(AgsJackMidiin *jack_midiin);

AgsJackMidiin* ags_jack_midiin_new(AgsApplicationContext *application_context);

#endif /*__AGS_JACK_MIDIIN_H__*/
