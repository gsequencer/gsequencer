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

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_wave_editor.h>

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

void ags_wave_edit_size_request(GtkWidget *widget,
				GtkRequisition   *requisition);
void ags_wave_edit_size_allocate(GtkWidget *widget,
				 GtkAllocation *allocation);
void ags_wave_edit_show(GtkWidget *widget);
void ags_wave_edit_show_all(GtkWidget *widget);

gboolean ags_wave_edit_auto_scroll_timeout(GtkWidget *widget);

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

static GQuark quark_accessible_object = 0;

GtkStyle *wave_edit_style = NULL;

GHashTable *ags_wave_edit_auto_scroll = NULL;

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
  GtkWidgetClass *widget;

  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_wave_edit_parent_class = g_type_class_peek_parent(wave_edit);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) wave_edit;

  widget->size_request = ags_wave_edit_size_request;
  widget->size_allocate = ags_wave_edit_size_allocate;
  widget->show = ags_wave_edit_show;
  widget->show_all = ags_wave_edit_show_all;
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
  quark_accessible_object = g_quark_from_static_string("ags-accessible-object");
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

  g_object_set(wave_edit,
	       "can-focus", FALSE,
	       "n-columns", 3,
	       "n-rows", 4,
	       "homogeneous", FALSE,
	       NULL);

  wave_edit->flags = 0;
  wave_edit->mode = AGS_WAVE_EDIT_NO_EDIT_MODE;

  wave_edit->key_mask = 0;
  
  wave_edit->note_offset = 0;
  wave_edit->note_offset_absolute = 0;

  wave_edit->control_width = AGS_WAVE_EDIT_DEFAULT_CONTROL_WIDTH;
  wave_edit->control_height = AGS_WAVE_EDIT_DEFAULT_CONTROL_HEIGHT;
  
  wave_edit->cursor_position_x = AGS_WAVE_EDIT_DEFAULT_CURSOR_POSITION_X;
  wave_edit->cursor_position_y = AGS_WAVE_EDIT_DEFAULT_CURSOR_POSITION_Y;

  wave_edit->selection_x0 = 0;
  wave_edit->selection_x1 = 0;
  wave_edit->selection_y0 = 0;
  wave_edit->selection_y1 = 0;

  if(wave_edit_style == NULL){
    wave_edit_style = gtk_style_copy(gtk_widget_get_style(wave_edit));
  }

  wave_edit->ruler = ags_ruler_new();
  g_object_set(wave_edit->ruler,
	       "no-show-all", TRUE,
	       NULL);
  gtk_table_attach(GTK_TABLE(wave_edit),
		   (GtkWidget *) wave_edit->ruler,
		   0, 1,
		   0, 1,
		   GTK_FILL|GTK_EXPAND,
		   GTK_FILL,
		   0, 0);

  wave_edit->lower = AGS_WAVE_EDIT_DEFAULT_LOWER;
  wave_edit->upper = AGS_WAVE_EDIT_DEFAULT_UPPER;

  wave_edit->default_value = AGS_WAVE_EDIT_DEFAULT_VALUE;

  wave_edit->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
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
    
  gtk_widget_set_size_request(wave_edit->drawing_area,
			      -1, AGS_LEVEL_DEFAULT_HEIGHT);
  gtk_table_attach(GTK_TABLE(wave_edit),
		   (GtkWidget *) wave_edit->drawing_area,
		   0, 1,
		   1, 2,
		   GTK_FILL|GTK_EXPAND,
		   GTK_FILL|GTK_EXPAND,
		   0, 0);
    
  /* vscrollbar */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, wave_edit->control_height, 1.0);
  wave_edit->vscrollbar = gtk_vscrollbar_new(adjustment);
  g_object_set(wave_edit->vscrollbar,
	       "no-show-all", TRUE,
	       NULL);
  gtk_widget_set_size_request(wave_edit->vscrollbar,
			      -1, AGS_LEVEL_DEFAULT_HEIGHT);
  gtk_table_attach(GTK_TABLE(wave_edit),
		   (GtkWidget *) wave_edit->vscrollbar,
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* hscrollbar */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (gdouble) wave_edit->control_width, 1.0);
  wave_edit->hscrollbar = gtk_hscrollbar_new(adjustment);
  g_object_set(wave_edit->hscrollbar,
	       "no-show-all", TRUE,
	       NULL);
  gtk_widget_set_size_request(wave_edit->hscrollbar,
			      -1, -1);
  gtk_table_attach(GTK_TABLE(wave_edit),
		   (GtkWidget *) wave_edit->hscrollbar,
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

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
ags_wave_edit_connect(AgsConnectable *connectable)
{
  AgsWaveEdit *wave_edit;

  wave_edit = AGS_WAVE_EDIT(connectable);

  if((AGS_WAVE_EDIT_CONNECTED & (wave_edit->flags)) != 0){
    return;
  }
  
  wave_edit->flags |= AGS_WAVE_EDIT_CONNECTED;

  //TODO:JK: implement me
}

void
ags_wave_edit_disconnect(AgsConnectable *connectable)
{
  AgsWaveEdit *wave_edit;

  wave_edit = AGS_WAVE_EDIT(connectable);

  if((AGS_WAVE_EDIT_CONNECTED & (wave_edit->flags)) == 0){
    return;
  }
  
  wave_edit->flags &= (~AGS_WAVE_EDIT_CONNECTED);

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
  static const gchar *actions[] = {
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
  static const gchar *actions[] = {
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
  static const gchar *actions[] = {
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

void
ags_wave_edit_size_request(GtkWidget *widget,
				 GtkRequisition *requisition)
{
  AgsWaveEdit *wave_edit;

  wave_edit = AGS_WAVE_EDIT(widget);

  requisition->width = -1;
  requisition->height = AGS_LEVEL_DEFAULT_HEIGHT;  
}

void
ags_wave_edit_size_allocate(GtkWidget *widget,
				  GtkAllocation *allocation)
{
  AgsWaveEdit *wave_edit;

  GtkAllocation child_allocation;

  GdkWindow *window;

  wave_edit = AGS_WAVE_EDIT(widget);

  widget->allocation = *allocation;
  
  widget->allocation.height = AGS_LEVEL_DEFAULT_HEIGHT;
  allocation->height = AGS_LEVEL_DEFAULT_HEIGHT;
  
  child_allocation.x = allocation->x;
  child_allocation.y = allocation->y;
  
  child_allocation.width = allocation->width;
  child_allocation.height = AGS_LEVEL_DEFAULT_HEIGHT;

  gtk_widget_size_allocate(wave_edit->drawing_area,
  			   &child_allocation);

  window = gtk_widget_get_window(wave_edit->drawing_area);
  gdk_window_move(window,
  		  allocation->x, allocation->y);
}

void
ags_wave_edit_show(GtkWidget *widget)
{
  AgsWaveEdit *wave_edit;

  wave_edit = AGS_WAVE_EDIT(widget);

  /* call parent */
  GTK_WIDGET_CLASS(ags_wave_edit_parent_class)->show(widget);

  gtk_widget_show(wave_edit->drawing_area);
  
  if((AGS_WAVE_EDIT_SHOW_RULER & (wave_edit->flags)) != 0){
    gtk_widget_show(wave_edit->ruler);
  }

  if((AGS_WAVE_EDIT_SHOW_VSCROLLBAR & (wave_edit->flags)) != 0){
    gtk_widget_show(wave_edit->vscrollbar);
  }

  if((AGS_WAVE_EDIT_SHOW_HSCROLLBAR & (wave_edit->flags)) != 0){
    gtk_widget_show(wave_edit->hscrollbar);
  }
}

void
ags_wave_edit_show_all(GtkWidget *widget)
{
  AgsWaveEdit *wave_edit;

  wave_edit = AGS_WAVE_EDIT(widget);

  /* call parent */
  GTK_WIDGET_CLASS(ags_wave_edit_parent_class)->show_all(widget);

  gtk_widget_show_all(wave_edit->drawing_area);

  if((AGS_WAVE_EDIT_SHOW_RULER & (wave_edit->flags)) != 0){
    gtk_widget_show(wave_edit->ruler);
  }

  if((AGS_WAVE_EDIT_SHOW_VSCROLLBAR & (wave_edit->flags)) != 0){
    gtk_widget_show(wave_edit->vscrollbar);
  }

  if((AGS_WAVE_EDIT_SHOW_HSCROLLBAR & (wave_edit->flags)) != 0){
    gtk_widget_show(wave_edit->hscrollbar);
  }
}

gboolean
ags_wave_edit_auto_scroll_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_wave_edit_auto_scroll,
			 widget) != NULL){
    AgsWaveEditor *wave_editor;
    AgsWaveEdit *wave_edit;
    AgsWaveToolbar *wave_toolbar;

    GObject *output_soundcard;
    
    double zoom;
    double x;
    
    wave_edit = AGS_WAVE_EDIT(widget);

    if((AGS_WAVE_EDIT_AUTO_SCROLL & (wave_edit->flags)) == 0){
      return(TRUE);
    }
    
    wave_editor = gtk_widget_get_ancestor(wave_edit,
					  AGS_TYPE_WAVE_EDITOR);
    
    if(wave_editor->selected_machine == NULL){
      return(TRUE);
    }

    wave_toolbar = wave_editor->wave_toolbar;

    /* zoom */
    zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) wave_toolbar->zoom) - 2.0);

    /* reset offset */
    g_object_get(wave_editor->selected_machine->audio,
		 "output-soundcard", &output_soundcard,
		 NULL);
    
    wave_edit->note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));
    wave_edit->note_offset_absolute = ags_soundcard_get_note_offset_absolute(AGS_SOUNDCARD(output_soundcard));

    /* reset scrollbar */
    x = ((wave_edit->note_offset * wave_edit->control_width) / (AGS_WAVE_EDITOR_MAX_CONTROLS * wave_edit->control_width)) * GTK_RANGE(wave_edit->hscrollbar)->adjustment->upper;
    
    gtk_range_set_value(GTK_RANGE(wave_edit->hscrollbar),
			x);

    return(TRUE);
  }else{
    return(FALSE);
  }
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
