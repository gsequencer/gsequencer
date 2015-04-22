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

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_marshal.h>

#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_launch.h>
#include <ags/file/ags_file_sound.h>
#include <ags/file/ags_file_gui.h>

#include <ags/thread/ags_thread_pool.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#include <ags/thread/ags_audio_loop.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

void ags_file_class_init(AgsFileClass *file);
void ags_file_init(AgsFile *file);
void ags_file_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_file_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);
void ags_file_finalize(GObject *gobject);

void ags_file_real_open(AgsFile *file);
void ags_file_real_open_from_data(AgsFile *file,
				  gchar *data, guint length);
void ags_file_real_rw_open(AgsFile *file,
			   gboolean create);

void ags_file_real_write(AgsFile *file);
void ags_file_real_write_concurrent(AgsFile *file);
void ags_file_real_write_resolve(AgsFile *file);

void ags_file_real_read(AgsFile *file);
void ags_file_real_read_resolve(AgsFile *file);
void ags_file_real_read_start(AgsFile *file);

/**
 * SECTION:ags_file
 * @short_description: read/write XML file
 * @title: AgsFile
 * @section_id:
 * @include: ags/file/ags_file.h
 *
 * The #AgsFile is an object to read or write files using XML. It
 * is the persisting layer of Advanced Gtk+ Sequencer.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_ENCODING,
  PROP_AUDIO_FORMAT,
  PROP_AUDIO_ENCODING,
  PROP_MAIN,
};

enum{
  OPEN,
  OPEN_FROM_DATA,
  RW_OPEN,
  WRITE,
  WRITE_CONCURRENT,
  WRITE_RESOLVE,
  READ,
  READ_RESOLVE,
  READ_START,
  LAST_SIGNAL,
};

static gpointer ags_file_parent_class = NULL;
static guint file_signals[LAST_SIGNAL] = { 0 };

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
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  param_spec = g_param_spec_string("encoding\0",
				   "encoding to use\0",
				   "The encoding of the XML document.\0",
				   NULL,
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
  file->open = ags_file_real_open;
  file->rw_open = ags_file_real_rw_open;
  file->open_from_data = ags_file_real_open_from_data;

  file->write = ags_file_real_write;
  file->write_concurrent = ags_file_real_write_concurrent;
  file->write_resolve = ags_file_real_write_resolve;
  file->read = ags_file_real_read;
  file->read_resolve = ags_file_real_read_resolve;
  file->read_start = ags_file_real_read_start;

  file_signals[OPEN] =
    g_signal_new("open\0",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, open),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  file_signals[OPEN_FROM_DATA] =
    g_signal_new("open-from-data\0",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, open_from_data),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__STRING_UINT,
		 G_TYPE_NONE, 0);

  file_signals[RW_OPEN] =
    g_signal_new("rw-open\0",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, rw_open),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__BOOLEAN,
		 G_TYPE_NONE, 0);

  file_signals[WRITE] =
    g_signal_new("write\0",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, write),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  file_signals[WRITE_CONCURRENT] =
    g_signal_new("write_concurrent\0",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, write_concurrent),
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

  file->out = NULL;
  file->buffer = NULL;

  file->filename = NULL;
  file->encoding = AGS_FILE_DEFAULT_ENCODING;
  file->dtd = AGS_FILE_DEFAULT_DTD;

  file->audio_format = AGS_FILE_DEFAULT_AUDIO_FORMAT;
  file->audio_encoding = AGS_FILE_DEFAULT_AUDIO_ENCODING;

  file->doc = NULL;

  file->id_refs = NULL;
  file->lookup = NULL;
  file->launch = NULL;

  file->ags_main = NULL;

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

      if(file->filename == filename){
	return;
      }

      file->filename = g_strdup(filename);
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
      GObject *ags_main;

      ags_main = g_value_get_object(value);

      if(file->ags_main == ags_main){
	return;
      }

      if(file->ags_main != NULL){
	g_object_unref(file->ags_main);
      }

      if(ags_main != NULL){
	g_object_ref(ags_main);
      }

      file->ags_main = ags_main;
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
      g_value_set_string(value, file->filename);
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
      g_value_set_object(value, file->ags_main);
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
ags_file_str2md5(gchar *content, guint strlen)
{
  GChecksum *checksum;
  gchar *str;

  str = g_compute_checksum_for_string(G_CHECKSUM_MD5,
				      content,
				      strlen);

  return(str);
}

void
ags_file_add_id_ref(AgsFile *file, GObject *id_ref)
{
  if(id_ref == NULL)
    return;

  g_object_ref(id_ref);

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

  if(file == NULL || xpath == NULL || !g_str_has_prefix(xpath, "xpath=\0")){
    g_message("invalid xpath: %s", xpath);

    return(NULL);
  }

  xpath = &(xpath[6]);

  /* Create xpath evaluation context */
  xpath_context = xmlXPathNewContext(file->doc);

  if(xpath_context == NULL) {
    fprintf(stderr,"Error: unable to create new XPath context\n");

    return(NULL);
  }

  /* Evaluate xpath expression */
  xpath_object = xmlXPathEval(xpath, xpath_context);

  if(xpath_object == NULL) {
    g_message("Error: unable to evaluate xpath expression \"%s\"", xpath);
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

  g_message("no xpath match: %s\0", xpath);
  
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
  if(file == NULL || file_lookup == NULL){
    return;
  }

  g_object_ref(G_OBJECT(file_lookup));

  file->lookup = g_list_prepend(file->lookup,
				file_lookup);
}

void
ags_file_add_launch(AgsFile *file, GObject *file_launch)
{
  if(file == NULL || file_launch == NULL){
    return;
  }

  g_object_ref(G_OBJECT(file_launch));

  file->launch = g_list_prepend(file->launch,
				file_launch);
}

void
ags_file_real_open(AgsFile *file)
{
  if(file == NULL){
    return;
  }

  /* parse the file and get the DOM */
  file->doc = xmlReadFile(file->filename, NULL, 0);

  if(file->doc == NULL){
    printf("error: could not parse file %s\n", file->filename);
  }

  /*Get the root element node */
  file->root_node = xmlDocGetRootElement(file->doc);
}

void
ags_file_open(AgsFile *file)
{
  g_return_if_fail(AGS_IS_FILE(file));

  g_object_ref(G_OBJECT(file));
  g_signal_emit(G_OBJECT(file),
		file_signals[OPEN], 0);
  g_object_unref(G_OBJECT(file));
}

void
ags_file_real_open_from_data(AgsFile *file,
			     gchar *data, guint length)
{
  if(file == NULL){
    return;
  }

  file->doc = xmlReadMemory(data, length, file->filename, NULL, 0);

  if(file->doc == NULL) {
    printf("error: could not parse file %s\n", file->filename);
  }

  /*Get the root element node */
  file->root_node = xmlDocGetRootElement(file->doc);
}

void
ags_file_open_from_data(AgsFile *file,
			gchar *data, guint length)
{
  g_return_if_fail(AGS_IS_FILE(file));

  g_object_ref(G_OBJECT(file));
  g_signal_emit(G_OBJECT(file),
		file_signals[OPEN_FROM_DATA], 0,
		data, length);
  g_object_unref(G_OBJECT(file));
}

void
ags_file_real_rw_open(AgsFile *file,
		      gboolean create)
{
  if(file == NULL){
    return;
  }

  file->out = fopen(file->filename, "w+\0");

  file->doc = xmlNewDoc("1.0\0");
  file->root_node = xmlNewNode(NULL, "ags\0");
  xmlDocSetRootElement(file->doc, file->root_node);
}

void
ags_file_rw_open(AgsFile *file,
		 gboolean create)
{
  g_return_if_fail(AGS_IS_FILE(file));

  g_object_ref(G_OBJECT(file));
  g_signal_emit(G_OBJECT(file),
		file_signals[RW_OPEN], 0,
		create);
  g_object_unref(G_OBJECT(file));
}

void
ags_file_open_filename(AgsFile *file,
		       gchar *filename)
{
  if(file == NULL){
    return;
  }

  if(file->filename != NULL){
    ags_file_close(file);
  }

  g_object_set(file,
	       "filename\0", filename,
	       NULL);
  ags_file_open(file);
}

void
ags_file_close(AgsFile *file)
{
  if(file == NULL){
    return;
  }

  if(file->out != NULL){
    fclose(file->out);
  }
  
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

  file->filename = NULL;
}

void
ags_file_real_write(AgsFile *file)
{
  AgsMain *ags_main;
  GList *list;
  int size;

  //  ags_file_rw_open(file,
  //		   TRUE);

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
		      file->root_node,
		      file->ags_main);

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
  //  xmlSaveFormatFileEnc(file->filename, file->doc, "UTF-8\0", 1);
  xmlDocDumpFormatMemoryEnc(file->doc, &(file->buffer), &size, file->encoding, TRUE);

  fwrite(file->buffer, size, sizeof(xmlChar), file->out);
  fflush(file->out);
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
ags_file_real_write_concurrent(AgsFile *file)
{
  AgsMain *ags_main;
  AgsThread *main_loop, *gui_thread, *task_thread;
  xmlNode *root_node;
  FILE *file_out;
  GList *list;
  xmlChar *buffer;
  int size;

  xmlNode *parent, *node, *child;
  gchar *id;

  main_loop = AGS_MAIN(file->ags_main)->main_loop;
  gui_thread = AGS_AUDIO_LOOP(main_loop)->gui_thread;
  task_thread = AGS_AUDIO_LOOP(main_loop)->task_thread;

  ags_main = (AgsMain *) file->ags_main;

  file->doc = xmlNewDoc("1.0\0");
  root_node = xmlNewNode(NULL, "ags\0");
  xmlDocSetRootElement(file->doc, root_node);

  parent = root_node;

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

  /* the main code - write main */
  ags_thread_lock(main_loop);

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-main\0");

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", ags_main,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", ((~AGS_MAIN_CONNECTED) & (AGS_MAIN(ags_main)->flags))));

  xmlNewProp(node,
	     AGS_FILE_VERSION_PROP,
	     AGS_MAIN(ags_main)->version);

  xmlNewProp(node,
	     AGS_FILE_BUILD_ID_PROP,
	     AGS_MAIN(ags_main)->build_id);

  /* add to parent */
  xmlAddChild(parent,
	      node);

  ags_thread_unlock(main_loop);

  /* child elements */
  /* thread */
  ags_thread_lock(main_loop);

  ags_file_write_thread(file,
			node,
			AGS_THREAD(AGS_MAIN(ags_main)->main_loop));

  ags_thread_unlock(main_loop);

  /* thread pool */
  ags_thread_lock(main_loop);

  ags_file_write_thread_pool(file,
			     node,
			     AGS_THREAD_POOL(AGS_MAIN(ags_main)->thread_pool));

  ags_thread_unlock(main_loop);

  /* write audio */
  ags_thread_lock(task_thread);

  ags_file_write_devout_list(file,
			     node,
			     AGS_MAIN(ags_main)->devout);

  ags_thread_unlock(task_thread);

  ags_thread_lock(gui_thread);

  ags_file_write_window(file,
			node,
			AGS_MAIN(ags_main)->window);

  ags_thread_unlock(gui_thread);

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
  //  xmlSaveFormatFileEnc(file->filename, file->doc, "UTF-8\0", 1);
  xmlDocDumpFormatMemoryEnc(file->doc, &buffer, &size, file->encoding, TRUE);

  file_out = fopen(file->filename, "w+\0");
  fwrite(buffer, size, sizeof(xmlChar), file_out);
  fflush(file_out);
  fclose(file_out);

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
ags_file_write_concurrent(AgsFile *file)
{
  g_return_if_fail(AGS_IS_FILE(file));

  g_object_ref(G_OBJECT(file));
  g_signal_emit(G_OBJECT(file),
		file_signals[WRITE_CONCURRENT], 0);
  g_object_unref(G_OBJECT(file));
}

void
ags_file_real_write_resolve(AgsFile *file)
{
  GList *list;

  file->lookup = g_list_prepend(file->lookup,
				NULL);
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
  AgsMain *ags_main;
  xmlNode *root_node, *child;
  pid_t pid_num;

  root_node = file->root_node;

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
	ags_main = NULL;

	ags_file_read_main(file,
			   child,
			   (GObject **) &ags_main);
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

    child = child->next;
  }

  /* resolve */
  ags_file_read_resolve(file);

  gtk_window_set_title((GtkWindow *) ags_main->window,
		       g_strconcat("ags - \0",
				   ags_main->window->name,
				   NULL));
  gtk_widget_show_all(GTK_WIDGET(ags_main->window));
  
  g_message("XML file resolved\0");

  ags_connectable_connect(AGS_CONNECTABLE(ags_main));

  g_message("XML file connected\0");

  /* start */
  ags_file_read_start(file);
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

  file->lookup = g_list_prepend(file->lookup,
				NULL);
  list = g_list_reverse(file->lookup);
  
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
  GList *list;

  list = g_list_reverse(file->launch);

  while(list != NULL){
    ags_file_launch_start(AGS_FILE_LAUNCH(list->data));

    list = list->next;
  }
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
ags_file_read_main(AgsFile *file, xmlNode *node, GObject **ags_main)
{
  AgsMain *gobject;
  GList *list;
  xmlNode *child;
  int argc;
  static const gchar *argv[] = {
    "ags\0",
  };

  if(*ags_main == NULL){
    gobject = g_object_new(AGS_TYPE_MAIN,
			   NULL);

    *ags_main = (GObject *) gobject;
  }else{
    gobject = (AgsMain *) *ags_main;
  }

  file->ags_main = (GObject *) gobject;

  argc = 1;
  g_object_set(G_OBJECT(file),
	       "main\0", gobject,
	       NULL);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));
  
  /* properties */
  gobject->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  gobject->version = xmlGetProp(node,
				AGS_FILE_VERSION_PROP);

  gobject->build_id = xmlGetProp(node,
				 AGS_FILE_BUILD_ID_PROP);

  //TODO:JK: check version compatibelity

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp("ags-thread\0",
		     child->name,
		     11)){
	ags_file_read_thread(file,
			     child,
			     (AgsThread **) &(gobject->main_loop));
	
	AGS_AUDIO_LOOP(gobject->main_loop)->ags_main = (GObject *) gobject;
      }else if(!xmlStrncmp("ags-thread-pool\0",
			   child->name,
			   16)){
	ags_file_read_thread_pool(file,
				  child,
				  (AgsThreadPool **) &(gobject->thread_pool));
      }else if(!xmlStrncmp("ags-devout-list\0",
			   child->name,
			   16)){
	ags_file_read_devout_list(file,
				  child,
				  &(gobject->devout));
      }else if(!xmlStrncmp("ags-window\0",
			   child->name,
			   11)){
	ags_file_read_window(file,
			     child,
			     &(gobject->window));
      }
    }

    child = child->next;
  }

  //TODO:JK: should be resolved
  AGS_TASK_THREAD(AGS_AUDIO_LOOP(gobject->main_loop)->task_thread)->thread_pool = gobject->thread_pool;
  AGS_THREAD_POOL(gobject->thread_pool)->parent = AGS_THREAD(AGS_AUDIO_LOOP(gobject->main_loop)->task_thread);

  list = g_atomic_pointer_get(&(AGS_THREAD_POOL(gobject->thread_pool)->returnable_thread));

  while(list != NULL){
    ags_thread_add_child(AGS_THREAD(AGS_AUDIO_LOOP(gobject->main_loop)->task_thread),
			 AGS_THREAD(list->data));

    list = list->next;
  }
}

void
ags_file_write_main(AgsFile *file, xmlNode *parent, GObject *ags_main)
{
  xmlNode *node, *child;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-main\0");

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", ags_main,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", ((~AGS_MAIN_CONNECTED) & (AGS_MAIN(ags_main)->flags))));

  xmlNewProp(node,
	     AGS_FILE_VERSION_PROP,
	     AGS_MAIN(ags_main)->version);

  xmlNewProp(node,
	     AGS_FILE_BUILD_ID_PROP,
	     AGS_MAIN(ags_main)->build_id);

  /* add to parent */
  xmlAddChild(parent,
	      node);

  /* child elements */
  ags_file_write_thread(file,
			node,
			AGS_THREAD(AGS_MAIN(ags_main)->main_loop));

  ags_file_write_thread_pool(file,
			     node,
			     AGS_THREAD_POOL(AGS_MAIN(ags_main)->thread_pool));

  ags_file_write_devout_list(file,
			     node,
			     AGS_MAIN(ags_main)->devout);

  ags_file_write_window(file,
			node,
			AGS_MAIN(ags_main)->window);
}

/**
 * ags_file_new:
 *
 * Creates an #AgsFile
 *
 * Returns: a new #AgsFile
 *
 * Since: 0.3
 */
AgsFile*
ags_file_new()
{
  AgsFile *file;

  file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				  NULL);

  return(file);
}
