#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

echo_audio_run = (AgsEchoAudioRun *) g_object_new(AGS_TYPE_ECHO_AUDIO_RUN,
						  "soundcard\0", soundcard,
                                                  "audio\0", audio,
                                                  "recall-audio\0", echo_audio,
                                                  "recall-container\0", echo_container,
                                                  NULL);

AGS_RECALL(echo_audio_run)->flags = AGS_RECALL_TEMPLATE;
