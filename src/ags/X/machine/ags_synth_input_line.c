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
#include <ags/X/machine/ags_synth_input_line_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/widget/ags_expander_set.h>
#include <ags/widget/ags_expander.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_line_callbacks.h>
#include <ags/X/ags_line_member.h>

#include <ags/X/machine/ags_synth.h>

void ags_synth_input_line_class_init(AgsSynthInputLineClass *synth_input_line);
void ags_synth_input_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_synth_input_line_init(AgsSynthInputLine *synth_input_line);
void ags_synth_input_line_destroy(GtkObject *object);
void ags_synth_input_line_connect(AgsConnectable *connectable);
void ags_synth_input_line_disconnect(AgsConnectable *connectable);

void ags_synth_input_line_set_channel(AgsLine *line, AgsChannel *channel);
void ags_synth_input_line_group_changed(AgsLine *line);

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
  
  line->group_changed = ags_synth_input_line_group_changed;
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
}

void
ags_synth_input_line_destroy(GtkObject *object)
{
}

void
ags_synth_input_line_connect(AgsConnectable *connectable)
{
  AgsSynth *synth;
  AgsSynthInputLine *synth_input_line;

  synth_input_line = AGS_SYNTH_INPUT_LINE(connectable);


  if((AGS_LINE_CONNECTED & (AGS_LINE(synth_input_line)->flags)) != 0){
    return;
  }
  
  ags_synth_input_line_parent_connectable_interface->connect(connectable);

  /* AgsSynthInputLine */
  synth = AGS_SYNTH(gtk_widget_get_ancestor((GtkWidget *) AGS_LINE(synth_input_line)->pad, AGS_TYPE_SYNTH));

  /* AgsAudio */
  g_signal_connect_after(G_OBJECT(AGS_MACHINE(synth)->audio), "set_pads\0",
			 G_CALLBACK(ags_synth_input_line_audio_set_pads_callback), synth_input_line);

  /* AgsSynthInputLine */
  /* empty */
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

  g_message("ags_synth_input_line_set_channel - channel: %u\0",
	    channel->line);

  if(line->channel != NULL){
    line->flags &= (~AGS_LINE_MAPPED_RECALL);
  }

  if(channel != NULL){
    if((AGS_LINE_PREMAPPED_RECALL & (line->flags)) == 0){
      ags_synth_input_line_map_recall(synth_input_line, 0);
    }else{
      //TODO:JK: implement me
    }
  }
}

void
ags_synth_input_line_group_changed(AgsLine *line)
{
  AgsSynth *synth;

  synth = (AgsSynth *) gtk_widget_get_ancestor(GTK_WIDGET(line), AGS_TYPE_SYNTH);
}

void
ags_synth_input_line_map_recall(AgsSynthInputLine *synth_input_line,
				guint output_pad_start)
{
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
