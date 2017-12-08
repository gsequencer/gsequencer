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

#include <ags/X/editor/ags_wave_edit.h>
#include <ags/X/editor/ags_wave_edit_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/X/ags_wave_editor.h>

#include <ags/X/editor/ags_wave_edit.h>
#include <ags/X/editor/ags_wave_area.h>

#include <gdk/gdkkeysyms.h>
#include <atk/atk.h>

#include <cairo.h>
#include <math.h>

static GType ags_accessible_wave_edit_get_type(void);
void ags_wave_edit_class_init(AgsWaveEditClass *wave_edit);
void ags_accessible_wave_edit_class_init(AtkObject *object);
void ags_accessible_wave_edit_action_interface_init(AtkActionIface *action);
void ags_wave_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_wave_edit_init(AgsWaveEdit *wave_edit);
void ags_wave_edit_connect(AgsConnectable *connectable);
void ags_wave_edit_disconnect(AgsConnectable *connectable);

AtkObject* ags_wave_edit_get_accessible(GtkWidget *widget);

gboolean ags_accessible_wave_edit_do_action(AtkAction *action,
					    gint i);
gint ags_accessible_wave_edit_get_n_actions(AtkAction *action);
const gchar* ags_accessible_wave_edit_get_description(AtkAction *action,
						      gint i);
const gchar* ags_accessible_wave_edit_get_name(AtkAction *action,
					       gint i);
const gchar* ags_accessible_wave_edit_get_keybinding(AtkAction *action,
						     gint i);
gboolean ags_accessible_wave_edit_set_description(AtkAction *action,
						  gint i);
gchar* ags_accessible_wave_edit_get_localized_name(AtkAction *action,
						   gint i);

/**
 * SECTION:ags_wave_edit
 * @short_description: edit audio data
 * @title: AgsWaveEdit
 * @section_id:
 * @include: ags/X/editor/ags_wave_edit.h
 *
 * The #AgsWaveEdit lets you edit audio data.
 */

static gpointer ags_wave_edit_parent_class = NULL;

GtkStyle *wave_edit_style = NULL;

static GQuark quark_accessible_object = 0;

GType
ags_wave_edit_get_type(void)
{
  static GType ags_type_wave_edit = 0;

  if(!ags_type_wave_edit){
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

    ags_type_wave_edit = g_type_register_static(GTK_TYPE_TABLE,
						"AgsWaveEdit", &ags_wave_edit_info,
						0);
    
    g_type_add_interface_static(ags_type_wave_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_wave_edit);
}

static GType
ags_accessible_wave_edit_get_type(void)
{
  static GType ags_type_accessible_wave_edit = 0;

  if(!ags_type_accessible_wave_edit){
    const GTypeInfo ags_accesssible_wave_edit_info = {
      sizeof(GtkAccessibleClass),
      NULL,           /* base_init */
      NULL,           /* base_finalize */
      (GClassInitFunc) ags_accessible_wave_edit_class_init,
      NULL,           /* class_finalize */
      NULL,           /* class_data */
      sizeof(GtkAccessible),
      0,             /* n_preallocs */
      NULL, NULL
    };

    static const GInterfaceInfo atk_action_interface_info = {
      (GInterfaceInitFunc) ags_accessible_wave_edit_action_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_accessible_wave_edit = g_type_register_static(GTK_TYPE_ACCESSIBLE,
							   "AgsAccessibleWaveEdit", &ags_accesssible_wave_edit_info,
							   0);

    g_type_add_interface_static(ags_type_accessible_wave_edit,
				ATK_TYPE_ACTION,
				&atk_action_interface_info);
  }
  
  return(ags_type_accessible_wave_edit);
}


void
ags_wave_edit_class_init(AgsWaveEditClass *wave_edit)
{
  ags_wave_edit_parent_class = g_type_class_peek_parent(wave_edit);

  quark_accessible_object = g_quark_from_static_string("ags-accessible-object");
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
ags_accessible_wave_edit_class_init(AtkObject *object)
{
  /* empty */
}

void
ags_accessible_wave_edit_action_interface_init(AtkActionIface *action)
{
  action->do_action = ags_accessible_wave_edit_do_action;
  action->get_n_actions = ags_accessible_wave_edit_get_n_actions;
  action->get_description = ags_accessible_wave_edit_get_description;
  action->get_name = ags_accessible_wave_edit_get_name;
  action->get_keybinding = ags_accessible_wave_edit_get_keybinding;
  action->set_description = ags_accessible_wave_edit_set_description;
  action->get_localized_name = ags_accessible_wave_edit_get_localized_name;
}

void
ags_wave_edit_init(AgsWaveEdit *wave_edit)
{
  GtkAdjustment *adjustment;

  wave_edit->flags = 0;

  wave_edit->key_mask = 0;

  wave_edit->map_width = AGS_WAVE_EDIT_MAX_CONTROLS;
  wave_edit->map_height = 0;

  wave_edit->edit_x = 0;
  wave_edit->edit_y = 0;
  
  wave_edit->select_x0 = 0;
  wave_edit->select_x1 = 0;
  wave_edit->select_y0 = 0;
  wave_edit->select_y1 = 0;

  if(wave_edit_style == NULL){
    wave_edit_style = gtk_style_copy(gtk_widget_get_style(wave_edit));
  }

  wave_edit->ruler = ags_ruler_new();
  gtk_table_attach(GTK_TABLE(wave_edit),
		   (GtkWidget *) wave_edit->ruler,
		   0, 1,
		   0, 1,
		   GTK_FILL|GTK_EXPAND,
		   GTK_FILL,
		   0, 0);

  wave_edit->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_style((GtkWidget *) wave_edit->drawing_area,
		       wave_edit_style);
  gtk_widget_set_events(GTK_WIDGET (wave_edit->drawing_area), GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			| GDK_CONTROL_MASK
			| GDK_KEY_PRESS_MASK
			| GDK_KEY_RELEASE_MASK);
  gtk_widget_set_can_focus((GtkWidget *) wave_edit->drawing_area,
			   TRUE);
    
  gtk_table_attach(GTK_TABLE(wave_edit),
		   (GtkWidget *) wave_edit->drawing_area,
		   0, 1,
		   1, 2,
		   GTK_FILL|GTK_EXPAND,
		   GTK_FILL|GTK_EXPAND,
		   0, 0);

  wave_edit->wave_area = NULL;
  wave_edit->current_area = NULL;
    
  /* GtkScrollbars */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
  wave_edit->vscrollbar = (GtkVScrollbar *) gtk_vscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(wave_edit),
		   (GtkWidget *) wave_edit->vscrollbar,
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (gdouble) AGS_WAVE_EDIT_DEFAULT_WIDTH, 1.0);
  wave_edit->hscrollbar = (GtkHScrollbar *) gtk_hscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(wave_edit),
		   (GtkWidget *) wave_edit->hscrollbar,
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_wave_edit_connect(AgsConnectable *connectable)
{
  AgsWaveEditor *wave_editor;
  AgsWaveEdit *wave_edit;

  wave_edit = AGS_WAVE_EDIT(connectable);

  if((AGS_WAVE_EDIT_CONNECTED & (wave_edit->flags)) != 0){
    return;
  }
  
  wave_edit->flags |= AGS_WAVE_EDIT_CONNECTED;

  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
							  AGS_TYPE_WAVE_EDITOR);

  if(wave_editor != NULL && wave_editor->selected_machine != NULL){
    g_signal_connect_after(wave_editor->selected_machine->audio, "set-audio-channels",
			   G_CALLBACK(ags_wave_edit_set_audio_channels_callback), wave_edit);

    g_signal_connect_after(wave_editor->selected_machine->audio, "set-pads",
			   G_CALLBACK(ags_wave_edit_set_pads_callback), wave_edit);
  }
 
  /*  */
  g_signal_connect_after((GObject *) wave_edit->drawing_area, "expose_event",
			 G_CALLBACK (ags_wave_edit_drawing_area_expose_event), (gpointer) wave_edit);

  g_signal_connect_after((GObject *) wave_edit->drawing_area, "configure_event",
			 G_CALLBACK (ags_wave_edit_drawing_area_configure_event), (gpointer) wave_edit);

  g_signal_connect((GObject *) wave_edit->drawing_area, "button_press_event",
		   G_CALLBACK (ags_wave_edit_drawing_area_button_press_event), (gpointer) wave_edit);

  g_signal_connect((GObject *) wave_edit->drawing_area, "button_release_event",
		   G_CALLBACK (ags_wave_edit_drawing_area_button_release_event), (gpointer) wave_edit);
  
  g_signal_connect((GObject *) wave_edit->drawing_area, "motion_notify_event",
		   G_CALLBACK (ags_wave_edit_drawing_area_motion_notify_event), (gpointer) wave_edit);

  g_signal_connect((GObject *) wave_edit->drawing_area, "key_press_event",
		   G_CALLBACK(ags_wave_edit_drawing_area_key_press_event), (gpointer) wave_edit);

  g_signal_connect((GObject *) wave_edit->drawing_area, "key_release_event",
		   G_CALLBACK(ags_wave_edit_drawing_area_key_release_event), (gpointer) wave_edit);

  /*  */
  g_signal_connect_after((GObject *) wave_edit->vscrollbar, "value-changed",
			 G_CALLBACK (ags_wave_edit_vscrollbar_value_changed), (gpointer) wave_edit);

  g_signal_connect_after((GObject *) wave_edit->hscrollbar, "value-changed",
			 G_CALLBACK (ags_wave_edit_hscrollbar_value_changed), (gpointer) wave_edit);
}

void
ags_wave_edit_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

AtkObject*
ags_wave_edit_get_accessible(GtkWidget *widget)
{
  AtkObject* accessible;

  accessible = g_object_get_qdata(G_OBJECT(widget),
				  quark_accessible_object);
  
  if(!accessible){
    accessible = g_object_new(ags_accessible_wave_edit_get_type(),
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
ags_accessible_wave_edit_do_action(AtkAction *action,
				   gint i)
{
  AgsWaveEdit *wave_edit;
  
  GdkEventKey *key_press, *key_release;
  GdkEventKey *modifier_press, *modifier_release;
  GdkEventKey *second_level_press, *second_level_release;
  
  if(!(i >= 0 && i < 15)){
    return(FALSE);
  }

  wave_edit = gtk_accessible_get_widget(GTK_ACCESSIBLE(action));
  
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
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event(wave_edit->drawing_area, key_release);
    }
    break;
  case 1:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Right;
      
      /* send event */
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 2:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Up;
    
      /* send event */
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 3:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Down;
      
      /* send event */
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;    
  case 4:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Up;
      
      /* send event */
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) modifier_release);
    }
    break;
  case 5:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Down;
      
      /* send event */
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) modifier_release);
    }
    break;
  case 6:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Left;
      
      /* send event */
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) second_level_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) second_level_release);
    }
    break;
  case 7:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Right;
      
      /* send event */
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) second_level_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) second_level_release);
    }
    break;
  case 8:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Up;
      
      /* send event */
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) second_level_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) second_level_release);
    }
    break;
  case 9:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Down;
      
      /* send event */
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) second_level_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) second_level_release);
    }
    break;
  case 10:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_space;
      
      /* send event */
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 11:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Delete;
      
      /* send event */
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 12:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_c;

      /* send event */
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }    
    break;
  case 13:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_x;

      /* send event */
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }
    break;
  case 14:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_v;

      /* send event */
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) wave_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }
    break;
  }

  return(TRUE);
}

gint
ags_accessible_wave_edit_get_n_actions(AtkAction *action)
{
  return(15);
}

const gchar*
ags_accessible_wave_edit_get_description(AtkAction *action,
					 gint i)
{
  static const gchar **actions = {
    "move cursor left",
    "move cursor right",
    "move cursor small left",
    "move cursor small right",
    "copy wave to clipboard",
    "cut wave to clipbaord",
    "paste wave from clipboard",
  };

  if(i >= 0 && i < 15){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_wave_edit_get_name(AtkAction *action,
				  gint i)
{
  static const gchar **actions = {
    "left",
    "right",
    "small-left",
    "small-right",
    "copy",
    "cut",
    "paste",
  };
  
  if(i >= 0 && i < 15){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_wave_edit_get_keybinding(AtkAction *action,
					gint i)
{
  static const gchar **actions = {
    "left",
    "right",
    "Shft+Left",
    "Shft+Right",
    "Ctrl+c",
    "Ctrl+x",
    "Ctrl+v",
  };
  
  if(i >= 0 && i < 15){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

gboolean
ags_accessible_wave_edit_set_description(AtkAction *action,
					 gint i)
{
  //TODO:JK: implement me

  return(FALSE);
}

gchar*
ags_accessible_wave_edit_get_localized_name(AtkAction *action,
					    gint i)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_wave_edit_reset_vertically:
 * @wave_edit: the #AgsWaveEdit
 * @flags: the #AgsWaveEditResetFlags
 *
 * Reset @wave_edit as configured vertically.
 *
 * Since: 1.2.0
 */
void
ags_wave_edit_reset_vertically(AgsWaveEdit *wave_edit, guint flags)
{
  AgsWaveEditor *wave_editor;

  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor((GtkWidget *) wave_edit,
							  AGS_TYPE_WAVE_EDITOR);
  
  if((AGS_WAVE_EDIT_RESET_VSCROLLBAR & flags) != 0){
    GtkAdjustment *vadjustment;
    guint height;

    height = GTK_WIDGET(wave_edit->drawing_area)->allocation.height;

    vadjustment = GTK_RANGE(wave_edit->vscrollbar)->adjustment;

    if(height > wave_edit->map_height){
      gtk_adjustment_set_upper(vadjustment,
			       1.0);
    }else{
      gtk_adjustment_set_upper(vadjustment,
			       (gdouble) (wave_edit->map_height - height));
    }
    
    if(vadjustment->value > vadjustment->upper){
      gtk_adjustment_set_value(vadjustment, vadjustment->upper);
    }
  }

  gtk_widget_queue_draw((GtkWidget *) wave_edit->drawing_area);

  /* reset level */
  gtk_widget_queue_draw((GtkWidget *) wave_edit->level);
}

/**
 * ags_wave_edit_reset_horizontally:
 * @wave_edit: the #AgsWaveEdit
 * @flags: the #AgsWaveEditResetFlags
 *
 * Reset @wave_edit as configured horizontally.
 *
 * Since: 1.2.0
 */
void
ags_wave_edit_reset_horizontally(AgsWaveEdit *wave_edit, guint flags)
{
  AgsWaveEditor *wave_editor;

  double tact_factor;
  double tact;

  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor(GTK_WIDGET(wave_edit),
							  AGS_TYPE_WAVE_EDITOR);

  tact_factor = exp2(6.0 - (double) gtk_combo_box_get_active(wave_editor->wave_toolbar->zoom));
  tact = exp2((double) gtk_combo_box_get_active(wave_editor->wave_toolbar->zoom) - 2.0);

  if((AGS_WAVE_EDIT_RESET_WIDTH & flags) != 0){
    wave_edit->map_width = (guint) ((double) AGS_WAVE_EDIT_MAX_CONTROLS * tact);
    
    /* reset ruler */
    wave_edit->ruler->factor = tact_factor;
    wave_edit->ruler->precision = tact;
    wave_edit->ruler->scale_precision = 1.0 / tact;
  }

  //  g_message("%d",
  //	    wave_edit->map_width);

  if((AGS_WAVE_EDIT_RESET_HSCROLLBAR & flags) != 0){
    GtkWidget *widget;
    
    GtkAdjustment *hadjustment;
    
    gdouble prev_value, prev_upper;
    guint width;

    widget = GTK_WIDGET(wave_edit->drawing_area);
    hadjustment = GTK_RANGE(wave_edit->hscrollbar)->adjustment;

    if(wave_edit->map_width > widget->allocation.width){
      width = widget->allocation.width;

      /* reset hscrollbar */
      prev_value = hadjustment->value;
      prev_upper = hadjustment->upper;
      
      gtk_adjustment_set_upper(hadjustment,
			       (gdouble) (wave_edit->map_width - width));
      gtk_adjustment_set_value(hadjustment,
			       prev_value / (prev_upper / hadjustment->upper));

      /* reset ruler */
      prev_value = wave_edit->ruler->adjustment->value;
      prev_upper = wave_edit->ruler->adjustment->upper;

      gtk_adjustment_set_upper(wave_edit->ruler->adjustment,
			       (gdouble) (wave_edit->map_width - width) / AGS_WAVE_EDIT_DEFAULT_WIDTH);
      gtk_adjustment_set_value(wave_edit->ruler->adjustment,
			       prev_value / (prev_upper / wave_edit->ruler->adjustment->upper));
      
      if(hadjustment->value > hadjustment->upper){
	gtk_adjustment_set_value(hadjustment, hadjustment->upper);

	/* reset ruler */
	gtk_adjustment_set_value(wave_edit->ruler->adjustment,
				 wave_edit->ruler->adjustment->upper);
	gtk_widget_queue_draw((GtkWidget *) wave_edit->ruler);

      }
    }else{
      gtk_adjustment_set_upper(hadjustment, 0.0);
      gtk_adjustment_set_value(hadjustment, 0.0);
      
      /* reset ruler */
      gtk_adjustment_set_upper(wave_edit->ruler->adjustment, 0.0);
      gtk_adjustment_set_value(wave_edit->ruler->adjustment, 0.0);
      gtk_widget_queue_draw((GtkWidget *) wave_edit->ruler);
    }
  }

  gtk_widget_queue_draw((GtkWidget *) wave_edit->drawing_area);
  gtk_widget_queue_draw((GtkWidget *) wave_edit->ruler);
}

/**
 * ags_wave_edit_draw_position:
 * @wave_edit: the #AgsWaveEdit
 * @cr: the #cairo_t surface
 *
 * Draws the cursor.
 *
 * Since: 1.2.0
 */
void
ags_wave_edit_draw_position(AgsWaveEdit *wave_edit,
			    cairo_t *cr)
{
  AgsWaveArea *wave_area;
  
  GtkStyle *wave_edit_style;

  GList *list;
  
  guint selected_x, selected_y;
  guint x_offset[2], y_offset[2];
  guint x, y, width, height;
  gint size_width, size_height;

  static const gdouble white_gc = 65535.0;

  list = ags_wave_area_find_position(wave_edit->wave_area,
				     wave_edit->edit_x, wave_edit->edit_y);

  if(list != NULL){
    wave_area = AGS_WAVE_AREA(list->data);
  }else{
    return;
  }

  wave_edit_style = gtk_widget_get_style(GTK_WIDGET(wave_edit->drawing_area));

  selected_x = wave_edit->edit_x;
  selected_y = wave_edit->edit_y;

  size_width = GTK_WIDGET(wave_edit->drawing_area)->allocation.width;
  size_height = GTK_WIDGET(wave_edit->drawing_area)->allocation.height;

  x_offset[0] = (guint) gtk_range_get_value(GTK_RANGE(wave_edit->hscrollbar));
  x_offset[1] = x_offset[0] + (guint) size_width;

  y_offset[0] = (guint) gtk_range_get_value(GTK_RANGE(wave_edit->vscrollbar));
  y_offset[1] = y_offset[0] + (guint) size_height;

  /* calculate horizontally values */
  if(selected_x > x_offset[0] &&
     selected_x < x_offset[1]){
    x = 0;
    width = x_offset[1] - x_offset[0];
  }else{
    return;
  }

  /* calculate vertically values */
  if(selected_y > y_offset[0] &&
     selected_y < y_offset[1]){
    y = wave_area->y - y_offset[0];

    if(selected_y + wave_area->height < y_offset[1]){
      height = y + wave_area->height;
    }else{
      height = y + (y_offset[1] - wave_area->y);
    }
  }else{
    return;
  }
  
  /* draw */
  cairo_set_source_rgba(cr,
			wave_edit_style->base[0].red / white_gc,
			wave_edit_style->base[0].green / white_gc,
			wave_edit_style->base[0].blue / white_gc,
			0.5);
  cairo_rectangle(cr,
		  (double) x, (double) y,
		  (double) width, (double) height);
  cairo_fill(cr);
}

/**
 * ags_wave_edit_draw_scroll:
 * @wave_edit: the #AgsWaveEdit
 * @position: the new position
 *
 * Change visible x-position of @wave_edit.
 *
 * Since: 1.2.0
 */
void
ags_wave_edit_draw_scroll(AgsWaveEdit *wave_edit, cairo_t *cr,
			  gdouble position)
{
  GtkStyle *wave_edit_style;
  
  double x, y;
  double width, height;

  static const gdouble white_gc = 65535.0;

  wave_edit_style = gtk_widget_get_style(GTK_WIDGET(wave_edit->drawing_area));
  
  y = 0.0;
  x = (position) - (GTK_RANGE(wave_edit->hscrollbar)->adjustment->value);

  height = (double) GTK_WIDGET(wave_edit->drawing_area)->allocation.height;
  width = 3.0;

  /* draw */
  cairo_set_source_rgba(cr,
			wave_edit_style->dark[0].red / white_gc,
			wave_edit_style->dark[0].green / white_gc,
			wave_edit_style->dark[0].blue / white_gc,
			0.5);
  cairo_rectangle(cr,
		  (double) x, (double) y,
		  (double) width, (double) height);
  cairo_fill(cr);
}

void
ags_wave_edit_paint(AgsWaveEdit *wave_edit,
		    cairo_t *cr)
{
  AgsWaveEditor *wave_editor;
  
  double tact;
  gdouble x_offset, y_offset;

  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor((GtkWidget *) wave_edit,
							  AGS_TYPE_WAVE_EDITOR);

  tact = exp2((double) gtk_combo_box_get_active(wave_editor->wave_toolbar->zoom) - 2.0);

  x_offset = GTK_RANGE(wave_edit->hscrollbar)->adjustment->value;
  y_offset = GTK_RANGE(wave_edit->vscrollbar)->adjustment->value;

  //TODO:JK: implement me
}

/**
 * ags_wave_edit_new:
 *
 * Create a new #AgsWaveEdit.
 *
 * Returns: a new #AgsWaveEdit
 *
 * Since: 1.2.0
 */
AgsWaveEdit*
ags_wave_edit_new()
{
  AgsWaveEdit *wave_edit;

  wave_edit = (AgsWaveEdit *) g_object_new(AGS_TYPE_WAVE_EDIT, NULL);

  return(wave_edit);
}
