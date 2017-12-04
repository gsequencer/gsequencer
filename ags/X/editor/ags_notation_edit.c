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

#include <ags/X/editor/ags_notation_edit.h>
#include <ags/X/editor/ags_notation_edit_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/X/ags_notation_editor.h>

#include <gdk/gdkkeysyms.h>

#include <math.h>

static GType ags_accessible_notation_edit_get_type(void);
void ags_notation_edit_class_init(AgsNotationEditClass *notation_edit);
void ags_accessible_notation_edit_class_init(AtkObject *object);
void ags_accessible_notation_edit_action_interface_init(AtkActionIface *action);
void ags_notation_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_notation_edit_init(AgsNotationEdit *notation_edit);
void ags_notation_edit_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_notation_edit_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_notation_edit_connect(AgsConnectable *connectable);
void ags_notation_edit_disconnect(AgsConnectable *connectable);
AtkObject* ags_notation_edit_get_accessible(GtkWidget *widget);
void ags_notation_edit_finalize(GObject *gobject);

gboolean ags_accessible_notation_edit_do_action(AtkAction *action,
						gint i);
gint ags_accessible_notation_edit_get_n_actions(AtkAction *action);
const gchar* ags_accessible_notation_edit_get_description(AtkAction *action,
							  gint i);
const gchar* ags_accessible_notation_edit_get_name(AtkAction *action,
						   gint i);
const gchar* ags_accessible_notation_edit_get_keybinding(AtkAction *action,
							 gint i);
gboolean ags_accessible_notation_edit_set_description(AtkAction *action,
						      gint i);
gchar* ags_accessible_notation_edit_get_localized_name(AtkAction *action,
						       gint i);

gboolean ags_notation_edit_auto_scroll_timeout(GtkWidget *widget);

/**
 * SECTION:ags_notation_edit
 * @short_description: edit notes
 * @title: AgsNotationEdit
 * @section_id:
 * @include: ags/X/editor/ags_notation_edit.h
 *
 * The #AgsNotationEdit lets you edit notes.
 */

enum{
  PROP_0,
};

static gpointer ags_notation_edit_parent_class = NULL;

static GQuark quark_accessible_object = 0;

GtkStyle *notation_edit_style = NULL;

GHashTable *ags_notation_edit_auto_scroll = NULL;

GType
ags_notation_edit_get_type(void)
{
  static GType ags_type_notation_edit = 0;

  if(!ags_type_notation_edit){
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

    ags_type_notation_edit = g_type_register_static(GTK_TYPE_TABLE,
						    "AgsNotationEdit", &ags_notation_edit_info,
						    0);
    
    g_type_add_interface_static(ags_type_notation_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_notation_edit);
}

static GType
ags_accessible_notation_edit_get_type(void)
{
  static GType ags_type_accessible_notation_edit = 0;

  if(!ags_type_accessible_notation_edit){
    const GTypeInfo ags_accesssible_notation_edit_info = {
      sizeof(GtkAccessibleClass),
      NULL,           /* base_init */
      NULL,           /* base_finalize */
      (GClassInitFunc) ags_accessible_notation_edit_class_init,
      NULL,           /* class_finalize */
      NULL,           /* class_data */
      sizeof(GtkAccessible),
      0,             /* n_preallocs */
      NULL, NULL
    };

    static const GInterfaceInfo atk_action_interface_info = {
      (GInterfaceInitFunc) ags_accessible_notation_edit_action_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_accessible_notation_edit = g_type_register_static(GTK_TYPE_ACCESSIBLE,
							       "AgsAccessibleNotationEdit", &ags_accesssible_notation_edit_info,
							       0);

    g_type_add_interface_static(ags_type_accessible_notation_edit,
				ATK_TYPE_ACTION,
				&atk_action_interface_info);
  }
  
  return(ags_type_accessible_notation_edit);
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
ags_notation_edit_class_init(AgsNotationEditClass *notation_edit)
{
  GObjectClass *gobject;
  
  GParamSpec *param_spec;
  
  ags_notation_edit_parent_class = g_type_class_peek_parent(notation_edit);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(notation_edit);

  gobject->set_property = ags_notation_edit_set_property;
  gobject->get_property = ags_notation_edit_get_property;

  gobject->finalize = ags_notation_edit_finalize;
}

void
ags_accessible_notation_edit_class_init(AtkObject *object)
{
  quark_accessible_object = g_quark_from_static_string("ags-accessible-object");
}

void
ags_accessible_notation_edit_action_interface_init(AtkActionIface *action)
{
  action->do_action = ags_accessible_notation_edit_do_action;
  action->get_n_actions = ags_accessible_notation_edit_get_n_actions;
  action->get_description = ags_accessible_notation_edit_get_description;
  action->get_name = ags_accessible_notation_edit_get_name;
  action->get_keybinding = ags_accessible_notation_edit_get_keybinding;
  action->set_description = ags_accessible_notation_edit_set_description;
  action->get_localized_name = ags_accessible_notation_edit_get_localized_name;
}

void
ags_notation_edit_init(AgsNotationEdit *notation_edit)
{
  GtkAdjustment *adjustment;

  notation_edit->flags = 0;
  notation_edit->mode = 0;

  notation_edit->key_mask = 0;

  notation_edit->note_offset = 0;
  notation_edit->note_offset_absolute = 0;

  notation_edit->control_width = AGS_NOTATION_EDIT_DEFAULT_CONTROL_WIDTH;
  notation_edit->control_height = AGS_NOTATION_EDIT_DEFAULT_CONTROL_HEIGHT;

  notation_edit->control_margin_x = AGS_NOTATION_EDIT_DEFAULT_CONTROL_MARGIN_X;
  notation_edit->control_margin_y = AGS_NOTATION_EDIT_DEFAULT_CONTROL_MARGIN_Y;

  notation_edit->cursor_position_x = AGS_NOTATION_EDIT_DEFAULT_CURSOR_POSITION_X;
  notation_edit->cursor_position_y = AGS_NOTATION_EDIT_DEFAULT_CURSOR_POSITION_Y;

  notation_edit->selection_x0 = 0;
  notation_edit->selection_x1 = 0;
  notation_edit->selection_y0 = 0;
  notation_edit->selection_y1 = 0;

  notation_edit->current_note = NULL;

  notation_edit->ruler = ags_ruler_new();
  gtk_table_attach(GTK_TABLE(notation_edit),
		   (GtkWidget *) notation_edit->ruler,
		   0, 1,
		   0, 1,
		   GTK_FILL|GTK_EXPAND,
		   GTK_FILL,
		   0, 0);

  notation_edit->drawing_area = gtk_drawing_area_new();
  gtk_widget_set_events(GTK_WIDGET(notation_edit->drawing_area), GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			| GDK_CONTROL_MASK
			| GDK_KEY_PRESS_MASK
			| GDK_KEY_RELEASE_MASK);
  gtk_widget_set_can_focus((GtkWidget *) notation_edit->drawing_area,
			   TRUE);
  
  gtk_table_attach(GTK_TABLE(notation_edit),
		   (GtkWidget *) notation_edit->drawing_area,
		   0, 1,
		   1, 2,
		   GTK_FILL|GTK_EXPAND,
		   GTK_FILL|GTK_EXPAND,
		   0, 0);

  /* vscrollbar */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, notation_edit->control_height, 1.0);
  notation_edit->vscrollbar = gtk_vscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(notation_edit),
		   (GtkWidget *) notation_edit->vscrollbar,
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* hscrollbar */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (gdouble) notation_edit->control_width, 1.0);
  notation_edit->hscrollbar = gtk_hscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(notation_edit),
		   (GtkWidget *) notation_edit->hscrollbar,
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

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
ags_notation_edit_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsNotationEdit *notation_edit;

  notation_edit = AGS_NOTATION_EDIT(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_notation_edit_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsNotationEdit *notation_edit;

  notation_edit = AGS_NOTATION_EDIT(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_notation_edit_connect(AgsConnectable *connectable)
{
  AgsNotationEdit *notation_edit;

  notation_edit = AGS_NOTATION_EDIT(connectable);

  if((AGS_NOTATION_EDIT_CONNECTED & (notation_edit->flags)) != 0){
    return;
  }

  notation_edit->flags |= AGS_NOTATION_EDIT_CONNECTED;
  
  g_signal_connect_after((GObject *) notation_edit->drawing_area, "expose_event",
			 G_CALLBACK(ags_notation_edit_drawing_area_expose_event), (gpointer) notation_edit);

  g_signal_connect_after((GObject *) notation_edit->drawing_area, "configure_event",
			 G_CALLBACK(ags_notation_edit_drawing_area_configure_event), (gpointer) notation_edit);

  g_signal_connect((GObject *) notation_edit->drawing_area, "button_press_event",
		   G_CALLBACK(ags_notation_edit_drawing_area_button_press_event), (gpointer) notation_edit);

  g_signal_connect((GObject *) notation_edit->drawing_area, "button_release_event",
		   G_CALLBACK(ags_notation_edit_drawing_area_button_release_event), (gpointer) notation_edit);

  g_signal_connect((GObject *) notation_edit->drawing_area, "motion_notify_event",
		   G_CALLBACK(ags_notation_edit_drawing_area_motion_notify_event), (gpointer) notation_edit);
			
  g_signal_connect((GObject *) notation_edit->drawing_area, "key_press_event",
		   G_CALLBACK(ags_notation_edit_drawing_area_key_press_event), (gpointer) notation_edit);

  g_signal_connect((GObject *) notation_edit->drawing_area, "key_release_event",
		   G_CALLBACK(ags_notation_edit_drawing_area_key_release_event), (gpointer) notation_edit);

  g_signal_connect_after((GObject *) notation_edit->vscrollbar, "value-changed",
			 G_CALLBACK(ags_notation_edit_vscrollbar_value_changed), (gpointer) notation_edit);

  g_signal_connect_after((GObject *) notation_edit->hscrollbar, "value-changed",
			 G_CALLBACK(ags_notation_edit_hscrollbar_value_changed), (gpointer) notation_edit);
}

void
ags_notation_edit_disconnect(AgsConnectable *connectable)
{
  AgsNotationEdit *notation_edit;

  notation_edit = AGS_NOTATION_EDIT(connectable);

  if((AGS_NOTATION_EDIT_CONNECTED & (notation_edit->flags)) == 0){
    return;
  }

  notation_edit->flags &= (~AGS_NOTATION_EDIT_CONNECTED);

  g_object_disconnect(notation_edit->drawing_area,
		      "expose_event",
		      G_CALLBACK(ags_notation_edit_drawing_area_expose_event),
		      (gpointer) notation_edit,
		      NULL);

  g_object_disconnect(notation_edit->drawing_area,
		      "configure_event",
		      G_CALLBACK(ags_notation_edit_drawing_area_configure_event),
		      (gpointer) notation_edit,
		      NULL);

  g_object_disconnect(notation_edit->drawing_area,
		      "button_press_event",
		      G_CALLBACK(ags_notation_edit_drawing_area_button_press_event),
		      (gpointer) notation_edit,
		      NULL);

  g_object_disconnect(notation_edit->drawing_area,
		      "button_release_event",
		      G_CALLBACK(ags_notation_edit_drawing_area_button_release_event),
		      (gpointer) notation_edit,
		      NULL);

  g_object_disconnect(notation_edit->drawing_area,
		      "key_press_event",
		      G_CALLBACK(ags_notation_edit_drawing_area_key_press_event),
		      (gpointer) notation_edit,
		      NULL);

  g_object_disconnect(notation_edit->drawing_area,
		      "key_release_event",
		      G_CALLBACK(ags_notation_edit_drawing_area_key_release_event),
		      (gpointer) notation_edit,
		      NULL);

  g_object_disconnect(notation_edit->vscrollbar,
		      "value-changed",
		      G_CALLBACK(ags_notation_edit_vscrollbar_value_changed),
		      (gpointer) notation_edit,
		      NULL);
  
  g_object_disconnect(notation_edit->hscrollbar,
		      "value-changed",
		      G_CALLBACK(ags_notation_edit_hscrollbar_value_changed),
		      (gpointer) notation_edit,
		      NULL);
}

AtkObject*
ags_notation_edit_get_accessible(GtkWidget *widget)
{
  AtkObject* accessible;

  accessible = g_object_get_qdata(G_OBJECT(widget),
				  quark_accessible_object);
  
  if(!accessible){
    accessible = g_object_new(ags_accessible_notation_edit_get_type(),
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
ags_notation_edit_finalize(GObject *gobject)
{
  AgsNotationEdit *notation_edit;
  
  notation_edit = AGS_NOTATION_EDIT(gobject);
  
  /* remove indicator widget */
  g_hash_table_remove(ags_notation_edit_auto_scroll,
		      notation_edit);
}

gboolean
ags_accessible_notation_edit_do_action(AtkAction *action,
				       gint i)
{
  AgsNotationEdit *notation_edit;
  
  GdkEventKey *key_press, *key_release;
  GdkEventKey *modifier_press, *modifier_release;
  GdkEventKey *second_level_press, *second_level_release;
  
  if(!(i >= 0 && i < 13)){
    return(FALSE);
  }

  notation_edit = gtk_accessible_get_widget(GTK_ACCESSIBLE(action));
  
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
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 1:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Right;
      
      /* send event */
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 2:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Up;
    
      /* send event */
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 3:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Down;
      
      /* send event */
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 4:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_space;
      
      /* send event */
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 5:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Left;
      
      /* send event */
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) second_level_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) second_level_release);
    }
    break;
  case 6:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Right;
      
      /* send event */
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) second_level_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) second_level_release);
    }
    break;
  case 7:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Delete;
      
      /* send event */
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 8:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_c;

      /* send event */
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }    
    break;
  case 9:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_x;

      /* send event */
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }
    break;
  case 10:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_v;

      /* send event */
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }
    break;
  case 11:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_a;

      /* send event */
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }
    break;
  case 12:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_i;

      /* send event */
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) notation_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }
    break;
  }

  return(TRUE);
}

gint
ags_accessible_notation_edit_get_n_actions(AtkAction *action)
{
  return(13);
}

const gchar*
ags_accessible_notation_edit_get_description(AtkAction *action,
					     gint i)
{
  static const gchar **actions = {
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
    "invert note",
  };

  if(i >= 0 && i < 13){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_notation_edit_get_name(AtkAction *action,
				      gint i)
{
  static const gchar **actions = {
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
    "invert",
  };
  
  if(i >= 0 && i < 13){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_notation_edit_get_keybinding(AtkAction *action,
					    gint i)
{
  static const gchar **actions = {
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
    "Ctrl+i",
  };
  
  if(i >= 0 && i < 13){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

gboolean
ags_accessible_notation_edit_set_description(AtkAction *action,
					     gint i)
{
  //TODO:JK: implement me

  return(FALSE);
}

gchar*
ags_accessible_notation_edit_get_localized_name(AtkAction *action,
						gint i)
{
  //TODO:JK: implement me

  return(NULL);
}

gboolean
ags_notation_edit_auto_scroll_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_notation_edit_auto_scroll,
			 widget) != NULL){
    AgsNotationEditor *notation_editor;
    AgsNotationEdit *notation_edit;
    AgsNotationToolbar *notation_toolbar;

    double zoom;
    double x;
    
    notation_edit = AGS_NOTATION_EDIT(widget);

    if((AGS_NOTATION_EDIT_AUTO_SCROLL & (notation_edit->flags)) == 0){
      return(TRUE);
    }
    
    notation_editor = gtk_widget_get_ancestor(notation_edit,
					      AGS_TYPE_NOTATION_EDITOR);
    
    if(notation_editor->selected_machine == NULL){
      return(TRUE);
    }

    notation_toolbar = notation_editor->notation_toolbar;

    /* zoom */
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom) - 2.0);

    /* reset offset */
    notation_edit->note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(notation_editor->selected_machine->audio->soundcard));
    notation_edit->note_offset_absolute = ags_soundcard_get_note_offset_absolute(AGS_SOUNDCARD(notation_editor->selected_machine->audio->soundcard));

    /* reset scrollbar */
    x = ((notation_edit->note_offset * notation_edit->control_width) / (AGS_NOTATION_EDITOR_MAX_CONTROLS * notation_edit->control_width)) * GTK_RANGE(notation_edit->hscrollbar)->adjustment->upper;
    
    gtk_range_set_value(GTK_RANGE(notation_edit->hscrollbar),
			x);

    return(TRUE);
  }else{
    return(FALSE);
  }
}

void
ags_notation_edit_reset_vscrollbar(AgsNotationEdit *notation_edit)
{
  AgsNotationEditor *notation_editor;

  GtkAdjustment *adjustment;
  
  guint channel_count;
  double varea_height;
  gdouble upper, old_upper;
  
  if(!AGS_NOTATION_EDIT(notation_edit)){
    return;
  }

  notation_editor = gtk_widget_get_ancestor(notation_edit,
					    AGS_TYPE_NOTATION_EDITOR);

  if(notation_editor->selected_machine == NULL){
    return;
  }

  /* adjustment */
  adjustment = GTK_RANGE(notation_edit->vscrollbar)->adjustment;

  /* get channel count */
  if((AGS_AUDIO_NOTATION_DEFAULT & (notation_editor->selected_machine->audio->flags)) != 0){
    channel_count = notation_editor->selected_machine->audio->input_pads;
  }else{
    channel_count = notation_editor->selected_machine->audio->output_pads;
  }

  /* upper */
  old_upper = adjustment->upper; 

  varea_height = (channel_count * notation_edit->control_height);
  upper = varea_height - GTK_WIDGET(notation_edit->drawing_area)->allocation.height;

  if(upper < 0.0){
    upper = 0.0;
  }
	   
  gtk_adjustment_set_upper(adjustment,
			   upper);  

  /* reset value */
  if(old_upper != 0.0){
    gtk_adjustment_set_value(adjustment,
			     adjustment->value / old_upper * upper);
  }
}

void
ags_notation_edit_reset_hscrollbar(AgsNotationEdit *notation_edit)
{
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;

  GtkAdjustment *adjustment;

  double zoom_factor, zoom;
  double zoom_correction;
  guint map_width;
  gdouble upper, old_upper;
  
  if(!AGS_NOTATION_EDIT(notation_edit)){
    return;
  }

  notation_editor = gtk_widget_get_ancestor(notation_edit,
					    AGS_TYPE_NOTATION_EDITOR);

  if(notation_editor->selected_machine == NULL){
    return;
  }

  notation_toolbar = notation_editor->notation_toolbar;

  /* adjustment */
  adjustment = GTK_RANGE(notation_edit->hscrollbar)->adjustment;

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom) - 2.0);

  /* upper */
  old_upper = adjustment->upper;
  
  zoom_correction = 1.0 / 16;

  map_width = ((double) AGS_NOTATION_EDITOR_MAX_CONTROLS * notation_edit->control_width * zoom * zoom_correction);
  upper = map_width - GTK_WIDGET(notation_edit->drawing_area)->allocation.width;

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
			   upper / notation_edit->control_width);

  /* reset value */
  if(old_upper != 0.0){
    gtk_adjustment_set_value(adjustment,
			     adjustment->value / old_upper * upper);
  }
}

void
ags_notation_edit_draw_segment(AgsNotationEdit *notation_edit)
{
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  
  GtkStyle *notation_edit_style;

  cairo_t *cr;

  guint channel_count;
  guint width, height;
  gboolean width_fits, height_fits;
  double zoom;
  guint y0, x0;
  guint nth_x;
  guint i, j;
  guint j_set;

  static const gdouble white_gc = 65535.0;

  if(!AGS_NOTATION_EDIT(notation_edit)){
    return;
  }

  notation_editor = gtk_widget_get_ancestor(notation_edit,
					    AGS_TYPE_NOTATION_EDITOR);

  if(notation_editor->selected_machine == NULL){
    return;
  }

  notation_toolbar = notation_editor->notation_toolbar;
  
  notation_edit_style = gtk_widget_get_style(GTK_WIDGET(notation_edit->drawing_area));

  /* create cairo context */
  cr = gdk_cairo_create(GTK_WIDGET(notation_edit->drawing_area)->window);

  if(cr == NULL){
    return;
  }
  
  /* get channel count */
  if((AGS_AUDIO_NOTATION_DEFAULT & (notation_editor->selected_machine->audio->flags)) != 0){
    channel_count = notation_editor->selected_machine->audio->input_pads;
  }else{
    channel_count = notation_editor->selected_machine->audio->output_pads;
  }

  /* get width */
  width = GTK_WIDGET(notation_edit->drawing_area)->allocation.width;
  width_fits = FALSE;
  
  if(AGS_NOTATION_EDITOR_MAX_CONTROLS * notation_edit->control_width < width){
    width = AGS_NOTATION_EDITOR_MAX_CONTROLS * notation_edit->control_width;
    width_fits = TRUE;
  }

  /* get height */
  height = GTK_WIDGET(notation_edit->drawing_area)->allocation.height;
  height_fits = FALSE;

  if(channel_count * notation_edit->control_height < height){
    height = channel_count * notation_edit->control_height;
    height_fits = TRUE;
  }

  /* zoom */
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom) - 2.0);

  /*  */
  if(width_fits){
    x0 = 0;
  }else{
    x0 = notation_edit->control_width - ((guint) GTK_RANGE(notation_edit->hscrollbar)->adjustment->value) % notation_edit->control_width;
  }

  if(height_fits){
    y0 = 0;
  }else{
    y0 = notation_edit->control_height - ((guint) GTK_RANGE(notation_edit->vscrollbar)->adjustment->value) % notation_edit->control_height;
  }
  
  nth_x = (guint) floor(GTK_RANGE(notation_edit->hscrollbar)->adjustment->value / notation_edit->control_width);
  nth_x += 1;
  
  /* push group */
  cairo_push_group(cr);

  /* clear with background color */
  cairo_set_source_rgb(cr,
		       notation_edit_style->bg[0].red / white_gc,
		       notation_edit_style->bg[0].green / white_gc,
		       notation_edit_style->bg[0].blue / white_gc);

  cairo_rectangle(cr,
		  0.0, 0.0,
		  (double) GTK_WIDGET(notation_edit->drawing_area)->allocation.width, (double) GTK_WIDGET(notation_edit->drawing_area)->allocation.height);
  
  cairo_fill(cr);
  
  /* horizontal lines */
  cairo_set_line_width(cr,
		       1.0);

  cairo_set_source_rgb(cr,
		       notation_edit_style->fg[0].red / white_gc,
		       notation_edit_style->fg[0].green / white_gc,
		       notation_edit_style->fg[0].blue / white_gc);

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
  i = x0;
  
  if(i < width &&
     zoom > 1.0 ){
    j_set = nth_x % ((guint) zoom);

    /* thin lines */
    cairo_set_source_rgb(cr,
			 notation_edit_style->mid[0].red / white_gc,
			 notation_edit_style->mid[0].green / white_gc,
			 notation_edit_style->mid[0].blue / white_gc);

    if(j_set != 0){
      j = j_set;
      goto ags_notation_edit_draw_segment0;
    }
  }

  for(; i < width; ){
    /* strong lines */
    cairo_set_source_rgb(cr,
			 notation_edit_style->fg[0].red / white_gc,
			 notation_edit_style->fg[0].green / white_gc,
			 notation_edit_style->fg[0].blue / white_gc);
    
    cairo_move_to(cr,
		  (double) i, 0.0);
    cairo_line_to(cr,
		  (double) i, (double) height);
    cairo_stroke(cr);
    
    i += notation_edit->control_width;
    
    /* thin lines */
    cairo_set_source_rgb(cr,
			 notation_edit_style->mid[0].red / white_gc,
			 notation_edit_style->mid[0].green / white_gc,
			 notation_edit_style->mid[0].blue / white_gc);
    
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
      
  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);
}

void
ags_notation_edit_draw_position(AgsNotationEdit *notation_edit)
{
  AgsNotationEditor *notation_editor;

  GtkStyle *notation_edit_style;

  cairo_t *cr;

  guint channel_count;
  double position;
  double x, y;
  double width, height;
  gboolean height_fits;

  static const gdouble white_gc = 65535.0;

  if(!AGS_NOTATION_EDIT(notation_edit)){
    return;
  }

  notation_editor = gtk_widget_get_ancestor(notation_edit,
					    AGS_TYPE_NOTATION_EDITOR);

  if(notation_editor->selected_machine == NULL){
    return;
  }
  
  notation_edit_style = gtk_widget_get_style(GTK_WIDGET(notation_edit->drawing_area));

  /* create cairo context */
  cr = gdk_cairo_create(GTK_WIDGET(notation_edit->drawing_area)->window);

  if(cr == NULL){
    return;
  }
  
  /* get channel count */
  if((AGS_AUDIO_NOTATION_DEFAULT & (notation_editor->selected_machine->audio->flags)) != 0){
    channel_count = notation_editor->selected_machine->audio->input_pads;
  }else{
    channel_count = notation_editor->selected_machine->audio->output_pads;
  }

  /* get offset and dimensions */
  position = ((double) notation_edit->note_offset) * ((double) notation_edit->control_width);
  
  y = 0.0;
  x = (position) - (GTK_RANGE(notation_edit->hscrollbar)->adjustment->value);

  height = (double) GTK_WIDGET(notation_edit->drawing_area)->allocation.height;
  width = (double) AGS_NOTATION_EDIT_DEFAULT_FADER_WIDTH;

  if(height < channel_count * notation_edit->control_height){
    height = channel_count * notation_edit->control_height;
  }

  /* push group */
  cairo_push_group(cr);
  
  /* draw fader */
  cairo_set_source_rgba(cr,
			notation_edit_style->dark[0].red / white_gc,
			notation_edit_style->dark[0].green / white_gc,
			notation_edit_style->dark[0].blue / white_gc,
			0.5);
  cairo_rectangle(cr,
		  (double) x, (double) y,
		  (double) width, (double) height);
  cairo_fill(cr);

  /* complete */
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
      
  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);
}

void
ags_notation_edit_draw_cursor(AgsNotationEdit *notation_edit)
{
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;

  GtkStyle *notation_edit_style;

  double x, y;
  double width, height;

  cairo_t *cr;

  static const gdouble white_gc = 65535.0;

  if(!AGS_NOTATION_EDIT(notation_edit)){
    return;
  }
  
  notation_edit_style = gtk_widget_get_style(GTK_WIDGET(notation_edit->drawing_area));

  /* create cairo context */
  cr = gdk_cairo_create(GTK_WIDGET(notation_edit->drawing_area)->window);

  if(cr == NULL){
    return;
  }

  /* get offset */
  x = ((double) notation_edit->cursor_position_x * (double) notation_edit->control_width) - GTK_RANGE(notation_edit->hscrollbar)->adjustment->value;
  y = ((double) notation_edit->cursor_position_y * (double) notation_edit->control_height) - GTK_RANGE(notation_edit->vscrollbar)->adjustment->value;

  width = (double) notation_edit->control_width;
  height = (double) notation_edit->control_height;

  /* clip */
  if(x < 0.0){
    width += x;

    x = 0.0;
  }else if(x > GTK_WIDGET(notation_edit->drawing_area)->allocation.width){
    return;
  }

  if(x + width > GTK_WIDGET(notation_edit->drawing_area)->allocation.width){
    width = ((double) GTK_WIDGET(notation_edit->drawing_area)->allocation.width) - x;
  }
  
  if(y < 0.0){
    height += y;

    y = 0.0;
  }else if(y > GTK_WIDGET(notation_edit->drawing_area)->allocation.height){
    return;
  }

  if(y + height > GTK_WIDGET(notation_edit->drawing_area)->allocation.height){
    height = ((double) GTK_WIDGET(notation_edit->drawing_area)->allocation.height) - y;
  }
      
  /* push group */
  cairo_push_group(cr);

  /* draw cursor */
  cairo_set_source_rgba(cr,
			notation_edit_style->base[0].red / white_gc,
			notation_edit_style->base[0].green / white_gc,
			notation_edit_style->base[0].blue / white_gc,
			0.5);
  cairo_rectangle(cr,
		  x, y,
		  width, height);
  cairo_fill(cr);

  /* complete */
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
      
  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);
}

void
ags_notation_edit_draw_selection(AgsNotationEdit *notation_edit)
{
  GtkStyle *notation_edit_style;

  cairo_t *cr;

  double x, y;
  double width, height;

  if(!AGS_NOTATION_EDIT(notation_edit)){
    return;
  }
  
  notation_edit_style = gtk_widget_get_style(GTK_WIDGET(notation_edit->drawing_area));

  /* create cairo context */
  cr = gdk_cairo_create(GTK_WIDGET(notation_edit->drawing_area)->window);

  if(cr == NULL){
    return;
  }

  /* get offset and dimensions */
  if(notation_edit->selection_x0 < notation_edit->selection_x1){
    x = ((double) notation_edit->selection_x0) - GTK_RANGE(notation_edit->hscrollbar)->adjustment->value;
    width = ((double) notation_edit->selection_x1 - (double) notation_edit->selection_x0);
  }else{
    x = ((double) notation_edit->selection_x1) - GTK_RANGE(notation_edit->hscrollbar)->adjustment->value;
    width = ((double) notation_edit->selection_x0 - (double) notation_edit->selection_x1);
  }

  if(notation_edit->selection_y0 < notation_edit->selection_y1){
    y = ((double) notation_edit->selection_y0) - GTK_RANGE(notation_edit->hscrollbar)->adjustment->value;
    height = ((double) notation_edit->selection_y1 - (double) notation_edit->selection_y0);
  }else{
    y = ((double) notation_edit->selection_y1) - GTK_RANGE(notation_edit->hscrollbar)->adjustment->value;
    height = ((double) notation_edit->selection_y0 - (double) notation_edit->selection_y1);
  }

  /* clip */
  if(x < 0.0){
    x = 0.0;
  }else if(x > GTK_WIDGET(notation_edit->drawing_area)->allocation.width){
    return;
  }

  if(x + width > GTK_WIDGET(notation_edit->drawing_area)->allocation.width){
    width = ((double) GTK_WIDGET(notation_edit->drawing_area)->allocation.width) - x;
  }
  
  if(y < 0.0){
    y = 0.0;
  }else if(y > GTK_WIDGET(notation_edit->drawing_area)->allocation.height){
    return;
  }

  if(y + height > GTK_WIDGET(notation_edit->drawing_area)->allocation.height){
    height = ((double) GTK_WIDGET(notation_edit->drawing_area)->allocation.height) - y;
  }
    
  /* push group */
  cairo_push_group(cr);

  /* draw selection */
  cairo_set_source_rgba(cr,
			1.0, 0.0, 0.0, 0.3);
  cairo_rectangle(cr,
		  x, y,
		  width, height);
  cairo_fill(cr);

  /* complete */
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
      
  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);
}

void
ags_notation_edit_draw_note(AgsNotationEdit *notation_edit,
			    AgsNote *note,
			    cairo_t *cr,
			    double r, double g, double b, double a)
{
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;

  GtkStyle *notation_edit_style;

  double zoom;
  guint channel_count;
  double viewport_x, viewport_y;
  double x, y;
  double width, height;
  
  static const gdouble white_gc = 65535.0;

  if(!AGS_NOTATION_EDIT(notation_edit) ||
     cr == NULL){
    return;
  }

  notation_editor = gtk_widget_get_ancestor(notation_edit,
					    AGS_TYPE_NOTATION_EDITOR);

  if(notation_editor->selected_machine == NULL){
    return;
  }

  notation_toolbar = notation_editor->notation_toolbar;
  
  notation_edit_style = gtk_widget_get_style(GTK_WIDGET(notation_edit->drawing_area));

  /* get channel count */
  if((AGS_AUDIO_NOTATION_DEFAULT & (notation_editor->selected_machine->audio->flags)) != 0){
    channel_count = notation_editor->selected_machine->audio->input_pads;
  }else{
    channel_count = notation_editor->selected_machine->audio->output_pads;
  }

  /* zoom */
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom) - 2.0);

  /* get offset and dimensions */
  viewport_x = ((AGS_NOTATION_EDITOR_MAX_CONTROLS * notation_edit->control_width) / GTK_RANGE(notation_edit->hscrollbar)->adjustment->upper) * GTK_RANGE(notation_edit->hscrollbar)->adjustment->value;
  viewport_y = ((channel_count * notation_edit->control_height) / GTK_RANGE(notation_edit->vscrollbar)->adjustment->upper) * GTK_RANGE(notation_edit->vscrollbar)->adjustment->value;
  
  x = ((double) note->x[0]) * ((double) notation_edit->control_width) - viewport_x;
  y = ((double) note->y) * ((double) notation_edit->control_height) - viewport_y;

  width = ((double) (note->x[1] - note->x[0])) * ((double) notation_edit->control_width);
  height = ((double) notation_edit->control_height);

  /* apply zoom */
  x /= ((1.0 / AGS_NOTATION_EDIT_MIN_ZOOM) * zoom);
  x += ((double) notation_edit->control_margin_x);

  y += ((double) notation_edit->control_margin_y);
  
  width /= ((1.0 / AGS_NOTATION_EDIT_MIN_ZOOM) * zoom);
  width -= (2.0 * (double) notation_edit->control_margin_x);
  
  height -= (2.0 * (double) notation_edit->control_margin_y);
  
  /* clip */
  if(x < 0.0){
    if(x + width < 0.0){
      return;
    }else{
      x = 0.0;
    }
  }else if(x > GTK_WIDGET(notation_edit->drawing_area)->allocation.width){
    return;
  }

  if(x + width > GTK_WIDGET(notation_edit->drawing_area)->allocation.width){
    width = ((double) GTK_WIDGET(notation_edit->drawing_area)->allocation.width) - x;
  }
  
  if(y < 0.0){
    y = 0.0;
  }else if(y > GTK_WIDGET(notation_edit->drawing_area)->allocation.height){
    return;
  }

  if(y + height > GTK_WIDGET(notation_edit->drawing_area)->allocation.height){
    height = ((double) GTK_WIDGET(notation_edit->drawing_area)->allocation.height) - y;
  }

  /* check note selected */
  if((AGS_NOTE_IS_SELECTED & (note->flags)) != 0){
    double selected_x, selected_y;
    double selected_width, selected_height;

    selected_x = x - notation_edit->control_margin_x;
    selected_y = y - notation_edit->control_margin_y;

    selected_width = width + (2.0 * (double) notation_edit->control_margin_x);
    selected_height = height + (2.0 * (double) notation_edit->control_margin_y);

    /* clip */
    if(selected_x < 0.0){
      selected_x = 0.0;
    }
    
    if(selected_x + selected_width > GTK_WIDGET(notation_edit->drawing_area)->allocation.width){
      selected_width = ((double) GTK_WIDGET(notation_edit->drawing_area)->allocation.width) - selected_x;
    }
  
    if(selected_y < 0.0){
      selected_y = 0.0;
    }

    if(selected_y + selected_height > GTK_WIDGET(notation_edit->drawing_area)->allocation.height){
      selected_height = ((double) GTK_WIDGET(notation_edit->drawing_area)->allocation.height) - selected_y;
    }

    /* draw selected note */
    cairo_set_source_rgba(cr,
			  notation_edit_style->light[0].red / white_gc,
			  notation_edit_style->light[0].green / white_gc,
			  notation_edit_style->light[0].blue / white_gc,
			  0.7);
    
    cairo_rectangle(cr,
		    selected_x, selected_y,
		    selected_width, selected_height);
    cairo_stroke(cr);
  }
  
  /* draw note */
  cairo_set_source_rgba(cr,
			r, g, b, a);
  cairo_rectangle(cr,
		  x, y,
		  width, height);
  cairo_fill(cr);
}

void
ags_notation_edit_draw_notation(AgsNotationEdit *notation_edit)
{
  AgsNotationEditor *notation_editor;

  GtkStyle *notation_edit_style;
  
  AgsMutexManager *mutex_manager;
  
  cairo_t *cr;

  GList *list_notation;
  GList *list_note;
  
  guint x0, x1;
  guint y0, y1;
  guint offset;
  gint i;    
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  static const gdouble white_gc = 65535.0;
  
  if(!AGS_NOTATION_EDIT(notation_edit)){
    return;
  }

  notation_editor = gtk_widget_get_ancestor(notation_edit,
					    AGS_TYPE_NOTATION_EDITOR);

  if(notation_editor->selected_machine == NULL){
    return;
  }
  
  notation_edit_style = gtk_widget_get_style(GTK_WIDGET(notation_edit->drawing_area));

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) notation_editor->selected_machine->audio);
  
  pthread_mutex_unlock(application_mutex);

  /* create cairo context */
  cr = gdk_cairo_create(GTK_WIDGET(notation_edit->drawing_area)->window);

  if(cr == NULL){
    return;
  }

  /* get visisble region */
  x0 = GTK_RANGE(notation_edit->hscrollbar)->adjustment->value / notation_edit->control_width;
  x1 = (GTK_RANGE(notation_edit->hscrollbar)->adjustment->value + GTK_WIDGET(notation_edit->drawing_area)->allocation.width) / notation_edit->control_width;

  y0 = GTK_RANGE(notation_edit->vscrollbar)->adjustment->value / notation_edit->control_height;
  y1 = (GTK_RANGE(notation_edit->vscrollbar)->adjustment->value + GTK_WIDGET(notation_edit->drawing_area)->allocation.height) / notation_edit->control_height;
  
  /* push group */
  cairo_push_group(cr);

  /* draw notation */
  pthread_mutex_lock(audio_mutex);

  i = 0;
  
  while((i = ags_notebook_next_active_tab(notation_editor->notebook,
					  i)) != -1){
    list_notation = notation_editor->selected_machine->audio->notation;

    while((list_notation = ags_notation_find_near_timestamp(list_notation, i,
							    NULL)) != NULL){
      AgsNotation *notation;

      GList *list_note;

      notation = AGS_NOTATION(list_notation->data);
      
      if(notation->timestamp != NULL &&
	 AGS_TIMESTAMP(notation->timestamp)->timer.ags_offset.offset > x1){
	break;
      }

      if(notation->timestamp != NULL &&
	 AGS_TIMESTAMP(notation->timestamp)->timer.ags_offset.offset + AGS_NOTATION_DEFAULT_OFFSET < x0){
	list_notation = list_notation->next;

	continue;
      }

      list_note = notation->notes;

      while(list_note != NULL){
	ags_notation_edit_draw_note(notation_edit,
				    list_note->data,
				    cr,
				    notation_edit_style->fg[0].red / white_gc,
				    notation_edit_style->fg[0].green / white_gc,
				    notation_edit_style->fg[0].blue / white_gc,
				    0.8);

	list_note = list_note->next;
      }

      list_notation = list_notation->next;
    }
    
    i++;
  }
  
  /* complete */
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
      
  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);
}

void
ags_notation_edit_draw(AgsNotationEdit *notation_edit)
{
  /* segment */
  ags_notation_edit_draw_segment(notation_edit);

  /* notation */
  ags_notation_edit_draw_notation(notation_edit);
  
  /* edit mode */
  switch(notation_edit->mode){
  case AGS_NOTATION_EDIT_POSITION_CURSOR:
    {
      ags_notation_edit_draw_cursor(notation_edit);
    }
    break;
  case AGS_NOTATION_EDIT_SELECT_NOTE:
    {
      ags_notation_edit_draw_selection(notation_edit);
    }
    break;
  }

  /* fader */
  if((AGS_NOTATION_EDIT_AUTO_SCROLL & (notation_edit->flags)) != 0){
    ags_notation_edit_draw_position(notation_edit);
  }
}

/**
 * ags_notation_edit_new:
 *
 * Create a new #AgsNotationEdit.
 *
 * Returns: a new #AgsNotationEdit
 * 
 * Since: 1.2.0
 */
AgsNotationEdit*
ags_notation_edit_new()
{
  AgsNotationEdit *notation_edit;

  notation_edit = (AgsNotationEdit *) g_object_new(AGS_TYPE_NOTATION_EDIT,
						   NULL);

  return(notation_edit);
}
