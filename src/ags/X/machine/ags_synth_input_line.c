/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/X/machine/ags_synth_input_line.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/plugin/ags_plugin_stock.h>

#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_volume_channel.h>
#include <ags/audio/recall/ags_volume_channel_run.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_line_member.h>

#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_oscillator.h>

void ags_synth_input_line_class_init(AgsSynthInputLineClass *synth_input_line);
void ags_synth_input_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_synth_input_line_init(AgsSynthInputLine *synth_input_line);
void ags_synth_input_line_connect(AgsConnectable *connectable);
void ags_synth_input_line_disconnect(AgsConnectable *connectable);

void ags_synth_input_line_set_channel(AgsLine *line, AgsChannel *channel);

static gpointer ags_synth_input_line_parent_class = NULL;
static AgsConnectableInterface *ags_synth_input_line_parent_connectable_interface;

GType
ags_synth_input_line_get_type()
{
  static GType ags_type_synth_input_line = 0;

  if(!ags_type_synth_input_line){
    static const GTypeInfo ags_synth_input_line_info = {
      sizeof(AgsSynthInputLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_synth_input_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSynthInputLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_synth_input_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_synth_input_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_synth_input_line = g_type_register_static(AGS_TYPE_LINE,
						       "AgsSynthInputLine\0", &ags_synth_input_line_info,
						       0);

    g_type_add_interface_static(ags_type_synth_input_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_synth_input_line);
}

void
ags_synth_input_line_class_init(AgsSynthInputLineClass *synth_input_line)
{
  AgsLineClass *line;

  ags_synth_input_line_parent_class = g_type_class_peek_parent(synth_input_line);

  /* AgsLineClass */
  line = AGS_LINE_CLASS(synth_input_line);

  line->set_channel = ags_synth_input_line_set_channel;
}

void
ags_synth_input_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_synth_input_line_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_synth_input_line_connect;
  connectable->disconnect = ags_synth_input_line_disconnect;
}

void
ags_synth_input_line_init(AgsSynthInputLine *synth_input_line)
{
  AgsOscillator *oscillator;

  /* oscillator */
  oscillator = ags_oscillator_new();
  ags_expander_add(AGS_LINE(synth_input_line)->expander,
		   GTK_WIDGET(oscillator),
		   0, 0,
		   1, 1);
}

void
ags_synth_input_line_connect(AgsConnectable *connectable)
{
  AgsSynthInputLine *synth_input_line;

  synth_input_line = AGS_SYNTH_INPUT_LINE(connectable);

  if((AGS_LINE_CONNECTED & (AGS_LINE(synth_input_line)->flags)) != 0){
    return;
  }

  ags_synth_input_line_parent_connectable_interface->connect(connectable);
}

void
ags_synth_input_line_disconnect(AgsConnectable *connectable)
{
  ags_synth_input_line_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_synth_input_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  AgsSynthInputLine *synth_input_line;

  AGS_LINE_CLASS(ags_synth_input_line_parent_class)->set_channel(line, channel);

  synth_input_line = AGS_SYNTH_INPUT_LINE(line);

  if(line->channel != NULL){
    line->flags &= (~AGS_LINE_MAPPED_RECALL);
  }

  if(channel != NULL){
    if((AGS_LINE_PREMAPPED_RECALL & (line->flags)) == 0){
      ags_synth_input_line_map_recall(synth_input_line);
    }
  }
}

void
ags_synth_input_line_map_recall(AgsSynthInputLine *synth_input_line)
{
  AgsSynth *synth;
  AgsLine *line;
  AgsAudio *audio;
  AgsChannel *source;
  guint i;

  line = AGS_LINE(synth_input_line);
  line->flags |= AGS_LINE_MAPPED_RECALL;

  audio = AGS_AUDIO(line->channel->audio);

  synth = AGS_SYNTH(audio->machine);

  source = line->channel;

  /* ags-volume */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-volume\0",
			    source->audio_channel, source->audio_channel + 1,
			    source->pad, source->pad + 1,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL |
			     AGS_RECALL_FACTORY_ADD),
			    0);
}

AgsSynthInputLine*
ags_synth_input_line_new(AgsChannel *channel)
{
  AgsSynthInputLine *synth_input_line;

  synth_input_line = (AgsSynthInputLine *) g_object_new(AGS_TYPE_SYNTH_INPUT_LINE,
							"channel\0", channel,
							NULL);

  return(synth_input_line);
}

