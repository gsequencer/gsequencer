echo_audio_run = (AgsEchoAudioRun *) g_object_new(AGS_TYPE_ECHO_AUDIO_RUN,
						  "audio", audio,
						  "container", echo_container,
						  "recall_audio", echo_audio,
						  NULL);

AGS_RECALL(echo_audio_run)->flags = AGS_RECALL_TEMPLATE;
