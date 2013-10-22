/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/file/ags_file_sound.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>

#include <ags/util/ags_id_generator.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

void ags_file_read_channel_resolve_link(AgsFileLookup *file_lookup,
					AgsChannel *channel);
void ags_file_write_channel_resolve_link(AgsFileLookup *file_lookup,
					 AgsChannel *channel);

void
ags_file_read_devout(AgsFile *file, xmlNode *node, AgsDevout **devout)
{
  AgsDevout *gobject;
  AgsFileLookup *file_lookup;
  xmlChar *prop;

  if(*devout == NULL){
    gobject = g_object_new(AGS_TYPE_DEVOUT,
			   NULL);
    *devout = gobject;
  }else{
    gobject = *devout;
  }

  g_object_set(G_OBJECT(gobject),
	       "main\0", file->main,
	       NULL);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  gobject->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP),
					    NULL,
					    10);

  gobject->dsp_channels = (guint) g_ascii_strtoull(xmlGetProp(node, "dsp-channels\0"),
						   NULL,
						   10);
  gobject->pcm_channels = (guint) g_ascii_strtoull(xmlGetProp(node, "pcm-channels\0"),
						   NULL,
						   10);

  gobject->bits = (guint) g_ascii_strtoull(xmlGetProp(node, "bits\0"),
					   NULL,
					   10);
  gobject->buffer_size = (guint) g_ascii_strtoull(xmlGetProp(node, "buffer-size\0"),
						  NULL,
						  10);
  gobject->frequency = (guint) g_ascii_strtoull(xmlGetProp(node, "frequency\0"),
						NULL,
						10);

  gobject->bpm = (gdouble) g_ascii_strtod(xmlGetProp(node, "bpm\0"),
					  NULL);
  gobject->delay = (guint) g_ascii_strtoull(xmlGetProp(node, "delay\0"),
					    NULL,
					    10);
  gobject->delay_counter = (guint) g_ascii_strtoull(xmlGetProp(node, "delay-counter\0"),
						    NULL,
						    10);

  gobject->attack->first_start = (guint) g_ascii_strtoull(xmlGetProp(node, "attack-delay\0"),
							  NULL,
							  10);
  gobject->attack->first_length = (guint) g_ascii_strtoull(xmlGetProp(node, "attack-length\0"),
						    NULL,
						    10);

  //TODO:JK: calculate gobject->attack->second_start and gobject->attack->second_length

  if((AGS_DEVOUT_LIBAO & (gobject->flags)) != 0){
    //TODO:JK: implement me
  }else if((AGS_DEVOUT_OSS & (gobject->flags)) != 0){
    gobject->out.oss.device = xmlGetProp(node, "device\0");
  }else if((AGS_DEVOUT_ALSA & (gobject->flags)) != 0){
    gobject->out.alsa.device = xmlGetProp(node, "device\0");
  }
}

xmlNode*
ags_file_write_devout(AgsFile *file, xmlNode *parent, AgsDevout *devout)
{
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", devout,
				   NULL));
  
  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-devout\0");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", devout->flags));

  xmlNewProp(node,
	     "dsp-channels\0",
	     g_strdup_printf("%d\0", devout->dsp_channels));
  xmlNewProp(node,
	     "pcm-channels\0",
	     g_strdup_printf("%d\0", devout->pcm_channels));

  xmlNewProp(node,
	     "bits\0",
	     g_strdup_printf("%d\0", devout->bits));
  xmlNewProp(node,
	     "buffer-size\0",
	     g_strdup_printf("%d\0", devout->buffer_size));
  xmlNewProp(node,
	     "frequency\0",
	     g_strdup_printf("%d\0", devout->frequency));

  xmlNewProp(node,
	     "bpm\0",
	     g_strdup_printf("%Lf\0", devout->bpm));
  xmlNewProp(node,
	     "delay\0",
	     g_strdup_printf("%d\0", devout->delay));
  xmlNewProp(node,
	     "delay-counter\0",
	     g_strdup_printf("%Lf\0", devout->delay_counter));

  xmlNewProp(node,
	     "attack\0",
	     g_strdup_printf("%d\0", devout->attack->first_start));
  xmlNewProp(node,
	     "attack-length\0",
	     g_strdup_printf("%d\0", devout->attack->first_length));

  xmlNewProp(node,
	     "device\0",
	     g_strdup(devout->out.alsa.device));
  
  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_file_read_devout_list(AgsFile *file, xmlNode *node, GList **devout)
{
  AgsDevout *current;
  GList *list;
  xmlNode *child;
  xmlChar *id;

  id = xmlGetProp(node, AGS_FILE_ID_PROP);

  child = node->children;
  list = NULL;

  while(child != NULL){
    current = NULL;
    ags_file_read_devout(file, child, &current);

    list = g_list_prepend(list, current);

    child = child->next;
  }

  list = g_list_reverse(list);
  *devout = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_devout_list(AgsFile *file, xmlNode *parent, GList *devout)
{
  AgsDevout *current;
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));

  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-devout-list\0");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlAddChild(parent,
	      node);

  list = devout;

  while(list != NULL){
    ags_file_write_devout(file, node, AGS_DEVOUT(list->data));

    list = list->next;
  }

  return(node);
}

void
ags_file_read_devout_play(AgsFile *file, xmlNode *node, AgsDevoutPlay **play)
{
  AgsDevoutPlay *pointer;
  gchar *id;
  
  if(*play == NULL){
    pointer = ags_devout_play_alloc();
    *play = pointer;
  }else{
    pointer = *play;
  }

  id = xmlGetProp(node,
		  AGS_FILE_ID_PROP);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", pointer,
				   NULL));

  pointer->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP),
					    NULL,
					    10);

  pointer->audio_channel = (guint) g_ascii_strtoull(xmlGetProp(node, "audio-channel\0"),
						    NULL,
						    10);

  // read by parent call: play->source
}

xmlNode*
ags_file_write_devout_play(AgsFile *file, xmlNode *parent, AgsDevoutPlay *play)
{
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-devout-play\0");

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", play,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", play->flags));

  xmlNewProp(node,
	     "audio-channel\0",
	     g_strdup_printf("%d\0", play->audio_channel));

  // write by parent call: play->source

  return(node);
}

void
ags_file_read_devout_play_list(AgsFile *file, xmlNode *node, GList **play)
{
  GList *list;
  AgsDevoutPlay *current;
  xmlNode *child;
  gchar *id;

  id = xmlGetProp(node,
		  AGS_FILE_ID_PROP);

  child = node->children;
  list = NULL;

  while(child != NULL){
    current = NULL;
    ags_file_read_devout_play(file, child, &current);

    list = g_list_prepend(list, current);

    child = child->next;
  }

  list = g_list_reverse(list);
  *play = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_devout_play_list(AgsFile *file, xmlNode *parent, GList *play)
{
  AgsDevout *current;
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-devout-play-list\0");

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", play,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlAddChild(parent,
	      node);

  list = play;

  while(list != NULL){
    ags_file_write_devout_play(file, node, AGS_DEVOUT_PLAY(list->data));

    list = list->next;
  }

  return(node);
}

void
ags_file_read_audio(AgsFile *file, xmlNode *node, AgsAudio **audio)
{
  AgsAudio *gobject;
  xmlNode *child;

  if(*audio == NULL){
    gobject = (AgsAudio *) g_object_new(AGS_TYPE_AUDIO,
					NULL);
    *audio = gobject;
  }else{
    gobject = *audio;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  gobject->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP),
					    NULL,
					    10);

  gobject->sequence_length = (guint) g_ascii_strtoull(xmlGetProp(node, "sequence-length\0"),
						      NULL,
						      10);

  gobject->audio_channels = (guint) g_ascii_strtoull(xmlGetProp(node, "audio-channels\0"),
						     NULL,
						     10);

  ags_audio_set_pads(gobject,
		     AGS_TYPE_OUTPUT,
		     (guint) g_ascii_strtoull(xmlGetProp(node, "output-pads\0"),
					      NULL,
					      10));

  ags_audio_set_pads(gobject,
		     AGS_TYPE_INPUT,
		     (guint) g_ascii_strtoull(xmlGetProp(node, "input-pads\0"),
					      NULL,
					      10));

  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-channel-list\0",
		     16)){
	xmlXPathContext *xpath_context;
	xmlXPathObject *xpath_object;

	xpath_context = xmlXPathNewContext(file->doc);

	if(AGS_FILE_DEFAULT_NS != NULL){
	  xmlXPathRegisterNs(xpath_context, AGS_FILE_DEFAULT_PREFIX, AGS_FILE_DEFAULT_NS);
	}

	xpath_object = xmlXPathNodeEval(child,
					"./ags-channel/ags-output\0",
					xpath_context);

	if(xpath_object->nodesetval != NULL){
	  AgsChannel *channel;
	  xmlNode *channel_node;
	  
	  channel = gobject->output;
	  channel_node = child->children;

	  while(channel != NULL){
	    /* ags-channel output */
	    ags_file_read_channel(file,
				  channel_node,
				  &channel);

	    channel = channel->next;
	    channel_node = channel_node->next;
	  }
	}else{
	  AgsChannel *channel;
	  xmlNode *channel_node;
	  
	  channel = gobject->input;
	  channel_node = child->children;

	  while(channel != NULL){
	    /* ags-channel output */
	    ags_file_read_channel(file,
				  channel_node,
				  &channel);

	    channel = channel->next;
	    channel_node = channel_node->next;
	  }
	}
      }else if(!xmlStrncmp(child->name,
			   "ags-recall-list\0",
			   15)){
	if(!xmlStrncmp(xmlGetProp(child, "is-play\0"),
		       "true\0",
		       5)){
	  /* ags-recall-list play */
	  ags_file_read_recall_list(file,
				    child,
				    &(gobject->play));
	}else{
	  /* ags-recall-list recall */
	  ags_file_read_recall_list(file,
				    child,
				    &(gobject->recall));
	}
      }else if(!xmlStrncmp(child->name,
			   "ags-notation-list\0",
			   17)){
	/* ags-notation-list */
	ags_file_read_notation_list(file,
				    child,
				    &(gobject->notation));
      }else if(!xmlStrncmp(child->name,
			   "ags-devout-play\0",
			   15)){
	/* ags-devout-play */
	ags_file_read_devout_play(file,
				  node,
				  (AgsDevoutPlay **) &gobject->devout_play);
	AGS_DEVOUT_PLAY(gobject->devout_play)->source = (GObject *) gobject;
      }
    }
  }
}

xmlNode*
ags_file_write_audio(AgsFile *file, xmlNode *parent, AgsAudio *audio)
{
  AgsChannel *channel;
  xmlNode *node, *child;
  gchar *id;

  id = ags_id_generator_create_uuid();

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", audio,
				   NULL));

  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-audio\0");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", audio->flags));

  xmlNewProp(node,
	     "sequence-length\0",
	     g_strdup_printf("%d\0", audio->sequence_length));

  xmlNewProp(node,
	     "audio-channels\0",
	     g_strdup_printf("%d\0", audio->audio_channels));

  xmlNewProp(node,
	     "output-pads\0",
	     g_strdup_printf("%d\0", audio->output_pads));

  xmlNewProp(node,
	     "input-pads\0",
	     g_strdup_printf("%d\0", audio->input_pads));

  xmlAddChild(parent,
	      node);

  /* ags-channel-list output */
  channel = audio->output;

  child = xmlNewNode(AGS_FILE_DEFAULT_NS,
		     "ags-channel-list\0");
  xmlAddChild(node,
	      child);

  while(channel != NULL){
    ags_file_write_channel(file,
			   child,
			   channel);

    channel = channel->next;
  }

  /* ags-channel-list input */
  channel = audio->input;

  child = xmlNewNode(AGS_FILE_DEFAULT_NS,
		     "ags-channel-list\0");
  xmlAddChild(node,
	      child);

  while(channel != NULL){
    ags_file_write_channel(file,
			   child,
			   channel);

    channel = channel->next;
  }

  /* ags-recall-list play */
  ags_file_write_recall_list(file,
			     node,
			     audio->play);

  /* ags-recall-list recall */
  ags_file_write_recall_list(file,
			     node,
			     audio->recall);

  /* ags-notation-list */
  ags_file_write_notation_list(file,
			       node,
			       audio->notation);

  /* ags-devout-play */
  ags_file_write_devout_play(file,
			     node,
			     audio->devout_play);

  return(node);
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
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", list,
				   NULL));
  
  while(child != NULL){
    current = NULL;
    ags_file_read_audio(file,
			child,
			&current);

    list = g_list_prepend(list,
			  current);

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

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));
  
  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-audio-list\0");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

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
    
    if(AGS_FILE_DEFAULT_NS != NULL){
      xmlXPathRegisterNs(xpath_context, AGS_FILE_DEFAULT_PREFIX, AGS_FILE_DEFAULT_NS);
    }
    
    xpath_object = xmlXPathNodeEval(node,
				    "./ags-output\0",
				    xpath_context);


    if(xpath_object->nodesetval != NULL){
      gobject = (AgsChannel *) g_object_new(AGS_TYPE_CHANNEL,
					    NULL);

      is_output = TRUE;
    }else{
      gobject = (AgsChannel *) g_object_new(AGS_TYPE_CHANNEL,
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
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));
  
  gobject->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP),
					    NULL,
					    10);

  /* well known properties */
  pad = (guint) g_ascii_strtoull(xmlGetProp(node, "pad\0"),
				 NULL,
				 10);
  audio_channel = (guint) g_ascii_strtoull(xmlGetProp(node, "audio-channel\0"),
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
					       "reference\0", gobject,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_read_channel_resolve_link), gobject);

  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-recycling\0",
		     13)){
	/* ags-recycling */
	ags_file_read_recycling(file,
				child,
				&(gobject->first_recycling));
      }else if(!xmlStrncmp(child->name,
			   "ags-recall-list\0",
			   15)){
	if(!xmlStrncmp(xmlGetProp(child, "is-play\0"),
		       "true\0",
		       5)){
	  /* ags-recall-list play */
	  ags_file_read_recall_list(file,
				    child,
				    &(gobject->play));
	}else{
	  /* ags-recall-list recall */
	  ags_file_read_recall_list(file,
				    child,
				    &(gobject->recall));
	}
      }else if(!xmlStrncmp(child->name,
			   "ags-pattern-list\0",
			   17)){
	/* ags-notation-list */
	ags_file_read_pattern_list(file,
				    child,
				    &(gobject->pattern));
      }else if(!xmlStrncmp(child->name,
			   "ags-output\0",
			   10)){
	/* ags-output */
	ags_file_read_output(file,
			     child,
			     gobject);
      }else if(!xmlStrncmp(child->name,
			   "ags-input\0",
			   9)){
	/* ags-input */
	ags_file_read_input(file,
			    child,
			    gobject);
      }else if(!xmlStrncmp(child->name,
			   "ags-devout-play\0",
			   15)){
	/* ags-devout-play */
	ags_file_read_devout_play(file,
				  child,
				  (AgsDevoutPlay **) &gobject->devout_play);
	AGS_DEVOUT_PLAY(gobject->devout_play)->source = (GObject *) gobject;
      }
    }
  }
}

void
ags_file_read_channel_resolve_link(AgsFileLookup *file_lookup,
				   AgsChannel *channel)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) xmlGetProp(file_lookup->node,
			       "link\0");

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file, xpath);

  channel->link = (AgsChannel *) id_ref->ref;
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

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", channel,
				   NULL));
  
  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-channel\0");

  /* well known properties */
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", channel->flags));

  xmlNewProp(node,
	     "pad\0",
	     g_strdup_printf("%d\0", channel->pad));
  xmlNewProp(node,
	     "audio_channel0",
	     g_strdup_printf("%d\0", channel->audio_channel));


  /* link */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", channel,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_write_channel_resolve_link), channel);

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

  /* ags-recall-list play */
  ags_file_write_recall_list(file,
			     node,
			     channel->play);

  /* ags-recall-list recall */
  ags_file_write_recall_list(file,
			     node,
			     channel->recall);

  /* ags-pattern-list */
  if(channel->pattern != NULL){
    ags_file_write_pattern_list(file,
				node,
				channel->recall);
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

  /* ags-devout-play */
  child = ags_file_write_devout_play(file,
				     node,
				     channel->devout_play);

  return(node);
}

void
ags_file_write_channel_resolve_link(AgsFileLookup *file_lookup,
				    AgsChannel *channel)
{
  AgsFileIdRef *id_ref;
  gchar *id;

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, channel->link);

  id = xmlGetProp(id_ref->node, AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     "link\0",
	     g_strdup_printf("xpath=*/[@id='%s']\0", id));
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
    current = NULL;
    ags_file_read_channel(file,
			  child,
			  &current);

    list = g_list_prepend(list,
			  current);

    child = child->next;
  }

  list = g_list_reverse(list);
  *channel = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
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

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", channel,
				   NULL));

  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-channel-list\0");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlAddChild(parent,
	      node);

  list = channel;

  while(list != NULL){
    ags_file_write_channel(file,
			   node,
			   AGS_CHANNEL(list->data));

    list = list->next;
  }
}

void
ags_file_read_input(AgsFile *file, xmlNode *node, AgsChannel *input)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_input(AgsFile *file, xmlNode *parent, AgsChannel *input)
{
}

void
ags_file_read_output(AgsFile *file, xmlNode *node, AgsChannel *output)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_output(AgsFile *file, xmlNode *parent, AgsChannel *output)
{
}

void
ags_file_read_recall(AgsFile *file, xmlNode *node, AgsRecall **recall)
{
}

xmlNode*
ags_file_write_recall(AgsFile *file, xmlNode *parent, AgsRecall *recall)
{
}

void
ags_file_read_recall_list(AgsFile *file, xmlNode *node, GList **recall)
{
}

xmlNode*
ags_file_write_recall_list(AgsFile *file, xmlNode *parent, GList *recall)
{
}

void
ags_file_read_recall_audio(AgsFile *file, xmlNode *node, AgsRecall **recall)
{
}

xmlNode*
ags_file_write_recall_audio(AgsFile *file, xmlNode *parent, AgsRecall *recall)
{
}

void
ags_file_read_recall_audio_run(AgsFile *file, xmlNode *node, AgsRecall **recall)
{
}

xmlNode*
ags_file_write_recall_audio_run(AgsFile *file, xmlNode *parent, AgsRecall *recall)
{
}

void
ags_file_read_recall_channel(AgsFile *file, xmlNode *node, AgsRecall **recall)
{
}

xmlNode*
ags_file_write_recall_channel(AgsFile *file, xmlNode *parent, AgsRecall *recall)
{
}

void
ags_file_read_recall_channel_run(AgsFile *file, xmlNode *node, AgsRecall **recall)
{
}

xmlNode*
ags_file_write_recall_channel_run(AgsFile *file, xmlNode *parent, AgsRecall *recall)
{
}

void
ags_file_read_recycling(AgsFile *file, xmlNode *node, AgsRecycling **recycling)
{
}

xmlNode*
ags_file_write_recycling(AgsFile *file, xmlNode *parent, AgsRecycling *recycling)
{
}

void
ags_file_read_recycling_list(AgsFile *file, xmlNode *node, GList **recycling)
{
}

xmlNode*
ags_file_write_recycling_list(AgsFile *file, xmlNode *parent, GList *recycling)
{
}

void
ags_file_read_audio_signal(AgsFile *file, xmlNode *node, AgsAudioSignal **audio_signal)
{
}

xmlNode*
ags_file_write_audio_signal(AgsFile *file, xmlNode *parent, AgsAudioSignal *audio_signal)
{
}

void
ags_file_read_audio_signal_list(AgsFile *file, xmlNode *node, GList **audio_signal)
{
}

xmlNode*
ags_file_write_audio_signal_list(AgsFile *file, xmlNode *parent, GList *audio_signal)
{
}

void
ags_file_read_stream(AgsFile *file, xmlNode *node, GList **stream)
{
}

xmlNode*
ags_file_write_stream(AgsFile *file, xmlNode *parent, GList *stream)
{
}

void
ags_file_read_stream_list(AgsFile *file, xmlNode *node, GList **stream)
{
}

xmlNode*
ags_file_write_stream_list(AgsFile *file, xmlNode *parent, GList *stream)
{
}

void
ags_file_read_pattern(AgsFile *file, xmlNode *node, AgsPattern **pattern)
{
}

xmlNode*
ags_file_write_pattern(AgsFile *file, xmlNode *parent, AgsPattern *pattern)
{
}

void
ags_file_read_pattern_list(AgsFile *file, xmlNode *node, GList **pattern)
{
}

xmlNode*
ags_file_write_pattern_list(AgsFile *file, xmlNode *parent, GList *pattern)
{
}

void
ags_file_read_pattern_data(AgsFile *file, xmlNode *node, AgsPattern *pattern, guint i, guint j)
{
}

xmlNode*
ags_file_write_pattern_data(AgsFile *file, xmlNode *parent, AgsPattern *pattern, guint i, guint j)
{
  xmlNode *node;
  GString *content;
  gchar *id;
  guint k;

  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-pattern-data\0");

  id = ags_id_generator_create_uuid();

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/ags-pattern-data[@id='%s']\0", id),
				   "reference\0", pattern,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     "0\0");

  xmlNewProp(node,
	     "index-1st-level\0",
	     g_strdup_printf("%d\0", i));

  xmlNewProp(node,
	     "index-2nd-level\0",
	     g_strdup_printf("%d\0", j));

  xmlNewProp(node,
	     "coding\0",
	     "human readable\0");

  content = g_string_sized_new(pattern->dim[2] + 1);

  for(k = 0; k < pattern->dim[2]; k++){
    g_string_insert_c(content, k, (gchar) (ags_pattern_get_bit(pattern, i, j, k) ? '1': '0'));
  }

  g_string_insert_c(content, k, (gchar) '\0');
  xmlNodeAddContent(node, BAD_CAST (content->str));

  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_file_read_pattern_data_list(AgsFile *file, xmlNode *node, AgsPattern *pattern)
{
}

xmlNode*
ags_file_write_pattern_data_list(AgsFile *file, xmlNode *parent, AgsPattern *pattern)
{
}

void
ags_file_read_notation(AgsFile *file, xmlNode *node, AgsNotation **notation)
{
}

xmlNode*
ags_file_write_notation(AgsFile *file, xmlNode *parent, AgsNotation *notation)
{
}

void
ags_file_read_notation_list(AgsFile *file, xmlNode *node, GList **notation)
{
}

xmlNode*
ags_file_write_notation_list(AgsFile *file, xmlNode *parent, GList *notation)
{
}

void
ags_file_read_note(AgsFile *file, xmlNode *node, AgsNote **note)
{
}

xmlNode*
ags_file_write_note(AgsFile *file, xmlNode *parent, AgsNote *note)
{
}

void
ags_file_read_note_list(AgsFile *file, xmlNode *node, GList **note)
{
}

xmlNode*
ags_file_write_note_list(AgsFile *file, xmlNode *parent, GList *note)
{
}

void
ags_file_read_task(AgsFile *file, xmlNode *node, AgsTask **task)
{
}

xmlNode*
ags_file_write_task(AgsFile *file, xmlNode *parent, AgsTask *task)
{
}

void
ags_file_read_task_list(AgsFile *file, xmlNode *node, GList **task)
{
}

xmlNode*
ags_file_write_task_list(AgsFile *file, xmlNode *parent, GList *task)
{
}
