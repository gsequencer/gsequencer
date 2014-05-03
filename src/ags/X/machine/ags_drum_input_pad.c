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

#include <ags/X/machine/ags_drum_input_pad.h>
#include <ags/X/machine/ags_drum_input_pad_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

#include <ags/X/ags_window.h>

#include <ags/X/machine/ags_drum.h>

#include <math.h>

void ags_drum_input_pad_class_init(AgsDrumInputPadClass *drum_input_pad);
void ags_drum_input_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_drum_input_pad_init(AgsDrumInputPad *drum_input_pad);
void ags_drum_input_pad_destroy(GtkObject *object);
void ags_drum_input_pad_connect(AgsConnectable *connectable);
void ags_drum_input_pad_disconnect(AgsConnectable *connectable);

void ags_drum_input_pad_set_channel(AgsPad *pad, AgsChannel *channel);
void ags_drum_input_pad_resize_lines(AgsPad *pad, GType line_type,
				     guint audio_channels, guint audio_channels_old);

void ags_file_read_drum_input_pad(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_file_write_drum_input_pad(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

static gpointer ags_drum_input_pad_parent_class = NULL;
static AgsConnectableInterface *ags_drum_input_pad_parent_connectable_interface;

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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_drum_input_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_drum_input_pad = g_type_register_static(AGS_TYPE_PAD,
						     "AgsDrumInputPad\0", &ags_drum_input_pad_info,
						     0);

    g_type_add_interface_static(ags_type_drum_input_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_drum_input_pad);
}

void
ags_drum_input_pad_class_init(AgsDrumInputPadClass *drum_input_pad)
{
  AgsPadClass *pad;

  ags_drum_input_pad_parent_class = g_type_class_peek_parent(drum_input_pad);

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
  GtkHBox *hbox;

  drum_input_pad->flags = 0;

  pad = (AgsPad *) drum_input_pad;

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) pad, (GtkWidget *) hbox, FALSE, TRUE, 0);
  gtk_box_reorder_child((GtkBox *) pad, (GtkWidget *) hbox, 0);

  drum_input_pad->open = (GtkButton *) gtk_button_new();
  gtk_container_add((GtkContainer *) drum_input_pad->open, (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON));
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) drum_input_pad->open, TRUE, TRUE, 0);

  drum_input_pad->play = (GtkToggleButton *) gtk_toggle_button_new();
  gtk_container_add((GtkContainer *) drum_input_pad->play, (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON));
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) drum_input_pad->play, TRUE, TRUE, 0);

  drum_input_pad->edit = (GtkToggleButton *) gtk_toggle_button_new_with_label("edit\0");
  gtk_box_pack_start((GtkBox *) pad, (GtkWidget *) drum_input_pad->edit, FALSE, FALSE, 0);

  drum_input_pad->pad_open_play_ref = 0;
  drum_input_pad->pad_open_recalls = NULL;

  drum_input_pad->file_chooser = NULL;

  drum_input_pad->pad_play_ref = 0;
}

void
ags_drum_input_pad_connect(AgsConnectable *connectable)
{
  AgsDrumInputPad *drum_input_pad;

  drum_input_pad = AGS_DRUM_INPUT_PAD(connectable);

  if((AGS_PAD_CONNECTED & (AGS_PAD(drum_input_pad)->flags)) != 0){
    return;
  }

  ags_drum_input_pad_parent_connectable_interface->connect(connectable);

  /* AgsDrumInputPad */
  g_signal_connect(G_OBJECT(drum_input_pad->open), "clicked\0",
		   G_CALLBACK(ags_drum_input_pad_open_callback), (gpointer) drum_input_pad);

  g_signal_connect_after(G_OBJECT(drum_input_pad->play), "toggled\0",
			 G_CALLBACK(ags_drum_input_pad_play_callback), (gpointer) drum_input_pad);

  g_signal_connect(G_OBJECT(drum_input_pad->edit), "clicked\0",
		   G_CALLBACK(ags_drum_input_pad_edit_callback), (gpointer) drum_input_pad);
}

void
ags_drum_input_pad_disconnect(AgsConnectable *connectable)
{
  ags_drum_input_pad_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_drum_input_pad_destroy(GtkObject *object)
{
  /* empty */
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

void
ags_file_read_drum_input_pad(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_drum_input_pad(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  //TODO:JK: implement me
}

AgsDrumInputPad*
ags_drum_input_pad_new(AgsChannel *channel)
{
  AgsDrumInputPad *drum_input_pad;

  drum_input_pad = (AgsDrumInputPad *) g_object_new(AGS_TYPE_DRUM_INPUT_PAD,
						    "channel\0", channel,
						    NULL);

  return(drum_input_pad);
}
