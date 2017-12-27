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

#include <ags/X/editor/ags_automation_edit.h>
#include <ags/X/editor/ags_automation_edit_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_automation_editor.h>

#include <gdk/gdkkeysyms.h>
#include <atk/atk.h>

#include <cairo.h>
#include <math.h>

static GType ags_accessible_automation_edit_get_type(void);
void ags_automation_edit_class_init(AgsAutomationEditClass *automation_edit);
void ags_accessible_automation_edit_class_init(AtkObject *object);
void ags_accessible_automation_edit_action_interface_init(AtkActionIface *action);
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
void ags_automation_edit_connect(AgsConnectable *connectable);
void ags_automation_edit_disconnect(AgsConnectable *connectable);
AtkObject* ags_automation_edit_get_accessible(GtkWidget *widget);
void ags_automation_edit_finalize(GObject *gobject);

gboolean ags_accessible_automation_edit_do_action(AtkAction *action,
						  gint i);
gint ags_accessible_automation_edit_get_n_actions(AtkAction *action);
const gchar* ags_accessible_automation_edit_get_description(AtkAction *action,
							    gint i);
const gchar* ags_accessible_automation_edit_get_name(AtkAction *action,
						     gint i);
const gchar* ags_accessible_automation_edit_get_keybinding(AtkAction *action,
							   gint i);
gboolean ags_accessible_automation_edit_set_description(AtkAction *action,
							gint i);
gchar* ags_accessible_automation_edit_get_localized_name(AtkAction *action,
							 gint i);

gboolean ags_automation_edit_auto_scroll_timeout(GtkWidget *widget);

/**
 * SECTION:ags_automation_edit
 * @short_description: edit automations
 * @title: AgsAutomationEdit
 * @section_id:
 * @include: ags/X/editor/ags_automation_edit.h
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

GtkStyle *automation_edit_style = NULL;

GHashTable *ags_automation_edit_auto_scroll = NULL;

GType
ags_automation_edit_get_type(void)
{
  static GType ags_type_automation_edit = 0;

  if(!ags_type_automation_edit){
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

    ags_type_automation_edit = g_type_register_static(GTK_TYPE_TABLE,
						      "AgsAutomationEdit", &ags_automation_edit_info,
						      0);
    
    g_type_add_interface_static(ags_type_automation_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_automation_edit);
}

static GType
ags_accessible_automation_edit_get_type(void)
{
  static GType ags_type_accessible_automation_edit = 0;

  if(!ags_type_accessible_automation_edit){
    const GTypeInfo ags_accesssible_automation_edit_info = {
      sizeof(GtkAccessibleClass),
      NULL,           /* base_init */
      NULL,           /* base_finalize */
      (GClassInitFunc) ags_accessible_automation_edit_class_init,
      NULL,           /* class_finalize */
      NULL,           /* class_data */
      sizeof(GtkAccessible),
      0,             /* n_preallocs */
      NULL, NULL
    };

    static const GInterfaceInfo atk_action_interface_info = {
      (GInterfaceInitFunc) ags_accessible_automation_edit_action_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_accessible_automation_edit = g_type_register_static(GTK_TYPE_ACCESSIBLE,
								 "AgsAccessibleAutomationEdit", &ags_accesssible_automation_edit_info,
								 0);

    g_type_add_interface_static(ags_type_accessible_automation_edit,
				ATK_TYPE_ACTION,
				&atk_action_interface_info);
  }
  
  return(ags_type_accessible_automation_edit);
}


void
ags_automation_edit_class_init(AgsAutomationEditClass *automation_edit)
{
  GObjectClass *gobject;

  GtkWidgetClass *widget;
  
  ags_automation_edit_parent_class = g_type_class_peek_parent(automation_edit);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(automation_edit);

  gobject->set_property = ags_automation_edit_set_property;
  gobject->get_property = ags_automation_edit_get_property;

  gobject->finalize = ags_automation_edit_finalize;

  /* properties */
  /**
   * AgsAutomationArea:channel-type:
   *
   * The target channel.
   * 
   * Since: 1.3.0
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
   * Since: 1.3.0
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
   * Since: 1.3.0
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
   * Since: 1.3.0
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
   * Since: 1.3.0
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
   * Since: 1.3.0
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
   * Since: 1.3.0
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
   * Since: 1.3.0
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
}

void
ags_accessible_automation_edit_class_init(AtkObject *object)
{
  quark_accessible_object = g_quark_from_static_string("ags-accessible-object");
}

void
ags_accessible_automation_edit_action_interface_init(AtkActionIface *action)
{
  action->do_action = ags_accessible_automation_edit_do_action;
  action->get_n_actions = ags_accessible_automation_edit_get_n_actions;
  action->get_description = ags_accessible_automation_edit_get_description;
  action->get_name = ags_accessible_automation_edit_get_name;
  action->get_keybinding = ags_accessible_automation_edit_get_keybinding;
  action->set_description = ags_accessible_automation_edit_set_description;
  action->get_localized_name = ags_accessible_automation_edit_get_localized_name;
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
  GtkAdjustment *adjustment;

  automation_edit->flags = 0;
  automation_edit->mode = AGS_AUTOMATION_EDIT_NO_EDIT_MODE;

  automation_edit->key_mask = 0;
  
  automation_edit->note_offset = 0;
  automation_edit->note_offset_absolute = 0;

  automation_edit->point_radius = AGS_AUTOMATION_EDIT_DEFAULT_POINT_RADIUS;

  automation_edit->scan_width = AGS_AUTOMATION_EDIT_DEFAULT_SCAN_WIDTH;
  automation_edit->scan_height = AGS_AUTOMATION_EDIT_DEFAULT_SCAN_HEIGHT;

  automation_edit->control_width = AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH;
  automation_edit->control_height = AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_HEIGHT;

  automation_edit->step_count = AGS_AUTOMATION_EDIT_DEFAULT_STEP_COUNT;
  
  automation_edit->cursor_position_x = AGS_AUTOMATION_EDIT_DEFAULT_CURSOR_POSITION_X;
  automation_edit->cursor_position_y = AGS_AUTOMATION_EDIT_DEFAULT_CURSOR_POSITION_Y;

  automation_edit->selection_x0 = 0;
  automation_edit->selection_x1 = 0;
  automation_edit->selection_y0 = 0;
  automation_edit->selection_y1 = 0;

  automation_edit->current_acceleration = NULL;

  if(automation_edit_style == NULL){
    automation_edit_style = gtk_style_copy(gtk_widget_get_style(automation_edit));
  }

  automation_edit->ruler = ags_ruler_new();
  gtk_table_attach(GTK_TABLE(automation_edit),
		   (GtkWidget *) automation_edit->ruler,
		   0, 1,
		   0, 1,
		   GTK_FILL | GTK_EXPAND, GTK_FILL,
		   0, 0);

  automation_edit->channel_type = G_TYPE_NONE;
  
  automation_edit->filename = NULL;
  automation_edit->effect = NULL;

  automation_edit->control_specifier = NULL;
  automation_edit->control_name = NULL;

  automation_edit->lower = AGS_AUTOMATION_EDIT_DEFAULT_LOWER;
  automation_edit->upper = AGS_AUTOMATION_EDIT_DEFAULT_UPPER;

  automation_edit->default_value = AGS_AUTOMATION_EDIT_DEFAULT_VALUE;

  automation_edit->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_events(GTK_WIDGET (automation_edit->drawing_area), GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			| GDK_CONTROL_MASK
			| GDK_KEY_PRESS_MASK
			| GDK_KEY_RELEASE_MASK);
  gtk_widget_set_can_focus((GtkWidget *) automation_edit->drawing_area,
			   TRUE);
    
  gtk_table_attach(GTK_TABLE(automation_edit),
		   (GtkWidget *) automation_edit->drawing_area,
		   0, 1,
		   1, 2,
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);

  /* vscrollbar */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, automation_edit->control_height, 1.0);
  automation_edit->vscrollbar = gtk_vscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(automation_edit),
		   (GtkWidget *) automation_edit->vscrollbar,
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* hscrollbar */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (gdouble) automation_edit->control_width, 1.0);
  automation_edit->hscrollbar = gtk_hscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(automation_edit),
		   (GtkWidget *) automation_edit->hscrollbar,
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

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

      gtk_widget_queue_draw(automation_edit);
    }
    break;
  case PROP_UPPER:
    {
      automation_edit->upper = g_value_get_double(value);

      gtk_widget_queue_draw(automation_edit);
    }
    break;
  case PROP_DEFAULT_VALUE:
    {
      automation_edit->default_value = g_value_get_double(value);

      gtk_widget_queue_draw(automation_edit);
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
ags_automation_edit_connect(AgsConnectable *connectable)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationEdit *automation_edit;

  automation_edit = AGS_AUTOMATION_EDIT(connectable);

  if((AGS_AUTOMATION_EDIT_CONNECTED & (automation_edit->flags)) != 0){
    return;
  }
  
  automation_edit->flags |= AGS_AUTOMATION_EDIT_CONNECTED;
  
  /* drawing area */
  g_signal_connect_after((GObject *) automation_edit->drawing_area, "expose_event",
			 G_CALLBACK(ags_automation_edit_drawing_area_expose_event), (gpointer) automation_edit);

  g_signal_connect_after((GObject *) automation_edit->drawing_area, "configure_event",
			 G_CALLBACK(ags_automation_edit_drawing_area_configure_event), (gpointer) automation_edit);

  g_signal_connect((GObject *) automation_edit->drawing_area, "button_press_event",
		   G_CALLBACK(ags_automation_edit_drawing_area_button_press_event), (gpointer) automation_edit);

  g_signal_connect((GObject *) automation_edit->drawing_area, "button_release_event",
		   G_CALLBACK(ags_automation_edit_drawing_area_button_release_event), (gpointer) automation_edit);
  
  g_signal_connect((GObject *) automation_edit->drawing_area, "motion_notify_event",
		   G_CALLBACK(ags_automation_edit_drawing_area_motion_notify_event), (gpointer) automation_edit);

  g_signal_connect((GObject *) automation_edit->drawing_area, "key_press_event",
		   G_CALLBACK(ags_automation_edit_drawing_area_key_press_event), (gpointer) automation_edit);

  g_signal_connect((GObject *) automation_edit->drawing_area, "key_release_event",
		   G_CALLBACK(ags_automation_edit_drawing_area_key_release_event), (gpointer) automation_edit);

  /* scrollbars */
  g_signal_connect_after((GObject *) automation_edit->vscrollbar, "value-changed",
			 G_CALLBACK(ags_automation_edit_vscrollbar_value_changed), (gpointer) automation_edit);

  g_signal_connect_after((GObject *) automation_edit->hscrollbar, "value-changed",
			 G_CALLBACK(ags_automation_edit_hscrollbar_value_changed), (gpointer) automation_edit);
}

void
ags_automation_edit_disconnect(AgsConnectable *connectable)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationEdit *automation_edit;

  automation_edit = AGS_AUTOMATION_EDIT(connectable);

  if((AGS_AUTOMATION_EDIT_CONNECTED & (automation_edit->flags)) == 0){
    return;
  }
  
  automation_edit->flags &= (~AGS_AUTOMATION_EDIT_CONNECTED);
  
  /* drawing area */
  g_object_disconnect((GObject *) automation_edit->drawing_area,
		      "any_signal::expose_event",
		      G_CALLBACK(ags_automation_edit_drawing_area_expose_event),
		      automation_edit,
		      "any_signal::configure_event",
		      G_CALLBACK(ags_automation_edit_drawing_area_configure_event),
		      automation_edit,
		      "any_signal::button_press_event",
		      G_CALLBACK(ags_automation_edit_drawing_area_button_press_event),
		      automation_edit,
		      "any_signal::button_release_event",
		      G_CALLBACK(ags_automation_edit_drawing_area_button_release_event),
		      automation_edit,
		      "any_signal::motion_notify_event",
		      G_CALLBACK(ags_automation_edit_drawing_area_motion_notify_event),
		      automation_edit,
		      "any_signal::key_press_event",
		      G_CALLBACK(ags_automation_edit_drawing_area_key_press_event),
		      automation_edit,
		      "any_signal::key_release_event",
		      G_CALLBACK(ags_automation_edit_drawing_area_key_release_event),
		      automation_edit,
		      NULL);

  /* scrollbars */
  g_object_disconnect((GObject *) automation_edit->vscrollbar,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_automation_edit_vscrollbar_value_changed),
		      (gpointer) automation_edit,
		      NULL);

  g_object_disconnect((GObject *) automation_edit->hscrollbar,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_automation_edit_hscrollbar_value_changed),
		      (gpointer) automation_edit,
		      NULL);
}

AtkObject*
ags_automation_edit_get_accessible(GtkWidget *widget)
{
  AtkObject* accessible;

  accessible = g_object_get_qdata(G_OBJECT(widget),
				  quark_accessible_object);
  
  if(!accessible){
    accessible = g_object_new(ags_accessible_automation_edit_get_type(),
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

gboolean
ags_accessible_automation_edit_do_action(AtkAction *action,
					 gint i)
{
  AgsAutomationEdit *automation_edit;
  
  GdkEventKey *key_press, *key_release;
  GdkEventKey *modifier_press, *modifier_release;
  GdkEventKey *second_level_press, *second_level_release;
  
  if(!(i >= 0 && i < 13)){
    return(FALSE);
  }

  automation_edit = gtk_accessible_get_widget(GTK_ACCESSIBLE(action));
  
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
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event(automation_edit->drawing_area, key_release);
    }
    break;
  case 1:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Right;
      
      /* send event */
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 2:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Up;
    
      /* send event */
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 3:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Down;
      
      /* send event */
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;    
  case 4:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Left;
      
      /* send event */
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) second_level_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) second_level_release);
    }
    break;
  case 5:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Right;
      
      /* send event */
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) second_level_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) second_level_release);
    }
    break;
  case 6:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Up;
      
      /* send event */
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) second_level_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) second_level_release);
    }
    break;
  case 7:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Down;
      
      /* send event */
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) second_level_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) second_level_release);
    }
    break;
  case 8:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_space;
      
      /* send event */
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 9:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Delete;
      
      /* send event */
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_release);
    }
    break;
  case 10:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_c;

      /* send event */
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }    
    break;
  case 11:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_x;

      /* send event */
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }
    break;
  case 12:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_v;

      /* send event */
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) automation_edit->drawing_area,
		       (GdkEvent *) modifier_release);      
    }
    break;
  }

  return(TRUE);
}

gint
ags_accessible_automation_edit_get_n_actions(AtkAction *action)
{
  return(13);
}

const gchar*
ags_accessible_automation_edit_get_description(AtkAction *action,
					       gint i)
{
  static const gchar **actions = {
    "move cursor left",
    "move cursor right",
    "move cursor up",
    "move cursor down",
    "move cursor relative up",
    "move cursor relative down",
    "move cursor small left",
    "move cursor small right",
    "add acceleration",
    "remove acceleration",
    "copy automation to clipboard",
    "cut automation to clipbaord",
    "paste automation from clipboard",
  };

  if(i >= 0 && i < 13){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_automation_edit_get_name(AtkAction *action,
					gint i)
{
  static const gchar **actions = {
    "left",
    "right",
    "up",
    "down",
    "small-left",
    "small-right",
    "relative-up",
    "relative-down",
    "add",
    "remove",
    "copy",
    "cut",
    "paste",
  };
  
  if(i >= 0 && i < 13){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_automation_edit_get_keybinding(AtkAction *action,
					      gint i)
{
  static const gchar **actions = {
    "left",
    "right",
    "up",
    "down",
    "Shft+Left",
    "Shft+Right",
    "Shft+up",
    "Schft+down",
    "space",
    "Del"
    "Ctrl+c"
    "Ctrl+x",
    "Ctrl+v",
  };
  
  if(i >= 0 && i < 13){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

gboolean
ags_accessible_automation_edit_set_description(AtkAction *action,
					       gint i)
{
  //TODO:JK: implement me

  return(FALSE);
}

gchar*
ags_accessible_automation_edit_get_localized_name(AtkAction *action,
						  gint i)
{
  //TODO:JK: implement me

  return(NULL);
}

gboolean
ags_automation_edit_auto_scroll_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_automation_edit_auto_scroll,
			 widget) != NULL){
    AgsAutomationEditor *automation_editor;
    AgsAutomationEdit *automation_edit;
    AgsAutomationToolbar *automation_toolbar;

    double zoom;
    double x;
    
    automation_edit = AGS_AUTOMATION_EDIT(widget);

    if((AGS_AUTOMATION_EDIT_AUTO_SCROLL & (automation_edit->flags)) == 0){
      return(TRUE);
    }
    
    automation_editor = gtk_widget_get_ancestor(automation_edit,
						AGS_TYPE_AUTOMATION_EDITOR);
    
    if(automation_editor->selected_machine == NULL){
      return(TRUE);
    }

    automation_toolbar = automation_editor->automation_toolbar;

    /* zoom */
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom) - 2.0);

    /* reset offset */
    automation_edit->note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(automation_editor->selected_machine->audio->soundcard));
    automation_edit->note_offset_absolute = ags_soundcard_get_note_offset_absolute(AGS_SOUNDCARD(automation_editor->selected_machine->audio->soundcard));

    /* reset scrollbar */
    x = ((automation_edit->note_offset * automation_edit->control_width) / (AGS_AUTOMATION_EDITOR_MAX_CONTROLS * automation_edit->control_width)) * GTK_RANGE(automation_edit->hscrollbar)->adjustment->upper;
    
    gtk_range_set_value(GTK_RANGE(automation_edit->hscrollbar),
			x);

    return(TRUE);
  }else{
    return(FALSE);
  }
}

void
ags_automation_edit_reset_vscrollbar(AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;

  GtkAdjustment *adjustment;
  GtkAdjustment *piano_adjustment;
  
  double varea_height;
  gdouble upper, old_upper;
  
  if(!AGS_IS_AUTOMATION_EDIT(automation_edit)){
    return;
  }

  automation_editor = gtk_widget_get_ancestor(automation_edit,
					    AGS_TYPE_AUTOMATION_EDITOR);

  if(automation_editor->selected_machine == NULL){
    return;
  }

  /* adjustment */
  adjustment = GTK_RANGE(automation_edit->vscrollbar)->adjustment;

  g_object_get(automation_editor->scrolled_piano->viewport,
	       "vadjustment", &piano_adjustment,
	       NULL);

  /* upper */
  old_upper = adjustment->upper; 

  varea_height = automation_edit->step_count * automation_edit->control_height;
  upper = varea_height - GTK_WIDGET(automation_edit->drawing_area)->allocation.height;

  if(upper < 0.0){
    upper = 0.0;
  }
	   
  gtk_adjustment_set_upper(adjustment,
			   upper);

  /* piano - only upper */
  gtk_adjustment_set_upper(piano_adjustment,
			   upper);

  /* reset value */
  if(old_upper != 0.0){
    gtk_adjustment_set_value(adjustment,
			     adjustment->value / old_upper * upper);
  }
}

void
ags_automation_edit_reset_hscrollbar(AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;

  GtkAdjustment *adjustment;

  double zoom_factor, zoom;
  double zoom_correction;
  guint map_width;
  gdouble upper, old_upper;
  
  if(!AGS_IS_AUTOMATION_EDIT(automation_edit)){
    return;
  }

  automation_editor = gtk_widget_get_ancestor(automation_edit,
					    AGS_TYPE_AUTOMATION_EDITOR);

  if(automation_editor->selected_machine == NULL){
    return;
  }

  automation_toolbar = automation_editor->automation_toolbar;

  /* adjustment */
  adjustment = GTK_RANGE(automation_edit->hscrollbar)->adjustment;

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom) - 2.0);

  /* upper */
  old_upper = adjustment->upper;
  
  zoom_correction = 1.0 / 16;

  map_width = ((double) AGS_AUTOMATION_EDITOR_MAX_CONTROLS * automation_edit->control_width * zoom * zoom_correction);
  upper = map_width - GTK_WIDGET(automation_edit->drawing_area)->allocation.width;

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
			   upper / automation_edit->control_width);

  /* reset value */
  if(old_upper != 0.0){
    gtk_adjustment_set_value(adjustment,
			     adjustment->value / old_upper * upper);
  }
}

void
ags_automation_edit_draw_segment(AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;
  
  GtkStyle *automation_edit_style;

  cairo_t *cr;

  gdouble translated_ground;
  double tact;
  gdouble y;
  gdouble height;
  guint control_width;
  guint i, j;
  guint j_set;

  static const gdouble white_gc = 65535.0;

  if(!AGS_IS_AUTOMATION_EDIT(automation_edit)){
    return;
  }

  automation_editor = gtk_widget_get_ancestor(automation_edit,
					      AGS_TYPE_AUTOMATION_EDITOR);

  if(automation_editor->selected_machine == NULL){
    return;
  }

  automation_toolbar = automation_editor->automation_toolbar;
  
  automation_edit_style = gtk_widget_get_style(GTK_WIDGET(automation_edit->drawing_area));

  /* create cairo context */
  cr = gdk_cairo_create(GTK_WIDGET(automation_edit->drawing_area)->window);

  if(cr == NULL){
    return;
  }

  width = (gdouble) GTK_WIDGET(automation_edit->drawing_area)->allocation.width;
  height = (gdouble) GTK_WIDGET(automation_edit->drawing_area)->allocation.height;

  /* background */
  cairo_set_source_rgb(cr,
		       automation_edit_style->bg[0].red / white_gc,
		       automation_edit_style->bg[0].red / white_gc,
		       automation_edit_style->bg[0].red / white_gc);
  cairo_rectangle(cr, 0.0, y, width, height);
  cairo_fill(cr);

  /* background border */
  cairo_set_source_rgb(cr,
		       automation_edit_style->base[0].red / white_gc,
		       automation_edit_style->base[0].green / white_gc,
		       automation_edit_style->base[0].blue / white_gc);
  cairo_set_line_width(cr, 1.0);
  cairo_rectangle(cr, 0.0, y, width, height);
  cairo_stroke(cr);

  cairo_set_line_width(cr, 1.0);

  tact = exp2((double) gtk_combo_box_get_active(automation_editor->automation_toolbar->zoom) - 2.0);

  y = (gdouble) automation_area->y - y_offset;
  
  height = (gdouble) automation_area->height;

  control_width = 64;
  i = control_width - (guint) x_offset % control_width;
  
  if(i < widget->allocation.width &&
     tact > 1.0 ){
    j_set = ((guint) x_offset / control_width + 1) % ((guint) tact);

    cairo_set_source_rgb(cr,
			 automation_area_style->mid[0].red / white_gc,
			 automation_area_style->mid[0].green / white_gc,
			 automation_area_style->mid[0].blue / white_gc);

    if(j_set != 0){
      j = j_set;
      goto ags_automation_area_draw_segment0;
    }
  }

  for(; i < widget->allocation.width; ){
    cairo_set_source_rgb(cr,
			 automation_area_style->fg[0].red / white_gc,
			 automation_area_style->fg[0].blue / white_gc,
			 automation_area_style->fg[0].green / white_gc);
    
    cairo_move_to(cr, (double) i, y);
    cairo_line_to(cr, (double) i, y + height);
    cairo_stroke(cr);
    
    i += control_width;
    
    cairo_set_source_rgb(cr,
			 automation_area_style->mid[0].red / white_gc,
			 automation_area_style->mid[0].green / white_gc,
			 automation_area_style->mid[0].blue / white_gc);
    
    for(j = 1; i < widget->allocation.width && j < tact; j++){
    ags_automation_area_draw_segment0:
      cairo_move_to(cr, (double) i, y);
      cairo_line_to(cr, (double) i, y + height);
      cairo_stroke(cr);
      
      i += control_width;
    }
  }

  cairo_set_source_rgb(cr,
		       automation_area_style->bg[0].red / white_gc,
		       automation_area_style->bg[0].green / white_gc,
		       automation_area_style->bg[0].blue / white_gc);

  /* middle */
  cairo_move_to(cr,
		0.0, y + height * 0.5);
  cairo_line_to(cr,
		width, y + height * 0.5);
  cairo_stroke(cr);

  /* set dash */
  cairo_set_source_rgb(cr,
		       automation_area_style->bg[0].red / white_gc,
		       automation_area_style->bg[0].green / white_gc,
		       automation_area_style->bg[0].blue / white_gc);
  cairo_set_dash(cr,
		 &dashes,
		 1,
		 0.0);

  /* lower quarter */
  cairo_move_to(cr,
		0.0, y + height * 0.25);
  cairo_line_to(cr,
		width, y + height * 0.25);
  cairo_stroke(cr);

  /* upper quarter */
  cairo_move_to(cr,
		0.0, y + height * 0.75);
  cairo_line_to(cr,
		width, y + height * 0.75);
  cairo_stroke(cr);
  
  /* complete */
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
      
  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);
}

void
ags_automation_edit_draw_position(AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;

  GtkStyle *automation_edit_style;

  cairo_t *cr;

  double position;
  double x, y;
  double width, height;
  gboolean height_fits;

  static const gdouble white_gc = 65535.0;

  if(!AGS_IS_AUTOMATION_EDIT(automation_edit)){
    return;
  }

  automation_editor = gtk_widget_get_ancestor(automation_edit,
					    AGS_TYPE_AUTOMATION_EDITOR);

  if(automation_editor->selected_machine == NULL){
    return;
  }
  
  automation_edit_style = gtk_widget_get_style(GTK_WIDGET(automation_edit->drawing_area));

  /* create cairo context */
  cr = gdk_cairo_create(GTK_WIDGET(automation_edit->drawing_area)->window);

  if(cr == NULL){
    return;
  }

  /* get offset and dimensions */
  position = ((double) automation_edit->note_offset) * ((double) automation_edit->control_width);
  
  y = 0.0;
  x = (position) - (GTK_RANGE(automation_edit->hscrollbar)->adjustment->value);

  width = (double) AGS_AUTOMATION_EDIT_DEFAULT_FADER_WIDTH;
  height = automation_edit->step_count * automation_edit->control_height;

  /* push group */
  cairo_push_group(cr);
  
  /* draw fader */
  cairo_set_source_rgba(cr,
			automation_edit_style->dark[0].red / white_gc,
			automation_edit_style->dark[0].green / white_gc,
			automation_edit_style->dark[0].blue / white_gc,
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
ags_automation_edit_draw_cursor(AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;

  GtkStyle *automation_edit_style;

  double zoom, zoom_factor;
  gdouble c_range;
  double x, y;
  double width, height;

  cairo_t *cr;

  static const gdouble white_gc = 65535.0;

  if(!AGS_IS_AUTOMATION_EDIT(automation_edit)){
    return;
  }
  
  automation_editor = gtk_widget_get_ancestor(automation_edit,
					    AGS_TYPE_AUTOMATION_EDITOR);

  if(automation_editor->selected_machine == NULL){
    return;
  }

  automation_toolbar = automation_editor->automation_toolbar;

  automation_edit_style = gtk_widget_get_style(GTK_WIDGET(automation_edit->drawing_area));

  /* create cairo context */
  cr = gdk_cairo_create(GTK_WIDGET(automation_edit->drawing_area)->window);

  if(cr == NULL){
    return;
  }

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = exp(automation_edit->upper) - exp(automation_edit->lower);
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  /* zoom */
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom) - 2.0);
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

  /* get offset */
  x = ((double) automation_edit->cursor_position_x) - GTK_RANGE(automation_edit->hscrollbar)->adjustment->value;
  
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    y = (((double) exp(automation_edit->cursor_position_y) / c_range) * GTK_WIDGET(automation_edit->drawing_area)->allocation.height) - GTK_RANGE(automation_edit->vscrollbar)->adjustment->value;
  }else{
    y = (((double) automation_edit->cursor_position_y / c_range) * GTK_WIDGET(automation_edit->drawing_area)->allocation.height) - GTK_RANGE(automation_edit->vscrollbar)->adjustment->value;
  }
  
  width = (double) AGS_AUTOMATION_EDIT_CURSOR_WIDTH;
  height = (double) AGS_AUTOMATION_EDIT_CURSOR_HEIGHT;

  /* apply zoom */
  x /= zoom_factor;

  /* clip */
  if(x < 0.0){
    width += x;

    x = 0.0;
  }else if(x > GTK_WIDGET(automation_edit->drawing_area)->allocation.width){
    return;
  }

  if(x + width > GTK_WIDGET(automation_edit->drawing_area)->allocation.width){
    width = ((double) GTK_WIDGET(automation_edit->drawing_area)->allocation.width) - x;
  }
  
  if(y < 0.0){
    height += y;

    y = 0.0;
  }else if(y > GTK_WIDGET(automation_edit->drawing_area)->allocation.height){
    return;
  }

  if(y + height > GTK_WIDGET(automation_edit->drawing_area)->allocation.height){
    height = ((double) GTK_WIDGET(automation_edit->drawing_area)->allocation.height) - y;
  }

  /* push group */
  cairo_push_group(cr);

  /* draw cursor */
  cairo_set_source_rgba(cr,
			automation_edit_style->base[0].red / white_gc,
			automation_edit_style->base[0].green / white_gc,
			automation_edit_style->base[0].blue / white_gc,
			0.5);

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
      
  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);
}

void
ags_automation_edit_draw_selection(AgsAutomationEdit *automation_edit)
{
  GtkStyle *automation_edit_style;

  cairo_t *cr;

  double x, y;
  double width, height;

  if(!AGS_IS_AUTOMATION_EDIT(automation_edit)){
    return;
  }
  
  automation_edit_style = gtk_widget_get_style(GTK_WIDGET(automation_edit->drawing_area));

  /* create cairo context */
  cr = gdk_cairo_create(GTK_WIDGET(automation_edit->drawing_area)->window);

  if(cr == NULL){
    return;
  }

  /* get offset and dimensions */
  if(automation_edit->selection_x0 < automation_edit->selection_x1){
    x = ((double) automation_edit->selection_x0) - GTK_RANGE(automation_edit->hscrollbar)->adjustment->value;
    width = ((double) automation_edit->selection_x1 - (double) automation_edit->selection_x0);
  }else{
    x = ((double) automation_edit->selection_x1) - GTK_RANGE(automation_edit->hscrollbar)->adjustment->value;
    width = ((double) automation_edit->selection_x0 - (double) automation_edit->selection_x1);
  }

  if(automation_edit->selection_y0 < automation_edit->selection_y1){
    y = ((double) automation_edit->selection_y0) - GTK_RANGE(automation_edit->hscrollbar)->adjustment->value;
    height = ((double) automation_edit->selection_y1 - (double) automation_edit->selection_y0);
  }else{
    y = ((double) automation_edit->selection_y1) - GTK_RANGE(automation_edit->hscrollbar)->adjustment->value;
    height = ((double) automation_edit->selection_y0 - (double) automation_edit->selection_y1);
  }

  /* clip */
  if(x < 0.0){
    x = 0.0;
  }else if(x > GTK_WIDGET(automation_edit->drawing_area)->allocation.width){
    return;
  }

  if(x + width > GTK_WIDGET(automation_edit->drawing_area)->allocation.width){
    width = ((double) GTK_WIDGET(automation_edit->drawing_area)->allocation.width) - x;
  }
  
  if(y < 0.0){
    y = 0.0;
  }else if(y > GTK_WIDGET(automation_edit->drawing_area)->allocation.height){
    return;
  }

  if(y + height > GTK_WIDGET(automation_edit->drawing_area)->allocation.height){
    height = ((double) GTK_WIDGET(automation_edit->drawing_area)->allocation.height) - y;
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
ags_automation_edit_draw_acceleration(AgsAutomationEdit *automation_edit,
				      AgsAcceleration *acceleration_a, AgsAcceleration *acceleration_b,
				      cairo_t *cr,
				      double r, double g, double b, double a)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;

  GtkStyle *automation_edit_style;

  gdouble c_range;
  gdouble y_upper;
  double zoom, zoom_factor;
  double viewport_x, viewport_y;
  double x, y;
  double width, height;
  
  static const gdouble white_gc = 65535.0;

  if(!AGS_IS_AUTOMATION_EDIT(automation_edit) ||
     !AGS_IS_ACCELERATION(acceleration_a) ||
     cr == NULL){
    return;
  }

  automation_editor = gtk_widget_get_ancestor(automation_edit,
					      AGS_TYPE_AUTOMATION_EDITOR);

  if(automation_editor->selected_machine == NULL){
    return;
  }

  automation_toolbar = automation_editor->automation_toolbar;
  
  automation_edit_style = gtk_widget_get_style(GTK_WIDGET(automation_edit->drawing_area));

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = exp(automation_edit->upper) - exp(automation_edit->lower);
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  /* zoom */
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom) - 2.0);
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) automation_toolbar->zoom));

  /* get offset and dimensions */
  if(AGS_AUTOMATION_EDITOR_MAX_CONTROLS > GTK_WIDGET(automation_edit->drawing_area)->allocation.width){
    viewport_x = zoom_factor * GTK_RANGE(automation_edit->hscrollbar)->adjustment->value;
  }else{
    viewport_x = 0.0;
  }
  
  viewport_y = GTK_RANGE(automation_edit->vscrollbar)->adjustment->value;
  
  x = ((double) acceleration_a->x) - viewport_x;

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    y = ((double) exp(acceleration_a->y) / c_range) * GTK_WIDGET(automation_edit->drawing_area)->allocation.height - viewport_y;
  }else{
    y = ((double) acceleration_a->y / c_range) * GTK_WIDGET(automation_edit->drawing_area)->allocation.height - viewport_y;
  }
  
  if(acceleration_b != NULL){
    width = ((double) acceleration_b->x - acceleration_a->x);
  }else{
    width = ((double) GTK_WIDGET(automation_edit->drawing_area)->allocation.width - acceleration_a->x);
  }
  
  height = GTK_WIDGET(automation_edit->drawing_area)->allocation.height - y;

  /* apply zoom */
  x /= zoom_factor;
  x += ((double) automation_edit->control_margin_x);

  y += ((double) automation_edit->control_margin_y);
  
  width /= zoom_factor;
  
  /* clip */
  if(x < 0.0){
    if(x + width < 0.0){
      return;
    }else{
      width += x;
      x = 0.0;
    }
  }else if(x > GTK_WIDGET(automation_edit->drawing_area)->allocation.width){
    return;
  }

  if(x + width > GTK_WIDGET(automation_edit->drawing_area)->allocation.width){
    width = ((double) GTK_WIDGET(automation_edit->drawing_area)->allocation.width) - x;
  }
  
  if(y < 0.0){
    if(y + height < 0.0){
      return;
    }else{
      height += y;
      y = 0.0;
    }
  }else if(y > GTK_WIDGET(automation_edit->drawing_area)->allocation.height){
    return;
  }

  if(y + height > GTK_WIDGET(automation_edit->drawing_area)->allocation.height){
    height = ((double) GTK_WIDGET(automation_edit->drawing_area)->allocation.height) - y;
  }

  /* draw point */
  if((AGS_ACCELERATION_IS_SELECTED & (acceleration->flags)) != 0){
    /* draw selected acceleration */
    cairo_set_source_rgba(cr,
			  r, g, b, 1.0);
    
    cairo_arc(cr,
	      x, y,
	      automotion_edit->point_radius,
	      0.0,
	      2.0 * M_PI);

    cairo_stroke(cr);
  }else{
    /* draw acceleration */
    cairo_set_source_rgba(cr,
			  r, g, b, a);
    
    cairo_arc(cr,
	      x, y,
	      automotion_edit->point_radius,
	      0.0,
	      2.0 * M_PI);
    
    cairo_stroke(cr);
  }
  
  /* draw acceleration */
  cairo_set_source_rgba(cr,
			r, g, b, a);
  cairo_rectangle(cr,
		  x, y,
		  width, height);
  cairo_fill(cr);
}

void
ags_automation_edit_draw_automation(AgsAutomationEdit *automation_edit)
{
  AgsAutomationEditor *automation_editor;

  GtkStyle *automation_edit_style;
  
  AgsMutexManager *mutex_manager;
  
  cairo_t *cr;

  GList *list_automation;
  GList *list_acceleration;

  gdouble c_range;
  gdouble y_upper, y_value;
  guint x0, x1;
  guint y0, y1;
  guint offset;
  gint i;    
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  static const gdouble white_gc = 65535.0;
  
  if(!AGS_IS_AUTOMATION_EDIT(automation_edit)){
    return;
  }

  automation_editor = gtk_widget_get_ancestor(automation_edit,
					      AGS_TYPE_AUTOMATION_EDITOR);

  if(automation_editor->selected_machine == NULL){
    return;
  }
  
  automation_edit_style = gtk_widget_get_style(GTK_WIDGET(automation_edit->drawing_area));

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) automation_editor->selected_machine->audio);
  
  pthread_mutex_unlock(application_mutex);

  /* create cairo context */
  cr = gdk_cairo_create(GTK_WIDGET(automation_edit->drawing_area)->window);

  if(cr == NULL){
    return;
  }

  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    c_range = exp(automation_edit->upper) - exp(automation_edit->lower);
  }else{
    c_range = automation_edit->upper - automation_edit->lower;
  }

  /* get visisble region */
  x0 = GTK_RANGE(automation_edit->hscrollbar)->adjustment->value;
  x1 = (GTK_RANGE(automation_edit->hscrollbar)->adjustment->value + GTK_WIDGET(automation_edit->drawing_area)->allocation.width);

  y_value = GTK_RANGE(automation_edit->vscrollbar)->adjustment->value;
  y_upper = GTK_RANGE(automation_edit->vscrollbar)->adjustment->upper;
  
  if((AGS_AUTOMATION_EDIT_LOGARITHMIC & (automation_edit->flags)) != 0){
    y0 = log((y_value / y_upper) * c_range);
    y1 = log(((y_value + GTK_WIDGET(automation_edit->drawing_area)->allocation.height) / y_upper) * c_range);
  }else{
    y0 = (y_value / y_upper) * c_range;
    y1 = ((y_value + GTK_WIDGET(automation_edit->drawing_area)->allocation.height) / y_upper) * c_range;
  }
  
  /* push group */
  cairo_push_group(cr);

  /* draw automation */
  pthread_mutex_lock(audio_mutex);

  i = 0;
  
  while((i = ags_notebook_next_active_tab(automation_editor->notebook,
					  i)) != -1){
    list_automation = automation_editor->selected_machine->audio->automation;

    while((list_automation = ags_automation_find_near_timestamp(list_automation, i,
								NULL)) != NULL){
      AgsAutomation *automation;

      GList *list_acceleration;

      automation = AGS_AUTOMATION(list_automation->data);
      
      if(automation->timestamp != NULL &&
	 AGS_TIMESTAMP(automation->timestamp)->timer.ags_offset.offset > x1){
	break;
      }

      if(automation->timestamp != NULL &&
	 AGS_TIMESTAMP(automation->timestamp)->timer.ags_offset.offset + AGS_AUTOMATION_DEFAULT_OFFSET < x0){
	list_automation = list_automation->next;

	continue;
      }

      list_acceleration = automation->accelerations;

      while(list_acceleration != NULL){
	ags_automation_edit_draw_acceleration(automation_edit,
					      list_acceleration->data, ((list_acceleration->next != NULL) ? list_acceleration->next->data: NULL),
					      cr,
					      automation_edit_style->fg[0].red / white_gc,
					      automation_edit_style->fg[0].green / white_gc,
					      automation_edit_style->fg[0].blue / white_gc,
					      0.8);

	list_acceleration = list_acceleration->next;
      }

      list_automation = list_automation->next;
    }
    
    i++;
  }

  pthread_mutex_unlock(audio_mutex);

  /* complete */
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
      
  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);
}

void
ags_automation_edit_draw(AgsAutomationEdit *automation_edit)
{
  /* segment */
  ags_automation_edit_draw_segment(automation_edit);

  /* automation */
  ags_automation_edit_draw_automation(automation_edit);
  
  /* edit mode */
  switch(automation_edit->mode){
  case AGS_AUTOMATION_EDIT_POSITION_CURSOR:
    {
      ags_automation_edit_draw_cursor(automation_edit);
    }
    break;
  case AGS_AUTOMATION_EDIT_ADD_ACCELERATION:
    {
      if(automation_edit->current_acceleration != NULL){
	GtkStyle *automation_edit_style;
	
	cairo_t *cr;

	static const gdouble white_gc = 65535.0;
	
	automation_edit_style = gtk_widget_get_style(GTK_WIDGET(automation_edit->drawing_area));
	
	cr = gdk_cairo_create(GTK_WIDGET(automation_edit->drawing_area)->window);

	if(cr != NULL){
	  ags_automation_edit_draw_acceleration(automation_edit,
						automation_edit->current_acceleration, NULL,
						cr,
						automation_edit_style->fg[0].red / white_gc,
						automation_edit_style->fg[0].green / white_gc,
						automation_edit_style->fg[0].blue / white_gc,
						1.0);
	}
      }
    }
    break;
  case AGS_AUTOMATION_EDIT_SELECT_ACCELERATION:
    {
      ags_automation_edit_draw_selection(automation_edit);
    }
    break;
  }

  /* fader */
  if((AGS_AUTOMATION_EDIT_AUTO_SCROLL & (automation_edit->flags)) != 0){
    ags_automation_edit_draw_position(automation_edit);
  }
}

/**
 * ags_automation_edit_new:
 *
 * Create a new #AgsAutomationEdit.
 *
 * Returns: a new #AgsAutomationEdit
 *
 * Since: 1.0.0
 */
AgsAutomationEdit*
ags_automation_edit_new()
{
  AgsAutomationEdit *automation_edit;

  automation_edit = (AgsAutomationEdit *) g_object_new(AGS_TYPE_AUTOMATION_EDIT, NULL);

  return(automation_edit);
}
