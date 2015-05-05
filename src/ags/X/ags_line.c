/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_line.h>
#include <ags/X/ags_line_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_marshal.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/plugin/ags_ladspa_manager.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_ladspa.h>

#include <ags/audio/recall/ags_peak_channel_run.h>

#include <ags/widget/ags_dial.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line_member.h>

#include <ags/X/task/ags_add_line_member.h>

void ags_line_class_init(AgsLineClass *line);
void ags_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_line_plugin_interface_init(AgsPluginInterface *plugin);
void ags_line_init(AgsLine *line);
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

GList* ags_line_real_add_effect(AgsLine *line,
				gchar *filename,
				gchar *effect);
void ags_line_real_remove_effect(AgsLine *line,
				 guint nth);
void ags_line_real_set_channel(AgsLine *line, AgsChannel *channel);
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
  ADD_EFFECT,
  REMOVE_EFFECT,
  SET_CHANNEL,
  GROUP_CHANGED,
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

  line->add_effect = ags_line_real_add_effect;
  line->remove_effect = ags_line_real_remove_effect;
  line->group_changed = NULL;
  line->map_recall = ags_line_real_map_recall;
  line->find_port = ags_line_real_find_port;
  
  /* signals */
  /**
   * AgsLine::add-effect:
   * @line: the #AgsLine to modify
   * @effect: the effect's name
   *
   * The ::add-effect signal notifies about added effect.
   */
  line_signals[ADD_EFFECT] =
    g_signal_new("add-effect\0",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, add_effect),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__STRING_STRING,
		 G_TYPE_POINTER, 2,
		 G_TYPE_STRING,
		 G_TYPE_STRING);

  /**
   * AgsLine::remove-effect:
   * @line: the #AgsLine to modify
   * @nth: the nth effect
   *
   * The ::remove-effect signal notifies about removed effect.
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
   * AgsLine::set-channel:
   * @line: the #AgsLine to modify
   * @channel: the #AgsChannel to set
   *
   * The ::set-channel signal notifies about changed channel.
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
   * AgsLine::map-recall:
   * @line: the #AgsLine
   *
   * The ::map-recall as recalls should be mapped.
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
   * Returns: a #GList with associated ports
   *
   * The ::find-port retrieves all associated ports.
   */
  line_signals[FIND_PORT] =
    g_signal_new("find-port\0",
		 G_TYPE_FROM_CLASS(line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineClass, find_port),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__VOID,
		 G_TYPE_NONE, 0);
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
  g_signal_connect_after((GObject *) line, "parent_set\0",
			 G_CALLBACK(ags_line_parent_set_callback), (gpointer) line);

  line->flags = 0;

  line->version = AGS_VERSION;
  line->build_id = AGS_BUILD_ID;

  line->channel = NULL;

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
  AgsLine *line;
  GList *list, *list_start;

  line = AGS_LINE(connectable);

  if((AGS_LINE_CONNECTED & (line->flags)) != 0){
    return;
  }

  /* set connected flag */
  line->flags |= AGS_LINE_CONNECTED;

  g_message("line connect\0");
  
  if((AGS_LINE_PREMAPPED_RECALL & (line->flags)) == 0){
    if((AGS_LINE_MAPPED_RECALL & (line->flags)) == 0){
      ags_line_map_recall(line,
			  0);
    }
  }else{
    line->flags &= (~AGS_LINE_PREMAPPED_RECALL);
  }

  /* channel */
  g_signal_connect_after((GObject *) line->channel, "add-effect\0",
			 G_CALLBACK(ags_line_add_effect_callback), line);
  
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
  if(line->channel == channel){
    return;
  }

  if(line->channel != NULL){
    g_object_unref(G_OBJECT(line->channel));
  }

  if(channel != NULL){
    g_object_ref(G_OBJECT(channel));
  }

  if(line->channel != NULL){
    line->flags &= (~AGS_LINE_PREMAPPED_RECALL);
  }
  
  line->channel = channel;
  
  /* set label */
  gtk_label_set_label(line->label, g_strdup_printf("line %d\0", channel->audio_channel));
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
ags_line_real_add_effect(AgsLine *line,
			 gchar *filename,
			 gchar *effect)
{
  AgsLineMember *line_member;
  AgsAddLineMember *add_line_member;
  GtkAdjustment *adjustment;

  AgsLadspaPlugin *ladspa_plugin;

  GList *list;
  GList *recall, *recall_start;
  GList *port, *recall_port;
  gdouble step;
  guint x, y;
  
  void *plugin_so;
  LADSPA_Descriptor_Function ladspa_descriptor;
  LADSPA_Descriptor *plugin_descriptor;
  LADSPA_PortDescriptor *port_descriptor;
  LADSPA_Data lower_bound, upper_bound;
  unsigned long index;
  unsigned long i;

  index = ags_ladspa_manager_effect_index(filename,
					  effect);

  /* load plugin */
  ags_ladspa_manager_load_file(filename);
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(filename);

  plugin_so = ladspa_plugin->plugin_so;

  /* retrieve position within table  */
  x = 0;
  y = 0;
  i = 0;

  list = line->expander->children;

  while(list != NULL){
    if(y <= AGS_EXPANDER_CHILD(list->data)->y){
      y = AGS_EXPANDER_CHILD(list->data)->y + 1;
    }

    list = list->next;
  }

  /* find ports */
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
  
  /* load ports */
  if(index != -1 &&
     plugin_so){
    ladspa_descriptor = (LADSPA_Descriptor_Function) dlsym(plugin_so,
							   "ladspa_descriptor\0");

    if(dlerror() == NULL && ladspa_descriptor){
      plugin_descriptor = ladspa_descriptor(index);

      port_descriptor = plugin_descriptor->PortDescriptors;   
      
      while(port != NULL && recall_port != NULL){
	if((LADSPA_IS_PORT_CONTROL(port_descriptor[i]) && 
	    (LADSPA_IS_PORT_INPUT(port_descriptor[i]) ||
	     LADSPA_IS_PORT_OUTPUT(port_descriptor[i])))){
	  AgsDial *dial;
	  GtkAdjustment *adjustment;

	  if(x == 2){
	    x = 0;
	    y++;
	  }
	  
	  g_message("line_add_effect - add line member\0");

	  /* add line member */
	  line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
						       "widget-type\0", AGS_TYPE_DIAL,
						       "widget-label\0", plugin_descriptor->PortNames[i],
						       "plugin-name\0", AGS_PORT(port->data)->plugin_name,
						       "specifier\0", AGS_PORT(port->data)->specifier,
						       "control-port\0", AGS_PORT(port->data)->control_port,
						       NULL);
	  dial = ags_line_member_get_widget(line_member);
	  gtk_widget_set_size_request(dial,
				      2 * dial->radius + 2 * dial->outline_strength + dial->button_width + 1,
				      2 * dial->radius + 2 * dial->outline_strength + 1);
		
	  /* add controls of ports and apply range  */
	  lower_bound = plugin_descriptor->PortRangeHints[i].LowerBound;
	  upper_bound = plugin_descriptor->PortRangeHints[i].UpperBound;

	  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
	  g_object_set(dial,
		       "adjustment", adjustment,
		       NULL);

	  if(upper_bound >= 0.0 && lower_bound >= 0.0){
	    step = (upper_bound - lower_bound) / AGS_DIAL_DEFAULT_PRECISION;
	  }else if(upper_bound < 0.0 && lower_bound < 0.0){
	    step = -1.0 * (lower_bound - upper_bound) / AGS_DIAL_DEFAULT_PRECISION;
	  }else{
	    step = (upper_bound - lower_bound) / AGS_DIAL_DEFAULT_PRECISION;
	  }

	  gtk_adjustment_set_step_increment(adjustment,
					    step);
	  gtk_adjustment_set_lower(adjustment,
				   lower_bound);
	  gtk_adjustment_set_upper(adjustment,
				   upper_bound);
	  gtk_adjustment_set_value(adjustment,
				   lower_bound);

	  ags_expander_add(line->expander,
			   line_member,
			   x, y,
			   1, 1);

	  ags_connectable_connect(AGS_CONNECTABLE(line_member));
	  gtk_widget_show_all(line_member);

	  x++;
	  port = port->next;
	  recall_port = recall_port->next;
	}

	i++;
      }
    }
  }
  
  return(port);
}

GList*
ags_line_add_effect(AgsLine *line,
		    gchar *filename,
		    gchar *effect)
{
  GList *port;
  
  g_return_val_if_fail(AGS_IS_LINE(line), NULL);

  g_object_ref((GObject *) line);
  g_signal_emit(G_OBJECT(line),
		line_signals[ADD_EFFECT], 0,
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
  GList *control;
  GList *play_ladspa;
  GList *port;

  play_ladspa = ags_recall_template_find_type(line->channel->play,
					      AGS_TYPE_RECALL_LADSPA);

  /* destroy controls */
  port = AGS_RECALL(g_list_nth(play_ladspa,
			       nth)->data)->port;
    
  while(port != NULL){
    control = gtk_container_get_children(line->expander->table);
      
    while(control != NULL){
      if(AGS_IS_LINE_MEMBER(control->data) &&
	 AGS_LINE_MEMBER(control->data)->port == port->data){
	ags_expander_remove(line->expander,
			    control->data);
	break;
      }
	
      control = control->next;
    }
      
    port = port->next;
  }

  /* remove recalls */
  ags_channel_remove_effect(line->channel,
			    nth);
}

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
			 guint ouput_pad_start)
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
 * Returns: an #GList containing all related #AgsPort
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
  AgsChannel *channel, *next_pad;
  GList *list, *tmp;
  GList *line_member, *line_member_start;

  if(line == NULL || line->expander == NULL){
    return(NULL);
  }

  line_member_start = 
    line_member = gtk_container_get_children(GTK_CONTAINER(line->expander->table));

  while(line_member != NULL){
    if(AGS_IS_LINE_MEMBER(line_member->data)){
      ags_line_member_find_port(AGS_LINE_MEMBER(line_member->data));
    }

    line_member = line_member->next;
  }

  if(line_member_start != NULL){
    g_list_free(line_member_start);
  }
  
  /*  */
  channel = line->channel;

  if(channel != NULL){
    next_pad = channel->next_pad;

    list = NULL;
  
    while(channel != next_pad){
      if(list == NULL){
	list = ags_channel_find_port(channel);
      }else{
	tmp = ags_channel_find_port(channel);
	
	if(tmp != NULL){
	  list = g_list_concat(list,
			       tmp);
	}
      }
      
      channel = channel->next;
    }
  }
  
  return(list);

}

/**
 * ags_line_find_port:
 * @line: an #AgsLine
 * Returns: an #GList containing all related #AgsPort
 *
 * Lookup ports of assigned recalls.
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
 * @line: an #AgsLine
 *
 * Retrieve next grouped line.
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
