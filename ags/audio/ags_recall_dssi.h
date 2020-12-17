/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_RECALL_DSSI_H__
#define __AGS_RECALL_DSSI_H__

#include <glib.h>
#include <glib-object.h>

#include <dssi.h>

#include <ags/libags.h>

#include <ags/plugin/ags_dssi_plugin.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_channel.h>

G_BEGIN_DECLS

#define AGS_TYPE_RECALL_DSSI                (ags_recall_dssi_get_type())
#define AGS_RECALL_DSSI(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_DSSI, AgsRecallDssi))
#define AGS_RECALL_DSSI_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_DSSI, AgsRecallDssiClass))
#define AGS_IS_RECALL_DSSI(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RECALL_DSSI))
#define AGS_IS_RECALL_DSSI_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RECALL_DSSI))
#define AGS_RECALL_DSSI_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RECALL_DSSI, AgsRecallDssiClass))

typedef struct _AgsRecallDssi AgsRecallDssi;
typedef struct _AgsRecallDssiClass AgsRecallDssiClass;

struct _AgsRecallDssi
{
  AgsRecallChannel recall_channel;

  guint bank;
  guint program;
  
  AgsDssiPlugin *plugin;
  DSSI_Descriptor *plugin_descriptor;

  guint *input_port;
  guint input_lines;

  guint *output_port;
  guint output_lines;
};

struct _AgsRecallDssiClass
{
  AgsRecallChannelClass recall_channel;
};

G_DEPRECATED
GType ags_recall_dssi_get_type();

G_DEPRECATED
void ags_recall_dssi_load(AgsRecallDssi *recall_dssi);
G_DEPRECATED
GList* ags_recall_dssi_load_ports(AgsRecallDssi *recall_dssi);
G_DEPRECATED
void ags_recall_dssi_load_conversion(AgsRecallDssi *recall_dssi,
				     GObject *port,
				     gpointer plugin_port);

G_DEPRECATED
GList* ags_recall_dssi_find(GList *recall,
			    gchar *filename, gchar *effect);

G_DEPRECATED
AgsRecallDssi* ags_recall_dssi_new(AgsChannel *source,
				   gchar *filename,
				   gchar *effect,
				   guint effect_index);

G_END_DECLS

#endif /*__AGS_RECALL_DSSI_H__*/
