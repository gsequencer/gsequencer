#ifndef __AGS_RECYCLING_H__
#define __AGS_RECYCLING_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <glib/glist.h>

#include "ags_audio_signal.h"

#define AGS_TYPE_RECYCLING                (ags_recycling_get_type())
#define AGS_RECYCLING(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECYCLING, AgsRecycling))
#define AGS_RECYCLING_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECYCLING, AgsRecyclingClass))
#define AGS_IS_RECYCLING(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECYCLING))
#define AGS_IS_RECYCLING_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECYCLING))
#define AGS_RECYCLING_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECYCLING, AgsRecyclingClass))

typedef struct _AgsRecycling AgsRecycling;
typedef struct _AgsRecyclingClass AgsRecyclingClass;

typedef enum{
  AGS_RECYCLING_MUTED           =  1,
}AgsRecyclingFlags;

struct _AgsRecycling
{
  GObject object;

  GObject *channel;

  GObject *parent; // AGS_TYPE_CHANNEL

  AgsRecycling *next;
  AgsRecycling *prev;

  GList *audio_signal;
};

struct _AgsRecyclingClass
{
  GObjectClass object;

  void (*add_audio_signal)(AgsRecycling *recycling,
			   AgsAudioSignal *audio_signal);
  void (*add_audio_signal_with_frame_count)(AgsRecycling *recycling,
					    AgsAudioSignal *audio_signal,
					    guint frame_count);
  void (*remove_audio_signal)(AgsRecycling *recycling,
			      AgsAudioSignal *audio_signal);
};

void ags_recycling_connect(AgsRecycling *recycling);

void ags_recycling_add_audio_signal(AgsRecycling *recycling,
				    AgsAudioSignal *audio_signal);
void ags_recycling_add_audio_signal_with_frame_count(AgsRecycling *recycling,
						     AgsAudioSignal *audio_signal,
						     guint frame_count);

void ags_recycling_remove_audio_signal(AgsRecycling *recycling,
				       AgsAudioSignal *audio_signal);

AgsRecycling* ags_recycling_new(GObject *devout);

#endif /*__AGS_RECYCLING_H__*/
