/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_JACK_MIDIIN                (ags_jack_midiin_get_type())
#define AGS_JACK_MIDIIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_JACK_MIDIIN, AgsJackMidiin))
#define AGS_JACK_MIDIIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_JACK_MIDIIN, AgsJackMidiin))
#define AGS_IS_JACK_MIDIIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_JACK_MIDIIN))
#define AGS_IS_JACK_MIDIIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_JACK_MIDIIN))
#define AGS_JACK_MIDIIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_JACK_MIDIIN, AgsJackMidiinClass))

#define AGS_JACK_MIDIIN_GET_OBJ_MUTEX(obj) (&(((AgsJackMidiin *) obj)->obj_mutex))

#define AGS_JACK_MIDIIN_DEFAULT_BUFFER_SIZE (256)

typedef struct _AgsJackMidiin AgsJackMidiin;
typedef struct _AgsJackMidiinClass AgsJackMidiinClass;

/**
 * AgsJackMidiinFlags:
 * @AGS_JACK_MIDIIN_INITIALIZED: recording is initialized
 * @AGS_JACK_MIDIIN_START_RECORD: just started recording
 * @AGS_JACK_MIDIIN_RECORD: is recording
 * @AGS_JACK_MIDIIN_SHUTDOWN: stop recording
 * @AGS_JACK_MIDIIN_NONBLOCKING: do non-blocking calls
 * @AGS_JACK_MIDIIN_ATTACK_FIRST: use first attack, instead of second one
 *
 * Enum values to control the behavior or indicate internal state of #AgsJackMidiin by
 * enable/disable as flags.
 */
typedef enum
{
  AGS_JACK_MIDIIN_INITIALIZED        = 1,

  AGS_JACK_MIDIIN_START_RECORD       = 1 <<  1,
  AGS_JACK_MIDIIN_RECORD             = 1 <<  2,
  AGS_JACK_MIDIIN_SHUTDOWN           = 1 <<  3,

  AGS_JACK_MIDIIN_NONBLOCKING        = 1 <<  4,
  
  AGS_JACK_MIDIIN_ATTACK_FIRST       = 1 <<  5,
}AgsJackMidiinFlags;

/**
 * AgsJackMidiinAppBufferMode:
 * @AGS_JACK_MIDIIN_APP_BUFFER_0: ring-buffer 0
 * @AGS_JACK_MIDIIN_APP_BUFFER_1: ring-buffer 1
 * @AGS_JACK_MIDIIN_APP_BUFFER_2: ring-buffer 2
 * @AGS_JACK_MIDIIN_APP_BUFFER_3: ring-buffer 3
 * 
 * Enum values to indicate internal state of #AgsJackMidiin application buffer by
 * setting mode.
 */
typedef enum{
  AGS_JACK_MIDIIN_APP_BUFFER_0,
  AGS_JACK_MIDIIN_APP_BUFFER_1,
  AGS_JACK_MIDIIN_APP_BUFFER_2,
  AGS_JACK_MIDIIN_APP_BUFFER_3,
}AgsJackMidiinAppBufferMode;

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
  guint connectable_flags;
  volatile guint sync_flags;
  
  GRecMutex obj_mutex;

  AgsUUID *uuid;

  guint app_buffer_mode;

  GRecMutex **app_buffer_mutex;
  char **app_buffer;
  guint app_buffer_size[4];

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

  GMutex callback_mutex;
  GCond callback_cond;

  GMutex callback_finish_mutex;
  GCond callback_finish_cond;    
};

struct _AgsJackMidiinClass
{
  GObjectClass gobject;
};

GType ags_jack_midiin_get_type();
GType ags_jack_midiin_flags_get_type();

GQuark ags_jack_midiin_error_quark();

gboolean ags_jack_midiin_test_flags(AgsJackMidiin *jack_midiin, guint flags);
void ags_jack_midiin_set_flags(AgsJackMidiin *jack_midiin, guint flags);
void ags_jack_midiin_unset_flags(AgsJackMidiin *jack_midiin, guint flags);

void ags_jack_midiin_switch_buffer_flag(AgsJackMidiin *jack_midiin);

AgsJackMidiin* ags_jack_midiin_new();

G_END_DECLS

#endif /*__AGS_JACK_MIDIIN_H__*/
