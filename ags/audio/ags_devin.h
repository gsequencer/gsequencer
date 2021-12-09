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

#ifndef __AGS_DEVIN_H__
#define __AGS_DEVIN_H__

#include <glib.h>
#include <glib-object.h>

#include <sys/types.h>
#include <alsa/asoundlib.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_DEVIN                (ags_devin_get_type())
#define AGS_DEVIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DEVIN, AgsDevin))
#define AGS_DEVIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_DEVIN, AgsDevin))
#define AGS_IS_DEVIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_DEVIN))
#define AGS_IS_DEVIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_DEVIN))
#define AGS_DEVIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_DEVIN, AgsDevinClass))

#define AGS_DEVIN_GET_OBJ_MUTEX(obj) (&(((AgsDevin *) obj)->obj_mutex))

#define AGS_DEVIN_DEFAULT_ALSA_DEVICE "hw:0,0"
#define AGS_DEVIN_DEFAULT_OSS_DEVICE "/dev/dsp"

#define AGS_DEVIN_DEFAULT_RING_BUFFER_SIZE (8)

typedef struct _AgsDevin AgsDevin;
typedef struct _AgsDevinClass AgsDevinClass;

/**
 * AgsDevinFlags:
 * @AGS_DEVIN_ADDED_TO_REGISTRY: the devin was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_DEVIN_CONNECTED: indicates the devin was connected by calling #AgsConnectable::connect()
 * @AGS_DEVIN_BUFFER0: ring-buffer 0
 * @AGS_DEVIN_BUFFER1: ring-buffer 1
 * @AGS_DEVIN_BUFFER2: ring-buffer 2
 * @AGS_DEVIN_BUFFER3: ring-buffer 3
 * @AGS_DEVIN_ATTACK_FIRST: use first attack, instead of second one
 * @AGS_DEVIN_RECORD: do capture
 * @AGS_DEVIN_OSS: use OSS4 backend
 * @AGS_DEVIN_ALSA: use ALSA backend
 * @AGS_DEVIN_SHUTDOWN: stop capture
 * @AGS_DEVIN_START_RECORD: capture starting
 * @AGS_DEVIN_NONBLOCKING: do non-blocking calls
 * @AGS_DEVIN_INITIALIZED: the soundcard was initialized
 * 
 * Enum values to control the behavior or indicate internal state of #AgsDevin by
 * enable/disable as flags.
 */
typedef enum
{
  AGS_DEVIN_ADDED_TO_REGISTRY  = 1,
  AGS_DEVIN_CONNECTED          = 1 <<  1,

  AGS_DEVIN_BUFFER0            = 1 <<  2,
  AGS_DEVIN_BUFFER1            = 1 <<  3,
  AGS_DEVIN_BUFFER2            = 1 <<  4,
  AGS_DEVIN_BUFFER3            = 1 <<  5,

  AGS_DEVIN_ATTACK_FIRST       = 1 <<  6,

  AGS_DEVIN_RECORD             = 1 <<  7,

  AGS_DEVIN_OSS                = 1 <<  8,
  AGS_DEVIN_ALSA               = 1 <<  9,

  AGS_DEVIN_SHUTDOWN           = 1 << 10,
  AGS_DEVIN_START_RECORD       = 1 << 11,

  AGS_DEVIN_NONBLOCKING        = 1 << 12,
  AGS_DEVIN_INITIALIZED        = 1 << 13,
}AgsDevinFlags;

#define AGS_DEVIN_ERROR (ags_devin_error_quark())

typedef enum{
  AGS_DEVIN_ERROR_LOCKED_SOUNDCARD,
  AGS_DEVIN_ERROR_BROKEN_CONFIGURATION,
  AGS_DEVIN_ERROR_ACCESS_TYPE_NOT_AVAILABLE,
  AGS_DEVIN_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
  AGS_DEVIN_ERROR_CHANNELS_NOT_AVAILABLE,
  AGS_DEVIN_ERROR_SAMPLERATE_NOT_AVAILABLE,
  AGS_DEVIN_ERROR_BUFFER_SIZE_NOT_AVAILABLE,
  AGS_DEVIN_ERROR_HW_PARAMETERS_NOT_AVAILABLE,
}AgsDevinError;

struct _AgsDevin
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  AgsUUID *uuid;

  guint dsp_channels;
  guint pcm_channels;
  guint format;
  guint buffer_size;
  guint samplerate; // sample_rate

  GRecMutex **buffer_mutex;
  void **buffer;

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

  guint start_note_offset;
  guint note_offset;
  guint note_offset_absolute;
  
  guint loop_left;
  guint loop_right;
  gboolean do_loop;
  
  guint loop_offset;
  
  union{
    struct _AgsOssIn{
      int device_fd;
      char *device;
    }oss;
#ifdef AGS_WITH_ALSA
    struct _AgsAlsaIn{
      char *device;
      int rc;
      snd_pcm_t *handle;
      snd_async_handler_t *ahandler;
      snd_pcm_hw_params_t *params;
    }alsa;
#else
    struct _AgsAlsaDummyIn{
      char *device;
      int rc;
      void *handle;
      void *ahandler;
      void *params;
    }alsa;
#endif
  }out;
};

struct _AgsDevinClass
{
  GObjectClass gobject;
};

G_DEPRECATED
GType ags_devin_get_type();
G_DEPRECATED
GType ags_devin_flags_get_type();

G_DEPRECATED
GQuark ags_devin_error_quark();

G_DEPRECATED
gboolean ags_devin_test_flags(AgsDevin *devin, guint flags);
G_DEPRECATED
void ags_devin_set_flags(AgsDevin *devin, guint flags);
G_DEPRECATED
void ags_devin_unset_flags(AgsDevin *devin, guint flags);

G_DEPRECATED
void ags_devin_switch_buffer_flag(AgsDevin *devin);

G_DEPRECATED
void ags_devin_adjust_delay_and_attack(AgsDevin *devin);
G_DEPRECATED
void ags_devin_realloc_buffer(AgsDevin *devin);

G_DEPRECATED
AgsDevin* ags_devin_new();

G_END_DECLS

#endif /*__AGS_DEVIN_H__*/
