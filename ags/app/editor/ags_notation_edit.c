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

#include <ags/app/editor/ags_notation_edit.h>
#include <ags/app/editor/ags_notation_edit_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_navigation.h>
#include <ags/app/ags_composite_editor.h>

#include <ags/app/editor/ags_notation_meta.h>

#include <gdk/gdkkeysyms.h>

#include <math.h>

void ags_notation_edit_class_init(AgsNotationEditClass *notation_edit);
void ags_notation_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_notation_edit_init(AgsNotationEdit *notation_edit);
void ags_notation_edit_dispose(GObject *gobject);
void ags_notation_edit_finalize(GObject *gobject);

void ags_notation_edit_connect(AgsConnectable *connectable);
void ags_notation_edit_disconnect(AgsConnectable *connectable);

void ags_notation_edit_realize(GtkWidget *widget);
void ags_notation_edit_unrealize(GtkWidget *widget);

void ags_notation_edit_measure(GtkWidget *widget,
			       GtkOrientation orientation,
			       int for_size,
			       int *minimum,
			       int *natural,
			       int *minimum_baseline,
			       int *natural_baseline);
void ags_notation_edit_size_allocate(GtkWidget *widget,
				     int width,
				     int height,
				     int baseline);

void ags_notation_edit_show(GtkWidget *widget);

void ags_notation_edit_frame_clock_update_callback(GdkFrameClock *frame_clock,
						   AgsNotationEdit *notation_edit);

void ags_notation_edit_drawing_area_button_press_position_cursor(GtkWidget *editor,
								 GtkWidget *toolbar,
								 AgsNotationEdit *notation_edit,
								 AgsMachine *machine,
								 gint n_press,
								 gdouble x, gdouble y);
void ags_notation_edit_drawing_area_button_press_add_note(GtkWidget *editor,
							  GtkWidget *toolbar,
							  AgsNotationEdit *notation_edit,
							  AgsMachine *machine,
							  gint n_press,
							  gdouble x, gdouble y);
void ags_notation_edit_drawing_area_button_press_select_note(GtkWidget *editor,
							     GtkWidget *toolbar,
							     AgsNotationEdit *notation_edit,
							     AgsMachine *machine,
							     gint n_press,
							     gdouble x, gdouble y);

void ags_notation_edit_drawing_area_button_release_position_cursor(GtkWidget *editor,
								   GtkWidget *toolbar,
								   AgsNotationEdit *notation_edit,
								   AgsMachine *machine,
								   gint n_press, gdouble x, gdouble y);
void ags_notation_edit_drawing_area_button_release_add_note(GtkWidget *editor,
							    GtkWidget *toolbar,
							    AgsNotationEdit *notation_edit,
							    AgsMachine *machine,
							    gint n_press,
							    gdouble x, gdouble y);
void ags_notation_edit_drawing_area_button_release_delete_note(GtkWidget *editor,
							       GtkWidget *toolbar,
							       AgsNotationEdit *notation_edit,
							       AgsMachine *machine,
							       gint n_press,
							       gdouble x, gdouble y);
void ags_notation_edit_drawing_area_button_release_select_note(GtkWidget *editor,
							       GtkWidget *toolbar,
							       AgsNotationEdit *notation_edit,
							       AgsMachine *machine,
							       gint n_press,
							       gdouble x, gdouble y);

gboolean ags_notation_edit_gesture_click_pressed_callback(GtkGestureClick *event_controller,
							  gint n_press,
							  gdouble x,
							  gdouble y,
							  AgsNotationEdit *notation_edit);
gboolean ags_notation_edit_gesture_click_released_callback(GtkGestureClick *event_controller,
							   gint n_press,
							   gdouble x,
							   gdouble y,
							   AgsNotationEdit *notation_edit);

gboolean ags_notation_edit_key_pressed_callback(GtkEventControllerKey *event_controller,
						guint keyval,
						guint keycode,
						GdkModifierType state,
						AgsNotationEdit *notation_edit);
gboolean ags_notation_edit_key_released_callback(GtkEventControllerKey *event_controller,
						 guint keyval,
						 guint keycode,
						 GdkModifierType state,
						 AgsNotationEdit *notation_edit);
gboolean ags_notation_edit_modifiers_callback(GtkEventControllerKey *event_controller,
					      GdkModifierType keyval,
					      AgsNotationEdit *notation_edit);

void ags_notation_edit_drawing_area_motion_notify_position_cursor(GtkWidget *editor,
								  GtkWidget *toolbar,
								  AgsNotationEdit *notation_edit,
								  AgsMachine *machine,
								  gdouble x, gdouble y);
void ags_notation_edit_drawing_area_motion_notify_add_note(GtkWidget *editor,
							   GtkWidget *toolbar,
							   AgsNotationEdit *notation_edit,
							   AgsMachine *machine,
							   gdouble x, gdouble y);
void ags_notation_edit_drawing_area_motion_notify_select_note(GtkWidget *editor,
							      GtkWidget *toolbar,
							      AgsNotationEdit *notation_edit,
							      AgsMachine *machine,
							      gdouble x, gdouble y);

gboolean ags_notation_edit_motion_callback(GtkEventControllerMotion *event_controller,
					   gdouble x,
					   gdouble y,
					   AgsNotationEdit *notation_edit);

gboolean ags_notation_edit_auto_scroll_timeout(GtkWidget *widget);

/**
 * SECTION:ags_notation_edit
 * @short_description: edit notes
 * @title: AgsNotationEdit
 * @section_id:
 * @include: ags/app/editor/ags_notation_edit.h
 *
 * The #AgsNotationEdit lets you edit notes.
 */

enum{
  PROP_0,
};

static gpointer ags_notation_edit_parent_class = NULL;

GHashTable *ags_notation_edit_auto_scroll = NULL;

GType
ags_notation_edit_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_notation_edit = 0;

    static const GTypeInfo ags_notation_edit_info = {
      sizeof (AgsNotationEditClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_notation_edit_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsNotationEdit),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_notation_edit_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_notation_edit_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_notation_edit = g_type_register_static(GTK_TYPE_GRID,
						    "AgsNotationEdit", &ags_notation_edit_info,
						    0);
    
    g_type_add_interface_static(ags_type_notation_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_notation_edit);
  }

  return g_define_type_id__volatile;
}

void
ags_notation_edit_class_init(AgsNotationEditClass *notation_edit)
{
  GtkWidgetClass *widget;
  
  GObjectClass *gobject;
  
  ags_notation_edit_parent_class = g_type_class_peek_parent(notation_edit);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(notation_edit);

  gobject->dispose = ags_notation_edit_dispose;
  gobject->finalize = ags_notation_edit_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) notation_edit;

  widget->realize = ags_notation_edit_realize;
  widget->unrealize = ags_notation_edit_unrealize;

  widget->measure = ags_notation_edit_measure;
  widget->size_allocate = ags_notation_edit_size_allocate;

  widget->show = ags_notation_edit_show;
}

void
ags_notation_edit_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_notation_edit_connect;
  connectable->disconnect = ags_notation_edit_disconnect;
}

void
ags_notation_edit_init(AgsNotationEdit *notation_edit)
{
  GtkEventController *event_controller;
  GtkAdjustment *adjustment;

  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();

  notation_edit->flags = (AGS_NOTATION_EDIT_SHOW_RULER |
			  AGS_NOTATION_EDIT_SHOW_VSCROLLBAR |
			  AGS_NOTATION_EDIT_SHOW_HSCROLLBAR);
  notation_edit->connectable_flags = 0;
  notation_edit->mode = AGS_NOTATION_EDIT_NO_EDIT_MODE;

  notation_edit->button_mask = 0;
  notation_edit->key_mask = 0;

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  notation_edit->note_offset = 0;
  notation_edit->note_offset_absolute = 0;

  notation_edit->control_width = (guint) (gui_scale_factor * AGS_NOTATION_EDIT_DEFAULT_CONTROL_WIDTH);
  notation_edit->control_height = (guint) (gui_scale_factor * AGS_NOTATION_EDIT_DEFAULT_CONTROL_HEIGHT);

  notation_edit->control_margin_x = AGS_NOTATION_EDIT_DEFAULT_CONTROL_MARGIN_X;
  notation_edit->control_margin_y = AGS_NOTATION_EDIT_DEFAULT_CONTROL_MARGIN_Y;

  notation_edit->key_count = AGS_NOTATION_EDIT_DEFAULT_KEY_COUNT;

  notation_edit->cursor_position_x = AGS_NOTATION_EDIT_DEFAULT_CURSOR_POSITION_X;
  notation_edit->cursor_position_y = AGS_NOTATION_EDIT_DEFAULT_CURSOR_POSITION_Y;

  notation_edit->selected_note_border = AGS_NOTATION_EDIT_DEFAULT_SELECTED_NOTE_BORDER;

  notation_edit->selection_x0 = 0;
  notation_edit->selection_x1 = 0;
  notation_edit->selection_y0 = 0;
  notation_edit->selection_y1 = 0;

  notation_edit->current_note = NULL;
  
  notation_edit->ruler = ags_ruler_new(GTK_ORIENTATION_HORIZONTAL,
				       AGS_RULER_DEFAULT_STEP,
				       AGS_RULER_DEFAULT_FACTOR,
				       AGS_RULER_DEFAULT_PRECISION,
				       AGS_RULER_DEFAULT_SCALE_PRECISION);

  gtk_widget_set_visible(notation_edit->ruler,
			 FALSE);
  
  gtk_grid_attach(GTK_GRID(notation_edit),
		  (GtkWidget *) notation_edit->ruler,
		  0, 0,
		  1, 1);
  
  notation_edit->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_can_focus((GtkWidget *) notation_edit->drawing_area,
			   TRUE);
  gtk_widget_set_focusable((GtkWidget *) notation_edit->drawing_area,
			   TRUE);

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) notation_edit->drawing_area,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_notation_edit_key_pressed_callback), notation_edit);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_notation_edit_key_released_callback), notation_edit);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_notation_edit_modifiers_callback), notation_edit);

  event_controller = gtk_gesture_click_new();
  gtk_widget_add_controller((GtkWidget *) notation_edit->drawing_area,
			    event_controller);

  g_signal_connect(event_controller, "pressed",
		   G_CALLBACK(ags_notation_edit_gesture_click_pressed_callback), notation_edit);

  g_signal_connect(event_controller, "released",
		   G_CALLBACK(ags_notation_edit_gesture_click_released_callback), notation_edit);

  event_controller = gtk_event_controller_motion_new();
  gtk_widget_add_controller((GtkWidget *) notation_edit->drawing_area,
			    event_controller);

  g_signal_connect(event_controller, "motion",
		   G_CALLBACK(ags_notation_edit_motion_callback), notation_edit);
  
  gtk_widget_set_halign(notation_edit->drawing_area,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign(notation_edit->drawing_area,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand((GtkWidget *) notation_edit->drawing_area,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) notation_edit->drawing_area,
			 TRUE);

  gtk_grid_attach(GTK_GRID(notation_edit),
		  (GtkWidget *) notation_edit->drawing_area,
		  0, 1,
		  1, 1);

  /* vscrollbar */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, notation_edit->control_height, 1.0);
  notation_edit->vscrollbar = (GtkScrollbar *) gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL,
								 adjustment);

  gtk_widget_set_valign((GtkWidget *) notation_edit->vscrollbar,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) notation_edit->vscrollbar,
			 TRUE);

  gtk_widget_set_visible(notation_edit->vscrollbar,
			 FALSE);

  gtk_grid_attach(GTK_GRID(notation_edit),
		  (GtkWidget *) notation_edit->vscrollbar,
		  1, 1,
		  1, 1);

  /* hscrollbar */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (gdouble) notation_edit->control_width, 1.0);
  notation_edit->hscrollbar = (GtkScrollbar *) gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL,
								 adjustment);

  gtk_widget_set_halign((GtkWidget *) notation_edit->hscrollbar,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand((GtkWidget *) notation_edit->hscrollbar,
			 TRUE);

  gtk_widget_set_visible(notation_edit->hscrollbar,
			 FALSE);

  gtk_grid_attach(GTK_GRID(notation_edit),
		  (GtkWidget *) notation_edit->hscrollbar,
		  0, 2,
		  1, 1);

  /* auto-scroll */
  if(ags_notation_edit_auto_scroll == NULL){
    ags_notation_edit_auto_scroll = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							  NULL,
							  NULL);
  }

  g_hash_table_insert(ags_notation_edit_auto_scroll,
		      notation_edit, ags_notation_edit_auto_scroll_timeout);
  g_timeout_add(1000 / 30, (GSourceFunc) ags_notation_edit_auto_scroll_timeout, (gpointer) notation_edit);
}

void
ags_notation_edit_dispose(GObject *gobject)
{
  AgsNotationEdit *notation_edit;

  notation_edit = AGS_NOTATION_EDIT(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_notation_edit_parent_class)->dispose(gobject);
}

void
ags_notation_edit_finalize(GObject *gobject)
{
  AgsNotationEdit *notation_edit;

  notation_edit = AGS_NOTATION_EDIT(gobject);
  
  /* remove auto scroll */
  g_hash_table_remove(ags_notation_edit_auto_scroll,
		      notation_edit);

  /* call parent */
  G_OBJECT_CLASS(ags_notation_edit_parent_class)->finalize(gobject);
}

void
ags_notation_edit_connect(AgsConnectable *connectable)
{
  AgsNotationEdit *notation_edit;

  notation_edit = AGS_NOTATION_EDIT(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (notation_edit->connectable_flags)) != 0){
    return;
  }

  notation_edit->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  /* drawing area */
  gtk_drawing_area_set_draw_func(notation_edit->drawing_area,
				 ags_notation_edit_draw_callback,
				 notation_edit,
				 NULL);

  g_signal_connect_after((GObject *) notation_edit->drawing_area, "resize",
			 G_CALLBACK(ags_notation_edit_drawing_area_resize_callback), (gpointer) notation_edit);

  /* scrollbars */
  g_signal_connect_after((GObject *) gtk_scrollbar_get_adjustment(notation_edit->vscrollbar), "value-changed",
			 G_CALLBACK(ags_notation_edit_vscrollbar_value_changed), (gpointer) notation_edit);

  g_signal_connect_after((GObject *) gtk_scrollbar_get_adjustment(notation_edit->hscrollbar), "value-changed",
			 G_CALLBACK(ags_notation_edit_hscrollbar_value_changed), (gpointer) notation_edit);
}

void
ags_notation_edit_disconnect(AgsConnectable *connectable)
{
  AgsNotationEdit *notation_edit;

  notation_edit = AGS_NOTATION_EDIT(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (notation_edit->connectable_flags)) == 0){
    return;
  }

  notation_edit->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  /* drawing area */
  gtk_drawing_area_set_draw_func(notation_edit->drawing_area,
				 NULL,
				 NULL,
				 NULL);
  
  g_object_disconnect((GObject *) notation_edit->drawing_area,
		      "any_signal::resize",
		      G_CALLBACK(ags_notation_edit_drawing_area_resize_callback),
		      (gpointer) notation_edit,
		      NULL);

  /* scrollbars */
  g_object_disconnect(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar),
		      "any_signal::value-changed",
		      G_CALLBACK(ags_notation_edit_vscrollbar_value_changed),
		      (gpointer) notation_edit,
		      NULL);
  
  g_object_disconnect(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar),
		      "any_signal::value-changed",
		      G_CALLBACK(ags_notation_edit_hscrollbar_value_changed),
		      (gpointer) notation_edit,
		      NULL);
}

gboolean
ags_notation_edit_key_pressed_callback(GtkEventControllerKey *event_controller,
				       guint keyval,
				       guint keycode,
				       GdkModifierType state,
				       AgsNotationEdit *notation_edit)
{  
  GtkWidget *editor;
  AgsMachine *machine;

  AgsApplicationContext *application_context;

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

  application_context = ags_application_context_get_instance();

  machine = NULL;
  
  editor = gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
				   AGS_TYPE_COMPOSITE_EDITOR);
    
  machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;

#if defined(AGS_OSXAPI)
  l_control_key = AGS_NOTATION_EDIT_KEY_L_META;
  r_control_key = AGS_NOTATION_EDIT_KEY_R_META;
#else
  l_control_key = AGS_NOTATION_EDIT_KEY_L_CONTROL;
  r_control_key = AGS_NOTATION_EDIT_KEY_R_CONTROL;
#endif
  
  if(machine != NULL){
    switch(keyval){
    case GDK_KEY_Control_L:
    {
      notation_edit->key_mask |= AGS_NOTATION_EDIT_KEY_L_CONTROL;
    }
    break;
    case GDK_KEY_Control_R:
    {
      notation_edit->key_mask |= AGS_NOTATION_EDIT_KEY_R_CONTROL;
    }
    break;
    case GDK_KEY_Shift_L:
    {
      notation_edit->key_mask |= AGS_NOTATION_EDIT_KEY_L_SHIFT;
    }
    break;
    case GDK_KEY_Shift_R:
    {
      notation_edit->key_mask |= AGS_NOTATION_EDIT_KEY_R_SHIFT;
    }
    break;
    case GDK_KEY_Meta_L:
    {
      notation_edit->key_mask |= AGS_NOTATION_EDIT_KEY_L_META;
    }
    break;
    case GDK_KEY_Meta_R:
    {
      notation_edit->key_mask |= AGS_NOTATION_EDIT_KEY_R_META;
    }
    break;
    case GDK_KEY_a:
    {
      /* select all notes */
      if((l_control_key & (notation_edit->key_mask)) != 0 || (r_control_key & (notation_edit->key_mask)) != 0){
	ags_composite_editor_select_all(editor);
      }
    }
    break;
    case GDK_KEY_c:
    {
      /* copy notes */
      if((l_control_key & (notation_edit->key_mask)) != 0 || (r_control_key & (notation_edit->key_mask)) != 0){
	ags_composite_editor_copy(editor);
      }
    }
    break;
    case GDK_KEY_v:
    {
      /* paste notes */
      if((l_control_key & (notation_edit->key_mask)) != 0 || (r_control_key & (notation_edit->key_mask)) != 0){
	ags_composite_editor_paste(editor);
      }
    }
    break;
    case GDK_KEY_x:
    {
      /* cut notes */
      if((l_control_key & (notation_edit->key_mask)) != 0 || (r_control_key & (notation_edit->key_mask)) != 0){
	ags_composite_editor_cut(editor);
      }
    }
    break;
    case GDK_KEY_i:
    {
      /* invert notes */
      if((l_control_key & (notation_edit->key_mask)) != 0 || (r_control_key & (notation_edit->key_mask)) != 0){
	ags_composite_editor_invert(editor);
      }
    }
    break;
    case GDK_KEY_m:
    {
      AgsNotationMeta *notation_meta;

      /* meta */
      notation_meta = NULL;
      
      if((l_control_key & (notation_edit->key_mask)) != 0 || (r_control_key & (notation_edit->key_mask)) != 0){
	notation_meta = AGS_COMPOSITE_EDITOR(editor)->notation_edit->edit_meta;
	if((AGS_NOTATION_META_ENABLED & (notation_meta->flags)) != 0){
	  notation_meta->flags &= (~AGS_NOTATION_META_ENABLED);

	  gtk_widget_hide(notation_meta);
	}else{
	  notation_meta->flags |= AGS_NOTATION_META_ENABLED;

	  gtk_widget_show(notation_meta);

	  ags_notation_meta_refresh(notation_meta);
	}
      }
    }
    break;
    }
  }

  gtk_widget_queue_draw(notation_edit->drawing_area);

  return(key_handled);
}

gboolean
ags_notation_edit_key_released_callback(GtkEventControllerKey *event_controller,
					guint keyval,
					guint keycode,
					GdkModifierType state,
					AgsNotationEdit *notation_edit)
{  
  AgsMachine *machine;
  GtkWidget *editor;
  AgsNotebook *channel_selector;
  
  AgsApplicationContext *application_context;
  
  GtkAllocation allocation;

  guint l_control_key, r_control_key;  
  gboolean pattern_mode;
  double zoom_factor;
  gint i;
  gboolean do_feedback;
  gboolean key_handled;

  GRecMutex *audio_mutex;

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

  application_context = ags_application_context_get_instance();
  
  editor = gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
				   AGS_TYPE_COMPOSITE_EDITOR);

  channel_selector = AGS_COMPOSITE_EDITOR(editor)->notation_edit->channel_selector;
    
  machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;
    
  pattern_mode = (AGS_COMPOSITE_EDIT_PATTERN_MODE == AGS_COMPOSITE_EDITOR(editor)->notation_edit->edit_mode) ? TRUE: FALSE;    
  
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(AGS_COMPOSITE_EDITOR(editor)->toolbar)->zoom));

  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);

#if defined(AGS_OSXAPI)
  l_control_key = AGS_NOTATION_EDIT_KEY_L_META;
  r_control_key = AGS_NOTATION_EDIT_KEY_R_META;
#else
  l_control_key = AGS_NOTATION_EDIT_KEY_L_CONTROL;
  r_control_key = AGS_NOTATION_EDIT_KEY_R_CONTROL;
#endif

  if(machine != NULL){
    /* get audio mutex */
    audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(machine->audio);

    /* do feedback - initial set */
    do_feedback = FALSE;

    /* check key value */
    switch(keyval){
    case GDK_KEY_Control_L:
      {
	notation_edit->key_mask &= (~AGS_NOTATION_EDIT_KEY_L_CONTROL);
      }
      break;
    case GDK_KEY_Control_R:
      {
	notation_edit->key_mask &= (~AGS_NOTATION_EDIT_KEY_R_CONTROL);
      }
      break;
    case GDK_KEY_Shift_L:
      {
	notation_edit->key_mask &= (~AGS_NOTATION_EDIT_KEY_L_SHIFT);
      }
      break;
    case GDK_KEY_Shift_R:
      {
	notation_edit->key_mask &= (~AGS_NOTATION_EDIT_KEY_R_SHIFT);
      }
      break;
    case GDK_KEY_Meta_L:
      {
	notation_edit->key_mask &= (~AGS_NOTATION_EDIT_KEY_L_META);
      }
      break;
    case GDK_KEY_Meta_R:
      {
	notation_edit->key_mask &= (~AGS_NOTATION_EDIT_KEY_R_META);
      }
      break;
    case GDK_KEY_Left:
    case GDK_KEY_leftarrow:
      {
	if((AGS_NOTATION_EDIT_KEY_L_SHIFT & (notation_edit->key_mask)) != 0 ||
	   (AGS_NOTATION_EDIT_KEY_R_SHIFT & (notation_edit->key_mask)) != 0){
	  AgsTimestamp *timestamp;

	  if(pattern_mode){
	    return(key_handled);
	  }
	  
	  /* shrink note */
	  timestamp = ags_timestamp_new();

	  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
	  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

	  timestamp->timer.ags_offset.offset = AGS_NOTATION_DEFAULT_OFFSET * floor(notation_edit->cursor_position_x / AGS_NOTATION_DEFAULT_OFFSET);
	  
	  i = 0;
	  do_feedback = TRUE;
	
	  while((i = ags_notebook_next_active_tab(channel_selector,
						  i)) != -1){
	    GList *list_notation;
	    
	    g_rec_mutex_lock(audio_mutex);
	  
	    list_notation = ags_notation_find_near_timestamp(machine->audio->notation, i,
							     timestamp);
	    
	    if(list_notation != NULL){
	      AgsNote *note;
	      
	      note = ags_notation_find_point(AGS_NOTATION(list_notation->data),
					     notation_edit->cursor_position_x, notation_edit->cursor_position_y,
					     FALSE);

	      if(note != NULL &&
		 note->x[1] - note->x[0] - zoom_factor >= zoom_factor){
		note->x[1] -= zoom_factor;
	      }
	    }

	    g_rec_mutex_unlock(audio_mutex);
	  
	    i++;
	  }

	  g_object_unref(timestamp);
	}else{
	  gdouble x0_offset;

	  /* position cursor */
	  if(notation_edit->cursor_position_x > 0){
	    if(notation_edit->cursor_position_x - (zoom_factor) > 0){
	      notation_edit->cursor_position_x -= (zoom_factor);
	    }else{
	      notation_edit->cursor_position_x = 0;
	    }
	  
	    do_feedback = TRUE;
	  }

	  x0_offset = notation_edit->cursor_position_x * notation_edit->control_width;
      
	  if(x0_offset / zoom_factor < gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar))){
	    gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar),
				     x0_offset / zoom_factor);
	  }
	}
      }
      break;
    case GDK_KEY_Right:
    case GDK_KEY_rightarrow:
      {
	if((AGS_NOTATION_EDIT_KEY_L_SHIFT & (notation_edit->key_mask)) != 0 ||
	   (AGS_NOTATION_EDIT_KEY_R_SHIFT & (notation_edit->key_mask)) != 0){
	  AgsTimestamp *timestamp;

	  if(pattern_mode){
	    return(key_handled);
	  }

	  /* shrink note */
	  timestamp = ags_timestamp_new();

	  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
	  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

	  timestamp->timer.ags_offset.offset = AGS_NOTATION_DEFAULT_OFFSET * floor(notation_edit->cursor_position_x / AGS_NOTATION_DEFAULT_OFFSET);
	  
	  i = 0;
	  do_feedback = TRUE;
	
	  while((i = ags_notebook_next_active_tab(channel_selector,
						  i)) != -1){
	    GList *list_notation;
	    
	    g_rec_mutex_lock(audio_mutex);
	  
	    list_notation = ags_notation_find_near_timestamp(machine->audio->notation, i,
							     timestamp);
	    
	    if(list_notation != NULL){
	      AgsNote *note;
	      
	      note = ags_notation_find_point(AGS_NOTATION(list_notation->data),
					     notation_edit->cursor_position_x, notation_edit->cursor_position_y,
					     FALSE);

	      if(note != NULL){
		note->x[1] += zoom_factor;
	      }
	    }

	    g_rec_mutex_unlock(audio_mutex);
	  
	    i++;
	  }

	  g_object_unref(timestamp);
	}else{
	  gdouble x0_offset;
	  
	  /* position cursor */      
	  if(notation_edit->cursor_position_x < AGS_NAVIGATION_MAX_POSITION_TICS){
	    notation_edit->cursor_position_x += (zoom_factor);
	  
	    do_feedback = TRUE;
	  }

	  x0_offset = notation_edit->cursor_position_x * notation_edit->control_width;
      
	  if((x0_offset + notation_edit->control_width) / zoom_factor > gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar)) + allocation.width){
	    gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar),
				     x0_offset / zoom_factor);
	  }
	}
      }
      break;
    case GDK_KEY_Up:
    case GDK_KEY_uparrow:
      {
	gdouble y0_offset;
      
	if(notation_edit->cursor_position_y > 0){
	  notation_edit->cursor_position_y -= 1;
	
	  do_feedback = TRUE;
	}

	y0_offset = notation_edit->cursor_position_y * notation_edit->control_height;
      
	if(y0_offset < gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar))){
	  gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar),
				   y0_offset);
	}
      }
      break;
    case GDK_KEY_Down:
    case GDK_KEY_downarrow:
      {
	gdouble y0_offset;
      
	if(notation_edit->cursor_position_y > 0){
	  notation_edit->cursor_position_y += 1;
	
	  do_feedback = TRUE;
	}

	y0_offset = notation_edit->cursor_position_y * notation_edit->control_height;
      
	if(y0_offset < gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar))){
	  gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar),
				   y0_offset);
	}
      }
      break;
    case GDK_KEY_space:
      {
	AgsNote *note;
	
	do_feedback = TRUE;

	note = ags_note_new();

	note->x[0] = notation_edit->cursor_position_x;

	if(!pattern_mode){
	  note->x[1] = notation_edit->cursor_position_x + zoom_factor;
	}else{
	  note->x[1] = note->x[0] + 1;
	}
	
	note->y = notation_edit->cursor_position_y;

	/* add note */
	ags_composite_editor_add_note(editor,
				      note);
      }
      break;
    case GDK_KEY_Delete:
      {
	/* delete note */
	ags_composite_editor_delete_note(editor,
					 notation_edit->cursor_position_x, notation_edit->cursor_position_y);
      }
      break;
    }

    gtk_widget_queue_draw((GtkWidget *) notation_edit);

    /* do feedback */
    if(do_feedback){
      ags_composite_editor_do_feedback(editor);
    }
  }
  
  gtk_widget_queue_draw(notation_edit->drawing_area);
  
  return(key_handled);
}

gboolean
ags_notation_edit_modifiers_callback(GtkEventControllerKey *event_controller,
				     GdkModifierType keyval,
				     AgsNotationEdit *notation_edit)
{
  return(FALSE);
}

void
ags_notation_edit_drawing_area_motion_notify_position_cursor(GtkWidget *editor,
							     GtkWidget *toolbar,
							     AgsNotationEdit *notation_edit,
							     AgsMachine *machine,
							     gdouble x, gdouble y)
{
  AgsApplicationContext *application_context;
  
  double zoom_factor;

  application_context = ags_application_context_get_instance();

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));

  /* cursor position */
  notation_edit->cursor_position_x = (guint) (zoom_factor * (x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar)))) / notation_edit->control_width;
  notation_edit->cursor_position_x = zoom_factor * floor(notation_edit->cursor_position_x / zoom_factor);

  notation_edit->cursor_position_y = (guint) ((y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar))) / notation_edit->control_height);

#ifdef AGS_DEBUG
  g_message("%lu %lu", notation_edit->cursor_position_x, notation_edit->cursor_position_y);
#endif
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_notation_edit_drawing_area_motion_notify_add_note(GtkWidget *editor,
						      GtkWidget *toolbar,
						      AgsNotationEdit *notation_edit,
						      AgsMachine *machine,
						      gdouble x, gdouble y)
{
  AgsNote *note;
    
  AgsApplicationContext *application_context;
  
  gboolean pattern_mode;
  double zoom_factor;
  guint new_x;
    
  note = notation_edit->current_note;
    
  if(note == NULL){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* zoom */
  pattern_mode = (AGS_COMPOSITE_EDIT_PATTERN_MODE == AGS_COMPOSITE_EDITOR(editor)->notation_edit->edit_mode) ? TRUE: FALSE;
    
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));

  /* new x[1] */
  if(!pattern_mode){
    new_x = (guint) (zoom_factor * (x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar)))) / notation_edit->control_width;
    new_x = zoom_factor * floor((new_x + zoom_factor) / zoom_factor);
    
    if(new_x >= note->x[0] + zoom_factor){
      note->x[1] = new_x;
    }
  }else{
    note->x[1] = note->x[0] + 1;
  }
    
#ifdef AGS_DEBUG
  g_message("%lu-%lu %lu", note->x[0], note->x[1], note->y);
#endif
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_notation_edit_drawing_area_motion_notify_select_note(GtkWidget *editor,
							 GtkWidget *toolbar,
							 AgsNotationEdit *notation_edit,
							 AgsMachine *machine,
							 gdouble x, gdouble y)
{
  if(x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar)) >= 0.0){
    notation_edit->selection_x1 = (guint) x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar));
  }else{
    notation_edit->selection_x1 = 0.0;
  }
    
  if(y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar)) >= 0.0){
    notation_edit->selection_y1 = (guint) y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar));
  }else{
    notation_edit->selection_y1 = 0.0;
  }

  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

gboolean
ags_notation_edit_motion_callback(GtkEventControllerMotion *event_controller,
				  gdouble x,
				  gdouble y,
				  AgsNotationEdit *notation_edit)
{
  AgsCompositeToolbar *composite_toolbar;
  GtkWidget *editor;
  GtkWidget *toolbar;
  AgsMachine *machine;

  AgsApplicationContext *application_context;
  
  gboolean selected_position_cursor, selected_edit, selected_clear, selected_select;

  application_context = ags_application_context_get_instance();

  selected_position_cursor = FALSE;
  selected_edit = FALSE;
  selected_clear = FALSE;
  selected_select = FALSE;
    
  editor = gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
				   AGS_TYPE_COMPOSITE_EDITOR);
    
  toolbar = AGS_COMPOSITE_EDITOR(editor)->toolbar;

  machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;

  composite_toolbar = (AgsCompositeToolbar *) toolbar;

  selected_position_cursor = (composite_toolbar->selected_tool == composite_toolbar->position) ? TRUE: FALSE;
  selected_edit = (composite_toolbar->selected_tool == composite_toolbar->edit) ? TRUE: FALSE;
  selected_clear = (composite_toolbar->selected_tool == composite_toolbar->clear) ? TRUE: FALSE;
  selected_select = (composite_toolbar->selected_tool == composite_toolbar->select) ? TRUE: FALSE;

  gtk_widget_grab_focus((GtkWidget *) notation_edit->drawing_area);

  if(machine != NULL &&
     (AGS_NOTATION_EDIT_BUTTON_1 & (notation_edit->button_mask)) != 0){
    if(selected_position_cursor){
      ags_notation_edit_drawing_area_motion_notify_position_cursor(editor,
								   toolbar,
								   notation_edit,
								   machine,
								   x, y);
    }else if(selected_edit){
      ags_notation_edit_drawing_area_motion_notify_add_note(editor,
							    toolbar,
							    notation_edit,
							    machine,
							    x, y);
    }else if(selected_clear){
      //NOTE:JK: only takes action on release
    }else if(selected_select){
      ags_notation_edit_drawing_area_motion_notify_select_note(editor,
							       toolbar,
							       notation_edit,
							       machine,
							       x, y);
    }

    gtk_widget_queue_draw(notation_edit->drawing_area);
  }

  return(FALSE);
}

void
ags_notation_edit_drawing_area_button_press_position_cursor(GtkWidget *editor,
							    GtkWidget *toolbar,
							    AgsNotationEdit *notation_edit,
							    AgsMachine *machine,
							    gint n_press,
							    gdouble x, gdouble y)
{
  AgsApplicationContext *application_context;
  
  double zoom_factor;

  application_context = ags_application_context_get_instance();

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));
  
  /* cursor position */
  notation_edit->cursor_position_x = (guint) (zoom_factor * (x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar)))) / notation_edit->control_width;
  notation_edit->cursor_position_x = zoom_factor * floor(notation_edit->cursor_position_x / zoom_factor);
    
  notation_edit->cursor_position_y = (guint) ((y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar))) / notation_edit->control_height);

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_notation_edit_drawing_area_button_press_add_note(GtkWidget *editor,
						     GtkWidget *toolbar,
						     AgsNotationEdit *notation_edit,
						     AgsMachine *machine,
						     gint n_press,
						     gdouble x, gdouble y)
{
  AgsNote *note;

  AgsApplicationContext *application_context;
  
  gboolean pattern_mode;
  double zoom_factor;

  application_context = ags_application_context_get_instance();
    
  note = ags_note_new();

  /* zoom */
  pattern_mode = (AGS_COMPOSITE_EDIT_PATTERN_MODE == AGS_COMPOSITE_EDITOR(editor)->notation_edit->edit_mode) ? TRUE: FALSE;
    
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));

  /* note */
  note->x[0] = (guint) (zoom_factor * (x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar)))) / notation_edit->control_width;
  note->x[0] = zoom_factor * floor(note->x[0] / zoom_factor);

  if(!pattern_mode){
    note->x[1] = note->x[0] + zoom_factor;
  }else{
    note->x[1] = note->x[0] + 1;
  }
    
  note->y = (guint) ((y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar))) / notation_edit->control_height);

  /* current note */
  if(notation_edit->current_note != NULL){
    g_object_unref(notation_edit->current_note);

    notation_edit->current_note = NULL;
  }

  notation_edit->current_note = note;
  g_object_ref(note);

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_notation_edit_drawing_area_button_press_select_note(GtkWidget *editor,
							GtkWidget *toolbar,
							AgsNotationEdit *notation_edit,
							AgsMachine *machine,
							gint n_press,
							gdouble x, gdouble y)
{
  notation_edit->selection_x0 = (guint) x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar));
  notation_edit->selection_x1 = notation_edit->selection_x0;
    
  notation_edit->selection_y0 = (guint) y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar));
  notation_edit->selection_y1 = notation_edit->selection_y0;

  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_notation_edit_drawing_area_button_release_position_cursor(GtkWidget *editor,
							      GtkWidget *toolbar,
							      AgsNotationEdit *notation_edit,
							      AgsMachine *machine,
							      gint n_press,
							      gdouble x, gdouble y)
{
  AgsApplicationContext *application_context;
  
  double zoom_factor;

  application_context = ags_application_context_get_instance();
  
  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));

  /* cursor position */
  notation_edit->cursor_position_x = (guint) (zoom_factor * (x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar)))) / notation_edit->control_width;
  notation_edit->cursor_position_x = zoom_factor * floor(notation_edit->cursor_position_x / zoom_factor);

  notation_edit->cursor_position_y = (guint) ((y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar))) / notation_edit->control_height);
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_notation_edit_drawing_area_button_release_add_note(GtkWidget *editor,
						       GtkWidget *toolbar,
						       AgsNotationEdit *notation_edit,
						       AgsMachine *machine,
						       gint n_press,
						       gdouble x, gdouble y)
{
  AgsNote *note;
    
  AgsApplicationContext *application_context;

  gboolean pattern_mode;
  double zoom_factor;
  guint new_x;
    
  note = notation_edit->current_note;
    
  if(note == NULL){
    return;
  }

  application_context = ags_application_context_get_instance();
  
  /* zoom */
  pattern_mode = (AGS_COMPOSITE_EDIT_PATTERN_MODE == AGS_COMPOSITE_EDITOR(editor)->notation_edit->edit_mode) ? TRUE: FALSE;

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));

  /* new x[1] */
  if(!pattern_mode){
    new_x = (guint) (zoom_factor * (x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar)))) / notation_edit->control_width;
    new_x = zoom_factor * floor((new_x + zoom_factor) / zoom_factor);
    
    if(new_x >= note->x[0] + zoom_factor){
      note->x[1] = new_x;
    }
  }else{
    note->x[1] = note->x[0] + 1;
  }
    
#ifdef AGS_DEBUG
  g_message("%lu-%lu %lu", note->x[0], note->x[1], note->y);
#endif

  /* add note */
  ags_composite_editor_add_note(editor,
				note);
  
  notation_edit->current_note = NULL;
  g_object_unref(note);
}
  
void
ags_notation_edit_drawing_area_button_release_delete_note(GtkWidget *editor,
							  GtkWidget *toolbar,
							  AgsNotationEdit *notation_edit,
							  AgsMachine *machine,
							  gint n_press,
							  gdouble x, gdouble y)
{
  AgsApplicationContext *application_context;

  double zoom_factor;
  guint new_x, new_y;

  application_context = ags_application_context_get_instance();
  
  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));

  /* note */
  new_x = (guint) (zoom_factor * (x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar)))) / notation_edit->control_width;
  new_x = zoom_factor * floor(new_x / zoom_factor);
    
  new_y = (guint) ((y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar))) / notation_edit->control_height);

  /* delete note */
  ags_composite_editor_delete_note(editor,
				   new_x, new_y);
}

void
ags_notation_edit_drawing_area_button_release_select_note(GtkWidget *editor,
							  GtkWidget *toolbar,
							  AgsNotationEdit *notation_edit,
							  AgsMachine *machine,
							  gint n_press,
							  gdouble x, gdouble y)
{
  AgsApplicationContext *application_context;

  double zoom_factor;
  guint x0, x1, y0, y1;
    
  application_context = ags_application_context_get_instance();

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) AGS_COMPOSITE_TOOLBAR(toolbar)->zoom));

  /* region */
  x0 = (guint) (zoom_factor * notation_edit->selection_x0) / notation_edit->control_width;

  y0 = (guint) (notation_edit->selection_y0 / notation_edit->control_height);
    
  x1 = (guint) (zoom_factor * (x + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar)))) / notation_edit->control_width;
    
  y1 = (guint) ((y + gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar))) / notation_edit->control_height);

  /* select region */
  ags_composite_editor_select_region(editor,
				     x0, y0,
				     x1, y1);
}

gboolean
ags_notation_edit_gesture_click_pressed_callback(GtkGestureClick *event_controller,
						 gint n_press,
						 gdouble x,
						 gdouble y,
						 AgsNotationEdit *notation_edit)
{
  AgsCompositeToolbar *composite_toolbar;
  GtkWidget *editor;
  GtkWidget *toolbar;
  AgsMachine *machine;

  AgsApplicationContext *application_context;
  
  gboolean selected_position_cursor, selected_edit, selected_clear, selected_select;

  application_context = ags_application_context_get_instance();

  selected_position_cursor = FALSE;
  selected_edit = FALSE;
  selected_clear = FALSE;
  selected_select = FALSE;
      
  editor = gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
				   AGS_TYPE_COMPOSITE_EDITOR);
    
  toolbar = AGS_COMPOSITE_EDITOR(editor)->toolbar;

  machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;

  composite_toolbar = (AgsCompositeToolbar *) toolbar;
    
  selected_position_cursor = (composite_toolbar->selected_tool == composite_toolbar->position) ? TRUE: FALSE;
  selected_edit = (composite_toolbar->selected_tool == composite_toolbar->edit) ? TRUE: FALSE;
  selected_clear = (composite_toolbar->selected_tool == composite_toolbar->clear) ? TRUE: FALSE;
  selected_select = (composite_toolbar->selected_tool == composite_toolbar->select) ? TRUE: FALSE;
  
  gtk_widget_grab_focus((GtkWidget *) notation_edit->drawing_area);
  
  if(machine != NULL){
    notation_edit->button_mask |= AGS_NOTATION_EDIT_BUTTON_1;
    
    if(selected_position_cursor){
      notation_edit->mode = AGS_NOTATION_EDIT_POSITION_CURSOR;
      
      ags_notation_edit_drawing_area_button_press_position_cursor(editor,
								  toolbar,
								  notation_edit,
								  machine,
								  n_press,
								  x, y);
    }else if(selected_edit){
      notation_edit->mode = AGS_NOTATION_EDIT_ADD_NOTE;

      ags_notation_edit_drawing_area_button_press_add_note(editor,
							   toolbar,
							   notation_edit,
							   machine,
							   n_press,
							   x, y);
    }else if(selected_clear){
      notation_edit->mode = AGS_NOTATION_EDIT_DELETE_NOTE;

      //NOTE:JK: only takes action on release
    }else if(selected_select){
      notation_edit->mode = AGS_NOTATION_EDIT_SELECT_NOTE;

      ags_notation_edit_drawing_area_button_press_select_note(editor,
							      toolbar,
							      notation_edit,
							      machine,
							      n_press,
							      x, y);
    }

    gtk_widget_queue_draw(notation_edit->drawing_area);
  }
  
  return(FALSE);
}

gboolean
ags_notation_edit_gesture_click_released_callback(GtkGestureClick *event_controller,
						  gint n_press,
						  gdouble x,
						  gdouble y,
						  AgsNotationEdit *notation_edit)
{
  AgsCompositeToolbar *composite_toolbar;
  GtkWidget *editor;
  GtkWidget *toolbar;
  AgsMachine *machine;

  AgsApplicationContext *application_context;
  
  gboolean selected_position_cursor, selected_edit, selected_clear, selected_select;

  application_context = ags_application_context_get_instance();
  
  selected_position_cursor = FALSE;
  selected_edit = FALSE;
  selected_clear = FALSE;
  selected_select = FALSE;
    
  editor = gtk_widget_get_ancestor(GTK_WIDGET(notation_edit),
				   AGS_TYPE_COMPOSITE_EDITOR);
    
  toolbar = AGS_COMPOSITE_EDITOR(editor)->toolbar;

  machine = AGS_COMPOSITE_EDITOR(editor)->selected_machine;

  composite_toolbar = (AgsCompositeToolbar *) toolbar;
    
  selected_position_cursor = (composite_toolbar->selected_tool == composite_toolbar->position) ? TRUE: FALSE;
  selected_edit = (composite_toolbar->selected_tool == composite_toolbar->edit) ? TRUE: FALSE;
  selected_clear = (composite_toolbar->selected_tool == composite_toolbar->clear) ? TRUE: FALSE;
  selected_select = (composite_toolbar->selected_tool == composite_toolbar->select) ? TRUE: FALSE;

  if(machine != NULL){    
    notation_edit->button_mask &= (~AGS_NOTATION_EDIT_BUTTON_1);
    
    if(selected_position_cursor){
      ags_notation_edit_drawing_area_button_release_position_cursor(editor,
								    toolbar,
								    notation_edit,
								    machine,
								    n_press,
								    x, y);
      
      //      notation_edit->mode = AGS_NOTATION_EDIT_NO_EDIT_MODE;
    }else if(selected_edit){
      ags_notation_edit_drawing_area_button_release_add_note(editor,
							     toolbar,
							     notation_edit,
							     machine,
							     n_press,
							     x, y);

      notation_edit->mode = AGS_NOTATION_EDIT_NO_EDIT_MODE;
    }else if(selected_clear){
      ags_notation_edit_drawing_area_button_release_delete_note(editor,
								toolbar,
								notation_edit,
								machine,
								n_press,
								x, y);

      notation_edit->mode = AGS_NOTATION_EDIT_NO_EDIT_MODE;
    }else if(selected_select){
      ags_notation_edit_drawing_area_button_release_select_note(editor,
								toolbar,
								notation_edit,
								machine,
								n_press,
								x, y);

      notation_edit->mode = AGS_NOTATION_EDIT_NO_EDIT_MODE;
    }

    gtk_widget_queue_draw(notation_edit->drawing_area);
  }

  return(FALSE);
}

void
ags_notation_edit_realize(GtkWidget *widget)
{
  GdkFrameClock *frame_clock;
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_notation_edit_parent_class)->realize(widget);

#if 0
  frame_clock = gtk_widget_get_frame_clock(widget);
  
  g_signal_connect(frame_clock, "update", 
		   G_CALLBACK(ags_notation_edit_frame_clock_update_callback), widget);

  gdk_frame_clock_begin_updating(frame_clock);
#endif
}

void
ags_notation_edit_unrealize(GtkWidget *widget)
{
  GdkFrameClock *frame_clock;
  
#if 0
  frame_clock = gtk_widget_get_frame_clock(widget);
  
  g_object_disconnect(frame_clock,
		      "any_signal::update", 
		      G_CALLBACK(ags_notation_edit_frame_clock_update_callback),
		      widget,
		      NULL);

  gdk_frame_clock_end_updating(frame_clock);
#endif

  /* call parent */
  GTK_WIDGET_CLASS(ags_notation_edit_parent_class)->unrealize(widget);
}

void
ags_notation_edit_measure(GtkWidget *widget,
			  GtkOrientation orientation,
			  int for_size,
			  int *minimum,
			  int *natural,
			  int *minimum_baseline,
			  int *natural_baseline)
{
  AgsNotationEdit *notation_edit;

  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();

  notation_edit = (AgsNotationEdit *) widget;
  
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
ags_notation_edit_size_allocate(GtkWidget *widget,
				int width,
				int height,
				int baseline)
{
  AgsNotationEdit *notation_edit;

  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;

  notation_edit = (AgsNotationEdit *) widget;
  
  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  width = -1;
  height = (gint) (gui_scale_factor * AGS_SCALE_DEFAULT_HEIGHT_REQUEST);
  
  GTK_WIDGET_CLASS(ags_notation_edit_parent_class)->size_allocate(widget,
								  width,
								  height,
								  baseline);
}

void
ags_notation_edit_show(GtkWidget *widget)
{
  AgsNotationEdit *notation_edit;

  notation_edit = AGS_NOTATION_EDIT(widget);

  /* call parent */
  GTK_WIDGET_CLASS(ags_notation_edit_parent_class)->show(widget);

  ags_notation_edit_reset_vscrollbar(notation_edit);
  ags_notation_edit_reset_hscrollbar(notation_edit);
}

void
ags_notation_edit_frame_clock_update_callback(GdkFrameClock *frame_clock,
					      AgsNotationEdit *notation_edit)
{
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

gboolean
ags_notation_edit_auto_scroll_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_notation_edit_auto_scroll,
			 widget) != NULL){
    AgsCompositeEditor *composite_editor;  
    AgsNotationEdit *notation_edit;

    GtkAdjustment *hscrollbar_adjustment;

    AgsAudio *audio;
    
    AgsApplicationContext *application_context;

    GObject *output_soundcard;
      
    double x;
      
    notation_edit = AGS_NOTATION_EDIT(widget);

    if((AGS_NOTATION_EDIT_AUTO_SCROLL & (notation_edit->flags)) == 0){
      return(TRUE);
    }

    application_context = ags_application_context_get_instance();

    composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) notation_edit,
								      AGS_TYPE_COMPOSITE_EDITOR);
    
    if(composite_editor->selected_machine == NULL){
      return(TRUE);
    }

    audio = composite_editor->selected_machine->audio;      

    g_object_get(audio,
		 "output-soundcard", &output_soundcard,
		 NULL);    

    /* reset offset */
    notation_edit->note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));
    notation_edit->note_offset_absolute = ags_soundcard_get_note_offset_absolute(AGS_SOUNDCARD(output_soundcard));

    /* reset scrollbar */
    hscrollbar_adjustment = gtk_scrollbar_get_adjustment(notation_edit->hscrollbar);
    x = ((notation_edit->note_offset * notation_edit->control_width) / (AGS_NAVIGATION_MAX_POSITION_TICS * notation_edit->control_width)) * gtk_adjustment_get_upper(hscrollbar_adjustment);
    
    gtk_adjustment_set_value(hscrollbar_adjustment,
			     x);

    g_object_unref(output_soundcard);
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

void
ags_notation_edit_reset_vscrollbar(AgsNotationEdit *notation_edit)
{
  AgsCompositeEdit *composite_edit;
  GtkAdjustment *adjustment;
  GtkAdjustment *piano_adjustment;

  GtkAllocation allocation;
  
  AgsApplicationContext *application_context;
  
  guint key_count;
  double varea_height;
  gdouble upper, old_upper;
  
  if(!AGS_IS_NOTATION_EDIT(notation_edit) ||
     (AGS_NOTATION_EDIT_BLOCK_RESET_VSCROLLBAR & (notation_edit->flags)) != 0){
    return;
  }
  
  application_context = ags_application_context_get_instance();

  notation_edit->flags |= AGS_NOTATION_EDIT_BLOCK_RESET_VSCROLLBAR;

  composite_edit = (AgsCompositeEdit *) gtk_widget_get_ancestor((GtkWidget *) notation_edit,
								AGS_TYPE_COMPOSITE_EDIT);

  piano_adjustment = gtk_scrolled_window_get_vadjustment(AGS_SCROLLED_PIANO(composite_edit->edit_control)->scrolled_window);

  /* */
  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);
  
  /* adjustment */
  adjustment = gtk_scrollbar_get_adjustment(notation_edit->vscrollbar);

  /* get key count */
  key_count = notation_edit->key_count;

  /* upper */
  old_upper = gtk_adjustment_get_upper(adjustment); 

  varea_height = (key_count * notation_edit->control_height);
  upper = varea_height - allocation.height;

  if(upper < 0.0){
    upper = 0.0;
  }
	   
  gtk_adjustment_set_upper(adjustment,
			   upper);

  /* piano - upper */
  gtk_adjustment_set_lower(piano_adjustment,
			   gtk_adjustment_get_lower(adjustment));
  gtk_adjustment_set_step_increment(piano_adjustment,
				    gtk_adjustment_get_step_increment(adjustment));
  gtk_adjustment_set_page_increment(piano_adjustment,
				    gtk_adjustment_get_page_increment(adjustment));
  gtk_adjustment_set_page_size(piano_adjustment,
			       gtk_adjustment_get_page_size(adjustment));
  gtk_adjustment_set_upper(piano_adjustment,
			   gtk_adjustment_get_upper(adjustment));

  /* reset value */
  if(old_upper != 0.0){
    gtk_adjustment_set_value(adjustment,
			     gtk_adjustment_get_value(adjustment) / old_upper * upper);

    gtk_adjustment_set_value(piano_adjustment,
			     gtk_adjustment_get_value(adjustment));
  }

  notation_edit->flags &= (~AGS_NOTATION_EDIT_BLOCK_RESET_VSCROLLBAR);  
}

void
ags_notation_edit_reset_hscrollbar(AgsNotationEdit *notation_edit)
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
  
  if(!AGS_IS_NOTATION_EDIT(notation_edit) ||
     (AGS_NOTATION_EDIT_BLOCK_RESET_HSCROLLBAR & (notation_edit->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();

  notation_edit->flags |= AGS_NOTATION_EDIT_BLOCK_RESET_HSCROLLBAR;

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);

  /* adjustment */
  adjustment = gtk_scrollbar_get_adjustment(notation_edit->hscrollbar);

  /* zoom */
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) notation_edit,
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
  notation_edit->ruler->factor = zoom_factor;
  notation_edit->ruler->precision = zoom;
  notation_edit->ruler->scale_precision = 1.0 / zoom;

  gtk_adjustment_set_upper(notation_edit->ruler->adjustment,
			   upper);

  /* reset value */
  if(old_upper != 0.0){
    gtk_adjustment_set_value(adjustment,
			     gtk_adjustment_get_value(adjustment) / old_upper * upper);
  }

  notation_edit->flags &= (~AGS_NOTATION_EDIT_BLOCK_RESET_HSCROLLBAR);  
}

void
ags_notation_edit_draw_segment(AgsNotationEdit *notation_edit, cairo_t *cr)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *toolbar;
  
  GtkStyleContext *style_context;
  GtkSettings *settings;

  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;

  AgsApplicationContext *application_context;

  GtkAllocation allocation;

  GdkRGBA fg_color;
  GdkRGBA bg_color;
  GdkRGBA shadow_color;

  guint key_count;
  guint width, height;
  gboolean width_fits, height_fits;
  double zoom;
  guint y0, x0;
  guint nth_x;
  guint i, j;
  guint j_set;
  gboolean dark_theme;
  gboolean fg_success;
  gboolean bg_success;
  gboolean shadow_success;

  GValue value = {0,};

  const static double segment_dashes = {
    0.5,
  };

  if(!AGS_IS_NOTATION_EDIT(notation_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();
  
  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);
  
  /* style context */
  style_context = gtk_widget_get_style_context((GtkWidget *) notation_edit->drawing_area);  

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

  /* adjustment */
  vscrollbar_adjustment = gtk_scrollbar_get_adjustment(notation_edit->vscrollbar);
  hscrollbar_adjustment = gtk_scrollbar_get_adjustment(notation_edit->hscrollbar);
  
  /* get key count */
  key_count = notation_edit->key_count;
    
  /* get width */
  width = allocation.width;
  width_fits = FALSE;
  
  if(AGS_NAVIGATION_MAX_POSITION_TICS * notation_edit->control_width < width){
    width = AGS_NAVIGATION_MAX_POSITION_TICS * notation_edit->control_width;
    width_fits = TRUE;
  }

  /* get height */
  height = allocation.height;
  height_fits = FALSE;

  if(key_count * notation_edit->control_height < height){
    height = key_count * notation_edit->control_height;
    height_fits = TRUE;
  }

  /* zoom */
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) notation_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  toolbar = composite_editor->toolbar;

  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) toolbar->zoom) - 2.0);

  /*  */
  if(width_fits){
    x0 = 0;
  }else{
    x0 = notation_edit->control_width - ((guint) gtk_adjustment_get_value(hscrollbar_adjustment) % notation_edit->control_width);
  }

  if(height_fits){
    y0 = 0;
  }else{
    y0 = notation_edit->control_height - ((guint) gtk_adjustment_get_value(vscrollbar_adjustment) % notation_edit->control_height);
  }
  
  nth_x = (guint) floor(gtk_adjustment_get_value(hscrollbar_adjustment) / notation_edit->control_width);
  nth_x += 1;
  
  /* push group */
  cairo_push_group(cr);

  /* clear with background color */
  cairo_set_source_rgba(cr,
			bg_color.red,
			bg_color.green,
			bg_color.blue,
			bg_color.alpha);
  cairo_rectangle(cr,
		  0.0, 0.0,
		  (double) width, (double) height);
  cairo_fill(cr);
  
  /* horizontal lines */
  cairo_set_line_width(cr,
		       1.0);

  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.blue,
			fg_color.green,
			fg_color.alpha);

  for(i = y0 ; i < height; ){
    cairo_move_to(cr,
		  0.0, (double) i);
    cairo_line_to(cr,
		  (double) width, (double) i);
    cairo_stroke(cr);

    i += notation_edit->control_height;
  }

  if(height_fits){
    cairo_move_to(cr,
		  0.0, (double) i);
    cairo_line_to(cr,
		  (double) width, (double) i);
    cairo_stroke(cr);
  }

  /* vertical lines */
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.blue,
			fg_color.green,
			fg_color.alpha);

  i = x0;
  
  if(i < width &&
     zoom > 1.0 ){
    j_set = nth_x % ((guint) zoom);

    /* thin lines */
    cairo_set_dash(cr,
		   &segment_dashes,
		   1,
		   0.0);

    if(j_set != 0){
      j = j_set;
      goto ags_notation_edit_draw_segment0;
    }
  }

  for(; i < width; ){
    /* strong lines */
    cairo_set_dash(cr,
		   NULL,
		   0,
		   0.0);
    
    cairo_move_to(cr,
		  (double) i, 0.0);
    cairo_line_to(cr,
		  (double) i, (double) height);
    cairo_stroke(cr);
    
    i += notation_edit->control_width;
    
    /* thin lines */
    cairo_set_dash(cr,
		   &segment_dashes,
		   1,
		   0.0);
    
    for(j = 1; i < width && j < zoom; j++){
    ags_notation_edit_draw_segment0:
      cairo_move_to(cr, (double) i, 0.0);
      cairo_line_to(cr, (double) i, (double) height);
      cairo_stroke(cr);
      
      i += notation_edit->control_width;
    }
  }

  /* complete */
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
  
//  cairo_surface_mark_dirty(cairo_get_target(cr));
}

void
ags_notation_edit_draw_position(AgsNotationEdit *notation_edit, cairo_t *cr)
{
  AgsMachine *selected_machine;
  AgsCompositeEditor *composite_editor;
  
  GtkStyleContext *style_context;
  GtkSettings *settings;

  AgsApplicationContext *application_context;

  GtkAllocation allocation;

  GdkRGBA fg_color;

  guint channel_count;
  double position;
  double x, y;
  double width, height;
  gboolean height_fits;
  gboolean dark_theme;
  gboolean fg_success;

  GValue value = {0,};

  if(!AGS_IS_NOTATION_EDIT(notation_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();

  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);

  style_context = gtk_widget_get_style_context((GtkWidget *) notation_edit->drawing_area);

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
  
  /* get channel count */
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) notation_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  if(composite_editor->selected_machine == NULL){
    return;
  }

  selected_machine = composite_editor->selected_machine;

  g_object_get(selected_machine->audio,
	       "input-pads", &channel_count,
	       NULL);
  
  /* get offset and dimensions */
  position = ((double) notation_edit->note_offset) * ((double) notation_edit->control_width);
  
  y = 0.0;
  x = (position) - (gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar)));

  height = (double) allocation.height;
  width = (double) AGS_NOTATION_EDIT_DEFAULT_FADER_WIDTH;

  if(height < channel_count * notation_edit->control_height){
    height = channel_count * notation_edit->control_height;
  }

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
ags_notation_edit_draw_cursor(AgsNotationEdit *notation_edit, cairo_t *cr)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *toolbar;

  GtkStyleContext *style_context;
  GtkSettings *settings;

  AgsApplicationContext *application_context;

  GtkAllocation allocation;
  
  GdkRGBA fg_color;

  double zoom_factor;
  double x, y;
  double width, height;
  gboolean dark_theme;
  gboolean fg_success;

  GValue value = {0,};

  if(!AGS_IS_NOTATION_EDIT(notation_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();

  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);

  /* style context */
  style_context = gtk_widget_get_style_context((GtkWidget *) notation_edit->drawing_area);

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
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) notation_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  toolbar = composite_editor->toolbar;

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) toolbar->zoom));

  /* get offset */
  x = ((double) notation_edit->cursor_position_x * (double) notation_edit->control_width) - (gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar)) * zoom_factor);
  y = ((double) notation_edit->cursor_position_y * (double) notation_edit->control_height) - gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar));

  width = (double) notation_edit->control_width;
  height = (double) notation_edit->control_height;

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
ags_notation_edit_draw_selection(AgsNotationEdit *notation_edit, cairo_t *cr)
{
  GtkStyleContext *style_context;
  GtkSettings *settings;

  GdkRGBA fg_color;

  GtkAllocation allocation;
  
  double x, y;
  double width, height;
  gboolean dark_theme;
  gboolean fg_success;

  GValue value = {0,};

  if(!AGS_IS_NOTATION_EDIT(notation_edit)){
    return;
  }

  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);

  style_context = gtk_widget_get_style_context((GtkWidget *) notation_edit->drawing_area);

  settings = gtk_settings_get_default();
  
  dark_theme = TRUE;
  
  g_object_get(settings,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

  if(!fg_success){
    gdk_rgba_parse(&fg_color,
		   "#101010");
  }

  /* colors */
  fg_success = gtk_style_context_lookup_color(style_context,
					      "theme_fg_color",
					      &fg_color);

  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);

  /* get offset and dimensions */
  if(notation_edit->selection_x0 < notation_edit->selection_x1){
    x = ((double) notation_edit->selection_x0) - gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar));
    width = ((double) notation_edit->selection_x1 - (double) notation_edit->selection_x0);
  }else{
    x = ((double) notation_edit->selection_x1) - gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar));
    width = ((double) notation_edit->selection_x0 - (double) notation_edit->selection_x1);
  }

  if(notation_edit->selection_y0 < notation_edit->selection_y1){
    y = ((double) notation_edit->selection_y0) - gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar));
    height = ((double) notation_edit->selection_y1 - (double) notation_edit->selection_y0);
  }else{
    y = ((double) notation_edit->selection_y1) - gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar));
    height = ((double) notation_edit->selection_y0 - (double) notation_edit->selection_y1);
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
ags_notation_edit_draw_note(AgsNotationEdit *notation_edit,
			    AgsNote *note,
			    cairo_t *cr,
			    gdouble opacity)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *toolbar;
  AgsMachine *selected_machine;
  
  GtkStyleContext *style_context;
  GtkSettings *settings;

  AgsApplicationContext *application_context;

  GtkAllocation allocation;

  GdkRGBA fg_color;
  GdkRGBA highlight_color;
  
  double zoom_factor;
  guint channel_count;
  double viewport_x, viewport_y;
  double x, y;
  double width, height;
  gboolean dark_theme;
  gboolean fg_success;
  gboolean highlight_success;

  GValue value = {0,};
  
  if(!AGS_IS_NOTATION_EDIT(notation_edit) ||
     !AGS_IS_NOTE(note)){
    return;
  }

  application_context = ags_application_context_get_instance();
  
  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);

  /* style context */
  style_context = gtk_widget_get_style_context((GtkWidget *) notation_edit->drawing_area);

  settings = gtk_settings_get_default();

  dark_theme = TRUE;
  
  g_object_get(settings,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

  /* colors */
  fg_success = gtk_style_context_lookup_color(style_context,
					      "theme_fg_color",
					      &fg_color);
    
  highlight_success = gtk_style_context_lookup_color(style_context,
						     "theme_highlight_color",
						     &highlight_color);

  if(!fg_success ||
     !highlight_success){
    gdk_rgba_parse(&fg_color,
		   "#101010");

    gdk_rgba_parse(&highlight_color,
		   "#00000040");
  }

  /* get channel count */
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) notation_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  if(composite_editor->selected_machine == NULL){
    return;
  }

  selected_machine = composite_editor->selected_machine;

  g_object_get(selected_machine->audio,
	       "input-pads", &channel_count,
	       NULL);
  
  /* zoom */  
  toolbar = composite_editor->toolbar;

  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) toolbar->zoom));

  /* get offset and dimensions */
  if((AGS_NAVIGATION_MAX_POSITION_TICS * notation_edit->control_width) > allocation.width){
    viewport_x = zoom_factor * gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar));
  }else{
    viewport_x = 0.0;
  }
  
  if((channel_count * notation_edit->control_height) > allocation.height){
    viewport_y = gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->vscrollbar));
  }else{
    viewport_y = 0.0;
  }
  
  x = ((double) note->x[0]) * ((double) notation_edit->control_width) - viewport_x;
  y = ((double) note->y) * ((double) notation_edit->control_height) - viewport_y;

  width = ((double) (note->x[1] - note->x[0])) * ((double) notation_edit->control_width);
  height = ((double) notation_edit->control_height);

  /* apply zoom */
  x /= zoom_factor;
  x += ((double) notation_edit->control_margin_x);

  y += ((double) notation_edit->control_margin_y);
  
  width /= zoom_factor;
  width -= (2.0 * (double) notation_edit->control_margin_x);
  
  height -= (2.0 * (double) notation_edit->control_margin_y);
  
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
  
  /* draw note */
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.blue,
			fg_color.green,
			opacity * fg_color.alpha);
  
  cairo_rectangle(cr,
		  x, y,
		  width, height);
  cairo_fill(cr);

  /* draw note shadow */
  cairo_set_line_width(cr, 2.0);

  cairo_set_source_rgba(cr,
			highlight_color.red,
			highlight_color.blue,
			highlight_color.green,
			opacity * highlight_color.alpha);
  
  cairo_rectangle(cr,
		  x, y,
		  width, height);
  cairo_stroke(cr);
  
  /* check note selected */
  if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
    double selected_x, selected_y;
    double selected_width, selected_height;

    selected_x = x - notation_edit->selected_note_border;
    selected_y = y - notation_edit->selected_note_border;

    selected_width = width + (2.0 * (double) notation_edit->selected_note_border);
    selected_height = height + (2.0 * (double) notation_edit->selected_note_border);

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

    /* draw selected note */
    cairo_set_source_rgba(cr,
			  highlight_color.red,
			  highlight_color.blue,
			  highlight_color.green,
			  opacity / 3.0);
    
    cairo_rectangle(cr,
		    selected_x, selected_y,
		    selected_width, selected_height);
    cairo_fill(cr);
  }
}

void
ags_notation_edit_draw_notation(AgsNotationEdit *notation_edit, cairo_t *cr)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *toolbar;
  AgsMachine *selected_machine;

  AgsNotebook *notebook;
  
  GtkAllocation allocation;
  
  AgsTimestamp *timestamp;
  AgsTimestamp *current_timestamp;    
  
  AgsApplicationContext *application_context;

  GList *start_list_notation, *list_notation;

  gdouble opacity;
  gdouble zoom, zoom_factor;
  guint x0, x1;
  guint offset;
  guint audio_channel;
  gint i;    
  
  if(!AGS_IS_NOTATION_EDIT(notation_edit)){
    return;
  }

  application_context = ags_application_context_get_instance();

  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);

  /* zoom */
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) notation_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  notebook = composite_editor->notation_edit->channel_selector;
    
  toolbar = composite_editor->toolbar;

  selected_machine = composite_editor->selected_machine;
    
  opacity = gtk_spin_button_get_value(toolbar->opacity);

  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) toolbar->zoom) - 2.0);
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) toolbar->zoom));

  if(selected_machine == NULL){
    return;
  }
  
  /* get visisble region */
  x0 = (zoom_factor * gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar))) / notation_edit->control_width;
  x1 = ((zoom_factor * gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(notation_edit->hscrollbar))) / notation_edit->control_width) + (allocation.width * zoom);
  
  /* draw notation */
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  g_object_get(selected_machine->audio,
	       "notation", &start_list_notation,
	       NULL);

  timestamp->timer.ags_offset.offset = (guint64) AGS_NOTATION_DEFAULT_OFFSET * floor((double) x0 / (double) AGS_NOTATION_DEFAULT_OFFSET);

  i = 0;
  
  while((i = ags_notebook_next_active_tab(notebook,
					  i)) != -1){
    list_notation = ags_notation_find_near_timestamp(start_list_notation, i,
						     timestamp);
    
    while(list_notation != NULL){
      AgsNotation *notation;

      GList *start_list_note, *list_note;

      notation = AGS_NOTATION(list_notation->data);

      g_object_get(notation,
		   "audio-channel", &audio_channel,
		   "timestamp", &current_timestamp,
		   NULL);

      g_object_unref(current_timestamp);

      if(ags_timestamp_get_ags_offset(current_timestamp) > AGS_NOTATION_DEFAULT_OFFSET * floor((double) x1 / (double) AGS_NOTATION_DEFAULT_OFFSET) + AGS_NOTATION_DEFAULT_OFFSET){
	break;
      }

      if(ags_timestamp_get_ags_offset(current_timestamp) + AGS_NOTATION_DEFAULT_OFFSET < x0){
	list_notation = list_notation->next;

	continue;
      }
      
      if(i != audio_channel){
	list_notation = list_notation->next;

	continue;
      }

      g_object_get(notation,
		   "note", &start_list_note,
		   NULL);
      
      list_note = start_list_note;

      while(list_note != NULL){
	ags_notation_edit_draw_note(notation_edit,
				    list_note->data,
				    cr,
				    opacity);

	list_note = list_note->next;
      }

      g_list_free_full(start_list_note,
		       g_object_unref);
      
      list_notation = list_notation->next;
    }
    
    i++;
  }

  g_list_free_full(start_list_notation,
		   g_object_unref);

  g_object_unref(timestamp);
}

void
ags_notation_edit_draw(AgsNotationEdit *notation_edit, cairo_t *cr)
{
  /* segment */
  ags_notation_edit_draw_segment(notation_edit, cr);

  /* notation */
  ags_notation_edit_draw_notation(notation_edit, cr);
  
  /* edit mode */
  switch(notation_edit->mode){
  case AGS_NOTATION_EDIT_POSITION_CURSOR:
    {
      ags_notation_edit_draw_cursor(notation_edit, cr);
    }
    break;
  case AGS_NOTATION_EDIT_ADD_NOTE:
    {
      if(notation_edit->current_note != NULL){
	if(cr != NULL){
	  ags_notation_edit_draw_note(notation_edit,
				      notation_edit->current_note,
				      cr,
				      1.0);

	  cairo_surface_mark_dirty(cairo_get_target(cr));
	}
      }
    }
    break;
  case AGS_NOTATION_EDIT_SELECT_NOTE:
    {
      ags_notation_edit_draw_selection(notation_edit, cr);
    }
    break;
  }

  /* fader */
  if((AGS_NOTATION_EDIT_AUTO_SCROLL & (notation_edit->flags)) != 0){
    ags_notation_edit_draw_position(notation_edit, cr);
  }
}

/**
 * ags_notation_edit_new:
 *
 * Create a new #AgsNotationEdit.
 *
 * Returns: a new #AgsNotationEdit
 * 
 * Since: 3.0.0
 */
AgsNotationEdit*
ags_notation_edit_new()
{
  AgsNotationEdit *notation_edit;

  notation_edit = (AgsNotationEdit *) g_object_new(AGS_TYPE_NOTATION_EDIT,
						   NULL);

  return(notation_edit);
}
