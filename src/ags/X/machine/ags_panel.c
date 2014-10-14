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

#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_channel_run_master.h>

#include <ags/X/ags_window.h>

#include <ags/X/machine/ags_panel_input_pad.h>
#include <ags/X/machine/ags_panel_input_line.h>

void ags_panel_class_init(AgsPanelClass *panel);
void ags_panel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_panel_init(AgsPanel *panel);
static void ags_panel_finalize(GObject *gobject);
void ags_panel_connect(AgsConnectable *connectable);
void ags_panel_disconnect(AgsConnectable *connectable);
void ags_panel_show(GtkWidget *widget);
void ags_panel_add_default_recalls(AgsMachine *machine);

void ags_file_read_panel(AgsFile *file, xmlNode *node, AgsMachine *panel);
xmlNode* ags_file_write_panel(AgsFile *file, xmlNode *parent, AgsMachine *panel);

void ags_panel_set_audio_channels(AgsAudio *audio,
				  guint audio_channels, guint audio_channels_old,
				  gpointer data);
void ags_panel_set_pads(AgsAudio *audio, GType type,
			guint pads, guint pads_old,
			gpointer data);

/**
 * SECTION:ags_panel
 * @short_description: panel
 * @title: AgsPanel
 * @section_id:
 * @include: ags/X/machine/ags_panel.h
 *
 * The #AgsPanel is a composite widget to act as panel.
 */

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
  AgsMachineClass *machine;

  ags_panel_parent_class = g_type_class_peek_parent(panel);

  /* GtkObjectClass */
  gobject = (GObjectClass *) panel;

  gobject->finalize = ags_panel_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) panel;

  widget->show = ags_panel_show;

  /* AgsMachine */
  machine = (AgsMachineClass *) panel;

  machine->add_default_recalls = ags_panel_add_default_recalls;
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

  AGS_MACHINE(panel)->audio->flags |= (AGS_AUDIO_SYNC);
  AGS_MACHINE(panel)->input_pad_type = AGS_TYPE_PANEL_INPUT_PAD;

  panel->vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add((GtkContainer*) (gtk_bin_get_child((GtkContainer *) panel)), (GtkWidget *) panel->vbox);

  AGS_MACHINE(panel)->output = (GtkContainer *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) panel->vbox, (GtkWidget *) AGS_MACHINE(panel)->output, FALSE, FALSE, 0);

  AGS_MACHINE(panel)->input = (GtkContainer *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) panel->vbox, (GtkWidget *) AGS_MACHINE(panel)->input, FALSE, FALSE, 0);
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
ags_panel_add_default_recalls(AgsMachine *machine)
{
  //empty
}

void
ags_panel_set_audio_channels(AgsAudio *audio,
			     guint audio_channels, guint audio_channels_old,
			     gpointer data)
{
  AgsPanel *panel;
  GList *list_input_pad, *list_input_pad_start;
  guint i;

  panel = (AgsPanel *) audio->machine;

  list_input_pad_start = 
    list_input_pad = gtk_container_get_children((GtkContainer *) AGS_MACHINE(panel)->input);
    
  if(audio_channels > audio_channels_old){
    AgsPanelInputPad *panel_input_pad;
    AgsChannel *channel;

    /* AgsInput */
    channel = audio->input;

    for(i = 0; i < audio->input_pads; i++){
      if(audio_channels_old == 0){
	/* create AgsPad's if necessary */
	panel_input_pad = ags_panel_input_pad_new(channel);
	gtk_box_pack_start((GtkBox *) AGS_MACHINE(panel)->input,
			   (GtkWidget *) panel_input_pad,
			   FALSE, FALSE,
			   0);
	ags_pad_resize_lines((AgsPad *) panel_input_pad, AGS_TYPE_PANEL_INPUT_LINE,
			     audio->audio_channels, 0);

	if(GTK_WIDGET_VISIBLE((GtkWidget *) panel)){
	  ags_connectable_connect(AGS_CONNECTABLE(panel_input_pad));
	  ags_pad_find_port(AGS_PAD(panel_input_pad));
	  gtk_widget_show_all((GtkWidget *) panel_input_pad);
	}
      }else{
	panel_input_pad = AGS_PANEL_INPUT_PAD(list_input_pad->data);

	ags_pad_resize_lines((AgsPad *) panel_input_pad, AGS_TYPE_PANEL_INPUT_LINE,
			     audio_channels, audio_channels_old);
      }

      channel = channel->next_pad;

      if(audio_channels_old != 0){
	list_input_pad = list_input_pad->next;
      }
    }
  }else if(audio_channels < audio_channels_old){
    GList *list_input_pad_next;

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
	ags_pad_resize_lines(AGS_PAD(list_input_pad->data), AGS_TYPE_PANEL_INPUT_PAD,
			     audio_channels, audio_channels_old);

	list_input_pad = list_input_pad->next;
      }
    }
  }

  g_list_free(list_input_pad_start);
}

void
ags_panel_set_pads(AgsAudio *audio, GType type,
		   guint pads, guint pads_old,
		   gpointer data)
{
  AgsPanel *panel;
  AgsChannel *channel;
  GList *list, *list_next, *list_start;
  guint i, j;

  panel = (AgsPanel *) audio->machine;

  if(type == AGS_TYPE_INPUT){
    AgsPanelInputPad *panel_input_pad;

    if(pads_old < pads){
      /*  */
      channel = ags_channel_nth(audio->input, pads_old * audio->audio_channels);

      for(i = pads_old; i < pads; i++){
	panel_input_pad = ags_panel_input_pad_new(channel);
	gtk_box_pack_start((GtkBox *) AGS_MACHINE(panel)->input,
			   (GtkWidget *) panel_input_pad, FALSE, FALSE, 0);
	ags_pad_resize_lines((AgsPad *) panel_input_pad, AGS_TYPE_PANEL_INPUT_LINE,
			     AGS_AUDIO(channel->audio)->audio_channels, 0);

	if(GTK_WIDGET_VISIBLE((GtkWidget *) panel)){
	  ags_connectable_connect(AGS_CONNECTABLE(panel_input_pad));
	  ags_pad_find_port(AGS_PAD(panel_input_pad));
	  gtk_widget_show_all((GtkWidget *) panel_input_pad);
	}

	channel = channel->next_pad;
      }
    }else{
      /* destroy AgsPad's */
      if(pads != 0){
	
	list_start = 
	  list = gtk_container_get_children((GtkContainer *) AGS_MACHINE(panel)->input);
	list = g_list_nth(list, pads);
	
	while(list != NULL){
	  list_next = list->next;
	  
	  gtk_widget_destroy((GtkWidget *) list->data);
	  
	  list = list_next;
	}

	g_list_free(list_start);
      }
    }
  }
}

/**
 * ags_panel_new:
 * @devout: the assigned devout.
 *
 * Creates an #AgsPanel
 *
 * Returns: a new #AgsPanel
 *
 * Since: 0.3
 */
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
