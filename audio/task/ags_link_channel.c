#include "ags_link_channel.h"

#include "../../object/ags_connectable.h"

void ags_link_channel_class_init(AgsLinkChannelClass *link_channel);
void ags_link_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_link_channel_init(AgsLinkChannel *link_channel);
void ags_link_channel_connect(AgsConnectable *connectable);
void ags_link_channel_disconnect(AgsConnectable *connectable);
void ags_link_channel_finalize(GObject *gobject);

static gpointer ags_link_channel_parent_class = NULL;
static AgsConnectableInterface *ags_link_channel_parent_connectable_interface;

GType
ags_link_channel_get_type()
{
  static GType ags_type_link_channel = 0;

  if(!ags_type_link_channel){
    static const GTypeInfo ags_link_channel_info = {
      sizeof (AgsLinkChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_link_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLinkChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_link_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_link_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_link_channel = g_type_register_static(AGS_TYPE_TASK,
						   "AgsLinkChannel\0",
						   &ags_link_channel_info,
						   0);

    g_type_add_interface_static(ags_type_link_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_link_channel);
}

void
ags_link_channel_class_init(AgsLinkChannelClass *link_channel)
{
  GObjectClass *gobject;

  ags_link_channel_parent_class = g_type_class_peek_parent(link_channel);

  gobject = (GObjectClass *) link_channel;

  gobject->finalize = ags_link_channel_finalize;
}

void
ags_link_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_link_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_link_channel_connect;
  connectable->disconnect = ags_link_channel_disconnect;
}

void
ags_link_channel_init(AgsLinkChannel *link_channel)
{
}

void
ags_link_channel_connect(AgsConnectable *connectable)
{
  ags_link_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_link_channel_disconnect(AgsConnectable *connectable)
{
  ags_link_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_link_channel_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_link_channel_parent_class)->finalize(gobject);

  /* empty */
}

AgsLinkChannel*
ags_link_channel_new()
{
  AgsLinkChannel *link_channel;

  link_channel = (AgsLinkChannel *) g_object_new(AGS_TYPE_LINK_CHANNEL,
						 NULL);

  return(link_channel);
}
