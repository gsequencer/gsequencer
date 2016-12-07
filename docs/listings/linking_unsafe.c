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

/* link master_audio's input with slave_audio's output */
ags_channel_set_link(ags_channel_nth(master_audio->input, 0),
                     ags_channel_nth(slave_audio->output, 0),
                     &error);
  

ags_channel_set_link(ags_channel_nth(master_audio->input, 1),
                     ags_channel_nth(slave_audio->output, 1),
                     &error);
