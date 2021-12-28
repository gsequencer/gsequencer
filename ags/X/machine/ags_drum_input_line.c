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

#include <ags/X/machine/ags_drum_input_line.h>
#include <ags/X/machine/ags_drum_input_line_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_line_callbacks.h>
#include <ags/X/ags_line_member.h>

#include <ags/X/machine/ags_drum.h>

#include <ags/object/ags_config.h>

void ags_drum_input_line_class_init(AgsDrumInputLineClass *drum_input_line);
void ags_drum_input_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_drum_input_line_init(AgsDrumInputLine *drum_input_line);
void ags_drum_input_line_finalize(GObject *gobject);

void ags_drum_input_line_connect(AgsConnectable *connectable);
void ags_drum_input_line_disconnect(AgsConnectable *connectable);

void ags_drum_input_line_set_channel(AgsLine *line, AgsChannel *channel);
void ags_drum_input_line_group_changed(AgsLine *line);
void ags_drum_input_line_map_recall(AgsLine *line,
				    guint output_pad_start);

/**
 * SECTION:ags_drum_input_line
 * @short_description: drum sequencer input line
 * @title: AgsDrumInputLine
 * @section_id:
 * @include: ags/X/machine/ags_drum_input_line.h
 *
 * The #AgsDrumInputLine is a composite widget to act as drum sequencer input line.
 */

static gpointer ags_drum_input_line_parent_class = NULL;
static AgsConnectableInterface *ags_drum_input_line_parent_connectable_interface;

extern GHashTable *ags_line_indicator_queue_draw;

GType
ags_drum_input_line_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_drum_input_line = 0;

    static const GTypeInfo ags_drum_input_line_info = {
      sizeof(AgsDrumInputLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_drum_input_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDrumInputLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_drum_input_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_drum_input_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_drum_input_line = g_type_register_static(AGS_TYPE_LINE,
						      "AgsDrumInputLine", &ags_drum_input_line_info,
						      0);

    g_type_add_interface_static(ags_type_drum_input_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_drum_input_line);
  }

  return g_define_type_id__volatile;
}

void
ags_drum_input_line_class_init(AgsDrumInputLineClass *drum_input_line)
{
  GObjectClass *gobject;
  AgsLineClass *line;

  ags_drum_input_line_parent_class = g_type_class_peek_parent(drum_input_line);

  /* GObjectClass */
  gobject = (GObjectClass *) drum_input_line;

  gobject->finalize = ags_drum_input_line_finalize;
  
  /* AgsLineClass */
  line = AGS_LINE_CLASS(drum_input_line);

  line->set_channel = ags_drum_input_line_set_channel;
  
  line->group_changed = ags_drum_input_line_group_changed;

  line->map_recall = ags_drum_input_line_map_recall;
}

void
ags_drum_input_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_drum_input_line_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_drum_input_line_connect;
  connectable->disconnect = ags_drum_input_line_disconnect;
}

void
ags_drum_input_line_init(AgsDrumInputLine *drum_input_line)
{
  AgsLineMember *line_member;
  GtkWidget *widget;

  /* volume indicator */
  line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
					       "widget-type", AGS_TYPE_VINDICATOR,
					       "plugin-name", "ags-fx-peak",
					       "specifier", "./peak[0]",
					       "control-port", "1/1",
					       NULL);
  line_member->flags |= (AGS_LINE_MEMBER_PLAY_CALLBACK_WRITE |
			 AGS_LINE_MEMBER_RECALL_CALLBACK_WRITE);
  ags_expander_add(AGS_LINE(drum_input_line)->expander,
		   GTK_WIDGET(line_member),
		   0, 0,
		   1, 1);
  widget = gtk_bin_get_child(GTK_BIN(line_member));
  AGS_LINE(drum_input_line)->indicator = widget;
  g_hash_table_insert(ags_line_indicator_queue_draw,
		      widget, ags_line_indicator_queue_draw_timeout);
  g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0, (GSourceFunc) ags_line_indicator_queue_draw_timeout, (gpointer) widget);

  //TODO:JK: fix me
  //  g_object_set(G_OBJECT(line_member),
  //	       "port-data", (gpointer) &(adjustment->value),
  //	       NULL);

  /* volume control */
  line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
					       "widget-type", GTK_TYPE_SCALE,
					       "widget-orientation", GTK_ORIENTATION_VERTICAL,
					       "plugin-name", "ags-fx-volume",
					       "specifier", "./volume[0]",
					       "control-port", "2/2",
					       NULL);
  ags_expander_add(AGS_LINE(drum_input_line)->expander,
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

  //  g_object_set(G_OBJECT(line_member),
  //	       "port-data", (gpointer) &(GTK_RANGE(widget)->adjustment->value),
  //	       NULL);
}

void
ags_drum_input_line_finalize(GObject *gobject)
{
  AgsDrumInputLine *drum_input_line;

  drum_input_line = (AgsDrumInputLine *) gobject;

  g_hash_table_remove(ags_line_indicator_queue_draw,
		      drum_input_line);
  
  /* call parent */
  G_OBJECT_CLASS(ags_drum_input_line_parent_class)->finalize(gobject);
}

void
ags_drum_input_line_connect(AgsConnectable *connectable)
{
  AgsDrumInputLine *drum_input_line;

  drum_input_line = AGS_DRUM_INPUT_LINE(connectable);

  if((AGS_LINE_CONNECTED & (AGS_LINE(drum_input_line)->flags)) != 0){
    return;
  }
  
  ags_drum_input_line_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_drum_input_line_disconnect(AgsConnectable *connectable)
{
  AgsDrumInputLine *drum_input_line;

  drum_input_line = AGS_DRUM_INPUT_LINE(connectable);

  if((AGS_LINE_CONNECTED & (AGS_LINE(drum_input_line)->flags)) != 0){
    return;
  }
  
  ags_drum_input_line_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_drum_input_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  AgsChannel *old_channel;
  AgsRecycling *first_recycling;
  AgsAudioSignal *template;
  
  GObject *output_soundcard;

  GList *start_list;
  
  guint nth_line;

  if(line->channel != NULL){
    old_channel = line->channel;
  }else{
    old_channel = NULL;
  }

  /* call parent */
  AGS_LINE_CLASS(ags_drum_input_line_parent_class)->set_channel(line,
								channel);

  if(channel != NULL){
    ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_PLAYBACK));
    
    /* get some fields */
    g_object_get(channel,
		 "output-soundcard", &output_soundcard,
		 "first-recycling", &first_recycling,
		 "line", &nth_line,
		 NULL);

    if(first_recycling != NULL){
      g_object_unref(first_recycling);
    }
    
#ifdef AGS_DEBUG
    g_message("ags_drum_input_line_set_channel - channel: %u",
	      nth_line);
#endif

    /* get some fields */
    g_object_get(first_recycling,
		 "audio-signal", &start_list,
		 NULL);
    
    template = ags_audio_signal_get_template(start_list);
    
    /* create audio signal */
    if(output_soundcard != NULL &&
       template == NULL){
      AgsAudioSignal *audio_signal;

      audio_signal = ags_audio_signal_new(output_soundcard,
					  (GObject *) first_recycling,
					  NULL);
      audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
      ags_recycling_add_audio_signal(first_recycling,
				     audio_signal);
    }else{
      g_object_unref(template);
    }

    /* reset edit button */
    if(old_channel == NULL &&
       nth_line == 0){
      AgsDrum *drum;

      GList *list;

      drum = (AgsDrum *) gtk_widget_get_ancestor(GTK_WIDGET(line),
						 AGS_TYPE_DRUM);

      if(drum != NULL){
	list = gtk_container_get_children((GtkContainer *) drum->input_pad);

	drum->selected_pad = AGS_DRUM_INPUT_PAD(list->data);
	drum->selected_edit_button = drum->selected_pad->edit;
	gtk_toggle_button_set_active((GtkToggleButton *) drum->selected_edit_button,
				     TRUE);
	
	g_list_free(list);
      }
    }

    g_list_free_full(start_list,
		     g_object_unref);

    if(output_soundcard != NULL){
      g_object_unref(output_soundcard);
    }
  }
}

void
ags_drum_input_line_group_changed(AgsLine *line)
{
  AgsDrum *drum;

  drum = (AgsDrum *) gtk_widget_get_ancestor(GTK_WIDGET(line), AGS_TYPE_DRUM);
  ags_pattern_box_set_pattern(drum->pattern_box);
}

void
ags_drum_input_line_map_recall(AgsLine *line,
			       guint output_pad_start)
{
  AgsDrum *drum;
  AgsAudio *audio;
  AgsChannel *source;

  GList *start_recall;

  guint pad, audio_channel;
  gint position;
  
  if((AGS_LINE_MAPPED_RECALL & (line->flags)) != 0 ||
     (AGS_LINE_PREMAPPED_RECALL & (line->flags)) != 0){
    return;
  }

  drum = (AgsDrum *) gtk_widget_get_ancestor(GTK_WIDGET(line),
					     AGS_TYPE_DRUM);
  
  audio = AGS_MACHINE(drum)->audio;

  source = line->channel;

  position = 0;
  
  /* get some fields */
  g_object_get(source,
	       "pad", &pad,
	       "audio-channel", &audio_channel,
	       NULL);

  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       drum->playback_play_container, drum->playback_recall_container,
				       "ags-fx-playback",
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

  /* ags-fx-pattern */
  start_recall = ags_fx_factory_create(audio,
				       drum->pattern_play_container, drum->pattern_recall_container,
				       "ags-fx-pattern",
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

  /* ags-fx-notation */
  start_recall = ags_fx_factory_create(audio,
				       drum->notation_play_container, drum->notation_recall_container,
				       "ags-fx-notation",
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

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(audio,
				       drum->volume_play_container, drum->volume_recall_container,
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

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(audio,
				       drum->envelope_play_container, drum->envelope_recall_container,
				       "ags-fx-envelope",
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
				       drum->peak_play_container, drum->peak_recall_container,
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

  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(audio,
				       drum->buffer_play_container, drum->buffer_recall_container,
				       "ags-fx-buffer",
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
  AGS_LINE_CLASS(ags_drum_input_line_parent_class)->map_recall(line,
							       output_pad_start);
}

/**
 * ags_drum_input_line_new:
 * @channel: the assigned channel
 *
 * Creates an #AgsDrumInputLine
 *
 * Returns: a new #AgsDrumInputLine
 *
 * Since: 3.0.0
 */
AgsDrumInputLine*
ags_drum_input_line_new(AgsChannel *channel)
{
  AgsDrumInputLine *drum_input_line;

  drum_input_line = (AgsDrumInputLine *) g_object_new(AGS_TYPE_DRUM_INPUT_LINE,
						      "channel", channel,
						      NULL);

  return(drum_input_line);
}
