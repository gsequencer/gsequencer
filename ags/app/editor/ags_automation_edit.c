/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/app/editor/ags_automation_edit.h>
#include <ags/app/editor/ags_automation_edit_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_navigation.h>
#include <ags/app/ags_composite_editor.h>

#include <ags/app/editor/ags_automation_meta.h>

#include <gdk/gdkkeysyms.h>

#include <cairo.h>
#include <math.h>

#include <ags/i18n.h>

void ags_automation_edit_class_init(AgsAutomationEditClass *automation_edit);
void ags_automation_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_edit_init(AgsAutomationEdit *automation_edit);
void ags_automation_edit_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_automation_edit_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_automation_edit_dispose(GObject *gobject);
void ags_automation_edit_finalize(GObject *gobject);

void ags_automation_edit_connect(AgsConnectable *connectable);
void ags_automation_edit_disconnect(AgsConnectable *connectable);

void ags_automation_edit_realize(GtkWidget *widget);
void ags_automation_edit_unrealize(GtkWidget *widget);

void ags_automation_edit_measure(GtkWidget *widget,
				 GtkOrientation orientation,
				 int for_size,
				 int *minimum,
				 int *natural,
				 int *minimum_baseline,
				 int *natural_baseline);
void ags_automation_edit_size_allocate(GtkWidget *widget,
				       int width,
				       int height,
				       int baseline);

void ags_automation_edit_show(GtkWidget *widget);

void ags_automation_edit_frame_clock_update_callback(GdkFrameClock *frame_clock,
						     AgsAutomationEdit *automation_edit);

void ags_automation_edit_drawing_area_button_press_position_cursor(GtkWidget *editor,
								   GtkWidget *toolbar,
								   AgsAutomationEdit *automation_edit,
								   AgsMachine *machine,
								   gint n_press,
								   gdouble x, gdouble y);
void ags_automation_edit_drawing_area_button_press_add_acceleration(GtkWidget *editor,
								    GtkWidget *toolbar,
								    AgsAutomationEdit *automation_edit,
								    AgsMachine *machine,
								    gint n_press,
								    gdouble x, gdouble y);
void ags_automation_edit_drawing_area_button_press_select_acceleration(GtkWidget *editor,
								       GtkWidget *toolbar,
								       AgsAutomationEdit *automation_edit,
								       AgsMachine *machine,
								       gint n_press,
								       gdouble x, gdouble y);

void ags_automation_edit_drawing_area_button_release_position_cursor(GtkWidget *editor,
								     GtkWidget *toolbar,
								     AgsAutomationEdit *automation_edit,
								     AgsMachine *machine,
								     gint n_press, gdouble x, gdouble y);
void ags_automation_edit_drawing_area_button_release_add_acceleration(GtkWidget *editor,
								      GtkWidget *toolbar,
								      AgsAutomationEdit *automation_edit,
								      AgsMachine *machine,
								      gint n_press,
								      gdouble x, gdouble y);
void ags_automation_edit_drawing_area_button_release_delete_acceleration(GtkWidget *editor,
									 GtkWidget *toolbar,
									 AgsAutomationEdit *automation_edit,
									 AgsMachine *machine,
									 gint n_press,
									 gdouble x, gdouble y);
void ags_automation_edit_drawing_area_button_release_select_acceleration(GtkWidget *editor,
									 GtkWidget *toolbar,
									 AgsAutomationEdit *automation_edit,
									 AgsMachine *machine,
									 gint n_press,
									 gdouble x, gdouble y);

gboolean ags_automation_edit_gesture_click_pressed_callback(GtkGestureClick *event_controller,
							    gint n_press,
							    gdouble x,
							    gdouble y,
							    AgsAutomationEdit *automation_edit);
gboolean ags_automation_edit_gesture_click_released_callback(GtkGestureClick *event_controller,
							     gint n_press,
							     gdouble x,
							     gdouble y,
							     AgsAutomationEdit *automation_edit);

gboolean ags_automation_edit_key_pressed_callback(GtkEventControllerKey *event_controller,
						  guint keyval,
						  guint keycode,
						  GdkModifierType state,
						  AgsAutomationEdit *automation_edit);
gboolean ags_automation_edit_key_released_callback(GtkEventControllerKey *event_controller,
						   guint keyval,
						   guint keycode,
						   GdkModifierType state,
						   AgsAutomationEdit *automation_edit);
gboolean ags_automation_edit_modifiers_callback(GtkEventControllerKey *event_controller,
						GdkModifierType keyval,
						AgsAutomationEdit *automation_edit);


void ags_automation_edit_drawing_area_motion_notify_position_cursor(GtkWidget *editor,
								    GtkWidget *toolbar,
								    AgsAutomationEdit *automation_edit,
								    AgsMachine *machine,
								    gdouble x, gdouble y);
void ags_automation_edit_drawing_area_motion_notify_add_acceleration(GtkWidget *editor,
								     GtkWidget *toolbar,
								     AgsAutomationEdit *automation_edit,
								     AgsMachine *machine,
								     gdouble x, gdouble y);
void ags_automation_edit_drawing_area_motion_notify_select_acceleration(GtkWidget *editor,
									GtkWidget *toolbar,
									AgsAutomationEdit *automation_edit,
									AgsMachine *machine,
									gdouble x, gdouble y);

gboolean ags_automation_edit_motion_callback(GtkEventControllerMotion *event_controller,
					     gdouble x,
					     gdouble y,
					     AgsAutomationEdit *automation_edit);

gboolean ags_automation_edit_auto_scroll_timeout(GtkWidget *widget);

/**
 * SECTION:ags_automation_edit
 * @short_description: edit automations
 * @title: AgsAutomationEdit
 * @section_id:
 * @include: ags/app/editor/ags_automation_edit.h
 *
 * The #AgsAutomationEdit lets you edit automations.
 */

enum{
  PROP_0,
  PROP_CHANNEL_TYPE,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_CONTROL_SPECIFIER,
  PROP_CONTROL_NAME,
  PROP_LOWER,
  PROP_UPPER,
  PROP_DEFAULT_VALUE,
};

static gpointer ags_automation_edit_parent_class = NULL;

static GQuark quark_accessible_object = 0;

GHashTable *ags_automation_edit_auto_scroll = NULL;

GType
ags_automation_edit_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_automation_edit = 0;

    static const GTypeInfo ags_automation_edit_info = {
      sizeof (AgsAutomationEditClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_automation_edit_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAutomationEdit),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_automation_edit_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_automation_edit_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_automation_edit = g_type_register_static(GTK_TYPE_GRID,
						      "AgsAutomationEdit", &ags_automation_edit_info,
						      0);
    
    g_type_add_interface_static(ags_type_automation_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_automation_edit);
  }

  return g_define_type_id__volatile;
}

void
ags_automation_edit_class_init(AgsAutomationEditClass *automation_edit)
{
  GtkWidgetClass *widget;

  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_automation_edit_parent_class = g_type_class_peek_parent(automation_edit);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(automation_edit);

  gobject->set_property = ags_automation_edit_set_property;
  gobject->get_property = ags_automation_edit_get_property;

  gobject->dispose = ags_automation_edit_dispose;
  gobject->finalize = ags_automation_edit_finalize;

  /* properties */
  /**
   * AgsAutomationArea:channel-type:
   *
   * The target channel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_gtype("channel-type",
				  i18n_pspec("assigned channel type"),
				  i18n_pspec("The channel type it is assigned with"),
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL_TYPE,
				  param_spec);

  /**
   * AgsChannel:filename:
   *
   * The assigned #AgsFilename representing this channel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("filename assigned with"),
				   i18n_pspec("The filename it is assigned with"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsChannel:effect:
   *
   * The assigned #AgsEffect representing this channel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("effect",
				   i18n_pspec("effect assigned with"),
				   i18n_pspec("The effect name it is assigned with"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT,
				  param_spec);

  /**
   * AgsChannel:control-specifier:
   *
   * The assigned #AgsControl-Specifier representing this channel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("control-specifier",
				   i18n_pspec("assigned control specifier"),
				   i18n_pspec("The control specifier it is assigned with"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONTROL_SPECIFIER,
				  param_spec);

  /**
   * AgsChannel:control-name:
   *
   * The assigned #AgsControl-Name representing this channel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("control-name",
				   i18n_pspec("displayed control name"),
				   i18n_pspec("The control name to display"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONTROL_NAME,
				  param_spec);

  /**
   * AgsAutomationEdit:lower:
   *
   * The automation edit's lower range.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("lower",
				   "lower",
				   "The lower of automation edit",
				   -G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   AGS_AUTOMATION_EDIT_DEFAULT_LOWER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOWER,
				  param_spec);

  /**
   * AgsAutomationEdit:upper:
   *
   * The automation edit's upper range.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("upper",
				   "upper",
				   "The upper of automation edit",
				   -G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   AGS_AUTOMATION_EDIT_DEFAULT_UPPER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UPPER,
				  param_spec);

  /**
   * AgsAutomationEdit:default-value:
   *
   * The automation edit's default value.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("default-value",
				   "default value",
				   "The default value of automation edit",
				   -G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   AGS_AUTOMATION_EDIT_DEFAULT_VALUE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEFAULT_VALUE,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) automation_edit;

  widget->realize = ags_automation_edit_realize;
  widget->unrealize = ags_automation_edit_unrealize;

  widget->measure = ags_automation_edit_measure;
  widget->size_allocate = ags_automation_edit_size_allocate;

  widget->show = ags_automation_edit_show;  
}

void
ags_automation_edit_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_automation_edit_connect;
  connectable->disconnect = ags_automation_edit_disconnect;
}

void
ags_automation_edit_init(AgsAutomationEdit *automation_edit)
{
  GtkEventController *event_controller;
  GtkAdjustment *adjustment;

  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) automation_edit,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_automation_edit_key_pressed_callback), automation_edit);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_automation_edit_key_released_callback), automation_edit);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_automation_edit_modifiers_callback), automation_edit);

  event_controller = (GtkEventController *) gtk_gesture_click_new();
  gtk_widget_add_controller((GtkWidget *) automation_edit,
			    event_controller);

  g_signal_connect(event_controller, "pressed",
		   G_CALLBACK(ags_automation_edit_gesture_click_pressed_callback), automation_edit);

  g_signal_connect(event_controller, "released",
		   G_CALLBACK(ags_automation_edit_gesture_click_released_callback), automation_edit);

  event_controller = gtk_event_controller_motion_new();
  gtk_widget_add_controller((GtkWidget *) automation_edit,
			    event_controller);

  g_signal_connect(event_controller, "motion",
		   G_CALLBACK(ags_automation_edit_motion_callback), automation_edit);

  automation_edit->flags = 0;
  automation_edit->connectable_flags = 0;
  automation_edit->mode = AGS_AUTOMATION_EDIT_NO_EDIT_MODE;

  automation_edit->button_mask = 0;
  automation_edit->key_mask = 0;
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  automation_edit->note_offset = 0;
  automation_edit->note_offset_absolute = 0;

  automation_edit->point_radius = (guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_POINT_RADIUS);

  automation_edit->scan_width = (guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_SCAN_WIDTH);
  automation_edit->scan_height = (guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_SCAN_HEIGHT);

  automation_edit->control_width = (guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH);
  automation_edit->control_height = (guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_HEIGHT);

  automation_edit->step_count = (guint) (AGS_AUTOMATION_EDIT_DEFAULT_STEP_COUNT);
  
  automation_edit->cursor_position_x = AGS_AUTOMATION_EDIT_DEFAULT_CURSOR_POSITION_X;
  automation_edit->cursor_position_y = AGS_AUTOMATION_EDIT_DEFAULT_CURSOR_POSITION_Y;

  automation_edit->selected_acceleration_border = AGS_AUTOMATION_EDIT_DEFAULT_SELECTED_ACCELERATION_BORDER;

  automation_edit->selection_x0 = 0;
  automation_edit->selection_x1 = 0;
  automation_edit->selection_y0 = 0;
  automation_edit->selection_y1 = 0;

  automation_edit->current_acceleration = NULL;

  automation_edit->ruler = ags_ruler_new(GTK_ORIENTATION_HORIZONTAL,
					 AGS_RULER_DEFAULT_STEP,
					 AGS_RULER_DEFAULT_FACTOR,
					 AGS_RULER_DEFAULT_PRECISION,
					 AGS_RULER_DEFAULT_SCALE_PRECISION);

  gtk_widget_set_visible((GtkWidget *) automation_edit->ruler,
			 FALSE);
  
  gtk_grid_attach(GTK_GRID(automation_edit),
		  (GtkWidget *) automation_edit->ruler,
		  0, 0,
		  1, 1);

  automation_edit->channel_type = G_TYPE_NONE;
  
  automation_edit->filename = NULL;
  automation_edit->effect = NULL;

  automation_edit->control_specifier = NULL;
  automation_edit->control_name = NULL;

  automation_edit->lower = AGS_AUTOMATION_EDIT_DEFAULT_LOWER;
  automation_edit->upper = AGS_AUTOMATION_EDIT_DEFAULT_UPPER;

  automation_edit->default_value = AGS_AUTOMATION_EDIT_DEFAULT_VALUE;

  automation_edit->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_can_focus((GtkWidget *) automation_edit->drawing_area,
			   TRUE);
  gtk_widget_set_focusable((GtkWidget *) automation_edit->drawing_area,
			   TRUE);

  gtk_widget_set_halign((GtkWidget *) automation_edit->drawing_area,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) automation_edit->drawing_area,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand((GtkWidget *) automation_edit->drawing_area,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) automation_edit->drawing_area,
			 FALSE);

  gtk_widget_set_size_request((GtkWidget *) automation_edit->drawing_area,
			      -1, AGS_SCALE_DEFAULT_HEIGHT_REQUEST);
  
  gtk_grid_attach((GtkGrid *) automation_edit,
		  (GtkWidget *) automation_edit->drawing_area,
		  0, 1,
		  1, 1);

  /* vscrollbar */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, automation_edit->control_height, 1.0);
  automation_edit->vscrollbar = (GtkScrollbar *) gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL,
								   adjustment);
  gtk_widget_set_visible((GtkWidget *) automation_edit->vscrollbar,
			 FALSE);
  gtk_widget_set_size_request((GtkWidget *) automation_edit->vscrollbar,
			      -1, (guint) (gui_scale_factor * AGS_SCALE_DEFAULT_HEIGHT_REQUEST));
  gtk_grid_attach((GtkGrid *) automation_edit,
		  (GtkWidget *) automation_edit->vscrollbar,
		  1, 1,
		  1, 1);

  /* hscrollbar */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (gdouble) automation_edit->control_width, 1.0);
  automation_edit->hscrollbar = (GtkScrollbar *) gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL,
								   adjustment);
  gtk_widget_set_visible((GtkWidget *) automation_edit->hscrollbar,
			 FALSE);
  gtk_widget_set_size_request((GtkWidget *) automation_edit->hscrollbar,
			      -1, -1);
  gtk_grid_attach(GTK_GRID(automation_edit),
		   (GtkWidget *) automation_edit->hscrollbar,
		   0, 2,
		   1, 1);

  /* auto-scroll */
  if(ags_automation_edit_auto_scroll == NULL){
    ags_automation_edit_auto_scroll = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							    NULL,
							    NULL);
  }

  g_hash_table_insert(ags_automation_edit_auto_scroll,
		      automation_edit, ags_automation_edit_auto_scroll_timeout);
  g_timeout_add(1000 / 30, (GSourceFunc) ags_automation_edit_auto_scroll_timeout, (gpointer) automation_edit);
}

void
ags_automation_edit_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsAutomationEdit *automation_edit;

  automation_edit = AGS_AUTOMATION_EDIT(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
    {
      automation_edit->channel_type = g_value_get_gtype(value);
    }
    break;
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(automation_edit->filename == filename){
	return;
      }

      automation_edit->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;

      effect = g_value_get_string(value);

      if(automation_edit->effect == effect){
	return;
      }

      automation_edit->effect = g_strdup(effect);
    }
    break;
  case PROP_CONTROL_SPECIFIER:
    {
      gchar *control_specifier;

      control_specifier = g_value_get_string(value);

      if(automation_edit->control_specifier == control_specifier){
	return;
      }

      automation_edit->control_specifier = g_strdup(control_specifier);
    }
    break;
  case PROP_CONTROL_NAME:
    {
      gchar *control_name;

      control_name = g_value_get_string(value);

      if(automation_edit->control_name == control_name){
	return;
      }

      automation_edit->control_name = g_strdup(control_name);
    }
    break;
  case PROP_LOWER:
    {
      automation_edit->lower = g_value_get_double(value);

      gtk_widget_queue_draw((GtkWidget *) automation_edit);
    }
    break;
  case PROP_UPPER:
    {
      automation_edit->upper = g_value_get_double(value);

      gtk_widget_queue_draw((GtkWidget *) automation_edit);
    }
    break;
  case PROP_DEFAULT_VALUE:
    {
      automation_edit->default_value = g_value_get_double(value);

      gtk_widget_queue_draw((GtkWidget *) automation_edit);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_edit_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsAutomationEdit *automation_edit;

  automation_edit = AGS_AUTOMATION_EDIT(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
    {
      g_value_set_gtype(value, automation_edit->channel_type);
    }
    break;
  case PROP_FILENAME:
    {
      g_value_set_string(value, automation_edit->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, automation_edit->effect);
    }
    break;
  case PROP_CONTROL_SPECIFIER:
    {
      g_value_set_string(value, automation_edit->control_specifier);
    }
    break;
  case PROP_CONTROL_NAME:
    {
      g_value_set_string(value, automation_edit->control_name);
    }
    break;
  case PROP_LOWER:
    {
      g_value_set_double(value,
			 automation_edit->lower);
    }
    break;
  case PROP_UPPER:
    {
      g_value_set_double(value,
			 automation_edit->upper);
    }
    break;
  case PROP_DEFAULT_VALUE:
    {
      g_value_set_double(value,
			 automation_edit->default_value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_edit_dispose(GObject *gobject)
{
  AgsAutomationEdit *automation_edit;
  
  automation_edit = AGS_AUTOMATION_EDIT(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_automation_edit_parent_class)->dispose(gobject);
}

void
ags_automation_edit_finalize(GObject *gobject)
{
  AgsAutomationEdit *automation_edit;
  
  automation_edit = AGS_AUTOMATION_EDIT(gobject);
  
  /* remove auto scroll */
  g_hash_table_remove(ags_automation_edit_auto_scroll,
		      automation_edit);

  /* call parent */
  G_OBJECT_CLASS(ags_automation_edit_parent_class)->finalize(gobject);
}

void
ags_automation_edit_connect(AgsConnectable *connectable)
{
  AgsAutomationEdit *automation_edit;

  automation_edit = AGS_AUTOMATION_EDIT(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (automation_edit->connectable_flags)) != 0){
    return;
  }
  
  automation_edit->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  /* drawing area */
  gtk_drawing_area_set_draw_func(automation_edit->drawing_area,
				 (GtkDrawingAreaDrawFunc) ags_automation_edit_draw_callback,
				 automation_edit,
				 NULL);

  g_signal_connect_after((GObject *) automation_edit->drawing_area, "resize",
			 G_CALLBACK(ags_automation_edit_drawing_area_resize_callback), (gpointer) automation_edit);

  /* scrollbars */
  g_signal_connect_after((GObject *) gtk_scrollbar_get_adjustment(automation_edit->vscrollbar), "value-changed",
			 G_CALLBACK(ags_automation_edit_vscrollbar_value_changed), (gpointer) automation_edit);

  g_signal_connect_after((GObject *) gtk_scrollbar_get_adjustment(automation_edit->hscrollbar), "value-changed",
			 G_CALLBACK(ags_automation_edit_hscrollbar_value_changed), (gpointer) automation_edit);
}

void
ags_automation_edit_disconnect(AgsConnectable *connectable)
{
  AgsAutomationEdit *automation_edit;

  automation_edit = AGS_AUTOMATION_EDIT(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (automation_edit->connectable_flags)) == 0){
    return;
  }
  
  automation_edit->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  /* drawing area */
  gtk_drawing_area_set_draw_func(automation_edit->drawing_area,
				 NULL,
				 NULL,
				 NULL);
  
  g_object_disconnect((GObject *) automation_edit->drawing_area,
		      "any_signal::resize",
		      G_CALLBACK(ags_automation_edit_drawing_area_resize_callback),
		      (gpointer) automation_edit,
		      NULL);

  /* scrollbars */
  g_object_disconnect((GObject *) gtk_scrollbar_get_adjustment(automation_edit->vscrollbar),
		      "any_signal::value-changed",
		      G_CALLBACK(ags_automation_edit_vscrollbar_value_changed),
		      (gpointer) automation_edit,
		      NULL);

  g_object_disconnect((GObject *) gtk_scrollbar_get_adjustment(automation_edit->hscrollbar),
		      "any_signal::value-changed",
		      G_CALLBACK(ags_automation_edit_hscrollbar_value_changed),
		      (gpointer) automation_edit,
		      NULL);
}

gboolean
ags_automation_edit_key_pressed_callback(GtkEventControllerKey *event_controller,
					 guint keyval,
					 guint keycode,
					 GdkModifierType state,
					 AgsAutomationEdit *automation_edit)
{
  AgsCompositeEditor *editor;
  AgsMachine *machine;

  AgsApplicationContext *application_context;
  
  guint l_control_key, r_control_key;  
  gboolean key_handled;

  application_context = ags_application_context_get_instance();
  
  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Meta_L ||
     keyval == GDK_KEY_Meta_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R ){
    key_handled = FALSE;
  }else{
    key_handled = TRUE;
  }

  machine = NULL;
  
  editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_edit,
							  AGS_TYPE_COMPOSITE_EDITOR);
    
  machine = editor->selected_machine;

#if defined(AGS_OSXAPI)
  l_control_key = AGS_AUTOMATION_EDIT_KEY_L_META;
  r_control_key = AGS_AUTOMATION_EDIT_KEY_R_META;
#else
  l_control_key = AGS_AUTOMATION_EDIT_KEY_L_CONTROL;
  r_control_key = AGS_AUTOMATION_EDIT_KEY_R_CONTROL;
#endif  
  
  if(machine != NULL){
    switch(keyval){
    case GDK_KEY_Control_L:
      {
	automation_edit->key_mask |= AGS_AUTOMATION_EDIT_KEY_L_CONTROL;
      }
      break;
    case GDK_KEY_Control_R:
      {
	automation_edit->key_mask |= AGS_AUTOMATION_EDIT_KEY_R_CONTROL;
      }
      break;
    case GDK_KEY_Shift_L:
      {
	automation_edit->key_mask |= AGS_AUTOMATION_EDIT_KEY_L_SHIFT;
      }
      break;
    case GDK_KEY_Shift_R:
      {
	automation_edit->key_mask |= AGS_AUTOMATION_EDIT_KEY_R_SHIFT;
      }
      break;
    case GDK_KEY_Meta_L:
      {
	automation_edit->key_mask |= AGS_AUTOMATION_EDIT_KEY_L_META;
      }
      break;
    case GDK_KEY_Meta_R:
      {
	automation_edit->key_mask |= AGS_AUTOMATION_EDIT_KEY_R_META;
      }
      break;
    case GDK_KEY_a:
      {
	/* select all accelerations */
	if((l_control_key & (automation_edit->key_mask)) != 0 || (r_control_key & (automation_edit->key_mask)) != 0){
	  ags_composite_editor_select_all(editor);
	}
      }
      break;
    case GDK_KEY_c:
      {
	/* copy accelerations */
	if((l_control_key & (automation_edit->key_mask)) != 0 || (r_control_key & (automation_edit->key_mask)) != 0){
	  ags_composite_editor_copy(editor);
	}
      }
      break;
    case GDK_KEY_v:
      {
	/* paste accelerations */
	if((l_control_key & (automation_edit->key_mask)) != 0 || (r_control_key & (automation_edit->key_mask)) != 0){
	  ags_composite_editor_paste(editor);
	}
      }
      break;
    case GDK_KEY_x:
      {
	/* cut accelerations */
	if((l_control_key & (automation_edit->key_mask)) != 0 || (r_control_key & (automation_edit->key_mask)) != 0){
	  ags_composite_editor_cut(editor);
	}
      }
      break;
    case GDK_KEY_i:
      {
	/* invert accelerations */
	if((l_control_key & (automation_edit->key_mask)) != 0 || (r_control_key & (automation_edit->key_mask)) != 0){
	  ags_composite_editor_invert(editor);
	}
      }
      break;
    case GDK_KEY_m:
      {
	/* meta */
	if((l_control_key & (automation_edit->key_mask)) != 0 || (r_control_key & (automation_edit->key_mask)) != 0){
	  AgsAutomationMeta *automation_meta;

	  automation_meta = NULL;
	  
	  automation_meta = (AgsAutomationMeta *) editor->automation_edit->edit_meta;

	  if((AGS_AUTOMATION_META_ENABLED & (automation_meta->flags)) != 0){
	    automation_meta->flags &= (~AGS_AUTOMATION_META_ENABLED);

	    gtk_widget_hide((GtkWidget *) automation_meta);
	  }else{
	    automation_meta->flags |= AGS_AUTOMATION_META_ENABLED;

	    gtk_widget_show((GtkWidget *) automation_meta);

	    ags_automation_meta_refresh(automation_meta);
	  }
	}
      }
      break;
    }
  }

  return(key_handled);
}

gboolean
ags_automation_edit_key_released_callback(GtkEventControllerKey *event_controller,
					  guint keyval,
					  guint keycode,
					  GdkModifierType state,
					  AgsAutomationEdit *automation_edit)
{  
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;

  AgsApplicationContext *application_context;
  
  GtkAllocation allocation;

  guint l_control_key, r_control_key;  
  double zoom_factor;
  gint i;
  gboolean key_handled;

  application_context = ags_application_context_get_instance();
  
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  machine = composite_editor->selected_machine;
  
  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);
  
  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Meta_L ||
     keyval == GDK_KEY_Meta_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R ){
    key_handled = FALSE;
  }else{
    key_handled = TRUE;
  }

#if defined(AGS_OSXAPI)
  l_control_key = AGS_AUTOMATION_EDIT_KEY_L_META;
  r_control_key = AGS_AUTOMATION_EDIT_KEY_R_META;
#else
  l_control_key = AGS_AUTOMATION_EDIT_KEY_L_CONTROL;
  r_control_key = AGS_AUTOMATION_EDIT_KEY_R_CONTROL;
#endif  

  if(machine != NULL){    
    /* check key value */
    switch(keyval){
    case GDK_KEY_Control_L:
      {
	automation_edit->key_mask &= (~AGS_AUTOMATION_EDIT_KEY_L_CONTROL);
      }
      break;
    case GDK_KEY_Control_R:
      {
	automation_edit->key_mask &= (~AGS_AUTOMATION_EDIT_KEY_R_CONTROL);
      }
      break;
    case GDK_KEY_Shift_L:
      {
	automation_edit->key_mask &= (~AGS_AUTOMATION_EDIT_KEY_L_SHIFT);
      }
      break;
    case GDK_KEY_Shift_R:
      {
	automation_edit->key_mask &= (~AGS_AUTOMATION_EDIT_KEY_R_SHIFT);
      }
      break;
    case GDK_KEY_Meta_L:
      {
	automation_edit->key_mask &= (~AGS_AUTOMATION_EDIT_KEY_L_META);
      }
      break;
    case GDK_KEY_Meta_R:
      {
	automation_edit->key_mask &= (~AGS_AUTOMATION_EDIT_KEY_R_META);
      }
      break;
    case GDK_KEY_Left:
    case GDK_KEY_leftarrow:
      {
	gdouble x0_offset;

	/* position cursor */
	if(automation_edit->cursor_position_x > 0){
	  if(automation_edit->cursor_position_x - (zoom_factor * automation_edit->control_width) > 0){
	    automation_edit->cursor_position_x -= (zoom_factor * automation_edit->control_width);
	  }else{
	    automation_edit->cursor_position_x = 0;
	  }
	}

	x0_offset = automation_edit->cursor_position_x / zoom_factor;
      
	if(x0_offset / zoom_factor < gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->hscrollbar))){
	  gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(automation_edit->hscrollbar),
			      x0_offset / zoom_factor);
	}
      }
      break;
    case GDK_KEY_Right:
    case GDK_KEY_rightarrow:
      {
	gdouble x0_offset;
	  
	/* position cursor */      
	if(automation_edit->cursor_position_x < AGS_AUTOMATION_DEFAULT_LENGTH){
	  automation_edit->cursor_position_x += (zoom_factor * automation_edit->control_width);
	}

	x0_offset = automation_edit->cursor_position_x / zoom_factor;
      
	if((x0_offset + automation_edit->control_width) / zoom_factor > gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->hscrollbar)) + allocation.width){
	  gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(automation_edit->hscrollbar),
			      x0_offset / zoom_factor);
	}
      }
      break;
    case GDK_KEY_Up:
    case GDK_KEY_uparrow:
      {
	GtkAdjustment *vscrollbar_adjustment;
	
	gdouble y0_offset;

	gdouble c_range;
	guint g_range;

	vscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->vscrollbar);

	if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
	  c_range = exp(automation_edit->upper) - exp(automation_edit->lower);
	}else{
	  c_range = automation_edit->upper - automation_edit->lower;
	}
	
	g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;
	
	if(automation_edit->cursor_position_y < automation_edit->upper){
	  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
	    automation_edit->cursor_position_y += log((1.0 / g_range) * c_range);
	  }else{
	    automation_edit->cursor_position_y += ((1.0 / g_range) * c_range);
	  }

	  if(automation_edit->cursor_position_y > automation_edit->upper){
	    automation_edit->cursor_position_y = automation_edit->upper;
	  }
	}

	if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
	  y0_offset = exp(automation_edit->cursor_position_y) / c_range * g_range;
	}else{
	  y0_offset = automation_edit->cursor_position_y / c_range * g_range;
	}
	
	if(y0_offset < gtk_adjustment_get_value(vscrollbar_adjustment)){
	  gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(automation_edit->vscrollbar),
			      y0_offset);
	}
      }
      break;
    case GDK_KEY_Down:
    case GDK_KEY_downarrow:
      {
	GtkAdjustment *vscrollbar_adjustment;

	gdouble y0_offset;

	gdouble c_range;
	guint g_range;

	vscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->vscrollbar);

	if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
	  c_range = exp(automation_edit->upper) - exp(automation_edit->lower);
	}else{
	  c_range = automation_edit->upper - automation_edit->lower;
	}

	g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;
	      
	if(automation_edit->cursor_position_y < automation_edit->lower){
	  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
	    automation_edit->cursor_position_y -= log((1.0 / g_range) * c_range);
	  }else{
	    automation_edit->cursor_position_y -= ((1.0 / g_range) * c_range);
	  }

	  if(automation_edit->cursor_position_y < automation_edit->lower){
	    automation_edit->cursor_position_y = automation_edit->lower;
	  }
	}

	if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
	  y0_offset = exp(automation_edit->cursor_position_y) / c_range * g_range;
	}else{
	  y0_offset = automation_edit->cursor_position_y / c_range * g_range;
	}
	
	if(y0_offset < gtk_adjustment_get_value(vscrollbar_adjustment)){
	  gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(automation_edit->vscrollbar),
			      y0_offset);
	}
      }
      break;
    case GDK_KEY_space:
      {
	AgsAcceleration *acceleration;

	acceleration = ags_acceleration_new();

	acceleration->x = automation_edit->cursor_position_x;
	acceleration->y = automation_edit->cursor_position_y;

	/* add acceleration */
	ags_composite_editor_add_acceleration((GtkWidget *) composite_editor,
					      acceleration);
      }
      break;
    case GDK_KEY_Delete:
      {
	/* delete acceleration */
	ags_composite_editor_delete_acceleration((GtkWidget *) composite_editor,
						 automation_edit->cursor_position_x, automation_edit->cursor_position_y);
      }
      break;
    }

    gtk_widget_queue_draw((GtkWidget *) automation_edit);
  }

  return(key_handled);
}

gboolean
ags_automation_edit_modifiers_callback(GtkEventControllerKey *event_controller,
				       GdkModifierType keyval,
				       AgsAutomationEdit *automation_edit)
{
  return(FALSE);
}

void
ags_automation_edit_drawing_area_motion_notify_position_cursor(GtkWidget *editor,
							       GtkWidget *toolbar,
							       AgsAutomationEdit *automation_edit,
							       AgsMachine *machine,
							       gdouble x, gdouble y)
{
  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;

  AgsApplicationContext *application_context;
   
  GtkAllocation allocation;
  
  gdouble gui_scale_factor;
  double zoom_factor;
  gdouble c_range;
  guint g_range;
  gdouble value, step;
  gdouble upper, lower, step_count;

  application_context = ags_application_context_get_instance();
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  
  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  vscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->vscrollbar);
  hscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->hscrollbar);

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* cursor position */
  automation_edit->cursor_position_x = (guint) zoom_factor * (x + gtk_adjustment_get_value(hscrollbar_adjustment));

  lower = automation_edit->lower;
  upper = automation_edit->upper;

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) y;
    automation_edit->cursor_position_y = lower * pow(upper / lower, step / (step_count - 1)) + lower;
  }else{
    if((AGS_AUTOMATION_EDIT_INTEGER & (automation_edit->flags)) != 0 ||
       (AGS_AUTOMATION_EDIT_TOGGLED & (automation_edit->flags)) != 0){
      automation_edit->cursor_position_y = round((((allocation.height - y) / g_range) * c_range) + lower);
    }else{
      automation_edit->cursor_position_y = (((allocation.height - y) / g_range) * c_range) + lower;
    }
  }

#ifdef AGS_DEBUG
  g_message("%lu %f", automation_edit->cursor_position_x, automation_edit->cursor_position_y);
#endif
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) automation_edit->drawing_area);
}

void
ags_automation_edit_drawing_area_motion_notify_add_acceleration(GtkWidget *editor,
								GtkWidget *toolbar,
								AgsAutomationEdit *automation_edit,
								AgsMachine *machine,
								gdouble x, gdouble y)
{
  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;

  AgsAcceleration *acceleration;
    
  AgsApplicationContext *application_context;
   
  GtkAllocation allocation;
  
  gdouble gui_scale_factor;
  double zoom_factor;
  gdouble c_range;
  guint g_range;
  gdouble value, step;
  gdouble upper, lower, step_count;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  
  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  acceleration = automation_edit->current_acceleration;
    
  if(acceleration == NULL){
    return;
  }
    
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  vscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->vscrollbar);
  hscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->hscrollbar);

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* acceleration */
  acceleration->x = (guint) zoom_factor * (x + gtk_adjustment_get_value(hscrollbar_adjustment));

  lower = automation_edit->lower;
  upper = automation_edit->upper;
    
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) y;

    acceleration->y = lower * pow(upper / lower, step / (step_count - 1)) + lower;
  }else{
    if((AGS_AUTOMATION_EDIT_INTEGER & (automation_edit->flags)) != 0 ||
       (AGS_AUTOMATION_EDIT_TOGGLED & (automation_edit->flags)) != 0){
      acceleration->y = round((((allocation.height - y) / g_range) * c_range) + lower);
    }else{
      acceleration->y = (((allocation.height - y) / g_range) * c_range) + lower;
    }
  }
    
#ifdef AGS_DEBUG
  g_message("%lu %f", acceleration->x, acceleration->y);
#endif
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) automation_edit->drawing_area);
}

void
ags_automation_edit_drawing_area_motion_notify_select_acceleration(GtkWidget *editor,
								   GtkWidget *toolbar,
								   AgsAutomationEdit *automation_edit,
								   AgsMachine *machine,
								   gdouble x, gdouble y)
{
  double zoom_factor;

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  
  if(zoom_factor * x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->hscrollbar)) >= 0.0){
    automation_edit->selection_x1 = (guint) zoom_factor * x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->hscrollbar));
  }else{
    automation_edit->selection_x1 = 0.0;
  }
  
  if(y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->vscrollbar)) >= 0.0){
    automation_edit->selection_y1 = (guint) y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->vscrollbar));
  }else{
    automation_edit->selection_y1 = 0.0;
  }
    
  gtk_widget_queue_draw((GtkWidget *) automation_edit->drawing_area);
}

gboolean
ags_automation_edit_motion_callback(GtkEventControllerMotion *event_controller,
				    gdouble x,
				    gdouble y,
				    AgsAutomationEdit *automation_edit)
{
  AgsCompositeEditor *editor;
  AgsCompositeToolbar *toolbar;
  AgsMachine *machine;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  editor = gtk_widget_get_ancestor(GTK_WIDGET(automation_edit),
				   AGS_TYPE_COMPOSITE_EDITOR);
    
  toolbar = editor->toolbar;

  machine = editor->selected_machine;
  
  gtk_widget_grab_focus((GtkWidget *) automation_edit->drawing_area);

  if(machine != NULL &&
     (AGS_AUTOMATION_EDIT_BUTTON_1 & (automation_edit->button_mask)) != 0){
    if(automation_edit->mode == AGS_AUTOMATION_EDIT_POSITION_CURSOR){
      ags_automation_edit_drawing_area_motion_notify_position_cursor((GtkWidget *) editor,
								     (GtkWidget *) toolbar,
								     automation_edit,
								     machine,
								     x, y);
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_ADD_ACCELERATION){
      ags_automation_edit_drawing_area_motion_notify_add_acceleration((GtkWidget *) editor,
								      (GtkWidget *) toolbar,
								      automation_edit,
								      machine,
								      x, y);
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_DELETE_ACCELERATION){
      //ACCELERATION:JK: only takes action on release
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_SELECT_ACCELERATION){
      ags_automation_edit_drawing_area_motion_notify_select_acceleration((GtkWidget *) editor,
									 (GtkWidget *) toolbar,
									 automation_edit,
									 machine,
									 x, y);
    }
  }

  return(FALSE);
}

void
ags_automation_edit_drawing_area_button_press_position_cursor(GtkWidget *editor,
							      GtkWidget *toolbar,
							      AgsAutomationEdit *automation_edit,
							      AgsMachine *machine,
							      gint n_press,
							      gdouble x, gdouble y)
{
  AgsApplicationContext *application_context;
  
  GtkAllocation allocation;
    
  gdouble gui_scale_factor;
  double zoom_factor;
  gdouble c_range;
  guint g_range;
  gdouble value, step;
  gdouble upper, lower, step_count;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
    
  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  g_range = gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->vscrollbar)) + allocation.height;

  /* cursor position */
  automation_edit->cursor_position_x = (guint) (zoom_factor * (x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->hscrollbar))));

  lower = automation_edit->lower;
  upper = automation_edit->upper;
    
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) y;
      
    automation_edit->cursor_position_y = lower * pow(upper / lower, step / (step_count - 1)) + lower;
  }else{
    if((AGS_AUTOMATION_EDIT_INTEGER & (automation_edit->flags)) != 0 ||
       (AGS_AUTOMATION_EDIT_TOGGLED & (automation_edit->flags)) != 0){
      automation_edit->cursor_position_y = round((((allocation.height - y) / g_range) * c_range) + lower);
    }else{
      automation_edit->cursor_position_y = (((allocation.height - y) / g_range) * c_range) + lower;
    }
  }

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) automation_edit);
}
  
void
ags_automation_edit_drawing_area_button_press_add_acceleration(GtkWidget *editor,
							       GtkWidget *toolbar,
							       AgsAutomationEdit *automation_edit,
							       AgsMachine *machine,
							       gint n_press,
							       gdouble x, gdouble y)
{
  AgsAcceleration *acceleration;

  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;
    
  GtkAllocation allocation;

  AgsApplicationContext *application_context;  
  
  gdouble gui_scale_factor;
  double zoom_factor;
  gdouble c_range;
  guint g_range;
  gdouble value, step;
  gdouble upper, lower, step_count;
    
  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
    
  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  acceleration = ags_acceleration_new();

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  vscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->vscrollbar);
  hscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->hscrollbar);
    
  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* acceleration */
  acceleration->x = (guint) (zoom_factor * (x + gtk_adjustment_get_value(hscrollbar_adjustment)));

  lower = automation_edit->lower;
  upper = automation_edit->upper;
    
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) y;

    acceleration->y = lower * pow(upper / lower, step / (step_count - 1)) + lower;
  }else{
    if((AGS_AUTOMATION_EDIT_INTEGER & (automation_edit->flags)) != 0 ||
       (AGS_AUTOMATION_EDIT_TOGGLED & (automation_edit->flags)) != 0){
      acceleration->y = round((((allocation.height - y) / g_range) * c_range) + lower);
    }else{
      acceleration->y = (((allocation.height - y) / g_range) * c_range) + lower;
    }
  }
    
  /* current acceleration */
  if(automation_edit->current_acceleration != NULL){
    g_object_unref(automation_edit->current_acceleration);

    automation_edit->current_acceleration = NULL;
  }

  automation_edit->current_acceleration = acceleration;
  g_object_ref(acceleration);

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) automation_edit);
}

void
ags_automation_edit_drawing_area_button_press_select_acceleration(GtkWidget *editor,
								  GtkWidget *toolbar,
								  AgsAutomationEdit *automation_edit,
								  AgsMachine *machine,
								  gint n_press,
								  gdouble x, gdouble y)
{
  AgsApplicationContext *application_context;
  
  double zoom_factor;

  application_context = ags_application_context_get_instance();

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  
  automation_edit->selection_x0 = (guint) zoom_factor * x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->hscrollbar));
  automation_edit->selection_x1 = automation_edit->selection_x0;
    
  automation_edit->selection_y0 = (guint) y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->vscrollbar));
  automation_edit->selection_y1 = automation_edit->selection_y0;

  gtk_widget_queue_draw((GtkWidget *) automation_edit);
}

gboolean
ags_automation_edit_gesture_click_pressed_callback(GtkGestureClick *event_controller,
						   gint n_press,
						   gdouble x,
						   gdouble y,
						   AgsAutomationEdit *automation_edit)
{
  AgsCompositeEditor *editor;
  AgsCompositeToolbar *composite_toolbar;
  AgsMachine *machine;

  AgsApplicationContext *application_context;

  GList *start_tab, *tab;
  
  guint audio_channels;
  guint output_lines, input_lines;
  guint length;
  guint i;
  gboolean replace_notebook;
  gboolean selected_position_cursor, selected_edit, selected_clear, selected_select;
  
  application_context = ags_application_context_get_instance();

  selected_position_cursor = FALSE;
  selected_edit = FALSE;
  selected_select = FALSE;
      
  editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_edit,
							  AGS_TYPE_COMPOSITE_EDITOR);
    
  composite_toolbar = AGS_COMPOSITE_EDITOR(editor)->toolbar;

  machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;
    
  selected_position_cursor = (composite_toolbar->selected_tool == (GtkButton *) composite_toolbar->position) ? TRUE: FALSE;
  selected_edit = (composite_toolbar->selected_tool == (GtkButton *) composite_toolbar->edit) ? TRUE: FALSE;
  selected_clear = (composite_toolbar->selected_tool == (GtkButton *) composite_toolbar->clear) ? TRUE: FALSE;
  selected_select = (composite_toolbar->selected_tool == (GtkButton *) composite_toolbar->select) ? TRUE: FALSE;

  replace_notebook = (AGS_COMPOSITE_EDITOR(editor)->automation_edit->focused_edit != (GtkWidget *) automation_edit) ? TRUE: FALSE;
  
  AGS_COMPOSITE_EDITOR(editor)->automation_edit->focused_edit = (GtkWidget *) automation_edit;
  
  /* automation edit notebook - remove tabs */
  if(replace_notebook){
    tab =
      start_tab = ags_notebook_get_tab(AGS_COMPOSITE_EDITOR(editor)->automation_edit->channel_selector);
  
    while(tab != NULL){
      ags_notebook_remove_tab(AGS_COMPOSITE_EDITOR(editor)->automation_edit->channel_selector,
			      tab->data);

      tab = tab->next;
    }
    
    g_list_free(start_tab);
  }
  
  audio_channels = 2;
    
  output_lines = 2;
  input_lines = 2;
    
  g_object_get(machine->audio,
	       "audio-channels", &audio_channels,
	       "output-lines", &output_lines,
	       "input-lines", &input_lines,
	       NULL);

  length = 2;

  if(automation_edit->channel_type == G_TYPE_NONE){
    length = audio_channels;
  }else if(automation_edit->channel_type == AGS_TYPE_OUTPUT){
    length = output_lines;
  }else if(automation_edit->channel_type == AGS_TYPE_INPUT){
    length = input_lines;
  }

  if(replace_notebook){
    for(i = 0; i < length; i++){
      gchar *str;

      str = g_strdup_printf("line %d",
			    i + 1);
    
      ags_notebook_add_tab(AGS_COMPOSITE_EDITOR(editor)->automation_edit->channel_selector,
			   (GtkToggleButton *) gtk_toggle_button_new_with_label(str));

      g_free(str);
    }
  }
  
  gtk_widget_grab_focus((GtkWidget *) automation_edit->drawing_area);
  gtk_widget_queue_draw((GtkWidget *) automation_edit);

  if(machine != NULL){    
    automation_edit->button_mask = AGS_AUTOMATION_EDIT_BUTTON_1;
    
    if(selected_position_cursor){
      automation_edit->mode = AGS_AUTOMATION_EDIT_POSITION_CURSOR;

      ags_automation_edit_drawing_area_button_press_position_cursor((GtkWidget *) editor,
								    (GtkWidget *) composite_toolbar,
								    automation_edit,
								    machine,
								    n_press,
								    x, y);
    }else if(selected_edit){
      automation_edit->mode = AGS_AUTOMATION_EDIT_ADD_ACCELERATION;

      ags_automation_edit_drawing_area_button_press_add_acceleration((GtkWidget *) editor,
								     (GtkWidget *) composite_toolbar,
								     automation_edit,
								     machine,
								     n_press,
								     x, y);
    }else if(selected_clear){
      automation_edit->mode = AGS_AUTOMATION_EDIT_DELETE_ACCELERATION;
      
      //ACCELERATION:JK: only takes action on release
    }else if(selected_select){
      automation_edit->mode = AGS_AUTOMATION_EDIT_SELECT_ACCELERATION;

      ags_automation_edit_drawing_area_button_press_select_acceleration((GtkWidget *) editor,
									(GtkWidget *) composite_toolbar,
									automation_edit,
									machine,
									n_press,
									x, y);
    }
  }

  return(FALSE);
}

void
ags_automation_edit_drawing_area_button_release_position_cursor(GtkWidget *editor,
								GtkWidget *toolbar,
								AgsAutomationEdit *automation_edit,
								AgsMachine *machine,
								gint n_press,
								gdouble x, gdouble y)
{
  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;
  
  AgsApplicationContext *application_context;

  GtkAllocation allocation;
  
  gdouble gui_scale_factor;
  double zoom_factor;
  gdouble c_range;
  guint g_range;
  gdouble value, step;
  gdouble upper, lower, step_count;

  application_context = ags_application_context_get_instance();
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  
  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  vscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->vscrollbar);
  hscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->hscrollbar);
    
  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* cursor position */
  automation_edit->cursor_position_x = (guint) (zoom_factor * (x + gtk_adjustment_get_value(hscrollbar_adjustment)));

  lower = automation_edit->lower;
  upper = automation_edit->upper;
    
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) y;
    automation_edit->cursor_position_y = lower * pow(upper / lower, step / (step_count - 1)) + lower;
  }else{
    if((AGS_AUTOMATION_EDIT_INTEGER & (automation_edit->flags)) != 0 ||
       (AGS_AUTOMATION_EDIT_TOGGLED & (automation_edit->flags)) != 0){
      automation_edit->cursor_position_y = round((((allocation.height - y) / g_range) * c_range) + lower);
    }else{
      automation_edit->cursor_position_y = (((allocation.height - y) / g_range) * c_range) + lower;
    }
  }
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) automation_edit);
}

void
ags_automation_edit_drawing_area_button_release_add_acceleration(GtkWidget *editor,
								 GtkWidget *toolbar,
								 AgsAutomationEdit *automation_edit,
								 AgsMachine *machine,
								 gint n_press,
								 gdouble x, gdouble y)
{
  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;

  AgsAcceleration *acceleration;

  AgsApplicationContext *application_context;

  GtkAllocation allocation;
  
  gdouble gui_scale_factor;
  double zoom_factor;
  gdouble c_range;
  guint g_range;
  gdouble value, step;
  gdouble upper, lower, step_count;
  guint new_x;
    
  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  
  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  acceleration = automation_edit->current_acceleration;
    
  if(acceleration == NULL){
    return;
  }

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  vscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->vscrollbar);
  hscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->hscrollbar);

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* acceleration */
  acceleration->x = (guint) (zoom_factor * (x + gtk_adjustment_get_value(hscrollbar_adjustment)));
    
  lower = automation_edit->lower;
  upper = automation_edit->upper;

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) y;

    acceleration->y = lower * pow(upper / lower, step / (step_count - 1)) + lower;
  }else{
    if((AGS_AUTOMATION_EDIT_INTEGER & (automation_edit->flags)) != 0 ||
       (AGS_AUTOMATION_EDIT_TOGGLED & (automation_edit->flags)) != 0){
      acceleration->y = round((((allocation.height - y) / g_range) * c_range) + lower);
    }else{
      acceleration->y = (((allocation.height - y) / g_range) * c_range) + lower;
    }
  }
    
#ifdef AGS_DEBUG
  g_message("%lu %f", acceleration->x, acceleration->y);
#endif

  /* add acceleration */
  ags_composite_editor_add_acceleration(editor,
					acceleration);
  
  automation_edit->current_acceleration = NULL;
  g_object_unref(acceleration);

  gtk_widget_queue_draw((GtkWidget *) automation_edit->drawing_area);
}
  
void
ags_automation_edit_drawing_area_button_release_delete_acceleration(GtkWidget *editor,
								    GtkWidget *toolbar,
								    AgsAutomationEdit *automation_edit,
								    AgsMachine *machine,
								    gint n_press,
								    gdouble x, gdouble y)
{
  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;

  AgsApplicationContext *application_context;
   
  GtkAllocation allocation;
  
  gdouble gui_scale_factor;
  double zoom_factor;
  gdouble c_range;
  guint g_range;
  gdouble value, step;
  gdouble upper, lower, step_count;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
    
  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  vscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->vscrollbar);
  hscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->hscrollbar);
    
  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;
  
  /* acceleration */
  x = (guint) zoom_factor * ((x + gtk_adjustment_get_value(hscrollbar_adjustment)));

  lower = automation_edit->lower;
  upper = automation_edit->upper;
    
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) y;
    y = lower * pow(upper / lower, step / (step_count - 1)) + lower;
  }else{
    if((AGS_AUTOMATION_EDIT_INTEGER & (automation_edit->flags)) != 0 ||
       (AGS_AUTOMATION_EDIT_TOGGLED & (automation_edit->flags)) != 0){
      y = round((((allocation.height - y) / g_range) * c_range) + lower);
    }else{
      y = (((allocation.height - y) / g_range) * c_range) + lower;
    }
  }
    
  /* delete acceleration */
  ags_composite_editor_delete_acceleration(editor,
					   x, y);

  gtk_widget_queue_draw((GtkWidget *) automation_edit->drawing_area);
}
  
void
ags_automation_edit_drawing_area_button_release_select_acceleration(GtkWidget *editor,
								    GtkWidget *toolbar,
								    AgsAutomationEdit *automation_edit,
								    AgsMachine *machine,
								    gint n_press,
								    gdouble x, gdouble y)
{
  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;

  AgsApplicationContext *application_context;
   
  GtkAllocation allocation;
  
  gdouble gui_scale_factor;
  double zoom_factor;
  gdouble c_range;
  guint g_range;
  gdouble value, step;
  gdouble upper, lower, step_count;
  guint x0, x1;
  gdouble y0, y1;
    
  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  
  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  vscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->vscrollbar);
  hscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->hscrollbar);

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* region */
  x0 = (guint) zoom_factor * automation_edit->selection_x0;

  lower = automation_edit->lower;
  upper = automation_edit->upper;

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) automation_edit->selection_y0;
      
    y0 = (lower * pow(upper / lower, step / (step_count - 1))) + lower;
  }else{
    if((AGS_AUTOMATION_EDIT_INTEGER & (automation_edit->flags)) != 0 ||
       (AGS_AUTOMATION_EDIT_TOGGLED & (automation_edit->flags)) != 0){
      y0 = round((((gdouble) (allocation.height - automation_edit->selection_y0) / g_range) * c_range) + lower);
    }else{
      y0 = (((gdouble) (allocation.height - automation_edit->selection_y0) / g_range) * c_range) + lower;
    }
  }
  
  x1 = (guint) zoom_factor * (x + gtk_adjustment_get_value(hscrollbar_adjustment));
    
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;

    step = (gdouble) allocation.height - (gdouble) y;
      
    y1 = (lower * pow(upper / lower, step / (step_count - 1))) + lower;
  }else{
    if((AGS_AUTOMATION_EDIT_INTEGER & (automation_edit->flags)) != 0 ||
       (AGS_AUTOMATION_EDIT_TOGGLED & (automation_edit->flags)) != 0){
      y1 = round(((((allocation.height - y) + gtk_adjustment_get_value(vscrollbar_adjustment) / g_range)) * c_range) + lower);
    }else{
      y1 = ((((allocation.height - y) + gtk_adjustment_get_value(vscrollbar_adjustment) / g_range)) * c_range) + lower;
    }
  }
    
  /* select region */
  ags_composite_editor_select_region((AgsCompositeEditor *) editor,
				     x0, y0,
				     x1, y1);

  gtk_widget_queue_draw((GtkWidget *) automation_edit->drawing_area);
}

gboolean
ags_automation_edit_gesture_click_released_callback(GtkGestureClick *event_controller,
						    gint n_press,
						    gdouble x,
						    gdouble y,
						    AgsAutomationEdit *automation_edit)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;
  AgsMachine *machine;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  machine = NULL;  
    
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  composite_toolbar = composite_editor->toolbar;

  machine = composite_editor->selected_machine;
  
  if(machine != NULL){
    automation_edit->button_mask &= (~AGS_AUTOMATION_EDIT_BUTTON_1);
    
    if(automation_edit->mode == AGS_AUTOMATION_EDIT_POSITION_CURSOR){
      ags_automation_edit_drawing_area_button_release_position_cursor((GtkWidget *) composite_editor,
								      (GtkWidget *) composite_toolbar,
								      automation_edit,
								      machine,
								      n_press,
								      x, y);

      automation_edit->mode = AGS_AUTOMATION_EDIT_NO_EDIT_MODE;
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_ADD_ACCELERATION){
      ags_automation_edit_drawing_area_button_release_add_acceleration((GtkWidget *) composite_editor,
								       (GtkWidget *) composite_toolbar,
								       automation_edit,
								       machine,
								       n_press,
								       x, y);

      automation_edit->mode = AGS_AUTOMATION_EDIT_NO_EDIT_MODE;
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_DELETE_ACCELERATION){
      ags_automation_edit_drawing_area_button_release_delete_acceleration((GtkWidget *) composite_editor,
									  (GtkWidget *) composite_toolbar,
									  automation_edit,
									  machine,
									  n_press,
									  x, y);

      automation_edit->mode = AGS_AUTOMATION_EDIT_NO_EDIT_MODE;
    }else if(automation_edit->mode == AGS_AUTOMATION_EDIT_SELECT_ACCELERATION){
      ags_automation_edit_drawing_area_button_release_select_acceleration((GtkWidget *) composite_editor,
									  (GtkWidget *) composite_toolbar,
									  automation_edit,
									  machine,
									  n_press,
									  x, y);

      automation_edit->mode = AGS_AUTOMATION_EDIT_NO_EDIT_MODE;
    }
  }

  return(FALSE);
}

void
ags_automation_edit_realize(GtkWidget *widget)
{
  GdkFrameClock *frame_clock;
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_automation_edit_parent_class)->realize(widget);

#if 0
  frame_clock = gtk_widget_get_frame_clock(widget);
  
  g_signal_connect(frame_clock, "update", 
		   G_CALLBACK(ags_automation_edit_frame_clock_update_callback), widget);

  gdk_frame_clock_begin_updating(frame_clock);
#endif
}

void
ags_automation_edit_unrealize(GtkWidget *widget)
{
  GdkFrameClock *frame_clock;
  
#if 0
  frame_clock = gtk_widget_get_frame_clock(widget);
  
  g_object_disconnect(frame_clock,
		      "any_signal::update", 
		      G_CALLBACK(ags_automation_edit_frame_clock_update_callback),
		      widget,
		      NULL);

  gdk_frame_clock_end_updating(frame_clock);
#endif
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_automation_edit_parent_class)->unrealize(widget);
}

void
ags_automation_edit_measure(GtkWidget *widget,
			    GtkOrientation orientation,
			    int for_size,
			    int *minimum,
			    int *natural,
			    int *minimum_baseline,
			    int *natural_baseline)
{
  AgsAutomationEdit *automation_edit;

  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();

  automation_edit = (AgsAutomationEdit *) widget;
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  if(orientation == GTK_ORIENTATION_VERTICAL){    
    minimum[0] =
      natural[0] = (gint) (gui_scale_factor * AGS_SCALE_DEFAULT_HEIGHT_REQUEST);
  }else{
    minimum =
      natural = NULL;
  }
}

void
ags_automation_edit_size_allocate(GtkWidget *widget,
				  int width,
				  int height,
				  int baseline)
{
  AgsAutomationEdit *automation_edit;

  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();

  automation_edit = (AgsAutomationEdit *) widget;
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  width = -1;
  height = (gint) (gui_scale_factor * AGS_SCALE_DEFAULT_HEIGHT_REQUEST);
  
  GTK_WIDGET_CLASS(ags_automation_edit_parent_class)->size_allocate(widget,
								    width,
								    height,
								    baseline);
}

void
ags_automation_edit_show(GtkWidget *widget)
{
  AgsAutomationEdit *automation_edit;

  automation_edit = AGS_AUTOMATION_EDIT(widget);

  /* call parent */
  GTK_WIDGET_CLASS(ags_automation_edit_parent_class)->show(widget);

  gtk_widget_show((GtkWidget *) automation_edit->drawing_area);
  
  if((AGS_AUTOMATION_EDIT_SHOW_RULER & (automation_edit->flags)) != 0){
    gtk_widget_show((GtkWidget *) automation_edit->ruler);
  }

  if((AGS_AUTOMATION_EDIT_SHOW_VSCROLLBAR & (automation_edit->flags)) != 0){
    gtk_widget_show((GtkWidget *) automation_edit->vscrollbar);
  }

  if((AGS_AUTOMATION_EDIT_SHOW_HSCROLLBAR & (automation_edit->flags)) != 0){
    gtk_widget_show((GtkWidget *) automation_edit->hscrollbar);
  }
}

void
ags_automation_edit_frame_clock_update_callback(GdkFrameClock *frame_clock,
						AgsAutomationEdit *automation_edit)
{
  gtk_widget_queue_draw((GtkWidget *) automation_edit);
}

gboolean
ags_automation_edit_auto_scroll_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_automation_edit_auto_scroll,
			 widget) != NULL){
    AgsCompositeEditor *composite_editor;
    AgsAutomationEdit *automation_edit;

    GtkAdjustment *hscrollbar_adjustment;
    
    GObject *output_soundcard;
    
    double x;
    
    automation_edit = AGS_AUTOMATION_EDIT(widget);

    if((AGS_AUTOMATION_EDIT_AUTO_SCROLL & (automation_edit->flags)) == 0){
      return(TRUE);
    }
    
    composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_edit,
								      AGS_TYPE_COMPOSITE_EDITOR);
    
    if(composite_editor->selected_machine == NULL){
      return(TRUE);
    }

    /* reset offset */
    g_object_get(composite_editor->selected_machine->audio,
		 "output-soundcard", &output_soundcard,
		 NULL);
    
    automation_edit->note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));
    automation_edit->note_offset_absolute = ags_soundcard_get_note_offset_absolute(AGS_SOUNDCARD(output_soundcard));

    /* reset scrollbar */
    hscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->hscrollbar);
    x = ((automation_edit->note_offset * automation_edit->control_width) / (AGS_AUTOMATION_DEFAULT_LENGTH * automation_edit->control_width)) * gtk_adjustment_get_upper(hscrollbar_adjustment);
    
    gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(automation_edit->hscrollbar),
			x);

    g_object_unref(output_soundcard);
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

void
ags_automation_edit_reset_vscrollbar(AgsAutomationEdit *automation_edit)
{
  AgsCompositeEditor *composite_editor;

  GtkAdjustment *adjustment;

  AgsApplicationContext *application_context;
  
  GtkAllocation allocation;
  
  double varea_height;
  gdouble upper, old_upper;
  
  if(!AGS_IS_AUTOMATION_EDIT(automation_edit) ||
     (AGS_AUTOMATION_EDIT_BLOCK_RESET_VSCROLLBAR & (automation_edit->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();

  automation_edit->flags |= AGS_AUTOMATION_EDIT_BLOCK_RESET_VSCROLLBAR;

  /* adjustment and allocation */
  adjustment = gtk_scrollbar_get_adjustment(automation_edit->vscrollbar);

  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);
  
  /* upper */
  old_upper = gtk_adjustment_get_upper(adjustment); 

  varea_height = (automation_edit->step_count * automation_edit->control_height);
  upper = varea_height - allocation.height;

  if(upper < 0.0){
    upper = 0.0;
  }
	   
  gtk_adjustment_set_upper(adjustment,
			   upper);

  /* reset value */
  if(old_upper != 0.0){
    gtk_adjustment_set_value(adjustment,
			     gtk_adjustment_get_value(adjustment) / old_upper * upper);
  }

  automation_edit->flags &= (~AGS_AUTOMATION_EDIT_BLOCK_RESET_VSCROLLBAR);  
}

void
ags_automation_edit_reset_hscrollbar(AgsAutomationEdit *automation_edit)
{
  GtkAdjustment *adjustment;
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;
  
  AgsApplicationContext *application_context;

  GtkAllocation allocation;
  
  gdouble gui_scale_factor;
  double zoom_factor, zoom;
  double zoom_correction;
  guint map_width;
  gdouble upper, old_upper;
  
  if(!AGS_IS_AUTOMATION_EDIT(automation_edit) ||
     (AGS_AUTOMATION_EDIT_BLOCK_RESET_HSCROLLBAR & (automation_edit->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();

  automation_edit->flags |= AGS_AUTOMATION_EDIT_BLOCK_RESET_HSCROLLBAR;

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);
  
  /* adjustment and allocation */
  adjustment = gtk_scrollbar_get_adjustment(automation_edit->hscrollbar);

  /* zoom */
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  composite_toolbar = composite_editor->toolbar;

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom) - 2.0);
  
  /* upper */
  old_upper = gtk_adjustment_get_upper(adjustment);
  
  zoom_correction = 1.0 / 16;

//  map_width = ((double) AGS_COMPOSITE_EDITOR_MAX_CONTROLS * zoom * zoom_correction);
  map_width = ((64.0) * (16.0 * 16.0 * 1200.0) * zoom * zoom_correction);
  upper = map_width - allocation.width;

  if(upper < 0.0){    
    upper = 0.0;
  }

  gtk_adjustment_set_upper(adjustment,
			   upper);

  /* ruler */
  automation_edit->ruler->factor = zoom_factor;
  automation_edit->ruler->precision = zoom;
  automation_edit->ruler->scale_precision = 1.0 / zoom;

  gtk_adjustment_set_upper(automation_edit->ruler->adjustment,
			   upper);

  /* reset value */
  if(old_upper != 0.0){
#if 0
    gtk_adjustment_set_value(adjustment,
			     gtk_adjustment_get_value(adjustment) / old_upper * upper);
#endif
  }

  automation_edit->flags &= (~AGS_AUTOMATION_EDIT_BLOCK_RESET_HSCROLLBAR);  
}

gint
ags_automation_edit_compare_x_offset_func(gconstpointer a,
					  gconstpointer b,
					  AgsAutomationEdit *automation_edit,
					  gdouble x_offset,
					  guint line,
					  GType channel_type,
					  gchar *control_name)
{
  AgsTimestamp *timestamp_a, *timestamp_b;
  
  GType line_a_channel_type;
  GType line_b_channel_type;

  gchar *line_a_control_name;
  gchar *line_b_control_name;

  guint64 current_offset;
  guint64 a_offset, b_offset;
  gint line_a, line_b;

  gint retval;
  
  timestamp_a = ags_automation_get_timestamp(a);
  timestamp_b = ags_automation_get_timestamp(b);

  current_offset = (guint64) ((double) AGS_AUTOMATION_DEFAULT_OFFSET * floor(x_offset / AGS_AUTOMATION_DEFAULT_OFFSET));

  a_offset = ags_timestamp_get_ags_offset(timestamp_a);
  b_offset = ags_timestamp_get_ags_offset(timestamp_b);

  g_object_unref(timestamp_a);
  g_object_unref(timestamp_b);
  
  line_a = ags_automation_get_line(a);
  line_b = ags_automation_get_line(b);
	
  line_a_control_name = ags_automation_get_control_name(a);
  line_b_control_name = ags_automation_get_control_name(b);

  line_a_channel_type = ags_automation_get_channel_type(a);
  line_b_channel_type = ags_automation_get_channel_type(b);

  retval = 0;
  
  if(a_offset == current_offset){
    if(b_offset == current_offset){
      if(line_a == line){
	if(line_b == line){
	  if(line_a_channel_type == channel_type){
	    if(line_b_channel_type == channel_type){
	      if(g_strcmp0(line_b_control_name, control_name) > 0){
		retval = g_strcmp0(line_a_control_name, control_name);
	      }else{
		retval = 1;
	      }
	    }else{
	      retval = -1;
	    }
	  }else if(line_b_channel_type == channel_type){
	    retval = 1;
	  }else{
	    if(line_b_channel_type > channel_type){
	      retval = (line_a_channel_type < channel_type) ? -1: 1;
	    }else{
	      retval = 1;
	    }
	  }
	}else{
	  retval = -1;
	}
      }else{
	if(line_b > line){
	  retval = (line_a < line) ? -1: 1;
	}else{
	  retval = 1;
	}
      }
    }else{
      retval = -1;
    }
  }else if(b_offset == current_offset){
    retval = 1;
  }else{
    if(b_offset > current_offset){
      retval = (a_offset < current_offset) ? -1: 1;
    }else{
      retval = 1;
    }
  }

  g_free(line_a_control_name);
  g_free(line_b_control_name);
  
  return(retval);
}


GList*
ags_automation_edit_find_first_drawn_func(AgsAutomationEdit *automation_edit,
					  GList *automation,
					  guint line)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;
  AgsCompositeEdit *composite_edit;

  GtkAdjustment *adjustment;

  AgsApplicationContext *application_context;

  GType channel_type;
  
  GList *automation_last;
  GList *a_list, *b_list, *c_list;
  GList *retval;

  gchar *control_name;
  
  gint automation_length;
  gint bisect_steps;
  gdouble zoom_factor;
  gdouble x_offset;
  gint nth_bisect;
  
  application_context = ags_application_context_get_instance();

  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  composite_edit = composite_editor->automation_edit;

  adjustment = gtk_scrollbar_get_adjustment(composite_edit->hscrollbar);

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));
  
  x_offset = (guint64) (AGS_AUTOMATION_DEFAULT_OFFSET * floor(((zoom_factor * gtk_adjustment_get_value(adjustment))) / (double) AGS_AUTOMATION_DEFAULT_OFFSET));

  retval = automation;
  
  automation_last = g_list_last(automation);
  automation_length = g_list_length(automation);

  if(automation == automation_last){
    return(automation);
  }

  channel_type = automation_edit->channel_type;

  control_name = automation_edit->control_name;
  
  a_list = automation;
  b_list = g_list_nth(automation,
		      (guint) floor((double) automation_length / 2.0));
  c_list = automation_last;
  
  bisect_steps = (guint) floor((automation_length) / 2.0);
  nth_bisect = 0;
  
  while(bisect_steps > 0){
    gint cmp_val_0, cmp_val_1;

    cmp_val_0 = 0;
    cmp_val_1 = 0;

    if(a_list == b_list){
      b_list = b_list->next;
    }
    
    if(a_list != NULL &&
       b_list != NULL){
      cmp_val_0 = ags_automation_edit_compare_x_offset_func(a_list->data,
							    b_list->data,
							    automation_edit,
							    x_offset,
							    line,
							    channel_type,
							    control_name);
    }else{
      break;
    }

    retval = a_list;

    if(cmp_val_0 <= 0){
      retval = a_list;
    }else{
      retval = b_list;
    }

    if(b_list == c_list){
      c_list = c_list->next;
    }
    
    if(b_list != NULL &&
       c_list != NULL){
      cmp_val_1 = ags_automation_edit_compare_x_offset_func(b_list->data,
							    c_list->data,
							    automation_edit,
							    x_offset,
							    line,
							    channel_type,
							    control_name);
    }else{
      break;
    }

    if(cmp_val_1 <= 0){
      retval = b_list;
    }

    nth_bisect++;

    if(retval == a_list){
      automation_length = g_list_position(a_list, b_list) + 1;
      bisect_steps = (gint) floor((double) (automation_length) / 2.0);
      
      c_list = b_list;

      b_list = g_list_nth(a_list,
			  bisect_steps);

      if(cmp_val_0 == 0){
	break;
      }
    }else if(retval == b_list){
      automation_length = g_list_position(b_list, c_list) + 1;
      bisect_steps = (gint) floor((double) (automation_length) / 2.0);

      a_list = b_list;

      b_list = g_list_nth(b_list,
			  bisect_steps);

      if(cmp_val_1 == 0){
	break;
      }
    }
  }
  
  return(retval);
}

GList*
ags_automation_edit_find_last_drawn_func(AgsAutomationEdit *automation_edit,
					 GList *automation,
					 guint line)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;
  AgsCompositeEdit *composite_edit;

  GtkAdjustment *adjustment;

  AgsApplicationContext *application_context;

  GType channel_type;
  
  GList *automation_last;
  GList *a_list, *b_list, *c_list;
  GList *retval;

  gchar *control_name;

  guint width;
  gint automation_length;
  gint bisect_steps;
  gdouble zoom_factor;
  gdouble x_offset;
  gint nth_bisect;
  
  application_context = ags_application_context_get_instance();

  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  composite_edit = composite_editor->automation_edit;

  adjustment = gtk_scrollbar_get_adjustment(composite_edit->hscrollbar);

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));
  
  width = gtk_widget_get_width(automation_edit->drawing_area);

  x_offset = (guint64) (AGS_AUTOMATION_DEFAULT_OFFSET * floor((zoom_factor * (gtk_adjustment_get_value(adjustment) + (double) width)) / (double) AGS_AUTOMATION_DEFAULT_OFFSET));

  retval = automation;
  
  automation_last = g_list_last(automation);
  automation_length = g_list_length(automation);

  if(automation == automation_last){
    return(automation);
  }

  channel_type = automation_edit->channel_type;

  control_name = automation_edit->control_name;
  
  a_list = automation;
  b_list = g_list_nth(automation,
		      (guint) floor((double) automation_length / 2.0));
  c_list = automation_last;
  
  bisect_steps = (guint) floor((automation_length) / 2.0);
  nth_bisect = 0;
  
  while(bisect_steps > 0){
    gint cmp_val_0, cmp_val_1;

    cmp_val_0 = 0;
    cmp_val_1 = 0;
    
    if(a_list != NULL &&
       b_list != NULL){
      cmp_val_0 = ags_automation_edit_compare_x_offset_func(a_list->data,
							    b_list->data,
							    automation_edit,
							    x_offset,
							    line,
							    channel_type,
							    control_name);
    }else{
      break;
    }

    retval = a_list;
    
    if(cmp_val_0 <= 0){
      retval = a_list;
    }else{
      retval = b_list;
    }

    if(b_list == c_list){
      c_list = c_list->next;
    }
    
    if(b_list != NULL &&
       c_list != NULL){
      cmp_val_1 = ags_automation_edit_compare_x_offset_func(b_list->data,
							    c_list->data,
							    automation_edit,
							    x_offset,
							    line,
							    channel_type,
							    control_name);
    }else{
      break;
    }

    if(cmp_val_1 <= 0){
      retval = b_list;
    }

    nth_bisect++;

    if(retval == a_list){
      automation_length = g_list_position(a_list, b_list) + 1;
      bisect_steps = (gint) floor((double) (automation_length) / 2.0);
      
      c_list = b_list;

      b_list = g_list_nth(a_list,
			  bisect_steps);

      if(cmp_val_0 == 0){
	break;
      }
    }else if(retval == b_list){
      automation_length = g_list_position(b_list, c_list) + 1;
      bisect_steps = (gint) floor((double) (automation_length) / 2.0);

      a_list = b_list;

      b_list = g_list_nth(b_list,
			  bisect_steps);

      if(cmp_val_1 == 0){
	break;
      }
    }
  }
  
  return(retval);
}

void
ags_automation_edit_draw_segment(AgsAutomationEdit *automation_edit, cairo_t *cr)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *toolbar;
  
  GtkStyleContext *style_context;
  GtkSettings *settings;

  GtkAdjustment *hscrollbar_adjustment;
  
  AgsApplicationContext *application_context;

  GtkAllocation allocation;

  GdkRGBA fg_color;
  GdkRGBA bg_color;
  GdkRGBA shadow_color;
  GdkRGBA text_color;

  gdouble gui_scale_factor;
  gdouble x_offset;
  gdouble translated_ground;
  double tact;
  gdouble y;
  gdouble map_height;
  gdouble width, height;
  guint control_width;
  guint i, j;
  guint j_set;
  gboolean dark_theme;
  gboolean fg_success;
  gboolean bg_success;
  gboolean text_success;
  gboolean shadow_success;

  GValue value = G_VALUE_INIT;

  const static double quarter_dashes = {
    0.25,
  };

  const static double segment_dashes = {
    0.5,
  };
  
  if(!AGS_IS_AUTOMATION_EDIT(automation_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);

  toolbar = composite_editor->toolbar;

  tact = exp2((double) gtk_combo_box_get_active(toolbar->zoom) - 2.0);
  
  /* dimension and offset */
  width = (gdouble) allocation.width;
  height = (gdouble) allocation.height;

  hscrollbar_adjustment = gtk_scrollbar_get_adjustment(automation_edit->hscrollbar);
  
  x_offset = gtk_adjustment_get_value(hscrollbar_adjustment);

  y = 0.0;

  style_context = gtk_widget_get_style_context((GtkWidget *) automation_edit);

  settings = gtk_settings_get_default();
  
  dark_theme = TRUE;
  
  g_object_get(settings,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

  /* colors */
  fg_success = gtk_style_context_lookup_color(style_context,
					      "theme_fg_color",
					      &fg_color);
    
  bg_success = gtk_style_context_lookup_color(style_context,
					      "theme_bg_color",
					      &bg_color);
    
  shadow_success = gtk_style_context_lookup_color(style_context,
						  "theme_shadow_color",
						  &shadow_color);
    
  text_success = gtk_style_context_lookup_color(style_context,
						"theme_text_color",
						&text_color);

  if(!fg_success ||
     !bg_success ||
     !shadow_success ||
     !text_success){
    gdk_rgba_parse(&fg_color,
		   "#101010");

    gdk_rgba_parse(&bg_color,
		   "#cbd5d9");

    gdk_rgba_parse(&shadow_color,
		   "#ffffff40");

    gdk_rgba_parse(&text_color,
		   "#1a1a1a");
  }

  /* push group */
  cairo_push_group(cr);
  
  /* background */
  cairo_set_source_rgba(cr,
			bg_color.red,
			bg_color.green,
			bg_color.blue,
			bg_color.alpha);

  cairo_rectangle(cr, 0.0, y, width, height);
  cairo_fill(cr);

  /* shadow */
  cairo_set_source_rgba(cr,
			shadow_color.red,
			shadow_color.green,
			shadow_color.blue,
			shadow_color.alpha);

  cairo_set_line_width(cr, 1.0);
  cairo_rectangle(cr, 0.0, y, width, height);
  cairo_stroke(cr);

  cairo_set_line_width(cr, 1.0);

  y = (gdouble) 0.0;
  
  map_height = (gdouble) height;

  control_width = (gint) (gui_scale_factor * (gdouble) AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH) * (tact / (gui_scale_factor * tact));
  i = control_width - (guint) x_offset % control_width;
  
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.blue,
			fg_color.green,
			fg_color.alpha);
  
  if(i < width &&
     tact > 1.0 ){
    j_set = ((guint) x_offset / control_width + 1) % ((guint) tact);

    cairo_set_dash(cr,
		   &segment_dashes,
		   1,
		   0.0);

    if(j_set != 0){
      j = j_set;
      goto ags_automation_edit_draw_segment0;
    }
  }

  for(; i < width; ){
    cairo_set_dash(cr,
		   NULL,
		   0,
		   0.0);
    
    cairo_move_to(cr, (double) i, y);
    cairo_line_to(cr, (double) i, y + height);
    cairo_stroke(cr);
    
    i += control_width;
    
    cairo_set_dash(cr,
		   &segment_dashes,
		   1,
		   0.0);
    
    for(j = 1; i < width && j < tact; j++){
    ags_automation_edit_draw_segment0:
      cairo_move_to(cr, (double) i, y);
      cairo_line_to(cr, (double) i, y + height);
      cairo_stroke(cr);
      
      i += control_width;
    }
  }

  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.green,
			fg_color.blue,
			fg_color.alpha);

  /* middle */
  if(map_height * 0.5 < height){
    cairo_move_to(cr,
		  0.0, y + map_height * 0.5);
    cairo_line_to(cr,
		  width, y + map_height * 0.5);
    cairo_stroke(cr);
  }
  
  /* set dash */
  cairo_set_dash(cr,
		 &quarter_dashes,
		 1,
		 0.0);

  /* lower quarter */
  if(map_height * 0.25 < height){
    cairo_move_to(cr,
		  0.0, y + map_height * 0.25);
    cairo_line_to(cr,
		  width, y + map_height * 0.25);
    cairo_stroke(cr);
  }
  
  /* upper quarter */
  if(map_height * 0.75 < height){
    cairo_move_to(cr,
		  0.0, y + map_height * 0.75);
    cairo_line_to(cr,
		  width, y + map_height * 0.75);
    cairo_stroke(cr);
  }
  
  /* complete */
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
      
//  cairo_surface_mark_dirty(cairo_get_target(cr));
}

void
ags_automation_edit_draw_position(AgsAutomationEdit *automation_edit, cairo_t *cr)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *toolbar;

  GtkStyleContext *style_context;
  GtkSettings *settings;

  AgsApplicationContext *application_context;

  GdkRGBA fg_color;

  gdouble gui_scale_factor;
  gdouble tact;
  guint control_width;
  double position;
  double x, y;
  double width, height;
  gboolean height_fits;
  gboolean dark_theme;
  gboolean fg_success;

  GValue value = G_VALUE_INIT;

  if(!AGS_IS_AUTOMATION_EDIT(automation_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);

  toolbar = composite_editor->toolbar;

  tact = exp2((double) gtk_combo_box_get_active(toolbar->zoom) - 2.0);
  
  /* style context */  
  style_context = gtk_widget_get_style_context((GtkWidget *) automation_edit);

  settings = gtk_settings_get_default();
  
  dark_theme = TRUE;
  
  g_object_get(settings,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

  /* colors */
  fg_success = gtk_style_context_lookup_color(style_context,
					      "theme_fg_color",
					      &fg_color);

  if(!fg_success){
    gdk_rgba_parse(&fg_color,
		   "#101010");
  }
  
  /* get offset and dimensions */
  control_width = (gint) (gui_scale_factor * (gdouble) AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH) * (tact / (gui_scale_factor * tact));

  position = ((double) automation_edit->note_offset) * ((double) control_width);
  
  y = 0.0;
  x = (position) - (gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->hscrollbar)));

  width = (double) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_FADER_WIDTH));
  height = automation_edit->step_count * automation_edit->control_height;

  /* push group */
  cairo_push_group(cr);
  
  /* draw fader */
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.blue,
			fg_color.green,
			fg_color.alpha);

  cairo_rectangle(cr,
		  (double) x, (double) y,
		  (double) width, (double) height);
  cairo_fill(cr);

  /* complete */
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
      
//cairo_surface_mark_dirty(cairo_get_target(cr));
}

void
ags_automation_edit_draw_cursor(AgsAutomationEdit *automation_edit, cairo_t *cr)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;

  GtkStyleContext *style_context;
  GtkSettings *settings;

  AgsApplicationContext *application_context;

  GtkAllocation allocation;

  GdkRGBA fg_color;

  gdouble gui_scale_factor;
  double zoom, zoom_factor;
  gdouble val, step;
  gdouble upper, lower, step_count;
  gdouble c_range;
  double x, y;
  double width, height;
  gboolean dark_theme;
  gboolean fg_success;

  GValue value = G_VALUE_INIT;

  if(!AGS_IS_AUTOMATION_EDIT(automation_edit)){
    return;
  }
  
  application_context = ags_application_context_get_instance();
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  /* style context */
  style_context = gtk_widget_get_style_context((GtkWidget *) automation_edit);

  settings = gtk_settings_get_default();

  dark_theme = TRUE;
  
  g_object_get(settings,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

  /* colors */
  fg_success = gtk_style_context_lookup_color(style_context,
					      "theme_fg_color",
					      &fg_color);

  if(!fg_success){
    gdk_rgba_parse(&fg_color,
		   "#101010");
  }

  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  /* zoom */
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  composite_toolbar = composite_editor->toolbar;

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom) - 2.0);
  
  /* get offset */
  x = ((double) automation_edit->cursor_position_x) - (gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->hscrollbar)) * zoom_factor);
  
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    lower = automation_edit->lower;
    upper = automation_edit->upper;

    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;
    
    val = automation_edit->cursor_position_y;
    
    y = (step_count - 1) * log(val / lower) / log(upper / lower);
  }else{
    y = allocation.height - ((((double) automation_edit->cursor_position_y / c_range) * allocation.height) - gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->vscrollbar)));
  }
  
  width = (double) AGS_AUTOMATION_EDIT_CURSOR_WIDTH;
  height = (double) AGS_AUTOMATION_EDIT_CURSOR_HEIGHT;

  /* apply zoom */
  x /= zoom_factor;

  /* clip */
  if(x < 0.0){
    width += x;

    x = 0.0;
  }else if(x > allocation.width){
    return;
  }

  if(x + width > allocation.width){
    width = ((double) allocation.width) - x;
  }
  
  if(y < 0.0){
    height += y;

    y = 0.0;
  }else if(y > allocation.height){
    return;
  }

  if(y + height > allocation.height){
    height = ((double) allocation.height) - y;
  }

  /* push group */
  cairo_push_group(cr);

  /* draw cursor */
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.blue,
			fg_color.green,
			fg_color.alpha);

  cairo_move_to(cr,
		x, y);
  cairo_line_to(cr,
		x + width, y);
  cairo_stroke(cr);

  cairo_move_to(cr,
		x, y);
  cairo_line_to(cr,
		x, y + height);
  cairo_stroke(cr);

  /* complete */
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
      
  //cairo_surface_mark_dirty(cairo_get_target(cr));
}

void
ags_automation_edit_draw_selection(AgsAutomationEdit *automation_edit, cairo_t *cr)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;

  GtkStyleContext *style_context;
  GtkSettings *settings;

  AgsApplicationContext *application_context;

  GtkAllocation allocation;

  GdkRGBA fg_color;
  
  double zoom, zoom_factor;
  double x, y;
  double width, height;
  gboolean dark_theme;
  gboolean fg_success;

  GValue value = G_VALUE_INIT;

  if(!AGS_IS_AUTOMATION_EDIT(automation_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* style context */
  style_context = gtk_widget_get_style_context((GtkWidget *) automation_edit);

  settings = gtk_settings_get_default();
  
  dark_theme = TRUE;
  
  g_object_get(settings,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

  /* colors */
  fg_success = gtk_style_context_lookup_color(style_context,
					      "theme_fg_color",
					      &fg_color);

  if(!fg_success){
    gdk_rgba_parse(&fg_color,
		   "#101010");
  }
  
  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);

  /* zoom */
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  composite_toolbar = composite_editor->toolbar;

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom) - 2.0);
  
  /* get offset and dimensions */
  if(automation_edit->selection_x0 < automation_edit->selection_x1){
    x = (((double) automation_edit->selection_x0) - gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->hscrollbar))) / zoom_factor;
    width = ((double) automation_edit->selection_x1 - (double) automation_edit->selection_x0) / zoom_factor;
  }else{
    x = (((double) automation_edit->selection_x1) - gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->hscrollbar))) / zoom_factor;
    width = ((double) automation_edit->selection_x0 - (double) automation_edit->selection_x1) / zoom_factor;
  }

  if(automation_edit->selection_y0 < automation_edit->selection_y1){
    y = ((double) automation_edit->selection_y0) - gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->vscrollbar));
    height = ((double) automation_edit->selection_y1 - (double) automation_edit->selection_y0);
  }else{
    y = ((double) automation_edit->selection_y1) - gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->vscrollbar));
    height = ((double) automation_edit->selection_y0 - (double) automation_edit->selection_y1);
  }
  
  /* clip */
  if(x < 0.0){
    width += x;

    x = 0.0;
  }else if(x > allocation.width){
    return;
  }

  if(x + width > allocation.width){
    width = ((double) allocation.width) - x;
  }
  
  if(y < 0.0){
    height += y;

    y = 0.0;
  }else if(y > allocation.height){
    return;
  }

  if(y + height > allocation.height){
    height = ((double) allocation.height) - y;
  }
    
  /* push group */
  cairo_push_group(cr);

  /* draw selection */
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.blue,
			fg_color.green,
			1.0 / 3.0);

  cairo_rectangle(cr,
		  x, y,
		  width, height);
  cairo_fill(cr);

  /* complete */
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
      
  //cairo_surface_mark_dirty(cairo_get_target(cr));
}

void
ags_automation_edit_draw_acceleration(AgsAutomationEdit *automation_edit,
				      AgsAcceleration *acceleration_a, AgsAcceleration *acceleration_b,
				      cairo_t *cr,
				      gdouble opacity)
{
  AgsMachine *selected_machine;
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;

  GtkStyleContext *style_context;
  GtkSettings *settings;

  AgsApplicationContext *application_context;

  GdkRGBA fg_color;
  GdkRGBA shadow_color;

  gdouble gui_scale_factor;
  double zoom, zoom_factor;
  double viewport_x, viewport_y;
  gdouble val, step;
  gdouble upper, lower, step_count;
  gdouble c_range;
  gint allocation_width, allocation_height;
  gint x, y;
  gint a_x, b_x;
  gdouble a_y, b_y;
  double width, height;
  gboolean dark_theme;
  gboolean fg_success;
  gboolean shadow_success;

  GValue value = {0};
  
  if(!AGS_IS_AUTOMATION_EDIT(automation_edit) ||
     !AGS_IS_ACCELERATION(acceleration_a) ||
     cr == NULL){
    return;
  }

  application_context = ags_application_context_get_instance();
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);

  if(composite_editor->selected_machine == NULL){
    return;
  }

  selected_machine = composite_editor->selected_machine;
    
  composite_toolbar = composite_editor->toolbar;

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom) - 2.0);
   
  /* style context */
  style_context = gtk_widget_get_style_context((GtkWidget *) automation_edit);

  settings = gtk_settings_get_default();
  
  dark_theme = TRUE;
  
  g_object_get(settings,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

  /* colors */
  fg_success = gtk_style_context_lookup_color(style_context,
					      "theme_fg_color",
					      &fg_color);
    
  shadow_success = gtk_style_context_lookup_color(style_context,
						  "theme_shadow_color",
						  &shadow_color);

  if(!fg_success ||
     !shadow_success){
    gdk_rgba_parse(&fg_color,
		   "#101010");

    gdk_rgba_parse(&shadow_color,
		   "#ffffff40");
  }

  allocation_width = gtk_widget_get_width(automation_edit->drawing_area);
  allocation_height = gtk_widget_get_height(automation_edit->drawing_area);

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = (gdouble) ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT));
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }
  
  /* get offset and dimensions */
  if((double) AGS_AUTOMATION_DEFAULT_LENGTH > zoom_factor * (double) allocation_width){
    viewport_x = zoom_factor * gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->hscrollbar));
  }else{
    viewport_x = 0.0;
  }
  
  viewport_y = gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->vscrollbar));

  g_object_get(acceleration_a,
	       "x", &a_x,
	       "y", &a_y,
	       NULL);
 
  x = (guint) ((double) a_x - viewport_x);

  lower = automation_edit->lower;
  upper = automation_edit->upper;

  a_y -= lower;
  
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    step_count = ((guint) (gui_scale_factor * AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT)) + 1.0;
    
    val = a_y;
    
    y = (step_count - 1.0) * log(val / lower) / log(upper / lower);
  }else{
    y = allocation_height - ((double) a_y / c_range) * allocation_height - viewport_y;
  }
  
  if(acceleration_b != NULL){
    g_object_get(acceleration_b,
		 "x", &b_x,
		 NULL);

    if((double) b_x < (double) viewport_x + (zoom_factor * (double) allocation_width)){
      width = ((double) b_x - a_x);

      if(a_x < viewport_x){
	x = 0;
	width = b_x - viewport_x;
      }

      if((double) x + (double) width > viewport_x + (zoom_factor * (double) allocation_width)){
	width = zoom_factor * (double) allocation_width;
	width -= x;
      }
    }else{
      if(a_x < viewport_x){
	x = 0;

	width = (gint) (zoom_factor * (double) allocation_width);
      }else{
	width = (gint) viewport_x + (zoom_factor * (double) allocation_width) - a_x;
      }
    }
  }else{
    if(viewport_x > a_x){
      x = 0;
      width = zoom_factor * (double) allocation_width;
    }else{
      width = (gint) viewport_x + (zoom_factor * (double) allocation_width) - a_x;
    }
  }

  height = allocation_height - y;

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    gdouble tmp;

    tmp = height;

    height = y;
    y = tmp;
  }
  
  /* apply zoom */
  x /= zoom_factor;
  
  width /= zoom_factor;

  /* clip */  
  if(x < 0.0){
    if(x + width < 0.0){
      return;
    }

    x = 0;
  }else if(x > allocation_width){
    return;
  }

  if(x + width > allocation_width){
    width = (double) allocation_width - x;
  }
  
  if(y < 0.0){
    if(y + height < 0.0){
      return;
    }else{
      height += y;
      y = 0.0;
    }
  }else if(y > allocation_height){
    return;
  }

  if(y + height > allocation_height){
    height = ((double) allocation_height) - y;
  }

  /* draw acceleration - dot */
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.blue,
			fg_color.green,
			opacity * fg_color.alpha);
  
  cairo_arc(cr,
	    (double) x, (double) y,
	    automation_edit->point_radius,
	    0.0,
	    2.0 * M_PI);
  
  cairo_stroke(cr);
  
  /* draw acceleration - area */
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.blue,
			fg_color.green,
			opacity * fg_color.alpha);
  cairo_rectangle(cr,
		  (double) x, (double) y,
		  width, height);
  cairo_fill(cr);

  /* check acceleration selected */
  if(ags_acceleration_test_flags(acceleration_a, AGS_ACCELERATION_IS_SELECTED)){
    double selected_x, selected_y;
    double selected_width, selected_height;

    selected_x = x - automation_edit->selected_acceleration_border;
    selected_y = y - automation_edit->selected_acceleration_border;

    selected_width = width + (2.0 * (double) automation_edit->selected_acceleration_border);
    selected_height = height + (2.0 * (double) automation_edit->selected_acceleration_border);

    /* clip */
    if(selected_x < 0.0){
      selected_x = 0.0;
    }
    
    if(selected_x + selected_width > allocation_width){
      selected_width = ((double) allocation_width) - selected_x;
    }
  
    if(selected_y < 0.0){
      selected_y = 0.0;
    }

    if(selected_y + selected_height > allocation_height){
      selected_height = ((double) allocation_height) - selected_y;
    }

    /* draw selected acceleration - dot */
    cairo_set_source_rgba(cr,
			  shadow_color.red,
			  shadow_color.blue,
			  shadow_color.green,
			  opacity / 3.0);
    
    cairo_arc(cr,
	      selected_x, selected_y,
	      automation_edit->point_radius + (2.0 * (double) automation_edit->selected_acceleration_border),
	      0.0,
	      2.0 * M_PI);

    cairo_stroke(cr);

    /* draw selected acceleration - area */
    cairo_rectangle(cr,
		    selected_x, selected_y,
		    selected_width, selected_height);
    cairo_fill(cr);
  }
}

void
ags_automation_edit_draw_automation(AgsAutomationEdit *automation_edit, cairo_t *cr)
{
  AgsWindow *window;
  AgsMachine *selected_machine;
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;
  AgsCompositeEdit *composite_edit;

  AgsNotebook *notebook;

  AgsAcceleration *last_acceleration;
  
  AgsTimestamp *timestamp;
  AgsTimestamp *current_timestamp;    
  
  AgsApplicationContext *application_context;

  GtkAdjustment *adjustment;

  GtkAllocation allocation;

  GType channel_type;

  GList *start_list_automation, *list_automation, *tmp_automation;
  GList *start_list_acceleration, *list_acceleration, *tmp_acceleration;

  gchar *control_name;

  guint width;
  gdouble zoom_factor;
  gdouble x_offset;
  gdouble opacity;
  guint x0, x1;
  guint offset;
  guint line;
  gint i, i_stop;
  
  if(!AGS_IS_AUTOMATION_EDIT(automation_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  gtk_widget_get_allocation(GTK_WIDGET(automation_edit->drawing_area),
			    &allocation);
  
  notebook = NULL;
  
  /* zoom */
  i_stop = 0;
  
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);

  if(composite_editor->selected_machine == NULL){
    return;
  }
    
  selected_machine = composite_editor->selected_machine;

  if(composite_editor->automation_edit->focused_edit == (GtkWidget *) automation_edit){
    notebook = composite_editor->automation_edit->channel_selector;
  }else{
    notebook = NULL;
      
    if(automation_edit->channel_type == G_TYPE_NONE){
      notebook = NULL;
    }else if(automation_edit->channel_type == AGS_TYPE_OUTPUT){
      g_object_get(selected_machine->audio,
		   "output-lines", &i_stop,
		   NULL);
    }else if(automation_edit->channel_type == AGS_TYPE_INPUT){
      g_object_get(selected_machine->audio,
		   "input-lines", &i_stop,
		   NULL);
    }
  }
    
  composite_toolbar = composite_editor->toolbar;

  composite_edit = composite_editor->automation_edit;

  adjustment = gtk_scrollbar_get_adjustment(composite_edit->hscrollbar);

  opacity = gtk_spin_button_get_value(composite_toolbar->opacity);  

  width = gtk_widget_get_width((GtkWidget *) automation_edit->drawing_area);

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));

  x_offset = (zoom_factor * gtk_adjustment_get_value(adjustment));

  /* get visisble region */
  x0 = zoom_factor * gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->hscrollbar));
  x1 = zoom_factor * (gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(automation_edit->hscrollbar)) + allocation.width);

  /* draw automation */
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  start_list_automation = ags_audio_get_automation(selected_machine->audio);
  ags_audio_set_automation(selected_machine->audio,
			   ags_automation_remove_all_empty(start_list_automation));

  start_list_automation = ags_audio_get_automation(selected_machine->audio);
  
  timestamp->timer.ags_offset.offset = (guint64) AGS_AUTOMATION_DEFAULT_OFFSET * floor((double) x0 / (double) AGS_AUTOMATION_DEFAULT_OFFSET);
    
  i = 0;

  if(notebook != NULL){
    i = ags_notebook_next_active_tab(notebook,
				     i);
  }

  goto ags_automation_edit_draw_automation_LOOP;
  
  while(notebook != NULL &&
	(i = ags_notebook_next_active_tab(notebook,
					  i)) != -1){
    AgsAcceleration *first_match;
    AgsAcceleration *last_match;
    
    GList *first_drawn;
    GList *last_drawn;
    GList *first_start_acceleration, *first_acceleration;
    GList *last_start_acceleration, *last_acceleration;
    
  ags_automation_edit_draw_automation_LOOP:      

    first_drawn = ags_automation_edit_find_first_drawn_func(automation_edit,
							    start_list_automation,
							    i);

    last_drawn = ags_automation_edit_find_last_drawn_func(automation_edit,
							  start_list_automation,
							  i);

    list_automation = ags_automation_find_near_timestamp_extended(start_list_automation, i,
								  automation_edit->channel_type, automation_edit->control_name,
								  timestamp);
    
    first_match = NULL;
    
    first_acceleration =
      first_start_acceleration = NULL;
    
    if(first_drawn != NULL){
      first_acceleration =
	first_start_acceleration = ags_automation_get_acceleration(first_drawn->data);
    }

    if(first_acceleration != NULL){
      first_match = first_acceleration->data;
    }
    
    while(first_acceleration != NULL){
      if(ags_acceleration_get_x(first_acceleration->data) >= x0){
	goto ags_automation_edit_draw_automation_LOOP_FIRST_END;
      }
      
      first_match = first_acceleration->data;

      first_acceleration = first_acceleration->next;
    }

  ags_automation_edit_draw_automation_LOOP_FIRST_END:

    if(first_acceleration != NULL){
      first_acceleration = first_acceleration->next;
    }
    
    last_match = NULL;
    
    last_acceleration =
      last_start_acceleration = NULL;

    if(last_drawn != NULL){
      last_acceleration =
	last_start_acceleration = ags_automation_get_acceleration(last_drawn->data);
    }

    if(last_acceleration != NULL){
      last_match = last_acceleration->data;
    }
    
    while(last_acceleration != NULL){
      if(ags_acceleration_get_x(last_acceleration->data) >= x1){
	goto ags_automation_edit_draw_automation_LOOP_LAST_END;
      }
      
      last_match = last_acceleration->data;

      last_acceleration = last_acceleration->next;
    }

  ags_automation_edit_draw_automation_LOOP_LAST_END:

    if(first_match != NULL &&
       first_match != last_match){
      GList *start_next_acceleration;
      GList *next_link;

      next_link = first_acceleration;
      start_next_acceleration = NULL;
      
      if(next_link == NULL &&
	 first_drawn != NULL){
	GList *tmp_list;

	tmp_list = first_drawn->next;

	while(tmp_list != NULL){
	  line = 0;
	  channel_type = G_TYPE_NONE;
	  control_name = NULL;
      
	  g_object_get(tmp_list->data,
		       "line", &line,
		       "channel-type", &channel_type,
		       "control-name", &control_name,
		       NULL);
	    
	  if(i == line &&
	     channel_type == automation_edit->channel_type &&
	     (!g_strcmp0(control_name,
			 automation_edit->control_name))){
	    start_next_acceleration = ags_automation_get_acceleration(tmp_list->data);
	    next_link = start_next_acceleration;

	    goto ags_automation_edit_draw_automation_FIRST_MATCH;
	  }
	    
	  tmp_list = tmp_list->next;
	}
      }

    ags_automation_edit_draw_automation_FIRST_MATCH:

      line = 0;
      channel_type = G_TYPE_NONE;
      control_name = NULL;
      
      g_object_get(first_drawn->data,
		   "line", &line,
		   "channel-type", &channel_type,
		   "control-name", &control_name,
		   NULL);

      if(i == line &&
	 channel_type == automation_edit->channel_type &&
	 !g_strcmp0(control_name,
		    automation_edit->control_name)){
	ags_automation_edit_draw_acceleration(automation_edit,
					      first_match, ((next_link != NULL) ? next_link->data: NULL),
					      cr,
					      opacity);

      }
      
      g_list_free_full(start_next_acceleration,
		       g_object_unref);
    }
    
    while(list_automation != NULL){
      AgsAutomation *automation;

      GList *start_list_acceleration, *list_acceleration;
      
      automation = AGS_AUTOMATION(list_automation->data);

      current_timestamp = NULL;

      line = 0;
      channel_type = G_TYPE_NONE;
      control_name = NULL;
      
      g_object_get(automation,
		   "timestamp", &current_timestamp,
		   "line", &line,
		   "channel-type", &channel_type,
		   "control-name", &control_name,
		   NULL);

      start_list_acceleration = NULL;

      if(i != line ||
	 channel_type != automation_edit->channel_type ||
	 (!g_strcmp0(control_name,
		     automation_edit->control_name)) == FALSE){
	goto ags_automation_edit_draw_automation_INNER_LOOP_END;
      }

      if(ags_timestamp_get_ags_offset(current_timestamp) >= x1){
	goto ags_automation_edit_draw_automation_INNER_LOOP_END;
      }
      
      list_acceleration =
	start_list_acceleration = ags_automation_get_acceleration(automation);

      while(list_acceleration != NULL){
	GList *start_next_acceleration;
	GList *next_link;

	start_next_acceleration = NULL;
	next_link = list_acceleration->next;

	if(next_link == NULL){
	  GList *tmp_list;

	  tmp_list = list_automation->next;

	  while(tmp_list != NULL){
	    line = 0;
	    channel_type = G_TYPE_NONE;
	    control_name = NULL;
      
	    g_object_get(tmp_list->data,
			 "line", &line,
			 "channel-type", &channel_type,
			 "control-name", &control_name,
			 NULL);
	    
	    if(i == line &&
	       channel_type == automation_edit->channel_type &&
	       (!g_strcmp0(control_name,
			   automation_edit->control_name))){
	      goto ags_automation_edit_draw_automation_FIND_FIRST_NEXT_LINK_END;
	    }
	    
	    tmp_list = tmp_list->next;
	  }

	ags_automation_edit_draw_automation_FIND_FIRST_NEXT_LINK_END:
	  
	  if(tmp_list != NULL){
	    start_next_acceleration = ags_automation_get_acceleration(tmp_list->data);
	    next_link = start_next_acceleration;
	  }
	}

#if 0
	g_message("found accel[%d] @ line = %d", AGS_ACCELERATION(list_acceleration->data)->x, automation->line);
#endif
	
	if(list_acceleration->data != first_match &&
	   list_acceleration->data != last_match &&
	   next_link != NULL){
	  ags_automation_edit_draw_acceleration(automation_edit,
						list_acceleration->data, next_link->data,
						cr,
						opacity);
	}

	g_list_free_full(start_next_acceleration,
			 g_object_unref);
	
	/* iterate */
	list_acceleration = list_acceleration->next;
      }
      
    ags_automation_edit_draw_automation_INNER_LOOP_END:

      g_object_unref(current_timestamp);
      
      g_list_free_full(start_list_acceleration,
		       g_object_unref);

      /* iterate */
      list_automation = list_automation->next;
    }

  ags_automation_edit_draw_automation_LOOP_END:

    line = 0;
    channel_type = G_TYPE_NONE;
    control_name = NULL;
      
    g_object_get(last_drawn->data,
		 "line", &line,
		 "channel-type", &channel_type,
		 "control-name", &control_name,
		 NULL);
    
    if(last_match != NULL &&
       i == line &&
       channel_type == automation_edit->channel_type &&
       (!g_strcmp0(control_name,
		   automation_edit->control_name))){
      GList *start_next_acceleration;
      GList *next_link;

      next_link = last_acceleration;
      start_next_acceleration = NULL;
      
      if(next_link == NULL &&
	 list_automation != NULL){
	GList *tmp_list;

	tmp_list = list_automation->next;

	while(tmp_list != NULL){
	  line = 0;
	  channel_type = G_TYPE_NONE;
	  control_name = NULL;
      
	  g_object_get(tmp_list->data,
		       "line", &line,
		       "channel-type", &channel_type,
		       "control-name", &control_name,
		       NULL);
	    
	  if(i == line &&
	     channel_type == automation_edit->channel_type &&
	     !g_strcmp0(control_name,
			automation_edit->control_name)){
	    start_next_acceleration = ags_automation_get_acceleration(tmp_list->data);
	    next_link = start_next_acceleration;

	    goto ags_automation_edit_draw_automation_LAST_MATCH;
	  }
	    
	  tmp_list = tmp_list->next;
	}
      }
      
    ags_automation_edit_draw_automation_LAST_MATCH:
      
      ags_automation_edit_draw_acceleration(automation_edit,
					    last_match, ((next_link != NULL) ? next_link->data: NULL),
					    cr,
					    opacity);

      g_list_free_full(start_next_acceleration,
		       g_object_unref);
    }
    
    i++;
    
    if(notebook == NULL &&
       i >= i_stop){
      break;
    }
  }

  g_list_free_full(start_list_automation,
		   g_object_unref);
  
  g_object_unref(timestamp);  
}

void
ags_automation_edit_draw(AgsAutomationEdit *automation_edit, cairo_t *cr)
{
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  ags_automation_edit_reset_vscrollbar(automation_edit);
  ags_automation_edit_reset_hscrollbar(automation_edit);
  
  /* segment */
  ags_automation_edit_draw_segment(automation_edit, cr);

  /* automation */
  ags_automation_edit_draw_automation(automation_edit, cr);
  
  /* edit mode */
  switch(automation_edit->mode){
  case AGS_AUTOMATION_EDIT_POSITION_CURSOR:
    {
      ags_automation_edit_draw_cursor(automation_edit, cr);
    }
    break;
  case AGS_AUTOMATION_EDIT_ADD_ACCELERATION:
    {
      if(automation_edit->current_acceleration != NULL){
	ags_automation_edit_draw_acceleration(automation_edit,
					      automation_edit->current_acceleration, NULL,
					      cr,
					      1.0);

	cairo_surface_mark_dirty(cairo_get_target(cr));
      }
    }
    break;
  case AGS_AUTOMATION_EDIT_SELECT_ACCELERATION:
    {
      ags_automation_edit_draw_selection(automation_edit, cr);
    }
    break;
  }

  /* fader */
  if((AGS_AUTOMATION_EDIT_AUTO_SCROLL & (automation_edit->flags)) != 0){
    ags_automation_edit_draw_position(automation_edit, cr);
  }
}

/**
 * ags_automation_edit_new:
 *
 * Create a new #AgsAutomationEdit.
 *
 * Returns: a new #AgsAutomationEdit
 *
 * Since: 3.0.0
 */
AgsAutomationEdit*
ags_automation_edit_new()
{
  AgsAutomationEdit *automation_edit;

  automation_edit = (AgsAutomationEdit *) g_object_new(AGS_TYPE_AUTOMATION_EDIT, NULL);

  return(automation_edit);
}
