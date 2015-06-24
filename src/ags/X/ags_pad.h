/* This file is part of GSequencer.
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

#ifndef __AGS_PAD_H__
#define __AGS_PAD_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>

#include <ags/widget/ags_expander_set.h>

#include <ags/X/ags_line.h>

#define AGS_TYPE_PAD                (ags_pad_get_type())
#define AGS_PAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PAD, AgsPad))
#define AGS_PAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PAD, AgsPadClass))
#define AGS_IS_PAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PAD))
#define AGS_IS_PAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PAD))
#define AGS_PAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PAD, AgsPadClass))

#define AGS_PAD_DEFAULT_VERSION "0.4.2\0"
#define AGS_PAD_DEFAULT_BUILD_ID "CEST 02-10-2014 19:36\0"

typedef struct _AgsPad AgsPad;
typedef struct _AgsPadClass AgsPadClass;

typedef enum{
  AGS_PAD_CONNECTED           = 1,
  AGS_PAD_SHOW_GROUPING       = 1 << 1,
  AGS_PAD_GROUP_ALL           = 1 << 2,
  AGS_PAD_GROUP_LINE          = 1 << 3,
  AGS_PAD_MAPPED_RECALL       = 1 << 4,
  AGS_PAD_PREMAPPED_RECALL    = 1 << 5,
}AgsPadFlags;

struct _AgsPad
{
  GtkVBox vbox;

  guint flags;

  gchar *name;

  gchar *version;
  gchar *build_id;

  AgsChannel *channel;

  guint cols;
  AgsExpanderSet *expander_set;

  GtkToggleButton *group;
  GtkToggleButton *mute;
  GtkToggleButton *solo;

  GtkToggleButton *play;
};

struct _AgsPadClass
{
  GtkVBoxClass vbox;

  void (*set_channel)(AgsPad *pad, AgsChannel *channel);

  void (*resize_lines)(AgsPad *pad, GType line_type,
		       guint audio_channels, guint audio_channels_old);
  void (*map_recall)(AgsPad *pad,
		     guint output_pad_start);
  GList* (*find_port)(AgsPad *pad);
};

GType ags_pad_get_type(void);

void ags_pad_set_channel(AgsPad *pad, AgsChannel *channel);

void ags_pad_resize_lines(AgsPad *pad, GType line_type,
			  guint audio_channels, guint audio_channels_old);

void ags_pad_map_recall(AgsPad *pad,
			guint output_pad_start);

GList* ags_pad_find_port(AgsPad *pad);

AgsPad* ags_pad_new(AgsChannel *channel);

#endif /*__AGS_PAD_H__*/
