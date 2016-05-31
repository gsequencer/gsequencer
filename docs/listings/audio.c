#include <ags/object/ags_application_context.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_notation.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>

AgsApplicationContext *application_context;
GList *soundcard;
AgsAudio *audio;
AgsNotation *notation;

guint audio_channels;
guint i;

/* get application context and soundcard */
application_context = ags_application_context_get_instance();
soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

/* creat audio and resize channels */
audio_channels = 2;

audio = ags_audio_new(soundcard->data);
ags_audio_set_audio_channels(audio,
                             audio_channels);
ags_audio_set_pads(audio,
                   AGS_TYPE_OUTPUT,
                   1);
ags_audio_set_pads(audio,
                   AGS_TYPE_INPUT,
                   1);

/* add notation */
for(i = 0; i < audio_channels; i++){
  notation = ags_notation_new(audio,
                              i);
  ags_audio_add_notation(audio,
                         notation);
}
