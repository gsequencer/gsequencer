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

void
ags_file_read_devout(AgsFile *file, xmlNode *node, AgsDevout **devout)
{
  AgsDevout *gobject;
  xmlChar *prop;

  gobject = g_object_new(AGS_TYPE_DEVOUT,
			NULL);
  *devout = gobject;
  g_object_set(G_OBJECT(gobject),
	       "main\0", file->main,
	       NULL);

  ags_file_add_id_ref(file,
		      (AgsFileIdRef *) g_object_new(AGS_TYPE_FILE_ID_REF,
						    "main\0", file->main,
						    "xpath\0", g_strdup_printf("xpath=*/[@id=%s]\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
						    "reference\0", gobject,
						    NULL));

  gobject->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP));

  gobject->dsp_channels = (guint) g_ascii_strtoull(xmlGetProp(node, "dsp-channels\0"));
  gobject->pcm_channels = (guint) g_ascii_strtoull(xmlGetProp(node, "pcm-channels\0"));

  gobject->bits = (guint) g_ascii_strtoull(xmlGetProp(node, "bits\0"));
  gobject->buffer_size = (guint) g_ascii_strtoull(xmlGetProp(node, "buffer-size\0"));
  gobject->frequency = (guint) g_ascii_strtoull(xmlGetProp(node, "frequency\0"));

  gobject->bpm = (gdouble) g_ascii_strtod(xmlGetProp(node, "bpm\0"));
  gobject->delay = (guint) g_ascii_strtoull(xmlGetProp(node, "delay\0"));
  gobject->delay_counter = (guint) g_ascii_strtoull(xmlGetProp(node, "delay-counter\0"));

  gobject->attack_delay = (guint) g_ascii_strtoull(xmlGetProp(node, "attack-delay\0"));
  gobject->attack_length = (guint) g_ascii_strtoull(xmlGetProp(node, "attack-length\0"));

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
		      (AgsFileIdRef *) g_object_new(AGS_TYPE_FILE_ID_REF,
						    "main\0", file->main,
						    "xpath\0", g_strdup_printf("xpath=*/[@id=%s]\0", id),
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
  xmlNode *child;
  xmlChar *id;

  *devout = NULL;

  id = xmlGetProp(node, AGS_FILE_ID_PROP);

  ags_file_add_id_ref(file,
		      (AgsFileIdRef *) g_object_new(AGS_TYPE_FILE_ID_REF,
						    "main\0", file->main,
						    "xpath\0", g_strdup_printf("xpath=*/[@id=%s]\0", id),
						    "reference\0", *devout,
						    NULL));

  child = node->children;

  while(child != NULL){
    current = NULL;
    ags_file_read_devout(child, &current);

    *devout = g_list_prepend(*devout, current);

    child = child->next;
  }

  *devout = g_list_reverse(*devout);

  ags_file_id_ref_alloc(id, *devout);
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
		      (AgsFileIdRef *) g_object_new(AGS_TYPE_FILE_ID_REF,
						    "main\0", file->main,
						    "xpath\0", g_strdup_printf("xpath=*/[@id=%s]\0", id),
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
    ags_file_write_devout(node, AGS_DEVOUT(list->data));

    list = list->next;
  }

  return(node);
}

void
ags_file_read_devout_play(AgsFile *file, xmlNode *node, AgsDevoutPlay **play)
{
  AgsDevoutPlay *pointer;
  gchar *id;
  
  pointer = ags_devout_play_alloc();
  *play = pointer;

  id = xmlGetProp(node,
		  AGS_FILE_ID_PROP);

  ags_file_add_id_ref(file,
		      (AgsFileIdRef *) g_object_new(AGS_TYPE_FILE_ID_REF,
						    "main\0", file->main,
						    "xpath\0", g_strdup_printf("xpath=*/[@id=%s]\0", id),
						    "reference\0", pointer,
						    NULL));

  pointer->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP));

  pointer->audio_channel = (guint) g_ascii_strtoull(xmlGetProp(node, "audio-channel\0"));

  // read by parent call: play->source
}

xmlNode*
ags_file_write_devout_play(AgsFile *file, xmlNode *parent, AgsDevoutPlay *play)
{
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-devout-play\0");

  ags_file_add_id_ref(file,
		      (AgsFileIdRef *) g_object_new(AGS_TYPE_FILE_ID_REF,
						    "main\0", file->main,
						    "xpath\0", g_strdup_printf("xpath=*/[@id=%s]\0", id),
						    "reference\0", play,
						    NULL));

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     play->flags);

  // write by parent call: play->source

  xmlNewProp(node,
	     "audio-channel\0",
	     play->audio_channel);


  return(node);
}

void
ags_file_read_devout_play_list(AgsFile *file, xmlNode *node, GList **play)
{
  GList *list;
  AgsDevoutPlay *current;
  xmlNode *child;
  gchar *id;

  list = NULL;

  id = xmlGetProp(node,
		  AGS_FILE_ID_PROP);

  child = node->children;

  while(child != NULL){
    current = NULL;
    ags_file_read_devout_play(child, &current);

    list = g_list_prepend(list, current);

    child = child->next;
  }

  list = g_list_reverse(list);
  *play = list;

  ags_file_add_id_ref(file,
		      (AgsFileIdRef *) g_object_new(AGS_TYPE_FILE_ID_REF,
						    "main\0", file->main,
						    "xpath\0", g_strdup_printf("xpath=*/[@id=%s]\0", id),
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
		      (AgsFileIdRef *) g_object_new(AGS_TYPE_FILE_ID_REF,
						    "main\0", file->main,
						    "xpath\0", g_strdup_printf("xpath=*/[@id=%s]\0", id),
						    "reference\0", list,
						    NULL));

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlAddChild(parent,
	      node);

  list = play;

  while(list != NULL){
    ags_file_write_devout_play(node, AGS_DEVOUT_PLAY(list->data));

    list = list->next;
  }

  return(node);
}

void
ags_file_read_audio(AgsFile *file, xmlNode *node, AgsAudio **audio)
{
  GObject *gobject;

  gobject = (AgsAudio *) g_object_new(AGS_TYPE_AUDIO,
				     NULL);
  *audio = gobject;

  gobject->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP));

  gobject->sequence_length = (guint) g_ascii_strtoull(xmlGetProp(node, "sequence-length\0"));

  gobject->audio_channels = (guint) g_ascii_strtoull(xmlGetProp(node, "audio-channels\0"));

  gobject->output_pads = (guint) g_ascii_strtoull(xmlGetProp(node, "output-pads\0"));
  gobject->input_pads = (guint) g_ascii_strtoull(xmlGetProp(node, "input-pads\0"));

  ags_file_write_read_devout_play(node,
				  &(audio->devout_play));
  audio->devout_play->source = (GObject *) audio;
}

xmlNode*
ags_file_write_audio(AgsFile *file, xmlNode *parent, AgsAudio *audio)
{
  xmlNode *node;

  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-audio\0");

  if((id = g_object_get_data(G_OBJECT(channel), AGS_FILE_ID_REF_SERIALIZE_ID)) == NULL){
    id = ags_id_generator_create_uuid();
    g_object_set_data(G_OBJECT(channel),
		      AGS_FILE_ID_REF_SERIALIZE_ID,
		      id);
  }

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     ags_id_generator_create_uuid());

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

  ags_file_write_devout_play(node,
			     audio->devout_play);

  return(node);
}

void
ags_file_read_audio_list(AgsFile *file, xmlNode *node, GList **audio)
{
  AgsAudio *current;
  xmlNode *child;

  *audio = NULL;

  child = node->children;

  while(child != NULL){
    current = NULL;
    ags_file_read_audio(child, &current);

    *audio = g_list_prepend(*audio, current);

    child = child->next;
  }

  *audio = g_list_reverse(*audio);
}

xmlNode*
ags_file_write_audio_list(AgsFile *file, xmlNode *parent, GList *audio)
{
  AgsAudio *current;
  xmlNode *node;
  GList *list;

  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-audio-list\0");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     ags_id_generator_create_uuid());

  xmlAddChild(parent,
	      node);

  list = audio;

  while(list != NULL){
    ags_file_write_audio(node, AGS_AUDIO(list->data));

    list = list->next;
  }

  return(node);
}

void
ags_file_read_channel(AgsFile *file, xmlNode *node, AgsChannel **channel)
{
  GObject *gobject;

  gobject = (AgsAudio *) g_object_new(AGS_TYPE_AUDIO,
				     NULL);
  *channel = gobject;

  gobject->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP));

  gobject->pad = g_ascii_strtoull(xmlGetProp(node, "pad\0"));
  gobject->audio_channel = g_ascii_strtoull(xmlGetProp(node, "audio-channel\0"));

  g_object_set_data(G_OBJECT(gobject),
		    AGS_FILE_ID_REF_SERIALIZE_LINK,
		    g_object_new(AGS_TYPE_FILE_ID_REF,
				 AGS_FILE_XPATH_PROP, g_strdup_printf("xpath://*/@id[%s]\0", xmlGetProp(node, "link\0")),
				 NULL));

  ags_file_read_devout_play(node,
			    &(audio->devout_play));
  audio->devout_play->source = (GObject *) gobject;
}

xmlNode*
ags_file_write_channel(AgsFile *file, xmlNode *parent, AgsChannel *channel)
{
  xmlNode *node, *child;
  gchar *id, *link_id;

  if((id = g_object_get_data(G_OBJECT(channel), AGS_FILE_ID_REF_SERIALIZE_ID)) == NULL){
    id = ags_id_generator_create_uuid();
  }

  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-channel\0");

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


  child = ags_file_write_devout_play(node,
				     audio->devout_play);

  if(channel->link != NULL && (id = g_object_get_data(G_OBJECT(channel->link), AGS_FILE_ID_REF_SERIALIZE_ID)) == NULL){
  }

  xmlNewProp(node,
	     "link\0",
	     link_id);

  xmlNewProp(child,
	     "source\0",
	     id);

  return(node);
}

void
ags_file_read_channel_list(AgsFile *file, xmlNode *node, GList **channel)
{
  AgsChannel *current;
  xmlNode *child;

  *channel = NULL;

  child = node->children;

  while(child != NULL){
    current = NULL;
    ags_file_read_channel(child, &current);

    *channel = g_list_prepend(*channel, current);

    child = child->next;
  }

  *channel = g_list_reverse(*channel);
}

xmlNode*
ags_file_write_channel_list(AgsFile *file, xmlNode *parent, GList *channel)
{
  AgsChannel *current;
  xmlNode *node;
  GList *list;

  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-channel-list\0");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     ags_id_generator_create_uuid());

  xmlAddChild(parent,
	      node);

  list = channel;

  while(list != NULL){
    ags_file_write_channel(node, AGS_CHANNEL(list->data));

    list = list->next;
  }
}

void
ags_file_read_input(AgsFile *file, xmlNode *node, AgsChannel *input)
{
}

xmlNode*
ags_file_write_input(AgsFile *file, xmlNode *parent, AgsChannel *input)
{
}

void
ags_file_read_output(AgsFile *file, xmlNode *node, AgsChannel *output)
{
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
ags_file_read_stream(AgsFile *file, xmlNode *node, AgsStream **stream)
{
}

xmlNode*
ags_file_write_stream(AgsFile *file, xmlNode *parent, AgsStream *stream)
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
