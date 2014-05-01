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

#include <ags/X/machine/ags_mixer.h>
#include <ags/X/machine/ags_mixer_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall.h>

#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>

#include <ags/X/machine/ags_mixer_input_pad.h>
#include <ags/X/machine/ags_mixer_input_line.h>

void ags_mixer_class_init(AgsMixerClass *mixer);
void ags_mixer_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_mixer_init(AgsMixer *mixer);
void ags_mixer_finalize(GObject *gobject);
void ags_mixer_connect(AgsConnectable *connectable);
void ags_mixer_disconnect(AgsConnectable *connectable);
void ags_mixer_show(GtkWidget *widget);
void ags_mixer_add_default_recalls(AgsMachine *machine);

void ags_mixer_set_audio_channels(AgsAudio *audio,
				  guint audio_channels, guint audio_channels_old,
				  gpointer data);
void ags_mixer_set_pads(AgsAudio *audio, GType type,
			guint pads, guint pads_old,
			gpointer data);

static gpointer ags_mixer_parent_class = NULL;

static AgsConnectableInterface *ags_mixer_parent_connectable_interface;

extern const char *AGS_MIX_VOLUME;

GType
ags_mixer_get_type(void)
{
  static GType ags_type_mixer = 0;

  if(!ags_type_mixer){
    static const GTypeInfo ags_mixer_info = {
      sizeof(AgsMixerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_mixer_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsMixer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_mixer_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_mixer_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_mixer = g_type_register_static(AGS_TYPE_MACHINE,
					    "AgsMixer\0", &ags_mixer_info,
					    0);
    
    g_type_add_interface_static(ags_type_mixer,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_mixer);
}

void
ags_mixer_class_init(AgsMixerClass *mixer)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsMachineClass *machine;

  ags_mixer_parent_class = g_type_class_peek_parent(mixer);

  /* GObjectClass */
  gobject = (GObjectClass *) mixer;

  gobject->finalize = ags_mixer_finalize;

  /* GtkWidget */
  widget = (GtkWidgetClass *) mixer;
  //  widget->show = ags_mixer_show;

  /* AgsMachine */
  machine = (AgsMachineClass *) mixer;

  machine->add_default_recalls = ags_mixer_add_default_recalls;
  //  machine->read_file = ags_file_read_mixer;
  //  machine->write_file = ags_file_write_mixer;
}

void
ags_mixer_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_mixer_connectable_parent_interface;

  ags_mixer_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_mixer_connect;
  connectable->disconnect = ags_mixer_disconnect;
}

void
ags_mixer_init(AgsMixer *mixer)
{
  g_signal_connect_after((GObject *) mixer, "parent_set\0",
			 G_CALLBACK(ags_mixer_parent_set_callback), (gpointer) mixer);

  AGS_MACHINE(mixer)->input_pad_type = AGS_TYPE_MIXER_INPUT_PAD;
  AGS_MACHINE(mixer)->audio->flags |= (AGS_AUDIO_ASYNC);

  mixer->input_pad = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  AGS_MACHINE(mixer)->input = (GtkContainer *) mixer->input_pad;
  gtk_container_add((GtkContainer*) (gtk_container_get_children((GtkContainer *) mixer))->data, (GtkWidget *) mixer->input_pad);
}

void
ags_mixer_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_mixer_parent_class)->finalize(gobject);
}

void
ags_mixer_connect(AgsConnectable *connectable)
{
  AgsMixer *mixer;

  ags_mixer_parent_connectable_interface->connect(connectable);

  /* AgsMixer */
  mixer = AGS_MIXER(connectable);

  //  g_signal_connect((GObject *) mixer, "destroy\0",
  //		   G_CALLBACK(ags_mixer_destroy_callback), (gpointer) mixer);

  /* AgsAudio */
  g_signal_connect_after(G_OBJECT(mixer->machine.audio), "set_audio_channels\0",
			 G_CALLBACK(ags_mixer_set_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(mixer->machine.audio), "set_pads\0",
			 G_CALLBACK(ags_mixer_set_pads), NULL);
}

void
ags_mixer_disconnect(AgsConnectable *connectable)
{
  AgsMixer *mixer;

  ags_mixer_parent_connectable_interface->disconnect(connectable);

  /* AgsMixer */
  mixer = AGS_MIXER(connectable);

  //TODO:JK: implement me
}

void
ags_mixer_show(GtkWidget *widget)
{
}

void
ags_mixer_add_default_recalls(AgsMachine *machine)
{
  /* empty */
}

void
ags_mixer_set_audio_channels(AgsAudio *audio,
			     guint audio_channels, guint audio_channels_old,
			     gpointer data)
{

  AgsMixer *mixer;
  GtkVScale *scale;
  GList *list_input_pad;
  guint i;

  mixer = AGS_MIXER(audio->machine);

  if(audio_channels_old < audio_channels){
    AgsMixerInputPad *mixer_input_pad;
    AgsChannel *channel;

    /* AgsInput */
    channel = audio->input;
    list_input_pad = gtk_container_get_children((GtkContainer *) mixer->input_pad);

    for(i = 0; i < audio->input_pads; i++){
      if(audio_channels_old == 0){
      /* create AgsPad's if necessary */
	mixer_input_pad = g_object_new(AGS_TYPE_MIXER_INPUT_PAD,
				       "channel\0", channel,
				       NULL);
	gtk_box_pack_start((GtkBox *) mixer->input_pad,
			   (GtkWidget *) mixer_input_pad,
			   FALSE, FALSE,
			   0);
	ags_pad_resize_lines((AgsPad *) mixer_input_pad, AGS_TYPE_MIXER_INPUT_LINE,
			     AGS_AUDIO(channel->audio)->audio_channels, 0);

	if(GTK_WIDGET_VISIBLE((GtkWidget *) mixer)){
	  ags_connectable_connect(AGS_CONNECTABLE(mixer_input_pad));
	  gtk_widget_show_all((GtkWidget *) mixer_input_pad);
	}
      }else{
	mixer_input_pad = AGS_MIXER_INPUT_PAD(list_input_pad->data);

	ags_pad_resize_lines((AgsPad *) mixer_input_pad, AGS_TYPE_MIXER_INPUT_LINE,
			     audio_channels, audio_channels_old);
      }

      channel = channel->next_pad;

      if(audio_channels_old != 0)
	list_input_pad = list_input_pad->next;
    }
  }else{
    GList *list_input_pad_next;

    list_input_pad = gtk_container_get_children((GtkContainer *) mixer->input_pad);

    if(audio_channels == 0){
      /* AgsInput */
      while(list_input_pad != NULL){
	list_input_pad_next = list_input_pad->next;

	gtk_widget_destroy(GTK_WIDGET(list_input_pad->data));

	list_input_pad->next = list_input_pad_next;
      }
    }else{
      /* AgsInput */
      for(i = 0; list_input_pad != NULL; i++){
	ags_pad_resize_lines(AGS_PAD(list_input_pad->data), AGS_TYPE_MIXER_INPUT_PAD,
			     audio_channels, audio_channels_old);

	list_input_pad = list_input_pad->next;
      }
    }
  }
}

void
ags_mixer_set_pads(AgsAudio *audio, GType type,
		   guint pads, guint pads_old,
		   gpointer data)
{
  AgsMixer *mixer;
  AgsChannel *channel;
  GList *list, *list_next;
  guint i, j;

  mixer = (AgsMixer *) audio->machine;

  if(type == AGS_TYPE_INPUT){
    AgsMixerInputPad *mixer_input_pad;

    if(pads_old < pads){
      channel = ags_channel_nth(audio->input, pads_old * audio->audio_channels);

      for(i = pads_old; i < pads; i++){
	mixer_input_pad = ags_mixer_input_pad_new(channel);
	gtk_box_pack_start((GtkBox *) mixer->input_pad,
			   (GtkWidget *) mixer_input_pad, FALSE, FALSE, 0);
	ags_pad_resize_lines((AgsPad *) mixer_input_pad, AGS_TYPE_MIXER_INPUT_LINE,
			     AGS_AUDIO(channel->audio)->audio_channels, 0);

	if(GTK_WIDGET_VISIBLE((GtkWidget *) mixer)){
	  ags_connectable_connect(AGS_CONNECTABLE(mixer_input_pad));
	  gtk_widget_show_all((GtkWidget *) mixer_input_pad);
	}

	channel = channel->next_pad;
      }

    }else{
      /* destroy AgsPad's */
      list = gtk_container_get_children((GtkContainer *) mixer->input_pad);
      list = g_list_nth(list, pads);

      while(list != NULL){
	list_next = list->next;

	gtk_widget_destroy((GtkWidget *) list->data);

	list = list_next;
      }
    }
  }
}

AgsMixer*
ags_mixer_new(GObject *devout)
{
  AgsMixer *mixer;

  mixer = (AgsMixer *) g_object_new(AGS_TYPE_MIXER,
				    NULL);

  g_object_set(G_OBJECT(AGS_MACHINE(mixer)->audio),
	       "devout\0", devout,
	       NULL);

  return(mixer);
}
