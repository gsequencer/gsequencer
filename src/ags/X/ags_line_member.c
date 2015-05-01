/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/X/ags_line_member.h>
#include <ags/X/ags_line_member_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>

#include <ags/widget/ags_dial.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_line.h>
#include <ags/X/ags_effect_line.h>

void ags_line_member_class_init(AgsLineMemberClass *line_member);
void ags_line_member_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_line_member_init(AgsLineMember *line_member);
void ags_line_member_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_line_member_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_line_member_connect(AgsConnectable *connectable);
void ags_line_member_disconnect(AgsConnectable *connectable);
void ags_line_member_finalize(GObject *gobject);

void ags_line_member_real_change_port(AgsLineMember *line_member,
				      gpointer port_data);

/**
 * SECTION:ags_line_member
 * @short_description: Modify assigned recall's port
 * @title: AgsLineMember
 * @section_id:
 * @include: ags/X/ags_line_member.h
 *
 * #AgsLineMember is a composite widget to modify ports of recalls. A line member
 * controls only one specific port of a recall but distinguishes between simple/complex
 * recall. It is generally packed into a #AgsLine.
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

static gpointer ags_line_member_parent_class = NULL;
static guint line_member_signals[LAST_SIGNAL];

GType
ags_line_member_get_type(void)
{
  static GType ags_type_line_member = 0;

  if(!ags_type_line_member){
    static const GTypeInfo ags_line_member_info = {
      sizeof(AgsLineMemberClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_line_member_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLineMember),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_line_member_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_line_member_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_line_member = g_type_register_static(GTK_TYPE_FRAME,
						  "AgsLineMember\0", &ags_line_member_info,
						  0);

    g_type_add_interface_static(ags_type_line_member,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_line_member);
}

void
ags_line_member_class_init(AgsLineMemberClass *line_member)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_line_member_parent_class = g_type_class_peek_parent(line_member);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(line_member);

  gobject->set_property = ags_line_member_set_property;
  gobject->get_property = ags_line_member_get_property;

  gobject->finalize = ags_line_member_finalize;

  /* properties */
  /**
   * AgsLineMember:widget-type:
   *
   * The widget type to instantiate and use as control.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_ulong("widget-type\0",
				  "widget type of line member\0",
				  "The widget type this line member packs\0",
				  0, G_MAXULONG, 
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WIDGET_TYPE,
				  param_spec);

  /**
   * AgsLineMember:widget-label:
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
   * AgsLineMember:plugin-name:
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
   * AgsLineMember:specifier:
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
   * AgsLineMember:control-port:
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
   * AgsLineMember:apply:
   *
   * The matching simple port of plugin name and specifier.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("port\0",
				   "port to apply\0",
				   "The port to apply\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT,
				  param_spec);

  /**
   * AgsLineMember:port-data:
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
   * AgsLineMember:recall-port:
   *
   * The matching complex port of plugin name and specifier.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("recall-port\0",
				   "recall port to apply\0",
				   "The recall port to apply\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_PORT,
				  param_spec);

  /**
   * AgsLineMember:recall-port-data:
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

  /* AgsLineMember */
  line_member->change_port = ags_line_member_real_change_port;

  /* signals */
  /**
   * AgsLineMember::change-port:
   * @line_member: the #AgsLineMember
   * @port_data: the port's data
   *
   * The ::change-port signal notifies modified port.
   */
  line_member_signals[CHANGE_PORT] =
    g_signal_new("change-port\0",
		 G_TYPE_FROM_CLASS(line_member),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineMemberClass, change_port),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);
}

void
ags_line_member_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_line_member_connect;
  connectable->disconnect = ags_line_member_disconnect;
}

void
ags_line_member_init(AgsLineMember *line_member)
{
  GtkWidget *control;

  g_signal_connect_after((GObject *) line_member, "parent_set\0",
			 G_CALLBACK(ags_line_member_parent_set_callback), (gpointer) line_member);

  line_member->flags = (AGS_LINE_MEMBER_RESET_BY_ATOMIC |
			AGS_LINE_MEMBER_APPLY_RECALL);

  line_member->widget_type = AGS_TYPE_DIAL;
  control = (GtkWidget *) g_object_new(AGS_TYPE_DIAL,
				       "adjustment\0", gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0),
				       NULL);
  gtk_container_add(GTK_CONTAINER(line_member),
		    control);

  line_member->widget_label = NULL;

  line_member->plugin_name = NULL;
  line_member->specifier = NULL;

  line_member->control_port = NULL;

  line_member->port = NULL;
  line_member->port_data = NULL;
  line_member->active = FALSE;

  line_member->recall_port = NULL;
  line_member->recall_port_data = NULL;
  line_member->recall_active = FALSE;

  line_member->task_type = G_TYPE_NONE;
}

void
ags_line_member_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsLineMember *line_member;

  line_member = AGS_LINE_MEMBER(gobject);

  switch(prop_id){
  case PROP_WIDGET_TYPE:
    {
      GtkWidget *child, *new_child;
      GType widget_type;

      widget_type = g_value_get_ulong(value);

      if(widget_type == line_member->widget_type){
	return;
      }

      child = gtk_bin_get_child(GTK_BIN(line_member));

      if(child != NULL){
	gtk_widget_destroy(child);
      }

      line_member->widget_type = widget_type;
      new_child = (GtkWidget *) g_object_new(widget_type,
					     NULL);

      gtk_container_add(GTK_CONTAINER(line_member),
			new_child);
			
    }
    break;
  case PROP_WIDGET_LABEL:
    {
      gchar *label;

      label = g_value_get_string(value);

      if(label == line_member->widget_label){
	return;
      }

      line_member->widget_label = g_strdup(label);
      ags_line_member_set_label(line_member, label);
    }
    break;
  case PROP_PLUGIN_NAME:
    {
      gchar *plugin_name;

      plugin_name = g_value_get_string(value);

      if(plugin_name == line_member->plugin_name){
	return;
      }

      line_member->plugin_name = g_strdup(plugin_name);
    }
    break;
  case PROP_SPECIFIER:
    {
      gchar *specifier;

      specifier = g_value_get_string(value);

      if(specifier == line_member->specifier){
	return;
      }

      line_member->specifier = g_strdup(specifier);
    }
    break;
  case PROP_CONTROL_PORT:
    {
      gchar *control_port;

      control_port = g_value_get_string(value);

      if(control_port == line_member->control_port){
	return;
      }

      line_member->control_port = g_strdup(control_port);
    }
    break;
  case PROP_PORT:
    {
      AgsPort *port;

      port = g_value_get_object(value);
      
      if(port == line_member->port){
	return;
      }
      
      if(line_member->port != NULL){
	g_object_unref(line_member->port);
      }

      if(port != NULL){
	g_object_ref(port);
      }

      line_member->port = port;
    }
    break;
  case PROP_PORT_DATA:
    {
      gpointer port_data;

      port_data = g_value_get_pointer(value);

      if(port_data == line_member->port_data){
	return;
      }

      line_member->port_data = port_data;
    }
    break;
  case PROP_RECALL_PORT:
    {
      AgsPort *port;

      port = g_value_get_object(value);
      
      if(port == line_member->recall_port){
	return;
      }
      
      if(line_member->recall_port != NULL){
	g_object_unref(line_member->recall_port);
      }

      if(port != NULL){
	g_object_ref(port);
      }

      line_member->recall_port = port;
    }
    break;
  case PROP_RECALL_PORT_DATA:
    {
      gpointer port_data;

      port_data = g_value_get_pointer(value);

      if(port_data == line_member->recall_port_data){
	return;
      }

      line_member->recall_port_data = port_data;
    }
    break;
  case PROP_TASK_TYPE:
    {
      GType type;

      type = g_value_get_ulong(value);
      
      if(line_member->task_type == type){
	return;
      }
      
      line_member->task_type = type;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_line_member_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsLineMember *line_member;

  line_member = AGS_LINE_MEMBER(gobject);

  switch(prop_id){
  case PROP_WIDGET_TYPE:
    {
      g_value_set_ulong(value, line_member->widget_type);
    }
    break;
  case PROP_WIDGET_LABEL:
    {
      g_value_set_string(value, line_member->widget_label);
    }
    break;
  case PROP_PLUGIN_NAME:
    {
      g_value_set_string(value, line_member->plugin_name);
    }
    break;
  case PROP_SPECIFIER:
    {
      g_value_set_string(value, line_member->specifier);
    }
    break;
  case PROP_CONTROL_PORT:
    {
      g_value_set_string(value, line_member->control_port);
    }
    break;
  case PROP_PORT:
    {
      g_value_set_object(value, line_member->port);
    }
    break;
  case PROP_PORT_DATA:
    {
      g_value_set_pointer(value, line_member->port_data);
    }
    break;
  case PROP_RECALL_PORT:
    {
      g_value_set_object(value, line_member->port);
    }
    break;
  case PROP_RECALL_PORT_DATA:
    {
      g_value_set_pointer(value, line_member->port_data);
    }
    break;
  case PROP_TASK_TYPE:
    {
      g_value_set_ulong(value, line_member->task_type);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_line_member_connect(AgsConnectable *connectable)
{
  AgsLineMember *line_member;
  GtkWidget *control;

  line_member = AGS_LINE_MEMBER(connectable);

  control = gtk_bin_get_child(GTK_BIN(line_member));

  /* widget callback */
  if(line_member->widget_type == AGS_TYPE_DIAL){
    g_signal_connect(GTK_WIDGET(control), "value-changed\0",
		     G_CALLBACK(ags_line_member_dial_changed_callback), line_member);
  }else if(line_member->widget_type == GTK_TYPE_VSCALE){
    g_signal_connect(GTK_WIDGET(control), "value-changed\0",
		     G_CALLBACK(ags_line_member_vscale_changed_callback), line_member);
  }else if(line_member->widget_type == GTK_TYPE_HSCALE){
    g_signal_connect(GTK_WIDGET(control), "value-changed\0",
		     G_CALLBACK(ags_line_member_hscale_changed_callback), line_member);
  }else if(line_member->widget_type == GTK_TYPE_SPIN_BUTTON){
    g_signal_connect(GTK_WIDGET(control), "value-changed\0",
		     G_CALLBACK(ags_line_member_spin_button_changed_callback), line_member);
  }else if(line_member->widget_type == GTK_TYPE_CHECK_BUTTON){
    g_signal_connect(GTK_WIDGET(control), "clicked\0",
		     G_CALLBACK(ags_line_member_check_button_clicked_callback), line_member);
  }else if(line_member->widget_type == GTK_TYPE_TOGGLE_BUTTON){
    g_signal_connect(GTK_WIDGET(control), "clicked\0",
		     G_CALLBACK(ags_line_member_toggle_button_clicked_callback), line_member);
  }else if(line_member->widget_type == GTK_TYPE_BUTTON){
    g_signal_connect(GTK_WIDGET(control), "clicked\0",
		     G_CALLBACK(ags_line_member_button_clicked_callback), line_member);
  }

  /* port callback */
  if((AGS_LINE_MEMBER_PLAY_CALLBACK_WRITE & (line_member->flags)) != 0 &&
     line_member->port != NULL){
    g_signal_connect_after(line_member->port, "safe-write\0",
			   G_CALLBACK(ags_line_member_port_safe_write_callback), line_member);
  }

  if((AGS_LINE_MEMBER_RECALL_CALLBACK_WRITE & (line_member->flags)) != 0 &&
     line_member->recall_port != NULL){
    g_signal_connect_after(line_member->recall_port, "safe-write\0",
			   G_CALLBACK(ags_line_member_port_safe_write_callback), line_member);
  }
}

void
ags_line_member_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_line_member_finalize(GObject *gobject)
{
  /* empty */
}

GtkWidget*
ags_line_member_get_widget(AgsLineMember *line_member)
{
  return(gtk_bin_get_child(line_member));
}

/**
 * ags_line_member_set_label:
 * @line_member: an #AgsLineMember
 * @label: the label of the control
 *
 * Modify the label of the line member.
 */
void
ags_line_member_set_label(AgsLineMember *line_member,
			  gchar *label)
{
  GtkWidget *child_widget;

  if(g_type_is_a(line_member->widget_type, GTK_TYPE_BUTTON) ||
     line_member->widget_type == GTK_TYPE_SPIN_BUTTON){
    child_widget = gtk_bin_get_child(GTK_BIN(line_member));

    g_object_set(G_OBJECT(child_widget),
		 "label\0", label,
		 NULL);
  }else{
    GtkLabel *label;

    //TODO:JK: implement me
  }


  line_member->widget_label = label;
}

void
ags_line_member_real_change_port(AgsLineMember *line_member,
				 gpointer port_data)
{
  if((AGS_LINE_MEMBER_RESET_BY_ATOMIC & (line_member->flags)) != 0){
    AgsPort *port;
    GValue value = {0,};

    port = line_member->port;

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

    ags_port_safe_write(line_member->port,
			&value);

    if((AGS_LINE_MEMBER_APPLY_RECALL & (line_member->flags)) != 0){
      ags_port_safe_write(line_member->recall_port,
			  &value);
    }
  }

  if((AGS_LINE_MEMBER_RESET_BY_TASK & (line_member->flags)) != 0){
    AgsWindow *window;
    AgsLine *line;
    
    AgsThread *main_loop;
    AgsTaskThread *task_thread;
    AgsTask *task;

    AgsApplicationContext *application_context;

    //TODO:JK: add support for effect_line
    line = (AgsLine *) gtk_widget_get_ancestor(GTK_WIDGET(line_member),
					       AGS_TYPE_LINE);

    window = gtk_widget_get_ancestor(line,
				     AGS_TYPE_WINDOW);

    application_context = window->application_context;
    
    main_loop = application_context->main_loop;
    task_thread = ags_thread_find_type(main_loop,
				       AGS_TYPE_TASK_THREAD);

    task = (AgsTask *) g_object_new(line_member->task_type,
				    line_member->control_port, port_data,
				    NULL);

    ags_task_thread_append_task(task_thread,
				task);
  }
}

/**
 * ags_line_change_port:
 * @line_member: an #AgsLineMember
 * @port_data: the port's value
 *
 * Is emitted as port's value is modified.
 *
 * Since: 0.4
 */
void
ags_line_member_change_port(AgsLineMember *line_member,
			    gpointer port_data)
{
  g_return_if_fail(AGS_IS_LINE_MEMBER(line_member));

  g_object_ref((GObject *) line_member);
  g_signal_emit(G_OBJECT(line_member),
		line_member_signals[CHANGE_PORT], 0,
		port_data);
  g_object_unref((GObject *) line_member);
}

/**
 * ags_line_member_find_port:
 * @line_member: an #AgsLineMember
 *
 * Lookup ports of assigned recall.
 *
 * Since: 0.4
 */
void
ags_line_member_find_port(AgsLineMember *line_member)
{
  GtkWidget *line;
  AgsAudio *audio;
  AgsChannel *channel;
  AgsPort *audio_port, *channel_port;
  AgsPort *recall_audio_port, *recall_channel_port;
  GList *recall;
  gchar *specifier;

  auto AgsPort* ags_line_member_find_specifier(GList *recall);

  AgsPort* ags_line_member_find_specifier(GList *recall){
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


  if(line_member == NULL){
    return;
  }

  specifier = line_member->specifier;

  if(!AGS_IS_LINE_MEMBER(line_member) || specifier == NULL){
    return;
  }

  line = gtk_widget_get_ancestor(GTK_WIDGET(line_member),
				 AGS_TYPE_LINE);

  if(line != NULL){
    channel = AGS_LINE(line)->channel;
  }else{
    line = gtk_widget_get_ancestor(GTK_WIDGET(line_member),
				   AGS_TYPE_EFFECT_LINE);

    if(line != NULL){
      channel = AGS_EFFECT_LINE(line)->channel;
    }
  }
  
  audio = AGS_AUDIO(channel->audio);

  audio_port = NULL;
  channel_port = NULL;
  
  recall_audio_port = NULL;
  recall_channel_port = NULL;
  
  /* search channels */
  recall = channel->play;
  channel_port = ags_line_member_find_specifier(recall);

  recall = channel->recall;
  recall_channel_port = ags_line_member_find_specifier(recall);
 
  /* search audio */
  if(channel_port == NULL){
    recall = audio->play;
    audio_port = ags_line_member_find_specifier(recall);

    recall = audio->recall;
    recall_audio_port = ags_line_member_find_specifier(recall);
  }

  if(channel_port != NULL){
    g_object_set(G_OBJECT(line_member),
		 "port\0", channel_port,
		 NULL);

    g_object_set(G_OBJECT(line_member),
		 "recall-port\0", recall_channel_port,
		 NULL);
  }else if(audio_port != NULL){
    g_object_set(G_OBJECT(line_member),
		 "port\0", audio_port,
		 NULL);

    g_object_set(G_OBJECT(line_member),
		 "recall-port\0", recall_audio_port,
		 NULL);
  }
}

/**
 * ags_line_member_new:
 *
 * Creates an #AgsLineMember
 *
 * Returns: a new #AgsLineMember
 *
 * Since: 0.4
 */
AgsLineMember*
ags_line_member_new()
{
  AgsLineMember *line_member;

  line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
					       NULL);

  return(line_member);
}
