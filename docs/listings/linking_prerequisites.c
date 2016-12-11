#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

AgsAudio *master_audio, *slave_audio;
AgsLinkChannel *linkChannel;

GObject *soundcard;

GError *error;

/* instantiate AgsDevout */
soundcard = ags_devout_new(NULL);

/* create AgsAudio objects */
master_audio = (AgsAudio *) g_object_new(AGS_TYPE_AUDIO,
                                         "soundcard\0", soundcard,
                                         NULL);
slave_audio = (AgsAudio *) g_object_new(AGS_TYPE_AUDIO,
                                        "soundcard\0", soundcard,
                                        NULL);

/* assign AgsAudioSignal objects to master_audio and slave_audio */
ags_audio_set_flags(master_audio,
                    AGS_AUDIO_OUTPUT_HAS_RECYCLING);
ags_audio_set_flags(slave_audio,
                    (AGS_AUDIO_ASYNC | AGS_AUDIO_OUTPUT_HAS_RECYCLING | AGS_AUDIO_INPUT_HAS_RECYCLING));

/* create AgsChannel objects within master_audio and slave_audio */
ags_audio_set_audio_channels(master_audio, 2);
ags_audio_set_pads(master_audio, AGS_TYPE_OUTPUT, 1);
ags_audio_set_pads(master_audio, AGS_TYPE_INPUT, 1);
      
ags_audio_set_audio_channels(slave_audio, 2);
ags_audio_set_pads(slave_audio, AGS_TYPE_OUTPUT, 1);
ags_audio_set_pads(slave_audio, AGS_TYPE_INPUT, 8);
