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

#include "ags_hindicator.h"

void ags_hindicator_class_init(AgsHindicatorClass *indicator);
void ags_hindicator_init(AgsHindicator *indicator);
void ags_hindicator_show(GtkWidget *widget);

static gpointer ags_hindicator_parent_class = NULL;

GType
ags_hindicator_get_type(void)
{
  static GType ags_type_hindicator = 0;

  if(!ags_type_hindicator){
    static const GTypeInfo ags_hindicator_info = {
      sizeof(AgsHindicatorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_hindicator_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsHindicator),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_hindicator_init,
    };

    ags_type_hindicator = g_type_register_static(AGS_TYPE_INDICATOR,
						 "AgsHindicator\0", &ags_hindicator_info,
						 0);
  }

  return(ags_type_hindicator);
}

void
ags_hindicator_class_init(AgsHindicatorClass *indicator)
{
  GtkWidgetClass *widget;

  ags_hindicator_parent_class = g_type_class_peek_parent(indicator);
}

void
ags_hindicator_init(AgsHindicator *indicator)
{
  g_object_set(G_OBJECT(indicator),
	       "app-paintable\0", TRUE,
	       NULL);
}

AgsHindicator*
ags_hindicator_new()
{
  AgsHindicator *indicator;

  indicator = (AgsHindicator *) g_object_new(AGS_TYPE_HINDICATOR,
					     NULL);
  
  return(indicator);
}
