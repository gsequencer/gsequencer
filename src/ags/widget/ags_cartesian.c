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

#include "ags_cartesian.h"

void ags_cartesian_class_init(AgsCartesianClass *cartesian);
void ags_cartesian_init(AgsCartesian *cartesian);
void ags_cartesian_show(GtkWidget *widget);

static gpointer ags_cartesian_parent_class = NULL;

GType
ags_cartesian_get_type(void)
{
  static GType ags_type_cartesian = 0;

  if(!ags_type_cartesian){
    static const GTypeInfo ags_cartesian_info = {
      sizeof(AgsCartesianClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_cartesian_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCartesian),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_cartesian_init,
    };

    ags_type_cartesian = g_type_register_static(GTK_TYPE_WIDGET,
						"AgsCartesian\0", &ags_cartesian_info,
						0);
  }

  return(ags_type_cartesian);
}

void
ags_cartesian_class_init(AgsCartesianClass *cartesian)
{
  GtkWidgetClass *widget;

  ags_cartesian_parent_class = g_type_class_peek_parent(cartesian);
}

void
ags_cartesian_init(AgsCartesian *cartesian)
{
  g_object_set(G_OBJECT(cartesian),
	       "app-paintable\0", TRUE,
	       NULL);
}

AgsCartesian*
ags_cartesian_new()
{
  AgsCartesian *cartesian;

  cartesian = (AgsCartesian *) g_object_new(AGS_TYPE_CARTESIAN,
					    NULL);
  
  return(cartesian);
}
