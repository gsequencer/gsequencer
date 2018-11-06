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

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_marshal.h>

#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_launch.h>

#include <ags/thread/ags_thread_pool.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

#include <ags/i18n.h>

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

void ags_file_real_open(AgsFile *file,
			GError **error);
void ags_file_real_open_from_data(AgsFile *file,
				  gchar *data, guint length,
				  GError **error);
void ags_file_real_rw_open(AgsFile *file,
			   gboolean create,
			   GError **error);

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
  PROP_XML_DOC,
  PROP_APPLICATION_CONTEXT,
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

static pthread_mutex_t ags_file_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_file_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_file = 0;

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
					   "AgsFile",
					   &ags_file_info,
					   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_file);
  }

  return g_define_type_id__volatile;
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
  /**
   * AgsFile:filename:
   *
   * The assigned filename to open and read from.
   *
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("filename to read or write"),
				   i18n_pspec("The filename to read or write to"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsFile:encoding:
   *
   * The charset encoding to use.
   *
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("encoding",
				   i18n_pspec("encoding to use"),
				   i18n_pspec("The encoding of the XML document"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ENCODING,
				  param_spec);

  /**
   * AgsFile:audio-format:
   *
   * The format of embedded audio data.
   *
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("audio-format",
				   i18n_pspec("audio format to use"),
				   i18n_pspec("The audio format used by embedded audio"),
				   AGS_FILE_DEFAULT_AUDIO_FORMAT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_FORMAT,
				  param_spec);

  /**
   * AgsFile:audio-encoding:
   *
   * The encoding to use for embedding audio data.
   *
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("audio-encoding",
				   i18n_pspec("audio encoding to use"),
				   i18n_pspec("The audio encoding used by embedded audio"),
				   AGS_FILE_DEFAULT_AUDIO_ENCODING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_ENCODING,
				  param_spec);

  /**
   * AgsFile:xml-doc:
   *
   * The assigned xml-doc.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("xml-doc",
				    i18n_pspec("xml document of file"),
				    i18n_pspec("The xml document assigned with file"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_XML_DOC,
				  param_spec);

  /**
   * AgsFile:application-context:
   *
   * The application context assigned with.
   *
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("application-context",
				   i18n_pspec("application context of file"),
				   i18n_pspec("The application context to write to file"),
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
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

  /* signals */
  /**
   * AgsFile::open:
   * @file: the #AgsFile
   * @error: a #GError-struct pointer to return error
   * 
   * Open @file with appropriate filename.
   *
   * Since: 2.0.0
   */
  file_signals[OPEN] =
    g_signal_new("open",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, open),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);

  /**
   * AgsFile::open-from-data:
   * @file: the #AgsFile
   * @buffer: the buffer containing the file
   * @length: the buffer length
   * @error: a #GError-struct pointer to return error
   * 
   * Open @file from a buffer containing the file.
   *
   * Since: 2.0.0
   */
  file_signals[OPEN_FROM_DATA] =
    g_signal_new("open-from-data",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, open_from_data),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__STRING_UINT_POINTER,
		 G_TYPE_NONE, 3,
		 G_TYPE_STRING,
		 G_TYPE_UINT,
		 G_TYPE_POINTER);

  /**
   * AgsFile::open-from-data:
   * @file: the #AgsFile
   * @create: if %TRUE the file will be created if not exists
   * @error: a #GError-struct pointer to return error
   * 
   * Open @file in read-write mode.
   *
   * Since: 2.0.0
   */
  file_signals[RW_OPEN] =
    g_signal_new("rw-open",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, rw_open),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__BOOLEAN_POINTER,
		 G_TYPE_NONE, 2,
		 G_TYPE_BOOLEAN,
		 G_TYPE_POINTER);

  /**
   * AgsFile::write:
   * @file: the #AgsFile
   * 
   * Write XML Document to disk.
   *
   * Since: 2.0.0
   */
  file_signals[WRITE] =
    g_signal_new("write",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, write),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  file_signals[WRITE_CONCURRENT] =
    g_signal_new("write_concurrent",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, write_concurrent),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsFile::write-resolve:
   * @file: the #AgsFile
   *
   * Resolve references and generate thus XPath expressions just
   * before writing to disk.
   *
   * Since: 2.0.0
   */
  file_signals[WRITE_RESOLVE] =
    g_signal_new("write_resolve",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, write_resolve),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsFile::read:
   * @file: the #AgsFile
   *
   * Read a XML document from disk with specified filename.
   * 
   * Since: 2.0.0
   */
  file_signals[READ] =
    g_signal_new("read",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, read),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsFile::read-resolve:
   * @file: the #AgsFile
   *
   * Resolve XPath expressions to their counterpart the newly created
   * instances refering to.
   * 
   * Since: 2.0.0
   */
  file_signals[READ_RESOLVE] =
    g_signal_new("read_resolve",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, read_resolve),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsFile::read-start:
   * @file: the #AgsFile
   *
   * Hook after reading XML document to update or start the application.
   * 
   * Since: 2.0.0
   */
  file_signals[READ_START] =
    g_signal_new("read_start",
		 G_TYPE_FROM_CLASS(file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileClass, read_start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

GQuark
ags_file_error_quark()
{
  return(g_quark_from_static_string("ags-file-error-quark"));
}

void
ags_file_init(AgsFile *file)
{
  
  file->flags = 0;

  /* add file mutex */
  file->obj_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(file->obj_mutexattr);
  pthread_mutexattr_settype(file->obj_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(file->obj_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  
  file->obj_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(file->obj_mutex, file->obj_mutexattr);

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

  file->application_context = NULL;

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

  pthread_mutex_t *file_mutex;

  file = AGS_FILE(gobject);

  /* get file mutex */
  pthread_mutex_lock(ags_file_get_class_mutex());
  
  file_mutex = file->obj_mutex;
  
  pthread_mutex_unlock(ags_file_get_class_mutex());

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      pthread_mutex_lock(file_mutex);

      if(file->filename == filename){
	pthread_mutex_unlock(file_mutex);
	
	return;
      }

      if(file->filename != NULL){
	g_free(file->filename);
      }
      
      file->filename = g_strdup(filename);

      pthread_mutex_unlock(file_mutex);
    }
    break;
  case PROP_ENCODING:
    {
      gchar *encoding;

      encoding = g_value_get_string(value);

      pthread_mutex_lock(file_mutex);

      file->encoding = encoding;

      pthread_mutex_unlock(file_mutex);
    }
    break;
  case PROP_AUDIO_FORMAT:
    {
      gchar *audio_format;

      audio_format = g_value_get_string(value);

      pthread_mutex_lock(file_mutex);

      file->audio_format = audio_format;

      pthread_mutex_unlock(file_mutex);
    }
    break;
  case PROP_AUDIO_ENCODING:
    {
      gchar *audio_encoding;

      audio_encoding = g_value_get_string(value);

      pthread_mutex_lock(file_mutex);

      file->audio_encoding = audio_encoding;

      pthread_mutex_unlock(file_mutex);
    }
    break;
  case PROP_XML_DOC:
    {
      xmlDoc *doc;

      doc = (xmlDoc *) g_value_get_pointer(value);
      
      pthread_mutex_lock(file_mutex);

      file->doc = doc;

      pthread_mutex_unlock(file_mutex);
    }
    break;
  case PROP_APPLICATION_CONTEXT:
    {
      GObject *application_context;

      application_context = g_value_get_object(value);

      pthread_mutex_lock(file_mutex);

      if(file->application_context == application_context){
	pthread_mutex_unlock(file_mutex);

	return;
      }

      if(file->application_context != NULL){
	g_object_unref(file->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      file->application_context = application_context;

      pthread_mutex_unlock(file_mutex);
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

  pthread_mutex_t *file_mutex;

  file = AGS_FILE(gobject);

  /* get file mutex */
  pthread_mutex_lock(ags_file_get_class_mutex());
  
  file_mutex = file->obj_mutex;
  
  pthread_mutex_unlock(ags_file_get_class_mutex());

  switch(prop_id){
  case PROP_FILENAME:
    {
      pthread_mutex_lock(file_mutex);

      g_value_set_string(value, file->filename);

      pthread_mutex_unlock(file_mutex);
    }
    break;
  case PROP_ENCODING:
    {
      pthread_mutex_lock(file_mutex);

      g_value_set_string(value, file->encoding);

      pthread_mutex_unlock(file_mutex);
    }
    break;
  case PROP_AUDIO_FORMAT:
    {
      pthread_mutex_lock(file_mutex);

      g_value_set_string(value, file->audio_format);

      pthread_mutex_unlock(file_mutex);
    }
    break;
  case PROP_AUDIO_ENCODING:
    {
      pthread_mutex_lock(file_mutex);

      g_value_set_string(value, file->audio_encoding);

      pthread_mutex_unlock(file_mutex);
    }
    break;
  case PROP_XML_DOC:
    {
      pthread_mutex_lock(file_mutex);

      g_value_set_pointer(value, file->doc);

      pthread_mutex_unlock(file_mutex);
    }
    break;
  case PROP_APPLICATION_CONTEXT:
    {
      pthread_mutex_lock(file_mutex);

      g_value_set_object(value, file->application_context);

      pthread_mutex_unlock(file_mutex);
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

  /* file mutex */
  pthread_mutexattr_destroy(file->obj_mutexattr);
  free(file->obj_mutexattr);

  pthread_mutex_destroy(file->obj_mutex);
  free(file->obj_mutex);

  G_OBJECT_CLASS(ags_file_parent_class)->finalize(gobject);
}

/**
 * ags_file_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_file_get_class_mutex()
{
  return(&ags_file_class_mutex);
}

/**
 * ags_file_str2md5:
 * @content: the string buffer
 * @content_length: the length of the string
 *
 * Compute MD5 sums of a buffer.
 *
 * Returns: the md5 checksum
 *
 * Since: 2.0.0
 */
gchar*
ags_file_str2md5(gchar *content, guint content_length)
{
  GChecksum *checksum;
  gchar *str;

  str = g_compute_checksum_for_string(G_CHECKSUM_MD5,
				      content,
				      content_length);

  return(str);
}

/**
 * ags_file_add_id_ref:
 * @file: the @AgsFile
 * @id_ref: a reference
 *
 * Adds @id_ref to @file.
 * 
 * Since: 2.0.0
 */
void
ags_file_add_id_ref(AgsFile *file, GObject *id_ref)
{
  pthread_mutex_t *file_mutex;

  if(!AGS_IS_FILE(file) ||
     !AGS_IS_FILE_ID_REF(id_ref)){
    return;
  }

  /* get file mutex */
  pthread_mutex_lock(ags_file_get_class_mutex());
  
  file_mutex = file->obj_mutex;
  
  pthread_mutex_unlock(ags_file_get_class_mutex());

  /* add */
  pthread_mutex_lock(file_mutex);
  
  if(g_list_find(file->id_refs,
		 id_ref) == NULL){
    g_object_ref(id_ref);
    file->id_refs = g_list_prepend(file->id_refs,
				   id_ref);
  }

  pthread_mutex_unlock(file_mutex);
}

/**
 * ags_file_add_id_ref_by_node:
 * @file: the @AgsFile
 * @node: a XML node
 *
 * Find a reference by its XML node.
 * 
 * Returns: the matching #GObject
 *
 * Since: 2.0.0
 */
GObject*
ags_file_find_id_ref_by_node(AgsFile *file, xmlNode *node)
{
  AgsFileIdRef *file_id_ref;

  xmlNode *current_node;
  
  GList *start_list, *list;

  pthread_mutex_t *file_mutex;

  if(!AGS_IS_FILE(file) ||
     node == NULL){
    return(NULL);
  }

  /* get file mutex */
  pthread_mutex_lock(ags_file_get_class_mutex());
  
  file_mutex = file->obj_mutex;
  
  pthread_mutex_unlock(ags_file_get_class_mutex());

  /* find */
  file_id_ref = NULL;
  
  pthread_mutex_lock(file_mutex);

  list =
    start_list = g_list_copy(file->id_refs);

  pthread_mutex_unlock(file_mutex);

  while(list != NULL){
    g_object_get(list->data,
		 "node", &current_node,
		 NULL);

    if(current_node == node){
      file_id_ref = AGS_FILE_ID_REF(list->data);

      break;
    }

    list = list->next;
  }

  g_list_free(start_list);

  return(file_id_ref);
}

/**
 * ags_file_find_id_ref_by_xpath:
 * @file: the #AgsFile
 * @xpath: a XPath expression
 *
 * Lookup a reference by @xpath.
 * 
 * Returns: the matching #GObject
 *
 * Since: 2.0.0
 */
GObject*
ags_file_find_id_ref_by_xpath(AgsFile *file, gchar *xpath)
{
  AgsFileIdRef *file_id_ref;

  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;

  guint i;

  if(!AGS_IS_FILE(file) || xpath == NULL || !g_str_has_prefix(xpath, "xpath=")){
    g_message("invalid xpath: %s", xpath);

    return(NULL);
  }

  xpath = &(xpath[6]);

  /* Create xpath evaluation context */
  xpath_context = xmlXPathNewContext(file->doc);

  if(xpath_context == NULL) {
    g_warning("Error: unable to create new XPath context");

    return(NULL);
  }

  /* Evaluate xpath expression */
  xpath_object = xmlXPathEval(xpath, xpath_context);

  if(xpath_object == NULL) {
    g_warning("Error: unable to evaluate xpath expression \"%s\"", xpath);
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

  g_message("no xpath match: %s", xpath);
  
  return(NULL);
}

/**
 * ags_file_find_id_ref_by_reference:
 * @file: the #AgsFile
 * @ref: a %gpointer
 *
 * Find a reference matching @ref.
 * 
 * Returns: the matching #GObject
 *
 * Since: 2.0.0
 */
GObject*
ags_file_find_id_ref_by_reference(AgsFile *file, gpointer ref)
{
  AgsFileIdRef *file_id_ref;

  GList *start_list, *list;

  gchar *current_ref;
  
  pthread_mutex_t *file_mutex;

  if(!AGS_IS_FILE(file)){
    return(NULL);
  }  

  /* get file mutex */
  pthread_mutex_lock(ags_file_get_class_mutex());
  
  file_mutex = file->obj_mutex;
  
  pthread_mutex_unlock(ags_file_get_class_mutex());

  /* find */
  file_id_ref = NULL;
  
  pthread_mutex_lock(file_mutex);

  list =
    start_list = g_list_copy(file->id_refs);

  pthread_mutex_unlock(file_mutex);

  while(list != NULL){
    g_object_get(list->data,
		 "reference", &current_ref,
		 NULL);

    if(current_ref == ref){
      file_id_ref = AGS_FILE_ID_REF(list->data);

      break;
    }

    list = list->next;
  }

  g_list_free(start_list);

  return(file_id_ref);
}

/**
 * ags_file_add_lookup:
 * @file: the #AgsFile
 * @file_lookup: a #AgsFileLookup
 *
 * Add @file_lookup for later invoking.
 *
 * Since: 2.0.0
 */
void
ags_file_add_lookup(AgsFile *file, GObject *file_lookup)
{  
  pthread_mutex_t *file_mutex;

  if(!AGS_IS_FILE(file) || !AGS_IS_FILE_LOOKUP(file_lookup)){
    return;
  }

  /* get file mutex */
  pthread_mutex_lock(ags_file_get_class_mutex());
  
  file_mutex = file->obj_mutex;
  
  pthread_mutex_unlock(ags_file_get_class_mutex());

  /* add */
  pthread_mutex_lock(file_mutex);

  if(g_list_find(file->lookup,
		 file_lookup) == NULL){
    g_object_ref(G_OBJECT(file_lookup));
    
    file->lookup = g_list_prepend(file->lookup,
				  file_lookup);
  }

  pthread_mutex_unlock(file_mutex);
}

/**
 * ags_file_add_launch:
 * @file: the #AgsFile
 * @file_launch: a #AgsFileLaunch
 *
 * Add @file_launch for later invoking.
 *
 * Since: 2.0.0
 */
void
ags_file_add_launch(AgsFile *file, GObject *file_launch)
{
  pthread_mutex_t *file_mutex;

  if(!AGS_IS_FILE(file) || !AGS_IS_FILE_LAUNCH(file_launch)){
    return;
  }

  /* get file mutex */
  pthread_mutex_lock(ags_file_get_class_mutex());
  
  file_mutex = file->obj_mutex;
  
  pthread_mutex_unlock(ags_file_get_class_mutex());

  /* add */
  pthread_mutex_lock(file_mutex);

  if(g_list_find(file->launch,
		 file_launch) == NULL){
    g_object_ref(G_OBJECT(file_launch));

    file->launch = g_list_prepend(file->launch,
				  file_launch);
  }
  
  pthread_mutex_unlock(file_mutex);
}

void
ags_file_real_open(AgsFile *file,
		   GError **error)
{
  xmlDoc *doc;
  
  pthread_mutex_t *file_mutex;

  if(!AGS_IS_FILE(file)){
    return;
  }

  /* get file mutex */
  pthread_mutex_lock(ags_file_get_class_mutex());
  
  file_mutex = file->obj_mutex;
  
  pthread_mutex_unlock(ags_file_get_class_mutex());

  /* parse the file and get the DOM */
  doc = xmlReadFile(file->filename, NULL, 0);

  pthread_mutex_lock(file_mutex);

  file->doc = doc;
  
  if(doc == NULL){
    g_warning("ags_file.c - failed to read XML document %s", file->filename);

    if(error != NULL){
      g_set_error(error,
		  AGS_FILE_ERROR,
		  AGS_FILE_ERROR_PARSER_FAILURE,
		  "unable to parse document: %s\n",
		  file->filename);
    }
  }else{
    /*Get the root element node */
    file->root_node = xmlDocGetRootElement(file->doc);
  }

  pthread_mutex_unlock(file_mutex);
}

/**
 * ags_file_open:
 * @file: the #AgsFile
 * @error: the return location for errors
 *
 * Opens the file specified by :filename property.
 *
 * Since: 2.0.0 
 */
void
ags_file_open(AgsFile *file,
	      GError **error)
{
  g_return_if_fail(AGS_IS_FILE(file));

  g_object_ref(G_OBJECT(file));
  g_signal_emit(G_OBJECT(file),
		file_signals[OPEN], 0,
		error);
  g_object_unref(G_OBJECT(file));
}

void
ags_file_real_open_from_data(AgsFile *file,
			     gchar *data, guint length,
			     GError **error)
{
  xmlDoc *doc;
  
  pthread_mutex_t *file_mutex;

  if(!AGS_IS_FILE(file)){
    return;
  }

  /* get file mutex */
  pthread_mutex_lock(ags_file_get_class_mutex());
  
  file_mutex = file->obj_mutex;
  
  pthread_mutex_unlock(ags_file_get_class_mutex());

  /* parse */
  doc = xmlReadMemory(data, length, file->filename, NULL, 0);

  pthread_mutex_lock(file_mutex);

  file->doc = doc;

  if(file->doc == NULL){
    g_warning("ags_file.c - failed to read XML document %s", file->filename);

    if(error != NULL){
      g_set_error(error,
		  AGS_FILE_ERROR,
		  AGS_FILE_ERROR_PARSER_FAILURE,
		  "unable to parse document from data: %s\n",
		  file->filename);
    }
  }else{
    /*Get the root element node */
    file->root_node = xmlDocGetRootElement(file->doc);
  }

  pthread_mutex_unlock(file_mutex);
}

/**
 * ags_file_open_from_data:
 * @file: the #AgsFile
 * @data: a buffer containing the XML document
 * @length: the buffer length
 * @error: the return location for errors
 *
 * Opens the file provided by @data.
 *
 * Since: 2.0.0 
 */
void
ags_file_open_from_data(AgsFile *file,
			gchar *data, guint length,
			GError **error)
{
  g_return_if_fail(AGS_IS_FILE(file));

  g_object_ref(G_OBJECT(file));
  g_signal_emit(G_OBJECT(file),
		file_signals[OPEN_FROM_DATA], 0,
		data, length,
		error);
  g_object_unref(G_OBJECT(file));
}

void
ags_file_real_rw_open(AgsFile *file,
		      gboolean create,
		      GError **error)
{
  pthread_mutex_t *file_mutex;

  if(!AGS_IS_FILE(file)){
    return;
  }

  /* get file mutex */
  pthread_mutex_lock(ags_file_get_class_mutex());
  
  file_mutex = file->obj_mutex;
  
  pthread_mutex_unlock(ags_file_get_class_mutex());

  /* create */
  pthread_mutex_lock(file_mutex);

  file->out = fopen(file->filename, "w+");

  file->doc = xmlNewDoc("1.0");
  file->root_node = xmlNewNode(NULL, "ags");
  xmlDocSetRootElement(file->doc, file->root_node);

  pthread_mutex_unlock(file_mutex);
}

/**
 * ags_file_rw_open:
 * @file: the #AgsFile
 * @create: if %TRUE create the file as needed
 * @error: the return location for errors
 *
 * Opens the file specified by :filename property in read-write mode.
 *
 * Since: 2.0.0 
 */
void
ags_file_rw_open(AgsFile *file,
		 gboolean create,
		 GError **error)
{
  g_return_if_fail(AGS_IS_FILE(file));

  g_object_ref(G_OBJECT(file));
  g_signal_emit(G_OBJECT(file),
		file_signals[RW_OPEN], 0,
		create,
		error);
  g_object_unref(G_OBJECT(file));
}

/**
 * ags_file_open_filename:
 * @file: the #AgsFile
 * @filename: a path
 *
 * Opens the file specified by @filename property.
 *
 * Since: 2.0.0 
 */
void
ags_file_open_filename(AgsFile *file,
		       gchar *filename)
{
  gchar *current_filename;
  
  GError *error;

  pthread_mutex_t *file_mutex;
  
  if(!AGS_IS_FILE(file)){
    return;
  }

  /* get file mutex */
  pthread_mutex_lock(ags_file_get_class_mutex());
  
  file_mutex = file->obj_mutex;
  
  pthread_mutex_unlock(ags_file_get_class_mutex());

  /* check close */
  pthread_mutex_lock(file_mutex);

  current_filename = file->filename;
  
  pthread_mutex_unlock(file_mutex);

  if(current_filename != NULL){
    ags_file_close(file);
  }

  error = NULL;
  g_object_set(file,
	       "filename", filename,
	       NULL);
  ags_file_open(file,
		&error);
}

/**
 * ags_file_close:
 * @file: the #AgsFile
 *
 * Closes @file.
 *
 * Since: 2.0.0 
 */
void
ags_file_close(AgsFile *file)
{
  pthread_mutex_t *file_mutex;

  if(!AGS_IS_FILE(file)){
    return;
  }

  /* get file mutex */
  pthread_mutex_lock(ags_file_get_class_mutex());
  
  file_mutex = file->obj_mutex;
  
  pthread_mutex_unlock(ags_file_get_class_mutex());

  /* close */
  pthread_mutex_lock(file_mutex);

  if(file->out != NULL){
    fclose(file->out);
  }

  /* free the document */
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

  pthread_mutex_unlock(file_mutex);
}

void
ags_file_real_write(AgsFile *file)
{
  AgsApplicationContext *application_context;

  GList *list;

  int size;

  pthread_mutex_t *file_mutex;

  /* get file mutex */
  pthread_mutex_lock(ags_file_get_class_mutex());
  
  file_mutex = file->obj_mutex;
  
  pthread_mutex_unlock(ags_file_get_class_mutex());

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

  /* write application context */
  ags_file_write_application_context(file,
				     file->root_node,
				     file->application_context);

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
  //  xmlSaveFormatFileEnc(file->filename, file->doc, "UTF-8", 1);
  xmlDocDumpFormatMemoryEnc(file->doc, &(file->buffer), &size, file->encoding, TRUE);

  fwrite(file->buffer, size, sizeof(xmlChar), file->out);
  fflush(file->out);
}

/**
 * ags_file_write:
 * @file: the #AgsFile
 *
 * Write the XML document to disk.
 *
 * Since: 2.0.0
 */
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
  AgsApplicationContext *application_context;
  AgsThread *main_loop, *gui_thread, *task_thread;
  xmlNode *root_node;
  FILE *file_out;
  GList *list;
  xmlChar *buffer;
  int size;

  xmlNode *parent, *node, *child;
  gchar *id;

  main_loop = (AgsThread *) AGS_APPLICATION_CONTEXT(file->application_context)->main_loop;
  //gui_thread = AGS_AUDIO_LOOP(main_loop)->gui_thread;
  //task_thread = AGS_AUDIO_LOOP(main_loop)->task_thread;

  application_context = (AgsApplicationContext *) file->application_context;

  file->doc = xmlNewDoc("1.0");
  root_node = xmlNewNode(NULL, "ags");
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
  //TODO:JK: implement me

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
  //  xmlSaveFormatFileEnc(file->filename, file->doc, "UTF-8", 1);
  xmlDocDumpFormatMemoryEnc(file->doc, &buffer, &size, file->encoding, TRUE);

  file_out = fopen(file->filename, "w+");
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
  GList *start_list, *list;

  pthread_mutex_t *file_mutex;

  /* get file mutex */
  pthread_mutex_lock(ags_file_get_class_mutex());
  
  file_mutex = file->obj_mutex;
  
  pthread_mutex_unlock(ags_file_get_class_mutex());

  /* resolve */
  pthread_mutex_lock(file_mutex);

  file->lookup = g_list_prepend(file->lookup,
				NULL);
  
  list =
    start_list = g_list_copy(file->lookup);

  pthread_mutex_unlock(file_mutex);

  while(list != NULL){
    ags_file_lookup_resolve(AGS_FILE_LOOKUP(list->data));

    list = list->next;
  }

  g_list_free(start_list);
}

/**
 * ags_file_write_resolve:
 * @file: the #AgsFile
 *
 * Resolve references to XPath expressions.
 *
 * Since: 2.0.0
 */
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
  AgsApplicationContext *application_context;

  xmlNode *root_node, *child;

  pid_t pid_num;

  pthread_mutex_t *file_mutex;

  /* get file mutex */
  pthread_mutex_lock(ags_file_get_class_mutex());
  
  file_mutex = file->obj_mutex;
  
  pthread_mutex_unlock(ags_file_get_class_mutex());

  
  /* child elements */
  pthread_mutex_lock(file_mutex);

  root_node = file->root_node;

  child = root_node->children;
  application_context = (AgsApplicationContext *) file->application_context;
  
  pthread_mutex_unlock(file_mutex);

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp("ags-main",
		     child->name,
		     9)){
	ags_file_read_application_context(file,
					  child,
					  (GObject **) &application_context);
      }else if(!xmlStrncmp("ags-embedded-audio-list",
			   child->name,
			   24)){
	//TODO:JK: implement me
      }else if(!xmlStrncmp("ags-file-link-list",
			   child->name,
			   19)){
	//TODO:JK: implement me
      }else if(!xmlStrncmp("ags-history",
			   child->name,
			   12)){
	//TODO:JK: implement me
      }
    }

    child = child->next;
  }

  /* resolve */
  ags_file_read_resolve(file);
  
  g_message("XML file resolved");

  ags_connectable_connect(AGS_CONNECTABLE(application_context));

  g_message("XML file connected");

  /* start */
  ags_file_read_start(file);
}

/**
 * ags_file_read:
 * @file: the #AgsFile
 *
 * Read XML document from disk.
 *
 * Since: 2.0.0
 */
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
  GList *start_list, *list;

  pthread_mutex_t *file_mutex;

  /* get file mutex */
  pthread_mutex_lock(ags_file_get_class_mutex());
  
  file_mutex = file->obj_mutex;
  
  pthread_mutex_unlock(ags_file_get_class_mutex());

  /* resolve */
  pthread_mutex_lock(file_mutex);

  file->lookup = g_list_prepend(file->lookup,
				NULL);
  list =
    start_list = g_list_reverse(g_list_copy(file->lookup));
  
  pthread_mutex_unlock(file_mutex);

  while(list != NULL){
    ags_file_lookup_resolve(AGS_FILE_LOOKUP(list->data));

    list = list->next;
  }

  g_list_free(start_list);
}

/**
 * ags_file_resolve:
 * @file: the #AgsFile
 *
 * Resolve XPath expressions to references.
 *
 * Since: 2.0.0
 */
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
  GList *start_list, *list;

  pthread_mutex_t *file_mutex;

  /* get file mutex */
  pthread_mutex_lock(ags_file_get_class_mutex());
  
  file_mutex = file->obj_mutex;
  
  pthread_mutex_unlock(ags_file_get_class_mutex());

  /* start */
  pthread_mutex_lock(file_mutex);

  list =
    start_list = g_list_reverse(g_list_copy(file->launch));

  pthread_mutex_unlock(file_mutex);
  
  while(list != NULL){
    ags_file_launch_start(AGS_FILE_LAUNCH(list->data));

    list = list->next;
  }

  g_list_free(start_list);
}

/**
 * ags_file_read_start:
 * @file: the #AgsFile
 *
 * Update or start the application.
 *
 * Since: 2.0.0
 */
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
ags_file_read_config(AgsFile *file, xmlNode *node, GObject **config)
{
  AgsConfig *gobject;

  gchar *id;

  char *buffer;
  gsize buffer_length;

  gobject = (AgsConfig *) *config;
  gobject->version = xmlGetProp(node,
				AGS_FILE_VERSION_PROP);

  gobject->build_id = xmlGetProp(node,
				 AGS_FILE_BUILD_ID_PROP);

  buffer = xmlNodeGetContent(node);
  buffer_length = xmlStrlen(buffer);

  ags_config_load_from_data(gobject,
			    buffer, buffer_length);
}

void
ags_file_write_config(AgsFile *file, xmlNode *parent, GObject *config)
{
  xmlNode *node;
  xmlNode *cdata;

  gchar *id;
  char *buffer;
  gsize buffer_length;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-config");

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", config,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_VERSION_PROP,
	     AGS_CONFIG(config)->version);

  xmlNewProp(node,
	     AGS_FILE_BUILD_ID_PROP,
	     AGS_CONFIG(config)->build_id);

  xmlAddChild(parent,
	      node);

  /* cdata */
  ags_config_to_data(AGS_CONFIG(config),
		     &buffer,
		     &buffer_length);

  cdata = xmlNewCDataBlock(file->doc,
			   buffer,
			   buffer_length);

  xmlAddChild(node,
	      cdata);
}

void
ags_file_read_application_context(AgsFile *file, xmlNode *node, GObject **application_context)
{
  GList *list;
  gchar *context;

  context = xmlGetProp(node,
		       "context");

  AGS_APPLICATION_CONTEXT_GET_CLASS(file->application_context)->register_types(AGS_APPLICATION_CONTEXT(file->application_context));
  AGS_APPLICATION_CONTEXT_GET_CLASS(file->application_context)->read(file,
								     node,
								     application_context);
}

void
ags_file_write_application_context(AgsFile *file, xmlNode *parent, GObject *application_context)
{
  AGS_APPLICATION_CONTEXT_GET_CLASS(file->application_context)->register_types(AGS_APPLICATION_CONTEXT(file->application_context));
  AGS_APPLICATION_CONTEXT_GET_CLASS(application_context)->write(file,
								parent,
								application_context);
}

/**
 * ags_file_new:
 *
 * Creates an #AgsFile
 *
 * Returns: a new #AgsFile
 *
 * Since: 2.0.0
 */
AgsFile*
ags_file_new()
{
  AgsFile *file;

  file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				  NULL);

  return(file);
}
