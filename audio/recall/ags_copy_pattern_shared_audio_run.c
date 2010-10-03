#include "ags_copy_pattern_shared_audio_run.h"

#include "../../object/ags_run_connectable.h"

GType ags_copy_pattern_shared_audio_run_get_type();
void ags_copy_pattern_shared_audio_run_class_init(AgsCopyPatternSharedAudioRunClass *copy_pattern_shared_audio_run);
void ags_copy_pattern_shared_audio_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_copy_pattern_shared_audio_run_init(AgsCopyPatternSharedAudioRun *copy_pattern_shared_audio_run);
void ags_copy_pattern_shared_audio_run_run_connect(AgsRunConnectable *run_connectable);
void ags_copy_pattern_shared_audio_run_run_disconnect(AgsRunConnectable *run_connectable);
void ags_copy_pattern_shared_audio_run_finalize(GObject *gobject);

void ags_copy_pattern_shared_audio_run_tic_callback(AgsDelay *delay, guint audio_channel,
						    AgsCopyPatternSharedAudioRun *copy_pattern_shared_audio_run);

static gpointer ags_copy_pattern_shared_audio_run_parent_class = NULL;
static AgsRunConnectableInterface *ags_copy_pattern_shared_audio_run_parent_run_connectable_interface;

GType
ags_copy_pattern_shared_audio_run_get_type()
{
  static GType ags_type_copy_pattern_shared_audio_run = 0;

  if(!ags_type_copy_pattern_shared_audio_run){
    static const GTypeInfo ags_copy_pattern_shared_audio_run_info = {
      sizeof (AgsCopyPatternSharedAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_pattern_shared_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyPatternSharedAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_pattern_shared_audio_run_init,
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_pattern_shared_audio_run_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_copy_pattern_shared_audio_run = g_type_register_static(AGS_TYPE_RECALL_SHARED_AUDIO,
								    "AgsCopyPatternSharedAudioRun\0",
								    &ags_copy_pattern_shared_audio_run_info,
								    0);

    g_type_add_interface_static(ags_type_copy_pattern_shared_audio_run,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_copy_pattern_shared_audio_run);
}

void
ags_copy_pattern_shared_audio_run_class_init(AgsCopyPatternSharedAudioRunClass *copy_pattern_shared_audio_run)
{
  GObjectClass *gobject;

  ags_copy_pattern_shared_audio_run_parent_class = g_type_class_peek_parent(copy_pattern_shared_audio_run);

  gobject = (GObjectClass *) copy_pattern_shared_audio_run;

  gobject->finalize = ags_copy_pattern_shared_audio_run_finalize;
}

void
ags_copy_pattern_shared_audio_run_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_copy_pattern_shared_audio_run_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_copy_pattern_shared_audio_run_run_connect;
  run_connectable->disconnect = ags_copy_pattern_shared_audio_run_run_disconnect;
}

void
ags_copy_pattern_shared_audio_run_init(AgsCopyPatternSharedAudioRun *copy_pattern_shared_audio_run)
{
  copy_pattern_shared_audio_run->copy_pattern_shared_audio = NULL;

  copy_pattern_shared_audio_run->recall_ref = 0;

  copy_pattern_shared_audio_run->delay = NULL;
  copy_pattern_shared_audio_run->bit = 0;
}

void
ags_copy_pattern_shared_audio_run_run_connect(AgsRunConnectable *run_connectable)
{
  AgsCopyPatternSharedAudioRun *copy_pattern_shared_audio_run;

  ags_copy_pattern_shared_audio_run_parent_run_connectable_interface->connect(run_connectable);

  /* AgsCopyPattern */
  copy_pattern_shared_audio_run = AGS_COPY_PATTERN_SHARED_AUDIO_RUN(run_connectable);

  copy_pattern_shared_audio_run->tic_handler =
    g_signal_connect_after(G_OBJECT(copy_pattern_shared_audio_run->delay), "tic\0",
			   G_CALLBACK(ags_copy_pattern_shared_audio_run_tic_callback), copy_pattern_shared_audio_run);
}

void
ags_copy_pattern_shared_audio_run_run_disconnect(AgsRunConnectable *run_connectable)
{
  AgsCopyPatternSharedAudioRun *copy_pattern_shared_audio_run;

  ags_copy_pattern_shared_audio_run_parent_run_connectable_interface->connect(run_connectable);

  /* AgsCopyPattern */
  copy_pattern_shared_audio_run = AGS_COPY_PATTERN_SHARED_AUDIO_RUN(run_connectable);

  g_signal_handler_disconnect(G_OBJECT(copy_pattern_shared_audio_run), copy_pattern_shared_audio_run->tic_handler);
}

void
ags_copy_pattern_shared_audio_run_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_copy_pattern_shared_audio_run_parent_class)->finalize(gobject);
}

void
ags_copy_pattern_shared_audio_run_tic_callback(AgsDelay *delay, guint audio_channel,
					       AgsCopyPatternSharedAudioRun *copy_pattern_shared_audio_run)
{
  AgsCopyPatternSharedAudio *copy_pattern_shared_audio;

  printf("ags_copy_pattern_shared_audio_run_tic_callback: %u\n\0", copy_pattern_shared_audio_run->bit);

  copy_pattern_shared_audio = copy_pattern_shared_audio_run->copy_pattern_shared_audio;


  if(copy_pattern_shared_audio_run->bit == copy_pattern_shared_audio->length - 1){
    if(copy_pattern_shared_audio->loop ||
       copy_pattern_shared_audio_run->recall_ref == 0)
      copy_pattern_shared_audio_run->bit = 0;
  }else{
    copy_pattern_shared_audio_run->bit++;
  }
}

AgsCopyPatternSharedAudioRun*
ags_copy_pattern_shared_audio_run_new(AgsCopyPatternSharedAudio *copy_pattern_shared_audio,
				      AgsDelay *delay, guint bit)
{
  AgsCopyPatternSharedAudioRun *copy_pattern_shared_audio_run;

  copy_pattern_shared_audio_run = (AgsCopyPatternSharedAudioRun *) g_object_new(AGS_TYPE_COPY_PATTERN_SHARED_AUDIO_RUN, NULL);

  copy_pattern_shared_audio_run->copy_pattern_shared_audio = copy_pattern_shared_audio;

  copy_pattern_shared_audio_run->delay = delay;
  copy_pattern_shared_audio_run->bit = bit;

  return(copy_pattern_shared_audio_run);
}
