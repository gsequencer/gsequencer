#include "ags_recall_shared.h"

GType ags_recall_shared_get_type();
void ags_recall_shared_class_init(AgsRecallSharedClass *recall_shared);
void ags_recall_shared_init(AgsRecallShared *recall_shared);
void ags_recall_shared_finalize(GObject *gobject);

static gpointer ags_recall_shared_parent_class = NULL;

GType
ags_recall_shared_get_type()
{
  static GType ags_type_recall_shared = 0;

  if(!ags_type_recall_shared){
    static const GTypeInfo ags_recall_shared_info = {
      sizeof (AgsRecallSharedClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_shared_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallShared),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_shared_init,
    };

    ags_type_recall_shared = g_type_register_static(G_TYPE_OBJECT,
						    "AgsRecallShared\0",
						    &ags_recall_shared_info,
						    0);
  }

  return(ags_type_recall_shared);
}

void
ags_recall_shared_class_init(AgsRecallSharedClass *recall_shared)
{
  GObjectClass *gobject;

  ags_recall_shared_parent_class = g_type_class_peek_parent(recall_shared);

  gobject = (GObjectClass *) recall_shared;

  gobject->finalize = ags_recall_shared_finalize;
}

void
ags_recall_shared_init(AgsRecallShared *recall_shared)
{
  recall_shared->flags = 0;

  recall_shared->group_id = 0;
}

void
ags_recall_shared_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_recall_shared_parent_class)->finalize(gobject);
}

GList*
ags_recall_shared_find_type(GList *recall_shared_i,
			    GType type)
{
  AgsRecallShared *recall_shared;

  while(recall_shared_i != NULL){
    recall_shared = AGS_RECALL_SHARED(recall_shared_i->data);

    if(G_TYPE_CHECK_INSTANCE_TYPE(recall_shared, type))
      break;

    recall_shared_i = recall_shared_i->next;
  }

  return(recall_shared_i);
}

GList*
ags_recall_shared_find_type_with_group_id(GList *recall_shared_i,
					  GType type, guint group_id)
{
  AgsRecallShared *recall_shared;

  while(recall_shared_i != NULL){
    recall_shared = AGS_RECALL_SHARED(recall_shared_i->data);

    if(G_TYPE_CHECK_INSTANCE_TYPE(recall_shared, type) &&
       recall_shared->group_id == group_id)
      break;

    recall_shared_i = recall_shared_i->next;
  }

  return(recall_shared_i);
}

AgsRecallShared*
ags_recall_shared_new()
{
  AgsRecallShared *recall_shared;

  recall_shared = (AgsRecallShared *) g_object_new(AGS_TYPE_RECALL_SHARED, NULL);

  return(recall_shared);
}
