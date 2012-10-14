AgsEchoChannelRun *echo_channel_run;

echo_channel_run = (AgsEchoChannelRun *) g_object_new(AGS_TYPE_ECHO_CHANNEL_RUN,
						      "channel\0", channel
						      "container\0", echo_container,
						      "recall_audio\0", echo_audio,
						      "recall_channel\0", echo_channel,
						      "recall_audio_run\0", echo_audio_run,
						      NULL);

AGS_RECALL(echo_channel_run)->flags = AGS_RECALL_TEMPLATE;
