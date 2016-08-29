/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_channel.h>

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

  gchar *filename;
  gchar *effect;
  unsigned long index;

  unsigned long bank;
  unsigned long program;
  
  DSSI_Descriptor *plugin_descriptor;

  unsigned long *input_port;
  unsigned long input_lines;

  unsigned long *output_port;
  unsigned long output_lines;
};

struct _AgsRecallDssiClass
{
  AgsRecallChannelClass recall_channel;
};

GType ags_recall_dssi_get_type();

GList* ags_recall_dssi_load_ports(AgsRecallDssi *recall_dssi);
void ags_recall_dssi_load_conversion(AgsRecallDssi *recall_dssi,
				     GObject *port,
				     gpointer port_descriptor);

GList* ags_recall_dssi_find(GList *recall,
			    gchar *filename, gchar *effect);

AgsRecallDssi* ags_recall_dssi_new(AgsChannel *source,
				   gchar *filename,
				   gchar *effect,
				   unsigned long index);

#endif /*__AGS_RECALL_DSSI_H__*/
