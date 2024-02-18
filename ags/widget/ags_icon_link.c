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

gboolean ags_icon_link_gesture_click_pressed_callback(GtkGestureClick *event_controller,
						      gint n_press,
						      gdouble x,
						      gdouble y,
						      AgsIconLink *icon_link);
gboolean ags_icon_link_gesture_click_released_callback(GtkGestureClick *event_controller,
						       gint n_press,
						       gdouble x,
						       gdouble y,
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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
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
    
    g_once_init_leave(&g_define_type_id__volatile, ags_type_icon_link);
  }

  return g_define_type_id__volatile;
}

void
ags_icon_link_class_init(AgsIconLinkClass *icon_link)
{
  GObjectClass *gobject;

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
  
  /* AgsIconLinkClass */  
  icon_link->clicked = NULL;

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
  
  gtk_widget_set_can_focus((GtkWidget *) icon_link,
			   TRUE);

  gtk_orientable_set_orientation(GTK_ORIENTABLE(icon_link),
				 GTK_ORIENTATION_VERTICAL);

  icon_link->icon = gtk_image_new();
  gtk_image_set_icon_size(icon_link->icon,
			  GTK_ICON_SIZE_LARGE);
  gtk_box_append(icon_link,
		 icon_link->icon);

  icon_link->link = gtk_label_new(NULL);
  gtk_label_set_use_markup(icon_link->link,
			   TRUE);
  gtk_box_append(icon_link,
		 icon_link->link);

  event_controller = gtk_gesture_click_new();
  gtk_widget_add_controller((GtkWidget *) icon_link,
			    event_controller);

  g_signal_connect(event_controller, "pressed",
		   G_CALLBACK(ags_icon_link_gesture_click_pressed_callback), icon_link);

  g_signal_connect(event_controller, "released",
		   G_CALLBACK(ags_icon_link_gesture_click_released_callback), icon_link);
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
  /* call parent */
  G_OBJECT_CLASS(ags_icon_link_parent_class)->dispose(gobject);
}

void
ags_icon_link_finalize(GObject *gobject)
{  
  /* call parent */
  G_OBJECT_CLASS(ags_icon_link_parent_class)->finalize(gobject);
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

gboolean
ags_icon_link_gesture_click_pressed_callback(GtkGestureClick *event_controller,
					     gint n_press,
					     gdouble x,
					     gdouble y,
					     AgsIconLink *icon_link)
{
  return(FALSE);
}

gboolean
ags_icon_link_gesture_click_released_callback(GtkGestureClick *event_controller,
					      gint n_press,
					      gdouble x,
					      gdouble y,
					      AgsIconLink *icon_link)
{  
  gtk_widget_grab_focus((GtkWidget *) icon_link);

  ags_icon_link_clicked(icon_link);
  
  return(FALSE);
}

/**
 * ags_icon_link_new:
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
