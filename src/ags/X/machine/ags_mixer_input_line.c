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

#include <ags/audio/recall/ags_peak_channel.h>
#include <ags/audio/recall/ags_peak_channel_run.h>
#include <ags/audio/recall/ags_volume_channel.h>
#include <ags/audio/recall/ags_volume_channel_run.h>

#include <ags/widget/ags_vindicator.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_line_callbacks.h>
#include <ags/X/ags_line_member.h>

#include <ags/X/machine/ags_mixer.h>

void ags_mixer_input_line_class_init(AgsMixerInputLineClass *mixer_input_line);
void ags_mixer_input_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_mixer_input_line_init(AgsMixerInputLine *mixer_input_line);
void ags_mixer_input_line_connect(AgsConnectable *connectable);
void ags_mixer_input_line_disconnect(AgsConnectable *connectable);

void ags_mixer_input_line_set_channel(AgsLine *line, AgsChannel *channel);
void ags_mixer_input_line_map_recall(AgsLine *line,
				     guint output_pad_start);

/**
 * SECTION:ags_mixer_input_line
 * @short_description: mixer input line
 * @title: AgsMixerInputLine
 * @section_id:
 * @include: ags/X/machine/ags_mixer_input_line.h
 *
 * The #AgsMixerInputLine is a composite widget to act as mixer input line.
 */

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
  line->map_recall = ags_mixer_input_line_map_recall;
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
  GtkAdjustment *adjustment;

  /* volume indicator */
  line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
					       "widget-type\0", AGS_TYPE_VINDICATOR,
					       "plugin-name\0", "ags-peak\0",
					       "specifier\0", "./peak[0]\0",
					       "control-port\0", "1/1\0",
					       NULL);
  line_member->flags |= (AGS_LINE_MEMBER_PLAY_CALLBACK_WRITE |
			 AGS_LINE_MEMBER_RECALL_CALLBACK_WRITE);
  ags_expander_add(AGS_LINE(mixer_input_line)->expander,
		   GTK_WIDGET(line_member),
		   0, 0,
		   1, 1);
  widget = gtk_bin_get_child(GTK_BIN(line_member));

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 10.0, 1.0, 1.0, 10.0);
  g_object_set(widget,
	       "adjustment\0", adjustment,
	       NULL);

  gtk_widget_set_size_request(widget,
			      16, 100);
  gtk_widget_queue_draw(widget);

  /* volume */
  line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
					       "widget-type\0", GTK_TYPE_VSCALE,
					       "plugin-name\0", "ags-volume\0",
					       "specifier\0", "./volume[0]\0",
					       "control-port\0", "1/1\0",
					       NULL);
  ags_expander_add(AGS_LINE(mixer_input_line)->expander,
		   GTK_WIDGET(line_member),
		   1, 0,
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

  /* empty */
}

void
ags_mixer_input_line_map_recall(AgsLine *line,
				guint output_pad_start)
{
  AgsMixer *mixer;
  AgsMixerInputLine *mixer_input_line;
  AgsAudio *audio;
  AgsChannel *source;
  AgsRecallHandler *recall_handler;

  AgsPeakChannelRun *recall_peak_channel_run, *play_peak_channel_run;

  GList *list;
  guint i;

  if((AGS_LINE_MAPPED_RECALL & (line->flags)) != 0 ||
     (AGS_LINE_PREMAPPED_RECALL & (line->flags)) != 0){
    return;
  }

  mixer_input_line = AGS_MIXER_INPUT_LINE(line);

  audio = AGS_AUDIO(line->channel->audio);

  mixer = AGS_MIXER(audio->machine);

  source = line->channel;

  /* ags-peak */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-peak\0",
			    source->audio_channel, source->audio_channel + 1, 
			    source->pad, source->pad + 1,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* play - connect run_post */
  list = ags_recall_template_find_type(source->play,
				       AGS_TYPE_PEAK_CHANNEL_RUN);

  if(list != NULL){
    play_peak_channel_run = AGS_PEAK_CHANNEL_RUN(list->data);

    recall_handler = (AgsRecallHandler *) malloc(sizeof(AgsRecallHandler));

    recall_handler->signal_name = "run-post\0";
    recall_handler->callback = G_CALLBACK(ags_line_peak_run_post_callback);
    recall_handler->data = (gpointer) line;

    ags_recall_add_handler(AGS_RECALL(play_peak_channel_run), recall_handler);
  }

  /* recall - connect run_post */
  list = ags_recall_template_find_type(source->recall,
				       AGS_TYPE_PEAK_CHANNEL_RUN);

  if(list != NULL){
    recall_peak_channel_run = AGS_PEAK_CHANNEL_RUN(list->data);

    recall_handler = (AgsRecallHandler *) malloc(sizeof(AgsRecallHandler));

    recall_handler->signal_name = "run-post\0";
    recall_handler->callback = G_CALLBACK(ags_line_peak_run_post_callback);
    recall_handler->data = (gpointer) line;

    ags_recall_add_handler(AGS_RECALL(recall_peak_channel_run), recall_handler);
  }

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

  /* call parent */
  AGS_LINE_CLASS(ags_mixer_input_line_parent_class)->map_recall(line,
								output_pad_start);
}

/**
 * ags_mixer_input_line_new:
 * @channel: the assigned channel
 *
 * Creates an #AgsMixerInputLine
 *
 * Returns: a new #AgsMixerInputLine
 *
 * Since: 0.4
 */
AgsMixerInputLine*
ags_mixer_input_line_new(AgsChannel *channel)
{
  AgsMixerInputLine *mixer_input_line;

  mixer_input_line = (AgsMixerInputLine *) g_object_new(AGS_TYPE_MIXER_INPUT_LINE,
							"channel\0", channel,
							NULL);

  return(mixer_input_line);
}

