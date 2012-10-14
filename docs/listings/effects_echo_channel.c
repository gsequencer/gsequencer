AgsEchoChannel *echo_channel;

echo_channel = (AgsEchoChannel *) g_object_new(AGS_TYPE_ECHO_CHANNEL,
					       "channel\0", channel,
					       "container\0", echo_container,
					       "recall_audio\0", echo_audio,
					       "delay\0", (devout->frequency * (60 / devout->bpm) / 4),
					       "repeat\0", 3,
					       "fade\0", -0.25,
					       "dry\0", 0.5,
					       NULL);

AGS_RECALL(echo_channel)->flags = AGS_RECALL_TEMPLATE;
