/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/ags_line_member.h>
#include <ags/X/ags_line_member_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_marshal.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_channel.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/widget/ags_led.h>
#include <ags/widget/ags_hindicator.h>
#include <ags/widget/ags_dial.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>
#include <ags/X/ags_effect_pad.h>
#include <ags/X/ags_effect_line.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <ags/i18n.h>

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
GList* ags_line_member_real_find_port(AgsLineMember *line_member);

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
  PROP_PORT,
  PROP_PORT_DATA,
  PROP_RECALL_PORT,
  PROP_RECALL_PORT_DATA,
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
						  "AgsLineMember", &ags_line_member_info,
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
   * Since: 1.0.0
   */
  param_spec = g_param_spec_ulong("widget-type",
				  i18n_pspec("widget type of line member"),
				  i18n_pspec("The widget type this line member packs"),
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
   * Since: 1.0.0
   */
  param_spec = g_param_spec_string("widget-label",
				   i18n_pspec("label to display"),
				   i18n_pspec("The label to display"),
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
   * Since: 1.0.0
   */
  param_spec = g_param_spec_string("plugin-name",
				   i18n_pspec("plugin name to control"),
				   i18n_pspec("The plugin's name to control"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLUGIN_NAME,
				  param_spec);

  /**
   * AgsLineMember:filename:
   *
   * The plugin filename of the recall to apply.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("the filename"),
				   i18n_pspec("The filename of the plugin"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

    /**
   * AgsLineMember:effect:
   *
   * The plugin effect of the recall to apply.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_string("effect",
				   i18n_pspec("the effect"),
				   i18n_pspec("The effect of the plugin"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT,
				  param_spec);

  /**
   * AgsLineMember:specifier:
   *
   * The plugin specifier of the recall to apply.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_string("specifier",
				   i18n_pspec("port specifier"),
				   i18n_pspec("The specifier of the port"),
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
   * Since: 1.0.0
   */
  param_spec = g_param_spec_string("control-port",
				   i18n_pspec("control port index"),
				   i18n_pspec("The index of the port to control"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONTROL_PORT,
				  param_spec);

  /**
   * AgsLineMember:steps:
   *
   * If line member has integer ports, this is the number of steps.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("steps",
				 i18n_pspec("steps of line members port"),
				 i18n_pspec("The steps this line members port has"),
				 0,
				 G_MAXUINT,
				 AGS_DIAL_DEFAULT_PRECISION,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_STEPS,
				  param_spec);

  /**
   * AgsLineMember:port:
   *
   * The matching simple port of plugin name and specifier.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("port",
				   i18n_pspec("port to apply"),
				   i18n_pspec("The port to apply"),
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
   * Since: 1.0.0
   */
  param_spec = g_param_spec_pointer("port-data",
				    i18n_pspec("port data"),
				    i18n_pspec("The port data"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_DATA,
				  param_spec);

  /**
   * AgsLineMember:recall-port:
   *
   * The matching complex port of plugin name and specifier.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("recall-port",
				   i18n_pspec("recall port to apply"),
				   i18n_pspec("The recall port to apply"),
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
   * Since: 1.0.0
   */
  param_spec = g_param_spec_pointer("recall-port-data",
				    i18n_pspec("recall port data"),
				    i18n_pspec("The recall port data"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_PORT_DATA,
				  param_spec);

  /**
   * AgsLineMember:task-type:
   *
   * The task type to apply the port.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_ulong("task-type",
				  i18n_pspec("task type to apply"),
				  i18n_pspec("The task type to apply the port"),
				  0, G_MAXULONG, 
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TASK_TYPE,
				  param_spec);

  /* AgsLineMember */
  line_member->change_port = ags_line_member_real_change_port;
  line_member->find_port = ags_line_member_real_find_port;

  /* signals */
  /**
   * AgsLineMember::change-port:
   * @line_member: the #AgsLineMember
   * @port_data: the port's data
   *
   * The ::change-port signal notifies modified port.
   */
  line_member_signals[CHANGE_PORT] =
    g_signal_new("change-port",
		 G_TYPE_FROM_CLASS(line_member),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineMemberClass, change_port),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);

  /**
   * AgsLine_Member::find-port:
   * @line_member: the #AgsLineMember to resize
   * Returns: a #GList with associated ports
   *
   * The ::find-port retrieves all associated ports
   */
  line_member_signals[FIND_PORT] =
    g_signal_new("find-port",
		 G_TYPE_FROM_CLASS(line_member),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineMemberClass, find_port),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);
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
  AgsDial *dial;
  
  g_signal_connect_after((GObject *) line_member, "parent_set",
			 G_CALLBACK(ags_line_member_parent_set_callback), (gpointer) line_member);

  line_member->flags = (AGS_LINE_MEMBER_RESET_BY_ATOMIC |
			AGS_LINE_MEMBER_APPLY_RECALL);
  line_member->port_flags = 0;

  line_member->widget_type = AGS_TYPE_DIAL;
  dial = (AgsDial *) g_object_new(AGS_TYPE_DIAL,
				  "adjustment", gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0),
				  NULL);
  
  gtk_container_add(GTK_CONTAINER(line_member),
		    (GtkWidget *) dial);

  line_member->widget_label = NULL;

  line_member->filename = NULL;
  line_member->effect = NULL;

  line_member->plugin_name = NULL;
  line_member->specifier = NULL;

  line_member->control_port = NULL;

  line_member->steps = 0;

  line_member->conversion = NULL;
  
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

      GtkAdjustment *adjustment;
      
      GType widget_type;

      gboolean active;
      
      //TODO:JK: verify me
      widget_type = g_value_get_ulong(value);

      if(widget_type == line_member->widget_type){
	return;
      }

      child = gtk_bin_get_child(GTK_BIN(line_member));

      /* preserver previous range */
      adjustment = NULL;
      active = FALSE;
    
      if(GTK_IS_RANGE(child)){
	adjustment = GTK_RANGE(child)->adjustment;
      }else if(GTK_IS_SPIN_BUTTON(child)){
	adjustment = GTK_SPIN_BUTTON(child)->adjustment;
      }else if(AGS_IS_DIAL(child)){
	adjustment = AGS_DIAL(child)->adjustment;
      }else if(GTK_IS_TOGGLE_BUTTON(child)){
	active = gtk_toggle_button_get_active((GtkToggleButton *) child);
      }
      
      if(adjustment != NULL){
	g_object_ref(adjustment);
      }
      
      /* destroy old */
      if(child != NULL){
	gtk_widget_destroy(child);
      }

      line_member->widget_type = widget_type;
      new_child = (GtkWidget *) g_object_new(widget_type,
					     NULL);


      if(AGS_IS_DIAL(new_child)){
	AgsDial *dial;

	dial = (AgsDial *) new_child;
	
	gtk_widget_set_size_request((GtkWidget *) dial,
				    2 * (dial->radius + dial->outline_strength + dial->button_width + 4),
				    2 * (dial->radius + dial->outline_strength + 1));
      }

      /* set range */
      if(GTK_IS_RANGE(new_child)){
	gtk_adjustment_set_lower(GTK_RANGE(new_child)->adjustment,
				 adjustment->lower);
	gtk_adjustment_set_upper(GTK_RANGE(new_child)->adjustment,
				 adjustment->upper);

	gtk_adjustment_set_value(GTK_RANGE(new_child)->adjustment,
				 adjustment->value);
      }else if(GTK_IS_SPIN_BUTTON(new_child)){
	gtk_adjustment_set_lower(GTK_SPIN_BUTTON(new_child)->adjustment,
				 adjustment->lower);
	gtk_adjustment_set_upper(GTK_SPIN_BUTTON(new_child)->adjustment,
				 adjustment->upper);

	gtk_adjustment_set_value(GTK_SPIN_BUTTON(new_child)->adjustment,
				 adjustment->value);
      }else if(AGS_IS_DIAL(new_child)){
	gtk_adjustment_set_lower(AGS_DIAL(new_child)->adjustment,
				 adjustment->lower);
	gtk_adjustment_set_upper(AGS_DIAL(new_child)->adjustment,
				 adjustment->upper);

	gtk_adjustment_set_value(AGS_DIAL(new_child)->adjustment,
				 adjustment->value);
	ags_dial_draw((AgsDial *) new_child);
      }else if(GTK_IS_TOGGLE_BUTTON(new_child)){
	gtk_toggle_button_set_active((GtkToggleButton *) new_child,
				     active);
      }else{
	if(!(AGS_IS_INDICATOR(new_child) ||
	     AGS_IS_LED(new_child))){
	  g_warning("ags_line_member_set_property() - unknown child type %s", g_type_name(widget_type));
	}
      }

      /* add */
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

      if(line_member->widget_label != NULL){
	g_free(line_member->widget_label);
      }
      
      line_member->widget_label = g_strdup(label);
      ags_line_member_set_label(line_member, line_member->widget_label);
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
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == line_member->filename){
	return;
      }

      if(line_member->filename != NULL){
	g_free(line_member->filename);
      }
      
      if(filename != NULL){
	if(!g_file_test(filename,
			G_FILE_TEST_EXISTS)){
	  AgsWindow *window;

	  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) line_member);

	  ags_window_show_error(window,
				g_strdup_printf("Plugin file not present %s",
						filename));
	}
      }

      line_member->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;

      effect = g_value_get_string(value);

      if(effect == line_member->effect){
	return;
      }

      line_member->effect = g_strdup(effect);
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
  case PROP_STEPS:
    {
      GtkWidget *child;
      
      guint steps;

      steps = g_value_get_uint(value);

      line_member->steps = steps;

      child = gtk_bin_get_child(GTK_BIN(line_member));

      if(AGS_IS_DIAL(child)){
	g_object_set(child,
		     "scale-precision", steps,
		     NULL);
      }
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

	if((AGS_PORT_INFINITE_RANGE & (port->flags)) != 0){
	  GtkWidget *child;

	  child = gtk_bin_get_child(GTK_BIN(line_member));

	  //TODO:JK: add more types

	  if(AGS_IS_DIAL(child)){
	    AGS_DIAL(child)->flags |= AGS_DIAL_SEEMLESS_MODE;
	  }
	}
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
  case PROP_FILENAME:
    {
      g_value_set_string(value, line_member->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, line_member->effect);
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

  if((AGS_LINE_MEMBER_CONNECTED & (line_member->flags)) != 0){
    return;
  }

  line_member->flags |= AGS_LINE_MEMBER_CONNECTED;

  ags_line_member_find_port(line_member);
  
  control = gtk_bin_get_child(GTK_BIN(line_member));

  if((AGS_LINE_MEMBER_APPLY_INITIAL & (line_member->flags)) != 0){
    GtkAdjustment *adjustment;

    gboolean is_toggled;
    
    adjustment = NULL;
    is_toggled = FALSE;
    
    if(AGS_IS_DIAL(control)){
      adjustment = AGS_DIAL(control)->adjustment;
    }else if(GTK_IS_RANGE(control)){
      adjustment = GTK_RANGE(control)->adjustment;
    }else if(GTK_IS_SPIN_BUTTON(control)){
      adjustment = GTK_SPIN_BUTTON(control)->adjustment;
    }else if(GTK_IS_TOGGLE_BUTTON(control)){
      line_member->active = gtk_toggle_button_get_active((GtkToggleButton *) control);

      is_toggled = TRUE;
    }

    if(is_toggled){
      ags_line_member_change_port(line_member,
				  &(line_member->active));
    }else if(adjustment != NULL){
      ags_line_member_change_port(line_member,
				  &(adjustment->value));
    }
    
    line_member->flags &= (~AGS_LINE_MEMBER_APPLY_INITIAL);
  }
  
  /* widget callback */
  if(line_member->widget_type == AGS_TYPE_DIAL){
    g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			   G_CALLBACK(ags_line_member_dial_changed_callback), line_member);
  }else if(line_member->widget_type == GTK_TYPE_VSCALE){
    g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			   G_CALLBACK(ags_line_member_vscale_changed_callback), line_member);
  }else if(line_member->widget_type == GTK_TYPE_HSCALE){
    g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			   G_CALLBACK(ags_line_member_hscale_changed_callback), line_member);
  }else if(line_member->widget_type == GTK_TYPE_SPIN_BUTTON){
    g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			   G_CALLBACK(ags_line_member_spin_button_changed_callback), line_member);
  }else if(line_member->widget_type == GTK_TYPE_CHECK_BUTTON){
    g_signal_connect_after(GTK_WIDGET(control), "clicked",
			   G_CALLBACK(ags_line_member_check_button_clicked_callback), line_member);
  }else if(line_member->widget_type == GTK_TYPE_TOGGLE_BUTTON){
    g_signal_connect_after(GTK_WIDGET(control), "clicked",
			   G_CALLBACK(ags_line_member_toggle_button_clicked_callback), line_member);
  }else if(line_member->widget_type == GTK_TYPE_BUTTON){
    g_signal_connect_after(GTK_WIDGET(control), "clicked",
			   G_CALLBACK(ags_line_member_button_clicked_callback), line_member);
  }

  /* port callback */
  if((AGS_LINE_MEMBER_PLAY_CALLBACK_WRITE & (line_member->flags)) != 0 &&
     line_member->port != NULL){
    g_signal_connect_after(line_member->port, "safe-write",
			   G_CALLBACK(ags_line_member_port_safe_write_callback), line_member);
  }

  if((AGS_LINE_MEMBER_RECALL_CALLBACK_WRITE & (line_member->flags)) != 0 &&
     line_member->recall_port != NULL){
    g_signal_connect_after(line_member->recall_port, "safe-write",
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
  /* call parent */
  G_OBJECT_CLASS(ags_line_member_parent_class)->finalize(gobject);  
}

GtkWidget*
ags_line_member_get_widget(AgsLineMember *line_member)
{
  return(gtk_bin_get_child(GTK_BIN(line_member)));
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

  if(g_type_is_a(line_member->widget_type,
		 GTK_TYPE_BUTTON)){
    child_widget = gtk_bin_get_child(GTK_BIN(line_member));

    g_object_set(G_OBJECT(child_widget),
		 "label", label,
		 NULL);
  }else{
    gtk_frame_set_label_widget((GtkFrame *) line_member,
			       g_object_new(GTK_TYPE_LABEL,
					    "wrap", TRUE,
					    "wrap-mode", PANGO_WRAP_CHAR,
					    "use-markup", TRUE,
					    "label", g_markup_printf_escaped("<small>%s</small>", label),
					    NULL));
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

    if(port == NULL){
      g_warning("ags_line_member_change_port() - no port available");
      return;
    }

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
	
	if(GTK_IS_TOGGLE_BUTTON(gtk_bin_get_child((GtkBin *) line_member))){
	  if(((gboolean *) port_data)[0]){
	    val = 1.0;
	  }else{
	    val = 0.0;
	  }
	}else{
	  val = ((gdouble *) port_data)[0];
	}

	if(line_member->conversion != NULL){
	  gfloat upper, lower, range, step;
	  gfloat c_upper, c_lower, c_range;

	  gboolean success;

	  success = FALSE;
	    
	  if(AGS_IS_DIAL(gtk_bin_get_child((GtkBin *) line_member))){
	    AgsDial *dial;

	    dial = (AgsDial *) gtk_bin_get_child((GtkBin *) line_member);

	    upper = dial->adjustment->upper;
	    lower = dial->adjustment->lower;

	    success = TRUE;
	  }else if(GTK_IS_RANGE(gtk_bin_get_child((GtkBin *) line_member))){
	    GtkRange *range;

	    range = (GtkRange *) gtk_bin_get_child((GtkBin *) line_member);

	    upper = range->adjustment->upper;
	    lower = range->adjustment->lower;

	    success = TRUE;
	  }else if(GTK_IS_SPIN_BUTTON(gtk_bin_get_child((GtkBin *) line_member))){
	    GtkSpinButton *spin_button;

	    spin_button = (GtkSpinButton *) gtk_bin_get_child((GtkBin *) line_member);

	    upper = spin_button->adjustment->upper;
	    lower = spin_button->adjustment->lower;

	    success = TRUE;
	  }else{
	    g_warning("unsupported child type in conversion");
	  }

	  if(success){
	    range = upper - lower;
	    step = range / val;

	    val = ags_conversion_convert(line_member->conversion,
					 val,
					 FALSE);
	    c_upper = ags_conversion_convert(line_member->conversion,
					     upper,
					     FALSE);
	    c_lower = ags_conversion_convert(line_member->conversion,
					     lower,
					     FALSE);
	    c_range = c_upper - c_lower;
	    
	    val = ags_conversion_convert(line_member->conversion,
					 c_lower + (c_range / step),
					 TRUE);
	  }
	}

	g_value_init(&value,
		     G_TYPE_FLOAT);
	g_value_set_float(&value,
			  (gfloat) val);
      }else if(port->port_value_type == G_TYPE_DOUBLE){
	gdouble val;

	if(GTK_IS_TOGGLE_BUTTON(gtk_bin_get_child((GtkBin *) line_member))){
	  if(((gboolean *) port_data)[0]){
	    val = 1.0;
	  }else{
	    val = 0.0;
	  }
	}else{
	  val = ((gdouble *) port_data)[0];
	}
	
	if(line_member->conversion != NULL){
	  gdouble upper, lower, range, step;
	  gdouble c_upper, c_lower, c_range;

	  gboolean success;

	  success = FALSE;
	    
	  if(AGS_IS_DIAL(gtk_bin_get_child((GtkBin *) line_member))){
	    AgsDial *dial;

	    dial = (AgsDial *) gtk_bin_get_child((GtkBin *) line_member);

	    upper = dial->adjustment->upper;
	    lower = dial->adjustment->lower;

	    success = TRUE;
	  }else if(GTK_IS_RANGE(gtk_bin_get_child((GtkBin *) line_member))){
	    GtkRange *range;

	    range = (GtkRange *) gtk_bin_get_child((GtkBin *) line_member);

	    upper = range->adjustment->upper;
	    lower = range->adjustment->lower;

	    success = TRUE;
	  }else if(GTK_IS_SPIN_BUTTON(gtk_bin_get_child((GtkBin *) line_member))){
	    GtkSpinButton *spin_button;

	    spin_button = (GtkSpinButton *) gtk_bin_get_child((GtkBin *) line_member);

	    upper = spin_button->adjustment->upper;
	    lower = spin_button->adjustment->lower;

	    success = TRUE;
	  }else{
	    g_warning("unsupported child type in conversion");
	  }

	  if(success){
	    range = upper - lower;
	    step = range / val;

	    val = ags_conversion_convert(line_member->conversion,
					 val,
					 FALSE);
	    c_upper = ags_conversion_convert(line_member->conversion,
					     upper,
					     FALSE);
	    c_lower = ags_conversion_convert(line_member->conversion,
					     lower,
					     FALSE);
	    c_range = c_upper - c_lower;

	    
	    val = ags_conversion_convert(line_member->conversion,
					 c_lower + (c_range / step),
					 TRUE);
	  }
	}

	g_value_init(&value,
		     G_TYPE_DOUBLE);

	g_value_set_double(&value,
			   val);
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
    
    ags_port_safe_write(line_member->port,
			&value);

    if((AGS_LINE_MEMBER_APPLY_RECALL & (line_member->flags)) != 0){
      ags_port_safe_write(line_member->recall_port,
			  &value);
    }
  }

  if((AGS_LINE_MEMBER_RESET_BY_TASK & (line_member->flags)) != 0){
    AgsWindow *window;

    AgsMutexManager *mutex_manager;
    AgsThread *main_loop;
    AgsGuiThread *gui_thread;
    AgsTask *task;

    AgsApplicationContext *application_context;

    pthread_mutex_t *application_mutex;

    window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) line_member,
						   AGS_TYPE_WINDOW);
  
    application_context = (AgsApplicationContext *) window->application_context;

    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
    
    /* get audio loop */
    pthread_mutex_lock(application_mutex);

    main_loop = (AgsThread *) application_context->main_loop;

    pthread_mutex_unlock(application_mutex);

    /* get task and soundcard thread */
    gui_thread = (AgsGuiThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_GUI_THREAD);

    task = (AgsTask *) g_object_new(line_member->task_type,
				    line_member->control_port, port_data,
				    NULL);

    ags_gui_thread_schedule_task(gui_thread,
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
 * Since: 1.0.0
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

GList*
ags_line_member_real_find_port(AgsLineMember *line_member)
{
  AgsMachine *machine;
  GtkWidget *parent;

  AgsAudio *audio;
  AgsChannel *channel;
  AgsPort *audio_port, *channel_port;
  AgsPort *recall_audio_port, *recall_channel_port;

  GList *recall;
  GList *port;
  
  gchar *specifier;

  auto AgsPort* ags_line_member_find_specifier(GList *recall);

  AgsPort* ags_line_member_find_specifier(GList *recall){
    GList *port;
    
    while(recall != NULL){
      if((AGS_RECALL_BULK_MODE & (AGS_RECALL(recall->data)->flags)) != 0){
	recall = recall->next;

	continue;
      }

    port = AGS_RECALL(recall->data)->port;

#ifdef AGS_DEBUG
      g_message("search port in %s", G_OBJECT_TYPE_NAME(recall->data));
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
    return(NULL);
  }

  specifier = line_member->specifier;

  if(!AGS_IS_LINE_MEMBER(line_member) || specifier == NULL){
    return(NULL);
  }

  machine = (AgsMachine *) gtk_widget_get_ancestor(GTK_WIDGET(line_member),
						   AGS_TYPE_MACHINE);
  parent = gtk_widget_get_ancestor(GTK_WIDGET(line_member),
				   AGS_TYPE_LINE);

  audio = machine->audio;
  
  if(parent != NULL){
    channel = AGS_LINE(parent)->channel;
  }else{
    parent = gtk_widget_get_ancestor(GTK_WIDGET(line_member),
				     AGS_TYPE_EFFECT_LINE);

    if(parent != NULL){
      channel = AGS_EFFECT_LINE(parent)->channel;
    }else{
      return(NULL);
    }
  }    
  
  audio_port = NULL;
  channel_port = NULL;
  
  recall_audio_port = NULL;
  recall_channel_port = NULL;

  port = NULL;
    
  /* search channels */
  recall = channel->play;
  channel_port = ags_line_member_find_specifier(recall);

  recall = channel->recall;
  recall_channel_port = ags_line_member_find_specifier(recall);

  if(channel_port != NULL){
    port = g_list_prepend(port,
			  channel_port);
  }

  if(recall_channel_port != NULL){
    port = g_list_prepend(port,
			  recall_channel_port);
  }
  
  /* search audio */
  if(channel_port == NULL && recall_channel_port == NULL){
    recall = audio->play;
    audio_port = ags_line_member_find_specifier(recall);

    recall = audio->recall;
    recall_audio_port = ags_line_member_find_specifier(recall);

    if(audio_port != NULL){
      port = g_list_prepend(port,
			    audio_port);
    }

    if(recall_audio_port != NULL){
      port = g_list_prepend(port,
			    recall_audio_port);
    }
  }

  if(channel_port != NULL || recall_channel_port != NULL){
    g_object_set(G_OBJECT(line_member),
		 "port", channel_port,
		 NULL);

    g_object_set(G_OBJECT(line_member),
		 "recall-port", recall_channel_port,
		 NULL);
  }else if(audio_port != NULL || recall_audio_port != NULL){
    g_object_set(G_OBJECT(line_member),
		 "port", audio_port,
		 NULL);

    g_object_set(G_OBJECT(line_member),
		 "recall-port", recall_audio_port,
		 NULL);
  }

  return(port);
}

/**
 * ags_line_member_find_port:
 * @line_member: an #AgsLineMember
 *
 * Lookup ports of assigned recalls.
 *
 * Returns: an #GList containing all related #AgsPort
 *
 * Since: 1.0.0
 */
GList*
ags_line_member_find_port(AgsLineMember *line_member)
{
  GList *list;

  list = NULL;
  g_return_val_if_fail(AGS_IS_LINE_MEMBER(line_member),
		       NULL);

  g_object_ref((GObject *) line_member);
  g_signal_emit((GObject *) line_member,
		line_member_signals[FIND_PORT], 0,
		&list);
  g_object_unref((GObject *) line_member);

  return(list);
}

/**
 * ags_line_member_chained_event:
 * @line_member: an #AgsLineMember
 * 
 * Chain changed control and apply the very same value to grouped
 * controls if sticky controls set.
 * 
 * Since: 1.0.0
 */
void
ags_line_member_chained_event(AgsLineMember *line_member)
{
  AgsMachine *machine;

  if(!AGS_IS_LINE_MEMBER(line_member) ||
     (AGS_LINE_MEMBER_BLOCK_CHAINED & (line_member->flags)) != 0){
    return;
  }
  
  machine = gtk_widget_get_ancestor(line_member,
				    AGS_TYPE_MACHINE);

  if((AGS_MACHINE_STICKY_CONTROLS & (machine->flags)) != 0){
    AgsPad *pad;
    AgsEffectPad *effect_pad;
    GtkWidget *child_widget;
    
    GtkAdjustment *adjustment;
    
    gboolean is_active;

    pad = gtk_widget_get_ancestor(line_member,
				  AGS_TYPE_PAD);

    effect_pad = gtk_widget_get_ancestor(line_member,
					 AGS_TYPE_EFFECT_PAD);

    is_active = FALSE;

    child_widget = gtk_bin_get_child(line_member);
    
    if(AGS_IS_DIAL(child_widget) ||
       GTK_IS_SPIN_BUTTON(child_widget) ||
       GTK_IS_SCALE(child_widget)){
      g_object_get(gtk_bin_get_child(line_member),
		   "adjustment", &adjustment,
		   NULL);
    }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
      is_active = gtk_toggle_button_get_active(gtk_bin_get_child(line_member));
    }

    if(pad != NULL){
      AgsLine *line;

      GList *list_line, *list_line_start;
      GList *list_line_member, *list_line_member_start;

      line = gtk_widget_get_ancestor(line_member,
				     AGS_TYPE_LINE);
      
      list_line =
	list_line_start = gtk_container_get_children(GTK_CONTAINER(pad->expander_set));

      while((list_line = ags_line_find_next_grouped(list_line)) != NULL){
	if(list_line->data != line){
	  list_line_member =
	    list_line_member_start = gtk_container_get_children((GtkContainer *) AGS_LINE(list_line->data)->expander->table);

	  while(list_line_member != NULL){
	    if(!g_strcmp0(line_member->specifier,
			  AGS_LINE_MEMBER(list_line_member->data)->specifier)){
	      AGS_LINE_MEMBER(list_line_member->data)->flags |= AGS_LINE_MEMBER_BLOCK_CHAINED;

	      child_widget = gtk_bin_get_child(AGS_LINE_MEMBER(list_line_member->data));

	      if(AGS_IS_DIAL(child_widget)){
		ags_dial_set_value(gtk_bin_get_child(AGS_LINE_MEMBER(list_line_member->data)),
				   adjustment->value);
	      }else if(GTK_IS_SPIN_BUTTON(child_widget)){
		gtk_spin_button_set_value(gtk_bin_get_child(AGS_LINE_MEMBER(list_line_member->data)),
					  adjustment->value);
	      }else if(GTK_IS_SCALE(child_widget)){
		gtk_range_set_value(gtk_bin_get_child(AGS_LINE_MEMBER(list_line_member->data)),
				    adjustment->value);
	      }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
		gtk_toggle_button_set_active(gtk_bin_get_child(list_line_member->data),
					     is_active);
	      }else if(GTK_IS_BUTTON(child_widget)){
		gtk_button_clicked(gtk_bin_get_child(AGS_LINE_MEMBER(list_line_member->data)));
	      }
	      
	      AGS_LINE_MEMBER(list_line_member->data)->flags &= (~AGS_LINE_MEMBER_BLOCK_CHAINED);

	      break;
	    }
	      
	    list_line_member = list_line_member->next;
	  }
	  
	  g_list_free(list_line_member_start);
	}
	
	list_line = list_line->next;
      }

      g_list_free(list_line_start);
    }else if(effect_pad != NULL){
      AgsEffectLine *effect_line;

      GList *list_effect_line, *list_effect_line_start;
      GList *list_line_member, *list_line_member_start;

      effect_line = gtk_widget_get_ancestor(line_member,
					    AGS_TYPE_EFFECT_LINE);
      
      list_effect_line =
	list_effect_line_start = gtk_container_get_children(GTK_CONTAINER(effect_pad->table));

      while((list_effect_line = ags_effect_line_find_next_grouped(list_effect_line)) != NULL){
	if(list_effect_line->data != effect_line){
	  list_line_member =
	    list_line_member_start = gtk_container_get_children((GtkContainer *) AGS_EFFECT_LINE(list_effect_line->data)->table);

	  while(list_line_member != NULL){
	    if(!g_strcmp0(line_member->specifier,
			 AGS_LINE_MEMBER(list_line_member->data)->specifier)){
	      AGS_LINE_MEMBER(list_line_member->data)->flags |= AGS_LINE_MEMBER_BLOCK_CHAINED;

	      child_widget = gtk_bin_get_child(AGS_LINE_MEMBER(list_line_member->data));

	      if(AGS_IS_DIAL(child_widget)){
		ags_dial_set_value(gtk_bin_get_child(AGS_LINE_MEMBER(list_line_member->data)),
				   adjustment->value);
	      }else if(GTK_IS_SPIN_BUTTON(child_widget)){
		gtk_spin_button_set_value(gtk_bin_get_child(AGS_LINE_MEMBER(list_line_member->data)),
					  adjustment->value);
	      }else if(GTK_IS_SCALE(child_widget)){
		gtk_range_set_value(gtk_bin_get_child(AGS_LINE_MEMBER(list_line_member->data)),
				    adjustment->value);
	      }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
		gtk_toggle_button_set_active(gtk_bin_get_child(list_line_member->data),
					     is_active);
	      }else if(GTK_IS_BUTTON(child_widget)){
		gtk_button_clicked(gtk_bin_get_child(AGS_LINE_MEMBER(list_line_member->data)));
	      }
	      
	      AGS_LINE_MEMBER(list_line_member->data)->flags &= (~AGS_LINE_MEMBER_BLOCK_CHAINED);

	      break;
	    }

	    list_line_member = list_line_member->next;
	  }
	  
	  g_list_free(list_line_member_start);
	}
	
	list_effect_line = list_effect_line->next;
      }

      g_list_free(list_effect_line_start);
    }
  }
}

/**
 * ags_line_member_new:
 *
 * Creates an #AgsLineMember
 *
 * Returns: a new #AgsLineMember
 *
 * Since: 1.0.0
 */
AgsLineMember*
ags_line_member_new()
{
  AgsLineMember *line_member;

  line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
					       NULL);

  return(line_member);
}
