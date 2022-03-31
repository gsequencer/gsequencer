/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/editor/ags_wave_edit.h>
#include <ags/app/editor/ags_wave_edit_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_navigation.h>

#include <ags/app/editor/ags_wave_meta.h>

#include <gdk/gdkkeysyms.h>

#include <cairo.h>
#include <math.h>

void ags_wave_edit_class_init(AgsWaveEditClass *wave_edit);
void ags_wave_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_wave_edit_init(AgsWaveEdit *wave_edit);
void ags_wave_edit_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_wave_edit_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_wave_edit_finalize(GObject *gobject);

void ags_wave_edit_connect(AgsConnectable *connectable);
void ags_wave_edit_disconnect(AgsConnectable *connectable);

void ags_wave_edit_realize(GtkWidget *widget);

void ags_wave_edit_measure(GtkWidget *widget,
			   GtkOrientation orientation,
			   int for_size,
			   int *minimum,
			   int *natural,
			   int *minimum_baseline,
			   int *natural_baseline);
void ags_wave_edit_size_allocate(GtkWidget *widget,
				 int width,
				 int height,
				 int baseline);

void ags_wave_edit_show(GtkWidget *widget);

void ags_wave_edit_frame_clock_update_callback(GdkFrameClock *frame_clock,
					       AgsWaveEdit *wave_edit);

void ags_wave_edit_drawing_area_button_press_position_cursor(GtkWidget *editor,
							     GtkWidget *toolbar,
							     AgsWaveEdit *wave_edit,
							     AgsMachine *machine,
							     gint n_press,
							     gdouble x, gdouble y);
void ags_wave_edit_drawing_area_button_press_select_buffer(GtkWidget *editor,
							   GtkWidget *toolbar,
							   AgsWaveEdit *wave_edit,
							   AgsMachine *machine,
							   gint n_press,
							   gdouble x, gdouble y);

void ags_wave_edit_drawing_area_button_release_position_cursor(GtkWidget *editor,
							       GtkWidget *toolbar,
							       AgsWaveEdit *wave_edit,
							       AgsMachine *machine,
							       gint n_press, gdouble x, gdouble y);
void ags_wave_edit_drawing_area_button_release_select_buffer(GtkWidget *editor,
							     GtkWidget *toolbar,
							     AgsWaveEdit *wave_edit,
							     AgsMachine *machine,
							     gint n_press,
							     gdouble x, gdouble y);

gboolean ags_wave_edit_gesture_click_pressed_callback(GtkGestureClick *event_controller,
						      gint n_press,
						      gdouble x,
						      gdouble y,
						      AgsWaveEdit *wave_edit);
gboolean ags_wave_edit_gesture_click_released_callback(GtkGestureClick *event_controller,
						       gint n_press,
						       gdouble x,
						       gdouble y,
						       AgsWaveEdit *wave_edit);

gboolean ags_wave_edit_key_pressed_callback(GtkEventControllerKey *event_controller,
					    guint keyval,
					    guint keycode,
					    GdkModifierType state,
					    AgsWaveEdit *wave_edit);
gboolean ags_wave_edit_key_released_callback(GtkEventControllerKey *event_controller,
					     guint keyval,
					     guint keycode,
					     GdkModifierType state,
					     AgsWaveEdit *wave_edit);
gboolean ags_wave_edit_modifiers_callback(GtkEventControllerKey *event_controller,
					  GdkModifierType keyval,
					  AgsWaveEdit *wave_edit);

void ags_wave_edit_drawing_area_motion_notify_position_cursor(GtkWidget *editor,
							      GtkWidget *toolbar,
							      AgsWaveEdit *wave_edit,
							      AgsMachine *machine,
							      gdouble x, gdouble y);
void ags_wave_edit_drawing_area_motion_notify_select_buffer(GtkWidget *editor,
							    GtkWidget *toolbar,
							    AgsWaveEdit *wave_edit,
							    AgsMachine *machine,
							    gdouble x, gdouble y);

gboolean ags_wave_edit_motion_callback(GtkEventControllerMotion *event_controller,
				       gdouble x,
				       gdouble y,
				       AgsWaveEdit *wave_edit);

gboolean ags_wave_edit_auto_scroll_timeout(GtkWidget *widget);

/**
 * SECTION:ags_wave_edit
 * @short_description: edit audio data
 * @title: AgsWaveEdit
 * @section_id:
 * @include: ags/app/editor/ags_wave_edit.h
 *
 * The #AgsWaveEdit lets you edit audio data.
 */

static gpointer ags_wave_edit_parent_class = NULL;

GHashTable *ags_wave_edit_auto_scroll = NULL;

enum{
  PROP_0,
  PROP_LINE,
};

GType
ags_wave_edit_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_wave_edit = 0;

    static const GTypeInfo ags_wave_edit_info = {
      sizeof (AgsWaveEditClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_wave_edit_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsWaveEdit),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_wave_edit_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_wave_edit_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_wave_edit = g_type_register_static(GTK_TYPE_GRID,
						"AgsWaveEdit", &ags_wave_edit_info,
						0);
    
    g_type_add_interface_static(ags_type_wave_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_wave_edit);
  }

  return g_define_type_id__volatile;
}

void
ags_wave_edit_class_init(AgsWaveEditClass *wave_edit)
{
  GtkWidgetClass *widget;

  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_wave_edit_parent_class = g_type_class_peek_parent(wave_edit);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(wave_edit);

  gobject->set_property = ags_wave_edit_set_property;
  gobject->get_property = ags_wave_edit_get_property;

  gobject->finalize = ags_wave_edit_finalize;

  /* properties */
  /**
   * AgsWaveEdit:line:
   *
   * The wave edit's line.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("line",
				 "line",
				 "The line of wave edit",
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LINE,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) wave_edit;

  widget->realize = ags_wave_edit_realize;

  widget->measure = ags_wave_edit_measure;
  widget->size_allocate = ags_wave_edit_size_allocate;

  widget->show = ags_wave_edit_show;
}

void
ags_wave_edit_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_wave_edit_connect;
  connectable->disconnect = ags_wave_edit_disconnect;
}

void
ags_wave_edit_init(AgsWaveEdit *wave_edit)
{
  GtkEventController *event_controller;
  GtkAdjustment *adjustment;

  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();

  g_object_set(wave_edit,
	       "can-focus", FALSE,
	       "homogeneous", FALSE,
	       NULL);

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) wave_edit,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_wave_edit_key_pressed_callback), wave_edit);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_wave_edit_key_released_callback), wave_edit);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_wave_edit_modifiers_callback), wave_edit);

  event_controller = gtk_gesture_click_new();
  gtk_widget_add_controller((GtkWidget *) wave_edit,
			    event_controller);

  g_signal_connect(event_controller, "pressed",
		   G_CALLBACK(ags_wave_edit_gesture_click_pressed_callback), wave_edit);

  g_signal_connect(event_controller, "released",
		   G_CALLBACK(ags_wave_edit_gesture_click_released_callback), wave_edit);

  event_controller = gtk_event_controller_motion_new();
  gtk_widget_add_controller((GtkWidget *) wave_edit,
			    event_controller);

  g_signal_connect(event_controller, "motion",
		   G_CALLBACK(ags_wave_edit_motion_callback), wave_edit);

  wave_edit->flags = 0;
  wave_edit->connectable_flags = 0;
  wave_edit->mode = AGS_WAVE_EDIT_NO_EDIT_MODE;

  wave_edit->button_mask = 0;
  wave_edit->key_mask = 0;

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  wave_edit->line = 0;
  
  wave_edit->note_offset = 0;
  wave_edit->note_offset_absolute = 0;

  wave_edit->control_width = (guint) (gui_scale_factor * AGS_WAVE_EDIT_DEFAULT_CONTROL_WIDTH);
  wave_edit->control_height = (guint) (gui_scale_factor * AGS_WAVE_EDIT_DEFAULT_CONTROL_HEIGHT);
  
  wave_edit->cursor_position_x = AGS_WAVE_EDIT_DEFAULT_CURSOR_POSITION_X;
  wave_edit->cursor_position_y = AGS_WAVE_EDIT_DEFAULT_CURSOR_POSITION_Y;

  wave_edit->selected_buffer_border = AGS_WAVE_EDIT_DEFAULT_SELECTED_BUFFER_BORDER;
  
  wave_edit->selection_x0 = 0;
  wave_edit->selection_x1 = 0;
  wave_edit->selection_y0 = 0;
  wave_edit->selection_y1 = 0;

  wave_edit->ruler = ags_ruler_new(GTK_ORIENTATION_HORIZONTAL,
				   AGS_RULER_DEFAULT_STEP,
				   AGS_RULER_DEFAULT_FACTOR,
				   AGS_RULER_DEFAULT_PRECISION,
				   AGS_RULER_DEFAULT_SCALE_PRECISION);
  gtk_grid_attach(GTK_GRID(wave_edit),
		  (GtkWidget *) wave_edit->ruler,
		  0, 0,
		  1, 1);

  wave_edit->lower = AGS_WAVE_EDIT_DEFAULT_LOWER;
  wave_edit->upper = AGS_WAVE_EDIT_DEFAULT_UPPER;

  wave_edit->default_value = AGS_WAVE_EDIT_DEFAULT_VALUE;

  wave_edit->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_can_focus((GtkWidget *) wave_edit->drawing_area,
			   TRUE);
    
  gtk_widget_set_size_request((GtkWidget *) wave_edit->drawing_area,
			      -1, (gint) (gui_scale_factor * AGS_LEVEL_DEFAULT_HEIGHT_REQUEST));

  gtk_grid_attach(GTK_GRID(wave_edit),
		   (GtkWidget *) wave_edit->drawing_area,
		   0, 1,
		   1, 1);

  wave_edit->wave_data = NULL;
  wave_edit->stride = -1;
  
  /* vscrollbar */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, wave_edit->control_height, 1.0);
  wave_edit->vscrollbar = (GtkScrollbar *) gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL,
							     adjustment);
  g_object_set(wave_edit->vscrollbar,
	       "no-show-all", TRUE,
	       NULL);
  gtk_widget_set_size_request((GtkWidget *) wave_edit->vscrollbar,
			      -1, (gint) (gui_scale_factor * AGS_LEVEL_DEFAULT_HEIGHT_REQUEST));
  gtk_grid_attach(GTK_GRID(wave_edit),
		  (GtkWidget *) wave_edit->vscrollbar,
		  1, 1,
		  1, 1);

  /* hscrollbar */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (gdouble) wave_edit->control_width, 1.0);
  wave_edit->hscrollbar = (GtkScrollbar *) gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL,
							     adjustment);
  g_object_set(wave_edit->hscrollbar,
	       "no-show-all", TRUE,
	       NULL);
  gtk_widget_set_size_request((GtkWidget *) wave_edit->hscrollbar,
			      -1, -1);
  gtk_grid_attach(GTK_GRID(wave_edit),
		   (GtkWidget *) wave_edit->hscrollbar,
		   0, 2,
		   1, 1);

  /* auto-scroll */
  if(ags_wave_edit_auto_scroll == NULL){
    ags_wave_edit_auto_scroll = g_hash_table_new_full(g_direct_hash, g_direct_equal,
						      NULL,
						      NULL);
  }

  g_hash_table_insert(ags_wave_edit_auto_scroll,
		      wave_edit, ags_wave_edit_auto_scroll_timeout);
  g_timeout_add(1000 / 30, (GSourceFunc) ags_wave_edit_auto_scroll_timeout, (gpointer) wave_edit);
}

void
ags_wave_edit_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsWaveEdit *wave_edit;

  wave_edit = AGS_WAVE_EDIT(gobject);

  switch(prop_id){
  case PROP_LINE:
  {
    wave_edit->line = g_value_get_uint(value);

    gtk_widget_queue_draw((GtkWidget *) wave_edit);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wave_edit_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsWaveEdit *wave_edit;

  wave_edit = AGS_WAVE_EDIT(gobject);

  switch(prop_id){
  case PROP_LINE:
  {
    g_value_set_uint(value,
		     wave_edit->line);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wave_edit_finalize(GObject *gobject)
{
  AgsWaveEdit *wave_edit;
  
  wave_edit = AGS_WAVE_EDIT(gobject);
  
  /* remove auto scroll */
  g_hash_table_remove(ags_wave_edit_auto_scroll,
		      wave_edit);

  /* call parent */
  G_OBJECT_CLASS(ags_wave_edit_parent_class)->finalize(gobject);
}

void
ags_wave_edit_connect(AgsConnectable *connectable)
{
  AgsWaveEdit *wave_edit;

  wave_edit = AGS_WAVE_EDIT(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (wave_edit->connectable_flags)) != 0){
    return;
  }
  
  wave_edit->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  /* drawing area */
  gtk_drawing_area_set_draw_func(wave_edit->drawing_area,
				 ags_wave_edit_draw_callback,
				 wave_edit,
				 NULL);

  /* scrollbars */
  g_signal_connect_after((GObject *) gtk_scrollbar_get_adjustment(wave_edit->vscrollbar), "value-changed",
			 G_CALLBACK(ags_wave_edit_vscrollbar_value_changed), (gpointer) wave_edit);

  g_signal_connect_after((GObject *) gtk_scrollbar_get_adjustment(wave_edit->hscrollbar), "value-changed",
			 G_CALLBACK(ags_wave_edit_hscrollbar_value_changed), (gpointer) wave_edit);
}

void
ags_wave_edit_disconnect(AgsConnectable *connectable)
{
  AgsWaveEdit *wave_edit;

  wave_edit = AGS_WAVE_EDIT(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (wave_edit->connectable_flags)) == 0){
    return;
  }
  
  wave_edit->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  /* drawing area */
  gtk_drawing_area_set_draw_func(wave_edit->drawing_area,
				 NULL,
				 NULL,
				 NULL);

  /* scrollbars */
  g_object_disconnect((GObject *) gtk_scrollbar_get_adjustment(wave_edit->vscrollbar),
		      "any_signal::value-changed",
		      G_CALLBACK(ags_wave_edit_vscrollbar_value_changed),
		      (gpointer) wave_edit,
		      NULL);

  g_object_disconnect((GObject *) gtk_scrollbar_get_adjustment(wave_edit->hscrollbar),
		      "any_signal::value-changed",
		      G_CALLBACK(ags_wave_edit_hscrollbar_value_changed),
		      (gpointer) wave_edit,
		      NULL);
}

gboolean
ags_wave_edit_key_pressed_callback(GtkEventControllerKey *event_controller,
				   guint keyval,
				   guint keycode,
				   GdkModifierType state,
				   AgsWaveEdit *wave_edit)
{  
  GtkWidget *editor;
  AgsMachine *machine;

  AgsApplicationContext *application_context;

  gboolean key_handled;
  
  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R ){
    key_handled = FALSE;
  }else{
    key_handled = TRUE;
  }

  application_context = ags_application_context_get_instance();
  
  editor = gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
				   AGS_TYPE_COMPOSITE_EDITOR);
    
  machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;
  
  if(machine != NULL){
    switch(keyval){
    case GDK_KEY_Control_L:
      {
	wave_edit->key_mask |= AGS_WAVE_EDIT_KEY_L_CONTROL;
      }
      break;
    case GDK_KEY_Control_R:
      {
	wave_edit->key_mask |= AGS_WAVE_EDIT_KEY_R_CONTROL;
      }
      break;
    case GDK_KEY_Shift_L:
      {
	wave_edit->key_mask |= AGS_WAVE_EDIT_KEY_L_SHIFT;
      }
      break;
    case GDK_KEY_Shift_R:
      {
	wave_edit->key_mask |= AGS_WAVE_EDIT_KEY_R_SHIFT;
      }
      break;
    case GDK_KEY_a:
      {
	/* select all accelerations */
	if((AGS_WAVE_EDIT_KEY_L_CONTROL & (wave_edit->key_mask)) != 0 || (AGS_WAVE_EDIT_KEY_R_CONTROL & (wave_edit->key_mask)) != 0){
	  ags_composite_editor_select_all(editor);
	}
      }
      break;
    case GDK_KEY_c:
      {
	/* copy accelerations */
	if((AGS_WAVE_EDIT_KEY_L_CONTROL & (wave_edit->key_mask)) != 0 || (AGS_WAVE_EDIT_KEY_R_CONTROL & (wave_edit->key_mask)) != 0){
	  ags_composite_editor_copy(editor);
	}
      }
      break;
    case GDK_KEY_v:
      {
	/* paste accelerations */
	if((AGS_WAVE_EDIT_KEY_L_CONTROL & (wave_edit->key_mask)) != 0 || (AGS_WAVE_EDIT_KEY_R_CONTROL & (wave_edit->key_mask)) != 0){
	  ags_composite_editor_paste(editor);
	}
      }
      break;
    case GDK_KEY_x:
      {
	/* cut accelerations */
	if((AGS_WAVE_EDIT_KEY_L_CONTROL & (wave_edit->key_mask)) != 0 || (AGS_WAVE_EDIT_KEY_R_CONTROL & (wave_edit->key_mask)) != 0){
	  ags_composite_editor_cut(editor);
	}
      }
      break;
    case GDK_KEY_m:
      {
	/* meta */
	if((AGS_WAVE_EDIT_KEY_L_CONTROL & (wave_edit->key_mask)) != 0 || (AGS_WAVE_EDIT_KEY_R_CONTROL & (wave_edit->key_mask)) != 0){
	  AgsWaveMeta *wave_meta;

	  wave_meta = AGS_COMPOSITE_EDITOR(editor)->wave_edit->edit_meta;
	  
	  if((AGS_WAVE_META_ENABLED & (wave_meta->flags)) != 0){
	    wave_meta->flags &= (~AGS_WAVE_META_ENABLED);

	    gtk_widget_hide(wave_meta);
	  }else{
	    wave_meta->flags |= AGS_WAVE_META_ENABLED;

	    gtk_widget_show(wave_meta);

	    ags_wave_meta_refresh(wave_meta);
	  }
	}
      }
      break;
    }
  }
  
  return(key_handled);
}

gboolean
ags_wave_edit_key_released_callback(GtkEventControllerKey *event_controller,
				    guint keyval,
				    guint keycode,
				    GdkModifierType state,
				    AgsWaveEdit *wave_edit)
{  
  GtkWidget *editor;
  GtkWidget *toolbar;
  AgsMachine *machine;
  AgsNotebook *channel_selector;
  
  AgsApplicationContext *application_context;
  
  GtkAllocation allocation;

  double zoom_factor;
  gint i;
  gboolean key_handled;

  application_context = ags_application_context_get_instance();
  
  editor = gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
				   AGS_TYPE_COMPOSITE_EDITOR);

  channel_selector = AGS_COMPOSITE_EDITOR(editor)->wave_edit->channel_selector;
    
  machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;  
  
  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R ){
    key_handled = FALSE;
  }else{
    key_handled = TRUE;
  }
  
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(AGS_COMPOSITE_EDITOR(editor)->toolbar)->zoom));

  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);

  if(machine != NULL){    
    /* check key value */
    switch(keyval){
    case GDK_KEY_Control_L:
      {
	wave_edit->key_mask &= (~AGS_WAVE_EDIT_KEY_L_CONTROL);
      }
      break;
    case GDK_KEY_Control_R:
      {
	wave_edit->key_mask &= (~AGS_WAVE_EDIT_KEY_R_CONTROL);
      }
      break;
    case GDK_KEY_Shift_L:
      {
	wave_edit->key_mask &= (~AGS_WAVE_EDIT_KEY_L_SHIFT);
      }
      break;
    case GDK_KEY_Shift_R:
      {
	wave_edit->key_mask &= (~AGS_WAVE_EDIT_KEY_R_SHIFT);
      }
      break;
    case GDK_KEY_Left:
    case GDK_KEY_leftarrow:
      {
	gdouble x0_offset;

	/* position cursor */
	if(wave_edit->cursor_position_x > 0){
	  if(wave_edit->cursor_position_x - (zoom_factor * wave_edit->control_width) > 0){
	    wave_edit->cursor_position_x -= (zoom_factor * wave_edit->control_width);
	  }else{
	    wave_edit->cursor_position_x = 0;
	  }
	}

	x0_offset = wave_edit->cursor_position_x / zoom_factor;
      
	if(x0_offset / zoom_factor < gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar))){
	  gtk_range_set_value(GTK_RANGE(wave_edit->hscrollbar),
			      x0_offset / zoom_factor);
	}
      }
      break;
    case GDK_KEY_Right:
    case GDK_KEY_rightarrow:
      {
	gdouble x0_offset;
	  
	/* position cursor */      
	if(wave_edit->cursor_position_x < AGS_WAVE_DEFAULT_LENGTH){
	  wave_edit->cursor_position_x += (zoom_factor * wave_edit->control_width);
	}

	x0_offset = wave_edit->cursor_position_x / zoom_factor;
      
	if((x0_offset + wave_edit->control_width) / zoom_factor > gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) + allocation.width){
	  gtk_range_set_value(GTK_RANGE(wave_edit->hscrollbar),
			      x0_offset / zoom_factor);
	}
      }
      break;
    }

    gtk_widget_queue_draw((GtkWidget *) wave_edit);
  }
  
  return(key_handled);
}

gboolean
ags_wave_edit_modifiers_callback(GtkEventControllerKey *event_controller,
				 GdkModifierType keyval,
				 AgsWaveEdit *wave_edit)
{
  return(FALSE);
}

void
ags_wave_edit_drawing_area_motion_notify_position_cursor(GtkWidget *editor,
							 GtkWidget *toolbar,
							 AgsWaveEdit *wave_edit,
							 AgsMachine *machine,
							 gdouble x, gdouble y)
{
  GtkAdjustment *vscrollbar_adjustment;
    
  GtkAllocation allocation;

  AgsApplicationContext *application_context;
  
  gdouble c_range;
  guint g_range;
  double zoom_factor, zoom;
  double zoom_correction;

  application_context = ags_application_context_get_instance();

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom) - 2.0);
  
  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);

  vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(wave_edit->vscrollbar));

  c_range = wave_edit->upper - wave_edit->lower;

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* cursor position */
  zoom_correction = 1.0 / 16;

  wave_edit->cursor_position_x = (guint) ((zoom_factor * x + (gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) / zoom / zoom_correction)));

  wave_edit->cursor_position_y = (((allocation.height - y) / g_range) * c_range);

#ifdef AGS_DEBUG
  g_message("%lu %f", wave_edit->cursor_position_x, wave_edit->cursor_position_y);
#endif
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) wave_edit);
}

void
ags_wave_edit_drawing_area_motion_notify_select_buffer(GtkWidget *editor,
						       GtkWidget *toolbar,
						       AgsWaveEdit *wave_edit,
						       AgsMachine *machine,
						       gdouble x, gdouble y)
{
  AgsApplicationContext *application_context;
  
  double zoom_factor, zoom;
  double zoom_correction;

  application_context = ags_application_context_get_instance();

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom) - 2.0);

  zoom_correction = 1.0 / 16;
  
  wave_edit->selection_x1 = (guint) (zoom_factor * x) + (gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) / zoom / zoom_correction);
  
  if(y + gtk_range_get_value(GTK_RANGE(wave_edit->vscrollbar)) >= 0.0){
    wave_edit->selection_y1 = (guint) y + gtk_range_get_value(GTK_RANGE(wave_edit->vscrollbar));
  }else{
    wave_edit->selection_y1 = 0.0;
  }
    
  gtk_widget_queue_draw((GtkWidget *) wave_edit);
}

gboolean
ags_wave_edit_motion_callback(GtkEventControllerMotion *event_controller,
			      gdouble x,
			      gdouble y,
			      AgsWaveEdit *wave_edit)
{
  GtkWidget *editor;
  GtkWidget *toolbar;
  AgsCompositeToolbar *composite_toolbar;
  AgsMachine *machine;

  AgsApplicationContext *application_context;
  
  gboolean selected_position_cursor, selected_edit, selected_clear, selected_select;

  application_context = ags_application_context_get_instance();

  editor = gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
				   AGS_TYPE_COMPOSITE_EDITOR);
    
  toolbar = AGS_COMPOSITE_EDITOR(editor)->toolbar;

  machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;

  composite_toolbar = (AgsCompositeToolbar *) toolbar;

  selected_position_cursor = (composite_toolbar->selected_tool == composite_toolbar->position) ? TRUE: FALSE;
  selected_select = (composite_toolbar->selected_tool == composite_toolbar->select) ? TRUE: FALSE;
  
  gtk_widget_grab_focus((GtkWidget *) wave_edit->drawing_area);

  if(machine != NULL &&
     (AGS_WAVE_EDIT_BUTTON_1 & (wave_edit->button_mask)) != 0){
    if(wave_edit->mode == AGS_WAVE_EDIT_POSITION_CURSOR){
      ags_wave_edit_drawing_area_motion_notify_position_cursor(editor,
							       toolbar,
							       wave_edit,
							       machine,
							       x, y);
    }else if(wave_edit->mode == AGS_WAVE_EDIT_SELECT_BUFFER){
      ags_wave_edit_drawing_area_motion_notify_select_buffer(editor,
							     toolbar,
							     wave_edit,
							     machine,
							     x, y);
    }
  }

  return(FALSE);
}

void
ags_wave_edit_drawing_area_button_press_position_cursor(GtkWidget *editor,
							GtkWidget *toolbar,
							AgsWaveEdit *wave_edit,
							AgsMachine *machine,
							gint n_press,
							gdouble x, gdouble y)
{
  GtkAdjustment *vscrollbar_adjustment;
    
  GtkAllocation allocation;

  AgsApplicationContext *application_context;
  
  gdouble c_range;
  guint g_range;
  double zoom_factor, zoom;
  double zoom_correction;

  application_context = ags_application_context_get_instance();

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom) - 2.0);
  
  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);

  vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(wave_edit->vscrollbar));

  c_range = wave_edit->upper - wave_edit->lower;

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* cursor position */
  zoom_correction = 1.0 / 16;

  wave_edit->cursor_position_x = (guint) ((zoom_factor * x + (gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) / zoom / zoom_correction)));
    
  wave_edit->cursor_position_y = (((allocation.height - y) / g_range) * c_range);

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) wave_edit);
}
  
void
ags_wave_edit_drawing_area_button_press_select_buffer(GtkWidget *editor,
						      GtkWidget *toolbar,
						      AgsWaveEdit *wave_edit,
						      AgsMachine *machine,
						      gint n_press,
						      gdouble x, gdouble y)
{
  AgsApplicationContext *application_context;
  
  double zoom_factor, zoom;
  double zoom_correction;

  application_context = ags_application_context_get_instance();

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom) - 2.0);
  
  zoom_correction = 1.0 / 16;

  wave_edit->selection_x0 = (guint) (zoom_factor * x) + (gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) / zoom / zoom_correction);
  wave_edit->selection_x1 = wave_edit->selection_x0;
    
  wave_edit->selection_y0 = (guint) y + gtk_range_get_value(GTK_RANGE(wave_edit->vscrollbar));
  wave_edit->selection_y1 = wave_edit->selection_y0;

  gtk_widget_queue_draw((GtkWidget *) wave_edit);
}  

gboolean
ags_wave_edit_gesture_click_pressed_callback(GtkGestureClick *event_controller,
					     gint n_press,
					     gdouble x,
					     gdouble y,
					     AgsWaveEdit *wave_edit)
{
  GtkWidget *editor;
  AgsCompositeToolbar *composite_toolbar;
  GtkWidget *toolbar;
  AgsMachine *machine;

  AgsApplicationContext *application_context;
  
  gboolean selected_position_cursor, selected_select;

  application_context = ags_application_context_get_instance();

  selected_position_cursor = FALSE;
  selected_select = FALSE;

  editor = gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
				   AGS_TYPE_COMPOSITE_EDITOR);
    
  toolbar = AGS_COMPOSITE_EDITOR(editor)->toolbar;

  machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;

  composite_toolbar = (AgsCompositeToolbar *) toolbar;
    
  selected_position_cursor = (composite_toolbar->selected_tool == composite_toolbar->position) ? TRUE: FALSE;
  selected_select = (composite_toolbar->selected_tool == composite_toolbar->select) ? TRUE: FALSE;

  AGS_COMPOSITE_EDITOR(editor)->wave_edit->focused_edit = wave_edit;
  
  gtk_widget_grab_focus((GtkWidget *) wave_edit->drawing_area);

  if(machine != NULL){    
    wave_edit->button_mask = AGS_WAVE_EDIT_BUTTON_1;
    
    if(selected_position_cursor){
      wave_edit->mode = AGS_WAVE_EDIT_POSITION_CURSOR;

      ags_wave_edit_drawing_area_button_press_position_cursor(editor,
							      toolbar,
							      wave_edit,
							      machine,
							      n_press,
							      x, y);
    }else if(selected_select){
      wave_edit->mode = AGS_WAVE_EDIT_SELECT_BUFFER;

      ags_wave_edit_drawing_area_button_press_select_buffer(editor,
							    toolbar,
							    wave_edit,
							    machine,
							    n_press,
							    x, y);
    }
  }

  return(FALSE);
}

void
ags_wave_edit_drawing_area_button_release_position_cursor(GtkWidget *editor,
							  GtkWidget *toolbar,
							  AgsWaveEdit *wave_edit,
							  AgsMachine *machine,
							  gint n_press,
							  gdouble x, gdouble y)
{
  GtkAdjustment *vscrollbar_adjustment;

  GtkAllocation allocation;

  AgsApplicationContext *application_context;
  
  gdouble c_range;
  guint g_range;
  double zoom_factor, zoom;
  double zoom_correction;

  application_context = ags_application_context_get_instance();

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom) - 2.0);
  
  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);

  vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(wave_edit->vscrollbar));

  c_range = wave_edit->upper - wave_edit->lower;

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  /* cursor position */
  zoom_correction = 1.0 / 16;

  wave_edit->cursor_position_x = (guint) ((zoom_factor * x + (gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) / zoom / zoom_correction)));
    
  wave_edit->cursor_position_y = (((allocation.height - y) / g_range) * c_range);
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) wave_edit);
}

void
ags_wave_edit_drawing_area_button_release_select_buffer(GtkWidget *editor,
							GtkWidget *toolbar,
							AgsWaveEdit *wave_edit,
							AgsMachine *machine,
							gint n_press,
							gdouble x, gdouble y)
{
  GtkAdjustment *vscrollbar_adjustment;

  GtkAllocation allocation;

  AgsApplicationContext *application_context;

  gdouble c_range;
  guint g_range;
  double zoom_factor, zoom;
  double zoom_correction;
  guint x0, x1;
  gdouble y0, y1;

  application_context = ags_application_context_get_instance();
  
  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom) - 2.0);
  
  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);

  vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(wave_edit->vscrollbar));

  c_range = wave_edit->upper - wave_edit->lower;

  g_range = gtk_adjustment_get_upper(vscrollbar_adjustment) + allocation.height;

  //FIXME:JK: this won't work
//  wave_edit->selection_x1 = (guint) (zoom_factor * x) + (gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) / zoom / zoom_correction);

  /* region */
  x0 = (guint) wave_edit->selection_x0;

  y0 = ((gdouble) (allocation.height - wave_edit->selection_y0) / g_range) * c_range;
  
  x1 = (guint) wave_edit->selection_x1;
    
  y1 = (((allocation.height - y) + gtk_range_get_value(GTK_RANGE(wave_edit->vscrollbar))) / g_range) * c_range;
    
  /* select region */
  ags_composite_editor_select_region(editor,
				     x0, y0,
				     x1, y1);
}

gboolean
ags_wave_edit_gesture_click_released_callback(GtkGestureClick *event_controller,
					      gint n_press,
					      gdouble x,
					      gdouble y,
					      AgsWaveEdit *wave_edit)
{
  GtkWidget *editor;
  GtkWidget *toolbar;
  AgsCompositeToolbar *composite_toolbar;
  AgsMachine *machine;

  AgsApplicationContext *application_context;
  
  gboolean selected_position_cursor, selected_select;

  application_context = ags_application_context_get_instance();

  editor = gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
				   AGS_TYPE_COMPOSITE_EDITOR);
    
  toolbar = AGS_COMPOSITE_EDITOR(editor)->toolbar;

  machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;

  composite_toolbar = (AgsCompositeToolbar *) toolbar;
    
  selected_position_cursor = (composite_toolbar->selected_tool == composite_toolbar->position) ? TRUE: FALSE;
  selected_select = (composite_toolbar->selected_tool == composite_toolbar->select) ? TRUE: FALSE;
  
  if(machine != NULL){
    wave_edit->button_mask &= (~AGS_WAVE_EDIT_BUTTON_1);
    
    if(selected_position_cursor){
      ags_wave_edit_drawing_area_button_release_position_cursor(editor,
								toolbar,
								wave_edit,
								machine,
								n_press,
								x, y);

      wave_edit->mode = AGS_WAVE_EDIT_NO_EDIT_MODE;
    }else if(selected_select){
      ags_wave_edit_drawing_area_button_release_select_buffer(editor,
							      toolbar,
							      wave_edit,
							      machine,
							      n_press,
							      x, y);

      wave_edit->mode = AGS_WAVE_EDIT_NO_EDIT_MODE;
    }
  }

  return(FALSE);
}

void
ags_wave_edit_realize(GtkWidget *widget)
{
  GdkFrameClock *frame_clock;
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_wave_edit_parent_class)->realize(widget);

  frame_clock = gtk_widget_get_frame_clock(widget);
  
  g_signal_connect(frame_clock, "update", 
		   G_CALLBACK(ags_wave_edit_frame_clock_update_callback), widget);

  gdk_frame_clock_begin_updating(frame_clock);
}

void
ags_wave_edit_measure(GtkWidget *widget,
		      GtkOrientation orientation,
		      int for_size,
		      int *minimum,
		      int *natural,
		      int *minimum_baseline,
		      int *natural_baseline)
{
  AgsWaveEdit *wave_edit;

  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();

  wave_edit = (AgsWaveEdit *) widget;
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  if(orientation == GTK_ORIENTATION_VERTICAL){    
    minimum[0] =
      natural[0] = (gint) (gui_scale_factor * AGS_LEVEL_DEFAULT_HEIGHT_REQUEST);
  }else{
    minimum =
      natural = NULL;
  }
}

void
ags_wave_edit_size_allocate(GtkWidget *widget,
			    int width,
			    int height,
			    int baseline)
{
  AgsWaveEdit *wave_edit;

  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;

  wave_edit = (AgsWaveEdit *) widget;
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  width = -1;
  height = (gint) (gui_scale_factor * AGS_LEVEL_DEFAULT_HEIGHT_REQUEST);
  
  GTK_WIDGET_CLASS(ags_wave_edit_parent_class)->size_allocate(widget,
							      width,
							      height,
							      baseline);
}

void
ags_wave_edit_show(GtkWidget *widget)
{
  AgsWaveEdit *wave_edit;

  GtkAllocation allocation;
  
  wave_edit = AGS_WAVE_EDIT(widget);

  /* call parent */
  GTK_WIDGET_CLASS(ags_wave_edit_parent_class)->show(widget);

  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);
  
  gtk_widget_show((GtkWidget *) wave_edit->drawing_area);
  
  if((AGS_WAVE_EDIT_SHOW_RULER & (wave_edit->flags)) != 0){
    gtk_widget_show((GtkWidget *) wave_edit->ruler);
  }

  if((AGS_WAVE_EDIT_SHOW_VSCROLLBAR & (wave_edit->flags)) != 0){
    gtk_widget_show((GtkWidget *) wave_edit->vscrollbar);
  }

  if((AGS_WAVE_EDIT_SHOW_HSCROLLBAR & (wave_edit->flags)) != 0){
    gtk_widget_show((GtkWidget *) wave_edit->hscrollbar);
  }
}

void
ags_wave_edit_frame_clock_update_callback(GdkFrameClock *frame_clock,
					  AgsWaveEdit *wave_edit)
{
  gtk_widget_queue_draw((GtkWidget *) wave_edit);
}

gboolean
ags_wave_edit_auto_scroll_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_wave_edit_auto_scroll,
			 widget) != NULL){
    AgsCompositeEditor *composite_editor;
    AgsWaveEdit *wave_edit;

    GtkAdjustment *hscrollbar_adjustment;

    GObject *output_soundcard;
    
    double x;
    
    wave_edit = AGS_WAVE_EDIT(widget);

    if((AGS_WAVE_EDIT_AUTO_SCROLL & (wave_edit->flags)) == 0){
      return(TRUE);
    }
    
    composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) wave_edit,
								      AGS_TYPE_COMPOSITE_EDITOR);
    
    if(composite_editor->selected_machine == NULL){
      return(TRUE);
    }

    hscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(wave_edit->hscrollbar));
    
    /* reset offset */
    g_object_get(composite_editor->selected_machine->audio,
		 "output-soundcard", &output_soundcard,
		 NULL);
    
    wave_edit->note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));
    wave_edit->note_offset_absolute = ags_soundcard_get_note_offset_absolute(AGS_SOUNDCARD(output_soundcard));

    /* reset scrollbar */
    x = ((wave_edit->note_offset * wave_edit->control_width) / (AGS_WAVE_DEFAULT_LENGTH * wave_edit->control_width)) * gtk_adjustment_get_upper(hscrollbar_adjustment);
    
    gtk_range_set_value(GTK_RANGE(wave_edit->hscrollbar),
			x);

    g_object_unref(output_soundcard);
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

void
ags_wave_edit_reset_vscrollbar(AgsWaveEdit *wave_edit)
{
  GtkAdjustment *adjustment;

  AgsApplicationContext *application_context;
  
  GtkAllocation allocation;
  
  gdouble gui_scale_factor;
  double varea_height;
  gdouble upper, old_upper;
  
  if(!AGS_IS_WAVE_EDIT(wave_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  adjustment = gtk_range_get_adjustment(GTK_RANGE(wave_edit->vscrollbar));

  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);
  
  /* upper */
  old_upper = gtk_adjustment_get_upper(adjustment); 

  varea_height = (wave_edit->step_count * wave_edit->control_height);
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
}

void
ags_wave_edit_reset_hscrollbar(AgsWaveEdit *wave_edit)
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
  
  if(!AGS_IS_WAVE_EDIT(wave_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);
  
  /* adjustment */
  adjustment = gtk_range_get_adjustment(GTK_RANGE(wave_edit->hscrollbar));

  /* zoom */
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) wave_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  composite_toolbar = composite_editor->toolbar;

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom) - 2.0);

  /* upper */
  old_upper = gtk_adjustment_get_upper(adjustment);
  
  zoom_correction = 1.0 / 16;

//  map_width = ((double) AGS_WAVE_DEFAULT_LENGTH * zoom * zoom_correction);
  map_width = (gui_scale_factor * 64.0) * (16.0 * 16.0 * 1200.0) * zoom * zoom_correction;
  upper = map_width - allocation.width;

  if(upper < 0.0){    
    upper = 0.0;
  }

  gtk_adjustment_set_upper(adjustment,
			   upper);

  /* ruler */
  wave_edit->ruler->factor = zoom_factor;
  wave_edit->ruler->precision = zoom;
  wave_edit->ruler->scale_precision = 1.0 / zoom;

  gtk_adjustment_set_upper(wave_edit->ruler->adjustment,
			   upper);

  /* reset value */
  if(old_upper != 0.0){
#if 0
    gtk_adjustment_set_value(adjustment,
			     gtk_adjustment_get_value(adjustment) / old_upper * upper);
#endif
  }
}

void
ags_wave_edit_draw_segment(AgsWaveEdit *wave_edit, cairo_t *cr)
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

  gdouble gui_scale_factor;
  gdouble x_offset, y_offset;
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
  gboolean shadow_success;

  GValue value = {0,};

  const static double quarter_dashes = {
    0.25,
  };

  const static double segment_dashes = {
    0.5,
  };
  
  if(!AGS_IS_WAVE_EDIT(wave_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);

  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) wave_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);

  toolbar = composite_editor->toolbar;

  tact = exp2((double) gtk_combo_box_get_active(toolbar->zoom) - 2.0);
   
  /* dimension and offset */
  x_offset = gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar));
  y_offset = gtk_range_get_value(GTK_RANGE(wave_edit->vscrollbar));

  y = (gdouble) 0.0;

  width = (gdouble) allocation.width;
  height = (gdouble) allocation.height;

  /* style context */
  style_context = gtk_widget_get_style_context((GtkWidget *) wave_edit);

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
  
  if(!fg_success ||
     !bg_success ||
     !shadow_success){
    gdk_rgba_parse(&fg_color,
		   "#101010");

    gdk_rgba_parse(&bg_color,
		   "#cbd5d9");

    gdk_rgba_parse(&shadow_color,
		   "#ffffff40");
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

  /* background border */
  cairo_set_source_rgba(cr,
			shadow_color.red,
			shadow_color.green,
			shadow_color.blue,
			shadow_color.alpha);
  cairo_set_line_width(cr, 1.0);
  cairo_rectangle(cr, 0.0, y, width, height);
  cairo_stroke(cr);

  cairo_set_line_width(cr, 1.0);

  map_height = (gdouble) height;

  control_width = (gint) (gui_scale_factor * (gdouble) AGS_WAVE_EDIT_DEFAULT_CONTROL_WIDTH);
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
      goto ags_wave_edit_draw_segment0;
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
    ags_wave_edit_draw_segment0:
      cairo_move_to(cr, (double) i, y);
      cairo_line_to(cr, (double) i, y + height);
      cairo_stroke(cr);
      
      i += control_width;
    }
  }

  cairo_set_source_rgba(cr,
			bg_color.red,
			bg_color.green,
			bg_color.blue,
			bg_color.alpha);

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
      
  cairo_surface_mark_dirty(cairo_get_target(cr));
}

void
ags_wave_edit_draw_position(AgsWaveEdit *wave_edit, cairo_t *cr)
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

  GValue value = {0,};

  if(!AGS_IS_WAVE_EDIT(wave_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) wave_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);

  toolbar = composite_editor->toolbar;

  tact = exp2((double) gtk_combo_box_get_active(toolbar->zoom) - 2.0);
  
  /* style context */
  style_context = gtk_widget_get_style_context((GtkWidget *) wave_edit);

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
  control_width = (gint) (gui_scale_factor * (gdouble) AGS_WAVE_EDIT_DEFAULT_CONTROL_WIDTH);

  position = ((double) wave_edit->note_offset) * ((double) control_width);
  
  y = 0.0;
  x = (position) - (gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)));

  width = (double) ((guint) (gui_scale_factor * AGS_WAVE_EDIT_DEFAULT_FADER_WIDTH));
  height = (double) ((guint) (gui_scale_factor * AGS_WAVE_EDIT_DEFAULT_HEIGHT));

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
ags_wave_edit_draw_cursor(AgsWaveEdit *wave_edit, cairo_t *cr)
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
  double zoom_correction;
  double x, y;
  double width, height;
  gboolean height_fits;
  gboolean dark_theme;
  gboolean fg_success;

  GValue value = {0,};

  if(!AGS_IS_WAVE_EDIT(wave_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  /* style context */
  style_context = gtk_widget_get_style_context((GtkWidget *) wave_edit);

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

  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);
  
  /* zoom */
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) wave_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  composite_toolbar = composite_editor->toolbar;

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom) - 2.0);
  
  zoom_correction = 1.0 / 16;

  y = 0.0;
  x = (((double) wave_edit->cursor_position_x) - (gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) / zoom / zoom_correction)) /  zoom_factor;

  width = (double) ((guint) (gui_scale_factor * AGS_WAVE_EDIT_DEFAULT_FADER_WIDTH));
  height = (double) ((guint) (gui_scale_factor * AGS_WAVE_EDIT_DEFAULT_HEIGHT));

  /* push group */
  cairo_push_group(cr);
  
  /* draw cursor */
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
      
  cairo_surface_mark_dirty(cairo_get_target(cr));
}

void
ags_wave_edit_draw_selection(AgsWaveEdit *wave_edit, cairo_t *cr)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;

  GtkStyleContext *style_context;
  GtkSettings *settings;

  AgsApplicationContext *application_context;

  GtkAllocation allocation;

  GdkRGBA fg_color;
  
  double zoom, zoom_factor;
  double zoom_correction;
  double x, y;
  double width, height;
  gboolean dark_theme;
  gboolean fg_success;

  GValue value = {0,};

  if(!AGS_IS_WAVE_EDIT(wave_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* style context */
  style_context = gtk_widget_get_style_context((GtkWidget *) wave_edit);

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

  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);

  /* zoom */
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) wave_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  composite_toolbar = composite_editor->toolbar;

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom) - 2.0);
  
  /* get offset and dimensions */  
  zoom_correction = 1.0 / 16;
  
  if(wave_edit->selection_x0 < wave_edit->selection_x1){
    x = (((double) wave_edit->selection_x0) - (gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) / zoom / zoom_correction)) / zoom_factor;
    width = ((double) wave_edit->selection_x1 - (double) wave_edit->selection_x0) / zoom_factor;
  }else{
    x = (((double) wave_edit->selection_x1) - (gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) / zoom / zoom_correction)) / zoom_factor;
    width = ((double) wave_edit->selection_x0 - (double) wave_edit->selection_x1) / zoom_factor;
  }

  if(wave_edit->selection_y0 < wave_edit->selection_y1){
    y = ((double) wave_edit->selection_y0) - gtk_range_get_value(GTK_RANGE(wave_edit->vscrollbar));
    height = ((double) wave_edit->selection_y1 - (double) wave_edit->selection_y0);
  }else{
    y = ((double) wave_edit->selection_y1) - gtk_range_get_value(GTK_RANGE(wave_edit->vscrollbar));
    height = ((double) wave_edit->selection_y0 - (double) wave_edit->selection_y1);
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
ags_wave_edit_draw_buffer(AgsWaveEdit *wave_edit,
			  AgsBuffer *buffer,
			  cairo_t *cr,
			  gdouble bpm,
			  gdouble opacity)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;
  AgsMachine *selected_machine;

  GtkStyleContext *style_context;
  GtkSettings *settings;

  AgsApplicationContext *application_context;

  GtkAllocation allocation;

  GdkRGBA fg_color;

  GObject *soundcard;
  
  cairo_t *i_cr;
  cairo_surface_t *surface;

  unsigned char *image_data, *bg_data;
  
  gdouble gui_scale_factor;
  gdouble tact;
  guint samplerate;
  guint buffer_size;
  guint format;
  guint x0, x1;
  guint x_cut;
  guint64 x;
  gdouble width, height;
  double zoom, zoom_factor;
  gdouble delay_factor;
  guint wave_data_width;
  guint i;
  gboolean dark_theme;
  gboolean fg_success;

  GValue value = {0};

  GRecMutex *buffer_mutex;

  if(!AGS_IS_WAVE_EDIT(wave_edit) ||
     !AGS_IS_BUFFER(buffer)){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) wave_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);

  if(composite_editor->selected_machine == NULL){
    return;
  }

  selected_machine = composite_editor->selected_machine;
    
  composite_toolbar = composite_editor->toolbar;

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom) - 2.0);
    
  tact = exp2((double) gtk_combo_box_get_active(composite_toolbar->zoom) - 2.0);
  
  /* style context */
  style_context = gtk_widget_get_style_context((GtkWidget *) wave_edit);

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
  
  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);

  buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);
  
  g_object_get(selected_machine->audio,
	       "output-soundcard", &soundcard,
	       NULL);

  g_object_unref(soundcard);
  
  delay_factor = ags_soundcard_get_delay_factor(AGS_SOUNDCARD(soundcard));

  /* get visisble region */
  x0 = gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar));
  x1 = (gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) + allocation.width * zoom_factor);

  /* width and height */
  width = (gdouble) allocation.width;
  height = (gdouble) allocation.height;
      
  /* draw point */
  g_object_get(buffer,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       "x", &x,
	       NULL);

  x_cut = x0;

  if(((((double) (x) / samplerate * (bpm / 60.0) / delay_factor) * (gui_scale_factor * 64.0))) / zoom_factor - x_cut < 0.0 ||
     ((((double) (x) / samplerate * (bpm / 60.0) / delay_factor) * (gui_scale_factor * 64.0))) / zoom_factor - x_cut > allocation.width){
    return;
  }
  
  wave_data_width = buffer_size;

  /*
  if(wave_edit->stride != cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, wave_data_width)){
    if(wave_edit->wave_data != NULL){
      free(wave_edit->wave_data);
    }

    wave_edit->stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, wave_data_width);
    wave_edit->wave_data = malloc(wave_edit->stride * AGS_WAVE_EDIT_DEFAULT_HEIGHT);
  }
  
  surface = cairo_image_surface_create_for_data(wave_edit->wave_data,
						CAIRO_FORMAT_ARGB32,
						wave_data_width,
						AGS_WAVE_EDIT_DEFAULT_HEIGHT,
						wave_edit->stride);

  i_cr = cairo_create(surface);
  */

  /* draw buffer */
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.blue,
			fg_color.green,
			opacity * fg_color.alpha);
  
  cairo_set_line_width(cr, 1.0);

  //  cairo_scale(cr,
  //	      1.0 / (zoom_factor * (((60.0 / bpm) * ((double) buffer_size / (double) samplerate)) * AGS_WAVE_EDIT_X_RESOLUTION)), 1.0);
  
  for(i = 0; i < buffer_size; i += (zoom_factor * 16)){
    double y0, y1;

    y0 = 0.0;
    y1 = 0.0;

    g_rec_mutex_lock(buffer_mutex);

    switch(format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      y1 = (double) ((gint8 *) buffer->data)[i] / 127.0;
    }
    break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      y1 = (double) ((gint16 *) buffer->data)[i] / 32767.0;
    }
    break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      y1 = (double) ((gint32 *) buffer->data)[i] / 8388607.0;
    }
    break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      y1 = (double) ((gint32 *) buffer->data)[i] / 214748363.0;
    }
    break;
    case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      y1 = (double) ((gint64 *) buffer->data)[i] / 9223372036854775807.0;
    }
    break;
    case AGS_SOUNDCARD_FLOAT:
    {
      y1 = (double) ((gfloat *) buffer->data)[i];
    }
    break;
    case AGS_SOUNDCARD_DOUBLE:
    {
      y1 = (double) ((gdouble *) buffer->data)[i];
    }
    break;
    }

    g_rec_mutex_unlock(buffer_mutex);

    y0 = 0.5 * height;
    y1 = (((y1 + 1.0) * height) / 2.0);
    
    cairo_move_to(cr,
		  ((((double) (x + i) / samplerate * (bpm / 60.0) / delay_factor) * (gui_scale_factor * 64.0))) / zoom_factor - x_cut, y0);
    cairo_line_to(cr,
		  ((((double) (x + i) / samplerate * (bpm / 60.0) / delay_factor) * (gui_scale_factor * 64.0))) / zoom_factor - x_cut, y1);
    cairo_stroke(cr);
  }  
  
  /* check buffer selected */
  if(ags_buffer_test_flags(buffer, AGS_BUFFER_IS_SELECTED)){
    /* draw selected buffer */
    cairo_set_source_rgba(cr,
			  fg_color.red,
			  fg_color.blue,
			  fg_color.green,
			  opacity / 3.0);

    cairo_set_line_width(cr, 1.0 + (double) wave_edit->selected_buffer_border);

    for(i = 0; i < buffer_size; i += (zoom_factor * 16)){
      double y0, y1;

      y0 = 0.0;
      y1 = 0.0;

      g_rec_mutex_lock(buffer_mutex);

      switch(format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	y1 = (double) ((gint8 *) buffer->data)[i] / exp2(7.0);
      }
      break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	y1 = (double) ((gint16 *) buffer->data)[i] / exp2(15.0);
      }
      break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	y1 = (double) ((gint32 *) buffer->data)[i] / exp2(23.0);
      }
      break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	y1 = (double) ((gint32 *) buffer->data)[i] / exp2(31.0);
      }
      break;
      case AGS_SOUNDCARD_SIGNED_64_BIT:
      {
	y1 = (double) ((gint64 *) buffer->data)[i] / exp2(63.0);
      }
      break;
      case AGS_SOUNDCARD_FLOAT:
      {
	y1 = (double) ((gfloat *) buffer->data)[i];
      }
      break;
      case AGS_SOUNDCARD_DOUBLE:
      {
	y1 = (double) ((gdouble *) buffer->data)[i];
      }
      break;
      }

      g_rec_mutex_unlock(buffer_mutex);

      y0 = 0.5 * height;
      y1 = (((y1 + 1.0) * height) / 2.0);
    
      cairo_move_to(cr,
		    ((((double) (x + i) / samplerate * (bpm / 60.0) / delay_factor) * (gui_scale_factor * 64.0))) / zoom_factor - x_cut, y0);
      cairo_line_to(cr,
		    ((((double) (x + i) / samplerate * (bpm / 60.0) / delay_factor) * (gui_scale_factor * 64.0))) / zoom_factor - x_cut, y1);
      cairo_stroke(cr);
    }  
  }
  

  /* draw buffer */
  //  cairo_set_source_surface(cr, surface,
  //			   (bpm / (60.0 * (x / samplerate))) * AGS_WAVE_EDIT_X_RESOLUTION, 0.0);
}

void
ags_wave_edit_draw_wave(AgsWaveEdit *wave_edit, cairo_t *cr)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;
  AgsMachine *selected_machine;

  AgsTimestamp *current_timestamp;    

  AgsApplicationContext *application_context;

  GObject *soundcard;
  
  GtkAllocation allocation;

  GList *start_list_wave, *list_wave;
  GList *start_list_buffer, *list_buffer;

  gdouble gui_scale_factor;
  double zoom, zoom_factor;
  gdouble delay_factor;
  gdouble opacity;
  guint line;
  guint samplerate;
  gdouble bpm;
  guint x0, x1;
  guint x_cut;
  
  if(!AGS_IS_WAVE_EDIT(wave_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();

  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  gtk_widget_get_allocation(GTK_WIDGET(wave_edit->drawing_area),
			    &allocation);
 
  /* zoom */
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) wave_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);

  if(composite_editor->selected_machine == NULL){
    return;
  }

  selected_machine = composite_editor->selected_machine;
    
  composite_toolbar = composite_editor->toolbar;

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) composite_toolbar->zoom) - 2.0);

  opacity = gtk_spin_button_get_value(composite_toolbar->opacity);  

  bpm = gtk_spin_button_get_value(window->navigation->bpm);
 
  g_object_get(selected_machine->audio,
	       "output-soundcard", &soundcard,
	       NULL);
 
  delay_factor = ags_soundcard_get_delay_factor(AGS_SOUNDCARD(soundcard));
  
  /* get visisble region */
  x0 = (guint) gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar));
  x1 = ((guint) gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar)) + allocation.width * zoom_factor);

  x_cut = x0;
  
  /* draw wave */
  g_object_get(selected_machine->audio,
	       "wave", &start_list_wave,
	       NULL);

  list_wave = start_list_wave;
  line = wave_edit->line;

  while(list_wave != NULL){
    AgsWave *wave;

    GList *start_list_buffer, *list_buffer;

    guint current_line;
    guint64 offset;
    
    wave = AGS_WAVE(list_wave->data);

    g_object_get(wave,
		 "timestamp", &current_timestamp,
		 "line", &current_line,
		 "samplerate", &samplerate,
		 NULL);

    g_object_unref(current_timestamp);
    
    if(current_line != line ||
       current_timestamp == NULL){
      list_wave = list_wave->next;

      continue;
    }
    
    offset = ags_timestamp_get_ags_offset(current_timestamp);
    
    if(((((double) (offset) / samplerate * (bpm / 60.0) / delay_factor) * (gui_scale_factor * 64.0))) / zoom_factor - x_cut > allocation.width){
      break;
    }

    g_object_get(wave,
		 "buffer", &start_list_buffer,
		 NULL);
      
    list_buffer = start_list_buffer;

    while(list_buffer != NULL){
      ags_wave_edit_draw_buffer(wave_edit,
				list_buffer->data,
				cr,
				bpm,
				opacity);

      /* iterate */
      list_buffer = list_buffer->next;
    }

    g_list_free_full(start_list_buffer,
		     g_object_unref);
      
    /* iterate */
    list_wave = list_wave->next;
  }
  
  //TODO:JK: implement me

  g_object_unref(soundcard);
 
  g_list_free_full(start_list_wave,
		   g_object_unref);
}

void
ags_wave_edit_draw(AgsWaveEdit *wave_edit, cairo_t *cr)
{
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  ags_wave_edit_reset_vscrollbar(wave_edit);
  ags_wave_edit_reset_hscrollbar(wave_edit);

  /* segment */
  ags_wave_edit_draw_segment(wave_edit, cr);

  /* wave */
  ags_wave_edit_draw_wave(wave_edit, cr);
  
  /* edit mode */
  switch(wave_edit->mode){
  case AGS_WAVE_EDIT_POSITION_CURSOR:
    {
      ags_wave_edit_draw_cursor(wave_edit, cr);
    }
    break;
  case AGS_WAVE_EDIT_SELECT_BUFFER:
    {
      ags_wave_edit_draw_selection(wave_edit, cr);
    }
    break;
  }

  /* fader */
  if((AGS_WAVE_EDIT_AUTO_SCROLL & (wave_edit->flags)) != 0){
    ags_wave_edit_draw_position(wave_edit, cr);
  }
}

/**
 * ags_wave_edit_new:
 * @line: the line
 *
 * Create a new instance of #AgsWaveEdit.
 *
 * Returns: the new #AgsWaveEdit
 *
 * Since: 3.0.0
 */
AgsWaveEdit*
ags_wave_edit_new(guint line)
{
  AgsWaveEdit *wave_edit;

  wave_edit = (AgsWaveEdit *) g_object_new(AGS_TYPE_WAVE_EDIT,
					   "line", line,
					   NULL);

  return(wave_edit);
}
