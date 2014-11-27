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

#include <ags/X/editor/ags_automation_area.h>
#include <ags/X/editor/ags_automation_area_callbacks.h>

void ags_automation_area_class_init(AgsAutomationAreaClass *automation_area);
void ags_automation_area_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_area_init(AgsAutomationArea *automation_area);
void ags_automation_area_connect(AgsConnectable *connectable);
void ags_automation_area_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_automation_area
 * @short_description: area automations
 * @title: AgsAutomationArea
 * @section_id:
 * @include: ags/X/areaor/ags_automation_area.h
 *
 * The #AgsAutomationArea lets you area automations.
 */

GtkStyle *automation_area_style;

GType
ags_automation_area_get_type(void)
{
  static GType ags_type_automation_area = 0;

  if(!ags_type_automation_area){
    static const GTypeInfo ags_automation_area_info = {
      sizeof (AgsAutomationAreaClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_automation_area_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAutomationArea),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_automation_area_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_automation_area_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_automation_area = g_type_register_static(GTK_TYPE_DRAWING_AREA,
						      "AgsAutomationArea\0", &ags_automation_area_info,
						      0);
    
    g_type_add_interface_static(ags_type_automation_area,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_automation_area);
}

void
ags_automation_area_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_automation_area_connect;
  connectable->disconnect = ags_automation_area_disconnect;
}

void
ags_automation_area_class_init(AgsAutomationAreaClass *automation_area)
{
  //TODO:JK: implement me
}

void
ags_automation_area_init(AgsAutomationArea *automation_area)
{
  //TODO:JK: implement me
}

void
ags_automation_area_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_automation_area_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_automation_area_new:
 *
 * Create a new #AgsAutomationArea.
 *
 * Since: 0.4
 */
AgsAutomationArea*
ags_automation_area_new()
{
  AgsAutomationArea *automation_area;

  automation_area = (AgsAutomationArea *) g_object_new(AGS_TYPE_AUTOMATION_AREA, NULL);

  return(automation_area);
}
