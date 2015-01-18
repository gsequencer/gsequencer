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

#ifndef __AGS_RECALL_LADSPA_H__
#define __AGS_RECALL_LADSPA_H__

#include <glib.h>
#include <glib-object.h>

#include <ladspa.h>

#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_channel.h>

#define AGS_TYPE_RECALL_LADSPA                (ags_recall_ladspa_get_type())
#define AGS_RECALL_LADSPA(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_LADSPA, AgsRecallLadspa))
#define AGS_RECALL_LADSPA_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_LADSPA, AgsRecallLadspaClass))
#define AGS_IS_RECALL_LADSPA(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RECALL_LADSPA))
#define AGS_IS_RECALL_LADSPA_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RECALL_LADSPA))
#define AGS_RECALL_LADSPA_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RECALL_LADSPA, AgsRecallLadspaClass))

typedef struct _AgsRecallLadspa AgsRecallLadspa;
typedef struct _AgsRecallLadspaClass AgsRecallLadspaClass;

struct _AgsRecallLadspa
{
  AgsRecallChannel recall_channel;

  gchar *filename;
  gchar *effect;
  guint index;

  LADSPA_Descriptor *plugin_descriptor;

  unsigned long *input_port;
  unsigned long input_lines;

  unsigned long *output_port;
  unsigned long output_lines;
};

struct _AgsRecallLadspaClass
{
  AgsRecallChannelClass recall_channel;
};

GType ags_recall_ladspa_get_type();

GList* ags_recall_ladspa_load_ports(AgsRecallLadspa *recall_ladspa);

void ags_recall_ladspa_short_to_float(signed short *buffer,
				      float *destination,
				      guint buffer_size, guint lines);
void ags_recall_ladspa_float_to_short(float *buffer,
				      signed short *destination,
				      guint buffer_size, guint lines);

GList* ags_recall_ladpsa_find(GList *recall,
			      gchar *filename, gchar *effect);

AgsRecallLadspa* ags_recall_ladspa_new(AgsChannel *source,
				       gchar *filename,
				       gchar *effect,
				       guint index);

#endif /*__AGS_RECALL_LADSPA_H__*/
