#ifndef __AGS_CHANNEL_H__
#define __AGS_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_devout.h"
#include "ags_recall_id.h"
#include "ags_recall.h"
#include "ags_recycling.h"
#include "ags_notation.h"

#include "../file/ags_file.h"

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

struct _AgsChannel
{
  GObject object;

  guint flags;

  GObject *audio;

  AgsChannel *prev;
  AgsChannel *prev_pad;
  AgsChannel *next;
  AgsChannel *next_pad;

  guint pad;
  guint audio_channel;
  guint line;

  char *note;

  AgsDevoutPlay *devout_play;

  GList *recall_id; // there may be several recall's running
  GList *recall;
  GList *play;

  AgsChannel *link;
  AgsRecycling *first_recycling;
  AgsRecycling *last_recycling;

  GList *pattern;
  AgsNotation *notation;

  GtkWidget *line_widget;
  gpointer file_data;
};

struct _AgsChannelClass
{
  GObjectClass object;

  void (*write_file)(AgsFile *file, AgsChannel *channel);

  void (*recycling_changed)(AgsChannel *channel,
			    AgsRecycling *old_start_region, AgsRecycling *old_end_region,
			    AgsRecycling *new_start_region, AgsRecycling *new_end_region);
};

GType ags_channel_get_type();

void ags_channel_connect(AgsChannel *channel);

AgsRecall* ags_channel_find_recall(AgsChannel *channel, char *effect, char *name);

AgsChannel* ags_channel_last(AgsChannel *channel);
AgsChannel* ags_channel_nth(AgsChannel *channel, guint nth);
AgsChannel* ags_channel_nth_pad(AgsChannel *channel, guint nth);

void ags_channel_set_link(AgsChannel *channel, AgsChannel *link);
void ags_channel_set_recycling(AgsChannel *channel, AgsRecycling *first_recycling, AgsRecycling *last_recycling, gboolean update, gboolean destroy_old);
void ags_channel_recycling_changed(AgsChannel *channel,
				   AgsRecycling *old_start_region, AgsRecycling *old_end_region,
				   AgsRecycling *new_start_region, AgsRecycling *new_end_region);

void ags_channel_resize_audio_signal(AgsChannel *channel, guint size);

void ags_channel_recall_id_set_stage(AgsChannel *output, guint group_id, guint stage, gboolean ommit_own_channel);
void ags_channel_play(AgsChannel *channel, AgsRecallID *recall_id, gint stage, gboolean do_recall);
void ags_channel_recursive_play(AgsChannel *channel, guint group_id, gint stage);
void ags_channel_recursive_play_init(AgsChannel *channel, gint stage,
				     gboolean arrange_group_id, gboolean duplicate_templates,
				     guint group_id, guint child_group_id);

AgsChannel* ags_channel_new();

#endif /*__AGS_CHANNEL_H__*/
