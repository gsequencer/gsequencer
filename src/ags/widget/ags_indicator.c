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

#include "ags_indicator.h"

void ags_indicator_class_init(AgsIndicatorClass *indicator);
void ags_indicator_init(AgsIndicator *indicator);
void ags_indicator_show(GtkWidget *widget);

static gpointer ags_indicator_parent_class = NULL;

GType
ags_indicator_get_type(void)
{
  static GType ags_type_indicator = 0;

  if(!ags_type_indicator){
    static const GTypeInfo ags_indicator_info = {
      sizeof(AgsIndicatorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_indicator_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsIndicator),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_indicator_init,
    };

    ags_type_indicator = g_type_register_static(GTK_TYPE_WIDGET,
						"AgsIndicator\0", &ags_indicator_info,
						0);
  }

  return(ags_type_indicator);
}

void
ags_indicator_class_init(AgsIndicatorClass *indicator)
{
  GtkWidgetClass *widget;

  ags_indicator_parent_class = g_type_class_peek_parent(indicator);
}

void
ags_indicator_init(AgsIndicator *indicator)
{
  g_object_set(G_OBJECT(indicator),
	       "app-paintable\0", TRUE,
	       NULL);
}

AgsIndicator*
ags_indicator_new()
{
  AgsIndicator *indicator;

  indicator = (AgsIndicator *) g_object_new(AGS_TYPE_INDICATOR,
					    NULL);
  
  return(indicator);
}
