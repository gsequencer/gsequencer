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

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_marshal.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

#include <ags/widget/ags_dial.h>

#include <ags/X/ags_window.h>
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
GList* ags_bulk_member_real_find_port(AgsBulkMember *bulk_member);

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
  FIND_PORT,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_WIDGET_TYPE,
  PROP_WIDGET_LABEL,
  PROP_PLUGIN_NAME,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_SPECIFIER,
  PROP_CONTROL_PORT,
  PROP_STEPS,
  PROP_TASK_TYPE,
  PROP_BULK_PORT,
  PROP_RECALL_BULK_PORT,
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
   * Since: 0.4.3
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
   * Since: 0.4.3
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
   * Since: 0.4.3
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
   * Since: 0.4.3
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
   * AgsBulkMember:filename:
   *
   * The plugin filename of the recall to apply.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_string("filename\0",
				   "the filename\0",
				   "The filename of the plugin\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

    /**
   * AgsBulkMember:effect:
   *
   * The plugin effect of the recall to apply.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_string("effect\0",
				   "the effect\0",
				   "The effect of the plugin\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT,
				  param_spec);

  /**
   * AgsBulkMember:control-port:
   *
   * The control port of the recall.
   * 
   * Since: 0.4.3
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
   * AgsBulkMember:steps:
   *
   * If bulk member has integer ports, this is the number of steps.
   * 
   * Since: 0.7.21
   */
  param_spec = g_param_spec_uint("steps\0",
				 "steps of bulk members port\0",
				 "The steps this bulk members port has\0",
				 0,
				 G_MAXUINT,
				 AGS_DIAL_DEFAULT_PRECISION,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_STEPS,
				  param_spec);

  /**
   * AgsBulkMember:bulk-port:
   *
   * The playback bulk port to be added.
   * 
   * Since: 0.7.8
   */
  param_spec = g_param_spec_object("bulk-port\0",
				   "a bulk port\0",
				   "The bulk port to add\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BULK_PORT,
				  param_spec);

  /**
   * AgsBulkMember:recall-bulk-port:
   *
   * The recall bulk port to be added.
   * 
   * Since: 0.7.8
   */
  param_spec = g_param_spec_object("recall-bulk-port\0",
				   "a recall bulk port\0",
				   "The bulk port to add\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_BULK_PORT,
				  param_spec);

  /**
   * AgsBulkMember:task-type:
   *
   * The task type to apply the ports.
   * 
   * Since: 0.7.8
   */
  param_spec = g_param_spec_ulong("task-type\0",
				  "task type to apply\0",
				  "The task type to apply the ports\0",
				  0, G_MAXULONG, 
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TASK_TYPE,
				  param_spec);

  /* AgsBulkMember */
  bulk_member->change_port = ags_bulk_member_real_change_port;
  bulk_member->find_port = ags_bulk_member_real_find_port;

  /* signals */
  /**
   * AgsBulkMember::change-port:
   * @bulk_member: the #AgsBulkMember
   * @port_data: the port's data
   *
   * The ::change-port signal notifies modified port.
   *
   * Since: 0.7.2
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

  /**
   * AgsBulkMember::find-port:
   * @bulk_member: the #AgsBulkMember to resize
   *
   * The ::find-port as recall should be mapped
   *
   * Returns: a #GList with associated ports
   *
   * Since: 0.7.2
   */
  bulk_member_signals[FIND_PORT] =
    g_signal_new("find-port\0",
		 G_TYPE_FROM_CLASS(bulk_member),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsBulkMemberClass, find_port),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);
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
  AgsDial *dial;
  
  g_signal_connect_after((GObject *) bulk_member, "parent_set\0",
			 G_CALLBACK(ags_bulk_member_parent_set_callback), (gpointer) bulk_member);

  bulk_member->flags = (AGS_BULK_MEMBER_RESET_BY_ATOMIC |
			AGS_BULK_MEMBER_APPLY_RECALL);
  bulk_member->port_flags = 0;
  
  bulk_member->widget_type = AGS_TYPE_DIAL;
  dial = (GtkWidget *) g_object_new(AGS_TYPE_DIAL,
				    "adjustment\0", gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0),
				    NULL);
  gtk_widget_set_size_request(dial,
			      2 * (dial->radius + dial->outline_strength + dial->button_width + 4),
			      2 * (dial->radius + dial->outline_strength + 1));
  
  gtk_container_add(GTK_CONTAINER(bulk_member),
		    dial);

  bulk_member->widget_label = NULL;

  bulk_member->plugin_name = NULL;
  
  bulk_member->filename = NULL;
  bulk_member->effect = NULL;
  bulk_member->specifier = NULL;

  bulk_member->control_port = NULL;
  bulk_member->steps = 0;

  bulk_member->conversion = NULL;
  
  bulk_member->bulk_port = NULL;
  bulk_member->recall_bulk_port = NULL;

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

      if(AGS_IS_DIAL(new_child)){
	AgsDial *dial;
	
	dial = new_child;
	
	gtk_widget_set_size_request(dial,
				    2 * (dial->radius + dial->outline_strength + dial->button_width + 4),
				    2 * (dial->radius + dial->outline_strength + 1));
      }

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
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == bulk_member->filename){
	return;
      }

      bulk_member->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;

      effect = g_value_get_string(value);

      if(effect == bulk_member->effect){
	return;
      }

      bulk_member->effect = g_strdup(effect);
    }
    break;
  case PROP_SPECIFIER:
    {
      gchar *specifier;

      specifier = g_value_get_string(value);

      if(specifier == bulk_member->specifier){
	return;
      }

      if(bulk_member->specifier != NULL){
	g_free(bulk_member->specifier);
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
  case PROP_STEPS:
    {
      GtkWidget *child;
      
      guint steps;

      steps = g_value_get_uint(value);

      bulk_member->steps = steps;
      child = gtk_bin_get_child(GTK_BIN(bulk_member));

      if(AGS_IS_DIAL(child)){
	g_object_set(child,
		     "scale-precision\0", steps,
		     NULL);
      }
    }
    break;
  case PROP_BULK_PORT:
    {
      AgsPort *port;
      AgsBulkPort *bulk_port;
      
      port = (AgsPort *) g_value_get_object(value);

      if(ags_bulk_port_find(bulk_member->bulk_port, port) != NULL){
	return;
      }

      g_object_ref(port);
      bulk_port = ags_bulk_port_alloc(port);
      bulk_member->bulk_port = g_list_prepend(bulk_member->bulk_port,
					      bulk_port);
    }
    break;
  case PROP_RECALL_BULK_PORT:
    {
      AgsPort *port;
      AgsBulkPort *bulk_port;
      
      port = (AgsPort *) g_value_get_object(value);

      if(ags_bulk_port_find(bulk_member->recall_bulk_port, port) != NULL){
	return;
      }

      g_object_ref(port);
      bulk_port = ags_bulk_port_alloc(port);
      bulk_member->recall_bulk_port = g_list_prepend(bulk_member->recall_bulk_port,
						     bulk_port);
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
  case PROP_FILENAME:
    {
      g_value_set_string(value, bulk_member->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, bulk_member->effect);
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

  if((AGS_BULK_MEMBER_CONNECTED & (bulk_member->flags)) != 0){
    return;
  }

  bulk_member->flags |= AGS_BULK_MEMBER_CONNECTED;
  
  ags_bulk_member_find_port(bulk_member);
  
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
 * ags_bulk_port_alloc:
 * @port: the #AgsPort to set
 * 
 * Allocate #AgsBulkPort-struct.
 *
 * Returns: the newly allocated #AgsBulkPort-struct
 *
 * Since: 0.7.8
 */
AgsBulkPort*
ags_bulk_port_alloc(AgsPort *port)
{
  AgsBulkPort *bulk_port;

  bulk_port = (AgsBulkPort *) malloc(sizeof(AgsBulkPort));

  bulk_port->port = port;
  bulk_port->port_data = &(port->port_value);
  bulk_port->active = FALSE;
  
  return(bulk_port);
}

/**
 * ags_bulk_port_find:
 * @list: the #GList-struct to search
 * @port: the #AgsPort to find
 * 
 * Find port within @list.
 *
 * Returns: the #GList-struct containing port if found otherwise %NULL
 *
 * Since: 0.7.8
 */
GList*
ags_bulk_port_find(GList *list, AgsPort *port)
{
  while(list != NULL){
    if(AGS_BULK_PORT(list->data)->port == port){
      break;
    }
    
    list = list->next;
  }

  return(list);
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

  if(g_type_is_a(bulk_member->widget_type, GTK_TYPE_BUTTON)){
    child_widget = gtk_bin_get_child(GTK_BIN(bulk_member));

    g_object_set(G_OBJECT(child_widget),
		 "label\0", label,
		 NULL);
  }else{
    gtk_frame_set_label_widget(bulk_member,
			       g_object_new(GTK_TYPE_LABEL,
					    "wrap\0", TRUE,
					    "wrap-mode\0", PANGO_WRAP_CHAR,
					    "use-markup\0", TRUE,
					    "label", g_strdup_printf("<small>%s</small>", label),
					    NULL));
  }


  bulk_member->widget_label = label;
}

void
ags_bulk_member_real_change_port(AgsBulkMember *bulk_member,
				 gpointer port_data)
{
  AgsWindow *window;

  AgsThread *main_loop;
  AgsTaskThread *task_thread;
  
  AgsApplicationContext *application_context;

  auto void ags_bulk_member_real_change_port_iter(GList *list);

  void ags_bulk_member_real_change_port_iter(GList *list){
    AgsPort *port;
    
    while(list != NULL){
      GValue value = {0,};

      port = AGS_BULK_PORT(list->data)->port;
      
      pthread_mutex_lock(port->mutex);

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
	  gfloat val;
	  
	  val = ((gdouble *) port_data)[0];
	  
	  if(bulk_member->conversion != NULL){
	    gfloat upper, lower, range, step;
	    gfloat c_upper, c_lower, c_range;

	    gboolean success;

	    success = FALSE;
	    
	    if(AGS_IS_DIAL(gtk_bin_get_child(bulk_member))){
	      AgsDial *dial;

	      dial = gtk_bin_get_child(bulk_member);

	      upper = dial->adjustment->upper;
	      lower = dial->adjustment->lower;

	      success = TRUE;
	    }else{
	      g_warning("unsupported child type in conversion\0");
	    }

	    if(success){
	      range = upper - lower;
	      step = range / val;

	      val = ags_conversion_convert(bulk_member->conversion,
					   val,
					   FALSE);
	      c_upper = ags_conversion_convert(bulk_member->conversion,
					       upper,
					       FALSE);
	      c_lower = ags_conversion_convert(bulk_member->conversion,
					       lower,
					       FALSE);
	      c_range = c_upper - c_lower;
	    
	      val = ags_conversion_convert(bulk_member->conversion,
					   c_lower + (c_range / step),
					   TRUE);
	    }
	  }
	  
	  g_value_init(&value,
		       G_TYPE_FLOAT);

	  g_value_set_float(&value,
			     val);
	}else if(port->port_value_type == G_TYPE_DOUBLE){
	  gdouble val;
	  
	  val = ((gdouble *) port_data)[0];
	  
	  if(bulk_member->conversion != NULL){
	    gdouble upper, lower, range, step;
	    gdouble c_upper, c_lower, c_range;

	    gboolean success;

	    success = FALSE;
	    
	    if(AGS_IS_DIAL(gtk_bin_get_child(bulk_member))){
	      AgsDial *dial;

	      dial = gtk_bin_get_child(bulk_member);

	      upper = dial->adjustment->upper;
	      lower = dial->adjustment->lower;

	      success = TRUE;
	    }else{
	      g_warning("unsupported child type in conversion\0");
	    }

	    if(success){
	      range = upper - lower;
	      step = range / val;

	      val = ags_conversion_convert(bulk_member->conversion,
					   val,
					   FALSE);
	      c_upper = ags_conversion_convert(bulk_member->conversion,
					       upper,
					       FALSE);
	      c_lower = ags_conversion_convert(bulk_member->conversion,
					       lower,
					       FALSE);
	      c_range = c_upper - c_lower;

	      val = ags_conversion_convert(bulk_member->conversion,
					   c_lower + (c_range / step),
					   TRUE);
	    }
	  }

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

      pthread_mutex_unlock(port->mutex);
      
      //      g_message("change %f\0", g_value_get_float(&value));
      ags_port_safe_write(port,
			  &value);

      list = list->next;
    }
  }
  
  window = gtk_widget_get_ancestor(bulk_member,
				   AGS_TYPE_WINDOW);
  
  application_context = window->application_context;

  main_loop = application_context->main_loop;

  task_thread = ags_thread_find_type(main_loop,
				     AGS_TYPE_TASK_THREAD);
  
  if((AGS_BULK_MEMBER_RESET_BY_ATOMIC & (bulk_member->flags)) != 0){
    ags_bulk_member_real_change_port_iter(bulk_member->bulk_port);

    if((AGS_BULK_MEMBER_APPLY_RECALL & (bulk_member->flags)) != 0){
      ags_bulk_member_real_change_port_iter(bulk_member->recall_bulk_port);
    }
  }

  if((AGS_BULK_MEMBER_RESET_BY_TASK & (bulk_member->flags)) != 0){
    AgsEffectBulk *effect_bulk;
    AgsTaskThread *task_thread;
    AgsTask *task;

    effect_bulk = (AgsEffectBulk *) gtk_widget_get_ancestor(GTK_WIDGET(bulk_member),
							    AGS_TYPE_EFFECT_BULK);
    
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
 * Since: 0.4.3
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

GList*
ags_bulk_member_real_find_port(AgsBulkMember *bulk_member)
{
  GtkWidget *effect_bulk;

  AgsAudio *audio;
  AgsChannel *channel;
  AgsPort *audio_port, *channel_port;
  AgsPort *recall_audio_port, *recall_channel_port;
  
  gchar *specifier;

  auto AgsPort* ags_bulk_member_find_specifier(GList *recall);

  AgsPort* ags_bulk_member_find_specifier(GList *recall){
    GList *port;
    
    while(recall != NULL){
      if((AGS_RECALL_BULK_MODE & (AGS_RECALL(recall->data)->flags)) == 0){
	recall = recall->next;

	continue;
      }
      
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


  if(bulk_member == NULL ||
     !AGS_IS_BULK_MEMBER(bulk_member)){
    return;
  }

  specifier = bulk_member->specifier;

  if(specifier == NULL){
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
  
  if(AGS_EFFECT_BULK(effect_bulk)->channel_type == AGS_TYPE_OUTPUT){
    channel = audio->output;
  }else if(AGS_EFFECT_BULK(effect_bulk)->channel_type == AGS_TYPE_INPUT){
    channel = audio->input;
  }

  while(channel != NULL){
    channel_port = ags_bulk_member_find_specifier(channel->play);

    if(channel_port != NULL &&
       ags_bulk_port_find(bulk_member->bulk_port, channel_port) == NULL){
      bulk_member->bulk_port = g_list_prepend(bulk_member->bulk_port,
					      ags_bulk_port_alloc(channel_port));
    }

    recall_channel_port = ags_bulk_member_find_specifier(channel->recall);
    
    if(recall_channel_port != NULL &&
       ags_bulk_port_find(bulk_member->recall_bulk_port, recall_channel_port) == NULL){
      bulk_member->recall_bulk_port = g_list_prepend(bulk_member->recall_bulk_port,
						     ags_bulk_port_alloc(recall_channel_port));
    }
    
    channel = channel->next;
  }

  /* search audio */
  if(channel_port == NULL &&
     recall_channel_port == NULL){
    audio_port = ags_bulk_member_find_specifier(audio->play);

    if(audio_port != NULL &&
       ags_bulk_port_find(bulk_member->bulk_port, audio_port) == NULL){
      bulk_member->bulk_port = g_list_prepend(bulk_member->bulk_port,
					      ags_bulk_port_alloc(audio_port));
    }
    
    recall_audio_port = ags_bulk_member_find_specifier(audio->recall);
    
    if(recall_audio_port != NULL &&
       ags_bulk_port_find(bulk_member->recall_bulk_port, recall_audio_port) == NULL){
      bulk_member->recall_bulk_port = g_list_prepend(bulk_member->recall_bulk_port,
						     ags_bulk_port_alloc(recall_audio_port));
    }
  }

  return(g_list_copy(bulk_member->bulk_port));
}

/**
 * ags_bulk_member_find_port:
 * @bulk_member: an #AgsBulkMember
 *
 * Lookup ports of assigned recalls.
 *
 * Returns: an #GList containing all related #AgsPort
 *
 * Since: 0.7.8
 */
GList*
ags_bulk_member_find_port(AgsBulkMember *bulk_member)
{
  GList *list;

  list = NULL;
  g_return_val_if_fail(AGS_IS_BULK_MEMBER(bulk_member),
		       NULL);

  g_object_ref((GObject *) bulk_member);
  g_signal_emit((GObject *) bulk_member,
		bulk_member_signals[FIND_PORT], 0,
		&list);
  g_object_unref((GObject *) bulk_member);

  return(list);
}

/**
 * ags_bulk_member_new:
 *
 * Creates an #AgsBulkMember
 *
 * Returns: a new #AgsBulkMember
 *
 * Since: 0.4.3
 */
AgsBulkMember*
ags_bulk_member_new()
{
  AgsBulkMember *bulk_member;

  bulk_member = (AgsBulkMember *) g_object_new(AGS_TYPE_BULK_MEMBER,
					       NULL);

  return(bulk_member);
}
