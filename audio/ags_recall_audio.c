#include "ags_recall_audio.h"

#include "../object/ags_connectable.h"
#include "../object/ags_run_connectable.h"

void ags_recall_audio_class_init(AgsRecallAudioClass *recall_audio);
void ags_recall_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_audio_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_recall_audio_init(AgsRecallAudio *recall_audio);
void ags_recall_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_recall_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_recall_audio_connect(AgsConnectable *connectable);
void ags_recall_audio_disconnect(AgsConnectable *connectable);
void ags_recall_audio_runconnect(AgsRunConnectable *run_connectable);
void ags_recall_audio_rundisconnect(AgsRunConnectable *run_connectable);
void ags_recall_audio_finalize(GObject *gobject);

AgsRecall* ags_recall_audio_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

enum{
  PROP_0,
  PROP_AUDIO,
};

static gpointer ags_recall_audio_parent_class = NULL;
static AgsConnectableInterface* ags_recall_audio_parent_connectable_interface;
static AgsRunConnectableInterface *ags_recall_audio_parent_run_connectable_interface;

GType
ags_recall_audio_get_type()
{
  static GType ags_type_recall_audio = 0;

  if(!ags_type_recall_audio){
    static const GTypeInfo ags_recall_audio_info = {
      sizeof (AgsRecallAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_audio = g_type_register_static(AGS_TYPE_RECALL,
						   "AgsRecallAudio\0",
						   &ags_recall_audio_info,
						   0);

    g_type_add_interface_static(ags_type_recall_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
    
    g_type_add_interface_static(ags_type_recall_audio,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_recall_audio);
}

void
ags_recall_audio_class_init(AgsRecallAudioClass *recall_audio)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_audio_parent_class = g_type_class_peek_parent(recall_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_audio;

  gobject->finalize = ags_recall_audio_finalize;

  gobject->set_property = ags_recall_audio_set_property;
  gobject->get_property = ags_recall_audio_get_property;

  param_spec = g_param_spec_object("audio\0",
				   "assigned audio\0",
				   "The audio object it is assigned to\0",
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_audio;

  recall->duplicate = ags_recall_audio_duplicate;
}

void
ags_recall_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_recall_audio_connectable_parent_interface;

  ags_recall_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_audio_connect;
  connectable->disconnect = ags_recall_audio_disconnect;
}

void
ags_recall_audio_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_recall_audio_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_recall_audio_runconnect;
  run_connectable->disconnect = ags_recall_audio_rundisconnect;
}

void
ags_recall_audio_init(AgsRecallAudio *recall_audio)
{
  recall_audio->audio = NULL;
}

void
ags_recall_audio_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsRecallAudio *recall_audio;

  recall_audio = AGS_RECALL_AUDIO(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      recall_audio->audio = audio;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_audio_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsRecallAudio *recall_audio;

  recall_audio = AGS_RECALL_AUDIO(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    g_value_set_object(value, recall_audio->audio);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_audio_connect(AgsConnectable *connectable)
{
  ags_recall_audio_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_recall_audio_disconnect(AgsConnectable *connectable)
{
  ags_recall_audio_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_recall_audio_runconnect(AgsConnectable *connectable)
{
  ags_recall_audio_parent_run_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_recall_audio_rundisconnect(AgsConnectable *connectable)
{
  ags_recall_audio_parent_run_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_recall_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_recall_audio_parent_class)->finalize(gobject);
}

AgsRecall*
ags_recall_audio_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsRecallAudio *recall_audio, *copy;

  recall_audio = AGS_RECALL_AUDIO(recall);
  copy = AGS_RECALL_AUDIO(AGS_RECALL_CLASS(ags_recall_audio_parent_class)->duplicate(recall, recall_id));

  printf("ags warning - ags_recall_audio_duplicate: you shouldn't do this %s\n\0", G_OBJECT_TYPE_NAME(recall));

  return((AgsRecall *) copy);
}

AgsRecallAudio*
ags_recall_audio_new(AgsAudio *audio)
{
  AgsRecallAudio *recall_audio;

  recall_audio = (AgsRecallAudio *) g_object_new(AGS_TYPE_RECALL_AUDIO,
						 "audio\0", audio,
						 NULL);

  return(recall_audio);
}
