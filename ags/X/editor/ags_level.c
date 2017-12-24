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

#include <ags/X/editor/ags_level.h>
#include <ags/X/editor/ags_level_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <math.h>

void ags_level_class_init(AgsLevelClass *level);
void ags_level_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_level_init(AgsLevel *level);
void ags_level_connect(AgsConnectable *connectable);
void ags_level_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_level
 * @short_description: volume level widget
 * @title: AgsLevel
 * @section_id:
 * @include: ags/X/editor/ags_level.h
 *
 * The #AgsLevel draws you a volume level.
 */

GtkStyle *level_style = NULL;

GType
ags_level_get_type(void)
{
  static GType ags_type_level = 0;

  if (!ags_type_level){
    static const GTypeInfo ags_level_info = {
      sizeof (AgsLevelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_level_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLevel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_level_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_level_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_level = g_type_register_static(GTK_TYPE_DRAWING_AREA,
					    "AgsLevel", &ags_level_info,
					    0);
    
    g_type_add_interface_static(ags_type_level,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_level);
}

void
ags_level_class_init(AgsLevelClass *level)
{
}

void
ags_level_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_level_connect;
  connectable->disconnect = ags_level_disconnect;
}

void
ags_level_init(AgsLevel *level)
{
  if(level_style == NULL){
    level_style = gtk_style_copy(gtk_widget_get_style(level));
  }

  gtk_widget_set_style((GtkWidget *) level,
		       level_style);
  gtk_widget_set_size_request((GtkWidget *) level,
			      60, -1);
  gtk_widget_set_events(GTK_WIDGET(level), GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK);
}

void
ags_level_connect(AgsConnectable *connectable)
{
  AgsLevel *level;

  level = AGS_LEVEL(connectable);

  g_signal_connect((GObject *) level, "expose_event",
  		   G_CALLBACK(ags_level_expose_event), (gpointer) level);

  g_signal_connect((GObject *) level, "configure_event",
  		   G_CALLBACK(ags_level_configure_event), (gpointer) level);
}

void
ags_level_disconnect(AgsConnectable *connectable)
{
  AgsLevel *level;

  level = AGS_LEVEL(connectable);

  //TODO:JK: implement me
}

void
ags_level_paint(AgsLevel *level,
		cairo_t *cr)
{
  if(!AGS_IS_LEVEL(level) ||
     cr == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_level_new:
 *
 * Create a new #AgsLevel.
 *
 * Returns: a new #AgsLevel
 *
 * Since: 1.2.0
 */
AgsLevel*
ags_level_new()
{
  AgsLevel *level;

  level = (AgsLevel *) g_object_new(AGS_TYPE_LEVEL, NULL);

  return(level);
}
