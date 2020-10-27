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

#include <ags/X/machine/ags_panel_input_line.h>
#include <ags/X/machine/ags_panel_input_line_callbacks.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_line_callbacks.h>
#include <ags/X/ags_line_member.h>

#include <ags/X/machine/ags_panel.h>

void ags_panel_input_line_class_init(AgsPanelInputLineClass *panel_input_line);
void ags_panel_input_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_panel_input_line_init(AgsPanelInputLine *panel_input_line);
void ags_panel_input_line_finalize(GObject *gobject);

void ags_panel_input_line_connect(AgsConnectable *connectable);
void ags_panel_input_line_disconnect(AgsConnectable *connectable);

void ags_panel_input_line_show(GtkWidget *line);
void ags_panel_input_line_show_all(GtkWidget *line);

void ags_panel_input_line_set_channel(AgsLine *line, AgsChannel *channel);
void ags_panel_input_line_group_changed(AgsLine *line);
void ags_panel_input_line_map_recall(AgsLine *line,
				     guint output_pad_start);

/**
 * SECTION:ags_panel_input_line
 * @short_description: panel input line
 * @title: AgsPanelInputLine
 * @section_id:
 * @include: ags/X/machine/ags_panel_input_line.h
 *
 * The #AgsPanelInputLine is a composite widget to act as panel input line.
 */

static gpointer ags_panel_input_line_parent_class = NULL;
static AgsConnectableInterface *ags_panel_input_line_parent_connectable_interface;

GType
ags_panel_input_line_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_panel_input_line = 0;

    static const GTypeInfo ags_panel_input_line_info = {
      sizeof(AgsPanelInputLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_panel_input_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPanelInputLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_panel_input_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_panel_input_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_panel_input_line = g_type_register_static(AGS_TYPE_LINE,
						       "AgsPanelInputLine", &ags_panel_input_line_info,
						       0);

    g_type_add_interface_static(ags_type_panel_input_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_panel_input_line);
  }

  return g_define_type_id__volatile;
}

void
ags_panel_input_line_class_init(AgsPanelInputLineClass *panel_input_line)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsLineClass *line;

  ags_panel_input_line_parent_class = g_type_class_peek_parent(panel_input_line);

  /* GObjectClass */
  gobject = (GObjectClass *) panel_input_line;

  gobject->finalize = ags_panel_input_line_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) panel_input_line;

  widget->show = ags_panel_input_line_show;
  widget->show_all = ags_panel_input_line_show_all;

  /* AgsLineClass */
  line = AGS_LINE_CLASS(panel_input_line);

  line->set_channel = ags_panel_input_line_set_channel;
  line->map_recall = ags_panel_input_line_map_recall;
}

void
ags_panel_input_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_panel_input_line_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_panel_input_line_connect;
  connectable->disconnect = ags_panel_input_line_disconnect;
}

void
ags_panel_input_line_init(AgsPanelInputLine *panel_input_line)
{
  AgsLineMember *line_member;

  /* mute line member */
  panel_input_line->soundcard_connection = (GtkLabel *) gtk_label_new("(null)");
  ags_expander_add(AGS_LINE(panel_input_line)->expander,
		   GTK_WIDGET(panel_input_line->soundcard_connection),
		   0, 0,
		   1, 1);

  line_member = (AgsLineMember *) g_object_new(AGS_TYPE_LINE_MEMBER,
					       "widget-type", GTK_TYPE_CHECK_BUTTON,
					       "widget-label", "mute",
					       "plugin-name", "ags-fx-volume",
					       "specifier", "./muted[0]",
					       "control-port", "1/2",
					       NULL);
  ags_expander_add(AGS_LINE(panel_input_line)->expander,
		   GTK_WIDGET(line_member),
		   1, 0,
		   1, 1);
}

void
ags_panel_input_line_finalize(GObject *gobject)
{
  /* empty */

  G_OBJECT_CLASS(ags_panel_input_line_parent_class)->finalize(gobject);
}

void
ags_panel_input_line_connect(AgsConnectable *connectable)
{
  AgsPanelInputLine *panel_input_line;

  panel_input_line = AGS_PANEL_INPUT_LINE(connectable);

  if((AGS_LINE_CONNECTED & (AGS_LINE(panel_input_line)->flags)) != 0){
    return;
  }
  
  ags_panel_input_line_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_panel_input_line_disconnect(AgsConnectable *connectable)
{
  AgsPanelInputLine *panel_input_line;

  panel_input_line = AGS_PANEL_INPUT_LINE(connectable);

  if((AGS_LINE_CONNECTED & (AGS_LINE(panel_input_line)->flags)) == 0){
    return;
  }

  ags_panel_input_line_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_panel_input_line_show(GtkWidget *line)
{
  GTK_WIDGET_CLASS(ags_panel_input_line_parent_class)->show(line);

  //  gtk_widget_hide(GTK_WIDGET(AGS_LINE(line)->group));
}

void
ags_panel_input_line_show_all(GtkWidget *line)
{
  GTK_WIDGET_CLASS(ags_panel_input_line_parent_class)->show_all(line);

  //  gtk_widget_hide(GTK_WIDGET(AGS_LINE(line)->group));
}

void
ags_panel_input_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  AgsPanelInputLine *panel_input_line;

  AgsAudio *audio;

  GObject *output_soundcard;
  
  GList *list;

  gchar *device;
  gchar *str;

  gint output_soundcard_channel;

  panel_input_line = AGS_PANEL_INPUT_LINE(line);

  /* call parent */
  AGS_LINE_CLASS(ags_panel_input_line_parent_class)->set_channel(line,
								 channel);

  /* update label */
  g_object_get(channel,
	       "output-soundcard", &output_soundcard,
	       "output-soundcard-channel", &output_soundcard_channel,
	       NULL);

  if(output_soundcard_channel == -1){
    g_object_get(channel,
		 "line", &output_soundcard_channel,
		 NULL);
    
    g_object_set(channel,
		 "output-soundcard-channel", output_soundcard_channel,
		 NULL);
  }

  if(AGS_IS_SOUNDCARD(output_soundcard)){
    device = ags_soundcard_get_device(AGS_SOUNDCARD(output_soundcard));

    /* label */
    str = g_strdup_printf("%s:%s[%d]",
			  G_OBJECT_TYPE_NAME(output_soundcard),
			  device,
			  output_soundcard_channel);
    gtk_label_set_label(panel_input_line->soundcard_connection,
			str);
    
    g_free(str);

    g_object_unref(output_soundcard);
  }
  
#ifdef AGS_DEBUG
  g_message("ags_panel_input_line_set_channel - channel: %u",
	    channel->line);
#endif
}

void
ags_panel_input_line_map_recall(AgsLine *line,
				guint output_pad_start)
{
  AgsPanel *panel;
  AgsAudio *audio;
  AgsChannel *source;

  GList *start_recall;

  guint pad, audio_channel;
  gint position;
  
  if((AGS_LINE_MAPPED_RECALL & (line->flags)) != 0 ||
     (AGS_LINE_PREMAPPED_RECALL & (line->flags)) != 0){
    return;
  }

  panel = (AgsPanel *) gtk_widget_get_ancestor(line,
					       AGS_TYPE_PANEL);
  
  audio = AGS_MACHINE(panel)->audio;

  source = line->channel;

  position = 0;
  
  /* get some fields */
  g_object_get(source,
	       "pad", &pad,
	       "audio-channel", &audio_channel,
	       NULL);

  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       panel->playback_play_container, panel->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       audio_channel, audio_channel + 1,
				       pad, pad + 1,
				       position,
				       (AGS_FX_FACTORY_REMAP),
				       0);

  /* unref */
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(audio,
				       panel->volume_play_container, panel->volume_recall_container,
				       "ags-fx-volume",
				       NULL,
				       NULL,
				       audio_channel, audio_channel + 1,
				       pad, pad + 1,
				       position,
				       (AGS_FX_FACTORY_REMAP),
				       0);

  /* unref */
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* call parent */
  AGS_LINE_CLASS(ags_panel_input_line_parent_class)->map_recall(line,
								output_pad_start);
}

/**
 * ags_panel_input_line_new:
 * @channel: the assigned channel
 *
 * Create a new instance of #AgsPanelInputLine
 *
 * Returns: the new #AgsPanelInputLine
 *
 * Since: 3.0.0
 */
AgsPanelInputLine*
ags_panel_input_line_new(AgsChannel *channel)
{
  AgsPanelInputLine *panel_input_line;

  panel_input_line = (AgsPanelInputLine *) g_object_new(AGS_TYPE_PANEL_INPUT_LINE,
							"channel", channel,
							NULL);

  return(panel_input_line);
}
