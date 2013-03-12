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

#include <ags/X/machine/ags_mixer_input_line.h>

#include <ags/object/ags_connectable.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_volume_channel.h>
#include <ags/audio/recall/ags_volume_channel_run.h>

#include <ags/X/ags_window.h>

#include <ags/X/machine/ags_mixer.h>

void ags_mixer_input_line_class_init(AgsMixerInputLineClass *mixer_input_line);
void ags_mixer_input_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_mixer_input_line_init(AgsMixerInputLine *mixer_input_line);
void ags_mixer_input_line_destroy(GtkObject *object);
void ags_mixer_input_line_connect(AgsConnectable *connectable);
void ags_mixer_input_line_disconnect(AgsConnectable *connectable);

void ags_mixer_input_line_set_channel(AgsLine *line, AgsChannel *channel);

static gpointer ags_mixer_input_line_parent_class = NULL;
static AgsConnectableInterface *ags_mixer_input_line_parent_connectable_interface;

GType
ags_mixer_input_line_get_type()
{
  static GType ags_type_mixer_input_line = 0;

  if(!ags_type_mixer_input_line){
    static const GTypeInfo ags_mixer_input_line_info = {
      sizeof(AgsMixerInputLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_mixer_input_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsMixerInputLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_mixer_input_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_mixer_input_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_mixer_input_line = g_type_register_static(AGS_TYPE_LINE,
						       "AgsMixerInputLine", &ags_mixer_input_line_info,
						       0);

    g_type_add_interface_static(ags_type_mixer_input_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_mixer_input_line);
}

void
ags_mixer_input_line_class_init(AgsMixerInputLineClass *mixer_input_line)
{
  AgsLineClass *line;

  ags_mixer_input_line_parent_class = g_type_class_peek_parent(mixer_input_line);

  /* AgsLineClass */
  line = AGS_LINE_CLASS(mixer_input_line);

  line->set_channel = ags_mixer_input_line_set_channel;
}

void
ags_mixer_input_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_mixer_input_line_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_mixer_input_line_connect;
  connectable->disconnect = ags_mixer_input_line_disconnect;
}

void
ags_mixer_input_line_init(AgsMixerInputLine *mixer_input_line)
{
  mixer_input_line->flags = 0;

  mixer_input_line->volume = (GtkVScale *) gtk_vscale_new_with_range(0.0, 2.00, 0.025);
  gtk_range_set_value((GtkRange *) mixer_input_line->volume, 1.0);
  gtk_range_set_inverted((GtkRange *) mixer_input_line->volume, TRUE);
  gtk_scale_set_digits((GtkScale *) mixer_input_line->volume, 3);
  gtk_widget_set_size_request((GtkWidget *) mixer_input_line->volume, -1, 100);
  gtk_table_attach(AGS_LINE(mixer_input_line)->table,
		   (GtkWidget *) mixer_input_line->volume,
		   0, 1,
		   1, 2,
		   GTK_EXPAND, GTK_EXPAND,
		   0, 0);
}

void
ags_mixer_input_line_destroy(GtkObject *object)
{
}

void
ags_mixer_input_line_connect(AgsConnectable *connectable)
{
  AgsMixerInputLine *mixer_input_line;

  mixer_input_line = AGS_MIXER_INPUT_LINE(connectable);

  if((AGS_LINE_CONNECTED & (AGS_LINE(mixer_input_line)->flags)) != 0){
    return;
  }

  ags_mixer_input_line_parent_connectable_interface->connect(connectable);
}

void
ags_mixer_input_line_disconnect(AgsConnectable *connectable)
{
  ags_mixer_input_line_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_mixer_input_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  AgsMixerInputLine *mixer_input_line;

  AGS_LINE_CLASS(ags_mixer_input_line_parent_class)->set_channel(line, channel);

  mixer_input_line = AGS_MIXER_INPUT_LINE(line);

  if(line->channel != NULL){
    mixer_input_line->flags &= (~AGS_MIXER_INPUT_LINE_MAPPED_RECALL);
  }

  if(channel != NULL)
    ags_mixer_input_line_map_recall(mixer_input_line);
}

void
ags_mixer_input_line_map_recall(AgsMixerInputLine *mixer_input_line)
{
  AgsMixer *mixer;
  AgsLine *line;
  AgsAudio *audio;
  AgsChannel *source;
  AgsRecallContainer *play_volume_channel_container;
  AgsVolumeChannel *volume_channel;
  AgsVolumeChannelRun *volume_channel_run;
  guint i;

  line = AGS_LINE(mixer_input_line);

  audio = AGS_AUDIO(line->channel->audio);

  mixer = AGS_MIXER(audio->machine);

  source = line->channel;

  if((AGS_MIXER_INPUT_LINE_MAPPED_RECALL & (mixer_input_line->flags)) == 0){
    mixer_input_line->flags |= AGS_MIXER_INPUT_LINE_MAPPED_RECALL;

    /* volume */
    /* recall for channel->play */
    play_volume_channel_container = ags_recall_container_new();
    ags_channel_add_recall_container(source, (GObject *) play_volume_channel_container);

    /* AgsVolumeChannel */
    volume_channel = (AgsVolumeChannel *) g_object_new(AGS_TYPE_VOLUME_CHANNEL,
						       "devout", audio->devout,
						       "source", source,
						       "recall_container", play_volume_channel_container,
						       NULL);
							      
    AGS_RECALL(volume_channel)->flags |= (AGS_RECALL_TEMPLATE |
					  AGS_RECALL_PLAYBACK |
					  AGS_RECALL_PROPAGATE_DONE |
					  AGS_RECALL_OUTPUT_ORIENTATED);
    ags_channel_add_recall(source, (GObject *) volume_channel, TRUE);

    /* AgsVolumeChannelRun */
    volume_channel_run = (AgsVolumeChannelRun *) g_object_new(AGS_TYPE_VOLUME_CHANNEL_RUN,
							      "devout", audio->devout,
							      "recall_channel", volume_channel,
							      "source", source,
							      "recall_container", play_volume_channel_container,
							      "volume", &(GTK_RANGE(mixer_input_line->volume)->adjustment->value),
							      NULL);
    
    AGS_RECALL(volume_channel_run)->flags |= (AGS_RECALL_TEMPLATE |
					      AGS_RECALL_PLAYBACK |
					      AGS_RECALL_PROPAGATE_DONE |
					      AGS_RECALL_OUTPUT_ORIENTATED);
    ags_channel_add_recall(source, (GObject *) volume_channel_run, TRUE);

    if(GTK_WIDGET_VISIBLE(mixer))
      ags_connectable_connect(AGS_CONNECTABLE(volume_channel_run));
  }
}

AgsMixerInputLine*
ags_mixer_input_line_new(AgsChannel *channel)
{
  AgsMixerInputLine *mixer_input_line;

  mixer_input_line = (AgsMixerInputLine *) g_object_new(AGS_TYPE_MIXER_INPUT_LINE,
							"channel", channel,
							NULL);

  return(mixer_input_line);
}

