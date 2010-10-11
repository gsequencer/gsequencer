#include "ags_drum_input_line.h"
#include "ags_drum_input_line_callbacks.h"

#include "../../object/ags_connectable.h"

#include "../ags_window.h"

#include "ags_drum.h"

#include "../../audio/recall/ags_delay_audio_run.h"
#include "../../audio/recall/ags_play_volume.h"
#include "../../audio/recall/ags_play_channel.h"
#include "../../audio/recall/ags_copy_channel.h"
#include "../../audio/recall/ags_stream_channel.h"
#include "../../audio/recall/ags_recall_volume.h"
#include "../../audio/recall/ags_copy_pattern_audio.h"
#include "../../audio/recall/ags_copy_pattern_audio_run.h"
#include "../../audio/recall/ags_copy_pattern_channel.h"
#include "../../audio/recall/ags_copy_pattern_channel_run.h"

GType ags_drum_input_line_get_type();
void ags_drum_input_line_class_init(AgsDrumInputLineClass *drum_input_line);
void ags_drum_input_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_drum_input_line_init(AgsDrumInputLine *drum_input_line);
void ags_drum_input_line_destroy(GtkObject *object);
void ags_drum_input_line_connect(AgsConnectable *connectable);
void ags_drum_input_line_disconnect(AgsConnectable *connectable);

void ags_drum_input_line_set_channel(AgsLine *line, AgsChannel *channel);

static gpointer ags_drum_input_line_parent_class = NULL;
static AgsConnectableInterface *ags_drum_input_line_parent_connectable_interface;

GType
ags_drum_input_line_get_type()
{
  static GType ags_type_drum_input_line = 0;

  if(!ags_type_drum_input_line){
    static const GTypeInfo ags_drum_input_line_info = {
      sizeof(AgsDrumInputLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_drum_input_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDrumInputLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_drum_input_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_drum_input_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_drum_input_line = g_type_register_static(AGS_TYPE_LINE,
						      "AgsDrumInputLine\0", &ags_drum_input_line_info,
						      0);

    g_type_add_interface_static(ags_type_drum_input_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_drum_input_line);
}

void
ags_drum_input_line_class_init(AgsDrumInputLineClass *drum_input_line)
{
  AgsLineClass *line;

  ags_drum_input_line_parent_class = g_type_class_peek_parent(drum_input_line);

  /* AgsLineClass */
  line = AGS_LINE_CLASS(drum_input_line);

  line->set_channel = ags_drum_input_line_set_channel;
}

void
ags_drum_input_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_drum_input_line_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_drum_input_line_connect;
  connectable->disconnect = ags_drum_input_line_disconnect;
}

void
ags_drum_input_line_init(AgsDrumInputLine *drum_input_line)
{
  GtkVScale *scale;

  drum_input_line->flags = 0;

  scale = (GtkVScale *) gtk_vscale_new_with_range(0.0, 1.25, 0.025);
  gtk_range_set_value((GtkRange *) scale, 0.8);
  gtk_range_set_inverted((GtkRange *) scale, TRUE);
  gtk_scale_set_digits((GtkScale *) scale, 3);
  gtk_widget_set_size_request((GtkWidget *) scale, -1, 100);
  gtk_table_attach(AGS_LINE(drum_input_line)->table,
		   (GtkWidget *) scale,
		   0, 1,
		   1, 2,
		   GTK_EXPAND, GTK_EXPAND,
		   0, 0);
}

void
ags_drum_input_line_destroy(GtkObject *object)
{
}

void
ags_drum_input_line_connect(AgsConnectable *connectable)
{
  AgsDrum *drum;
  AgsDrumInputLine *drum_input_line;

  ags_drum_input_line_parent_connectable_interface->connect(connectable);

  fprintf(stdout, "ags_drum_input_line_connect\n\0");

  /* AgsDrumInputLine */
  drum_input_line = AGS_DRUM_INPUT_LINE(connectable);
  drum = AGS_DRUM(gtk_widget_get_ancestor((GtkWidget *) drum_input_line->line.pad, AGS_TYPE_DRUM));

  /* AgsAudio */
  g_signal_connect_after(G_OBJECT(drum->machine.audio), "set_pads\0",
			 G_CALLBACK(ags_drum_input_line_audio_set_pads_callback), drum_input_line);
}

void
ags_drum_input_line_disconnect(AgsConnectable *connectable)
{
  ags_drum_input_line_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_drum_input_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  AgsDrumInputLine *drum_input_line;

  AGS_LINE_CLASS(ags_drum_input_line_parent_class)->set_channel(line, channel);

  fprintf(stdout, "ags_drum_input_line_set_channel\n\0");
  drum_input_line = AGS_DRUM_INPUT_LINE(line);

  if(line->channel != NULL){
    drum_input_line->flags &= (~AGS_DRUM_INPUT_LINE_MAPPED_RECALL);
  }

  if(channel != NULL){
    channel->pattern = g_list_alloc();
    channel->pattern->data = (gpointer) ags_pattern_new();
    ags_pattern_set_dim((AgsPattern *) channel->pattern->data, 4, 12, 64);

    ags_drum_input_line_map_recall(drum_input_line, 0);
  }
}

void
ags_drum_input_line_map_recall(AgsDrumInputLine *drum_input_line,
			       guint output_pad_start)
{
  AgsDrum *drum;
  AgsLine *line;
  AgsAudio *audio;
  AgsChannel *source, *destination;
  AgsPlayVolume *play_volume;
  AgsRecallVolume *recall_volume;
  AgsPlayChannel *play_channel;
  AgsCopyPatternAudio *copy_pattern_audio;
  AgsCopyPatternAudioRun *copy_pattern_audio_run;
  AgsCopyPatternChannel *copy_pattern_channel;
  AgsCopyPatternChannelRun *copy_pattern_channel_run;
  AgsCopyChannel *copy_channel;
  AgsStreamChannel *stream_channel;
  GList *list;
  guint i;

  fprintf(stdout, "ags_drum_input_line_map_recall\n\0");

  line = AGS_LINE(drum_input_line);

  audio = AGS_AUDIO(line->channel->audio);

  drum = AGS_DRUM(audio->machine);

  source = line->channel;

  if((AGS_DRUM_INPUT_LINE_MAPPED_RECALL & (drum_input_line->flags)) == 0){
    drum_input_line->flags |= AGS_DRUM_INPUT_LINE_MAPPED_RECALL;

    /* AgsPlayChannel */
    play_channel = ags_play_channel_new(source,
					AGS_DEVOUT(audio->devout));
    
    AGS_RECALL(play_channel)->flags |= AGS_RECALL_TEMPLATE;
        
    source->play = g_list_append(source->play, (gpointer) play_channel);

    ags_connectable_connect(AGS_CONNECTABLE(play_channel));
    
    g_signal_connect((GObject *) play_channel, "done\0",
		     G_CALLBACK(ags_drum_input_line_play_channel_done), drum_input_line);
    
    g_signal_connect((GObject *) play_channel, "cancel\0",
		     G_CALLBACK(ags_drum_input_line_play_channel_cancel), drum_input_line);

    /* AgsStreamChannel */
    stream_channel = ags_stream_channel_new(source);
    AGS_RECALL(stream_channel)->flags |= AGS_RECALL_TEMPLATE;
    
    source->recall = g_list_append(source->recall, (gpointer) stream_channel);
    ags_connectable_connect(AGS_CONNECTABLE(stream_channel));
  }

  /* create recalls which depend on output */
  destination = ags_channel_nth(audio->output, audio->audio_channels * output_pad_start);

  list = ags_recall_template_find_type(audio->play, AGS_TYPE_COPY_PATTERN_AUDIO);
  copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);

  list = ags_recall_template_find_type(audio->play, AGS_TYPE_COPY_PATTERN_AUDIO_RUN);
  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(list->data);


  while(destination != NULL){
    /* AgsCopyChannel */
    g_object_ref(G_OBJECT(destination));

    copy_channel = ags_copy_channel_new(destination,
					source,
					(AgsDevout *) audio->devout);
    AGS_RECALL(copy_channel)->flags |= AGS_RECALL_TEMPLATE;
    
    source->recall = g_list_append(source->recall, (gpointer) copy_channel);
    ags_connectable_connect(AGS_CONNECTABLE(copy_channel));

    /* AgsCopyPatternChannel */
    g_object_ref(G_OBJECT(destination));

    copy_pattern_channel = ags_copy_pattern_channel_new((AgsRecallAudio *) copy_pattern_audio,
							destination,
							source, (AgsPattern *) source->pattern->data);
    AGS_RECALL(copy_pattern_channel)->flags |= AGS_RECALL_TEMPLATE;
    source->recall = g_list_append(source->recall, (gpointer) copy_pattern_channel);

    /* AgsCopyPatternChannelRun */
    copy_pattern_channel_run = ags_copy_pattern_channel_run_new((AgsRecallAudio *) copy_pattern_audio,
								(AgsRecallAudioRun *) copy_pattern_audio_run,
								(AgsRecallChannel *) copy_pattern_channel);
    
    AGS_RECALL(copy_pattern_channel_run)->flags |= AGS_RECALL_TEMPLATE;
        
    source->recall = g_list_append(source->recall, (gpointer) copy_pattern_channel_run);

    ags_connectable_connect(AGS_CONNECTABLE(copy_pattern_channel_run));
    
    g_signal_connect((GObject *) copy_pattern_channel_run, "done\0",
		     G_CALLBACK(ags_drum_input_line_copy_pattern_done), drum_input_line);
    
    g_signal_connect((GObject *) copy_pattern_channel_run, "cancel\0",
		     G_CALLBACK(ags_drum_input_line_copy_pattern_cancel), drum_input_line);
    
    g_signal_connect((GObject *) copy_pattern_channel_run, "loop\0",
		     G_CALLBACK(ags_drum_input_line_copy_pattern_loop), drum_input_line);

    destination = destination->next_pad;
  }
}

AgsDrumInputLine*
ags_drum_input_line_new(AgsChannel *channel)
{
  AgsDrumInputLine *drum_input_line;

  drum_input_line = (AgsDrumInputLine *) g_object_new(AGS_TYPE_DRUM_INPUT_LINE,
						      "channel\0", channel,
						      NULL);

  return(drum_input_line);
}
