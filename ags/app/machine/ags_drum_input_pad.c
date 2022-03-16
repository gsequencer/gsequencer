/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/app/machine/ags_drum_input_pad.h>
#include <ags/app/machine/ags_drum_input_pad_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/app/machine/ags_drum.h>

#include <math.h>

#include <ags/i18n.h>

void ags_drum_input_pad_class_init(AgsDrumInputPadClass *drum_input_pad);
void ags_drum_input_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_drum_input_pad_init(AgsDrumInputPad *drum_input_pad);
static void ags_drum_input_pad_finalize(GObject *gobject);

void ags_drum_input_pad_connect(AgsConnectable *connectable);
void ags_drum_input_pad_disconnect(AgsConnectable *connectable);

void ags_drum_input_pad_set_channel(AgsPad *pad, AgsChannel *channel);
void ags_drum_input_pad_resize_lines(AgsPad *pad, GType line_type,
				     guint audio_channels, guint audio_channels_old);

/**
 * SECTION:ags_drum_input_pad
 * @short_description: drum sequencer input pad
 * @title: AgsDrumInputPad
 * @section_id:
 * @include: ags/app/machine/ags_drum_input_pad.h
 *
 * The #AgsDrumInputPad is a composite widget to act as drum sequencer input pad.
 */

static gpointer ags_drum_input_pad_parent_class = NULL;
static AgsConnectableInterface *ags_drum_input_pad_parent_connectable_interface;

GType
ags_drum_input_pad_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_drum_input_pad = 0;

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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_drum_input_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_drum_input_pad = g_type_register_static(AGS_TYPE_PAD,
						     "AgsDrumInputPad", &ags_drum_input_pad_info,
						     0);

    g_type_add_interface_static(ags_type_drum_input_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_drum_input_pad);
  }

  return g_define_type_id__volatile;
}

void
ags_drum_input_pad_class_init(AgsDrumInputPadClass *drum_input_pad)
{
  AgsPadClass *pad;
  GObjectClass *gobject;
  
  ags_drum_input_pad_parent_class = g_type_class_peek_parent(drum_input_pad);

  /*  */
  gobject = (GObjectClass *) drum_input_pad;

  gobject->finalize = ags_drum_input_pad_finalize;

  /*  */
  pad = (AgsPadClass *) drum_input_pad;

  pad->set_channel = ags_drum_input_pad_set_channel;
  pad->resize_lines = ags_drum_input_pad_resize_lines;
}

void
ags_drum_input_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_drum_input_pad_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_drum_input_pad_connect;
  connectable->disconnect = ags_drum_input_pad_disconnect;
}

void
ags_drum_input_pad_init(AgsDrumInputPad *drum_input_pad)
{
  AgsPad *pad;
  GtkBox *hbox;

  drum_input_pad->flags = 0;

  drum_input_pad->name = NULL;
  drum_input_pad->xml_type = "ags-drum-input-pad";

  pad = (AgsPad *) drum_input_pad;

  /* toplevel functions */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_append(pad,
		 (GtkWidget *) hbox);
  gtk_box_reorder_child_after((GtkBox *) pad,
			      (GtkWidget *) hbox,
			      NULL);

  /* open */
  drum_input_pad->open = (GtkButton *) gtk_button_new_from_icon_name("document-open");
  gtk_box_append(hbox,
		 (GtkWidget *) drum_input_pad->open);

  /* play */
  AGS_PAD(drum_input_pad)->play = 
    drum_input_pad->play = (GtkToggleButton *) gtk_toggle_button_new();
  g_object_set(drum_input_pad->play,
	       "icon-name", "media-playback-start",
	       NULL);
  gtk_box_append(hbox,
		 (GtkWidget *) drum_input_pad->play);

  /* bottom functions - edit */
  drum_input_pad->edit = (GtkToggleButton *) gtk_toggle_button_new_with_label(i18n("edit"));
  gtk_box_append((GtkBox *) pad,
		 (GtkWidget *) drum_input_pad->edit);
  
  /* recall specific */
  drum_input_pad->pad_open_play_ref = 0;
  drum_input_pad->pad_open_recalls = NULL;

  drum_input_pad->file_chooser = NULL;

  drum_input_pad->pad_play_ref = 0;
}

static void
ags_drum_input_pad_finalize(GObject *gobject)
{
  AgsDrumInputPad *drum_input_pad;

  drum_input_pad = AGS_DRUM_INPUT_PAD(gobject);

  //FIXME:JK: won't be called
  //NOTE:JK: work-around in ags_drum.c
  if(drum_input_pad->file_chooser != NULL){
    //    gtk_widget_destroy(drum_input_pad->file_chooser);
  }
  
  G_OBJECT_CLASS(ags_drum_input_pad_parent_class)->finalize(gobject);
}

void
ags_drum_input_pad_connect(AgsConnectable *connectable)
{
  AgsDrumInputPad *drum_input_pad;

  drum_input_pad = AGS_DRUM_INPUT_PAD(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (AGS_PAD(drum_input_pad)->connectable_flags)) != 0){
    return;
  }

  ags_drum_input_pad_parent_connectable_interface->connect(connectable);

  /* AgsDrumInputPad */
  g_signal_connect(G_OBJECT(drum_input_pad->open), "clicked",
		   G_CALLBACK(ags_drum_input_pad_open_callback), (gpointer) drum_input_pad);

  g_signal_connect_after(G_OBJECT(drum_input_pad->play), "toggled",
			 G_CALLBACK(ags_drum_input_pad_play_callback), (gpointer) drum_input_pad);

  g_signal_connect(G_OBJECT(drum_input_pad->edit), "clicked",
		   G_CALLBACK(ags_drum_input_pad_edit_callback), (gpointer) drum_input_pad);
}

void
ags_drum_input_pad_disconnect(AgsConnectable *connectable)
{
  AgsDrumInputPad *drum_input_pad;

  drum_input_pad = AGS_DRUM_INPUT_PAD(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (AGS_PAD(drum_input_pad)->connectable_flags)) == 0){
    return;
  }

  ags_drum_input_pad_parent_connectable_interface->disconnect(connectable);

  /* AgsDrumInputPad */
  g_object_disconnect(G_OBJECT(drum_input_pad->open),
		      "any_signal::clicked",
		      G_CALLBACK(ags_drum_input_pad_open_callback),
		      (gpointer) drum_input_pad,
		      NULL);

  g_object_disconnect(G_OBJECT(drum_input_pad->play),
		      "any_signal::toggled",
		      G_CALLBACK(ags_drum_input_pad_play_callback),
		      (gpointer) drum_input_pad,
		      NULL);

  g_object_disconnect(G_OBJECT(drum_input_pad->edit),
		      "any_signal::clicked",
		      G_CALLBACK(ags_drum_input_pad_edit_callback),
		      (gpointer) drum_input_pad,
		      NULL);
}

void
ags_drum_input_pad_set_channel(AgsPad *pad, AgsChannel *channel)
{
  AGS_PAD_CLASS(ags_drum_input_pad_parent_class)->set_channel(pad, channel);

  /* empty */
}

void
ags_drum_input_pad_resize_lines(AgsPad *pad, GType line_type,
				guint audio_channels, guint audio_channels_old)
{
  AGS_PAD_CLASS(ags_drum_input_pad_parent_class)->resize_lines(pad, line_type,
							       audio_channels, audio_channels_old);

  /* empty */
}

/**
 * ags_drum_input_pad_new:
 * @channel: the assigned channel
 *
 * Creates an #AgsDrumInputPad
 *
 * Returns: a new #AgsDrumInputPad
 *
 * Since: 3.0.0
 */
AgsDrumInputPad*
ags_drum_input_pad_new(AgsChannel *channel)
{
  AgsDrumInputPad *drum_input_pad;

  drum_input_pad = (AgsDrumInputPad *) g_object_new(AGS_TYPE_DRUM_INPUT_PAD,
						    "channel", channel,
						    NULL);

  return(drum_input_pad);
}
