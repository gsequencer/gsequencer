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

#include <ags/app/machine/ags_mixer_input_line.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_line_callbacks.h>

#include <ags/app/machine/ags_mixer.h>

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
 * @include: ags/app/machine/ags_mixer_input_line.h
 *
 * The #AgsMixerInputLine is a composite widget to act as mixer input line.
 */

static gpointer ags_mixer_input_line_parent_class = NULL;
static AgsConnectableInterface *ags_mixer_input_line_parent_connectable_interface;

extern GHashTable *ags_line_indicator_queue_draw;

GType
ags_mixer_input_line_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_mixer_input_line = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_mixer_input_line);
  }

  return g_define_type_id__volatile;
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

  /* volume indicator */
  line_member =
    mixer_input_line->volume_indicator = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
									"widget-type", AGS_TYPE_VINDICATOR,
									"margin-end", AGS_UI_PROVIDER_DEFAULT_MARGIN_END,
									"plugin-name", "ags-fx-peak",
									"specifier", "./peak[0]",
									"control-port", "1/1",
									NULL);
  line_member->flags |= (AGS_LINE_MEMBER_PLAY_CALLBACK_WRITE |
			 AGS_LINE_MEMBER_RECALL_CALLBACK_WRITE);
  ags_expander_add(AGS_LINE(mixer_input_line)->expander,
		   GTK_WIDGET(line_member),
		   0, 0,
		   1, 1);
  widget = gtk_bin_get_child(GTK_BIN(line_member));
  AGS_LINE(mixer_input_line)->indicator = widget;
  g_hash_table_insert(ags_line_indicator_queue_draw,
		      widget, ags_line_indicator_queue_draw_timeout);
  g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0, (GSourceFunc) ags_line_indicator_queue_draw_timeout, (gpointer) widget);

  /* volume */
  line_member =
    mixer_input_line->volume_control = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
								      "widget-type", GTK_TYPE_SCALE,
								      "widget-orientation", GTK_ORIENTATION_VERTICAL,
								      "margin-end", AGS_UI_PROVIDER_DEFAULT_MARGIN_END,
								      "plugin-name", "ags-fx-volume",
								      "specifier", "./volume[0]",
								      "control-port", "2/2",
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
  AgsMixerInputLine *mixer_input_line;

  mixer_input_line = AGS_MIXER_INPUT_LINE(connectable);

  if((AGS_LINE_CONNECTED & (AGS_LINE(mixer_input_line)->flags)) == 0){
    return;
  }

  ags_mixer_input_line_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_mixer_input_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  AGS_LINE_CLASS(ags_mixer_input_line_parent_class)->set_channel(line, channel);

  /* empty */
}

void
ags_mixer_input_line_map_recall(AgsLine *line,
				guint output_pad_start)
{
  AgsMixer *mixer;
  AgsAudio *audio;
  AgsChannel *source;

  GList *start_recall;

  guint pad, audio_channel;
  gint position;
  
  if((AGS_LINE_MAPPED_RECALL & (line->flags)) != 0 ||
     (AGS_LINE_PREMAPPED_RECALL & (line->flags)) != 0){
    return;
  }

  mixer = (AgsMixer *) gtk_widget_get_ancestor((GtkWidget *) line,
					       AGS_TYPE_MIXER);
  
  audio = AGS_MACHINE(mixer)->audio;

  source = line->channel;

  position = 0;
  
  /* get some fields */
  g_object_get(source,
	       "pad", &pad,
	       "audio-channel", &audio_channel,
	       NULL);

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(audio,
				       mixer->volume_play_container, mixer->volume_recall_container,
				       "ags-fx-volume",
				       NULL,
				       NULL,
				       audio_channel, audio_channel + 1,
				       pad, pad + 1,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT),
				       0);

  /* unref */
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-peak */
  start_recall = ags_fx_factory_create(audio,
				       mixer->peak_play_container, mixer->peak_recall_container,
				       "ags-fx-peak",
				       NULL,
				       NULL,
				       audio_channel, audio_channel + 1,
				       pad, pad + 1,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT),
				       0);
  
  /* unref */
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* call parent */
  AGS_LINE_CLASS(ags_mixer_input_line_parent_class)->map_recall(line,
								output_pad_start);
}

/**
 * ags_mixer_input_line_new:
 * @channel: the assigned channel
 *
 * Create a new instance of #AgsMixerInputLine
 *
 * Returns: the new #AgsMixerInputLine
 *
 * Since: 3.0.0
 */
AgsMixerInputLine*
ags_mixer_input_line_new(AgsChannel *channel)
{
  AgsMixerInputLine *mixer_input_line;

  mixer_input_line = (AgsMixerInputLine *) g_object_new(AGS_TYPE_MIXER_INPUT_LINE,
							"channel", channel,
							NULL);

  return(mixer_input_line);
}

