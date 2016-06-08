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
void ags_simple_file_real_write_concurrent(AgsSimpleFile *simple_file);
void ags_simple_file_real_write_resolve(AgsSimpleFile *simple_file);

void ags_simple_file_real_read(AgsSimpleFile *simple_file);
void ags_simple_file_real_read_resolve(AgsSimpleFile *simple_file);
void ags_simple_file_real_read_start(AgsSimpleFile *simple_file);

void ags_simple_file_read_config(AgsFile *file, xmlNode *node, AgsConfig **config);
void ags_simple_file_read_property_list(AgsFile *file, xmlNode *node, GList **property);
void ags_simple_file_read_property(AgsFile *file, xmlNode *node, GParameter **property);
void ags_simple_file_read_window(AgsFile *file, xmlNode *node, AgsWindow **window);
void ags_simple_file_read_machine_list(AgsFile *file, xmlNode *node, GList **machine);
void ags_simple_file_read_machine(AgsFile *file, xmlNode *node, AgsMachine **machine);
void ags_simple_file_read_pad_list(AgsFile *file, xmlNode *node, GList **pad);
void ags_simple_file_read_pad(AgsFile *file, xmlNode *node, AgsPad **pad);
void ags_simple_file_read_line_list(AgsFile *file, xmlNode *node, GList **line);
void ags_simple_file_read_line(AgsFile *file, xmlNode *node, AgsLine **line);
void ags_simple_file_read_editor(AgsFile *file, xmlNode *node, AgsEditor **editor);
void ags_simple_file_read_automation_editor(AgsFile *file, xmlNode *node, AgsAutomationEditor **automation_editor);

xmlNode* ags_simple_file_write_window(AgsFile *file, xmlNode *parent, AgsConfig *config);
xmlNode* ags_simple_file_write_property_list(AgsFile *file, xmlNode *parent, GList *property);
xmlNode* ags_simple_file_write_property(AgsFile *file, xmlNode *parent, GParameter *property);
xmlNode* ags_simple_file_write_window(AgsFile *file, xmlNode *parent, AgsWindow *window);
xmlNode* ags_simple_file_write_machine_list(AgsFile *file, xmlNode *parent, GList *machine);
xmlNode* ags_simple_file_write_machine(AgsFile *file, xmlNode *parent, AgsMachine *machine);
xmlNode* ags_simple_file_write_pad_list(AgsFile *file, xmlNode *parent, GList *pad);
xmlNode* ags_simple_file_write_pad(AgsFile *file, xmlNode *parent, AgsPad *pad);
xmlNode* ags_simple_file_write_line_list(AgsFile *file, xmlNode *parent, GList *line);
xmlNode* ags_simple_file_write_line(AgsFile *file, xmlNode *parent, AgsLine *line);
xmlNode* ags_simple_file_write_editor(AgsFile *file, xmlNode *parent, AgsEditor *editor);
xmlNode* ags_simple_file_write_automation_editor(AgsFile *file, xmlNode *parent, AgsAutomationEditor *automation_editor);

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
  simple_file->write_concurrent = ags_simple_file_real_write_concurrent;
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

  simple_file_signals[WRITE_CONCURRENT] =
    g_signal_new("write_concurrent\0",
		 G_TYPE_FROM_CLASS(simple_file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSimpleFileClass, write_concurrent),
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
      gchar *filename;

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
  //TODO:JK: implement me
}

void
ags_simple_file_add_id_ref(AgsSimpleFile *simple_file, GObject *id_ref)
{
  //TODO:JK: implement me
}

GObject*
ags_simple_file_find_id_ref_by_node(AgsSimpleFile *simple_file, xmlNode *node)
{
  //TODO:JK: implement me
}

GObject*
ags_simple_file_find_id_ref_by_xpath(AgsSimpleFile *simple_file, gchar *xpath)
{
  //TODO:JK: implement me
}

GObject*
ags_simple_file_find_id_ref_by_reference(AgsSimpleFile *simple_file, gpointer ref)
{
  //TODO:JK: implement me
}

void
ags_simple_file_add_lookup(AgsSimpleFile *simple_file, GObject *file_lookup)
{
  //TODO:JK: implement me
}

void
ags_simple_file_add_launch(AgsSimpleFile *simple_file, GObject *file_launch)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_open(AgsSimpleFile *simple_file,
			       GError **error)
{
  //TODO:JK: implement me
}

void
ags_simple_file_open(AgsSimpleFile *simple_file,
			  GError **error)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_open_from_data(AgsSimpleFile *simple_file,
					 gchar *data, guint length,
					 GError **error)
{
  //TODO:JK: implement me
}

void
ags_simple_file_open_from_data(AgsSimpleFile *simple_file,
				    gchar *data, guint length,
				    GError **error)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_rw_open(AgsSimpleFile *simple_file,
				  gboolean create,
				  GError **error)
{
  //TODO:JK: implement me
}

void
ags_simple_file_rw_open(AgsSimpleFile *simple_file,
			     gboolean create,
			     GError **error)
{
  //TODO:JK: implement me
}

void
ags_simple_file_open_filename(AgsSimpleFile *simple_file,
				   gchar *filename)
{
  //TODO:JK: implement me
}

void
ags_simple_file_close(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_write(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}


void
ags_simple_file_write(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_write_concurrent(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_write_concurrent(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_write_resolve(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_write_resolve(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_read(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_read_resolve(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_resolve(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_read_start(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_start(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_config(AgsSimpleFile *simple_file, xmlNode *node, GObject **ags_config)
{
  //TODO:JK: implement me
}

void
ags_simple_file_write_config(AgsSimpleFile *simple_file, xmlNode *parent, GObject *ags_config)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_application_context(AgsSimpleFile *simple_file, xmlNode *node, GObject **application_context)
{
  //TODO:JK: implement me
}

void
ags_simple_file_write_application_context(AgsSimpleFile *simple_file, xmlNode *parent, GObject *application_context)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_config(AgsFile *file, xmlNode *node, AgsConfig **config)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_property_list(AgsFile *file, xmlNode *node, GList **property)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_property(AgsFile *file, xmlNode *node, GParameter **property)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_window(AgsFile *file, xmlNode *node, AgsWindow **window)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_machine_list(AgsFile *file, xmlNode *node, GList **machine)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_machine(AgsFile *file, xmlNode *node, AgsMachine **machine)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_pad_list(AgsFile *file, xmlNode *node, GList **pad)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_pad(AgsFile *file, xmlNode *node, AgsPad **pad)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_line_list(AgsFile *file, xmlNode *node, GList **line)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_line(AgsFile *file, xmlNode *node, AgsLine **line)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_editor(AgsFile *file, xmlNode *node, AgsEditor **editor)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_automation_editor(AgsFile *file, xmlNode *node, AgsAutomationEditor **automation_editor)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_window(AgsFile *file, xmlNode *parent, AgsConfig *config)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_property_list(AgsFile *file, xmlNode *parent, GList *property)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_property(AgsFile *file, xmlNode *parent, GParameter *property)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_window(AgsFile *file, xmlNode *parent, AgsWindow *window)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_machine_list(AgsFile *file, xmlNode *parent, GList *machine)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_machine(AgsFile *file, xmlNode *parent, AgsMachine *machine)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_pad_list(AgsFile *file, xmlNode *parent, GList *pad)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_pad(AgsFile *file, xmlNode *parent, AgsPad *pad)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_line_list(AgsFile *file, xmlNode *parent, GList *line)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_line(AgsFile *file, xmlNode *parent, AgsLine *line)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_editor(AgsFile *file, xmlNode *parent, AgsEditor *editor)
{
  //TODO:JK: implement me
}

xmlNode*
ags_simple_file_write_automation_editor(AgsFile *file, xmlNode *parent, AgsAutomationEditor *automation_editor)
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

