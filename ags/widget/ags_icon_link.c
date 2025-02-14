/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include "ags_icon_link.h"

#include <ags/widget/ags_widget_marshal.h>

#include <gmodule.h>

#include <ags/i18n.h>

void ags_icon_link_class_init(AgsIconLinkClass *icon_link);
void ags_icon_link_orientable_interface_init(GtkOrientableIface *orientable);
void ags_icon_link_init(AgsIconLink *icon_link);
void ags_icon_link_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_icon_link_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_icon_link_dispose(GObject *gobject);
void ags_icon_link_finalize(GObject *gobject);

void ags_icon_link_snapshot(GtkWidget *widget,
			    GtkSnapshot *snapshot);

AgsIconLink* ags_icon_link_real_copy_event(AgsIconLink *icon_link);

void ags_icon_link_delete_callback(GAction *action, GVariant *parameter,
				   AgsIconLink *icon_link);

void ags_icon_link_gesture_click_pressed_callback(GtkGestureClick *event_controller,
						  gint n_press,
						  gdouble x,
						  gdouble y,
						  AgsIconLink *icon_link);
void ags_icon_link_gesture_click_released_callback(GtkGestureClick *event_controller,
						   gint n_press,
						   gdouble x,
						   gdouble y,
						   AgsIconLink *icon_link);

void ags_icon_link_gesture_secondary_pressed_callback(GtkGestureClick *event_controller,
						      gint n_press,
						      gdouble x,
						      gdouble y,
						      AgsIconLink *icon_link);
void ags_icon_link_gesture_secondary_released_callback(GtkGestureClick *event_controller,
						       gint n_press,
						       gdouble x,
						       gdouble y,
						       AgsIconLink *icon_link);

void ags_icon_link_enter_callback(GtkEventControllerMotion *event_controller,
				  gdouble x,
				  gdouble y,
				  AgsIconLink *icon_link);
void ags_icon_link_leave_callback(GtkEventControllerMotion *event_controller,
				  AgsIconLink *icon_link);

/**
 * SECTION:ags_icon_link
 * @short_description: a icon_link widget
 * @title: AgsIconLink
 * @section_id:
 * @include: ags/widget/ags_icon_link.h
 *
 * #AgsIconLink is a widget representing a #GtkAdjustment.
 */

enum{
  DELETE_EVENT,
  COPY_EVENT,
  CLICKED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_ACTION,
};

static gpointer ags_icon_link_parent_class = NULL;
static guint icon_link_signals[LAST_SIGNAL];

GType
ags_icon_link_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_icon_link = 0;

    static const GTypeInfo ags_icon_link_info = {
      sizeof(AgsIconLinkClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_icon_link_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsIconLink),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_icon_link_init,
    };

    ags_type_icon_link = g_type_register_static(GTK_TYPE_BOX,
						"AgsIconLink", &ags_icon_link_info,
						0);
    
    g_once_init_leave(&g_define_type_id__static, ags_type_icon_link);
  }

  return(g_define_type_id__static);
}

void
ags_icon_link_class_init(AgsIconLinkClass *icon_link)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  
  GParamSpec *param_spec;

  ags_icon_link_parent_class = g_type_class_peek_parent(icon_link);

  /* GObjectClass */
  gobject = (GObjectClass *) icon_link;

  gobject->set_property = ags_icon_link_set_property;
  gobject->get_property = ags_icon_link_get_property;

  gobject->dispose = ags_icon_link_dispose;
  gobject->finalize = ags_icon_link_finalize;
  
  /* properties */
  /**
   * AgsIconLink:action:
   *
   * The assigned action.
   * 
   * Since: 6.6.0
   */
  param_spec = g_param_spec_string("action",
				   "action",
				   "The assigned action",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ACTION,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) icon_link;

  widget->snapshot = ags_icon_link_snapshot;

  /* AgsIconLinkClass */  
  icon_link->delete_event = NULL;

  icon_link->copy_event = ags_icon_link_real_copy_event;

  icon_link->clicked = NULL;

  /* events */
  /**
   * AgsIconLink::delete-event:
   * @icon_link: the #AgsIconLink
   *
   * The ::delete-event signal notifies about widget delete_event.
   *
   * Since: 6.6.0
   */
  icon_link_signals[DELETE_EVENT] =
    g_signal_new("delete-event",
		 G_TYPE_FROM_CLASS(icon_link),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsIconLinkClass, delete_event),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsIconLink::copy-event:
   * @icon_link: the #AgsIconLink
   *
   * The ::copy-event signal notifies about widget copy_event.
   *
   * Returns: (transfer none): the #AgsIconLink
   * 
   * Since: 6.6.0
   */
  icon_link_signals[COPY_EVENT] =
    g_signal_new("copy-event",
		 G_TYPE_FROM_CLASS(icon_link),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsIconLinkClass, copy_event),
		 NULL, NULL,
		 ags_widget_cclosure_marshal_OBJECT__VOID,
		 AGS_TYPE_ICON_LINK, 0);

  /**
   * AgsIconLink::clicked:
   * @icon_link: the #AgsIconLink
   *
   * The ::clicked signal notifies about widget clicked.
   *
   * Since: 6.6.0
   */
  icon_link_signals[CLICKED] =
    g_signal_new("clicked",
		 G_TYPE_FROM_CLASS(icon_link),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsIconLinkClass, clicked),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_icon_link_init(AgsIconLink *icon_link)
{
  GtkEventController *event_controller;
  
  GSimpleActionGroup *context_group;
  GSimpleAction *action;
  GMenuItem *menu_item;

  gtk_widget_set_can_focus((GtkWidget *) icon_link,
			   TRUE);

  gtk_orientable_set_orientation(GTK_ORIENTABLE(icon_link),
				 GTK_ORIENTATION_VERTICAL);

  icon_link->flags = 0;
  
  icon_link->icon = (GtkImage *) gtk_image_new();
  gtk_image_set_icon_size(icon_link->icon,
			  GTK_ICON_SIZE_LARGE);
  gtk_box_append((GtkBox *) icon_link,
		 (GtkWidget *) icon_link->icon);

  icon_link->link = (GtkLabel *) gtk_label_new(NULL);
  gtk_label_set_use_markup(icon_link->link,
			   TRUE);
  gtk_box_append((GtkBox *) icon_link,
		 (GtkWidget *) icon_link->link);

  /* context menu */
  icon_link->context_popup = (GMenu *) g_menu_new();

  menu_item = g_menu_item_new(i18n("delete"),
			      "icon_link.delete");
  g_menu_append_item(icon_link->context_popup,
		     menu_item);

  icon_link->context_popover = (GtkPopoverMenu *) gtk_popover_menu_new_from_model(G_MENU_MODEL(icon_link->context_popup));
  gtk_widget_set_parent((GtkWidget *) icon_link->context_popover,
			(GtkWidget *) icon_link->icon);
  
  context_group =
    icon_link->context_group = g_simple_action_group_new();
  gtk_widget_insert_action_group((GtkWidget *) icon_link->context_popover,
				 "icon_link",
				 G_ACTION_GROUP(context_group));

  /* delete */
  action = g_simple_action_new("delete",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_icon_link_delete_callback), icon_link);
  g_action_map_add_action(G_ACTION_MAP(context_group),
			  G_ACTION(action));

  /* events - gesture click */
  event_controller = (GtkEventController *) gtk_gesture_click_new();
  gtk_widget_add_controller((GtkWidget *) icon_link,
			    event_controller);

  g_signal_connect(event_controller, "pressed",
		   G_CALLBACK(ags_icon_link_gesture_click_pressed_callback), icon_link);

  g_signal_connect(event_controller, "released",
		   G_CALLBACK(ags_icon_link_gesture_click_released_callback), icon_link);

  /* events - gesture secondary */
  event_controller = (GtkEventController *) gtk_gesture_click_new();

  gtk_gesture_single_set_button((GtkGestureSingle *) event_controller,
				GDK_BUTTON_SECONDARY);
  
  gtk_widget_add_controller((GtkWidget *) icon_link,
			    event_controller);

  g_signal_connect(event_controller, "pressed",
		   G_CALLBACK(ags_icon_link_gesture_secondary_pressed_callback), icon_link);

  g_signal_connect(event_controller, "released",
		   G_CALLBACK(ags_icon_link_gesture_secondary_released_callback), icon_link);
  
  /* events - motion */
  event_controller = gtk_event_controller_motion_new();
  gtk_widget_add_controller((GtkWidget *) icon_link,
			    event_controller);

  g_signal_connect_after(event_controller, "enter",
			 G_CALLBACK(ags_icon_link_enter_callback), icon_link);

  g_signal_connect_after(event_controller, "leave",
			 G_CALLBACK(ags_icon_link_leave_callback), icon_link);
}

void
ags_icon_link_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsIconLink *icon_link;

  icon_link = AGS_ICON_LINK(gobject);

  switch(prop_id){
  case PROP_ACTION:
    {
      gchar *action;

      action = g_value_get_string(value);

      if(action == icon_link->action){
	return;
      }

      g_free(icon_link->action);
      
      icon_link->action = g_strdup(action);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_icon_link_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsIconLink *icon_link;

  icon_link = AGS_ICON_LINK(gobject);

  switch(prop_id){
  case PROP_ACTION:
    {
      g_value_set_string(value, icon_link->action);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_icon_link_dispose(GObject *gobject)
{  
  AgsIconLink *icon_link;

  icon_link = AGS_ICON_LINK(gobject);

  gtk_widget_unparent((GtkWidget *) icon_link->context_popover);

  /* call parent */
  G_OBJECT_CLASS(ags_icon_link_parent_class)->dispose(gobject);
}

void
ags_icon_link_finalize(GObject *gobject)
{  
  /* call parent */
  G_OBJECT_CLASS(ags_icon_link_parent_class)->finalize(gobject);
}

void
ags_icon_link_snapshot(GtkWidget *widget,
		       GtkSnapshot *snapshot)
{
  GtkStyleContext *style_context;

  cairo_t *cr;

  graphene_rect_t rect;
  
  gint width, height;

  /* call parent */
  GTK_WIDGET_CLASS(ags_icon_link_parent_class)->snapshot(widget,
							 snapshot);

  if(ags_icon_link_test_flags((AgsIconLink *) widget, AGS_ICON_LINK_HIGHLIGHT)){
    style_context = gtk_widget_get_style_context((GtkWidget *) widget);  

    width = gtk_widget_get_width(widget);
    height = gtk_widget_get_height(widget);
  
    graphene_rect_init(&rect,
		       0.0, 0.0,
		       (float) width, (float) height);
  
    cr = gtk_snapshot_append_cairo(snapshot,
				   &rect);
  
    /* highlight */
    cairo_set_source_rgba(cr,
			  1.0,
			  1.0,
			  1.0,
			  0.333);
    cairo_rectangle(cr,
		    0.0, 0.0,
		    (gdouble) width, (gdouble) height);
    cairo_fill(cr);
    
    cairo_destroy(cr);
  }
}

void
ags_icon_link_delete_callback(GAction *action, GVariant *parameter,
			      AgsIconLink *icon_link)
{
  ags_icon_link_delete_event(icon_link);
}

/**
 * ags_icon_link_test_flags:
 * @icon_link: the #AgsIconLink
 * @flags: the flags
 *
 * Test @flags of @icon_link.
 * 
 * Returns: %TRUE if flags set, otherwise %FALSE
 * 
 * Since: 6.6.0
 */
gboolean
ags_icon_link_test_flags(AgsIconLink *icon_link,
			 guint flags)
{
  gboolean success;

  if(!AGS_IS_ICON_LINK(icon_link)){
    return(FALSE);
  }
  
  success = ((flags & (icon_link->flags)) != 0) ? TRUE: FALSE;
  
  return(success);
}

/**
 * ags_icon_link_set_flags:
 * @icon_link: the #AgsIconLink
 * @flags: the flags
 *
 * Set @flags of @icon_link.
 * 
 * Since: 6.6.0
 */
void
ags_icon_link_set_flags(AgsIconLink *icon_link,
			guint flags)
{
  if(!AGS_IS_ICON_LINK(icon_link)){
    return;
  }

  icon_link->flags |= flags;
}

/**
 * ags_icon_link_unset_flags:
 * @icon_link: the #AgsIconLink
 * @flags: the flags
 *
 * Set @flags of @icon_link.
 * 
 * Since: 6.6.0
 */
void
ags_icon_link_unset_flags(AgsIconLink *icon_link,
			  guint flags)
{
  if(!AGS_IS_ICON_LINK(icon_link)){
    return;
  }

  icon_link->flags &= (~flags);
}

/**
 * ags_icon_link_get_icon_name:
 * @icon_link: the #AgsIconLink
 *
 * Get icon name of @icon_link.
 *
 * Returns: the icon name as string
 * 
 * Since: 6.6.0
 */
gchar*
ags_icon_link_get_icon_name(AgsIconLink *icon_link)
{
  gchar *icon_name;
  
  if(!AGS_IS_ICON_LINK(icon_link)){
    return(NULL);
  }

  icon_name = gtk_image_get_icon_name(icon_link->icon);

  return(icon_name);
}

/**
 * ags_icon_link_set_icon_name:
 * @icon_link: the #AgsIconLink
 * @icon_name: the icon name
 *
 * Set @icon_name of @icon_link.
 * 
 * Since: 6.6.0
 */
void
ags_icon_link_set_icon_name(AgsIconLink *icon_link,
			    gchar *icon_name)
{
  if(!AGS_IS_ICON_LINK(icon_link)){
    return;
  }

  gtk_image_set_from_icon_name(icon_link->icon,
			       icon_name);
}

/**
 * ags_icon_link_get_action:
 * @icon_link: the #AgsIconLink
 *
 * Get action of @icon_link.
 *
 * Returns: the action as string
 * 
 * Since: 6.6.0
 */
gchar*
ags_icon_link_get_action(AgsIconLink *icon_link)
{
  gchar *action;

  if(!AGS_IS_ICON_LINK(icon_link)){
    return(NULL);
  }

  action = NULL;
  
  g_object_get(icon_link,
	       "action", &action,
	       NULL);

  return(action);
}

/**
 * ags_icon_link_set_action:
 * @icon_link: the #AgsIconLink
 * @action: the action
 *
 * Set @action of @icon_link.
 * 
 * Since: 6.6.0
 */
void
ags_icon_link_set_action(AgsIconLink *icon_link,
			 gchar *action)
{
  if(!AGS_IS_ICON_LINK(icon_link)){
    return;
  }

  g_object_set(icon_link,
	       "action", action,
	       NULL);
}

/**
 * ags_icon_link_get_link_text:
 * @icon_link: the #AgsIconLink
 *
 * Get link text of @icon_link.
 *
 * Returns: the link text as string
 * 
 * Since: 6.6.0
 */
gchar*
ags_icon_link_get_link_text(AgsIconLink *icon_link)
{
  gchar *link_text;

  if(!AGS_IS_ICON_LINK(icon_link)){
    return(NULL);
  }

  link_text = gtk_label_get_text(icon_link->link);

  return(link_text);
}

/**
 * ags_icon_link_set_link_text:
 * @icon_link: the #AgsIconLink
 * @link_text: the link text
 *
 * Set @link_text of @icon_link.
 * 
 * Since: 6.6.0
 */
void
ags_icon_link_set_link_text(AgsIconLink *icon_link,
			    gchar *link_text)
{
  if(!AGS_IS_ICON_LINK(icon_link)){
    return;
  }

  gtk_label_set_markup(icon_link->link,
		       link_text);
}

/**
 * ags_icon_link_delete_event:
 * @icon_link: the #AgsIconLink
 * 
 * Emits ::delete-event event.
 * 
 * Since: 6.6.0
 */
void
ags_icon_link_delete_event(AgsIconLink *icon_link)
{
  g_return_if_fail(AGS_IS_ICON_LINK(icon_link));
  
  g_object_ref((GObject *) icon_link);
  g_signal_emit(G_OBJECT(icon_link),
		icon_link_signals[DELETE_EVENT], 0);
  g_object_unref((GObject *) icon_link);
}

AgsIconLink*
ags_icon_link_real_copy_event(AgsIconLink *icon_link)
{
  AgsIconLink *new_icon_link;

  gchar *icon_name;
  gchar *link_text;

  icon_name = ags_icon_link_get_icon_name(icon_link);
  link_text = ags_icon_link_get_link_text(icon_link);
  
  new_icon_link = ags_icon_link_new(icon_link->action,
				    icon_name,
				    link_text);

  g_free(icon_name);
  g_free(link_text);
  
  return(new_icon_link);
}

/**
 * ags_icon_link_copy_event:
 * @icon_link: the #AgsIconLink
 * 
 * Emits ::copy-event event.
 *
 * Returns: (transfer none): the icon link
 * 
 * Since: 6.6.0
 */
AgsIconLink*
ags_icon_link_copy_event(AgsIconLink *icon_link)
{
  AgsIconLink *copy_link;
  
  g_return_val_if_fail(AGS_IS_ICON_LINK(icon_link), NULL);

  copy_link = NULL;
  
  g_object_ref((GObject *) icon_link);
  g_signal_emit(G_OBJECT(icon_link),
		icon_link_signals[COPY_EVENT], 0,
		&copy_link);
  g_object_unref((GObject *) icon_link);

  return(copy_link);
}

/**
 * ags_icon_link_clicked:
 * @icon_link: the #AgsIconLink
 * 
 * Emits ::clicked event.
 * 
 * Since: 6.6.0
 */
void
ags_icon_link_clicked(AgsIconLink *icon_link)
{
  g_return_if_fail(AGS_IS_ICON_LINK(icon_link));
  
  g_object_ref((GObject *) icon_link);
  g_signal_emit(G_OBJECT(icon_link),
		icon_link_signals[CLICKED], 0);
  g_object_unref((GObject *) icon_link);
}

void
ags_icon_link_gesture_click_pressed_callback(GtkGestureClick *event_controller,
					     gint n_press,
					     gdouble x,
					     gdouble y,
					     AgsIconLink *icon_link)
{
  //empty
}

void
ags_icon_link_gesture_click_released_callback(GtkGestureClick *event_controller,
					      gint n_press,
					      gdouble x,
					      gdouble y,
					      AgsIconLink *icon_link)
{  
  gtk_widget_grab_focus((GtkWidget *) icon_link);

  ags_icon_link_clicked(icon_link);
}

void
ags_icon_link_gesture_secondary_pressed_callback(GtkGestureClick *event_controller,
						 gint n_press,
						 gdouble x,
						 gdouble y,
						 AgsIconLink *icon_link)
{
  //empty
}

void
ags_icon_link_gesture_secondary_released_callback(GtkGestureClick *event_controller,
						  gint n_press,
						  gdouble x,
						  gdouble y,
						  AgsIconLink *icon_link)
{
  if(ags_icon_link_test_flags(icon_link, AGS_ICON_LINK_SHOW_CONTEXT_MENU)){
    gtk_popover_popup((GtkPopover *) icon_link->context_popover);
  }
}

void
ags_icon_link_enter_callback(GtkEventControllerMotion *event_controller,
			     gdouble x,
			     gdouble y,
			     AgsIconLink *icon_link)
{
  ags_icon_link_set_flags(icon_link,
			  AGS_ICON_LINK_HIGHLIGHT);

  gtk_widget_queue_draw((GtkWidget *) icon_link);
}

void
ags_icon_link_leave_callback(GtkEventControllerMotion *event_controller,
			     AgsIconLink *icon_link)
{
  ags_icon_link_unset_flags(icon_link,
			    AGS_ICON_LINK_HIGHLIGHT);

  gtk_widget_queue_draw((GtkWidget *) icon_link);
}

/**
 * ags_icon_link_new:
 * @icon_name: the icon name
 * @action: the action
 * @link_text: the link text
 *
 * Creates a new instance of #AgsIconLink.
 *
 * Returns: the new #AgsIconLink
 *
 * Since: 6.6.0
 */
AgsIconLink*
ags_icon_link_new(gchar *icon_name,
		  gchar *action,
		  gchar *link_text)
{
  AgsIconLink *icon_link;

  icon_link = (AgsIconLink *) g_object_new(AGS_TYPE_ICON_LINK,
					   "action", action,
					   NULL);
  ags_icon_link_set_icon_name(icon_link,
			      icon_name);
  ags_icon_link_set_link_text(icon_link,
			      link_text);
  
  return(icon_link);
}
