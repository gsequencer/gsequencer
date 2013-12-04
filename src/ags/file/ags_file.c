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
#include <ags/file/ags_file_thread.h>
#include <ags/file/ags_file_sound.h>
#include <ags/file/ags_file_gui.h>
#include <ags/file/ags_file_stock.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include <openssl/md5.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

#include <ags/main.h>

#include <ags/util/ags_id_generator.h>

#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_id_ref.h>
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
void ags_file_real_read_start(AgsFile *file);

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_ENCODING,
  PROP_AUDIO_FORMAT,
  PROP_AUDIO_ENCODING,
  PROP_MAIN,
};

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
  GParamSpec *param_spec;

  ags_file_parent_class = g_type_class_peek_parent(file);

  /* GObjectClass */
  gobject = (GObjectClass *) file;

  gobject->get_property = ags_file_get_property;
  gobject->set_property = ags_file_set_property;

  gobject->finalize = ags_file_finalize;

  /* properties */
  param_spec = g_param_spec_string("filename\0",
				   "filename to read or write\0",
				   "The filename to read or write to.\0",
				   "unnamed\0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  param_spec = g_param_spec_string("encoding\0",
				   "encoding to use\0",
				   "The encoding of the XML document.\0",
				   AGS_FILE_DEFAULT_NS,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ENCODING,
				  param_spec);

  param_spec = g_param_spec_string("audio format\0",
				   "audio format to use\0",
				   "The audio format used to embedded audio.\0",
				   AGS_FILE_DEFAULT_AUDIO_FORMAT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_FORMAT,
				  param_spec);

  param_spec = g_param_spec_string("audio encoding\0",
				   "audio encoding to use\0",
				   "The audio encoding used to embedded audio.\0",
				   AGS_FILE_DEFAULT_AUDIO_ENCODING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_ENCODING,
				  param_spec);

  param_spec = g_param_spec_object("main\0",
				   "main object of file\0",
				   "The main object to write to file.\0",
				   AGS_TYPE_MAIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAIN,
				  param_spec);

  /* AgsFileClass */
  file->write = ags_file_real_write;
  file->write_resolve = ags_file_real_write_resolve;
  file->read = ags_file_real_read;
  file->read_resolve = ags_file_real_read_resolve;
  file->read_start = ags_file_real_read_start;

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

  file->audio_format = AGS_FILE_DEFAULT_AUDIO_FORMAT;
  file->audio_encoding = AGS_FILE_DEFAULT_AUDIO_ENCODING;

  file->doc = NULL;

  file->id_refs = NULL;
  file->lookup = NULL;

  file->main = NULL;

  file->clipboard = NULL;
  file->property = NULL;
  file->script = NULL;
  file->cluster = NULL;
  file->client = NULL;
  file->server = NULL;

  file->history = NULL;

  file->embedded_audio = NULL;
  file->file_link = NULL;
}

void
ags_file_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  AgsFile *file;

  file = AGS_FILE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      file->name = filename;
    }
    break;
  case PROP_ENCODING:
    {
      gchar *encoding;

      encoding = g_value_get_string(value);

      file->encoding = encoding;
    }
    break;
  case PROP_AUDIO_FORMAT:
    {
      gchar *audio_format;

      audio_format = g_value_get_string(value);

      file->audio_format = audio_format;
    }
    break;
  case PROP_AUDIO_ENCODING:
    {
      gchar *audio_encoding;

      audio_encoding = g_value_get_string(value);

      file->audio_encoding = audio_encoding;
    }
    break;
  case PROP_MAIN:
    {
      GObject *main;

      main = g_value_get_object(value);

      if(file->main == main){
	return;
      }

      if(file->main != NULL){
	g_object_unref(file->main);
      }

      if(main != NULL){
	g_object_ref(main);
      }

      file->main = main;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_get_property(GObject *gobject,
		      guint prop_id,
		      GValue *value,
		      GParamSpec *param_spec)
{
  AgsFile *file;

  file = AGS_FILE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, file->name);
    }
    break;
  case PROP_ENCODING:
    {
      g_value_set_string(value, file->encoding);
    }
    break;
  case PROP_AUDIO_FORMAT:
    {
      g_value_set_string(value, file->audio_format);
    }
    break;
  case PROP_AUDIO_ENCODING:
    {
      g_value_set_string(value, file->audio_encoding);
    }
    break;
  case PROP_MAIN:
    {
      g_value_set_object(value, file->main);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
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

gchar*
ags_file_str2md5(gchar *content, guint strlen, unsigned long len)
{
  gchar *checksum;
  MD5_CTX c;
  unsigned char *digest;
  guint checksumlen;
  guint i, i_stop;

  checksumlen = sqrt(len / 2);
  checksum = (gchar *) malloc((checksumlen + 1) * sizeof(gchar));

  MD5_Init(&c);

  i_stop = (guint) floor((double) strlen / len);

  for(i = 0; i <= i_stop; i++){
    if(i < i_stop){
      MD5_Update(&c, &(content[i * len]), len);
    }else{
      MD5_Update(&c, &(content[i * len]), strlen % len);
    }
  }

  MD5_Final(digest, &c);

  checksum[checksumlen] = '\0';

  for(i = 0; i < checksumlen; i++){
    sprintf(&(checksum[i * 2]), "%02x", (unsigned int) digest[i]);
  }

  return(checksum);
}

void
ags_file_add_id_ref(AgsFile *file, GObject *id_ref)
{
  if(id_ref == NULL)
    return;

  g_object_ref(G_OBJECT(id_ref));

  file->id_refs = g_list_prepend(file->id_refs,
				 id_ref);
}

GObject*
ags_file_find_id_ref_by_node(AgsFile *file, xmlNode *node)
{
  AgsFileIdRef *file_id_ref;
  GList *list;

  list = file->id_refs;

  while(list != NULL){
    file_id_ref = AGS_FILE_ID_REF(list->data);

    if(file_id_ref->node == node){
      return((GObject *) file_id_ref);
    }

    list = list->next;
  }

  return(NULL);
}

GObject*
ags_file_find_id_ref_by_xpath(AgsFile *file, gchar *xpath)
{
  AgsFileIdRef *file_id_ref;
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;
  guint i;

  /* Create xpath evaluation context */
  xpath_context = xmlXPathNewContext(file->doc);

  if(xpath_context == NULL) {
    fprintf(stderr,"Error: unable to create new XPath context\n");

    return(NULL);
  }

  /* Evaluate xpath expression */
  xpath_object = xmlXPathEvalExpression(xpath, xpath_context);

  if(xpath_object == NULL) {
    fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", xpath);
    xmlXPathFreeContext(xpath_context); 

    return(NULL);
  }

  node = xpath_object->nodesetval->nodeTab;

  for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
    if(node[i]->type == XML_ELEMENT_NODE){
      return(ags_file_find_id_ref_by_node(file,
					  node[i]));
    }
  }

  return(NULL);
}

GObject*
ags_file_find_id_ref_by_reference(AgsFile *file, gpointer ref)
{
  AgsFileIdRef *file_id_ref;
  GList *list;

  list = file->id_refs;

  while(list != NULL){
    file_id_ref = AGS_FILE_ID_REF(list->data);

    if(file_id_ref->ref == ref){
      return((GObject *) file_id_ref);
    }

    list = list->next;
  }

  return(NULL);
}

void
ags_file_add_lookup(AgsFile *file, GObject *file_lookup)
{
  if(file_lookup == NULL){
    return;
  }

  g_object_ref(G_OBJECT(file_lookup));

  file->lookup = g_list_prepend(file->lookup,
				file_lookup);
}

void
ags_file_real_write(AgsFile *file)
{
  AgsMain *main;
  xmlNode *root_node;
  GList *list;

  file->doc = xmlNewDoc(BAD_CAST "1.0\0");
  root_node = xmlNewNode(NULL, BAD_CAST "ags\0");
  xmlDocSetRootElement(file->doc, root_node);

  /* write clip board */
  //TODO:JK: implement me

  /* write scripts */
  //TODO:JK: implement me

  /* write cluster */
  //TODO:JK: implement me

  /* write client */
  //TODO:JK: implement me

  /* write server */
  //TODO:JK: implement me

  /* write main */
  ags_file_write_main(file,
		      root_node,
		      file->main);

  /* write embedded audio */
  //TODO:JK: implement me

  /* write file link */
  //TODO:JK: implement me
 
  /* write history */
  //TODO:JK: implement me

  /* resolve */
  ags_file_write_resolve(file);

  /* 
   * Dumping document to file
   */
  xmlSaveFormatFileEnc(file->name, file->doc, "UTF-8\0", 1);

  /*free the document */
  xmlFreeDoc(file->doc);

  /*
   *Free the global variables that may
   *have been allocated by the parser.
   */
  xmlCleanupParser();

  /*
   * this is to debug memory for regression tests
   */
  xmlMemoryDump();
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
  GList *list;

  list = file->lookup;

  while(list != NULL){
    ags_file_lookup_resolve(AGS_FILE_LOOKUP(list->data));

    list = list->next;
  }
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
  AgsMain *main;
  xmlNode *root_node, *child;

  /* parse the file and get the DOM */
  file->doc = xmlReadFile(file->name, NULL, 0);

  if(file->doc == NULL){
    printf("error: could not parse file %s\n", file->name);
  }

  /*Get the root element node */
  root_node = xmlDocGetRootElement(file->doc);

  /* child elements */
  child = root_node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp("ags-clip-board\0",
		     child->name,
		     15)){
	//TODO:JK: implement me
      }else if(!xmlStrncmp("ags-script-list\0",
			   child->name,
			   16)){
	//TODO:JK: implement me
      }else if(!xmlStrncmp("ags-cluster\0",
			   child->name,
			   12)){
	//TODO:JK: implement me
      }else if(!xmlStrncmp("ags-client\0",
			   child->name,
			   11)){
	//TODO:JK: implement me
      }else if(!xmlStrncmp("ags-server\0",
			   child->name,
			   11)){
	//TODO:JK: implement me
      }else if(!xmlStrncmp("ags-main\0",
			   child->name,
			   9)){
	main = NULL;

	ags_file_read_main(file,
			   child,
			   (GObject **) &main);
      }else if(!xmlStrncmp("ags-embedded-audio-list\0",
			   child->name,
			   24)){
	//TODO:JK: implement me
      }else if(!xmlStrncmp("ags-file-link-list\0",
			   child->name,
			   19)){
	//TODO:JK: implement me
      }else if(!xmlStrncmp("ags-history\0",
			   child->name,
			   12)){
	//TODO:JK: implement me
      }
    }
  }

  /* resolve */
  ags_file_read_resolve(file);

  /* start */
  ags_file_read_start(file);

  /*free the document */
  xmlFreeDoc(file->doc);

  /*
   *Free the global variables that may
   *have been allocated by the parser.
   */
  xmlCleanupParser();
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
  GList *list;

  list = file->lookup;

  while(list != NULL){
    ags_file_lookup_resolve(AGS_FILE_LOOKUP(list->data));

    list = list->next;
  }
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
  AgsMain *gobject;
  xmlNode *child;

  if(*main == NULL){
    gobject = g_object_new(AGS_TYPE_WINDOW,
			   NULL);

    *main = (GObject *) gobject;
  }else{
    gobject = (AgsMain *) *main;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));
  
  /* properties */
  gobject->version = xmlGetProp(node,
				AGS_FILE_VERSION_PROP);

  gobject->build_id = xmlGetProp(node,
				 AGS_FILE_BUILD_ID_PROP);

  //TODO:JK: check version compatibelity

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp("ags-audio-loop\0",
		     child->name,
		     15)){
	ags_file_read_audio_loop(file,
				 child,
				 (AgsAudioLoop **) &gobject->main_loop);
      }else if(!xmlStrncmp("ags-devout-list\0",
			   child->name,
			   16)){
	ags_file_read_devout_list(file,
				  child,
				  &gobject->devout);
      }else if(!xmlStrncmp("ags-window\0",
			   child->name,
			   11)){
	ags_file_read_window(file,
			     child,
			     &gobject->window);
      }
    }

    child = child->next;
  }
}

void
ags_file_write_main(AgsFile *file, xmlNode *parent, GObject *main)
{
  xmlNode *node, *child;
  gchar *id;

  id = ags_id_generator_create_uuid();

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", main,
				   NULL));

  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-main\0");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_VERSION_PROP,
	     AGS_MAIN(main)->version);

  xmlNewProp(node,
	     AGS_FILE_BUILD_ID_PROP,
	     AGS_MAIN(main)->build_id);

  /* ags-audio-list */
  ags_file_write_audio_loop(file,
			    node,
			    AGS_AUDIO_LOOP(AGS_MAIN(main)->main_loop));

  ags_file_write_devout_list(file,
			     node,
			     AGS_MAIN(main)->devout);

  ags_file_write_window(file,
			node,
			AGS_MAIN(main)->window);
}

AgsFile*
ags_file_new()
{
  AgsFile *file;

  file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				  NULL);

  return(file);
}
