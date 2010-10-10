#ifndef __AGS_RECALL_H__
#define __AGS_RECALL_H__

#include <glib.h>
#include <glib-object.h>

#include "ags_recall_id.h"

#define AGS_TYPE_RECALL                (ags_recall_get_type())
#define AGS_RECALL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL, AgsRecall))
#define AGS_RECALL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL, AgsRecallClass))
#define AGS_IS_RECALL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RECALL))
#define AGS_IS_RECALL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RECALL))
#define AGS_RECALL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RECALL, AgsRecallClass))

typedef struct _AgsRecall AgsRecall;
typedef struct _AgsRecallClass AgsRecallClass;

typedef enum{
  AGS_RECALL_RUN_INITIALIZED    = 1,
  AGS_RECALL_TEMPLATE           = 1 << 1, // if a sequencer's AgsOutput->recall is called all AgsRecalls with this flag has to be duplicated
  AGS_RECALL_PERSISTENT         = 1 << 2,
  AGS_RECALL_DONE               = 1 << 3,
  AGS_RECALL_CANCEL             = 1 << 4,
  AGS_RECALL_REMOVE             = 1 << 5,
  AGS_RECALL_HIDE               = 1 << 6,
  AGS_RECALL_PROPAGATE_DONE     = 1 << 7, // see ags_recall_real_remove
}AgsRecallFlags;

typedef enum{
  AGS_RECALL_NOTIFY_RUN,
  AGS_RECALL_NOTIFY_SHARED_AUDIO,
  AGS_RECALL_NOTIFY_SHARED_AUDIO_RUN,
  AGS_RECALL_NOTIFY_SHARED_CHANNEL,
  AGS_RECALL_NOTIFY_CHANNEL_RUN,
}AgsRecallNotifyDependencyMode;

struct _AgsRecall
{
  GObject object;

  guint flags;

  GType recall_audio_type;
  AgsRecall *recall_audio;

  GType recall_audio_run_type;
  gpointer recall_audio_run;

  GType recall_channel_type;
  gpointer recall_channel;

  GType recall_channel_run_type;
  GList *recall_channel_run;

  char *name;
  AgsRecallID *recall_id;

  AgsRecall *parent;
  GList *child;
};

struct _AgsRecallClass
{
  GObjectClass object;

  void (*run_init_pre)(AgsRecall *recall, guint audio_channel);
  void (*run_init_inter)(AgsRecall *recall, guint audio_channel);
  void (*run_init_post)(AgsRecall *recall, guint audio_channel);

  void (*run_pre)(AgsRecall *recall, guint audio_channel);
  void (*run_inter)(AgsRecall *recall, guint audio_channel);
  void (*run_post)(AgsRecall *recall, guint audio_channel);

  void (*done)(AgsRecall *recall);
  void (*loop)(AgsRecall *recall);

  void (*cancel)(AgsRecall *recall);
  void (*remove)(AgsRecall *recall);

  AgsRecall* (*duplicate)(AgsRecall *recall, AgsRecallID *recall_id); // if a sequencer is linked with a sequencer the AgsRecall's with the flag AGS_RECALL_SOURCE must be duplicated

  void (*notify_dependency)(AgsRecall *recall, guint dependency, gboolean increase);
};

void ags_recall_run_init_pre(AgsRecall *recall, guint audio_channel);
void ags_recall_run_init_inter(AgsRecall *recall, guint audio_channel);
void ags_recall_run_init_post(AgsRecall *recall, guint audio_channel);

void ags_recall_run_pre(AgsRecall *recall, guint audio_channel);
void ags_recall_run_inter(AgsRecall *recall, guint audio_channel);
void ags_recall_run_post(AgsRecall *recall, guint audio_channel);

void ags_recall_done(AgsRecall *recall);
void ags_recall_loop(AgsRecall *recall);

void ags_recall_cancel(AgsRecall *recall);
void ags_recall_remove(AgsRecall *recall);

AgsRecall* ags_recall_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

void ags_recall_notify_dependency(AgsRecall *recall, guint dependency, gint count);

void ags_recall_add_child(AgsRecall *recall, AgsRecall *child, guint audio_channel);

void ags_recall_check_cancel(AgsRecall *recall);
void ags_recall_child_check_remove(AgsRecall *recall);

void ags_recall_set_effect(AgsRecall *recall, char *effect);
GList* ags_recall_find_by_effect(GList *list, AgsRecallID *recall_id, char *effect);

GList* ags_recall_find_type(GList *recall, GType type);
GList* ags_recall_template_find_type(GList *recall, GType type);
GList* ags_recall_find_type_with_group_id(GList *recall, GType type, guint group_id);

void ags_recall_run_init(AgsRecall *recall, guint stage);

AgsRecall* ags_recall_new();

#endif /*__AGS_RECALL_H__*/
