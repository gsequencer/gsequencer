#include <ags/audio/task/ags_cancel_recall.h>

#include <ags/object/ags_connectable.h>

void ags_cancel_recall_class_init(AgsCancelRecallClass *cancel_recall);
void ags_cancel_recall_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_cancel_recall_init(AgsCancelRecall *cancel_recall);
void ags_cancel_recall_connect(AgsConnectable *connectable);
void ags_cancel_recall_disconnect(AgsConnectable *connectable);
void ags_cancel_recall_finalize(GObject *gobject);

void ags_cancel_recall_launch(AgsTask *task);

static gpointer ags_cancel_recall_parent_class = NULL;
static AgsConnectableInterface *ags_cancel_recall_parent_connectable_interface;

GType
ags_cancel_recall_get_type()
{
  static GType ags_type_cancel_recall = 0;

  if(!ags_type_cancel_recall){
    static const GTypeInfo ags_cancel_recall_info = {
      sizeof (AgsCancelRecallClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_cancel_recall_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCancelRecall),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_cancel_recall_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_cancel_recall_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_cancel_recall = g_type_register_static(AGS_TYPE_TASK,
						    "AgsCancelRecall\0",
						    &ags_cancel_recall_info,
						    0);
    
    g_type_add_interface_static(ags_type_cancel_recall,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_cancel_recall);
}

void
ags_cancel_recall_class_init(AgsCancelRecallClass *cancel_recall)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_cancel_recall_parent_class = g_type_class_peek_parent(cancel_recall);

  /* gobject */
  gobject = (GObjectClass *) cancel_recall;

  gobject->finalize = ags_cancel_recall_finalize;

  /* task */
  task = (AgsTaskClass *) cancel_recall;

  task->launch = ags_cancel_recall_launch;
}

void
ags_cancel_recall_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_cancel_recall_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_cancel_recall_connect;
  connectable->disconnect = ags_cancel_recall_disconnect;
}

void
ags_cancel_recall_init(AgsCancelRecall *cancel_recall)
{
  cancel_recall->recall = NULL;
  cancel_recall->audio_channel = 0;
}

void
ags_cancel_recall_connect(AgsConnectable *connectable)
{
  ags_cancel_recall_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_cancel_recall_disconnect(AgsConnectable *connectable)
{
  ags_cancel_recall_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_cancel_recall_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_cancel_recall_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_cancel_recall_launch(AgsTask *task)
{
  AgsCancelRecall *cancel_recall;

  cancel_recall = AGS_CANCEL_RECALL(task);

  /*  */
  ags_recall_cancel(cancel_recall->recall, cancel_recall->audio_channel);
}

AgsCancelRecall*
ags_cancel_recall_new(AgsRecall *recall, guint audio_channel)
{
  AgsCancelRecall *cancel_recall;

  cancel_recall = (AgsCancelRecall *) g_object_new(AGS_TYPE_CANCEL_RECALL,
						   NULL);

  cancel_recall->recall = recall;
  cancel_recall->audio_channel = audio_channel;

  return(cancel_recall);
}
