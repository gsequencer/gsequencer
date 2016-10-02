/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/editor/ags_note_edit.h>
#include <ags/X/editor/ags_note_edit_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/recall/ags_count_beats_audio_run.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_editor.h>

#include <ags/X/editor/ags_note_edit.h>

#include <gdk/gdkkeysyms.h>

#include <atk/atk.h>

#include <math.h>

static GType ags_accessible_note_edit_get_type(void);
void ags_note_edit_class_init(AgsNoteEditClass *note_edit);
void ags_accessible_note_edit_class_init(AtkObject *object);
void ags_accessible_note_edit_action_interface_init(AtkActionIface *action);
void ags_note_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_note_edit_init(AgsNoteEdit *note_edit);
void ags_note_edit_connect(AgsConnectable *connectable);
void ags_note_edit_disconnect(AgsConnectable *connectable);
AtkObject* ags_note_edit_get_accessible(GtkWidget *widget);

gboolean ags_accessible_note_edit_do_action(AtkAction *action,
					    gint i);
gint ags_accessible_note_edit_get_n_actions(AtkAction *action);
const gchar* ags_accessible_note_edit_get_description(AtkAction *action,
						      gint i);
const gchar* ags_accessible_note_edit_get_name(AtkAction *action,
					       gint i);
const gchar* ags_accessible_note_edit_get_keybinding(AtkAction *action,
						     gint i);
gboolean ags_accessible_note_edit_set_description(AtkAction *action,
						  gint i);
gchar* ags_accessible_note_edit_get_localized_name(AtkAction *action,
						   gint i);

/**
 * SECTION:ags_note_edit
 * @short_description: edit notes
 * @title: AgsNoteEdit
 * @section_id:
 * @include: ags/X/editor/ags_note_edit.h
 *
 * The #AgsNoteEdit lets you edit notes.
 */

GtkStyle *note_edit_style;

static GQuark quark_accessible_object = 0;

GType
ags_note_edit_get_type(void)
{
  static GType ags_type_note_edit = 0;

  if(!ags_type_note_edit){
    static const GTypeInfo ags_note_edit_info = {
      sizeof (AgsNoteEditClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_note_edit_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsNoteEdit),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_note_edit_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_note_edit_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_note_edit = g_type_register_static(GTK_TYPE_TABLE,
						"AgsNoteEdit\0", &ags_note_edit_info,
						0);
    
    g_type_add_interface_static(ags_type_note_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_note_edit);
}

static GType
ags_accessible_note_edit_get_type(void)
{
  static GType ags_type_accessible_note_edit = 0;

  if(!ags_type_accessible_note_edit){
    const GTypeInfo ags_accesssible_note_edit_info = {
      sizeof(GtkAccessibleClass),
      NULL,           /* base_init */
      NULL,           /* base_finalize */
      (GClassInitFunc) ags_accessible_note_edit_class_init,
      NULL,           /* class_finalize */
      NULL,           /* class_data */
      sizeof(GtkAccessible),
      0,             /* n_preallocs */
      NULL, NULL
    };

    static const GInterfaceInfo atk_action_interface_info = {
      (GInterfaceInitFunc) ags_accessible_note_edit_action_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_accessible_note_edit = g_type_register_static(GTK_TYPE_ACCESSIBLE,
							   "AgsAccessibleNoteEdit\0", &ags_accesssible_note_edit_info,
							   0);

    g_type_add_interface_static(ags_type_accessible_note_edit,
				ATK_TYPE_ACTION,
				&atk_action_interface_info);
  }
  
  return(ags_type_accessible_note_edit);
}

void
ags_note_edit_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_note_edit_connect;
  connectable->disconnect = ags_note_edit_disconnect;
}

void
ags_note_edit_class_init(AgsNoteEditClass *note_edit)
{
  quark_accessible_object = g_quark_from_static_string("ags-accessible-object\0");
}

void
ags_accessible_note_edit_class_init(AtkObject *object)
{
  /* empty */
}

void
ags_accessible_note_edit_action_interface_init(AtkActionIface *action)
{
  action->do_action = ags_accessible_note_edit_do_action;
  action->get_n_actions = ags_accessible_note_edit_get_n_actions;
  action->get_description = ags_accessible_note_edit_get_description;
  action->get_name = ags_accessible_note_edit_get_name;
  action->get_keybinding = ags_accessible_note_edit_get_keybinding;
  action->set_description = ags_accessible_note_edit_set_description;
  action->get_localized_name = ags_accessible_note_edit_get_localized_name;
}

void
ags_note_edit_init(AgsNoteEdit *note_edit)
{
  GtkAdjustment *adjustment;

  note_edit->key_mask = 0;
  
  note_edit->flags = 0;

  note_edit->ruler = ags_ruler_new();
  gtk_table_attach(GTK_TABLE(note_edit),
		   (GtkWidget *) note_edit->ruler,
		   0, 1,
		   0, 1,
		   GTK_FILL|GTK_EXPAND,
		   GTK_FILL,
		   0, 0);

  note_edit->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_style((GtkWidget *) note_edit->drawing_area,
		       note_edit_style);
  gtk_widget_set_events(GTK_WIDGET(note_edit->drawing_area), GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			| GDK_CONTROL_MASK
			| GDK_KEY_PRESS_MASK
			| GDK_KEY_RELEASE_MASK);
  gtk_widget_set_can_focus((GtkWidget *) note_edit->drawing_area,
			   TRUE);
  
  gtk_table_attach(GTK_TABLE(note_edit),
		   (GtkWidget *) note_edit->drawing_area,
		   0, 1,
		   1, 2,
		   GTK_FILL|GTK_EXPAND,
		   GTK_FILL|GTK_EXPAND,
		   0, 0);

  note_edit->control.note = ags_note_new();

  note_edit->width = 0;
  note_edit->height = 0;
  note_edit->map_width = AGS_NOTE_EDIT_MAX_CONTROLS * 16 * 64;
  note_edit->map_height = 78;

  note_edit->control_height = 14;
  note_edit->control_margin_y = 2;

  note_edit->control_width = 16;

  note_edit->y0 = 0;
  note_edit->y1 = 0;

  note_edit->nth_y = 0;
  note_edit->stop_y = 0;

  /* AgsNoteEditControlCurrent is used by ags_note_edit_draw_segment */
  note_edit->control_current.control_count = AGS_NOTE_EDIT_MAX_CONTROLS;
  note_edit->control_current.control_width = 64;

  note_edit->control_current.x0 = 0;
  note_edit->control_current.x1 = 0;

  note_edit->control_current.nth_x = 0;

  /* AgsNoteEditControlUnit is used by ags_note_edit_draw_notation */
  note_edit->control_unit.control_count = 16 * AGS_NOTE_EDIT_MAX_CONTROLS;
  note_edit->control_unit.control_width = 1 * 4;

  note_edit->control_unit.x0 = 0;
  note_edit->control_unit.x1 = 0;

  note_edit->control_unit.nth_x = 0;
  note_edit->control_unit.stop_x = 0;

  /* offset for pasting from clipboard */
  note_edit->selected_x = 0;
  note_edit->selected_y = 0;

  /* GtkScrollbars */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 16.0, 1.0);
  note_edit->vscrollbar = (GtkVScrollbar *) gtk_vscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(note_edit),
		   (GtkWidget *) note_edit->vscrollbar,
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (gdouble) note_edit->control_current.control_width, 1.0);
  note_edit->hscrollbar = (GtkHScrollbar *) gtk_hscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(note_edit),
		   (GtkWidget *) note_edit->hscrollbar,
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_note_edit_connect(AgsConnectable *connectable)
{
  AgsEditor *editor;
  AgsNoteEdit *note_edit;

  note_edit = AGS_NOTE_EDIT(connectable);

  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(note_edit),
						 AGS_TYPE_EDITOR);

  if(editor != NULL && editor->selected_machine != NULL){
    g_signal_connect_after(editor->selected_machine->audio, "set-audio-channels\0",
			   G_CALLBACK(ags_note_edit_set_audio_channels_callback), note_edit);
    g_signal_connect_after(editor->selected_machine->audio, "set-pads\0",
			   G_CALLBACK(ags_note_edit_set_pads_callback), note_edit);
  }
  
  g_signal_connect_after((GObject *) note_edit->drawing_area, "expose_event\0",
			 G_CALLBACK(ags_note_edit_drawing_area_expose_event), (gpointer) note_edit);

  g_signal_connect_after((GObject *) note_edit->drawing_area, "configure_event\0",
			 G_CALLBACK(ags_note_edit_drawing_area_configure_event), (gpointer) note_edit);

  g_signal_connect((GObject *) note_edit->drawing_area, "button_press_event\0",
		   G_CALLBACK(ags_note_edit_drawing_area_button_press_event), (gpointer) note_edit);

  g_signal_connect((GObject *) note_edit->drawing_area, "button_release_event\0",
		   G_CALLBACK(ags_note_edit_drawing_area_button_release_event), (gpointer) note_edit);

  g_signal_connect((GObject *) note_edit->drawing_area, "motion_notify_event\0",
		   G_CALLBACK(ags_note_edit_drawing_area_motion_notify_event), (gpointer) note_edit);
			
  g_signal_connect((GObject *) note_edit->drawing_area, "key_press_event\0",
		   G_CALLBACK(ags_note_edit_drawing_area_key_press_event), (gpointer) note_edit);

  g_signal_connect((GObject *) note_edit->drawing_area, "key_release_event\0",
		   G_CALLBACK(ags_note_edit_drawing_area_key_release_event), (gpointer) note_edit);

  g_signal_connect_after((GObject *) note_edit->vscrollbar, "value-changed\0",
			 G_CALLBACK(ags_note_edit_vscrollbar_value_changed), (gpointer) note_edit);

  g_signal_connect_after((GObject *) note_edit->hscrollbar, "value-changed\0",
			 G_CALLBACK(ags_note_edit_hscrollbar_value_changed), (gpointer) note_edit);
}

void
ags_note_edit_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

AtkObject*
ags_note_edit_get_accessible(GtkWidget *widget)
{
  AtkObject* accessible;

  accessible = g_object_get_qdata(G_OBJECT(widget),
				  quark_accessible_object);
  
  if(!accessible){
    accessible = g_object_new(ags_accessible_note_edit_get_type(),
			      NULL);
    
    g_object_set_qdata(G_OBJECT(widget),
		       quark_accessible_object,
		       accessible);
    gtk_accessible_set_widget(GTK_ACCESSIBLE(accessible),
			      widget);
  }
  
  return(accessible);
}

gboolean
ags_accessible_note_edit_do_action(AtkAction *action,
				   gint i)
{
  AgsNoteEdit *note_edit;
  
  GdkEventKey *key_press, *key_release;
  GdkEventKey *modifier_press, *modifier_release;
  GdkEventKey *second_level_press, *second_level_release;
  
  if(!(i >= 0 && i < 13)){
    return(FALSE);
  }

  note_edit = gtk_accessible_get_widget(GTK_ACCESSIBLE(action));
  
  key_press = (GdkEventKey *) gdk_event_new(GDK_KEY_PRESS);
  key_release = (GdkEventKey *) gdk_event_new(GDK_KEY_RELEASE);

  /* create modifier */
  modifier_press = (GdkEventKey *) gdk_event_new(GDK_KEY_PRESS);
  modifier_release = (GdkEventKey *) gdk_event_new(GDK_KEY_RELEASE);
  
  modifier_press->keyval =
    modifier_release->keyval = GDK_KEY_Control_R;

  /* create second level */
  second_level_press = (GdkEventKey *) gdk_event_new(GDK_KEY_PRESS);
  second_level_release = (GdkEventKey *) gdk_event_new(GDK_KEY_RELEASE);
  
  second_level_press->keyval =
    second_level_release->keyval = GDK_KEY_Shift_R;

  switch(i){
  case 0:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Left;
      
      /* send event */
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 1:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Right;
      
      /* send event */
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 2:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Up;
    
      /* send event */
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 3:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Down;
      
      /* send event */
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 4:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_space;
      
      /* send event */
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 5:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Left;
      
      /* send event */
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) second_level_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) second_level_release);
    }
    break;
  case 6:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Right;
      
      /* send event */
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) second_level_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) second_level_release);
    }
    break;
  case 7:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Delete;
      
      /* send event */
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 8:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_c;

      /* send event */
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }    
    break;
  case 9:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_x;

      /* send event */
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }
    break;
  case 10:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_v;

      /* send event */
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }
    break;
  case 11:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_a;

      /* send event */
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }
    break;
  case 12:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_i;

      /* send event */
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) note_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }
    break;
  }

  return(TRUE);
}

gint
ags_accessible_note_edit_get_n_actions(AtkAction *action)
{
  return(13);
}

const gchar*
ags_accessible_note_edit_get_description(AtkAction *action,
					 gint i)
{
  static const gchar **actions = {
    "move cursor left\0",
    "move cursor right\0",
    "move cursor up\0",
    "move cursor down\0",
    "add audio note\0",
    "shrink audio note\0",
    "grow audio note\0",
    "remove audio note\0",
    "copy note to clipboard\0",
    "cut note to clipbaord\0",
    "paste note from clipboard\0",
    "select all note\0",
    "invert note\0",
  };

  if(i >= 0 && i < 13){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_note_edit_get_name(AtkAction *action,
				  gint i)
{
  static const gchar **actions = {
    "left\0",
    "right\0",
    "up\0",
    "down\0",
    "add\0",
    "shrink\0",
    "grow\0",
    "remove\0",
    "copy\0",
    "cut\0",
    "paste\0",
    "select-all\0",
    "invert\0",
  };
  
  if(i >= 0 && i < 13){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_note_edit_get_keybinding(AtkAction *action,
					gint i)
{
  static const gchar **actions = {
    "left\0",
    "right\0",
    "up\0",
    "down\0",
    "space",
    "Shft+Left\0",
    "Shft+Right\0",
    "Del\0"
    "Ctrl+c"
    "Ctrl+x",
    "Ctrl+v",
    "Ctrl+a",
    "Ctrl+i",
  };
  
  if(i >= 0 && i < 13){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

gboolean
ags_accessible_note_edit_set_description(AtkAction *action,
					 gint i)
{
  //TODO:JK: implement me

  return(FALSE);
}

gchar*
ags_accessible_note_edit_get_localized_name(AtkAction *action,
					    gint i)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_note_edit_set_map_height:
 * @note_edit: the #AgsNoteEdit
 * @map_height: the new height
 *
 * Set the map height in pixel.
 *
 * Since: 0.4
 */
void
ags_note_edit_set_map_height(AgsNoteEdit *note_edit, guint map_height)
{
  note_edit->map_height = map_height;
  
  note_edit->flags |= AGS_NOTE_EDIT_RESETING_VERTICALLY;
  ags_note_edit_reset_vertically(note_edit, AGS_NOTE_EDIT_RESET_VSCROLLBAR);
  note_edit->flags &= (~AGS_NOTE_EDIT_RESETING_VERTICALLY);
  
  note_edit->flags |= AGS_NOTE_EDIT_RESETING_HORIZONTALLY;
  ags_note_edit_reset_horizontally(note_edit, AGS_NOTE_EDIT_RESET_HSCROLLBAR);
  note_edit->flags &= (~AGS_NOTE_EDIT_RESETING_HORIZONTALLY);
}

/**
 * ags_note_edit_reset_vertically:
 * @note_edit: the #AgsNoteEdit
 * @flags: the #AgsNoteEditResetFlags
 *
 * Reset @note_edit as configured vertically.
 *
 * Since: 0.4
 */
void
ags_note_edit_reset_vertically(AgsNoteEdit *note_edit, guint flags)
{
  AgsEditor *editor;

  gdouble value;

  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(note_edit),
						 AGS_TYPE_EDITOR);

  if(editor->selected_machine != NULL &&
     editor->current_edit_widget == (GtkWidget *) note_edit){
    cairo_t *cr;
    gdouble value;

    value = GTK_RANGE(note_edit->vscrollbar)->adjustment->value;

    if((AGS_NOTE_EDIT_RESET_VSCROLLBAR & flags) != 0){
      GtkWidget *widget;
      GtkAdjustment *adjustment;
      guint height;

      widget = GTK_WIDGET(note_edit->drawing_area);
      adjustment = GTK_RANGE(note_edit->vscrollbar)->adjustment;
      
      if(note_edit->map_height > widget->allocation.height){
	height = widget->allocation.height;
	gtk_adjustment_set_upper(adjustment,
				 (gdouble) (note_edit->map_height - height));
	
	if(adjustment->value > adjustment->upper){
	  gtk_adjustment_set_value(adjustment, adjustment->upper);
	}
      }else{
	height = note_edit->map_height;
	
	gtk_adjustment_set_upper(adjustment, 0.0);
	gtk_adjustment_set_value(adjustment, 0.0);
      }
      
      note_edit->height = height;
    }

    note_edit->y0 = ((guint) round((double) value)) % note_edit->control_height;

    if(note_edit->y0 != 0){
      note_edit->y0 = note_edit->control_height - note_edit->y0;
    }

    note_edit->y1 = (note_edit->height - note_edit->y0) % note_edit->control_height;

    note_edit->nth_y = (guint) ceil(round((double) value) / (double)(note_edit->control_height));
    note_edit->stop_y = note_edit->nth_y + (note_edit->height - note_edit->y0 - note_edit->y1) / note_edit->control_height;

    /* refresh display */
    if(GTK_WIDGET_VISIBLE(editor)){
      cr = gdk_cairo_create(GTK_WIDGET(note_edit->drawing_area)->window);

      cairo_surface_flush(cairo_get_target(cr));
      cairo_push_group(cr);

      ags_note_edit_draw_segment(note_edit, cr);
      ags_note_edit_draw_notation(note_edit, cr);

      if(editor->toolbar->selected_edit_mode == editor->toolbar->position){
	ags_note_edit_draw_position(note_edit, cr);
      }

      if((AGS_NOTE_EDIT_DRAW_FADER & (note_edit->flags)) != 0){
	AgsCountBeatsAudioRun *count_beats_audio_run;

	AgsMutexManager *mutex_manager;
	
	GList *recall;

	gdouble position;

	pthread_mutex_t *application_mutex;
	pthread_mutex_t *audio_mutex;
  
	mutex_manager = ags_mutex_manager_get_instance();
	application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
	/* lookup audio mutex */
	pthread_mutex_lock(application_mutex);

	audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) editor->selected_machine->audio);
  
	pthread_mutex_unlock(application_mutex);

	/* retrieve position */
	pthread_mutex_lock(audio_mutex);

	recall = editor->selected_machine->audio->play;

	while((recall = ags_recall_find_type(recall,
					     AGS_TYPE_COUNT_BEATS_AUDIO_RUN)) != NULL){
	  if(AGS_RECALL(recall->data)->recall_id != NULL && (AGS_RECALL_NOTATION & (AGS_RECALL(recall->data)->recall_id->flags)) != 0){
	    break;
	  }

	  recall = recall->next;
	}

	if(recall != NULL){
	  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(recall->data);

	  position = count_beats_audio_run->notation_counter * note_edit->control_unit.control_width;
	}
	
	pthread_mutex_unlock(audio_mutex);

	/* draw fader */
	if(recall != NULL){
	  ags_note_edit_draw_scroll(note_edit, cr,
				    position);
	}
      }
      
      cairo_pop_group_to_source(cr);
      cairo_paint(cr);

      cairo_surface_mark_dirty(cairo_get_target(cr));
      cairo_destroy(cr);
    }
  }
}

/**
 * ags_note_edit_reset_horizontally:
 * @note_edit: the #AgsNoteEdit
 * @flags: the #AgsNoteEditResetFlags
 *
 * Reset @note_edit as configured horizontally.
 *
 * Since: 0.4
 */
void
ags_note_edit_reset_horizontally(AgsNoteEdit *note_edit, guint flags)
{
  AgsEditor *editor;
  double tact_factor, zoom_factor;
  double tact;
  gdouble value;

  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(note_edit),
						 AGS_TYPE_EDITOR);

  zoom_factor = 0.25;

  tact_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) editor->toolbar->zoom));
  tact = exp2((double) gtk_combo_box_get_active((GtkComboBox *) editor->toolbar->zoom) - 2.0);

  if((AGS_NOTE_EDIT_RESET_WIDTH & flags) != 0){
    note_edit->control_unit.control_width = (guint) (((double) note_edit->control_width * zoom_factor * tact));

    note_edit->control_current.control_count = (guint) ((double) note_edit->control_unit.control_count * tact);
    note_edit->control_current.control_width = (note_edit->control_width * zoom_factor * tact_factor * tact);

    note_edit->map_width = (guint) ((double) note_edit->control_current.control_count * (double) note_edit->control_current.control_width);

    /* reset ruler */
    note_edit->ruler->factor = tact_factor;
    note_edit->ruler->precision = tact;
    note_edit->ruler->scale_precision = 1.0 / tact;

    gtk_widget_queue_draw((GtkWidget *) note_edit->ruler);
  }

  value = GTK_RANGE(note_edit->hscrollbar)->adjustment->value;

  if((AGS_NOTE_EDIT_RESET_HSCROLLBAR & flags) != 0){
    GtkWidget *widget;
    GtkAdjustment *adjustment;
    guint width;

    widget = GTK_WIDGET(note_edit->drawing_area);
    adjustment = GTK_RANGE(note_edit->hscrollbar)->adjustment;

    if(note_edit->map_width > widget->allocation.width){
      width = widget->allocation.width;
      //	gtk_adjustment_set_upper(adjustment, (double) (note_edit->map_width - width));
      gtk_adjustment_set_upper(adjustment,
			       (gdouble) (note_edit->map_width - width));
      gtk_adjustment_set_upper(note_edit->ruler->adjustment,
			       (gdouble) (note_edit->map_width - width) / note_edit->control_current.control_width);

      if(adjustment->value > adjustment->upper){
	gtk_adjustment_set_value(adjustment, adjustment->upper);

	/* reset ruler */
	gtk_adjustment_set_value(note_edit->ruler->adjustment, note_edit->ruler->adjustment->upper);
	gtk_widget_queue_draw((GtkWidget *) note_edit->ruler);
      }
    }else{
      width = note_edit->map_width;

      gtk_adjustment_set_upper(adjustment, 0.0);
      gtk_adjustment_set_value(adjustment, 0.0);
	
      /* reset ruler */
      gtk_adjustment_set_upper(note_edit->ruler->adjustment, 0.0);
      gtk_adjustment_set_value(note_edit->ruler->adjustment, 0.0);
      gtk_widget_queue_draw((GtkWidget *) note_edit->ruler);
    }

    note_edit->width = width;
  }

  /* reset AgsNoteEditControlCurrent */
  if(note_edit->map_width > note_edit->width){
    note_edit->control_current.x0 = ((guint) round((double) value)) % note_edit->control_current.control_width;

    if(note_edit->control_current.x0 != 0){
      note_edit->control_current.x0 = note_edit->control_current.control_width - note_edit->control_current.x0;
    }

    note_edit->control_current.x1 = (note_edit->width - note_edit->control_current.x0) % note_edit->control_current.control_width;

    note_edit->control_current.nth_x = (guint) ceil((double)(value) / (double)(note_edit->control_current.control_width));
  }else{
    note_edit->control_current.x0 = 0;
    note_edit->control_current.x1 = 0;
    note_edit->control_current.nth_x = 0;
  }

  /* reset AgsNoteEditControlUnit */
  if(note_edit->map_width > note_edit->width){
    note_edit->control_unit.x0 = ((guint)round((double) value)) % note_edit->control_unit.control_width;

    if(note_edit->control_unit.x0 != 0){
      note_edit->control_unit.x0 = note_edit->control_unit.control_width - note_edit->control_unit.x0;
    }
    
    note_edit->control_unit.x1 = (note_edit->width - note_edit->control_unit.x0) % note_edit->control_unit.control_width;
      
    note_edit->control_unit.nth_x = (guint) ceil(round((double) value) / (double) (note_edit->control_unit.control_width));
    note_edit->control_unit.stop_x = note_edit->control_unit.nth_x + (note_edit->width - note_edit->control_unit.x0 - note_edit->control_unit.x1) / note_edit->control_unit.control_width;
  }else{
    note_edit->control_unit.x0 = 0;
    note_edit->control_unit.x1 = 0;
    note_edit->control_unit.nth_x = 0;
  }

  /* refresh display */
  if(editor->selected_machine != NULL){
    cairo_t *cr;

    if(GTK_WIDGET_VISIBLE(editor)){
      gdouble position;
      
      cr = gdk_cairo_create(GTK_WIDGET(note_edit->drawing_area)->window);

      cairo_surface_flush(cairo_get_target(cr));
      cairo_push_group(cr);

      ags_note_edit_draw_segment(note_edit, cr);
      ags_note_edit_draw_notation(note_edit, cr);

      if(editor->toolbar->selected_edit_mode == editor->toolbar->position){
	ags_note_edit_draw_position(note_edit, cr);
      }

      //TODO:JK: implement me
      //      position = gtk_range_get_value(GTK_RANGE(note_edit->hscrollbar));
      //      position -= floor(position / note_edit->control_current.control_width);
      //      ags_note_edit_draw_scroll(note_edit, cr,
      //				position);

      /* fader */
      if((AGS_NOTE_EDIT_DRAW_FADER & (note_edit->flags)) != 0){
	AgsCountBeatsAudioRun *count_beats_audio_run;

	AgsMutexManager *mutex_manager;
	
	GList *recall;

	gdouble position;

	pthread_mutex_t *application_mutex;
	pthread_mutex_t *audio_mutex;

	mutex_manager = ags_mutex_manager_get_instance();
	application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

	/* lookup audio mutex */
	pthread_mutex_lock(application_mutex);
  
	audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) editor->selected_machine->audio);
  
	pthread_mutex_unlock(application_mutex);

	/* retrieve position */
	pthread_mutex_lock(audio_mutex);

	recall = editor->selected_machine->audio->play;

	while((recall = ags_recall_find_type(recall,
					     AGS_TYPE_COUNT_BEATS_AUDIO_RUN)) != NULL){
	  if(AGS_RECALL(recall->data)->recall_id != NULL && (AGS_RECALL_NOTATION & (AGS_RECALL(recall->data)->recall_id->flags)) != 0){
	    break;
	  }

	  recall = recall->next;
	}

	if(recall != NULL){
	  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(recall->data);

	  position = count_beats_audio_run->notation_counter * note_edit->control_unit.control_width;
	}
	
	pthread_mutex_unlock(audio_mutex);

	/* draw fader */
	if(recall != NULL){
	  ags_note_edit_draw_scroll(note_edit, cr,
				    position);
	}
      }

      cairo_pop_group_to_source(cr);
      cairo_paint(cr);
	
      cairo_surface_mark_dirty(cairo_get_target(cr));
      cairo_destroy(cr);
    }
  }
}

/**
 * ags_note_edit_draw_segment:
 * @note_edit: the #AgsNoteEdit
 * @cr: the #cairo_t surface
 *
 * Draws horizontal and vertical lines.
 *
 * Since: 0.4
 */
void
ags_note_edit_draw_segment(AgsNoteEdit *note_edit, cairo_t *cr)
{
  AgsEditor *editor;
  GtkWidget *widget;

  GtkStyle *note_edit_style;
  
  double tact;
  guint i, j;
  guint j_set;

  static const gdouble white_gc = 65535.0;

  widget = (GtkWidget *) note_edit->drawing_area;
  note_edit_style = gtk_widget_get_style(widget);
  
  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(note_edit),
						 AGS_TYPE_EDITOR);

  /* clear with background color */
  cairo_set_source_rgb(cr,
		       note_edit_style->bg[0].red / white_gc,
		       note_edit_style->bg[0].green / white_gc,
		       note_edit_style->bg[0].blue / white_gc);

  cairo_rectangle(cr,
		  0.0, 0.0,
		  (double) widget->allocation.width, (double) widget->allocation.height);
  
  cairo_fill(cr);

  /* horizontal lines */
  cairo_set_line_width(cr, 1.0);

  cairo_set_source_rgb(cr,
		       note_edit_style->fg[0].red / white_gc,
		       note_edit_style->fg[0].green / white_gc,
		       note_edit_style->fg[0].blue / white_gc);

  for(i = note_edit->y0 ; i < note_edit->height;){
    cairo_move_to(cr, 0.0, (double) i);
    cairo_line_to(cr, (double) note_edit->width, (double) i);
    cairo_stroke(cr);

    i += note_edit->control_height;
  }

  cairo_move_to(cr,
		0.0, (double) i);
  cairo_line_to(cr,
		(double) note_edit->width, (double) i);
  cairo_stroke(cr);

  /* vertical lines */
  tact = exp2((double) gtk_combo_box_get_active((GtkComboBox *) editor->toolbar->zoom) - 2.0);

  i = note_edit->control_current.x0;
  
  if(i < note_edit->width &&
     tact > 1.0 ){
    j_set = note_edit->control_current.nth_x % ((guint) tact);

    /* thin lines */
    cairo_set_source_rgb(cr,
			 note_edit_style->mid[0].red / white_gc,
			 note_edit_style->mid[0].green / white_gc,
			 note_edit_style->mid[0].blue / white_gc);

    if(j_set != 0){
      j = j_set;
      goto ags_note_edit_draw_segment0;
    }
  }

  for(; i < note_edit->width; ){
    /* strong lines */
    cairo_set_source_rgb(cr,
			 note_edit_style->fg[0].red / white_gc,
			 note_edit_style->fg[0].green / white_gc,
			 note_edit_style->fg[0].blue / white_gc);
    
    cairo_move_to(cr,
		  (double) i, 0.0);
    cairo_line_to(cr,
		  (double) i, (double) note_edit->height);
    cairo_stroke(cr);
    
    i += note_edit->control_current.control_width;
    
    /* thin lines */
    cairo_set_source_rgb(cr,
			 note_edit_style->mid[0].red / white_gc,
			 note_edit_style->mid[0].green / white_gc,
			 note_edit_style->mid[0].blue / white_gc);
    
    for(j = 1; i < note_edit->width && j < tact; j++){
    ags_note_edit_draw_segment0:
      cairo_move_to(cr, (double) i, 0.0);
      cairo_line_to(cr, (double) i, (double) note_edit->height);
      cairo_stroke(cr);
      
      i += note_edit->control_current.control_width;
    }
  }
}

/**
 * ags_note_edit_draw_position:
 * @note_edit: the #AgsNoteEdit
 * @cr: the #cairo_t surface
 *
 * Draws the cursor.
 *
 * Since: 0.4.0
 */
void
ags_note_edit_draw_position(AgsNoteEdit *note_edit, cairo_t *cr)
{
  GtkStyle *note_edit_style;
  
  guint selected_x, selected_y;
  guint x_offset[2], y_offset[2];
  guint x, y, width, height;
  gint size_width, size_height;

  static const gdouble white_gc = 65535.0;

  note_edit_style = gtk_widget_get_style(GTK_WIDGET(note_edit->drawing_area));
  
  selected_x = note_edit->selected_x * note_edit->control_unit.control_width;
  selected_y = note_edit->selected_y * note_edit->control_height;

  size_width = GTK_WIDGET(note_edit->drawing_area)->allocation.width;
  size_height = GTK_WIDGET(note_edit->drawing_area)->allocation.height;

  x_offset[0] = (guint) gtk_range_get_value(GTK_RANGE(note_edit->hscrollbar));
  x_offset[1] = x_offset[0] + (guint) size_width;

  y_offset[0] = (guint) gtk_range_get_value(GTK_RANGE(note_edit->vscrollbar));
  y_offset[1] = y_offset[0] + (guint) size_height;

  /* calculate horizontally values */
  if(selected_x < x_offset[0]){
    if(selected_x + note_edit->control_current.control_width > x_offset[0]){
      x = 0;
      width = selected_x + note_edit->control_current.control_width - x_offset[0];
    }else{
      return;
    }
  }else if(selected_x > x_offset[1]){
    return;
  }else{
    x = selected_x - x_offset[0];

    if(selected_x + note_edit->control_current.control_width < x_offset[1]){
      width = note_edit->control_current.control_width;
    }else{
      width = x_offset[1] - selected_x;
    }
  }

  /* calculate vertically values */
  if(selected_y < y_offset[0]){
    if(selected_y + note_edit->control_height > y_offset[0]){
      y = 0;
      height = selected_y + note_edit->control_height - y_offset[0];
    }else{
      return;
    }
  }else if(selected_y > y_offset[1]){
    return;
  }else{
    y = selected_y - y_offset[0];

    if(selected_y + note_edit->control_height < y_offset[1]){
      height = note_edit->control_height;
    }else{
      height = y_offset[1] - selected_y;
    }
  }

  /* draw */
  cairo_set_source_rgba(cr,
			note_edit_style->base[0].red / white_gc,
			note_edit_style->base[0].green / white_gc,
			note_edit_style->base[0].blue / white_gc,
			0.5);
  cairo_rectangle(cr,
		  (double) x, (double) y,
		  (double) width, (double) height);
  cairo_fill(cr);
}

/**
 * ags_note_edit_draw_notation:
 * @note_edit: the #AgsNoteEdit
 * @cr: the #cairo_t surface
 *
 * Draw the #AgsNotation of selected #AgsMachine on @note_edit.
 *
 * Since: 0.4
 */
void
ags_note_edit_draw_notation(AgsNoteEdit *note_edit, cairo_t *cr)
{
  AgsMachine *machine;
  AgsEditor *editor;
  GtkWidget *widget;

  GtkStyle *note_edit_style;
  AgsNote *note;

  AgsMutexManager *mutex_manager;

  GList *list_notation, *list_note;

  guint x_offset;
  guint control_height;
  guint x, y, width, height;
  gint selected_channel;
  gint i;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  static const gdouble white_gc = 65535.0;

  note_edit_style = gtk_widget_get_style(GTK_WIDGET(note_edit->drawing_area));
  
  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(note_edit),
						 AGS_TYPE_EDITOR);

  if(editor->selected_machine == NULL ||
     (machine = editor->selected_machine) == NULL){
    return;
  }

  widget = (GtkWidget *) note_edit->drawing_area;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* lookup audio mutex */
  pthread_mutex_lock(application_mutex);
  
  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) editor->selected_machine->audio);
  
  pthread_mutex_unlock(application_mutex);

  /* draw */
  pthread_mutex_lock(audio_mutex);

  cairo_set_source_rgb(cr,
		       note_edit_style->fg[0].red / white_gc,
		       note_edit_style->fg[0].green / white_gc,
		       note_edit_style->fg[0].blue / white_gc);

  i = 0;

  while((selected_channel = ags_notebook_next_active_tab(editor->current_notebook,
							 i)) != -1){
    list_notation = g_list_nth(machine->audio->notation,
			       selected_channel);

    if(list_notation == NULL){
      i++;
      continue;
    }

    list_note = AGS_NOTATION(list_notation->data)->notes;

    control_height = note_edit->control_height - 2 * note_edit->control_margin_y;

    x_offset = (guint) GTK_RANGE(note_edit->hscrollbar)->adjustment->value;

    /* draw controls smaller than note_edit->nth_x */
    while(list_note != NULL && (note = (AgsNote *) list_note->data)->x[0] < note_edit->control_unit.nth_x){
      if(note->x[1] >= note_edit->control_unit.nth_x){
	if(note->y >= note_edit->nth_y && note->y <= note_edit->stop_y){
	  x = 0;
	  y = (note->y - note_edit->nth_y) * note_edit->control_height + note_edit->y0 + note_edit->control_margin_y;

	  width = (guint) ((double) note->x[1] * note_edit->control_unit.control_width - (double) x_offset);

	  if(width > widget->allocation.width)
	    width = widget->allocation.width;

	  height = control_height;

	  /* draw note */
	  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	  cairo_fill(cr);

	  /* check if note is selected */
	  if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	    cairo_set_source_rgba(cr,
				  note_edit_style->light[0].red / white_gc,
				  note_edit_style->light[0].green / white_gc,
				  note_edit_style->light[0].blue / white_gc,
				  0.7);

	    cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	    cairo_stroke(cr);

	    cairo_set_source_rgb(cr,
				 note_edit_style->fg[0].red / white_gc,
				 note_edit_style->fg[0].green / white_gc,
				 note_edit_style->fg[0].blue / white_gc);	    
	  }
	}else if(note->y == (note_edit->nth_y - 1) && note_edit->y0 != 0){
	  if(note_edit->y0 > note_edit->control_margin_y){
	    x = 0;
	    width = (guint) ((double) note->x[1] * (double) note_edit->control_unit.control_width - x_offset);

	    if(width > widget->allocation.width)
	      width = widget->allocation.width;

	    if(note_edit->y0 > control_height + note_edit->control_margin_y){
	      y = note_edit->y0 - (control_height + note_edit->control_margin_y);
	      height = control_height;
	    }else{
	      y = 0;
	      height = note_edit->y0 - note_edit->control_margin_y;
	    }

	    /* draw note */
	    cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	    cairo_fill(cr);

	    /* check if note is selected */
	    if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	      cairo_set_source_rgba(cr,
				    note_edit_style->light[0].red / white_gc,
				    note_edit_style->light[0].green / white_gc,
				    note_edit_style->light[0].blue / white_gc,
				    0.7);
	    
	      cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	      cairo_stroke(cr);
	    
	      cairo_set_source_rgb(cr,
				   note_edit_style->fg[0].red / white_gc,
				   note_edit_style->fg[0].green / white_gc,
				   note_edit_style->fg[0].blue / white_gc);	    
	    }
	  }
	}else if(note->y == (note_edit->stop_y + 1) && note_edit->y1 != 0){
	  if(note_edit->y1 > note_edit->control_margin_y){
	    x = 0;
	    width = note->x[1] * note_edit->control_unit.control_width - x_offset;

	    if(width > widget->allocation.width)
	      width = widget->allocation.width;

	    y = (note->y - note_edit->nth_y) * note_edit->control_height + note_edit->control_margin_y;

	    if(note_edit->y1 > control_height + note_edit->control_margin_y){
	      height = control_height;
	    }else{
	      height = note_edit->y1 - note_edit->control_margin_y;
	    }

	    /* draw note */
	    cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	    cairo_fill(cr);
	  
	    /* check if note is selected */
	    if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	      cairo_set_source_rgba(cr,
				    note_edit_style->light[0].red / white_gc,
				    note_edit_style->light[0].green / white_gc,
				    note_edit_style->light[0].blue / white_gc,
				    0.7);
	    
	      cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	      cairo_stroke(cr);
	    
	      cairo_set_source_rgb(cr,
				   note_edit_style->fg[0].red / white_gc,
				   note_edit_style->fg[0].green / white_gc,
				   note_edit_style->fg[0].blue / white_gc);	    
	    }
	  }
	}
      }

      list_note = list_note->next;
    }

    /* draw controls equal or greater than note_edit->nth_x */
    while(list_note != NULL && (note = (AgsNote *) list_note->data)->x[0] <= note_edit->control_unit.stop_x){
      if(note->y >= note_edit->nth_y && note->y <= note_edit->stop_y){
	x = (guint) note->x[0] * note_edit->control_unit.control_width;
	y = (note->y - note_edit->nth_y) * note_edit->control_height +
	  note_edit->y0 +
	  note_edit->control_margin_y;

	width = note->x[1] * note_edit->control_unit.control_width - x;
	x -= x_offset;

	if(x + width > widget->allocation.width)
	  width = widget->allocation.width - x;

	height = control_height;

	/* draw note*/
	cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	cairo_fill(cr);

	/* check if note is selected */
	if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	  cairo_set_source_rgba(cr,
				note_edit_style->light[0].red / white_gc,
				note_edit_style->light[0].green / white_gc,
				note_edit_style->light[0].blue / white_gc,
				0.7);
	
	  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	  cairo_stroke(cr);
	
	  cairo_set_source_rgb(cr,
			       note_edit_style->fg[0].red / white_gc,
			       note_edit_style->fg[0].green / white_gc,
			       note_edit_style->fg[0].blue / white_gc);	    
	}
      }else if(note->y == (note_edit->nth_y - 1) && note_edit->y0 != 0){
	if(note_edit->y0 > note_edit->control_margin_y){
	  x = note->x[0] * note_edit->control_unit.control_width - x_offset;
	  width = note->x[1] * note_edit->control_unit.control_width - x_offset - x;
      
	  if(x + width > widget->allocation.width)
	    width = widget->allocation.width - x;

	  if(note_edit->y0 > control_height + note_edit->control_margin_y){
	    y = note_edit->y0 - (control_height + note_edit->control_margin_y);
	    height = control_height;
	  }else{
	    y = 0;
	    height = note_edit->y0 - note_edit->control_margin_y;
	  }

	  /* draw note */
	  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	  cairo_fill(cr);

	  /* check if note is selected */
	  if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	    cairo_set_source_rgba(cr,
				  note_edit_style->light[0].red / white_gc,
				  note_edit_style->light[0].green / white_gc,
				  note_edit_style->light[0].blue / white_gc,
				  0.7);

	    cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	    cairo_stroke(cr);

	    cairo_set_source_rgb(cr,
				 note_edit_style->fg[0].red / white_gc,
				 note_edit_style->fg[0].green / white_gc,
				 note_edit_style->fg[0].blue / white_gc);	    
	  }
	}
      }else if(note->y == (note_edit->stop_y + 1) && note_edit->y1 != 0){
	if(note_edit->y1 > note_edit->control_margin_y){
	  x = note->x[0] * note_edit->control_unit.control_width - x_offset;
	  width = note->x[1] * note_edit->control_unit.control_width - x_offset - x;
      
	  if(x + width > widget->allocation.width)
	    width = widget->allocation.width - x;

	  y = (note->y - note_edit->nth_y) * note_edit->control_height + note_edit->control_margin_y;

	  if(note_edit->y1 > control_height + note_edit->control_margin_y){
	    height = control_height;
	  }else{
	    height = note_edit->y1 - note_edit->control_margin_y;
	  }

	  /* draw note */
	  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	  cairo_fill(cr);

	  /* check if note is selected */
	  if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
	    cairo_set_source_rgba(cr,
				  note_edit_style->light[0].red / white_gc,
				  note_edit_style->light[0].green / white_gc,
				  note_edit_style->light[0].blue / white_gc,
				  0.7);

	    cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
	    cairo_stroke(cr);

	    cairo_set_source_rgb(cr,
				 note_edit_style->fg[0].red / white_gc,
				 note_edit_style->fg[0].green / white_gc,
				 note_edit_style->fg[0].blue / white_gc);	    
	  }
	}
      }

      list_note = list_note->next;
    }

    i++;
  }

  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_note_edit_draw_scroll:
 * @note_edit: the #AgsNoteEdit
 * @cr: the #cairo_t surface
 * @position: the new position
 *
 * Change visible x-position of @note_edit.
 *
 * Since: 0.4
 */
void
ags_note_edit_draw_scroll(AgsNoteEdit *note_edit, cairo_t *cr,
			  gdouble position)
{
  GtkStyle *note_edit_style;
  
  double x, y;
  double width, height;

  static const gdouble white_gc = 65535.0;

  note_edit_style = gtk_widget_get_style(GTK_WIDGET(note_edit->drawing_area));
  
  y = 0.0;
  x = (position) - (GTK_RANGE(note_edit->hscrollbar)->adjustment->value);

  height = (double) GTK_WIDGET(note_edit->drawing_area)->allocation.height;
  width = 3.0;

  /* draw */
  cairo_set_source_rgba(cr,
			note_edit_style->dark[0].red / white_gc,
			note_edit_style->dark[0].green / white_gc,
			note_edit_style->dark[0].blue / white_gc,
			0.5);
  cairo_rectangle(cr,
		  (double) x, (double) y,
		  (double) width, (double) height);
  cairo_fill(cr);
}

/**
 * ags_note_edit_new:
 *
 * Create a new #AgsNoteEdit.
 *
 * Returns: a new #AgsNoteEdit
 * 
 * Since: 0.4
 */
AgsNoteEdit*
ags_note_edit_new()
{
  AgsNoteEdit *note_edit;

  note_edit = (AgsNoteEdit *) g_object_new(AGS_TYPE_NOTE_EDIT,
					   NULL);

  return(note_edit);
}

