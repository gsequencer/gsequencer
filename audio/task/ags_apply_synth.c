#include "ags_apply_synth.h"

#include "../../object/ags_connectable.h"

void ags_apply_synth_class_init(AgsApplySynthClass *apply_synth);
void ags_apply_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_apply_synth_init(AgsApplySynth *apply_synth);
void ags_apply_synth_connect(AgsConnectable *connectable);
void ags_apply_synth_disconnect(AgsConnectable *connectable);
void ags_apply_synth_finalize(GObject *gobject);

static gpointer ags_apply_synth_parent_class = NULL;
static AgsConnectableInterface *ags_apply_synth_parent_connectable_interface;

GType
ags_apply_synth_get_type()
{
  static GType ags_type_apply_synth = 0;

  if(!ags_type_apply_synth){
    static const GTypeInfo ags_apply_synth_info = {
      sizeof (AgsApplySynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_apply_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsApplySynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_apply_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_apply_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_apply_synth = g_type_register_static(AGS_TYPE_TASK,
						  "AgsApplySynth\0",
						  &ags_apply_synth_info,
						  0);

    g_type_add_interface_static(ags_type_apply_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_apply_synth);
}

void
ags_apply_synth_class_init(AgsApplySynthClass *apply_synth)
{
  GObjectClass *gobject;

  ags_apply_synth_parent_class = g_type_class_peek_parent(apply_synth);

  gobject = (GObjectClass *) apply_synth;

  gobject->finalize = ags_apply_synth_finalize;
}

void
ags_apply_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_apply_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_apply_synth_connect;
  connectable->disconnect = ags_apply_synth_disconnect;
}

void
ags_apply_synth_init(AgsApplySynth *apply_synth)
{
}

void
ags_apply_synth_connect(AgsConnectable *connectable)
{
  ags_apply_synth_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_apply_synth_disconnect(AgsConnectable *connectable)
{
  ags_apply_synth_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_apply_synth_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_apply_synth_parent_class)->finalize(gobject);

  /* empty */
}

AgsApplySynth*
ags_apply_synth_new()
{
  AgsApplySynth *apply_synth;

  apply_synth = (AgsApplySynth *) g_object_new(AGS_TYPE_APPLY_SYNTH,
					       NULL);
  
  return(apply_synth);
}
