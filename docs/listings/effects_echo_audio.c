AgsEchoAudio *echo_audio;

echo_audio = (AgsEchoAudio *) g_object_new(AGS_TYPE_ECHO_AUDIO,
					   "soundcard\0", soundcard,
                                           "audio\0", audio,
                                           "recall-container\0", echo_container,
                                           NULL);

AGS_RECALL(echo_audio)->flags = AGS_RECALL_TEMPLATE;
