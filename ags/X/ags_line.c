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

#include <ags/X/ags_line.h>
#include <ags/X/ags_line_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_marshal.h>
#include <ags/object/ags_plugin.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 
#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_ladspa_plugin.h>
#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_ladspa_conversion.h>
#include <ags/plugin/ags_lv2_conversion.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_recall_lv2.h>

#include <ags/audio/recall/ags_peak_channel_run.h>

#include <ags/widget/ags_dial.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line_member.h>

#include <ladspa.h>
#include <dlfcn.h>

void ags_line_class_init(AgsLineClass *line);
void ags_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_line_plugin_interface_init(AgsPluginInterface *plugin);
void ags_line_init(AgsLine *line);
void ags_line_finalize(GObject *gobject);
void ags_line_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_line_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);
void ags_line_connect(AgsConnectable *connectable);
void ags_line_disconnect(AgsConnectable *connectable);
gchar* ags_line_get_version(AgsPlugin *plugin);
void ags_line_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_line_get_build_id(AgsPlugin *plugin);
void ags_line_set_build_id(AgsPlugin *plugin, gchar *build_id);

void ags_line_real_set_channel(AgsLine *line, AgsChannel *channel);
GList* ags_line_add_ladspa_effect(AgsLine *line,
				  GList *control_type_name,
				  gchar *filename,
				  gchar *effect);
GList* ags_line_add_lv2_effect(AgsLine *line,
			       GList *control_type_name,
			       gchar *filename,
			       gchar *effect);
GList* ags_line_real_add_effect(AgsLine *line,
				GList *control_type_name,
				gchar *filename,
				gchar *effect);
void ags_line_real_remove_effect(AgsLine *line,
				 guint nth);
void ags_line_real_map_recall(AgsLine *line,
			      guint output_pad_start);
GList* ags_line_real_find_port(AgsLine *line);

/**
 * SECTION:ags_line
 * @short_description: A composite widget to visualize #AgsChannel
 * @title: AgsLine
 * @section_id:
 * @include: ags/X/ags_line.h
 *
 * #AgsLine is a composite widget to visualize #AgsChannel. It should be
 * packed by an #AgsPad. It may contain #AgsLineMember to modify ports of
 * #AgsRecall.
 */

enum{
  SET_CHANNEL,
  GROUP_CHANGED,
  ADD_EFFECT,
  REMOVE_EFFECT,
  MAP_RECALL,
  FIND_PORT,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_PAD,
  PROP_CHANNEL,
};

static gpointer ags_line_parent_class = NULL;
static guint line_signals[LAST_SIGNAL];

GHashTable *ags_indicator_queue_draw = NULL;

GType
ags_line_get_type(void)
{
  static GType ags_type_line = 0;

  if(!ags_type_line){
    static const GTypeInfo ags_line_info = {
      sizeof(AgsLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_line_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_line = g_type_register_static(GTK_TYPE_VBOX,
					   "AgsLine\0", &ags_line_info,
					   0);

    g_type_add_interface_static(ags_type_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_line,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_line);
}

void
ags_line_class_init(AgsLineClass *line)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_line_parent_class = g_type_class_peek_parent(line);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(line);

  gobject->set_property = ags_line_set_property;
  gobject->get_property = ags_line_get_property;

  gobject->finalize = ags_line_finalize;
  
  /* properties */
  /**
   * AgsLine:pad:
   *
   * The assigned #AgsPad.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("pad\0",
				   "parent pad\0",
				   "The pad which is its parent\0",
				   AGS_TYPE_PAD,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PAD,
				  param_spec);

  /**
   * AgsLine:channel:
   *
   * The assigned #AgsChannel to visualize.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("channel\0",
				   "assigned channel\0",
				   "The channel it is assigned with\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /* AgsLineClass */
  line->set_channel = ags_line_real_set_channel;
  line->group_changed = NULL;
  
  line->add_effect = ags_line_real_add_effect;
  line->remove_effect = ags_line_real_remove_effect;
  
  line->map_recall = ags_line_real_map_recall;
  line->find_port = ags_line_real_find_port;

  /* signals */
  /**
   * AgsLine::set-channel:
   * @line: the #AgsLine to modify
   * @channel: the #AgsChannel to set
   *
   * The ::set-channel signal notifies about changed channel.
   *
   * Since: 0.4.3
   */
  line_signals[SET_CHANNEL] =
    g_signal_new("set-channel\0",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, set_channel),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsLine::group-changed:
   * @line: the object group changed
   *
   * The ::group-changed signal notifies about changed grouping. This
   * normally happens as toggling group button in #AgsPad or #AgsLine.
   *
   * Since: 0.4.3
   */
  line_signals[GROUP_CHANGED] =
    g_signal_new("group-changed\0",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, group_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsLine::add-effect:
   * @line: the #AgsLine to modify
   * @control_type_name: the control #GType string representation
   * @filename: the effect's filename
   * @effect: the effect's name
   *
   * The ::add-effect signal notifies about added effect.
   *
   * Returns: a #GList-struct containing new #AgsPort objects
   *
   * Since: 0.4.3
   */
  line_signals[ADD_EFFECT] =
    g_signal_new("add-effect\0",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, add_effect),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__POINTER_STRING_STRING,
		 G_TYPE_POINTER, 3,
		 G_TYPE_POINTER,
		 G_TYPE_STRING,
		 G_TYPE_STRING);

  /**
   * AgsLine::remove-effect:
   * @line: the #AgsLine to modify
   * @nth: the nth effect
   *
   * The ::remove-effect signal notifies about removed effect.
   *
   * Since: 0.4.3
   */
  line_signals[REMOVE_EFFECT] =
    g_signal_new("remove-effect\0",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, remove_effect),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsLine::map-recall:
   * @line: the #AgsLine
   * @output_pad_start: the channels start pad
   *
   * The ::map-recall as recalls should be mapped.
   *
   * Since: 0.4.3
   */
  line_signals[MAP_RECALL] =
    g_signal_new("map-recall\0",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, map_recall),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsLine::find-port:
   * @line: the #AgsLine 
   *
   * The ::find-port retrieves all associated ports.
   * 
   * Returns: a #GList with associated ports
   *
   * Since: 0.4.3
   */
  line_signals[FIND_PORT] =
    g_signal_new("find-port\0",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, find_port),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);
}

void
ags_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_line_connect;
  connectable->disconnect = ags_line_disconnect;
}

void
ags_line_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_line_get_version;
  plugin->set_version = ags_line_set_version;
  plugin->get_build_id = ags_line_get_build_id;
  plugin->set_build_id = ags_line_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_line_init(AgsLine *line)
{
  if(ags_indicator_queue_draw == NULL){
    ags_indicator_queue_draw = g_hash_table_new_full(g_direct_hash, g_direct_equal,
						     NULL,
						     NULL);
  }
  
  g_signal_connect_after((GObject *) line, "parent_set\0",
			 G_CALLBACK(ags_line_parent_set_callback), (gpointer) line);

  line->flags = 0;

  line->version = AGS_VERSION;
  line->build_id = AGS_BUILD_ID;

  line->channel = NULL;

  //  gtk_widget_set_can_focus(line,
  //			   TRUE);

  line->pad = NULL;
  
  line->label = (GtkLabel *) gtk_label_new(NULL);
  gtk_box_pack_start(GTK_BOX(line),
		     GTK_WIDGET(line->label),
		     FALSE, FALSE,
		     0);

  line->group = (GtkToggleButton *) gtk_toggle_button_new_with_label("group\0");
  gtk_toggle_button_set_active(line->group, TRUE);
  gtk_box_pack_start(GTK_BOX(line),
		     GTK_WIDGET(line->group),
		     FALSE, FALSE,
		     0);

  line->expander = ags_expander_new(1, 1);
  gtk_table_set_row_spacings(line->expander->table,
			     2);
  gtk_table_set_col_spacings(line->expander->table,
			     2);
  gtk_box_pack_start(GTK_BOX(line),
		     GTK_WIDGET(line->expander),
		     TRUE, TRUE,
		     0);
  
  line->indicator = NULL;
}

void
ags_line_finalize(GObject *gobject)
{
  AgsLine *line;

  line = AGS_LINE(gobject);
  
  if(line->indicator != NULL){
    g_hash_table_remove(ags_indicator_queue_draw,
			line->indicator);
  }
  
  G_OBJECT_CLASS(ags_line_parent_class)->finalize(gobject);
}

void
ags_line_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  AgsLine *line;

  line = AGS_LINE(gobject);

  switch(prop_id){
  case PROP_PAD:
    {
      GtkWidget *pad;

      pad = (GtkWidget *) g_value_get_object(value);

      if(line->pad == pad){
	return;
      }

      if(line->pad != NULL){
	g_object_unref(G_OBJECT(line->pad));
      }

      if(pad != NULL){
	g_object_ref(G_OBJECT(pad));
      }
      
      line->pad = pad;
    }
    break;
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      ags_line_set_channel(line, channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_line_get_property(GObject *gobject,
		      guint prop_id,
		      GValue *value,
		      GParamSpec *param_spec)
{
  AgsLine *line;

  line = AGS_LINE(gobject);

  switch(prop_id){
  case PROP_PAD:
    g_value_set_object(value, line->pad);
    break;
  case PROP_CHANNEL:
    g_value_set_object(value, line->channel);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_line_connect(AgsConnectable *connectable)
{
  AgsMachine *machine;
  AgsLine *line;
  GList *list, *list_start;

  line = AGS_LINE(connectable);

  if((AGS_LINE_CONNECTED & (line->flags)) != 0){
    return;
  }

  /* set connected flag */
  line->flags |= AGS_LINE_CONNECTED;

#ifdef AGS_DEBUG
  g_message("line connect\0");
#endif
  
  if((AGS_LINE_PREMAPPED_RECALL & (line->flags)) == 0){
    if((AGS_LINE_MAPPED_RECALL & (line->flags)) == 0){
      ags_line_map_recall(line,
			 0);
    }
  }else{
    ags_line_find_port(line);
  }

  /* AgsMachine */
  machine = AGS_MACHINE(gtk_widget_get_ancestor((GtkWidget *) AGS_LINE(line),
						AGS_TYPE_MACHINE));

  /* connect group button */
  g_signal_connect_after((GObject *) line->group, "clicked\0",
			 G_CALLBACK(ags_line_group_clicked_callback), (gpointer) line);

  /* connect line members */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(line->expander->table));
  
  while(list != NULL){
    if(AGS_IS_CONNECTABLE(list->data)){
      ags_connectable_connect(AGS_CONNECTABLE(list->data));
    }

    list = list->next;
  }

  if(list_start != NULL){
    g_list_free(list_start);
  }
}

void
ags_line_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

gchar*
ags_line_get_version(AgsPlugin *plugin)
{
  return(AGS_LINE(plugin)->version);
}

void
ags_line_set_version(AgsPlugin *plugin, gchar *version)
{
  AGS_LINE(plugin)->version = version;
}

gchar*
ags_line_get_build_id(AgsPlugin *plugin)
{
  return(AGS_LINE(plugin)->build_id);
}

void
ags_line_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AGS_LINE(plugin)->build_id = build_id;
}

void
ags_line_real_set_channel(AgsLine *line, AgsChannel *channel)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;
  
  if(line->channel == channel){
    return;
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  if(line->channel != NULL){
    g_signal_handler_disconnect(line->channel,
				line->add_effect_handler);
    //    g_signal_handler_disconnect(line->channel,
    //				line->remove_effect_handler);
    
    g_object_unref(G_OBJECT(line->channel));
  }

  if(channel != NULL){
    g_object_ref(G_OBJECT(channel));

    line->add_effect_handler = g_signal_connect_after(channel, "add-effect\0",
						      G_CALLBACK(ags_line_add_effect_callback), line);
    //    line->remove_effect_handler = g_signal_connect_after(channel, "remove-effect\0",
    //							 G_CALLBACK(ags_line_remove_effect_callback), line);
  }

  if(line->channel != NULL){
    line->flags &= (~AGS_LINE_PREMAPPED_RECALL);
  }
  
  line->channel = channel;

  /* lookup channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* set label */
  pthread_mutex_lock(channel_mutex);
  
  gtk_label_set_label(line->label, g_strdup_printf("channel %d\0", channel->audio_channel));

  pthread_mutex_unlock(channel_mutex);
}

/**
 * ags_line_set_channel:
 * @line: an #AgsLine
 * @channel: the #AgsChannel to set
 *
 * Is emitted as channel gets modified.
 *
 * Since: 0.3
 */
void
ags_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  g_return_if_fail(AGS_IS_LINE(line));

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[SET_CHANNEL], 0,
		channel);
  g_object_unref((GObject *) line);
}

/**
 * ags_line_group_changed:
 * @line: an #AgsLine
 *
 * Is emitted as group is changed.
 *
 * Since: 0.4
 */
void
ags_line_group_changed(AgsLine *line)
{
  g_return_if_fail(AGS_IS_LINE(line));

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[GROUP_CHANGED], 0);
  g_object_unref((GObject *) line);
}

GList*
ags_line_add_ladspa_effect(AgsLine *line,
			   GList *control_type_name,
			   gchar *filename,
			   gchar *effect)
{
  AgsLineMember *line_member;
  GtkAdjustment *adjustment;

  AgsLadspaPlugin *ladspa_plugin;

  AgsMutexManager *mutex_manager;

  GList *list;
  GList *recall, *recall_start;
  GList *port, *recall_port;
  GList *port_descriptor;

  gdouble step;
  guint port_count;
  guint x, y;
  guint k;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;
  
  /* load plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(filename, effect);
  
  /* retrieve position within table  */
  x = 0;
  y = 0;

  list = line->expander->children;

  while(list != NULL){
    if(y <= AGS_EXPANDER_CHILD(list->data)->y){
      y = AGS_EXPANDER_CHILD(list->data)->y + 1;
    }

    list = list->next;
  }

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) line->channel);
  
  pthread_mutex_unlock(application_mutex);
  
  /* find ports */
  pthread_mutex_lock(channel_mutex);
  
  recall_start =
    recall = ags_recall_get_by_effect(line->channel->play,
				      filename,
				      effect);
  recall = g_list_last(recall);
  port = AGS_RECALL(recall->data)->port;

  g_list_free(recall_start);

  recall_start = 
    recall = ags_recall_get_by_effect(line->channel->recall,
				      filename,
				      effect);
  recall = g_list_last(recall);

  recall_port = AGS_RECALL(recall->data)->port;
  g_list_free(recall_start);

  pthread_mutex_unlock(channel_mutex);
  
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
      
      if((AGS_PORT_DESCRIPTOR_TOGGLED & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	widget_type = GTK_TYPE_TOGGLE_BUTTON;
      }else{
	widget_type = AGS_TYPE_DIAL;
      }

      if(control_type_name != NULL){
	widget_type = g_type_from_name(control_type_name->data);

	control_type_name = control_type_name->next;
      }
      
      step_count = AGS_DIAL_DEFAULT_PRECISION;

      if((AGS_PORT_DESCRIPTOR_INTEGER & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	step_count = AGS_PORT_DESCRIPTOR(port_descriptor->data)->scale_steps;
      }
      
      /* add line member */
      line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
						   "widget-type\0", widget_type,
						   "widget-label\0", AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name,
						   "plugin-name\0", g_strdup_printf("ladspa-%u\0", ladspa_plugin->unique_id),
						   "filename\0", filename,
						   "effect\0", effect,
						   "specifier\0", g_strdup(AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name),
						   "control-port\0", g_strdup_printf("%u/%u\0",
										     k,
										     port_count),
						   "steps\0", step_count,
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
      }

#ifdef AGS_DEBUG
      g_message("ladspa bounds: %f %f\0", lower_bound, upper_bound);
#endif
	  
      ags_expander_add(line->expander,
		       (GtkWidget *) line_member,
		       x, y,
		       1, 1);
      ags_connectable_connect(AGS_CONNECTABLE(line_member));
      gtk_widget_show_all((GtkWidget *) line_member);
      
      port = port->next;
      x++;
    }

    port_descriptor = port_descriptor->next;
    k++;
  }

  return(port);
}

GList*
ags_line_add_lv2_effect(AgsLine *line,
			GList *control_type_name,
			gchar *filename,
			gchar *effect)
{
  AgsLineMember *line_member;
  GtkAdjustment *adjustment;

  AgsLv2Plugin *lv2_plugin;

  AgsMutexManager *mutex_manager;

  GList *list;
  GList *recall, *recall_start;
  GList *port, *recall_port;
  GList *port_descriptor;

  gchar *port_type_0, *port_type_1;
  gchar *str;

  gdouble step;
  guint port_count;
  guint x, y;
  guint k;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;

  /* load plugin */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(filename, effect);

  /* retrieve position within table  */
  x = 0;
  y = 0;

  list = line->expander->children;

  while(list != NULL){
    if(y <= AGS_EXPANDER_CHILD(list->data)->y){
      y = AGS_EXPANDER_CHILD(list->data)->y + 1;
    }

    list = list->next;
  }

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) line->channel);
  
  pthread_mutex_unlock(application_mutex);
  
  /* find ports */
  pthread_mutex_lock(channel_mutex);
  
  recall_start =
    recall = ags_recall_get_by_effect(line->channel->play,
				      filename,
				      effect);

  if(recall == NULL){
    pthread_mutex_unlock(channel_mutex);
    
    return(NULL);
  }
  
  recall = g_list_last(recall);
  port = AGS_RECALL(recall->data)->port;

  g_list_free(recall_start);

  recall_start = 
    recall = ags_recall_get_by_effect(line->channel->recall,
				      filename,
				      effect);
  recall = g_list_last(recall);

  recall_port = AGS_RECALL(recall->data)->port;
  g_list_free(recall_start);

  pthread_mutex_unlock(channel_mutex);
  
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
      
      if((AGS_PORT_DESCRIPTOR_TOGGLED & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	widget_type = GTK_TYPE_TOGGLE_BUTTON;
      }else{
	widget_type = AGS_TYPE_DIAL;
      }

      if(control_type_name != NULL){
	widget_type = g_type_from_name(control_type_name->data);

	control_type_name = control_type_name->next;
      }
      
      step_count = AGS_DIAL_DEFAULT_PRECISION;

      if((AGS_PORT_DESCRIPTOR_INTEGER & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	step_count = AGS_PORT_DESCRIPTOR(port_descriptor->data)->scale_steps;
      }

      /* add line member */
      line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
						   "widget-type\0", widget_type,
						   "widget-label\0", AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name,
						   "plugin-name\0", g_strdup_printf("lv2-<%s>\0", lv2_plugin->uri),
						   "filename\0", filename,
						   "effect\0", effect,
						   "specifier\0", g_strdup(AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name),
						   "control-port\0", g_strdup_printf("%d/%d\0",
										     k,
										     port_count),
						   "steps\0", step_count,
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
      }

#ifdef AGS_DEBUG
      g_message("lv2 bounds: %f %f\0", lower_bound, upper_bound);
#endif

      ags_expander_add(line->expander,
		       (GtkWidget *) line_member,
		       x, y,
		       1, 1);      
      ags_connectable_connect(AGS_CONNECTABLE(line_member));
      gtk_widget_show_all((GtkWidget *) line_member);

      port = port->next;
      x++;
    }

    port_descriptor = port_descriptor->next;
    k++;
  }
  
  return(port);
}

GList*
ags_line_real_add_effect(AgsLine *line,
			 GList *control_type_name,
			 gchar *filename,
			 gchar *effect)
{
  AgsWindow *window;
  AgsMachine *machine;

  AgsLadspaPlugin *ladspa_plugin;
  AgsLv2Plugin *lv2_plugin;
  
  GList *port;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) line);
  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) line,
						   AGS_TYPE_MACHINE);
  
  /* load plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(filename, effect);
  port = NULL;
  
  if(ladspa_plugin != NULL){
    port = ags_line_add_ladspa_effect(line,
				      control_type_name,
				      filename,
				      effect);
  }else{
    lv2_plugin = ags_lv2_manager_find_lv2_plugin(filename, effect);
    
    if(lv2_plugin != NULL){
      port = ags_line_add_lv2_effect(line,
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
 * ags_line_add_effect:
 * @line: the #AgsLine
 * @control_type_name: the control #GType string representation
 * @filename: the filename of the plugin
 * @effect: the effect's name
 *
 * Add a line member.
 *
 * Returns: a #GList-struct containing new #AgsPort objects
 *
 * Since: 0.4.3 
 */
GList*
ags_line_add_effect(AgsLine *line,
		    GList *control_type_name,
		    gchar *filename,
		    gchar *effect)
{
  GList *port;
  
  g_return_val_if_fail(AGS_IS_LINE(line), NULL);

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[ADD_EFFECT], 0,
		control_type_name,
		filename,
		effect,
		&port);
  g_object_unref((GObject *) line);
  
  return(port);
}

void
ags_line_real_remove_effect(AgsLine *line,
			    guint nth)
{
  AgsWindow *window;
  AgsMachine *machine;
  
  AgsMutexManager *mutex_manager;

  GList *control;
  GList *recall;
  GList *port;

  gchar **remove_specifier;
  
  guint nth_effect, n_bulk;
  guint i;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) line);
  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) line,
						   AGS_TYPE_MACHINE);
  
  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) line->channel);
  
  pthread_mutex_unlock(application_mutex);
  
  /* get nth_effect */
  pthread_mutex_lock(channel_mutex);
  
  recall = line->channel->play;
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
  
  /* destroy controls */
  port = AGS_RECALL(recall->data)->port;
  remove_specifier = NULL;
  i = 0;
  
  while(port != NULL){
    control = gtk_container_get_children((GtkContainer *) line->expander->table);
      
    while(control != NULL){
      if(AGS_IS_LINE_MEMBER(control->data) &&
	 AGS_LINE_MEMBER(control->data)->port == port->data){
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
	ags_expander_remove(line->expander,
			    control->data);

	break;
      }
	
      control = control->next;
    }
      
    port = port->next;
  }

  pthread_mutex_unlock(channel_mutex);

  if(remove_specifier != NULL){
    remove_specifier[i] = NULL;
  }
  
  /* remove recalls */
  ags_channel_remove_effect(line->channel,
			    nth_effect);

  /* reset automation editor */
  ags_automation_editor_reset_port(window->automation_window->automation_editor,
				   machine,
				   (AGS_IS_OUTPUT(line->channel) ? AGS_TYPE_OUTPUT: AGS_TYPE_INPUT),
				   remove_specifier);
}

/**
 * ags_line_remove_effect:
 * @line: the #AgsLine
 * @nth: nth effect to remove
 *
 * Remove a line member.
 *
 * Since: 0.4.3
 */
void
ags_line_remove_effect(AgsLine *line,
		       guint nth)
{
  g_return_if_fail(AGS_IS_LINE(line));

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[REMOVE_EFFECT], 0,
		nth);
  g_object_unref((GObject *) line);
}

void
ags_line_real_map_recall(AgsLine *line,
			 guint output_pad_start)
{
  if((AGS_LINE_MAPPED_RECALL & (line->flags)) != 0){
    return;
  }
  
  line->flags |= AGS_LINE_MAPPED_RECALL;

  ags_line_find_port(line);
}

/**
 * ags_line_map_recall:
 * @line: an #AgsLine
 * @output_pad_start: the start pad
 *
 * Is emitted as group is changed.
 *
 * Since: 0.4
 */
void
ags_line_map_recall(AgsLine *line,
		    guint output_pad_start)
{
  g_return_if_fail(AGS_IS_LINE(line));

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[MAP_RECALL], 0,
		output_pad_start);
  g_object_unref((GObject *) line);
}

GList*
ags_line_real_find_port(AgsLine *line)
{
  GList *port, *tmp_port;
  GList *line_member, *line_member_start;

  if(line == NULL || line->expander == NULL){
    return(NULL);
  }

  line_member_start = 
    line_member = gtk_container_get_children(GTK_CONTAINER(line->expander->table));
  
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
 * ags_line_find_port:
 * @line: an #AgsLine
 *
 * Lookup ports of assigned recalls.
 *
 * Returns: an #GList containing all related #AgsPort
 *
 * Since: 0.4
 */
GList*
ags_line_find_port(AgsLine *line)
{
  GList *list;

  list = NULL;
  g_return_val_if_fail(AGS_IS_LINE(line),
		       NULL);

  g_object_ref((GObject *) line);
  g_signal_emit((GObject *) line,
		line_signals[FIND_PORT], 0,
		&list);
  g_object_unref((GObject *) line);

  return(list);
}

/**
 * ags_line_find_next_grouped:
 * @line: a #GList-struct of #AgsLine objects
 *
 * Retrieve next grouped line.
 *
 * Returns: next matching #GList-struct containing #AgsLine
 * 
 * Since: 0.4
 */
GList*
ags_line_find_next_grouped(GList *line)
{
  while(line != NULL && !gtk_toggle_button_get_active(AGS_LINE(line->data)->group)){
    line = line->next;
  }

  return(line);
}

/**
 * ags_line_indicator_queue_draw_timeout:
 * @widget: the indicator widgt
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed with redraw, otherwise %FALSE
 *
 * Since: 0.7.8
 */
gboolean
ags_line_indicator_queue_draw_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_indicator_queue_draw,
			 widget) != NULL){
    gtk_widget_queue_draw(widget);
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_line_new:
 * @pad: the parent pad
 * @channel: the channel to visualize
 *
 * Creates an #AgsLine
 *
 * Returns: a new #AgsLine
 *
 * Since: 0.3
 */
AgsLine*
ags_line_new(GtkWidget *pad, AgsChannel *channel)
{
  AgsLine *line;

  line = (AgsLine *) g_object_new(AGS_TYPE_LINE,
				  "pad\0", pad,
				  "channel\0", channel,
				  NULL);

  return(line);
}
