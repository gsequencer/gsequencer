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

#include <ags-lib/object/ags_connectable.h>

#include <ags/plugin/ags_plugin_stock.h>

#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_volume_channel.h>
#include <ags/audio/recall/ags_volume_channel_run.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_line_member.h>

#include <ags/X/machine/ags_mixer.h>

void ags_mixer_input_line_class_init(AgsMixerInputLineClass *mixer_input_line);
void ags_mixer_input_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_mixer_input_line_init(AgsMixerInputLine *mixer_input_line);
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
						       "AgsMixerInputLine\0", &ags_mixer_input_line_info,
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
  AgsLineMember *line_member;
  GtkWidget *widget;

  mixer_input_line->flags = 0;

  /* volume */
  line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
					       "widget-type\0", GTK_TYPE_VSCALE,
					       "plugin-name\0", "ags-volume\0",
					       //					       "specifier\0", g_type_name(AGS_TYPE_RECALL_CHANNEL),
					       //					       "control-port\0", g_strdup("adjustmet[0].value\0"),
					       //					       "port-data-length\0", sizeof(gdouble),
					       NULL);
  ags_expander_add(AGS_LINE(mixer_input_line)->expander,
		   GTK_WIDGET(line_member),
		   0, 0,
		   1, 1);

  widget = gtk_bin_get_child(GTK_BIN(line_member));

  gtk_scale_set_digits(GTK_SCALE(widget),
		       3);

  gtk_range_set_range(GTK_RANGE(widget),
		      0.0, 2.00);
  gtk_range_set_increments(GTK_RANGE(widget),
			   0.025, 0.1);
  gtk_range_set_value(GTK_RANGE(widget),
		      1.0);
  gtk_range_set_inverted(GTK_RANGE(widget),
			 TRUE);

  gtk_widget_set_size_request(widget,
			      -1, 100);
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
  guint i;

  line = AGS_LINE(mixer_input_line);

  audio = AGS_AUDIO(line->channel->audio);

  mixer = AGS_MIXER(audio->machine);

  source = line->channel;

  if((AGS_MIXER_INPUT_LINE_MAPPED_RECALL & (mixer_input_line->flags)) == 0){
    mixer_input_line->flags |= AGS_MIXER_INPUT_LINE_MAPPED_RECALL;

    /* ags-volume */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-volume\0",
			      0, audio->audio_channels,
			      source->pad, source->pad + 1,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }
}

AgsMixerInputLine*
ags_mixer_input_line_new(AgsChannel *channel)
{
  AgsMixerInputLine *mixer_input_line;

  mixer_input_line = (AgsMixerInputLine *) g_object_new(AGS_TYPE_MIXER_INPUT_LINE,
							"channel\0", channel,
							NULL);

  return(mixer_input_line);
}

