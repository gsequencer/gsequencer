#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <pthread.h>

AgsAudio* setup_master(AgsApplicationContext *application_context);
AgsAudio* setup_slave(AgsApplicationContext *application_context);

AgsAudio*
setup_master(AgsApplicationContext *application_context)
{
  AgsAudio *audio;
  AgsChannel *channel;
  
  GObject *soundcard;

  GList *list;
  GList *recall;
  
  guint n_audio_channels, n_output_pads, n_input_pads;
  
  /* get soundcard */
  list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  soundcard = list->data;

  /* create master playback */
  audio = ags_audio_new(soundcard);

  n_audio_channels = 2;

  n_output_pads = 1;
  n_input_pads = 1;
  
  ags_audio_set_audio_channels(audio,
                               n_audio_channels);
  
  ags_audio_set_pads(audio,
                     AGS_TYPE_OUTPUT,
                     n_output_pads);
  ags_audio_set_pads(audio,
                     AGS_TYPE_INPUT,
                     n_input_pads);

  /* add ags-play-master recall */
  ags_recall_factory_create(audio,
                            NULL, NULL,
                            "ags-play-master",
                            0, n_audio_channels,
                            0, n_output_pads,
                            (AGS_RECALL_FACTORY_INPUT,
                             AGS_RECALL_FACTORY_PLAY |
                             AGS_RECALL_FACTORY_ADD),
                            0);
  
  /* set audio channel on play channel */
  channel = audio->output;
  
  while(channel != NULL){
    recall = channel->play;

    while((recall = ags_recall_template_find_type(recall,
                                                  AGS_TYPE_PLAY_CHANNEL)) != NULL){
      GValue audio_channel_value = {0,};

      play_channel = AGS_PLAY_CHANNEL(recall->data);

      g_value_init(&audio_channel_value, G_TYPE_UINT64);
      g_value_set_uint64(&audio_channel_value,
                         channel->audio_channel);
      ags_port_safe_write(play_channel->audio_channel,
                          &audio_channel_value);

      recall = recall->next;
    }

    channel = channel->next;
  }

  return(audio);
}

AgsAudio*
setup_slave(AgsApplicationContext *application_context)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsAudioSignal *audio_signal;
  AgsPattern *pattern;

  AgsDelayAudioRun *play_delay_audio_run;
  AgsCountBeatsAudioRun *play_count_beats_audio_run;

  GObject *soundcard;

  GList *list;
  GList *recall;
  
  guint n_audio_channels, n_output_pads, n_input_pads;
  gdouble volume;
  guint current_phase, prev_phase;
  guint i, j, k;
  
  GValue value;
  
  /* get soundcard */
  list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  soundcard = list->data;

  /* create master playback */
  audio = ags_audio_new(soundcard);
  ags_audio_set_flags(audio,
		      (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		       AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_audio_set_ability_flags(audio, (AGS_SOUND_ABILITY_SEQUENCER));
  ags_audio_set_behaviour_flags(audio, (AGS_SOUND_BEHAVIOUR_PATTERN_MODE |
					AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING |
					AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT));

  n_audio_channels = 2;

  n_output_pads = 1;
  n_input_pads = 1;
  
  ags_audio_set_audio_channels(audio,
                               n_audio_channels);
  
  ags_audio_set_pads(audio,
                     AGS_TYPE_OUTPUT,
                     n_output_pads);
  ags_audio_set_pads(audio,
                     AGS_TYPE_INPUT,
                     n_input_pads);

  /* add pattern and generate sound */
  channel = audio->output;

  while(channel != NULL){
    ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_SEQUENCER));

    channel = channel->next;
  }
  
  /* add pattern and generate sound */
  channel = audio->input;
  
  for(i = 0; i < n_input_pads; i++){
    for(j = 0; j < n_audio_channels; j++){
      /* pattern */
      pattern = ags_pattern_new();
      ags_pattern_set_dim(pattern,
                          1,
                          1,
                          16);
      ags_channel_add_pattern(channel,
                              pattern);

      for(k = 0; k < 16;){
        ags_pattern_toggle_bit(pattern,
                               0,
                               0,
                               k);
        
        k += 4;
      }

      /* sound */
      audio_signal = ags_audio_signal_new();
      ags_audio_signal_stream_resize(audio_signal,
                                     5);

      stream = audio_signal->stream;

      current_phase = 0;
      volume = 1.0;

      k = 0;
      
      while(stream != NULL){
        ags_synth_sin(soundcard, (signed short *) stream->data,
                      0, 440.0, current_phase, audio_signal->buffer_size,
                      volume);

        prev_phase = current_phase;
        current_phase = (prev_phase + (audio_signal->buffer_size) + k * audio_signal->buffer_size) % 440.0;

        stream = stream->next;
        k++;
      }
      
      channel = channel->next;
    }
  }
  
  /* add ags-delay recall */
  ags_recall_factory_create(audio,
                            NULL, NULL,
                            "ags-delay",
                            0, 0,
                            0, 0,
                            (AGS_RECALL_FACTORY_OUTPUT |
                             AGS_RECALL_FACTORY_ADD |
                             AGS_RECALL_FACTORY_PLAY),
                            0);

  recall = ags_recall_find_type(audio->play, AGS_TYPE_DELAY_AUDIO_RUN);

  if(recall != NULL){
    play_delay_audio_run = AGS_DELAY_AUDIO_RUN(recall->data);
  }
  
  /* ags-count-beats */
  ags_recall_factory_create(audio,
                            NULL, NULL,
                            "ags-count-beats",
                            0, 0,
                            0, 0,
                            (AGS_RECALL_FACTORY_OUTPUT |
                             AGS_RECALL_FACTORY_ADD |
                             AGS_RECALL_FACTORY_PLAY),
                            0);
  
  recall = ags_recall_find_type(audio->play, AGS_TYPE_COUNT_BEATS_AUDIO_RUN);

  if(recall != NULL){
    play_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(recall->data);

    /* set dependency */  
    g_object_set(G_OBJECT(play_count_beats_audio_run),
                 "delay-audio-run", play_delay_audio_run,
                 NULL);

    /* make it loop */
    g_value_init(&value,
                 G_TYPE_BOOLEAN);
    g_value_set_boolean(&value, gtk_toggle_button_get_active(window->navigation->loop));
    ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(play_count_beats_audio_run)->recall_audio)->notation_loop,
                        &value);
  }

  /* ags-copy-pattern */
  ags_recall_factory_create(audio,
                            NULL, NULL,
                            "ags-copy-pattern",
                            0, n_audio_channels,
                            0, n_input_pads,
                            (AGS_RECALL_FACTORY_INPUT |
                             AGS_RECALL_FACTORY_ADD |
                             AGS_RECALL_FACTORY_RECALL),
                            0);

  recall = ags_recall_find_type(audio->recall, AGS_TYPE_COPY_PATTERN_AUDIO_RUN);

  if(recall != NULL){
    recall_copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(recall->data);

    /* set dependency */
    g_object_set(G_OBJECT(recall_copy_pattern_audio_run),
                 "delay-audio-run", play_delay_audio_run,
                 "count-beats-audio-run", play_count_beats_audio_run,
                 NULL);

  }

  /* set pattern object on port */
  channel = ags_channel_pad_nth(audio->input, 0);
      
  for(i = 0; i < n_input_pads; i++){
    for(j = 0; j < n_audio_channels; j++){
      GList *list;
      
      recall = ags_recall_template_find_type(channel->recall, AGS_TYPE_COPY_PATTERN_CHANNEL);
      copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(recall->data);

      list = channel->pattern;
      pattern = AGS_PATTERN(list->data);

      copy_pattern_channel->pattern->port_value.ags_port_object = (GObject *) pattern;    
      ags_portlet_set_port(AGS_PORTLET(pattern), copy_pattern_channel->pattern);
          
      channel = channel->next;
    }
  }

  return(audio);
}

int
main(int argc, char **argv)
{
  AgsAudio *master, *slave;
  AgsChannel *output, *input;

  AgsStartAudio *start_audio;
  
  AgsThread *main_loop;
  AgsTaskThread *task_thread;
  
  AgsApplicationContext *application_context;
  AgsConfig *config;

  GError *error;

  /* create application context */
  application_context = ags_audio_application_context_new();
  g_object_get(application_context,
	       "task-thread", &task_thread,
	       NULL);
  
  /* set config */
  config = application_context->config;

  ags_config_set_value(config,
                       AGS_CONFIG_THREAD,
                       "model",
                       "super-threaded");
  ags_config_set_value(config,
                       AGS_CONFIG_THREAD,
                       "super-threaded-scope",
                       "channel");

  /* main loop */
  main_loop = application_context->main_loop;

  /* setup audio tree */
  master = setup_master(application_context);
  slave = setup_slave(application_context);

  /* set link */
  input = master->input;
  output = slave->output;

  while(input != NULL &&
        output != NULL){
    error = NULL;
    ags_channel_set_link(input,
                         output,
                         &error);

    if(error != NULL){
      g_message("%s", error->message);
    }
    
    input = input->next;
    output = output->next;
  }

  start_audio = ags_start_audio_new(slave,
				    AGS_SOUND_SCOPE_SEQUENCER);

  /* start threads */
  ags_thread_start(main_loop);

  /* launch task */
  sleep(3);
  ags_task_thread_append_task(task_thread,
			      start_audio);

  /* join main loop */
  pthread_join(*(main_loop->thread),
               NULL);

  return(0);
}
