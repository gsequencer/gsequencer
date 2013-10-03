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

#include "ags_vindicator.h"

void ags_vindicator_class_init(AgsVIndicatorClass *indicator);
void ags_vindicator_init(AgsVIndicator *indicator);
void ags_vindicator_show(GtkWidget *widget);

static gpointer ags_vindicator_parent_class = NULL;

GType
ags_vindicator_get_type(void)
{
  static GType ags_type_vindicator = 0;

  if(!ags_type_vindicator){
    static const GTypeInfo ags_vindicator_info = {
      sizeof(AgsVIndicatorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_vindicator_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsVIndicator),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_vindicator_init,
    };

    ags_type_vindicator = g_type_register_static(AGS_TYPE_INDICATOR,
						 "AgsVIndicator\0", &ags_vindicator_info,
						 0);
  }

  return(ags_type_vindicator);
}

void
ags_vindicator_class_init(AgsVIndicatorClass *indicator)
{
  GtkWidgetClass *widget;

  ags_vindicator_parent_class = g_type_class_peek_parent(indicator);
}

void
ags_vindicator_init(AgsVIndicator *indicator)
{
  g_object_set(G_OBJECT(indicator),
	       "app-paintable\0", TRUE,
	       NULL);
}

AgsVIndicator*
ags_vindicator_new()
{
  AgsVIndicator *indicator;

  indicator = (AgsVIndicator *) g_object_new(AGS_TYPE_VINDICATOR,
					     NULL);
  
  return(indicator);
}
