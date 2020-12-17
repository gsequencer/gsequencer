/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#ifndef __AGS_RECALL_LADSPA_H__
#define __AGS_RECALL_LADSPA_H__

#include <glib.h>
#include <glib-object.h>

#include <ladspa.h>

#include <ags/libags.h>

#include <ags/plugin/ags_ladspa_plugin.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_channel.h>

G_BEGIN_DECLS

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

  AgsLadspaPlugin *plugin;
  LADSPA_Descriptor *plugin_descriptor;

  guint *input_port;
  guint input_lines;

  guint *output_port;
  guint output_lines;
};

struct _AgsRecallLadspaClass
{
  AgsRecallChannelClass recall_channel;
};

G_DEPRECATED
GType ags_recall_ladspa_get_type();

G_DEPRECATED
void ags_recall_ladspa_load(AgsRecallLadspa *recall_ladspa);
G_DEPRECATED
GList* ags_recall_ladspa_load_ports(AgsRecallLadspa *recall_ladspa);
G_DEPRECATED
void ags_recall_ladspa_load_conversion(AgsRecallLadspa *recall_ladspa,
				       GObject *port,
				       gpointer plugin_port);

G_DEPRECATED
GList* ags_recall_ladspa_find(GList *recall,
			      gchar *filename, gchar *effect);

G_DEPRECATED
AgsRecallLadspa* ags_recall_ladspa_new(AgsChannel *source,
				       gchar *filename,
				       gchar *effect,
				       guint effect_index);

G_END_DECLS

#endif /*__AGS_RECALL_LADSPA_H__*/
