/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#ifndef __AGS_CHANNEL_H__
#define __AGS_CHANNEL_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_notation.h>

#include <stdarg.h>

#define AGS_TYPE_CHANNEL                (ags_channel_get_type())
#define AGS_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CHANNEL, AgsChannel))
#define AGS_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CHANNEL, AgsChannelClass))
#define AGS_IS_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CHANNEL))
#define AGS_IS_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CHANNEL))
#define AGS_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CHANNEL, AgsChannelClass))

typedef struct _AgsChannel AgsChannel;
typedef struct _AgsChannelClass AgsChannelClass;

typedef enum{
  AGS_CHANNEL_RUNNING        = 1,
}AgsChannelFlags;

typedef enum{
  AGS_CHANNEL_RECALL_ID_RUN_STAGE,
  AGS_CHANNEL_RECALL_ID_CANCEL,
}AgsChannelRecallIDMode;

#define AGS_CHANNEL_ERROR (ags_channel_error_quark())

typedef enum{
  AGS_CHANNEL_ERROR_LOOP_IN_LINK,
}AgsChannelError;

struct _AgsChannel
{
  GObject object;

  guint flags;

  GObject *audio;
  GObject *devout;

  AgsChannel *prev;
  AgsChannel *prev_pad;
  AgsChannel *next;
  AgsChannel *next_pad;

  guint pad;
  guint audio_channel;
  guint line;

  char *note;

  gpointer devout_play;

  // GObject *recycling_container; // contains child recycling
  GList *recall_id; // there may be several recall's running
  GList *container;

  GList *recall;
  GList *play;

  AgsChannel *link;
  AgsRecycling *first_recycling;
  AgsRecycling *last_recycling;
  GObject *recycling_thread;

  GList *pattern;
  AgsNotation *notation;

  GtkWidget *line_widget;
  gpointer file_data;
};

struct _AgsChannelClass
{
  GObjectClass object;

  void (*recycling_changed)(AgsChannel *channel,
			    AgsRecycling *old_start_region, AgsRecycling *old_end_region,
			    AgsRecycling *new_start_region, AgsRecycling *new_end_region,
			    AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
			    AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region);
};

GType ags_channel_get_type();

GQuark ags_channel_error_quark();

AgsRecall* ags_channel_find_recall(AgsChannel *channel, char *effect, char *name);

AgsChannel* ags_channel_first(AgsChannel *channel);
AgsChannel* ags_channel_last(AgsChannel *channel);
AgsChannel* ags_channel_nth(AgsChannel *channel, guint nth);

AgsChannel* ags_channel_pad_first(AgsChannel *channel);
AgsChannel* ags_channel_pad_last(AgsChannel *channel);
AgsChannel* ags_channel_pad_nth(AgsChannel *channel, guint nth);

AgsChannel* ags_channel_first_with_recycling(AgsChannel *channel);
AgsChannel* ags_channel_last_with_recycling(AgsChannel *channel);

void ags_channel_set_devout(AgsChannel *channel, GObject *devout);

void ags_channel_remove_recall_id(AgsChannel *channel, AgsRecallID *recall_id);
void ags_channel_add_recall_id(AgsChannel *channel, AgsRecallID *recall_id);
void ags_channel_add_recall_container(AgsChannel *channel, GObject *recall_container);
void ags_channel_remove_recall(AgsChannel *channel, GObject *recall, gboolean play);
void ags_channel_add_recall(AgsChannel *channel, GObject *recall, gboolean play);

void ags_channel_set_link(AgsChannel *channel, AgsChannel *link,
			  GError **error);
void ags_channel_set_recycling(AgsChannel *channel, AgsRecycling *first_recycling, AgsRecycling *last_recycling, gboolean update, gboolean destroy_old);
void ags_channel_recycling_changed(AgsChannel *channel,
				   AgsRecycling *old_start_region, AgsRecycling *old_end_region,
				   AgsRecycling *new_start_region, AgsRecycling *new_end_region,
				   AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
				   AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region);

void ags_channel_safe_resize_audio_signal(AgsChannel *channel,
					  guint size);

void ags_channel_resolve_recall(AgsChannel *channel,
				AgsRecallID *recall_id);

void ags_channel_play(AgsChannel *channel,
		      AgsRecallID *recall_id, gint stage);
void ags_channel_recursive_play_threaded(AgsChannel *channel,
					 AgsRecallID *recall_id, gint stage);
void ags_channel_recursive_play(AgsChannel *channel,
				AgsRecallID *recall_id, gint stage);
void ags_channel_duplicate_recall(AgsChannel *channel,
				  AgsRecallID *recall_id);
void ags_channel_init_recall(AgsChannel *channel, gint stage,
			     AgsRecallID *recall_id);
AgsRecallID* ags_channel_recursive_play_init(AgsChannel *channel, gint stage,
					     gboolean arrange_recall_id,
					     gboolean duplicate_templates, gboolean playback, gboolean sequencer, gboolean notation,
					     gboolean resolve_dependencies,
					     AgsRecallID *recall_id);

void ags_channel_cancel(AgsChannel *channel, AgsRecallID *recall_id);
void ags_channel_tillrecycling_cancel(AgsChannel *channel, AgsRecallID *recall_id);

void ags_channel_recursive_reset_recall_ids(AgsChannel *channel, AgsChannel *link,
					    AgsChannel *old_channel_link, AgsChannel *old_link_link);

void ags_channel_recall_id_set(AgsChannel *output, AgsRecallID *recall_id, gboolean ommit_own_channel,
			       guint mode, ...);

AgsChannel* ags_channel_new(GObject *audio);

#endif /*__AGS_CHANNEL_H__*/
