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

#ifndef __AGS_FIFOOUT_H__
#define __AGS_FIFOOUT_H__

#include <glib.h>
#include <glib-object.h>

#include <sys/types.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_FIFOOUT                (ags_fifoout_get_type())
#define AGS_FIFOOUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FIFOOUT, AgsFifoout))
#define AGS_FIFOOUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_FIFOOUT, AgsFifoout))
#define AGS_IS_FIFOOUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FIFOOUT))
#define AGS_IS_FIFOOUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FIFOOUT))
#define AGS_FIFOOUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_FIFOOUT, AgsFifooutClass))

#define AGS_FIFOOUT_GET_OBJ_MUTEX(obj) (&(((AgsFifoout *) obj)->obj_mutex))

#define AGS_FIFOOUT_DEFAULT_DEVICE "/dev/null"

#define AGS_FIFOOUT_DEFAULT_APP_BUFFER_SIZE (4)
#define AGS_FIFOOUT_DEFAULT_RING_BUFFER_SIZE (8)

typedef struct _AgsFifoout AgsFifoout;
typedef struct _AgsFifooutClass AgsFifooutClass;

/**
 * AgsFifooutFlags:
 * @AGS_FIFOOUT_INITIALIZED: the soundcard was initialized
 * @AGS_FIFOOUT_START_PLAY: playback starting
 * @AGS_FIFOOUT_PLAY: use first attack, instead of second one
 * @AGS_FIFOOUT_SHUTDOWN: stop playback
 * @AGS_FIFOOUT_NONBLOCKING: do non-blocking calls
 * @AGS_FIFOOUT_ATTACK_FIRST: use first attack, instead of second one
 * 
 * Enum values to control the behavior or indicate internal state of #AgsFifoout by
 * enable/disable as flags.
 */
typedef enum
{
  AGS_FIFOOUT_INITIALIZED        = 1,

  AGS_FIFOOUT_START_PLAY         = 1 <<  1,
  AGS_FIFOOUT_PLAY               = 1 <<  2,
  AGS_FIFOOUT_SHUTDOWN           = 1 <<  3,

  AGS_FIFOOUT_NONBLOCKING        = 1 <<  4,
  
  AGS_FIFOOUT_ATTACK_FIRST       = 1 <<  5,
}AgsFifooutFlags;

/**
 * AgsFifooutAppBufferMode:
 * @AGS_FIFOOUT_APP_BUFFER_0: ring-buffer 0
 * @AGS_FIFOOUT_APP_BUFFER_1: ring-buffer 1
 * @AGS_FIFOOUT_APP_BUFFER_2: ring-buffer 2
 * @AGS_FIFOOUT_APP_BUFFER_3: ring-buffer 3
 * 
 * Enum values to indicate internal state of #AgsFifoout application buffer by
 * setting mode.
 */
typedef enum{
  AGS_FIFOOUT_APP_BUFFER_0,
  AGS_FIFOOUT_APP_BUFFER_1,
  AGS_FIFOOUT_APP_BUFFER_2,
  AGS_FIFOOUT_APP_BUFFER_3,
}AgsFifooutAppBufferMode;

#define AGS_FIFOOUT_ERROR (ags_fifoout_error_quark())

typedef enum{
  AGS_FIFOOUT_ERROR_LOCKED_SOUNDCARD,
}AgsFifooutError;

struct _AgsFifoout
{
  GObject gobject;

  guint flags;
  guint connectable_flags;

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

  volatile gboolean available;
  
  guint ring_buffer_size;
  guint nth_ring_buffer;

  unsigned char **ring_buffer;

  double bpm; // beats per minute
  gdouble delay_factor;
  
  gdouble *delay; // count of tics within buffer size
  guint *attack; // where currently tic resides in the stream's offset, measured in 1/64 of bpm

  gdouble tact_counter;
  gdouble delay_counter; // next time attack changeing when delay_counter == delay
  guint tic_counter; // in the range of default period

  guint note_offset;
  guint note_offset_absolute;

  guint loop_left;
  guint loop_right;
  gboolean do_loop;
  
  guint loop_offset;

  gchar *device;
  int fifo_fd;
};

struct _AgsFifooutClass
{
  GObjectClass gobject;
};

GType ags_fifoout_get_type();
GType ags_fifoout_flags_get_type();

GQuark ags_fifoout_error_quark();

gboolean ags_fifoout_test_flags(AgsFifoout *fifoout, guint flags);
void ags_fifoout_set_flags(AgsFifoout *fifoout, guint flags);
void ags_fifoout_unset_flags(AgsFifoout *fifoout, guint flags);

void ags_fifoout_switch_buffer_flag(AgsFifoout *fifoout);

void ags_fifoout_adjust_delay_and_attack(AgsFifoout *fifoout);
void ags_fifoout_realloc_buffer(AgsFifoout *fifoout);

AgsFifoout* ags_fifoout_new();

G_END_DECLS

#endif /*__AGS_FIFOOUT_H__*/
