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

#include <ags/GSequencer/ags_bulk_member.h>
#include <ags/GSequencer/ags_bulk_member_callbacks.h>

#include <ags/GSequencer/ags_ui_provider.h>
#include <ags/GSequencer/ags_window.h>
#include <ags/GSequencer/ags_effect_bulk.h>

#include <ags/i18n.h>

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
void ags_bulk_member_finalize(GObject *gobject);

void ags_bulk_member_connect(AgsConnectable *connectable);
void ags_bulk_member_disconnect(AgsConnectable *connectable);

void ags_bulk_member_change_port_all(AgsBulkMember *bulk_member,
				     GList *list,
				     gpointer port_data);

void ags_bulk_member_real_change_port(AgsBulkMember *bulk_member,
				      gpointer port_data);

AgsPort* ags_bulk_member_find_specifier(GList *recall,
					gchar *specifier);

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
  PROP_WIDGET_ORIENTATION,
  PROP_WIDGET_LABEL,
  PROP_PLAY_CONTAINER,
  PROP_RECALL_CONTAINER,
  PROP_PLUGIN_NAME,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_SPECIFIER,
  PROP_PORT_INDEX,
  PROP_CONTROL_PORT,
  PROP_SCALE_PRECISION,
  PROP_STEP_COUNT,
  PROP_CONVERSION,
  PROP_TASK_TYPE,
  PROP_BULK_PORT,
  PROP_RECALL_BULK_PORT,
};

static gpointer ags_bulk_member_parent_class = NULL;
static guint bulk_member_signals[LAST_SIGNAL];

GType
ags_bulk_member_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_bulk_member = 0;

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
						  "AgsBulkMember", &ags_bulk_member_info,
						  0);

    g_type_add_interface_static(ags_type_bulk_member,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_bulk_member);
  }

  return g_define_type_id__volatile;
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_ulong("widget-type",
				  i18n_pspec("widget type of bulk member"),
				  i18n_pspec("The widget type this bulk member packs"),
				  0, G_MAXULONG, 
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WIDGET_TYPE,
				  param_spec);

  /**
   * AgsBulkMember:widget-orientation:
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
   * AgsBulkMember:widget-label:
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
   * AgsBulkMember:play-container:
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
   * AgsBulkMember:recall-container:
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
   * AgsBulkMember:plugin-name:
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
   * AgsBulkMember:specifier:
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
   * AgsBulkMember:filename:
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
   * AgsBulkMember:effect:
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
   * AgsBulkMember:port-index:
   *
   * The port index.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("port-index",
				 i18n_pspec("port index"),
				 i18n_pspec("The port's index"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_INDEX,
				  param_spec);

  /**
   * AgsBulkMember:control-port:
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
   * AgsBulkMember:scale-precision:
   *
   * If bulk member has integer ports, this is the number of steps.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("scale-precision",
				 i18n_pspec("scale precision of bulk members port"),
				 i18n_pspec("The scale precision this bulk members port has"),
				 0,
				 G_MAXUINT,
				 (guint) AGS_DIAL_DEFAULT_PRECISION,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCALE_PRECISION,
				  param_spec);

  /**
   * AgsBulkMember:step-count:
   *
   * If bulk member has logarithmic ports, this is the number of step count.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("step-count",
				   i18n_pspec("step count of bulk members port"),
				   i18n_pspec("The step count this bulk members port has"),
				   0.0,
				   G_MAXDOUBLE,
				   AGS_LADSPA_CONVERSION_DEFAULT_STEP_COUNT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_STEP_COUNT,
				  param_spec);
  
  /**
   * AgsBulkMember:conversion:
   *
   * The conversion of the plugin.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("conversion",
				   i18n_pspec("conversion"),
				   i18n_pspec("The conversion of the plugin"),
				   AGS_TYPE_CONVERSION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONVERSION,
				  param_spec);

  /**
   * AgsBulkMember:bulk-port:
   *
   * The playback bulk port to be added.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("bulk-port",
				   i18n_pspec("a bulk port"),
				   i18n_pspec("The bulk port to add"),
				   AGS_TYPE_PORT,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BULK_PORT,
				  param_spec);

  /**
   * AgsBulkMember:recall-bulk-port:
   *
   * The recall bulk port to be added.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("recall-bulk-port",
				   i18n_pspec("a recall bulk port"),
				   i18n_pspec("The bulk port to add"),
				   AGS_TYPE_PORT,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_BULK_PORT,
				  param_spec);

  /**
   * AgsBulkMember:task-type:
   *
   * The task type to apply the ports.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_ulong("task-type",
				  i18n_pspec("task type to apply"),
				  i18n_pspec("The task type to apply the ports"),
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
   * Since: 3.0.0
   */
  bulk_member_signals[CHANGE_PORT] =
    g_signal_new("change-port",
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
   * Since: 3.0.0
   */
  bulk_member_signals[FIND_PORT] =
    g_signal_new("find-port",
		 G_TYPE_FROM_CLASS(bulk_member),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsBulkMemberClass, find_port),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
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

  AgsApplicationContext *application_context;

  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();

  g_signal_connect_after((GObject *) bulk_member, "parent_set",
			 G_CALLBACK(ags_bulk_member_parent_set_callback), (gpointer) bulk_member);

  bulk_member->flags = (AGS_BULK_MEMBER_RESET_BY_ATOMIC |
			AGS_BULK_MEMBER_APPLY_RECALL);
  bulk_member->port_flags = 0;
  
  bulk_member->widget_type = AGS_TYPE_DIAL;
  bulk_member->widget_orientation = GTK_ORIENTATION_VERTICAL;
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

  gtk_container_add(GTK_CONTAINER(bulk_member),
		    (GtkWidget *) dial);

  bulk_member->widget_label = NULL;

  bulk_member->play_container = NULL;
  bulk_member->recall_container = NULL;

  bulk_member->plugin_name = NULL;
  
  bulk_member->filename = NULL;
  bulk_member->effect = NULL;

  bulk_member->specifier = NULL;

  bulk_member->port_index = 0;
  bulk_member->control_port = NULL;

  bulk_member->scale_precision = AGS_DIAL_DEFAULT_PRECISION;
  bulk_member->step_count = AGS_LADSPA_CONVERSION_DEFAULT_STEP_COUNT;

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

      GtkAdjustment *adjustment;

      AgsApplicationContext *application_context;

      GType widget_type;

      gdouble gui_scale_factor;
      gdouble lower_value, upper_value;
      gdouble current_value;
      gboolean active;

      widget_type = g_value_get_ulong(value);

      if(widget_type == bulk_member->widget_type){
	return;
      }

      application_context = ags_application_context_get_instance();

      child = gtk_bin_get_child(GTK_BIN(bulk_member));

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

      bulk_member->widget_type = widget_type;
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
	
	g_object_get(bulk_member,
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
	  g_warning("ags_bulk_member_set_property() - unknown child type %s", g_type_name(widget_type));
	}
      }

      /* add */
      gtk_container_add(GTK_CONTAINER(bulk_member),
			new_child);
			
    }
    break;
  case PROP_WIDGET_ORIENTATION:
    {
      GtkWidget *child;

      AgsApplicationContext *application_context;

      guint widget_orientation;
      gdouble gui_scale_factor;

      widget_orientation = g_value_get_uint(value);
      
      bulk_member->widget_orientation = widget_orientation;

      application_context = ags_application_context_get_instance();

      /* scale factor */
      gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

      child = gtk_bin_get_child(GTK_BIN(bulk_member));

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

      if(label == bulk_member->widget_label){
	return;
      }

      if(bulk_member->widget_label != NULL){
	g_free(bulk_member->widget_label);
      }
      
      bulk_member->widget_label = g_strdup(label);
      ags_bulk_member_set_label(bulk_member, label);
    }
    break;
  case PROP_PLAY_CONTAINER:
    {
      AgsRecallContainer *play_container;
      
      play_container = (AgsRecallContainer *) g_value_get_object(value);

      if(play_container == bulk_member->play_container){
	return;
      }

      if(bulk_member->play_container != NULL){
	g_object_unref(bulk_member->play_container);
      }

      if(play_container != NULL){
	g_object_ref(play_container);
      }

      bulk_member->play_container = play_container;
    }
    break;
  case PROP_RECALL_CONTAINER:
    {
      AgsRecallContainer *recall_container;
      
      recall_container = (AgsRecallContainer *) g_value_get_object(value);

      if(recall_container == bulk_member->recall_container){
	return;
      }

      if(bulk_member->recall_container != NULL){
	g_object_unref(bulk_member->recall_container);
      }

      if(recall_container != NULL){
	g_object_ref(recall_container);
      }

      bulk_member->recall_container = recall_container;
    }
    break;
  case PROP_PLUGIN_NAME:
    {
      gchar *plugin_name;

      plugin_name = g_value_get_string(value);

      if(plugin_name == bulk_member->plugin_name){
	return;
      }

      if(bulk_member->plugin_name != NULL){
	g_free(bulk_member->plugin_name);
      }
      
      bulk_member->plugin_name = g_strdup(plugin_name);
    }
    break;
  case PROP_FILENAME:
    {
      gchar *str;
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == bulk_member->filename){
	return;
      }

      if(bulk_member->filename != NULL){
	g_free(bulk_member->filename);
      }

      if(filename != NULL){
	if(!g_file_test(filename,
			G_FILE_TEST_EXISTS)){
	  AgsWindow *window;

	  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) bulk_member);

	  str = g_strdup_printf("%s %s",
				i18n("Plugin file not present"),
				filename);
	  ags_window_show_error(window,
				str);

	  g_free(str);
	}
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

      if(bulk_member->effect != NULL){
	g_free(bulk_member->effect);
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
  case PROP_PORT_INDEX:
    {
      guint port_index;

      port_index = g_value_get_uint(value);

      bulk_member->port_index = port_index;
    }
    break;
  case PROP_CONTROL_PORT:
    {
      gchar *control_port;

      control_port = g_value_get_string(value);

      if(control_port == bulk_member->control_port){
	return;
      }

      if(bulk_member->control_port != NULL){
	g_free(bulk_member->control_port);
      }
      
      bulk_member->control_port = g_strdup(control_port);
    }
    break;
  case PROP_SCALE_PRECISION:
    {
      GtkWidget *child;
      
      guint scale_precision;

      scale_precision = g_value_get_uint(value);

      bulk_member->scale_precision = scale_precision;
      child = gtk_bin_get_child(GTK_BIN(bulk_member));

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

      bulk_member->step_count = step_count;
    }
    break;
  case PROP_CONVERSION:
    {
      AgsConversion *conversion;

      conversion = g_value_get_object(value);
      
      if(conversion == bulk_member->conversion){
	return;
      }
      
      if(bulk_member->conversion != NULL){
	g_object_unref(bulk_member->conversion);
      }

      if(conversion != NULL){
	g_object_ref(conversion);
      }
      
      bulk_member->conversion = conversion;
    }
    break;
  case PROP_BULK_PORT:
    {
      AgsPort *port;
      AgsBulkPort *bulk_port;
      
      port = (AgsPort *) g_value_get_object(value);

      if(port == NULL){
	return;
      }
      
      if(ags_bulk_port_find(bulk_member->bulk_port, port) != NULL){
	return;
      }

      if((AGS_PORT_INFINITE_RANGE & (port->flags)) != 0){
	GtkWidget *child;

	child = gtk_bin_get_child(GTK_BIN(bulk_member));

	//TODO:JK: add more types

	if(AGS_IS_DIAL(child)){
	  AGS_DIAL(child)->flags |= AGS_DIAL_SEEMLESS_MODE;
	}
      }
      
      bulk_port = ags_bulk_port_alloc(port,
				      0, 0);
      bulk_member->bulk_port = g_list_prepend(bulk_member->bulk_port,
					      bulk_port);
    }
    break;
  case PROP_RECALL_BULK_PORT:
    {
      AgsPort *port;
      AgsBulkPort *bulk_port;
      
      port = (AgsPort *) g_value_get_object(value);

      if(port == NULL){
	return;
      }
      
      if(ags_bulk_port_find(bulk_member->recall_bulk_port, port) != NULL){
	return;
      }

      bulk_port = ags_bulk_port_alloc(port,
				      0, 0);
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
  case PROP_WIDGET_ORIENTATION:
    {
      g_value_set_uint(value, bulk_member->widget_orientation);
    }
    break;
  case PROP_WIDGET_LABEL:
    {
      g_value_set_string(value, bulk_member->widget_label);
    }
    break;
  case PROP_PLAY_CONTAINER:
    {
      g_value_set_object(value, bulk_member->play_container);
    }
    break;
  case PROP_RECALL_CONTAINER:
    {
      g_value_set_object(value, bulk_member->recall_container);
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
  case PROP_PORT_INDEX:
    {
      g_value_set_uint(value, bulk_member->port_index);
    }
    break;
  case PROP_CONTROL_PORT:
    {
      g_value_set_string(value, bulk_member->control_port);
    }
    break;
  case PROP_SCALE_PRECISION:
    {
      g_value_set_uint(value, bulk_member->scale_precision);
    }
    break;
  case PROP_STEP_COUNT:
    {
      g_value_set_double(value, bulk_member->step_count);
    }
    break;
  case PROP_CONVERSION:
    {
      g_value_set_object(value, bulk_member->conversion);
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
ags_bulk_member_finalize(GObject *gobject)
{
  AgsBulkMember *bulk_member;

  bulk_member = (AgsBulkMember *) gobject;
  
  g_free(bulk_member->widget_label);

  if(bulk_member->play_container != NULL){
    g_object_unref(bulk_member->play_container);
  }

  if(bulk_member->recall_container != NULL){
    g_object_unref(bulk_member->recall_container);
  }

  g_free(bulk_member->plugin_name);

  g_free(bulk_member->filename);
  g_free(bulk_member->effect);

  g_free(bulk_member->control_port);

  if(bulk_member->conversion != NULL){
    g_object_unref(bulk_member->conversion);
  }
  
  /* bulk port */
  g_list_free_full(bulk_member->bulk_port,
		   (GDestroyNotify) ags_bulk_port_free);
  
  g_list_free_full(bulk_member->recall_bulk_port,
		   (GDestroyNotify) ags_bulk_port_free);

  /* call parent */
  G_OBJECT_CLASS(ags_bulk_member_parent_class)->finalize(gobject);
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
  
  ags_bulk_member_remap_bulk_port(bulk_member);
  
  control = gtk_bin_get_child(GTK_BIN(bulk_member));

  if((AGS_BULK_MEMBER_APPLY_INITIAL & (bulk_member->flags)) != 0){
    GtkAdjustment *adjustment;

    gboolean active;  
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
      
      active = gtk_toggle_button_get_active((GtkToggleButton *) control);

      is_toggled = TRUE;
    }

    if(is_toggled){
      ags_bulk_member_change_port(bulk_member,
				  &(active));
    }else if(adjustment != NULL){
      gdouble value;

      value = gtk_adjustment_get_value(adjustment);
      
      ags_bulk_member_change_port(bulk_member,
				  &value);
    }
    
    bulk_member->flags &= (~AGS_BULK_MEMBER_APPLY_INITIAL);
  }

  /* widget callback */
  if(bulk_member->widget_type == AGS_TYPE_DIAL){
    g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			   G_CALLBACK(ags_bulk_member_dial_changed_callback), bulk_member);
  }else if(g_type_is_a(bulk_member->widget_type, GTK_TYPE_SCALE)){
    g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			   G_CALLBACK(ags_bulk_member_vscale_changed_callback), bulk_member);
  }else if(bulk_member->widget_type == GTK_TYPE_SPIN_BUTTON){
    g_signal_connect_after(GTK_WIDGET(control), "value-changed",
			   G_CALLBACK(ags_bulk_member_spin_button_changed_callback), bulk_member);
  }else if(bulk_member->widget_type == GTK_TYPE_CHECK_BUTTON){
    g_signal_connect_after(GTK_WIDGET(control), "clicked",
			   G_CALLBACK(ags_bulk_member_check_button_clicked_callback), bulk_member);
  }else if(bulk_member->widget_type == GTK_TYPE_TOGGLE_BUTTON){
    g_signal_connect_after(GTK_WIDGET(control), "clicked",
			   G_CALLBACK(ags_bulk_member_toggle_button_clicked_callback), bulk_member);
  }else if(bulk_member->widget_type == GTK_TYPE_BUTTON){
    g_signal_connect_after(GTK_WIDGET(control), "clicked",
			   G_CALLBACK(ags_bulk_member_button_clicked_callback), bulk_member);
  }
}

void
ags_bulk_member_disconnect(AgsConnectable *connectable)
{
  AgsBulkMember *bulk_member;
  GtkWidget *control;

  bulk_member = AGS_BULK_MEMBER(connectable);

  if((AGS_BULK_MEMBER_CONNECTED & (bulk_member->flags)) == 0){
    return;
  }

  bulk_member->flags &= (~AGS_BULK_MEMBER_CONNECTED);

  control = gtk_bin_get_child(GTK_BIN(bulk_member));

  /* widget callback */
  if(bulk_member->widget_type == AGS_TYPE_DIAL){
    g_object_disconnect(GTK_WIDGET(control),
			"any_signal::value-changed",
			G_CALLBACK(ags_bulk_member_dial_changed_callback),
			bulk_member,
			NULL);
  }else if(g_type_is_a(bulk_member->widget_type, GTK_TYPE_SCALE)){
    g_object_disconnect(GTK_WIDGET(control),
			"any_signal::value-changed",
			G_CALLBACK(ags_bulk_member_vscale_changed_callback),
			bulk_member,
			NULL);
  }else if(bulk_member->widget_type == GTK_TYPE_SPIN_BUTTON){
    g_object_disconnect(GTK_WIDGET(control),
			"any_signal::value-changed",
			G_CALLBACK(ags_bulk_member_spin_button_changed_callback),
			bulk_member,
			NULL);
  }else if(bulk_member->widget_type == GTK_TYPE_CHECK_BUTTON){
    g_object_disconnect(GTK_WIDGET(control),
			"any_signal::clicked",
			G_CALLBACK(ags_bulk_member_check_button_clicked_callback),
			bulk_member,
			NULL);
  }else if(bulk_member->widget_type == GTK_TYPE_TOGGLE_BUTTON){
    g_object_disconnect(GTK_WIDGET(control),
			"any_signal::clicked",
			G_CALLBACK(ags_bulk_member_toggle_button_clicked_callback),
			bulk_member,
			NULL);
  }else if(bulk_member->widget_type == GTK_TYPE_BUTTON){
    g_object_disconnect(GTK_WIDGET(control),
			"any_signal::clicked",
			G_CALLBACK(ags_bulk_member_button_clicked_callback),
			bulk_member,
			NULL);
  }
}

GtkWidget*
ags_bulk_member_get_widget(AgsBulkMember *bulk_member)
{
  return(gtk_bin_get_child(GTK_BIN(bulk_member)));
}

/**
 * ags_bulk_port_alloc:
 * @port: the #AgsPort to set
 * @pad: the pad
 * @audio_channel: the audio channel
 * 
 * Allocate #AgsBulkPort-struct.
 *
 * Returns: the newly allocated #AgsBulkPort-struct
 *
 * Since: 3.0.0
 */
AgsBulkPort*
ags_bulk_port_alloc(AgsPort *port,
		    guint pad, guint audio_channel)
{
  AgsBulkPort *bulk_port;

  bulk_port = (AgsBulkPort *) g_malloc(sizeof(AgsBulkPort));

  bulk_port->port = port;

  if(port != NULL){
    g_object_ref(port);
  }
  
  bulk_port->pad = pad;
  bulk_port->audio_channel = audio_channel;
    
  bulk_port->port_data = &(port->port_value);
  bulk_port->active = FALSE;
  
  return(bulk_port);
}

/**
 * ags_bulk_port_free:
 * @bulk_port: the #AgsBulkPort-struct
 * 
 * Free @bulk_port.
 *
 * Since: 3.0.0
 */
void
ags_bulk_port_free(AgsBulkPort *bulk_port)
{
  if(bulk_port == NULL){
    return;
  }
  
  g_object_unref(bulk_port->port);

  g_free(bulk_port);
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
 * Since: 3.0.0
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
		 "label", label,
		 NULL);
  }else{
    gtk_frame_set_label_widget((GtkFrame *) bulk_member,
			       (GtkWidget *) g_object_new(GTK_TYPE_LABEL,
							  "wrap", FALSE,
							  "use-markup", TRUE,
							  "label", g_markup_printf_escaped("<small>%s</small>", label),
							  NULL));
  }


  bulk_member->widget_label = g_strdup(label);
}

void
ags_bulk_member_change_port_all(AgsBulkMember *bulk_member,
				GList *list,
				gpointer port_data)
{
  AgsPort *port;

  GRecMutex *port_mutex;
    
  while(list != NULL){
    GValue value = {0,};

    port = AGS_BULK_PORT(list->data)->port;

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
	  
	if(GTK_IS_TOGGLE_BUTTON(gtk_bin_get_child((GtkBin *) bulk_member))){
	  if(((gboolean *) port_data)[0]){
	    val = 1.0;
	  }else{
	    val = 0.0;
	  }
	}else{
	  val = ((gdouble *) port_data)[0];
	}

	port_val = (gfloat) val;
	  
	if(bulk_member->conversion != NULL){
	  gboolean success;

	  success = FALSE;
	    
	  if(AGS_IS_DIAL(gtk_bin_get_child(GTK_BIN(bulk_member)))){
	    success = TRUE;
	  }else{
	    g_warning("unsupported child type in conversion");
	  }

	  if(success){
	    port_val = (gfloat) ags_conversion_convert(bulk_member->conversion,
						       val,
						       FALSE);
	  }
	}
	  
	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  port_val);
      }else if(port->port_value_type == G_TYPE_DOUBLE){
	gdouble val;
	gdouble port_val;
	  
	if(GTK_IS_TOGGLE_BUTTON(gtk_bin_get_child((GtkBin *) bulk_member))){
	  if(((gboolean *) port_data)[0]){
	    val = 1.0;
	  }else{
	    val = 0.0;
	  }
	}else{
	  val = ((gdouble *) port_data)[0];
	}

	port_val = val;
	  
	if(bulk_member->conversion != NULL){
	  gboolean success;

	  success = FALSE;
	    
	  if(AGS_IS_DIAL(gtk_bin_get_child(GTK_BIN(bulk_member)))){
	    success = TRUE;
	  }else{
	    g_warning("unsupported child type in conversion");
	  }

	  if(success){
	    port_val = ags_conversion_convert(bulk_member->conversion,
					      val,
					      TRUE);
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
        
    ags_port_safe_write(port,
			&value);

    list = list->next;
  }
}

void
ags_bulk_member_real_change_port(AgsBulkMember *bulk_member,
				 gpointer port_data)
{
  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();

  if((AGS_BULK_MEMBER_RESET_BY_ATOMIC & (bulk_member->flags)) != 0){
    ags_bulk_member_change_port_all(bulk_member,
				    bulk_member->bulk_port,
				    port_data);

    if((AGS_BULK_MEMBER_APPLY_RECALL & (bulk_member->flags)) != 0){
      ags_bulk_member_change_port_all(bulk_member,
				      bulk_member->recall_bulk_port,
				      port_data);
    }
  }

  if((AGS_BULK_MEMBER_RESET_BY_TASK & (bulk_member->flags)) != 0){
    AgsTask *task;
    
    task = (AgsTask *) g_object_new(bulk_member->task_type,
				    bulk_member->control_port, port_data,
				    NULL);

    ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
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
 * Since: 3.0.0
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

AgsPort*
ags_bulk_member_find_specifier(GList *recall,
			       gchar *specifier){
  AgsPort *current_port;
    
  GList *start_port, *port;

  current_port = NULL;
    
  while(recall != NULL){
    if(!ags_recall_test_behaviour_flags(recall->data, AGS_SOUND_BEHAVIOUR_BULK_MODE)){
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
ags_bulk_member_real_find_port(AgsBulkMember *bulk_member)
{
  GtkWidget *effect_bulk;

  AgsAudio *audio;
  AgsChannel *start_channel;
  AgsChannel *channel, *next_channel;
  AgsPort *audio_port, *channel_port;
  AgsPort *recall_audio_port, *recall_channel_port;
  
  gchar *specifier;

  if(!AGS_IS_BULK_MEMBER(bulk_member)){
    return(NULL);
  }

  specifier = bulk_member->specifier;

  if(specifier == NULL){
    return(NULL);
  }

  effect_bulk = gtk_widget_get_ancestor(GTK_WIDGET(bulk_member),
					AGS_TYPE_EFFECT_BULK);

  audio = AGS_EFFECT_BULK(effect_bulk)->audio;
  
  audio_port = NULL;
  channel_port = NULL;
  
  recall_audio_port = NULL;
  recall_channel_port = NULL;
  
  /* search channels */
  start_channel = NULL;
  
  if(g_type_is_a(AGS_EFFECT_BULK(effect_bulk)->channel_type, AGS_TYPE_OUTPUT)){
    g_object_get(audio,
		 "output", &start_channel,
		 NULL);
  }else if(g_type_is_a(AGS_EFFECT_BULK(effect_bulk)->channel_type, AGS_TYPE_INPUT)){
    g_object_get(audio,
		 "input", &start_channel,
		 NULL);
  }

  channel = start_channel;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    GList *start_list;

    guint pad, audio_channel;

    start_list = NULL;
    
    pad = 0;
    audio_channel = 0;
    
    /* play context */
    g_object_get(channel,
		 "pad", &pad,
		 "audio-channel", &audio_channel,
		 "play", &start_list,
		 NULL);
    
    channel_port = ags_bulk_member_find_specifier(start_list,
						  specifier);

    if(channel_port != NULL &&
       ags_bulk_port_find(bulk_member->bulk_port, channel_port) == NULL){
      bulk_member->bulk_port = g_list_prepend(bulk_member->bulk_port,
					      ags_bulk_port_alloc(channel_port,
								  pad, audio_channel));
    }

    g_list_free_full(start_list,
		     g_object_unref);

    /* recall context */
    g_object_get(channel,
		 "recall", &start_list,
		 NULL);
    
    recall_channel_port = ags_bulk_member_find_specifier(start_list,
							 specifier);
    
    if(recall_channel_port != NULL &&
       ags_bulk_port_find(bulk_member->recall_bulk_port, recall_channel_port) == NULL){
      bulk_member->recall_bulk_port = g_list_prepend(bulk_member->recall_bulk_port,
						     ags_bulk_port_alloc(recall_channel_port,
									 pad, audio_channel));
    }

    g_list_free_full(start_list,
		     g_object_unref);

    /* iterate */
    next_channel = ags_channel_next(channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }

  /* search audio */
  if(channel_port == NULL &&
     recall_channel_port == NULL){
    GList *start_list;

    /* play context */
    g_object_get(audio,
		 "play", &start_list,
		 NULL);

    audio_port = ags_bulk_member_find_specifier(start_list,
						specifier);

    if(audio_port != NULL &&
       ags_bulk_port_find(bulk_member->bulk_port, audio_port) == NULL){
      bulk_member->bulk_port = g_list_prepend(bulk_member->bulk_port,
					      ags_bulk_port_alloc(audio_port,
								  0, 0));
    }
    
    g_list_free_full(start_list,
		     g_object_unref);

    /* recall context */
    g_object_get(audio,
		 "recall", &start_list,
		 NULL);

    recall_audio_port = ags_bulk_member_find_specifier(start_list,
						       specifier);
    
    if(recall_audio_port != NULL &&
       ags_bulk_port_find(bulk_member->recall_bulk_port, recall_audio_port) == NULL){
      bulk_member->recall_bulk_port = g_list_prepend(bulk_member->recall_bulk_port,
						     ags_bulk_port_alloc(recall_audio_port,
									 0, 0));
    }
    
    g_list_free_full(start_list,
		     g_object_unref);
  }

  return(g_list_copy(bulk_member->bulk_port));
}

/**
 * ags_bulk_member_find_port:
 * @bulk_member: an #AgsBulkMember
 *
 * Lookup ports of assigned recalls.
 *
 * Returns: a #GList-struct containing all related #AgsPort
 *
 * Since: 3.0.0
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
 * ags_bulk_member_find_effect_and_specifier:
 * @bulk_member: the #GList-struct containing #AgsBulkMember
 * @filename: the filename
 * @effect: the effect
 * @specifier: the specifier
 * 
 * Find next matching bulk member by effect and specifier.
 * 
 * Returns: the next matching #GList-struct or %NULL if not found
 *
 * Since: 3.0.0
 */
GList*
ags_bulk_member_find_effect_and_specifier(GList *bulk_member,
					  gchar *filename, gchar *effect,
					  gchar *specifier)
{
  if(filename == NULL ||
     effect == NULL ||
     specifier == NULL){
    return(NULL);
  }
  
  while(bulk_member != NULL){
    if(!AGS_IS_BULK_MEMBER(bulk_member->data)){
      bulk_member = bulk_member->next;

      continue;
    }
    
    if(!g_strcmp0(AGS_BULK_MEMBER(bulk_member->data)->filename,
		  filename) &&
       !g_strcmp0(AGS_BULK_MEMBER(bulk_member->data)->effect,
		  effect) &&
       !g_strcmp0(AGS_BULK_MEMBER(bulk_member->data)->specifier,
		  specifier)){
      break;
    }
    
    /* iterate */
    bulk_member = bulk_member->next;
  }
  
  return(bulk_member);
}

/**
 * ags_bulk_member_remap_bulk_port:
 * @bulk_member: the #AgsBulkMember
 * 
 * Remap bulk port.
 *
 * Since: 3.3.0
 */
void
ags_bulk_member_remap_bulk_port(AgsBulkMember *bulk_member)
{
  if(!AGS_IS_BULK_MEMBER(bulk_member)){
    return;
  }

  g_list_free_full(bulk_member->bulk_port,
		   (GDestroyNotify) ags_bulk_port_free);

  bulk_member->bulk_port = NULL;

  bulk_member->bulk_port = ags_bulk_member_find_port(bulk_member);
}

/**
 * ags_bulk_member_new:
 *
 * Creates an #AgsBulkMember
 *
 * Returns: a new #AgsBulkMember
 *
 * Since: 3.0.0
 */
AgsBulkMember*
ags_bulk_member_new()
{
  AgsBulkMember *bulk_member;

  bulk_member = (AgsBulkMember *) g_object_new(AGS_TYPE_BULK_MEMBER,
					       NULL);

  return(bulk_member);
}
