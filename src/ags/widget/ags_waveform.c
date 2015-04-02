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

#include "ags_waveform.h"

void ags_waveform_class_init(AgsWaveformClass *waveform);
void ags_waveform_init(AgsWaveform *waveform);
void ags_waveform_show(GtkWidget *widget);

static gpointer ags_waveform_parent_class = NULL;

GType
ags_waveform_get_type(void)
{
  static GType ags_type_waveform = 0;

  if(!ags_type_waveform){
    static const GTypeInfo ags_waveform_info = {
      sizeof(AgsWaveformClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_waveform_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsWaveform),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_waveform_init,
    };

    ags_type_waveform = g_type_register_static(GTK_TYPE_WIDGET,
					       "AgsWaveform\0", &ags_waveform_info,
					       0);
  }

  return(ags_type_waveform);
}

void
ags_waveform_class_init(AgsWaveformClass *waveform)
{
  GtkWidgetClass *widget;

  ags_waveform_parent_class = g_type_class_peek_parent(waveform);
}

void
ags_waveform_init(AgsWaveform *waveform)
{
  g_object_set(G_OBJECT(waveform),
	       "app-paintable\0", TRUE,
	       NULL);
}

AgsWaveform*
ags_waveform_new()
{
  AgsWaveform *waveform;

  waveform = (AgsWaveform *) g_object_new(AGS_TYPE_WAVEFORM,
					  NULL);
  
  return(waveform);
}
