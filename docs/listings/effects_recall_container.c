#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

AgsAudio *audio;
AgsChannel *channel;
AgsRecallContainer *echo_container;

GObject *soundcard;

soundcard = ags_devout_new(NULL);
audio = ags_audio_new(devout);

/* create the container */
recall_container = (AgsRecallContainer *) g_object_new(AGS_TYPE_RECALL_CONTAINER,
                                                       NULL);
ags_audio_add_recall_container(audio,
			       (GObject *) recall_container);
