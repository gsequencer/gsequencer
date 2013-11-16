/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/machine/ags_panel.h>
#include <ags/X/machine/ags_panel_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_channel_run_master.h>

#include <ags/X/ags_window.h>

void ags_panel_class_init(AgsPanelClass *panel);
void ags_panel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_panel_init(AgsPanel *panel);
static void ags_panel_finalize(GObject *gobject);
void ags_panel_connect(AgsConnectable *connectable);
void ags_panel_disconnect(AgsConnectable *connectable);
void ags_panel_show(GtkWidget *widget);

void ags_panel_set_audio_channels(AgsAudio *audio,
				  guint audio_channels, guint audio_channels_old,
				  gpointer data);
void ags_panel_set_pads(AgsAudio *audio, GType type,
			guint pads, guint pads_old,
			gpointer data);

static gpointer ags_panel_parent_class = NULL;
static AgsConnectableInterface *ags_panel_parent_connectable_interface;

extern const char *AGS_COPY_INPUT_TO_OUTPUT;

GType
ags_panel_get_type(void)
{
  static GType ags_type_panel = 0;

  if(!ags_type_panel){
    static const GTypeInfo ags_panel_info = {
      sizeof(AgsPanelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_panel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPanel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_panel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_panel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_panel = g_type_register_static(AGS_TYPE_MACHINE,
					    "AgsPanel\0", &ags_panel_info,
					    0);
    
    g_type_add_interface_static(ags_type_panel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_panel);
}

void
ags_panel_class_init(AgsPanelClass *panel)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsMachineClass *machine = (AgsMachineClass *) panel;

  ags_panel_parent_class = g_type_class_peek_parent(panel);

  /* GtkObjectClass */
  gobject = (GObjectClass *) panel;

  gobject->finalize = ags_panel_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) panel;

  widget->show = ags_panel_show;

  //  machine->read_file = ags_file_read_panel;
  //  machine->write_file = ags_file_write_panel;

}

void
ags_panel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_panel_connectable_parent_interface;

  ags_panel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_panel_connect;
  connectable->disconnect = ags_panel_disconnect;
}

void
ags_panel_init(AgsPanel *panel)
{
  g_signal_connect_after((GObject *) panel, "parent_set\0",
			 G_CALLBACK(ags_panel_parent_set_callback), (gpointer) panel);

  panel->vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add((GtkContainer*) (gtk_container_get_children((GtkContainer *) panel))->data, (GtkWidget *) panel->vbox);


  panel->machine.audio->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING | AGS_AUDIO_SYNC);
}

static void
ags_panel_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_panel_parent_class)->finalize(gobject);
}

void
ags_panel_connect(AgsConnectable *connectable)
{
  AgsPanel *panel;

  ags_panel_parent_connectable_interface->connect(connectable);

  /* AgsPanel */
  panel = AGS_PANEL(connectable);

  /* AgsAudio */
  g_signal_connect_after(G_OBJECT(panel->machine.audio), "set_audio_channels\0",
			 G_CALLBACK(ags_panel_set_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(panel->machine.audio), "set_pads\0",
			 G_CALLBACK(ags_panel_set_pads), NULL);
}

void
ags_panel_disconnect(AgsConnectable *connectable)
{
  AgsPanel *panel;

  ags_panel_parent_connectable_interface->disconnect(connectable);

  /* AgsPanel */
  panel = AGS_PANEL(connectable);

  //TODO:JK: implement me
}

void
ags_panel_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_panel_parent_class)->show(widget);
}

void
ags_panel_set_audio_channels(AgsAudio *audio,
			     guint audio_channels, guint audio_channels_old,
			     gpointer data)
{
  AgsPanel *panel;

  panel = (AgsPanel *) audio->machine;

  if(audio_channels_old < audio_channels){
    AgsChannel *input, *output;
    AgsPlayChannel *play_channel;
    AgsPlayChannelRunMaster *play_channel_run;
    GtkHBox *hbox;
    guint i;

    input = ags_channel_nth(audio->input, ((audio_channels_old == 0) ? 0: audio_channels_old -1));
    output = ags_channel_nth(audio->output, ((audio_channels_old == 0) ? 0: audio_channels_old -1));

    for(i = audio_channels_old; i < audio_channels; i++){

      /* ags-play */
      ags_recall_factory_create(audio,
				"ags-play-master\0",
				0, audio_channels,
				input->pad, input->pad + 1,
				FALSE,
				FALSE);

      /* GtkWidgets */
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
		       audio_channels_old);

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
ags_panel_set_pads(AgsAudio *audio, GType type,
		   guint pads, guint pads_old,
		   gpointer data)
{
  printf("AgsPanel only audio channels can be adjusted\n\0");
}

AgsPanel*
ags_panel_new(GObject *devout)
{
  AgsPanel *panel;
  GValue value = {0,};

  panel = (AgsPanel *) g_object_new(AGS_TYPE_PANEL,
				    NULL);

  if(devout != NULL){
    g_value_init(&value, G_TYPE_OBJECT);
    g_value_set_object(&value, devout);
    g_object_set_property(G_OBJECT(AGS_MACHINE(panel)->audio),
			  "devout\0", &value);
    g_value_unset(&value);
  }

  return(panel);
}
