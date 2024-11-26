/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/editor/ags_tempo_edit.h>
#include <ags/app/editor/ags_tempo_edit_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_navigation.h>
#include <ags/app/ags_composite_editor.h>

#include <gdk/gdkkeysyms.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/audio/ags_program.h>

#include <cairo.h>
#include <math.h>

#include <ags/i18n.h>

void ags_tempo_edit_class_init(AgsTempoEditClass *tempo_edit);
void ags_tempo_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_tempo_edit_init(AgsTempoEdit *tempo_edit);
void ags_tempo_edit_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_tempo_edit_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_tempo_edit_dispose(GObject *gobject);
void ags_tempo_edit_finalize(GObject *gobject);

gboolean ags_tempo_edit_is_connected(AgsConnectable *connectable);
void ags_tempo_edit_connect(AgsConnectable *connectable);
void ags_tempo_edit_disconnect(AgsConnectable *connectable);

void ags_tempo_edit_realize(GtkWidget *widget);
void ags_tempo_edit_unrealize(GtkWidget *widget);

void ags_tempo_edit_measure(GtkWidget *widget,
			    GtkOrientation orientation,
			    int for_size,
			    int *minimum,
			    int *natural,
			    int *minimum_baseline,
			    int *natural_baseline);
void ags_tempo_edit_size_allocate(GtkWidget *widget,
				  int width,
				  int height,
				  int baseline);

void ags_tempo_edit_show(GtkWidget *widget);

void ags_tempo_edit_frame_clock_update_callback(GdkFrameClock *frame_clock,
						AgsTempoEdit *tempo_edit);

void ags_tempo_edit_drawing_area_button_press_position_cursor(GtkWidget *editor,
							      GtkWidget *toolbar,
							      AgsTempoEdit *tempo_edit,
							      gint n_press,
							      gdouble x, gdouble y);
void ags_tempo_edit_drawing_area_button_press_add_marker(GtkWidget *editor,
							 GtkWidget *toolbar,
							 AgsTempoEdit *tempo_edit,
							 gint n_press,
							 gdouble x, gdouble y);
void ags_tempo_edit_drawing_area_button_press_select_marker(GtkWidget *editor,
							    GtkWidget *toolbar,
							    AgsTempoEdit *tempo_edit,
							    gint n_press,
							    gdouble x, gdouble y);

void ags_tempo_edit_drawing_area_button_release_position_cursor(GtkWidget *editor,
								GtkWidget *toolbar,
								AgsTempoEdit *tempo_edit,
								gint n_press, gdouble x, gdouble y);
void ags_tempo_edit_drawing_area_button_release_add_marker(GtkWidget *editor,
							   GtkWidget *toolbar,
							   AgsTempoEdit *tempo_edit,
							   gint n_press,
							   gdouble x, gdouble y);
void ags_tempo_edit_drawing_area_button_release_delete_marker(GtkWidget *editor,
							      GtkWidget *toolbar,
							      AgsTempoEdit *tempo_edit,
							      gint n_press,
							      gdouble x, gdouble y);
void ags_tempo_edit_drawing_area_button_release_select_marker(GtkWidget *editor,
							      GtkWidget *toolbar,
							      AgsTempoEdit *tempo_edit,
							      gint n_press,
							      gdouble x, gdouble y);

gboolean ags_tempo_edit_gesture_click_pressed_callback(GtkGestureClick *event_controller,
						       gint n_press,
						       gdouble x,
						       gdouble y,
						       AgsTempoEdit *tempo_edit);
gboolean ags_tempo_edit_gesture_click_released_callback(GtkGestureClick *event_controller,
							gint n_press,
							gdouble x,
							gdouble y,
							AgsTempoEdit *tempo_edit);

gboolean ags_tempo_edit_key_pressed_callback(GtkEventControllerKey *event_controller,
					     guint keyval,
					     guint keycode,
					     GdkModifierType state,
					     AgsTempoEdit *tempo_edit);
gboolean ags_tempo_edit_key_released_callback(GtkEventControllerKey *event_controller,
					      guint keyval,
					      guint keycode,
					      GdkModifierType state,
					      AgsTempoEdit *tempo_edit);
gboolean ags_tempo_edit_modifiers_callback(GtkEventControllerKey *event_controller,
					   GdkModifierType keyval,
					   AgsTempoEdit *tempo_edit);


void ags_tempo_edit_drawing_area_motion_notify_position_cursor(GtkWidget *editor,
							       GtkWidget *toolbar,
							       AgsTempoEdit *tempo_edit,
							       gdouble x, gdouble y);
void ags_tempo_edit_drawing_area_motion_notify_add_marker(GtkWidget *editor,
							  GtkWidget *toolbar,
							  AgsTempoEdit *tempo_edit,
							  gdouble x, gdouble y);
void ags_tempo_edit_drawing_area_motion_notify_select_marker(GtkWidget *editor,
							     GtkWidget *toolbar,
							     AgsTempoEdit *tempo_edit,
							     gdouble x, gdouble y);

gboolean ags_tempo_edit_motion_callback(GtkEventControllerMotion *event_controller,
					gdouble x,
					gdouble y,
					AgsTempoEdit *tempo_edit);

/**
 * SECTION:ags_tempo_edit
 * @short_description: edit tempos
 * @title: AgsTempoEdit
 * @section_id:
 * @include: ags/app/editor/ags_tempo_edit.h
 *
 * The #AgsTempoEdit lets you edit tempos.
 */

enum{
  PROP_0,
};

static gpointer ags_tempo_edit_parent_class = NULL;

static GQuark quark_accessible_object = 0;

GType
ags_tempo_edit_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_tempo_edit = 0;

    static const GTypeInfo ags_tempo_edit_info = {
      sizeof (AgsTempoEditClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_tempo_edit_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTempoEdit),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_tempo_edit_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_tempo_edit_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_tempo_edit = g_type_register_static(GTK_TYPE_GRID,
						 "AgsTempoEdit", &ags_tempo_edit_info,
						 0);
    
    g_type_add_interface_static(ags_type_tempo_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_tempo_edit);
  }

  return(g_define_type_id__static);
}

void
ags_tempo_edit_class_init(AgsTempoEditClass *tempo_edit)
{
  GtkWidgetClass *widget;

  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_tempo_edit_parent_class = g_type_class_peek_parent(tempo_edit);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(tempo_edit);

  gobject->set_property = ags_tempo_edit_set_property;
  gobject->get_property = ags_tempo_edit_get_property;

  gobject->dispose = ags_tempo_edit_dispose;
  gobject->finalize = ags_tempo_edit_finalize;

  /* properties */

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) tempo_edit;

  widget->realize = ags_tempo_edit_realize;
  widget->unrealize = ags_tempo_edit_unrealize;

  widget->measure = ags_tempo_edit_measure;
  widget->size_allocate = ags_tempo_edit_size_allocate;

  widget->show = ags_tempo_edit_show;  
}

void
ags_tempo_edit_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_tempo_edit_is_connected;  
  connectable->connect = ags_tempo_edit_connect;
  connectable->disconnect = ags_tempo_edit_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_tempo_edit_init(AgsTempoEdit *tempo_edit)
{
  GtkEventController *event_controller;
  GtkAdjustment *adjustment;

  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) tempo_edit,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_tempo_edit_key_pressed_callback), tempo_edit);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_tempo_edit_key_released_callback), tempo_edit);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_tempo_edit_modifiers_callback), tempo_edit);

  event_controller = (GtkEventController *) gtk_gesture_click_new();
  gtk_widget_add_controller((GtkWidget *) tempo_edit,
			    event_controller);

  g_signal_connect(event_controller, "pressed",
		   G_CALLBACK(ags_tempo_edit_gesture_click_pressed_callback), tempo_edit);

  g_signal_connect(event_controller, "released",
		   G_CALLBACK(ags_tempo_edit_gesture_click_released_callback), tempo_edit);

  event_controller = gtk_event_controller_motion_new();
  gtk_widget_add_controller((GtkWidget *) tempo_edit,
			    event_controller);

  g_signal_connect(event_controller, "motion",
		   G_CALLBACK(ags_tempo_edit_motion_callback), tempo_edit);

  tempo_edit->flags = 0;
  tempo_edit->connectable_flags = 0;
  tempo_edit->mode = AGS_TEMPO_EDIT_NO_EDIT_MODE;

  tempo_edit->button_mask = 0;
  tempo_edit->key_mask = 0;
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  tempo_edit->note_offset = 0;
  tempo_edit->note_offset_absolute = 0;

  tempo_edit->point_radius = (guint) (gui_scale_factor * AGS_TEMPO_EDIT_DEFAULT_POINT_RADIUS);

  tempo_edit->scan_width = (guint) (gui_scale_factor * AGS_TEMPO_EDIT_DEFAULT_SCAN_WIDTH);
  tempo_edit->scan_height = (guint) (gui_scale_factor * AGS_TEMPO_EDIT_DEFAULT_SCAN_HEIGHT);

  tempo_edit->control_width = (guint) (gui_scale_factor * AGS_TEMPO_EDIT_DEFAULT_CONTROL_WIDTH);
  tempo_edit->control_height = (guint) (gui_scale_factor * AGS_TEMPO_EDIT_DEFAULT_CONTROL_HEIGHT);

  tempo_edit->step_count = (guint) (AGS_TEMPO_EDIT_DEFAULT_STEP_COUNT);
  
  tempo_edit->cursor_position_x = AGS_TEMPO_EDIT_DEFAULT_CURSOR_POSITION_X;
  tempo_edit->cursor_position_y = AGS_TEMPO_EDIT_DEFAULT_CURSOR_POSITION_Y;

  tempo_edit->selected_marker_border = AGS_TEMPO_EDIT_DEFAULT_SELECTED_MARKER_BORDER;

  tempo_edit->selection_x0 = 0;
  tempo_edit->selection_x1 = 0;
  tempo_edit->selection_y0 = 0;
  tempo_edit->selection_y1 = 0;

  tempo_edit->current_marker = NULL;

  tempo_edit->ruler = ags_ruler_new(GTK_ORIENTATION_HORIZONTAL,
				    AGS_RULER_DEFAULT_STEP,
				    AGS_RULER_DEFAULT_FACTOR,
				    AGS_RULER_DEFAULT_PRECISION,
				    AGS_RULER_DEFAULT_SCALE_PRECISION);

  gtk_widget_set_visible((GtkWidget *) tempo_edit->ruler,
			 FALSE);
  
  gtk_grid_attach(GTK_GRID(tempo_edit),
		  (GtkWidget *) tempo_edit->ruler,
		  0, 0,
		  1, 1);

  tempo_edit->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_can_focus((GtkWidget *) tempo_edit->drawing_area,
			   TRUE);
  gtk_widget_set_focusable((GtkWidget *) tempo_edit->drawing_area,
			   TRUE);

  gtk_widget_set_halign((GtkWidget *) tempo_edit->drawing_area,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) tempo_edit->drawing_area,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand((GtkWidget *) tempo_edit->drawing_area,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) tempo_edit->drawing_area,
			 FALSE);

  gtk_widget_set_size_request((GtkWidget *) tempo_edit->drawing_area,
			      -1, AGS_SCALE_DEFAULT_HEIGHT_REQUEST);
  
  gtk_grid_attach((GtkGrid *) tempo_edit,
		  (GtkWidget *) tempo_edit->drawing_area,
		  0, 1,
		  1, 1);

  /* vscrollbar */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, tempo_edit->control_height, 1.0);
  tempo_edit->vscrollbar = (GtkScrollbar *) gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL,
							      adjustment);
  gtk_widget_set_visible((GtkWidget *) tempo_edit->vscrollbar,
			 FALSE);
  gtk_widget_set_size_request((GtkWidget *) tempo_edit->vscrollbar,
			      -1, (guint) (gui_scale_factor * AGS_SCALE_DEFAULT_HEIGHT_REQUEST));
  gtk_grid_attach((GtkGrid *) tempo_edit,
		  (GtkWidget *) tempo_edit->vscrollbar,
		  1, 1,
		  1, 1);

  /* hscrollbar */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (gdouble) tempo_edit->control_width, 1.0);
  tempo_edit->hscrollbar = (GtkScrollbar *) gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL,
							      adjustment);
  gtk_widget_set_visible((GtkWidget *) tempo_edit->hscrollbar,
			 FALSE);
  gtk_widget_set_size_request((GtkWidget *) tempo_edit->hscrollbar,
			      -1, -1);
  gtk_grid_attach(GTK_GRID(tempo_edit),
		  (GtkWidget *) tempo_edit->hscrollbar,
		  0, 2,
		  1, 1);

  /* 256th */
  tempo_edit->note_offset_256th = 16 * tempo_edit->note_offset;
  tempo_edit->note_offset_256th_absolute = 16 * tempo_edit->note_offset_absolute;

  /* auto-scroll */
  g_signal_connect(application_context, "update-ui",
		   G_CALLBACK(ags_tempo_edit_update_ui_callback), tempo_edit);
}

void
ags_tempo_edit_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsTempoEdit *tempo_edit;

  tempo_edit = AGS_TEMPO_EDIT(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_tempo_edit_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsTempoEdit *tempo_edit;

  tempo_edit = AGS_TEMPO_EDIT(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_tempo_edit_dispose(GObject *gobject)
{
  AgsTempoEdit *tempo_edit;
  
  tempo_edit = AGS_TEMPO_EDIT(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_tempo_edit_parent_class)->dispose(gobject);
}

void
ags_tempo_edit_finalize(GObject *gobject)
{
  AgsTempoEdit *tempo_edit;
  
  AgsApplicationContext *application_context;

  tempo_edit = AGS_TEMPO_EDIT(gobject);
  
  application_context = ags_application_context_get_instance();

  /* remove auto scroll */
  g_object_disconnect(application_context,
		      "any_signal::update-ui",
		      G_CALLBACK(ags_tempo_edit_update_ui_callback),
		      (gpointer) tempo_edit,
		      NULL);

  /* call parent */
  G_OBJECT_CLASS(ags_tempo_edit_parent_class)->finalize(gobject);
}

gboolean
ags_tempo_edit_is_connected(AgsConnectable *connectable)
{
  AgsTempoEdit *tempo_edit;
  
  gboolean is_connected;
  
  tempo_edit = AGS_TEMPO_EDIT(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (tempo_edit->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_tempo_edit_connect(AgsConnectable *connectable)
{
  AgsTempoEdit *tempo_edit;

  tempo_edit = AGS_TEMPO_EDIT(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }
  
  tempo_edit->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  /* drawing area */
  gtk_drawing_area_set_draw_func(tempo_edit->drawing_area,
				 (GtkDrawingAreaDrawFunc) ags_tempo_edit_draw_callback,
				 tempo_edit,
				 NULL);

  g_signal_connect_after((GObject *) tempo_edit->drawing_area, "resize",
			 G_CALLBACK(ags_tempo_edit_drawing_area_resize_callback), (gpointer) tempo_edit);

  /* scrollbars */
  g_signal_connect_after((GObject *) gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar), "value-changed",
			 G_CALLBACK(ags_tempo_edit_vscrollbar_value_changed), (gpointer) tempo_edit);

  g_signal_connect_after((GObject *) gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar), "value-changed",
			 G_CALLBACK(ags_tempo_edit_hscrollbar_value_changed), (gpointer) tempo_edit);
}

void
ags_tempo_edit_disconnect(AgsConnectable *connectable)
{
  AgsTempoEdit *tempo_edit;

  tempo_edit = AGS_TEMPO_EDIT(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }
  
  tempo_edit->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  /* drawing area */
  gtk_drawing_area_set_draw_func(tempo_edit->drawing_area,
				 NULL,
				 NULL,
				 NULL);
  
  g_object_disconnect((GObject *) tempo_edit->drawing_area,
		      "any_signal::resize",
		      G_CALLBACK(ags_tempo_edit_drawing_area_resize_callback),
		      (gpointer) tempo_edit,
		      NULL);

  /* scrollbars */
  g_object_disconnect((GObject *) gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar),
		      "any_signal::value-changed",
		      G_CALLBACK(ags_tempo_edit_vscrollbar_value_changed),
		      (gpointer) tempo_edit,
		      NULL);

  g_object_disconnect((GObject *) gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar),
		      "any_signal::value-changed",
		      G_CALLBACK(ags_tempo_edit_hscrollbar_value_changed),
		      (gpointer) tempo_edit,
		      NULL);
}

gboolean
ags_tempo_edit_key_pressed_callback(GtkEventControllerKey *event_controller,
				    guint keyval,
				    guint keycode,
				    GdkModifierType state,
				    AgsTempoEdit *tempo_edit)
{  
  GtkWidget *editor;

  guint l_control_key, r_control_key;  
  gboolean key_handled;

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
  
  editor = gtk_widget_get_ancestor(GTK_WIDGET(tempo_edit),
				   AGS_TYPE_COMPOSITE_EDITOR);

#if defined(AGS_OSXAPI)
  l_control_key = AGS_TEMPO_EDIT_KEY_L_META;
  r_control_key = AGS_TEMPO_EDIT_KEY_R_META;
#else
  l_control_key = AGS_TEMPO_EDIT_KEY_L_CONTROL;
  r_control_key = AGS_TEMPO_EDIT_KEY_R_CONTROL;
#endif
  
  switch(keyval){
  case GDK_KEY_Control_L:
    {
      tempo_edit->key_mask |= AGS_TEMPO_EDIT_KEY_L_CONTROL;
    }
    break;
  case GDK_KEY_Control_R:
    {
      tempo_edit->key_mask |= AGS_TEMPO_EDIT_KEY_R_CONTROL;
    }
    break;
  case GDK_KEY_Shift_L:
    {
      tempo_edit->key_mask |= AGS_TEMPO_EDIT_KEY_L_SHIFT;
    }
    break;
  case GDK_KEY_Shift_R:
    {
      tempo_edit->key_mask |= AGS_TEMPO_EDIT_KEY_R_SHIFT;
    }
    break;
  case GDK_KEY_Meta_L:
    {
      tempo_edit->key_mask |= AGS_TEMPO_EDIT_KEY_L_META;
    }
    break;
  case GDK_KEY_Meta_R:
    {
      tempo_edit->key_mask |= AGS_TEMPO_EDIT_KEY_R_META;
    }
    break;
  }

  gtk_widget_queue_draw((GtkWidget *) tempo_edit->drawing_area);

  return(key_handled);
}

gboolean
ags_tempo_edit_key_released_callback(GtkEventControllerKey *event_controller,
				     guint keyval,
				     guint keycode,
				     GdkModifierType state,
				     AgsTempoEdit *tempo_edit)
{  
  AgsCompositeEditor *composite_editor;
  
  GtkAllocation allocation;

  guint l_control_key, r_control_key;  
  double zoom_factor;
  gint i;
  gboolean do_feedback;
  gboolean key_handled;

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
  
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) tempo_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(composite_editor->toolbar)->zoom));

  gtk_widget_get_allocation(GTK_WIDGET(tempo_edit->drawing_area),
			    &allocation);

#if defined(AGS_OSXAPI)
  l_control_key = AGS_TEMPO_EDIT_KEY_L_META;
  r_control_key = AGS_TEMPO_EDIT_KEY_R_META;
#else
  l_control_key = AGS_TEMPO_EDIT_KEY_L_CONTROL;
  r_control_key = AGS_TEMPO_EDIT_KEY_R_CONTROL;
#endif

  /* do feedback - initial set */
  do_feedback = FALSE;

  /* check key value */
  switch(keyval){
  case GDK_KEY_Control_L:
    {
      tempo_edit->key_mask &= (~AGS_TEMPO_EDIT_KEY_L_CONTROL);
    }
    break;
  case GDK_KEY_Control_R:
    {
      tempo_edit->key_mask &= (~AGS_TEMPO_EDIT_KEY_R_CONTROL);
    }
    break;
  case GDK_KEY_Shift_L:
    {
      tempo_edit->key_mask &= (~AGS_TEMPO_EDIT_KEY_L_SHIFT);
    }
    break;
  case GDK_KEY_Shift_R:
    {
      tempo_edit->key_mask &= (~AGS_TEMPO_EDIT_KEY_R_SHIFT);
    }
    break;
  case GDK_KEY_Meta_L:
    {
      tempo_edit->key_mask &= (~AGS_TEMPO_EDIT_KEY_L_META);
    }
    break;
  case GDK_KEY_Meta_R:
    {
      tempo_edit->key_mask &= (~AGS_TEMPO_EDIT_KEY_R_META);
    }
    break;
  }

  gtk_widget_queue_draw((GtkWidget *) tempo_edit);
  
  gtk_widget_queue_draw((GtkWidget *) tempo_edit->drawing_area);
  
  return(key_handled);
}

gboolean
ags_tempo_edit_modifiers_callback(GtkEventControllerKey *event_controller,
				  GdkModifierType keyval,
				  AgsTempoEdit *tempo_edit)
{
  return(FALSE);
}

void
ags_tempo_edit_drawing_area_motion_notify_position_cursor(GtkWidget *editor,
							  GtkWidget *toolbar,
							  AgsTempoEdit *tempo_edit,
							  gdouble x, gdouble y)
{
  AgsApplicationContext *application_context;
  
  double zoom_factor;

  application_context = ags_application_context_get_instance();

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));

  /* cursor position */
  tempo_edit->cursor_position_x = (guint) (zoom_factor * (x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar)))) / tempo_edit->control_width;
  tempo_edit->cursor_position_x = zoom_factor * floor(tempo_edit->cursor_position_x / zoom_factor);

  tempo_edit->cursor_position_y = (guint) ((y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar))) / tempo_edit->control_height);

#ifdef AGS_DEBUG
  g_message("%lu %lu", tempo_edit->cursor_position_x, tempo_edit->cursor_position_y);
#endif
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) tempo_edit);
}

void
ags_tempo_edit_drawing_area_motion_notify_add_marker(GtkWidget *editor,
						     GtkWidget *toolbar,
						     AgsTempoEdit *tempo_edit,
						     gdouble x, gdouble y)
{
  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;

  AgsMarker *marker;
    
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
  
  gtk_widget_get_allocation(GTK_WIDGET(tempo_edit->drawing_area),
			    &allocation);

  marker = tempo_edit->current_marker;
    
  if(marker == NULL){
    return;
  }
    
  c_range = 240.0;

  vscrollbar_adjustment = gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar);
  hscrollbar_adjustment = gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar);

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* marker */
  marker->x = (guint) zoom_factor * (x + gtk_adjustment_get_value(hscrollbar_adjustment));

  lower = 0.0;
  upper = 240.0;
    
  marker->y = (((allocation.height - y) / g_range) * c_range) + lower;
    
#ifdef AGS_DEBUG
  g_message("%lu %f", marker->x, marker->y);
#endif
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) tempo_edit->drawing_area);
}

void
ags_tempo_edit_drawing_area_motion_notify_select_marker(GtkWidget *editor,
							GtkWidget *toolbar,
							AgsTempoEdit *tempo_edit,
							gdouble x, gdouble y)
{
  if(x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar)) >= 0.0){
    tempo_edit->selection_x1 = (guint) x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar));
  }else{
    tempo_edit->selection_x1 = 0.0;
  }
    
  if(y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar)) >= 0.0){
    tempo_edit->selection_y1 = (guint) y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar));
  }else{
    tempo_edit->selection_y1 = 0.0;
  }

  gtk_widget_queue_draw((GtkWidget *) tempo_edit);
}

gboolean
ags_tempo_edit_motion_callback(GtkEventControllerMotion *event_controller,
			       gdouble x,
			       gdouble y,
			       AgsTempoEdit *tempo_edit)
{
  AgsCompositeToolbar *composite_toolbar;
  AgsCompositeEditor *composite_editor;
  
  gboolean selected_position_cursor, selected_edit, selected_clear, selected_select;

  selected_position_cursor = FALSE;
  selected_edit = FALSE;
  selected_clear = FALSE;
  selected_select = FALSE;
    
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) tempo_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);

  composite_toolbar = composite_editor->toolbar;

  selected_position_cursor = (composite_toolbar->selected_tool == (GtkButton *) composite_toolbar->position) ? TRUE: FALSE;
  selected_edit = (composite_toolbar->selected_tool == (GtkButton *) composite_toolbar->edit) ? TRUE: FALSE;
  selected_clear = (composite_toolbar->selected_tool == (GtkButton *) composite_toolbar->clear) ? TRUE: FALSE;
  selected_select = (composite_toolbar->selected_tool == (GtkButton *) composite_toolbar->select) ? TRUE: FALSE;

  gtk_widget_grab_focus((GtkWidget *) tempo_edit->drawing_area);

  if((AGS_TEMPO_EDIT_BUTTON_1 & (tempo_edit->button_mask)) != 0){
    if(selected_position_cursor){
      ags_tempo_edit_drawing_area_motion_notify_position_cursor((GtkWidget *) composite_editor,
								(GtkWidget *) composite_toolbar,
								tempo_edit,
								x, y);
    }else if(selected_edit){
      ags_tempo_edit_drawing_area_motion_notify_add_marker((GtkWidget *) composite_editor,
							   (GtkWidget *) composite_toolbar,
							   tempo_edit,
							   x, y);
    }else if(selected_clear){
      //NOTE:JK: only takes action on release
    }else if(selected_select){
      ags_tempo_edit_drawing_area_motion_notify_select_marker((GtkWidget *) composite_editor,
							      (GtkWidget *) composite_toolbar,
							      tempo_edit,
							      x, y);
    }

    gtk_widget_queue_draw((GtkWidget *) tempo_edit->drawing_area);
  }

  return(FALSE);
}

void
ags_tempo_edit_drawing_area_button_press_position_cursor(GtkWidget *editor,
							 GtkWidget *toolbar,
							 AgsTempoEdit *tempo_edit,
							 gint n_press,
							 gdouble x, gdouble y)
{
  double zoom_factor;

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  
  /* cursor position */
  tempo_edit->cursor_position_x = (guint) (zoom_factor * (x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar)))) / tempo_edit->control_width;
  tempo_edit->cursor_position_x = zoom_factor * floor(tempo_edit->cursor_position_x / zoom_factor);
    
  tempo_edit->cursor_position_y = (guint) ((y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar))) / tempo_edit->control_height);

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) tempo_edit);
}

void
ags_tempo_edit_drawing_area_button_press_add_marker(GtkWidget *editor,
						    GtkWidget *toolbar,
						    AgsTempoEdit *tempo_edit,
						    gint n_press,
						    gdouble x, gdouble y)
{
  AgsMarker *marker;

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
    
  gtk_widget_get_allocation(GTK_WIDGET(tempo_edit->drawing_area),
			    &allocation);

  marker = ags_marker_new();

  c_range = 240.0;

  vscrollbar_adjustment = gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar);
  hscrollbar_adjustment = gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar);
    
  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* marker */
  marker->x = (guint) (zoom_factor * (x + gtk_adjustment_get_value(hscrollbar_adjustment)));

  lower = 0.0;
  upper = 240.0;
    
  marker->y = (((allocation.height - y) / g_range) * c_range) + lower;
    
  /* current marker */
  if(tempo_edit->current_marker != NULL){
    g_object_unref(tempo_edit->current_marker);

    tempo_edit->current_marker = NULL;
  }

  tempo_edit->current_marker = marker;
  g_object_ref(marker);

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) tempo_edit);
}

void
ags_tempo_edit_drawing_area_button_press_select_marker(GtkWidget *editor,
						       GtkWidget *toolbar,
						       AgsTempoEdit *tempo_edit,
						       gint n_press,
						       gdouble x, gdouble y)
{
  tempo_edit->selection_x0 = (guint) x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar));
  tempo_edit->selection_x1 = tempo_edit->selection_x0;
    
  tempo_edit->selection_y0 = (guint) y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar));
  tempo_edit->selection_y1 = tempo_edit->selection_y0;

  gtk_widget_queue_draw((GtkWidget *) tempo_edit);
}

void
ags_tempo_edit_drawing_area_button_release_position_cursor(GtkWidget *editor,
							   GtkWidget *toolbar,
							   AgsTempoEdit *tempo_edit,
							   gint n_press,
							   gdouble x, gdouble y)
{  
  double zoom_factor;
  
  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));

  /* cursor position */
  tempo_edit->cursor_position_x = (guint) (zoom_factor * (x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar)))) / tempo_edit->control_width;
  tempo_edit->cursor_position_x = zoom_factor * floor(tempo_edit->cursor_position_x / zoom_factor);

  tempo_edit->cursor_position_y = (guint) ((y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar))) / tempo_edit->control_height);
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) tempo_edit);
}

void
ags_tempo_edit_drawing_area_button_release_add_marker(GtkWidget *editor,
						      GtkWidget *toolbar,
						      AgsTempoEdit *tempo_edit,
						      gint n_press,
						      gdouble x, gdouble y)
{
  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;

  AgsMarker *marker;

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
  
  gtk_widget_get_allocation(GTK_WIDGET(tempo_edit->drawing_area),
			    &allocation);

  marker = tempo_edit->current_marker;
    
  if(marker == NULL){
    return;
  }

  c_range = 240.0;

  vscrollbar_adjustment = gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar);
  hscrollbar_adjustment = gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar);

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* marker */
  marker->x = (guint) (zoom_factor * (x + gtk_adjustment_get_value(hscrollbar_adjustment)));
    
  lower = 0.0;
  upper = 240.0;

  marker->y = (((allocation.height - y) / g_range) * c_range) + lower;
    
#ifdef AGS_DEBUG
  g_message("%lu %f", marker->x, marker->y);
#endif

  /* add marker */
  ags_composite_editor_add_marker((AgsCompositeEditor *) editor,
				  marker);
  
  tempo_edit->current_marker = NULL;
  g_object_unref(marker);

  gtk_widget_queue_draw((GtkWidget *) tempo_edit->drawing_area);
}
  
void
ags_tempo_edit_drawing_area_button_release_delete_marker(GtkWidget *editor,
							 GtkWidget *toolbar,
							 AgsTempoEdit *tempo_edit,
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
    
  gtk_widget_get_allocation(GTK_WIDGET(tempo_edit->drawing_area),
			    &allocation);

  c_range = 240.0;

  vscrollbar_adjustment = gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar);
  hscrollbar_adjustment = gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar);
    
  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;
  
  /* marker */
  x = (guint) zoom_factor * ((x + gtk_adjustment_get_value(hscrollbar_adjustment)));

  lower = 0.0;
  upper = 240.0;
    
  y = (((allocation.height - y) / g_range) * c_range) + lower;
    
  /* delete marker */
  ags_composite_editor_delete_marker((AgsCompositeEditor *) editor,
				     x, y);

  gtk_widget_queue_draw((GtkWidget *) tempo_edit->drawing_area);
}

void
ags_tempo_edit_drawing_area_button_release_select_marker(GtkWidget *editor,
							 GtkWidget *toolbar,
							 AgsTempoEdit *tempo_edit,
							 gint n_press,
							 gdouble x, gdouble y)
{
  double zoom_factor;
  guint x0, x1, y0, y1;

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));

  /* region */
  x0 = (guint) (zoom_factor * tempo_edit->selection_x0) / tempo_edit->control_width;

  y0 = (guint) (tempo_edit->selection_y0 / tempo_edit->control_height);
    
  x1 = (guint) (zoom_factor * (x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar)))) / tempo_edit->control_width;
    
  y1 = (guint) ((y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar))) / tempo_edit->control_height);

  /* select region */
  ags_composite_editor_select_region((AgsCompositeEditor *) editor,
				     x0, y0,
				     x1, y1);
}

gboolean
ags_tempo_edit_gesture_click_pressed_callback(GtkGestureClick *event_controller,
					      gint n_press,
					      gdouble x,
					      gdouble y,
					      AgsTempoEdit *tempo_edit)
{
  AgsCompositeToolbar *composite_toolbar;
  AgsCompositeEditor *composite_editor;
  
  gboolean selected_position_cursor, selected_edit, selected_clear, selected_select;

  selected_position_cursor = FALSE;
  selected_edit = FALSE;
  selected_clear = FALSE;
  selected_select = FALSE;
      
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) tempo_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
  
  composite_toolbar = composite_editor->toolbar;
    
  selected_position_cursor = (composite_toolbar->selected_tool == (GtkButton *) composite_toolbar->position) ? TRUE: FALSE;
  selected_edit = (composite_toolbar->selected_tool == (GtkButton *) composite_toolbar->edit) ? TRUE: FALSE;
  selected_clear = (composite_toolbar->selected_tool == (GtkButton *) composite_toolbar->clear) ? TRUE: FALSE;
  selected_select = (composite_toolbar->selected_tool == (GtkButton *) composite_toolbar->select) ? TRUE: FALSE;
  
  gtk_widget_grab_focus((GtkWidget *) tempo_edit->drawing_area);
  
  tempo_edit->button_mask |= AGS_TEMPO_EDIT_BUTTON_1;
    
  if(selected_position_cursor){
    tempo_edit->mode = AGS_TEMPO_EDIT_POSITION_CURSOR;
      
    ags_tempo_edit_drawing_area_button_press_position_cursor((GtkWidget *) composite_editor,
							     (GtkWidget *) composite_toolbar,
							     tempo_edit,
							     n_press,
							     x, y);
  }else if(selected_edit){
    tempo_edit->mode = AGS_TEMPO_EDIT_ADD_MARKER;

    ags_tempo_edit_drawing_area_button_press_add_marker((GtkWidget *) composite_editor,
							(GtkWidget *) composite_toolbar,
							tempo_edit,
							n_press,
							x, y);
  }else if(selected_clear){
    tempo_edit->mode = AGS_TEMPO_EDIT_DELETE_MARKER;

    //NOTE:JK: only takes action on release
  }else if(selected_select){
    tempo_edit->mode = AGS_TEMPO_EDIT_SELECT_MARKER;

    ags_tempo_edit_drawing_area_button_press_select_marker((GtkWidget *) composite_editor,
							   (GtkWidget *) composite_toolbar,
							   tempo_edit,
							   n_press,
							   x, y);
  }

  gtk_widget_queue_draw((GtkWidget *) tempo_edit->drawing_area);
  
  return(FALSE);
}

gboolean
ags_tempo_edit_gesture_click_released_callback(GtkGestureClick *event_controller,
					       gint n_press,
					       gdouble x,
					       gdouble y,
					       AgsTempoEdit *tempo_edit)
{
  AgsCompositeToolbar *composite_toolbar;
  AgsCompositeEditor *composite_editor;
  
  gboolean selected_position_cursor, selected_edit, selected_clear, selected_select;
  
  selected_position_cursor = FALSE;
  selected_edit = FALSE;
  selected_clear = FALSE;
  selected_select = FALSE;
    
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) tempo_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
  
  composite_toolbar = composite_editor->toolbar;
    
  selected_position_cursor = (composite_toolbar->selected_tool == (GtkWidget *) composite_toolbar->position) ? TRUE: FALSE;
  selected_edit = (composite_toolbar->selected_tool == (GtkWidget *) composite_toolbar->edit) ? TRUE: FALSE;
  selected_clear = (composite_toolbar->selected_tool == (GtkWidget *) composite_toolbar->clear) ? TRUE: FALSE;
  selected_select = (composite_toolbar->selected_tool == (GtkWidget *) composite_toolbar->select) ? TRUE: FALSE;

  tempo_edit->button_mask &= (~AGS_TEMPO_EDIT_BUTTON_1);
    
  if(selected_position_cursor){
    ags_tempo_edit_drawing_area_button_release_position_cursor((GtkWidget *) composite_editor,
							       (GtkWidget *) composite_toolbar,
							       tempo_edit,
							       n_press,
							       x, y);
      
    //      tempo_edit->mode = AGS_TEMPO_EDIT_NO_EDIT_MODE;
  }else if(selected_edit){
    ags_tempo_edit_drawing_area_button_release_add_marker((GtkWidget *) composite_editor,
							  (GtkWidget *) composite_toolbar,
							  tempo_edit,
							  n_press,
							  x, y);

    tempo_edit->mode = AGS_TEMPO_EDIT_NO_EDIT_MODE;
  }else if(selected_clear){
    ags_tempo_edit_drawing_area_button_release_delete_marker((GtkWidget *) composite_editor,
							     (GtkWidget *) composite_toolbar,
							     tempo_edit,
							     n_press,
							     x, y);

    tempo_edit->mode = AGS_TEMPO_EDIT_NO_EDIT_MODE;
  }else if(selected_select){
    ags_tempo_edit_drawing_area_button_release_select_marker((GtkWidget *) composite_editor,
							     (GtkWidget *) composite_toolbar,
							     tempo_edit,
							     n_press,
							     x, y);

    tempo_edit->mode = AGS_TEMPO_EDIT_NO_EDIT_MODE;
  }

  gtk_widget_queue_draw((GtkWidget *) tempo_edit->drawing_area);

  return(FALSE);
}

void
ags_tempo_edit_realize(GtkWidget *widget)
{
  GdkFrameClock *frame_clock;
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_tempo_edit_parent_class)->realize(widget);

#if 0
  frame_clock = gtk_widget_get_frame_clock(widget);
  
  g_signal_connect(frame_clock, "update", 
		   G_CALLBACK(ags_tempo_edit_frame_clock_update_callback), widget);

  gdk_frame_clock_begin_updating(frame_clock);
#endif
}

void
ags_tempo_edit_unrealize(GtkWidget *widget)
{
  GdkFrameClock *frame_clock;
  
#if 0
  frame_clock = gtk_widget_get_frame_clock(widget);
  
  g_object_disconnect(frame_clock,
		      "any_signal::update", 
		      G_CALLBACK(ags_tempo_edit_frame_clock_update_callback),
		      widget,
		      NULL);

  gdk_frame_clock_end_updating(frame_clock);
#endif

  /* call parent */
  GTK_WIDGET_CLASS(ags_tempo_edit_parent_class)->unrealize(widget);
}

void
ags_tempo_edit_measure(GtkWidget *widget,
		       GtkOrientation orientation,
		       int for_size,
		       int *minimum,
		       int *natural,
		       int *minimum_baseline,
		       int *natural_baseline)
{
  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();
  
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
ags_tempo_edit_size_allocate(GtkWidget *widget,
			     int width,
			     int height,
			     int baseline)
{
  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  width = -1;
  height = (gint) (gui_scale_factor * AGS_SCALE_DEFAULT_HEIGHT_REQUEST);
  
  GTK_WIDGET_CLASS(ags_tempo_edit_parent_class)->size_allocate(widget,
							       width,
							       height,
							       baseline);
}

void
ags_tempo_edit_show(GtkWidget *widget)
{
  AgsTempoEdit *tempo_edit;

  tempo_edit = AGS_TEMPO_EDIT(widget);

  /* call parent */
  GTK_WIDGET_CLASS(ags_tempo_edit_parent_class)->show(widget);

  ags_tempo_edit_reset_vscrollbar(tempo_edit);
  ags_tempo_edit_reset_hscrollbar(tempo_edit);
}

void
ags_tempo_edit_frame_clock_update_callback(GdkFrameClock *frame_clock,
					   AgsTempoEdit *tempo_edit)
{
  gtk_widget_queue_draw((GtkWidget *) tempo_edit);
}

void
ags_tempo_edit_reset_vscrollbar(AgsTempoEdit *tempo_edit)
{
  AgsCompositeEdit *composite_edit;
  GtkAdjustment *adjustment;

  GtkAllocation allocation;
  
  double varea_height;
  gdouble upper, old_upper;
  
  if(!AGS_IS_TEMPO_EDIT(tempo_edit) ||
     (AGS_TEMPO_EDIT_BLOCK_RESET_VSCROLLBAR & (tempo_edit->flags)) != 0){
    return;
  }

  tempo_edit->flags |= AGS_TEMPO_EDIT_BLOCK_RESET_VSCROLLBAR;

  composite_edit = (AgsCompositeEdit *) gtk_widget_get_ancestor((GtkWidget *) tempo_edit,
								AGS_TYPE_COMPOSITE_EDIT);

  /* */
  gtk_widget_get_allocation(GTK_WIDGET(tempo_edit->drawing_area),
			    &allocation);
  
  /* adjustment */
  adjustment = gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar);

  /* upper */
  old_upper = gtk_adjustment_get_upper(adjustment); 

  varea_height = tempo_edit->control_height;
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

  tempo_edit->flags &= (~AGS_TEMPO_EDIT_BLOCK_RESET_VSCROLLBAR);  
}

void
ags_tempo_edit_reset_hscrollbar(AgsTempoEdit *tempo_edit)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;

  GtkAdjustment *adjustment;

  AgsApplicationContext *application_context;
  
  GtkAllocation allocation;

  gdouble gui_scale_factor;
  double zoom_factor, zoom;
  double zoom_correction;
  guint map_width;
  gdouble upper, old_upper;
  
  if(!AGS_IS_TEMPO_EDIT(tempo_edit) ||
     (AGS_TEMPO_EDIT_BLOCK_RESET_HSCROLLBAR & (tempo_edit->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();

  tempo_edit->flags |= AGS_TEMPO_EDIT_BLOCK_RESET_HSCROLLBAR;

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_widget_get_allocation(GTK_WIDGET(tempo_edit->drawing_area),
			    &allocation);

  /* adjustment */
  adjustment = gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar);

  /* zoom */
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) tempo_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  composite_toolbar = composite_editor->toolbar;

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom) - 2.0);
  
  /* upper */
  old_upper = gtk_adjustment_get_upper(adjustment);
  
  zoom_correction = 1.0 / 16;

  map_width = ((64.0) * (16.0 * 16.0 * 1200.0) * zoom * zoom_correction);
  upper = map_width - allocation.width;

  if(upper < 0.0){    
    upper = 0.0;
  }

  gtk_adjustment_set_upper(adjustment,
			   upper);

  /* ruler */
  tempo_edit->ruler->factor = zoom_factor;
  tempo_edit->ruler->precision = zoom;
  tempo_edit->ruler->scale_precision = 1.0 / zoom;

  gtk_adjustment_set_upper(tempo_edit->ruler->adjustment,
			   upper);

  /* reset value */
  if(old_upper != 0.0){
    gtk_adjustment_set_value(adjustment,
			     gtk_adjustment_get_value(adjustment) / old_upper * upper);
  }

  tempo_edit->flags &= (~AGS_TEMPO_EDIT_BLOCK_RESET_HSCROLLBAR);  
}

gint
ags_tempo_edit_compare_x_offset_func(gconstpointer a,
				     gconstpointer b,
				     AgsTempoEdit *tempo_edit,
				     gdouble x_offset)
{
  AgsTimestamp *timestamp_a, *timestamp_b;

  guint64 current_offset;
  guint64 a_offset, b_offset;

  gint retval;

  timestamp_a = ags_program_get_timestamp(a);
  timestamp_b = ags_program_get_timestamp(b);
  
  current_offset = (guint64) ((double) AGS_PROGRAM_DEFAULT_OFFSET * floor(x_offset / AGS_PROGRAM_DEFAULT_OFFSET));

  a_offset = ags_timestamp_get_ags_offset(timestamp_a);
  b_offset = ags_timestamp_get_ags_offset(timestamp_b);

  g_object_unref(timestamp_a);
  g_object_unref(timestamp_b);

  retval = 0;
  
  if(a_offset == current_offset){
    if(b_offset == current_offset){
      retval = 0; //NOTE:JK: argh - not unique
    }else{
      retval = 0;
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
  
  return(retval);
}

GList*
ags_tempo_edit_find_first_drawn_func(AgsTempoEdit *tempo_edit,
				     GList *tempo)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;

  GtkAdjustment *adjustment;

  AgsApplicationContext *application_context;
  
  GList *tempo_last;
  GList *a_list, *b_list, *c_list;
  GList *retval;
  
  gint tempo_length;
  gint bisect_steps;
  gdouble zoom_factor;
  gdouble x_offset;
  gint nth_bisect;
  
  application_context = ags_application_context_get_instance();

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  tempo_edit = composite_editor->tempo_edit;

  adjustment = gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar);

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));
  
  x_offset = (guint64) (AGS_PROGRAM_DEFAULT_OFFSET * floor(((zoom_factor * gtk_adjustment_get_value(adjustment))) / (double) AGS_PROGRAM_DEFAULT_OFFSET));

  retval = tempo;
  
  tempo_last = g_list_last(tempo);
  tempo_length = g_list_length(tempo);

  if(tempo == tempo_last){
    return(tempo);
  }
  
  a_list = tempo;
  b_list = g_list_nth(tempo,
		      (guint) floor((double) tempo_length / 2.0));
  c_list = tempo_last;
  
  bisect_steps = (guint) floor((tempo_length) / 2.0);
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
      cmp_val_0 = ags_tempo_edit_compare_x_offset_func(a_list->data,
						       b_list->data,
						       tempo_edit,
						       x_offset);
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
      cmp_val_1 = ags_tempo_edit_compare_x_offset_func(b_list->data,
						       c_list->data,
						       tempo_edit,
						       x_offset);
    }else{
      break;
    }

    if(cmp_val_1 <= 0){
      retval = b_list;
    }

    nth_bisect++;

    if(retval == a_list){
      if(cmp_val_0 == 0){
	break;
      }

      tempo_length = g_list_position(a_list, b_list) + 1;
      bisect_steps = (gint) floor((double) (tempo_length) / 2.0);

      c_list = b_list->prev;

      b_list = g_list_nth(a_list,
			  bisect_steps);

      a_list = a_list->next;
    }else if(retval == b_list){
      if(cmp_val_1 == 0){
	break;
      }

      tempo_length = g_list_position(a_list, b_list) + 1;
      bisect_steps = (gint) floor((double) (tempo_length) / 2.0);

      a_list = b_list->next;

      b_list = g_list_nth(b_list,
			  bisect_steps);
    }
  }
  
  return(retval);
}

GList*
ags_tempo_edit_find_last_drawn_func(AgsTempoEdit *tempo_edit,
				    GList *tempo)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;

  GtkAdjustment *adjustment;

  AgsApplicationContext *application_context;
  
  GList *tempo_last;
  GList *a_list, *b_list, *c_list;
  GList *retval;

  guint width;
  gint tempo_length;
  gint bisect_steps;
  gdouble zoom_factor;
  gdouble x_offset;
  gint nth_bisect;
  
  application_context = ags_application_context_get_instance();

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  composite_toolbar = composite_editor->toolbar;

  tempo_edit = composite_editor->tempo_edit;

  adjustment = gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar);

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));
  
  width = gtk_widget_get_width((GtkWidget *) tempo_edit->drawing_area);

  x_offset = (guint64) (AGS_PROGRAM_DEFAULT_OFFSET * floor((zoom_factor * (gtk_adjustment_get_value(adjustment) + (double) width)) / (double) AGS_PROGRAM_DEFAULT_OFFSET));

  retval = tempo;
  
  tempo_last = g_list_last(tempo);
  tempo_length = g_list_length(tempo);

  if(tempo == tempo_last){
    return(tempo);
  }
  
  a_list = tempo;
  b_list = g_list_nth(tempo,
		      (guint) floor((double) tempo_length / 2.0));
  c_list = tempo_last;
  
  bisect_steps = (guint) floor((tempo_length - 1) / 2.0);
  nth_bisect = 0;
  
  while(bisect_steps > 0){
    gint cmp_val_0, cmp_val_1;

    cmp_val_0 = 0;
    cmp_val_1 = 0;
    
    if(a_list != NULL &&
       b_list != NULL){
      cmp_val_0 = ags_tempo_edit_compare_x_offset_func(a_list->data,
						       b_list->data,
						       tempo_edit,
						       x_offset);
    }else{
      break;
    }

    retval = a_list;
    
    if(cmp_val_0 == 0 ||
       cmp_val_0 < 0){
      retval = b_list;
    }
    
    if(b_list != NULL &&
       c_list != NULL){
      cmp_val_1 = ags_tempo_edit_compare_x_offset_func(b_list->data,
						       c_list->data,
						       tempo_edit,
						       x_offset);
    }else{
      break;
    }

    if(cmp_val_1 == 0 ||
       cmp_val_1 < 0){
      retval = c_list;
    }

    nth_bisect++;

    if(retval == a_list){
      tempo_length = g_list_position(a_list, b_list) + 1;
      bisect_steps = (gint) floor((double) (tempo_length - 1) / 2.0);
      
      c_list = b_list->prev;

      b_list = g_list_nth(a_list,
			  bisect_steps);
    }else if(retval == b_list){
      tempo_length = g_list_position(a_list, b_list) + 1;
      bisect_steps = (gint) floor((double) (tempo_length - 1) / 2.0);

      a_list = b_list;

      b_list = g_list_nth(a_list,
			  bisect_steps);
    }else if(retval == c_list){
      break;
    }
  }
  
  return(retval);
}

void
ags_tempo_edit_draw_segment(AgsTempoEdit *tempo_edit, cairo_t *cr)
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
  
  if(!AGS_IS_TEMPO_EDIT(tempo_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_widget_get_allocation(GTK_WIDGET(tempo_edit->drawing_area),
			    &allocation);

  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) tempo_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);

  toolbar = composite_editor->toolbar;

  tact = exp2((double) gtk_combo_box_get_active(toolbar->zoom) - 2.0);
  
  /* dimension and offset */
  width = (gdouble) allocation.width;
  height = (gdouble) allocation.height;

  hscrollbar_adjustment = gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar);
  
  x_offset = gtk_adjustment_get_value(hscrollbar_adjustment);

  y = 0.0;

  style_context = gtk_widget_get_style_context((GtkWidget *) tempo_edit);

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

  control_width = (gint) (gui_scale_factor * (gdouble) AGS_TEMPO_EDIT_DEFAULT_CONTROL_WIDTH) * (tact / (gui_scale_factor * tact));
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
      goto ags_tempo_edit_draw_segment0;
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
    ags_tempo_edit_draw_segment0:
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
ags_tempo_edit_draw_position(AgsTempoEdit *tempo_edit, cairo_t *cr)
{
  AgsCompositeEditor *composite_editor;
  
  GtkStyleContext *style_context;
  GtkSettings *settings;

  AgsApplicationContext *application_context;

  GtkAllocation allocation;

  GdkRGBA fg_color;

  double zoom_factor;
  double position;
  double x, y;
  double width, height;
  gboolean height_fits;
  gboolean dark_theme;
  gboolean fg_success;

  GValue value = G_VALUE_INIT;

  if(!AGS_IS_TEMPO_EDIT(tempo_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();    

  gtk_widget_get_allocation(GTK_WIDGET(tempo_edit->drawing_area),
			    &allocation);

  style_context = gtk_widget_get_style_context((GtkWidget *) tempo_edit->drawing_area);

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
  
  /*  */
  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));
  
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(composite_editor->toolbar)->zoom));

  /* get offset and dimensions */
  position = ((double) tempo_edit->note_offset) * ((double) tempo_edit->control_width) / zoom_factor;
  
  y = 0.0;
  x = (position) - (gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar)));

  height = (double) allocation.height;
  width = (double) AGS_TEMPO_EDIT_DEFAULT_FADER_WIDTH;

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
      
  //  cairo_surface_mark_dirty(cairo_get_target(cr));
}

void
ags_tempo_edit_draw_cursor(AgsTempoEdit *tempo_edit, cairo_t *cr)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *toolbar;

  GtkStyleContext *style_context;
  GtkSettings *settings;

  GtkAllocation allocation;
  
  GdkRGBA fg_color;

  double zoom_factor;
  double x, y;
  double width, height;
  gboolean dark_theme;
  gboolean fg_success;

  GValue value = G_VALUE_INIT;

  if(!AGS_IS_TEMPO_EDIT(tempo_edit)){
    return;
  }

  gtk_widget_get_allocation(GTK_WIDGET(tempo_edit->drawing_area),
			    &allocation);

  /* style context */
  style_context = gtk_widget_get_style_context((GtkWidget *) tempo_edit->drawing_area);

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
  
  /* zoom */
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) tempo_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  toolbar = composite_editor->toolbar;

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) toolbar->zoom));

  /* get offset */
  x = ((double) tempo_edit->cursor_position_x * (double) tempo_edit->control_width) - (gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar)) * zoom_factor);
  y = ((double) tempo_edit->cursor_position_y * (double) tempo_edit->control_height) - gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar));

  width = (double) tempo_edit->control_width;
  height = (double) tempo_edit->control_height;

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
      
  //  cairo_surface_mark_dirty(cairo_get_target(cr));
}

void
ags_tempo_edit_draw_selection(AgsTempoEdit *tempo_edit, cairo_t *cr)
{
  GtkStyleContext *style_context;
  GtkSettings *settings;

  GdkRGBA fg_color;

  GtkAllocation allocation;
  
  double x, y;
  double width, height;
  gboolean dark_theme;
  gboolean fg_success;

  GValue value = G_VALUE_INIT;

  if(!AGS_IS_TEMPO_EDIT(tempo_edit)){
    return;
  }

  gtk_widget_get_allocation(GTK_WIDGET(tempo_edit->drawing_area),
			    &allocation);

  style_context = gtk_widget_get_style_context((GtkWidget *) tempo_edit->drawing_area);

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

  gtk_widget_get_allocation(GTK_WIDGET(tempo_edit->drawing_area),
			    &allocation);

  /* get offset and dimensions */
  if(tempo_edit->selection_x0 < tempo_edit->selection_x1){
    x = ((double) tempo_edit->selection_x0) - gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar));
    width = ((double) tempo_edit->selection_x1 - (double) tempo_edit->selection_x0);
  }else{
    x = ((double) tempo_edit->selection_x1) - gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar));
    width = ((double) tempo_edit->selection_x0 - (double) tempo_edit->selection_x1);
  }

  if(tempo_edit->selection_y0 < tempo_edit->selection_y1){
    y = ((double) tempo_edit->selection_y0) - gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar));
    height = ((double) tempo_edit->selection_y1 - (double) tempo_edit->selection_y0);
  }else{
    y = ((double) tempo_edit->selection_y1) - gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar));
    height = ((double) tempo_edit->selection_y0 - (double) tempo_edit->selection_y1);
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
      
  //  cairo_surface_mark_dirty(cairo_get_target(cr));
}

void
ags_tempo_edit_draw_marker(AgsTempoEdit *tempo_edit,
			   AgsMarker *marker_a, AgsMarker *marker_b,
			   cairo_t *cr,
			   gdouble opacity)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;

  GtkStyleContext *style_context;
  GtkSettings *settings;

  AgsApplicationContext *application_context;

  GtkAllocation allocation;

  GdkRGBA fg_color;
  GdkRGBA shadow_color;

  gdouble gui_scale_factor;
  double zoom, zoom_factor;
  double viewport_x, viewport_y;
  gdouble val, step;
  gdouble upper, lower, step_count;
  gdouble c_range;
  gint x, y;
  gint a_x, b_x;
  gdouble a_y, b_y;
  double width, height;
  gboolean dark_theme;
  gboolean fg_success;
  gboolean shadow_success;

  GValue value = {0};
  
  if(!AGS_IS_TEMPO_EDIT(tempo_edit) ||
     !AGS_IS_MARKER(marker_a) ||
     cr == NULL){
    return;
  }

  application_context = ags_application_context_get_instance();
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) tempo_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  composite_toolbar = composite_editor->toolbar;

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom) - 2.0);
   
  /* style context */
  style_context = gtk_widget_get_style_context((GtkWidget *) tempo_edit);

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
  
  gtk_widget_get_allocation(GTK_WIDGET(tempo_edit->drawing_area),
			    &allocation);

  c_range = 240.0;
  
  /* get offset and dimensions */
  if(AGS_PROGRAM_DEFAULT_LENGTH > allocation.width){
    viewport_x = zoom_factor * gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar));
  }else{
    viewport_x = 0.0;
  }
  
  viewport_y = gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->vscrollbar));

  g_object_get(marker_a,
	       "x", &a_x,
	       "y", &a_y,
	       NULL);
 
  x = ((double) a_x) - viewport_x;

  lower = 0.0;
  upper = 240.0;

  a_y -= lower;
  
  y = allocation.height - ((double) a_y / c_range) * allocation.height - viewport_y;
  
  if(marker_b != NULL){
    g_object_get(marker_b,
		 "x", &b_x,
		 NULL);
    
    width = ((double) b_x - a_x);
  }else{
    width = ((double) allocation.width) - x;
  }

  height = allocation.height - y;
  
  /* apply zoom */
  x /= zoom_factor;
  
  width /= zoom_factor;

  /* clip */
  if(x < 0.0){
    if(x + width < 0.0){
      return;
    }else{
      width += x;
      x = 0.0;
    }
  }else if(x > allocation.width){
    return;
  }

  if(x + width > allocation.width){
    width = ((double) allocation.width) - x;
  }

  if(marker_b == NULL){
    width = ((double) allocation.width - x);
  }
  
  if(y < 0.0){
    if(y + height < 0.0){
      return;
    }else{
      height += y;
      y = 0.0;
    }
  }else if(y > allocation.height){
    return;
  }

  if(y + height > allocation.height){
    height = ((double) allocation.height) - y;
  }

  /* draw marker - dot */
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.blue,
			fg_color.green,
			opacity * fg_color.alpha);
  
  cairo_arc(cr,
	    x, y,
	    tempo_edit->point_radius,
	    0.0,
	    2.0 * M_PI);
  
  cairo_stroke(cr);
  
  /* draw marker - area */
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.blue,
			fg_color.green,
			opacity * fg_color.alpha);
  cairo_rectangle(cr,
		  x, y,
		  width, height);
  cairo_fill(cr);

  /* check marker selected */
  if(ags_marker_test_flags(marker_a, AGS_MARKER_IS_SELECTED)){
    double selected_x, selected_y;
    double selected_width, selected_height;

    selected_x = x - tempo_edit->selected_marker_border;
    selected_y = y - tempo_edit->selected_marker_border;

    selected_width = width + (2.0 * (double) tempo_edit->selected_marker_border);
    selected_height = height + (2.0 * (double) tempo_edit->selected_marker_border);

    /* clip */
    if(selected_x < 0.0){
      selected_x = 0.0;
    }
    
    if(selected_x + selected_width > allocation.width){
      selected_width = ((double) allocation.width) - selected_x;
    }
  
    if(selected_y < 0.0){
      selected_y = 0.0;
    }

    if(selected_y + selected_height > allocation.height){
      selected_height = ((double) allocation.height) - selected_y;
    }

    /* draw selected marker - dot */
    cairo_set_source_rgba(cr,
			  shadow_color.red,
			  shadow_color.blue,
			  shadow_color.green,
			  opacity / 3.0);
    
    cairo_arc(cr,
	      selected_x, selected_y,
	      tempo_edit->point_radius + (2.0 * (double) tempo_edit->selected_marker_border),
	      0.0,
	      2.0 * M_PI);

    cairo_stroke(cr);

    /* draw selected marker - area */
    cairo_rectangle(cr,
		    selected_x, selected_y,
		    selected_width, selected_height);
    cairo_fill(cr);
  }
}

void
ags_tempo_edit_draw_tempo(AgsTempoEdit *tempo_edit, cairo_t *cr)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;

  AgsMarker *first_match;
  AgsMarker *last_match;

  AgsTimestamp *timestamp;
  AgsTimestamp *current_timestamp;    
  
  AgsApplicationContext *application_context;

  GtkAllocation allocation;

  GList *start_list, *list;
  GList *start_list_marker, *list_marker;
    
  GList *first_drawn;
  GList *last_drawn;
  GList *first_start_marker, *first_marker;
  GList *last_start_marker, *last_marker;

  gchar *control_name;

  gdouble opacity;
  guint x0, x1;
  guint offset;
  
  if(!AGS_IS_TEMPO_EDIT(tempo_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  gtk_widget_get_allocation(GTK_WIDGET(tempo_edit->drawing_area),
			    &allocation);
    
  /* zoom */  
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) tempo_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);

  composite_toolbar = composite_editor->toolbar;

  opacity = gtk_spin_button_get_value(composite_toolbar->opacity);  

  /* get visisble region */
  x0 = gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar));
  x1 = (gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(tempo_edit->hscrollbar)) + allocation.width);

  /* draw tempo */
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;
  
  timestamp->timer.ags_offset.offset = (guint64) AGS_PROGRAM_DEFAULT_OFFSET * floor((double) x0 / (double) AGS_PROGRAM_DEFAULT_OFFSET);

  list = 
    start_list = ags_sound_provider_get_tempo(AGS_SOUND_PROVIDER(application_context));
  
  first_drawn = ags_tempo_edit_find_first_drawn_func(tempo_edit,
						     start_list);

  last_drawn = ags_tempo_edit_find_last_drawn_func(tempo_edit,
						   start_list);
    
  first_match = NULL;
    
  first_marker =
    first_start_marker = NULL;
    
  if(first_drawn != NULL){
    first_marker =
      first_start_marker = ags_program_get_marker(first_drawn->data);
  }

  if(first_marker != NULL){
    first_match = first_marker->data;
  }
    
  while(first_marker != NULL){
    if(ags_marker_get_x(first_marker->data) >= x0){
      goto ags_tempo_edit_draw_tempo_LOOP_FIRST_END;
    }
      
    first_match = first_marker->data;

    first_marker = first_marker->next;
  }

 ags_tempo_edit_draw_tempo_LOOP_FIRST_END:

  if(first_marker != NULL){
    first_marker = first_marker->next;
  }
    
  last_match = NULL;
    
  last_marker =
    last_start_marker = NULL;

  if(last_drawn != NULL){
    last_marker =
      last_start_marker = ags_program_get_marker(last_drawn->data);
  }

  if(last_marker != NULL){
    last_match = last_marker->data;
  }
    
  while(last_marker != NULL){
    if(ags_marker_get_x(last_marker->data) >= x1){
      goto ags_tempo_edit_draw_tempo_LOOP_LAST_END;
    }
      
    last_match = last_marker->data;

    last_marker = last_marker->next;
  }

 ags_tempo_edit_draw_tempo_LOOP_LAST_END:

  if(first_match != NULL &&
     first_match != last_match){
    GList *start_next_marker;
    GList *next_link;

    next_link = first_marker;
    start_next_marker = NULL;
      
    if(next_link == NULL &&
       first_drawn != NULL){
      GList *tmp_list;

      tmp_list = first_drawn->next;

      while(tmp_list != NULL){
	control_name = NULL;
      
	g_object_get(tmp_list->data,
		     "control-name", &control_name,
		     NULL);
	    
	if((!g_strcmp0(control_name,
		       "tempo"))){
	  start_next_marker = ags_program_get_marker(tmp_list->data);
	  next_link = start_next_marker;

	  goto ags_tempo_edit_draw_tempo_FIRST_MATCH;
	}
	    
	tmp_list = tmp_list->next;
      }
    }

  ags_tempo_edit_draw_tempo_FIRST_MATCH:

    control_name = NULL;
      
    g_object_get(first_drawn->data,
		 "control-name", &control_name,
		 NULL);

    if(!g_strcmp0(control_name,
		  "tempo")){
      ags_tempo_edit_draw_marker(tempo_edit,
				 first_match, ((next_link != NULL) ? next_link->data: NULL),
				 cr,
				 opacity);

    }
      
    g_list_free_full(start_next_marker,
		     g_object_unref);
  }
  
  while((list = ags_program_find_near_timestamp(list,
						timestamp)) != NULL){
    AgsProgram *tempo;

    GList *next_link;
    
    tempo = AGS_PROGRAM(list->data);

    current_timestamp = NULL;

    control_name = NULL;

    g_object_get(list_marker->data,
		 "timestamp", &current_timestamp,
		 "control-name", &control_name,
		 NULL);

    start_list_marker = NULL;

    if((!g_strcmp0(control_name,
		   "tempo")) == FALSE){
      goto ags_tempo_edit_draw_tempo_INNER_LOOP_END;
    }

    if(ags_timestamp_get_ags_offset(current_timestamp) >= x1){
      goto ags_tempo_edit_draw_tempo_INNER_LOOP_END;
    }

    list_marker =
      start_list_marker = ags_program_get_marker(tempo);

    while(list_marker != NULL){
      GList *start_next_marker;

      start_next_marker = NULL;
      next_link = list_marker->next;

      if(next_link == NULL){
	GList *tmp_list;

	tmp_list = list->next;

	while(tmp_list != NULL){
	  control_name = NULL;
      
	  g_object_get(tmp_list->data,
		       "control-name", &control_name,
		       NULL);
	    
	  if((!g_strcmp0(control_name,
			 "tempo"))){
	    goto ags_tempo_edit_draw_tempo_FIND_FIRST_NEXT_LINK_END;
	  }
	    
	  tmp_list = tmp_list->next;
	}

      ags_tempo_edit_draw_tempo_FIND_FIRST_NEXT_LINK_END:
	  
	if(tmp_list != NULL){
	  start_next_marker = ags_program_get_marker(tmp_list->data);
	  next_link = start_next_marker;
	}
      }
    }            

#if 0
    g_message("found marker[%d]", AGS_MARKER(list_marker->data)->x);
#endif

    while(list_marker != NULL){	
      if(list_marker->data != first_match &&
	 list_marker->data != last_match &&
	 next_link != NULL){
	ags_tempo_edit_draw_marker(tempo_edit,
				   list_marker->data, ((list_marker->next != NULL) ? list_marker->next->data: NULL),
				   cr,
				   opacity);
      }
      
      /* iterate */
      list_marker = list_marker->next;
    }

  ags_tempo_edit_draw_tempo_INNER_LOOP_END:

    g_object_unref(current_timestamp);

    g_list_free_full(start_list_marker,
		     g_object_unref);
      
    /* iterate */
    list = list->next;
  }

  if(last_match != NULL){
    GList *start_next_marker;
    GList *next_link;

    next_link = last_marker;
    start_next_marker = NULL;
      
    if(next_link == NULL &&
       list != NULL){
      GList *tmp_list;

      tmp_list = list->next;

      while(tmp_list != NULL){
	control_name = NULL;
      
	g_object_get(tmp_list->data,
		     "control-name", &control_name,
		     NULL);
	    
	if(!g_strcmp0(control_name,
		      "tempo")){
	  start_next_marker = ags_program_get_marker(tmp_list->data);
	  next_link = start_next_marker;

	  goto ags_tempo_edit_draw_tempo_LAST_MATCH;
	}
	    
	tmp_list = tmp_list->next;
      }
    }
      
  ags_tempo_edit_draw_tempo_LAST_MATCH:
      
    ags_tempo_edit_draw_marker(tempo_edit,
			       last_match, ((next_link != NULL) ? next_link->data: NULL),
			       cr,
			       opacity);

    g_list_free_full(start_next_marker,
		     g_object_unref);
  }
  
  g_list_free_full(start_list,
		   g_object_unref);
}

void
ags_tempo_edit_draw(AgsTempoEdit *tempo_edit, cairo_t *cr)
{
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  ags_tempo_edit_reset_vscrollbar(tempo_edit);
  ags_tempo_edit_reset_hscrollbar(tempo_edit);

  /* segment */
  ags_tempo_edit_draw_segment(tempo_edit, cr);

  /* tempo */
  ags_tempo_edit_draw_tempo(tempo_edit, cr);
  
  /* edit mode */
  switch(tempo_edit->mode){
  case AGS_TEMPO_EDIT_POSITION_CURSOR:
    {
      ags_tempo_edit_draw_cursor(tempo_edit, cr);
    }
    break;
  case AGS_TEMPO_EDIT_ADD_MARKER:
    {
      if(tempo_edit->current_marker != NULL){
	ags_tempo_edit_draw_marker(tempo_edit,
				   tempo_edit->current_marker, NULL,
				   cr,
				   1.0);

	cairo_surface_mark_dirty(cairo_get_target(cr));
      }
    }
    break;
  case AGS_TEMPO_EDIT_SELECT_MARKER:
    {
      ags_tempo_edit_draw_selection(tempo_edit, cr);
    }
    break;
  }

  /* fader */
  if((AGS_TEMPO_EDIT_AUTO_SCROLL & (tempo_edit->flags)) != 0){
    ags_tempo_edit_draw_position(tempo_edit, cr);
  }
}

/**
 * ags_tempo_edit_new:
 *
 * Create a new #AgsTempoEdit.
 *
 * Returns: a new #AgsTempoEdit
 * 
 * Since: 5.1.0
 */
AgsTempoEdit*
ags_tempo_edit_new()
{
  AgsTempoEdit *tempo_edit;

  tempo_edit = (AgsTempoEdit *) g_object_new(AGS_TYPE_TEMPO_EDIT,
					     NULL);

  return(tempo_edit);
}
