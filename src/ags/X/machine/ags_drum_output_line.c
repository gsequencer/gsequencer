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

#include <ags/X/machine/ags_drum_output_line.h>
#include <ags/X/machine/ags_drum_output_line_callbacks.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

#include <ags/audio/ags_recall_factory.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_stream_channel.h>
#include <ags/audio/recall/ags_stream_channel_run.h>
#include <ags/audio/recall/ags_loop_channel.h>
#include <ags/audio/recall/ags_loop_channel_run.h>

#include <ags/X/ags_window.h>

#include <ags/X/machine/ags_drum.h>

#include <math.h>

void ags_drum_output_line_class_init(AgsDrumOutputLineClass *drum_output_line);
void ags_drum_output_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_drum_output_line_plugin_interface_init(AgsPluginInterface *plugin);
void ags_drum_output_line_init(AgsDrumOutputLine *drum_output_line);
void ags_drum_output_line_destroy(GtkObject *object);
void ags_drum_output_line_connect(AgsConnectable *connectable);
void ags_drum_output_line_disconnect(AgsConnectable *connectable);
gchar* ags_drum_output_line_get_name(AgsPlugin *plugin);
void ags_drum_output_line_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_drum_output_line_get_xml_type(AgsPlugin *plugin);
void ags_drum_output_line_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_drum_output_line_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_drum_output_line_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_drum_output_line_set_channel(AgsLine *line, AgsChannel *channel);
void ags_drum_output_line_map_recall(AgsLine *line,
				     guint output_pad_start);

/**
 * SECTION:ags_drum_output_line
 * @short_description: drum sequencer output line
 * @title: AgsDrumOutputLine
 * @section_id:
 * @include: ags/X/machine/ags_drum_output_line.h
 *
 * The #AgsDrumOutputLine is a composite widget to act as drum sequencer output line.
 */

static gpointer ags_drum_output_line_parent_class = NULL;
static AgsConnectableInterface *ags_drum_output_line_parent_connectable_interface;

GType
ags_drum_output_line_get_type()
{
  static GType ags_type_drum_output_line = 0;

  if(!ags_type_drum_output_line){
    static const GTypeInfo ags_drum_output_line_info = {
      sizeof(AgsDrumOutputLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_drum_output_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDrumOutputLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_drum_output_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_drum_output_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_drum_output_line_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_drum_output_line = g_type_register_static(AGS_TYPE_LINE,
						       "AgsDrumOutputLine\0", &ags_drum_output_line_info,
						       0);

    g_type_add_interface_static(ags_type_drum_output_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_drum_output_line,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_drum_output_line);
}

void
ags_drum_output_line_class_init(AgsDrumOutputLineClass *drum_output_line)
{
  AgsLineClass *line;

  ags_drum_output_line_parent_class = g_type_class_peek_parent(drum_output_line);

  /* AgsLineClass */
  line = AGS_LINE_CLASS(drum_output_line);

  line->set_channel = ags_drum_output_line_set_channel;
  line->map_recall = ags_drum_output_line_map_recall;
}

void
ags_drum_output_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_drum_output_line_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_drum_output_line_connect;
  connectable->disconnect = ags_drum_output_line_disconnect;
}

void
ags_drum_output_line_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_drum_output_line_get_name;
  plugin->set_name = ags_drum_output_line_set_name;
  plugin->get_xml_type = ags_drum_output_line_get_xml_type;
  plugin->set_xml_type = ags_drum_output_line_set_xml_type;
  plugin->read = ags_drum_output_line_read;
  plugin->write = ags_drum_output_line_write;
}

void
ags_drum_output_line_init(AgsDrumOutputLine *drum_output_line)
{
  g_signal_connect_after((GObject *) drum_output_line, "parent_set\0",
			 G_CALLBACK(ags_drum_output_line_parent_set_callback), NULL);

  drum_output_line->xml_type = "ags-drum-output-line\0";
}

void
ags_drum_output_line_destroy(GtkObject *object)
{
}

void
ags_drum_output_line_connect(AgsConnectable *connectable)
{
  AgsDrumOutputLine *drum_output_line;

  drum_output_line = AGS_DRUM_OUTPUT_LINE(connectable);

  if((AGS_LINE_CONNECTED & (AGS_LINE(drum_output_line)->flags)) != 0){
    return;
  }
  
  ags_drum_output_line_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_drum_output_line_disconnect(AgsConnectable *connectable)
{
  ags_drum_output_line_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

gchar*
ags_drum_output_line_get_name(AgsPlugin *plugin)
{
  return(AGS_DRUM_OUTPUT_LINE(plugin)->name);
}

void
ags_drum_output_line_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_DRUM_OUTPUT_LINE(plugin)->name = name;
}

gchar*
ags_drum_output_line_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_DRUM_OUTPUT_LINE(plugin)->xml_type);
}

void
ags_drum_output_line_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_DRUM_OUTPUT_LINE(plugin)->xml_type = xml_type;
}

void
ags_drum_output_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  AgsDrumOutputLine *drum_output_line;

  AGS_LINE_CLASS(ags_drum_output_line_parent_class)->set_channel(line, channel);

  drum_output_line = AGS_DRUM_OUTPUT_LINE(line);

  if(channel != NULL){
    AgsSoundcard *soundcard;
    AgsAudioSignal *audio_signal;
    gdouble delay;
    guint stop;

    if(channel->audio != NULL &&
       AGS_AUDIO(channel->audio)->soundcard != NULL){
      soundcard = AGS_SOUNDCARD(AGS_AUDIO(channel->audio)->soundcard);

      audio_signal = ags_audio_signal_new(soundcard,
					  channel->first_recycling,
					  NULL);
      audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
      ags_recycling_add_audio_signal(channel->first_recycling,
				     audio_signal);
    }
  }
}

void
ags_drum_output_line_map_recall(AgsLine *line,
				guint output_pad_start)
{
  AgsAudio *audio;

  AgsChannel *output;
  AgsDelayAudio *recall_delay_audio;
  AgsCountBeatsAudioRun *recall_count_beats_audio_run;

  GList *list;

  if((AGS_LINE_MAPPED_RECALL & (line->flags)) != 0 ||
     (AGS_LINE_PREMAPPED_RECALL & (line->flags)) != 0){
    return;
  }

  output = line->channel;
  audio = AGS_AUDIO(output->audio);

  /* get some recalls */
  list = ags_recall_find_type(audio->play, AGS_TYPE_DELAY_AUDIO);

  if(list != NULL){
    recall_delay_audio = AGS_DELAY_AUDIO(list->data);
  }else{
    recall_delay_audio = NULL;
  }

  list = ags_recall_find_type(audio->play, AGS_TYPE_COUNT_BEATS_AUDIO_RUN);

  if(list != NULL){
    recall_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(list->data);
  }else{
    recall_count_beats_audio_run = NULL;
  }

  /* ags-stream */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-stream\0",
			    output->audio_channel, output->audio_channel + 1,
			    output->pad, output->pad + 1,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL | 
			     AGS_RECALL_FACTORY_ADD),
			    0);

  AGS_LINE_CLASS(ags_drum_output_line_parent_class)->map_recall(line,
								output_pad_start);
}

void
ags_drum_output_line_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsDrumOutputLine *gobject;

  gobject = AGS_DRUM_OUTPUT_LINE(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));
}

xmlNode*
ags_drum_output_line_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsDrumOutputLine *drum_output_line;
  xmlNode *node;
  gchar *id;

  drum_output_line = AGS_DRUM_OUTPUT_LINE(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-drum-output-line\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", drum_output_line,
				   NULL));

  return(node);
}

/**
 * ags_drum_output_line_new:
 * @channel: the assigned channel
 *
 * Creates an #AgsDrumOutputLine
 *
 * Returns: a new #AgsDrumOutputLine
 *
 * Since: 0.4
 */
AgsDrumOutputLine*
ags_drum_output_line_new(AgsChannel *channel)
{
  AgsDrumOutputLine *drum_output_line;

  drum_output_line = (AgsDrumOutputLine *) g_object_new(AGS_TYPE_DRUM_OUTPUT_LINE,
							"channel\0", channel,
							NULL);

  return(drum_output_line);
}
