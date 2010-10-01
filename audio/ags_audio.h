#ifndef __AGS_AUDIO_H__
#define __AGS_AUDIO_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_channel.h"

#define AGS_TYPE_AUDIO                (ags_audio_get_type ())
#define AGS_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO, AgsAudio))
#define AGS_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO, AgsAudioClass))
#define AGS_IS_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_AUDIO))
#define AGS_IS_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_AUDIO))
#define AGS_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_AUDIO, AgsAudioClass))

typedef struct _AgsAudio AgsAudio;
typedef struct _AgsAudioClass AgsAudioClass;

typedef enum{
  AGS_AUDIO_OUTPUT_HAS_RECYCLING   = 1,
  AGS_AUDIO_INPUT_HAS_RECYCLING    = 1 << 1,
  AGS_AUDIO_INPUT_TAKES_FILE       = 1 << 2,
  AGS_AUDIO_HAS_NOTATION           = 1 << 3,
  AGS_AUDIO_SYNC                   = 1 << 4, // can be combined with below
  AGS_AUDIO_ASYNC                  = 1 << 5,
  //  AGS_AUDIO_DEVOUT_PLAY_REMOVE     = 1 << 6,
  //  AGS_AUDIO_DEVOUT_PLAY_CANCEL     = 1 << 7,
  AGS_AUDIO_RUNNING                = 1 << 6,
  AGS_AUDIO_PLAYING                = 1 << 7,
  /*  AGS_AUDIO_RECALL_PRE_INIT_DONE   = 1 << 10,
  AGS_AUDIO_RECALL_INTER_INIT_DONE = 1 << 11,
  AGS_AUDIO_RECALL_POST_INIT_DONE  = 1 << 12,
  AGS_AUDIO_PLAY_PRE_INIT_DONE     = 1 << 13,
  AGS_AUDIO_PLAY_INTER_INIT_DONE   = 1 << 14,
  AGS_AUDIO_PLAY_POST_INIT_DONE    = 1 << 15, */
}AgsAudioFlags;

struct _AgsAudio
{
  GObject object;

  guint flags;

  GObject *devout;

  guint sequence_length;
  guint audio_channels;
  guint frequence;

  guint output_pads;
  guint output_lines;

  guint input_pads;
  guint input_lines;

  AgsChannel *output;
  AgsChannel *input;

  GList *notation;

  AgsDevoutPlay *devout_play;

  GList *recall_id;
  GList *recall; // eg. AGS_DELAY in AgsRecallClass->recall_inter
  GList *play;
  GList *recall_shared;

  GtkWidget *machine;
};

struct _AgsAudioClass
{
  GObjectClass object;

  void (*set_audio_channels)(AgsAudio *audio,
			     guint audio_channels, guint audio_channels_old);
  void (*set_pads)(AgsAudio *audio,
		   GType type,
		   guint pads, guint pads_old);
};

void ags_audio_set_audio_channels(AgsAudio *audio, guint audio_channels);
void ags_audio_set_pads(AgsAudio *audio, GType type, guint pads);

void ags_audio_set_devout(AgsAudio *audio, GObject *devout);
void ags_audio_set_sequence_length(AgsAudio *audio, guint sequence_length);

void ags_audio_recall_change_state(AgsAudio *audio, gboolean enable);
void ags_audio_play(AgsAudio *audio, guint group_id, gint stage, gboolean do_recall);
guint ags_audio_recursive_play_init(AgsAudio *audio);

AgsAudio* ags_audio_new();

#endif /*__AGS_AUDIO_H__*/
