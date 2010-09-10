#ifndef __AGS_AUDIO_SIGNAL_H__
#define __AGS_AUDIO_SIGNAL_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <glib/glist.h>

#define AGS_TYPE_AUDIO_SIGNAL                (ags_audio_signal_get_type())
#define AGS_AUDIO_SIGNAL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_SIGNAL, AgsAudioSignal))
#define AGS_AUDIO_SIGNAL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO_SIGNAL, AgsAudioSignalClass))
#define AGS_IS_AUDIO_SIGNAL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_SIGNAL))
#define AGS_IS_AUDIO_SIGNAL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_SIGNAL))
#define AGS_AUDIO_SIGNAL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_AUDIO_SIGNAL, AgsAudioSignalClass))

typedef struct _AgsAudioSignal AgsAudioSignal;
typedef struct _AgsAudioSignalClass AgsAudioSignalClass;
typedef struct _AgsAttack AgsAttack;

typedef enum{
  AGS_AUDIO_SIGNAL_TEMPLATE             = 1,
  AGS_AUDIO_SIGNAL_PLAY_DONE            = 1 << 1,
}AgsAudioSignalFlags;

struct _AgsAudioSignal
{
  GObject object;

  guint flags;

  GObject *devout;

  GObject *recycling;
  GObject *recall_id; // AGS_TYPE_RECALL_ID to identify the AgsAudioSignal

  guint length;
  guint last_frame; // the last frame at stream_end

  GList *stream_beginning;
  GList *stream_current;
  GList *stream_end;
};

struct _AgsAudioSignalClass
{
  GObjectClass object;
};

struct _AgsAttack
{
  guint first_start;
  guint first_length;
  guint second_start;
  guint second_length;
};

short* ags_stream_alloc(guint buffer_size);

AgsAttack* ags_attack_alloc(guint first_start, guint first_length,
			    guint second_start, guint second_length);
AgsAttack* ags_attack_duplicate(AgsAttack *attack);
AgsAttack* ags_attack_get_from_devout(GObject *devout);

void ags_audio_signal_connect(AgsAudioSignal *audio_signal);

void ags_audio_signal_add_stream(AgsAudioSignal *audio_signal);
void ags_audio_signal_stream_resize(AgsAudioSignal *audio_signal, guint length);

void ags_audio_signal_copy_buffer_to_buffer(short *destination, guint dchannels,
					    short *source, guint schannels, guint size);

void ags_audio_signal_duplicate_stream(AgsAudioSignal *audio_signal,
				       AgsAudioSignal *template);

AgsAudioSignal* ags_audio_signal_get_template(GList *audio_signal);
GList* ags_audio_signal_get_stream_current(GList *audio_signal,
					   GObject *recall_id);
GList* ags_audio_signal_get_by_recall_id(GList *audio_signal,
					 GObject *recall_id);

AgsAudioSignal* ags_audio_signal_new(GObject *recycling,
				     GObject *owner);

#endif /*__AGS_AUDIO_SIGNAL_H__*/
