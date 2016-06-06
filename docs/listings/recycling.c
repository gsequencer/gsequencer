#include <ags/object/ags_application_context.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>

AgsRecycling *recycling;
AgsAudioSignal *template;

AgsApplicationContext *application_context;

GList *soundcard;

guint stream_length;

application_context = ags_application_context_get_instance();
soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

/* create recycling */
recycling = ags_recycling_new(soundcard->data);

/* create audio signal and add to recycling */
stream_length = 5;

audio_signal = ags_audio_signal_new(soundcard->data,
                                    recycling,
                                    NULL,
                                    stream_length);
audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
ags_recycling_add_audio_signal(recyclig,
                               audio_signal);
