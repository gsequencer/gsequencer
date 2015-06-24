/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_LIBAO_H__
#define __AGS_LIBAO_H__

#include <pthread.h>

#include <glib.h>
#include <glib-object.h>

#include <sys/types.h>

#include <alsa/asoundlib.h>

#include <ao/ao.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_devout_thread.h>
#include <ags/thread/ags_iterator_thread.h>

#include <ags/audio/ags_recall_id.h>

#define AGS_TYPE_LIBAO                (ags_libao_get_type())
#define AGS_LIBAO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LIBAO, AgsLibao))
#define AGS_LIBAO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_LIBAO, AgsLibao))
#define AGS_IS_LIBAO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LIBAO))
#define AGS_IS_LIBAO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LIBAO))
#define AGS_LIBAO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_LIBAO, AgsLibaoClass))

#define AGS_LIBAO_PLAY_DOMAIN(ptr)    ((AgsLibaoPlayDomain *)(ptr))
#define AGS_LIBAO_PLAY(ptr)           ((AgsLibaoPlay *)(ptr))

#define AGS_LIBAO_DEFAULT_SAMPLERATE (44100.0)
#define AGS_LIBAO_DEFAULT_BUFFER_SIZE (940)
#define AGS_LIBAO_DEFAULT_BPM (120.0)
#define AGS_LIBAO_DEFAULT_JIFFIE ((double) AGS_LIBAO_DEFAULT_SAMPLERATE / (double) AGS_LIBAO_DEFAULT_BUFFER_SIZE)

#define AGS_LIBAO_DEFAULT_TACT (1.0)
#define AGS_LIBAO_DEFAULT_TACT_JIFFIE (60.0 / AGS_LIBAO_DEFAULT_BPM * AGS_LIBAO_DEFAULT_TACT)
#define AGS_LIBAO_DEFAULT_TACTRATE (1.0 / AGS_LIBAO_DEFAULT_TACT_JIFFIE)

#define AGS_LIBAO_DEFAULT_DELAY (AGS_LIBAO_DEFAULT_JIFFIE / AGS_LIBAO_DEFAULT_TACTRATE)

typedef struct _AgsLibao AgsLibao;
typedef struct _AgsLibaoClass AgsLibaoClass;
typedef struct _AgsLibaoPlay AgsLibaoPlay;
typedef struct _AgsLibaoPlayDomain AgsLibaoPlayDomain;

typedef enum
  {
    AGS_LIBAO_BUFFER0                        = 1,
    AGS_LIBAO_BUFFER1                        = 1 << 1,
    AGS_LIBAO_BUFFER2                        = 1 << 2,
    AGS_LIBAO_BUFFER3                        = 1 << 3,

    AGS_LIBAO_ATTACK_FIRST                   = 1 << 4,

    AGS_LIBAO_PLAY                           = 1 << 5,

    AGS_LIBAO_LIBAO                          = 1 << 6,
    AGS_LIBAO_OSS                            = 1 << 7,
    AGS_LIBAO_ALSA                           = 1 << 8,

    AGS_LIBAO_SHUTDOWN                       = 1 << 9,
    AGS_LIBAO_START_PLAY                     = 1 << 10,

    AGS_LIBAO_NONBLOCKING                    = 1 << 11,

    AGS_LIBAO_TIMING_SET_0                   = 1 << 12,
    AGS_LIBAO_TIMING_SET_1                   = 1 << 13,
  }AgsLibaoFlags;

typedef enum
  {
    AGS_LIBAO_PLAY_DONE              = 1,
    AGS_LIBAO_PLAY_REMOVE            = 1 <<  1,
    AGS_LIBAO_PLAY_CHANNEL           = 1 <<  2,
    AGS_LIBAO_PLAY_PAD               = 1 <<  3,
    AGS_LIBAO_PLAY_AUDIO             = 1 <<  4,
    AGS_LIBAO_PLAY_PLAYBACK          = 1 <<  5,
    AGS_LIBAO_PLAY_SEQUENCER         = 1 <<  6,
    AGS_LIBAO_PLAY_NOTATION          = 1 <<  7,
    AGS_LIBAO_PLAY_SUPER_THREADED    = 1 <<  8,
  }AgsLibaoPlayFlags;

typedef enum{
  AGS_LIBAO_RESOLUTION_8_BIT    = 8,
  AGS_LIBAO_RESOLUTION_16_BIT   = 16,
  AGS_LIBAO_RESOLUTION_24_BIT   = 24,
  AGS_LIBAO_RESOLUTION_32_BIT   = 32,
  AGS_LIBAO_RESOLUTION_64_BIT   = 64,
}AgsLibaoResolutionMode;

#define AGS_LIBAO_ERROR (ags_libao_error_quark())

typedef enum{
  AGS_LIBAO_ERROR_LOCKED_SOUNDCARD,
}AgsLibaoError;

struct _AgsLibao
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
    struct _AgsAO{
      ao_device *device;
      ao_sample_format *format;
      int driver_ao;
    }ao;
  }out;

  GObject *ags_main;
  
  GList *audio;
};

struct _AgsLibaoClass
{
  GObjectClass object;

  void (*play_init)(AgsLibao *libao,
		    GError **error);
  void (*play)(AgsLibao *libao,
	       GError **error);
  void (*stop)(AgsLibao *libao);

  void (*tic)(AgsLibao *libao);

  void (*note_offset_changed)(AgsLibao *libao, guint note_offset);
};

struct _AgsLibaoPlayDomain
{
  GObject *domain;
  
  gboolean playback;
  gboolean sequencer;
  gboolean notation;

  GList *libao_play;
};

struct _AgsLibaoPlay
{
  guint flags;

  AgsIteratorThread **iterator_thread;

  GObject *source;
  guint audio_channel;

  AgsRecallID *recall_id[3];
};

GType ags_libao_get_type();

GQuark ags_libao_error_quark();

AgsLibaoPlayDomain* ags_libao_play_domain_alloc();
void ags_libao_play_domain_free(AgsLibaoPlayDomain *libao_play_domain);

AgsLibaoPlay* ags_libao_play_alloc();
void ags_libao_play_free(AgsLibaoPlay *libao_play);

void ags_libao_list_cards(GList **card_id, GList **card_name);
void ags_libao_pcm_info(char *card_id,
			guint *channels_min, guint *channels_max,
			guint *rate_min, guint *rate_max,
			guint *buffer_size_min, guint *buffer_size_max,
			GError **error);
void ags_libao_tic(AgsLibao *libao);

void ags_libao_note_offset_changed(AgsLibao *libao, guint note_offset);

AgsLibao* ags_libao_new(GObject *ags_main);

#endif /*__AGS_LIBAO_H__*/

