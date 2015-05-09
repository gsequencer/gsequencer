/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#ifndef __AGS_RECALL_LV2_H__
#define __AGS_RECALL_LV2_H__

#include <glib.h>
#include <glib-object.h>

#include <lv2.h>

#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_channel.h>

#define AGS_TYPE_RECALL_LV2                (ags_recall_lv2_get_type())
#define AGS_RECALL_LV2(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_LV2, AgsRecallLv2))
#define AGS_RECALL_LV2_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_LV2, AgsRecallLv2Class))
#define AGS_IS_RECALL_LV2(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RECALL_LV2))
#define AGS_IS_RECALL_LV2_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RECALL_LV2))
#define AGS_RECALL_LV2_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RECALL_LV2, AgsRecallLv2Class))

typedef struct _AgsRecallLv2 AgsRecallLv2;
typedef struct _AgsRecallLv2Class AgsRecallLv2Class;

struct _AgsRecallLv2
{
  AgsRecallChannel recall_channel;

  gchar *filename;
  gchar *uri;
  uint32_t index;

  LV2_Descriptor *plugin_descriptor;

  uint32_t *input_port;
  uint32_t input_lines;

  uint32_t *output_port;
  uint32_t output_lines;
};

struct _AgsRecallLv2Class
{
  AgsRecallChannelClass recall_channel;
};

GType ags_recall_lv2_get_type();

GList* ags_recall_lv2_load_ports(AgsRecallLv2 *recall_lv2);

void ags_recall_lv2_short_to_float(signed short *buffer,
				   float *destination,
				   guint buffer_size, guint lines);
void ags_recall_lv2_float_to_short(float *buffer,
				   signed short *destination,
				   guint buffer_size, guint lines);

GList* ags_recall_ladpsa_find(GList *recall,
			      gchar *filename, gchar *uri);

AgsRecallLv2* ags_recall_lv2_new(AgsChannel *source,
				 gchar *filename,
				 gchar *uri,
				 uint32_t index);

#endif /*__AGS_RECALL_LV2_H__*/
