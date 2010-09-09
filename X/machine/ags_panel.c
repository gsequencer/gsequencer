#include "ags_panel.h"
#include "ags_panel_callbacks.h"

#include "../ags_window.h"

#include "../../audio/ags_audio.h"
#include "../../audio/ags_input.h"
#include "../../audio/ags_output.h"
#include "../../audio/ags_recall.h"

#include "../../audio/recall/ags_play_channel.h"

GType ags_panel_get_type(void);
void ags_panel_class_init(AgsPanelClass *panel);
void ags_panel_init(AgsPanel *panel);
void ags_panel_connect(AgsPanel *panel);
void ags_panel_destroy(GtkObject *object);
void ags_panel_show(GtkWidget *widget);

void ags_panel_set_audio_channels(AgsAudio *audio, guint audio_channels);
void ags_panel_set_pads(AgsAudio *audio, GType type, guint pads);
void ags_panel_set_lines(AgsAudio *audio, GType type, guint lines);

extern void ags_file_read_panel(AgsFile *file, AgsMachine *machine);
extern void ags_file_write_panel(AgsFile *file, AgsMachine *machine);

extern const char *AGS_COPY_INPUT_TO_OUTPUT;

GType
ags_panel_get_type(void)
{
  static GType panel_type = 0;

  if (!panel_type){
    static const GtkTypeInfo panel_info = {
      "AgsPanel\0",
      sizeof(AgsPanel), /* base_init */
      sizeof(AgsPanelClass), /* base_finalize */
      (GtkClassInitFunc) ags_panel_class_init,
      (GtkObjectInitFunc) ags_panel_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    panel_type = gtk_type_unique (AGS_TYPE_MACHINE, &panel_info);
  }

  return (panel_type);
}

void
ags_panel_class_init(AgsPanelClass *panel)
{
  AgsMachineClass *machine = (AgsMachineClass *) panel;

  //  machine->read_file = ags_file_read_panel;
  //  machine->write_file = ags_file_write_panel;
}

void
ags_panel_init(AgsPanel *panel)
{
  g_signal_connect_after((GObject *) panel, "parent_set\0",
			 G_CALLBACK(ags_panel_parent_set_callback), (gpointer) panel);

  panel->vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add((GtkContainer*) (gtk_container_get_children((GtkContainer *) panel))->data, (GtkWidget *) panel->vbox);

  AGS_AUDIO_GET_CLASS(panel->machine.audio)->set_audio_channels = ags_panel_set_audio_channels;
  AGS_AUDIO_GET_CLASS(panel->machine.audio)->set_pads = ags_panel_set_pads;
  AGS_AUDIO_GET_CLASS(panel->machine.audio)->set_lines = ags_panel_set_lines;

  panel->machine.audio->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING | AGS_AUDIO_SYNC);

  panel->machine.audio->output_pads = 1;
  panel->machine.audio->input_pads = 1;
  AGS_AUDIO_GET_CLASS(panel->machine.audio)->set_audio_channels(panel->machine.audio, 2);
}

void
ags_panel_connect(AgsPanel *panel)
{
  ags_machine_connect((AgsMachine *) panel);

  g_signal_connect((GObject *) panel, "destroy\0",
		   G_CALLBACK(ags_panel_destroy_callback), (gpointer) panel);
}

void
ags_panel_destroy(GtkObject *object)
{
}

void
ags_panel_show(GtkWidget *widget)
{
}

void
ags_panel_set_audio_channels(AgsAudio *audio, guint audio_channels)
{
  AgsPanel *panel;
  guint old_audio_channels;

  old_audio_channels = audio->audio_channels;

  if(audio_channels == old_audio_channels)
    return;

  ags_audio_real_set_audio_channels(audio, audio_channels);

  panel = (AgsPanel *) audio->machine;

  if(old_audio_channels < audio_channels){
    AgsChannel *input, *output;
    AgsPlayChannel *play_channel;
    GtkHBox *hbox;
    guint i;

    input = ags_channel_nth(audio->input, ((old_audio_channels == 0) ? 0: old_audio_channels -1));
    output = ags_channel_nth(audio->output, ((old_audio_channels == 0) ? 0: old_audio_channels -1));

    for(i = old_audio_channels; i < audio_channels; i++){
      play_channel = ags_play_channel_new(input,
					  AGS_DEVOUT(AGS_AUDIO(input->audio)->devout));

      hbox = (GtkHBox*) gtk_hbox_new(FALSE, 0);
      gtk_box_pack_start((GtkBox *) panel->vbox, (GtkWidget *) hbox, FALSE, FALSE, 0);

      gtk_box_pack_start((GtkBox *) hbox, 
			 (GtkWidget *) gtk_label_new(g_strdup_printf("channel %d: \0", i)),
			 FALSE, FALSE, 0);

      gtk_box_pack_start((GtkBox *) hbox,
			 (GtkWidget *) gtk_check_button_new_with_label("mute\0"),
			 FALSE, FALSE, 0);

      if(GTK_WIDGET_VISIBLE((GtkWidget *) panel))
	gtk_widget_show_all((GtkWidget *) hbox);

      input = input->next;
      output = output->next;
    }
  }else{
    GList *list0, *list1;
    GList *list2, *list3;

    list0 = g_list_nth(gtk_container_get_children((GtkContainer *) panel->vbox),
		       old_audio_channels);

    while(list0 != NULL){
      list1 = list0->next;
      list2 = gtk_container_get_children((GtkContainer *) list0->data);

      list3 = list2->next;
      gtk_widget_destroy((GtkWidget *) list2->data);

      gtk_widget_destroy((GtkWidget *) list3->data);

      gtk_widget_destroy((GtkWidget *) list0->data);

      list0 = list1;
    }
  }
}

void
ags_panel_set_pads(AgsAudio *audio, GType type, guint pads)
{
  printf("AgsPanel only audio channels can be adjusted\n\0");
}

void
ags_panel_set_lines(AgsAudio *audio, GType type, guint lines)
{
  printf("AgsPanel only audio channels can be adjusted\n\0");
}

AgsPanel*
ags_panel_new()
{
  AgsPanel *panel;

  panel = (AgsPanel *) g_object_new(AGS_TYPE_PANEL, NULL);

  return(panel);
}
