/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/app/machine/ags_ffplayer_input_pad.h>
#include <ags/app/machine/ags_ffplayer_input_pad_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_effect_bridge.h>
#include <ags/app/ags_effect_line.h>

void ags_ffplayer_input_pad_class_init(AgsFFPlayerInputPadClass *ffplayer_input_pad);
void ags_ffplayer_input_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ffplayer_input_pad_init(AgsFFPlayerInputPad *ffplayer_input_pad);
void ags_ffplayer_input_pad_finalize(GObject *gobject);

void ags_ffplayer_input_pad_connect(AgsConnectable *connectable);
void ags_ffplayer_input_pad_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_ffplayer_input_pad
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsFFPlayerInputPad
 * @section_id:
 * @include: ags/app/machine/ags_ffplayer_input_pad.h
 *
 * #AgsFFPlayerInputPad is a composite widget to visualize one #AgsChannel. It should be
 * packed by an #AgsFFPlayerInputPad.
 */

static gpointer ags_ffplayer_input_pad_parent_class = NULL;
static AgsConnectableInterface *ags_ffplayer_input_pad_parent_connectable_interface;

GType
ags_ffplayer_input_pad_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ffplayer_input_pad = 0;

    static const GTypeInfo ags_ffplayer_input_pad_info = {
      sizeof(AgsFFPlayerInputPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ffplayer_input_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFFPlayerInputPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ffplayer_input_pad_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ffplayer_input_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ffplayer_input_pad = g_type_register_static(AGS_TYPE_EFFECT_PAD,
							 "AgsFFPlayerInputPad", &ags_ffplayer_input_pad_info,
							 0);

    g_type_add_interface_static(ags_type_ffplayer_input_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ffplayer_input_pad);
  }

  return g_define_type_id__volatile;
}

void
ags_ffplayer_input_pad_class_init(AgsFFPlayerInputPadClass *ffplayer_input_pad)
{
  GObjectClass *gobject;

  ags_ffplayer_input_pad_parent_class = g_type_class_peek_parent(ffplayer_input_pad);

  /* GObjectClass */
  gobject = (GObjectClass *) ffplayer_input_pad;

  gobject->finalize = ags_ffplayer_input_pad_finalize;
}

void
ags_ffplayer_input_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_ffplayer_input_pad_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_ffplayer_input_pad_connect;
  connectable->disconnect = ags_ffplayer_input_pad_disconnect;
}

void
ags_ffplayer_input_pad_init(AgsFFPlayerInputPad *ffplayer_input_pad)
{
  //TODO:JK: implement me
}

void
ags_ffplayer_input_pad_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_ffplayer_input_pad_parent_class)->finalize(gobject);
}

void
ags_ffplayer_input_pad_connect(AgsConnectable *connectable)
{
  if((AGS_EFFECT_PAD_CONNECTED & (AGS_EFFECT_PAD(connectable)->flags)) != 0){
    return;
  }

  ags_ffplayer_input_pad_parent_connectable_interface->connect(connectable);

  //TODO:JK: implement me
}

void
ags_ffplayer_input_pad_disconnect(AgsConnectable *connectable)
{
  if((AGS_EFFECT_PAD_CONNECTED & (AGS_EFFECT_PAD(connectable)->flags)) == 0){
    return;
  }

  ags_ffplayer_input_pad_parent_connectable_interface->disconnect(connectable);

  //TODO:JK: implement me
}

/**
 * ags_ffplayer_input_pad_new:
 * @channel: the #AgsChannel to visualize
 *
 * Creates an #AgsFFPlayerInputPad
 *
 * Returns: a new #AgsFFPlayerInputPad
 *
 * Since: 3.0.0
 */
AgsFFPlayerInputPad*
ags_ffplayer_input_pad_new(AgsChannel *channel)
{
  AgsFFPlayerInputPad *ffplayer_input_pad;

  ffplayer_input_pad = (AgsFFPlayerInputPad *) g_object_new(AGS_TYPE_FFPLAYER_INPUT_PAD,
							    "channel", channel,
							    NULL);

  return(ffplayer_input_pad);
}
