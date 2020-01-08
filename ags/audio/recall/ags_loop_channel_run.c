/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/audio/recall/ags_loop_channel_run.h>

#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_delay_audio.h>

#include <math.h>

#include <ags/i18n.h>

void ags_loop_channel_run_class_init(AgsLoopChannelRunClass *loop_channel_run);
void ags_loop_channel_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_loop_channel_run_init(AgsLoopChannelRun *loop_channel_run);
void ags_loop_channel_run_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_loop_channel_run_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_loop_channel_run_dispose(GObject *gobject);
void ags_loop_channel_run_finalize(GObject *gobject);

void ags_loop_channel_run_connect(AgsConnectable *connectable);
void ags_loop_channel_run_disconnect(AgsConnectable *connectable);
void ags_loop_channel_run_connect_connection(AgsConnectable *connectable,
					     GObject *connection);
void ags_loop_channel_run_disconnect_connection(AgsConnectable *connectable,
						GObject *connection);

void ags_loop_channel_run_resolve_dependency(AgsRecall *recall);

void ags_loop_channel_run_start_callback(AgsCountBeatsAudioRun *count_beats_audio_run,
					 guint run_order,
					 AgsLoopChannelRun *loop_channel_run);
void ags_loop_channel_run_loop_callback(AgsCountBeatsAudioRun *count_beats_audio_run,
					guint run_order,
					AgsLoopChannelRun *loop_channel_run);
void ags_loop_channel_run_stop_callback(AgsCountBeatsAudioRun *count_beats_audio_run,
					guint run_order,
					AgsLoopChannelRun *loop_channel_run);

/**
 * SECTION:ags_loop_channel_run
 * @short_description: loop
 * @title: AgsLoopChannelRun
 * @section_id:
 * @include: ags/audio/recall/ags_loop_channel_run.h
 *
 * The #AgsLoopChannelRun class loops the channel.
 */

enum{
  PROP_0,
  PROP_COUNT_BEATS_AUDIO_RUN,
};

static gpointer ags_loop_channel_run_parent_class = NULL;
static AgsConnectableInterface *ags_loop_channel_run_parent_connectable_interface;

GType
ags_loop_channel_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_loop_channel_run = 0;

    static const GTypeInfo ags_loop_channel_run_info = {
      sizeof (AgsLoopChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_loop_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLoopChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_loop_channel_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_loop_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_loop_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
						       "AgsLoopChannelRun",
						       &ags_loop_channel_run_info,
						       0);
    g_type_add_interface_static(ags_type_loop_channel_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_loop_channel_run);
  }

  return g_define_type_id__volatile;
}

void
ags_loop_channel_run_class_init(AgsLoopChannelRunClass *loop_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_loop_channel_run_parent_class = g_type_class_peek_parent(loop_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) loop_channel_run;

  gobject->set_property = ags_loop_channel_run_set_property;
  gobject->get_property = ags_loop_channel_run_get_property;

  gobject->dispose = ags_loop_channel_run_dispose;
  gobject->finalize = ags_loop_channel_run_finalize;

  /* properties */
  /**
   * AgsLoopChannelRun:count-beats-audio-run:
   *
   * The count beats audio run dependency.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("count-beats-audio-run",
				   i18n_pspec("assigned AgsCountBeatsAudioRun"),
				   i18n_pspec("The pointer to a counter object which indicates when looping should happen"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNT_BEATS_AUDIO_RUN,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) loop_channel_run;

  recall->resolve_dependency = ags_loop_channel_run_resolve_dependency;
}

void
ags_loop_channel_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_loop_channel_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_loop_channel_run_connect;
  connectable->disconnect = ags_loop_channel_run_disconnect;

  connectable->connect_connection = ags_loop_channel_run_connect_connection;
  connectable->disconnect_connection = ags_loop_channel_run_disconnect_connection;
}

void
ags_loop_channel_run_init(AgsLoopChannelRun *loop_channel_run)
{
  ags_recall_set_ability_flags((AgsRecall *) loop_channel_run, (AGS_SOUND_ABILITY_SEQUENCER |
								AGS_SOUND_ABILITY_NOTATION));
  
  AGS_RECALL(loop_channel_run)->name = "ags-loop";
  AGS_RECALL(loop_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(loop_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(loop_channel_run)->xml_type = "ags-loop-channel-run";
  AGS_RECALL(loop_channel_run)->port = NULL;

  AGS_RECALL(loop_channel_run)->child_type = G_TYPE_NONE;

  loop_channel_run->count_beats_audio_run = NULL;
}

void
ags_loop_channel_run_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsLoopChannelRun *loop_channel_run;

  GRecMutex *recall_mutex;

  loop_channel_run = AGS_LOOP_CHANNEL_RUN(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(loop_channel_run);

  switch(prop_id){
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      AgsCountBeatsAudioRun *count_beats_audio_run, *old_count_beats_audio_run;

      gboolean is_template;

      count_beats_audio_run = (AgsCountBeatsAudioRun *) g_value_get_object(value);
      old_count_beats_audio_run = NULL;
      
      g_rec_mutex_lock(recall_mutex);

      if(loop_channel_run->count_beats_audio_run == count_beats_audio_run){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(loop_channel_run->count_beats_audio_run != NULL){
	old_count_beats_audio_run = loop_channel_run->count_beats_audio_run;
	
	g_object_unref(loop_channel_run->count_beats_audio_run);
      }

      if(count_beats_audio_run != NULL){
	g_object_ref(count_beats_audio_run);
      }

      loop_channel_run->count_beats_audio_run = count_beats_audio_run;

      g_rec_mutex_unlock(recall_mutex);

      /* dependency */
      if(ags_recall_test_flags((AgsRecall *) count_beats_audio_run, AGS_RECALL_TEMPLATE)){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(is_template){
	if(old_count_beats_audio_run != NULL){
	  AgsRecallDependency *recall_dependency;

	  GList *list;
	  
	  recall_dependency = NULL;
	  list = ags_recall_dependency_find_dependency(AGS_RECALL(loop_channel_run)->recall_dependency,
						       (GObject *) old_count_beats_audio_run);

	  if(list != NULL){
	    recall_dependency = list->data;
	  }
	  
	  ags_recall_remove_recall_dependency(AGS_RECALL(loop_channel_run),
					      recall_dependency);
	}
      }

      if(is_template &&
	 count_beats_audio_run != NULL){
	ags_recall_add_recall_dependency(AGS_RECALL(loop_channel_run),
					 ags_recall_dependency_new((GObject *) count_beats_audio_run));
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_loop_channel_run_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsLoopChannelRun *loop_channel_run;

  GRecMutex *recall_mutex;

  loop_channel_run = AGS_LOOP_CHANNEL_RUN(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(loop_channel_run);

  switch(prop_id){
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value,
			 loop_channel_run->count_beats_audio_run);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_loop_channel_run_dispose(GObject *gobject)
{
  AgsLoopChannelRun *loop_channel_run;

  loop_channel_run = AGS_LOOP_CHANNEL_RUN(gobject);

  /* count beats audio run */
  if(loop_channel_run->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(loop_channel_run->count_beats_audio_run));

    loop_channel_run->count_beats_audio_run = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_loop_channel_run_parent_class)->dispose(gobject);
}

void
ags_loop_channel_run_finalize(GObject *gobject)
{
  AgsLoopChannelRun *loop_channel_run;

  loop_channel_run = AGS_LOOP_CHANNEL_RUN(gobject);

  /* count beats audio run */
  if(loop_channel_run->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(loop_channel_run->count_beats_audio_run));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_loop_channel_run_parent_class)->finalize(gobject);
}

void
ags_loop_channel_run_connect(AgsConnectable *connectable)
{
  AgsLoopChannelRun *loop_channel_run;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  loop_channel_run = AGS_LOOP_CHANNEL_RUN(connectable);

  /* call parent */
  ags_loop_channel_run_parent_connectable_interface->connect(connectable);

  /* count beats audio run */
  ags_connectable_connect_connection(connectable,
				     (GObject *) loop_channel_run->count_beats_audio_run);
}

void
ags_loop_channel_run_disconnect(AgsConnectable *connectable)
{
  AgsLoopChannelRun *loop_channel_run;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  loop_channel_run = AGS_LOOP_CHANNEL_RUN(connectable);

  /* call parent */
  ags_loop_channel_run_parent_connectable_interface->disconnect(connectable);

  /* count beats audio run */
  ags_connectable_disconnect_connection(connectable,
					(GObject *) loop_channel_run->count_beats_audio_run);
}

void
ags_loop_channel_run_connect_connection(AgsConnectable *connectable,
					GObject *connection)
{
  AgsLoopChannelRun *loop_channel_run;
  AgsCountBeatsAudioRun *count_beats_audio_run;

  loop_channel_run = AGS_LOOP_CHANNEL_RUN(connectable);

  if(connection == NULL){
    return;
  }

  g_object_get(loop_channel_run,
	       "count-beats-audio-run", &count_beats_audio_run,
	       NULL);

  /* AgsCountBeatsAudioRun */
  if((GObject *) count_beats_audio_run == connection){
    g_signal_connect(count_beats_audio_run, "sequencer-start",
		     G_CALLBACK(ags_loop_channel_run_start_callback), loop_channel_run);
    
    g_signal_connect(count_beats_audio_run, "sequencer-loop",
		     G_CALLBACK(ags_loop_channel_run_loop_callback), loop_channel_run);
    
    g_signal_connect(count_beats_audio_run, "sequencer-stop",
		     G_CALLBACK(ags_loop_channel_run_stop_callback), loop_channel_run);
  }

  g_object_unref(count_beats_audio_run);
}

void
ags_loop_channel_run_disconnect_connection(AgsConnectable *connectable,
					   GObject *connection)
{
  AgsLoopChannelRun *loop_channel_run;
  AgsCountBeatsAudioRun *count_beats_audio_run;
  
  loop_channel_run = AGS_LOOP_CHANNEL_RUN(connectable);

  if(connection == NULL){
    return;
  }

  g_object_get(loop_channel_run,
	       "count-beats-audio-run", &count_beats_audio_run,
	       NULL);
  
  /* AgsCountBeatsAudioRun */
  if((GObject *) loop_channel_run->count_beats_audio_run == connection){
    g_object_disconnect(count_beats_audio_run,
			"any_signal::sequencer-start",
			G_CALLBACK(ags_loop_channel_run_start_callback),
			loop_channel_run,
			"any_signal::sequencer-loop",
			G_CALLBACK(ags_loop_channel_run_loop_callback),
			loop_channel_run,
			"any_signal::sequencer-stop",
			G_CALLBACK(ags_loop_channel_run_stop_callback),
			loop_channel_run,
			NULL);
  }

  g_object_unref(count_beats_audio_run);
}

void
ags_loop_channel_run_resolve_dependency(AgsRecall *recall)
{
  AgsRecall *template;
  AgsRecallContainer *recall_container;
  AgsRecallID *recall_id;
  AgsRecallDependency *recall_dependency;

  AgsCountBeatsAudioRun *count_beats_audio_run;

  GList *list_start, *list;  

  guint i, i_stop;

  g_object_get(recall,
	       "recall-container", &recall_container,
	       NULL);

  g_object_get(recall_container,
	       "recall-channel-run", &list_start,
	       NULL);

  list = ags_recall_find_template(list_start);

  if(list == NULL){
    g_warning("AgsRecallClass::resolve - missing dependency");

    g_object_unref(recall_container);
    
    g_list_free_full(list_start,
		     g_object_unref);

    return;
  }
  
  template = AGS_RECALL(list->data);
  
  g_list_free_full(list_start,
		   g_object_unref);

  g_object_get(template,
	       "recall-dependency", &list_start,
	       NULL);

  g_object_get(recall,
	       "recall-id", &recall_id,
	       NULL);

  /* prepare to resolve */
  count_beats_audio_run = NULL;

  list = list_start;

  i_stop = 1;

  for(i = 0; i < i_stop && list != NULL;){
    GObject *dependency;
    
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);

    g_object_get(recall_dependency,
		 "dependency", &dependency,
		 NULL);

    if(AGS_IS_COUNT_BEATS_AUDIO_RUN(dependency)){
      count_beats_audio_run = (AgsCountBeatsAudioRun *) ags_recall_dependency_resolve(recall_dependency,
										      recall_id);

      i++;
    }

    g_object_unref(dependency);
    
    list = list->next;
  }
  
  g_object_set(G_OBJECT(recall),
	       "count-beats-audio-run", count_beats_audio_run,
	       NULL);

  g_object_unref(recall_container);

  g_object_unref(recall_id);

  g_list_free_full(list_start,
		   g_object_unref);
}

void
ags_loop_channel_run_create_audio_signals(AgsLoopChannelRun *loop_channel_run)
{
  AgsChannel *channel;
  AgsRecycling *first_recycling, *last_recycling;
  AgsRecycling *end_recycling;
  AgsRecycling *recycling, *next_recycling;
  AgsAudioSignal *audio_signal;
  AgsRecallID *recall_id;
  
  GObject *output_soundcard;

  gdouble delay;
  guint attack;

  //  g_message("debug");

  g_object_get(loop_channel_run,
	       "source", &channel,
	       NULL);

  /* recycling */
  g_object_get(channel,
	       "first-recycling", &first_recycling,
	       "last-recycling", &last_recycling,
	       NULL);

  if(first_recycling == NULL){
    g_object_unref(channel);
    
    return;
  }

  g_object_get(loop_channel_run,
	       "output-soundcard", &output_soundcard,
	       "recall-id", &recall_id,
	       NULL);

  end_recycling = ags_recycling_next(last_recycling);
  
  /* delay and attack */
  //TODO:JK: unclear
  attack = 0;// soundcard->attack[((tic_counter_incr == AGS_NOTATION_TICS_PER_BEAT) ?
  //		   0:
  //			   tic_counter_incr)];
  delay = 0.0; //soundcard->delay[((tic_counter_incr == AGS_NOTATION_TICS_PER_BEAT) ?
  //		 0:
  //			 tic_counter_incr)];

  recycling = first_recycling;
  g_object_ref(recycling);

  next_recycling = NULL;
  
  while(recycling != end_recycling){
    audio_signal = ags_audio_signal_new((GObject *) output_soundcard,
					(GObject *) recycling,
					(GObject *) recall_id);
    
    audio_signal->stream_current = audio_signal->stream;
    
    /*  */
    ags_recycling_create_audio_signal_with_defaults(recycling,
						    audio_signal,
						    delay, attack);

    ags_connectable_connect(AGS_CONNECTABLE(audio_signal));    
    ags_recycling_add_audio_signal(recycling,
				   audio_signal);

    /*
     * unref AgsAudioSignal because AgsLoopChannelRun has no need for it
     * if you need a valid reference to audio_signal you have to g_object_ref(audio_signal)
     */
    //FIXME:JK:

#ifdef AGS_DEBUG
    g_message("+++++++++++++++++++++++++\n\nloop channel created: AgsAudioSignal#%llx[%u]\n\n+++++++++++++++++++++++++",
	      (long long unsigned int) audio_signal, audio_signal->length);
#endif

    next_recycling = ags_recycling_next(recycling);

    g_object_unref(recycling);

    recycling = next_recycling;
  }

  /* unref */
  g_object_unref(channel);

  g_object_unref(output_soundcard);
  
  g_object_unref(recall_id);

  g_object_unref(first_recycling);
  g_object_unref(last_recycling);

  if(end_recycling != NULL){
    g_object_unref(end_recycling);
  }

  if(next_recycling != NULL){
    g_object_unref(next_recycling);
  }
}

void
ags_loop_channel_run_start_callback(AgsCountBeatsAudioRun *count_beats_audio_run,
				    guint run_order,
				    AgsLoopChannelRun *loop_channel_run)
{  
#ifdef AGS_DEBUG
  g_message("ags_loop_channel_run_start_callback - run_order: %u; %u\n",
	    AGS_RECALL_CHANNEL_RUN(loop_channel_run)->run_order,
	    run_order);
#endif 

  ags_loop_channel_run_create_audio_signals(loop_channel_run);
}

void 
ags_loop_channel_run_loop_callback(AgsCountBeatsAudioRun *count_beats_audio_run,
				   guint run_order,
				   AgsLoopChannelRun *loop_channel_run)
{
#ifdef AGS_DEBUG
  g_message("ags_loop_channel_run_loop_callback - run_order: %u; %u",
	    AGS_RECALL_CHANNEL_RUN(loop_channel_run)->run_order,
	    run_order);
#endif

  ags_loop_channel_run_create_audio_signals(loop_channel_run);
}

void 
ags_loop_channel_run_stop_callback(AgsCountBeatsAudioRun *count_beats_audio_run,
				   guint run_order,
				   AgsLoopChannelRun *loop_channel_run)
{
  /* empty */
}

/**
 * ags_loop_channel_run_new:
 * @source: the #AgsChannel
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun dependency
 *
 * Create a new instance of #AgsLoopChannelRun
 *
 * Returns: the new #AgsLoopChannelRun
 *
 * Since: 3.0.0
 */
AgsLoopChannelRun*
ags_loop_channel_run_new(AgsChannel *source,
			 AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsLoopChannelRun *loop_channel_run;

  loop_channel_run = (AgsLoopChannelRun *) g_object_new(AGS_TYPE_LOOP_CHANNEL_RUN,
							"source", source,
							"count-beats-audio-run", count_beats_audio_run,
							NULL);

  return(loop_channel_run);
}
