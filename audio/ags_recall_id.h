#ifndef __AGS_RECALL_ID_H__
#define __AGS_RECALL_ID_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recycling.h>

#define AGS_TYPE_RECALL_ID                (ags_recall_id_get_type ())
#define AGS_RECALL_ID(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), AGS_TYPE_RECALL_ID, AgsRecallID))
#define AGS_RECALL_ID_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST ((class), AGS_TYPE_RECALL_ID, AgsRecallIDClass))
#define AGS_IS_RECALL_ID(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECALL_ID))
#define AGS_IS_RECALL_ID_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECALL_ID))
#define AGS_RECALL_ID_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECALL_ID, AgsRecallIDClass))

typedef struct _AgsRecallID AgsRecallID;
typedef struct _AgsRecallIDClass AgsRecallIDClass;

typedef enum{
  AGS_RECALL_ID_PRE_SYNC_ASYNC_DONE     = 1,
  AGS_RECALL_ID_INTER_SYNC_ASYNC_DONE   = 1 << 1,
  AGS_RECALL_ID_POST_SYNC_ASYNC_DONE    = 1 << 2,
  AGS_RECALL_ID_HIGHER_LEVEL_IS_RECALL  = 1 << 3,
}AgsRecallIDFlags;

struct _AgsRecallID
{
  GObject object;

  guint flags;

  guint parent_group_id;
  guint group_id;
  guint child_group_id;

  AgsRecycling *first_recycling; // the AgsRecycling that indicates the beginning for affecting AgsRecalls, NULL means AgsChannel's first_recycling
  AgsRecycling *last_recycling; // the AgsRecycling that indicates the end for affecting AgsRecalls, NULL means AgsChannel's last_recycling
};

struct _AgsRecallIDClass
{
  GObjectClass object;
};

GType ags_recall_id_get_type(void);

void ags_recall_id_connect(AgsRecallID *recall_id);

guint ags_recall_id_generate_group_id();

gboolean ags_recall_id_get_run_stage(AgsRecallID *id, gint stage);

void ags_recall_id_set_stage(AgsRecallID *recall_id, gint stage);
void ags_recall_id_unset_stage(AgsRecallID *recall_id, gint stage);

GList* ags_recall_id_add(GList *recall_id_list,
			 guint parent_group_id, guint group_id, guint child_group_id,
			 AgsRecycling *first_recycling, AgsRecycling *last_recycling,
			 gboolean higher_level_is_recall);
AgsRecallID* ags_recall_id_find_group_id(GList *recall_id_list, guint group_id);
AgsRecallID* ags_recall_id_find_parent_group_id(GList *recall_id_list, guint parent_group_id);

AgsRecallID* ags_recall_id_new();

#endif /*__AGS_RECALL_ID_H__*/
