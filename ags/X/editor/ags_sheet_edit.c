/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/editor/ags_sheet_edit.h>
#include <ags/X/editor/ags_sheet_edit_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_sheet_editor.h>

#include <gdk/gdkkeysyms.h>

#include <math.h>

static GType ags_accessible_sheet_edit_get_type(void);
void ags_sheet_edit_class_init(AgsSheetEditClass *sheet_edit);
void ags_accessible_sheet_edit_class_init(AtkObject *object);
void ags_accessible_sheet_edit_action_interface_init(AtkActionIface *action);
void ags_sheet_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sheet_edit_init(AgsSheetEdit *sheet_edit);
void ags_sheet_edit_finalize(GObject *gobject);

AtkObject* ags_sheet_edit_get_accessible(GtkWidget *widget);

void ags_sheet_edit_connect(AgsConnectable *connectable);
void ags_sheet_edit_disconnect(AgsConnectable *connectable);

gboolean ags_accessible_sheet_edit_do_action(AtkAction *action,
						gint i);
gint ags_accessible_sheet_edit_get_n_actions(AtkAction *action);
const gchar* ags_accessible_sheet_edit_get_description(AtkAction *action,
							  gint i);
const gchar* ags_accessible_sheet_edit_get_name(AtkAction *action,
						   gint i);
const gchar* ags_accessible_sheet_edit_get_keybinding(AtkAction *action,
							 gint i);
gboolean ags_accessible_sheet_edit_set_description(AtkAction *action,
						      gint i);
gchar* ags_accessible_sheet_edit_get_localized_name(AtkAction *action,
						       gint i);

void ags_sheet_edit_show(GtkWidget *widget);
void ags_sheet_edit_show_all(GtkWidget *widget);

gboolean ags_sheet_edit_auto_scroll_timeout(GtkWidget *widget);

/**
 * SECTION:ags_sheet_edit
 * @short_description: edit notes
 * @title: AgsSheetEdit
 * @section_id:
 * @include: ags/X/editor/ags_sheet_edit.h
 *
 * The #AgsSheetEdit lets you edit notes.
 */

enum{
  PROP_0,
};

static gpointer ags_sheet_edit_parent_class = NULL;

static GQuark quark_accessible_object = 0;

GtkStyle *sheet_edit_style = NULL;

GHashTable *ags_sheet_edit_auto_scroll = NULL;

GType
ags_sheet_edit_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sheet_edit = 0;

    static const GTypeInfo ags_sheet_edit_info = {
      sizeof (AgsSheetEditClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sheet_edit_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSheetEdit),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sheet_edit_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sheet_edit_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_sheet_edit = g_type_register_static(GTK_TYPE_TABLE,
						    "AgsSheetEdit", &ags_sheet_edit_info,
						    0);
    
    g_type_add_interface_static(ags_type_sheet_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sheet_edit);
  }

  return g_define_type_id__volatile;
}

static GType
ags_accessible_sheet_edit_get_type(void)
{
  static GType ags_type_accessible_sheet_edit = 0;

  if(!ags_type_accessible_sheet_edit){
    const GTypeInfo ags_accesssible_sheet_edit_info = {
      sizeof(GtkAccessibleClass),
      NULL,           /* base_init */
      NULL,           /* base_finalize */
      (GClassInitFunc) ags_accessible_sheet_edit_class_init,
      NULL,           /* class_finalize */
      NULL,           /* class_data */
      sizeof(GtkAccessible),
      0,             /* n_preallocs */
      NULL, NULL
    };

    static const GInterfaceInfo atk_action_interface_info = {
      (GInterfaceInitFunc) ags_accessible_sheet_edit_action_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_accessible_sheet_edit = g_type_register_static(GTK_TYPE_ACCESSIBLE,
							       "AgsAccessibleSheetEdit", &ags_accesssible_sheet_edit_info,
							       0);

    g_type_add_interface_static(ags_type_accessible_sheet_edit,
				ATK_TYPE_ACTION,
				&atk_action_interface_info);
  }
  
  return(ags_type_accessible_sheet_edit);
}

void
ags_sheet_edit_class_init(AgsSheetEditClass *sheet_edit)
{
  GtkWidgetClass *widget;
  
  GObjectClass *gobject;
  
  ags_sheet_edit_parent_class = g_type_class_peek_parent(sheet_edit);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(sheet_edit);

  gobject->finalize = ags_sheet_edit_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) sheet_edit;

  widget->show = ags_sheet_edit_show;
  widget->show_all = ags_sheet_edit_show_all;
}

void
ags_accessible_sheet_edit_class_init(AtkObject *object)
{
  quark_accessible_object = g_quark_from_static_string("ags-accessible-object");
}

void
ags_accessible_sheet_edit_action_interface_init(AtkActionIface *action)
{
  action->do_action = ags_accessible_sheet_edit_do_action;
  action->get_n_actions = ags_accessible_sheet_edit_get_n_actions;
  action->get_description = ags_accessible_sheet_edit_get_description;
  action->get_name = ags_accessible_sheet_edit_get_name;
  action->get_keybinding = ags_accessible_sheet_edit_get_keybinding;
  action->set_description = ags_accessible_sheet_edit_set_description;
  action->get_localized_name = ags_accessible_sheet_edit_get_localized_name;
}

void
ags_sheet_edit_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_sheet_edit_connect;
  connectable->disconnect = ags_sheet_edit_disconnect;
}

void
ags_sheet_edit_init(AgsSheetEdit *sheet_edit)
{
  GtkAdjustment *adjustment;

  sheet_edit->flags = 0;
  sheet_edit->mode = AGS_SHEET_EDIT_NO_EDIT_MODE;

  sheet_edit->button_mask = 0;
  sheet_edit->key_mask = 0;

  sheet_edit->note_offset = 0;
  sheet_edit->note_offset_absolute = 0;

  sheet_edit->cursor_position_x = AGS_SHEET_EDIT_DEFAULT_CURSOR_POSITION_X;
  sheet_edit->cursor_position_y = AGS_SHEET_EDIT_DEFAULT_CURSOR_POSITION_Y;

  sheet_edit->selection_x0 = 0;
  sheet_edit->selection_x1 = 0;
  sheet_edit->selection_y0 = 0;
  sheet_edit->selection_y1 = 0;

  if(sheet_edit_style == NULL){
    sheet_edit_style = gtk_style_copy(gtk_widget_get_style((GtkWidget *) sheet_edit));
  }

  sheet_edit->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_events(GTK_WIDGET(sheet_edit->drawing_area), GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			| GDK_CONTROL_MASK
			| GDK_KEY_PRESS_MASK
			| GDK_KEY_RELEASE_MASK);
  gtk_widget_set_can_focus((GtkWidget *) sheet_edit->drawing_area,
			   TRUE);
  
  gtk_table_attach(GTK_TABLE(sheet_edit),
		   (GtkWidget *) sheet_edit->drawing_area,
		   0, 1,
		   1, 2,
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);

  /* auto-scroll */
  if(ags_sheet_edit_auto_scroll == NULL){
    ags_sheet_edit_auto_scroll = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							  NULL,
							  NULL);
  }

  g_hash_table_insert(ags_sheet_edit_auto_scroll,
		      sheet_edit, ags_sheet_edit_auto_scroll_timeout);
  g_timeout_add(1000 / 30, (GSourceFunc) ags_sheet_edit_auto_scroll_timeout, (gpointer) sheet_edit);
}

void
ags_sheet_edit_finalize(GObject *gobject)
{
  AgsSheetEdit *sheet_edit;
  
  sheet_edit = AGS_SHEET_EDIT(gobject);
  
  /* remove auto scroll */
  g_hash_table_remove(ags_sheet_edit_auto_scroll,
		      sheet_edit);

  /* call parent */
  G_OBJECT_CLASS(ags_sheet_edit_parent_class)->finalize(gobject);
}

AtkObject*
ags_sheet_edit_get_accessible(GtkWidget *widget)
{
  AtkObject* accessible;

  accessible = g_object_get_qdata(G_OBJECT(widget),
				  quark_accessible_object);
  
  if(!accessible){
    accessible = g_object_new(ags_accessible_sheet_edit_get_type(),
			      NULL);
    
    g_object_set_qdata(G_OBJECT(widget),
		       quark_accessible_object,
		       accessible);
    gtk_accessible_set_widget(GTK_ACCESSIBLE(accessible),
			      widget);
  }
  
  return(accessible);
}

void
ags_sheet_edit_connect(AgsConnectable *connectable)
{
  AgsSheetEdit *sheet_edit;

  sheet_edit = AGS_SHEET_EDIT(connectable);

  if((AGS_SHEET_EDIT_CONNECTED & (sheet_edit->flags)) != 0){
    return;
  }

  sheet_edit->flags |= AGS_SHEET_EDIT_CONNECTED;
  
  /* drawing area */
  g_signal_connect_after((GObject *) sheet_edit->drawing_area, "configure_event",
			 G_CALLBACK(ags_sheet_edit_drawing_area_configure_event), (gpointer) sheet_edit);

  g_signal_connect((GObject *) sheet_edit->drawing_area, "button_press_event",
		   G_CALLBACK(ags_sheet_edit_drawing_area_button_press_event), (gpointer) sheet_edit);

  g_signal_connect((GObject *) sheet_edit->drawing_area, "button_release_event",
		   G_CALLBACK(ags_sheet_edit_drawing_area_button_release_event), (gpointer) sheet_edit);

  g_signal_connect((GObject *) sheet_edit->drawing_area, "motion_notify_event",
		   G_CALLBACK(ags_sheet_edit_drawing_area_motion_notify_event), (gpointer) sheet_edit);
			
  g_signal_connect((GObject *) sheet_edit->drawing_area, "key_press_event",
		   G_CALLBACK(ags_sheet_edit_drawing_area_key_press_event), (gpointer) sheet_edit);

  g_signal_connect((GObject *) sheet_edit->drawing_area, "key_release_event",
		   G_CALLBACK(ags_sheet_edit_drawing_area_key_release_event), (gpointer) sheet_edit);
}

void
ags_sheet_edit_disconnect(AgsConnectable *connectable)
{
  AgsSheetEdit *sheet_edit;

  sheet_edit = AGS_SHEET_EDIT(connectable);

  if((AGS_SHEET_EDIT_CONNECTED & (sheet_edit->flags)) == 0){
    return;
  }

  sheet_edit->flags &= (~AGS_SHEET_EDIT_CONNECTED);

  /* drawing area */
  g_object_disconnect(sheet_edit->drawing_area,
		      "any_signal::configure_event",
		      G_CALLBACK(ags_sheet_edit_drawing_area_configure_event),
		      (gpointer) sheet_edit,
		      "any_signal::button_press_event",
		      G_CALLBACK(ags_sheet_edit_drawing_area_button_press_event),
		      (gpointer) sheet_edit,
		      "any_signal::button_release_event",
		      G_CALLBACK(ags_sheet_edit_drawing_area_button_release_event),
		      (gpointer) sheet_edit,
		      "any_signal::motion_notify_event",
		      G_CALLBACK(ags_sheet_edit_drawing_area_motion_notify_event),
		      sheet_edit,
		      "any_signal::key_press_event",
		      G_CALLBACK(ags_sheet_edit_drawing_area_key_press_event),
		      (gpointer) sheet_edit,
		      "any_signal::key_release_event",
		      G_CALLBACK(ags_sheet_edit_drawing_area_key_release_event),
		      (gpointer) sheet_edit,
		      NULL);
}

gboolean
ags_accessible_sheet_edit_do_action(AtkAction *action,
				       gint i)
{
  AgsSheetEdit *sheet_edit;
  
  GdkEventKey *key_press, *key_release;
  GdkEventKey *modifier_press, *modifier_release;
  GdkEventKey *second_level_press, *second_level_release;
  
  if(!(i >= 0 && i < 12)){
    return(FALSE);
  }

  sheet_edit = (AgsSheetEdit *) gtk_accessible_get_widget(GTK_ACCESSIBLE(action));
  
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
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 1:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Right;
      
      /* send event */
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 2:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Up;
    
      /* send event */
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 3:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Down;
      
      /* send event */
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 4:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_space;
      
      /* send event */
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 5:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Left;
      
      /* send event */
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) second_level_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) second_level_release);
    }
    break;
  case 6:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Right;
      
      /* send event */
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) second_level_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) second_level_release);
    }
    break;
  case 7:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Delete;
      
      /* send event */
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 8:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_c;

      /* send event */
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }    
    break;
  case 9:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_x;

      /* send event */
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }
    break;
  case 10:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_v;

      /* send event */
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }
    break;
  case 11:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_a;

      /* send event */
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) sheet_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }
    break;
  }

  return(TRUE);
}

gint
ags_accessible_sheet_edit_get_n_actions(AtkAction *action)
{
  return(12);
}

const gchar*
ags_accessible_sheet_edit_get_description(AtkAction *action,
					     gint i)
{
  static const gchar *actions[] = {
    "move cursor left",
    "move cursor right",
    "move cursor up",
    "move cursor down",
    "add audio note",
    "shrink audio note",
    "grow audio note",
    "remove audio note",
    "copy note to clipboard",
    "cut note to clipbaord",
    "paste note from clipboard",
    "select all note",
  };

  if(i >= 0 && i < 12){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_sheet_edit_get_name(AtkAction *action,
				      gint i)
{
  static const gchar *actions[] = {
    "left",
    "right",
    "up",
    "down",
    "add",
    "shrink",
    "grow",
    "remove",
    "copy",
    "cut",
    "paste",
    "select-all",
  };
  
  if(i >= 0 && i < 12){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_sheet_edit_get_keybinding(AtkAction *action,
					    gint i)
{
  static const gchar *actions[] = {
    "left",
    "right",
    "up",
    "down",
    "space",
    "Shft+Left",
    "Shft+Right",
    "Del"
    "Ctrl+c"
    "Ctrl+x",
    "Ctrl+v",
    "Ctrl+a",
  };
  
  if(i >= 0 && i < 12){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

gboolean
ags_accessible_sheet_edit_set_description(AtkAction *action,
					     gint i)
{
  //TODO:JK: implement me

  return(FALSE);
}

gchar*
ags_accessible_sheet_edit_get_localized_name(AtkAction *action,
						gint i)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_sheet_edit_show(GtkWidget *widget)
{
  AgsSheetEdit *sheet_edit;

  sheet_edit = AGS_SHEET_EDIT(widget);

  /* call parent */
  GTK_WIDGET_CLASS(ags_sheet_edit_parent_class)->show(widget);
}

void
ags_sheet_edit_show_all(GtkWidget *widget)
{
  AgsSheetEdit *sheet_edit;

  sheet_edit = AGS_SHEET_EDIT(widget);

  /* call parent */
  GTK_WIDGET_CLASS(ags_sheet_edit_parent_class)->show_all(widget);
}

gboolean
ags_sheet_edit_auto_scroll_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_sheet_edit_auto_scroll,
			 widget) != NULL){
    AgsSheetEditor *sheet_editor;
    AgsSheetEdit *sheet_edit;

    GObject *output_soundcard;
    
    double x;
    
    sheet_edit = AGS_SHEET_EDIT(widget);

    if((AGS_SHEET_EDIT_AUTO_SCROLL & (sheet_edit->flags)) == 0){
      return(TRUE);
    }
    
    sheet_editor = (AgsSheetEditor *) gtk_widget_get_ancestor((GtkWidget *) sheet_edit,
							      AGS_TYPE_SHEET_EDITOR);
    
    if(sheet_editor->selected_machine == NULL){
      return(TRUE);
    }

    //TODO:JK: implement me
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

void
ags_sheet_edit_draw(AgsSheetEdit *sheet_edit)
{
  //TODO:JK: implement me
}

/**
 * ags_sheet_edit_new:
 *
 * Create a new #AgsSheetEdit.
 *
 * Returns: a new #AgsSheetEdit
 * 
 * Since: 3.0.0
 */
AgsSheetEdit*
ags_sheet_edit_new()
{
  AgsSheetEdit *sheet_edit;

  sheet_edit = (AgsSheetEdit *) g_object_new(AGS_TYPE_SHEET_EDIT,
						   NULL);

  return(sheet_edit);
}
