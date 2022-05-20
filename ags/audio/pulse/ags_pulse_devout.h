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

#ifndef __AGS_PULSE_DEVOUT_H__
#define __AGS_PULSE_DEVOUT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_PULSE_DEVOUT                (ags_pulse_devout_get_type())
#define AGS_PULSE_DEVOUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PULSE_DEVOUT, AgsPulseDevout))
#define AGS_PULSE_DEVOUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PULSE_DEVOUT, AgsPulseDevout))
#define AGS_IS_PULSE_DEVOUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PULSE_DEVOUT))
#define AGS_IS_PULSE_DEVOUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PULSE_DEVOUT))
#define AGS_PULSE_DEVOUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_PULSE_DEVOUT, AgsPulseDevoutClass))

#define AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(obj) (&(((AgsPulseDevout *) obj)->obj_mutex))

#define AGS_PULSE_DEVOUT_DEFAULT_APP_BUFFER_SIZE (8)

typedef struct _AgsPulseDevout AgsPulseDevout;
typedef struct _AgsPulseDevoutClass AgsPulseDevoutClass;

/**
 * AgsPulseDevoutFlags:
 * @AGS_PULSE_DEVOUT_INITIALIZED: the soundcard was initialized
 * @AGS_PULSE_DEVOUT_START_PLAY: playback starting
 * @AGS_PULSE_DEVOUT_PLAY: do playback
 * @AGS_PULSE_DEVOUT_SHUTDOWN: stop playback
 * @AGS_PULSE_DEVOUT_NONBLOCKING: do non-blocking calls
 * @AGS_PULSE_DEVOUT_ATTACK_FIRST: use first attack, instead of second one
 *
 * Enum values to control the behavior or indicate internal state of #AgsPulseDevout by
 * enable/disable as flags.
 */
typedef enum{
  AGS_PULSE_DEVOUT_INITIALIZED                    = 1,

  AGS_PULSE_DEVOUT_START_PLAY                     = 1 <<  1,
  AGS_PULSE_DEVOUT_PLAY                           = 1 <<  2,
  AGS_PULSE_DEVOUT_SHUTDOWN                       = 1 <<  3,

  AGS_PULSE_DEVOUT_NONBLOCKING                    = 1 <<  4,

  AGS_PULSE_DEVOUT_ATTACK_FIRST                   = 1 <<  5,
}AgsPulseDevoutFlags;

/**
 * AgsPulseDevoutAppBufferMode:
 * @AGS_PULSE_DEVOUT_APP_BUFFER_0: ring-buffer 0
 * @AGS_PULSE_DEVOUT_APP_BUFFER_1: ring-buffer 1
 * @AGS_PULSE_DEVOUT_APP_BUFFER_2: ring-buffer 2
 * @AGS_PULSE_DEVOUT_APP_BUFFER_3: ring-buffer 3
 * @AGS_PULSE_DEVOUT_APP_BUFFER_4: ring-buffer 4
 * @AGS_PULSE_DEVOUT_APP_BUFFER_5: ring-buffer 5
 * @AGS_PULSE_DEVOUT_APP_BUFFER_6: ring-buffer 6
 * @AGS_PULSE_DEVOUT_APP_BUFFER_7: ring-buffer 7
 * 
 * Enum values to indicate internal state of #AgsPulseDevout application buffer by
 * setting mode.
 */
typedef enum{
  AGS_PULSE_DEVOUT_APP_BUFFER_0,
  AGS_PULSE_DEVOUT_APP_BUFFER_1,
  AGS_PULSE_DEVOUT_APP_BUFFER_2,
  AGS_PULSE_DEVOUT_APP_BUFFER_3,
  AGS_PULSE_DEVOUT_APP_BUFFER_4,
  AGS_PULSE_DEVOUT_APP_BUFFER_5,
  AGS_PULSE_DEVOUT_APP_BUFFER_6,
  AGS_PULSE_DEVOUT_APP_BUFFER_7,
}AgsPulseDevoutAppBufferMode;

/**
 * AgsPulseDevoutSyncFlags:
 * @AGS_PULSE_DEVOUT_PASS_THROUGH: do not sync
 * @AGS_PULSE_DEVOUT_INITIAL_CALLBACK: initial callback
 * @AGS_PULSE_DEVOUT_CALLBACK_WAIT: sync wait, soundcard conditional lock
 * @AGS_PULSE_DEVOUT_CALLBACK_DONE: sync done, soundcard conditional lock
 * @AGS_PULSE_DEVOUT_CALLBACK_FINISH_WAIT: sync wait, client conditional lock
 * @AGS_PULSE_DEVOUT_CALLBACK_FINISH_DONE: sync done, client conditional lock
 * 
 * Enum values to control the synchronization between soundcard and client.
 */
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
  GObject gobject;

  guint flags;
  guint connectable_flags;
  volatile guint sync_flags;
  
  GRecMutex obj_mutex;

  AgsUUID *uuid;

  guint dsp_channels;
  guint pcm_channels;
  guint format;
  guint buffer_size;
  guint samplerate;

  guint app_buffer_mode;

  GRecMutex **app_buffer_mutex;

  guint sub_block_count;
  GRecMutex **sub_block_mutex;

  void **app_buffer;

  double bpm; // beats per minute
  gdouble delay_factor;
  
  gdouble *delay; // count of tics within buffer size
  guint *attack; // where currently tic resides in the stream's offset, measured in 1/64 of bpm

  gdouble tact_counter;
  gdouble delay_counter; // next time attack changeing when delay_counter == delay
  guint tic_counter; // in the range of default period

  guint start_note_offset;
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

  GMutex callback_mutex;
  GCond callback_cond;

  GMutex callback_finish_mutex;
  GCond callback_finish_cond;

  GObject *notify_soundcard;
};

struct _AgsPulseDevoutClass
{
  GObjectClass gobject;
};

GType ags_pulse_devout_get_type();
GType ags_pulse_devout_flags_get_type();

GQuark ags_pulse_devout_error_quark();

gboolean ags_pulse_devout_test_flags(AgsPulseDevout *pulse_devout, guint flags);
void ags_pulse_devout_set_flags(AgsPulseDevout *pulse_devout, guint flags);
void ags_pulse_devout_unset_flags(AgsPulseDevout *pulse_devout, guint flags);

void ags_pulse_devout_switch_buffer_flag(AgsPulseDevout *pulse_devout);

void ags_pulse_devout_adjust_delay_and_attack(AgsPulseDevout *pulse_devout);
void ags_pulse_devout_realloc_buffer(AgsPulseDevout *pulse_devout);

AgsPulseDevout* ags_pulse_devout_new();

G_END_DECLS

#endif /*__AGS_PULSE_DEVOUT_H__*/
