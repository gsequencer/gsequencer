AgsDevout *devout;
AgsAudio *master_audio, *slave_audio;
AgsLinkChannel *linkChannel;
GError *error;

/* some pseudo code */
devout = AGS_WINDOW(gtk_widget_get_toblevel(widget))->devout;

/* create AgsAudio objects */
master_audio = (AgsAudio *) g_object_new(AGS_TYPE_AUDIO,
					 "devout\0", devout,
					 NULL);
slave_audio = (AgsAudio *) g_object_new(AGS_TYPE_AUDIO,
					"devout\0", devout,
					NULL);

/* assign AgsAudioSignal objects to master_audio and slave_audio */
ags_audio_set_flags(master_audio,
		    AGS_AUDIO_OUTPUT_HAS_RECYCLING);
ags_audio_set_flags(slave_audio,
		    (AGS_AUDIO_ASYNC | AGS_AUDIO_OUTPUT_HAS_RECYCLING | AGS_AUDIO_INPUT_HAS_RECYCLING));

/* create AgsChannel objects within master_audio and slave_audio */
ags_audio_set_audio_channels(master_audio, 2);
ags_audio_set_pads(master_audio, AGS_TYPE_OUTPUT, 1);
ags_audio_set_pads(master_audio, AGS_TYPE_INPUT, 1);
      
ags_audio_set_audio_channels(slave_audio, 2);
ags_audio_set_pads(slave_audio, AGS_TYPE_OUTPUT, 1);
ags_audio_set_pads(slave_audio, AGS_TYPE_INPUT, 8);
