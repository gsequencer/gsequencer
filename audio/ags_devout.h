#ifndef __AGS_DEVOUT_H__
#define __AGS_DEVOUT_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <unistd.h>
#include <sys/types.h>

#define __USE_UNIX98

#include <pthread.h>

#include <ao/ao.h>

#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

//#include "ags_garbage_collector.h"
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_task.h>

#define AGS_TYPE_DEVOUT                (ags_devout_get_type())
#define AGS_DEVOUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DEVOUT, AgsDevout))
#define AGS_DEVOUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_DEVOUT, AgsDevout))
#define AGS_IS_DEVOUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_DEVOUT))
#define AGS_IS_DEVOUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_DEVOUT))
#define AGS_DEVOUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_DEVOUT, AgsDevoutClass))

typedef struct _AgsDevout AgsDevout;
typedef struct _AgsDevoutClass AgsDevoutClass;
typedef struct _AgsDevoutPlay AgsDevoutPlay;

typedef enum
{
  AGS_DEVOUT_BUFFER0                = 1,
  AGS_DEVOUT_BUFFER1                = 1 << 1,
  AGS_DEVOUT_BUFFER2                = 1 << 2,
  AGS_DEVOUT_BUFFER3                = 1 << 3,
  AGS_DEVOUT_ATTACK_FIRST           = 1 << 4,
  AGS_DEVOUT_PLAY                   = 1 << 5,
  AGS_DEVOUT_WAIT_DEVICE            = 1 << 6,
  AGS_DEVOUT_WAIT_RECALL            = 1 << 7,
  AGS_DEVOUT_WAIT_TASK              = 1 << 8,
  AGS_DEVOUT_WAIT_APPEND_TASK       = 1 << 9,
  AGS_DEVOUT_LIBAO                  = 1 << 10,
  AGS_DEVOUT_OSS                    = 1 << 11,
  AGS_DEVOUT_ALSA                   = 1 << 12,
  AGS_DEVOUT_PLAY_RECALL            = 1 << 13,
  AGS_DEVOUT_PLAYING_RECALL         = 1 << 14,
  AGS_DEVOUT_PLAY_CHANNEL           = 1 << 15,
  AGS_DEVOUT_PLAYING_CHANNEL        = 1 << 16,
  AGS_DEVOUT_PLAY_AUDIO             = 1 << 17,
  AGS_DEVOUT_PLAYING_AUDIO          = 1 << 18,
  AGS_DEVOUT_PLAY_NOTE              = 1 << 19,
}AgsDevoutFlags;

typedef enum
{
  AGS_DEVOUT_PLAY_DONE              = 1,
  AGS_DEVOUT_PLAY_REMOVE            = 1 << 1,
  AGS_DEVOUT_PLAY_PAD               = 1 << 2,
}AgsDevoutPlayFlags;

struct _AgsDevout
{
  GObject object;

  guint32 flags;

  guint dsp_channels;
  guint pcm_channels;
  guint bits;
  guint buffer_size;
  guint frequency;

  //  guint64 offset; // for timed tasks in AgsChannel

  //  guint note_delay;
  //  guint note_counter;
  //  guint64 note_offset; // corresponds to AgsNote->x

  short** buffer;

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
      int rc;
      snd_pcm_t *handle;
      snd_pcm_hw_params_t *params;
    }alsa;
  }out;

  pthread_t play_thread;
  pthread_attr_t play_thread_attr;
  pthread_mutex_t play_mutex;
  pthread_mutexattr_t play_mutex_attr;
  pthread_cond_t play_cond;

  pthread_t play_functions_thread;
  pthread_attr_t play_functions_thread_attr;
  pthread_mutex_t play_functions_mutex;
  pthread_mutexattr_t play_functions_mutex_attr;
  pthread_cond_t play_functions_cond;

  pthread_mutex_t task_mutex;
  pthread_cond_t task_cond;

  pthread_mutex_t append_task_mutex;
  pthread_cond_t append_task_cond;
  
  GList *task;
  GList *tactable;

  guint play_recall_ref;
  GList *play_recall; // play AgsRecall

  guint play_channel_ref;
  GList *play_channel; // play AgsChannel

  guint play_audio_ref;
  GList *play_audio; // play AgsAudio
};

struct _AgsDevoutClass
{
  GObjectClass object;

  void (*run)(AgsDevout *devout);
  void (*stop)(AgsDevout *devout);
};

struct _AgsDevoutPlay
{
  guint flags;

  GObject *source;
  guint audio_channel;

  guint group_id; // if source is an AgsChannel or an AgsAudio
  AgsRecallID *recall_id; // if source is an AgsRecall
};

GType ags_devout_get_type();

AgsDevoutPlay* ags_devout_play_alloc();

void ags_devout_append_task(AgsDevout *devout, AgsTask *task);

void ags_devout_run(AgsDevout *devout);
void ags_devout_stop(AgsDevout *devout);

AgsDevout* ags_devout_new();

#endif /*__AGS_DEVOUT_H__*/
