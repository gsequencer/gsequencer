#ifndef __AGS_RECALL_SHARED_H__
#define __AGS_RECALL_SHARED_H__

#include <glib.h>
#include <glib-object.h>

#include "ags_recall.h"

#define AGS_TYPE_RECALL_SHARED                (ags_recall_shared_get_type())
#define AGS_RECALL_SHARED(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_SHARED, AgsRecallShared))
#define AGS_RECALL_SHARED_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_SHARED, AgsRecallSharedClass))
#define AGS_IS_RECALL_SHARED(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RECALL_SHARED))
#define AGS_IS_RECALL_SHARED_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RECALL_SHARED))
#define AGS_RECALL_SHARED_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RECALL_SHARED, AgsRecallSharedClass))

typedef struct _AgsRecallShared AgsRecallShared;
typedef struct _AgsRecallSharedClass AgsRecallSharedClass;

typedef enum
{
  AGS_RECALL_SHARED_TEMPLATE           = 1,
  AGS_RECALL_SHARED_DUPLICATE_FOR_RUN  = 1 << 1,
}AgsRecallSharedFlags;

struct _AgsRecallShared
{
  GObject object;

  guint flags;

  guint group_id;
};

struct _AgsRecallSharedClass
{
  GObjectClass object;
};

GList* ags_recall_shared_find_type(GList *recall_shared,
				   GType type);
GList* ags_recall_shared_find_type_with_group_id(GList *recall_shared_i,
						 GType type, guint group_id);

AgsRecallShared* ags_recall_shared_new();

#endif /*__AGS_RECALL_SHARED_H__*/
