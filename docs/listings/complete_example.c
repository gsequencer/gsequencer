#include <ags/object/ags_config.h>
#include <ags/object/ags_application_context.h>

#include <ags/thread/ags_thread-posix.h>

#include <ags/audio/ags_audio_application_context.h>
#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall_audio_run.h>

#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>

#include <ags/audio/recall/ags_play_channel.h>

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
			    "ags-play-master\0",
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
  audio->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		   AGS_AUDIO_INPUT_HAS_RECYCLING);
  
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
			    "ags-delay\0",
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
			    "ags-count-beats\0",
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
		 "delay-audio-run\0", play_delay_audio_run,
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
			    "ags-copy-pattern\0",
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
		 "delay-audio-run\0", play_delay_audio_run,
		 "count-beats-audio-run\0", play_count_beats_audio_run,
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
  
  AgsThread *main_loop;
  
  AgsApplicationContext *application_context;
  AgsConfig *config;

  GList *playback;
  GList *start_queue;
  
  GError *error;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *main_loop_mutex;
  pthread_mutex_t *audio_thread_mutex;

  /* create application context */
  application_context = ags_audio_application_context_new();

  /* set config */
  config = application_context->config;

  ags_config_set_value(config,
		       AGS_CONFIG_THREAD,
		       "model\0",
		       "super-threaded\0");
  ags_config_set_value(config,
		       AGS_CONFIG_THREAD,
		       "super-threaded-scope\0",
		       "channel\0");

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* main loop */
  main_loop = application_context->main_loop;

  pthread_mutex_lock(application_mutex);

  main_loop_mutex = ags_mutex_manager_lookup(mutex_manager,
					      main_loop);
  
  pthread_mutex_unlock(application_mutex);

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
      g_message("%s\0", error->message);
    }
    
    input = input->next;
    output = output->next;
  }

  /* initialize tree */
  g_atomic_int_or(&(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->flags),
		  AGS_PLAYBACK_DOMAIN_SEQUENCER);

  playback = AGS_PLAYBACK_DOMAIN(audio->playback_domain)->playback;

  list = ags_audio_recursive_play_init(audio,
				       FALSE, TRUE, FALSE);

  while(playback != NULL){
    g_atomic_int_or(&(AGS_PLAYBACK(playback->data)->flags),
		    AGS_PLAYBACK_SEQUENCER);
      
    playback = playback->next;
  }

  /* parent mutex */
  pthread_mutex_lock(application_mutex);

  audio_thread_mutex = ags_mutex_manager_lookup(mutex_manager,
						AGS_PLAYBACK_DOMAIN(slave->playback_domain)->audio_thread[1]);
  
  pthread_mutex_unlock(application_mutex);
  
  /* add audio and channel threads */
  output = slave->output;

  start_queue = NULL;
	
  while(output != NULL){
    g_atomic_int_or(&(AGS_CHANNEL_THREAD(AGS_PLAYBACK(output->playback)->channel_thread[1])->flags),
		    (AGS_CHANNEL_THREAD_WAIT |
		     AGS_CHANNEL_THREAD_DONE |
		     AGS_CHANNEL_THREAD_WAIT_SYNC |
		     AGS_CHANNEL_THREAD_DONE_SYNC));
    start_queue = g_list_prepend(start_queue,
				 AGS_PLAYBACK(output->playback)->channel_thread[1]);

    if(AGS_PLAYBACK(output->playback)->channel_thread[1]->parent == NULL){
      ags_thread_add_child_extended(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1],
				    AGS_PLAYBACK(output->playback)->channel_thread[1],
				    TRUE, TRUE);
      ags_connectable_connect(AGS_CONNECTABLE(AGS_PLAYBACK(output->playback)->channel_thread[1]));
    }
	  
    output = output->next;
  }

  /* start queue */
  start_queue = g_list_reverse(start_queue);

  pthread_mutex_lock(audio_thread_mutex);

  if(start_queue != NULL){
    if(g_atomic_pointer_get(&(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]->start_queue)) != NULL){
      g_atomic_pointer_set(&(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]->start_queue),
			   g_list_concat(start_queue,
					 g_atomic_pointer_get(&(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]->start_queue))));
    }else{
      g_atomic_pointer_set(&(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]->start_queue),
			   start_queue);
    }
  }

  pthread_mutex_unlock(audio_thread_mutex);

  /* super threaded setup - audio */
  start_queue = NULL;

  if(append_audio->do_sequencer){
    g_atomic_int_or(&(AGS_AUDIO_THREAD(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1])->flags),
		    (AGS_AUDIO_THREAD_WAIT |
		     AGS_AUDIO_THREAD_DONE |
		     AGS_AUDIO_THREAD_WAIT_SYNC |
		     AGS_AUDIO_THREAD_DONE_SYNC));
    start_queue = g_list_prepend(start_queue,
				 AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]);

    if(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]->parent == NULL){
      ags_thread_add_child_extended(main_loop,
				    AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1],
				    TRUE, TRUE);
      ags_connectable_connect(AGS_CONNECTABLE(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]));
    }
  }

  /* start queue */
  start_queue = g_list_reverse(start_queue);

  pthread_mutex_lock(main_loop_mutex);

  if(start_queue != NULL){
    if(g_atomic_pointer_get(&(AGS_THREAD(main_loop)->start_queue)) != NULL){
      g_atomic_pointer_set(&(AGS_THREAD(main_loop)->start_queue),
			   g_list_concat(start_queue,
					 g_atomic_pointer_get(&(AGS_THREAD(main_loop)->start_queue))));
    }else{
      g_atomic_pointer_set(&(AGS_THREAD(main_loop)->start_queue),
			   start_queue);
    }
  }

  pthread_mutex_unlock(main_loop_mutex);

  /* start threads and join main loop */
  ags_thread_start(main_loop);
  
  pthread_join(*(main_loop->thread),
	       NULL);

  return(0);
}
