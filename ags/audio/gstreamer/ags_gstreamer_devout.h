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

#ifndef __AGS_GSTREAMER_DEVOUT_H__
#define __AGS_GSTREAMER_DEVOUT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_GSTREAMER_DEVOUT                (ags_gstreamer_devout_get_type())
#define AGS_GSTREAMER_DEVOUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GSTREAMER_DEVOUT, AgsGstreamerDevout))
#define AGS_GSTREAMER_DEVOUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_GSTREAMER_DEVOUT, AgsGstreamerDevout))
#define AGS_IS_GSTREAMER_DEVOUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_GSTREAMER_DEVOUT))
#define AGS_IS_GSTREAMER_DEVOUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_GSTREAMER_DEVOUT))
#define AGS_GSTREAMER_DEVOUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_GSTREAMER_DEVOUT, AgsGstreamerDevoutClass))

#define AGS_GSTREAMER_DEVOUT_GET_OBJ_MUTEX(obj) (&(((AgsGstreamerDevout *) obj)->obj_mutex))

typedef struct _AgsGstreamerDevout AgsGstreamerDevout;
typedef struct _AgsGstreamerDevoutClass AgsGstreamerDevoutClass;

/**
 * AgsGstreamerDevoutFlags:
 * @AGS_GSTREAMER_DEVOUT_ADDED_TO_REGISTRY: the gstreameraudio devout was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_GSTREAMER_DEVOUT_CONNECTED: indicates the gstreameraudio devout was connected by calling #AgsConnectable::connect()
 * @AGS_GSTREAMER_DEVOUT_BUFFER0: ring-buffer 0
 * @AGS_GSTREAMER_DEVOUT_BUFFER1: ring-buffer 1
 * @AGS_GSTREAMER_DEVOUT_BUFFER2: ring-buffer 2
 * @AGS_GSTREAMER_DEVOUT_BUFFER3: ring-buffer 3
 * @AGS_GSTREAMER_DEVOUT_BUFFER4: ring-buffer 4
 * @AGS_GSTREAMER_DEVOUT_BUFFER5: ring-buffer 5
 * @AGS_GSTREAMER_DEVOUT_BUFFER6: ring-buffer 6
 * @AGS_GSTREAMER_DEVOUT_BUFFER7: ring-buffer 7
 * @AGS_GSTREAMER_DEVOUT_ATTACK_FIRST: use first attack, instead of second one
 * @AGS_GSTREAMER_DEVOUT_PLAY: do playback
 * @AGS_GSTREAMER_DEVOUT_SHUTDOWN: stop playback
 * @AGS_GSTREAMER_DEVOUT_START_PLAY: playback starting
 * @AGS_GSTREAMER_DEVOUT_NONBLOCKING: do non-blocking calls
 * @AGS_GSTREAMER_DEVOUT_INITIALIZED: the soundcard was initialized
 *
 * Enum values to control the behavior or indicate internal state of #AgsGstreamerDevout by
 * enable/disable as flags.
 */
typedef enum{
  AGS_GSTREAMER_DEVOUT_ADDED_TO_REGISTRY              = 1,
  AGS_GSTREAMER_DEVOUT_CONNECTED                      = 1 <<  1,

  AGS_GSTREAMER_DEVOUT_BUFFER0                        = 1 <<  2,
  AGS_GSTREAMER_DEVOUT_BUFFER1                        = 1 <<  3,
  AGS_GSTREAMER_DEVOUT_BUFFER2                        = 1 <<  4,
  AGS_GSTREAMER_DEVOUT_BUFFER3                        = 1 <<  5,
  AGS_GSTREAMER_DEVOUT_BUFFER4                        = 1 <<  6,
  AGS_GSTREAMER_DEVOUT_BUFFER5                        = 1 <<  7,
  AGS_GSTREAMER_DEVOUT_BUFFER6                        = 1 <<  8,
  AGS_GSTREAMER_DEVOUT_BUFFER7                        = 1 <<  9,

  AGS_GSTREAMER_DEVOUT_ATTACK_FIRST                   = 1 << 10,

  AGS_GSTREAMER_DEVOUT_PLAY                           = 1 << 11,
  AGS_GSTREAMER_DEVOUT_SHUTDOWN                       = 1 << 12,
  AGS_GSTREAMER_DEVOUT_START_PLAY                     = 1 << 13,

  AGS_GSTREAMER_DEVOUT_NONBLOCKING                    = 1 << 14,
  AGS_GSTREAMER_DEVOUT_INITIALIZED                    = 1 << 15,
}AgsGstreamerDevoutFlags;

/**
 * AgsGstreamerDevoutSyncFlags:
 * @AGS_GSTREAMER_DEVOUT_PASS_THROUGH: do not sync
 * @AGS_GSTREAMER_DEVOUT_INITIAL_CALLBACK: initial callback
 * @AGS_GSTREAMER_DEVOUT_CALLBACK_WAIT: sync wait, soundcard conditional lock
 * @AGS_GSTREAMER_DEVOUT_CALLBACK_DONE: sync done, soundcard conditional lock
 * @AGS_GSTREAMER_DEVOUT_CALLBACK_FINISH_WAIT: sync wait, client conditional lock
 * @AGS_GSTREAMER_DEVOUT_CALLBACK_FINISH_DONE: sync done, client conditional lock
 * 
 * Enum values to control the synchronization between soundcard and client.
 */
typedef enum{
  AGS_GSTREAMER_DEVOUT_PASS_THROUGH                   = 1,
  AGS_GSTREAMER_DEVOUT_INITIAL_CALLBACK               = 1 <<  1,
  AGS_GSTREAMER_DEVOUT_CALLBACK_WAIT                  = 1 <<  2,
  AGS_GSTREAMER_DEVOUT_CALLBACK_DONE                  = 1 <<  3,
  AGS_GSTREAMER_DEVOUT_CALLBACK_FINISH_WAIT           = 1 <<  4,
  AGS_GSTREAMER_DEVOUT_CALLBACK_FINISH_DONE           = 1 <<  5,
}AgsGstreamerDevoutSyncFlags;

#define AGS_GSTREAMER_DEVOUT_ERROR (ags_gstreamer_devout_error_quark())

typedef enum{
  AGS_GSTREAMER_DEVOUT_ERROR_LOCKED_SOUNDCARD,
}AgsGstreamerDevoutError;

struct _AgsGstreamerDevout
{
  GObject gobject;

  guint flags;
  volatile guint sync_flags;
  
  GRecMutex obj_mutex;

  AgsUUID *uuid;

  guint dsp_channels;
  guint pcm_channels;
  guint format;
  guint buffer_size;
  guint samplerate;

  GRecMutex **buffer_mutex;

  guint sub_block_count;
  GRecMutex **sub_block_mutex;

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
  GObject *gstreamer_client;

  gchar **port_name;
  GList *gstreamer_port;

  GMutex callback_mutex;
  GCond callback_cond;

  GMutex callback_finish_mutex;
  GCond callback_finish_cond;

  GObject *notify_soundcard;
};

struct _AgsGstreamerDevoutClass
{
  GObjectClass gobject;
};

GType ags_gstreamer_devout_get_type();

GQuark ags_gstreamer_devout_error_quark();

gboolean ags_gstreamer_devout_test_flags(AgsGstreamerDevout *gstreamer_devout, guint flags);
void ags_gstreamer_devout_set_flags(AgsGstreamerDevout *gstreamer_devout, guint flags);
void ags_gstreamer_devout_unset_flags(AgsGstreamerDevout *gstreamer_devout, guint flags);

void ags_gstreamer_devout_switch_buffer_flag(AgsGstreamerDevout *gstreamer_devout);

void ags_gstreamer_devout_adjust_delay_and_attack(AgsGstreamerDevout *gstreamer_devout);
void ags_gstreamer_devout_realloc_buffer(AgsGstreamerDevout *gstreamer_devout);

AgsGstreamerDevout* ags_gstreamer_devout_new();

G_END_DECLS

#endif /*__AGS_GSTREAMER_DEVOUT_H__*/
