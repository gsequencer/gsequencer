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

#include <ags/audio/file/ags_audio_file_xml.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_plugin.h>
#include <ags/object/ags_packable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_launch.h>
#include <ags/file/ags_file_util.h>

#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_recycling.h>
#include <ags/audio/ags_recall_audio_signal.h>

#define AGS_FILE_READ_PORT_LIST_PORT_RESOLVED_COUNTER "ags-file-read-port-list-port-resolved-counter\0"

void ags_file_read_audio_resolve_soundcard(AgsFileLookup *file_lookup,
					   AgsAudio *audio);
void ags_file_write_audio_resolve_soundcard(AgsFileLookup *file_lookup,
					    AgsAudio *audio);

void ags_file_read_channel_resolve_link(AgsFileLookup *file_lookup,
					AgsChannel *channel);
void ags_file_write_channel_resolve_link(AgsFileLookup *file_lookup,
					 AgsChannel *channel);

void ags_file_read_file_link_launch(AgsFileLaunch *file_launch,
				    AgsFileLink *file_link);

void ags_file_read_recall_container_resolve_value(AgsFileLookup *file_lookup,
						  AgsRecallContainer *recall_container);

void ags_file_read_recall_resolve_audio(AgsFileLookup *file_lookup,
					AgsRecall *recall);
void ags_file_read_recall_resolve_channel(AgsFileLookup *file_lookup,
					  AgsRecall *recall);
void ags_file_read_recall_resolve_port(AgsFileLookup *file_lookup,
				       AgsRecall *recall);
void ags_file_read_recall_resolve_parameter(AgsFileLookup *file_lookup,
					    AgsRecall *recall);
void ags_file_read_recall_resolve_soundcard(AgsFileLookup *file_lookup,
					    AgsRecall *recall);
void ags_file_write_recall_resolve_soundcard(AgsFileLookup *file_lookup,
					     AgsRecall *recall);

void ags_file_read_pattern_resolve_port(AgsFileLookup *file_lookup,
					AgsPattern *pattern);
void ags_file_write_pattern_resolve_port(AgsFileLookup *file_lookup,
					 AgsPattern *pattern);

void ags_file_read_notation_resolve_port(AgsFileLookup *file_lookup,
					 AgsNotation *notation);
void ags_file_write_notation_resolve_port(AgsFileLookup *file_lookup,
					  AgsNotation *notation);

void ags_file_read_port_resolve_port_value(AgsFileLookup *file_lookup,
					   AgsPort *port);

void ags_file_read_task_resolve_parameter(AgsFileLookup *file_lookup,
					  AgsTask *task);

void
ags_file_read_soundcard(AgsFile *file, xmlNode *node, GObject **soundcard)
{
  GObject *gobject;
  xmlNode *child;
  GList *audio;

  gchar *type_name;
  xmlChar *device_id;
  guint channels, samplerate, buffer_size, format;
  gdouble bpm;
  guint note_offset;
  
  if(*soundcard == NULL){
    GType type;

    type_name = (gchar *) xmlGetProp(node,
				     (xmlChar *) AGS_FILE_TYPE_PROP);

    type = g_type_from_name(type_name);

    gobject = g_object_new(type,
			   NULL);
    *soundcard = gobject;
  }else{
    gobject = *soundcard;
  }

  g_object_set(gobject,
	       "application-context\0", file->application_context,
	       NULL);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node,
												  (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));
  
  /* device */
  device_id = xmlGetProp(node,
			 (xmlChar *) "device\0");
  ags_soundcard_set_device(AGS_SOUNDCARD(gobject),
			   (gchar *) device_id);

  /* presets */
  channels = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
							   (xmlChar *) "channels\0"),
				      NULL,
				      10);

  samplerate = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
							     (xmlChar *) "samplerate\0"),
					NULL,
					10);

  buffer_size = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
							      (xmlChar *) "buffer-size\0"),
					 NULL,
					 10);

  format = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
							 (xmlChar *) "format\0"),
				    NULL,
				    10);
  
  ags_soundcard_set_presets(AGS_SOUNDCARD(gobject),
			    channels,
			    samplerate,
			    buffer_size,
			    format);

  /* bpm */
  bpm = (gdouble) g_ascii_strtod((gchar *) xmlGetProp(node,
						      (xmlChar *) "bpm\0"),
				 NULL);
  
  ags_soundcard_set_bpm(AGS_SOUNDCARD(gobject),
			bpm);

  /* note offset */
  note_offset = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
							      (xmlChar *) "note-offset\0"),
					 NULL,
					 10);
  ags_soundcard_set_note_offset(AGS_SOUNDCARD(gobject),
				note_offset);

  /* child elements */
  child = node->children;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-audio-list\0",
		     15)){
	audio = NULL;
	ags_file_read_audio_list(file,
				 child,
				 &audio);
	ags_soundcard_set_audio(AGS_SOUNDCARD(gobject),
				audio);

	while(audio != NULL){
	  g_object_set(G_OBJECT(audio->data),
		       "soundcard\0", gobject,
		       NULL);
	  audio = audio->next;
	}
      }
    }
    
    child = child->next;
  }
}

xmlNode*
ags_file_write_soundcard(AgsFile *file, xmlNode *parent, GObject *soundcard)
{
  xmlNode *node;
  GList *audio;
  
  gchar *id;
  gchar *device_id;
  guint channels, samplerate, buffer_size, format;
  gdouble bpm;
  guint note_offset;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-soundcard\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", soundcard,
				   NULL));

  /* device */
  device_id = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard));

  xmlNewProp(node,
	     (xmlChar *) "device\0",
	     (xmlChar *) device_id);
  
  /* presets */
  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    &channels,
			    &samplerate,
			    &buffer_size,
			    &format);

  xmlNewProp(node,
	     (xmlChar *) "channels\0",
	     (xmlChar *) g_strdup_printf("%d\0", channels));
  xmlNewProp(node,
	     (xmlChar *) "samplerate\0",
	     (xmlChar *) g_strdup_printf("%d\0", samplerate));
  xmlNewProp(node,
	     (xmlChar *) "buffer-size\0",
	     (xmlChar *) g_strdup_printf("%d\0", buffer_size));
  xmlNewProp(node,
	     (xmlChar *) "format\0",
	     (xmlChar *) g_strdup_printf("%d\0", format));

  /* bpm */
  bpm = ags_soundcard_get_bpm(AGS_SOUNDCARD(soundcard));

  xmlNewProp(node,
	     (xmlChar *) "bpm\0",
	     (xmlChar *) g_strdup_printf("%f\0", bpm));

  /* note offset */
  note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(soundcard));

  xmlNewProp(node,
	     (xmlChar *) "note-offset\0",
	     (xmlChar *) g_strdup_printf("%u\0", note_offset));

  /* ags-audio-list */
  audio = ags_soundcard_get_audio(AGS_SOUNDCARD(soundcard));
  ags_file_write_audio_list(file,
			    node,
			    audio);
  
  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_file_read_soundcard_list(AgsFile *file, xmlNode *node, GList **soundcard)
{
  GObject *current;
  GList *list;
  xmlNode *child;
  xmlChar *id;

  id = xmlGetProp(node,
		  (xmlChar *) AGS_FILE_ID_PROP);

  child = node->children;
  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-soundcard\0",
		     11)){
	current = NULL;
	ags_file_read_soundcard(file, child, &current);
	list = g_list_prepend(list, current);
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *soundcard = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_soundcard_list(AgsFile *file, xmlNode *parent, GList *soundcard)
{
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-soundcard-list\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", soundcard,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = soundcard;

  while(list != NULL){
    ags_file_write_soundcard(file, node, AGS_SOUNDCARD(list->data));

    list = list->next;
  }

  return(node);
}

void
ags_file_read_playback(AgsFile *file, xmlNode *node, AgsPlayback **play)
{
  AgsPlayback *pointer;
  gchar *id;
  
  if(*play == NULL){
    pointer = ags_playback_new();
    *play = pointer;
  }else{
    pointer = *play;
  }

  id = (gchar *) xmlGetProp(node,
			    (xmlChar *) AGS_FILE_ID_PROP);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", pointer,
				   NULL));

  pointer->flags = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								 (xmlChar *) AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  pointer->audio_channel = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
									 (xmlChar *) "audio-channel\0"),
						    NULL,
						    10);

  // read by parent call: play->source
}

xmlNode*
ags_file_write_playback(AgsFile *file, xmlNode *parent, AgsPlayback *play)
{
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-playback\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", play,
				   NULL));

  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_FLAGS_PROP,
	     (xmlChar *) g_strdup_printf("%x\0", play->flags));

  xmlNewProp(node,
	     (xmlChar *) "audio-channel\0",
	     (xmlChar *) g_strdup_printf("%d\0", play->audio_channel));

  // write by parent call: play->source

  return(node);
}

void
ags_file_read_playback_list(AgsFile *file, xmlNode *node, GList **play)
{
  GList *list;
  AgsPlayback *current;
  xmlNode *child;
  gchar *id;

  id = (gchar *) xmlGetProp(node,
			    (xmlChar *) AGS_FILE_ID_PROP);

  child = node->children;
  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-playback\0",
		     16)){
	current = NULL;
	ags_file_read_playback(file, child, &current);

	list = g_list_prepend(list, current);
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *play = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_playback_list(AgsFile *file, xmlNode *parent, GList *play)
{
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-playback-list\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", play,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = play;

  while(list != NULL){
    ags_file_write_playback(file, node, AGS_PLAYBACK(list->data));

    list = list->next;
  }

  return(node);
}

void
ags_file_read_audio(AgsFile *file, xmlNode *node, AgsAudio **audio)
{
  AgsAudio *gobject;
  AgsFileLookup *file_lookup;
  xmlNode *child;
  guint pads;

  if(audio[0] == NULL){
    gobject = (AgsAudio *) g_object_new(AGS_TYPE_AUDIO,
					NULL);
    *audio = gobject;
  }else{
    gobject = *audio;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  gobject->flags = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								 (xmlChar *) AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  gobject->sequence_length = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
									   (xmlChar *) "sequence-length\0"),
						      NULL,
						      10);

  gobject->audio_channels = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
									  (xmlChar *) "audio-channels\0"),
						     NULL,
						     10);

  pads= (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
						      (xmlChar *) "output-pads\0"),
				 NULL,
				 10);
  ags_audio_set_pads(gobject,
		     AGS_TYPE_OUTPUT,
		     pads);

  pads = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
						       (xmlChar *) "input-pads\0"),
				  NULL,
				  10);
  ags_audio_set_pads(gobject,
		     AGS_TYPE_INPUT,
		     pads);

  /* soundcard */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", gobject,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_read_audio_resolve_soundcard), gobject);

  /* read child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-channel-list\0",
		     17)){
	xmlXPathContext *xpath_context;
	xmlXPathObject *xpath_object;

	xpath_context = xmlXPathNewContext(file->doc);
	//	xmlXPathSetContextNode(child,
	//		       xpath_context);
	xpath_context->node = child;

	xpath_object = xmlXPathEval((xmlChar *) "./ags-channel/ags-output\0",
				    xpath_context);

	if(xmlXPathCastToBoolean(xpath_object)){
	  xmlNode *channel_node;
	  
	  channel_node = child->children;

	  while(channel_node != NULL){
	    if(channel_node->type == XML_ELEMENT_NODE){
	      if(!xmlStrncmp(channel_node->name,
			     (xmlChar *) "ags-channel\0",
			     12)){
		AgsChannel *channel;
		guint pad, audio_channel;

		pad = (guint) g_ascii_strtoull((gchar *) xmlGetProp(channel_node,
								    (xmlChar *) "pad\0"),
					       NULL,
					       10);
		audio_channel = (guint) g_ascii_strtoull((gchar *) xmlGetProp(channel_node,
									      (xmlChar *) "audio-channel\0"),
							 NULL,
							 10);

		channel = ags_channel_nth(gobject->output,
					  pad * gobject->audio_channels + audio_channel);

		/* ags-channel output */
		ags_file_read_channel(file,
				      channel_node,
				      &channel);
		g_object_set(channel,
			     "audio\0", gobject,
			     NULL);
	      }
	    }

	    channel_node = channel_node->next;
	  }
	}else{
	  xmlNode *channel_node;
	  
	  channel_node = child->children;

	  while(channel_node != NULL){
	    if(channel_node->type == XML_ELEMENT_NODE){
	      if(!xmlStrncmp(channel_node->name,
			     (xmlChar *) "ags-channel\0",
			     12)){
		AgsChannel *channel;
		guint pad, audio_channel;

		pad = (guint) g_ascii_strtoull((gchar *) xmlGetProp(channel_node,
								    (xmlChar *) "pad\0"),
					       NULL,
					       10);
		audio_channel = (guint) g_ascii_strtoull((gchar *) xmlGetProp(channel_node,
									      (xmlChar *) "audio-channel\0"),
							 NULL,
							 10);

		channel = ags_channel_nth(gobject->input,
					  pad * gobject->audio_channels + audio_channel);

		/* ags-channel input */
		ags_file_read_channel(file,
				      channel_node,
				      &channel);
		g_object_set(channel,
			     "audio\0", gobject,
			     NULL);
	      }
	    }
	    
	    channel_node = channel_node->next;
	  }
	}
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-recall-container-list\0",
			   26)){
	ags_file_read_recall_container_list(file,
					    child,
					    &(gobject->container));
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-recall-list\0",
			   15)){
	GList *list;

	if(!xmlStrncmp(xmlGetProp(child, (xmlChar *) "is-play\0"),
		       (xmlChar *) "TRUE\0",
		       4)){

	  /* ags-recall-list play */
	  ags_file_read_recall_list(file,
				    child,
				    &list);
	  gobject->play = list;
	}else{
	  /* ags-recall-list recall */
	  ags_file_read_recall_list(file,
				    child,
				    &list);

	  gobject->recall = list;
	}

	while(list != NULL){
	  g_object_ref(list->data);

	  list = list->next;
	}
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-notation-list\0",
			   17)){
	/* ags-notation-list */
	ags_file_read_notation_list(file,
				    child,
				    &(gobject->notation));
      }
    }
    
    child = child->next;
  }
}

void
ags_file_read_audio_resolve_soundcard(AgsFileLookup *file_lookup,
				      AgsAudio *audio)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) (gchar *) xmlGetProp(file_lookup->node,
					 (xmlChar *) "soundcard\0");

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file, xpath);

  if(id_ref != NULL){
    GObject *soundcard;
    AgsChannel *channel;
    AgsAudioSignal *audio_signal;

    soundcard = (GObject *) id_ref->ref;

    g_object_set(G_OBJECT(audio),
		 "soundcard\0", soundcard,
		 NULL);

    /* create output audio signal template */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      channel = audio->output;

      while(channel != NULL){
	audio_signal = ags_audio_signal_new(soundcard,
					    (GObject *) channel->first_recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_recycling_add_audio_signal(channel->first_recycling,
				       audio_signal);

	channel = channel->next;
      }
    }

    /* create input audio signal template */
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
      channel = audio->input;

      while(channel != NULL){
	audio_signal = ags_audio_signal_new(soundcard,
					    (GObject *) channel->first_recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_recycling_add_audio_signal(channel->first_recycling,
				       audio_signal);

	channel = channel->next;
      }
    }
  }
}

xmlNode*
ags_file_write_audio(AgsFile *file, xmlNode *parent, AgsAudio *audio)
{
  AgsFileLookup *file_lookup;
  AgsChannel *channel;
  xmlNode *node, *child;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-audio\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", audio,
				   NULL));

  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_FLAGS_PROP,
	     (xmlChar *) g_strdup_printf("%x\0", audio->flags));

  xmlNewProp(node,
	     (xmlChar *) "sequence-length\0",
	     (xmlChar *) g_strdup_printf("%d\0", audio->sequence_length));

  xmlNewProp(node,
	     (xmlChar *) "audio-channels\0",
	     (xmlChar *) g_strdup_printf("%d\0", audio->audio_channels));

  xmlNewProp(node,
	     (xmlChar *) "output-pads\0",
	     (xmlChar *) g_strdup_printf("%d\0", audio->output_pads));

  xmlNewProp(node,
	     (xmlChar *) "input-pads\0",
	     (xmlChar *) g_strdup_printf("%d\0", audio->input_pads));


  /* soundcard */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", audio,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_write_audio_resolve_soundcard), audio);

  /*  */
  xmlAddChild(parent,
	      node);

  /* child elements */
  /* ags-channel-list output */
  channel = audio->output;

  child = xmlNewNode(NULL,
		     (xmlChar *)  "ags-channel-list\0");
  xmlAddChild(node,
	      child);

  //TODO:JK: generate id and add id ref

  while(channel != NULL){
    ags_file_write_channel(file,
			   child,
			   channel);

    channel = channel->next;
  }

  /* ags-channel-list input */
  channel = audio->input;

  child = xmlNewNode(NULL,
		     (xmlChar *)  "ags-channel-list\0");
  xmlAddChild(node,
	      child);

  //TODO:JK: generate id and add id ref

  while(channel != NULL){
    ags_file_write_channel(file,
			   child,
			   channel);

    channel = channel->next;
  }

  /* ags-recall-container */
  ags_file_write_recall_container_list(file,
				       node,
				       audio->container);

  /* ags-recall-list play */
  child = ags_file_write_recall_list(file,
				     node,
				     audio->play);

  xmlNewProp(child,
	     (xmlChar *) "is-play\0",
	     (xmlChar *) AGS_FILE_TRUE);

  /* ags-recall-list recall */
  child = ags_file_write_recall_list(file,
				     node,
				     audio->recall);

  xmlNewProp(child,
	     (xmlChar *) "is-play\0",
	     (xmlChar *) AGS_FILE_FALSE);

  /* ags-notation-list */
  ags_file_write_notation_list(file,
			       node,
			       audio->notation);

  return(node);
}

void
ags_file_write_audio_resolve_soundcard(AgsFileLookup *file_lookup,
				       AgsAudio *audio)
{
  AgsFileIdRef *id_ref;
  gchar *id;

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, audio->soundcard);

  id = (gchar *) xmlGetProp(id_ref->node, (xmlChar *) AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     (xmlChar *) "soundcard\0",
	     (xmlChar *) g_strdup_printf("xpath=//ags-soundcard[@id='%s']\0", id));
}

void
ags_file_read_audio_list(AgsFile *file, xmlNode *node, GList **audio)
{
  AgsAudio *current;
  GList *list;
  xmlNode *child;

  list = NULL;
  child = node->children;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", list,
				   NULL));
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-audio\0",
		     10)){
	current = NULL;
	ags_file_read_audio(file,
			    child,
			    &current);

	list = g_list_prepend(list,
			      current);
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *audio = list;
}

xmlNode*
ags_file_write_audio_list(AgsFile *file, xmlNode *parent, GList *audio)
{
  AgsAudio *current;
  GList *list;
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-audio-list\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", audio,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = audio;

  while(list != NULL){
    ags_file_write_audio(file,
			 node,
			 AGS_AUDIO(list->data));

    list = list->next;
  }

  return(node);
}

void
ags_file_read_channel(AgsFile *file, xmlNode *node, AgsChannel **channel)
{
  AgsChannel *gobject;
  AgsFileLookup *file_lookup;
  xmlNode *child;
  gboolean preset;
  guint pad, audio_channel;
  gboolean is_output;

  if(*channel == NULL){
    xmlXPathContext *xpath_context;
    xmlXPathObject *xpath_object;
    
    xpath_context = xmlXPathNewContext(file->doc);
    //    xmlXPathSetContextNode(node,
    //			   xpath_context);
    xpath_context->node = node;

    xpath_object = xmlXPathEval((xmlChar *) "./ags-output\0",
				xpath_context);


    if(xmlXPathCastToBoolean(xpath_object)){
      gobject = (AgsChannel *) g_object_new(AGS_TYPE_OUTPUT,
					    NULL);

      is_output = TRUE;
    }else{
      gobject = (AgsChannel *) g_object_new(AGS_TYPE_INPUT,
					    NULL);

      is_output = FALSE;
    }

    *channel = gobject;

    preset = FALSE;
  }else{
    gobject = *channel;

    if(AGS_IS_OUTPUT(gobject)){
      is_output = TRUE;
    }else{
      is_output = FALSE;
    }

    preset = TRUE;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));
  
  gobject->flags = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								 (xmlChar *) AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  /* well known properties */
  pad = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
						      (xmlChar *) "pad\0"),
				 NULL,
				 10);
  audio_channel = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								(xmlChar *) "audio-channel\0"),
  					   NULL,
  					   10);

  if(!preset){
    gobject->pad = pad;
    gobject->audio_channel = audio_channel;
  }


  /* link */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", AGS_CHANNEL(gobject),
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_read_channel_resolve_link), AGS_CHANNEL(gobject));

  /*  */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-recycling\0",
		     13)){
	/* ags-recycling */
	ags_file_read_recycling(file,
				child,
				&(gobject->first_recycling));
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-recall-container-list\0",
			   26)){
	ags_file_read_recall_container_list(file,
					    child,
					    &(gobject->container));
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-recall-list\0",
			   15)){
	GList *list;

	if(!xmlStrncmp((gchar *) xmlGetProp(child, (xmlChar *) "is-play\0"),
		       (xmlChar *) "TRUE\0",
		       4)){
	  /* ags-recall-list play */
	  ags_file_read_recall_list(file,
				    child,
				    &list);

	  gobject->play = list;
	}else{
	  /* ags-recall-list recall */
	  ags_file_read_recall_list(file,
				    child,
				    &list);

	  gobject->recall = list;
	}

	while(list != NULL){
	  g_object_ref(list->data);

	  list = list->next;
	}
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-pattern-list\0",
			   17)){
	g_list_free_full(gobject->pattern,
			 g_object_unref);
	gobject->pattern = NULL;

	/* ags-pattern-list */
	ags_file_read_pattern_list(file,
				   child,
				   &(gobject->pattern));
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-output\0",
			   10)){
	/* ags-output */
	ags_file_read_output(file,
			     child,
			     gobject);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-input\0",
			   9)){
	/* ags-input */
	ags_file_read_input(file,
			    child,
			    gobject);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-playback\0",
			   15)){
	/* ags-playback */
	ags_file_read_playback(file,
			       child,
			       (AgsPlayback **) &gobject->playback);
	AGS_PLAYBACK(gobject->playback)->source = (GObject *) gobject;
      }
    }

    child = child->next;
  }
}

void
ags_file_read_channel_resolve_link(AgsFileLookup *file_lookup,
				   AgsChannel *channel)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) (gchar *) xmlGetProp(file_lookup->node,
					 (xmlChar *) "link\0");

  if(xpath == NULL){
    return;
  }

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file, xpath);

  if(id_ref != NULL){
    GError *error;
    
    error = NULL;

    if(channel->link == NULL){
      ags_channel_set_link(channel,
			   (AgsChannel *) id_ref->ref,
			   &error);
    }
  }
}

xmlNode*
ags_file_write_channel(AgsFile *file, xmlNode *parent, AgsChannel *channel)
{
  AgsFileLookup *file_lookup;
  xmlNode *node, *child;
  gchar *id, *link_id;
  gboolean is_output;

  id = ags_id_generator_create_uuid();

  if(AGS_IS_OUTPUT(channel)){
    is_output = TRUE;
  }else{
    is_output = FALSE;
  }

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-channel\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", AGS_CHANNEL(channel),
				   NULL));
  
  /* well known properties */
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_FLAGS_PROP,
	     (xmlChar *) g_strdup_printf("%x\0", channel->flags));

  xmlNewProp(node,
	     (xmlChar *) "pad\0",
	     (xmlChar *) g_strdup_printf("%d\0", channel->pad));
  xmlNewProp(node,
	     (xmlChar *) "audio-channel\0",
	     (xmlChar *) g_strdup_printf("%d\0", channel->audio_channel));


  /* link */
  if(channel->link != NULL){
    file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
						 "file\0", file,
						 "node\0", node,
						 "reference\0", AGS_CHANNEL(channel),
						 NULL);
    ags_file_add_lookup(file, (GObject *) file_lookup);
    g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		     G_CALLBACK(ags_file_write_channel_resolve_link), AGS_CHANNEL(channel));
  }

  xmlAddChild(parent,
	      node);

  /* ags-recycling */
  if(is_output){
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (AGS_AUDIO(channel->audio)->flags)) != 0){
      ags_file_write_recycling(file,
			       node,
			       channel->first_recycling);
    }
  }else{
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (AGS_AUDIO(channel->audio)->flags)) != 0){
      ags_file_write_recycling(file,
			       node,
			       channel->first_recycling);
    }
  }

  /* ags-recall-container */
  if(g_list_find(AGS_AUDIO(channel->audio)->container,
		 channel->container) != NULL){
    ags_file_write_recall_container_list(file,
					 node,
					 channel->container);
  }

  /* ags-recall-list play */
  child = ags_file_write_recall_list(file,
				     node,
				     channel->play);
  
  xmlNewProp(child,
	     (xmlChar *) "is-play\0",
	     (xmlChar *) AGS_FILE_TRUE);

  /* ags-recall-list recall */
  child = ags_file_write_recall_list(file,
				     node,
				     channel->recall);
  
  xmlNewProp(child,
	     (xmlChar *) "is-play\0",
	     (xmlChar *) AGS_FILE_FALSE);

  /* ags-pattern-list */
  if(channel->pattern != NULL){
    ags_file_write_pattern_list(file,
				node,
				channel->pattern);
  }

  /* ags-input or ags-output */
  if(AGS_IS_OUTPUT(channel)){
    ags_file_write_output(file,
			  node,
			  channel);
  }else{
    ags_file_write_input(file,
			 node,
			 channel);
  }

  /* ags-playback */
  child = ags_file_write_playback(file,
				  node,
				  (AgsPlayback *) channel->playback);

  return(node);
}

void
ags_file_write_channel_resolve_link(AgsFileLookup *file_lookup,
				    AgsChannel *channel)
{
  AgsFileIdRef *id_ref;
  gchar *id;

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, AGS_CHANNEL(channel->link));

  id = (gchar *) xmlGetProp(id_ref->node, (xmlChar *) AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     (xmlChar *) "link\0",
	     (xmlChar *) g_strdup_printf("xpath=//*[@id='%s']\0", id));
}

void
ags_file_read_channel_list(AgsFile *file, xmlNode *node, GList **channel)
{
  AgsChannel *current;
  xmlNode *child;
  GList *list;

  list = NULL;
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-channel\0",
		     12)){
	current = NULL;
	ags_file_read_channel(file,
			      child,
			      &current);

	list = g_list_prepend(list,
			      current);
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *channel = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_channel_list(AgsFile *file, xmlNode *parent, GList *channel)
{
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-channel-list\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", channel,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = channel;

  while(list != NULL){
    ags_file_write_channel(file,
			   node,
			   AGS_CHANNEL(list->data));

    list = list->next;
  }

  return(node);
}

void
ags_file_read_input(AgsFile *file, xmlNode *node, AgsChannel *channel)
{
  AgsInput *input;
  xmlNode *child;

  input = AGS_INPUT(channel);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", input,
				   NULL));

  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-file-link\0",
		     13)){
	ags_file_read_file_link(file,
				child,
				(AgsFileLink **) &(input->file_link));
      }
    }

    child = child->next;
  }
}

xmlNode*
ags_file_write_input(AgsFile *file, xmlNode *parent, AgsChannel *channel)
{
  AgsInput *input;
  xmlNode *node;
  gchar *id;

  input = AGS_INPUT(channel);

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-input\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", input,
				   NULL));

  xmlAddChild(parent,
	      node);

  if(input->file_link != NULL){
    ags_file_write_file_link(file,
			     node,
			     (AgsFileLink *) input->file_link);
  }

  return(node);
}

void
ags_file_read_output(AgsFile *file, xmlNode *node, AgsChannel *channel)
{
  AgsOutput *output;

  output = AGS_OUTPUT(channel);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", output,
				   NULL));
}

xmlNode*
ags_file_write_output(AgsFile *file, xmlNode *parent, AgsChannel *channel)
{
  AgsOutput *output;
  xmlNode *node;
  gchar *id;

  output = AGS_OUTPUT(channel);

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-output\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", output,
				   NULL));
  
  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_file_read_recall(AgsFile *file, xmlNode *node, AgsRecall **recall)
{
  AgsFileLookup *file_lookup;
  AgsRecall *gobject;
  xmlNode *child;
  gchar *type_name;

  if(*recall == NULL){
    GType type;

    type_name = (gchar *) xmlGetProp(node,
				     (xmlChar *) AGS_FILE_TYPE_PROP);

    type = g_type_from_name(type_name);

    gobject = g_object_new(type,
			   NULL);

    *recall = gobject;
  }else{
    gobject = *recall;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  gobject->version = (gchar *) (gchar *) xmlGetProp(node,
						    (xmlChar *) AGS_FILE_VERSION_PROP);

  gobject->build_id = (gchar *) (gchar *) xmlGetProp(node,
						     (xmlChar *) AGS_FILE_BUILD_ID_PROP);

  gobject->flags = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								 (xmlChar *) AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  gobject->flags |= AGS_RECALL_TEMPLATE;
  gobject->flags &= (~(AGS_RECALL_CONNECTED |
		       AGS_RECALL_DYNAMIC_CONNECTED));
  /* soundcard */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", gobject,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_read_recall_resolve_soundcard), gobject);

  /* audio */
  if(AGS_IS_RECALL_AUDIO(gobject)){
    file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
						 "file\0", file,
						 "node\0", node,
						 "reference\0", gobject,
						 NULL);
    ags_file_add_lookup(file, (GObject *) file_lookup);
    g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		     G_CALLBACK(ags_file_read_recall_resolve_audio), gobject);
  }

  /* source and destination */
  if(AGS_IS_RECALL_CHANNEL(gobject) ||
     AGS_IS_RECALL_CHANNEL_RUN(gobject)){
    file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
						 "file\0", file,
						 "node\0", node,
						 "reference\0", gobject,
						 NULL);
    ags_file_add_lookup(file, (GObject *) file_lookup);
    g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		     G_CALLBACK(ags_file_read_recall_resolve_channel), gobject);
  }

  /*  */
  gobject->effect = (gchar *) (gchar *) xmlGetProp(node,
						   (xmlChar *) "effect\0");

  gobject->name = (gchar *) (gchar *) xmlGetProp(node,
						 (xmlChar *) "name\0");

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-recall-audio\0",
		     17)){
	ags_file_read_recall_audio(file,
				   child,
				   gobject);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-recall-audio-run\0",
			   21)){
	ags_file_read_recall_audio_run(file,
				       child,
				       gobject);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-recall-channel\0",
			   19)){
	ags_file_read_recall_channel(file,
				     child,
				     gobject);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-recall-channel-run\0",
			   23)){
	ags_file_read_recall_channel_run(file,
					 child,
					 gobject);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-recall-recycling\0",
			   21)){
	ags_file_read_recall_recycling(file,
				       child,
				       gobject);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-recall-audio-signal\0",
			   24)){
	ags_file_read_recall_audio_signal(file,
					  child,
					  gobject);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-recall-list\0",
			   16)){
	GList *list, *start;

	ags_file_read_recall_list(file,
				  child,
				  &start);

	list = start;

	while(list != NULL){
	  g_object_set(G_OBJECT(gobject),
		       "child\0", AGS_RECALL(list->data),
		       NULL);

	  list = list->next;
	}

	g_list_free(start);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-port-list\0",
			   14)){
	GList *list;

	list = NULL;

	ags_file_read_port_list(file,
				child,
				&list);

	ags_plugin_set_ports(AGS_PLUGIN(gobject),
			     list);
	gobject->port = list;
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-parameter\0",
			   13)){
	ags_file_util_read_parameter(file,
				     child, NULL,
				     &(gobject->child_parameters), NULL, NULL);
      }
    }

    child = child->next;
  }
}

void
ags_file_read_recall_resolve_audio(AgsFileLookup *file_lookup,
				   AgsRecall *recall)
{
  AgsAudio *audio;
  AgsFileIdRef *file_id_ref;
  xmlNode *node;

  audio = NULL;

  node = file_lookup->node->parent->parent;
  file_id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_node(file_lookup->file,
							      node);

  if(file_id_ref != NULL){
    audio = (AgsAudio *) file_id_ref->ref;
  }

  g_object_set(G_OBJECT(recall),
	       "audio\0", AGS_AUDIO(audio),
	       NULL);
}

void
ags_file_read_recall_resolve_channel(AgsFileLookup *file_lookup,
				     AgsRecall *recall)
{
  AgsChannel *source, *destination;
  AgsFileIdRef *file_id_ref;
  xmlNode *node;
    
  source = NULL;
  destination = NULL;

  node = file_lookup->node->parent->parent;
  file_id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_node(file_lookup->file,
							      node);

  if(file_id_ref != NULL){
    source = AGS_CHANNEL(file_id_ref->ref);

    g_object_set(G_OBJECT(recall),
		 "source\0", source,
		 NULL);

    if(AGS_IS_INPUT(source)){
      if((AGS_AUDIO_ASYNC & (AGS_AUDIO(source->audio)->flags)) != 0){
	destination = ags_channel_nth(AGS_AUDIO(source->audio)->output,
				      source->audio_channel);
      }else{
	destination = ags_channel_nth(AGS_AUDIO(source->audio)->output,
				      source->line);
      }

      g_object_set(G_OBJECT(recall),
		   "destination\0", AGS_CHANNEL(destination),
		   NULL);
    }
  }
}

void
ags_file_read_recall_resolve_port(AgsFileLookup *file_lookup,
				  AgsRecall *recall)
{
  //TODO:JK: implement me
}

void
ags_file_read_recall_resolve_parameter(AgsFileLookup *file_lookup,
				       AgsRecall *recall)
{
  //TODO:JK: implement me
}

void
ags_file_read_recall_resolve_soundcard(AgsFileLookup *file_lookup,
				       AgsRecall *recall)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) (gchar *) xmlGetProp(file_lookup->node,
					 (xmlChar *) "soundcard\0");

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file,
							  xpath);

  if(id_ref != NULL){
    recall->soundcard = (GObject *) id_ref->ref;
  }
}

xmlNode*
ags_file_write_recall(AgsFile *file, xmlNode *parent, AgsRecall *recall)
{
  AgsFileLookup *file_lookup;
  xmlNode *node;
  gchar *id;

  if((AGS_RECALL_TEMPLATE & (recall->flags)) == 0){
    return(NULL);
  }

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-recall\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", recall,
				   NULL));

  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_TYPE_PROP,
	     (xmlChar *) G_OBJECT_TYPE_NAME(recall));

  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_VERSION_PROP,
	     (xmlChar *) recall->version);
  
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_BUILD_ID_PROP,
	     (xmlChar *) recall->build_id);
  
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_FLAGS_PROP,
	     (xmlChar *) g_strdup_printf("%x\0", recall->flags));
  
  /* soundcard */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", recall,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_write_recall_resolve_soundcard), recall);

  /*  */
  xmlNewProp(node,
	     (xmlChar *) "effect\0",
	     (xmlChar *) recall->effect);
  
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_NAME_PROP,
	     (xmlChar *) recall->name);
  
  xmlAddChild(parent,
	      node);

  /* child elements */
  if(AGS_IS_RECALL_AUDIO(recall)){
    ags_file_write_recall_audio(file,
				node,
				recall);
  }else if(AGS_IS_RECALL_AUDIO_RUN(recall)){
    ags_file_write_recall_audio_run(file,
				    node,
				    recall);
  }else if(AGS_IS_RECALL_CHANNEL(recall)){
    ags_file_write_recall_channel(file,
				  node,
				  recall);
  }else if(AGS_IS_RECALL_CHANNEL_RUN(recall)){
    ags_file_write_recall_channel_run(file,
				      node,
				      recall);
  }else if(AGS_IS_RECALL_RECYCLING(recall)){
    ags_file_write_recall_recycling(file,
				    node,
				    recall);
  }else if(AGS_IS_RECALL_AUDIO_SIGNAL(recall)){
    ags_file_write_recall_audio_signal(file,
				       node,
				       recall);
  }

  if(recall->children != NULL){
    ags_file_write_recall_list(file,
			       node,
			       recall->children);
  }

  if(recall->port != NULL){
    ags_file_write_port_list(file,
			     node,
			     recall->port);
  }
  
  /* child parameters */
  ags_file_util_write_parameter(file,
				node,
				ags_id_generator_create_uuid(),
				recall->child_parameters, recall->n_params);

  return(node);
}

void
ags_file_write_recall_resolve_soundcard(AgsFileLookup *file_lookup,
					AgsRecall *recall)
{
  AgsFileIdRef *id_ref;
  gchar *id;

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, recall->soundcard);

  id = (gchar *) xmlGetProp(id_ref->node, (xmlChar *) AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     (xmlChar *) "soundcard\0",
	     (xmlChar *) g_strdup_printf("xpath=//ags-soundcard[@id='%s']\0", id));
}

void
ags_file_read_recall_list(AgsFile *file, xmlNode *node, GList **recall)
{
  AgsRecall *current;
  xmlNode *child;
  GList *list;

  list = NULL;
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-recall\0",
		     11)){
	current = NULL;
	ags_file_read_recall(file,
			     child,
			     &current);
    
	g_object_ref(current);
	list = g_list_prepend(list,
			      current);
      }
    }
    
    child = child->next;
  }

  list = g_list_reverse(list);
  *recall = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_recall_list(AgsFile *file, xmlNode *parent, GList *recall)
{
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-recall-list\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", recall,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = recall;

  while(list != NULL){
    ags_file_write_recall(file,
			  node,
			  AGS_RECALL(list->data));
    
    list = list->next;
  }

  return(node);
}

void
ags_file_read_recall_container(AgsFile *file, xmlNode *node, AgsRecallContainer **recall_container)
{
  AgsRecallContainer *gobject;
  xmlNode *child;
  xmlChar *type_name;
  
  if(*recall_container == NULL){
    gobject = g_object_new(AGS_TYPE_RECALL_CONTAINER,
			   NULL);

    *recall_container = gobject;
  }else{
    gobject = *recall_container;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-parameter\0",
		     14)){
	AgsFileLookup *file_lookup;
	xmlNode *value_node;
	GList *list;

	ags_file_util_read_parameter(file,
				     child, NULL,
				     NULL, NULL, NULL);

	value_node = child->children;

	while(value_node != NULL){
	  if(value_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(value_node->name,
			   (xmlChar *) "ags-value\0",
			   10)){
	      list = ags_file_lookup_find_by_node(file->lookup,
						  value_node);
	  
	      if(list != NULL){
		file_lookup = AGS_FILE_LOOKUP(list->data);
		g_signal_connect_after(G_OBJECT(file_lookup), "resolve\0",
				       G_CALLBACK(ags_file_read_recall_container_resolve_value), gobject);
	      }
	    }
	  }

	  value_node = value_node->next;
	}
      }
    }

    child = child->next;
  }
}

void
ags_file_read_recall_container_resolve_value(AgsFileLookup *file_lookup,
					     AgsRecallContainer *recall_container)
{
  GObject *gobject;
  GValue *value;

  value = file_lookup->ref;

  if(G_VALUE_HOLDS(value, G_TYPE_OBJECT)){
    gobject = g_value_get_object(value);

    if(gobject == NULL){
      return;
    }

    g_object_set(gobject,
		 "recall-container\0", recall_container,
		 NULL);
  }
}

xmlNode*
ags_file_write_recall_container(AgsFile *file, xmlNode *parent, AgsRecallContainer *recall_container)
{
  xmlNode *node;
  GParameter *parameter;
  GList *list;
  gchar *id;
  gint n_params;

  auto GParameter* ags_file_write_recall_container_parameter(GList *list, GParameter *parameter, gchar *prop, gint *n_params);

  GParameter* ags_file_write_recall_container_parameter(GList *list, GParameter *parameter, gchar *prop, gint *n_params){
    gint i;

    if(n_params == NULL){
      i = 0;
    }else{
      i = *n_params;
    }

    while(list != NULL){
      if((AGS_RECALL_TEMPLATE & (AGS_RECALL(list->data)->flags)) == 0){
	list = list->next;
	continue;
      }

      if(parameter == NULL){
	parameter = (GParameter *) malloc(sizeof(GParameter));
      }else{
	parameter = (GParameter *) realloc(parameter,
					   (i + 1) * sizeof(GParameter));
      }

      parameter[i].name = prop;

      memset(&(parameter[i].value), 0, sizeof(GValue));
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value),
			 G_OBJECT(list->data));

      list = list->next;
      i++;
    }

    if(n_params != NULL){
      *n_params = i;
    }

    return(parameter);
  }

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-recall-container\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", recall_container,
				   NULL));

  xmlAddChild(parent,
	      node);

  /* child elements */
  parameter = NULL;
  n_params = 0;

  if(recall_container->recall_audio != NULL){
    parameter = (GParameter *) malloc(sizeof(GParameter));

    parameter[0].name = "recall-audio\0";

    memset(&(parameter[0].value), 0, sizeof(GValue));
    g_value_init(&(parameter[0].value), G_TYPE_OBJECT);
    g_value_set_object(&(parameter[0].value),
		       recall_container->recall_audio);

    n_params++;
  }

  list = ags_recall_container_get_recall_audio_run(recall_container);
  parameter = ags_file_write_recall_container_parameter(list, parameter, "recall-audio-run\0", &n_params);

  list = ags_recall_container_get_recall_channel(recall_container);
  parameter = ags_file_write_recall_container_parameter(list, parameter, "recall-channel\0", &n_params);

  list = ags_recall_container_get_recall_channel_run(recall_container);
  parameter = ags_file_write_recall_container_parameter(list, parameter, "recall-channel-run\0", &n_params);

  ags_file_util_write_parameter(file,
				node,
				ags_id_generator_create_uuid(),
				parameter, n_params);

  return(node);
}

void
ags_file_read_recall_container_list(AgsFile *file, xmlNode *node, GList **recall_container)
{
  AgsRecallContainer *current;
  GList *list;
  xmlNode *child;

  list = NULL;
  child = node->children;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", list,
				   NULL));
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-recall-container\0",
		     21)){
	current = NULL;
	ags_file_read_recall_container(file,
				       child,
				       &current);

	list = g_list_prepend(list,
			      current);
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *recall_container = list;
}

xmlNode*
ags_file_write_recall_container_list(AgsFile *file, xmlNode *parent, GList *recall_container)
{
  GList *list;
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-recall-container-list\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", recall_container,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = recall_container;

  while(list != NULL){
    ags_file_write_recall_container(file,
				    node,
				    AGS_RECALL_CONTAINER(list->data));

    list = list->next;
  }

  return(node);
}

void
ags_file_read_recall_audio(AgsFile *file, xmlNode *node, AgsRecall *recall)
{
  AgsRecallAudio *recall_audio;
  xmlNode *child;

  recall_audio = AGS_RECALL_AUDIO(recall);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", recall_audio,
				   NULL));

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrcmp((xmlChar *) child->name,
		    (xmlChar *) ags_plugin_get_xml_type(AGS_PLUGIN(recall)))){
	ags_plugin_read(file,
			child,
			AGS_PLUGIN(recall));
      }
    }

    child = child->next;
  }
}

xmlNode*
ags_file_write_recall_audio(AgsFile *file, xmlNode *parent, AgsRecall *recall)
{
  AgsRecallAudio *recall_audio;
  xmlNode *node;
  gchar *id;

  recall_audio = AGS_RECALL_AUDIO(recall);

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-recall-audio\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
  
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", recall,
				   NULL));
  
  xmlAddChild(parent,
	      node);

  /* child elements */
  ags_plugin_write(file,
		   node,
		   AGS_PLUGIN(recall));

  return(node);
}

void
ags_file_read_recall_audio_run(AgsFile *file, xmlNode *node, AgsRecall *recall)
{
  AgsRecallAudioRun *recall_audio_run;
  xmlNode *child;

  recall_audio_run = AGS_RECALL_AUDIO_RUN(recall);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", recall_audio_run,
				   NULL));

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrcmp((xmlChar *) child->name,
		    (xmlChar *) ags_plugin_get_xml_type(AGS_PLUGIN(recall)))){
	ags_plugin_read(file,
			child,
			AGS_PLUGIN(recall));
      }
    }

    child = child->next;
  }
}

xmlNode*
ags_file_write_recall_audio_run(AgsFile *file, xmlNode *parent, AgsRecall *recall)
{
  AgsRecallAudioRun *recall_audio_run;
  xmlNode *node;
  gchar *id;

  recall_audio_run = AGS_RECALL_AUDIO_RUN(recall);

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-recall-audio-run\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", recall_audio_run,
				   NULL));
  
  xmlAddChild(parent,
	      node);

  /* child elements */
  ags_plugin_write(file,
		   node,
		   AGS_PLUGIN(recall));

  return(node);
}

void
ags_file_read_recall_channel(AgsFile *file, xmlNode *node, AgsRecall *recall)
{
  AgsRecallChannel *recall_channel;
  xmlNode *child;

  recall_channel = AGS_RECALL_CHANNEL(recall);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", recall_channel,
				   NULL));

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrcmp((xmlChar *) child->name,
		    (xmlChar *) ags_plugin_get_xml_type(AGS_PLUGIN(recall)))){
	ags_plugin_read(file,
			child,
			AGS_PLUGIN(recall));
      }
    }

    child = child->next;
  }
}

xmlNode*
ags_file_write_recall_channel(AgsFile *file, xmlNode *parent, AgsRecall *recall)
{
  AgsRecallChannel *recall_channel;
  xmlNode *node;
  gchar *id;

  recall_channel = AGS_RECALL_CHANNEL(recall);

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-recall-channel\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", recall_channel,
				   NULL));
  
  xmlAddChild(parent,
	      node);

  /* child elements */
  ags_plugin_write(file,
		   node,
		   AGS_PLUGIN(recall));

  return(node);
}

void
ags_file_read_recall_channel_run(AgsFile *file, xmlNode *node, AgsRecall *recall)
{
  AgsRecallChannelRun *recall_channel_run;
  xmlNode *child;

  recall_channel_run = AGS_RECALL_CHANNEL_RUN(recall);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", recall_channel_run,
				   NULL));

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrcmp((xmlChar *) child->name,
		    (xmlChar *) ags_plugin_get_xml_type(AGS_PLUGIN(recall)))){
	ags_plugin_read(file,
			child,
			AGS_PLUGIN(recall));
      }
    }

    child = child->next;
  }
}

xmlNode*
ags_file_write_recall_channel_run(AgsFile *file, xmlNode *parent, AgsRecall *recall)
{
  AgsRecallChannelRun *recall_channel_run;
  xmlNode *node;
  gchar *id;

  recall_channel_run = AGS_RECALL_CHANNEL_RUN(recall);

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-recall-channel-run\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", recall_channel_run,
				   NULL));
  
  xmlAddChild(parent,
	      node);

  /* child elements */
  ags_plugin_write(file,
		   node,
		   AGS_PLUGIN(recall));

  return(node);
}

void
ags_file_read_recall_recycling(AgsFile *file, xmlNode *node, AgsRecall *recall)
{
  AgsRecallRecycling *recall_recycling;
  xmlNode *child;

  recall_recycling = AGS_RECALL_RECYCLING(recall);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", recall_recycling,
				   NULL));


  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrcmp((xmlChar *) child->name,
		    (xmlChar *) ags_plugin_get_xml_type(AGS_PLUGIN(recall)))){
	ags_plugin_read(file,
			child,
			AGS_PLUGIN(recall));
      }
    }
    
    child = child->next;
  }
}

xmlNode*
ags_file_write_recall_recycling(AgsFile *file, xmlNode *parent, AgsRecall *recall)
{
  AgsRecallRecycling *recall_recycling;
  xmlNode *node;
  gchar *id;

  recall_recycling = AGS_RECALL_RECYCLING(recall);

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-recall-recycling\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", recall_recycling,
				   NULL));
  
  xmlAddChild(parent,
	      node);

  /* child elements */
  ags_plugin_write(file,
		   node,
		   AGS_PLUGIN(recall));

  return(node);
}

void
ags_file_read_recall_audio_signal(AgsFile *file, xmlNode *node, AgsRecall *recall)
{
  AgsRecallAudioSignal *recall_audio_signal;
  xmlNode *child;

  recall_audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", recall_audio_signal,
				   NULL));

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrcmp((xmlChar *) child->name,
		    (xmlChar *) ags_plugin_get_xml_type(AGS_PLUGIN(recall)))){
	ags_plugin_read(file,
			child,
			AGS_PLUGIN(recall));
      }
    }

    child = child->next;
  }
}

xmlNode*
ags_file_write_recall_audio_signal(AgsFile *file, xmlNode *parent, AgsRecall *recall)
{
  AgsRecallAudioSignal *recall_audio_signal;
  xmlNode *node;
  gchar *id;

  recall_audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall);

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-recall-audio-signal\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", recall_audio_signal,
				   NULL));
  
  xmlAddChild(parent,
	      node);

  /* child elements */
  ags_plugin_write(file,
		   node,
		   AGS_PLUGIN(recall));

  return(node);
}

void
ags_file_read_port(AgsFile *file, xmlNode *node, AgsPort **port)
{
  AgsPort *gobject;
  AgsFileLookup *file_lookup;
  xmlNode *child;
  GList *list;

  if(*port == NULL){
    gobject = g_object_new(AGS_TYPE_PORT,
			   NULL);
    *port = gobject;
  }else{
    gobject = *port;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  gobject->plugin_name = g_strdup((gchar *) xmlGetProp(node,
						       (xmlChar *) "plugin-name\0"));

  gobject->specifier = g_strdup((gchar *) xmlGetProp(node,
						     (xmlChar *) "specifier\0"));

  gobject->control_port = g_strdup((gchar *) xmlGetProp(node,
							(xmlChar *) "control-port\0"));

  gobject->port_value_is_pointer = g_ascii_strtoull((gchar *) xmlGetProp(node,
									 (xmlChar *) "port-data-is-pointer\0"),
						    NULL,
						    10);
  gobject->port_value_type = g_type_from_name((gchar *) xmlGetProp(node,
								   (xmlChar *) "port-data-type\0"));

  gobject->port_value_size = g_ascii_strtoull((gchar *) xmlGetProp(node,
								   (xmlChar *) "port-data-size\0"),
					      NULL,
					      10);
  gobject->port_value_length = g_ascii_strtoull((gchar *) xmlGetProp(node,
								     (xmlChar *) "port-data-length\0"),
						NULL,
						10);

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-value\0",
		     10)){
	GValue *value;

	//FIXME:JK: memory leak
	value = (GValue *) g_new0(GValue, 1);
	memset(value, 0, sizeof(GValue));
	g_value_init(value,
		     gobject->port_value_type);

	ags_file_util_read_value(file,
				 child, NULL,
				 value, NULL);

	if(gobject->port_value_type == G_TYPE_POINTER ||
	   gobject->port_value_type == G_TYPE_OBJECT){
	  list = ags_file_lookup_find_by_node(file->lookup,
					      child);
	  
	  if(list != NULL){
	    file_lookup = AGS_FILE_LOOKUP(list->data);
	    
	    g_signal_connect_after(G_OBJECT(file_lookup), "resolve\0",
				   G_CALLBACK(ags_file_read_port_resolve_port_value), gobject);
	  }
	}else{
	  ags_port_safe_write((AgsPort *) gobject,
			      value);
	}
      }
    }

    child = child->next;
  }
}

void
ags_file_read_port_resolve_port_value(AgsFileLookup *file_lookup,
				      AgsPort *port)
{
  AgsFileIdRef *file_id_ref;
  
  ags_port_safe_write(port,
		      (GValue *) file_lookup->ref);

  file_id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file,
								   port);
  ags_file_id_ref_resolved(file_id_ref);
}

xmlNode*
ags_file_write_port(AgsFile *file, xmlNode *parent, AgsPort *port)
{
  xmlNode *node;
  gchar *id;
  GValue *a;

  if(!AGS_IS_PORT(port)){
    return(node);
  }

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-port\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", port,
				   NULL));

  xmlNewProp(node,
	     (xmlChar *) BAD_CAST "plugin-name\0",
	     (xmlChar *) port->plugin_name);

  xmlNewProp(node,
	     (xmlChar *) BAD_CAST "specifier\0",
	     (xmlChar *) port->specifier);

  xmlNewProp(node,
	     (xmlChar *) BAD_CAST "control-port\0",
	     (xmlChar *) port->control_port);

  xmlNewProp(node,
	     (xmlChar *) BAD_CAST "port-data-is-pointer\0",
	     (xmlChar *) g_strdup_printf("%d\0", port->port_value_is_pointer));

  xmlNewProp(node,
	     (xmlChar *) BAD_CAST "port-data-type\0",
	     (xmlChar *) g_strdup(g_type_name(port->port_value_type)));

  xmlNewProp(node,
	     (xmlChar *) BAD_CAST "port-data-size\0",
	     (xmlChar *) g_strdup_printf("%d\0", port->port_value_size));

  xmlNewProp(node,
	     (xmlChar *) BAD_CAST "port-data-length\0",
	     (xmlChar *) g_strdup_printf("%d\0", port->port_value_length));

  xmlAddChild(parent,
	      node);

  /* child elements */
  a = g_new0(GValue, 1);

  if(port->port_value_is_pointer){
    if(port->port_value_type == G_TYPE_BOOLEAN){
      gboolean *ptr;

      ptr = (gboolean *) port->port_value.ags_port_boolean_ptr;

      g_value_init(a,
		   G_TYPE_POINTER);
      g_value_set_pointer(a,
			  ptr);
    }else if(port->port_value_type == G_TYPE_UINT64){
      guint64 *ptr;

      ptr = (guint64 *) port->port_value.ags_port_uint_ptr;

      g_value_init(a,
		   G_TYPE_POINTER);
      g_value_set_pointer(a,
			  ptr);
    }else if(port->port_value_type == G_TYPE_INT64){
      gint64 *ptr;

      ptr = (gint64 *) port->port_value.ags_port_int_ptr;

      g_value_init(a,
		   G_TYPE_POINTER);
      g_value_set_pointer(a,
			  ptr);
    }else if(port->port_value_type == G_TYPE_FLOAT){
      gfloat *ptr;
      
      ptr = (gfloat *) port->port_value.ags_port_float_ptr;
      
      g_value_init(a,
		   G_TYPE_POINTER);
      g_value_set_pointer(a,
			  ptr);
    }else if(port->port_value_type == G_TYPE_DOUBLE){
      gdouble *ptr;

      ptr = (gdouble *) port->port_value.ags_port_double_ptr;

      g_value_init(a,
		   G_TYPE_POINTER);
      g_value_set_pointer(a,
			  ptr);
    }else if(port->port_value_type == G_TYPE_POINTER){
      g_value_init(a,
		   G_TYPE_POINTER);
      g_value_set_pointer(a,
			  port->port_value.ags_port_pointer);
    }
  }else{
    if(port->port_value_type == G_TYPE_BOOLEAN){
      g_value_init(a,
		   G_TYPE_BOOLEAN);
      g_value_set_boolean(a,
			  port->port_value.ags_port_boolean);
    }else if(port->port_value_type == G_TYPE_UINT64){
      g_value_init(a,
		   G_TYPE_UINT64);
      g_value_set_uint64(a,
			 port->port_value.ags_port_uint);
    }else if(port->port_value_type == G_TYPE_INT64){
      g_value_init(a,
		   G_TYPE_INT64);
      g_value_set_int64(a,
			port->port_value.ags_port_int);
    }else if(port->port_value_type == G_TYPE_FLOAT){
      g_value_init(a,
		   G_TYPE_FLOAT);
      g_value_set_float(a,
			port->port_value.ags_port_float);
    }else if(port->port_value_type == G_TYPE_DOUBLE){
      g_value_init(a,
		   G_TYPE_DOUBLE);
      g_value_set_double(a,
			 port->port_value.ags_port_double);
    }else if(port->port_value_type == G_TYPE_OBJECT){
      g_value_init(a,
		   G_TYPE_OBJECT);
      g_value_set_object(a,
			 port->port_value.ags_port_object);
    }
  }

  /*  */
  ags_file_util_write_value(file,
  			    node,
  			    ags_id_generator_create_uuid(),
  			    a, port->port_value_type, port->port_value_size);

  return(node);
}

void
ags_file_read_port_list(AgsFile *file, xmlNode *node, GList **port)
{
  AgsPort *current;
  xmlNode *child;
  GList *list;

  if(*port != NULL){
    list = *port;
  }else{
    list = NULL;
  }

  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-port\0",
		     9)){

	if(*port != NULL){
	  GList *list;

	  list = ags_port_find_specifier(*port,
					 (gchar *) xmlGetProp(child, (xmlChar *) "specifier\0"));

	  if(list == NULL){
	    child = child->next;

	    continue;
	  }else{
	    current = list->data;
	    g_message("found: %s\0", (gchar *) xmlGetProp(child, (xmlChar *) "specifier\0"));
	  }
	}else{
	  current = NULL;
	}

	ags_file_read_port(file,
			   child,
			   &current);
    
	if(*port == NULL){
	  list = g_list_prepend(list,
				current);
	}
      }
    }
    
    child = child->next;
  }

  if(*port == NULL){
    list = g_list_reverse(list);
    
    /* set return value */
    *port = list;
  }

  /* add id ref */
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", (gchar *) xmlGetProp(node,
													   (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", *port,
				   NULL));
}

xmlNode*
ags_file_write_port_list(AgsFile *file, xmlNode *parent, GList *port)
{
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-port-list\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", port,
				   NULL));

  xmlAddChild(parent,
	      node);

  /* child elements */
  list = port;

  while(list != NULL){
    ags_file_write_port(file,
			node,
			AGS_PORT(list->data));
    
    list = list->next;
  }

  return(node);
}

void
ags_file_read_recycling(AgsFile *file, xmlNode *node, AgsRecycling **recycling)
{
  AgsRecycling *gobject;
  xmlNode *child;

  if(*recycling == NULL){
    gobject = (AgsRecycling *) g_object_new(AGS_TYPE_RECYCLING,
					    NULL);

    *recycling = gobject;
  }else{
    gobject = *recycling;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  gobject->flags = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								 (xmlChar *) AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-audio-signal-list\0",
		     21)){
	if((AGS_FILE_READ_AUDIO_SIGNAL & (file->flags)) != 0){
	  ags_file_read_audio_signal_list(file,
					  child,
					  &gobject->audio_signal);
	}
      }
    }

    child = child->next;
  }
}

xmlNode*
ags_file_write_recycling(AgsFile *file, xmlNode *parent, AgsRecycling *recycling)
{
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-recycling\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", recycling,
				   NULL));
  
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_FLAGS_PROP,
	     (xmlChar *) g_strdup_printf("%x\0", recycling->flags));

  xmlAddChild(parent,
	      node);

  /* child elements */
  if((AGS_FILE_WRITE_AUDIO_SIGNAL & (file->flags)) != 0){
    ags_file_write_audio_signal_list(file,
				     node,
				     recycling->audio_signal);
  }
    
  return(node);
}

void
ags_file_read_recycling_list(AgsFile *file, xmlNode *node, GList **recycling)
{
  AgsRecycling *current;
  xmlNode *child;
  GList *list;

  list = NULL;
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-recycling\0",
		     14)){
	current = NULL;
	ags_file_read_recycling(file,
				child,
				&current);
    
	list = g_list_prepend(list,
			      current);
      }
    }
    
    child = child->next;
  }

  list = g_list_reverse(list);
  *recycling = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_recycling_list(AgsFile *file, xmlNode *parent, GList *recycling)
{
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-recycling-list\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", recycling,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = recycling;

  while(list != NULL){
    ags_file_write_recycling(file,
			     node,
			     AGS_RECYCLING(list->data));
    
    list = list->next;
  }

  return(node);
}

void
ags_file_read_audio_signal(AgsFile *file, xmlNode *node, AgsAudioSignal **audio_signal)
{
  AgsAudioSignal *gobject;
  xmlNode *child;

  if(*audio_signal == NULL){
    gobject = (AgsAudioSignal *) g_object_new(AGS_TYPE_AUDIO_SIGNAL,
					      NULL);

    *audio_signal = gobject;
  }else{
    gobject = *audio_signal;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  gobject->flags = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								 (xmlChar *) AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  gobject->samplerate = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								      (xmlChar *) "samplerate\0"),
						 NULL,
						 10);

  gobject->buffer_size = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								       (xmlChar *) "buffer-size\0"),
						  NULL,
						  10);

  gobject->format = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								  (xmlChar *) "format\0"),
					     NULL,
					     10);

  gobject->length = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								  (xmlChar *) "length\0"),
					     NULL,
					     10);

  gobject->last_frame = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								      (xmlChar *) "last-frame\0"),
						 NULL,
						 10);
  
  gobject->loop_start = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								      (xmlChar *) "loop-start\0"),
						 NULL,
						 10);
  
  gobject->loop_end = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								    (xmlChar *) "loop-end\0"),
					       NULL,
					       10);
  
  gobject->delay = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								 (xmlChar *) "delay\0"),
					    NULL,
					    10);
  
  gobject->attack = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								  (xmlChar *) "attack\0"),
					     NULL,
					     10);
  
  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-stream-list\0",
		     21)){
	ags_file_read_stream_list(file, child,
				  &gobject->stream_beginning,
				  gobject->buffer_size);
      }
    }

    child = child->next;
  }
}

xmlNode*
ags_file_write_audio_signal(AgsFile *file, xmlNode *parent, AgsAudioSignal *audio_signal)
{
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-audio-signal\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", audio_signal,
				   NULL));
  
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_FLAGS_PROP,
	     (xmlChar *) g_strdup_printf("%x\0", audio_signal->flags));

  xmlNewProp(node,
	     (xmlChar *) "samplerate\0",
	     (xmlChar *) g_strdup_printf("%d\0", audio_signal->samplerate));

  xmlNewProp(node,
	     (xmlChar *) "buffer-size\0",
	     (xmlChar *) g_strdup_printf("%d\0", audio_signal->buffer_size));

  xmlNewProp(node,
	     (xmlChar *) "format\0",
	     (xmlChar *) g_strdup_printf("%d\0", audio_signal->format));

  xmlNewProp(node,
	     (xmlChar *) "length\0",
	     (xmlChar *) g_strdup_printf("%d\0", audio_signal->length));

  xmlNewProp(node,
	     (xmlChar *) "last-frame\0",
	     (xmlChar *) g_strdup_printf("%d\0", audio_signal->last_frame));

  xmlNewProp(node,
	     (xmlChar *) "loop-start\0",
	     (xmlChar *) g_strdup_printf("%d\0", audio_signal->loop_start));

  xmlNewProp(node,
	     (xmlChar *) "loop-end\0",
	     (xmlChar *) g_strdup_printf("%d\0", audio_signal->loop_end));

  xmlNewProp(node,
	     (xmlChar *) "delay\0",
	     (xmlChar *) g_strdup_printf("%d\0", audio_signal->delay));

  xmlNewProp(node,
	     (xmlChar *) "attack\0",
	     (xmlChar *) g_strdup_printf("%d\0", audio_signal->attack));

  xmlAddChild(parent,
	      node);

  /* child elements */
  ags_file_write_stream_list(file, node,
			     audio_signal->stream_beginning,
			     audio_signal->buffer_size);
  
  return(node);
}

void
ags_file_read_audio_signal_list(AgsFile *file, xmlNode *node, GList **audio_signal)
{
  AgsAudioSignal *current;
  xmlNode *child;
  GList *list;

  list = NULL;
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-audio-signal\0",
		     17)){
	current = NULL;
	ags_file_read_audio_signal(file,
				   child,
				   &current);
    
	list = g_list_prepend(list,
			      current);
      }
    }
    
    child = child->next;
  }

  list = g_list_reverse(list);
  *audio_signal = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_audio_signal_list(AgsFile *file, xmlNode *parent, GList *audio_signal)
{
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-audio-signal-list\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", audio_signal,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = audio_signal;

  while(list != NULL){
    ags_file_write_audio_signal(file,
				node,
				AGS_AUDIO_SIGNAL(list->data));
    
    list = list->next;
  }

  return(node);
}

void
ags_file_read_stream(AgsFile *file, xmlNode *node,
		     GList **stream, guint *index,
		     guint buffer_size)
{
  GList *list;
  gchar *encoding;
  gchar *demuxer;
  xmlChar *content;

  if(*stream == NULL){
    list = g_list_alloc();

    *stream = list;
  }else{
    list = *stream;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", list,
				   NULL));

  if(index != NULL){
    *index = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
							   (xmlChar *) "index\0"),
				      NULL,
				      10);
  }

  encoding = (gchar *) xmlGetProp(node,
				  (xmlChar *) "encoding\0");

  if(!xmlStrncmp((xmlChar *) encoding,
		 (xmlChar *) "base64\0",
		 7)){
    demuxer = (gchar *) xmlGetProp(node,
				   (xmlChar *) "demuxer\0");

    if(!xmlStrncmp((xmlChar *) demuxer,
		   (xmlChar *) "raw\0",
		   4)){
      content = node->content;

      //TODO:JK: verify
      list->data = g_base64_decode((gchar *) content,
				   NULL);
    }else{
      g_warning("ags_file_read_stream: unsupported demuxer %s\0", demuxer);
    }    
  }else{
    g_warning("ags_file_read_stream: unsupported encoding %s\0", encoding);
  }
}

xmlNode*
ags_file_write_stream(AgsFile *file, xmlNode *parent,
		      GList *stream, guint index,
		      guint buffer_size)
{
  xmlNode *node;
  xmlChar *content;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-stream\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", stream,
				   NULL));
  
  xmlNewProp(node,
	     (xmlChar *) "index\0",
	     (xmlChar *) g_strdup_printf("%d\0", index));

  xmlNewProp(node,
	     (xmlChar *) "encoding\0",
	     (xmlChar *) file->audio_encoding);

  xmlNewProp(node,
	     (xmlChar *) "demuxer\0",
	     (xmlChar *) file->audio_format);

  content = g_base64_encode(stream->data,
			    buffer_size);

  node->content = content;

  return(node);
}

void
ags_file_read_stream_list(AgsFile *file, xmlNode *node,
			  GList **stream,
			  guint buffer_size)
{
  GList *current;
  xmlNode *child;
  GList *list;
  guint *index;
  guint i;

  auto void ags_file_read_stream_list_sort(GList **stream, guint *index);

  void ags_file_read_stream_list_sort(GList **stream, guint *index){
    GList *start, *list;
    GList *sorted;
    guint stream_length;
    guint i, i_stop;
    guint j, k;

    start =
      list = *stream;

    stream_length = 
      i_stop = g_list_length(list);

    sorted = NULL;

    while(list != NULL){
      j = index[stream_length - i_stop];

      for(i = 0; i < stream_length - i_stop; i++){
	if(j < index[i]){
	  break;
	}
      }
      
      sorted = g_list_insert(sorted,
			     list->data,
			     i);
      
      i_stop--;
      list = list->next;
    }

    *stream = sorted;
    g_list_free(start);
  }

  child = node->children;

  list = NULL;
  index = NULL;

  for(i = 0; child != NULL; i++){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-stream\0",
		     11)){
	current = NULL;

	if(index == NULL){
	  index = (guint *) malloc(sizeof(guint));
	}else{
	  index = (guint *) realloc(index,
				    (i + 1) * sizeof(guint));
	}

	ags_file_read_stream(file, child,
			     &current, &(index[i]),
			     buffer_size);
    
	list = g_list_prepend(list,
			      current);
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  ags_file_read_stream_list_sort(&list, index);
  *stream = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_stream_list(AgsFile *file, xmlNode *parent,
			   GList *stream,
			   guint buffer_size)
{
  xmlNode *node;
  GList *list;
  gchar *id;
  guint i;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-stream-list\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", stream,
				   NULL));

  xmlAddChild(parent,
	      node);

  /* child elements */
  list = stream;

  for(i = 0; list != NULL; i++){
    ags_file_write_stream(file, node,
			  list, i,
			  buffer_size);
    
    list = list->next;
  }

  return(node);
}

void
ags_file_read_pattern(AgsFile *file, xmlNode *node, AgsPattern **pattern)
{
  AgsPattern *gobject;
  AgsFileLookup *file_lookup;
  xmlNode *child;
  guint dim[3];

  if(*pattern == NULL){
    gobject = (AgsPattern *) g_object_new(AGS_TYPE_PATTERN,
					  NULL);

    *pattern = gobject;
  }else{
    gobject = *pattern;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));
  
  dim[0] = g_ascii_strtoull((gchar *) xmlGetProp(node,
						 (xmlChar *) "dim-1st-level\0"),
			    NULL,
			    10);

  dim[1] = g_ascii_strtoull((gchar *) xmlGetProp(node,
						 (xmlChar *) "dim-2nd-level\0"),
			    NULL,
			    10);
  
  dim[2] = g_ascii_strtoull((gchar *) xmlGetProp(node,
						 (xmlChar *) "length\0"),
			    NULL,
			    10);

  ags_pattern_set_dim((AgsPattern *) gobject, dim[0], dim[1], dim[2]);

  /* port */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", gobject,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_read_pattern_resolve_port), gobject);

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-timestamp\0",
		     13)){
	ags_file_read_timestamp(file,
				child,
				(AgsTimestamp **) &gobject->timestamp);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-pattern-data-list\0",
			   21)){
	ags_file_read_pattern_data_list(file,
					child,
					gobject,
					gobject->dim[2]);
      }
    }

    child = child->next;
  }
}

void
ags_file_read_pattern_resolve_port(AgsFileLookup *file_lookup,
				   AgsPattern *pattern)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) (gchar *) xmlGetProp(file_lookup->node,
					 (xmlChar *) "port\0");

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file, xpath);

  if(id_ref != NULL){
    AgsPort *port;
    GValue value = {0,};

    port = (AgsPort *) id_ref->ref;

    g_object_set(G_OBJECT(pattern),
		 "port\0", (AgsPort *) port,
		 NULL);

    g_value_init(&value, G_TYPE_OBJECT);
    g_value_set_object(&value, pattern);
    ags_port_safe_write(AGS_PORT(id_ref->ref),
			&value);
  }
}

xmlNode*
ags_file_write_pattern(AgsFile *file, xmlNode *parent, AgsPattern *pattern)
{
  AgsFileLookup *file_lookup;
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-pattern\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", pattern,
				   NULL));

  xmlNewProp(node,
	     (xmlChar *) "dim-1st-level\0",
	     (xmlChar *) g_strdup_printf("%d\0",
					 pattern->dim[0]));

  xmlNewProp(node,
	     (xmlChar *) "dim-2nd-level\0",
	     (xmlChar *) g_strdup_printf("%d\0",
					 pattern->dim[1]));

  xmlNewProp(node,
	     (xmlChar *) "length\0",
	     (xmlChar *) g_strdup_printf("%d\0",
					 pattern->dim[2]));

  /* port */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", pattern,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_write_pattern_resolve_port), pattern);

  /*  */
  xmlAddChild(parent,
	      node);

  /* child elements */
  if(pattern->timestamp != NULL){
    ags_file_write_timestamp(file,
			     node,
			     (AgsTimestamp *) pattern->timestamp);
  }

  ags_file_write_pattern_data_list(file,
				   node,
				   pattern,
				   pattern->dim[2]);

  return(node);
}

void
ags_file_write_pattern_resolve_port(AgsFileLookup *file_lookup,
				    AgsPattern *pattern)
{
  AgsFileIdRef *id_ref;
  gchar *id;

  if(pattern->port == NULL){
    return;
  }

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, pattern->port);

  id = (gchar *) xmlGetProp(id_ref->node, (xmlChar *) AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     (xmlChar *) "port\0",
	     (xmlChar *) g_strdup_printf("xpath=//*[@id='%s']\0", id));
}

void
ags_file_read_pattern_list(AgsFile *file, xmlNode *node, GList **pattern)
{
  AgsPattern *current;
  xmlNode *child;
  GList *list;

  child = node->children;

  if(*pattern == NULL){
    list = NULL;
  }else{
    list = *pattern;
  }

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-pattern\0",
		     12)){
	if(*pattern == NULL){
	  current = NULL;
	}else{
	  current = list->data;
	}

	ags_file_read_pattern(file, child,
			      &current);
    
	if(*pattern == NULL){
	  list = g_list_prepend(list,
				current);
	}else{
	  list = list->next;
	}
      }
    }
    
    child = child->next;
  }

  if(*pattern == NULL){
    list = g_list_reverse(list);
    
    *pattern = list;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_pattern_list(AgsFile *file, xmlNode *parent, GList *pattern)
{
  xmlNode *node;
  GList *list;
  gchar *id;
  guint i;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-pattern-list\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", pattern,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = pattern;

  for(i = 0; list != NULL; i++){
    ags_file_write_pattern(file,
			   node,
			   AGS_PATTERN(list->data));
    
    list = list->next;
  }

  return(node);
}

void
ags_file_read_pattern_data(AgsFile *file, xmlNode *node,
			   AgsPattern *pattern, guint *i, guint *j,
			   guint length)
{
  xmlChar *content;
  xmlChar *coding;
  guint k;

  if(i != NULL){
    *i = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
						       (xmlChar *) "index-1st-level\0"),
				  NULL,
				  10);
  }

  if(j != NULL){
    *j = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
						       (xmlChar *) "index-2nd-level\0"),
				  NULL,
				  10);
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", pattern->pattern[*i][*j],
				   NULL));

  content = xmlNodeGetContent(node);
  coding = (gchar *) xmlGetProp(node,
				(xmlChar *) "coding\0");

  if(!xmlStrncmp(coding,
		 (xmlChar *) "human readable\0",
		 14)){
    for(k = 0; k < length; k++){
      if(!g_ascii_strncasecmp((gchar *) &(content[k]),
			"1\0",
			1)){
	ags_pattern_toggle_bit(pattern, *i, *j, k);
      }
    }
  }else{
    g_warning("ags_file_read_pattern_data - unsupported coding: %s\0", coding);
  }
}

xmlNode*
ags_file_write_pattern_data(AgsFile *file, xmlNode *parent,
			    AgsPattern *pattern, guint i, guint j,
			    guint length)
{
  xmlNode *node;
  GString *content;
  gchar *id;
  guint k;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-pattern-data\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", pattern->pattern[i][j],
				   NULL));

  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_FLAGS_PROP,
	     (xmlChar *) "0\0");

  xmlNewProp(node,
	     (xmlChar *) "index-1st-level\0",
	     (xmlChar *) g_strdup_printf("%d\0", i));

  xmlNewProp(node,
	     (xmlChar *) "index-2nd-level\0",
	     (xmlChar *) g_strdup_printf("%d\0", j));

  xmlNewProp(node,
	     (xmlChar *) "coding\0",
	     (xmlChar *) "human readable\0");

  content = g_string_sized_new(length + 1);

  for(k = 0; k < length; k++){
    g_string_insert_c(content, k, (gchar) (ags_pattern_get_bit(pattern, i, j, k) ? '1': '0'));
  }

  g_string_insert_c(content, k, (gchar) '\0');
  xmlNodeAddContent(node, BAD_CAST (content->str));

  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_file_read_pattern_data_list(AgsFile *file, xmlNode *node,
				AgsPattern *pattern,
				guint length)
{
  xmlNode *child;
  guint i, j;

  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-pattern-data\0",
		     17)){
	ags_file_read_pattern_data(file, child,
				   pattern, &i, &j,
				   length);
      }
    }
    
    child = child->next;
  }
  
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", pattern->pattern,
				   NULL));
}

xmlNode*
ags_file_write_pattern_data_list(AgsFile *file, xmlNode *parent,
				 AgsPattern *pattern,
				 guint length)
{
  xmlNode *node;
  gchar *id;
  guint i, j;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-pattern-data-list\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", pattern->pattern,
				   NULL));

  xmlAddChild(parent,
	      node);

  for(i = 0; i < pattern->dim[0]; i++){
    for(j = 0; j < pattern->dim[1]; j++){
      ags_file_write_pattern_data(file, node,
				  pattern, i, j,
				  length);
    }
  }

  return(node);
}

void
ags_file_read_notation(AgsFile *file, xmlNode *node, AgsNotation **notation)
{
  AgsNotation *gobject;
  AgsFileLookup *file_lookup;
  xmlNode *child;
  xmlChar *prop;

  if(*notation == NULL){
    gobject = (AgsNotation *) g_object_new(AGS_TYPE_NOTATION,
					   NULL);

    *notation = gobject;
  }else{
    gobject = *notation;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));
  
  gobject->flags = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								 (xmlChar *) AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  gobject->audio_channel = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
									 (xmlChar *) "audio-channel\0"),
						    NULL,
						    10);
  
  gobject->key = g_strdup((gchar *) xmlGetProp(node,
					       (xmlChar *) "key\0"));
  
  gobject->base_frequency = (gdouble) g_ascii_strtod((gchar *) xmlGetProp(node,
									  (xmlChar *) "base-frequency\0"),
						     NULL);

  gobject->tact = (gdouble) g_ascii_strtod((gchar *) xmlGetProp(node,
								(xmlChar *) "tact\0"),
					   NULL);

  gobject->bpm = (gdouble) g_ascii_strtod((gchar *) xmlGetProp(node,
							       (xmlChar *) "bpm\0"),
					  NULL);

  gobject->maximum_note_length = (gdouble) g_ascii_strtod((gchar *) xmlGetProp(node,
									       (xmlChar *) "max-note-length\0"),
							  NULL);

  prop = (gchar *) xmlGetProp(node,
			      (xmlChar *) "loop-start\0");

  if(prop != NULL){
    gobject->start_loop = (gdouble) g_ascii_strtod((gchar *) prop,
						   NULL);
    gobject->end_loop = (gdouble) g_ascii_strtod((gchar *) xmlGetProp(node,
								      (xmlChar *) "loop-end\0"),
						 NULL);
  }

  /* port */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", gobject,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_read_notation_resolve_port), gobject);

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-timestamp\0",
		     13)){
	ags_file_read_timestamp(file,
				child,
				(AgsTimestamp **) &gobject->timestamp);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-note-list\0",
			   13)){
	ags_file_read_note_list(file,
				child,
				&gobject->notes);
      }
    }

    child = child->next;
  }
}

void
ags_file_read_notation_resolve_port(AgsFileLookup *file_lookup,
				    AgsNotation *notation)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) (gchar *) xmlGetProp(file_lookup->node,
					 (xmlChar *) "port\0");

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file, xpath);

  if(id_ref != NULL){
    AgsPort *port;
    GValue value = {0,};

    port = (AgsPort *) id_ref->ref;

    g_object_set(G_OBJECT(notation),
		 "port\0", port,
		 NULL);

    g_value_init(&value, G_TYPE_OBJECT);
    g_value_set_object(&value, notation);
    ags_port_safe_write(AGS_PORT(port),
			&value);
  }
}

xmlNode*
ags_file_write_notation(AgsFile *file, xmlNode *parent, AgsNotation *notation)
{
  AgsFileLookup *file_lookup;
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-notation\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", notation,
				   NULL));
  
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_FLAGS_PROP,
	     (xmlChar *) g_strdup_printf("%x\0", notation->flags));

  xmlNewProp(node,
	     (xmlChar *) "key\0",
	     (xmlChar *) notation->key);

  xmlNewProp(node,
	     (xmlChar *) "audio-channel\0",
	     (xmlChar *) g_strdup_printf("%d\0", notation->audio_channel));

  xmlNewProp(node,
	     (xmlChar *) "base-frequency\0",
	     (xmlChar *) g_strdup_printf("%f\0", notation->base_frequency));

  xmlNewProp(node,
	     (xmlChar *) "tact\0",
	     (xmlChar *) g_strdup_printf("%f\0", notation->tact));

  xmlNewProp(node,
	     (xmlChar *) "bpm\0",
	     (xmlChar *) g_strdup_printf("%f\0", notation->bpm));
  
  xmlNewProp(node,
	     (xmlChar *) "max-note-length\0",
	     (xmlChar *) g_strdup_printf("%f\0", notation->maximum_note_length));

  xmlNewProp(node,
	     (xmlChar *) "loop-start\0",
	     (xmlChar *) g_strdup_printf("%f\0", notation->start_loop));

  xmlNewProp(node,
	     (xmlChar *) "loop-end\0",
	     (xmlChar *) g_strdup_printf("%f\0", notation->end_loop));

  /* port */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", notation,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_write_notation_resolve_port), notation);

  /*  */
  xmlAddChild(parent,
	      node);

  /* child elements */
  ags_file_write_timestamp(file,
			   node,
			   (AgsTimestamp *) notation->timestamp);

  ags_file_write_note_list(file,
			   node,
			   notation->notes);

  return(node);
}

void
ags_file_write_notation_resolve_port(AgsFileLookup *file_lookup,
				     AgsNotation *notation)
{
  AgsFileIdRef *id_ref;
  gchar *id;

  if(notation->port == NULL){
    return;
  }

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, notation->port);

  id = (gchar *) xmlGetProp(id_ref->node, (xmlChar *) AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     (xmlChar *) "port\0",
	     (xmlChar *) g_strdup_printf("xpath=//*[@id='%s']\0", id));
}

void
ags_file_read_notation_list(AgsFile *file, xmlNode *node, GList **notation)
{
  AgsNotation *current;
  xmlNode *child;
  GList *list;

  child = node->children;

  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-notation\0",
		     13)){
	current = NULL;
    
	ags_file_read_notation(file, child,
			       &current);
    
	list = g_list_prepend(list,
			      current);
      }
    }
    
    child = child->next;
  }

  list = g_list_reverse(list);

  *notation = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_notation_list(AgsFile *file, xmlNode *parent, GList *notation)
{
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-notation-list\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", notation,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = notation;

  while(list != NULL){
    ags_file_write_notation(file,
			    node,
			    AGS_NOTATION(list->data));
    
    list = list->next;
  }

  return(node);
}

void
ags_file_read_note(AgsFile *file, xmlNode *node, AgsNote **note)
{
  AgsNote *gobject;

  if(*note == NULL){
    gobject = (AgsNote *) g_object_new(AGS_TYPE_NOTE,
				       NULL);

    *note = gobject;
  }else{
    gobject = *note;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));
  
  gobject->flags = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								 (xmlChar *) AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  gobject->x[0] = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								(xmlChar *) "x0\0"),
					   NULL,
					   10);
  
  gobject->x[1] = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								(xmlChar *) "x1\0"),
					   NULL,
					   10);
  
  gobject->y = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
							     (xmlChar *) "y\0"),
					NULL,
					10);

  gobject->name = g_strdup((gchar *) xmlGetProp(node,
						(xmlChar *) "name\0"));

  gobject->frequency = (gdouble) g_ascii_strtod((gchar *) xmlGetProp(node,
								     (xmlChar *) "frequency\0"),
						NULL);
}

xmlNode*
ags_file_write_note(AgsFile *file, xmlNode *parent, AgsNote *note)
{
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-note\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", note,
				   NULL));
  
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_FLAGS_PROP,
	     (xmlChar *) g_strdup_printf("%x\0", note->flags));

  xmlNewProp(node,
	     (xmlChar *) "x0\0",
	     (xmlChar *) g_strdup_printf("%d\0", note->x[0]));

  xmlNewProp(node,
	     (xmlChar *) "x1\0",
	     (xmlChar *) g_strdup_printf("%d\0", note->x[1]));

  xmlNewProp(node,
	     (xmlChar *) "y\0",
	     (xmlChar *) g_strdup_printf("%d\0", note->y));

  xmlNewProp(node,
	     (xmlChar *) "name\0",
	     (xmlChar *) note->name);

  xmlNewProp(node,
	     (xmlChar *) "frequency\0",
	     (xmlChar *) g_strdup_printf("%f\0", note->frequency));

  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_file_read_note_list(AgsFile *file, xmlNode *node, GList **note)
{
  AgsNote *current;
  xmlNode *child;
  GList *list;

  child = node->children;

  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-note\0",
		     9)){
	current = NULL;
    
	ags_file_read_note(file, child,
			   &current);
    
	list = g_list_prepend(list,
			      current);
      }
    }
    
    child = child->next;
  }
  
  list = g_list_reverse(list);

  *note = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_note_list(AgsFile *file, xmlNode *parent, GList *note)
{
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-note-list\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", note,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = note;

  while(list != NULL){
    ags_file_write_note(file,
			node,
			AGS_NOTE(list->data));
    
    list = list->next;
  }

  return(node);
}

void
ags_file_read_task(AgsFile *file, xmlNode *node, AgsTask **task)
{
  AgsTask *gobject;
  GParameter *parameter;
  xmlNode *child;
  char *type_name;
  guint n_params;

  if(*task == NULL){
    GType type;

    type_name = (gchar *) xmlGetProp(node,
				     (xmlChar *) AGS_FILE_TYPE_PROP);

    type = g_type_from_name(type_name);

    gobject = g_object_new(type,
			   NULL);

    *task = gobject;
  }else{
    gobject = *task;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  /*  */
  gobject->flags = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								 (xmlChar *) AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  gobject->name = (gchar *) (gchar *) xmlGetProp(node,
						 (xmlChar *) "name\0");

  gobject->delay = g_ascii_strtoull((gchar *) (gchar *) xmlGetProp(node,
								   (xmlChar *) "delay\0"),
				    NULL,
				    10);
  
  //TODO:JK: implement error message

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-parameter\0",
		     13)){
	AgsFileLookup *file_lookup;

	ags_file_util_read_parameter(file,
				     child,
				     NULL,
				     &parameter, &n_params, NULL);

	file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
						     "file\0", file,
						     "node\0", node,
						     "reference\0", parameter,
						     NULL);
	ags_file_add_lookup(file, (GObject *) file_lookup);
	g_signal_connect_after(G_OBJECT(file_lookup), "resolve\0",
			       G_CALLBACK(ags_file_read_task_resolve_parameter), gobject);
      }
    }

    child = child->next;
  }
}

void
ags_file_read_task_resolve_parameter(AgsFileLookup *file_lookup,
				     AgsTask *task)
{
  GParameter *parameter;
  GParamSpec **param_spec;
  guint n_properties;
  guint i, j;

  parameter = (GParameter *) file_lookup->ref;

  param_spec = g_object_class_list_properties(G_OBJECT_GET_CLASS(task),
					      &n_properties);

  for(i = 0, j = 0; i < n_properties; i++){
    if(g_type_is_a(param_spec[i]->owner_type,
		   AGS_TYPE_TASK)){
	
      g_object_set_property(G_OBJECT(task),
			    parameter[j].name,
			    &parameter[j].value);

      j++;
    }
  }
}

xmlNode*
ags_file_write_task(AgsFile *file, xmlNode *parent, AgsTask *task)
{
  GParameter *parameter;
  GParamSpec **param_spec;
  xmlNode *node;
  gchar *id;
  guint n_properties, n_params;
  guint i, j;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-task\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", task,
				   NULL));

  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_TYPE_PROP,
	     (xmlChar *) G_OBJECT_TYPE_NAME(task));

  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_FLAGS_PROP,
	     (xmlChar *) g_strdup_printf("%x\0", task->flags));

  /*  */  
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_NAME_PROP,
	     (xmlChar *) task->name);
  
  xmlNewProp(node,
	     (xmlChar *) "delay\0",
	     (xmlChar *) g_strdup_printf("%d\0",
					 task->delay));
  
  //TODO:JK: implement error message
  
  xmlAddChild(parent,
	      node);

  /* child parameters */
  param_spec = g_object_class_list_properties(G_OBJECT_GET_CLASS(task),
					      &n_properties);

  parameter = NULL;

  for(i = 0, j = 0; i < n_properties; i++){
    if(g_type_is_a(param_spec[i]->owner_type,
		   AGS_TYPE_TASK)){
      if(parameter == NULL){
	parameter = (GParameter *) g_new(GParameter,
					 1);
      }else{
	parameter = (GParameter *) g_renew(GParameter,
					   parameter,
					   (j + 1));
      }

      parameter[j].name = param_spec[i]->name;
      g_object_get_property(G_OBJECT(task),
			    param_spec[i]->name,
			    &(parameter[j].value));

      j++;
    }
  }

  n_params = j;

  ags_file_util_write_parameter(file,
				node,
				ags_id_generator_create_uuid(),
				parameter, n_params);

  return(node);
}

void
ags_file_read_task_list(AgsFile *file, xmlNode *node, GList **task)
{
  AgsTask *current;
  xmlNode *child;
  GList *list;

  child = node->children;

  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-task\0",
		     9)){
	current = NULL;
    
	ags_file_read_task(file, child,
			   &current);
    
	list = g_list_prepend(list,
			      current);
      }
    }
    
    child = child->next;
  }
  
  list = g_list_reverse(list);

  *task = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_task_list(AgsFile *file, xmlNode *parent, GList *task)
{
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-task-list\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", task,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = task;

  while(list != NULL){
    ags_file_write_task(file,
			node,
			AGS_TASK(list->data));
    
    list = list->next;
  }

  return(node);
}

void
ags_file_read_timestamp(AgsFile *file, xmlNode *node, AgsTimestamp **timestamp)
{
  AgsTimestamp *gobject;

  if(*timestamp == NULL){
    gobject = (AgsTimestamp *) g_object_new(AGS_TYPE_TIMESTAMP,
					    NULL);

    *timestamp = gobject;
  }else{
    gobject = *timestamp;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));
  
  gobject->flags = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								 (xmlChar *) AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  gobject->delay = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								 (xmlChar *) "delay\0"),
					    NULL,
					    10);
  
  gobject->attack = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								  (xmlChar *) "attack\0"),
					     NULL,
					     10);

  gobject->timer.unix_time.time_val = (guint) g_ascii_strtoull((gchar *) node->content,
							       NULL,
							       10);
}

xmlNode*
ags_file_write_timestamp(AgsFile *file, xmlNode *parent, AgsTimestamp *timestamp)
{
  xmlNode *node;
  gchar *id;

  if(timestamp == NULL){
    return(NULL);
  }

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-timestamp\0");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", timestamp,
				   NULL));
  
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_FLAGS_PROP,
	     (xmlChar *) g_strdup_printf("%x\0", timestamp->flags));

  xmlNewProp(node,
	     (xmlChar *) "delay\0",
	     (xmlChar *) g_strdup_printf("%d\0", timestamp->delay));

  xmlNewProp(node,
	     (xmlChar *) "attack\0",
	     (xmlChar *) g_strdup_printf("%d\0", timestamp->attack));

  xmlAddChild(parent,
	      node);

  xmlNodeAddContent(node,
		    (xmlChar *) g_strdup_printf("%d\0", timestamp->timer.unix_time.time_val));

  return(node);
}

void
ags_file_read_timestamp_list(AgsFile *file, xmlNode *node, GList **timestamp)
{
  AgsTimestamp *current;
  xmlNode *child;
  GList *list;

  child = node->children;

  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-timestamp\0",
		     14)){
	current = NULL;
    
	ags_file_read_timestamp(file, child,
				&current);
    
	list = g_list_prepend(list,
			      current);
      }
    }
    
    child = child->next;
  }
  
  list = g_list_reverse(list);

  *timestamp = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_timestamp_list(AgsFile *file, xmlNode *parent, GList *timestamp)
{
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-timestamp-list\0");

  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
  
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", timestamp,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = timestamp;
  
  while(list != NULL){
    ags_file_write_timestamp(file,
			     node,
			     AGS_TIMESTAMP(list->data));
    
    list = list->next;
  }

  return(node);
}

void
ags_file_read_embedded_audio(AgsFile *file, xmlNode *node,
			     gchar **embedded_audio)
{
  gchar *data;
  xmlChar *content;

  if(*embedded_audio == NULL){
    return;
  }else{
    data = *embedded_audio;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", data,
				   NULL));

  content = node->content;

  *embedded_audio = content;
}

xmlNode*
ags_file_write_embedded_audio(AgsFile *file, xmlNode *parent, gchar *embedded_audio)
{
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-embedded-audio\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", embedded_audio,
				   NULL));

  xmlNewProp(node,
	     "encoding\0",
	     g_strdup("base64\0"));

  xmlNewProp(node,
	     "demuxer\0",
	     g_strdup("raw\0"));

  xmlNodeSetContent(node,
		    embedded_audio);

  xmlAddChild(parent,
	      node);
}

void
ags_file_read_embedded_audio_list(AgsFile *file, xmlNode *node, GList **embedded_audio)
{
  gchar *current;
  GList *list;
  xmlNode *child;
  xmlChar *id;

  id = xmlGetProp(node, AGS_FILE_ID_PROP);

  child = node->children;
  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-embedded-audio\0",
		     11)){
	current = NULL;
	ags_file_read_embedded_audio(file, child, &current);
	list = g_list_prepend(list, current);
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *embedded_audio = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_embedded_audio_list(AgsFile *file, xmlNode *parent, GList *embedded_audio)
{
  gchar *current;
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-embedded-audio-list\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = embedded_audio;

  while(list != NULL){
    ags_file_write_embedded_audio(file, node, (gchar *) list->data);

    list = list->next;
  }

  return(node);
}

void
ags_file_read_file_link(AgsFile *file, xmlNode *node, AgsFileLink **file_link)
{
  AgsFileLink *gobject;
  AgsFileLaunch *file_launch;
  xmlNode *child;

  if(*file_link == NULL){
    gobject = g_object_new(AGS_TYPE_FILE_LINK,
			   NULL);
    *file_link = gobject;
  }else{
    gobject = *file_link;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "file\0", file,
					       "node\0", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start\0",
		   G_CALLBACK(ags_file_read_file_link_launch), gobject);
  ags_file_add_launch(file,
		      file_launch);
}

void
ags_file_read_file_link_launch(AgsFileLaunch *file_launch,
			       AgsFileLink *file_link)
{
  AgsFileIdRef *id_ref;
  AgsSoundcard *soundcard;
  AgsChannel *input;
  AgsAudioFile *audio_file;
  GList *audio_signal;
  xmlNode *node, *child;
  xmlChar *type;
  xmlChar *xpath;
  xmlChar *filename;
  xmlChar *encoding, *demuxer;

  node = file_launch->node;

  /*  */
  xpath = xmlGetProp(node,
		     "devout\0");
  
  /*  */
  input = NULL;
  id_ref = ags_file_find_id_ref_by_node(file_launch->file, node->parent->parent);

  if(id_ref != NULL){
    input = (AgsChannel *) id_ref->ref;
  }

  soundcard = AGS_AUDIO(input->audio)->soundcard;

  type = xmlGetProp(node,
		    "type\0");

  if(!xmlStrncmp(type,
		 "url\0",
		 4)){
    filename = xmlGetProp(node,
			  "filename\0");
    audio_file = ags_audio_file_new((gchar *) filename,
				    soundcard,
				    0, AGS_AUDIO(input->audio)->audio_channels);

    ags_audio_file_open(audio_file);
    ags_audio_file_read_audio_signal(audio_file);

    audio_signal = g_list_nth(audio_file->audio_signal,
			      input->audio_channel);

    AGS_AUDIO_SIGNAL(audio_signal->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;

    if(input->link != NULL){
      GError *error;

      error = NULL;

      ags_channel_set_link(input, NULL,
			   &error);

      g_object_set(G_OBJECT(input),
		   "file-link", g_object_new(AGS_TYPE_FILE_LINK,
					     "filename\0", filename,
					     NULL),
		   NULL);

      if(error != NULL){
	g_warning(error->message);
      }
    }else{
      g_object_set(G_OBJECT(input),
		   "file-link", g_object_new(AGS_TYPE_FILE_LINK,
					     "filename\0", filename,
					     NULL),
		   NULL);
    }

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
					0, AGS_AUDIO(input->audio)->audio_channels);
	data = child->content;

	ags_audio_file_open_from_data(audio_file, data);
	ags_audio_file_read_audio_signal(audio_file);

	audio_signal = audio_file->audio_signal;
	ags_recycling_add_audio_signal(input->first_recycling,
				       AGS_AUDIO_SIGNAL(audio_signal->data));
      }
    }
  }
}

xmlNode*
ags_file_write_file_link(AgsFile *file, xmlNode *parent, AgsFileLink *file_link)
{
  xmlNode *node;
  gchar *id;
  gchar *filename;

  if(file_link == NULL){
    return;
  }
  
  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-file-link\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", file_link,
				   NULL));

  xmlNewProp(node,
	     "type\0",
	     g_strdup("url\0"));

  if(file_link->filename != NULL){
    g_message("XML outputting: %s\0", file_link->filename);
    xmlNewProp(node,
	       "filename\0",
	       g_strdup(file_link->filename));
  }else{
    xmlNewProp(node,
	       "filename\0",
	       g_strdup("(null)\0"));
  }

  xmlNewProp(node,
	     "delay\0",
	     g_strdup_printf("%d\0", 0));

  if((AGS_FILE_WRITE_EMBEDDED_AUDIO & (file->flags)) != 0){
    //TODO:JK: implement me
  }

  xmlAddChild(parent,
	      node);
}

void
ags_file_read_file_link_list(AgsFile *file, xmlNode *node, GList **file_link)
{
  AgsFileLink *current;
  GList *list;
  xmlNode *child;
  xmlChar *id;

  id = xmlGetProp(node, AGS_FILE_ID_PROP);

  child = node->children;
  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-file-link\0",
		     11)){
	current = NULL;
	ags_file_read_file_link(file, child, &current);
	list = g_list_prepend(list, current);
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *file_link = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_file_link_list(AgsFile *file, xmlNode *parent, GList *file_link)
{
  AgsFileLink *current;
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-file-link-list\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = file_link;

  while(list != NULL){
    ags_file_write_file_link(file, node, AGS_FILE_LINK(list->data));

    list = list->next;
  }

  return(node);
}
