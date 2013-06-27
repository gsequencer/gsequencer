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

#define _GNU_SOURCE
#include <pthread.h>

#include <glib.h>
#include <glib-object.h>

#include <sys/types.h>

#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

#include <ao/ao.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_devout_thread.h>

#include <ags/audio/ags_recall_id.h>

#define AGS_TYPE_DEVOUT                (ags_devout_get_type())
#define AGS_DEVOUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DEVOUT, AgsDevout))
#define AGS_DEVOUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_DEVOUT, AgsDevout))
#define AGS_IS_DEVOUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_DEVOUT))
#define AGS_IS_DEVOUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_DEVOUT))
#define AGS_DEVOUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_DEVOUT, AgsDevoutClass))

#define AGS_DEVOUT_PLAY(ptr)           ((AgsDevoutPlay *)(ptr))

#define AGS_DEVOUT_DEFAULT_SAMPLERATE (44100)
#define AGS_DEVOUT_DEFAULT_BUFFER_SIZE (128)

typedef struct _AgsDevout AgsDevout;
typedef struct _AgsDevoutClass AgsDevoutClass;
typedef struct _AgsDevoutPlay AgsDevoutPlay;

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
}AgsDevoutFlags;

typedef enum
{
  AGS_DEVOUT_PLAY_DONE              = 1,
  AGS_DEVOUT_PLAY_REMOVE            = 1 <<  1,
  AGS_DEVOUT_PLAY_PAD               = 1 <<  2,
  AGS_DEVOUT_PLAY_PLAYBACK          = 1 <<  3,
  AGS_DEVOUT_PLAY_SEQUENCER         = 1 <<  4,
  AGS_DEVOUT_PLAY_NOTATION          = 1 <<  5,
}AgsDevoutPlayFlags;

#define AGS_DEVOUT_ERROR (ags_devout_error_quark())

typedef enum{
  AGS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
  AGS_DEVOUT_ERROR_EMPTY_GATE,
}AgsDevoutError;

struct _AgsDevout
{
  GObject object;

  guint64 flags;

  guint dsp_channels;
  guint pcm_channels;
  guint bits;
  guint buffer_size;
  guint frequency; // sample_rate

  //  guint64 offset; // for timed tasks in AgsChannel

  //  guint note_delay;
  //  guint note_counter;
  //  guint note_offset; // corresponds to AgsNote->x

  signed short** buffer;

  double bpm; // beats per minute
  guint delay; // delay between tic change
  guint delay_counter; // next time attack changeing when delay_counter == delay
  AgsAttack *attack; // where currently tic resides in the stream's offset, measured in 1/64 of bpm

  union{
    struct _AgsAO{
      ao_device *device;
      ao_sample_format format;
      int driver_ao;
    }ao;
    struct _AgsOss{
      int device_fd;
      char *device;
    }oss;
    struct _AgsAlsa{
      char *device;
      int rc;
      snd_pcm_t *handle;
      snd_pcm_hw_params_t *params;
    }alsa;
  }out;
  
  AgsDevoutFifoIO *push;
  pthread_mutex_t push_inject_mutex;
  AgsDevoutFifoIO *pop;
  pthread_mutex_t pop_inject_mutex;

  GSList *gate;
  guint refresh_gate;
  pthread_mutex_t fifo_mutex;
  pthread_mutex_t gate_mutex;

  unsigned int wait_sync;

  GList *audio;

  AgsAudioLoop *audio_loop;
  AgsTaskThread *task_thread;
  AgsDevoutThread *devout_thread;
};

struct _AgsDevoutClass
{
  GObjectClass object;

  void (*run)(AgsDevout *devout);
  void (*stop)(AgsDevout *devout);
  void (*tic)(AgsDevout *devout);

  void (*note_offset_changed)(AgsDevout *devout, guint note_offset);
};

struct _AgsDevoutPlay
{
  guint flags;

  GObject *source;
  guint audio_channel;

  AgsGroupId group_id; // if source is an AgsChannel or an AgsAudio
  AgsRecallID *recall_id; // if source is an AgsRecall
};

struct _AgsDevoutFifoIO{
  AgsDevout *devout;

  pthread_t thread;
  pthread_cond_t cond;
  pthread_mutex_t mutex;
  gboolean sleep;

  AgsDevoutGateControl *push;
  AgsDevoutGateControl *pop;
};

GType ags_devout_get_type();

GQuark ags_devout_error_quark();

AgsDevoutPlay* ags_devout_play_alloc();
AgsDevoutFifoIO* ags_devout_fifo_io_alloc();
AgsDevoutGate* ags_devout_gate_alloc();
AgsDevoutGateControl* ags_devout_gate_control_alloc();

GList *ags_devout_list_cards();
void ags_devout_pcm_info(char *card_id,
			 guint *channels_min, guint *channels_max,
			 guint *rate_min, guint *rate_max,
			 guint *buffer_size_min, guint *buffer_size_max,
			 GError **error);

void ags_devout_run(AgsDevout *devout);
void ags_devout_stop(AgsDevout *devout);
void ags_devout_tic(AgsDevout *devout);

void ags_devout_note_offset_changed(AgsDevout *devout, guint note_offset);

void ags_devout_start_default_threads(AgsDevout *devout);

AgsDevout* ags_devout_new();

#endif /*__AGS_DEVOUT_H__*/
