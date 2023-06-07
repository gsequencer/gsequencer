/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_W32_MIDIIN_H__
#define __AGS_W32_MIDIIN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_W32_MIDIIN                (ags_w32_midiin_get_type())
#define AGS_W32_MIDIIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_W32_MIDIIN, AgsW32Midiin))
#define AGS_W32_MIDIIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_W32_MIDIIN, AgsW32Midiin))
#define AGS_IS_W32_MIDIIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_W32_MIDIIN))
#define AGS_IS_W32_MIDIIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_W32_MIDIIN))
#define AGS_W32_MIDIIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_W32_MIDIIN, AgsW32MidiinClass))

#define AGS_W32_MIDIIN_GET_OBJ_MUTEX(obj) (&(((AgsW32Midiin *) obj)->obj_mutex))

#define AGS_W32_MIDIIN_DEFAULT_W32_DEVICE NULL

#define AGS_W32_MIDIIN_DEFAULT_APP_BUFFER_SIZE (4)
#define AGS_W32_MIDIIN_DEFAULT_BACKEND_BUFFER_SIZE (8)

#define AGS_W32_MIDIIN_DEFAULT_BUFFER_SIZE (4096)

typedef struct _AgsW32Midiin AgsW32Midiin;
typedef struct _AgsW32MidiinClass AgsW32MidiinClass;

/**
 * AgsW32MidiinFlags:
 * @AGS_W32_MIDIIN_INITIALIZED: recording is initialized
 * @AGS_W32_MIDIIN_START_RECORD: just started recording
 * @AGS_W32_MIDIIN_RECORD: is recording
 * @AGS_W32_MIDIIN_SHUTDOWN: stop recording
 * @AGS_W32_MIDIIN_NONBLOCKING: do non-blocking calls
 * @AGS_W32_MIDIIN_ATTACK_FIRST: use first attack, instead of second one
 *
 * Enum values to control the behavior or indicate internal state of #AgsW32Midiin by
 * enable/disable as flags.
 */
typedef enum
{
  AGS_W32_MIDIIN_INITIALIZED        = 1,

  AGS_W32_MIDIIN_START_RECORD       = 1 <<  1,
  AGS_W32_MIDIIN_RECORD             = 1 <<  2,
  AGS_W32_MIDIIN_SHUTDOWN           = 1 <<  3,

  AGS_W32_MIDIIN_NONBLOCKING        = 1 <<  4,
  AGS_W32_MIDIIN_ATTACK_FIRST       = 1 <<  5,
}AgsW32MidiinFlags;

/**
 * AgsW32MidiinAppBufferMode:
 * @AGS_W32_MIDIIN_APP_BUFFER_0: ring-buffer 0
 * @AGS_W32_MIDIIN_APP_BUFFER_1: ring-buffer 1
 * @AGS_W32_MIDIIN_APP_BUFFER_2: ring-buffer 2
 * @AGS_W32_MIDIIN_APP_BUFFER_3: ring-buffer 3
 * 
 * Enum values to indicate internal state of #AgsW32Midiin application buffer by
 * setting mode.
 */
typedef enum{
  AGS_W32_MIDIIN_APP_BUFFER_0,
  AGS_W32_MIDIIN_APP_BUFFER_1,
  AGS_W32_MIDIIN_APP_BUFFER_2,
  AGS_W32_MIDIIN_APP_BUFFER_3,
}AgsW32MidiinAppBufferMode;

/**
 * AgsW32MidiinBackendBufferMode:
 * @AGS_W32_MIDIIN_BACKEND_BUFFER_0: ring-buffer 0
 * @AGS_W32_MIDIIN_BACKEND_BUFFER_1: ring-buffer 1
 * @AGS_W32_MIDIIN_BACKEND_BUFFER_2: ring-buffer 2
 * @AGS_W32_MIDIIN_BACKEND_BUFFER_3: ring-buffer 3
 * @AGS_W32_MIDIIN_BACKEND_BUFFER_4: ring-buffer 4
 * @AGS_W32_MIDIIN_BACKEND_BUFFER_5: ring-buffer 5
 * @AGS_W32_MIDIIN_BACKEND_BUFFER_6: ring-buffer 6
 * @AGS_W32_MIDIIN_BACKEND_BUFFER_7: ring-buffer 7
 * 
 * Enum values to indicate internal state of #AgsW32Midiin backend buffer by
 * setting mode.
 */
typedef enum{
  AGS_W32_MIDIIN_BACKEND_BUFFER_0,
  AGS_W32_MIDIIN_BACKEND_BUFFER_1,
  AGS_W32_MIDIIN_BACKEND_BUFFER_2,
  AGS_W32_MIDIIN_BACKEND_BUFFER_3,
  AGS_W32_MIDIIN_BACKEND_BUFFER_4,
  AGS_W32_MIDIIN_BACKEND_BUFFER_5,
  AGS_W32_MIDIIN_BACKEND_BUFFER_6,
  AGS_W32_MIDIIN_BACKEND_BUFFER_7,
}AgsW32MidiinBackendBufferMode;

/**
 * AgsW32MidiinSyncFlags:
 * @AGS_W32_MIDIIN_PASS_THROUGH: do not sync
 * @AGS_W32_MIDIIN_INITIAL_POLL: initial poll
 * @AGS_W32_MIDIIN_POLL_WAIT: sync wait, sequencer conditional lock
 * @AGS_W32_MIDIIN_POLL_DONE: sync done, sequencer conditional lock
 * @AGS_W32_MIDIIN_POLL_FINISH_WAIT: sync wait, client conditional lock
 * @AGS_W32_MIDIIN_POLL_FINISH_DONE: sync done, client conditional lock
 * @AGS_W32_MIDIIN_POLL_SWITCH_BUFFER: switch buffer
 * 
 * Enum values to control the synchronization between sequencer and poll.
 */
typedef enum{
  AGS_W32_MIDIIN_PASS_THROUGH                   = 1,
  AGS_W32_MIDIIN_INITIAL_POLL                   = 1 <<  1,
  AGS_W32_MIDIIN_POLL_WAIT                      = 1 <<  2,
  AGS_W32_MIDIIN_POLL_DONE                      = 1 <<  3,
  AGS_W32_MIDIIN_POLL_FINISH_WAIT               = 1 <<  4,
  AGS_W32_MIDIIN_POLL_FINISH_DONE               = 1 <<  5,
  AGS_W32_MIDIIN_POLL_SWITCH_BUFFER             = 1 <<  6,
}AgsW32MidiinSyncFlags;

#define AGS_W32_MIDIIN_ERROR (ags_w32_midiin_error_quark())

typedef enum{
  AGS_W32_MIDIIN_ERROR_LOCKED_SEQUENCER,
}AgsW32MidiinError;

struct _AgsW32Midiin
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
  guint app_buffer_size[AGS_W32_MIDIIN_DEFAULT_APP_BUFFER_SIZE];

  guint backend_buffer_mode;
  
  char **backend_buffer;
  guint backend_buffer_size[AGS_W32_MIDIIN_DEFAULT_BACKEND_BUFFER_SIZE];
  
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

  gchar *device;

  HMIDIIN midi_handle;
  MIDIHDR midi_header;
};

struct _AgsW32MidiinClass
{
  GObjectClass gobject;
};

GType ags_w32_midiin_get_type();
GType ags_w32_midiin_flags_get_type();

GQuark ags_w32_midiin_error_quark();

gboolean ags_w32_midiin_test_flags(AgsW32Midiin *w32_midiin, guint flags);
void ags_w32_midiin_set_flags(AgsW32Midiin *w32_midiin, guint flags);
void ags_w32_midiin_unset_flags(AgsW32Midiin *w32_midiin, guint flags);

void ags_w32_midiin_switch_buffer_flag(AgsW32Midiin *w32_midiin);

AgsW32Midiin* ags_w32_midiin_new();

G_END_DECLS

#endif /*__AGS_W32_MIDIIN_H__*/
