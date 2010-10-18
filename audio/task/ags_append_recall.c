#include "ags_append_recall.h"

#include "../../object/ags_connectable.h"

void ags_append_recall_class_init(AgsAppendRecallClass *append_recall);
void ags_append_recall_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_append_recall_init(AgsAppendRecall *append_recall);
void ags_append_recall_connect(AgsConnectable *connectable);
void ags_append_recall_disconnect(AgsConnectable *connectable);
void ags_append_recall_finalize(GObject *gobject);

static gpointer ags_append_recall_parent_class = NULL;
static AgsConnectableInterface *ags_append_recall_parent_connectable_interface;

GType
ags_append_recall_get_type()
{
  static GType ags_type_append_recall = 0;

  if(!ags_type_append_recall){
    static const GTypeInfo ags_append_recall_info = {
      sizeof (AgsAppendRecallClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_append_recall_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAppendRecall),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_append_recall_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_append_recall_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_append_recall = g_type_register_static(AGS_TYPE_TASK,
						  "AgsAppendRecall\0",
						  &ags_append_recall_info,
						  0);

    g_type_add_interface_static(ags_type_append_recall,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_append_recall);
}

void
ags_append_recall_class_init(AgsAppendRecallClass *append_recall)
{
  GObjectClass *gobject;

  ags_append_recall_parent_class = g_type_class_peek_parent(append_recall);

  gobject = (GObjectClass *) append_recall;

  gobject->finalize = ags_append_recall_finalize;
}

void
ags_append_recall_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_append_recall_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_append_recall_connect;
  connectable->disconnect = ags_append_recall_disconnect;
}

void
ags_append_recall_init(AgsAppendRecall *append_recall)
{
}

void
ags_append_recall_connect(AgsConnectable *connectable)
{
  ags_append_recall_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_append_recall_disconnect(AgsConnectable *connectable)
{
  ags_append_recall_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_append_recall_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_append_recall_parent_class)->finalize(gobject);

  /* empty */
}

AgsAppendRecall*
ags_append_recall_new()
{
  AgsAppendRecall *append_recall;

  append_recall = (AgsAppendRecall *) g_object_new(AGS_TYPE_APPEND_RECALL,
					       NULL);

  return(append_recall);
}
