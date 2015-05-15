/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags/X/ags_effect_line.h>
#include <ags/X/ags_effect_line_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_marshal.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_lv2_manager.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_ladspa.h>

#include <ags/widget/ags_dial.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_line_member.h>

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

GList* ags_effect_line_real_add_effect(AgsEffectLine *effect_line,
				       gchar *filename,
				       gchar *effect);
void ags_effect_line_real_remove_effect(AgsEffectLine *effect_line,
					guint nth);

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
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_CHANNEL,
};

static gpointer ags_effect_line_parent_class = NULL;
static guint effect_line_signals[LAST_SIGNAL];

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
						  "AgsEffectLine\0", &ags_effect_line_info,
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

  /* properties */
  /**
   * AgsEffectLine:channel:
   *
   * The start of a bunch of #AgsChannel to visualize.
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

  /* AgsEffectLineClass */
  effect_line->add_effect = ags_effect_line_real_add_effect;
  effect_line->remove_effect = ags_effect_line_real_remove_effect;
  
  /* signals */
  /**
   * AgsEffectLine::add-effect:
   * @effect_line: the #AgsEffectLine to modify
   * @effect: the effect's name
   *
   * The ::add-effect signal notifies about added effect.
   */
  effect_line_signals[ADD_EFFECT] =
    g_signal_new("add-effect\0",
		 G_TYPE_FROM_CLASS(effect_line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectLineClass, add_effect),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__STRING_STRING,
		 G_TYPE_POINTER, 2,
		 G_TYPE_STRING,
		 G_TYPE_STRING);

  /**
   * AgsEffectLine::remove-effect:
   * @effect_line: the #AgsEffectLine to modify
   * @nth: the nth effect
   *
   * The ::remove-effect signal notifies about removed effect.
   */
  effect_line_signals[REMOVE_EFFECT] =
    g_signal_new("remove-effect\0",
		 G_TYPE_FROM_CLASS(effect_line),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectLineClass, remove_effect),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);
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
  effect_line->flags = 0;

  effect_line->name = NULL;
  
  effect_line->version = AGS_EFFECT_LINE_DEFAULT_VERSION;
  effect_line->build_id = AGS_EFFECT_LINE_DEFAULT_BUILD_ID;

  effect_line->channel = NULL;

  effect_line->label = g_object_new(GTK_TYPE_LABEL,
				    NULL);
  gtk_box_pack_start(GTK_BOX(effect_line),
		     GTK_WIDGET(effect_line->label),
		     FALSE, FALSE,
		     0);

  effect_line->table = gtk_table_new(1, AGS_EFFECT_LINE_COLUMNS_COUNT,
				    TRUE);
  gtk_box_pack_start(GTK_BOX(effect_line),
		     GTK_WIDGET(effect_line->table),
		     FALSE, FALSE,
		     0);
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

  if((AGS_EFFECT_LINE_CONNECTED & (effect_line->flags)) != 0){
    return;
  }
  
  /* channel */
  g_signal_connect_after((GObject *) effect_line->channel, "add-effect\0",
			 G_CALLBACK(ags_effect_line_add_effect_callback), effect_line);

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
}

void
ags_effect_line_disconnect(AgsConnectable *connectable)
{
  AgsEffectLine *effect_line;

  effect_line = AGS_EFFECT_LINE(connectable);

  if((AGS_EFFECT_LINE_CONNECTED & (effect_line->flags)) == 0){
    return;
  }

  //TODO:JK: implement me
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

GList*
ags_effect_line_real_add_effect(AgsEffectLine *effect_line,
				gchar *filename,
				gchar *effect)
{
  AgsLineMember *line_member;
  AgsAddLineMember *add_line_member;
  GtkAdjustment *adjustment;

  AgsLadspaPlugin *ladspa_plugin;
  
  GList *list, *list_start;
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

  list_start = 
    list = effect_line->table->children;

  while(list != NULL){
    if(y <= ((GtkTableChild *) list->data)->top_attach){
      y = ((GtkTableChild *) list->data)->top_attach + 1;
    }

    list = list->next;
  }

  /* find ports */
  recall_start =
    recall = ags_recall_get_by_effect(effect_line->channel->play,
				      filename,
				      effect);
  recall = g_list_last(recall);
  port = AGS_RECALL(recall->data)->port;

  g_list_free(recall_start);

  recall_start = 
    recall = ags_recall_get_by_effect(effect_line->channel->recall,
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

	  if(x == AGS_EFFECT_LINE_COLUMNS_COUNT){
	    x = 0;
	    y++;
	    gtk_table_resize(effect_line->table,
			     y + 1, AGS_EFFECT_LINE_COLUMNS_COUNT);
	  }

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
	    step = -1.0 * (upper_bound + lower_bound) / AGS_DIAL_DEFAULT_PRECISION;
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

	  gtk_table_attach(effect_line->table,
			   line_member,
			   x, x + 1,
			   y, y + 1,
			   GTK_FILL, GTK_FILL,
			   0, 0);

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
ags_effect_line_add_effect(AgsEffectLine *effect_line,
			   gchar *filename,
			   gchar *effect)
{
  GList *port;
  
  g_return_val_if_fail(AGS_IS_EFFECT_LINE(effect_line), NULL);

  g_object_ref((GObject *) effect_line);
  g_signal_emit(G_OBJECT(effect_line),
		effect_line_signals[ADD_EFFECT], 0,
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
  GList *control;
  GList *play_ladspa;
  GList *port;

  play_ladspa = ags_recall_template_find_type(effect_line->channel->play,
					      AGS_TYPE_RECALL_LADSPA);

  /* destroy controls */
  port = AGS_RECALL(g_list_nth(play_ladspa,
			       nth)->data)->port;
    
  while(port != NULL){
    control = gtk_container_get_children(effect_line->table);
      
    while(control != NULL){
      if(AGS_IS_LINE_MEMBER(control->data) &&
	 AGS_LINE_MEMBER(control->data)->port == port->data){
	gtk_widget_destroy(control->data);
	break;
      }
	
      control = control->next;
    }
      
    port = port->next;
  }

  /* remove recalls */
  ags_channel_remove_effect(effect_line->channel,
			    nth);
}

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

/**
 * ags_effect_line_new:
 * @effect_line: the parent effect_line
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
					       "channel\0", channel,
					       NULL);

  return(effect_line);
}
