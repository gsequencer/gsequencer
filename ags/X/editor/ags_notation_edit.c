/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_notation_editor.h>

#include <gdk/gdkkeysyms.h>

#include <math.h>

static GType ags_accessible_notation_edit_get_type(void);
void ags_notation_edit_class_init(AgsNotationEditClass *notation_edit);
void ags_accessible_notation_edit_class_init(AtkObject *object);
void ags_accessible_notation_edit_action_interface_init(AtkActionIface *action);
void ags_notation_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_notation_edit_init(AgsNotationEdit *notation_edit);
void ags_notation_edit_finalize(GObject *gobject);

AtkObject* ags_notation_edit_get_accessible(GtkWidget *widget);

void ags_notation_edit_connect(AgsConnectable *connectable);
void ags_notation_edit_disconnect(AgsConnectable *connectable);

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

void ags_notation_edit_show(GtkWidget *widget);
void ags_notation_edit_show_all(GtkWidget *widget);

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

    ags_type_notation_edit = g_type_register_static(GTK_TYPE_TABLE,
						    "AgsNotationEdit", &ags_notation_edit_info,
						    0);
    
    g_type_add_interface_static(ags_type_notation_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_notation_edit);
  }

  return g_define_type_id__volatile;
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
ags_notation_edit_class_init(AgsNotationEditClass *notation_edit)
{
  GtkWidgetClass *widget;
  
  GObjectClass *gobject;
  
  ags_notation_edit_parent_class = g_type_class_peek_parent(notation_edit);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(notation_edit);

  gobject->finalize = ags_notation_edit_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) notation_edit;

  widget->show = ags_notation_edit_show;
  widget->show_all = ags_notation_edit_show_all;
//  widget->draw = ags_notation_edit_draw;
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
  GtkStyleContext *style_context;
  
  GtkAdjustment *adjustment;

  AgsApplicationContext *application_context;
  
  gdouble gui_scale_factor;

  notation_edit->flags = (AGS_NOTATION_EDIT_SHOW_RULER |
			  AGS_NOTATION_EDIT_SHOW_VSCROLLBAR |
			  AGS_NOTATION_EDIT_SHOW_HSCROLLBAR);
  notation_edit->mode = AGS_NOTATION_EDIT_NO_EDIT_MODE;

  notation_edit->button_mask = 0;
  notation_edit->key_mask = 0;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  notation_edit->note_offset = 0;
  notation_edit->note_offset_absolute = 0;

  notation_edit->control_width = (guint) (gui_scale_factor * AGS_NOTATION_EDIT_DEFAULT_CONTROL_WIDTH);
  notation_edit->control_height = (guint) (gui_scale_factor * AGS_NOTATION_EDIT_DEFAULT_CONTROL_HEIGHT);

  notation_edit->control_margin_x = AGS_NOTATION_EDIT_DEFAULT_CONTROL_MARGIN_X;
  notation_edit->control_margin_y = AGS_NOTATION_EDIT_DEFAULT_CONTROL_MARGIN_Y;

  notation_edit->cursor_position_x = AGS_NOTATION_EDIT_DEFAULT_CURSOR_POSITION_X;
  notation_edit->cursor_position_y = AGS_NOTATION_EDIT_DEFAULT_CURSOR_POSITION_Y;

  notation_edit->selected_note_border = AGS_NOTATION_EDIT_DEFAULT_SELECTED_NOTE_BORDER;

  notation_edit->selection_x0 = 0;
  notation_edit->selection_x1 = 0;
  notation_edit->selection_y0 = 0;
  notation_edit->selection_y1 = 0;

  notation_edit->current_note = NULL;

  gtk_table_set_homogeneous(notation_edit,
			    FALSE);
  
  notation_edit->ruler = ags_ruler_new();
  g_object_set(notation_edit->ruler,
	       "height-request", (gint) (gui_scale_factor * AGS_RULER_DEFAULT_HEIGHT),
	       "font-size",  (guint) (gui_scale_factor * notation_edit->ruler->font_size),
	       "step", (guint) (gui_scale_factor * AGS_RULER_DEFAULT_STEP),
	       "large-step", (guint) (gui_scale_factor * AGS_RULER_DEFAULT_LARGE_STEP),
	       "small-step", (guint) (gui_scale_factor * AGS_RULER_DEFAULT_SMALL_STEP),
	       NULL);
  gtk_table_attach(GTK_TABLE(notation_edit),
		   (GtkWidget *) notation_edit->ruler,
		   0, 1,
		   0, 1,
		   GTK_FILL | GTK_EXPAND, GTK_FILL,
		   0, 0);

  notation_edit->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_has_window(notation_edit->drawing_area,
			    TRUE);
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
		   GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND,
		   0, 0);

  /* vscrollbar */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, notation_edit->control_height, 1.0);
  notation_edit->vscrollbar = (GtkVScrollbar *) gtk_vscrollbar_new(adjustment);
  g_object_set(notation_edit->vscrollbar,
	       "no-show-all", TRUE,
	       NULL);
  gtk_table_attach(GTK_TABLE(notation_edit),
		   (GtkWidget *) notation_edit->vscrollbar,
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* hscrollbar */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, (gdouble) notation_edit->control_width, 1.0);
  notation_edit->hscrollbar = (GtkHScrollbar *) gtk_hscrollbar_new(adjustment);
  g_object_set(notation_edit->hscrollbar,
	       "no-show-all", TRUE,
	       NULL);
  gtk_table_attach(GTK_TABLE(notation_edit),
		   (GtkWidget *) notation_edit->hscrollbar,
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* style context */
  style_context = gtk_widget_get_style_context(notation_edit);
  gtk_style_context_add_class(style_context,
			      "editor");

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
ags_notation_edit_connect(AgsConnectable *connectable)
{
  AgsNotationEdit *notation_edit;

  notation_edit = AGS_NOTATION_EDIT(connectable);

  if((AGS_NOTATION_EDIT_CONNECTED & (notation_edit->flags)) != 0){
    return;
  }

  notation_edit->flags |= AGS_NOTATION_EDIT_CONNECTED;
  
  /* drawing area */
  g_signal_connect(G_OBJECT(notation_edit->drawing_area), "draw",
		   G_CALLBACK(ags_notation_edit_draw_callback), (gpointer) notation_edit);

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

  /* scrollbars */
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

  /* drawing area */
  g_object_disconnect(notation_edit->drawing_area,
		      "any_signal::draw",
		      G_CALLBACK(ags_notation_edit_draw_callback),
		      (gpointer) notation_edit,
		      "any_signal::configure_event",
		      G_CALLBACK(ags_notation_edit_drawing_area_configure_event),
		      (gpointer) notation_edit,
		      "any_signal::button_press_event",
		      G_CALLBACK(ags_notation_edit_drawing_area_button_press_event),
		      (gpointer) notation_edit,
		      "any_signal::button_release_event",
		      G_CALLBACK(ags_notation_edit_drawing_area_button_release_event),
		      (gpointer) notation_edit,
		      "any_signal::motion_notify_event",
		      G_CALLBACK(ags_notation_edit_drawing_area_motion_notify_event),
		      notation_edit,
		      "any_signal::key_press_event",
		      G_CALLBACK(ags_notation_edit_drawing_area_key_press_event),
		      (gpointer) notation_edit,
		      "any_signal::key_release_event",
		      G_CALLBACK(ags_notation_edit_drawing_area_key_release_event),
		      (gpointer) notation_edit,
		      NULL);

  /* scrollbars */
  g_object_disconnect(notation_edit->vscrollbar,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_notation_edit_vscrollbar_value_changed),
		      (gpointer) notation_edit,
		      NULL);
  
  g_object_disconnect(notation_edit->hscrollbar,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_notation_edit_hscrollbar_value_changed),
		      (gpointer) notation_edit,
		      NULL);
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

  notation_edit = (AgsNotationEdit *) gtk_accessible_get_widget(GTK_ACCESSIBLE(action));
  
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
    "cut note to clipboard",
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

void
ags_notation_edit_show(GtkWidget *widget)
{
  AgsNotationEdit *notation_edit;

  notation_edit = AGS_NOTATION_EDIT(widget);

  /* call parent */
  GTK_WIDGET_CLASS(ags_notation_edit_parent_class)->show(widget);

  if((AGS_NOTATION_EDIT_SHOW_RULER & (notation_edit->flags)) != 0){    
    gtk_widget_show((GtkWidget *) notation_edit->ruler);
  }

  if((AGS_NOTATION_EDIT_SHOW_VSCROLLBAR & (notation_edit->flags)) != 0){
    gtk_widget_show((GtkWidget *) notation_edit->vscrollbar);
  }

  if((AGS_NOTATION_EDIT_SHOW_HSCROLLBAR & (notation_edit->flags)) != 0){
    gtk_widget_show((GtkWidget *) notation_edit->hscrollbar);
  }

  ags_notation_edit_reset_vscrollbar(notation_edit);
  ags_notation_edit_reset_hscrollbar(notation_edit);
}

void
ags_notation_edit_show_all(GtkWidget *widget)
{
  AgsNotationEdit *notation_edit;

  notation_edit = AGS_NOTATION_EDIT(widget);

  /* call parent */
  GTK_WIDGET_CLASS(ags_notation_edit_parent_class)->show_all(widget);

  if((AGS_NOTATION_EDIT_SHOW_RULER & (notation_edit->flags)) != 0){
    gtk_widget_show((GtkWidget *) notation_edit->ruler);
  }

  if((AGS_NOTATION_EDIT_SHOW_VSCROLLBAR & (notation_edit->flags)) != 0){
    gtk_widget_show((GtkWidget *) notation_edit->vscrollbar);
  }

  if((AGS_NOTATION_EDIT_SHOW_HSCROLLBAR & (notation_edit->flags)) != 0){
    gtk_widget_show((GtkWidget *) notation_edit->hscrollbar);
  }

  ags_notation_edit_reset_vscrollbar(notation_edit);
  ags_notation_edit_reset_hscrollbar(notation_edit);
}

gboolean
ags_notation_edit_auto_scroll_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_notation_edit_auto_scroll,
			 widget) != NULL){
    AgsNotationEditor *notation_editor;
    AgsNotationEdit *notation_edit;

    GtkAdjustment *hscrollbar_adjustment;
    
    GObject *output_soundcard;
    
    double x;
    
    notation_edit = AGS_NOTATION_EDIT(widget);

    if((AGS_NOTATION_EDIT_AUTO_SCROLL & (notation_edit->flags)) == 0){
      return(TRUE);
    }
    
    notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor((GtkWidget *) notation_edit,
								    AGS_TYPE_NOTATION_EDITOR);
    
    if(notation_editor->selected_machine == NULL){
      return(TRUE);
    }

    /* reset offset */
    g_object_get(notation_editor->selected_machine->audio,
		 "output-soundcard", &output_soundcard,
		 NULL);
    
    notation_edit->note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));
    notation_edit->note_offset_absolute = ags_soundcard_get_note_offset_absolute(AGS_SOUNDCARD(output_soundcard));

    /* reset scrollbar */
    hscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(notation_edit->hscrollbar));
    x = ((notation_edit->note_offset * notation_edit->control_width) / (AGS_NOTATION_EDITOR_MAX_CONTROLS * notation_edit->control_width)) * gtk_adjustment_get_upper(hscrollbar_adjustment);
    
    gtk_range_set_value(GTK_RANGE(notation_edit->hscrollbar),
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
  AgsNotationEditor *notation_editor;

  GtkAdjustment *adjustment;
  GtkAdjustment *piano_adjustment;

  GtkAllocation allocation;
  
  guint channel_count;
  double varea_height;
  gdouble upper, old_upper;
  
  if(!AGS_IS_NOTATION_EDIT(notation_edit)){
    return;
  }

  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor((GtkWidget *) notation_edit,
								  AGS_TYPE_NOTATION_EDITOR);

  /* fix margin-bottom */
  gtk_widget_get_allocation(notation_editor->notation_edit->hscrollbar, &allocation);
  
  g_object_set(notation_editor->scrolled_piano,
	       "margin-bottom", (gint) allocation.height,
	       NULL);

  if(notation_editor->selected_machine == NULL){
    return;
  }
  
  /* */
  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);
  
  /* adjustment */
  adjustment = gtk_range_get_adjustment(GTK_RANGE(notation_edit->vscrollbar));

  g_object_get(notation_editor->scrolled_piano->viewport,
	       "vadjustment", &piano_adjustment,
	       NULL);

  /* get channel count */
#if 0
  if(ags_audio_test_flags(notation_editor->selected_machine->audio, AGS_AUDIO_NOTATION_DEFAULT)){
    g_object_get(notation_editor->selected_machine->audio,
		 "input-pads", &channel_count,
		 NULL);
    channel_count = notation_editor->selected_machine->audio->input_pads;
  }else{
    g_object_get(notation_editor->selected_machine->audio,
		 "output-pads", &channel_count,
		 NULL);
  }
#else
  g_object_get(notation_editor->selected_machine->audio,
	       "input-pads", &channel_count,
	       NULL);
#endif
  
  /* upper */
  old_upper = gtk_adjustment_get_upper(adjustment); 

  varea_height = (channel_count * notation_edit->control_height);
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
}

void
ags_notation_edit_reset_hscrollbar(AgsNotationEdit *notation_edit)
{
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;

  GtkAdjustment *adjustment;

  AgsApplicationContext *application_context;
  
  GtkAllocation allocation;

  gdouble gui_scale_factor;
  double zoom_factor, zoom;
  double zoom_correction;
  guint map_width;
  gdouble upper, old_upper;
  
  if(!AGS_IS_NOTATION_EDIT(notation_edit)){
    return;
  }

  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor((GtkWidget *) notation_edit,
								  AGS_TYPE_NOTATION_EDITOR);

  if(notation_editor->selected_machine == NULL){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  notation_toolbar = notation_editor->notation_toolbar;

  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);

  /* adjustment */
  adjustment = gtk_range_get_adjustment(GTK_RANGE(notation_edit->hscrollbar));

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom) - 2.0);

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
#if 0
    gtk_adjustment_set_value(adjustment,
			     gtk_adjustment_get_value(adjustment) / old_upper * upper);
#endif
  }
}

void
ags_notation_edit_draw_segment(AgsNotationEdit *notation_edit, cairo_t *cr)
{
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;
  
  GtkStyleContext *notation_edit_style_context;

  GtkAdjustment *vscrollbar_adjustment;
  GtkAdjustment *hscrollbar_adjustment;

  GtkAllocation allocation;

  GdkRGBA *fg_color;
  GdkRGBA *bg_color;
  GdkRGBA *border_color;

  guint channel_count;
  guint width, height;
  gboolean width_fits, height_fits;
  double zoom;
  guint y0, x0;
  guint nth_x;
  guint i, j;
  guint j_set;

  GValue value = {0,};

  const static double segment_dashes = {
    0.5,
  };

  if(!AGS_IS_NOTATION_EDIT(notation_edit)){
    return;
  }

  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor((GtkWidget *) notation_edit,
								  AGS_TYPE_NOTATION_EDITOR);

  if(notation_editor->selected_machine == NULL){
    return;
  }

  notation_toolbar = notation_editor->notation_toolbar;
  
  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);
  
  /* style context */
  notation_edit_style_context = gtk_widget_get_style_context(GTK_WIDGET(notation_edit->drawing_area));

  gtk_style_context_get_property(notation_edit_style_context,
				 "color",
				 GTK_STATE_FLAG_NORMAL,
				 &value);

  fg_color = g_value_dup_boxed(&value);
  g_value_unset(&value);

  gtk_style_context_get_property(notation_edit_style_context,
				 "background-color",
				 GTK_STATE_FLAG_NORMAL,
				 &value);

  bg_color = g_value_dup_boxed(&value);
  g_value_unset(&value);

  gtk_style_context_get_property(notation_edit_style_context,
				 "border-color",
				 GTK_STATE_FLAG_NORMAL,
				 &value);

  border_color = g_value_dup_boxed(&value);
  g_value_unset(&value);

  /* adjustment */
  vscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(notation_edit->vscrollbar));
  hscrollbar_adjustment = gtk_range_get_adjustment(GTK_RANGE(notation_edit->hscrollbar));
  
  /* get channel count */
#if 0
  if(ags_audio_test_flags(notation_editor->selected_machine->audio, AGS_AUDIO_NOTATION_DEFAULT)){
    g_object_get(notation_editor->selected_machine->audio,
		 "input-pads", &channel_count,
		 NULL);
    channel_count = notation_editor->selected_machine->audio->input_pads;
  }else{
    g_object_get(notation_editor->selected_machine->audio,
		 "output-pads", &channel_count,
		 NULL);
  }
#else
  g_object_get(notation_editor->selected_machine->audio,
	       "input-pads", &channel_count,
	       NULL);
#endif
    
  /* get width */
  width = allocation.width;
  width_fits = FALSE;
  
  if(AGS_NOTATION_EDITOR_MAX_CONTROLS * notation_edit->control_width < width){
    width = AGS_NOTATION_EDITOR_MAX_CONTROLS * notation_edit->control_width;
    width_fits = TRUE;
  }

  /* get height */
  height = allocation.height;
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
			bg_color->red,
			bg_color->green,
			bg_color->blue,
			bg_color->alpha);
  cairo_rectangle(cr,
		  0.0, 0.0,
		  (double) allocation.width, (double) allocation.height);
  cairo_fill(cr);
  
  /* horizontal lines */
  cairo_set_line_width(cr,
		       1.0);

  cairo_set_source_rgba(cr,
			fg_color->red,
			fg_color->blue,
			fg_color->green,
			fg_color->alpha);

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
			fg_color->red,
			fg_color->blue,
			fg_color->green,
			fg_color->alpha);

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

  g_boxed_free(GDK_TYPE_RGBA, fg_color);
  g_boxed_free(GDK_TYPE_RGBA, bg_color);
  g_boxed_free(GDK_TYPE_RGBA, border_color);
}

void
ags_notation_edit_draw_position(AgsNotationEdit *notation_edit, cairo_t *cr)
{
  AgsNotationEditor *notation_editor;

  GtkStyleContext *notation_edit_style_context;

  GtkAllocation allocation;

  GdkRGBA *fg_color_active;

  guint channel_count;
  double position;
  double x, y;
  double width, height;
  gboolean height_fits;

  GValue value = {0,};

  if(!AGS_IS_NOTATION_EDIT(notation_edit)){
    return;
  }

  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor((GtkWidget *) notation_edit,
								  AGS_TYPE_NOTATION_EDITOR);

  if(notation_editor->selected_machine == NULL){
    return;
  }

  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);
  
  /* style context */
  notation_edit_style_context = gtk_widget_get_style_context(GTK_WIDGET(notation_edit->drawing_area));

  gtk_style_context_get_property(notation_edit_style_context,
				 "color",
				 GTK_STATE_FLAG_ACTIVE,
				 &value);

  fg_color_active = g_value_dup_boxed(&value);
  g_value_unset(&value);
  
  /* get channel count */
#if 0
  if(ags_audio_test_flags(notation_editor->selected_machine->audio, AGS_AUDIO_NOTATION_DEFAULT)){
    g_object_get(notation_editor->selected_machine->audio,
		 "input-pads", &channel_count,
		 NULL);
    channel_count = notation_editor->selected_machine->audio->input_pads;
  }else{
    g_object_get(notation_editor->selected_machine->audio,
		 "output-pads", &channel_count,
		 NULL);
  }
#else
  g_object_get(notation_editor->selected_machine->audio,
	       "input-pads", &channel_count,
	       NULL);
#endif
  
  /* get offset and dimensions */
  position = ((double) notation_edit->note_offset) * ((double) notation_edit->control_width);
  
  y = 0.0;
  x = (position) - (gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)));

  height = (double) allocation.height;
  width = (double) AGS_NOTATION_EDIT_DEFAULT_FADER_WIDTH;

  if(height < channel_count * notation_edit->control_height){
    height = channel_count * notation_edit->control_height;
  }

  /* push group */
  cairo_push_group(cr);
  
  /* draw fader */
  cairo_set_source_rgba(cr,
			fg_color_active->red,
			fg_color_active->blue,
			fg_color_active->green,
			fg_color_active->alpha);

  cairo_rectangle(cr,
		  (double) x, (double) y,
		  (double) width, (double) height);
  cairo_fill(cr);

  /* complete */
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
      
//  cairo_surface_mark_dirty(cairo_get_target(cr));

  g_boxed_free(GDK_TYPE_RGBA, fg_color_active);
}

void
ags_notation_edit_draw_cursor(AgsNotationEdit *notation_edit, cairo_t *cr)
{
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;

  GtkStyleContext *notation_edit_style_context;

  GtkAllocation allocation;
  
  GdkRGBA *fg_color_focused;

  double zoom_factor;
  double x, y;
  double width, height;

  GValue value = {0,};

  if(!AGS_IS_NOTATION_EDIT(notation_edit)){
    return;
  }
  
  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor((GtkWidget *) notation_edit,
								  AGS_TYPE_NOTATION_EDITOR);

  if(notation_editor->selected_machine == NULL){
    return;
  }

  notation_toolbar = notation_editor->notation_toolbar;

  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);

  /* style context */
  notation_edit_style_context = gtk_widget_get_style_context(GTK_WIDGET(notation_edit->drawing_area));

  gtk_style_context_get_property(notation_edit_style_context,
				 "color",
				 GTK_STATE_FLAG_FOCUSED,
				 &value);

  fg_color_focused = g_value_dup_boxed(&value);
  g_value_unset(&value);

  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

  /* get offset */
  x = ((double) notation_edit->cursor_position_x * (double) notation_edit->control_width) - (gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar)) * zoom_factor);
  y = ((double) notation_edit->cursor_position_y * (double) notation_edit->control_height) - gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar));

  width = (double) notation_edit->control_width;
  height = (double) notation_edit->control_height;

  /* apply zoom */
  x /= zoom_factor;

  /* clip */
  if(x < 0.0){
    width += x;

    x = 0.0;
  }else if(x > allocation.width){
    g_boxed_free(GDK_TYPE_RGBA, fg_color_focused);
    
    return;
  }

  if(x + width > allocation.width){
    width = ((double) allocation.width) - x;
  }
  
  if(y < 0.0){
    height += y;

    y = 0.0;
  }else if(y > allocation.height){
    g_boxed_free(GDK_TYPE_RGBA, fg_color_focused);

    return;
  }

  if(y + height > allocation.height){
    height = ((double) allocation.height) - y;
  }

  /* push group */
  cairo_push_group(cr);

  /* draw cursor */
  cairo_set_source_rgba(cr,
			fg_color_focused->red,
			fg_color_focused->blue,
			fg_color_focused->green,
			fg_color_focused->alpha);

  cairo_rectangle(cr,
		  x, y,
		  width, height);
  cairo_fill(cr);

  /* complete */
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
      
//  cairo_surface_mark_dirty(cairo_get_target(cr));

  g_boxed_free(GDK_TYPE_RGBA, fg_color_focused);
}

void
ags_notation_edit_draw_selection(AgsNotationEdit *notation_edit, cairo_t *cr)
{
  GtkAllocation allocation;

  GtkStyleContext *notation_edit_style_context;

  GdkRGBA *fg_color_prelight;
  
  double x, y;
  double width, height;

  GValue value = {0,};

  if(!AGS_IS_NOTATION_EDIT(notation_edit)){
    return;
  }

  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);

  /* style context */
  notation_edit_style_context = gtk_widget_get_style_context(GTK_WIDGET(notation_edit->drawing_area));

  gtk_style_context_get_property(notation_edit_style_context,
				 "color",
				 GTK_STATE_FLAG_PRELIGHT,
				 &value);

  fg_color_prelight = g_value_dup_boxed(&value);
  g_value_unset(&value);

  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);

  /* get offset and dimensions */
  if(notation_edit->selection_x0 < notation_edit->selection_x1){
    x = ((double) notation_edit->selection_x0) - gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar));
    width = ((double) notation_edit->selection_x1 - (double) notation_edit->selection_x0);
  }else{
    x = ((double) notation_edit->selection_x1) - gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar));
    width = ((double) notation_edit->selection_x0 - (double) notation_edit->selection_x1);
  }

  if(notation_edit->selection_y0 < notation_edit->selection_y1){
    y = ((double) notation_edit->selection_y0) - gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar));
    height = ((double) notation_edit->selection_y1 - (double) notation_edit->selection_y0);
  }else{
    y = ((double) notation_edit->selection_y1) - gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar));
    height = ((double) notation_edit->selection_y0 - (double) notation_edit->selection_y1);
  }

  /* clip */
  if(x < 0.0){
    width += x;

    x = 0.0;
  }else if(x > allocation.width){
    g_boxed_free(GDK_TYPE_RGBA, fg_color_prelight);
    
    return;
  }

  if(x + width > allocation.width){
    width = ((double) allocation.width) - x;
  }
  
  if(y < 0.0){
    height += y;

    y = 0.0;
  }else if(y > allocation.height){
    g_boxed_free(GDK_TYPE_RGBA, fg_color_prelight);

    return;
  }

  if(y + height > allocation.height){
    height = ((double) allocation.height) - y;
  }
    
  /* push group */
  cairo_push_group(cr);

  /* draw selection */
  cairo_set_source_rgba(cr,
			fg_color_prelight->red,
			fg_color_prelight->blue,
			fg_color_prelight->green,
			1.0 / 3.0);

  cairo_rectangle(cr,
		  x, y,
		  width, height);
  cairo_fill(cr);

  /* complete */
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
      
//  cairo_surface_mark_dirty(cairo_get_target(cr));

  g_boxed_free(GDK_TYPE_RGBA, fg_color_prelight);
}

void
ags_notation_edit_draw_note(AgsNotationEdit *notation_edit,
			    AgsNote *note,
			    cairo_t *cr,
			    gdouble opacity)
{
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;

  GtkStyleContext *notation_edit_style_context;

  GtkAllocation allocation;

  GdkRGBA *fg_color;
  GdkRGBA *fg_color_selected;
  
  double zoom_factor;
  guint channel_count;
  double viewport_x, viewport_y;
  double x, y;
  double width, height;

  GValue value = {0,};
  
  if(!AGS_IS_NOTATION_EDIT(notation_edit) ||
     !AGS_IS_NOTE(note)){
    return;
  }

  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor((GtkWidget *) notation_edit,
								  AGS_TYPE_NOTATION_EDITOR);

  if(notation_editor->selected_machine == NULL){
    return;
  }

  notation_toolbar = notation_editor->notation_toolbar;
  
  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);

  /* style context */
  notation_edit_style_context = gtk_widget_get_style_context(GTK_WIDGET(notation_edit->drawing_area));

  gtk_style_context_get_property(notation_edit_style_context,
				 "color",
				 GTK_STATE_FLAG_NORMAL,
				 &value);

  fg_color = g_value_dup_boxed(&value);
  g_value_unset(&value);

  gtk_style_context_get_property(notation_edit_style_context,
				 "color",
				 GTK_STATE_FLAG_SELECTED,
				 &value);

  fg_color_selected = g_value_dup_boxed(&value);
  g_value_unset(&value);

  /* get channel count */
#if 0
  if(ags_audio_test_flags(notation_editor->selected_machine->audio, AGS_AUDIO_NOTATION_DEFAULT)){
    g_object_get(notation_editor->selected_machine->audio,
		 "input-pads", &channel_count,
		 NULL);
    channel_count = notation_editor->selected_machine->audio->input_pads;
  }else{
    g_object_get(notation_editor->selected_machine->audio,
		 "output-pads", &channel_count,
		 NULL);
  }
#else
  g_object_get(notation_editor->selected_machine->audio,
	       "input-pads", &channel_count,
	       NULL);
#endif
  
  /* zoom */
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

  /* get offset and dimensions */
  if((AGS_NOTATION_EDITOR_MAX_CONTROLS * notation_edit->control_width) > allocation.width){
    viewport_x = zoom_factor * gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar));
  }else{
    viewport_x = 0.0;
  }
  
  if((channel_count * notation_edit->control_height) > allocation.height){
    viewport_y = gtk_range_get_value(GTK_RANGE(notation_edit->vscrollbar));
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
      g_boxed_free(GDK_TYPE_RGBA, fg_color);
      g_boxed_free(GDK_TYPE_RGBA, fg_color_selected);
      
      return;
    }else{
      width += x;
      x = 0.0;
    }
  }else if(x > allocation.width){
    g_boxed_free(GDK_TYPE_RGBA, fg_color);
    g_boxed_free(GDK_TYPE_RGBA, fg_color_selected);

    return;
  }

  if(x + width > allocation.width){
    width = ((double) allocation.width) - x;
  }
  
  if(y < 0.0){
    if(y + height < 0.0){
      g_boxed_free(GDK_TYPE_RGBA, fg_color);
      g_boxed_free(GDK_TYPE_RGBA, fg_color_selected);

      return;
    }else{
      height += y;
      y = 0.0;
    }
  }else if(y > allocation.height){
    g_boxed_free(GDK_TYPE_RGBA, fg_color);
    g_boxed_free(GDK_TYPE_RGBA, fg_color_selected);

    return;
  }

  if(y + height > allocation.height){
    height = ((double) allocation.height) - y;
  }
  
  /* draw note */
  cairo_set_source_rgba(cr,
			fg_color->red,
			fg_color->blue,
			fg_color->green,
			opacity * fg_color->alpha);
  
  cairo_rectangle(cr,
		  x, y,
		  width, height);
  cairo_fill(cr);

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
			  fg_color_selected->red,
			  fg_color_selected->blue,
			  fg_color_selected->green,
			  opacity / 3.0);
    
    cairo_rectangle(cr,
		    selected_x, selected_y,
		    selected_width, selected_height);
    cairo_fill(cr);
  }

  g_boxed_free(GDK_TYPE_RGBA, fg_color);
  g_boxed_free(GDK_TYPE_RGBA, fg_color_selected);
}

void
ags_notation_edit_draw_notation(AgsNotationEdit *notation_edit, cairo_t *cr)
{
  AgsNotationEditor *notation_editor;
  AgsNotationToolbar *notation_toolbar;

  GtkAllocation allocation;
  
  AgsTimestamp *timestamp;
  AgsTimestamp *current_timestamp;    
  
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

  notation_editor = (AgsNotationEditor *) gtk_widget_get_ancestor((GtkWidget *) notation_edit,
								  AGS_TYPE_NOTATION_EDITOR);
  notation_toolbar = notation_editor->notation_toolbar;

  if(notation_editor->selected_machine == NULL){
    return;
  }
  
  gtk_widget_get_allocation(GTK_WIDGET(notation_edit->drawing_area),
			    &allocation);

  opacity = gtk_spin_button_get_value(notation_editor->notation_toolbar->opacity);

  /* zoom */
  zoom = exp2((double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom) - 2.0);
  zoom_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) notation_toolbar->zoom));

  /* get visisble region */
  x0 = (zoom_factor * gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar))) / notation_edit->control_width;
  x1 = ((zoom_factor * gtk_range_get_value(GTK_RANGE(notation_edit->hscrollbar))) / notation_edit->control_width) + (allocation.width * zoom);
  
  /* draw notation */
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  g_object_get(notation_editor->selected_machine->audio,
	       "notation", &start_list_notation,
	       NULL);

  timestamp->timer.ags_offset.offset = (guint64) AGS_NOTATION_DEFAULT_OFFSET * floor((double) x0 / (double) AGS_NOTATION_DEFAULT_OFFSET);

  i = 0;
  
  while((i = ags_notebook_next_active_tab(notation_editor->notebook,
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
