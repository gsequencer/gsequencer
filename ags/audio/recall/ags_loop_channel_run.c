/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/libags.h>

#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_delay_audio.h>

#include <math.h>

#include <ags/i18n.h>

void ags_loop_channel_run_class_init(AgsLoopChannelRunClass *loop_channel_run);
void ags_loop_channel_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_loop_channel_run_plugin_interface_init(AgsPluginInterface *plugin);
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

void ags_loop_channel_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_loop_channel_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

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

void ags_loop_channel_run_write_resolve_dependency(AgsFileLookup *file_lookup,
						   GObject *recall);
void ags_loop_channel_run_read_resolve_dependency(AgsFileLookup *file_lookup,
						  GObject *recall);

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
static AgsPluginInterface *ags_loop_channel_run_parent_plugin_interface;

GType
ags_loop_channel_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_loop_channel_run;

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

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_loop_channel_run_plugin_interface_init,
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

    g_type_add_interface_static(ags_type_loop_channel_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave (&g_define_type_id__volatile, ags_type_loop_channel_run);
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
   * Since: 2.0.0
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
ags_loop_channel_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_loop_channel_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->read = ags_loop_channel_run_read;
  plugin->write = ags_loop_channel_run_write;
}

void
ags_loop_channel_run_init(AgsLoopChannelRun *loop_channel_run)
{
  ags_recall_set_ability_flags(loop_channel_run, (AGS_SOUND_ABILITY_SEQUENCER |
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

  pthread_mutex_t *recall_mutex;

  loop_channel_run = AGS_LOOP_CHANNEL_RUN(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      AgsCountBeatsAudioRun *count_beats_audio_run, *old_count_beats_audio_run;

      gboolean is_template;

      count_beats_audio_run = (AgsCountBeatsAudioRun *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(loop_channel_run->count_beats_audio_run == count_beats_audio_run){
	pthread_mutex_unlock(recall_mutex);

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

      pthread_mutex_unlock(recall_mutex);

      /* dependency */
      if(ags_recall_test_flags(count_beats_audio_run, AGS_RECALL_TEMPLATE)){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(is_template &&
	 old_count_beats_audio_run != NULL){
	ags_recall_remove_recall_dependency(AGS_RECALL(loop_channel_run),
					    (AgsRecall *) old_count_beats_audio_run);
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

  pthread_mutex_t *recall_mutex;

  loop_channel_run = AGS_LOOP_CHANNEL_RUN(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value,
			 loop_channel_run->count_beats_audio_run);

      pthread_mutex_unlock(recall_mutex);
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
				     loop_channel_run->count_beats_audio_run);
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
					loop_channel_run->count_beats_audio_run);
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
  if(count_beats_audio_run == connection){
    g_signal_connect(count_beats_audio_run, "sequencer-start",
		     G_CALLBACK(ags_loop_channel_run_start_callback), loop_channel_run);
    
    g_signal_connect(count_beats_audio_run, "sequencer-loop",
		     G_CALLBACK(ags_loop_channel_run_loop_callback), loop_channel_run);
    
    g_signal_connect(count_beats_audio_run, "sequencer-stop",
		     G_CALLBACK(ags_loop_channel_run_stop_callback), loop_channel_run);
  }
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
  if(loop_channel_run->count_beats_audio_run == connection){
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
}

void
ags_loop_channel_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;
  xmlNode *iter;

  /* read parent */
  ags_loop_channel_run_parent_plugin_interface->read(file, node, plugin);

  /* read depenendency */
  iter = node->children;

  while(iter != NULL){
    if(iter->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(iter->name,
		     "ags-dependency-list",
		     19)){
	xmlNode *dependency_node;

	dependency_node = iter->children;

	while(dependency_node != NULL){
	  if(dependency_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(dependency_node->name,
			   "ags-dependency",
			   15)){
	      file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
							   "file", file,
							   "node", dependency_node,
							   "reference", G_OBJECT(plugin),
							   NULL);
	      ags_file_add_lookup(file, (GObject *) file_lookup);
	      g_signal_connect(G_OBJECT(file_lookup), "resolve",
			       G_CALLBACK(ags_loop_channel_run_read_resolve_dependency), G_OBJECT(plugin));
	    }
	  }
	  
	  dependency_node = dependency_node->next;
	}
      }
    }

    iter = iter->next;
  }
}

xmlNode*
ags_loop_channel_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;
  xmlNode *node, *child;
  xmlNode *dependency_node;
  GList *list;
  gchar *id;

  /* write parent */
  node = ags_loop_channel_run_parent_plugin_interface->write(file, parent, plugin);

  /* write dependencies */
  child = xmlNewNode(NULL,
		     "ags-dependency-list");

  xmlNewProp(child,
	     AGS_FILE_ID_PROP,
	     ags_id_generator_create_uuid());

  xmlAddChild(node,
	      child);

  list = AGS_RECALL(plugin)->recall_dependency;

  while(list != NULL){
    id = ags_id_generator_create_uuid();

    dependency_node = xmlNewNode(NULL,
				 "ags-dependency");

    xmlNewProp(dependency_node,
	       AGS_FILE_ID_PROP,
	       id);

    xmlAddChild(child,
		dependency_node);

    file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
						 "file", file,
						 "node", dependency_node,
						 "reference", G_OBJECT(plugin),
						 NULL);
    ags_file_add_lookup(file, (GObject *) file_lookup);
    g_signal_connect(G_OBJECT(file_lookup), "resolve",
		     G_CALLBACK(ags_loop_channel_run_write_resolve_dependency), G_OBJECT(plugin));

    list = list->next;
  }

  return(node);
}

void
ags_loop_channel_run_resolve_dependency(AgsRecall *recall)
{
  AgsRecall *template;
  AgsRecallContainer *recall_container;
  AgsRecallID *recall_id;
  AgsRecallDependency *recall_dependency;
  AgsCountBeatsAudioRun *count_beats_audio_run;

  GObject *dependency;

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

    return;
  }
  
  template = AGS_RECALL(list->data);
  g_list_free(list_start);

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
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);

    g_object_get(recall_dependency,
		 "dependency", &dependency,
		 NULL);

    if(AGS_IS_COUNT_BEATS_AUDIO_RUN(dependency)){
      count_beats_audio_run = (AgsCountBeatsAudioRun *) ags_recall_dependency_resolve(recall_dependency,
										      recall_id);

      i++;
    }

    list = list->next;
  }
  
  g_object_set(G_OBJECT(recall),
	       "count-beats-audio-run", count_beats_audio_run,
	       NULL);
}

void
ags_loop_channel_run_create_audio_signals(AgsLoopChannelRun *loop_channel_run)
{
  AgsChannel *channel;
  AgsRecycling *recycling;
  AgsRecycling *last_recycling, *end_recycling;
  AgsAudioSignal *audio_signal;
  AgsRecallID *recall_id;
  
  GObject *output_soundcard;

  gdouble delay;
  guint attack;

  //  g_message("debug");

  g_object_get(loop_channel_run,
	       "output-soundcard", &output_soundcard,
	       "source", &channel,
	       "recall-id", &recall_id,
	       NULL);

  /* recycling */
  g_object_get(channel,
	       "first-recycling", &recycling,
	       "last-recycling", &last_recycling,
	       NULL);

  if(recycling == NULL){
    return;
  }
  
  g_object_get(last_recycling,
	       "next", &end_recycling,
	       NULL);
  
  /* delay and attack */
  //TODO:JK: unclear
  attack = 0;// soundcard->attack[((tic_counter_incr == AGS_NOTATION_TICS_PER_BEAT) ?
  //		   0:
  //			   tic_counter_incr)];
  delay = 0.0; //soundcard->delay[((tic_counter_incr == AGS_NOTATION_TICS_PER_BEAT) ?
  //		 0:
  //			 tic_counter_incr)];

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

    recycling = recycling->next;
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

void
ags_loop_channel_run_write_resolve_dependency(AgsFileLookup *file_lookup,
					      GObject *recall)
{
  AgsFileIdRef *id_ref;
  gchar *id;

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, file_lookup->ref);

  id = xmlGetProp(id_ref->node, AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     "xpath",
  	     g_strdup_printf("xpath=//*[@id='%s']", id));
}

void
ags_loop_channel_run_read_resolve_dependency(AgsFileLookup *file_lookup,
					     GObject *recall)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) xmlGetProp(file_lookup->node,
			       "xpath");

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file, xpath);

  if(AGS_IS_DELAY_AUDIO_RUN(id_ref->ref)){
    g_object_set(G_OBJECT(recall),
		 "count-beats-audio-run", id_ref->ref,
		 NULL);
  }
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
 * Since: 2.0.0
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
