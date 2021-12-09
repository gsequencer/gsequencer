/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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
#include <alsa/asoundlib.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_MIDIIN                (ags_midiin_get_type())
#define AGS_MIDIIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MIDIIN, AgsMidiin))
#define AGS_MIDIIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_MIDIIN, AgsMidiin))
#define AGS_IS_MIDIIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MIDIIN))
#define AGS_IS_MIDIIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MIDIIN))
#define AGS_MIDIIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_MIDIIN, AgsMidiinClass))

#define AGS_MIDIIN_GET_OBJ_MUTEX(obj) (&(((AgsMidiin *) obj)->obj_mutex))

#define AGS_MIDIIN_DEFAULT_ALSA_DEVICE "hw:0,0"
#define AGS_MIDIIN_DEFAULT_OSS_DEVICE "/dev/midi00"
#define AGS_MIDIIN_DEFAULT_BUFFER_SIZE (4096)

typedef struct _AgsMidiin AgsMidiin;
typedef struct _AgsMidiinClass AgsMidiinClass;

/**
 * AgsMidiinFlags:
 * @AGS_MIDIIN_ADDED_TO_REGISTRY: the midiin was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_MIDIIN_CONNECTED: indicates the midiin was connected by calling #AgsConnectable::connect()
 * @AGS_MIDIIN_BUFFER0: ring-buffer 0
 * @AGS_MIDIIN_BUFFER1: ring-buffer 1
 * @AGS_MIDIIN_BUFFER2: ring-buffer 2
 * @AGS_MIDIIN_BUFFER3: ring-buffer 3
 * @AGS_MIDIIN_ATTACK_FIRST: use first attack, instead of second one
 * @AGS_MIDIIN_RECORD: is recording
 * @AGS_MIDIIN_OSS: use OSS4 backend
 * @AGS_MIDIIN_ALSA: use ALSA backend
 * @AGS_MIDIIN_SHUTDOWN: stop recording
 * @AGS_MIDIIN_START_RECORD: just started recording
 * @AGS_MIDIIN_NONBLOCKING: do non-blocking calls
 * @AGS_MIDIIN_INITIALIZED: recording is initialized
 *
 * Enum values to control the behavior or indicate internal state of #AgsMidiin by
 * enable/disable as flags.
 */
typedef enum
{
  AGS_MIDIIN_ADDED_TO_REGISTRY  = 1,
  AGS_MIDIIN_CONNECTED          = 1 <<  1,

  AGS_MIDIIN_BUFFER0            = 1 <<  2,
  AGS_MIDIIN_BUFFER1            = 1 <<  3,
  AGS_MIDIIN_BUFFER2            = 1 <<  4,
  AGS_MIDIIN_BUFFER3            = 1 <<  5,

  AGS_MIDIIN_ATTACK_FIRST       = 1 <<  6,

  AGS_MIDIIN_RECORD             = 1 <<  7,

  AGS_MIDIIN_OSS                = 1 <<  8,
  AGS_MIDIIN_ALSA               = 1 <<  9,

  AGS_MIDIIN_SHUTDOWN           = 1 << 10,
  AGS_MIDIIN_START_RECORD       = 1 << 11,

  AGS_MIDIIN_NONBLOCKING        = 1 << 12,
  AGS_MIDIIN_INITIALIZED        = 1 << 13,
}AgsMidiinFlags;

/**
 * AgsMidiinSyncFlags:
 * @AGS_MIDIIN_PASS_THROUGH: do not sync
 * @AGS_MIDIIN_INITIAL_POLL: initial poll
 * @AGS_MIDIIN_POLL_WAIT: sync wait, sequencer conditional lock
 * @AGS_MIDIIN_POLL_DONE: sync done, sequencer conditional lock
 * @AGS_MIDIIN_POLL_FINISH_WAIT: sync wait, client conditional lock
 * @AGS_MIDIIN_POLL_FINISH_DONE: sync done, client conditional lock
 * @AGS_MIDIIN_POLL_SWITCH_BUFFER: switch buffer
 * 
 * Enum values to control the synchronization between sequencer and poll.
 */
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
  GObject gobject;

  guint flags;
  volatile guint sync_flags;

  GRecMutex obj_mutex;

  AgsUUID *uuid;
  
  char **ring_buffer;
  guint ring_buffer_size[2];
  
  GRecMutex **buffer_mutex;
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

  union{
    struct _AgsOssMidi{
      int device_fd;
      char *device;
    }oss;
    struct _AgsAlsaMidi{
      char *device;
      int rc;
#ifdef AGS_WITH_ALSA
      snd_rawmidi_t *handle;
#else
      gpointer handle;
#endif
    }alsa;
  }in;
};

struct _AgsMidiinClass
{
  GObjectClass gobject;
};

G_DEPRECATED
GType ags_midiin_get_type();

G_DEPRECATED
GQuark ags_midiin_error_quark();

G_DEPRECATED
gboolean ags_midiin_test_flags(AgsMidiin *midiin, guint flags);
G_DEPRECATED
void ags_midiin_set_flags(AgsMidiin *midiin, guint flags);
G_DEPRECATED
void ags_midiin_unset_flags(AgsMidiin *midiin, guint flags);

G_DEPRECATED
void ags_midiin_switch_buffer_flag(AgsMidiin *midiin);

G_DEPRECATED
AgsMidiin* ags_midiin_new();

G_END_DECLS

#endif /*__AGS_MIDIIN_H__*/
