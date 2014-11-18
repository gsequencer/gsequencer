/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/ags_automation.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_tactable.h>
#include <ags/object/ags_portlet.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_timestamp.h>

#include <stdlib.h>
#include <errno.h>

void ags_automation_class_init(AgsAutomationClass *automation);
void ags_automation_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_portlet_interface_init(AgsPortletInterface *portlet);
void ags_automation_init(AgsAutomation *automation);
void ags_automation_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_automation_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_automation_connect(AgsConnectable *connectable);
void ags_automation_disconnect(AgsConnectable *connectable);
void ags_automation_finalize(GObject *object);

void ags_automation_set_port(AgsPortlet *portlet, AgsPort *port);
AgsPort* ags_automation_get_port(AgsPortlet *portlet);
GList* ags_automation_list_safe_properties(AgsPortlet *portlet);
void ags_automation_safe_set_property(AgsPortlet *portlet, gchar *property_name, GValue *value);
void ags_automation_safe_get_property(AgsPortlet *portlet, gchar *property_name, GValue *value);

/**
 * SECTION:ags_automation
 * @short_description: Automation class supporting selection and clipboard.
 * @title: AgsAutomation
 * @section_id:
 * @include: ags/audio/ags_automation.h
 *
 * #AgsAutomation acts as a container of #AgsNote.
 */

#define AGS_AUTOMATION_CLIPBOARD_VERSION "0.4.3\0"
#define AGS_AUTOMATION_CLIPBOARD_TYPE "AgsAutomationClipboardXml\0"
#define AGS_AUTOMATION_CLIPBOARD_FORMAT "AgsAutomationNativePiano\0"

enum{
  PROP_0,
  PROP_PORT,
  PROP_CURRENT_NOTES,
  PROP_NEXT_NOTES,
};

static gpointer ags_automation_parent_class = NULL;

GType
ags_automation_get_type()
{
  static GType ags_type_automation = 0;

  if(!ags_type_automation){
    static const GTypeInfo ags_automation_info = {
      sizeof(AgsAutomationClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_automation_class_init,
      NULL,
      NULL,
      sizeof(AgsAutomation),
      0,
      (GInstanceInitFunc) ags_automation_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_automation_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_portlet_interface_info = {
      (GInterfaceInitFunc) ags_automation_portlet_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_automation = g_type_register_static(G_TYPE_OBJECT,
						 "AgsAutomation\0",
						 &ags_automation_info,
						 0);

    g_type_add_interface_static(ags_type_automation,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_automation,
				AGS_TYPE_PORTLET,
				&ags_portlet_interface_info);
  }

  return(ags_type_automation);
}

void 
ags_automation_class_init(AgsAutomationClass *automation)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_automation_parent_class = g_type_class_peek_parent(automation);

  gobject = (GObjectClass *) automation;

  gobject->set_property = ags_automation_set_property;
  gobject->get_property = ags_automation_get_property;

  gobject->finalize = ags_automation_finalize;

  /* properties */
  /**
   * AgsAutomation:port:
   *
   * The assigned #AgsPort
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("port\0",
				   "port of automation\0",
				   "The port of automation\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT,
				  param_spec);

  /**
   * AgsAutomation:current-notes:
   *
   * Offset of current position.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_pointer("current-notes\0",
				    "current notes for offset\0",
				    "The current notes for offset\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CURRENT_NOTES,
				  param_spec);

  /**
   * AgsAutomation:next-notes:
   *
   * Offset of next position.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_pointer("next-notes\0",
				    "next notes for offset\0",
				    "The next notes for offset\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NEXT_NOTES,
				  param_spec);
}

void
ags_automation_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_automation_connect;
  connectable->disconnect = ags_automation_disconnect;
}

void
ags_automation_portlet_interface_init(AgsPortletInterface *portlet)
{
  portlet->set_port = ags_automation_set_port;
  portlet->get_port = ags_automation_get_port;
  portlet->list_safe_properties = ags_automation_list_safe_properties;
  portlet->safe_set_property = ags_automation_safe_set_property;
  portlet->safe_get_property = ags_automation_safe_get_property;
}

void
ags_automation_init(AgsAutomation *automation)
{
  automation->flags = 0;

  //TODO:JK: implement me
}

void
ags_automation_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_automation_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_automation_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsAutomation *automation;

  automation = AGS_AUTOMATION(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsAutomation *automation;

  automation = AGS_AUTOMATION(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_automation_parent_class)->finalize(gobject);
}

void
ags_automation_set_port(AgsPortlet *portlet, AgsPort *port)
{
  g_object_set(G_OBJECT(portlet),
	       "port\0", port,
	       NULL);
}

AgsPort*
ags_automation_get_port(AgsPortlet *portlet)
{
  AgsPort *port;

  g_object_get(G_OBJECT(portlet),
	       "port\0", &port,
	       NULL);

  return(port);
}

GList*
ags_automation_list_safe_properties(AgsPortlet *portlet)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_automation_safe_set_property(AgsPortlet *portlet, gchar *property_name, GValue *value)
{
  //TODO:JK: add check for safe property

  g_object_set_property(G_OBJECT(portlet),
			property_name, value);
}

void
ags_automation_safe_get_property(AgsPortlet *portlet, gchar *property_name, GValue *value)
{
  //TODO:JK: add check for safe property

  g_object_get_property(G_OBJECT(portlet),
			property_name, value);
}

GList*
ags_automation_find_near_timestamp(GList *automation, guint line,
				   GObject *timestamp)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_automation_add_acceleration(AgsAutomation *automation,
				AgsAcceleration *acceleration,
				gboolean use_selection_list)
{
  //TODO:JK: implement me

}

gboolean
ags_automation_remove_acceleration_at_position(AgsAutomation *automation,
					       guint x, guint y)
{
  //TODO:JK: implement me

}

GList*
ags_automation_get_selection(AgsAutomation *automation)
{
  //TODO:JK: implement me

  return(NULL);
}

gboolean
ags_automation_is_acceleration_selected(AgsAutomation *automation, AgsAcceleration *acceleration)
{
  //TODO:JK: implement me

  return(FALSE);
}

AgsAcceleration*
ags_automation_find_point(AgsAutomation *automation,
			  guint x, guint y,
			  gboolean use_selection_list)
{
  //TODO:JK: implement me

  return(NULL);
}

GList*
ags_automation_find_region(AgsAutomation *automation,
			   guint x0, guint y0,
			   guint x1, guint y1,
			   gboolean use_selection_list)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_automation_free_selection(AgsAutomation *automation)
{
  //TODO:JK: implement me
}

void
ags_automation_add_point_to_selection(AgsAutomation *automation,
				      guint x, guint y,
				      gboolean replace_current_selection)
{
  //TODO:JK: implement me
}

void
ags_automation_remove_point_from_selection(AgsAutomation *automation,
					   guint x, guint y)
{
  //TODO:JK: implement me
}

void
ags_automation_add_region_to_selection(AgsAutomation *automation,
				       guint x0, guint y0,
				       guint x1, guint y1,
				       gboolean replace_current_selection)
{
  //TODO:JK: implement me
}

void
ags_automation_remove_region_from_selection(AgsAutomation *automation,
					    guint x0, guint y0,
					    guint x1, guint y1)
{
  //TODO:JK: implement me
}

xmlNodePtr
ags_automation_copy_selection(AgsAutomation *automation)
{
  xmlNodePtr node;
  
  node = NULL;

  //TODO:JK: implement me

  return(node);
}

xmlNodePtr
ags_automation_cut_selection(AgsAutomation *automation)
{
  xmlNodePtr node;
  
  node = NULL;

  //TODO:JK: implement me

  return(node);
}

void
ags_automation_insert_from_clipboard(AgsAutomation *automation,
				     xmlNodePtr content,
				     gboolean reset_x_offset, guint x_offset,
				     gboolean reset_y_offset, guint y_offset)
{
  //TODO:JK: implement me
}

GList*
ags_automation_get_current(AgsAutomation *automation)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_automation_new:
 *
 * Creates a #AgsAutomation.
 *
 * Returns: a new #AgsAutomation
 *
 * Since: 0.4
 */
AgsAutomation*
ags_automation_new(guint audio_channel)
{
  AgsAutomation *automation;

  automation = (AgsAutomation *) g_object_new(AGS_TYPE_AUTOMATION, NULL);

  return(automation);
}
