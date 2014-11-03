/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_DEVOUT_H__
#define __AGS_DEVOUT_H__

#include <pthread.h>

#include <glib.h>
#include <glib-object.h>

#include <sys/types.h>

#include <alsa/asoundlib.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_devout_thread.h>
#include <ags/thread/ags_iterator_thread.h>

#include <ags/audio/ags_recall_id.h>

#define AGS_TYPE_DEVOUT                (ags_devout_get_type())
#define AGS_DEVOUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DEVOUT, AgsDevout))
#define AGS_DEVOUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_DEVOUT, AgsDevout))
#define AGS_IS_DEVOUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_DEVOUT))
#define AGS_IS_DEVOUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_DEVOUT))
#define AGS_DEVOUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_DEVOUT, AgsDevoutClass))

#define AGS_DEVOUT_PLAY_DOMAIN(ptr)    ((AgsDevoutPlayDomain *)(ptr))
#define AGS_DEVOUT_PLAY(ptr)           ((AgsDevoutPlay *)(ptr))

#define AGS_DEVOUT_DEFAULT_SAMPLERATE (44100.0)
#define AGS_DEVOUT_DEFAULT_FORMAT (16)
#define AGS_DEVOUT_DEFAULT_BUFFER_SIZE (944)
#define AGS_DEVOUT_DEFAULT_BPM (120.0)
#define AGS_DEVOUT_DEFAULT_JIFFIE ((double) AGS_DEVOUT_DEFAULT_SAMPLERATE / (double) AGS_DEVOUT_DEFAULT_BUFFER_SIZE)

#define AGS_DEVOUT_DEFAULT_TACT (1.0 / 1.0)
#define AGS_DEVOUT_DEFAULT_TACT_JIFFIE (60.0 / AGS_DEVOUT_DEFAULT_BPM * AGS_DEVOUT_DEFAULT_TACT)
#define AGS_DEVOUT_DEFAULT_TACTRATE (1.0 / AGS_DEVOUT_DEFAULT_TACT_JIFFIE)

#define AGS_DEVOUT_DEFAULT_SCALE (1.0)
#define AGS_DEVOUT_DEFAULT_DELAY (AGS_DEVOUT_DEFAULT_JIFFIE * (60.0 / AGS_DEVOUT_DEFAULT_BPM))

typedef struct _AgsDevout AgsDevout;
typedef struct _AgsDevoutClass AgsDevoutClass;
typedef struct _AgsDevoutPlay AgsDevoutPlay;
typedef struct _AgsDevoutPlayDomain AgsDevoutPlayDomain;

typedef enum
{
  AGS_DEVOUT_BUFFER0                        = 1,
  AGS_DEVOUT_BUFFER1                        = 1 << 1,
  AGS_DEVOUT_BUFFER2                        = 1 << 2,
  AGS_DEVOUT_BUFFER3                        = 1 << 3,

  AGS_DEVOUT_ATTACK_FIRST                   = 1 << 4,

  AGS_DEVOUT_PLAY                           = 1 << 5,

  AGS_DEVOUT_LIBAO                          = 1 << 6,
  AGS_DEVOUT_OSS                            = 1 << 7,
  AGS_DEVOUT_ALSA                           = 1 << 8,

  AGS_DEVOUT_SHUTDOWN                       = 1 << 9,
  AGS_DEVOUT_START_PLAY                     = 1 << 10,

  AGS_DEVOUT_NONBLOCKING                    = 1 << 11,

  AGS_DEVOUT_TIMING_SET_0                   = 1 << 12,
  AGS_DEVOUT_TIMING_SET_1                   = 1 << 13,
}AgsDevoutFlags;

typedef enum
{
  AGS_DEVOUT_PLAY_DONE              = 1,
  AGS_DEVOUT_PLAY_REMOVE            = 1 <<  1,
  AGS_DEVOUT_PLAY_CHANNEL           = 1 <<  2,
  AGS_DEVOUT_PLAY_PAD               = 1 <<  3,
  AGS_DEVOUT_PLAY_AUDIO             = 1 <<  4,
  AGS_DEVOUT_PLAY_PLAYBACK          = 1 <<  5,
  AGS_DEVOUT_PLAY_SEQUENCER         = 1 <<  6,
  AGS_DEVOUT_PLAY_NOTATION          = 1 <<  7,
  AGS_DEVOUT_PLAY_SUPER_THREADED    = 1 <<  8,
}AgsDevoutPlayFlags;

typedef enum{
  AGS_DEVOUT_RESOLUTION_8_BIT    = 8,
  AGS_DEVOUT_RESOLUTION_16_BIT   = 16,
  AGS_DEVOUT_RESOLUTION_24_BIT   = 24,
  AGS_DEVOUT_RESOLUTION_32_BIT   = 32,
  AGS_DEVOUT_RESOLUTION_64_BIT   = 64,
}AgsDevoutResolutionMode;

#define AGS_DEVOUT_ERROR (ags_devout_error_quark())

typedef enum{
  AGS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
}AgsDevoutError;

struct _AgsDevout
{
  GObject object;

  guint flags;

  guint dsp_channels;
  guint pcm_channels;
  guint bits;
  guint buffer_size;
  guint frequency; // sample_rate

  signed short** buffer;

  double bpm; // beats per minute

  gdouble *delay; // count of tics within buffer size
  guint *attack; // where currently tic resides in the stream's offset, measured in 1/64 of bpm

  gdouble delay_counter; // next time attack changeing when delay_counter == delay
  guint tic_counter;

  union{
    struct _AgsOss{
      int device_fd;
      char *device;
    }oss;
    struct _AgsAlsa{
      char *device;
      int rc;
      snd_pcm_t *handle;
      snd_async_handler_t *ahandler;
      snd_pcm_hw_params_t *params;
    }alsa;
  }out;

  GObject *ags_main;
  
  GList *audio;
};

struct _AgsDevoutClass
{
  GObjectClass object;

  void (*play_init)(AgsDevout *devout,
		    GError **error);
  void (*play)(AgsDevout *devout,
	       GError **error);
  void (*stop)(AgsDevout *devout);

  void (*tic)(AgsDevout *devout);

  void (*note_offset_changed)(AgsDevout *devout, guint note_offset);
};

/**
 * AgsDevoutPlayDomain:
 * @domain: the source
 * @playback: if %TRUE playback is on
 * @sequencer: if %TRUE sequencer is on
 * @notation: if %TRUE notation is on
 * @devout_play: a #GList of #AgsDevoutPlay-struct
 *
 * A #AgsDevoutPlayDomain-struct represents the entire possible play/recall
 * context.
 */
struct _AgsDevoutPlayDomain
{
  GObject *domain;
  
  gboolean playback;
  gboolean sequencer;
  gboolean notation;

  GList *devout_play;
};

/**
 * AgsDevoutPlay:
 * @flags: the internal state
 * @iterator_thread: Super-threaded related #AgsThread. Index 0 playback, 1 sequencer and 2 notation.
 * @source: either #AgsChannel or #AgsRecall
 * @audio_channel: destination audio channel
 * @recall_id: array pointing to appropriate #AgsRecallID. Index 0 playback, 1 sequencer and 2 notation.
 *
 * A #AgsDevoutPlay-struct represents the play/recall in #AgsChannel or #AgsRecall
 * scope to do output to device.
 */
struct _AgsDevoutPlay
{
  guint flags;

  AgsIteratorThread **iterator_thread;

  GObject *source;
  guint audio_channel;

  AgsRecallID **recall_id;
};

GType ags_devout_get_type();

GQuark ags_devout_error_quark();

AgsDevoutPlayDomain* ags_devout_play_domain_alloc();
void ags_devout_play_domain_free(AgsDevoutPlayDomain *devout_play_domain);

AgsDevoutPlay* ags_devout_play_alloc();
void ags_devout_play_free(AgsDevoutPlay *devout_play);
AgsDevoutPlay* ags_devout_play_find_source(GList *devout_play,
					   GObject *source);

void ags_devout_list_cards(GList **card_id, GList **card_name);
void ags_devout_pcm_info(char *card_id,
			 guint *channels_min, guint *channels_max,
			 guint *rate_min, guint *rate_max,
			 guint *buffer_size_min, guint *buffer_size_max,
			 GError **error);
void ags_devout_tic(AgsDevout *devout);

void ags_devout_note_offset_changed(AgsDevout *devout, guint note_offset);

AgsDevout* ags_devout_new(GObject *ags_main);

#endif /*__AGS_DEVOUT_H__*/
