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

#include <ags/X/file/ags_simple_file.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_marshal.h>

#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_launch.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_editor.h>
#include <ags/X/ags_automation_window.h>
#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>
#include <ags/X/ags_line_member.h>
#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bulk.h>
#include <ags/X/ags_bulk_member.h>
#include <ags/X/ags_effect_pad.h>
#include <ags/X/ags_effect_line.h>

#include <ags/X/machine/ags_panel.h>
#include <ags/X/machine/ags_mixer.h>
#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_ffplayer.h>
#include <ags/X/machine/ags_ladspa_bridge.h>
#include <ags/X/machine/ags_dssi_bridge.h>
#include <ags/X/machine/ags_lv2_bridge.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

void ags_simple_file_class_init(AgsSimpleFileClass *simple_file);
void ags_simple_file_init(AgsSimpleFile *simple_file);
void ags_simple_file_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_simple_file_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_simple_file_finalize(GObject *gobject);

void ags_simple_file_real_open(AgsSimpleFile *simple_file,
			       GError **error);
void ags_simple_file_real_open_from_data(AgsSimpleFile *simple_file,
					 gchar *data, guint length,
					 GError **error);
void ags_simple_file_real_rw_open(AgsSimpleFile *simple_file,
				  gboolean create,
				  GError **error);

void ags_simple_file_real_write(AgsSimpleFile *simple_file);
void ags_simple_file_real_write_resolve(AgsSimpleFile *simple_file);

void ags_simple_file_real_read(AgsSimpleFile *simple_file);
void ags_simple_file_real_read_resolve(AgsSimpleFile *simple_file);
void ags_simple_file_real_read_start(AgsSimpleFile *simple_file);

void ags_simple_file_read_config(AgsSimpleFile *simple_file, xmlNode *node, AgsConfig **config);
void ags_simple_file_read_property_list(AgsSimpleFile *simple_file, xmlNode *node, GList **property);
void ags_simple_file_read_property(AgsSimpleFile *simple_file, xmlNode *node, GParameter **property);
void ags_simple_file_read_window(AgsSimpleFile *simple_file, xmlNode *node, AgsWindow **window);
void ags_simple_file_read_window_launch(AgsFileLaunch *file_launch,
					AgsWindow *window);
void ags_simple_file_read_machine_list(AgsSimpleFile *simple_file, xmlNode *node, GList **machine);
void ags_simple_file_read_machine(AgsSimpleFile *simple_file, xmlNode *node, AgsMachine **machine);
void ags_simple_file_read_machine_launch(AgsFileLaunch *file_launch,
					 AgsMachine *machine);
void ags_simple_file_read_pad_list(AgsSimpleFile *simple_file, xmlNode *node, GList **pad);
void ags_simple_file_read_pad(AgsSimpleFile *simple_file, xmlNode *node, AgsPad **pad);
void ags_simple_file_read_pad_launch(AgsFileLaunch *file_launch,
				     AgsPad *pad);
void ags_simple_file_read_line_list(AgsSimpleFile *simple_file, xmlNode *node, GList **line);
void ags_simple_file_read_line(AgsSimpleFile *simple_file, xmlNode *node, AgsLine **line);
void ags_simple_file_read_line_launch(AgsFileLaunch *file_launch,
				      AgsLine *line);
void ags_simple_file_read_editor(AgsSimpleFile *simple_file, xmlNode *node, AgsEditor **editor);
void ags_simple_file_read_automation_editor(AgsSimpleFile *simple_file, xmlNode *node, AgsAutomationEditor **automation_editor);
void ags_simple_file_read_notation_list(AgsSimpleFile *simple_file, xmlNode *node, GList **notation);
void ags_simple_file_read_notation(AgsSimpleFile *simple_file, xmlNode *node, AgsNotation **notation);
void ags_simple_file_read_automation_list(AgsSimpleFile *simple_file, xmlNode *node, GList **automation);
void ags_simple_file_read_automation(AgsSimpleFile *simple_file, xmlNode *node, AgsAutomation **automation);

xmlNode* ags_simple_file_write_window(AgsSimpleFile *simple_file, xmlNode *parent, AgsConfig *config);
xmlNode* ags_simple_file_write_property_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *property);
xmlNode* ags_simple_file_write_property(AgsSimpleFile *simple_file, xmlNode *parent, GParameter *property);
xmlNode* ags_simple_file_write_window(AgsSimpleFile *simple_file, xmlNode *parent, AgsWindow *window);
xmlNode* ags_simple_file_write_machine_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *machine);
xmlNode* ags_simple_file_write_machine(AgsSimpleFile *simple_file, xmlNode *parent, AgsMachine *machine);
xmlNode* ags_simple_file_write_pad_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *pad);
xmlNode* ags_simple_file_write_pad(AgsSimpleFile *simple_file, xmlNode *parent, AgsPad *pad);
xmlNode* ags_simple_file_write_line_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *line);
xmlNode* ags_simple_file_write_line(AgsSimpleFile *simple_file, xmlNode *parent, AgsLine *line);
xmlNode* ags_simple_file_write_editor(AgsSimpleFile *simple_file, xmlNode *parent, AgsEditor *editor);
xmlNode* ags_simple_file_write_automation_editor(AgsSimpleFile *simple_file, xmlNode *parent, AgsAutomationEditor *automation_editor);
xmlNode* ags_simple_file_write_notation_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *notation);
xmlNode* ags_simple_file_write_notation(AgsSimpleFile *simple_file, xmlNode *parent, AgsNotation *notation);
xmlNode* ags_simple_file_write_automation_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *automation);
xmlNode* ags_simple_file_write_automation(AgsSimpleFile *simple_file, xmlNode *parent, AgsAutomation *automation);

/**
 * SECTION:ags_file
 * @short_description: read/write XML file
 * @title: AgsSimpleFile
 * @section_id:
 * @include: ags/file/ags_file.h
 *
 * The #AgsSimpleFile is an object to read or write files using XML. It
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
  WRITE_RESOLVE,
  READ,
  READ_RESOLVE,
  READ_START,
  LAST_SIGNAL,
};

static gpointer ags_simple_file_parent_class = NULL;
static guint simple_file_signals[LAST_SIGNAL] = { 0 };

GType
ags_simple_file_get_type(void)
{
  static GType ags_type_simple_file = 0;

  if(!ags_type_simple_file){
    static const GTypeInfo ags_simple_file_info = {
      sizeof (AgsSimpleFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_simple_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSimpleFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_simple_file_init,
    };

    ags_type_simple_file = g_type_register_static(G_TYPE_OBJECT,
						  "AgsSimpleFile\0",
						  &ags_simple_file_info,
						  0);
  }

  return(ags_type_simple_file);
}

void
ags_simple_file_class_init(AgsSimpleFileClass *simple_file)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_simple_file_parent_class = g_type_class_peek_parent(simple_file);

  /* GObjectClass */
  gobject = (GObjectClass *) simple_file;

  gobject->get_property = ags_simple_file_get_property;
  gobject->set_property = ags_simple_file_set_property;

  gobject->finalize = ags_simple_file_finalize;

  /* properties */
  /**
   * AgsSimpleFile:filename:
   *
   * The assigned filename to open and read from.
   *
   * Since: w0.7.25
   */
  param_spec = g_param_spec_string("filename\0",
				   "filename to read or write\0",
				   "The filename to read or write to.\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsSimpleFile:encoding:
   *
   * The charset encoding to use.
   *
   * Since: 0.7.25
   */
  param_spec = g_param_spec_string("encoding\0",
				   "encoding to use\0",
				   "The encoding of the XML document.\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ENCODING,
				  param_spec);

  /**
   * AgsSimpleFile:audio-format:
   *
   * The format of embedded audio data.
   *
   * Since: 0.7.25
   */
  param_spec = g_param_spec_string("audio-format\0",
				   "audio format to use\0",
				   "The audio format used to embedded audio.\0",
				   AGS_SIMPLE_FILE_DEFAULT_AUDIO_FORMAT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_FORMAT,
				  param_spec);

  /**
   * AgsSimpleFile:audio-encoding:
   *
   * The encoding to use for embedding audio data.
   *
   * Since: 0.7.25
   */
  param_spec = g_param_spec_string("audio-encoding\0",
				   "audio encoding to use\0",
				   "The audio encoding used to embedded audio.\0",
				   AGS_SIMPLE_FILE_DEFAULT_AUDIO_ENCODING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_ENCODING,
				  param_spec);

  /**
   * AgsSimpleFile:xml-doc:
   *
   * The assigned xml-doc.
   * 
   * Since: 0.7.25
   */
  param_spec = g_param_spec_pointer("xml-doc\0",
				    "xml document of file\0",
				    "The xml document assigned with file\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_XML_DOC,
				  param_spec);

  /**
   * AgsSimpleFile:application-context:
   *
   * The application context assigned with.
   *
   * Since: 0.7.25
   */
  param_spec = g_param_spec_object("application-context\0",
				   "application context of file\0",
				   "The application context to write to file.\0",
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /* AgsSimpleFileClass */
  simple_file->open = ags_simple_file_real_open;
  simple_file->rw_open = ags_simple_file_real_rw_open;
  simple_file->open_from_data = ags_simple_file_real_open_from_data;

  simple_file->write = ags_simple_file_real_write;
  simple_file->write_resolve = ags_simple_file_real_write_resolve;
  simple_file->read = ags_simple_file_real_read;
  simple_file->read_resolve = ags_simple_file_real_read_resolve;
  simple_file->read_start = ags_simple_file_real_read_start;

  /* signals */
  /**
   * AgsSimpleFile::open:
   * @simple_file: the #AgsSimpleFile
   * @error: a #GError-struct pointer to return error
   * 
   * Open @simple_file with appropriate filename.
   *
   * Since: 0.7.25
   */
  simple_file_signals[OPEN] =
    g_signal_new("open\0",
		 G_TYPE_FROM_CLASS(simple_file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSimpleFileClass, open),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);

  /**
   * AgsSimpleFile::open-from-data:
   * @simple_file: the #AgsSimpleFile
   * @buffer: the buffer containing the file
   * @length: the buffer length
   * @error: a #GError-struct pointer to return error
   * 
   * Open @simple_file from a buffer containing the file.
   *
   * Since: 0.7.25
   */
  simple_file_signals[OPEN_FROM_DATA] =
    g_signal_new("open-from-data\0",
		 G_TYPE_FROM_CLASS(simple_file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSimpleFileClass, open_from_data),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__STRING_UINT_POINTER,
		 G_TYPE_NONE, 3,
		 G_TYPE_STRING,
		 G_TYPE_UINT,
		 G_TYPE_POINTER);

  /**
   * AgsSimpleFile::open-from-data:
   * @simple_file: the #AgsSimpleFile
   * @create: if %TRUE the file will be created if not exists
   * @error: a #GError-struct pointer to return error
   * 
   * Open @simple_file in read-write mode.
   *
   * Since: 0.7.25
   */
  simple_file_signals[RW_OPEN] =
    g_signal_new("rw-open\0",
		 G_TYPE_FROM_CLASS(simple_file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSimpleFileClass, rw_open),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__BOOLEAN_POINTER,
		 G_TYPE_NONE, 2,
		 G_TYPE_BOOLEAN,
		 G_TYPE_POINTER);

  /**
   * AgsSimpleFile::write:
   * @simple_file: the #AgsSimpleFile
   * 
   * Write XML Document to disk.
   *
   * Since: 0.7.25
   */
  simple_file_signals[WRITE] =
    g_signal_new("write\0",
		 G_TYPE_FROM_CLASS(simple_file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSimpleFileClass, write),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsSimpleFile::write-resolve:
   * @simple_file: the #AgsSimpleFile
   *
   * Resolve references and generate thus XPath expressions just
   * before writing to disk.
   *
   * Since: 0.7.25
   */
  simple_file_signals[WRITE_RESOLVE] =
    g_signal_new("write_resolve\0",
		 G_TYPE_FROM_CLASS(simple_file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSimpleFileClass, write_resolve),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsSimpleFile::read:
   * @simple_file: the #AgsSimpleFile
   *
   * Read a XML document from disk with specified simple_filename.
   * 
   * Since: 0.7.25
   */
  simple_file_signals[READ] =
    g_signal_new("read\0",
		 G_TYPE_FROM_CLASS(simple_file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSimpleFileClass, read),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsSimpleFile::read-resolve:
   * @simple_file: the #AgsSimpleFile
   *
   * Resolve XPath expressions to their counterpart the newly created
   * instances refering to.
   * 
   * Since: 0.7.25
   */
  simple_file_signals[READ_RESOLVE] =
    g_signal_new("read_resolve\0",
		 G_TYPE_FROM_CLASS(simple_file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSimpleFileClass, read_resolve),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsSimpleFile::read-start:
   * @simple_file: the #AgsSimpleFile
   *
   * Hook after reading XML document to update or start the application.
   * 
   * Since: 0.7.25
   */
  simple_file_signals[READ_START] =
    g_signal_new("read_start\0",
		 G_TYPE_FROM_CLASS(simple_file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSimpleFileClass, read_start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

GQuark
ags_simple_file_error_quark()
{
  return(g_quark_from_static_string("ags-simple-file-error-quark\0"));
}

void
ags_simple_file_init(AgsSimpleFile *simple_file)
{
  simple_file->flags = 0;

  simple_file->out = NULL;
  simple_file->buffer = NULL;

  simple_file->filename = NULL;
  simple_file->encoding = AGS_SIMPLE_FILE_DEFAULT_ENCODING;
  simple_file->dtd = AGS_SIMPLE_FILE_DEFAULT_DTD;

  simple_file->audio_format = AGS_SIMPLE_FILE_DEFAULT_AUDIO_FORMAT;
  simple_file->audio_encoding = AGS_SIMPLE_FILE_DEFAULT_AUDIO_ENCODING;

  simple_file->doc = NULL;

  simple_file->id_refs = NULL;
  simple_file->lookup = NULL;
  simple_file->launch = NULL;

  simple_file->application_context = NULL;
}

void
ags_simple_file_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsSimpleFile *simple_file;

  simple_file = AGS_SIMPLE_FILE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *simple_filename;

      filename = g_value_get_string(value);

      if(simple_file->filename == filename){
	return;
      }

      if(simple_file->filename != NULL){
	g_free(simple_file->filename);
      }
      
      simple_file->filename = g_strdup(filename);
    }
    break;
  case PROP_ENCODING:
    {
      gchar *encoding;

      encoding = g_value_get_string(value);

      simple_file->encoding = encoding;
    }
    break;
  case PROP_AUDIO_FORMAT:
    {
      gchar *audio_format;

      audio_format = g_value_get_string(value);

      simple_file->audio_format = audio_format;
    }
    break;
  case PROP_AUDIO_ENCODING:
    {
      gchar *audio_encoding;

      audio_encoding = g_value_get_string(value);

      simple_file->audio_encoding = audio_encoding;
    }
    break;
  case PROP_XML_DOC:
    {
      xmlDoc *doc;

      doc = (xmlDoc *) g_value_get_pointer(value);
      
      simple_file->doc = doc;
    }
    break;
  case PROP_APPLICATION_CONTEXT:
    {
      GObject *application_context;

      application_context = g_value_get_object(value);

      if(simple_file->application_context == application_context){
	return;
      }

      if(simple_file->application_context != NULL){
	g_object_unref(simple_file->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      simple_file->application_context = application_context;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_simple_file_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsSimpleFile *simple_file;

  simple_file = AGS_SIMPLE_FILE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, simple_file->filename);
    }
    break;
  case PROP_ENCODING:
    {
      g_value_set_string(value, simple_file->encoding);
    }
    break;
  case PROP_AUDIO_FORMAT:
    {
      g_value_set_string(value, simple_file->audio_format);
    }
    break;
  case PROP_AUDIO_ENCODING:
    {
      g_value_set_string(value, simple_file->audio_encoding);
    }
    break;
  case PROP_XML_DOC:
    {
      g_value_set_pointer(value, simple_file->doc);
    }
    break;
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, simple_file->application_context);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_simple_file_finalize(GObject *gobject)
{
  //TODO:JK: implement me
}

gchar*
ags_simple_file_str2md5(gchar *content, guint content_length)
{
  GChecksum *checksum;
  gchar *str;

  str = g_compute_checksum_for_string(G_CHECKSUM_MD5,
				      content,
				      content_length);

  return(str);
}

void
ags_simple_file_add_id_ref(AgsSimpleFile *simple_file, GObject *id_ref)
{
  if(simple_file == NULL ||
     id_ref == NULL){
    return;
  }
  
  g_object_ref(id_ref);
  simple_file->id_refs = g_list_prepend(simple_file->id_ref,
					id_ref);
}

GObject*
ags_simple_file_find_id_ref_by_node(AgsSimpleFile *simple_file, xmlNode *node)
{
  AgsSimpleFileIdRef *simple_file_id_ref;
  GList *list;

  if(simple_file == NULL ||
     node == NULL){
    return(NULL);
  }
  
  list = file->id_ref;

  while(list != NULL){
    file_id_ref = AGS_FILE_ID_REF(list->data);

    if(file_id_ref->node == node){
      return((GObject *) file_id_ref);
    }

    list = list->next;
  }

  return(NULL);
}

GList*
ags_simple_file_find_id_ref_by_xpath(AgsSimpleFile *simple_file, gchar *xpath)
{
  AgsSimpleFileIdRef *simple_file_id_ref;

  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;

  GList *list;
  
  guint i;

  if(simple_file == NULL || xpath == NULL || !g_str_has_prefix(xpath, "xpath=\0")){
    g_message("invalid xpath: %s", xpath);

    return(NULL);
  }

  xpath = &(xpath[6]);

  /* Create xpath evaluation context */
  xpath_context = xmlXPathNewContext(simple_file->doc);

  if(xpath_context == NULL) {
    g_warning("Error: unable to create new XPath context\0");

    return(NULL);
  }

  /* Evaluate xpath expression */
  xpath_object = xmlXPathEval(xpath, xpath_context);

  if(xpath_object == NULL) {
    g_warning("Error: unable to evaluate xpath expression \"%s\"\0", xpath);
    xmlXPathFreeContext(xpath_context); 

    return(NULL);
  }

  node = xpath_object->nodesetval->nodeTab;

  list = NULL;
  
  for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
    if(node[i]->type == XML_ELEMENT_NODE){
      list = g_list_prepend(list,
			    ags_simple_file_find_id_ref_by_node(simple_file,
								node[i])));
  }
}

if(list == NULL){
  g_message("no xpath match: %s\0", xpath);
 }else{
  list = g_list_reverse(list);
 }
  
return(list);
}

GObject*
ags_simple_file_find_id_ref_by_reference(AgsSimpleFile *simple_file, gpointer ref)
{
  AgsSimpleFileIdRef *simple_file_id_ref;
  
  GList *list;
  GList *ref_list;
  
  if(simple_file == NULL || ref == NULL){
    return(NULL);
  }

  list = simple_file->id_refs;
  ref_list = NULL;
  
  while(list != NULL){
    file_id_ref = AGS_FILE_ID_REF(list->data);

    if(file_id_ref->ref == ref){
      ref_list = g_list_prepend(ref_list,
				file_id_ref);
    }

    list = list->next;
  }

  return(ref_list);
}

void
ags_simple_file_add_lookup(AgsSimpleFile *simple_file, GObject *simple_file_lookup)
{
  if(simple_file == NULL || file_lookup == NULL){
    return;
  }

  g_object_ref(G_OBJECT(file_lookup));

  simple_file->lookup = g_list_prepend(simple_file->lookup,
				       file_lookup);
}

void
ags_simple_file_add_launch(AgsSimpleFile *simple_file, GObject *simple_file_launch)
{
  if(simple_file == NULL || file_launch == NULL){
    return;
  }

  g_object_ref(G_OBJECT(file_launch));

  simple_file->launch = g_list_prepend(simple_file->launch,
				       file_launch);
}

void
ags_simple_file_real_open(AgsSimpleFile *simple_file,
			  GError **error)
{
  if(simple_file == NULL){
    return;
  }

  /* parse the file and get the DOM */
  simple_file->doc = xmlReadFile(simple_file->filename, NULL, 0);

  if(simple_file->doc == NULL){
    g_warning("ags_simple_file.c - failed to read XML document %s\0", simple_file->filename);

    if(error != NULL){
      g_set_error(error,
		  AGS_SIMPLE_FILE_ERROR,
		  AGS_SIMPLE_FILE_ERROR_PARSER_FAILURE,
		  "unable to parse document: %s\n\0",
		  simple_file->filename);
    }
  }else{
    /* Get the root element node */
    simple_file->root_node = xmlDocGetRootElement(simple_file->doc);
  }
}

void
ags_simple_file_open(AgsSimpleFile *simple_file,
		     GError **error)
{
  g_return_if_fail(AGS_IS_SIMPLE_FILE(simple_file));

  g_object_ref(G_OBJECT(simple_file));
  g_signal_emit(G_OBJECT(simple_file),
		simple_file_signals[OPEN], 0,
		error);
  g_object_unref(G_OBJECT(simple_file));
}

void
ags_simple_file_real_open_from_data(AgsSimpleFile *simple_file,
				    gchar *data, guint length,
				    GError **error)
{
  if(simple_file == NULL){
    return;
  }

  simple_file->doc = xmlReadMemory(data, length, simple_file->filename, NULL, 0);

  if(simple_file->doc == NULL){
    g_warning("ags_simple_file.c - failed to read XML document %s\0", simple_file->filename);

    if(error != NULL){
      g_set_error(error,
		  AGS_SIMPLE_FILE_ERROR,
		  AGS_SIMPLE_FILE_ERROR_PARSER_FAILURE,
		  "unable to parse document from data: %s\n\0",
		  simple_file->filename);
    }
  }else{
    /* Get the root element node */
    simple_file->root_node = xmlDocGetRootElement(simple_file->doc);
  }
}

void
ags_simple_file_open_from_data(AgsSimpleFile *simple_file,
			       gchar *data, guint length,
			       GError **error)
{
  g_return_if_fail(AGS_IS_SIMPLE_FILE(simple_file));

  g_object_ref(G_OBJECT(simple_file));
  g_signal_emit(G_OBJECT(simple_file),
		simple_file_signals[OPEN_FROM_DATA], 0,
		data, length,
		error);
  g_object_unref(G_OBJECT(simple_file));
}

void
ags_simple_file_real_rw_open(AgsSimpleFile *simple_file,
			     gboolean create,
			     GError **error)
{
  if(simple_file == NULL){
    return;
  }

  simple_file->out = fopen(simple_file->filename, "w+\0");

  simple_file->doc = xmlNewDoc("1.0\0");
  simple_file->root_node = xmlNewNode(NULL, "ags-simple-file\0");
  xmlDocSetRootElement(simple_file->doc, simple_file->root_node);
}

void
ags_simple_file_rw_open(AgsSimpleFile *simple_file,
			gboolean create,
			GError **error)
{
  g_return_if_fail(AGS_IS_SIMPLE_FILE(simple_file));

  g_object_ref(G_OBJECT(simple_file));
  g_signal_emit(G_OBJECT(simple_file),
		simple_file_signals[RW_OPEN], 0,
		create,
		error);
  g_object_unref(G_OBJECT(simple_file));
}

void
ags_simple_file_open_filename(AgsSimpleFile *simple_file,
			      gchar *simple_filename)
{
  GError *error;
  
  if(simple_file == NULL){
    return;
  }

  if(simple_file->filename != NULL){
    ags_simple_file_close(simple_file);
  }

  error = NULL;
  g_object_set(simple_file,
	       "filename\0", filename,
	       NULL);
  ags_simple_file_open(simple_file,
		       &error);

  if(error != NULL){
    g_warning(error->message);
  }
}

void
ags_simple_file_close(AgsSimpleFile *simple_file)
{
  if(simple_file == NULL){
    return;
  }

  if(simple_file->out != NULL){
    fclose(simple_file->out);
  }
  
  /* free the document */
  xmlFreeDoc(simple_file->doc);

  /*
   * Free the global variables that may
   * have been allocated by the parser.
   */
  xmlCleanupParser();

  /*
   * this is to debug memory for regression tests
   */
  xmlMemoryDump();

  simple_file->filename = NULL;
}

void
ags_simple_file_real_write(AgsSimpleFile *simple_file)
{
  AgsApplicationContext *application_context;
  AgsConfig *config;
  
  xmlNode *node, *child;
  
  gchar *id;

  if(simple_file == NULL ||
     simple_file->root_node == NULL){
    return;
  }

  application_context = simple_file->application_context;
  config = ags_config_get_instance();
  
  id = ags_id_generator_create_uuid();

  node = simple_file->root_node;
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_VERSION_PROP,
	     AGS_APPLICATION_CONTEXT(application_context)->version);

  xmlNewProp(node,
	     AGS_FILE_BUILD_ID_PROP,
	     AGS_APPLICATION_CONTEXT(application_context)->build_id);

  /* add to parent */
  ags_simple_file_write_config(simple_file,
			       node,
			       config);  
  
  ags_simple_file_write_window(simple_file,
			       node,
			       AGS_XORG_APPLICATION_CONTEXT(application_context)->window);

  /* resolve */
  ags_simple_file_write_resolve(simple_file);

  /* 
   * Dumping document to file
   */
  //  xmlSaveFormatFileEnc(simple_file->filename, simple_file->doc, "UTF-8\0", 1);
  xmlDocDumpFormatMemoryEnc(simple_file->doc, &(simple_file->buffer), &size, simple_file->encoding, TRUE);

  fwrite(simple_file->buffer, size, sizeof(xmlChar), simple_file->out);
  fflush(simple_file->out);
}


void
ags_simple_file_write(AgsSimpleFile *simple_file)
{
  g_return_if_fail(AGS_IS_SIMPLE_FILE(simple_file));

  g_object_ref(G_OBJECT(simple_file));
  g_signal_emit(G_OBJECT(simple_file),
		simple_file_signals[WRITE], 0);
  g_object_unref(G_OBJECT(simple_file));
}

void
ags_simple_file_real_write_resolve(AgsSimpleFile *simple_file)
{
  GList *list;
  
  simple_file->lookup = g_list_prepend(simple_file->lookup,
				       NULL);
  list = simple_file->lookup;

  while(list != NULL){
    ags_simple_file_lookup_resolve(AGS_FILE_LOOKUP(list->data));

    list = list->next;
  }
}

void
ags_simple_file_write_resolve(AgsSimpleFile *simple_file)
{
  g_return_if_fail(AGS_IS_SIMPLE_FILE(simple_file));

  g_object_ref(G_OBJECT(simple_file));
  g_signal_emit(G_OBJECT(simple_file),
		simple_file_signals[WRITE_RESOLVE], 0);
  g_object_unref(G_OBJECT(simple_file));
}

void
ags_simple_file_real_read(AgsSimpleFile *simple_file)
{
  AgsApplicationContext *application_context;

  xmlNode *root_node, *child;

  root_node = simple_file->root_node;
  
  /* child elements */
  child = root_node->children;
  application_context = simple_file->application_context;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp("ags-sf-config\0",
		     child->name,
		     13)){
	ags_simple_file_read_config(simple_file,
				    child,
				    (GObject **) &(application_context->config));
      }else if(!xmlStrncmp("ags-window\0",
			   child->name,
			   11)){
	ags_simple_file_read_window(simple_file,
				    child,
				    (AgsWindow **) &(AGS_XORG_APPLICATION_CONTEXT(application_context)->window));
      }
    }

    child = child->next;
  }

  /* resolve */
  ags_simple_file_read_resolve(simple_file);
  
  g_message("XML simple file resolved\0");

  ags_connectable_connect(AGS_CONNECTABLE(application_context));

  g_message("XML simple file connected\0");

  /* start */
  ags_simple_file_read_start(simple_file);
}

void
ags_simple_file_read(AgsSimpleFile *simple_file)
{
  g_return_if_fail(AGS_IS_SIMPLE_FILE(simple_file));

  g_object_ref(G_OBJECT(simple_file));
  g_signal_emit(G_OBJECT(simple_file),
		simple_file_signals[READ], 0);
  g_object_unref(G_OBJECT(simple_file));
}

void
ags_simple_file_real_read_resolve(AgsSimpleFile *simple_file)
{
  GList *list;

  simple_file->lookup = g_list_prepend(simple_file->lookup,
				       NULL);
  list = g_list_reverse(simple_file->lookup);
  
  while(list != NULL){
    ags_simple_file_lookup_resolve(AGS_SIMPLE_FILE_LOOKUP(list->data));

    list = list->next;
  }
}

void
ags_simple_file_read_resolve(AgsSimpleFile *simple_file)
{
  g_return_if_fail(AGS_IS_SIMPLE_FILE(simple_file));

  g_object_ref(G_OBJECT(simple_file));
  g_signal_emit(G_OBJECT(simple_file),
		simple_file_signals[READ_RESOLVE], 0);
  g_object_unref(G_OBJECT(simple_file));
}

void
ags_simple_file_real_read_start(AgsSimpleFile *simple_file)
{
  GList *list;

  list = g_list_reverse(simple_file->launch);

  while(list != NULL){
    ags_simple_file_launch_start(AGS_SIMPLE_FILE_LAUNCH(list->data));

    list = list->next;
  }
}

void
ags_simple_file_read_start(AgsSimpleFile *simple_file)
{
  g_return_if_fail(AGS_IS_SIMPLE_FILE(simple_file));

  g_object_ref(G_OBJECT(simple_file));
  g_signal_emit(G_OBJECT(simple_file),
		simple_file_signals[READ_START], 0);
  g_object_unref(G_OBJECT(simple_file));
}

void
ags_simple_file_read_config(AgsSimpleFile *simple_file, xmlNode *node, GObject **ags_config)
{
  AgsConfig *gobject;

  gchar *id;

  char *buffer;
  gsize buffer_length;

  gobject = config;
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
ags_simple_file_read_property_list(AgsSimpleFile *simple_file, xmlNode *node, GList **property)
{
  GParameter *current;
  GList *list;
  
  xmlNode *child;

  child = node->children;
  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-property\0",
		     11)){
	current = NULL;
	ags_simple_file_read_property(simple_file, child, &current);
	list = g_list_prepend(list, current);
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *property = list;
}

void
ags_simple_file_read_property(AgsSimpleFile *simple_file, xmlNode *node, GParameter **property)
{
  GParameter *pointer;

  xmlChar *str;
  xmlChar *type;
  
  if(*property != NULL){
    pointer = *property;
  }else{
    pointer = (GParameter *) g_new(GParameter,
				   1);
    pointer->name = NULL;
    pointer->value = {0,};
    
    *property = pointer;
  }

  str = xmlGetProp(node,
		   "name\0");

  if(str != NULL){
    pointer->name = g_strdup(str);
  }
  
  str = xmlGetProp(node,
		   "value\0");
  type = xmlGetProp(node,
		    "type\0");

  if(str != NULL &&
     type != NULL){
    if(!g_strcmp0(type,
		  "gboolean\0")){
      g_value_init(pointer->value,
		   G_TYPE_BOOLEAN);
      
      if(!g_strcmp0(str,
		    "false\0")){
	g_value_set_boolean(pointer->value,
			    FALSE);
      }else{
	g_value_set_boolean(pointer->value,
			    TRUE);
      }
    }else if(!g_strcmp0(type,
			"guint\0")){
      guint val;

      g_value_init(pointer->value,
		   G_TYPE_UINT);
      
      val = g_ascii_strtoull(str,
			     10,
			     NULL);

      g_value_set_uint(pointer->value,
		       val);
    }else if(!g_strcmp0(type,
			"gint\0")){
      gint val;

      g_value_init(pointer->value,
		   G_TYPE_UINT);
      
      val = g_ascii_strtoll(str,
			    10,
			    NULL);

      g_value_set_int(pointer->value,
		      val);
    }else if(!g_strcmp0(type,
			"gdouble\0")){
      gdouble val;
      
      g_value_init(pointer->value,
		   G_TYPE_DOUBLE);

      val = g_ascii_strtod(str,
			   NULL);

      g_value_set_double(pointer->value,
			 val);
    }else{
      g_warning("ags_simple_file_read_property() unsupported type - %s\0", type);
    }
  }
}

void
ags_simple_file_read_window(AgsSimpleFile *simple_file, xmlNode *node, AgsWindow **window)
{
  AgsWindow *gobject;

  AgsFileLaunch *file_launch;
  
  xmlNode *child;

  xmlChar *str;
  
  if(*window != NULL){
    gobject = *window;
  }else{
    gobject = ags_window_new(simple_file->application_context);

    *window = gobject;
  }

  str = xmlGetProp(node,
		   "filename\0");

  if(str != NULL){
    gobject->name = g_strdup(str);
  }

  /* connect */
  ags_connectable_connect(AGS_CONNECTABLE(gobject));
  
  /* children */
  child = node->children;
  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-machine-list\0",
		     17)){
	GList *machine_start;

	machine_start = NULL;
	ags_simple_file_read_machine_list(simple_file,
					  child,
					  &machine_start);
	g_list_free(machine_start);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-editor\0",
			   11)){
	ags_simple_file_read_editor(simple_file,
				    child,
				    &(window->editor));
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-automation-editor\0",
			   22)){
	ags_simple_file_read_automation_editor(simple_file,
					       child,
					       &(AGS_AUTOMATION_WINDOW(window->automation_window)->automation_editor));
      }
    }

    child = child->next;
  }

  /* launch settings */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "node\0", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start\0",
		   G_CALLBACK(ags_simple_file_read_window_launch), gobject);
  ags_simple_file_add_launch(simple_file,
			     (GObject *) file_launch);
}

void
ags_simple_file_read_window_launch(AgsFileLaunch *file_launch,
				   AgsWindow *window)
{
  xmlChar *str;
  
  gdouble bpm;
  gdouble loop_start, loop_end;
  gboolean loop;

  /* bpm */
  str = xmlGetProp(file_launch->node,
		   "bpm\0");

  if(str != NULL){
    bpm = g_ascii_strtod(str,
			 NULL);
    gtk_adjustment_set_value(window->navigation->bpm,
			     bpm);
  }

  /* loop */
  str = xmlGetProp(file_launch->node,
		   "bpm\0");

  if(str != NULL){
    if(g_strcmp(str,
		"false\0")){
      gtk_toggle_button_set_active(window->navigation->loop,
				   TRUE);
    }
  }
  
  /* loop start */
  str = xmlGetProp(file_launch->node,
		   "loop-start\0");
  
  if(str != NULL){
    bpm = g_ascii_strtod(str,
			 NULL);
    gtk_adjustment_set_value(window->navigation->loop_left_tact,
			     bpm);
  }

  /* loop end */
  str = xmlGetProp(file_launch->node,
		   "loop-end\0");

  if(str != NULL){
    bpm = g_ascii_strtod(str,
			 NULL);
    gtk_adjustment_set_value(window->navigation->loop_right_tact,
			     bpm);
  }
}

void
ags_simple_file_read_machine_list(AgsSimpleFile *simple_file, xmlNode *node, GList **machine)
{
  AgsMachine *current;
  GList *list;
  
  xmlNode *child;

  child = node->children;
  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-machine\0",
		     11)){
	current = NULL;
	ags_simple_file_read_machine(simple_file, child, &current);
	list = g_list_prepend(list, current);
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *machine = list;
}

void
ags_simple_file_read_machine(AgsSimpleFile *simple_file, xmlNode *node, AgsMachine **machine)
{
  AgsWindow *window;
  AgsMachine *gobject;

  GObject *soundcard;

  AgsFileLaunch *file_launch;

  xmlNode *child;
  
  GList *list;
  GList *output_pad;
  GList *input_pad;

  xmlChar *type_name;
  xmlChar *str;

  guint audio_channels;
  guit output_pads, input_pads;

  type_name = xmlGetProp(node,
			 AGS_FILE_TYPE_PROP);

  if(*machine != NULL){
    gobject = *machine;
  }else{
    gobject = g_object_new(g_type_from_name(type_name),
			   NULL);

    *machine = gobject;
  }

  ags_file_add_id_ref(simple_file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", simple_file->application_context,
				   "file\0", simple_file,
				   "node\0", node,
				   "reference\0", gobject,
				   NULL));

  /* retrieve window */  
  window = ags_simple_file_find_id_ref_by_node(simple_file,
					       node->parent->parent);
  soundcard = AGS_XORG_APPLICATION_CONTEXT(file->application_context)->soundcard->data;

  /* add audio to soundcard */
  g_object_ref(G_OBJECT(gobject->audio));
  
  list = ags_soundcard_get_audio(AGS_SOUNDCARD(soundcard));

  if(g_list_find(list,
		 gobject->audio) == NULL){
    list = g_list_prepend(list,
			  gobject->audio);
    ags_soundcard_set_audio(AGS_SOUNDCARD(soundcard),
			    list);
  }
  
  /* connect AgsAudio */
  ags_connectable_connect(AGS_CONNECTABLE(gobject->audio));
  
  /* retrieve channel allocation */
  str = xmlGetProp(node,
		   "channels\0");

  if(str != NULL){
    audio_channels = g_ascii_strtoull(str,
				      10,
				      NULL);
    ags_audio_set_audio_channels(gobject->audio,
				 audio_channels);
  }

  str = xmlGetProp(node,
		   "input-pads\0");

  if(str != NULL){
    input_pads = g_ascii_strtoull(str,
				  10,
				  NULL);
    ags_audio_set_pads(gobject->audio,
		       AGS_TYPE_INPUT,
		       input_pads);
  }

  str = xmlGetProp(node,
		   "output-pads\0");

  if(str != NULL){
    output_pads = g_ascii_strtoull(str,
				   10,
				   NULL);
    ags_audio_set_pads(gobject->audio,
		       AGS_TYPE_OUTPUT,
		       output_pads);
  }

  /* children */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-pad-list\0",
		     16)){
	GList *pad;

	pad = NULL;
	ags_simple_file_read_pad_list(simple_file,
				      child,
				      &pad);

	g_list_free(pad);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-effect-list\0",
			   19)){
	AgsEffectBulk *effect_bulk;
	
	xmlNode *effect_list_child;

	xmlChar *filename, *effect;
	
	gboolean is_output;

	is_output = TRUE;
	str = xmlGetProp(child,
			 "is-output\0");

	if(str != NULL){
	  if(!g_strcmp0(str,
			"false\0")){
	    is_output = FALSE;
	  }
	}

	if(is_output){
	  effect_bulk = AGS_EFFECT_BRIDGE(gobject->bridge)->bulk_output;
	}else{
	  effect_bulk = AGS_EFFECT_BRIDGE(gobject->bridge)->bulk_input;
	}

	filename = xmlGetProp(child,
			      "filename\0");
	effect = xmlGetProp(child,
			    "effect\0");

	ags_effect_bulk_add_effect(effect_bulk,
				   NULL,
				   filename,
				   effect);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-pattern-list\0",
			   20)){
	xmlNode *pattern_list_child;
	
	/* pattern list children */
	pattern_list_child = child->children;

	while(pattern_list_child != NULL){
	  if(pattern_list_child->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(pattern_list_child->name,
			   (xmlChar *) "ags-sf-pattern\0",
			   15)){
	      AgsChannel *channel;
	      AgsPattern *pattern;

	      GList *list;

	      xmlChar *content;
	      
	      guint line;
	      guint bank_0, bank_1;
	      guint i;

	      /* retrieve channel */
	      line = 0;
	      str = xmlGetProp(pattern_list_child,
			       "nth-line\0");

	      if(str != NULL){
		line = g_ascii_strtoull(str,
					10,
					NULL);
	      }
	      
	      channel = ags_channel_nth(machine->audio->input,
					line);

	      /* retrieve bank */
	      bank_0 =
		bank_1 = 0;

	      str = xmlGetProp(pattern_list_child,
			       "bank-0\0");

	      if(str != NULL){
		bank_0 = g_ascii_strtoull(str,
					  10,
					  NULL);
	      }

	      str = xmlGetProp(pattern_list_child,
			       "bank-1\0");

	      if(str != NULL){
		bank_1 = g_ascii_strtoull(str,
					  10,
					  NULL);
	      }

	      machine->bank_0 = bank_0;
	      machine->bank_1 = bank_1;
	      
	      /* toggle pattern */
	      pattern = channel->pattern->content;

	      content = pattern_list_child->content;
	      i = 0;

	      if(content != NULL){
		for(i = 0; i < pattern->dim[2]; i++){
		  if(content[i] == '\0'){
		    break;
		  }
		  
		  if(content[i] == '1'){
		    ags_pattern_toggle_bit(pattern,
					   bank_0,
					   bank_1,
					   i);
		  }
		}
	      }
	    }
	  }
	  pattern_list_child = pattern_list_child->next;
	}
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-notation-list\0",
			   21)){
	GList *notation;

	notation = NULL;
	ags_simple_file_read_notation_list(simple_file,
					   child,
					   &notation);

	g_list_free(notation);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-automation-list\0",
			   23)){
	GList *automation;

	automation = NULL;
	ags_simple_file_read_automation_list(simple_file,
					     child,
					     &automation);

	g_list_free(automation);
      }
    }

    child = child->next;
  }

  ags_connectable_connect(AGS_CONNECTABLE(gobject));

  /* machine specific */  
  if(AGS_IS_LADSPA_BRIDGE(gobject)){
    ags_ladspa_bridge_load(gobject);
  }else if(AGS_IS_DSSI_BRIDGE(gobject)){
    ags_dssi_bridge_load(gobject);
  }else if(AGS_IS_LV2_BRIDGE(gobject)){
    ags_lv2_bridge_load(gobject);
  }

  gtk_widget_show_all(gobject);

  /* launch AgsMachine */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "node\0", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start\0",
		   G_CALLBACK(ags_simple_file_read_machine_launch), gobject);
  ags_simple_file_add_launch(simple_file,
			     (GObject *) file_launch);
}

void
ags_simple_file_read_machine_launch(AgsFileLaunch *file_launch,
				    AgsMachine *machine)
{
  xmlNode *child;
  
  /* children */
  child = file_launch->node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-property-list\0",
		     21)){
	GList *property_start, *property;

	property_start = NULL;
	ags_simple_file_read_property_list(file_launch->file,
					   child,
					   property_start);

	property = property_start;

	while(property != NULL){
	  g_object_set_property(machine,
				((GParameter *) property->data)->name,
				((GParameter *) property->data)->value);
	  
	  property = property->next;
	}
	
	g_list_free_full(property_start,
			 g_free);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-effect-list\0",
			   19)){
	xmlNode *effect_list_child;

	gboolean is_output;

	is_output = TRUE;
	str = xmlGetProp(child,
			 "is-output\0");

	if(str != NULL){
	  if(!g_strcmp0(str,
			"false\0")){
	    is_output = FALSE;
	  }
	}
	
	/* effect list children */
	effect_list_child = child->children;
	
	while(effect_list_child != NULL){
	  if(effect_list_child->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(effect_list_child->name,
			   (xmlChar *) "ags-sf-effect\0",
			   14)){
	      AgsFileLaunch *file_launch;
	      
	      xmlNode *effect_child;

	      /* effect list children */
	      effect_child = child->children;

	      while(effect_child != NULL){
		if(effect_child->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(effect_child->name,
				 (xmlChar *) "ags-sf-control\0",
				 15)){
		    xmlChar *specifier;
		    
		    //TODO:JK: implement me
		    
		  }
		}

		effect_child = effect_child->next;
	      }
	    }
	  }

	  effect_list_child = effect_list_child->next;
	}
      }
    }

    child = child->next;
  }
}

void
ags_simple_file_read_pad_list(AgsSimpleFile *simple_file, xmlNode *node, GList **pad)
{
  AgsPad *current;
  GList *list;
  
  xmlNode *child;

  child = node->children;
  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-pad\0",
		     11)){
	current = NULL;
	ags_simple_file_read_pad(simple_file, child, &current);
	list = g_list_prepend(list, current);
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *pad = list;
}

void
ags_simple_file_read_pad(AgsSimpleFile *simple_file, xmlNode *node, AgsPad **pad)
{
  AgsMachine *machine;
  AgsPad *gobject;
  
  AgsFileLaunch *file_launch;

  xmlNode *child;

  GList *list, *list_start;
    
  xmlChar *str;

  guint nth_pad;
  
  if(*pad != NULL){
    gobject = *pad;

    nth_pad = gobject->channel->pad;
  }else{
    machine = ags_simple_file_get_ref_by_node(node->parent->parent);

    /* get nth pad */
    nth_pad = 0;
    str = xmlGetProp(node->parent,
		     "nth-pad\0");

    if(str != NULL){
      nth_pad = g_ascii_strtoull(str,
				 10,
				 NULL);
    }

    /* retrieve pad */
    gobject = NULL;
    str = xmlGetProp(node->parent,
		     "is-output\0");
    
    if(!g_strcmp0(str,
		  "false\0")){
      list_start = gtk_container_get_children(machine->input);
    }else{
      list_start = gtk_container_get_children(machine->output);
    }

    list = g_list_nth(list_start,
		      nth_pad);

    if(list != NULL){
      gobject = AGS_PAD(list->data);
    }

    g_list_free(list_start);
  }

  ags_file_add_id_ref(simple_file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", simple_file->application_context,
				   "file\0", simple_file,
				   "node\0", node,
				   "reference\0", gobject,
				   NULL));

  /* children */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-line-list\0",
		     17)){
	GList *line;

	line = NULL;
	ags_simple_file_read_line_list(simple_file,
				       child,
				       &line);

	g_list_free(line);
      }
    }

    child = child->next;
  }

  /* launch AgsPad */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "node\0", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start\0",
		   G_CALLBACK(ags_simple_file_read_pad_launch), gobject);
  ags_simple_file_add_launch(simple_file,
			     (GObject *) file_launch);
}

void
ags_simple_file_read_pad_launch(AgsFileLaunch *file_launch,
				AgsPad *pad)
{
  xmlNode *child;

  xmlChar *str;
    
  if(AGS_IS_PAD(pad)){
    str = xmlGetProp(file_launch->node,
		     "group\0");
    
    gtk_toggle_button_set_active(pad->group,
				 g_strcmp(str,
					  "false\0"));

    str = xmlGetProp(file_launch->node,
		     "mute\0");
    
    gtk_toggle_button_set_active(pad->mute,
				 g_strcmp(str,
					  "false\0"));

    str = xmlGetProp(file_launch->node,
		     "solo\0");
    
    gtk_toggle_button_set_active(pad->solo,
				 g_strcmp(str,
					  "false\0"));
  }
  
  /* children */
  child = file_launch->node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-property-list\0",
		     21)){
	GList *property_start, *property;

	property_start = NULL;
	ags_simple_file_read_property_list(file_launch->file,
					   child,
					   property_start);

	property = property_start;

	while(property != NULL){
	  g_object_set_property(pad,
				((GParameter *) property->data)->name,
				((GParameter *) property->data)->value);
	  
	  property = property->next;
	}
	
	g_list_free_full(property_start,
			 g_free);
      }
    }

    child = child->next;
  }
}

void
ags_simple_file_read_line_list(AgsSimpleFile *simple_file, xmlNode *node, GList **line)
{
  AgsLine *current;
  GList *list;
  
  xmlNode *child;

  child = node->children;
  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-line\0",
		     11)){
	current = NULL;
	ags_simple_file_read_line(simple_file, child, &current);
	list = g_list_prepend(list, current);
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *line = list;
}

void
ags_simple_file_read_line(AgsSimpleFile *simple_file, xmlNode *node, AgsLine **line)
{
  AgsPad *pad;
  AgsLine *gobject;

  AgsFileLaunch *file_launch;

  xmlNode *child;

  if(*line != NULL){
    gobject = *line;

    nth_line = gobject->channel->line;
  }else{
    pad = ags_simple_file_get_ref_by_node(node->parent->parent);

    /* get nth line */
    nth_line = 0;
    str = xmlGetProp(node->parent,
		     "nth-line\0");

    if(str != NULL){
      nth_line = g_ascii_strtoull(str,
				  10,
				  NULL);
    }

    /* retrieve line */
    gobject = NULL;

    if(pad != NULL){
      list_start = gtk_container_get_children(pad->expander_set);

      list = g_list_nth(list_start,
			nth_line);

      if(list != NULL){
	gobject = AGS_LINE(list->data);
      }

      g_list_free(list_start);
    }
  }

  ags_file_add_id_ref(simple_file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", simple_file->application_context,
				   "file\0", simple_file,
				   "node\0", node,
				   "reference\0", gobject,
				   NULL));

  /* children */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-effect-list\0",
		     19)){
	xmlNode *effect_list_child;

	/* effect list children */
	effect_list_child = child->children;

	while(effect_list_child != NULL){
	  if(effect_list_child->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(effect_list_child->name,
			   (xmlChar *) "ags-sf-effect\0",
			   14)){
	      xmlNode *effect_child;

	      /* effect list children */
	      effect_child = child->children;

	      while(effect_child != NULL){
		if(effect_child->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(effect_child->name,
				 (xmlChar *) "ags-sf-control\0",
				 15)){
		    //TODO:JK: implement me
		  }
		}

		effect_child = effect_child->next;
	      }
	    }
	  }

	  effect_list_child = effect_list_child->next;
	}
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-property-list\0",
			   14)){
	//TODO:JK: implement me
      }
    }

    child = child->next;
  }

  /* launch AgsLine */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "node\0", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start\0",
		   G_CALLBACK(ags_simple_file_read_line_launch), gobject);
  ags_simple_file_add_launch(simple_file,
			     (GObject *) file_launch);
}

void
ags_simple_file_read_line_launch(AgsFileLaunch *file_launch,
				 AgsLine *line)
{
  AgsMachine *machine;
  AgsChannel *channel, *link;
  
  xmlNode *child;

  GList *xpath_result;
  
  guint nth_line;
  gboolean is_output;

  machine = gtk_widget_get_ancestor(line,
				    AGS_TYPE_MACHINE);
  
  /* link */
  is_output = TRUE;
  str = xmlGetProp(child,
		   "is-output\0");

  if(str != NULL){
    if(!g_strcmp0(str,
		  "false\0")){
      is_output = FALSE;
    }
  }

  nth_line = 0;
  str = xmlGetProp(child,
		   "is-output\0");

  if(str != NULL){
    nth_line = g_ascii_strtoull(str,
				10,
				NULL);
  }
  
  if(is_output){
    channel = ags_channel_nth(machine->audio->output,
			      nth_line);
  }else{
    channel = ags_channel_nth(machine->audio->input,
			      nth_line);
  }

  xpath_result = ags_simple_file_find_id_ref_by_xpath(simple_file,
						      xmlGetProp(file_launch->link));

  if(xpath_result != NULL){
    GError *error;
    
    link = xpath_result->data;

    error = NULL;
    ags_channel_set_link(channel,
			 link,
			 error);

    if(error != NULL){
      g_warning("ags_simple_file_read_line_launch() - %s", error->message);
    }
  }
  
  /* is-grouped */
  if(AGS_IS_LINE(line)){
    gtk_toggle_button_set_active(line->group,
				 g_strcmp0(file_launch->node,
					   "false\0"));
  }
  
  /* children */
  child = file_launch->node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-property-list\0",
		     21)){
	GList *property_start, *property;

	property_start = NULL;
	ags_simple_file_read_property_list(file_launch->file,
					   child,
					   property_start);

	property = property_start;

	while(property != NULL){
	  g_object_set_property(line,
				((GParameter *) property->data)->name,
				((GParameter *) property->data)->value);
	  
	  property = property->next;
	}
	
	g_list_free_full(property_start,
			 g_free);
      }
    }

    child = child->next;
  }
}

void
ags_simple_file_read_editor(AgsSimpleFile *simple_file, xmlNode *node, AgsEditor **editor)
{
  AgsFileLaunch *file_launch;

  //TODO:JK: implement me
}

void
ags_simple_file_read_automation_editor(AgsSimpleFile *simple_file, xmlNode *node, AgsAutomationEditor **automation_editor)
{
  AgsFileLaunch *file_launch;

  //TODO:JK: implement me
}

void
ags_simple_file_read_notation_list(AgsSimpleFile *simple_file, xmlNode *node, GList **notation)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_notation(AgsSimpleFile *simple_file, xmlNode *node, AgsNotation **notation)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_automation_list(AgsSimpleFile *simple_file, xmlNode *node, GList **automation)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_automation(AgsSimpleFile *simple_file, xmlNode *node, AgsAutomation **automation)
{
  //TODO:JK: implement me
}

void
ags_simple_file_write_config(AgsSimpleFile *simple_file, xmlNode *parent, GObject *ags_config)
{
  xmlNode *node;
  xmlNode *cdata;

  gchar *id;
  char *buffer;
  gsize buffer_length;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-config\0");

  ags_simple_file_add_id_ref(simple_file,
			     g_object_new(AGS_TYPE_FILE_ID_REF,
					  "application-context\0", simple_file->application_context,
					  "file\0", simple_file,
					  "node\0", node,
					  "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
					  "reference\0", config,
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
  ags_config_to_data(config,
		     &buffer,
		     &buffer_length);

  cdata = xmlNewCDataBlock(simple_file->doc,
			   buffer,
			   buffer_length);

  xmlAddChild(node,
	      cdata);
}

xmlNode*
ags_simple_file_write_window(AgsSimpleFile *simple_file, xmlNode *parent, AgsConfig *config)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_property_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *property)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_property(AgsSimpleFile *simple_file, xmlNode *parent, GParameter *property)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_window(AgsSimpleFile *simple_file, xmlNode *parent, AgsWindow *window)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_machine_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *machine)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_machine(AgsSimpleFile *simple_file, xmlNode *parent, AgsMachine *machine)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_pad_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *pad)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_pad(AgsSimpleFile *simple_file, xmlNode *parent, AgsPad *pad)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_line_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *line)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_line(AgsSimpleFile *simple_file, xmlNode *parent, AgsLine *line)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_editor(AgsSimpleFile *simple_file, xmlNode *parent, AgsEditor *editor)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_automation_editor(AgsSimpleFile *simple_file, xmlNode *parent, AgsAutomationEditor *automation_editor)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_notation_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *notation)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_notation(AgsSimpleFile *simple_file, xmlNode *parent, AgsNotation *notation)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_automation_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *automation)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_automation(AgsSimpleFile *simple_file, xmlNode *parent, AgsAutomation *automation)
{
  //TODO:JK: implement me
}

AgsSimpleFile*
ags_simple_file_new()
{
  AgsSimpleFile *simple_file;

  simple_file = (AgsSimpleFile *) g_object_new(AGS_TYPE_SIMPLE_FILE,
					       NULL);

  return(simple_file);
}

