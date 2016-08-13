/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/file/ags_audio_file_link.h>

#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_launch.h>
#include <ags/file/ags_file_link.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_audio_signal.h>

#include <ags/audio/file/ags_audio_file.h>

#include <libxml/xpath.h>
#include <libxml/tree.h>

void ags_audio_file_link_class_init(AgsAudioFileLinkClass *audio_file_link);
void ags_audio_file_link_plugin_interface_init(AgsPluginInterface *plugin);
void ags_audio_file_link_init(AgsAudioFileLink *audio_file_link);
void ags_audio_file_link_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_audio_file_link_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_audio_file_link_read(AgsFile *file,
			      xmlNode *node,
			      AgsPlugin *plugin);
xmlNode* ags_audio_file_link_write(AgsFile *file,
				   xmlNode *parent,
				   AgsPlugin *plugin);
void ags_audio_file_link_finalize(GObject *gobject);

void
ags_audio_file_link_read_launch(AgsFileLaunch *file_launch,
				AgsAudioFileLink *audio_file_link);

/**
 * SECTION:ags_audio_file_link
 * @short_description: link objects read of file.
 * @title: AgsAudioFileLink
 * @section_id:
 * @include: ags/file/ags_audio_file_link.h
 *
 * The #AgsAudioFileLink links read objects of file.
 */

enum{
  PROP_0,
  PROP_PRESET,
  PROP_INSTRUMENT,
  PROP_SAMPLE,
  PROP_AUDIO_CHANNEL,
  PROP_TIMESTAMP,
};

static gpointer ags_audio_file_link_parent_class = NULL;
static AgsPluginInterface *ags_audio_file_link_parent_plugin_interface;

static const gchar *ags_audio_file_link_plugin_name = "ags-audio-file-link\0";

GType
ags_audio_file_link_get_type()
{
  static GType ags_type_audio_file_link = 0;

  if(!ags_type_audio_file_link){
    static const GTypeInfo ags_audio_file_link_info = {
      sizeof (AgsAudioFileLinkClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_file_link_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioFileLink),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_file_link_init,
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_audio_file_link_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_file_link = g_type_register_static(AGS_TYPE_FILE_LINK,
						      "AgsAudioFileLink\0",
						      &ags_audio_file_link_info,
						      0);

    g_type_add_interface_static(ags_type_audio_file_link,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return (ags_type_audio_file_link);
}

void
ags_audio_file_link_class_init(AgsAudioFileLinkClass *audio_file_link)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_audio_file_link_parent_class = g_type_class_peek_parent(audio_file_link);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_file_link;

  gobject->set_property = ags_audio_file_link_set_property;
  gobject->get_property = ags_audio_file_link_get_property;

  gobject->finalize = ags_audio_file_link_finalize;

  /* properties */
  /**
   * AgsAudioFileLink:preset:
   *
   * The assigned preset.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_string("preset\0",
				   "the preset\0",
				   "The preset to locate the file\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PRESET,
				  param_spec);

  /**
   * AgsAudioFileLink:instrument:
   *
   * The assigned instrument.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_string("instrument\0",
				   "the instrument\0",
				   "The instrument to locate the file\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INSTRUMENT,
				  param_spec);

  /**
   * AgsAudioFileLink:sample:
   *
   * The assigned sample.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_string("sample\0",
				   "the sample\0",
				   "The sample to locate the file\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLE,
				  param_spec);

  /**
   * AgsAudioFileLink:audio-channel:
   *
   * The assigned audio channel.
   * 
   * Since: 0.4.2
   */
  param_spec = g_param_spec_uint("audio-channel\0",
				 "audio channel to read\0",
				 "The selected audio channel to read\0",
				 0, 256,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);

  /**
   * AgsAudioFileLink:timestamp:
   *
   * The assigned timestamp.
   * 
   * Since: 0.4.2
   */
  param_spec = g_param_spec_object("timestamp\0",
				   "timestamp\0",
				   "The timestamp\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMESTAMP,
				  param_spec);
}

void
ags_audio_file_link_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_audio_file_link_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->read = ags_audio_file_link_read;
  plugin->write = ags_audio_file_link_write;
}

void
ags_audio_file_link_init(AgsAudioFileLink *audio_file_link)
{
  audio_file_link->preset = NULL;
  audio_file_link->instrument = NULL;
  audio_file_link->sample = NULL;
  
  audio_file_link->audio_channel = 0;
  
  audio_file_link->timestamp = NULL;
}

void
ags_audio_file_link_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsAudioFileLink *audio_file_link;

  audio_file_link = AGS_AUDIO_FILE_LINK(gobject);
  
  switch(prop_id){
  case PROP_PRESET:
    {
      char *preset;

      preset = (char *) g_value_get_string(value);

      if(preset == audio_file_link->preset){
	return;
      }

      if(audio_file_link->preset != NULL){
	free(audio_file_link->preset);
      }
	
      audio_file_link->preset = g_strdup(preset);
    }
    break;
  case PROP_INSTRUMENT:
    {
      char *instrument;

      instrument = (char *) g_value_get_string(value);

      if(instrument == audio_file_link->instrument){
	return;
      }

      if(audio_file_link->instrument != NULL){
	free(audio_file_link->instrument);
      }
	
      audio_file_link->instrument = g_strdup(instrument);
    }
    break;
  case PROP_SAMPLE:
    {
      char *sample;

      sample = (char *) g_value_get_string(value);

      if(sample == audio_file_link->sample){
	return;
      }

      if(audio_file_link->sample != NULL){
	free(audio_file_link->sample);
      }
	
      audio_file_link->sample = g_strdup(sample);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      audio_file_link->audio_channel = g_value_get_uint(value);
    }
    break;
  case PROP_TIMESTAMP:
    {
      GObject *timestamp;

      timestamp = (GObject *) g_value_get_object(value);

      if((AgsTimestamp *) timestamp == audio_file_link->timestamp){
	return;
      }

      if(audio_file_link->timestamp != NULL){
	g_object_unref(audio_file_link->timestamp);
      }

      if(timestamp != NULL){
	g_object_ref(timestamp);
      }

      audio_file_link->timestamp = (AgsTimestamp *) timestamp;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_file_link_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsAudioFileLink *audio_file_link;

  audio_file_link = AGS_AUDIO_FILE_LINK(gobject);
  
  switch(prop_id){
  case PROP_PRESET:
    {
      g_value_set_string(value, audio_file_link->preset);
    }
    break;
  case PROP_INSTRUMENT:
    {
      g_value_set_string(value, audio_file_link->instrument);
    }
    break;
  case PROP_SAMPLE:
    {
      g_value_set_string(value, audio_file_link->sample);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      g_value_set_uint(value, audio_file_link->audio_channel);
    }
    break;
  case PROP_TIMESTAMP:
    {
      g_value_set_object(value, audio_file_link->timestamp);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_file_link_read(AgsFile *file,
			 xmlNode *node,
			 AgsPlugin *plugin)
{
  AgsAudioFileLink *gobject;
  AgsFileLaunch *file_launch;

  xmlChar *str;

  gobject = AGS_AUDIO_FILE_LINK(plugin);
  
  /* read audio channel */
  str = xmlGetProp(node,
		   "audio-channel\0");

  if(str != NULL){
    gobject->audio_channel = g_ascii_strtoull(str,
						      NULL, 10);
  }else{
    gobject->audio_channel = 0;
  }

  //TODO:JK: add missing
  
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "file\0", file,
					       "node\0", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start\0",
		   G_CALLBACK(ags_audio_file_link_read_launch), gobject);
  ags_file_add_launch(file,
		      (GObject *) file_launch);
}

xmlNode*
ags_audio_file_link_write(AgsFile *file,
			  xmlNode *parent,
			  AgsPlugin *plugin)
{
  AgsAudioFileLink *audio_file_link;
  xmlNode *node;
  gchar *id;

  audio_file_link = AGS_AUDIO_FILE_LINK(plugin);

  /* allocate new node with uuid */
  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-audio-file-link\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  /* add reference and node to file object */
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", audio_file_link,
				   NULL));

  /* write audio channel */
  xmlNewProp(node,
	     "audio-channel\0",
	     g_strdup_printf("%d\0", audio_file_link->audio_channel));

  //TODO:JK: add missing
  
  /* write timestamp */
  if(audio_file_link->timestamp != NULL){
    ags_file_write_timestamp(file,
			     node,
			     audio_file_link->timestamp);
  }
}

void
ags_audio_file_link_finalize(GObject *gobject)
{
  AgsAudioFileLink *audio_file_link;

  audio_file_link = AGS_AUDIO_FILE_LINK(gobject);

  if(audio_file_link->timestamp != NULL){
    g_object_unref(audio_file_link->timestamp);
  }
}

void
ags_audio_file_link_read_launch(AgsFileLaunch *file_launch,
				AgsAudioFileLink *audio_file_link)
{
  GObject *soundcard;
  AgsAudio *audio;
  AgsChannel *input;
  AgsAudioFile *audio_file;

  AgsFileIdRef *id_ref;

  GList *audio_signal;
  xmlNode *node, *child;
  xmlChar *type;
  xmlChar *filename;
  xmlChar *encoding, *demuxer;
  
  guint audio_channel;
  
  node = file_launch->node;

  audio_channel = audio_file_link->audio_channel;
  
  /* retrieve input */
  input = NULL;
  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_node((AgsFile *) file_launch->file,
							 node->parent->parent->parent);

  if(id_ref != NULL){
    input = (AgsChannel *) id_ref->ref;
  }

  /* get soundcard */
  soundcard = input->soundcard;

  /* get audio */
  audio = AGS_AUDIO(input->audio);
  
  /* read file link using URL or embedded */
  type = xmlGetProp(node->parent,
		    "type\0");

  if(!xmlStrncmp(type,
		 "url\0",
		 4)){
    //TODO:JK: add missing
    
    /* instantiat audio file */
    filename = AGS_FILE_LINK(audio_file_link)->filename;
    audio_file = ags_audio_file_new((gchar *) filename,
				    soundcard,
				    0, audio->audio_channels);

    /* open file and read audio signal */
    ags_audio_file_open(audio_file);
    ags_audio_file_read_audio_signal(audio_file);

    /* retrieve audio channel */
    audio_signal = g_list_nth(audio_file->audio_signal,
			      audio_channel);

    if(audio_signal == NULL){
      return;
    }

    /* set template flag */
    AGS_AUDIO_SIGNAL(audio_signal->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;

    /* unlink if needed */
    if(input->link != NULL){
      GError *error;

      error = NULL;

      ags_channel_set_link(input, NULL,
			   &error);

      if(error != NULL){
	g_warning("%s\0", error->message);
      }
    }

    /* set file link */
    g_object_set(G_OBJECT(input),
		 "file-link", g_object_new(AGS_TYPE_FILE_LINK,
					   "filename\0", filename,
					   NULL),
		 NULL);

    /* add audio signal */
    ags_recycling_add_audio_signal(input->first_recycling,
				   AGS_AUDIO_SIGNAL(audio_signal->data));
  }else if(!xmlStrncmp(type,
		       "embedded\0",
		       9)){
    xmlXPathContext *xpath_context;
    xmlXPathObject *xpath_object;

    /*  */
    xpath_context = xmlXPathNewContext(AGS_FILE(file_launch->file)->doc);
    //    xmlXPathSetContextNode(node,
    //			   xpath_context);
    xpath_context->node = node;  

    xpath_object = xmlXPathEval("./ags-embedded-audio\0",
				xpath_context);

    child = NULL;
    
    if(xpath_object->nodesetval != NULL && xpath_object->nodesetval->nodeTab != NULL){
      guint i;

      for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
	if(xpath_object->nodesetval->nodeTab[i]->type == XML_ELEMENT_NODE){
	  break;
	}
      }

      child = xpath_object->nodesetval->nodeTab[i];
    }

    /**/
    encoding = xmlGetProp(child, "encoding\0");
    demuxer = xmlGetProp(child, "demuxer\0");

    if(!xmlStrncmp(encoding,
		   "base64\0",
		   7)){
      if(!xmlStrncmp(demuxer,
		     "raw\0",
		     4)){
	gchar *data;

	audio_file = ags_audio_file_new(NULL,
					soundcard,
					0, audio->audio_channels);
	data = child->content;

	ags_audio_file_open_from_data(audio_file, data);
	ags_audio_file_read_audio_signal(audio_file);

	audio_signal = audio_file->audio_signal;

	if(audio_signal == NULL){
	  return;
	}
    
	ags_recycling_add_audio_signal(input->first_recycling,
				       AGS_AUDIO_SIGNAL(audio_signal->data));
      }
    }
  }
}

/**
 * ags_audio_file_link_new:
 *
 * Creates an #AgsAudioFileLink
 *
 * Returns: a new #AgsAudioFileLink
 *
 * Since: 0.4
 */
AgsAudioFileLink*
ags_audio_file_link_new()
{
  AgsAudioFileLink *audio_file_link;

  audio_file_link = (AgsAudioFileLink *) g_object_new(AGS_TYPE_AUDIO_FILE_LINK,
						      NULL);

  return(audio_file_link);
}
