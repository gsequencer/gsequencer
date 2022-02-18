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

#include <ags/app/machine/ags_ffplayer_input_line.h>
#include <ags/app/machine/ags_ffplayer_input_line_callbacks.h>

#include <ags/app/ags_effect_bridge.h>
#include <ags/app/ags_effect_line.h>

void ags_ffplayer_input_line_class_init(AgsFFPlayerInputLineClass *ffplayer_input_line);
void ags_ffplayer_input_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ffplayer_input_line_init(AgsFFPlayerInputLine *ffplayer_input_line);
void ags_ffplayer_input_line_finalize(GObject *gobject);

void ags_ffplayer_input_line_connect(AgsConnectable *connectable);
void ags_ffplayer_input_line_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_ffplayer_input_line
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsFFPlayerInputLine
 * @section_id:
 * @include: ags/X/machine/ags_ffplayer_input_line.h
 *
 * #AgsFFPlayerInputLine is a composite widget to visualize one #AgsChannel. It should be
 * packed by an #AgsFFPlayerInputLine.
 */

static gpointer ags_ffplayer_input_line_parent_class = NULL;
static AgsConnectableInterface *ags_ffplayer_input_line_parent_connectable_interface;

GType
ags_ffplayer_input_line_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ffplayer_input_line = 0;

    static const GTypeInfo ags_ffplayer_input_line_info = {
      sizeof(AgsFFPlayerInputLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ffplayer_input_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFFPlayerInputLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ffplayer_input_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ffplayer_input_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ffplayer_input_line = g_type_register_static(AGS_TYPE_EFFECT_LINE,
							  "AgsFFPlayerInputLine", &ags_ffplayer_input_line_info,
							  0);

    g_type_add_interface_static(ags_type_ffplayer_input_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ffplayer_input_line);
  }

  return g_define_type_id__volatile;
}

void
ags_ffplayer_input_line_class_init(AgsFFPlayerInputLineClass *ffplayer_input_line)
{
  GObjectClass *gobject;
  
  ags_ffplayer_input_line_parent_class = g_type_class_peek_parent(ffplayer_input_line);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(ffplayer_input_line);

  gobject->finalize = ags_ffplayer_input_line_finalize;
}

void
ags_ffplayer_input_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_ffplayer_input_line_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_ffplayer_input_line_connect;
  connectable->disconnect = ags_ffplayer_input_line_disconnect;
}

void
ags_ffplayer_input_line_init(AgsFFPlayerInputLine *ffplayer_input_line)
{
  g_signal_connect_after(ffplayer_input_line, "notify::channel",
			 G_CALLBACK(ags_ffplayer_input_line_notify_channel_callback), NULL);
}

void
ags_ffplayer_input_line_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_ffplayer_input_line_parent_class)->finalize(gobject);
}

void
ags_ffplayer_input_line_connect(AgsConnectable *connectable)
{
  if((AGS_EFFECT_LINE_CONNECTED & (AGS_EFFECT_LINE(connectable)->flags)) != 0){
    return;
  }

  ags_ffplayer_input_line_parent_connectable_interface->connect(connectable);
}

void
ags_ffplayer_input_line_disconnect(AgsConnectable *connectable)
{
  if((AGS_EFFECT_LINE_CONNECTED & (AGS_EFFECT_LINE(connectable)->flags)) == 0){
    return;
  }

  ags_ffplayer_input_line_parent_connectable_interface->disconnect(connectable);
}

/**
 * ags_ffplayer_input_line_new:
 * @channel: the #AgsChannel to visualize
 *
 * Creates an #AgsFFPlayerInputLine
 *
 * Returns: a new #AgsFFPlayerInputLine
 *
 * Since: 3.0.0
 */
AgsFFPlayerInputLine*
ags_ffplayer_input_line_new(AgsChannel *channel)
{
  AgsFFPlayerInputLine *ffplayer_input_line;

  ffplayer_input_line = (AgsFFPlayerInputLine *) g_object_new(AGS_TYPE_FFPLAYER_INPUT_LINE,
							      "channel", channel,
							      NULL);

  return(ffplayer_input_line);
}
