/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/machine/ags_ladspa_bridge.h>
#include <ags/X/machine/ags_ladspa_bridge_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bulk.h>
#include <ags/X/ags_bulk_member.h>

#include <ags/i18n.h>

void ags_ladspa_bridge_class_init(AgsLadspaBridgeClass *ladspa_bridge);
void ags_ladspa_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ladspa_bridge_plugin_interface_init(AgsPluginInterface *plugin);
void ags_ladspa_bridge_init(AgsLadspaBridge *ladspa_bridge);
void ags_ladspa_bridge_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_ladspa_bridge_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_ladspa_bridge_connect(AgsConnectable *connectable);
void ags_ladspa_bridge_disconnect(AgsConnectable *connectable);
void ags_ladspa_bridge_finalize(GObject *gobject);
gchar* ags_ladspa_bridge_get_version(AgsPlugin *plugin);
void ags_ladspa_bridge_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_ladspa_bridge_get_build_id(AgsPlugin *plugin);
void ags_ladspa_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id);
gchar* ags_ladspa_bridge_get_xml_type(AgsPlugin *plugin);
void ags_ladspa_bridge_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_ladspa_bridge_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
void ags_ladspa_bridge_launch_task(AgsFileLaunch *file_launch, AgsLadspaBridge *ladspa_bridge);
xmlNode* ags_ladspa_bridge_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

/**
 * SECTION:ags_ladspa_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsLadspaBridge
 * @section_id:
 * @include: ags/X/machine/ags_ladspa_bridge.h
 *
 * #AgsLadspaBridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_INDEX,
};

static gpointer ags_ladspa_bridge_parent_class = NULL;
static AgsConnectableInterface* ags_ladspa_bridge_parent_connectable_interface;
static AgsPluginInterface* ags_ladspa_bridge_parent_plugin_interface;

GType
ags_ladspa_bridge_get_type(void)
{
  static GType ags_type_ladspa_bridge = 0;

  if(!ags_type_ladspa_bridge){
    static const GTypeInfo ags_ladspa_bridge_info = {
      sizeof(AgsLadspaBridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ladspa_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLadspaBridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ladspa_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ladspa_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_ladspa_bridge_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ladspa_bridge = g_type_register_static(AGS_TYPE_MACHINE,
						    "AgsLadspaBridge", &ags_ladspa_bridge_info,
						    0);

    g_type_add_interface_static(ags_type_ladspa_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ladspa_bridge,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_ladspa_bridge);
}

void
ags_ladspa_bridge_class_init(AgsLadspaBridgeClass *ladspa_bridge)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_ladspa_bridge_parent_class = g_type_class_peek_parent(ladspa_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(ladspa_bridge);

  gobject->set_property = ags_ladspa_bridge_set_property;
  gobject->get_property = ags_ladspa_bridge_get_property;

  gobject->finalize = ags_ladspa_bridge_finalize;
  
  /* properties */
  /**
   * AgsRecallLadspa:filename:
   *
   * The plugins filename.
   * 
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_string("filename",
				    i18n_pspec("the object file"),
				    i18n_pspec("The filename as string of object file"),
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsRecallLadspa:effect:
   *
   * The effect's name.
   * 
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_string("effect",
				    i18n_pspec("the effect"),
				    i18n_pspec("The effect's string representation"),
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT,
				  param_spec);

  /**
   * AgsRecallLadspa:index:
   *
   * The effect's index.
   * 
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_ulong("index",
				   i18n_pspec("index of effect"),
				   i18n_pspec("The numerical index of effect"),
				   0,
				   65535,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INDEX,
				  param_spec);
}

void
ags_ladspa_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_ladspa_bridge_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_ladspa_bridge_connect;
  connectable->disconnect = ags_ladspa_bridge_disconnect;
}

void
ags_ladspa_bridge_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_ladspa_bridge_get_version;
  plugin->set_version = ags_ladspa_bridge_set_version;
  plugin->get_build_id = ags_ladspa_bridge_get_build_id;
  plugin->set_build_id = ags_ladspa_bridge_set_build_id;
  plugin->get_xml_type = ags_ladspa_bridge_get_xml_type;
  plugin->set_xml_type = ags_ladspa_bridge_set_xml_type;
  plugin->read = ags_ladspa_bridge_read;
  plugin->write = ags_ladspa_bridge_write;
  plugin->get_ports = NULL;
  plugin->set_ports = NULL;
}

void
ags_ladspa_bridge_init(AgsLadspaBridge *ladspa_bridge)
{
  GtkTable *table;

  AgsAudio *audio;

  g_signal_connect_after((GObject *) ladspa_bridge, "parent-set",
			 G_CALLBACK(ags_ladspa_bridge_parent_set_callback), (gpointer) ladspa_bridge);

  audio = AGS_MACHINE(ladspa_bridge)->audio;
  audio->flags |= (AGS_AUDIO_SYNC);
  
  ladspa_bridge->flags = 0;

  ladspa_bridge->name = NULL;

  ladspa_bridge->version = AGS_LADSPA_BRIDGE_DEFAULT_VERSION;
  ladspa_bridge->build_id = AGS_LADSPA_BRIDGE_DEFAULT_BUILD_ID;

  ladspa_bridge->xml_type = "ags-ladspa-bridge";
  
  ladspa_bridge->mapped_output = 0;
  ladspa_bridge->mapped_input = 0;
  
  ladspa_bridge->filename = NULL;
  ladspa_bridge->effect = NULL;
  ladspa_bridge->effect_index = 0;

  AGS_MACHINE(ladspa_bridge)->bridge = (GtkContainer *) ags_effect_bridge_new(audio);
  gtk_container_add((GtkContainer *) gtk_bin_get_child((GtkBin *) ladspa_bridge),
		    (GtkWidget *) AGS_MACHINE(ladspa_bridge)->bridge);

  table = (GtkTable *) gtk_table_new(1, 2, FALSE);
  gtk_box_pack_start((GtkBox *) AGS_EFFECT_BRIDGE(AGS_MACHINE(ladspa_bridge)->bridge),
		     (GtkWidget *) table,
		     FALSE, FALSE,
		     0);

  AGS_EFFECT_BRIDGE(AGS_MACHINE(ladspa_bridge)->bridge)->bulk_input = (GtkWidget *) ags_effect_bulk_new(audio,
													AGS_TYPE_INPUT);
  AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(ladspa_bridge)->bridge)->bulk_input)->flags |= (AGS_EFFECT_BULK_HIDE_BUTTONS |
												AGS_EFFECT_BULK_HIDE_ENTRIES |
												AGS_EFFECT_BULK_SHOW_LABELS);
  gtk_table_attach(table,
		   (GtkWidget *) AGS_EFFECT_BRIDGE(AGS_MACHINE(ladspa_bridge)->bridge)->bulk_input,
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_ladspa_bridge_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsLadspaBridge *ladspa_bridge;

  ladspa_bridge = AGS_LADSPA_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == ladspa_bridge->filename){
	return;
      }

      if(ladspa_bridge->filename != NULL){
	g_free(ladspa_bridge->filename);
      }

      if(filename != NULL){
	if(!g_file_test(filename,
			G_FILE_TEST_EXISTS)){
	  AgsWindow *window;

	  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) ladspa_bridge);

	  ags_window_show_error(window,
				g_strdup_printf("Plugin file not present %s",
						filename));
	}
      }
      
      ladspa_bridge->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;
      
      effect = g_value_get_string(value);

      if(effect == ladspa_bridge->effect){
	return;
      }

      if(ladspa_bridge->effect != NULL){
	g_free(ladspa_bridge->effect);
      }

      ladspa_bridge->effect = g_strdup(effect);
    }
    break;
  case PROP_INDEX:
    {
      unsigned long effect_index;
      
      effect_index = g_value_get_ulong(value);

      if(effect_index == ladspa_bridge->effect_index){
	return;
      }

      ladspa_bridge->effect_index = effect_index;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ladspa_bridge_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsLadspaBridge *ladspa_bridge;

  ladspa_bridge = AGS_LADSPA_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, ladspa_bridge->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, ladspa_bridge->effect);
    }
    break;
  case PROP_INDEX:
    {
      g_value_set_ulong(value, ladspa_bridge->effect_index);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ladspa_bridge_connect(AgsConnectable *connectable)
{
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_ladspa_bridge_parent_connectable_interface->connect(connectable);
}

void
ags_ladspa_bridge_disconnect(AgsConnectable *connectable)
{
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_ladspa_bridge_parent_connectable_interface->disconnect(connectable);
}

void
ags_ladspa_bridge_finalize(GObject *gobject)
{
  AgsLadspaBridge *ladspa_bridge;

  ladspa_bridge = (AgsLadspaBridge *) gobject;
  
  g_free(ladspa_bridge->filename);
  g_free(ladspa_bridge->effect);

  /* call parent */
  G_OBJECT_CLASS(ags_ladspa_bridge_parent_class)->finalize(gobject);
}

gchar*
ags_ladspa_bridge_get_version(AgsPlugin *plugin)
{
  return(AGS_LADSPA_BRIDGE(plugin)->version);
}

void
ags_ladspa_bridge_set_version(AgsPlugin *plugin, gchar *version)
{
  AgsLadspaBridge *ladspa_bridge;

  ladspa_bridge = AGS_LADSPA_BRIDGE(plugin);

  ladspa_bridge->version = version;
}

gchar*
ags_ladspa_bridge_get_build_id(AgsPlugin *plugin)
{
  return(AGS_LADSPA_BRIDGE(plugin)->build_id);
}

void
ags_ladspa_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsLadspaBridge *ladspa_bridge;

  ladspa_bridge = AGS_LADSPA_BRIDGE(plugin);

  ladspa_bridge->build_id = build_id;
}


gchar*
ags_ladspa_bridge_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_LADSPA_BRIDGE(plugin)->xml_type);
}

void
ags_ladspa_bridge_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_LADSPA_BRIDGE(plugin)->xml_type = xml_type;
}

void
ags_ladspa_bridge_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsLadspaBridge *gobject;
  AgsFileLaunch *file_launch;

  gobject = AGS_LADSPA_BRIDGE(plugin);

  g_object_set(gobject,
	       "filename", xmlGetProp(node,
					"filename"),
	       "effect", xmlGetProp(node,
				      "effect"),
	       NULL);

  /* launch */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "node", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start",
		   G_CALLBACK(ags_ladspa_bridge_launch_task), gobject);
  ags_file_add_launch(file,
		      G_OBJECT(file_launch));
}

void
ags_ladspa_bridge_launch_task(AgsFileLaunch *file_launch, AgsLadspaBridge *ladspa_bridge)
{
  GtkTreeModel *model;

  GtkTreeIter iter;

  GList *list, *list_start;
  GList *recall;

  ags_ladspa_bridge_load(ladspa_bridge);

  /* block update bulk port */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(ladspa_bridge)->bridge)->bulk_input)->table);

  while(list != NULL){
    if(AGS_IS_BULK_MEMBER(list->data)){
      AGS_BULK_MEMBER(list->data)->flags |= AGS_BULK_MEMBER_NO_UPDATE;
    }

    list = list->next;
  }

  /* update value and unblock update bulk port */
  recall = NULL;
  
  if(AGS_MACHINE(ladspa_bridge)->audio->input != NULL){
    recall = AGS_MACHINE(ladspa_bridge)->audio->input->recall;
    
    while((recall = ags_recall_template_find_type(recall, AGS_TYPE_RECALL_LADSPA)) != NULL){
      if(!g_strcmp0(AGS_RECALL_LADSPA(recall->data)->filename,
		  ladspa_bridge->filename) &&
	 !g_strcmp0(AGS_RECALL_LADSPA(recall->data)->effect,
		    ladspa_bridge->effect)){
	break;
      }

      recall = recall->next;
    }
  }

  while(list != NULL){
    if(AGS_IS_BULK_MEMBER(list->data)){
      GtkWidget *child_widget;
      
      GList *port;

      child_widget = gtk_bin_get_child(list->data);
      
      if(recall != NULL){
	port = AGS_RECALL(recall->data)->port;

	while(port != port->next){
	  if(!g_strcmp0(AGS_BULK_MEMBER(list->data)->specifier,
			AGS_PORT(port->data)->specifier)){
	    if(AGS_IS_DIAL(child_widget)){
	      gtk_adjustment_set_value(AGS_DIAL(child_widget)->adjustment,
				       AGS_PORT(port->data)->port_value.ags_port_ladspa);
	      ags_dial_draw((AgsDial *) child_widget);
	    }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
	      gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
					   ((AGS_PORT(port->data)->port_value.ags_port_ladspa != 0.0) ? TRUE: FALSE));
	    }

	    break;
	  }

	  port = port->next;
	}
      }
     
      AGS_BULK_MEMBER(list->data)->flags &= (~AGS_BULK_MEMBER_NO_UPDATE);
    }
    
    list = list->next;
  }

  g_list_free(list_start);
}

xmlNode*
ags_ladspa_bridge_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsLadspaBridge *ladspa_bridge;

  xmlNode *node;

  gchar *id;
  
  ladspa_bridge = AGS_LADSPA_BRIDGE(plugin);

  id = ags_id_generator_create_uuid();
    
  node = xmlNewNode(NULL,
		    "ags-ladspa-bridge");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     "filename",
	     ladspa_bridge->filename);

  xmlNewProp(node,
	     "effect",
	     ladspa_bridge->effect);
  
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", ladspa_bridge,
				   NULL));

  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_ladspa_bridge_load(AgsLadspaBridge *ladspa_bridge)
{
  /* empty */
#ifdef AGS_DEBUG
  g_message("%s %s",ladspa_bridge->filename, ladspa_bridge->effect);
#endif
  
  ags_effect_bulk_add_effect((AgsEffectBulk *) AGS_EFFECT_BRIDGE(AGS_MACHINE(ladspa_bridge)->bridge)->bulk_input,
			     NULL,
			     ladspa_bridge->filename,
			     ladspa_bridge->effect);
}

/**
 * ags_ladspa_bridge_new:
 * @soundcard: the assigned soundcard.
 * @filename: the plugin.so
 * @effect: the effect
 *
 * Creates an #AgsLadspaBridge
 *
 * Returns: a new #AgsLadspaBridge
 *
 * Since: 1.0.0
 */
AgsLadspaBridge*
ags_ladspa_bridge_new(GObject *soundcard,
		      gchar *filename,
		      gchar *effect)
{
  AgsLadspaBridge *ladspa_bridge;
  GValue value = {0,};

  ladspa_bridge = (AgsLadspaBridge *) g_object_new(AGS_TYPE_LADSPA_BRIDGE,
						   NULL);

  if(soundcard != NULL){
    g_value_init(&value, G_TYPE_OBJECT);
    g_value_set_object(&value, soundcard);
    g_object_set_property(G_OBJECT(AGS_MACHINE(ladspa_bridge)->audio),
			  "soundcard", &value);
    g_value_unset(&value);
  }

  g_object_set(ladspa_bridge,
	       "filename", filename,
	       "effect", effect,
	       NULL);

  return(ladspa_bridge);
}
