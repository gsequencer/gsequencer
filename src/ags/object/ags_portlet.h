/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#ifndef __AGS_PORTLET_H__
#define __AGS_PORTLET_H__

#include <glib-object.h>

#include <ags/audio/ags_port.h>

#define AGS_TYPE_PORTLET                    (ags_portlet_get_type())
#define AGS_PORTLET(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PORTLET, AgsPortlet))
#define AGS_PORTLET_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_PORTLET, AgsPortletInterface))
#define AGS_IS_PORTLET(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PORTLET))
#define AGS_IS_PORTLET_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_PORTLET))
#define AGS_PORTLET_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_PORTLET, AgsPortletInterface))

typedef void AgsPortlet;
typedef struct _AgsPortletInterface AgsPortletInterface;

struct _AgsPortletInterface
{
  GTypeInterface interface;

  AgsPort* (*get_port)(AgsPortlet *portlet);
  void (*set_port)(AgsPortlet *portlet, AgsPort *port);

  GList* (*list_safe_properties)(AgsPortlet *portlet);

  void (*safe_get_property)(AgsPortlet *portlet, gchar *property_name, GValue *value);
  void (*safe_set_property)(AgsPortlet *portlet, gchar *property_name, GValue *value);
};

GType ags_portlet_get_type();

AgsPort* ags_portlet_get_port(AgsPortlet *portlet);
void ags_portlet_set_port(AgsPortlet *portlet, AgsPort *port);

GList* ags_portlet_list_safe_properties(AgsPortlet *portlet);

void ags_portlet_safe_get_property(AgsPortlet *portlet, gchar *property_name, GValue *value);
void ags_portlet_safe_set_property(AgsPortlet *portlet, gchar *property_name, GValue *value);

#endif /*__AGS_PORTLET_H__*/
