#include "ags_drum_input_pad.h"
#include "ags_drum_input_pad_callbacks.h"

#include "../ags_window.h"

#include "ags_drum.h"

#include <math.h>

GType ags_drum_input_pad_get_type();
void ags_drum_input_pad_class_init(AgsDrumInputPadClass *drum_input_pad);
void ags_drum_input_pad_init(AgsDrumInputPad *drum_input_pad);
void ags_drum_input_pad_destroy(GtkObject *object);
void ags_drum_input_pad_connect(AgsDrumInputPad *drum_input_pad);

void ags_drum_input_pad_resize_lines(AgsPad *pad, GType line_type,
				     guint audio_channels, guint audio_channels_old);

static gpointer ags_drum_input_pad_parent_class = NULL;

GType
ags_drum_input_pad_get_type()
{
  static GType ags_type_drum_input_pad = 0;

  if(!ags_type_drum_input_pad){
    static const GTypeInfo ags_drum_input_pad_info = {
      sizeof(AgsDrumInputPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_drum_input_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDrumInputPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_drum_input_pad_init,
    };

    ags_type_drum_input_pad = g_type_register_static(AGS_TYPE_PAD,
						     "AgsDrumInputPad\0", &ags_drum_input_pad_info,
						     0);
  }

  return(ags_type_drum_input_pad);
}

void
ags_drum_input_pad_class_init(AgsDrumInputPadClass *drum_input_pad)
{
  AgsPadClass *pad;

  ags_drum_input_pad_parent_class = g_type_class_peek_parent(drum_input_pad);

  pad = (AgsPadClass *) drum_input_pad;

  pad->resize_lines = ags_drum_input_pad_resize_lines;
}

void
ags_drum_input_pad_init(AgsDrumInputPad *drum_input_pad)
{
  AgsPad *pad;
  GtkHBox *hbox;

  drum_input_pad->flags = 0;

  pad = (AgsPad *) drum_input_pad;

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) pad, (GtkWidget *) hbox, TRUE, TRUE, 0);
  gtk_box_reorder_child((GtkBox *) pad, (GtkWidget *) hbox, 0);

  drum_input_pad->open = (GtkButton *) gtk_button_new();
  gtk_container_add((GtkContainer *) drum_input_pad->open, (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON));
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) drum_input_pad->open, TRUE, TRUE, 0);

  drum_input_pad->play = (GtkToggleButton *) gtk_toggle_button_new();
  gtk_container_add((GtkContainer *) drum_input_pad->play, (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON));
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) drum_input_pad->play, TRUE, TRUE, 0);

  drum_input_pad->edit = (GtkToggleButton *) gtk_toggle_button_new_with_label("edit\0");
  gtk_box_pack_start((GtkBox *) pad, (GtkWidget *) drum_input_pad->edit, FALSE, FALSE, 0);

  drum_input_pad->play_ref = 0;

  drum_input_pad->file_chooser = NULL;
}

void
ags_drum_input_pad_destroy(GtkObject *object)
{
}

void
ags_drum_input_pad_connect(AgsDrumInputPad *drum_input_pad)
{
  g_signal_connect(G_OBJECT(drum_input_pad->open), "clicked\0",
		   G_CALLBACK(ags_drum_input_pad_open_callback), (gpointer) drum_input_pad);

  g_signal_connect(G_OBJECT(drum_input_pad->play), "toggled\0",
		   G_CALLBACK(ags_drum_input_pad_play_callback), (gpointer) drum_input_pad);

  g_signal_connect(G_OBJECT(AGS_PAD(drum_input_pad)->option), "changed\0",
		   G_CALLBACK(ags_drum_input_pad_option_callback), (gpointer) drum_input_pad);

  g_signal_connect(G_OBJECT(drum_input_pad->edit), "clicked\0",
		   G_CALLBACK(ags_drum_input_pad_edit_callback), (gpointer) drum_input_pad);
}

void
ags_drum_input_pad_resize_lines(AgsPad *pad, GType line_type,
				guint audio_channels, guint audio_channels_old)
{
  AgsDrumInputPad *drum_input_pad;
  AgsDelay *delay;

  AGS_PAD_CLASS(ags_drum_input_pad_parent_class)->resize_lines(pad, line_type,
							       audio_channels, audio_channels_old);

  drum_input_pad = (AgsDrumInputPad *) pad;
  delay = AGS_DELAY(ags_recall_find_by_effect(AGS_AUDIO(pad->channel->audio)->play, NULL, (char *) g_type_name(AGS_TYPE_DELAY))->data);

  if(audio_channels > audio_channels_old){
    AgsWindow *window;
    AgsDrum *drum;
    AgsAudio *audio;
    AgsLine *line;
    GtkMenu *menu;
    AgsChannel *channel;
    GList *list_line;
    guint i;

    drum = (AgsDrum *) gtk_widget_get_ancestor((GtkWidget *) pad, AGS_TYPE_DRUM);
    audio = drum->machine.audio;
    window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) drum);

    channel = pad->channel;
    channel = ags_channel_nth(channel, audio_channels_old);

    //    delay->recall_ref += (audio_channels - audio_channels_old);

    list_line = gtk_container_get_children((GtkContainer *) pad->option->menu);
    list_line = g_list_nth(list_line, audio_channels_old);

    while(list_line != NULL){
      line = AGS_LINE(list_line->data);





      channel = channel->next;
      list_line = list_line->next;
    }


  }else{
    //    delay->recall_ref -= (audio_channels_old - audio_channels);
  }
}

AgsDrumInputPad*
ags_drum_input_pad_new(AgsChannel *channel)
{
  AgsDrumInputPad *drum_input_pad;

  drum_input_pad = (AgsDrumInputPad *) g_object_new(AGS_TYPE_DRUM_INPUT_PAD, NULL);

  drum_input_pad->pad.channel = channel;

  return(drum_input_pad);
}
