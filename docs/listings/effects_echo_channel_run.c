AgsEchoChannelRun *echo_channel_run;

echo_channel_run = (AgsEchoChannelRun *) g_object_new(AGS_TYPE_ECHO_CHANNEL_RUN,
						      "soundcard\0", soundcard,
                                                      "channel\0", channel
                                                      "recall-channel\0", echo_channel,
                                                      "recall-container\0", echo_container,
                                                      NULL);

AGS_RECALL(echo_channel_run)->flags = AGS_RECALL_TEMPLATE;
