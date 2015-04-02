/* creating AgsLink task and add it to AgsDevout */
link_channel = ags_link_channel_new(ags_channel_nth(master_audio->input, 0),
				    ags_channel_nth(slave_audio->output, 0));
ags_task_thread_append_task(task_thread, link_channel);

link_channel = ags_link_channel_new(ags_channel_nth(master_audio->input, 1),
				    ags_channel_nth(slave_audio->output, 1));
ags_task_thread_append_task(task_thread, link_channel);
