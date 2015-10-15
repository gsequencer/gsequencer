AgsEchoChannelRun *echo_channel_run;

echo_channel_run = (AgsEchoChannelRun *) g_object_new(AGS_TYPE_ECHO_CHANNEL_RUN,
						      "channel", channel
						      "container", echo_container,
						      "recall_audio", echo_audio,
						      "recall_channel", echo_channel,
						      "recall_audio_run", echo_audio_run,
						      NULL);

AGS_RECALL(echo_channel_run)->flags = AGS_RECALL_TEMPLATE;
