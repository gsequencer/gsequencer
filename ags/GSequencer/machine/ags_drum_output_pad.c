/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/GSequencer/machine/ags_drum_output_pad.h>
#include <ags/GSequencer/machine/ags_drum_output_pad_callbacks.h>

#include <ags/GSequencer/ags_line.h>

#include <ags/GSequencer/machine/ags_drum.h>

void ags_drum_output_pad_class_init(AgsDrumOutputPadClass *drum_output_pad);
void ags_drum_output_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_drum_output_pad_init(AgsDrumOutputPad *drum_output_pad);

void ags_drum_output_pad_connect(AgsConnectable *connectable);
void ags_drum_output_pad_disconnect(AgsConnectable *connectable);

void ags_drum_output_pad_set_channel(AgsPad *pad, AgsChannel *channel);
void ags_drum_output_pad_resize_lines(AgsPad *pad, GType line_type,
				      guint audio_channels, guint audio_channels_old);

/**
 * SECTION:ags_drum_output_pad
 * @short_description: drum sequencer output pad
 * @title: AgsDrumOutputPad
 * @section_id:
 * @include: ags/X/machine/ags_drum_output_pad.h
 *
 * The #AgsDrumOutputPad is a composite widget to act as drum sequencer output pad.
 */

static gpointer ags_drum_output_pad_parent_class = NULL;
static AgsConnectableInterface *ags_drum_output_pad_parent_connectable_interface;

GType
ags_drum_output_pad_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_drum_output_pad = 0;

    static const GTypeInfo ags_drum_output_pad_info = {
      sizeof(AgsDrumOutputPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_drum_output_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDrumOutputPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_drum_output_pad_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_drum_output_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_drum_output_pad = g_type_register_static(AGS_TYPE_PAD,
						      "AgsDrumOutputPad", &ags_drum_output_pad_info,
						      0);
    
    g_type_add_interface_static(ags_type_drum_output_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_drum_output_pad);
  }

  return g_define_type_id__volatile;
}

void
ags_drum_output_pad_class_init(AgsDrumOutputPadClass *drum_output_pad)
{
  AgsPadClass *pad;

  ags_drum_output_pad_parent_class = g_type_class_peek_parent(drum_output_pad);

  /* AgsPadClass */
  pad = (AgsPadClass *) drum_output_pad;

  pad->set_channel = ags_drum_output_pad_set_channel;
  pad->resize_lines = ags_drum_output_pad_resize_lines;
}

void
ags_drum_output_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_drum_output_pad_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_drum_output_pad_connect;
  connectable->disconnect = ags_drum_output_pad_disconnect;
}

void
ags_drum_output_pad_init(AgsDrumOutputPad *drum_output_pad)
{
  drum_output_pad->flags = 0;

  drum_output_pad->xml_type = "ags-drum-output-pad";
}

void
ags_drum_output_pad_connect(AgsConnectable *connectable)
{
  AgsDrumOutputPad *drum_output_pad;

  drum_output_pad = AGS_DRUM_OUTPUT_PAD(connectable);

  if((AGS_PAD_CONNECTED & (AGS_PAD(drum_output_pad)->flags)) != 0){
    return;
  }

  ags_drum_output_pad_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_drum_output_pad_disconnect(AgsConnectable *connectable)
{
  ags_drum_output_pad_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_drum_output_pad_set_channel(AgsPad *pad, AgsChannel *channel)
{
  AGS_PAD_CLASS(ags_drum_output_pad_parent_class)->set_channel(pad, channel);

  /* empty */
}

void
ags_drum_output_pad_resize_lines(AgsPad *pad, GType line_type,
				 guint audio_channels, guint audio_channels_old)
{
  AGS_PAD_CLASS(ags_drum_output_pad_parent_class)->resize_lines(pad, line_type,
								audio_channels, audio_channels_old);

  /* empty */
}

/**
 * ags_drum_output_pad_new:
 * @channel: the assigned channel
 *
 * Create a new instance of #AgsDrumOutputPad
 *
 * Returns: the new #AgsDrumOutputPad
 *
 * Since: 3.0.0
 */
AgsDrumOutputPad*
ags_drum_output_pad_new(AgsChannel *channel)
{
  AgsDrumOutputPad *drum_output_pad;

  drum_output_pad = (AgsDrumOutputPad *) g_object_new(AGS_TYPE_DRUM_OUTPUT_PAD,
						      "channel", channel,
						      NULL);

  return(drum_output_pad);
}
