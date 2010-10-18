#include "ags_append_audio.h"

#include "../../object/ags_connectable.h"

void ags_append_audio_class_init(AgsAppendAudioClass *append_audio);
void ags_append_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_append_audio_init(AgsAppendAudio *append_audio);
void ags_append_audio_connect(AgsConnectable *connectable);
void ags_append_audio_disconnect(AgsConnectable *connectable);
void ags_append_audio_finalize(GObject *gobject);

static gpointer ags_append_audio_parent_class = NULL;
static AgsConnectableInterface *ags_append_audio_parent_connectable_interface;

GType
ags_append_audio_get_type()
{
  static GType ags_type_append_audio = 0;

  if(!ags_type_append_audio){
    static const GTypeInfo ags_append_audio_info = {
      sizeof (AgsAppendAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_append_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAppendAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_append_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_append_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_append_audio = g_type_register_static(AGS_TYPE_TASK,
						 "AgsAppendAudio\0",
						 &ags_append_audio_info,
						 0);

    g_type_add_interface_static(ags_type_append_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_append_audio);
}

void
ags_append_audio_class_init(AgsAppendAudioClass *append_audio)
{
  GObjectClass *gobject;

  ags_append_audio_parent_class = g_type_class_peek_parent(append_audio);

  gobject = (GObjectClass *) append_audio;

  gobject->finalize = ags_append_audio_finalize;
}

void
ags_append_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_append_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_append_audio_connect;
  connectable->disconnect = ags_append_audio_disconnect;
}

void
ags_append_audio_init(AgsAppendAudio *append_audio)
{
}

void
ags_append_audio_connect(AgsConnectable *connectable)
{
  ags_append_audio_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_append_audio_disconnect(AgsConnectable *connectable)
{
  ags_append_audio_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_append_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_append_audio_parent_class)->finalize(gobject);

  /* empty */
}

AgsAppendAudio*
ags_append_audio_new()
{
  AgsAppendAudio *append_audio;

  append_audio = (AgsAppendAudio *) g_object_new(AGS_TYPE_APPEND_AUDIO,
					     NULL);

  return(append_audio);
}
