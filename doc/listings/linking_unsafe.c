/* link master_audio's input with slave_audio's output */
ags_channel_set_link(ags_channel_nth(master_audio->input, 0),
		     ags_channel_nth(slave_audio->output, 0),
		     &error);
  

ags_channel_set_link(ags_channel_nth(master_audio->input, 1),
		     ags_channel_nth(slave_audio->output, 1),
		     &error);
