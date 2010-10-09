#include "ags_recall_channel.h"

#include "../object/ags_connectable.h"
#include "../object/ags_run_connectable.h"

GType ags_recall_channel_get_type();
void ags_recall_channel_class_init(AgsRecallChannelClass *recall_channel);
void ags_recall_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_channel_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_recall_channel_init(AgsRecallChannel *recall_channel);
void ags_recall_channel_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_recall_channel_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_recall_channel_connect(AgsConnectable *connectable);
void ags_recall_channel_disconnect(AgsConnectable *connectable);
void ags_recall_channel_runconnect(AgsRunConnectable *run_connectable);
void ags_recall_channel_rundisconnect(AgsRunConnectable *run_connectable);
void ags_recall_channel_finalize(GObject *gobject);

AgsRecall* ags_recall_channel_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

enum{
  PROP_0,
  PROP_CHANNEL,
};

static gpointer ags_recall_channel_parent_class = NULL;
static AgsConnectableInterface* ags_recall_channel_parent_connectable_interface;
static AgsRunConnectableInterface *ags_recall_channel_parent_run_connectable_interface;

GType
ags_recall_channel_get_type()
{
  static GType ags_type_recall_channel = 0;

  if(!ags_type_recall_channel){
    static const GTypeInfo ags_recall_channel_info = {
      sizeof (AgsRecallChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_channel = g_type_register_static(AGS_TYPE_RECALL,
						     "AgsRecallChannel\0",
						     &ags_recall_channel_info,
						     0);

    g_type_add_interface_static(ags_type_recall_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
    
    g_type_add_interface_static(ags_type_recall_channel,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_recall_channel);
}

void
ags_recall_channel_class_init(AgsRecallChannelClass *recall_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_channel_parent_class = g_type_class_peek_parent(recall_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_channel;

  gobject->finalize = ags_recall_channel_finalize;

  gobject->set_property = ags_recall_channel_set_property;
  gobject->get_property = ags_recall_channel_get_property;

  param_spec = g_param_spec_object("channel\0",
				   "assigned channel\0",
				   "The channel object it is assigned to\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_channel;

  recall->duplicate = ags_recall_channel_duplicate;
}

void
ags_recall_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_recall_channel_connectable_parent_interface;

  ags_recall_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_channel_connect;
  connectable->disconnect = ags_recall_channel_disconnect;
}

void
ags_recall_channel_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_recall_channel_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_recall_channel_runconnect;
  run_connectable->disconnect = ags_recall_channel_rundisconnect;
}

void
ags_recall_channel_init(AgsRecallChannel *recall_channel)
{
  recall_channel->channel = NULL;
}

void
ags_recall_channel_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsRecallChannel *recall_channel;

  recall_channel = AGS_RECALL_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      recall_channel->channel = channel;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_channel_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsRecallChannel *recall_channel;

  recall_channel = AGS_RECALL_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    g_value_set_object(value, recall_channel->channel);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_channel_connect(AgsConnectable *connectable)
{
  ags_recall_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_recall_channel_disconnect(AgsConnectable *connectable)
{
  ags_recall_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_recall_channel_runconnect(AgsConnectable *connectable)
{
  ags_recall_channel_parent_run_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_recall_channel_rundisconnect(AgsConnectable *connectable)
{
  ags_recall_channel_parent_run_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_recall_channel_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_recall_channel_parent_class)->finalize(gobject);
}

AgsRecall*
ags_recall_channel_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsRecallChannel *recall_channel, *copy;

  recall_channel = AGS_RECALL_CHANNEL(recall);
  copy = AGS_RECALL_CHANNEL(AGS_RECALL_CLASS(ags_recall_channel_parent_class)->duplicate(recall, recall_id));

  printf("ags warning - ags_recall_channel_duplicate: you shouldn't do this %s\n\0", G_OBJECT_TYPE_NAME(recall));

  return((AgsRecall *) copy);
}

AgsRecallChannel*
ags_recall_channel_new(AgsChannel *channel)
{
  AgsRecallChannel *recall_channel;

  recall_channel = (AgsRecallChannel *) g_object_new(AGS_TYPE_RECALL_CHANNEL,
						     "channel\0", channel,
						     NULL);

  return(recall_channel);
}
