/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/app/ags_line_member.h>
#include <ags/app/ags_line_member_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line.h>
#include <ags/app/ags_effect_pad.h>
#include <ags/app/ags_effect_line.h>

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
void ags_line_member_finalize(GObject *gobject);

void ags_line_member_connect(AgsConnectable *connectable);
void ags_line_member_disconnect(AgsConnectable *connectable);

void ags_line_member_real_change_port(AgsLineMember *line_member,
				      gpointer port_data);

AgsPort* ags_line_member_find_specifier(GList *recall,
					gchar *specifier);

GList* ags_line_member_real_find_port(AgsLineMember *line_member);


/**
 * SECTION:ags_line_member
 * @short_description: Modify assigned recall's port
 * @title: AgsLineMember
 * @section_id:
 * @include: ags/app/ags_line_member.h
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
  PROP_WIDGET_ORIENTATION,
  PROP_WIDGET_LABEL,
  PROP_PLAY_CONTAINER,
  PROP_RECALL_CONTAINER,
  PROP_PLUGIN_NAME,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_SPECIFIER,
  PROP_CONTROL_PORT,
  PROP_SCALE_PRECISION,
  PROP_STEP_COUNT,
  PROP_CONVERSION,
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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_line_member = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_line_member);
  }

  return g_define_type_id__volatile;
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
   * Since: 3.0.0
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
   * AgsLineMember:widget-orientation:
   *
   * The widget orientation.
   * 
   * Since: 3.8.0
   */
  param_spec = g_param_spec_uint("widget-orientation",
				 i18n_pspec("widget orientation"),
				 i18n_pspec("widget orientation"),
				 0, G_MAXUINT,
				 GTK_ORIENTATION_VERTICAL,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WIDGET_ORIENTATION,
				  param_spec);
  
  /**
   * AgsLineMember:widget-label:
   *
   * The widget's label to use.
   * 
   * Since: 3.0.0
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
   * AgsLineMember:play-container:
   *
   * The play context recall container.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("play-container",
				   i18n_pspec("play container"),
				   i18n_pspec("The play context recall container"),
				   AGS_TYPE_RECALL_CONTAINER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY_CONTAINER,
				  param_spec);

  /**
   * AgsLineMember:recall-container:
   *
   * The recall context recall container.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("recall-container",
				   i18n_pspec("recall container"),
				   i18n_pspec("The recall context recall container"),
				   AGS_TYPE_RECALL_CONTAINER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CONTAINER,
				  param_spec);

  /**
   * AgsLineMember:plugin-name:
   *
   * The plugin name of the recall to use.
   * 
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * AgsLineMember:scale-precision:
   *
   * If line member has integer ports, this is the number of steps.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("scale-precision",
				 i18n_pspec("scale precision of line members port"),
				 i18n_pspec("The scale precision this line members port has"),
				 0,
				 G_MAXUINT,
				 (guint) AGS_DIAL_DEFAULT_PRECISION,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCALE_PRECISION,
				  param_spec);

  /**
   * AgsLineMember:step-count:
   *
   * If line member has logarithmic ports, this is the number of step count.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("step-count",
				   i18n_pspec("step count of line members port"),
				   i18n_pspec("The step count this line members port has"),
				   0.0,
				   G_MAXDOUBLE,
				   AGS_LADSPA_CONVERSION_DEFAULT_STEP_COUNT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_STEP_COUNT,
				  param_spec);

  /**
   * AgsLineMember:conversion:
   *
   * The conversion of plugin.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("conversion",
				   i18n_pspec("conversion to apply"),
				   i18n_pspec("The conversion to apply"),
				   AGS_TYPE_CONVERSION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONVERSION,
				  param_spec);

  /**
   * AgsLineMember:port:
   *
   * The matching simple port of plugin name and specifier.
   * 
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * 
   * Since: 3.0.0
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
   *
   * The ::find-port retrieves all associated ports
   * 
   * Returns: a #GList-struct with associated ports
   * 
   * Since: 3.0.0
   */
  line_member_signals[FIND_PORT] =
    g_signal_new("find-port",
		 G_TYPE_FROM_CLASS(line_member),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLineMemberClass, find_port),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
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

  AgsApplicationContext *application_context;

  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();

  g_signal_connect_after((GObject *) line_member, "parent_set",
			 G_CALLBACK(ags_line_member_parent_set_callback), (gpointer) line_member);

  line_member->flags = (AGS_LINE_MEMBER_RESET_BY_ATOMIC |
			AGS_LINE_MEMBER_APPLY_RECALL);
  line_member->port_flags = 0;
  
  line_member->widget_type = AGS_TYPE_DIAL;
  line_member->widget_orientation = GTK_ORIENTATION_VERTICAL;
  dial = (AgsDial *) g_object_new(AGS_TYPE_DIAL,
				  "adjustment", gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0),
				  NULL);

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  g_object_set(dial,
	       "radius", (guint) (gui_scale_factor * AGS_DIAL_DEFAULT_RADIUS),
	       "font-size", (guint) (gui_scale_factor * AGS_DIAL_DEFAULT_FONT_SIZE),
	       "button-width", (gint) (gui_scale_factor * AGS_DIAL_DEFAULT_BUTTON_WIDTH),
	       "button-height", (gint) (gui_scale_factor * AGS_DIAL_DEFAULT_BUTTON_HEIGHT),
	       NULL);

  gtk_container_add(GTK_CONTAINER(line_member),
		    (GtkWidget *) dial);

  line_member->widget_label = NULL;

  line_member->play_container = NULL;
  line_member->recall_container = NULL;

  line_member->plugin_name = NULL;

  line_member->filename = NULL;
  line_member->effect = NULL;

  line_member->specifier = NULL;

  line_member->control_port = NULL;

  line_member->scale_precision = AGS_DIAL_DEFAULT_PRECISION;
  line_member->step_count = AGS_LADSPA_CONVERSION_DEFAULT_STEP_COUNT;

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

      AgsApplicationContext *application_context;
   
      GType widget_type;

      gdouble gui_scale_factor;
      gdouble lower_value, upper_value;
      gdouble current_value;
      gboolean active;
      
      //TODO:JK: verify me
      widget_type = g_value_get_ulong(value);

      if(widget_type == line_member->widget_type){
	return;
      }

      application_context = ags_application_context_get_instance();

      child = gtk_bin_get_child(GTK_BIN(line_member));

      /* preserver previous range */
      adjustment = NULL;
      active = FALSE;
    
      if(GTK_IS_RANGE(child)){
	adjustment = gtk_range_get_adjustment(GTK_RANGE(child));
      }else if(GTK_IS_SPIN_BUTTON(child)){
	adjustment = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(child));
      }else if(AGS_IS_DIAL(child)){
	adjustment = AGS_DIAL(child)->adjustment;
      }else if(GTK_IS_TOGGLE_BUTTON(child)){
	active = gtk_toggle_button_get_active((GtkToggleButton *) child);
      }

      lower_value = 0.0;
      upper_value = 0.0;

      current_value = 0.0;

      if(adjustment != NULL){
	lower_value = gtk_adjustment_get_lower(adjustment);
	upper_value = gtk_adjustment_get_upper(adjustment);
	
	current_value = gtk_adjustment_get_value(adjustment);
      }
      
      /* destroy old */
      if(child != NULL){
	gtk_widget_destroy(child);
      }

      line_member->widget_type = widget_type;
      new_child = (GtkWidget *) g_object_new(widget_type,
					     NULL);

      /* scale factor */
      gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

      if(AGS_IS_DIAL(new_child)){
	g_object_set(new_child,
		     "radius", (guint) (gui_scale_factor * AGS_DIAL_DEFAULT_RADIUS),
		     "font-size", (guint) (gui_scale_factor * AGS_DIAL_DEFAULT_FONT_SIZE),
		     "button-width", (gint) (gui_scale_factor * AGS_DIAL_DEFAULT_BUTTON_WIDTH),
		     "button-height", (gint) (gui_scale_factor * AGS_DIAL_DEFAULT_BUTTON_HEIGHT),
		     NULL);
      }else if(GTK_IS_SCALE(new_child)){
	guint widget_orientation;

	widget_orientation = GTK_ORIENTATION_VERTICAL;
	
	g_object_get(line_member,
		     "widget-orientation", &widget_orientation,
		     NULL);
	
	gtk_orientable_set_orientation(GTK_ORIENTABLE(new_child),
				       widget_orientation);
	
	if(widget_orientation == GTK_ORIENTATION_VERTICAL){
	  gtk_widget_set_size_request(new_child,
				      gui_scale_factor * 16, gui_scale_factor * 100);
	}else{
	  gtk_widget_set_size_request(new_child,
				      gui_scale_factor * 100, gui_scale_factor * 16);
	}
      }else if(AGS_IS_VINDICATOR(new_child)){
	g_object_set(new_child,
		     "segment-width", (guint) (gui_scale_factor * AGS_VINDICATOR_DEFAULT_SEGMENT_WIDTH),
		     "segment-height", (guint) (gui_scale_factor * AGS_VINDICATOR_DEFAULT_SEGMENT_HEIGHT),
		     "segment-padding", (guint) (gui_scale_factor * AGS_INDICATOR_DEFAULT_SEGMENT_PADDING),
		     NULL);

	//FIXME:JK: make indicator orientable
      }else if(AGS_IS_HINDICATOR(new_child)){
	g_object_set(new_child,
		     "segment-width", (guint) (gui_scale_factor * AGS_HINDICATOR_DEFAULT_SEGMENT_WIDTH),
		     "segment-height", (guint) (gui_scale_factor * AGS_HINDICATOR_DEFAULT_SEGMENT_HEIGHT),
		     "segment-padding", (guint) (gui_scale_factor * AGS_INDICATOR_DEFAULT_SEGMENT_PADDING),
		     NULL);

	//FIXME:JK: make indicator orientable
      }

      gtk_widget_queue_resize_no_redraw(new_child);
      gtk_widget_queue_draw(new_child);

      /* set range */
      if(GTK_IS_RANGE(new_child)){
	GtkAdjustment *new_adjustment;

	new_adjustment = gtk_range_get_adjustment(GTK_RANGE(new_child));
	
	gtk_adjustment_set_lower(new_adjustment,
				 lower_value);
	gtk_adjustment_set_upper(new_adjustment,
				 upper_value);

	gtk_adjustment_set_value(new_adjustment,
				 current_value);
      }else if(GTK_IS_SPIN_BUTTON(new_child)){
	GtkAdjustment *new_adjustment;

	new_adjustment = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(new_child));

	gtk_adjustment_set_lower(new_adjustment,
				 lower_value);
	gtk_adjustment_set_upper(new_adjustment,
				 upper_value);

	gtk_adjustment_set_value(new_adjustment,
				 current_value);
      }else if(AGS_IS_DIAL(new_child)){
	GtkAdjustment *new_adjustment;

	new_adjustment = AGS_DIAL(new_child)->adjustment;

	gtk_adjustment_set_lower(new_adjustment,
				 lower_value);
	gtk_adjustment_set_upper(new_adjustment,
				 upper_value);

	gtk_adjustment_set_value(new_adjustment,
				 current_value);

	gtk_widget_queue_draw((GtkWidget *) new_child);
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
  case PROP_WIDGET_ORIENTATION:
    {
      GtkWidget *child;

      AgsApplicationContext *application_context;

      gdouble gui_scale_factor;
      guint widget_orientation;

      widget_orientation = g_value_get_uint(value);
      
      line_member->widget_orientation = widget_orientation;

      application_context = ags_application_context_get_instance();

      /* scale factor */
      gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

      child = gtk_bin_get_child(GTK_BIN(line_member));

      if(GTK_IS_SCALE(child)){
	gtk_orientable_set_orientation(GTK_ORIENTABLE(child),
				       widget_orientation);
	
	if(widget_orientation == GTK_ORIENTATION_VERTICAL){
	  gtk_widget_set_size_request(child,
				      gui_scale_factor * 16, gui_scale_factor * 100);
	}else{
	  gtk_widget_set_size_request(child,
				      gui_scale_factor * 100, gui_scale_factor * 16);
	}
      }else if(AGS_IS_INDICATOR(child)){
	gtk_orientable_set_orientation(GTK_ORIENTABLE(child),
				       widget_orientation);

	//FIXME:JK: make indicator orientable
      }
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
  case PROP_PLAY_CONTAINER:
    {
      AgsRecallContainer *play_container;
      
      play_container = (AgsRecallContainer *) g_value_get_object(value);

      if(play_container == line_member->play_container){
	return;
      }

      if(line_member->play_container != NULL){
	g_object_unref(line_member->play_container);
      }

      if(play_container != NULL){
	g_object_ref(play_container);
      }

      line_member->play_container = play_container;
    }
    break;
  case PROP_RECALL_CONTAINER:
    {
      AgsRecallContainer *recall_container;
      
      recall_container = (AgsRecallContainer *) g_value_get_object(value);

      if(recall_container == line_member->recall_container){
	return;
      }

      if(line_member->recall_container != NULL){
	g_object_unref(line_member->recall_container);
      }

      if(recall_container != NULL){
	g_object_ref(recall_container);
      }

      line_member->recall_container = recall_container;
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
  case PROP_SCALE_PRECISION:
    {
      GtkWidget *child;
      
      guint scale_precision;

      scale_precision = g_value_get_uint(value);

      line_member->scale_precision = scale_precision;
      child = gtk_bin_get_child(GTK_BIN(line_member));

      if(AGS_IS_DIAL(child)){
	g_object_set(child,
		     "scale-precision", scale_precision,
		     NULL);
      }
    }
    break;
  case PROP_STEP_COUNT:
    {
      gdouble step_count;

      step_count = g_value_get_double(value);

      line_member->step_count = step_count;
    }
    break;
  case PROP_CONVERSION:
    {
      AgsConversion *conversion;

      conversion = g_value_get_object(value);
      
      if(conversion == line_member->conversion){
	return;
      }
      
      if(line_member->conversion != NULL){
	g_object_unref(line_member->conversion);
      }

      if(conversion != NULL){
	g_object_ref(conversion);
      }
      
      line_member->conversion = conversion;
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

	  //TODO:JK: add more orientations

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
  case PROP_WIDGET_ORIENTATION:
    {
      g_value_set_uint(value, line_member->widget_orientation);
    }
    break;
  case PROP_WIDGET_LABEL:
    {
      g_value_set_string(value, line_member->widget_label);
    }
    break;
  case PROP_PLAY_CONTAINER:
    {
      g_value_set_object(value, line_member->play_container);
    }
    break;
  case PROP_RECALL_CONTAINER:
    {
      g_value_set_object(value, line_member->recall_container);
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
  case PROP_SCALE_PRECISION:
    {
      g_value_set_uint(value, line_member->scale_precision);
    }
    break;
  case PROP_STEP_COUNT:
    {
      g_value_set_double(value, line_member->step_count);
    }
    break;
  case PROP_CONVERSION:
    {
      g_value_set_object(value, line_member->conversion);
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
ags_line_member_finalize(GObject *gobject)
{
  AgsLineMember *line_member;

  line_member = (AgsLineMember *) gobject;

  g_free(line_member->widget_label);

  if(line_member->play_container != NULL){
    g_object_unref(line_member->play_container);
  }

  if(line_member->recall_container != NULL){
    g_object_unref(line_member->recall_container);
  }

  g_free(line_member->plugin_name);

  g_free(line_member->filename);
  g_free(line_member->effect);

  g_free(line_member->specifier);

  g_free(line_member->control_port);
  
  if(line_member->conversion != NULL){
    g_object_unref(line_member->conversion);
  }
  
  if(line_member->port != NULL){
    g_object_unref(line_member->port);
  }

  if(line_member->recall_port != NULL){
    g_object_unref(line_member->recall_port);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_line_member_parent_class)->finalize(gobject);  
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
      adjustment = gtk_range_get_adjustment(GTK_RANGE(control));
    }else if(GTK_IS_SPIN_BUTTON(control)){
      adjustment = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(control));
    }else if(GTK_IS_TOGGLE_BUTTON(control)){
      line_member->active = gtk_toggle_button_get_active((GtkToggleButton *) control);

      is_toggled = TRUE;
    }

    if(is_toggled){
      ags_line_member_change_port(line_member,
				  &(line_member->active));
    }else if(adjustment != NULL){
      gdouble value;

      value = gtk_adjustment_get_value(adjustment);
      
      ags_line_member_change_port(line_member,
				  &value);
    }
    
    line_member->flags &= (~AGS_LINE_MEMBER_APPLY_INITIAL);
  }
  
  /* widget callback */
  if(line_member->widget_type == AGS_TYPE_DIAL){
    g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			   G_CALLBACK(ags_line_member_dial_changed_callback), line_member);
  }else if(g_type_is_a(line_member->widget_type, GTK_TYPE_SCALE)){
    g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			   G_CALLBACK(ags_line_member_scale_changed_callback), line_member);
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
}

void
ags_line_member_disconnect(AgsConnectable *connectable)
{
  AgsLineMember *line_member;
  GtkWidget *control;

  line_member = AGS_LINE_MEMBER(connectable);

  if((AGS_LINE_MEMBER_CONNECTED & (line_member->flags)) == 0){
    return;
  }

  line_member->flags &= (~AGS_LINE_MEMBER_CONNECTED);

  /* widget callback */
  control = gtk_bin_get_child(GTK_BIN(line_member));
  
  if(line_member->widget_type == AGS_TYPE_DIAL){
    g_object_disconnect(GTK_WIDGET(control),
			"any_signal::value-changed",
			G_CALLBACK(ags_line_member_dial_changed_callback),
			line_member,
			NULL);
  }else if(g_type_is_a(line_member->widget_type, GTK_TYPE_SCALE)){
    g_object_disconnect(GTK_WIDGET(control),
			"any_signal::value-changed",
			G_CALLBACK(ags_line_member_scale_changed_callback),
			line_member,
			NULL);
  }else if(line_member->widget_type == GTK_TYPE_SPIN_BUTTON){
    g_object_disconnect(GTK_WIDGET(control),
			"any_signal::value-changed",
			G_CALLBACK(ags_line_member_spin_button_changed_callback),
			line_member,
			NULL);
  }else if(line_member->widget_type == GTK_TYPE_CHECK_BUTTON){
    g_object_disconnect(GTK_WIDGET(control),
			"any_signal::clicked",
			G_CALLBACK(ags_line_member_check_button_clicked_callback),
			line_member,
			NULL);
  }else if(line_member->widget_type == GTK_TYPE_TOGGLE_BUTTON){
    g_object_disconnect(GTK_WIDGET(control),
			"any_signal::clicked",
			G_CALLBACK(ags_line_member_toggle_button_clicked_callback),
			line_member,
			NULL);
  }else if(line_member->widget_type == GTK_TYPE_BUTTON){
    g_object_disconnect(GTK_WIDGET(control),
			"any_signal::clicked",
			G_CALLBACK(ags_line_member_button_clicked_callback),
			line_member,
			NULL);
  }
}

GtkWidget*
ags_line_member_get_widget(AgsLineMember *line_member)
{
  return(gtk_frame_get_child((GtkFrame *) line_member));
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
					    "wrap", FALSE,
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

    GRecMutex *port_mutex;    

    GValue value = {0,};

    port = line_member->port;

    if(port == NULL){
      g_warning("ags_line_member_change_port() - no port available");
      return;
    }

    /* get port mutex */
    port_mutex = AGS_PORT_GET_OBJ_MUTEX(port);

    /* change */
    g_rec_mutex_lock(port_mutex);

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
	gdouble val;
	gfloat port_val;
	
	if(GTK_IS_TOGGLE_BUTTON(gtk_bin_get_child((GtkBin *) line_member))){
	  if(((gboolean *) port_data)[0]){
	    val = 1.0;
	  }else{
	    val = 0.0;
	  }
	}else{
	  val = ((gdouble *) port_data)[0];
	}

	port_val = (gfloat) val;

	if(line_member->conversion != NULL){
	  gboolean success;

	  success = FALSE;
	    
	  if(AGS_IS_DIAL(gtk_bin_get_child((GtkBin *) line_member))){
	    success = TRUE;
	  }else if(GTK_IS_RANGE(gtk_bin_get_child((GtkBin *) line_member))){
	    success = TRUE;
	  }else if(GTK_IS_SPIN_BUTTON(gtk_bin_get_child((GtkBin *) line_member))){
	    success = TRUE;
	  }else{
	    g_warning("unsupported child type in conversion");
	  }

	  if(success){
	    port_val = ags_conversion_convert(line_member->conversion,
					      val,
					      FALSE);
	  }
	}

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) port_val);
      }else if(port->port_value_type == G_TYPE_DOUBLE){
	gdouble val;
	gdouble port_val;

	if(GTK_IS_TOGGLE_BUTTON(gtk_bin_get_child((GtkBin *) line_member))){
	  if(((gboolean *) port_data)[0]){
	    val = 1.0;
	  }else{
	    val = 0.0;
	  }
	}else{
	  val = ((gdouble *) port_data)[0];
	}
	
	port_val = val;
	
	if(line_member->conversion != NULL){
	  gboolean success;

	  success = FALSE;
	    
	  if(AGS_IS_DIAL(gtk_bin_get_child((GtkBin *) line_member))){
	    success = TRUE;
	  }else if(GTK_IS_RANGE(gtk_bin_get_child((GtkBin *) line_member))){
	    success = TRUE;
	  }else if(GTK_IS_SPIN_BUTTON(gtk_bin_get_child((GtkBin *) line_member))){
	    success = TRUE;
	  }else{
	    g_warning("unsupported child type in conversion");
	  }

	  if(success){
	    port_val = ags_conversion_convert(line_member->conversion,
					      val,
					      FALSE);
	  }
	}

	g_value_init(&value,
		     G_TYPE_DOUBLE);

	g_value_set_double(&value,
			   port_val);
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

    g_rec_mutex_unlock(port_mutex);
    
    ags_port_safe_write(line_member->port,
			&value);

    if((AGS_LINE_MEMBER_APPLY_RECALL & (line_member->flags)) != 0){
      ags_port_safe_write(line_member->recall_port,
			  &value);
    }
  }

  if((AGS_LINE_MEMBER_RESET_BY_TASK & (line_member->flags)) != 0){
    AgsTask *task;

    AgsApplicationContext *application_context;
  
    application_context = ags_application_context_get_instance();

    task = (AgsTask *) g_object_new(line_member->task_type,
				    line_member->control_port, port_data,
				    NULL);

    ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				  (AgsTask *) task);
  }
}

/**
 * ags_line_change_port:
 * @line_member: an #AgsLineMember
 * @port_data: the port's value
 *
 * Is emitted as port's value is modified.
 *
 * Since: 3.0.0
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

AgsPort*
ags_line_member_find_specifier(GList *recall,
			       gchar *specifier)
{
  AgsPort *current_port;
    
  GList *start_port, *port;

  current_port = NULL;
    
  while(recall != NULL){
    if(ags_recall_test_behaviour_flags(recall->data, AGS_SOUND_BEHAVIOUR_BULK_MODE)){
      recall = recall->next;
	
      continue;
    }

    g_object_get(recall->data,
		 "port", &start_port,
		 NULL);

    port = ags_port_find_specifier(start_port,
				   specifier);
    
#ifdef AGS_DEBUG
    g_message("search port in %s", G_OBJECT_TYPE_NAME(recall->data));
#endif

    if(port != NULL){
      current_port = port->data;
    }

    g_list_free_full(start_port,
		     g_object_unref);

    if(current_port != NULL){
      break;
    }

    /* iterate */
    recall = recall->next;
  }

  return(current_port);
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

  GList *port;
  GList *start_list;
  
  gchar *specifier;

  if(!AGS_IS_LINE_MEMBER(line_member)){
    return(NULL);
  }

  specifier = line_member->specifier;

  if(specifier == NULL){
    return(NULL);
  }

  machine = (AgsMachine *) gtk_widget_get_ancestor(GTK_WIDGET(line_member),
						   AGS_TYPE_MACHINE);

  if(machine == NULL){
    return(NULL);
  }
  
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
    
  /* play context */
  g_object_get(channel,
	       "play", &start_list,
	       NULL);

  channel_port = ags_line_member_find_specifier(start_list,
						specifier);

  g_list_free_full(start_list,
		   g_object_unref);  

  /* recall context */
  g_object_get(channel,
	       "recall", &start_list,
	       NULL);
    
  recall_channel_port = ags_line_member_find_specifier(start_list,
						       specifier);

  g_list_free_full(start_list,
		   g_object_unref);

  if(channel_port != NULL){
    port = g_list_prepend(port,
			  channel_port);
  }

  if(recall_channel_port != NULL){
    port = g_list_prepend(port,
			  recall_channel_port);
  }
  
  /* search audio */
  if(channel_port == NULL &&
     recall_channel_port == NULL){
    GList *start_list;

    /* play context */
    g_object_get(audio,
		 "play", &start_list,
		 NULL);

    audio_port = ags_line_member_find_specifier(start_list,
						specifier);

    g_list_free_full(start_list,
		     g_object_unref);

    /* recall context */
    g_object_get(audio,
		 "recall", &start_list,
		 NULL);

    recall_audio_port = ags_line_member_find_specifier(start_list,
						       specifier);

    g_list_free_full(start_list,
		     g_object_unref);

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
 * Returns: a #GList-struct containing all related #AgsPort
 *
 * Since: 3.0.0
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
 * Since: 3.0.0
 */
void
ags_line_member_chained_event(AgsLineMember *line_member)
{
  AgsMachine *machine;

  if(!AGS_IS_LINE_MEMBER(line_member) ||
     (AGS_LINE_MEMBER_BLOCK_CHAINED & (line_member->flags)) != 0){
    return;
  }
  
  machine = (AgsMachine *) gtk_widget_get_ancestor(GTK_WIDGET(line_member),
						   AGS_TYPE_MACHINE);

  if((AGS_MACHINE_STICKY_CONTROLS & (machine->flags)) != 0){
    AgsPad *pad;
    AgsEffectPad *effect_pad;
    GtkWidget *child_widget;
    
    GtkAdjustment *adjustment;
    
    gboolean is_active;

    pad = (AgsPad *) gtk_widget_get_ancestor(GTK_WIDGET(line_member),
					     AGS_TYPE_PAD);

    effect_pad = (AgsEffectPad *) gtk_widget_get_ancestor(GTK_WIDGET(line_member),
							  AGS_TYPE_EFFECT_PAD);

    is_active = FALSE;

    child_widget = gtk_bin_get_child((GtkBin *) line_member);
    
    if(AGS_IS_DIAL(child_widget) ||
       GTK_IS_SPIN_BUTTON(child_widget) ||
       GTK_IS_SCALE(child_widget)){
      g_object_get(gtk_bin_get_child((GtkBin *) line_member),
		   "adjustment", &adjustment,
		   NULL);
    }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
      is_active = gtk_toggle_button_get_active((GtkToggleButton *) gtk_bin_get_child(GTK_BIN(line_member)));
    }

    if(pad != NULL){
      AgsLine *line;

      GList *list_line, *list_line_start;
      GList *list_line_member, *list_line_member_start;

      line = (AgsLine *) gtk_widget_get_ancestor(GTK_WIDGET(line_member),
						 AGS_TYPE_LINE);
      
      list_line =
	list_line_start = gtk_container_get_children(GTK_CONTAINER(pad->expander_set));

      while((list_line = ags_line_find_next_grouped(list_line)) != NULL){
	if(list_line->data != line){
	  list_line_member =
	    list_line_member_start = gtk_container_get_children((GtkContainer *) AGS_LINE(list_line->data)->expander->table);

	  while(list_line_member != NULL){
	    if(AGS_IS_LINE_MEMBER(list_line_member->data) &&
	       !g_strcmp0(line_member->specifier,
			  AGS_LINE_MEMBER(list_line_member->data)->specifier)){
	      AGS_LINE_MEMBER(list_line_member->data)->flags |= AGS_LINE_MEMBER_BLOCK_CHAINED;

	      child_widget = gtk_bin_get_child(GTK_BIN(list_line_member->data));

	      if(AGS_IS_DIAL(child_widget)){
		ags_dial_set_value((AgsDial *) child_widget,
				   gtk_adjustment_get_value(adjustment));
	      }else if(GTK_IS_SPIN_BUTTON(child_widget)){
		gtk_spin_button_set_value((GtkSpinButton *) child_widget,
					  gtk_adjustment_get_value(adjustment));
	      }else if(GTK_IS_SCALE(child_widget)){
		gtk_range_set_value((GtkRange *) child_widget,
				    gtk_adjustment_get_value(adjustment));
	      }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
		gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
					     is_active);
	      }else if(GTK_IS_BUTTON(child_widget)){
		gtk_button_clicked((GtkButton *) child_widget);
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

      effect_line = (AgsEffectLine *) gtk_widget_get_ancestor(GTK_WIDGET(line_member),
							      AGS_TYPE_EFFECT_LINE);
      
      list_effect_line =
	list_effect_line_start = gtk_container_get_children(GTK_CONTAINER(effect_pad->grid));

      while((list_effect_line = ags_effect_line_find_next_grouped(list_effect_line)) != NULL){
	if(list_effect_line->data != effect_line){
	  list_line_member =
	    list_line_member_start = gtk_container_get_children((GtkContainer *) AGS_EFFECT_LINE(list_effect_line->data)->grid);

	  while(list_line_member != NULL){
	    if(!g_strcmp0(line_member->specifier,
			 AGS_LINE_MEMBER(list_line_member->data)->specifier)){
	      AGS_LINE_MEMBER(list_line_member->data)->flags |= AGS_LINE_MEMBER_BLOCK_CHAINED;

	      child_widget = gtk_bin_get_child(GTK_BIN(list_line_member->data));

	      if(AGS_IS_DIAL(child_widget)){
		ags_dial_set_value((AgsDial *) child_widget,
				   gtk_adjustment_get_value(adjustment));
	      }else if(GTK_IS_SPIN_BUTTON(child_widget)){
		gtk_spin_button_set_value((GtkSpinButton *) child_widget,
					  gtk_adjustment_get_value(adjustment));
	      }else if(GTK_IS_SCALE(child_widget)){
		gtk_range_set_value((GtkRange *) child_widget,
				    gtk_adjustment_get_value(adjustment));
	      }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
		gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
					     is_active);
	      }else if(GTK_IS_BUTTON(child_widget)){
		gtk_button_clicked((GtkButton *) child_widget);
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
 * Create a new instance of #AgsLineMember
 *
 * Returns: the new #AgsLineMember
 *
 * Since: 3.0.0
 */
AgsLineMember*
ags_line_member_new()
{
  AgsLineMember *line_member;

  line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
					       NULL);

  return(line_member);
}
