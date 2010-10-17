#include "ags_change_bpm.h"

#include "../../object/ags_connectable.h"

void ags_change_bpm_class_init(AgsChangeBpmClass *change_bpm);
void ags_change_bpm_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_change_bpm_init(AgsChangeBpm *change_bpm);
void ags_change_bpm_connect(AgsConnectable *connectable);
void ags_change_bpm_disconnect(AgsConnectable *connectable);
void ags_change_bpm_finalize(GObject *gobject);

static gpointer ags_change_bpm_parent_class = NULL;
static AgsConnectableInterface *ags_change_bpm_parent_connectable_interface;

GType
ags_change_bpm_get_type()
{
  static GType ags_type_change_bpm = 0;

  if(!ags_type_change_bpm){
    static const GTypeInfo ags_change_bpm_info = {
      sizeof (AgsChangeBpmClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_change_bpm_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsChangeBpm),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_change_bpm_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_change_bpm_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_change_bpm = g_type_register_static(AGS_TYPE_TASK,
						 "AgsChangeBpm\0",
						 &ags_change_bpm_info,
						 0);

    g_type_add_interface_static(ags_type_change_bpm,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_change_bpm);
}

void
ags_change_bpm_class_init(AgsChangeBpmClass *change_bpm)
{
  GObjectClass *gobject;

  ags_change_bpm_parent_class = g_type_class_peek_parent(change_bpm);

  gobject = (GObjectClass *) change_bpm;

  gobject->finalize = ags_change_bpm_finalize;
}

void
ags_change_bpm_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_change_bpm_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_change_bpm_connect;
  connectable->disconnect = ags_change_bpm_disconnect;
}

void
ags_change_bpm_init(AgsChangeBpm *change_bpm)
{
}

void
ags_change_bpm_connect(AgsConnectable *connectable)
{
  ags_change_bpm_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_change_bpm_disconnect(AgsConnectable *connectable)
{
  ags_change_bpm_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_change_bpm_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_change_bpm_parent_class)->finalize(gobject);

  /* empty */
}

AgsChangeBpm*
ags_change_bpm_new()
{
  AgsChangeBpm *change_bpm;

  change_bpm = (AgsChangeBpm *) g_object_new(AGS_TYPE_CHANGE_BPM,
					     NULL);

  return(change_bpm);
}
