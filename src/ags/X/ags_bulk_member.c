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

#include <ags/X/ags_bulk_member.h>
#include <ags/X/ags_bulk_member_callbacks.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_devout.h>

#include <ags/widget/ags_dial.h>

#include <ags/X/ags_bulk.h>
#include <ags/X/ags_effect_bulk.h>

void ags_bulk_member_class_init(AgsBulkMemberClass *bulk_member);
void ags_bulk_member_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_bulk_member_init(AgsBulkMember *bulk_member);
void ags_bulk_member_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_bulk_member_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_bulk_member_connect(AgsConnectable *connectable);
void ags_bulk_member_disconnect(AgsConnectable *connectable);
void ags_bulk_member_finalize(GObject *gobject);

void ags_bulk_member_real_change_port(AgsBulkMember *bulk_member,
				      gpointer port_data);

/**
 * SECTION:ags_bulk_member
 * @short_description: Modify assigned recall's port
 * @title: AgsBulkMember
 * @section_id:
 * @include: ags/X/ags_bulk_member.h
 *
 * #AgsBulkMember is a composite widget to modify ports of recalls. A bulk member
 * controls only one specific port of a recall but distinguishes between simple/complex
 * recall. It is generally packed into a #AgsBulk.
 */

enum{
  CHANGE_PORT,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_WIDGET_TYPE,
  PROP_WIDGET_LABEL,
  PROP_PLUGIN_NAME,
  PROP_SPECIFIER,
  PROP_CONTROL_PORT,
  PROP_PORT,
  PROP_PORT_DATA,
  PROP_PORT_DATA_LENGTH,
  PROP_RECALL_PORT,
  PROP_RECALL_PORT_DATA,
  PROP_RECALL_PORT_DATA_LENGTH,
  PROP_TASK_TYPE,
};

static gpointer ags_bulk_member_parent_class = NULL;
static guint bulk_member_signals[LAST_SIGNAL];

GType
ags_bulk_member_get_type(void)
{
  static GType ags_type_bulk_member = 0;

  if(!ags_type_bulk_member){
    static const GTypeInfo ags_bulk_member_info = {
      sizeof(AgsBulkMemberClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_bulk_member_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsBulkMember),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_bulk_member_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_bulk_member_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_bulk_member = g_type_register_static(GTK_TYPE_FRAME,
						  "AgsBulkMember\0", &ags_bulk_member_info,
						  0);

    g_type_add_interface_static(ags_type_bulk_member,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_bulk_member);
}

void
ags_bulk_member_class_init(AgsBulkMemberClass *bulk_member)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_bulk_member_parent_class = g_type_class_peek_parent(bulk_member);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(bulk_member);

  gobject->set_property = ags_bulk_member_set_property;
  gobject->get_property = ags_bulk_member_get_property;

  gobject->finalize = ags_bulk_member_finalize;

  /* properties */
  /**
   * AgsBulkMember:widget-type:
   *
   * The widget type to instantiate and use as control.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_ulong("widget-type\0",
				  "widget type of bulk member\0",
				  "The widget type this bulk member packs\0",
				  0, G_MAXULONG, 
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WIDGET_TYPE,
				  param_spec);

  /**
   * AgsBulkMember:widget-label:
   *
   * The widget's label to use.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_string("widget-label\0",
				   "label to display\0",
				   "The label to display\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WIDGET_LABEL,
				  param_spec);

  /**
   * AgsBulkMember:plugin-name:
   *
   * The plugin name of the recall to use.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_string("plugin-name\0",
				   "plugin name to control\0",
				   "The plugin's name to control\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLUGIN_NAME,
				  param_spec);

  /**
   * AgsBulkMember:specifier:
   *
   * The plugin specifier of the recall to apply.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_string("specifier\0",
				   "port specifier\0",
				   "The specifier of the port\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SPECIFIER,
				  param_spec);

  /**
   * AgsBulkMember:control-port:
   *
   * The control port of the recall.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_string("control-port\0",
				   "control port index\0",
				   "The index of the port to control\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONTROL_PORT,
				  param_spec);

  /**
   * AgsBulkMember:apply:
   *
   * The matching simple port of plugin name and specifier.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("port\0",
				   "port to apply\0",
				   "The port to apply\0",
				   G_TYPE_POINTER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT,
				  param_spec);

  /**
   * AgsBulkMember:port-data:
   *
   * The port data to apply.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_pointer("port-data\0",
				    "port data\0",
				    "The port data\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_DATA,
				  param_spec);

  /**
   * AgsBulkMember:recall-port:
   *
   * The matching complex port of plugin name and specifier.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("recall-port\0",
				   "recall port to apply\0",
				   "The recall port to apply\0",
				   G_TYPE_POINTER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_PORT,
				  param_spec);

  /**
   * AgsBulkMember:recall-port-data:
   *
   * The complex port data to apply.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_pointer("recall-port-data\0",
				    "recall port data\0",
				    "The recall port data\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_PORT_DATA,
				  param_spec);

  /* AgsBulkMember */
  bulk_member->change_port = ags_bulk_member_real_change_port;

  /* signals */
  /**
   * AgsBulkMember::change-port:
   * @bulk_member: the #AgsBulkMember
   * @port_data: the port's data
   *
   * The ::change-port signal notifies modified port.
   */
  bulk_member_signals[CHANGE_PORT] =
    g_signal_new("change-port\0",
		 G_TYPE_FROM_CLASS(bulk_member),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsBulkMemberClass, change_port),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);
}

void
ags_bulk_member_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_bulk_member_connect;
  connectable->disconnect = ags_bulk_member_disconnect;
}

void
ags_bulk_member_init(AgsBulkMember *bulk_member)
{
  GtkWidget *control;

  g_signal_connect_after((GObject *) bulk_member, "parent_set\0",
			 G_CALLBACK(ags_bulk_member_parent_set_callback), (gpointer) bulk_member);

  bulk_member->flags = (AGS_BULK_MEMBER_RESET_BY_ATOMIC |
			AGS_BULK_MEMBER_APPLY_RECALL);

  bulk_member->widget_type = AGS_TYPE_DIAL;
  control = (GtkWidget *) g_object_new(AGS_TYPE_DIAL,
				       "adjustment\0", gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0),
				       NULL);
  gtk_container_add(GTK_CONTAINER(bulk_member),
		    control);

  bulk_member->widget_label = NULL;

  bulk_member->plugin_name = NULL;
  bulk_member->specifier = NULL;

  bulk_member->control_port = NULL;

  bulk_member->port = NULL;
  bulk_member->port_data = NULL;
  bulk_member->active = FALSE;

  bulk_member->recall_port = NULL;
  bulk_member->recall_port_data = NULL;
  bulk_member->recall_active = FALSE;

  bulk_member->task_type = G_TYPE_NONE;
}

void
ags_bulk_member_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsBulkMember *bulk_member;

  bulk_member = AGS_BULK_MEMBER(gobject);

  switch(prop_id){
  case PROP_WIDGET_TYPE:
    {
      GtkWidget *child, *new_child;
      GType widget_type;

      widget_type = g_value_get_ulong(value);

      if(widget_type == bulk_member->widget_type){
	return;
      }

      child = gtk_bin_get_child(GTK_BIN(bulk_member));

      if(child != NULL){
	gtk_widget_destroy(child);
      }

      bulk_member->widget_type = widget_type;
      new_child = (GtkWidget *) g_object_new(widget_type,
					     NULL);

      gtk_container_add(GTK_CONTAINER(bulk_member),
			new_child);
			
    }
    break;
  case PROP_WIDGET_LABEL:
    {
      gchar *label;

      label = g_value_get_string(value);

      if(label == bulk_member->widget_label){
	return;
      }

      bulk_member->widget_label = g_strdup(label);
      ags_bulk_member_set_label(bulk_member, label);
    }
    break;
  case PROP_PLUGIN_NAME:
    {
      gchar *plugin_name;

      plugin_name = g_value_get_string(value);

      if(plugin_name == bulk_member->plugin_name){
	return;
      }

      bulk_member->plugin_name = g_strdup(plugin_name);
    }
    break;
  case PROP_SPECIFIER:
    {
      gchar *specifier;

      specifier = g_value_get_string(value);

      if(specifier == bulk_member->specifier){
	return;
      }

      bulk_member->specifier = g_strdup(specifier);
    }
    break;
  case PROP_CONTROL_PORT:
    {
      gchar *control_port;

      control_port = g_value_get_string(value);

      if(control_port == bulk_member->control_port){
	return;
      }

      bulk_member->control_port = g_strdup(control_port);
    }
    break;
  case PROP_PORT:
    {
      AgsPort *port;

      port = g_value_get_object(value);
      
      if(port != NULL){
	g_object_ref(port);

	if(g_list_find(bulk_member->port, port) == NULL){
	  bulk_member->port = g_list_prepend(bulk_member->port,
					     port);
	}
      }
    }
    break;
  case PROP_PORT_DATA:
    {
      gpointer port_data;

      port_data = g_value_get_pointer(value);

      if(port_data == bulk_member->port_data){
	return;
      }

      bulk_member->port_data = port_data;
    }
    break;
  case PROP_RECALL_PORT:
    {
      AgsPort *port;

      port = g_value_get_object(value);
      
      if(port != NULL){
	g_object_ref(port);

	if(g_list_find(bulk_member->port, port) == NULL){
	  bulk_member->port = g_list_prepend(bulk_member->port,
					     port);
	}
      }
    }
    break;
  case PROP_RECALL_PORT_DATA:
    {
      gpointer port_data;

      port_data = g_value_get_pointer(value);

      if(port_data == bulk_member->recall_port_data){
	return;
      }

      bulk_member->recall_port_data = port_data;
    }
    break;
  case PROP_TASK_TYPE:
    {
      GType type;

      type = g_value_get_ulong(value);
      
      if(bulk_member->task_type == type){
	return;
      }
      
      bulk_member->task_type = type;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_bulk_member_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsBulkMember *bulk_member;

  bulk_member = AGS_BULK_MEMBER(gobject);

  switch(prop_id){
  case PROP_WIDGET_TYPE:
    {
      g_value_set_ulong(value, bulk_member->widget_type);
    }
    break;
  case PROP_WIDGET_LABEL:
    {
      g_value_set_string(value, bulk_member->widget_label);
    }
    break;
  case PROP_PLUGIN_NAME:
    {
      g_value_set_string(value, bulk_member->plugin_name);
    }
    break;
  case PROP_SPECIFIER:
    {
      g_value_set_string(value, bulk_member->specifier);
    }
    break;
  case PROP_CONTROL_PORT:
    {
      g_value_set_string(value, bulk_member->control_port);
    }
    break;
  case PROP_PORT:
    {
      g_value_set_pointer(value, bulk_member->port);
    }
    break;
  case PROP_PORT_DATA:
    {
      g_value_set_pointer(value, bulk_member->port_data);
    }
    break;
  case PROP_RECALL_PORT:
    {
      g_value_set_pointer(value, bulk_member->port);
    }
    break;
  case PROP_RECALL_PORT_DATA:
    {
      g_value_set_pointer(value, bulk_member->port_data);
    }
    break;
  case PROP_TASK_TYPE:
    {
      g_value_set_ulong(value, bulk_member->task_type);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_bulk_member_connect(AgsConnectable *connectable)
{
  AgsBulkMember *bulk_member;
  GtkWidget *control;

  bulk_member = AGS_BULK_MEMBER(connectable);

  control = gtk_bin_get_child(GTK_BIN(bulk_member));

  /* widget callback */
  if(bulk_member->widget_type == AGS_TYPE_DIAL){
    g_signal_connect(GTK_WIDGET(control), "value-changed\0",
		     G_CALLBACK(ags_bulk_member_dial_changed_callback), bulk_member);
  }else if(bulk_member->widget_type == GTK_TYPE_VSCALE){
    g_signal_connect(GTK_WIDGET(control), "value-changed\0",
		     G_CALLBACK(ags_bulk_member_vscale_changed_callback), bulk_member);
  }else if(bulk_member->widget_type == GTK_TYPE_HSCALE){
    g_signal_connect(GTK_WIDGET(control), "value-changed\0",
		     G_CALLBACK(ags_bulk_member_hscale_changed_callback), bulk_member);
  }else if(bulk_member->widget_type == GTK_TYPE_SPIN_BUTTON){
    g_signal_connect(GTK_WIDGET(control), "value-changed\0",
		     G_CALLBACK(ags_bulk_member_spin_button_changed_callback), bulk_member);
  }else if(bulk_member->widget_type == GTK_TYPE_CHECK_BUTTON){
    g_signal_connect(GTK_WIDGET(control), "clicked\0",
		     G_CALLBACK(ags_bulk_member_check_button_clicked_callback), bulk_member);
  }else if(bulk_member->widget_type == GTK_TYPE_TOGGLE_BUTTON){
    g_signal_connect(GTK_WIDGET(control), "clicked\0",
		     G_CALLBACK(ags_bulk_member_toggle_button_clicked_callback), bulk_member);
  }else if(bulk_member->widget_type == GTK_TYPE_BUTTON){
    g_signal_connect(GTK_WIDGET(control), "clicked\0",
		     G_CALLBACK(ags_bulk_member_button_clicked_callback), bulk_member);
  }

  /* port callback */
  if((AGS_BULK_MEMBER_PLAY_CALLBACK_WRITE & (bulk_member->flags)) != 0 &&
     bulk_member->port != NULL){
    g_signal_connect_after(bulk_member->port, "safe-write\0",
			   G_CALLBACK(ags_bulk_member_port_safe_write_callback), bulk_member);
  }

  if((AGS_BULK_MEMBER_RECALL_CALLBACK_WRITE & (bulk_member->flags)) != 0 &&
     bulk_member->recall_port != NULL){
    g_signal_connect_after(bulk_member->recall_port, "safe-write\0",
			   G_CALLBACK(ags_bulk_member_port_safe_write_callback), bulk_member);
  }
}

void
ags_bulk_member_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_bulk_member_finalize(GObject *gobject)
{
  /* empty */
}

GtkWidget*
ags_bulk_member_get_widget(AgsBulkMember *bulk_member)
{
  return(gtk_bin_get_child(bulk_member));
}

/**
 * ags_bulk_member_set_label:
 * @bulk_member: an #AgsBulkMember
 * @label: the label of the control
 *
 * Modify the label of the bulk member.
 */
void
ags_bulk_member_set_label(AgsBulkMember *bulk_member,
			  gchar *label)
{
  GtkWidget *child_widget;

  if(g_type_is_a(bulk_member->widget_type, GTK_TYPE_BUTTON) ||
     bulk_member->widget_type == GTK_TYPE_SPIN_BUTTON){
    child_widget = gtk_bin_get_child(GTK_BIN(bulk_member));

    g_object_set(G_OBJECT(child_widget),
		 "label\0", label,
		 NULL);
  }else{
    GtkLabel *label;

    //TODO:JK: implement me
  }


  bulk_member->widget_label = label;
}

void
ags_bulk_member_real_change_port(AgsBulkMember *bulk_member,
				 gpointer port_data)
{
  if((AGS_BULK_MEMBER_RESET_BY_ATOMIC & (bulk_member->flags)) != 0){
    AgsPort *port;
    GList *list;
    GValue value = {0,};

    list = bulk_member->port;

    while(list != NULL){
      port = AGS_BULK_PORT(list->data)->port;
      
      if(!port->port_value_is_pointer){
	if(port->port_value_type == G_TYPE_BOOLEAN){
	  g_value_init(&value,
		       G_TYPE_BOOLEAN);

	  g_value_set_boolean(&value,
			      ((gboolean *) port_data)[0]);
	}else if(port->port_value_type == G_TYPE_INT64){
	  g_value_init(&value,
		       G_TYPE_INT64);
	  g_value_set_int64(&value,
			    ((gint *) port_data)[0]);
	}else if(port->port_value_type == G_TYPE_UINT64){
	  g_value_init(&value,
		       G_TYPE_UINT64);

	  g_value_set_uint64(&value,
			     ((guint *) port_data)[0]);
	}else if(port->port_value_type == G_TYPE_FLOAT){
	  g_value_init(&value,
		       G_TYPE_DOUBLE);

	  g_value_set_double(&value,
			     ((gdouble *) port_data)[0]);
	}else if(port->port_value_type == G_TYPE_DOUBLE){
	  g_value_init(&value,
		       G_TYPE_DOUBLE);

	  g_value_set_double(&value,
			     ((gdouble *) port_data)[0]);
	}
      }else{
	if(port->port_value_type == G_TYPE_OBJECT){
	  g_value_init(&value,
		       G_TYPE_OBJECT);
	  g_value_set_object(&value,
			     port_data);
	}else{
	  if(port->port_value_type == G_TYPE_BOOLEAN ||
	     port->port_value_type == G_TYPE_INT64 ||
	     port->port_value_type == G_TYPE_UINT64 ||
	     port->port_value_type == G_TYPE_FLOAT ||
	     port->port_value_type == G_TYPE_DOUBLE ||
	     port->port_value_type == G_TYPE_POINTER){
	    g_value_init(&value,
			 G_TYPE_POINTER);

	    g_value_set_pointer(&value,
				port_data);

	  }
	}
      }

      ags_port_safe_write(bulk_member->port,
			  &value);

      if((AGS_BULK_MEMBER_APPLY_RECALL & (bulk_member->flags)) != 0){
	ags_port_safe_write(AGS_BULK_PORT(list->data)->recall_port,
			    &value);
      }

      list = list->next;
    }
  }

  if((AGS_BULK_MEMBER_RESET_BY_TASK & (bulk_member->flags)) != 0){
    AgsEffectBulk *effect_bulk;
    AgsTaskThread *task_thread;
    AgsTask *task;

    effect_bulk = (AgsEffectBulk *) gtk_widget_get_ancestor(GTK_WIDGET(bulk_member),
							    AGS_TYPE_EFFECT_BULK);
    
    task_thread = AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(AGS_DEVOUT(effect_bulk->audio->devout)->ags_main)->main_loop)->task_thread);

    task = (AgsTask *) g_object_new(bulk_member->task_type,
				    bulk_member->control_port, port_data,
				    NULL);

    ags_task_thread_append_task(task_thread,
				task);
  }
}

/**
 * ags_bulk_change_port:
 * @bulk_member: an #AgsBulkMember
 * @port_data: the port's value
 *
 * Is emitted as port's value is modified.
 *
 * Since: 0.4
 */
void
ags_bulk_member_change_port(AgsBulkMember *bulk_member,
			    gpointer port_data)
{
  g_return_if_fail(AGS_IS_BULK_MEMBER(bulk_member));

  g_object_ref((GObject *) bulk_member);
  g_signal_emit(G_OBJECT(bulk_member),
		bulk_member_signals[CHANGE_PORT], 0,
		port_data);
  g_object_unref((GObject *) bulk_member);
}

/**
 * ags_bulk_member_find_port:
 * @bulk_member: an #AgsBulkMember
 *
 * Lookup ports of assigned recall.
 *
 * Since: 0.4
 */
void
ags_bulk_member_find_port(AgsBulkMember *bulk_member)
{
  GtkWidget *effect_bulk;
  AgsAudio *audio;
  AgsChannel *channel;
  AgsPort *audio_port, *channel_port;
  AgsPort *recall_audio_port, *recall_channel_port;
  GList *recall;
  gchar *specifier;

  auto AgsPort* ags_bulk_member_find_specifier(GList *recall);

  AgsPort* ags_bulk_member_find_specifier(GList *recall){
    GList *port;
    
    while(recall != NULL){
      port = AGS_RECALL(recall->data)->port;

#ifdef AGS_DEBUG
      g_message("search port in %s\0", G_OBJECT_TYPE_NAME(recall->data));
#endif

      while(port != NULL){
	if(!g_strcmp0(AGS_PORT(port->data)->specifier,
		      specifier)){
	  return(AGS_PORT(port->data));
	}

	port = port->next;
      }

      recall = recall->next;
    }

    return(NULL);
  }


  if(bulk_member == NULL){
    return;
  }

  specifier = bulk_member->specifier;

  if(!AGS_IS_BULK_MEMBER(bulk_member) || specifier == NULL){
    return;
  }

  effect_bulk = gtk_widget_get_ancestor(GTK_WIDGET(bulk_member),
					AGS_TYPE_EFFECT_BULK);

  audio = AGS_EFFECT_BULK(effect_bulk)->audio;
  
  audio_port = NULL;
  channel_port = NULL;
  
  recall_audio_port = NULL;
  recall_channel_port = NULL;
  
  /* search channels */
  channel = NULL;
  
  if(effect_bulk->channel_type == AGS_TYPE_OUTPUT){
    channel = audio->output;
  }else if(effect_bulk->channel_type == AGS_TYPE_INPUT){
    channel = audio->input;
  }

  while(channel != NULL){
    recall = channel->play;
    channel_port = ags_bulk_member_find_specifier(recall);

    recall = channel->recall;
    recall_channel_port = ags_bulk_member_find_specifier(recall);

    if(channel_port != NULL){
      g_object_set(G_OBJECT(bulk_member),
		   "port\0", channel_port,
		   NULL);

      g_object_set(G_OBJECT(bulk_member),
		   "recall-port\0", recall_channel_port,
		   NULL);
    }

    channel = channel->next;
  }
  
  /* search audio */
  if(channel_port == NULL){
    recall = audio->play;
    audio_port = ags_bulk_member_find_specifier(recall);

    recall = audio->recall;
    recall_audio_port = ags_bulk_member_find_specifier(recall);

    if(audio_port != NULL){
      g_object_set(G_OBJECT(bulk_member),
		   "port\0", audio_port,
		   NULL);

      g_object_set(G_OBJECT(bulk_member),
		   "recall-port\0", recall_audio_port,
		   NULL);
    }
  }
}

/**
 * ags_bulk_member_new:
 *
 * Creates an #AgsBulkMember
 *
 * Returns: a new #AgsBulkMember
 *
 * Since: 0.4
 */
AgsBulkMember*
ags_bulk_member_new()
{
  AgsBulkMember *bulk_member;

  bulk_member = (AgsBulkMember *) g_object_new(AGS_TYPE_BULK_MEMBER,
					       NULL);

  return(bulk_member);
}
