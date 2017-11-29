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

#include <ags/X/ags_notation_editor.h>

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
  PROP_ZOOM_FACTOR,
};

static gpointer ags_line_parent_class = NULL;

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
  quark_accessible_object = g_quark_from_static_string("ags-accessible-object");
}

void
ags_accessible_notation_edit_class_init(AtkObject *object)
{
  GObjectClass *gobject;
  
  GParamSpec *param_spec;
  
  ags_line_parent_class = g_type_class_peek_parent(line);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(line);

  gobject->set_property = ags_line_set_property;
  gobject->get_property = ags_line_get_property;

  gobject->finalize = ags_line_finalize;

  /* properties */
  /**
   * AgsNotationEdit:zoom-factor:
   *
   * The zoom-factor to use to draw notes.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_double("zoom-factor",
				   i18n_pspec("zoom factor of notes"),
				   i18n_pspec("The zoom factor to use for drawing notes"),
				   AGS_NOTATION_EDIT_MAX_ZOOM_FACTOR,
				   AGS_NOTATION_EDIT_MIN_ZOOM_FACTOR,
				   AGS_NOTATION_EDIT_DEFAULT_ZOOM_FACTOR,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ZOOM_FACTOR,
				  param_spec);
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

  notation_edit->tact_length = AGS_NOTATION_EDIT_DEFAULT_TACT_LENGTH;

  notation_edit->zoom_factor = AGS_NOTATION_EDIT_DEFAULT_ZOOM_FACTOR;

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
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 16.0, 1.0);
  notation_edit->vscrollbar = gtk_vscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(notation_edit),
		   (GtkWidget *) notation_edit->vscrollbar,
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* hscrollbar */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (gdouble) notation_edit->control_width, 1.0);
  notation_edit->vscrollbar = gtk_hrollbar_new(adjustment);
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
		      child_widget, ags_notation_edit_auto_scroll_timeout);
  g_timeout_add(1000 / 30, (GSourceFunc) ags_notation_edit_auto_scroll_timeout, (gpointer) child_widget);
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
  case PROP_ZOOM_FACTOR:
    {
      notation_edit->zoom_factor = g_value_get_double(value);

      gtk_widget_queue_draw(notation_edit);
    }
    break;
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
  case PROP_ZOOM_FACTOR:
    {
      g_value_set_double(value, notation_edit->zoom_factor);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_notation_edit_connect(AgsConnectable *connectable);
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
ags_notation_edit_disconnect(AgsConnectable *connectable);
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
ags_notation_edit_get_accessible(GtkWidget *widget);
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
				       gint i);
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
ags_accessible_notation_edit_get_n_actions(AtkAction *action);
{
  return(13);
}

const gchar*
ags_accessible_notation_edit_get_description(AtkAction *action,
					     gint i);
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
				      gint i);
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
					    gint i);
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
					     gint i);
{
  //TODO:JK: implement me

  return(FALSE);
}

gchar*
ags_accessible_notation_edit_get_localized_name(AtkAction *action,
						gint i);
{
  //TODO:JK: implement me

  return(NULL);
}

gboolean
ags_notation_edit_auto_scroll_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_notation_edit_auto_scroll,
			 widget) != NULL){
    AgsNotationEdit *notation_edit;

    notation_edit = AGS_NOTATION_EDIT(widget);

    if((AGS_NOTATION_EDIT_AUTO_SCROLL & (notation_edit->flags)) == 0){
      return(TRUE);
    }
    
    //TODO:JK: implement me
    
    /* queue draw */
    gtk_widget_queue_draw(widget);
    
    return(TRUE);
  }else{
    return(FALSE);
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
  double tact;
  guint y0, x0;
  guint nth_x;
  guint i, j;
  guint j_set;

  static const gdouble white_gc = 65535.0;

  if(!AGS_NOTATION_EDIT(notation_edit)){
    return;
  }

  notation_editor = gtk_widget_get_ancestor(notation_edit,
					    AGS_NOTATION_EDITOR);
  notation_toolbar = notation_editor->notation_toolbar;
  
  notation_edit_style = gtk_widget_get_style(GTK_WIDGET(notation_edit->drawing_area));

  cr = gdk_cairo_create(GTK_WIDGET(notation_edit->drawing_area)->window);

  if(cr == NULL){
    return;
  }

  /* get channel count */
  if((AGS_AUDIO_NOTATION_DEFAULT & (notation_editor->machine->audio->flags)) != 0){
    channel_count = notation_editor->machine->audio->input_pads;
  }else{
    channel_count = notation_editor->machine->audio->output_pads;
  }

  /* get width */
  width = GTK_WIDGET(notation_edit->drawing_area)->allocation.width;
  width_fits = FALSE;
  
  if(width < AGS_NOTATION_EDITOR_MAX_CONTROLS * notation_edit->control_width){
    width = AGS_NOTATION_EDITOR_MAX_CONTROLS * notation_edit->control_width;
    width_fits = TRUE;
  }

  /* get height */
  height = GTK_WIDGET(notation_edit->drawing_area)->allocation.height;
  height_fits = FALSE;

  if(height < channel_count * notation_edit->control_height){
    height = ;
    height_fits = TRUE;
  }

  /* tact */
  tact = exp2((double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom) - 2.0);

  /*  */
  if(width_fits){
    x0 = 0;
  }else{
    x0 = ((guint) GTK_RANGE(notation_edit->hscrollbar)->adjustment->value) % notation_edit->control_width;
  }

  if(height_fits){
    y0 = 0;
  }else{
    y0 = ((guint) GTK_RANGE(notation_edit->vscrollbar)->adjustment->value) % notation_edit->control_height;
  }
  
  nth_x = (guint) floor(GTK_RANGE(notation_edit->hscrollbar)->adjustment->value / notation_edit->control_width);

  cairo_push_group(cr);

  /* clear with background color */
  cairo_set_source_rgb(cr,
		       notation_edit_style->bg[0].red / white_gc,
		       notation_edit_style->bg[0].green / white_gc,
		       notation_edit_style->bg[0].blue / white_gc);

  cairo_rectangle(cr,
		  0.0, 0.0,
		  (double) widget->allocation.width, (double) widget->allocation.height);
  
  cairo_fill(cr);
  
  /* horizontal lines */
  cairo_set_line_width(cr,
		       1.0);

  cairo_set_source_rgb(cr,
		       notation_edit_style->fg[0].red / white_gc,
		       notation_edit_style->fg[0].green / white_gc,
		       notation_edit_style->fg[0].blue / white_gc);

  for(i = y0 ; i < ; ){
    cairo_move_to(cr,
		  0.0, (double) i);
    cairo_line_to(cr,
		  (double) width, (double) i);
    cairo_stroke(cr);

    i += notation_edit->control_height;
  }

  cairo_move_to(cr,
		0.0, (double) i);
  cairo_line_to(cr,
		(double) width, (double) i);
  cairo_stroke(cr);

  /* vertical lines */
  i = x0;
  
  if(i < width &&
     tact > 1.0 ){
    j_set = nth_x % ((guint) tact);

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
    
    for(j = 1; i < width && j < tact; j++){
    ags_notation_edit_draw_segment0:
      cairo_move_to(cr, (double) i, 0.0);
      cairo_line_to(cr, (double) i, (double) notation_edit->height);
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
  cairo_t *cr;

  if(!AGS_NOTATION_EDIT(notation_edit)){
    return;
  }
  
  cr = gdk_cairo_create(GTK_WIDGET(notation_edit->drawing_area)->window);

  if(cr == NULL){
    return;
  }
  
  cairo_push_group(cr);

  //TODO:JK: implement me

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
      
  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);
}

void
ags_notation_edit_draw_cursor(AgsNotationEdit *notation_edit)
{
  cairo_t *cr;

  if(!AGS_NOTATION_EDIT(notation_edit)){
    return;
  }
  
  cr = gdk_cairo_create(GTK_WIDGET(notation_edit->drawing_area)->window);

  if(cr == NULL){
    return;
  }
  
  cairo_push_group(cr);

  //TODO:JK: implement me

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
      
  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);
}

void
ags_notation_edit_draw_selection(AgsNotationEdit *notation_edit)
{
  cairo_t *cr;

  if(!AGS_NOTATION_EDIT(notation_edit)){
    return;
  }
  
  cr = gdk_cairo_create(GTK_WIDGET(notation_edit->drawing_area)->window);

  if(cr == NULL){
    return;
  }
  
  cairo_push_group(cr);

  //TODO:JK: implement me

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
      
  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);
}

void
ags_notation_edit_draw_notation(AgsNotationEdit *notation_edit)
{
  cairo_t *cr;

  if(!AGS_NOTATION_EDIT(notation_edit)){
    return;
  }
  
  cr = gdk_cairo_create(GTK_WIDGET(notation_edit->drawing_area)->window);

  if(cr == NULL){
    return;
  }
  
  cairo_push_group(cr);

  //TODO:JK: implement me

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
      
  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);
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
