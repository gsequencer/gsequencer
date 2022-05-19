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

#ifndef __AGS_GSTREAMER_DEVIN_H__
#define __AGS_GSTREAMER_DEVIN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_GSTREAMER_DEVIN                (ags_gstreamer_devin_get_type())
#define AGS_GSTREAMER_DEVIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GSTREAMER_DEVIN, AgsGstreamerDevin))
#define AGS_GSTREAMER_DEVIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_GSTREAMER_DEVIN, AgsGstreamerDevin))
#define AGS_IS_GSTREAMER_DEVIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_GSTREAMER_DEVIN))
#define AGS_IS_GSTREAMER_DEVIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_GSTREAMER_DEVIN))
#define AGS_GSTREAMER_DEVIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_GSTREAMER_DEVIN, AgsGstreamerDevinClass))

#define AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(obj) (&(((AgsGstreamerDevin *) obj)->obj_mutex))

typedef struct _AgsGstreamerDevin AgsGstreamerDevin;
typedef struct _AgsGstreamerDevinClass AgsGstreamerDevinClass;

/**
 * AgsGstreamerDevinFlags:
 * @AGS_GSTREAMER_DEVIN_INITIALIZED: the soundcard was initialized
 * @AGS_GSTREAMER_DEVIN_START_RECORD: capture starting
 * @AGS_GSTREAMER_DEVIN_RECORD: do capture
 * @AGS_GSTREAMER_DEVIN_SHUTDOWN: stop capture
 * @AGS_GSTREAMER_DEVIN_NONBLOCKING: do non-blocking calls
 * @AGS_GSTREAMER_DEVIN_ATTACK_FIRST: use first attack, instead of second one
 *
 * Enum values to control the behavior or indicate internal state of #AgsGstreamerDevin by
 * enable/disable as flags.
 */
typedef enum{
  AGS_GSTREAMER_DEVIN_INITIALIZED                    = 1,

  AGS_GSTREAMER_DEVIN_START_RECORD                   = 1 <<  1,
  AGS_GSTREAMER_DEVIN_RECORD                         = 1 <<  2,
  AGS_GSTREAMER_DEVIN_SHUTDOWN                       = 1 <<  3,

  AGS_GSTREAMER_DEVIN_NONBLOCKING                    = 1 <<  4,

  AGS_GSTREAMER_DEVIN_ATTACK_FIRST                   = 1 <<  5,
}AgsGstreamerDevinFlags;

/**
 * AgsGstreamerDevinAppBufferMode:
 * @AGS_GSTREAMER_DEVIN_APP_BUFFER_0: ring-buffer 0
 * @AGS_GSTREAMER_DEVIN_APP_BUFFER_1: ring-buffer 1
 * @AGS_GSTREAMER_DEVIN_APP_BUFFER_2: ring-buffer 2
 * @AGS_GSTREAMER_DEVIN_APP_BUFFER_3: ring-buffer 3
 * @AGS_GSTREAMER_DEVIN_APP_BUFFER_4: ring-buffer 4
 * @AGS_GSTREAMER_DEVIN_APP_BUFFER_5: ring-buffer 5
 * @AGS_GSTREAMER_DEVIN_APP_BUFFER_6: ring-buffer 6
 * @AGS_GSTREAMER_DEVIN_APP_BUFFER_7: ring-buffer 7
 * 
 * Enum values to indicate internal state of #AgsGstreamerDevin application buffer by
 * setting mode.
 */
typedef enum{
  AGS_GSTREAMER_DEVIN_APP_BUFFER_0,
  AGS_GSTREAMER_DEVIN_APP_BUFFER_1,
  AGS_GSTREAMER_DEVIN_APP_BUFFER_2,
  AGS_GSTREAMER_DEVIN_APP_BUFFER_3,
  AGS_GSTREAMER_DEVIN_APP_BUFFER_4,
  AGS_GSTREAMER_DEVIN_APP_BUFFER_5,
  AGS_GSTREAMER_DEVIN_APP_BUFFER_6,
  AGS_GSTREAMER_DEVIN_APP_BUFFER_7,
}AgsGstreamerDevinAppBufferMode;

/**
 * AgsGstreamerDevinSyncFlags:
 * @AGS_GSTREAMER_DEVIN_PASS_THROUGH: do not sync
 * @AGS_GSTREAMER_DEVIN_INITIAL_CALLBACK: initial callback
 * @AGS_GSTREAMER_DEVIN_CALLBACK_WAIT: sync wait, soundcard conditional lock
 * @AGS_GSTREAMER_DEVIN_CALLBACK_DONE: sync done, soundcard conditional lock
 * @AGS_GSTREAMER_DEVIN_CALLBACK_FINISH_WAIT: sync wait, client conditional lock
 * @AGS_GSTREAMER_DEVIN_CALLBACK_FINISH_DONE: sync done, client conditional lock
 * 
 * Enum values to control the synchronization between soundcard and client.
 */
typedef enum{
  AGS_GSTREAMER_DEVIN_PASS_THROUGH                   = 1,
  AGS_GSTREAMER_DEVIN_INITIAL_CALLBACK               = 1 <<  1,
  AGS_GSTREAMER_DEVIN_CALLBACK_WAIT                  = 1 <<  2,
  AGS_GSTREAMER_DEVIN_CALLBACK_DONE                  = 1 <<  3,
  AGS_GSTREAMER_DEVIN_CALLBACK_FINISH_WAIT           = 1 <<  4,
  AGS_GSTREAMER_DEVIN_CALLBACK_FINISH_DONE           = 1 <<  5,
}AgsGstreamerDevinSyncFlags;

#define AGS_GSTREAMER_DEVIN_ERROR (ags_gstreamer_devin_error_quark())

typedef enum{
  AGS_GSTREAMER_DEVIN_ERROR_LOCKED_SOUNDCARD,
}AgsGstreamerDevinError;

struct _AgsGstreamerDevin
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
  void** app_buffer;

  guint sub_block_count;
  GRecMutex **sub_block_mutex;

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
  GObject *gstreamer_client;

  gchar **port_name;
  GList *gstreamer_port;

  GMutex callback_mutex;
  GCond callback_cond;

  GMutex callback_finish_mutex;
  GCond callback_finish_cond;
};

struct _AgsGstreamerDevinClass
{
  GObjectClass gobject;
};

GType ags_gstreamer_devin_get_type();
GType ags_gstreamer_devin_flags_get_type();

GQuark ags_gstreamer_devin_error_quark();

gboolean ags_gstreamer_devin_test_flags(AgsGstreamerDevin *gstreamer_devin, guint flags);
void ags_gstreamer_devin_set_flags(AgsGstreamerDevin *gstreamer_devin, guint flags);
void ags_gstreamer_devin_unset_flags(AgsGstreamerDevin *gstreamer_devin, guint flags);

void ags_gstreamer_devin_switch_buffer_flag(AgsGstreamerDevin *gstreamer_devin);

void ags_gstreamer_devin_adjust_delay_and_attack(AgsGstreamerDevin *gstreamer_devin);
void ags_gstreamer_devin_realloc_buffer(AgsGstreamerDevin *gstreamer_devin);

AgsGstreamerDevin* ags_gstreamer_devin_new();

G_END_DECLS

#endif /*__AGS_GSTREAMER_DEVIN_H__*/
