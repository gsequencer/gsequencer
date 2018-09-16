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

#include <ags/audio/recall/ags_play_lv2_audio.h>

#include <ags/libags.h>

#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_lv2_conversion.h>
#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_port.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <libxml/tree.h>

#include <ags/i18n.h>

void ags_play_lv2_audio_class_init(AgsPlayLv2AudioClass *play_lv2_audio);
void ags_play_lv2_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_play_lv2_audio_init(AgsPlayLv2Audio *play_lv2_audio);
void ags_play_lv2_audio_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_play_lv2_audio_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_play_lv2_audio_dispose(GObject *gobject);
void ags_play_lv2_audio_finalize(GObject *gobject);

void ags_play_lv2_audio_set_ports(AgsPlugin *plugin, GList *port);
void ags_play_lv2_audio_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_play_lv2_audio_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

/**
 * SECTION:ags_play_lv2_audio
 * @short_description: play audio lv2
 * @title: AgsPlayLv2Audio
 * @section_id:
 * @include: ags/audio/recall/ags_play_lv2_audio.h
 *
 * The #AgsPlayLv2Audio class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_TURTLE,
  PROP_URI,
  PROP_PLUGIN,
  PROP_INPUT_LINES,
  PROP_OUTPUT_LINES,
};

static gpointer ags_play_lv2_audio_parent_class = NULL;
static AgsPluginInterface *ags_play_lv2_parent_plugin_interface;

static const gchar *ags_play_lv2_audio_plugin_name = "ags-play-lv2";
static const gchar *ags_play_lv2_audio_specifier[] = {
};
static const gchar *ags_play_lv2_audio_control_port[] = {
};

GType
ags_play_lv2_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_play_lv2_audio;

    static const GTypeInfo ags_play_lv2_audio_info = {
      sizeof (AgsPlayLv2AudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_lv2_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsPlayLv2Audio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_lv2_audio_init,
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_play_lv2_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };    

    ags_type_play_lv2_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						     "AgsPlayLv2Audio",
						     &ags_play_lv2_audio_info,
						     0);

    g_type_add_interface_static(ags_type_play_lv2_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave (&g_define_type_id__volatile, ags_type_play_lv2_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_play_lv2_audio_class_init(AgsPlayLv2AudioClass *play_lv2_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_play_lv2_audio_parent_class = g_type_class_peek_parent(play_lv2_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) play_lv2_audio;

  gobject->set_property = ags_play_lv2_audio_set_property;
  gobject->get_property = ags_play_lv2_audio_get_property;

  gobject->dispose = ags_play_lv2_audio_dispose;
  gobject->finalize = ags_play_lv2_audio_finalize;

  /* properties */
  /**
   * AgsPlayLv2Audio:turtle:
   *
   * The assigned turtle.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("turtle",
				   i18n_pspec("turtle of recall lv2"),
				   i18n_pspec("The turtle which this recall lv2 is described by"),
				   AGS_TYPE_TURTLE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TURTLE,
				  param_spec);

  /**
   * AgsPlayLv2Audio:uri:
   *
   * The uri's name.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_string("uri",
				    i18n_pspec("the uri"),
				    i18n_pspec("The uri's string representation"),
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_URI,
				  param_spec);

  /**
   * AgsPlayLv2Audio:plugin:
   *
   * The plugin's plugin object.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_object("plugin",
				    i18n_pspec("the plugin"),
				    i18n_pspec("The plugin as plugin object"),
				    AGS_TYPE_LV2_PLUGIN,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLUGIN,
				  param_spec);

  /**
   * AgsPlayLv2Audio:input-lines:
   *
   * The effect's input lines count.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_ulong("input-lines",
				   i18n_pspec("input lines of effect"),
				   i18n_pspec("The effect's count of input lines"),
				   0,
				   65535,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_LINES,
				  param_spec);

  /**
   * AgsPlayLv2Audio:output-lines:
   *
   * The effect's output lines count.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_ulong("output-lines",
				   i18n_pspec("output lines of effect"),
				   i18n_pspec("The effect's count of output lines"),
				   0,
				   65535,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_LINES,
				  param_spec);
}

void
ags_play_lv2_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_play_lv2_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->read = ags_play_lv2_audio_read;
  plugin->write = ags_play_lv2_audio_write;
  plugin->set_ports = ags_play_lv2_audio_set_ports;
}

void
ags_play_lv2_audio_init(AgsPlayLv2Audio *play_lv2_audio)
{
  GList *port;

  AGS_RECALL(play_lv2_audio)->name = "ags-play-lv2";
  AGS_RECALL(play_lv2_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_lv2_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_lv2_audio)->xml_type = "ags-play-lv2-audio";

  play_lv2_audio->turtle = NULL;

  play_lv2_audio->uri = NULL;

  play_lv2_audio->plugin = NULL;
  play_lv2_audio->plugin_descriptor = NULL;

  play_lv2_audio->input_port = NULL;
  play_lv2_audio->input_lines = 0;

  play_lv2_audio->output_port = NULL;
  play_lv2_audio->output_lines = 0;

  play_lv2_audio->event_port = 0;
  play_lv2_audio->atom_port = 0;

  play_lv2_audio->bank = 0;
  play_lv2_audio->program = 0;
}

void
ags_play_lv2_audio_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsPlayLv2Audio *play_lv2_audio;

  pthread_mutex_t *recall_mutex;

  play_lv2_audio = AGS_PLAY_LV2_AUDIO(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_TURTLE:
    {
      AgsTurtle *turtle;

      turtle = (AgsTurtle *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(play_lv2_audio->turtle == turtle){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(play_lv2_audio->turtle != NULL){
	g_object_unref(play_lv2_audio->turtle);
      }

      if(turtle != NULL){
	g_object_ref(turtle);
      }

      play_lv2_audio->turtle = turtle;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_URI:
    {
      gchar *uri;
      
      uri = g_value_get_string(value);

      pthread_mutex_lock(recall_mutex);

      if(uri == play_lv2_audio->uri){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(play_lv2_audio->uri != NULL){
	g_free(play_lv2_audio->uri);
      }

      play_lv2_audio->uri = g_strdup(uri);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_PLUGIN:
    {
      AgsLv2Plugin *lv2_plugin;
      
      lv2_plugin = g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(play_lv2_audio->plugin == lv2_plugin){
	pthread_mutex_unlock(recall_mutex);
	
	return;
      }

      if(play_lv2_audio->plugin != NULL){
	g_object_unref(play_lv2_audio->plugin);
      }

      if(lv2_plugin != NULL){
	g_object_ref(lv2_plugin);
      }

      play_lv2_audio->plugin = lv2_plugin;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_INPUT_LINES:
    {
      unsigned long effect_input_lines;
      
      effect_input_lines = g_value_get_ulong(value);

      pthread_mutex_lock(recall_mutex);

      if(effect_input_lines == play_lv2_audio->input_lines){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      play_lv2_audio->input_lines = effect_input_lines;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_OUTPUT_LINES:
    {
      unsigned long effect_output_lines;
      
      effect_output_lines = g_value_get_ulong(value);

      pthread_mutex_lock(recall_mutex);

      if(effect_output_lines == play_lv2_audio->output_lines){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      play_lv2_audio->output_lines = effect_output_lines;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_lv2_audio_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsPlayLv2Audio *play_lv2_audio;

  pthread_mutex_t *recall_mutex;

  play_lv2_audio = AGS_PLAY_LV2_AUDIO(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_TURTLE:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, play_lv2_audio->turtle);
    }
    break;
  case PROP_URI:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_string(value, play_lv2_audio->uri);
    }
    break;
  case PROP_PLUGIN:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, play_lv2_audio->plugin);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_INPUT_LINES:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_ulong(value, play_lv2_audio->input_lines);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_OUTPUT_LINES:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_ulong(value, play_lv2_audio->output_lines);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_lv2_audio_dispose(GObject *gobject)
{
  AgsPlayLv2Audio *play_lv2_audio;
  
  play_lv2_audio = AGS_PLAY_LV2_AUDIO(gobject);

  /* turtle */
  if(play_lv2_audio->turtle != NULL){
    g_object_unref(play_lv2_audio->turtle);

    play_lv2_audio->turtle = NULL;
  }

  /* plugin */
  if(play_lv2_audio->plugin != NULL){
    g_object_unref(play_lv2_audio->plugin);

    play_lv2_audio->plugin = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_lv2_audio_parent_class)->dispose(gobject);
}

void
ags_play_lv2_audio_finalize(GObject *gobject)
{
  AgsPlayLv2Audio *play_lv2_audio;
  
  play_lv2_audio = AGS_PLAY_LV2_AUDIO(gobject);

  /* turtle */
  if(play_lv2_audio->turtle != NULL){
    g_object_unref(play_lv2_audio->turtle);
  }

  /* plugin */
  if(play_lv2_audio->plugin != NULL){
    g_object_unref(play_lv2_audio->plugin);
  }

  /* filename, effect and uri */
  g_free(play_lv2_audio->uri);
  
  /* call parent */
  G_OBJECT_CLASS(ags_play_lv2_audio_parent_class)->finalize(gobject);
}

void
ags_play_lv2_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  AgsPlayLv2Audio *play_lv2_audio;
  AgsPort *current;
  
  AgsLv2Plugin *lv2_plugin;
  
  GList *start_list, *list;  
  GList *start_plugin_port, *plugin_port;
  
  unsigned long port_count;
  unsigned long i;

  pthread_mutex_t *recall_mutex;

  play_lv2_audio = AGS_PLAY_LV2_AUDIO(plugin);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(play_lv2_audio)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  g_object_get(play_lv2_audio,
	       "port", &start_list,
	       "plugin", &lv2_plugin,
	       NULL);

  g_object_get(lv2_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);
  
  if(start_plugin_port != NULL){
    plugin_port = start_plugin_port;
    
    port_count = g_list_length(start_plugin_port);

    for(i = 0; i < port_count; i++){
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
	gchar *specifier;

	pthread_mutex_t *plugin_port_mutex;
	
	/* get plugin port mutex */
	pthread_mutex_lock(ags_plugin_port_get_class_mutex());
	
	plugin_port_mutex = AGS_PLUGIN_PORT(plugin_port->data)->obj_mutex;
	
	pthread_mutex_unlock(ags_plugin_port_get_class_mutex());

	/* get specifier */
	pthread_mutex_lock(plugin_port_mutex);
	
	specifier = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);

	pthread_mutex_unlock(plugin_port_mutex);

	if(specifier == NULL){
	  plugin_port = plugin_port->next;
	  
	  continue;
	}
	
	list = start_list;
	current = NULL;
	
	while(list != NULL){
	  gboolean success;

	  pthread_mutex_t *port_mutex;

	  pthread_mutex_lock(ags_port_get_class_mutex());
	  
	  port_mutex = AGS_PORT(list->data)->obj_mutex;
	  
	  pthread_mutex_unlock(ags_port_get_class_mutex());

	  /* check success */
	  pthread_mutex_lock(port_mutex);
	  
	  success = (!g_strcmp0(specifier,
				AGS_PORT(list->data)->specifier)) ? TRUE: FALSE;

	  pthread_mutex_unlock(port_mutex);

	  if(success){
	    current = list->data;
	    
	    break;
	  }

	  /* iterate */
	  list = list->next;
	}

	if(current != NULL){
	  GValue value = {0,};
	  
	  g_object_set(current,
		       "plugin-port", plugin_port->data,
		       NULL);
	  
	  ags_play_lv2_audio_load_conversion(play_lv2_audio,
					     (GObject *) current,
					     (GObject *) plugin_port->data);

	  g_object_get_property(plugin_port->data,
				"default-value",
				&value);

	  ags_port_safe_write_raw(current,
				  &value);
	
#ifdef AGS_DEBUG
	  g_message("connecting port: %d/%d", i, port_count);      
#endif
	}
      }else if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_AUDIO)){
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INPUT)){
	  pthread_mutex_lock(recall_mutex);

	  if(play_lv2_audio->input_port == NULL){
	    play_lv2_audio->input_port = (uint32_t *) malloc(sizeof(uint32_t));
	    play_lv2_audio->input_port[0] = i;
	  }else{
	    play_lv2_audio->input_port = (uint32_t *) realloc(play_lv2_audio->input_port,
							      (play_lv2_audio->input_lines + 1) * sizeof(uint32_t));
	    play_lv2_audio->input_port[play_lv2_audio->input_lines] = i;
	  }

	  play_lv2_audio->input_lines += 1;

	  pthread_mutex_unlock(recall_mutex);
	}else if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  pthread_mutex_lock(recall_mutex);

	  if(play_lv2_audio->output_port == NULL){
	    play_lv2_audio->output_port = (uint32_t *) malloc(sizeof(uint32_t));
	    play_lv2_audio->output_port[0] = i;
	  }else{
	    play_lv2_audio->output_port = (uint32_t *) realloc(play_lv2_audio->output_port,
							       (play_lv2_audio->output_lines + 1) * sizeof(uint32_t));
	    play_lv2_audio->output_port[play_lv2_audio->output_lines] = i;
	  }

	  play_lv2_audio->output_lines += 1;

	  pthread_mutex_unlock(recall_mutex);
	}
      }

      plugin_port = plugin_port->next;
    }

    AGS_RECALL(play_lv2_audio)->port = g_list_reverse(port);
  }
}

void
ags_play_lv2_audio_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsPlayLv2Audio *gobject;

  AgsLv2Plugin *lv2_plugin;

  gchar *filename, *effect, *uri;

  uint32_t effect_index;

  gobject = AGS_PLAY_LV2_AUDIO(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", gobject,
				   NULL));

  filename = xmlGetProp(node,
			"filename");
  effect = xmlGetProp(node,
		      "effect");
  uri = xmlGetProp(node,
		   "uri");
  effect_index = g_ascii_strtoull(xmlGetProp(node,
					     "index"),
				  NULL,
				  10);

  g_object_set(gobject,
	       "filename", filename,
	       "effect", effect,
	       "uri", uri,
	       "effect-index", effect_index,
	       NULL);

  ags_play_lv2_audio_load(gobject);
}

xmlNode*
ags_play_lv2_audio_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsPlayLv2Audio *play_lv2_audio;

  xmlNode *node;

  GList *list;

  gchar *id;

  play_lv2_audio = AGS_PLAY_LV2_AUDIO(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-recall-lv2");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", play_lv2_audio,
				   NULL));

  xmlNewProp(node,
	     "filename",
	     g_strdup(AGS_RECALL(play_lv2_audio)->filename));
  
  xmlNewProp(node,
	     "effect",
	     g_strdup(AGS_RECALL(play_lv2_audio)->effect));

  xmlNewProp(node,
	     "uri",
	     g_strdup(play_lv2_audio->uri));
  
  xmlNewProp(node,
	     "index",
	     g_strdup_printf("%d", AGS_RECALL(play_lv2_audio)->effect_index));
  
  xmlAddChild(parent,
	      node);

  return(node);
}

/**
 * ags_play_lv2_audio_test_flags:
 * @play_lv2_audio: the #AgsPlayLv2Audio
 * @flags: the flags
 *
 * Test @flags to be set on @play_lv2_audio.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_play_lv2_audio_test_flags(AgsPlayLv2Audio *play_lv2_audio, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *recall_mutex;

  if(!AGS_IS_PLAY_LV2_AUDIO(play_lv2_audio)){
    return(FALSE);
  }

  /* get play_lv2_audio mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(play_lv2_audio)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* test */
  pthread_mutex_lock(recall_mutex);

  retval = (flags & (play_lv2_audio->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(recall_mutex);

  return(retval);
}

/**
 * ags_play_lv2_audio_set_flags:
 * @play_lv2_audio: the #AgsPlayLv2Audio
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.0.0
 */
void
ags_play_lv2_audio_set_flags(AgsPlayLv2Audio *play_lv2_audio, guint flags)
{
  pthread_mutex_t *recall_mutex;

  if(!AGS_IS_PLAY_LV2_AUDIO(play_lv2_audio)){
    return;
  }

  /* get play_lv2_audio mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(play_lv2_audio)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(recall_mutex);

  play_lv2_audio->flags |= flags;

  pthread_mutex_unlock(recall_mutex);
}

/**
 * ags_play_lv2_audio_unset_flags:
 * @play_lv2_audio: the #AgsPlayLv2Audio
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.0.0
 */
void
ags_play_lv2_audio_unset_flags(AgsPlayLv2Audio *play_lv2_audio, guint flags)
{
  pthread_mutex_t *recall_mutex;

  if(!AGS_IS_PLAY_LV2_AUDIO(play_lv2_audio)){
    return;
  }

  /* get play_lv2_audio mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(play_lv2_audio)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(recall_mutex);

  play_lv2_audio->flags &= (~flags);

  pthread_mutex_unlock(recall_mutex);
}

/**
 * ags_play_lv2_audio_load:
 * @play_lv2_audio: the #AgsPlayLv2Audio
 *
 * Set up LV2 handle.
 * 
 * Since: 2.0.0
 */
void
ags_play_lv2_audio_load(AgsPlayLv2Audio *play_lv2_audio)
{
  AgsLv2Plugin *lv2_plugin;

  gchar *filename, *effect;

  unsigned long effect_index;

  void *plugin_so;
  LV2_Descriptor_Function lv2_descriptor;
  LV2_Descriptor *plugin_descriptor;
  
  pthread_mutex_t *recall_mutex;

  if(!AGS_IS_PLAY_LV2_AUDIO(play_lv2_audio)){
    return;
  }
  
  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(play_lv2_audio)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get filename and effect */
  pthread_mutex_lock(recall_mutex);

  filename = g_strdup(AGS_RECALL(play_lv2_audio)->filename);
  effect = g_strdup(AGS_RECALL(play_lv2_audio)->effect);
  
  pthread_mutex_unlock(recall_mutex);
  
  /* find AgsLv2Plugin */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
						  filename, effect);

  /* get some fields */
  g_object_get(play_lv2_audio,
	       "effect-index", &effect_index,
	       "plugin", &lv2_plugin,
	       NULL);

  g_object_get(lv2_plugin,
	       "plugin-so", &plugin_so,
	       NULL);
  
  if(plugin_so != NULL){
    lv2_descriptor = (LV2_Descriptor_Function) dlsym(plugin_so,
						     "lv2_descriptor");

    if(dlerror() == NULL && lv2_descriptor){
      play_lv2_audio->plugin_descriptor = 
	plugin_descriptor = lv2_descriptor(effect_index);

      if(ags_lv2_plugin_test_flags(lv2_plugin, AGS_LV2_PLUGIN_NEEDS_WORKER)){
	ags_play_lv2_audio_set_flags(play_lv2_audio, AGS_PLAY_LV2_AUDIO_HAS_WORKER);
      }
    }
  }

  /* free */
  g_free(filename);
  g_free(effect);
}

/**
 * ags_play_lv2_audio_load_ports:
 * @play_lv2_audio: the #AgsPlayLv2Audio
 *
 * Set up LV2 ports.
 *
 * Returns: a #GList-struct containing #AgsPort
 * 
 * Since: 2.0.0
 */
GList*
ags_play_lv2_audio_load_ports(AgsPlayLv2Audio *play_lv2_audio)
{
  AgsPort *current;

  AgsLv2Plugin *lv2_plugin;

  GList *start_port;
  GList *start_plugin_port, *plugin_port;

  gchar *filename, *effect;
  gchar *plugin_name;

  uint32_t port_count;
  uint32_t i;

  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *base_plugin_mutex;

  if(!AGS_IS_PLAY_LV2_AUDIO(play_lv2_audio)){
    return(NULL);
  }

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(play_lv2_audio)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get filename and effect */
  pthread_mutex_lock(recall_mutex);

  filename = g_strdup(AGS_RECALL(play_lv2_audio)->filename);
  effect = g_strdup(AGS_RECALL(play_lv2_audio)->effect);
  
  pthread_mutex_unlock(recall_mutex);
  
  /* find AgsLv2Plugin */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
						  filename, effect);

  /* get plugin port */
  g_object_get(lv2_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = AGS_BASE_PLUGIN(lv2_plugin)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());
  
  /* get plugin name */
  pthread_mutex_lock(base_plugin_mutex);

  plugin_name = g_strdup_printf("lv2-<%s>", lv2_plugin->uri);

  pthread_mutex_unlock(base_plugin_mutex);

  plugin_port = start_plugin_port;
  start_port = NULL;
  
  if(plugin_port != NULL){
    port_count = g_list_length(plugin_port);
    
    for(i = 0; i < port_count; i++){
      gchar *specifier;

      GValue *value;
	
      pthread_mutex_t *plugin_port_mutex;

      /* get plugin port mutex */
      pthread_mutex_lock(ags_plugin_port_get_class_mutex());
	
      plugin_port_mutex = AGS_PLUGIN_PORT(plugin_port->data)->obj_mutex;
	
      pthread_mutex_unlock(ags_plugin_port_get_class_mutex());
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INPUT)){
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_EVENT)){
	  play_lv2_audio->flags |= AGS_PLAY_LV2_AUDIO_HAS_EVENT_PORT;
	  play_lv2_audio->event_port = AGS_PLUGIN_PORT(plugin_port->data)->port_index;
	}
      
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_ATOM)){
	  play_lv2_audio->flags |= AGS_PLAY_LV2_AUDIO_HAS_ATOM_PORT;
	  play_lv2_audio->atom_port = AGS_PLUGIN_PORT(plugin_port->data)->port_index;
	}
      }
      
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL) &&
	 (ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INPUT) ||
	  ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT))){
	gchar *specifier;

	/* get specifier */
	pthread_mutex_lock(plugin_port_mutex);
	
	specifier = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);

	pthread_mutex_unlock(plugin_port_mutex);

	if(specifier == NULL){
	  plugin_port = plugin_port->next;
	  
	  continue;
	}
	
	current = g_object_new(AGS_TYPE_PORT,
			       "plugin-name", plugin_name,
			       "specifier", specifier,
			       "control-port", g_strdup_printf("%u/%u",
								 i,
								 port_count),
			       "port-value-is-pointer", FALSE,
			       "port-value-type", G_TYPE_FLOAT,
			       NULL);
	g_object_ref(current);
	
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  ags_recall_set_flags(play_lv2_audio,
			       AGS_RECALL_HAS_OUTPUT_PORT);

	  ags_port_set_flags(current,
			     AGS_PORT_IS_OUTPUT);
	}else{
	  gint scale_steps;

	  g_object_get(plugin_port->data,
		       "scale-steps", &scale_steps,
		       NULL);
	  
	  if(!ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INTEGER) &&
	     !ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_TOGGLED) &&
	     scale_steps == -1){
	    ags_port_set_flags(current,
			       AGS_PORT_INFINITE_RANGE);
	  }
	}

	g_object_set(current,
		     "plugin-port", plugin_port->data,
		     NULL);

	ags_play_lv2_audio_load_conversion(play_lv2_audio,
					   (GObject *) current,
					   (GObject *) plugin_port->data);

	g_object_get(plugin_port->data,
		     "default-value", &value,
		      NULL);

	current->port_value.ags_port_ladspa = g_value_get_float(value);

#ifdef AGS_DEBUG
	g_message("connecting port: %s %d/%d", specifier, i, port_count);
#endif
	
	start_port = g_list_prepend(start_port,
				    current);
      }else if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_AUDIO)){
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INPUT)){
	  pthread_mutex_lock(recall_mutex);

	  if(play_lv2_audio->input_port == NULL){
	    play_lv2_audio->input_port = (uint32_t *) malloc(sizeof(uint32_t));
	    play_lv2_audio->input_port[0] = AGS_PLUGIN_PORT(plugin_port->data)->port_index;
	  }else{
	    play_lv2_audio->input_port = (uint32_t *) realloc(play_lv2_audio->input_port,
							      (play_lv2_audio->input_lines + 1) * sizeof(uint32_t));
	    play_lv2_audio->input_port[play_lv2_audio->input_lines] = AGS_PLUGIN_PORT(plugin_port->data)->port_index;
	  }
	  
	  play_lv2_audio->input_lines += 1;

	  pthread_mutex_unlock(recall_mutex);
	}else if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  pthread_mutex_lock(recall_mutex);

	  if(play_lv2_audio->output_port == NULL){
	    play_lv2_audio->output_port = (uint32_t *) malloc(sizeof(uint32_t));
	    play_lv2_audio->output_port[0] = AGS_PLUGIN_PORT(plugin_port->data)->port_index;
	  }else{
	    play_lv2_audio->output_port = (uint32_t *) realloc(play_lv2_audio->output_port,
							       (play_lv2_audio->output_lines + 1) * sizeof(uint32_t));
	    play_lv2_audio->output_port[play_lv2_audio->output_lines] = AGS_PLUGIN_PORT(plugin_port->data)->port_index;
	  }
	  
	  play_lv2_audio->output_lines += 1;

	  pthread_mutex_unlock(recall_mutex);
	}
      }

      /* iterate */
      plugin_port = plugin_port->next;
    }
    

    start_port = g_list_reverse(start_port);
    AGS_RECALL(play_lv2_audio)->port = g_list_copy(start_port);
  }
  
  /* free */
  g_free(filename);
  g_free(effect);

  g_free(plugin_name);

#ifdef AGS_DEBUG
  g_message("output lines: %d", play_lv2_audio->output_lines);
#endif
  
  return(start_port);
}

/**
 * ags_play_lv2_audio_load_conversion:
 * @play_lv2_audio: the #AgsPlayLv2Audio
 * @port: the #AgsPort
 * @plugin_port: the #AgsPluginPort
 * 
 * Loads conversion object by using @plugin_port and sets in on @port.
 * 
 * Since: 2.0.0
 */
void
ags_play_lv2_audio_load_conversion(AgsPlayLv2Audio *play_lv2_audio,
				   GObject *port,
				   GObject *plugin_port)
{
  AgsLv2Conversion *lv2_conversion;

  if(!AGS_IS_PLAY_LV2_AUDIO(play_lv2_audio) ||
     !AGS_IS_PORT(port) ||
     !AGS_PLUGIN_PORT(plugin_port)){
    return;
  }

  lv2_conversion = NULL;
  
  if(ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_LOGARITHMIC)){
    lv2_conversion = ags_lv2_conversion_new();
    lv2_conversion->flags |= AGS_LV2_CONVERSION_LOGARITHMIC;

    g_object_set(port,
		 "conversion", lv2_conversion,
		 NULL);    
  }
}

/**
 * ags_play_lv2_audio_find:
 * @recall: a #GList-struct containing #AgsRecall
 * @filename: plugin's filename
 * @uri: plugin's uri
 *
 * Retrieve LV2 recall.
 *
 * Returns: Next match.
 * 
 * Since: 2.0.0
 */
GList*
ags_play_lv2_audio_find(GList *recall,
			gchar *filename, gchar *uri)
{
  while(recall != NULL){
    if(AGS_IS_PLAY_LV2_AUDIO(recall->data)){
      gboolean success;
      
      pthread_mutex_t *recall_mutex;

      /* get recall mutex */
      pthread_mutex_lock(ags_recall_get_class_mutex());
  
      recall_mutex = AGS_RECALL(recall->data)->obj_mutex;
  
      pthread_mutex_unlock(ags_recall_get_class_mutex());

      /* check current filename and effect */
      pthread_mutex_lock(recall_mutex);

      success = (!g_strcmp0(AGS_RECALL(recall->data)->filename,
			    filename) &&
		 !g_strcmp0(AGS_PLAY_LV2_AUDIO(recall->data)->uri,
			    uri)) ? TRUE: FALSE;
      
      pthread_mutex_unlock(recall_mutex);

      if(success){
	break;
      }
    }
    
    /* iterate */
    recall = recall->next;
  }

  return(recall);
}

/**
 * ags_play_lv2_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsPlayLv2Audio
 *
 * Returns: the new #AgsPlayLv2Audio
 *
 * Since: 2.0.0
 */
AgsPlayLv2Audio*
ags_play_lv2_audio_new(AgsAudio *audio)
{
  AgsPlayLv2Audio *play_lv2_audio;

  play_lv2_audio = (AgsPlayLv2Audio *) g_object_new(AGS_TYPE_PLAY_LV2_AUDIO,
						    "audio", audio,
						    NULL);

  return(play_lv2_audio);
}
