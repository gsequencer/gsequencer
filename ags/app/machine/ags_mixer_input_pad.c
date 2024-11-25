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

#include <ags/app/machine/ags_mixer_input_pad.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_window.h>

#include <ags/app/machine/ags_mixer.h>

#include <math.h>

void ags_mixer_input_pad_class_init(AgsMixerInputPadClass *mixer_input_pad);
void ags_mixer_input_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_mixer_input_pad_init(AgsMixerInputPad *mixer_input_pad);

void ags_mixer_input_pad_connect(AgsConnectable *connectable);
void ags_mixer_input_pad_disconnect(AgsConnectable *connectable);

void ags_mixer_input_pad_set_channel(AgsPad *pad, AgsChannel *channel);
void ags_mixer_input_pad_resize_lines(AgsPad *pad, GType line_type,
				      guint audio_channels, guint audio_channels_old);

/**
 * SECTION:ags_mixer_input_pad
 * @short_description: mixer input pad
 * @title: AgsMixerInputPad
 * @section_id:
 * @include: ags/app/machine/ags_mixer_input_pad.h
 *
 * The #AgsMixerInputPad is a composite widget to act as mixer input pad.
 */

static gpointer ags_mixer_input_pad_parent_class = NULL;
static AgsConnectableInterface *ags_mixer_input_pad_parent_connectable_interface;

GType
ags_mixer_input_pad_get_type()
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_mixer_input_pad = 0;

    static const GTypeInfo ags_mixer_input_pad_info = {
      sizeof(AgsMixerInputPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_mixer_input_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsMixerInputPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_mixer_input_pad_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_mixer_input_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_mixer_input_pad = g_type_register_static(AGS_TYPE_PAD,
						      "AgsMixerInputPad", &ags_mixer_input_pad_info,
						      0);

    g_type_add_interface_static(ags_type_mixer_input_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_mixer_input_pad);
  }

  return g_define_type_id__volatile;
}

void
ags_mixer_input_pad_class_init(AgsMixerInputPadClass *mixer_input_pad)
{
  AgsPadClass *pad;

  ags_mixer_input_pad_parent_class = g_type_class_peek_parent(mixer_input_pad);

  pad = (AgsPadClass *) mixer_input_pad;

  pad->set_channel = ags_mixer_input_pad_set_channel;
  pad->resize_lines = ags_mixer_input_pad_resize_lines;
}

void
ags_mixer_input_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_mixer_input_pad_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_mixer_input_pad_connect;
  connectable->disconnect = ags_mixer_input_pad_disconnect;
}

void
ags_mixer_input_pad_init(AgsMixerInputPad *mixer_input_pad)
{
}

void
ags_mixer_input_pad_connect(AgsConnectable *connectable)
{
  AgsMixerInputPad *mixer_input_pad;

  /* AgsMixerInputPad */
  mixer_input_pad = AGS_MIXER_INPUT_PAD(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ags_mixer_input_pad_parent_connectable_interface->connect(connectable);
}

void
ags_mixer_input_pad_disconnect(AgsConnectable *connectable)
{
  AgsMixerInputPad *mixer_input_pad;

  /* AgsMixerInputPad */
  mixer_input_pad = AGS_MIXER_INPUT_PAD(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ags_mixer_input_pad_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_mixer_input_pad_set_channel(AgsPad *pad, AgsChannel *channel)
{
  AGS_PAD_CLASS(ags_mixer_input_pad_parent_class)->set_channel(pad, channel);

  /* empty */
}

void
ags_mixer_input_pad_resize_lines(AgsPad *pad, GType line_type,
				guint audio_channels, guint audio_channels_old)
{
  AGS_PAD_CLASS(ags_mixer_input_pad_parent_class)->resize_lines(pad, line_type,
								audio_channels, audio_channels_old);

  /* empty */
}

/**
 * ags_mixer_input_pad_new:
 * @channel: the assigned #AgsChannel
 *
 * Create a new instance of #AgsMixerInputPad
 *
 * Returns: the new #AgsMixerInputPad
 *
 * Since: 3.0.0
 */
AgsMixerInputPad*
ags_mixer_input_pad_new(AgsChannel *channel)
{
  AgsMixerInputPad *mixer_input_pad;

  mixer_input_pad = (AgsMixerInputPad *) g_object_new(AGS_TYPE_MIXER_INPUT_PAD,
						      "channel", channel,
						      NULL);
  
  return(mixer_input_pad);
}

