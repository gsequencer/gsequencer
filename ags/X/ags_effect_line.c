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

#include <ags/X/ags_effect_line.h>
#include <ags/X/ags_effect_line_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_marshal.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_ladspa_plugin.h>
#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_ladspa_conversion.h>
#include <ags/plugin/ags_lv2_conversion.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 
#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_recall_lv2.h>

#include <ags/widget/ags_led.h>
#include <ags/widget/ags_vindicator.h>
#include <ags/widget/ags_hindicator.h>
#include <ags/widget/ags_dial.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_effect_pad.h>
#include <ags/X/ags_line_member.h>
#include <ags/X/ags_effect_separator.h>

#include <ags/X/task/ags_add_line_member.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>

void ags_effect_line_class_init(AgsEffectLineClass *effect_line);
void ags_effect_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_effect_line_plugin_interface_init(AgsPluginInterface *plugin);
void ags_effect_line_init(AgsEffectLine *effect_line);
void ags_effect_line_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_effect_line_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_effect_line_connect(AgsConnectable *connectable);
void ags_effect_line_disconnect(AgsConnectable *connectable);
gchar* ags_effect_line_get_name(AgsPlugin *plugin);
void ags_effect_line_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_effect_line_get_version(AgsPlugin *plugin);
void ags_effect_line_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_effect_line_get_build_id(AgsPlugin *plugin);
void ags_effect_line_set_build_id(AgsPlugin *plugin, gchar *build_id);
void ags_effect_line_finalize(GObject *gobject);

GList* ags_effect_line_add_ladspa_effect(AgsEffectLine *effect_line,
					 GList *control_type_name,
					 gchar *filename,
					 gchar *effect);
GList* ags_effect_line_add_lv2_effect(AgsEffectLine *effect_line,
				      GList *control_type_name,
				      gchar *filename,
				      gchar *effect);
GList* ags_effect_line_real_add_effect(AgsEffectLine *effect_line,
				       GList *control_type_name,
				       gchar *filename,
				       gchar *effect);
void ags_effect_line_real_remove_effect(AgsEffectLine *effect_line,
					guint nth);
void ags_effect_line_real_map_recall(AgsEffectLine *effect_line,
				     guint output_pad_start);
GList* ags_effect_line_real_find_port(AgsEffectLine *effect_line);

/**
 * SECTION:ags_effect_line
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsEffectLine
 * @section_id:
 * @include: ags/X/ags_effect_line.h
 *
 * #AgsEffectLine is a composite widget to visualize one #AgsChannel. It should be
 * packed by an #AgsEffectLine.
 */

enum{
  ADD_EFFECT,
  REMOVE_EFFECT,
  MAP_RECALL,
  FIND_PORT,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_CHANNEL,
};

static gpointer ags_effect_line_parent_class = NULL;
static guint effect_line_signals[LAST_SIGNAL];

GHashTable *ags_effect_line_indicator_queue_draw = NULL;

GType
ags_effect_line_get_type(void)
{
  static GType ags_type_effect_line = 0;

  if(!ags_type_effect_line){
    static const GTypeInfo ags_effect_line_info = {
      sizeof(AgsEffectLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_effect_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsEffectLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_effect_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_effect_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_effect_line_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_effect_line = g_type_register_static(GTK_TYPE_VBOX,
						  "AgsEffectLine", &ags_effect_line_info,
						  0);

    g_type_add_interface_static(ags_type_effect_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_effect_line,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_effect_line);
}

void
ags_effect_line_class_init(AgsEffectLineClass *effect_line)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_effect_line_parent_class = g_type_class_peek_parent(effect_line);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(effect_line);

  gobject->set_property = ags_effect_line_set_property;
  gobject->get_property = ags_effect_line_get_property;

  gobject->finalize = ags_effect_line_finalize;
  
  /* properties */
  /**
   * AgsEffectLine:channel:
   *
   * The start of a bunch of #AgsChannel to visualize.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("channel",
				   "assigned channel",
				   "The channel it is assigned with",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /* AgsEffectLineClass */
  effect_line->add_effect = ags_effect_line_real_add_effect;
  effect_line->remove_effect = ags_effect_line_real_remove_effect;
  effect_line->map_recall = ags_effect_line_real_map_recall;
  effect_line->find_port = ags_effect_line_real_find_port;

  /* signals */
  /**
   * AgsEffectLine::add-effect:
   * @effect_line: the #AgsEffectLine to modify
   * @control_type_name: the string representation of a #GType
   * @filename: the effect's filename
   * @effect: the effect's name
   *
   * The ::add-effect signal notifies about added effect.
   *
   * Returns: the #GList-struct containing the #AgsPort objects added
   *
   * Since: 0.7.42
   */
  effect_line_signals[ADD_EFFECT] =
    g_signal_new("add-effect",
		 G_TYPE_FROM_CLASS(effect_line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectLineClass, add_effect),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__POINTER_STRING_STRING,
		 G_TYPE_POINTER, 3,
		 G_TYPE_POINTER,
		 G_TYPE_STRING,
		 G_TYPE_STRING);

  /**
   * AgsEffectLine::remove-effect:
   * @effect_line: the #AgsEffectLine to modify
   * @nth: the nth effect
   *
   * The ::remove-effect signal notifies about removed effect.
   *
   * Since: 0.7.42
   */
  effect_line_signals[REMOVE_EFFECT] =
    g_signal_new("remove-effect",
		 G_TYPE_FROM_CLASS(effect_line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectLineClass, remove_effect),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsEffectLine::map-recall:
   * @effect_line: the #AgsEffectLine
   * @output_pad_start: the channel's start pad
   *
   * The ::map-recall should be used to add the effect_line's default recall. This function
   * may call ags_effect_line_find_port().
   *
   * Since: 0.7.42
   */
  effect_line_signals[MAP_RECALL] =
    g_signal_new("map-recall",
                 G_TYPE_FROM_CLASS (effect_line),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsEffectLineClass, map_recall),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__UINT,
                 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsEffectLine::find-port:
   * @effect_line: the #AgsEffectLine to resize
   * Returns: a #GList with associated ports
   *
   * The ::find-port as recall should be mapped
   *
   * Returns: an #GList containing all related #AgsPort
   *
   * Since: 0.7.8
   */
  effect_line_signals[FIND_PORT] =
    g_signal_new("find-port",
		 G_TYPE_FROM_CLASS(effect_line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectLineClass, find_port),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);
}

void
ags_effect_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_effect_line_connect;
  connectable->disconnect = ags_effect_line_disconnect;
}

void
ags_effect_line_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_effect_line_get_version;
  plugin->set_version = ags_effect_line_set_version;
  plugin->get_build_id = ags_effect_line_get_build_id;
  plugin->set_build_id = ags_effect_line_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_effect_line_init(AgsEffectLine *effect_line)
{
  if(ags_effect_line_indicator_queue_draw == NULL){
    ags_effect_line_indicator_queue_draw = g_hash_table_new_full(g_direct_hash, g_direct_equal,
								 NULL,
								 NULL);
  }

  effect_line->flags = 0;

  effect_line->name = NULL;
  
  effect_line->version = AGS_EFFECT_LINE_DEFAULT_VERSION;
  effect_line->build_id = AGS_EFFECT_LINE_DEFAULT_BUILD_ID;

  effect_line->channel = NULL;

  effect_line->label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
						 NULL);
  gtk_box_pack_start(GTK_BOX(effect_line),
		     GTK_WIDGET(effect_line->label),
		     FALSE, FALSE,
		     0);

  effect_line->table = (GtkTable *) gtk_table_new(1, AGS_EFFECT_LINE_COLUMNS_COUNT,
						  TRUE);
  gtk_box_pack_start(GTK_BOX(effect_line),
		     GTK_WIDGET(effect_line->table),
		     FALSE, FALSE,
		     0);

  effect_line->queued_drawing = NULL;
}

void
ags_effect_line_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsEffectLine *effect_line;

  effect_line = AGS_EFFECT_LINE(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(effect_line->channel == channel){
	return;
      }

      if(effect_line->channel != NULL){
	g_object_unref(effect_line->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);

	g_signal_connect_after(channel, "add-effect",
			       G_CALLBACK(ags_effect_line_add_effect_callback), effect_line);
	
	//	g_signal_connect_after(channel, "remove-effect",
	//		       G_CALLBACK(ags_effect_line_remove_effect_callback), effect_line);
      }

      effect_line->channel = channel;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_line_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsEffectLine *effect_line;

  effect_line = AGS_EFFECT_LINE(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value,
			 effect_line->channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_line_connect(AgsConnectable *connectable)
{
  AgsEffectLine *effect_line;
  GList *list, *list_start;

  effect_line = AGS_EFFECT_LINE(connectable);

  if((AGS_EFFECT_LINE_CONNECTED & (effect_line->flags)) == 0){
    return;
  }

  effect_line->flags &= (~AGS_EFFECT_LINE_CONNECTED);

  /* connect line members */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(effect_line->table));
  
  while(list != NULL){
    if(AGS_IS_CONNECTABLE(list->data)){
      ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    }

    list = list->next;
  }

  if(list_start != NULL){
    g_list_free(list_start);
  }
}

void
ags_effect_line_disconnect(AgsConnectable *connectable)
{
  AgsEffectLine *effect_line;
  GList *list, *list_start;

  effect_line = AGS_EFFECT_LINE(connectable);

  if((AGS_EFFECT_LINE_CONNECTED & (effect_line->flags)) != 0){
    return;
  }

  effect_line->flags |= AGS_EFFECT_LINE_CONNECTED;

  if((AGS_EFFECT_LINE_PREMAPPED_RECALL & (effect_line->flags)) == 0){
    if((AGS_EFFECT_LINE_MAPPED_RECALL & (effect_line->flags)) == 0){
      ags_effect_line_map_recall(effect_line,
				 0);
    }
  }else{
    ags_effect_line_find_port(effect_line);
  }

  /* connect line members */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(effect_line->table));
  
  while(list != NULL){
    if(AGS_IS_CONNECTABLE(list->data)){
      ags_connectable_connect(AGS_CONNECTABLE(list->data));
    }

    list = list->next;
  }

  if(list_start != NULL){
    g_list_free(list_start);
  }

  g_signal_handlers_disconnect_by_data(effect_line->channel,
				       effect_line);
}

gchar*
ags_effect_line_get_name(AgsPlugin *plugin)
{
  return(AGS_EFFECT_LINE(plugin)->name);
}

void
ags_effect_line_set_name(AgsPlugin *plugin, gchar *name)
{
  AgsEffectLine *effect_line;

  effect_line = AGS_EFFECT_LINE(plugin);

  effect_line->name = name;
}

gchar*
ags_effect_line_get_version(AgsPlugin *plugin)
{
  return(AGS_EFFECT_LINE(plugin)->version);
}

void
ags_effect_line_set_version(AgsPlugin *plugin, gchar *version)
{
  AgsEffectLine *effect_line;

  effect_line = AGS_EFFECT_LINE(plugin);

  effect_line->version = version;
}

gchar*
ags_effect_line_get_build_id(AgsPlugin *plugin)
{
  return(AGS_EFFECT_LINE(plugin)->build_id);
}

void
ags_effect_line_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsEffectLine *effect_line;

  effect_line = AGS_EFFECT_LINE(plugin);

  effect_line->build_id = build_id;
}

void
ags_effect_line_finalize(GObject *gobject)
{
  AgsEffectLine *effect_line;
  GList *list;

  effect_line = AGS_EFFECT_LINE(gobject);
  
  /* remove of the queued drawing hash */
  list = effect_line->queued_drawing;

  while(list != NULL){
    g_hash_table_remove(ags_effect_line_indicator_queue_draw,
			list->data);

    list = list->next;
  }
}

GList*
ags_effect_line_add_ladspa_effect(AgsEffectLine *effect_line,
				  GList *control_type_name,
				  gchar *filename,
				  gchar *effect)
{
  AgsLineMember *line_member;
  AgsEffectSeparator *separator;
  GtkAdjustment *adjustment;
  
  AgsAddLineMember *add_line_member;

  AgsRecallHandler *recall_handler;

  AgsLadspaPlugin *ladspa_plugin;

  AgsMutexManager *mutex_manager;
  
  GList *list;
  GList *recall, *recall_start;
  GList *port, *recall_port;
  GList *port_descriptor;
  
  gdouble step;
  guint port_count;
  gboolean has_output_port;

  guint x, y;
  guint k;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* load plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_ladspa_manager_get_instance(),
							filename, effect);

  /* retrieve position within table  */
  x = 0;
  y = 0;
  
  list = effect_line->table->children;

  while(list != NULL){
    if(y <= ((GtkTableChild *) list->data)->top_attach){
      y = ((GtkTableChild *) list->data)->top_attach + 1;
    }

    list = list->next;
  }

  /* get channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) effect_line->channel);
  
  pthread_mutex_unlock(application_mutex);
  
  /* play - find ports */
  pthread_mutex_lock(channel_mutex);

  recall_start =
    recall = ags_recall_get_by_effect(effect_line->channel->play,
				      filename,
				      effect);

  if(recall == NULL){
    pthread_mutex_unlock(channel_mutex);
    
    return(NULL);
  }

  /* check has output port */
  if((AGS_RECALL_HAS_OUTPUT_PORT & (AGS_RECALL(recall->data)->flags)) != 0){
    has_output_port = TRUE;
  }else{
    has_output_port = FALSE;
  }

  /* recall handler of output port */
  if(has_output_port){
    AgsRecall *recall_channel_run_dummy;

    recall_channel_run_dummy = ags_recall_find_template(AGS_RECALL_CONTAINER(AGS_RECALL(recall->data)->container)->recall_channel_run)->data;
    
    /* alloc handler */
    recall_handler = (AgsRecallHandler *) malloc(sizeof(AgsRecallHandler));

    recall_handler->signal_name = "run-post";
    recall_handler->callback = G_CALLBACK(ags_effect_line_output_port_run_post_callback);
    recall_handler->data = (gpointer) effect_line;

    ags_recall_add_handler(AGS_RECALL(recall_channel_run_dummy), recall_handler);
  }
  
  /* recall - find ports */
  recall = g_list_last(recall);
  port = AGS_RECALL(recall->data)->port;

  g_list_free(recall_start);

  recall_start = 
    recall = ags_recall_get_by_effect(effect_line->channel->recall,
				      filename,
				      effect);
  recall = g_list_last(recall);

  recall_port = AGS_RECALL(recall->data)->port;

  /* recall handler of output port */
  if(has_output_port){
    AgsRecall *recall_channel_run_dummy;

    recall_channel_run_dummy = ags_recall_find_template(AGS_RECALL_CONTAINER(AGS_RECALL(recall->data)->container)->recall_channel_run)->data;
    
    /* alloc handler */
    recall_handler = (AgsRecallHandler *) malloc(sizeof(AgsRecallHandler));

    recall_handler->signal_name = "run-post";
    recall_handler->callback = G_CALLBACK(ags_effect_line_output_port_run_post_callback);
    recall_handler->data = (gpointer) effect_line;

    ags_recall_add_handler(AGS_RECALL(recall_channel_run_dummy), recall_handler);
  }

  g_list_free(recall_start);
  
  pthread_mutex_unlock(channel_mutex);

  /* add separator */
  separator = ags_effect_separator_new();
  g_object_set(separator,
	       "text", effect,
	       "filename", filename,
	       "effect", effect,
	       NULL);
  
  gtk_table_attach(effect_line->table,
		   (GtkWidget *) separator,
		   0, AGS_EFFECT_LINE_COLUMNS_COUNT,
		   y, y + 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  gtk_widget_show_all(separator);

  y++;

  /* load ports */
  port_descriptor = AGS_BASE_PLUGIN(ladspa_plugin)->port;

  port_count = g_list_length(port_descriptor);
  k = 0;

  while(port_descriptor != NULL){
    if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
      GtkWidget *child_widget;

      AgsLadspaConversion *ladspa_conversion;

      GType widget_type;

      guint step_count;
      gboolean disable_seemless;

      disable_seemless = FALSE;

      if(x == AGS_EFFECT_LINE_COLUMNS_COUNT){
	x = 0;
	y++;
	gtk_table_resize(effect_line->table,
			 y + 1, AGS_EFFECT_LINE_COLUMNS_COUNT);
      }
      
      if((AGS_PORT_DESCRIPTOR_TOGGLED & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	disable_seemless = TRUE;

	if((AGS_PORT_DESCRIPTOR_OUTPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  widget_type = AGS_TYPE_LED;
	}else{
	  widget_type = GTK_TYPE_TOGGLE_BUTTON;
	}
      }else{
	if((AGS_PORT_DESCRIPTOR_OUTPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  widget_type = AGS_TYPE_HINDICATOR;
	}else{
	  widget_type = AGS_TYPE_DIAL;
	}
      }

      if(control_type_name != NULL){
	widget_type = g_type_from_name(control_type_name->data);

	control_type_name = control_type_name->next;
      }
      
      step_count = AGS_DIAL_DEFAULT_PRECISION;

      if((AGS_PORT_DESCRIPTOR_INTEGER & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	step_count = AGS_PORT_DESCRIPTOR(port_descriptor->data)->scale_steps;

	disable_seemless = TRUE;
      }
      
      /* add line member */
      line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
						   "widget-type", widget_type,
						   "widget-label", AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name,
						   "plugin-name", g_strdup_printf("ladspa-%u", ladspa_plugin->unique_id),
						   "filename", filename,
						   "effect", effect,
						   "specifier", g_strdup(AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name),
						   "control-port", g_strdup_printf("%u/%u",
										     k,
										     port_count),
						   "steps", step_count,
						   NULL);
      child_widget = ags_line_member_get_widget(line_member);

      /* ladspa conversion */
      ladspa_conversion = NULL;

      if((AGS_PORT_DESCRIPTOR_BOUNDED_BELOW & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_BELOW;
      }

      if((AGS_PORT_DESCRIPTOR_BOUNDED_ABOVE & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_ABOVE;
      }
      if((AGS_PORT_DESCRIPTOR_SAMPLERATE & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_SAMPLERATE;
      }

      if((AGS_PORT_DESCRIPTOR_LOGARITHMIC & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}
    
	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_LOGARITHMIC;
      }

      line_member->conversion = (AgsConversion *) ladspa_conversion;

      /* child widget */
      if((AGS_PORT_DESCRIPTOR_TOGGLED & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	line_member->port_flags = AGS_LINE_MEMBER_PORT_BOOLEAN;
      }
      
      if((AGS_PORT_DESCRIPTOR_INTEGER & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	line_member->port_flags = AGS_LINE_MEMBER_PORT_INTEGER;
      }
      
      if(AGS_IS_DIAL(child_widget)){
	AgsDial *dial;
	GtkAdjustment *adjustment;
	float lower_bound, upper_bound;
	
	dial = (AgsDial *) child_widget;

	if(disable_seemless){
	  dial->flags &= (~AGS_DIAL_SEEMLESS_MODE);
	}

	/* add controls of ports and apply range  */
	lower_bound = g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->upper_value);

	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);

	if(upper_bound >= 0.0 && lower_bound >= 0.0){
	  step = (upper_bound - lower_bound) / step_count;
	}else if(upper_bound < 0.0 && lower_bound < 0.0){
	  step = -1.0 * (lower_bound - upper_bound) / step_count;
	}else{
	  step = (upper_bound - lower_bound) / step_count;
	}

	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_lower(adjustment,
				 lower_bound);
	gtk_adjustment_set_upper(adjustment,
				 upper_bound);
	gtk_adjustment_set_value(adjustment,
				 g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->default_value));
      }else if(AGS_IS_INDICATOR(child_widget) ||
	       AGS_IS_LED(child_widget)){
	g_hash_table_insert(ags_effect_line_indicator_queue_draw,
			    child_widget, ags_effect_line_indicator_queue_draw_timeout);
	effect_line->queued_drawing = g_list_prepend(effect_line->queued_drawing,
						     child_widget);
	g_timeout_add(1000 / 30, (GSourceFunc) ags_effect_line_indicator_queue_draw_timeout, (gpointer) child_widget);
      }

#ifdef AGS_DEBUG
      g_message("ladspa bounds: %f %f", lower_bound, upper_bound);
#endif
	  
      gtk_table_attach(effect_line->table,
		       (GtkWidget *) line_member,
		       (x % AGS_EFFECT_LINE_COLUMNS_COUNT), (x % AGS_EFFECT_LINE_COLUMNS_COUNT) + 1,
		       y, y + 1,
		       GTK_FILL, GTK_FILL,
		       0, 0);

      ags_connectable_connect(AGS_CONNECTABLE(line_member));
      gtk_widget_show_all((GtkWidget *) line_member);
      
      port = port->next;
      x++;

      if(x % AGS_EFFECT_LINE_COLUMNS_COUNT == 0){
	y++;
      }
    }
    
    port_descriptor = port_descriptor->next;
    k++;
  }
  
  return(g_list_concat(g_list_copy(port),
		       g_list_copy(recall_port)));
}

GList*
ags_effect_line_add_lv2_effect(AgsEffectLine *effect_line,
			       GList *control_type_name,
			       gchar *filename,
			       gchar *effect)
{
  AgsLineMember *line_member;
  AgsEffectSeparator *separator;
  GtkAdjustment *adjustment;

  AgsAddLineMember *add_line_member;

  AgsRecallHandler *recall_handler;

  AgsLv2Plugin *lv2_plugin;

  AgsMutexManager *mutex_manager;

  GList *list;
  GList *recall, *recall_start;
  GList *port, *recall_port;
  GList *port_descriptor;
  
  gdouble step;
  guint port_count;
  gboolean has_output_port;

  guint x, y;
  guint k;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* load plugin */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       filename, effect);

  /* retrieve position within table  */
  x = 0;
  y = 0;

  list = effect_line->table->children;

  while(list != NULL){
    if(y <= ((GtkTableChild *) list->data)->top_attach){
      y = ((GtkTableChild *) list->data)->top_attach + 1;
    }

    list = list->next;
  }

  /* get channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) effect_line->channel);
  
  pthread_mutex_unlock(application_mutex);
  
  /* play - find ports */
  pthread_mutex_lock(channel_mutex);
  
  recall_start =
    recall = ags_recall_get_by_effect(effect_line->channel->play,
				      filename,
				      effect);

  if(recall == NULL){
    pthread_mutex_unlock(channel_mutex);
    
    return(NULL);
  }

  recall = g_list_last(recall);
  port = AGS_RECALL(recall->data)->port;

  /* check has output port */
  if((AGS_RECALL_HAS_OUTPUT_PORT & (AGS_RECALL(recall->data)->flags)) != 0){
    has_output_port = TRUE;
  }else{
    has_output_port = FALSE;
  }

  /* recall handler of output port */
  if(has_output_port){
    AgsRecall *recall_channel_run_dummy;

    recall_channel_run_dummy = ags_recall_find_template(AGS_RECALL_CONTAINER(AGS_RECALL(recall->data)->container)->recall_channel_run)->data;
    
    /* alloc handler */
    recall_handler = (AgsRecallHandler *) malloc(sizeof(AgsRecallHandler));

    recall_handler->signal_name = "run-post";
    recall_handler->callback = G_CALLBACK(ags_effect_line_output_port_run_post_callback);
    recall_handler->data = (gpointer) effect_line;

    ags_recall_add_handler(AGS_RECALL(recall_channel_run_dummy), recall_handler);
  }

  g_list_free(recall_start);

  /* recall - find ports */
  recall_start = 
    recall = ags_recall_get_by_effect(effect_line->channel->recall,
				      filename,
				      effect);
  recall = g_list_last(recall);

  recall_port = AGS_RECALL(recall->data)->port;

  /* recall handler of output port */
  if(has_output_port){
    AgsRecall *recall_channel_run_dummy;

    recall_channel_run_dummy = ags_recall_find_template(AGS_RECALL_CONTAINER(AGS_RECALL(recall->data)->container)->recall_channel_run)->data;
    
    /* alloc handler */
    recall_handler = (AgsRecallHandler *) malloc(sizeof(AgsRecallHandler));

    recall_handler->signal_name = "run-post";
    recall_handler->callback = G_CALLBACK(ags_effect_line_output_port_run_post_callback);
    recall_handler->data = (gpointer) effect_line;

    ags_recall_add_handler(AGS_RECALL(recall_channel_run_dummy), recall_handler);
  }
  
  g_list_free(recall_start);
  
  pthread_mutex_unlock(channel_mutex);

  /* add separator */
  separator = ags_effect_separator_new();
  g_object_set(separator,
	       "text", effect,
	       "filename", filename,
	       "effect", effect,
	       NULL);
  gtk_table_attach(effect_line->table,
		   (GtkWidget *) separator,
		   0, AGS_EFFECT_LINE_COLUMNS_COUNT,
		   y, y + 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  gtk_widget_show_all(separator);
  
  y++;

  /* load ports */
  port_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->port;

  port_count = g_list_length(port_descriptor);
  k = 0;
  
  while(port_descriptor != NULL &&
	port != NULL){
    if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
      GtkWidget *child_widget;

      AgsLv2Conversion *lv2_conversion;
      
      GType widget_type;

      guint step_count;
      gboolean disable_seemless;

      disable_seemless = FALSE;

      if(x == AGS_EFFECT_LINE_COLUMNS_COUNT){
	x = 0;
	y++;
	gtk_table_resize(effect_line->table,
			 y + 1, AGS_EFFECT_LINE_COLUMNS_COUNT);
      }

      if((AGS_PORT_DESCRIPTOR_TOGGLED & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	disable_seemless = TRUE;

	if((AGS_PORT_DESCRIPTOR_OUTPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  widget_type = AGS_TYPE_LED;
	}else{
	  widget_type = GTK_TYPE_TOGGLE_BUTTON;
	}
      }else{
	if((AGS_PORT_DESCRIPTOR_OUTPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  widget_type = AGS_TYPE_HINDICATOR;
	}else{
	  widget_type = AGS_TYPE_DIAL;
	}
      }

      if(control_type_name != NULL){
	widget_type = g_type_from_name(control_type_name->data);

	control_type_name = control_type_name->next;
      }
      
      step_count = AGS_DIAL_DEFAULT_PRECISION;

      if((AGS_PORT_DESCRIPTOR_INTEGER & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	step_count = AGS_PORT_DESCRIPTOR(port_descriptor->data)->scale_steps;

	disable_seemless = TRUE;
      }

      /* add line member */
      line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
						   "widget-type", widget_type,
						   "widget-label", AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name,
						   "plugin-name", g_strdup_printf("lv2-<%s>", lv2_plugin->uri),
						   "filename", filename,
						   "effect", effect,
						   "specifier", g_strdup(AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name),
						   "control-port", g_strdup_printf("%u/%u",
										     k,
										     port_count),
						   "steps", step_count,
						   NULL);
      child_widget = ags_line_member_get_widget(line_member);

      /* lv2 conversion */
      lv2_conversion = NULL;

      if((AGS_PORT_DESCRIPTOR_LOGARITHMIC & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if(lv2_conversion == NULL ||
	   !AGS_IS_LV2_CONVERSION(lv2_conversion)){
	  lv2_conversion = ags_lv2_conversion_new();
	}
    
	lv2_conversion->flags |= AGS_LV2_CONVERSION_LOGARITHMIC;
      }

      line_member->conversion = (AgsConversion *) lv2_conversion;

      /* child widget */
      if((AGS_PORT_DESCRIPTOR_TOGGLED & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	line_member->port_flags = AGS_LINE_MEMBER_PORT_BOOLEAN;
      }
      
      if((AGS_PORT_DESCRIPTOR_INTEGER & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	line_member->port_flags = AGS_LINE_MEMBER_PORT_INTEGER;
      }

      if(AGS_IS_DIAL(child_widget)){
	AgsDial *dial;
	GtkAdjustment *adjustment;

	float lower_bound, upper_bound;
	
	dial = (AgsDial *) child_widget;

	if(disable_seemless){
	  dial->flags &= (~AGS_DIAL_SEEMLESS_MODE);
	}

	/* add controls of ports and apply range  */
	lower_bound = g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->lower_value);
	upper_bound = g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->upper_value);

	adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	g_object_set(dial,
		     "adjustment", adjustment,
		     NULL);

	if(upper_bound >= 0.0 && lower_bound >= 0.0){
	  step = (upper_bound - lower_bound) / step_count;
	}else if(upper_bound < 0.0 && lower_bound < 0.0){
	  step = -1.0 * (lower_bound - upper_bound) / step_count;
	}else{
	  step = (upper_bound - lower_bound) / step_count;
	}

	gtk_adjustment_set_step_increment(adjustment,
					  step);
	gtk_adjustment_set_lower(adjustment,
				 lower_bound);
	gtk_adjustment_set_upper(adjustment,
				 upper_bound);
	gtk_adjustment_set_value(adjustment,
				 g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->default_value));
      }else if(AGS_IS_INDICATOR(child_widget) ||
	       AGS_IS_LED(child_widget)){
	g_hash_table_insert(ags_effect_line_indicator_queue_draw,
			    child_widget, ags_effect_line_indicator_queue_draw_timeout);
	effect_line->queued_drawing = g_list_prepend(effect_line->queued_drawing,
						     child_widget);
	g_timeout_add(1000 / 30, (GSourceFunc) ags_effect_line_indicator_queue_draw_timeout, (gpointer) child_widget);
      }

#ifdef AGS_DEBUG
      g_message("lv2 bounds: %f %f", lower_bound, upper_bound);
#endif
	  
      gtk_table_attach(effect_line->table,
		       (GtkWidget *) line_member,
		       (x % AGS_EFFECT_LINE_COLUMNS_COUNT), (x % AGS_EFFECT_LINE_COLUMNS_COUNT) + 1,
		       y, y + 1,
		       GTK_FILL, GTK_FILL,
		       0, 0);
      
      ags_connectable_connect(AGS_CONNECTABLE(line_member));
      gtk_widget_show_all((GtkWidget *) line_member);

      port = port->next;
      x++;

      if(x % AGS_EFFECT_LINE_COLUMNS_COUNT == 0){
	y++;
      }
    }

    port_descriptor = port_descriptor->next;
    k++;
  }
  
  return(g_list_concat(g_list_copy(port),
		       g_list_copy(recall_port)));
}

GList*
ags_effect_line_real_add_effect(AgsEffectLine *effect_line,
				GList *control_type_name,
				gchar *filename,
				gchar *effect)
{
  AgsWindow *window;

  AgsLadspaPlugin *ladspa_plugin;
  AgsLv2Plugin *lv2_plugin;
  
  GList *port;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) effect_line);

  /* load plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_ladspa_manager_get_instance(),
							filename, effect);
  port = NULL;
  
  if(ladspa_plugin != NULL){
    port = ags_effect_line_add_ladspa_effect(effect_line,
					     control_type_name,
					     filename,
					     effect);
  }else{
    lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
						 filename, effect);
    
    if(lv2_plugin != NULL){
      port = ags_effect_line_add_lv2_effect(effect_line,
					    control_type_name,
					    filename,
					    effect);
    }
  }

  /*  */
  ags_automation_toolbar_load_port(window->automation_window->automation_editor->automation_toolbar);
  
  return(port);
}

/**
 * ags_effect_line_add_effect:
 * @effect_line: the #AgsEffectLine to modify
 * @control_type_name: the string representation of a #GType
 * @filename: the effect's filename
 * @effect: the effect's name
 *
 * Add an effect by its filename and effect specifier.
 *
 * Returns: the #GList-struct containing the #AgsPort objects added
 *
 * Since: 0.7.42
 */
GList*
ags_effect_line_add_effect(AgsEffectLine *effect_line,
			   GList *control_type_name,
			   gchar *filename,
			   gchar *effect)
{
  GList *port;
  
  g_return_val_if_fail(AGS_IS_EFFECT_LINE(effect_line), NULL);

  g_object_ref((GObject *) effect_line);
  g_signal_emit(G_OBJECT(effect_line),
		effect_line_signals[ADD_EFFECT], 0,
		control_type_name,
		filename,
		effect,
		&port);
  g_object_unref((GObject *) effect_line);

  return(port);
}

void
ags_effect_line_real_remove_effect(AgsEffectLine *effect_line,
				   guint nth)
{
  AgsWindow *window;
  AgsMachine *machine;

  AgsMutexManager *mutex_manager;

  GList *control, *control_start;
  GList *recall;
  GList *port;

  gchar *filename, *effect;
  gchar **remove_specifier;

  guint nth_effect, n_bulk;
  guint i;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) effect_line);
  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) effect_line,
						   AGS_TYPE_MACHINE);

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) effect_line->channel);
  
  pthread_mutex_unlock(application_mutex);
  
  /* get nth_effect */
  pthread_mutex_lock(channel_mutex);
  
  recall = effect_line->channel->play;
  nth_effect = 0;
  n_bulk = 0;
  
  while((recall = ags_recall_template_find_all_type(recall,
						    AGS_TYPE_RECALL_LADSPA,
						    AGS_TYPE_RECALL_LV2,
						    G_TYPE_NONE)) != NULL){
    if((AGS_RECALL_TEMPLATE & (AGS_RECALL(recall->data)->flags)) != 0){
      nth_effect++;
    }

    if((AGS_RECALL_BULK_MODE & (AGS_RECALL(recall->data)->flags)) != 0){
      n_bulk++;
    }

    if(nth_effect - n_bulk == nth + 1){
      break;
    }
    
    recall = recall->next;
  }

  if(recall == NULL){
    pthread_mutex_unlock(channel_mutex);
    
    return;
  }

  nth_effect--;

  /* destroy separator */
  filename = NULL;
  effect = NULL;
  
  if(AGS_IS_RECALL_LV2(recall->data)){
    filename = AGS_RECALL_LV2(recall->data)->filename;
    effect = AGS_RECALL_LV2(recall->data)->effect;
  }else if(AGS_IS_RECALL_LADSPA(recall->data)){
    filename = AGS_RECALL_LADSPA(recall->data)->filename;
    effect = AGS_RECALL_LADSPA(recall->data)->effect;
  }

  /* destroy separator */
  control_start =
    control = gtk_container_get_children((GtkContainer *) effect_line->table);

  while(control != NULL){
    gchar *separator_filename;
    gchar *separator_effect;
    
    if(AGS_IS_EFFECT_SEPARATOR(control->data)){
      g_object_get(control->data,
		   "filename", &separator_filename,
		   "effect", &separator_effect,
		   NULL);
      
      if(separator_filename != NULL &&
	 separator_effect != NULL &&
	 !g_strcmp0(filename,
		    separator_filename) &&
	 !g_strcmp0(effect,
		    separator_effect)){
	gtk_widget_destroy(control->data);
      
	break;
      }
    }
    
    control = control->next;
  }
  
  g_list_free(control_start);
    
  /* destroy controls */
  port = AGS_RECALL(recall->data)->port;
  remove_specifier = NULL;
  i = 0;
  
  while(port != NULL){
    control_start = 
      control = gtk_container_get_children((GtkContainer *) effect_line->table);
    
    while(control != NULL){
      if(AGS_IS_LINE_MEMBER(control->data) &&
	 AGS_LINE_MEMBER(control->data)->port == port->data){
	GtkWidget *child_widget;
	
	child_widget = gtk_bin_get_child(control->data);
	
	/* collect specifier */
	if(remove_specifier == NULL){
	  remove_specifier = (gchar **) malloc(2 * sizeof(gchar *));
	}else{
	  remove_specifier = (gchar **) realloc(remove_specifier,
						(i + 2) * sizeof(gchar *));
	}	
	
	remove_specifier[i] = g_strdup(AGS_LINE_MEMBER(control->data)->specifier);
	i++;

	/* remove widget */
	if(AGS_IS_LED(child_widget) ||
	   AGS_IS_INDICATOR(child_widget)){
	  g_hash_table_remove(ags_effect_line_indicator_queue_draw,
			      child_widget);
	}

	gtk_widget_destroy(control->data);
	
	break;
      }
	
      control = control->next;
    }

    g_list_free(control_start);
    
    port = port->next;
  }

  pthread_mutex_unlock(channel_mutex);

  if(remove_specifier != NULL){
    remove_specifier[i] = NULL;
  }

  /* remove recalls */
  ags_channel_remove_effect(effect_line->channel,
			    nth_effect);

  /* reset automation editor */
  ags_automation_editor_reset_port(window->automation_window->automation_editor,
				   machine,
				   (AGS_IS_OUTPUT(effect_line->channel) ? AGS_TYPE_OUTPUT: AGS_TYPE_INPUT),
				   remove_specifier);
}

/**
 * ags_effect_line_remove_effect:
 * @effect_line: the #AgsEffectLine to modify
 * @nth: the nth effect to remove
 *
 * Remove an effect by its position.
 *
 * Since: 0.7.42
 */
void
ags_effect_line_remove_effect(AgsEffectLine *effect_line,
			      guint nth)
{
  g_return_if_fail(AGS_IS_EFFECT_LINE(effect_line));

  g_object_ref((GObject *) effect_line);
  g_signal_emit(G_OBJECT(effect_line),
		effect_line_signals[REMOVE_EFFECT], 0,
		nth);
  g_object_unref((GObject *) effect_line);
}

void
ags_effect_line_real_map_recall(AgsEffectLine *effect_line,
				guint ouput_pad_start)
{
  if((AGS_EFFECT_LINE_MAPPED_RECALL & (effect_line->flags)) != 0){
    return;
  }

  effect_line->flags |= AGS_EFFECT_LINE_MAPPED_RECALL;

  ags_effect_line_find_port(effect_line);
}

/**
 * ags_effect_line_map_recall:
 * @effect_line: the #AgsEffectLine to add its default recall.
 * @output_pad_start: the start channel's index
 *
 * You may want the @effect_line to add its default recall. This function
 * may call ags_effect_line_find_port().
 *
 * Since: 0.7.42
 */
void
ags_effect_line_map_recall(AgsEffectLine *effect_line,
			   guint output_pad_start)
{
  g_return_if_fail(AGS_IS_EFFECT_LINE(effect_line));

  g_object_ref((GObject *) effect_line);
  g_signal_emit((GObject *) effect_line,
		effect_line_signals[MAP_RECALL], 0,
		output_pad_start);
  g_object_unref((GObject *) effect_line);
}

GList*
ags_effect_line_real_find_port(AgsEffectLine *effect_line)
{
  GList *port, *tmp_port;
  GList *line_member, *line_member_start;

  if(effect_line == NULL || effect_line->table == NULL){
    return(NULL);
  }

  line_member_start = 
    line_member = gtk_container_get_children(GTK_CONTAINER(effect_line->table));
  
  port = NULL;

  if(line_member != NULL){
    while(line_member != NULL){
      if(AGS_IS_LINE_MEMBER(line_member->data)){
	tmp_port = ags_line_member_find_port(AGS_LINE_MEMBER(line_member->data));

	if(port != NULL){
	  port = g_list_concat(port,
			       tmp_port);
	}else{
	  port = tmp_port;
	}
      }

      line_member = line_member->next;
    }

    g_list_free(line_member_start);
  }  
  
  return(port);
}

/**
 * ags_effect_line_find_port:
 * @effect_line: the #AgsEffectLine
 *
 * Lookup ports of associated recalls.
 *
 * Returns: an #GList containing all related #AgsPort
 *
 * Since: 0.7.8
 */
GList*
ags_effect_line_find_port(AgsEffectLine *effect_line)
{
  GList *list;

  list = NULL;
  g_return_val_if_fail(AGS_IS_EFFECT_LINE(effect_line),
		       NULL);

  g_object_ref((GObject *) effect_line);
  g_signal_emit((GObject *) effect_line,
		effect_line_signals[FIND_PORT], 0,
		&list);
  g_object_unref((GObject *) effect_line);

  return(list);
}

/**
 * ags_effect_line_indicator_queue_draw_timeout:
 * @widget: the indicator widgt
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed with redraw, otherwise %FALSE
 *
 * Since: 0.7.128
 */
gboolean
ags_effect_line_indicator_queue_draw_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_effect_line_indicator_queue_draw,
			 widget) != NULL){
    gtk_widget_queue_draw(widget);
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_effect_line_new:
 * @channel: the #AgsChannel to visualize
 *
 * Creates an #AgsEffectLine
 *
 * Returns: a new #AgsEffectLine
 *
 * Since: 0.4
 */
AgsEffectLine*
ags_effect_line_new(AgsChannel *channel)
{
  AgsEffectLine *effect_line;

  effect_line = (AgsEffectLine *) g_object_new(AGS_TYPE_EFFECT_LINE,
					       "channel", channel,
					       NULL);

  return(effect_line);
}
