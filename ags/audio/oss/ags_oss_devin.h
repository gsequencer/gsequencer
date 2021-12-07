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

#ifndef __AGS_OSS_DEVIN_H__
#define __AGS_OSS_DEVIN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_OSS_DEVIN                (ags_oss_devin_get_type())
#define AGS_OSS_DEVIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSS_DEVIN, AgsOssDevin))
#define AGS_OSS_DEVIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_OSS_DEVIN, AgsOssDevin))
#define AGS_IS_OSS_DEVIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSS_DEVIN))
#define AGS_IS_OSS_DEVIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSS_DEVIN))
#define AGS_OSS_DEVIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_OSS_DEVIN, AgsOssDevinClass))

#define AGS_OSS_DEVIN_GET_OBJ_MUTEX(obj) (&(((AgsOssDevin *) obj)->obj_mutex))

#define AGS_OSS_DEVIN_DEFAULT_OSS_DEVICE "/dev/dsp"

#define AGS_OSS_DEVIN_DEFAULT_APP_BUFFER_SIZE (4)
#define AGS_OSS_DEVIN_DEFAULT_BACKEND_BUFFER_SIZE (8)

typedef struct _AgsOssDevin AgsOssDevin;
typedef struct _AgsOssDevinClass AgsOssDevinClass;

/**
 * AgsOssDevinFlags:
 * @AGS_OSS_DEVIN_ADDED_TO_REGISTRY: the oss_devin was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_OSS_DEVIN_CONNECTED: indicates the oss_devin was connected by calling #AgsConnectable::connect()
 * @AGS_OSS_DEVIN_INITIALIZED: the soundcard was initialized
 * @AGS_OSS_DEVIN_START_RECORD: capture starting
 * @AGS_OSS_DEVIN_RECORD: do capture
 * @AGS_OSS_DEVIN_SHUTDOWN: stop capture
 * @AGS_OSS_DEVIN_NONBLOCKING: do non-blocking calls
 * @AGS_OSS_DEVIN_ATTACK_FIRST: use first attack, instead of second one
 * 
 * Enum values to control the behavior or indicate internal state of #AgsOssDevin by
 * enable/disable as flags.
 */
typedef enum
{
  AGS_OSS_DEVIN_ADDED_TO_REGISTRY  = 1,
  AGS_OSS_DEVIN_CONNECTED          = 1 <<  1,

  AGS_OSS_DEVIN_INITIALIZED        = 1 <<  2,

  AGS_OSS_DEVIN_START_RECORD       = 1 <<  3,
  AGS_OSS_DEVIN_RECORD             = 1 <<  4,
  AGS_OSS_DEVIN_SHUTDOWN           = 1 <<  5,

  AGS_OSS_DEVIN_NONBLOCKING        = 1 <<  6,

  AGS_OSS_DEVIN_ATTACK_FIRST       = 1 <<  7,
}AgsOssDevinFlags;

/**
 * AgsOssDevinAppBufferMode:
 * @AGS_OSS_DEVIN_APP_BUFFER_0: ring-buffer 0
 * @AGS_OSS_DEVIN_APP_BUFFER_1: ring-buffer 1
 * @AGS_OSS_DEVIN_APP_BUFFER_2: ring-buffer 2
 * @AGS_OSS_DEVIN_APP_BUFFER_3: ring-buffer 3
 * 
 * Enum values to indicate internal state of #AgsOssDevin application buffer by
 * setting mode.
 */
typedef enum{
  AGS_OSS_DEVIN_APP_BUFFER_0,
  AGS_OSS_DEVIN_APP_BUFFER_1,
  AGS_OSS_DEVIN_APP_BUFFER_2,
  AGS_OSS_DEVIN_APP_BUFFER_3,
}AgsOssDevinAppBufferMode;

/**
 * AgsOssDevinBackendBufferMode:
 * @AGS_OSS_DEVIN_BACKEND_BUFFER_0: ring-buffer 0
 * @AGS_OSS_DEVIN_BACKEND_BUFFER_1: ring-buffer 1
 * @AGS_OSS_DEVIN_BACKEND_BUFFER_2: ring-buffer 2
 * @AGS_OSS_DEVIN_BACKEND_BUFFER_3: ring-buffer 3
 * @AGS_OSS_DEVIN_BACKEND_BUFFER_4: ring-buffer 4
 * @AGS_OSS_DEVIN_BACKEND_BUFFER_5: ring-buffer 5
 * @AGS_OSS_DEVIN_BACKEND_BUFFER_6: ring-buffer 6
 * @AGS_OSS_DEVIN_BACKEND_BUFFER_7: ring-buffer 7
 * 
 * Enum values to indicate internal state of #AgsOssDevin backend buffer by
 * setting mode.
 */
typedef enum{
  AGS_OSS_DEVIN_BACKEND_BUFFER_0,
  AGS_OSS_DEVIN_BACKEND_BUFFER_1,
  AGS_OSS_DEVIN_BACKEND_BUFFER_2,
  AGS_OSS_DEVIN_BACKEND_BUFFER_3,
  AGS_OSS_DEVIN_BACKEND_BUFFER_4,
  AGS_OSS_DEVIN_BACKEND_BUFFER_5,
  AGS_OSS_DEVIN_BACKEND_BUFFER_6,
  AGS_OSS_DEVIN_BACKEND_BUFFER_7,
}AgsOssDevinBackendBufferMode;

#define AGS_OSS_DEVIN_ERROR (ags_oss_devin_error_quark())

typedef enum{
  AGS_OSS_DEVIN_ERROR_LOCKED_SOUNDCARD,
  AGS_OSS_DEVIN_ERROR_BROKEN_CONFIGURATION,
  AGS_OSS_DEVIN_ERROR_ACCESS_TYPE_NOT_AVAILABLE,
  AGS_OSS_DEVIN_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
  AGS_OSS_DEVIN_ERROR_CHANNELS_NOT_AVAILABLE,
  AGS_OSS_DEVIN_ERROR_SAMPLERATE_NOT_AVAILABLE,
  AGS_OSS_DEVIN_ERROR_BUFFER_SIZE_NOT_AVAILABLE,
  AGS_OSS_DEVIN_ERROR_HW_PARAMETERS_NOT_AVAILABLE,
}AgsOssDevinError;

struct _AgsOssDevin
{
  GObject gobject;

  guint flags;

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
  GRecMutex **sub_block_mutex; //TODO:JK: implement reading from soundcard by locking sub block

  void **app_buffer;

  volatile gboolean available;
  
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
};

struct _AgsOssDevinClass
{
  GObjectClass gobject;
};

GType ags_oss_devin_get_type();
GType ags_oss_devin_flags_get_type();

GQuark ags_oss_devin_error_quark();

gboolean ags_oss_devin_test_flags(AgsOssDevin *oss_devin, guint flags);
void ags_oss_devin_set_flags(AgsOssDevin *oss_devin, guint flags);
void ags_oss_devin_unset_flags(AgsOssDevin *oss_devin, guint flags);

void ags_oss_devin_switch_buffer_flag(AgsOssDevin *oss_devin);

void ags_oss_devin_adjust_delay_and_attack(AgsOssDevin *oss_devin);
void ags_oss_devin_realloc_buffer(AgsOssDevin *oss_devin);

AgsOssDevin* ags_oss_devin_new();

G_END_DECLS

#endif /*__AGS_OSS_DEVIN_H__*/
