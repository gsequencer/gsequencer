/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_OSS_DEVOUT_H__
#define __AGS_OSS_DEVOUT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_OSS_DEVOUT                (ags_oss_devout_get_type())
#define AGS_OSS_DEVOUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSS_DEVOUT, AgsOssDevout))
#define AGS_OSS_DEVOUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_OSS_DEVOUT, AgsOssDevout))
#define AGS_IS_OSS_DEVOUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSS_DEVOUT))
#define AGS_IS_OSS_DEVOUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSS_DEVOUT))
#define AGS_OSS_DEVOUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_OSS_DEVOUT, AgsOssDevoutClass))

#define AGS_OSS_DEVOUT_GET_OBJ_MUTEX(obj) (&(((AgsOssDevout *) obj)->obj_mutex))

#define AGS_OSS_DEVOUT_DEFAULT_OSS_DEVICE "/dev/dsp"
#define AGS_OSS_DEVOUT_DEFAULT_OSS_MIXER_DEVICE "/dev/mixer"

#define AGS_OSS_DEVOUT_DEFAULT_APP_BUFFER_SIZE (4)
#define AGS_OSS_DEVOUT_DEFAULT_BACKEND_BUFFER_SIZE (8)

typedef struct _AgsOssDevout AgsOssDevout;
typedef struct _AgsOssDevoutClass AgsOssDevoutClass;

/**
 * AgsOssDevoutFlags:
 * @AGS_OSS_DEVOUT_INITIALIZED: the soundcard was initialized
 * @AGS_OSS_DEVOUT_START_PLAY: playback starting
 * @AGS_OSS_DEVOUT_PLAY: devout is running
 * @AGS_OSS_DEVOUT_SHUTDOWN: stop playback
 * @AGS_OSS_DEVOUT_NONBLOCKING: do non-blocking calls
 * @AGS_OSS_DEVOUT_ATTACK_FIRST: use first attack, instead of second one
 * 
 * Enum values to control the behavior or indicate internal state of #AgsOssDevout by
 * enable/disable as flags.
 */
typedef enum
{
  AGS_OSS_DEVOUT_INITIALIZED        = 1,

  AGS_OSS_DEVOUT_PLAY               = 1 <<  1,
  AGS_OSS_DEVOUT_START_PLAY         = 1 <<  2,
  AGS_OSS_DEVOUT_SHUTDOWN           = 1 <<  3,

  AGS_OSS_DEVOUT_NONBLOCKING        = 1 <<  4,
  
  AGS_OSS_DEVOUT_ATTACK_FIRST       = 1 <<  5,
}AgsOssDevoutFlags;

/**
 * AgsOssDevoutAppBufferMode:
 * @AGS_OSS_DEVOUT_APP_BUFFER_0: ring-buffer 0
 * @AGS_OSS_DEVOUT_APP_BUFFER_1: ring-buffer 1
 * @AGS_OSS_DEVOUT_APP_BUFFER_2: ring-buffer 2
 * @AGS_OSS_DEVOUT_APP_BUFFER_3: ring-buffer 3
 * 
 * Enum values to indicate internal state of #AgsOssDevout application buffer by
 * setting mode.
 */
typedef enum{
  AGS_OSS_DEVOUT_APP_BUFFER_0,
  AGS_OSS_DEVOUT_APP_BUFFER_1,
  AGS_OSS_DEVOUT_APP_BUFFER_2,
  AGS_OSS_DEVOUT_APP_BUFFER_3,
}AgsOssDevoutAppBufferMode;

/**
 * AgsOssDevoutBackendBufferMode:
 * @AGS_OSS_DEVOUT_BACKEND_BUFFER_0: ring-buffer 0
 * @AGS_OSS_DEVOUT_BACKEND_BUFFER_1: ring-buffer 1
 * @AGS_OSS_DEVOUT_BACKEND_BUFFER_2: ring-buffer 2
 * @AGS_OSS_DEVOUT_BACKEND_BUFFER_3: ring-buffer 3
 * @AGS_OSS_DEVOUT_BACKEND_BUFFER_4: ring-buffer 4
 * @AGS_OSS_DEVOUT_BACKEND_BUFFER_5: ring-buffer 5
 * @AGS_OSS_DEVOUT_BACKEND_BUFFER_6: ring-buffer 6
 * @AGS_OSS_DEVOUT_BACKEND_BUFFER_7: ring-buffer 7
 * 
 * Enum values to indicate internal state of #AgsOssDevout backend buffer by
 * setting mode.
 */
typedef enum{
  AGS_OSS_DEVOUT_BACKEND_BUFFER_0,
  AGS_OSS_DEVOUT_BACKEND_BUFFER_1,
  AGS_OSS_DEVOUT_BACKEND_BUFFER_2,
  AGS_OSS_DEVOUT_BACKEND_BUFFER_3,
  AGS_OSS_DEVOUT_BACKEND_BUFFER_4,
  AGS_OSS_DEVOUT_BACKEND_BUFFER_5,
  AGS_OSS_DEVOUT_BACKEND_BUFFER_6,
  AGS_OSS_DEVOUT_BACKEND_BUFFER_7,
}AgsOssDevoutBackendBufferMode;

#define AGS_OSS_DEVOUT_ERROR (ags_oss_devout_error_quark())

typedef enum{
  AGS_OSS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
  AGS_OSS_DEVOUT_ERROR_BROKEN_CONFIGURATION,
  AGS_OSS_DEVOUT_ERROR_ACCESS_TYPE_NOT_AVAILABLE,
  AGS_OSS_DEVOUT_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
  AGS_OSS_DEVOUT_ERROR_CHANNELS_NOT_AVAILABLE,
  AGS_OSS_DEVOUT_ERROR_SAMPLERATE_NOT_AVAILABLE,
  AGS_OSS_DEVOUT_ERROR_BUFFER_SIZE_NOT_AVAILABLE,
  AGS_OSS_DEVOUT_ERROR_HW_PARAMETERS_NOT_AVAILABLE,
}AgsOssDevoutError;

struct _AgsOssDevout
{
  GObject gobject;

  AgsOssDevoutFlags flags;
  AgsConnectableFlags connectable_flags;
  
  GRecMutex obj_mutex;

  AgsUUID *uuid;

  guint dsp_channels;
  guint pcm_channels;
  AgsSoundcardFormat format;
  guint buffer_size;
  guint samplerate;
  
  AgsOssDevoutBackendBufferMode app_buffer_mode;
  
  GRecMutex **app_buffer_mutex;

  guint sub_block_count;
  GRecMutex **sub_block_mutex;

  void **app_buffer;

  gboolean available;
  
  guint backend_buffer_mode;
  
  guchar **backend_buffer;

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
  
  int device_fd;
  char *device;

  GList *io_channel;
  GList *tag;

  gint64 poll_timeout;

  gdouble note_256th_delay;

  GList *note_256th_attack;

  guint note_256th_offset;
  guint note_256th_offset_last;

  guint note_256th_attack_of_16th_pulse;
  guint note_256th_attack_of_16th_pulse_position;

  gdouble note_256th_delay_counter;
};

struct _AgsOssDevoutClass
{
  GObjectClass gobject;
};

GType ags_oss_devout_get_type();
GType ags_oss_devout_flags_get_type();

GQuark ags_oss_devout_error_quark();

gboolean ags_oss_devout_test_flags(AgsOssDevout *oss_devout, AgsOssDevoutFlags flags);
void ags_oss_devout_set_flags(AgsOssDevout *oss_devout, AgsOssDevoutFlags flags);
void ags_oss_devout_unset_flags(AgsOssDevout *oss_devout, AgsOssDevoutFlags flags);

void ags_oss_devout_switch_buffer_flag(AgsOssDevout *oss_devout);

void ags_oss_devout_adjust_delay_and_attack(AgsOssDevout *oss_devout);
void ags_oss_devout_realloc_buffer(AgsOssDevout *oss_devout);

AgsOssDevout* ags_oss_devout_new();

G_END_DECLS

#endif /*__AGS_OSS_DEVOUT_H__*/
