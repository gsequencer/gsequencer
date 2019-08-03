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

#ifndef __AGS_PULSE_DEVIN_H__
#define __AGS_PULSE_DEVIN_H__

#include <glib.h>
#include <glib-object.h>

#include <pthread.h>

#include <ags/libags.h>

#define AGS_TYPE_PULSE_DEVIN                (ags_pulse_devin_get_type())
#define AGS_PULSE_DEVIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PULSE_DEVIN, AgsPulseDevin))
#define AGS_PULSE_DEVIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PULSE_DEVIN, AgsPulseDevin))
#define AGS_IS_PULSE_DEVIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PULSE_DEVIN))
#define AGS_IS_PULSE_DEVIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PULSE_DEVIN))
#define AGS_PULSE_DEVIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_PULSE_DEVIN, AgsPulseDevinClass))

#define AGS_PULSE_DEVIN_GET_OBJ_MUTEX(obj) (((AgsPulseDevin *) obj)->obj_mutex)

typedef struct _AgsPulseDevin AgsPulseDevin;
typedef struct _AgsPulseDevinClass AgsPulseDevinClass;

/**
 * AgsPulseDevinFlags:
 * @AGS_PULSE_DEVIN_ADDED_TO_REGISTRY: the pulseaudio devin was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_PULSE_DEVIN_CONNECTED: indicates the pulseaudio devin was connected by calling #AgsConnectable::connect()
 * @AGS_PULSE_DEVIN_BUFFER0: ring-buffer 0
 * @AGS_PULSE_DEVIN_BUFFER1: ring-buffer 1
 * @AGS_PULSE_DEVIN_BUFFER2: ring-buffer 2
 * @AGS_PULSE_DEVIN_BUFFER3: ring-buffer 3
 * @AGS_PULSE_DEVIN_BUFFER4: ring-buffer 4
 * @AGS_PULSE_DEVIN_BUFFER5: ring-buffer 5
 * @AGS_PULSE_DEVIN_BUFFER6: ring-buffer 6
 * @AGS_PULSE_DEVIN_BUFFER7: ring-buffer 7
 * @AGS_PULSE_DEVIN_ATTACK_FIRST: use first attack, instead of second one
 * @AGS_PULSE_DEVIN_RECORD: do capture
 * @AGS_PULSE_DEVIN_SHUTDOWN: stop capture
 * @AGS_PULSE_DEVIN_START_RECORD: capture starting
 * @AGS_PULSE_DEVIN_NONBLOCKING: do non-blocking calls
 * @AGS_PULSE_DEVIN_INITIALIZED: the soundcard was initialized
 *
 * Enum values to control the behavior or indicate internal state of #AgsPulseDevin by
 * enable/disable as flags.
 */
typedef enum{
  AGS_PULSE_DEVIN_ADDED_TO_REGISTRY              = 1,
  AGS_PULSE_DEVIN_CONNECTED                      = 1 <<  1,

  AGS_PULSE_DEVIN_BUFFER0                        = 1 <<  2,
  AGS_PULSE_DEVIN_BUFFER1                        = 1 <<  3,
  AGS_PULSE_DEVIN_BUFFER2                        = 1 <<  4,
  AGS_PULSE_DEVIN_BUFFER3                        = 1 <<  5,
  AGS_PULSE_DEVIN_BUFFER4                        = 1 <<  6,
  AGS_PULSE_DEVIN_BUFFER5                        = 1 <<  7,
  AGS_PULSE_DEVIN_BUFFER6                        = 1 <<  8,
  AGS_PULSE_DEVIN_BUFFER7                        = 1 <<  9,

  AGS_PULSE_DEVIN_ATTACK_FIRST                   = 1 << 10,

  AGS_PULSE_DEVIN_RECORD                         = 1 << 11,
  AGS_PULSE_DEVIN_SHUTDOWN                       = 1 << 12,
  AGS_PULSE_DEVIN_START_RECORD                   = 1 << 13,

  AGS_PULSE_DEVIN_NONBLOCKING                    = 1 << 14,
  AGS_PULSE_DEVIN_INITIALIZED                    = 1 << 15,
}AgsPulseDevinFlags;

/**
 * AgsPulseDevinSyncFlags:
 * @AGS_PULSE_DEVIN_PASS_THROUGH: do not sync
 * @AGS_PULSE_DEVIN_INITIAL_CALLBACK: initial callback
 * @AGS_PULSE_DEVIN_CALLBACK_WAIT: sync wait, soundcard conditional lock
 * @AGS_PULSE_DEVIN_CALLBACK_DONE: sync done, soundcard conditional lock
 * @AGS_PULSE_DEVIN_CALLBACK_FINISH_WAIT: sync wait, client conditional lock
 * @AGS_PULSE_DEVIN_CALLBACK_FINISH_DONE: sync done, client conditional lock
 * 
 * Enum values to control the synchronization between soundcard and client.
 */
typedef enum{
  AGS_PULSE_DEVIN_PASS_THROUGH                   = 1,
  AGS_PULSE_DEVIN_INITIAL_CALLBACK               = 1 <<  1,
  AGS_PULSE_DEVIN_CALLBACK_WAIT                  = 1 <<  2,
  AGS_PULSE_DEVIN_CALLBACK_DONE                  = 1 <<  3,
  AGS_PULSE_DEVIN_CALLBACK_FINISH_WAIT           = 1 <<  4,
  AGS_PULSE_DEVIN_CALLBACK_FINISH_DONE           = 1 <<  5,
}AgsPulseDevinSyncFlags;

#define AGS_PULSE_DEVIN_ERROR (ags_pulse_devin_error_quark())

typedef enum{
  AGS_PULSE_DEVIN_ERROR_LOCKED_SOUNDCARD,
}AgsPulseDevinError;

struct _AgsPulseDevin
{
  GObject gobject;

  guint flags;
  volatile guint sync_flags;
  
  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  AgsApplicationContext *application_context;

  AgsUUID *uuid;

  guint dsp_channels;
  guint pcm_channels;
  guint format;
  guint buffer_size;
  guint samplerate;

  void **buffer;

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

  pthread_mutex_t *callback_mutex;
  pthread_cond_t *callback_cond;

  pthread_mutex_t *callback_finish_mutex;
  pthread_cond_t *callback_finish_cond;

  GObject *notify_soundcard;
};

struct _AgsPulseDevinClass
{
  GObjectClass gobject;
};

GType ags_pulse_devin_get_type();

GQuark ags_pulse_devin_error_quark();

pthread_mutex_t* ags_pulse_devin_get_class_mutex();

gboolean ags_pulse_devin_test_flags(AgsPulseDevin *pulse_devin, guint flags);
void ags_pulse_devin_set_flags(AgsPulseDevin *pulse_devin, guint flags);
void ags_pulse_devin_unset_flags(AgsPulseDevin *pulse_devin, guint flags);

void ags_pulse_devin_switch_buffer_flag(AgsPulseDevin *pulse_devin);

void ags_pulse_devin_adjust_delay_and_attack(AgsPulseDevin *pulse_devin);
void ags_pulse_devin_realloc_buffer(AgsPulseDevin *pulse_devin);

AgsPulseDevin* ags_pulse_devin_new(AgsApplicationContext *application_context);

#endif /*__AGS_PULSE_DEVIN_H__*/
