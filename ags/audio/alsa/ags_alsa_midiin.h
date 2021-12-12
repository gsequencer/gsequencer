/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_ALSA_MIDIIN_H__
#define __AGS_ALSA_MIDIIN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/ags_api_config.h>

#if defined(AGS_WITH_ALSA)
#include <sys/types.h>
#include <alsa/asoundlib.h>
#endif

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_ALSA_MIDIIN                (ags_alsa_midiin_get_type())
#define AGS_ALSA_MIDIIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ALSA_MIDIIN, AgsAlsaMidiin))
#define AGS_ALSA_MIDIIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_ALSA_MIDIIN, AgsAlsaMidiin))
#define AGS_IS_ALSA_MIDIIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_ALSA_MIDIIN))
#define AGS_IS_ALSA_MIDIIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_ALSA_MIDIIN))
#define AGS_ALSA_MIDIIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_ALSA_MIDIIN, AgsAlsaMidiinClass))

#define AGS_ALSA_MIDIIN_GET_OBJ_MUTEX(obj) (&(((AgsAlsaMidiin *) obj)->obj_mutex))

#define AGS_ALSA_MIDIIN_DEFAULT_ALSA_DEVICE "hw:0,0"

#define AGS_ALSA_MIDIIN_DEFAULT_APP_BUFFER_SIZE (4)
#define AGS_ALSA_MIDIIN_DEFAULT_BACKEND_BUFFER_SIZE (8)

#define AGS_ALSA_MIDIIN_DEFAULT_BUFFER_SIZE (4096)

typedef struct _AgsAlsaMidiin AgsAlsaMidiin;
typedef struct _AgsAlsaMidiinClass AgsAlsaMidiinClass;

/**
 * AgsAlsaMidiinFlags:
 * @AGS_ALSA_MIDIIN_ADDED_TO_REGISTRY: the alsa_midiin was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_ALSA_MIDIIN_CONNECTED: indicates the alsa_midiin was connected by calling #AgsConnectable::connect()
 * @AGS_ALSA_MIDIIN_INITIALIZED: recording is initialized
 * @AGS_ALSA_MIDIIN_START_RECORD: just started recording
 * @AGS_ALSA_MIDIIN_RECORD: is recording
 * @AGS_ALSA_MIDIIN_SHUTDOWN: stop recording
 * @AGS_ALSA_MIDIIN_NONBLOCKING: do non-blocking calls
 * @AGS_ALSA_MIDIIN_ATTACK_FIRST: use first attack, instead of second one
 *
 * Enum values to control the behavior or indicate internal state of #AgsAlsaMidiin by
 * enable/disable as flags.
 */
typedef enum
{
  AGS_ALSA_MIDIIN_ADDED_TO_REGISTRY  = 1,
  AGS_ALSA_MIDIIN_CONNECTED          = 1 <<  1,

  AGS_ALSA_MIDIIN_INITIALIZED        = 1 <<  2,

  AGS_ALSA_MIDIIN_RECORD             = 1 <<  3,
  AGS_ALSA_MIDIIN_SHUTDOWN           = 1 <<  4,
  AGS_ALSA_MIDIIN_START_RECORD       = 1 <<  5,

  AGS_ALSA_MIDIIN_NONBLOCKING        = 1 <<  6,

  AGS_ALSA_MIDIIN_ATTACK_FIRST       = 1 <<  7,
}AgsAlsaMidiinFlags;

/**
 * AgsAlsaMidiinAppBufferMode:
 * @AGS_ALSA_MIDIIN_APP_BUFFER_0: ring-buffer 0
 * @AGS_ALSA_MIDIIN_APP_BUFFER_1: ring-buffer 1
 * @AGS_ALSA_MIDIIN_APP_BUFFER_2: ring-buffer 2
 * @AGS_ALSA_MIDIIN_APP_BUFFER_3: ring-buffer 3
 * 
 * Enum values to indicate internal state of #AgsAlsaMidiin application buffer by
 * setting mode.
 */
typedef enum{
  AGS_ALSA_MIDIIN_APP_BUFFER_0,
  AGS_ALSA_MIDIIN_APP_BUFFER_1,
  AGS_ALSA_MIDIIN_APP_BUFFER_2,
  AGS_ALSA_MIDIIN_APP_BUFFER_3,
}AgsAlsaMidiinAppBufferMode;

/**
 * AgsAlsaMidiinBackendBufferMode:
 * @AGS_ALSA_MIDIIN_BACKEND_BUFFER_0: ring-buffer 0
 * @AGS_ALSA_MIDIIN_BACKEND_BUFFER_1: ring-buffer 1
 * @AGS_ALSA_MIDIIN_BACKEND_BUFFER_2: ring-buffer 2
 * @AGS_ALSA_MIDIIN_BACKEND_BUFFER_3: ring-buffer 3
 * @AGS_ALSA_MIDIIN_BACKEND_BUFFER_4: ring-buffer 4
 * @AGS_ALSA_MIDIIN_BACKEND_BUFFER_5: ring-buffer 5
 * @AGS_ALSA_MIDIIN_BACKEND_BUFFER_6: ring-buffer 6
 * @AGS_ALSA_MIDIIN_BACKEND_BUFFER_7: ring-buffer 7
 * 
 * Enum values to indicate internal state of #AgsAlsaMidiin backend buffer by
 * setting mode.
 */
typedef enum{
  AGS_ALSA_MIDIIN_BACKEND_BUFFER_0,
  AGS_ALSA_MIDIIN_BACKEND_BUFFER_1,
  AGS_ALSA_MIDIIN_BACKEND_BUFFER_2,
  AGS_ALSA_MIDIIN_BACKEND_BUFFER_3,
  AGS_ALSA_MIDIIN_BACKEND_BUFFER_4,
  AGS_ALSA_MIDIIN_BACKEND_BUFFER_5,
  AGS_ALSA_MIDIIN_BACKEND_BUFFER_6,
  AGS_ALSA_MIDIIN_BACKEND_BUFFER_7,
}AgsAlsaMidiinBackendBufferMode;

/**
 * AgsAlsaMidiinSyncFlags:
 * @AGS_ALSA_MIDIIN_PASS_THROUGH: do not sync
 * @AGS_ALSA_MIDIIN_INITIAL_POLL: initial poll
 * @AGS_ALSA_MIDIIN_POLL_WAIT: sync wait, sequencer conditional lock
 * @AGS_ALSA_MIDIIN_POLL_DONE: sync done, sequencer conditional lock
 * @AGS_ALSA_MIDIIN_POLL_FINISH_WAIT: sync wait, client conditional lock
 * @AGS_ALSA_MIDIIN_POLL_FINISH_DONE: sync done, client conditional lock
 * @AGS_ALSA_MIDIIN_POLL_SWITCH_BUFFER: switch buffer
 * 
 * Enum values to control the synchronization between sequencer and poll.
 */
typedef enum{
  AGS_ALSA_MIDIIN_PASS_THROUGH                   = 1,
  AGS_ALSA_MIDIIN_INITIAL_POLL                   = 1 <<  1,
  AGS_ALSA_MIDIIN_POLL_WAIT                      = 1 <<  2,
  AGS_ALSA_MIDIIN_POLL_DONE                      = 1 <<  3,
  AGS_ALSA_MIDIIN_POLL_FINISH_WAIT               = 1 <<  4,
  AGS_ALSA_MIDIIN_POLL_FINISH_DONE               = 1 <<  5,
  AGS_ALSA_MIDIIN_POLL_SWITCH_BUFFER             = 1 <<  6,
}AgsAlsaMidiinSyncFlags;

#define AGS_ALSA_MIDIIN_ERROR (ags_alsa_midiin_error_quark())

typedef enum{
  AGS_ALSA_MIDIIN_ERROR_LOCKED_SEQUENCER,
}AgsAlsaMidiinError;

struct _AgsAlsaMidiin
{
  GObject gobject;

  guint flags;
  volatile guint sync_flags;

  GRecMutex obj_mutex;

  AgsUUID *uuid;
    
  guint app_buffer_mode;
  
  GRecMutex **app_buffer_mutex;

  char **app_buffer;
  guint app_buffer_size[AGS_ALSA_MIDIIN_DEFAULT_APP_BUFFER_SIZE];

  guint backend_buffer_mode;
  
  char **backend_buffer;
  guint backend_buffer_size[AGS_ALSA_MIDIIN_DEFAULT_BACKEND_BUFFER_SIZE];

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

  char *device;

  int rc;

#ifdef AGS_WITH_ALSA
  snd_rawmidi_t *handle;
#else
  gpointer handle;
#endif
};

struct _AgsAlsaMidiinClass
{
  GObjectClass gobject;
};

GType ags_alsa_midiin_get_type();

GQuark ags_alsa_midiin_error_quark();

gboolean ags_alsa_midiin_test_flags(AgsAlsaMidiin *alsa_midiin, guint flags);
void ags_alsa_midiin_set_flags(AgsAlsaMidiin *alsa_midiin, guint flags);
void ags_alsa_midiin_unset_flags(AgsAlsaMidiin *alsa_midiin, guint flags);

void ags_alsa_midiin_switch_buffer_flag(AgsAlsaMidiin *alsa_midiin);

AgsAlsaMidiin* ags_alsa_midiin_new();

G_END_DECLS

#endif /*__AGS_ALSA_MIDIIN_H__*/
