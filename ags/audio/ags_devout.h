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

#ifndef __AGS_DEVOUT_H__
#define __AGS_DEVOUT_H__

#include <glib.h>
#include <glib-object.h>

#include <sys/types.h>
#include <alsa/asoundlib.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_DEVOUT                (ags_devout_get_type())
#define AGS_DEVOUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DEVOUT, AgsDevout))
#define AGS_DEVOUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_DEVOUT, AgsDevout))
#define AGS_IS_DEVOUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_DEVOUT))
#define AGS_IS_DEVOUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_DEVOUT))
#define AGS_DEVOUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_DEVOUT, AgsDevoutClass))

#define AGS_DEVOUT_GET_OBJ_MUTEX(obj) (&(((AgsDevout *) obj)->obj_mutex))

#define AGS_DEVOUT_DEFAULT_ALSA_DEVICE "hw:0,0"
#define AGS_DEVOUT_DEFAULT_OSS_DEVICE "/dev/dsp"

#define AGS_DEVOUT_DEFAULT_RING_BUFFER_SIZE (8)

typedef struct _AgsDevout AgsDevout;
typedef struct _AgsDevoutClass AgsDevoutClass;

/**
 * AgsDevoutFlags:
 * @AGS_DEVOUT_ADDED_TO_REGISTRY: the devout was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_DEVOUT_CONNECTED: indicates the devout was connected by calling #AgsConnectable::connect()
 * @AGS_DEVOUT_BUFFER0: ring-buffer 0
 * @AGS_DEVOUT_BUFFER1: ring-buffer 1
 * @AGS_DEVOUT_BUFFER2: ring-buffer 2
 * @AGS_DEVOUT_BUFFER3: ring-buffer 3
 * @AGS_DEVOUT_ATTACK_FIRST: use first attack, instead of second one
 * @AGS_DEVOUT_PLAY: devout is running
 * @AGS_DEVOUT_OSS: use OSS4 backend
 * @AGS_DEVOUT_ALSA: use ALSA backend
 * @AGS_DEVOUT_SHUTDOWN: stop playback
 * @AGS_DEVOUT_START_PLAY: playback starting
 * @AGS_DEVOUT_NONBLOCKING: do non-blocking calls
 * @AGS_DEVOUT_INITIALIZED: the soundcard was initialized
 * 
 * Enum values to control the behavior or indicate internal state of #AgsDevout by
 * enable/disable as flags.
 */
typedef enum
{
  AGS_DEVOUT_ADDED_TO_REGISTRY  = 1,
  AGS_DEVOUT_CONNECTED          = 1 <<  1,

  AGS_DEVOUT_BUFFER0            = 1 <<  2,
  AGS_DEVOUT_BUFFER1            = 1 <<  3,
  AGS_DEVOUT_BUFFER2            = 1 <<  4,
  AGS_DEVOUT_BUFFER3            = 1 <<  5,

  AGS_DEVOUT_ATTACK_FIRST       = 1 <<  6,

  AGS_DEVOUT_PLAY               = 1 <<  7,

  AGS_DEVOUT_OSS                = 1 <<  8,
  AGS_DEVOUT_ALSA               = 1 <<  9,

  AGS_DEVOUT_SHUTDOWN           = 1 << 10,
  AGS_DEVOUT_START_PLAY         = 1 << 11,

  AGS_DEVOUT_NONBLOCKING        = 1 << 12,
  AGS_DEVOUT_INITIALIZED        = 1 << 13,
}AgsDevoutFlags;

#define AGS_DEVOUT_ERROR (ags_devout_error_quark())

typedef enum{
  AGS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
  AGS_DEVOUT_ERROR_BROKEN_CONFIGURATION,
  AGS_DEVOUT_ERROR_ACCESS_TYPE_NOT_AVAILABLE,
  AGS_DEVOUT_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
  AGS_DEVOUT_ERROR_CHANNELS_NOT_AVAILABLE,
  AGS_DEVOUT_ERROR_SAMPLERATE_NOT_AVAILABLE,
  AGS_DEVOUT_ERROR_BUFFER_SIZE_NOT_AVAILABLE,
  AGS_DEVOUT_ERROR_HW_PARAMETERS_NOT_AVAILABLE,
}AgsDevoutError;

struct _AgsDevout
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

  guint sub_block_count;
  GRecMutex **sub_block_mutex;

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
    struct _AgsOssOut{
      int device_fd;
      char *device;
    }oss;
#ifdef AGS_WITH_ALSA
    struct _AgsAlsaOut{
      char *device;
      int rc;
      snd_pcm_t *handle;
      snd_async_handler_t *ahandler;
      snd_pcm_hw_params_t *params;
    }alsa;
#else
    struct _AgsAlsaDummyOut{
      char *device;
      int rc;
      void *handle;
      void *ahandler;
      void *params;
    }alsa;
#endif
  }out;

  GList *io_channel;
  GList *tag;
};

struct _AgsDevoutClass
{
  GObjectClass gobject;
};

GType ags_devout_get_type();
GType ags_devout_flags_get_type();

GQuark ags_devout_error_quark();

gboolean ags_devout_test_flags(AgsDevout *devout, guint flags);
void ags_devout_set_flags(AgsDevout *devout, guint flags);
void ags_devout_unset_flags(AgsDevout *devout, guint flags);

void ags_devout_switch_buffer_flag(AgsDevout *devout);

void ags_devout_adjust_delay_and_attack(AgsDevout *devout);
void ags_devout_realloc_buffer(AgsDevout *devout);

AgsDevout* ags_devout_new();

G_END_DECLS

#endif /*__AGS_DEVOUT_H__*/
