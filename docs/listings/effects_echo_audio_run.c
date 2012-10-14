echo_audio_run = (AgsEchoAudioRun *) g_object_new(AGS_TYPE_ECHO_AUDIO_RUN,
						  "audio\0", audio,
						  "container\0", echo_container,
						  "recall_audio\0", echo_audio,
						  NULL);

AGS_RECALL(echo_audio_run)->flags = AGS_RECALL_TEMPLATE;
