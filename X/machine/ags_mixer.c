#include "ags_mixer.h"
#include "ags_mixer_callbacks.h"

#include "../../audio/ags_audio.h"
#include "../../audio/ags_input.h"
#include "../../audio/ags_output.h"
#include "../../audio/ags_recall.h"

#include "../../audio/recall/ags_play_volume.h"
#include "../../audio/recall/ags_recall_volume.h"

#include "../ags_pad.h"
#include "../ags_line.h"

GType ags_mixer_get_type(void);
void ags_mixer_class_init(AgsMixerClass *mixer);
void ags_mixer_init(AgsMixer *mixer);
void ags_mixer_connect(AgsMixer *mixer);
void ags_mixer_destroy(GtkObject *object);
void ags_mixer_show(GtkWidget *widget);

void ags_mixer_set_audio_channels(AgsAudio *audio,
				  guint audio_channels, guint audio_channels_old,
				  gpointer data);
void ags_mixer_set_pads(AgsAudio *audio, GType type,
			guint pads, guint pads_old,
			gpointer data);

extern void ags_file_read_mixer(AgsFile *file, AgsMachine *machine);
extern void ags_file_write_mixer(AgsFile *file, AgsMachine *machine);

extern const char *AGS_MIX_VOLUME;

GType
ags_mixer_get_type(void)
{
  static GType mixer_type = 0;

  if (!mixer_type){
    static const GtkTypeInfo mixer_info = {
      "AgsMixer\0",
      sizeof(AgsMixer), /* base_init */
      sizeof(AgsMixerClass), /* base_finalize */
      (GtkClassInitFunc) ags_mixer_class_init,
      (GtkObjectInitFunc) ags_mixer_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    mixer_type = gtk_type_unique (AGS_TYPE_MACHINE, &mixer_info);
  }

  return (mixer_type);
}

void
ags_mixer_class_init(AgsMixerClass *mixer)
{
  GtkObjectClass *object = (GtkObjectClass *) mixer;
  GtkWidgetClass *widget = (GtkWidgetClass *) mixer;
  AgsMachineClass *machine = (AgsMachineClass *) mixer;

  //  object->destroy = ags_mixer_destroy;

  //  widget->show = ags_mixer_show;

  //  machine->read_file = ags_file_read_mixer;
  //  machine->write_file = ags_file_write_mixer;
}

void
ags_mixer_init(AgsMixer *mixer)
{
  g_signal_connect_after((GObject *) mixer, "parent_set\0",
			 G_CALLBACK(ags_mixer_parent_set_callback), (gpointer) mixer);

  mixer->pad = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  mixer->machine.input = (GtkContainer *) mixer->pad;
  gtk_container_add((GtkContainer*) (gtk_container_get_children((GtkContainer *) mixer))->data, (GtkWidget *) mixer->pad);

  mixer->machine.audio->flags |= (AGS_AUDIO_SYNC | AGS_AUDIO_ASYNC);

  //  mixer->machine.audio->audio_channels = 2;
  //  ags_audio_set_pads(mixer->machine.audio, AGS_TYPE_OUTPUT, 1);
  //  ags_audio_set_pads(mixer->machine.audio, AGS_TYPE_INPUT, 8);
}

void
ags_mixer_connect(AgsMixer *mixer)
{
  GList *list;

  ags_machine_connect((AgsMachine *) mixer);

  g_signal_connect((GObject *) mixer, "destroy\0",
		   G_CALLBACK(ags_mixer_destroy_callback), (gpointer) mixer);

  //  list = gtk_container_get_children((GtkContainer *) mixer->pad);

  //  while(list != NULL){
  //    ags_pad_connect((AgsPad *) list->data);
  //
  //    list = list->next;
  //  }

  /* AgsAudio */
  g_signal_connect(G_OBJECT(mixer->machine.audio), "set_audio_channels\0",
		   G_CALLBACK(ags_mixer_set_audio_channels), NULL);

  g_signal_connect(G_OBJECT(mixer->machine.audio), "set_pads\0",
		   G_CALLBACK(ags_mixer_set_pads), NULL);
}

void
ags_mixer_destroy(GtkObject *object)
{
}

void
ags_mixer_show(GtkWidget *widget)
{
}

void
ags_mixer_set_audio_channels(AgsAudio *audio,
			     guint audio_channels, guint audio_channels_old,
			     gpointer data)
{

  AgsMixer *mixer;
  GtkVScale *scale;
  GList *list0;
  guint i;

  mixer = AGS_MIXER(audio->machine);
  list0 = gtk_container_get_children((GtkContainer *) mixer->pad);

  if(audio_channels_old < audio_channels){
    AgsLine *line;
    AgsChannel *channel;
    AgsRecall *recall;

    channel = audio->input;

    while(list0 != NULL){
      channel = ags_channel_nth(channel, audio_channels_old);

      for(i = audio_channels_old; i < audio_channels; i++){
	line = ags_line_new(channel);
	gtk_menu_shell_insert((GtkMenuShell *) AGS_PAD(list0->data)->option->menu, (GtkWidget *) line, i);

	scale = (GtkVScale *) gtk_vscale_new_with_range(0.0, 1.0, 0.025);
	gtk_range_set_inverted((GtkRange *) scale, TRUE);
	gtk_scale_set_digits((GtkScale *) scale, 3);
	gtk_widget_set_size_request((GtkWidget *) scale, -1, 100);
	gtk_table_attach(line->table,
			 (GtkWidget *) scale,
			 0, 1,
			 1, 2,
			 GTK_EXPAND, GTK_EXPAND,
			 0, 0);

	//	recall = ags_mix_volume_new();
	//	recall->control[0] = channel;
	//	recall->control[1] = &(GTK_RANGE(scale)->adjustment->value);

	//	channel->recall = g_list_alloc();
	//	channel->recall->data = (gpointer) recall;

	gtk_option_menu_set_history(AGS_PAD(list0->data)->option, 0);

	if(GTK_WIDGET_VISIBLE((GtkWidget *) mixer)){
	  gtk_widget_show_all((GtkWidget *) line);
	  ags_line_connect(line);
	}

	channel = channel->next;
      }

      list0 = list0->next;
    }
  }else{
    GList *list1;

    while(list0 != NULL){
      list0 = g_list_nth(list0, audio_channels_old -1);

      for(i = audio_channels; i < audio_channels_old; i++){
	list1 = list0->next;

	gtk_widget_destroy((GtkWidget *) list0->data);

	list0 = list1;
      }
    }
  }
}

void
ags_mixer_set_pads(AgsAudio *audio, GType type,
		   guint pads, guint pads_old,
		   gpointer data)
{
  GtkVScale *scale;

  if(type == AGS_TYPE_INPUT){
    AgsMixer *mixer = (AgsMixer *) audio->machine;
    AgsPad *pad;

    if(pads_old < pads){
      AgsLine *line;
      AgsChannel *channel;
      AgsRecall *recall;
      GtkMenu *menu;
      guint i, j;

      channel = ags_channel_nth(mixer->machine.audio->input, pads_old);
      i = pads_old;
      goto ags_mixer_set_pads0;
  
      for(; i < pads; i++){
      ags_mixer_set_pads0:

	pad = ags_pad_new(channel);
	menu = (GtkMenu *) pad->option->menu;

	for(j = 0; j < mixer->machine.audio->audio_channels; j++){
	  line = ags_line_new(channel);
	  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) line);

	  scale = (GtkVScale *) gtk_vscale_new_with_range(0.0, 1.0, 0.025);
	  gtk_range_set_inverted((GtkRange *) scale, TRUE);
	  gtk_widget_set_size_request((GtkWidget *) scale, -1, 100);
	  gtk_table_attach(line->table,
			   (GtkWidget *) scale,
			   0, 1,
			   1, 2,
			   GTK_EXPAND, GTK_EXPAND,
			   0, 0);

	  //	  recall = ags_mix_volume_new();
	  //	  recall->control[0] = channel;
	  //	  recall->control[1] = &(GTK_RANGE(scale)->adjustment->value);

	  //	  channel->recall = g_list_alloc();
	  //	  channel->recall->data = (gpointer) recall;

	  channel = channel->next;
	}

	gtk_option_menu_set_history(pad->option, 0);

	gtk_box_pack_start((GtkBox *) mixer->pad, (GtkWidget *) pad, FALSE, FALSE, 0);

	if(GTK_WIDGET_VISIBLE((GtkWidget *) mixer)){
	  gtk_widget_show_all((GtkWidget *) pad);
	  ags_pad_connect(pad);
	}
      }
    }else{
      GList *list0, *list1;

      list0 = gtk_container_get_children((GtkContainer *) mixer->pad);
      list0 = g_list_nth(list0, pads);

      while(list0 != NULL){
	list1 = list0->next;

	gtk_widget_destroy((GtkWidget *) list0->data);

	list0 = list1;
      }
    }
  }
}

AgsMixer*
ags_mixer_new()
{
  AgsMixer *mixer;

  mixer = (AgsMixer *) g_object_new(AGS_TYPE_MIXER, NULL);

  return(mixer);
}
