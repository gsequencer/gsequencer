AgsEchoChannel *echo_channel;

echo_channel = (AgsEchoChannel *) g_object_new(AGS_TYPE_ECHO_CHANNEL,
					       "channel", channel,
					       "container", echo_container,
					       "recall_audio", echo_audio,
					       "delay", (devout->frequency * (60 / devout->bpm) / 4),
					       "repeat", 3,
					       "fade", -0.25,
					       "dry", 0.5,
					       NULL);

AGS_RECALL(echo_channel)->flags = AGS_RECALL_TEMPLATE;
