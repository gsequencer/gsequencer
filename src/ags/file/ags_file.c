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

#include <ags/file/ags_file.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

#include <ags/main.h>

#include <ags/file/ags_file_sound.h>
#include <ags/file/ags_file_gui.h>

void ags_file_class_init(AgsFileClass *file);
void ags_file_init (AgsFile *file);
void ags_file_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_file_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);
void ags_file_finalize(GObject *gobject);

void ags_file_real_write(AgsFile *file);
void ags_file_real_write_resolve(AgsFile *file);

void ags_file_real_read(AgsFile *file);
void ags_file_real_read_resolve(AgsFile *file);
void ags_file_real_start(AgsFile *file);

enum{
  WRITE,
  WRITE_RESOLVE,
  READ,
  READ_RESOLVE,
  READ_START,
  LAST_SIGNAL,
};

static gpointer ags_file_parent_class = NULL;
static guint file_signals[LAST_SIGNAL];

GType
ags_file_get_type (void)
{
  static GType ags_type_file = 0;

  if(!ags_type_file){
    static const GTypeInfo ags_file_info = {
      sizeof (AgsFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_file_init,
    };

    ags_type_file = g_type_register_static(G_TYPE_OBJECT,
					   "AgsFile\0",
					   &ags_file_info,
					   0);
  }

  return (ags_type_file);
}

void
ags_file_class_init(AgsFileClass *file)
{
  GObjectClass *gobject;

  ags_file_parent_class = g_type_class_peek_parent(file);

  /* GObjectClass */
  gobject = (GObjectClass *) file;

  gobject->get_property = ags_file_get_property;
  gobject->set_property = ags_file_set_property;

  gobject->finalize = ags_file_finalize;

  /* AgsFileClass */
  file->write = ags_file_real_write;
  file->resolve_write = ags_file_real_write_resolve;
  file->read = ags_file_real_read;
  file->resolve_read = ags_file_real_read_resolve;
  file->start = ags_file_real_read_start;

  file_signals[WRITE] =
    g_signal_new("write\0",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, write),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  file_signals[WRITE_RESOLVE] =
    g_signal_new("write_resolve\0",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, write_resolve),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  file_signals[READ] =
    g_signal_new("read\0",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, read),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  file_signals[READ_RESOLVE] =
    g_signal_new("read_resolve\0",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, read_resolve),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  file_signals[READ_START] =
    g_signal_new("read_start\0",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, read_start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_file_init(AgsFile *file)
{
  file->flags = 0;

  file->name = NULL;

  file->encoding = AGS_FILE_DEFAULT_ENCODING;
  file->dtd = AGS_FILE_DEFAULT_DTD;

  file->doc = NULL;
  file->current = NULL;

  file->id_refs = NULL;
  file->lookup = NULL;

  file->clipboard = NULL;
  file->property = NULL;
  file->script = NULL;
  file->cluster = NULL;
  file->client = NULL;
  file->server = NULL;
  file->main = NULL;
  file->embedded_audio = NULL;
  file->file_link = NULL;
  file->history = NULL;
}

void
ags_file_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  //TODO:JK: implement me
}

void
ags_file_get_property(GObject *gobject,
		      guint prop_id,
		      GValue *value,
		      GParamSpec *param_spec)
{
  //TODO:JK: implement me
}

void
ags_file_finalize(GObject *gobject)
{
  AgsFile *file;
  void ags_file_destroy_node(xmlNodePtr node){
    xmlNodePtr child;

    child = node->children;

    while(child != NULL){
      ags_file_destroy_node(child);
      
      child = child->next;
    }
  }

  file = (AgsFile *) gobject;

  if((AGS_FILE_READ & (file->flags)) != 0){
  }else if((AGS_FILE_WRITE & (file->flags)) != 0){
  }else{
    return;
  }

  xmlFreeDoc(file->doc);
  //  xmlCleanupParser();
  //  xmlMemoryDump();

  G_OBJECT_CLASS(ags_file_parent_class)->finalize(gobject);
}

void
ags_file_add_id_ref(AgsFile *file, GObject *id_ref)
{
  //TODO:JK: implement me
}

GObject*
ags_file_find_id_ref_by_xpath(AgsFile *file, gchar *xpath)
{
  //TODO:JK: implement me
}

GObject*
ags_file_find_id_ref_by_reference(AgsFile *file, gpointer ref)
{
  //TODO:JK: implement me
}

void
ags_file_add_lookup(AgsFile *file, GObject *file_lookup)
{
  //TODO:JK: implement me
}

void
ags_file_real_write(AgsFile *file)
{
  //TODO:JK: implement me
}

void
ags_file_write(AgsFile *file)
{
  g_return_if_fail(AGS_IS_FILE(file));

  g_object_ref(G_OBJECT(file));
  g_signal_emit(G_OBJECT(file),
		file_signals[WRITE], 0);
  g_object_unref(G_OBJECT(file));
}


void
ags_file_real_write_resolve(AgsFile *file)
{
  //TODO:JK: implement me
}

void
ags_file_write_resolve(AgsFile *file)
{
  g_return_if_fail(AGS_IS_FILE(file));

  g_object_ref(G_OBJECT(file));
  g_signal_emit(G_OBJECT(file),
		file_signals[WRITE_RESOLVE], 0);
  g_object_unref(G_OBJECT(file));
}

void
ags_file_real_read(AgsFile *file)
{
  //TODO:JK: implement me
}

void
ags_file_read(AgsFile *file)
{
  g_return_if_fail(AGS_IS_FILE(file));

  g_object_ref(G_OBJECT(file));
  g_signal_emit(G_OBJECT(file),
		file_signals[READ], 0);
  g_object_unref(G_OBJECT(file));
}

void
ags_file_real_read_resolve(AgsFile *file)
{
  //TODO:JK: implement me
}

void
ags_file_read_resolve(AgsFile *file)
{
  g_return_if_fail(AGS_IS_FILE(file));

  g_object_ref(G_OBJECT(file));
  g_signal_emit(G_OBJECT(file),
		file_signals[READ_RESOLVE], 0);
  g_object_unref(G_OBJECT(file));
}

void
ags_file_real_read_start(AgsFile *file)
{
  //TODO:JK: implement me
}

void
ags_file_read_start(AgsFile *file)
{
  g_return_if_fail(AGS_IS_FILE(file));

  g_object_ref(G_OBJECT(file));
  g_signal_emit(G_OBJECT(file),
		file_signals[READ_START], 0);
  g_object_unref(G_OBJECT(file));
}

void
ags_file_read_server(AgsFile *file, xmlNode *node, GObject **server)
{
  //TODO:JK: implement me
}

void
ags_file_write_server(AgsFile *file, xmlNode *parent, GObject *server)
{
  //TODO:JK: implement me
}

void
ags_file_read_main(AgsFile *file, xmlNode *node, GObject **main)
{
  //TODO:JK: implement me
}

void
ags_file_write_main(AgsFile *file, xmlNode *parent, GObject *main)
{
  //TODO:JK: implement me
}

void
ags_file_write(AgsFile *file)
{
  FILE *file_out;
  xmlDtd *dtd;
  xmlChar *buffer;
  int size;

  file->doc = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);
  file->doc->standalone = FALSE;
  dtd = xmlCreateIntSubset(file->doc, BAD_CAST ("ags\0"), NULL, BAD_CAST (file->dtd));

  file->current = xmlNewNode(NULL, BAD_CAST "ags\0");
  //  file->current = xmlNewDocNode(file->doc, NULL, BAD_CAST "ags\0", NULL);
  xmlDocSetRootElement(file->doc, file->current);

  xmlSetProp(file->current, BAD_CAST "version\0", BAD_CAST g_strdup(AGS_VERSION));
  xmlSetProp(file->current, BAD_CAST "embedded_audio\0", BAD_CAST g_strdup(AGS_FILE_FALSE));

  xmlNodeAddContent(file->current, BAD_CAST "\n\0");

  //  dtd = (xmlDtd *) xmlParseDTD(NULL, "~/ags/file/ags_file.dtd\0");
  //  xmlAddChild((xmlNodePtr ) file->doc, (xmlNodePtr ) dtd);

  ags_file_write_window(file);

  //  xmlSaveFormatFileEnc(file->name, file->doc, file->encoding, TRUE);
  xmlDocDumpFormatMemoryEnc(file->doc, &buffer, &size, file->encoding, TRUE);

  file_out = fopen(file->name, "w+\0");
  fwrite(buffer, size, sizeof(xmlChar), file_out);
  fclose(file_out);
}

void
ags_file_read(AgsFile *file)
{
  xmlChar *string;

  //  LIBXML_TEST_VERSION;

  file->doc = xmlReadFile(file->name, NULL, 0);

  if(file->doc == NULL){
    printf("ags_file_read():\n  couldn't parse file: %s\n\0", file->name);
    return;
  }

  file->current = xmlDocGetRootElement(file->doc);

  while(file->current != NULL){
    if(file->current->type == XML_ELEMENT_NODE && !xmlStrncmp("ags\0", file->current->name, 3))
      break;

    file->current = file->current->next;
  }

  if(file->current != NULL)
    string = xmlGetProp(file->current, "version\0");
  else{
    g_printf("ags_file_read():\n  invalid format by opening file: %s\nexiting\n\0", file->name);
    return;
  }

  if(string != NULL){
    if(xmlStrncmp(string, AGS_VERSION, 8))
      g_printf("version doesn't match\nfile version: %s\nprogram version %s\n\0", string, AGS_VERSION);
  }else
    g_printf("ags_file_read():\n  no version attribute in file: %s\n\0", file->name);

  file->current = file->current->children;
  ags_file_read_window(file);
}

void
ags_file_read_audio(AgsFile *file, AgsAudio *audio)
{
  xmlNodePtr node, child;
  xmlChar *prop[2];
  GList *notation;
  gboolean  first_notation;
  int i;

  prop[0] = xmlGetProp(file->current, "audio_channels\0");

  if(prop[0] != NULL)
    audio->audio_channels = atoi(prop[0]);
  //    ags_audio_set_audio_channels(audio, atoi(prop[0]));

  prop[0] = xmlGetProp(file->current, "output_pads\0");

  if(prop[0] != NULL)
    ags_audio_set_pads(audio, AGS_TYPE_OUTPUT, atoi(prop[0]));

  prop[0] = xmlGetProp(file->current, "input_pads\0");
  
  if(prop[0] != NULL)
    ags_audio_set_pads(audio, AGS_TYPE_INPUT, atoi(prop[0]));
  
  node = file->current;
  file->current = node->children;

  first_notation = TRUE;
  i = 0;

  while(file->current != NULL){
    child = file->current;

    if(file->current->type == XML_ELEMENT_NODE)
      if(!xmlStrncmp(file->current->name, g_type_name(AGS_TYPE_CHANNEL), 10)){
	ags_file_read_channel(file, audio);
      }else if(!xmlStrncmp(file->current->name, g_type_name(AGS_TYPE_NOTATION), 11)){
	if(first_notation){
	  notation =
	    audio->notation = g_list_alloc();
	}else{
	  notation->next = g_list_alloc();
	  notation->next->prev = notation;
	  notation = notation->next;
	}

	notation->data = (gpointer) ags_notation_new(i);
	ags_file_read_notation(file, (AgsNotation *) notation->data);

	i++;
      }

    file->current = child->next;
  }
}

void
ags_file_read_channel(AgsFile *file, AgsAudio *audio)
{
  AgsChannel *channel;
  xmlNodePtr node, child;
  xmlChar *prop[2];
  GList *recall, *pattern;
  gboolean io, second_run, recycling, first_recall, first_pattern, is_input, is_output;

  prop[0] = xmlGetProp(file->current->children, "pad\0");
  prop[1] = xmlGetProp(file->current->children, "audio_channel\0");

  if(prop[0] == NULL || prop[1] == NULL)
    return;

  prop[0] = xmlGetProp(file->current->children, "link_name\0");
  prop[1] = xmlGetProp(file->current->children, "link_line\0");

  node = file->current;

  first_recall =
    first_pattern = TRUE;

  io =
    is_input =
    is_output = 
    second_run =
    recycling = FALSE;

 ags_file_read_channel0:
  file->current = node->children;

  if(io){
    g_object_set_data((GObject *) channel, AGS_CHANNEL_LINK_NAME, g_strdup(prop[0]));
    g_object_set_data((GObject *) channel, AGS_CHANNEL_LINK_LINE, g_strdup(prop[1]));
  }

  while(file->current != NULL){
    child = file->current;
    
    if(file->current->type == XML_ELEMENT_NODE){
      if(!io && !xmlStrncmp(file->current->name, g_type_name(AGS_TYPE_INPUT), 8)){
	io = TRUE;
	is_input = TRUE;
	second_run = TRUE;
	channel = ags_channel_nth(audio->input, atoi(prop[0]) * audio->audio_channels + atoi(prop[1]));
	goto ags_file_read_channel0;
      }else if(!io && !xmlStrncmp(file->current->name, g_type_name(AGS_TYPE_OUTPUT), 9)){
	io = TRUE;
	is_output = TRUE;
	second_run = TRUE;
	channel = ags_channel_nth(audio->output, atoi(prop[0]) * audio->audio_channels + atoi(prop[1]));
	goto ags_file_read_channel0;
      }else if(io && !xmlStrncmp(file->current->name, g_type_name(AGS_TYPE_RECALL), 9)){
	if(first_recall){
	  recall =
	    channel->recall = g_list_alloc();
	  first_recall = FALSE;
	}else{
	  recall->next = g_list_alloc();
	  recall->next->prev = recall;
	  recall = recall->next;
	}

	recall->data = (gpointer) g_object_new(AGS_TYPE_RECALL, NULL);
	ags_file_read_recall(file, (AgsRecall *) recall->data);
      }else if(io && !xmlStrncmp(file->current->name, g_type_name(AGS_TYPE_PATTERN), 10)){
	if(first_pattern){
	  pattern =
	    channel->pattern = g_list_alloc();
	  first_pattern = FALSE;
	}else{
	  pattern->next = g_list_alloc();
	  pattern->next->prev = pattern;
	  pattern = pattern->next;
	}
	
	pattern->data = (gpointer) ags_pattern_new();
      	ags_file_read_pattern(file, (AgsPattern *) pattern->data);
      }else if(io && !recycling && !xmlStrncmp(file->current->name, g_type_name(AGS_TYPE_RECYCLING), 12)){
	recycling = TRUE;
      }
    }

    file->current = child->next;
  }

  if(!second_run){
    io = TRUE;
    file->current = node;
    goto ags_file_read_channel0;
  }

  if(is_input)
    ags_file_read_input(file, (AgsInput *) channel);
  else if(is_output)
    ags_file_read_output(file, (AgsOutput *) channel);
}

void
ags_file_read_channel_link(AgsFile *file)
{
  AgsWindow *window;
  AgsMachine *machine, *machine_link;
  AgsAudio *audio;
  AgsChannel *channel, *channel_link;
  GList *list_start, *list;
  guint nth;
  GError *error;

  list_start =
    list = gtk_container_get_children((GtkContainer *) window->machines);

  error = NULL;

  while(list != NULL){
    machine = AGS_MACHINE(list->data);

    channel = machine->audio->output;

    while(channel != NULL){
      if(channel->link == NULL){
	machine_link = ags_machine_find_by_name(list_start, g_object_get_data((GObject *) channel, AGS_CHANNEL_LINK_NAME));

	if(machine_link != NULL){
	  nth = atoi(g_object_get_data((GObject *) channel, AGS_CHANNEL_LINK_LINE));
	  channel_link = ags_channel_nth(machine_link->audio->input, nth);
	  ags_channel_set_link(channel, channel_link, &error);
	}
      }

      channel = channel->next;
    }

    channel = machine->audio->input;

    while(channel != NULL){
      if(channel->link == NULL){
	machine_link = ags_machine_find_by_name(list_start, g_object_get_data((GObject *) channel, AGS_CHANNEL_LINK_NAME));

	if(machine_link != NULL){
	  nth = atoi(g_object_get_data((GObject *) channel, AGS_CHANNEL_LINK_LINE));
	  channel_link = ags_channel_nth(machine_link->audio->output, nth);
	  ags_channel_set_link(channel, channel_link, &error);
	}
      }

      channel = channel->next;
    }

    list = list->next;
  }
}

void
ags_file_read_output(AgsFile *file, AgsOutput *output)
{
}

void
ags_file_read_input(AgsFile *file, AgsInput *input)
{
  xmlChar *prop;

  prop = xmlGetProp(file->current, "file\0");
  //  ags_input_set_file(input, g_strdup(prop));
}

void
ags_file_read_recycling(AgsFile *file, AgsRecycling *recycling)
{
}

void
ags_file_read_audio_signal(AgsFile *file, AgsAudioSignal *audio_signal)
{
  xmlNodePtr node, child;
  xmlChar *prop;
  GList *list;
  gboolean first_offset, stream_init, lock_init;
   
  node =
    file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_AUDIO_SIGNAL), NULL);
  
  /* */
  node = file->current;
  file->current = node->children;

  first_offset =
    stream_init =
    lock_init = FALSE;

  while(file->current != NULL && (!stream_init || !lock_init)){
    child = file->current;

    if(file->current->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(file->current->name, "AgsStream\0", 9)){
	if(!stream_init){
	  list =
	    audio_signal->stream_beginning = g_list_alloc();

	  stream_init = TRUE;
	}else{
	  list->next = g_list_alloc();
	  list->next->prev = list;
	  list = list->next;
	}

	list->data = (gpointer) malloc(AGS_DEVOUT(audio_signal->devout)->buffer_size * sizeof(guint));
	ags_file_read_stream(file, (guint *) list->data, AGS_DEVOUT(audio_signal->devout)->buffer_size);
      }
    }

    file->current = child->next;
  }
}

void
ags_file_read_stream(AgsFile *file, guint *stream, guint buffer_size)
{
  xmlChar *content;

  content = xmlNodeGetContent(file->current);
  memcpy(stream, content, buffer_size * (sizeof(guint) / sizeof(xmlChar)));
}

void
ags_file_read_pattern(AgsFile *file, AgsPattern *pattern)
{
  xmlNodePtr node;
  xmlChar *prop[3];
  xmlChar *content;
  gboolean human_readable;
  guint i, j, k;
   
  prop[0] = xmlGetProp(file->current, "dim0\0");
  prop[1] = xmlGetProp(file->current, "dim1\0");
  prop[2] = xmlGetProp(file->current, "lenght\0");

  ags_pattern_set_dim(pattern, atoi(prop[0]), atoi(prop[1]), atoi(prop[2]));

  node = file->current->children;

  while(node != NULL){
    if(node->type == XML_ELEMENT_NODE && !xmlStrncmp(node->name, "AgsPatternData\0", 14)){
      prop[0] = xmlGetProp(node, "coding\0");
      
      if(prop != NULL){
	human_readable = (gboolean) !xmlStrncmp(prop[0], "human readable\0", 14);
      }else
	human_readable = TRUE;

      i = atoi(xmlGetProp(node, "index0\0"));
      j = atoi(xmlGetProp(node, "index1\0"));
       
      content = xmlNodeGetContent(node);

      if(human_readable){
	for(k = 0; k < pattern->dim[2]; k++){
	  if(content[k] == '1'){
	    ags_pattern_toggle_bit(pattern, i, j, k);
	  }
	}
      }else{
      }
    }

    node = node->next;
  }
}

void
ags_file_read_notation(AgsFile *file, AgsNotation *notation)
{
  AgsNote *note;
  xmlNodePtr node, child;
  xmlChar *prop;

  prop = xmlGetProp(file->current, "raster\0");

  if(prop)
    notation->flags |= ((!xmlStrncmp(prop, AGS_FILE_TRUE, 4)) ? AGS_NOTATION_RASTER: 0);

  prop = xmlGetProp(file->current, "tact\0");
  notation->tact = g_strdup(prop);

  prop = xmlGetProp(file->current, "bpm\0");
   
  if(prop)
    notation->bpm = atoi(prop);

  node = file->current;
  file->current = node->children;

  while(node != NULL){
    child = file->current;

    if(node->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name, "AgsNote\0", 7)){
	note = ags_note_new();
	ags_file_read_note(file, note);
	ags_notation_add_note(notation,
			      note,
			      FALSE);
      }
    }

    file->current = child->next;
  }
}

void
ags_file_read_note(AgsFile *file, AgsNote *note)
{
  xmlNodePtr node, child;
  xmlChar *prop;

  prop = xmlGetProp(file->current, "x0\0");
  note->x[0] = atoi(prop);

  prop = xmlGetProp(file->current, "x1\0");
  note->x[1] = atoi(prop);

  prop = xmlGetProp(file->current, "y\0");
  note->y = atoi(prop);
}

void
ags_file_read_devout(AgsFile *file, AgsDevout *devout)
{
  xmlChar *prop;

  prop = xmlGetProp(file->current, "pcm_channels\0");

  if(prop != NULL)
    devout->pcm_channels = atoi(prop);
  else
    devout->pcm_channels = 2;

  prop = xmlGetProp(file->current, "dsp_channels\0");

  if(prop != NULL)
    devout->dsp_channels = atoi(prop);
  else
    devout->dsp_channels = 2;

  prop = xmlGetProp(file->current, "bits\0");

  if(prop != NULL)
    devout->bits = atoi(prop);
  else
    devout->bits = 16;

  prop = xmlGetProp(file->current, "buffer_size\0");

  if(prop != NULL)
    devout->buffer_size = atoi(prop);
  else
    devout->buffer_size = 512;

  prop = xmlGetProp(file->current, "frequency\0");

  if(prop != NULL)
    devout->frequency = atoi(prop);
  else
    devout->frequency = 16;
  /*
  prop = xmlGetProp(file->current, "system\0");

  if(prop != NULL)
    devout->system = g_strdup(prop);
  else
    devout->system = AGS_DEVOUT_OSS;

  prop = xmlGetProp(file->current, "device\0");

  if(prop != NULL)
    devout->device = g_strdup(prop);
  else
    devout->device = "/dev/dsp";
  */
}

void
ags_file_read_recall(AgsFile *file, AgsRecall *recall)
{
  /*
  xmlChar *prop;
  xmlNodePtr node, child;

  recall->effect = xmlGetProp(file->current, "effect\0");
  recall->name = xmlGetProp(file->current, "name\0");

  prop = xmlGetProp(file->current, "control_count\0");

  if(prop != NULL){
    recall->control_count = atoi(prop);
    recall->control = (gpointer *) malloc(recall->control_count * sizeof(gpointer));
  }

  node = file->current;
  file->current = node->children;

  while(file->current != NULL){
    child = file->current;

    if(file->current->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(file->current->name, AGS_COPY_STREAM_TO_STREAM, 25))
	ags_file_read_copy_stream_to_stream(file, recall);
      else if(!xmlStrncmp(file->current->name, AGS_COPY_INPUT_TO_OUTPUT, 24))
	ags_file_read_copy_input_to_output(file, recall);
      else if(!xmlStrncmp(file->current->name, AGS_COPY_PATTERN, 16))
	ags_file_read_copy_pattern(file, recall);
      else if(!xmlStrncmp(file->current->name, AGS_MIX_VOLUME, 14))
	ags_file_read_mix_volume(file, recall);
      
    }

    file->current = child->next;
  }
  */
}

void
ags_file_read_play_pattern(AgsFile *file, AgsRecall *recall)
{
}

void
ags_file_read_play_channel(AgsFile *file, AgsRecall *recall)
{
}

void
ags_file_read_play_audio_signal(AgsFile *file, AgsRecall *recall)
{
}

void
ags_file_read_volume_channel(AgsFile *file, AgsRecall *recall)
{
}

void
ags_file_read_copy_pattern(AgsFile *file, AgsRecall *recall)
{
}

void
ags_file_read_copy_channel(AgsFile *file, AgsRecall *recall)
{
}

void
ags_file_read_copy_audio_signal(AgsFile *file, AgsRecall *recall)
{
}

void
ags_file_read_window(AgsFile *file)
{
  AgsWindow *window;
  xmlNodePtr node, child, child0;
  gboolean menu_bar, machines, editor, navigation;

  menu_bar =
    machines =
    editor =
    navigation = FALSE;

  while(file->current != NULL){
    if(file->current->type == XML_ELEMENT_NODE && !xmlStrncmp(g_type_name(AGS_TYPE_WINDOW), file->current->name, 9))
      break;

    file->current = file->current->next;
  }

  if(file->current == NULL)
    return;

  window = ags_window_new(file->main);
  file->window = (GtkWidget *) window;

  node = file->current;
  file->current = node->children;

  while(file->current != NULL){
    child = file->current;

    if(file->current->type == XML_ELEMENT_NODE){
      if(!menu_bar && !xmlStrncmp(g_type_name(AGS_TYPE_MENU_BAR), file->current->name, 10)){
	menu_bar = TRUE;
	ags_file_read_menu_bar(file, (AgsMenuBar *) window->menu_bar);
      }else if(!machines && !xmlStrncmp("AgsMachines\0", file->current->name, 11)){
	machines = TRUE;
	file->current = file->current->children;

	while(file->current != NULL){
	  child0 = file->current;

	  if(file->current->type == XML_ELEMENT_NODE && !xmlStrncmp(g_type_name(AGS_TYPE_MACHINE), file->current->name, 10))
	    ags_file_read_machine(file);

	  file->current = child0->next;
	}
      }else if(!editor && !xmlStrncmp(g_type_name(AGS_TYPE_EDITOR), file->current->name, 9)){
	editor = TRUE;
	ags_file_read_editor(file, window->editor);
      }else if(!navigation && !xmlStrncmp(g_type_name(AGS_TYPE_NAVIGATION), file->current->name, 13)){
	navigation = TRUE;
	ags_file_read_navigation(file, window->navigation);
      }else
	g_printf("ignoring unrecognized element: %s\n\0", file->current->name);
    }

    file->current = child->next;
  }

  gtk_widget_show_all((GtkWidget *) window);
  ags_window_connect(window);
}

void
ags_file_read_menu_bar(AgsFile *file, AgsMenuBar *menu_bar)
{
}

void
ags_file_read_machine(AgsFile *file)
{
  AgsMachine *machine;
  xmlNodePtr node, child;
  gboolean audio, instance;

  machine = NULL;

  node = file->current;
  file->current = node->children;

  audio =
    instance = FALSE;

 ags_file_read_machine0:

  while(file->current != NULL){
    child = file->current;

    if(file->current->type == XML_ELEMENT_NODE){
      if(instance && !audio && !xmlStrncmp(g_type_name(AGS_TYPE_AUDIO), file->current->name, 8)){
	ags_file_read_audio(file, machine->audio);
	audio = TRUE;
      }else if(!instance){
	if(!xmlStrncmp(g_type_name(AGS_TYPE_PANEL), file->current->name, 8)){
	  machine = (AgsMachine *) g_object_new(AGS_TYPE_PANEL, NULL);
	  ags_file_read_panel(file, machine);
	}else if(!xmlStrncmp(g_type_name(AGS_TYPE_MIXER), file->current->name, 8)){
	  machine = (AgsMachine *) g_object_new(AGS_TYPE_MIXER, NULL);
	  ags_file_read_mixer(file, machine);
	}else if(!xmlStrncmp(g_type_name(AGS_TYPE_DRUM), file->current->name, 7)){
	  machine = (AgsMachine *) g_object_new(AGS_TYPE_DRUM, NULL);
	  ags_file_read_drum(file, machine);
	}else if(!xmlStrncmp(g_type_name(AGS_TYPE_MATRIX), file->current->name, 9)){
	  machine = (AgsMachine *) g_object_new(AGS_TYPE_MATRIX, NULL);
	  ags_file_read_matrix(file, machine);
	}else if(!xmlStrncmp(g_type_name(AGS_TYPE_SYNTH), file->current->name, 8)){
	  machine = (AgsMachine *) g_object_new(AGS_TYPE_SYNTH, NULL);
	  ags_file_read_synth(file, machine);
	}else if(!xmlStrncmp(g_type_name(AGS_TYPE_FFPLAYER), file->current->name, 11)){
	  machine = (AgsMachine *) g_object_new(AGS_TYPE_FFPLAYER, NULL);
	  ags_file_read_ffplayer(file, machine);
	}else{
	  file->current = file->current->next;
	  continue;
	}

	machine->name = xmlGetProp(node, "name\0");
	gtk_box_pack_start((GtkBox *) AGS_WINDOW(file->window)->machines, (GtkWidget *) machine, FALSE, FALSE, 0);

	instance = TRUE;
	file->current = node->children;
	goto ags_file_read_machine0;
      }else if(audio)
	break;
    }

    file->current = child->next;
  }
}

void
ags_file_read_pad(AgsFile *file, AgsPad *pad)
{
  xmlNodePtr node, child;
  GList *list0;
  xmlChar *prop;

  prop = xmlGetProp(node, "show_grouping\0");

  /*
  if(prop != NULL)
    if(!xmlStrncmp(prop, AGS_FILE_TRUE, 4))
      gtk_widget_show((GtkWidget *) pad->hbox);
  */

  prop = xmlGetProp(node, "groupe_pad\0");

  if(prop != NULL){
    if(!xmlStrncmp(prop, AGS_FILE_TRUE, 4)){
    }
  }

  prop = xmlGetProp(node, "groupe_channel\0");

  if(prop != NULL)
    if(!xmlStrncmp(prop, AGS_FILE_TRUE, 4)){
    }

  list0 = gtk_container_get_children((GtkContainer *) pad->option->menu);

  node = file->current;
  file->current = file->current->children;

  while(file->current != NULL){
    if(file->current->type == XML_ELEMENT_NODE && !xmlStrncmp(file->current->name, g_type_name(AGS_TYPE_LINE), 7)){
      ags_file_read_line(file, (AgsLine *) list0->data);
      list0 = list0->next;
    }

    file->current = file->current->next;
  }
}

void
ags_file_read_line(AgsFile *file, AgsLine *line)
{
  xmlNodePtr node, child;
  xmlChar *prop[2];
  guint rows, cols;
  void ags_file_read_line_member(){
    GtkWidget *widget;
    AgsLineMember *line_member;
    xmlChar *prop[5];
    gboolean all_positions = FALSE;
    gboolean connect = TRUE;

    prop[0] = xmlGetProp(file->current, "visualization\0");

    if(prop[0] == NULL){
      g_printf("AgsLineMember has no visualization\n\0");
      return;
    }

    if(!xmlStrncmp(g_type_name(GTK_TYPE_SCALE), prop[0], 8))
      widget = g_object_new(GTK_TYPE_VSCALE, NULL);
    else if(!xmlStrncmp(g_type_name(GTK_TYPE_SPIN_BUTTON), prop[0], 13))
      widget = g_object_new(GTK_TYPE_SPIN_BUTTON, NULL);
    //    else if(!xmlStrncmp(g_type_name(GTK_TYPE_DIAL), prop))
    //      widget = g_object_new(GTK_TYPE_DIAL);
    else
      return;

    prop[0] = xmlGetProp(file->current, "left_attach\0");
    prop[1] = xmlGetProp(file->current, "right_attach\0");
    prop[2] = xmlGetProp(file->current, "top_attach\0");
    prop[3] = xmlGetProp(file->current, "bottom_attach\0");

    if(prop[0] == NULL || prop[1] == NULL || prop[2] == NULL || prop[3] == NULL)
      gtk_container_add((GtkContainer *) line->table, widget);
    else
      gtk_table_attach(line->table, widget,
		       atoi(prop[0]), atoi(prop[1]),
		       atoi(prop[2]), atoi(prop[3]),
		       GTK_FILL, GTK_FILL,
		       2, 2);

    prop[0] = xmlGetProp(file->current, "effect\0");
    prop[1] = xmlGetProp(file->current, "recall_name\0");
    prop[2] = xmlGetProp(file->current, "control\0");

    if(prop[0] == NULL || prop[1] == NULL || prop[2] == NULL)
      return;

    line_member->recall = ags_channel_find_recall(line->channel, prop[0], prop[1]);
    //    line_member->recall->control[atoi(prop[2])] = &(GTK_RANGE(widget)->adjustment->value);
  }

  prop[0] = xmlGetProp(file->current, "rows\0");
  prop[1] = xmlGetProp(file->current, "cols\0");

  if(prop[0] != NULL && prop[1]){
    rows = g_strtod(prop[0], NULL);
    cols = g_strtod(prop[1], NULL);
    gtk_table_resize(line->table, rows, cols);
  }

  node = file->current;
  file->current = node->children;
  
  while(file->current != NULL){
    child = file->current;

    if(file->current->type == XML_ELEMENT_NODE && !xmlStrncmp(file->current->name, "AgsLineMember\0", 13))
      ags_file_read_line_member();

    file->current = child->next;
  }
}

void
ags_file_read_line_member(AgsFile *file, AgsLineMember *line_member)
{
  //TODO:JK: implement me
}

void
ags_file_read_editor(AgsFile *file, AgsEditor *editor)
{
  xmlNodePtr node, child;
  GList *list0, *list1;

  node = file->current;
  file->current = node->children;

  while(file->current != NULL){
    child = file->current;

    if(file->current->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(g_type_name(AGS_TYPE_TOOLBAR), file->current->name, 10)){
	ags_file_read_toolbar(file, editor->toolbar);
      }else if(!xmlStrncmp(g_type_name(AGS_TYPE_NOTEBOOK), file->current->name, 11)){
	ags_file_read_notebook(file, editor->notebook);
      }
    }

    file->current = child->next;
  }
}

void
ags_file_read_toolbar(AgsFile *file, AgsToolbar *toolbar)
{
  GList *list0, *list1;
  xmlChar *prop;
  guint i;

  prop = xmlGetProp(file->current, "mode\0");

  list0 = gtk_container_get_children((GtkContainer *) toolbar);

  if(prop != NULL){
    if(!xmlStrncmp(prop, "edit\0", 4)){
      gtk_toggle_button_set_active((GtkToggleButton *) list0->data, TRUE);
      list0 = g_list_nth(list0, 3);
    }else if(!xmlStrncmp(prop, "clear\0", 5)){
      list0 = list0->next;
      gtk_toggle_button_set_active((GtkToggleButton *) list0->data, TRUE);
      list0 = list0->next->next;
    }else if(!xmlStrncmp(prop, "select\0", 6)){
      list0 = list0->next->next;
      gtk_toggle_button_set_active((GtkToggleButton *) list0->data, TRUE);
      list0 = list0->next;
    }
  }

  list0 = g_list_nth(list0, 3);

  prop = xmlGetProp(file->current, "tic\0");
  list0 = list0->next;

  if(prop != NULL){
    list1 = gtk_container_get_children((GtkContainer *) GTK_OPTION_MENU(list0->data)->menu);

    for(i = 0; i < 9; i++){
      if(!xmlStrncmp(prop, gtk_label_get_text(GTK_LABEL(GTK_BIN(list1->data)->child)), 4))
	break;

      list1 = list1->next;
    }

    gtk_option_menu_set_history((GtkOptionMenu *) list0->data, i);
  }

  prop = xmlGetProp(file->current, "zoom\0");
  list0 = list0->next->next;

  if(prop != NULL){
    list1 = gtk_container_get_children((GtkContainer *) GTK_OPTION_MENU(list0->data)->menu);

    for(i = 0; i < 9; i++){
      if(!xmlStrncmp(prop, gtk_label_get_text((GtkLabel *) GTK_BIN(list1->data)->child), 4))
	break;

      list1 = list1->next;
    }

    gtk_option_menu_set_history((GtkOptionMenu *) list0->data, i);
  }
}

void
ags_file_read_notebook(AgsFile *file, AgsNotebook *notebook)
{
}

void
ags_file_read_navigation(AgsFile *file, AgsNavigation *navigation)
{
  GList *list0, *list1, *list2;
  xmlChar *prop;

  list0 = gtk_container_get_children((GtkContainer *) navigation);
  list1 = gtk_container_get_children((GtkContainer *) list0->next->data);
  list0 = gtk_container_get_children((GtkContainer *) list0->data);

  prop = xmlGetProp(file->current, "expandet\0");

  if(prop != NULL)
    if(!xmlStrncmp(prop, AGS_FILE_TRUE, 4)){
      list2 = gtk_container_get_children((GtkContainer *) list0->data);
      gtk_arrow_set((GtkArrow *) list2->data, GTK_ARROW_DOWN, GTK_SHADOW_NONE);
    }

  list0 = list0->next->next;
  prop = xmlGetProp(file->current, "bpm\0");

  if(prop != NULL)
    gtk_adjustment_set_value(GTK_SPIN_BUTTON(list0->data)->adjustment, g_strtod(prop, NULL));

  list0 = g_list_nth(list0, 7);
  prop = xmlGetProp(file->current, "loop\0");

  if(prop != NULL)
    if(!xmlStrncmp(prop, AGS_FILE_TRUE, 4))
      gtk_toggle_button_set_active((GtkToggleButton *) list0->data, TRUE);

  list0 = list0->next->next;
  prop = xmlGetProp(file->current, "position_min\0");

  if(prop != NULL)
    gtk_adjustment_set_value(GTK_SPIN_BUTTON(list0->data)->adjustment, g_strtod(prop, NULL));

  list0 = list0->next;
  prop = xmlGetProp(file->current, "position_sec\0");

  if(prop != NULL)
    gtk_adjustment_set_value(GTK_SPIN_BUTTON(list0->data)->adjustment, g_strtod(prop, NULL));

  list0 = list0->next->next;
  prop = xmlGetProp(file->current, "duration_min\0");

  if(prop != NULL)
    gtk_adjustment_set_value(GTK_SPIN_BUTTON(list0->data)->adjustment, g_strtod(prop, NULL));

  list0 = list0->next;
  prop = xmlGetProp(file->current, "duration_sec\0");

  if(prop != NULL)
    gtk_adjustment_set_value(GTK_SPIN_BUTTON(list0->data)->adjustment, g_strtod(prop, NULL));


  list1 = list1->next;
  prop = xmlGetProp(file->current, "loop_left_min\0");

  if(prop != NULL)
    gtk_adjustment_set_value(GTK_SPIN_BUTTON(list1->data)->adjustment, g_strtod(prop, NULL));

  list1 = list1->next;
  prop = xmlGetProp(file->current, "loop_left_sec\0");

  if(prop != NULL)
    gtk_adjustment_set_value(GTK_SPIN_BUTTON(list1->data)->adjustment, g_strtod(prop, NULL));

  list1 = list1->next->next;
  prop = xmlGetProp(file->current, "loop_right_min\0");

  if(prop != NULL)
    gtk_adjustment_set_value(GTK_SPIN_BUTTON(list1->data)->adjustment, g_strtod(prop, NULL));

  list1 = list1->next;
  prop = xmlGetProp(file->current, "loop_right_sec\0");

  if(prop != NULL)
    gtk_adjustment_set_value(GTK_SPIN_BUTTON(list1->data)->adjustment, g_strtod(prop, NULL));

  list1 = list1->next;
  prop = xmlGetProp(file->current, "raster\0");

  if(prop != NULL)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(list1->data), TRUE);

}

void
ags_file_read_machine_editor(AgsFile *file, AgsMachineEditor *machine_editor)
{
  //TODO:JK: implement me
}

void
ags_file_read_pad_editor(AgsFile *file, AgsPadEditor *pad_editor)
{
  //TODO:JK: implement me
}

void
ags_file_read_line_editor(AgsFile *file, AgsLineEditor *line_editor)
{
  //TODO:JK: implement me
}

void
ags_file_read_link_editor(AgsFile *file, AgsLinkEditor *link_editor)
{
  //TODO:JK: implement me
}

void
ags_file_read_line_member_editor(AgsFile *file, AgsLineMemberEditor *line_member_editor)
{
  //TODO:JK: implement me
}

void
ags_file_read_line_member_editor(AgsFile *file, AgsLineMemberEditor *line_member_editor)
{
  //TODO:JK: implement me
}

void
ags_file_read_panel(AgsFile *file, AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_file_read_mixer(AgsFile *file, AgsMachine *machine)
{
}

void
ags_file_read_drum(AgsFile *file, AgsMachine *machine)
{
}

void
ags_file_read_matrix(AgsFile *file, AgsMachine *machine)
{
}

void
ags_file_read_synth(AgsFile *file, AgsMachine *machine)
{
}

void
ags_file_read_oscillator(AgsFile *file, AgsOscillator *oscillator)
{
}

void
ags_file_read_ffplayer(AgsFile *file, AgsMachine *machine)
{
}

void
ags_file_write_audio(AgsFile *file, AgsAudio *audio)
{
  xmlNodePtr node;
  AgsChannel *channel;
  GList *list;

  node =
    file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_AUDIO), NULL);

  xmlNewProp(node, BAD_CAST "audio_channels\0", BAD_CAST g_strdup_printf("%d\0", audio->audio_channels));
  xmlNewProp(node, BAD_CAST "output_pads\0", BAD_CAST g_strdup_printf("%d\0", audio->output_pads));
  xmlNewProp(node, BAD_CAST "input_pads\0", BAD_CAST g_strdup_printf("%d\0", audio->input_pads));
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");

  channel = audio->output;

  while(channel != NULL){
    ags_file_write_channel(file, channel);
    //    AGS_CHANNEL_GET_CLASS(channel)->write_file(file, channel);
    file->current = node;

    channel = channel->next;
  }

  channel = audio->input;

  while(channel != NULL){
    ags_file_write_channel(file, channel);
    //    AGS_CHANNEL_GET_CLASS(channel)->write_file(file, channel);
    file->current = node;

    channel = channel->next;
  }

  if(AGS_AUDIO_HAS_NOTATION & audio->flags){
    list = audio->notation;

    while(list != NULL){
      //      ags_file_write_notation(file, (AgsNotation *) list->data);

      list = list->next;
    }
  }
}

void
ags_file_write_channel(AgsFile *file, AgsChannel *channel)
{
  xmlNodePtr node;
  GList *list;
  gboolean is_output, is_input;

  node =
    file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_CHANNEL), NULL);

  xmlNewProp(node, BAD_CAST "pad\0", g_strdup_printf("%d\0", channel->pad));
  xmlNewProp(node, BAD_CAST "audio_channel\0", g_strdup_printf("%d\0", channel->audio_channel));
  xmlNewProp(node, BAD_CAST "link_name\0", g_strdup((channel->link != NULL) ? AGS_MACHINE(AGS_AUDIO(channel->link->audio)->machine)->name: "NULL\0"));
  xmlNewProp(node, BAD_CAST "link_line\0", g_strdup_printf("%d\0", ((channel->link != NULL) ? channel->link->line: 0)));
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");

  if(AGS_IS_OUTPUT(channel)){
    is_output = TRUE;

    //    if(AGS_AUDIO_OUTPUT_HAS_RECYCLING & AGS_AUDIO(channel->audio)->flags)
    //      ags_file_write_recycling(file, channel->first_recycling);
    //    file->current = node;
  }else if(AGS_IS_INPUT(channel)){
    is_input = TRUE;

    //    if(AGS_AUDIO_INPUT_HAS_RECYCLING & AGS_AUDIO(channel->audio)->flags)
    //      ags_file_write_recycling(channel->first_recycling);
    //    file->current = node;
  }else
    is_output =
      is_input = FALSE;

  list = channel->recall;

  while(list != NULL){
    ags_file_write_recall(file, (AgsRecall *) list->data);
    file->current = node;

    list = list->next;
  }

  list = channel->pattern;

  if(list != NULL){
    ags_file_write_pattern(file, (AgsPattern *) list->data);
    file->current = node;

    list = list->next;
  }

  if(is_output)
    ags_file_write_output(file, channel);
  else if(is_input)
    ags_file_write_input(file, channel);
}

void
ags_file_write_output(AgsFile *file, AgsChannel *channel)
{
  file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_OUTPUT), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_input(AgsFile *file, AgsChannel *channel)
{
  AgsInput *input = (AgsInput *) channel;

  file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_INPUT), NULL);

  if(input->playable != NULL){
    //TODO:JK: add support for more file types
    if(AGS_IS_AUDIO_FILE(input->playable)){
      xmlNewProp(file->current, BAD_CAST "file\0", BAD_CAST (AGS_AUDIO_FILE(input->playable)->name));
    }
  }

  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_recycling(AgsFile *file, AgsRecycling *recycling)
{
  xmlNodePtr node;
  GList *list;

  node =
    file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_RECYCLING), NULL);

  /*
  xmlNewProp(node, NULL, BAD_CAST "toplevel\0", BAD_CAST ((AGS_RECYCLING_TOPLEVEL & recycling->flags) ? AGS_FILE_TRUE: AGS_FILE_FALSE));
  xmlNewProp(node, NULL, BAD_CAST "owns_signal\0", BAD_CAST ((AGS_RECYCLING_OWNS_SIGNAL & recycling->flags) ? AGS_FILE_TRUE: AGS_FILE_FALSE));
  xmlNewProp(node, NULL, BAD_CAST "current_recall\0", BAD_CAST g_strdup_printf("%d\0",
									       g_list_position(recycling->current)));
  */
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");

  list = recycling->audio_signal;

  while(list != NULL){
    ags_file_write_audio_signal(file, (AgsAudioSignal *) list->data);
    file->current = node;

    list = list->next;
  }
}

void
ags_file_write_audio_signal(AgsFile *file, AgsAudioSignal *audio_signal)
{
  xmlNodePtr node;
  GList *list;

  node =
    file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_AUDIO_SIGNAL), NULL);

  xmlNodeAddContent(file->current, BAD_CAST "\n\0");

  list = audio_signal->stream_beginning;

  while(list != NULL){
    ags_file_write_stream(file, list->data, AGS_DEVOUT(audio_signal->devout)->buffer_size);
    file->current = node;

    list = list->next;
  }
}

void
ags_file_write_stream(AgsFile *file, gpointer stream, guint buffer_size)
{
  xmlChar *content;
  int length;

  file->current = xmlNewChild(file->current, NULL, BAD_CAST "AgsStream\0", NULL);
  length = buffer_size * (sizeof(guint) / sizeof(xmlChar));

  content = (xmlChar *) malloc(length);
  memcpy(content, (guint *) stream, length);
  xmlNodeAddContentLen(file->current, content, length);
}

void
ags_file_write_pattern(AgsFile *file, AgsPattern *pattern)
{
  xmlNodePtr node;
  GString *content;
  uint i, j, k;

  node =
    file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_PATTERN), NULL);

  xmlNewProp(node, BAD_CAST "dim0\0", BAD_CAST g_strdup_printf("%d\0", pattern->dim[0]));
  xmlNewProp(node, BAD_CAST "dim1\0", BAD_CAST g_strdup_printf("%d\0", pattern->dim[1]));
  xmlNewProp(node, BAD_CAST "lenght\0", BAD_CAST g_strdup_printf("%d\0", pattern->dim[2]));
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");

  for(i = 0; i < pattern->dim[0]; i++)
    for(j = 0; j < pattern->dim[1]; j++){
      file->current = xmlNewChild(node, NULL, BAD_CAST "AgsPatternData\0", NULL);

      xmlNewProp(file->current, BAD_CAST "coding\0", BAD_CAST g_strdup("human readable\0"));
      xmlNewProp(file->current, BAD_CAST "index0\0", BAD_CAST g_strdup_printf("%d\0", i));
      xmlNewProp(file->current, BAD_CAST "index1\0", BAD_CAST g_strdup_printf("%d\0", j));

      content = g_string_sized_new(pattern->dim[2] + 1);

      for(k = 0; k < pattern->dim[2]; k++)
	g_string_insert_c(content, k, (gchar) (ags_pattern_get_bit(pattern, i, j, k) ? '1': '0'));

      g_string_insert_c(content, k, (gchar) '\0');

      xmlNodeAddContent(file->current, BAD_CAST (content->str));
      file->current = node;
    }
}

void
ags_file_write_notation(AgsFile *file, AgsNotation *notation)
{
  xmlNodePtr node;
  GList *list;

  node =
    file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_NOTATION), NULL);

  xmlNewProp(node, BAD_CAST "raster\0", BAD_CAST g_strdup((AGS_NOTATION_RASTER & notation->flags) ? AGS_FILE_TRUE: AGS_FILE_FALSE));
  xmlNewProp(node, BAD_CAST "tact\0", BAD_CAST g_strdup(notation->tact));
  xmlNewProp(node, BAD_CAST "bpm\0", BAD_CAST g_strdup_printf("%d\n\0", notation->bpm));
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");

  list = notation->notes;

  while(list != NULL){
    ags_file_write_note(file, (AgsNote *) list->data);
    file->current = node;

    list = list->next;
  }
}

void
ags_file_write_note(AgsFile *file, AgsNote *note)
{
  xmlNodePtr node, child;

  node =
    file->current = xmlNewChild(file->current, NULL, BAD_CAST "AgsNote\0", NULL);

  xmlNewProp(file->current, BAD_CAST "x0\0", BAD_CAST g_strdup_printf("%d\0", note->x[0]));
  xmlNewProp(file->current, BAD_CAST "x1\0", BAD_CAST g_strdup_printf("%d\0", note->x[1]));
  xmlNewProp(file->current, BAD_CAST "y\0", BAD_CAST g_strdup_printf("%d\0", note->y));
}

void
ags_file_write_devout(AgsFile *file, AgsDevout *devout)
{
  file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_DEVOUT), NULL);

  xmlNewProp(file->current, BAD_CAST "pcm_channels\0", BAD_CAST g_strdup_printf("%d\0", devout->pcm_channels));
  xmlNewProp(file->current, BAD_CAST "dsp_channels\0", BAD_CAST g_strdup_printf("%d\0", devout->dsp_channels));
  xmlNewProp(file->current, BAD_CAST "bits\0", BAD_CAST g_strdup_printf("%d\0", devout->bits));
  xmlNewProp(file->current, BAD_CAST "buffer_size\0", BAD_CAST g_strdup_printf("%d\0", devout->buffer_size));
  xmlNewProp(file->current, BAD_CAST "frequency\0", BAD_CAST g_strdup_printf("%d\0", devout->frequency));
  //  xmlNewProp(file->current, BAD_CAST "system\0", BAD_CAST g_strdup(devout->system));
  //  xmlNewProp(file->current, BAD_CAST "device\0", BAD_CAST g_strdup(devout->device));
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_task(AgsFile *file, AgsTask *task)
{
  xmlNodePtr node;

  node =
    file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_TASK), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");

  xmlNewProp(file->current, BAD_CAST "name\0", BAD_CAST g_strdup(task->name));
  xmlNewProp(file->current, BAD_CAST "start\0", BAD_CAST g_strdup_printf("%d\0", task->start));
}

void
ags_file_write_recall(AgsFile *file, AgsRecall *recall)
{
  /*
  if(!xmlStrncmp(recall->effect, AGS_COPY_STREAM_TO_STREAM, 11) || !xmlStrncmp(recall->effect, AGS_EFFECT_COPY_BUFFER, 11) || !xmlStrncmp(recall->effect, AGS_EFFECT_DOWN_MIX, 8))
    return;

  file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_RECALL), NULL);

  xmlNewProp(file->current, BAD_CAST "effect\0", BAD_CAST g_strdup(recall->effect));
  xmlNewProp(file->current, BAD_CAST "name\0", BAD_CAST g_strdup(recall->name));
  xmlNewProp(file->current, BAD_CAST "control_count\0", BAD_CAST g_strdup_printf("%d\0", recall->control_count));
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");

  if(!xmlStrncmp(recall->effect, AGS_COPY_STREAM_TO_STREAM, 25))
    ags_file_write_copy_stream_to_stream(file, recall);
  else if(!xmlStrncmp(recall->effect, AGS_COPY_INPUT_TO_OUTPUT, 24))
    ags_file_write_copy_input_to_output(file, recall);
  else if(!xmlStrncmp(recall->effect, AGS_COPY_PATTERN, 16))
    ags_file_write_copy_pattern(file, recall);
  else if(!xmlStrncmp(recall->effect, AGS_MIX_VOLUME, 14))
    ags_file_write_mix_volume(file, recall);
  */
}

void
ags_file_write_play_pattern(AgsFile *file, AgsRecall *recall)
{
  file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_PLAY_PATTERN), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_play_channel(AgsFile *file, AgsRecall *recall)
{
  file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_PLAY_CHANNEL_RUN), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_play_audio_signal(AgsFile *file, AgsRecall *recall)
{
  file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_PLAY_AUDIO_SIGNAL), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_volume_channel(AgsFile *file, AgsRecall *recall)
{
  file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_VOLUME_CHANNEL), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_copy_pattern(AgsFile *file, AgsRecall *recall)
{
  file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_COPY_PATTERN_CHANNEL_RUN), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_copy_channel(AgsFile *file, AgsRecall *recall)
{
  file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_COPY_CHANNEL), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_copy_audio_signal(AgsFile *file, AgsRecall *recall)
{
  file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_COPY_AUDIO_SIGNAL), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_window(AgsFile *file)
{
  xmlNodePtr node, node1;
  GList *list;

  node = 
    file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_WINDOW), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");

  ags_file_write_menu_bar(file, AGS_WINDOW(file->window)->menu_bar);

  node1 =
    file->current = xmlNewChild(node, NULL, BAD_CAST "AgsMachines\0", NULL);
  list = gtk_container_get_children((GtkContainer *) AGS_WINDOW(file->window)->machines);

  while(list != NULL){
    ags_file_write_machine(file, (AgsMachine *) list->data);
    file->current = node1;

    list = list->next;
  }

  file->current = node;
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");

  ags_file_write_editor(file, AGS_WINDOW(file->window)->editor);

  file->current = node;
  ags_file_write_navigation(file, AGS_WINDOW(file->window)->navigation);
}

void
ags_file_write_menu_bar(AgsFile *file, AgsMenuBar *menu_bar)
{
  xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_MENU_BAR), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_machine(AgsFile *file, AgsMachine *machine)
{
  xmlNodePtr node;

  node =
    file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_MACHINE), NULL);
  xmlNewProp(node, BAD_CAST "name\0", BAD_CAST g_strdup(machine->name));
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");

  ags_file_write_audio(file, (AgsAudio *) machine->audio);
  file->current = node;

  if(AGS_IS_PANEL(machine))
    ags_file_write_panel(file, machine);
  else if(AGS_IS_MIXER(machine))
    ags_file_write_mixer(file, machine);
  else if(AGS_IS_DRUM(machine))
    ags_file_write_drum(file, machine);
  else if(AGS_IS_MATRIX(machine))
    ags_file_write_matrix(file, machine);
  else if(AGS_IS_SYNTH(machine))
    ags_file_write_synth(file, machine);
  else if(AGS_IS_FFPLAYER(machine))
    ags_file_write_ffplayer(file, machine);
}

void
ags_file_write_pad(AgsFile *file, AgsPad *pad)
{
  xmlNodePtr node;
  GList *list;

  node =
    file->current = xmlNewChild(file->current, NULL, g_type_name(AGS_TYPE_PAD), NULL);

  /*
  xmlNewProp(file->current, BAD_CAST "show_grouping\0", BAD_CAST g_strdup((GTK_WIDGET_VISIBLE(pad->hbox) ? AGS_FILE_TRUE: AGS_FILE_FALSE)));

  list = gtk_container_get_children((GtkContainer *) pad->hbox);
  xmlNewProp(file->current, BAD_CAST "groupe_pad\0", BAD_CAST g_strdup((GTK_TOGGLE_BUTTON(list->data)->active ? AGS_FILE_TRUE: AGS_FILE_FALSE)));

  list = list->next;
  xmlNewProp(file->current, BAD_CAST "groupe_pad\0", BAD_CAST g_strdup((GTK_TOGGLE_BUTTON(list->data)->active ? AGS_FILE_TRUE: AGS_FILE_FALSE)));
  */

  xmlNodeAddContent(file->current, BAD_CAST "\n\0");

  list = gtk_container_get_children((GtkContainer *) pad->option->menu);

  while(list != NULL){
    ags_file_write_line(file, (AgsLine *) list->data);
    file->current = node;

    list = list->next;
  }
}

void
ags_file_write_line(AgsFile *file, AgsLine *line)
{
  xmlNodePtr node;
  GList *list;
  AgsLineMember *line_member;
  char *range;

  file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_LINE), NULL);

  xmlNewProp(file->current, BAD_CAST "rows\0", g_strdup_printf("%d\0", line->table->nrows));
  xmlNewProp(file->current, BAD_CAST "cols\0", g_strdup_printf("%d\0", line->table->ncols));

  list = line->table->children;

  while(list != NULL){
    if(GTK_IS_SCALE(list->data))
      range = "GtkScale";
    else if(GTK_IS_SPIN_BUTTON(list->data))
      range = "GtkSpinButton";
    //    else if(GTK_IS_DIAL(list->data))
    //      visualization = "GtkDial";
    else{
      list = list->next;
      continue;
    }

    node = xmlNewChild(file->current, NULL, BAD_CAST "AgsLineMember\0", NULL);

    line_member = (AgsLineMember *) g_object_get_data(G_OBJECT(list->data), "AgsLineMember\0");

    //    xmlNewProp(node, BAD_CAST "effect\0", BAD_CAST g_strdup(line_member->recall->effect));
    xmlNewProp(node, BAD_CAST "recall_name\0", BAD_CAST g_strdup(line_member->recall->name));
    xmlNewProp(node, BAD_CAST "control\0", BAD_CAST g_strdup_printf("%d\0", line_member->control));

    xmlNewProp(node, BAD_CAST "visualization\0", BAD_CAST g_strdup(range));

    xmlNewProp(node, BAD_CAST "left_attach\0", BAD_CAST g_strdup_printf("%d\0", ((GtkTableChild*) list->data)->left_attach));
    xmlNewProp(node, BAD_CAST "right_attach\0", BAD_CAST g_strdup_printf("%d\0", ((GtkTableChild*) list->data)->right_attach));
    xmlNewProp(node, BAD_CAST "top_attach\0", BAD_CAST g_strdup_printf("%d\0", ((GtkTableChild*) list->data)->top_attach));
    xmlNewProp(node, BAD_CAST "bottom_attach\0", BAD_CAST g_strdup_printf("%d\0", ((GtkTableChild*) list->data)->bottom_attach));

    list = list->next;
  }
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}


void
ags_file_write_line_member(AgsFile *file, AgsLineMember *line_member)
{
  //TODO:JK: implement me
}

void
ags_file_write_editor(AgsFile *file, AgsEditor *editor)
{
  xmlNodePtr node;

  node =
    file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_EDITOR), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");

  ags_file_write_toolbar(file, editor->toolbar);

  file->current = node;
  ags_file_write_notebook(file, editor->notebook);
}

void
ags_file_write_navigation(AgsFile *file, AgsNavigation *navigation)
{
  GList *list0, *list1, *list2;

  list0 = gtk_container_get_children((GtkContainer *) navigation);
  list1 = gtk_container_get_children((GtkContainer *) list0->data);
  list2 = gtk_container_get_children((GtkContainer *) list1->data);

  file->current = xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_NAVIGATION), NULL);

  xmlNewProp(file->current, BAD_CAST "expandet\0", ((GTK_ARROW(list2->data)->arrow_type == GTK_ARROW_DOWN) ? AGS_FILE_TRUE: AGS_FILE_FALSE));
  list1 = list1->next->next;

  xmlNewProp(file->current, BAD_CAST "bpm\0", BAD_CAST g_strdup_printf("%f\0", (double) GTK_SPIN_BUTTON(list1->data)->adjustment->value));
  list1 = g_list_nth(list1, 7);

  xmlNewProp(file->current, BAD_CAST "loop\0", BAD_CAST g_strdup((GTK_TOGGLE_BUTTON(list1->data)->active ? AGS_FILE_TRUE: AGS_FILE_FALSE)));
  list1 = list1->next->next;

  xmlNewProp(file->current, BAD_CAST "position_min\0", BAD_CAST g_strdup_printf("%f\0", (double) GTK_SPIN_BUTTON(list1->data)->adjustment->value));
  list1 = list1->next;

  xmlNewProp(file->current, BAD_CAST "position_sec\0", BAD_CAST g_strdup_printf("%f\0", (double) GTK_SPIN_BUTTON(list1->data)->adjustment->value));
  list1 = list1->next->next;

  xmlNewProp(file->current, BAD_CAST "duration_min\0", BAD_CAST g_strdup_printf("%f\0", (double) GTK_SPIN_BUTTON(list1->data)->adjustment->value));
  list1 = list1->next;

  xmlNewProp(file->current, BAD_CAST "duration_sec\0", BAD_CAST g_strdup_printf("%f\0", (double) GTK_SPIN_BUTTON(list1->data)->adjustment->value));
  //  list1 = list1->next;

  /*expansion*/
  list1 = gtk_container_get_children((GtkContainer *) list0->next->data);

  list1 = list1->next;
  xmlNewProp(file->current, BAD_CAST "loop_left_min\0", BAD_CAST g_strdup_printf("%f\0", (double) GTK_SPIN_BUTTON(list1->data)->adjustment->value));
  list1 = list1->next;

  xmlNewProp(file->current, BAD_CAST "loop_left_sec\0", BAD_CAST g_strdup_printf("%f\0", (double) GTK_SPIN_BUTTON(list1->data)->adjustment->value));
  list1 = list1->next->next;

  xmlNewProp(file->current, BAD_CAST "loop_right_min\0", BAD_CAST g_strdup_printf("%f\0", (double) GTK_SPIN_BUTTON(list1->data)->adjustment->value));
  list1 = list1->next;

  xmlNewProp(file->current, BAD_CAST "loop_right_sec\0", BAD_CAST g_strdup_printf("%f\0", (double) GTK_SPIN_BUTTON(list1->data)->adjustment->value));
  list1 = list1->next;

  xmlNewProp(file->current, BAD_CAST "raster\0", g_strdup((GTK_TOGGLE_BUTTON(list1->data)->active ? AGS_FILE_TRUE: AGS_FILE_FALSE)));
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_machine_editor(AgsFile *file, AgsMachineEditor *machine_editor)
{
  //TODO:JK: implement me
}

void
ags_file_write_pad_editor(AgsFile *file, AgsPadEditor *pad_editor)
{
  //TODO:JK: implement me
}

void
ags_file_write_line_editor(AgsFile *file, AgsLineEditor *line_editor)
{
  //TODO:JK: implement me
}

void
ags_file_write_link_editor(AgsFile *file, AgsLinkEditor *link_editor)
{
  //TODO:JK: implement me
}

void
ags_file_write_line_member_editor(AgsFile *file, AgsLineMemberEditor *line_member_editor)
{
  //TODO:JK: implement me
}

void
ags_file_write_toolbar(AgsFile *file, AgsToolbar *toolbar)
{
  GList *list0, *list1;

  list0 = gtk_container_get_children((GtkContainer *) toolbar);

  if(GTK_TOGGLE_BUTTON(list0->data)->active)
    xmlNewProp(file->current, BAD_CAST "mode\0", BAD_CAST g_strdup("edit\0"));
  else if(GTK_TOGGLE_BUTTON(list0->next->data)->active)
    xmlNewProp(file->current, BAD_CAST "mode\0", BAD_CAST g_strdup("clear\0"));
  else if(GTK_TOGGLE_BUTTON(list0->next->next->data)->active)
    xmlNewProp(file->current, BAD_CAST "mode\0", BAD_CAST g_strdup("select\0"));

  list0 = g_list_nth(list0, 7);
  //  xmlNewProp(file->current, BAD_CAST "tic\0", BAD_CAST g_strdup(gtk_label_get_text(GTK_LABEL(GTK_BIN(GTK_OPTION_MENU(list0->data)->menu_item)->child))));

  list0 = list0->next->next;
  //  xmlNewProp(file->current, BAD_CAST "zoom\0", BAD_CAST g_strdup(gtk_label_get_text(GTK_LABEL(GTK_BIN(GTK_OPTION_MENU(list0->data)->menu_item)->child))));
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_notebook(AgsFile *file, AgsNotebook *notebook)
{
}

void
ags_file_write_panel(AgsFile *file, AgsMachine *machine)
{
  AgsPanel *panel;

  xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_PANEL), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_mixer(AgsFile *file, AgsMachine *machine)
{
  AgsMixer *mixer;

  xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_MIXER), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_drum(AgsFile *file, AgsMachine *machine)
{
  AgsDrum *drum;

  xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_DRUM), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_matrix(AgsFile *file, AgsMachine *machine)
{
  AgsMatrix *matrix;

  xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_MATRIX), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_synth(AgsFile *file, AgsMachine *machine)
{
  AgsSynth *synth;

  xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_SYNTH), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_oscillator(AgsFile *file, AgsOscillator *oscillator)
{
  xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_OSCILLATOR), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

void
ags_file_write_ffplayer(AgsFile *file, AgsMachine *machine)
{
  AgsFFPlayer *ffplayer;

  xmlNewChild(file->current, NULL, BAD_CAST g_type_name(AGS_TYPE_FFPLAYER), NULL);
  xmlNodeAddContent(file->current, BAD_CAST "\n\0");
}

AgsFile*
ags_file_new()
{
  AgsFile *file;

  file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				  NULL);

  return(file);
}
