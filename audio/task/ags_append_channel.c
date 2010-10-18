#include "ags_append_channel.h"

#include "../../object/ags_connectable.h"

void ags_append_channel_class_init(AgsAppendChannelClass *append_channel);
void ags_append_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_append_channel_init(AgsAppendChannel *append_channel);
void ags_append_channel_connect(AgsConnectable *connectable);
void ags_append_channel_disconnect(AgsConnectable *connectable);
void ags_append_channel_finalize(GObject *gobject);

static gpointer ags_append_channel_parent_class = NULL;
static AgsConnectableInterface *ags_append_channel_parent_connectable_interface;

GType
ags_append_channel_get_type()
{
  static GType ags_type_append_channel = 0;

  if(!ags_type_append_channel){
    static const GTypeInfo ags_append_channel_info = {
      sizeof (AgsAppendChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_append_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAppendChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_append_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_append_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_append_channel = g_type_register_static(AGS_TYPE_TASK,
						   "AgsAppendChannel\0",
						   &ags_append_channel_info,
						   0);

    g_type_add_interface_static(ags_type_append_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_append_channel);
}

void
ags_append_channel_class_init(AgsAppendChannelClass *append_channel)
{
  GObjectClass *gobject;

  ags_append_channel_parent_class = g_type_class_peek_parent(append_channel);

  gobject = (GObjectClass *) append_channel;

  gobject->finalize = ags_append_channel_finalize;
}

void
ags_append_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_append_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_append_channel_connect;
  connectable->disconnect = ags_append_channel_disconnect;
}

void
ags_append_channel_init(AgsAppendChannel *append_channel)
{
}

void
ags_append_channel_connect(AgsConnectable *connectable)
{
  ags_append_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_append_channel_disconnect(AgsConnectable *connectable)
{
  ags_append_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_append_channel_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_append_channel_parent_class)->finalize(gobject);

  /* empty */
}

AgsAppendChannel*
ags_append_channel_new()
{
  AgsAppendChannel *append_channel;

  append_channel = (AgsAppendChannel *) g_object_new(AGS_TYPE_APPEND_CHANNEL,
						 NULL);

  return(append_channel);
}
