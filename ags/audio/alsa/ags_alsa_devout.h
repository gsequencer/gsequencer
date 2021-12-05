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

#ifndef __AGS_ALSA_DEVOUT_H__
#define __AGS_ALSA_DEVOUT_H__

#include <glib.h>
#include <glib-object.h>

#include <sys/types.h>
#include <alsa/asoundlib.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_ALSA_DEVOUT                (ags_alsa_devout_get_type())
#define AGS_ALSA_DEVOUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ALSA_DEVOUT, AgsAlsaDevout))
#define AGS_ALSA_DEVOUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_ALSA_DEVOUT, AgsAlsaDevout))
#define AGS_IS_ALSA_DEVOUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_ALSA_DEVOUT))
#define AGS_IS_ALSA_DEVOUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_ALSA_DEVOUT))
#define AGS_ALSA_DEVOUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_ALSA_DEVOUT, AgsAlsaDevoutClass))

#define AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(obj) (&(((AgsAlsaDevout *) obj)->obj_mutex))

#define AGS_ALSA_DEVOUT_DEFAULT_ALSA_DEVICE "hw:0,0"

#define AGS_ALSA_DEVOUT_DEFAULT_RING_BUFFER_SIZE (8)

typedef struct _AgsAlsaDevout AgsAlsaDevout;
typedef struct _AgsAlsaDevoutClass AgsAlsaDevoutClass;

/**
 * AgsAlsaDevoutFlags:
 * @AGS_ALSA_DEVOUT_ADDED_TO_REGISTRY: the alsa_devout was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_ALSA_DEVOUT_CONNECTED: indicates the alsa_devout was connected by calling #AgsConnectable::connect()
 * @AGS_ALSA_DEVOUT_BUFFER0: ring-buffer 0
 * @AGS_ALSA_DEVOUT_BUFFER1: ring-buffer 1
 * @AGS_ALSA_DEVOUT_BUFFER2: ring-buffer 2
 * @AGS_ALSA_DEVOUT_BUFFER3: ring-buffer 3
 * @AGS_ALSA_DEVOUT_ATTACK_FIRST: use first attack, instead of second one
 * @AGS_ALSA_DEVOUT_PLAY: devout is running
 * @AGS_ALSA_DEVOUT_SHUTDOWN: stop playback
 * @AGS_ALSA_DEVOUT_START_PLAY: playback starting
 * @AGS_ALSA_DEVOUT_NONBLOCKING: do non-blocking calls
 * @AGS_ALSA_DEVOUT_INITIALIZED: the soundcard was initialized
 * 
 * Enum values to control the behavior or indicate internal state of #AgsAlsaDevout by
 * enable/disable as flags.
 */
typedef enum
{
  AGS_ALSA_DEVOUT_ADDED_TO_REGISTRY  = 1,
  AGS_ALSA_DEVOUT_CONNECTED          = 1 <<  1,

  AGS_ALSA_DEVOUT_BUFFER0            = 1 <<  2,
  AGS_ALSA_DEVOUT_BUFFER1            = 1 <<  3,
  AGS_ALSA_DEVOUT_BUFFER2            = 1 <<  4,
  AGS_ALSA_DEVOUT_BUFFER3            = 1 <<  5,

  AGS_ALSA_DEVOUT_ATTACK_FIRST       = 1 <<  6,

  AGS_ALSA_DEVOUT_PLAY               = 1 <<  7,

  AGS_ALSA_DEVOUT_SHUTDOWN           = 1 <<  8,
  AGS_ALSA_DEVOUT_START_PLAY         = 1 <<  9,

  AGS_ALSA_DEVOUT_NONBLOCKING        = 1 << 10,
  AGS_ALSA_DEVOUT_INITIALIZED        = 1 << 11,
}AgsAlsaDevoutFlags;

#define AGS_ALSA_DEVOUT_ERROR (ags_alsa_devout_error_quark())

typedef enum{
  AGS_ALSA_DEVOUT_ERROR_LOCKED_SOUNDCARD,
  AGS_ALSA_DEVOUT_ERROR_BROKEN_CONFIGURATION,
  AGS_ALSA_DEVOUT_ERROR_ACCESS_TYPE_NOT_AVAILABLE,
  AGS_ALSA_DEVOUT_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
  AGS_ALSA_DEVOUT_ERROR_CHANNELS_NOT_AVAILABLE,
  AGS_ALSA_DEVOUT_ERROR_SAMPLERATE_NOT_AVAILABLE,
  AGS_ALSA_DEVOUT_ERROR_BUFFER_SIZE_NOT_AVAILABLE,
  AGS_ALSA_DEVOUT_ERROR_HW_PARAMETERS_NOT_AVAILABLE,
}AgsAlsaDevoutError;

struct _AgsAlsaDevout
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
  
  char *device;
  int rc;
  snd_pcm_t *handle;
  snd_async_handler_t *ahandler;
  snd_pcm_hw_params_t *params;

  GList *io_channel;
  GList *tag;
};

struct _AgsAlsaDevoutClass
{
  GObjectClass gobject;
};

GType ags_alsa_devout_get_type();
GType ags_alsa_devout_flags_get_type();

GQuark ags_alsa_devout_error_quark();

gboolean ags_alsa_devout_test_flags(AgsAlsaDevout *alsa_devout, guint flags);
void ags_alsa_devout_set_flags(AgsAlsaDevout *alsa_devout, guint flags);
void ags_alsa_devout_unset_flags(AgsAlsaDevout *alsa_devout, guint flags);

void ags_alsa_devout_switch_buffer_flag(AgsAlsaDevout *alsa_devout);

void ags_alsa_devout_adjust_delay_and_attack(AgsAlsaDevout *alsa_devout);
void ags_alsa_devout_realloc_buffer(AgsAlsaDevout *alsa_devout);

AgsAlsaDevout* ags_alsa_devout_new();

G_END_DECLS

#endif /*__AGS_ALSA_DEVOUT_H__*/
