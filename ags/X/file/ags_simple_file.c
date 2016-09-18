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

#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_marshal.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_launch.h>
#include <ags/file/ags_file_link.h>

#include <ags/thread/ags_thread-posix.h>

#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_ladspa_plugin.h>
#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_dssi_plugin.h>
#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_lv2_manager.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_notation.h>
#include <ags/audio/ags_note.h>
#include <ags/audio/ags_automation.h>
#include <ags/audio/ags_acceleration.h>

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_devout.h>

#include <ags/audio/task/ags_change_soundcard.h>
#include <ags/audio/task/ags_apply_presets.h>

#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_audio_file.h>

#include <ags/widget/ags_dial.h>

#include <ags/X/ags_xorg_application_context.h>
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

#include <ags/X/editor/ags_machine_selector.h>
#include <ags/X/editor/ags_machine_radio_button.h>

#include <ags/X/machine/ags_panel.h>
#include <ags/X/machine/ags_mixer.h>
#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_synth_input_line.h>
#include <ags/X/machine/ags_oscillator.h>
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
void ags_simple_file_read_channel_line_launch(AgsFileLaunch *file_launch,
					      AgsChannel *channel);
void ags_simple_file_read_effect_pad_list(AgsSimpleFile *simple_file, xmlNode *node, GList **effect_pad);
void ags_simple_file_read_effect_pad(AgsSimpleFile *simple_file, xmlNode *node, AgsEffectPad **effect_pad);
void ags_simple_file_read_effect_pad_launch(AgsFileLaunch *file_launch,
					    AgsEffectPad *effect_pad);
void ags_simple_file_read_effect_line_list(AgsSimpleFile *simple_file, xmlNode *node, GList **effect_line);
void ags_simple_file_read_effect_line(AgsSimpleFile *simple_file, xmlNode *node, AgsEffectLine **effect_line);
void ags_simple_file_read_effect_line_launch(AgsFileLaunch *file_launch,
					     AgsEffectLine *effect_line);
void ags_simple_file_read_editor(AgsSimpleFile *simple_file, xmlNode *node, AgsEditor **editor);
void ags_simple_file_read_editor_launch(AgsFileLaunch *file_launch,
					AgsEditor *editor);
void ags_simple_file_read_automation_editor(AgsSimpleFile *simple_file, xmlNode *node, AgsAutomationEditor **automation_editor);
void ags_simple_file_read_automation_editor_launch(AgsFileLaunch *file_launch,
						   AgsAutomationEditor *automation_editor);
void ags_simple_file_read_notation_list(AgsSimpleFile *simple_file, xmlNode *node, GList **notation);
void ags_simple_file_read_notation(AgsSimpleFile *simple_file, xmlNode *node, AgsNotation **notation);
void ags_simple_file_read_automation_list(AgsSimpleFile *simple_file, xmlNode *node, GList **automation);
void ags_simple_file_read_automation(AgsSimpleFile *simple_file, xmlNode *node, AgsAutomation **automation);

xmlNode* ags_simple_file_write_config(AgsSimpleFile *simple_file, xmlNode *parent, AgsConfig *config);
xmlNode* ags_simple_file_write_window(AgsSimpleFile *simple_file, xmlNode *parent, AgsWindow *window);
xmlNode* ags_simple_file_write_property_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *property);
xmlNode* ags_simple_file_write_property(AgsSimpleFile *simple_file, xmlNode *parent, GParameter *property);
xmlNode* ags_simple_file_write_window(AgsSimpleFile *simple_file, xmlNode *parent, AgsWindow *window);
xmlNode* ags_simple_file_write_machine_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *machine);
xmlNode* ags_simple_file_write_machine(AgsSimpleFile *simple_file, xmlNode *parent, AgsMachine *machine);
xmlNode* ags_simple_file_write_pad_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *pad);
xmlNode* ags_simple_file_write_pad(AgsSimpleFile *simple_file, xmlNode *parent, AgsPad *pad);
xmlNode* ags_simple_file_write_line_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *line);
xmlNode* ags_simple_file_write_line(AgsSimpleFile *simple_file, xmlNode *parent, AgsLine *line);
void ags_simple_file_write_line_resolve_link(AgsFileLookup *file_lookup,
					     AgsChannel *channel);
xmlNode* ags_simple_file_write_effect_pad_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *effect_pad);
xmlNode* ags_simple_file_write_effect_pad(AgsSimpleFile *simple_file, xmlNode *parent, AgsEffectPad *effect_pad);
xmlNode* ags_simple_file_write_effect_line_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *effect_line);
xmlNode* ags_simple_file_write_effect_line(AgsSimpleFile *simple_file, xmlNode *parent, AgsEffectLine *effect_line);
xmlNode* ags_simple_file_write_editor(AgsSimpleFile *simple_file, xmlNode *parent, AgsEditor *editor);
void ags_simple_file_write_editor_resolve_machine(AgsFileLookup *file_lookup,
						  AgsEditor *editor);
xmlNode* ags_simple_file_write_automation_editor(AgsSimpleFile *simple_file, xmlNode *parent, AgsAutomationEditor *automation_editor);
void ags_simple_file_write_automation_editor_resolve_machine(AgsFileLookup *file_lookup,
							     AgsAutomationEditor *automation_editor);
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
   * Since: 0.7.25
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

  simple_file->id_ref = NULL;
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
  simple_file->id_ref = g_list_prepend(simple_file->id_ref,
				       id_ref);
}

GObject*
ags_simple_file_find_id_ref_by_node(AgsSimpleFile *simple_file, xmlNode *node)
{
  AgsFileIdRef *file_id_ref;
  GList *list;

  if(simple_file == NULL ||
     node == NULL){
    return(NULL);
  }
  
  list = simple_file->id_ref;

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
  AgsFileIdRef *file_id_ref;

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
      GObject *gobject;
      
      gobject = ags_simple_file_find_id_ref_by_node(simple_file,
						    node[i]);

      if(gobject != NULL){
	list = g_list_prepend(list,
			      gobject);
      }
    }
  }

  if(list == NULL){
    g_message("no xpath match: %s\0", xpath);
  }
  
  return(list);
}

GList*
ags_simple_file_find_id_ref_by_reference(AgsSimpleFile *simple_file, gpointer ref)
{
  AgsFileIdRef *file_id_ref;
  
  GList *list;
  GList *ref_list;
  
  if(simple_file == NULL || ref == NULL){
    return(NULL);
  }

  list = simple_file->id_ref;
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
ags_simple_file_add_lookup(AgsSimpleFile *simple_file, GObject *file_lookup)
{
  if(simple_file == NULL || file_lookup == NULL){
    return;
  }

  g_object_ref(G_OBJECT(file_lookup));

  simple_file->lookup = g_list_prepend(simple_file->lookup,
				       file_lookup);
}

void
ags_simple_file_add_launch(AgsSimpleFile *simple_file, GObject *file_launch)
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
			      gchar *filename)
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
    g_warning("%s\0", error->message);
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

  guint size;
  
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
  
  list = simple_file->lookup;

  while(list != NULL){
    ags_file_lookup_resolve(AGS_FILE_LOOKUP(list->data));

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

  application_context = simple_file->application_context;

  root_node = simple_file->root_node;

  /* child elements */
  child = root_node->children;

  ags_xorg_application_context_register_types(AGS_XORG_APPLICATION_CONTEXT(application_context));
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp("ags-sf-config\0",
		     child->name,
		     13)){	
	AgsJackServer *jack_server;
	
	AgsConfig *config;

	AgsThread *thread, *next_thread;

	GObject *current;

	GList *soundcard;
	GList *jack_soundcard, *alsa_soundcard, *oss_soundcard;
	GList *list;

	gchar *soundcard_group;
	gchar *backend;
	gchar *device;
	gchar *str;

	guint pcm_channels;
	guint samplerate;
	guint buffer_size;
	guint format;
	guint i;
	gboolean initial_run;
	gboolean use_jack, use_alsa, use_oss;
	gboolean found_jack, found_alsa, found_oss;

	initial_run = TRUE;
	
	use_jack = FALSE;
	use_alsa = FALSE;
	use_oss = FALSE;

	found_jack = FALSE;
	found_alsa = FALSE;	
	found_oss = FALSE;

	if(AGS_XORG_APPLICATION_CONTEXT(application_context)->distributed_manager != NULL){
	  jack_server = AGS_XORG_APPLICATION_CONTEXT(application_context)->distributed_manager->data;
	}
	
	config = ags_config_get_instance();
	ags_simple_file_read_config(simple_file,
				    child,
				    (GObject **) &config);

	soundcard = AGS_XORG_APPLICATION_CONTEXT(application_context)->soundcard;

	/* parse configuration */
	soundcard_group = g_strdup(AGS_CONFIG_SOUNDCARD);
  
	for(i = 0; ; i++){
	  guint pcm_channels, buffer_size, samplerate, format;
	  gboolean use_jack, use_alsa, use_oss;

	  if(!g_key_file_has_group(config->key_file,
				   soundcard_group)){
	    if(i == 0){
	      g_free(soundcard_group);
	      soundcard_group = g_strdup_printf("%s-%d\0",
						AGS_CONFIG_SOUNDCARD,
						i);
	      continue;
	    }else{
	      break;
	    }
	  }

	  current = NULL;
	  backend = ags_config_get_value(config,
					 soundcard_group,
					 "backend\0");
	  
	  g_free(soundcard_group);
	  soundcard_group = g_strdup_printf("%s-%d\0",
					    AGS_CONFIG_SOUNDCARD,
					    i);
	
	  /* change soundcard */
	  if(backend != NULL){
	    if(backend != NULL){
	      if(!g_ascii_strncasecmp(backend,
				      "jack\0",
				      5)){
		use_jack = TRUE;
	      }else if(!g_ascii_strncasecmp(backend,
					    "alsa\0",
					    5)){
		use_alsa = TRUE;
	      }else if(!g_ascii_strncasecmp(backend,
					    "oss\0",
					    4)){
		use_oss = TRUE;
	      }
	    }

	    /* remove old */
	    if(soundcard != NULL &&
	       (use_jack ||
		use_alsa ||
		use_oss)){
	      AGS_XORG_APPLICATION_CONTEXT(application_context)->soundcard = NULL;
	      g_list_free_full(soundcard,
			       g_object_unref);

	      soundcard = NULL;

	      thread = application_context->main_loop;
	    
	      while((thread = ags_thread_find_type(thread, AGS_TYPE_SOUNDCARD_THREAD)) != NULL){
		next_thread = g_atomic_pointer_get(&(thread->next));

		ags_thread_remove_child(application_context->main_loop,
					thread);

		thread = next_thread;
	      }
	    
	      jack_server->n_soundcards = 0;
	    }
	  
	    /* jack */
	    if(!g_ascii_strncasecmp(str,
				    "jack\0",
				    5)){
	      current = ags_distributed_manager_register_soundcard(AGS_DISTRIBUTED_MANAGER(jack_server),
								   TRUE);
	      ags_jack_server_connect_client(jack_server);
	
	      use_jack = TRUE;
	    }else if(!g_ascii_strncasecmp(str,
					  "alsa\0",
					  5)){
	      current = ags_devout_new(application_context);
	      AGS_DEVOUT(current)->flags &= (~AGS_DEVOUT_OSS);
	      AGS_DEVOUT(current)->flags |= AGS_DEVOUT_ALSA;
		
	      use_alsa = TRUE;
	    }else if(!g_ascii_strncasecmp(str,
					  "oss\0",
					  4)){
	      current = ags_devout_new(application_context);
	      AGS_DEVOUT(current)->flags &= (~AGS_DEVOUT_ALSA);
	      AGS_DEVOUT(current)->flags |= AGS_DEVOUT_OSS;

	      use_oss = TRUE;
	    }else{
	      g_warning("unknown soundcard backend\0");

	      continue;
	    }
	  }else{
	    g_warning("unknown soundcard backend\0");
      
	    continue;
	  }

	  AGS_XORG_APPLICATION_CONTEXT(application_context)->soundcard = g_list_append(AGS_XORG_APPLICATION_CONTEXT(application_context)->soundcard,
										       current);
	  
	  /* reset presets of soundcard */
	  pcm_channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
	  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
	  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
	  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

	  /* device */
	  device = ags_config_get_value(config,
					soundcard_group,
					"device\0");

	  if(device != NULL){
	    ags_soundcard_set_device(AGS_SOUNDCARD(current),
				     device);
	    g_free(device);
	  }

	  /* presets */
	  str = ags_config_get_value(config,
				     soundcard_group,
				     "pcm-channels\0");

	  if(str != NULL){
	    pcm_channels = g_ascii_strtoull(str,
					    NULL,
					    10);
	    g_free(str);
	  }
  
	  str = ags_config_get_value(config,
				     soundcard_group,
				     "samplerate\0");
  
	  if(str != NULL){
	    samplerate = g_ascii_strtoull(str,
					  NULL,
					  10);
	    free(str);
	  }

	  str = ags_config_get_value(config,
				     soundcard_group,
				     "buffer-size\0");
	  if(str != NULL){
	    buffer_size = g_ascii_strtoull(str,
					   NULL,
					   10);
	    free(str);
	  }

	  str = ags_config_get_value(config,
				     soundcard_group,
				     "format\0");
	  if(str != NULL){
	    format = g_ascii_strtoull(str,
				      NULL,
				      10);
	    free(str);

	  }

	  ags_soundcard_set_presets(AGS_SOUNDCARD(current),
				    pcm_channels,
				    samplerate,
				    buffer_size,
				    format);
	}
	
	/* reset default card */
	soundcard = AGS_XORG_APPLICATION_CONTEXT(application_context)->soundcard;
	
	if(AGS_XORG_APPLICATION_CONTEXT(application_context)->soundcard != NULL){
	  g_object_set(AGS_XORG_APPLICATION_CONTEXT(application_context)->window,
		       "soundcard\0", soundcard->data,
		       NULL);
	}
		
	/* calculate frequency of audio loop */
	AGS_THREAD(application_context->main_loop)->tic_delay = 0;
	AGS_THREAD(application_context->main_loop)->freq = ceil(samplerate / buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

      	thread = ags_thread_find_type(AGS_THREAD(application_context->main_loop),
				      AGS_TYPE_EXPORT_THREAD);
      	thread->tic_delay = 0;
	thread->freq = ceil(samplerate / buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

	/* AgsSoundcardThread */
	AGS_XORG_APPLICATION_CONTEXT(application_context)->soundcard_thread = NULL;
	list = AGS_XORG_APPLICATION_CONTEXT(application_context)->soundcard;
    
	while(list != NULL){
	  thread = (AgsThread *) ags_soundcard_thread_new(list->data);
	  thread->tic_delay = 0;
	  thread->freq = ceil(samplerate / buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;
	  ags_thread_add_child_extended(application_context->main_loop,
					thread,
					TRUE, TRUE);
	  
	  if(AGS_XORG_APPLICATION_CONTEXT(application_context)->soundcard_thread == NULL){
	    AGS_XORG_APPLICATION_CONTEXT(application_context)->soundcard_thread = thread;
	  }
    
	  list = list->next;      
	}
      }else if(!xmlStrncmp("ags-sf-window\0",
			   child->name,
			   14)){
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

  list = g_list_reverse(simple_file->lookup);
  
  while(list != NULL){
    ags_file_lookup_resolve(AGS_FILE_LOOKUP(list->data));

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
    ags_file_launch_start(AGS_FILE_LAUNCH(list->data));

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
ags_simple_file_read_config(AgsSimpleFile *simple_file, xmlNode *node, AgsConfig **ags_config)
{
  AgsConfig *gobject;

  gchar *id;

  char *buffer;
  gsize buffer_length;

  gobject = *ags_config;
  gobject->version = xmlGetProp(node,
				AGS_FILE_VERSION_PROP);

  gobject->build_id = xmlGetProp(node,
				 AGS_FILE_BUILD_ID_PROP);

  buffer = xmlNodeGetContent(node);
  buffer_length = xmlStrlen(buffer);

  g_message("%s\0", buffer);
  
  ags_config_load_from_data(gobject,
			    buffer, buffer_length);
}

void
ags_simple_file_read_property_list(AgsSimpleFile *simple_file, xmlNode *node, GList **property)
{
  GParameter *current;
  GList *list;
  
  xmlNode *child;

  guint i;
  
  child = node->children;
  list = NULL;

  i = 0;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-property\0",
		     11)){
	current = NULL;

	if(*property != NULL){
	  GList *iter;

	  iter = g_list_nth(*property,
			    i);

	  if(iter != NULL){
	    current = iter->data;
	  }
	}
	
	ags_simple_file_read_property(simple_file, child, &current);
	list = g_list_prepend(list, current);

	i++;
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
    pointer = (GParameter *) g_new0(GParameter,
				    1);
    pointer->name = NULL;
    
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

  if(str != NULL){
    if(!g_strcmp0(type,
		  "gboolean\0")){
      g_value_init(&(pointer->value),
		   G_TYPE_BOOLEAN);
      
      if(!g_strcmp0(str,
		    "false\0")){
	g_value_set_boolean(&(pointer->value),
			    FALSE);
      }else{
	g_value_set_boolean(&(pointer->value),
			    TRUE);
      }
    }else if(!g_strcmp0(type,
			"guint\0")){
      guint val;

      g_value_init(&(pointer->value),
		   G_TYPE_UINT);
      
      val = g_ascii_strtoull(str,
			     NULL,
			     10);

      g_value_set_uint(&(pointer->value),
		       val);
    }else if(!g_strcmp0(type,
			"gint\0")){
      gint val;

      g_value_init(&(pointer->value),
		   G_TYPE_UINT);
      
      val = g_ascii_strtoll(str,
			    NULL,
			    10);

      g_value_set_int(&(pointer->value),
		      val);
    }else if(!g_strcmp0(type,
			"gdouble\0")){
      gdouble val;
      
      g_value_init(&(pointer->value),
		   G_TYPE_DOUBLE);

      val = g_ascii_strtod(str,
			   NULL);

      g_value_set_double(&(pointer->value),
			 val);
    }else{
      g_value_init(&(pointer->value),
		   G_TYPE_STRING);

      g_value_set_string(&(pointer->value),
			 str);
    }
  }
}

void
ags_simple_file_read_window(AgsSimpleFile *simple_file, xmlNode *node, AgsWindow **window)
{
  AgsWindow *gobject;

  AgsFileLaunch *file_launch;

  AgsApplicationContext *application_context;
  AgsConfig *config;
  
  xmlNode *child;

  GList *list;
  
  xmlChar *str;

  guint pcm_channels;
  guint samplerate;
  guint buffer_size;
  guint format;
  
  if(*window != NULL){
    gobject = *window;
  }else{
    gobject = ags_window_new(simple_file->application_context);
    AGS_XORG_APPLICATION_CONTEXT(simple_file->application_context)->window = gobject;
    
    *window = gobject;
  }

  str = xmlGetProp(node,
		   "filename\0");

  if(str != NULL){
    gobject->name = g_strdup(str);

    gtk_window_set_title((GtkWindow *) gobject, g_strconcat("GSequencer - \0", gobject->name, NULL));
  }
  
  /* connect */
  ags_connectable_connect(AGS_CONNECTABLE(gobject));
  
  gtk_widget_show_all(gobject);

  /* children */  
  child = node->children;
  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-machine-list\0",
		     20)){
	GList *machine_start;

	machine_start = NULL;
	ags_simple_file_read_machine_list(simple_file,
					  child,
					  &machine_start);
	g_list_free(machine_start);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-editor\0",
			   14)){
	ags_simple_file_read_editor(simple_file,
				    child,
				    &(gobject->editor));
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-automation-editor\0",
			   25)){
	ags_simple_file_read_automation_editor(simple_file,
					       child,
					       &(AGS_AUTOMATION_WINDOW(gobject->automation_window)->automation_editor));
      }
    }

    child = child->next;
  }

  config = ags_config_get_instance();

  /* presets */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "pcm-channels\0");

  if(str != NULL){
    pcm_channels = g_ascii_strtoull(str,
				    NULL,
				    10);
    g_free(str);
  }
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate\0");
  
  if(str != NULL){
    samplerate = g_ascii_strtoull(str,
				  NULL,
				  10);
    free(str);
  }

  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "buffer-size\0");
  if(str != NULL){
    buffer_size = g_ascii_strtoull(str,
				   NULL,
				   10);
    free(str);
  }

  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "format\0");
  if(str != NULL){
    format = g_ascii_strtoull(str,
			      NULL,
			      10);
    free(str);
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
    gtk_adjustment_set_value(window->navigation->bpm->adjustment,
			     bpm);
  }

  /* loop */
  str = xmlGetProp(file_launch->node,
		   "loop\0");

  if(str != NULL){
    if(!g_strcmp0(str,
		  "false\0")){
      gtk_toggle_button_set_active(window->navigation->loop,
				   FALSE);
    }else{
      gtk_toggle_button_set_active(window->navigation->loop,
				   TRUE);
    }
  }
  
  /* loop start */
  str = xmlGetProp(file_launch->node,
		   "loop-start\0");
  
  if(str != NULL){
    loop_start = g_ascii_strtod(str,
				NULL);
    gtk_adjustment_set_value(window->navigation->loop_left_tact->adjustment,
			     loop_start);
  }

  /* loop end */
  str = xmlGetProp(file_launch->node,
		   "loop-end\0");

  if(str != NULL){
    loop_end = g_ascii_strtod(str,
			      NULL);
    gtk_adjustment_set_value(window->navigation->loop_right_tact->adjustment,
			     loop_end);
  }
}

void
ags_simple_file_read_machine_list(AgsSimpleFile *simple_file, xmlNode *node, GList **machine)
{
  AgsMachine *current;
  
  xmlNode *child;

  GList *list;

  guint i;
  
  child = node->children;
  list = NULL;

  i = 0;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-machine\0",
		     11)){
	current = NULL;

	if(*machine != NULL){
	  GList *iter;

	  iter = g_list_nth(*machine,
			    i);
	  current = iter->data;
	}
	
	ags_simple_file_read_machine(simple_file, child, &current);
	list = g_list_prepend(list, current);

	i++;
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

  AgsConfig *config;
  GObject *soundcard;

  AgsFileLaunch *file_launch;

  xmlNode *child;
  
  GList *list;
  GList *output_pad;
  GList *input_pad;

  gchar *soundcard_group;
  xmlChar *device;
  xmlChar *type_name;
  xmlChar *str;
  
  guint audio_channels;
  guint output_pads, input_pads;
  guint i;
  
  type_name = xmlGetProp(node,
			 AGS_FILE_TYPE_PROP);

  if(*machine != NULL){
    gobject = *machine;
  }else{
    gobject = g_object_new(g_type_from_name(type_name),
			   NULL);
    
    *machine = gobject;
  }

  if(gobject == NULL ||
     !AGS_IS_MACHINE(gobject)){
    return;
  }
  
  ags_simple_file_add_id_ref(simple_file,
			     g_object_new(AGS_TYPE_FILE_ID_REF,
					  "application-context\0", simple_file->application_context,
					  "file\0", simple_file,
					  "node\0", node,
					  "reference\0", gobject,
					  NULL));

  /* retrieve window */  
  window = AGS_XORG_APPLICATION_CONTEXT(simple_file->application_context)->window;

  config = ags_config_get_instance();
  
  /* find soundcard */
  soundcard = NULL;
  device = xmlGetProp(node,
		      "soundcard-device\0");

  if(device != NULL){
    list = AGS_XORG_APPLICATION_CONTEXT(simple_file->application_context)->soundcard;
    soundcard_group = g_strdup("soundcard\0");
  
    for(i = 0; list != NULL; i++){
      gboolean use_jack, use_alsa, use_oss;

      soundcard = list->data;

      if(!g_key_file_has_group(config->key_file,
			       soundcard_group)){
	if(i == 0){
	  g_free(soundcard_group);
	  soundcard_group = g_strdup_printf("%s-%d\0",
					    AGS_CONFIG_SOUNDCARD,
					    i);
	  continue;
	}else{
	  break;
	}
      }

      str = ags_config_get_value(config,
				 soundcard_group,
				 "backend\0");

      g_free(soundcard_group);
      soundcard_group = g_strdup_printf("%s-%d\0",
					AGS_CONFIG_SOUNDCARD,
					i);

      if(str != NULL){
	if(!g_ascii_strncasecmp(str,
				"jack\0",
				5)){
	  use_jack = TRUE;
	}else if(!g_ascii_strncasecmp(str,
				      "alsa\0",
				      5)){
	  use_alsa = TRUE;
	}else if(!g_ascii_strncasecmp(str,
				      "oss\0",
				      4)){
	  use_oss = TRUE;
	}else{
	  g_warning("unknown soundcard backend\0");

	  continue;
	}
      }else{
	g_warning("unknown soundcard backend\0");
      
	continue;
      }

      /* device */
      str = ags_config_get_value(config,
				 soundcard_group,
				 "device\0");

      if(str != NULL &&
	 !g_ascii_strcasecmp(str,
			     device)){
	soundcard = list->data;

	g_free(str);
	break;
      }

      /* iterate soundcard */
      list = list->next;
    }
  }
  
  if(soundcard == NULL &&
     AGS_XORG_APPLICATION_CONTEXT(simple_file->application_context)->soundcard != NULL){
    soundcard = AGS_XORG_APPLICATION_CONTEXT(simple_file->application_context)->soundcard->data;
  }
  
  g_object_set(gobject->audio,
	       "soundcard\0", soundcard,
	       NULL);
  g_free(soundcard_group);

  /* machine specific */  
  if(AGS_IS_LADSPA_BRIDGE(gobject)){
    xmlChar *filename, *effect;

    filename = xmlGetProp(node,
			  "plugin-file\0");

    effect = xmlGetProp(node,
			"effect\0");

    g_object_set(gobject,
		 "filename\0", filename,
		 "effect\0", effect,
		 NULL);
  }else if(AGS_IS_DSSI_BRIDGE(gobject)){
    xmlChar *filename, *effect;

    filename = xmlGetProp(node,
			  "plugin-file\0");

    effect = xmlGetProp(node,
			"effect\0");

    g_object_set(gobject,
		 "filename\0", filename,
		 "effect\0", effect,
		 NULL);
  }else if(AGS_IS_LV2_BRIDGE(gobject)){
    AgsLv2Plugin *lv2_plugin;

    xmlChar *filename, *effect;

    filename = xmlGetProp(node,
			  "plugin-file\0");

    effect = xmlGetProp(node,
			"effect\0");
    
    lv2_plugin = ags_lv2_manager_find_lv2_plugin(filename, effect);
  
    if(lv2_plugin != NULL &&
       (AGS_LV2_PLUGIN_IS_SYNTHESIZER & (lv2_plugin->flags)) != 0){
      
      gobject->audio->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
				AGS_AUDIO_INPUT_HAS_RECYCLING |
				AGS_AUDIO_SYNC |
				AGS_AUDIO_ASYNC |
				AGS_AUDIO_HAS_NOTATION | 
				AGS_AUDIO_NOTATION_DEFAULT);
      gobject->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
			 AGS_MACHINE_REVERSE_NOTATION);
    }

    g_object_set(gobject,
		 "filename\0", filename,
		 "effect\0", effect,
		 NULL);
  }
  
  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(gobject),
		     FALSE, FALSE, 0);

  /* reverse mapping */
  str = xmlGetProp(node,
		   "reverse-mapping\0");
  if(!g_strcmp0(str,
		"true\0")){
    gobject->audio->flags |= AGS_AUDIO_REVERSE_MAPPING;
  }
  
  /* retrieve channel allocation */
  str = xmlGetProp(node,
		   "channels\0");

  if(str != NULL){
    audio_channels = g_ascii_strtoull(str,
				      NULL,
				      10);
    gobject->audio->audio_channels = audio_channels;
  }

  str = xmlGetProp(node,
		   "input-pads\0");

  if(str != NULL){
    input_pads = g_ascii_strtoull(str,
				  NULL,
				  10);
    ags_audio_set_pads(gobject->audio,
		       AGS_TYPE_INPUT,
		       input_pads);
  }

  str = xmlGetProp(node,
		   "output-pads\0");

  if(str != NULL){
    output_pads = g_ascii_strtoull(str,
				   NULL,
				   10);
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
			   (xmlChar *) "ags-sf-effect-pad-list\0",
			   23)){
	GList *effect_pad;

	effect_pad = NULL;
	ags_simple_file_read_effect_pad_list(simple_file,
					     child,
					     &effect_pad);

	g_list_free(effect_pad);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-effect-list\0",
			   19)){
	AgsEffectBulk *effect_bulk;
	
	xmlNode *effect_list_child;

	xmlChar *filename, *effect;
	
	gboolean is_output;

	if(AGS_IS_LADSPA_BRIDGE(gobject) ||
	   AGS_IS_DSSI_BRIDGE(gobject) ||
	   AGS_IS_LV2_BRIDGE(gobject)){
	  child = child->next;
	  
	  continue;
	}
	
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
				       NULL,
				       10);
	      }
	      
	      channel = ags_channel_nth(gobject->audio->input,
					line);

	      /* retrieve bank */
	      bank_0 =
		bank_1 = 0;

	      str = xmlGetProp(pattern_list_child,
			       "bank-0\0");

	      if(str != NULL){
		bank_0 = g_ascii_strtoull(str,
					  NULL,
					  10);
	      }

	      str = xmlGetProp(pattern_list_child,
			       "bank-1\0");

	      if(str != NULL){
		bank_1 = g_ascii_strtoull(str,
					  NULL,
					  10);
	      }
	      
	      /* toggle pattern */
	      pattern = channel->pattern->data;

	      content = xmlNodeGetContent(pattern_list_child);
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

	g_list_free_full(gobject->audio->notation,
			 g_object_unref);

	gobject->audio->notation = notation;
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-automation-list\0",
			   23)){
	GList *automation;

	automation = NULL;
	ags_simple_file_read_automation_list(simple_file,
					     child,
					     &automation);

	g_list_free_full(gobject->audio->automation,
			 g_object_unref);

	gobject->audio->automation = automation;
      }
    }

    child = child->next;
  }

  ags_connectable_connect(AGS_CONNECTABLE(gobject));

  if(AGS_IS_LADSPA_BRIDGE(gobject)){
    ags_ladspa_bridge_load(gobject);
  }else if(AGS_IS_DSSI_BRIDGE(gobject)){
    ags_dssi_bridge_load(gobject);
  }else if(AGS_IS_LV2_BRIDGE(gobject)){
    ags_lv2_bridge_load(gobject);
  }

  /* retrieve midi mapping */
  str = xmlGetProp(node,
		   "audio-start-mapping\0");

  if(str != NULL){
    g_object_set(gobject->audio,
		 "audio-start-mapping\0", g_ascii_strtoull(str,
							   NULL,
							   10),
		 NULL);
  }

  str = xmlGetProp(node,
		   "audio-end-mapping\0");

  if(str != NULL){
    g_object_set(gobject->audio,
		 "audio-end-mapping\0", g_ascii_strtoull(str,
							 NULL,
							 10),
		 NULL);
  }

  str = xmlGetProp(node,
		   "midi-start-mapping\0");

  if(str != NULL){
    g_object_set(gobject->audio,
		 "midi-start-mapping\0", g_ascii_strtoull(str,
							  NULL,
							  10),
		 NULL);
  }

  str = xmlGetProp(node,
		   "midi-end-mapping\0");

  if(str != NULL){
    g_object_set(gobject->audio,
		 "midi-end-mapping\0", g_ascii_strtoull(str,
							NULL,
							10),
		 NULL);
  }
  
  gtk_widget_show_all(gobject);

  /* add audio to soundcard */
  list = ags_soundcard_get_audio(AGS_SOUNDCARD(soundcard));
  g_object_ref(G_OBJECT(gobject->audio));
  
  list = g_list_prepend(list,
			gobject->audio);
  ags_soundcard_set_audio(AGS_SOUNDCARD(soundcard),
			  list);

  /* connect AgsAudio */
  ags_connectable_connect(AGS_CONNECTABLE(gobject->audio));

  /* launch AgsMachine */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "file\0", simple_file,
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

  xmlChar *str;

  auto void ags_simple_file_read_drum_launch(AgsSimpleFile *simpleFile, xmlNode *node, AgsDrum *drum);
  auto void ags_simple_file_read_matrix_launch(AgsSimpleFile *simpleFile, xmlNode *node, AgsMatrix *matrix);
  auto void ags_simple_file_read_ffplayer_launch(AgsSimpleFile *simpleFile, xmlNode *node, AgsFFPlayer *ffplayer);
  auto void ags_simple_file_read_dssi_bridge_launch(AgsSimpleFile *simpleFile, xmlNode *node, AgsDssiBridge *dssi_bridge);

  auto void ags_simple_file_read_effect_bridge_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsEffectBridge *effect_bridge);
  auto void ags_simple_file_read_effect_bulk_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsEffectBulk *effect_bulk);
  auto void ags_simple_file_read_bulk_member_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsBulkMember *bulk_member);
  
  void ags_simple_file_read_drum_launch(AgsSimpleFile *simpleFile, xmlNode *node, AgsDrum *drum){
    xmlChar *str;
    
    guint bank_0, bank_1;

    /* bank 0 */
    bank_0 = 0;
    str = xmlGetProp(node,
		     "bank-0\0");

    if(str != NULL){      
      bank_0 = g_ascii_strtod(str,
			      NULL);

      if(bank_0 < 4){
	gtk_toggle_button_set_active(drum->index0[bank_0],
				     TRUE);
      }
    }

    /* bank 1 */
    bank_1 = 0;
    str = xmlGetProp(node,
		     "bank-1\0");

    if(str != NULL){      
      bank_1 = g_ascii_strtod(str,
			      NULL);

      if(bank_1 < 12){
	gtk_toggle_button_set_active(drum->index1[bank_1],
				     TRUE);
      }
    }

    /* loop */
    str = xmlGetProp(node,
		     "loop\0");
    
    if(str != NULL &&
       g_strcmp0(str,
		 "false\0")){
      gtk_toggle_button_set_active(drum->loop_button,
				   TRUE);
    }

    /* length */
    str = xmlGetProp(node,
		     "length\0");

    if(str != NULL){
      guint length;

      length = g_ascii_strtoull(str,
				NULL,
				10);
      
      gtk_adjustment_set_value(drum->length_spin->adjustment,
			       (gdouble) length);
    }
  }
  
  void ags_simple_file_read_matrix_launch(AgsSimpleFile *simpleFile, xmlNode *node, AgsMatrix *matrix){
    xmlChar *str;
    
    guint bank_0, bank_1;
    
    /* bank 1 */
    bank_1 = 0;
    str = xmlGetProp(node,
		     "bank-1\0");

    if(str != NULL){      
      bank_1 = g_ascii_strtod(str,
			      NULL);

      if(bank_1 < 9){
	gtk_toggle_button_set_active(matrix->index[bank_1],
				     TRUE);
      }
    }

    /* loop */
    str = xmlGetProp(node,
		     "loop\0");
    
    if(str != NULL &&
       g_strcmp0(str,
		 "false\0")){
      gtk_toggle_button_set_active(matrix->loop_button,
				   TRUE);
    }

    /* length */
    str = xmlGetProp(node,
		     "length\0");

    if(str != NULL){
      guint length;

      length = g_ascii_strtoull(str,
				NULL,
				10);
      
      gtk_adjustment_set_value(matrix->length_spin->adjustment,
			       (gdouble) length);
    }
  }

  void ags_simple_file_read_ffplayer_launch(AgsSimpleFile *simpleFile, xmlNode *node, AgsFFPlayer *ffplayer){
    GtkTreeModel *model;
    GtkTreeIter iter;

    xmlChar *str;
    gchar *value;

    str = xmlGetProp(node,
		     "filename\0");
    
    ags_ffplayer_open_filename(ffplayer,
			       str);

    while(g_static_rec_mutex_unlock_full(((IpatchItem *) (ffplayer->ipatch->base))->mutex) != 0);    
    
    /* preset */
    model = gtk_combo_box_get_model(ffplayer->preset);

    str = xmlGetProp(node,
		     "preset\0");

    if(gtk_tree_model_get_iter_first(model, &iter)){
      do{
	gtk_tree_model_get(model, &iter,
			   0, &value,
			   -1);

	if(!g_strcmp0(str,
		      value)){
	  gtk_combo_box_set_active_iter((GtkComboBox *) ffplayer->preset,
					&iter);
	  break;
	}
      }while(gtk_tree_model_iter_next(model,
				      &iter));
    }

    while(g_static_rec_mutex_unlock_full(((IpatchItem *) (ffplayer->ipatch->base))->mutex) != 0);

    /* instrument */
    model = gtk_combo_box_get_model(ffplayer->instrument);

    str = xmlGetProp(node,
		     "instrument\0");

    if(gtk_tree_model_get_iter_first(model, &iter)){
      do{
	gtk_tree_model_get(model, &iter,
			   0, &value,
			   -1);

	if(!g_strcmp0(str,
		      value)){
	  gtk_combo_box_set_active_iter((GtkComboBox *) ffplayer->instrument,
					&iter);
	  break;
	}
      }while(gtk_tree_model_iter_next(model,
				      &iter));
    }

    while(g_static_rec_mutex_unlock_full(((IpatchItem *) (ffplayer->ipatch->base))->mutex) != 0);
  }

  void ags_simple_file_read_dssi_bridge_launch(AgsSimpleFile *simpleFile, xmlNode *node, AgsDssiBridge *dssi_bridge){
    GtkTreeModel *model;
    GtkTreeIter iter;

    xmlChar *str;
    gchar *value;

    /* program */
    model = gtk_combo_box_get_model(dssi_bridge->program);

    str = xmlGetProp(node,
		     "program\0");

    if(gtk_tree_model_get_iter_first(model, &iter)){
      do{
	gtk_tree_model_get(model, &iter,
			   0, &value,
			   -1);

	if(!g_strcmp0(str,
		      value)){
	  gtk_combo_box_set_active_iter((GtkComboBox *) dssi_bridge->program,
					&iter);
	  break;
	}
      }while(gtk_tree_model_iter_next(model,
				      &iter));
    }
  }

  void ags_simple_file_read_effect_bridge_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsEffectBridge *effect_bridge){
    AgsEffectBulk *effect_bulk;
    
    xmlNode *child;

    xmlChar *str;
    
    gboolean is_output;
	
    is_output = TRUE;
    str = xmlGetProp(node,
		     "is-output\0");
    
    if(str != NULL &&
       !g_strcmp0(str,
		  "false\0")){
      is_output = FALSE;
    }

    if(is_output){
      effect_bulk = AGS_EFFECT_BRIDGE(machine->bridge)->bulk_output;
    }else{
      effect_bulk = AGS_EFFECT_BRIDGE(machine->bridge)->bulk_input;
    }
	
    /* effect list children */
    child = node->children;
	
    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!xmlStrncmp(child->name,
		       (xmlChar *) "ags-sf-effect\0",
		       14)){
	  ags_simple_file_read_effect_bulk_launch(simple_file, child, effect_bulk);
	}
      }

      child = child->next;
    }
  }
  
  void ags_simple_file_read_effect_bulk_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsEffectBulk *effect_bulk){
    xmlNode *child;
    
    xmlChar *filename, *effect;

    filename = xmlGetProp(node,
			  "filename\0");
	      
    effect = xmlGetProp(node,
			"effect\0");

    /* effect list children */
    child = node->children;

    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!xmlStrncmp(child->name,
		       (xmlChar *) "ags-sf-control\0",
		       15)){
	  GList *list_start, *list;
		    
	  xmlChar *specifier;

	  specifier = xmlGetProp(child,
				 "specifier\0");
	  
	  list_start = gtk_container_get_children(GTK_CONTAINER(effect_bulk->table));
	  list = list_start;

	  while(list != NULL){
	    if(AGS_IS_BULK_MEMBER(list->data)){
	      AgsBulkMember *bulk_member;
	      
	      bulk_member = AGS_BULK_MEMBER(list->data);

	      if(!g_strcmp0(bulk_member->filename,
			    filename) &&
		 !g_strcmp0(bulk_member->effect,
			    effect) &&
		 !g_strcmp0(bulk_member->specifier,
			    specifier)){
		ags_simple_file_read_bulk_member_launch(simple_file, child, bulk_member);
		break;
	      }
	    }

	    list = list->next;
	  }

	  g_list_free(list_start);
	}
      }

      child = child->next;
    }
  }
	
  void ags_simple_file_read_bulk_member_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsBulkMember *bulk_member)
  {
    GtkWidget *child_widget;

    xmlChar *str;
    
    gdouble val;

    str = xmlGetProp(node,
		     "value\0");
    
    if(str != NULL){
      val = g_ascii_strtod(str,
			   NULL);
		      
    }

    child_widget = gtk_bin_get_child(bulk_member);

    if(GTK_IS_RANGE(child_widget)){
      gtk_adjustment_set_value(GTK_RANGE(child_widget)->adjustment,
			       val);
    }else if(GTK_IS_SPIN_BUTTON(child_widget)){
      gtk_adjustment_set_value(GTK_SPIN_BUTTON(child_widget)->adjustment,
			       val);
    }else if(AGS_IS_DIAL(child_widget)){
      gtk_adjustment_set_value(AGS_DIAL(child_widget)->adjustment,
			       val);
      ags_dial_draw(child_widget);
    }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
      gtk_toggle_button_set_active(child_widget,
				   ((val != 0.0) ? TRUE: FALSE));
    }else{
      g_warning("ags_simple_file_read_bulk_member_launch() - unknown bulk member type\0");
    }			  
  }  
  
  if(AGS_IS_DRUM(machine)){
    ags_simple_file_read_drum_launch(file_launch->file, file_launch->node, machine);
  }else if(AGS_IS_MATRIX(machine)){
    ags_simple_file_read_matrix_launch(file_launch->file, file_launch->node, machine);
  }else if(AGS_IS_FFPLAYER(machine)){
    ags_simple_file_read_ffplayer_launch(file_launch->file, file_launch->node, machine);
  }else if(AGS_IS_DSSI_BRIDGE(machine)){
    ags_simple_file_read_dssi_bridge_launch(file_launch->file, file_launch->node, machine);
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
					   &property_start);

	property = property_start;

	while(property != NULL){
	  g_object_set_property(machine,
				((GParameter *) property->data)->name,
				&(((GParameter *) property->data)->value));
	  
	  property = property->next;
	}
	
	g_list_free_full(property_start,
			 g_free);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-effect-list\0",
			   19)){
	ags_simple_file_read_effect_bridge_launch(file_launch->file, child, machine->bridge);
      }
    }

    child = child->next;
  }
}

void
ags_simple_file_read_pad_list(AgsSimpleFile *simple_file, xmlNode *node, GList **pad)
{
  AgsPad *current;
  
  xmlNode *child;

  GList *list;

  guint i;
  
  child = node->children;
  list = NULL;

  i = 0;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-pad\0",
		     11)){
	current = NULL;

	if(*pad != NULL){
	  GList *iter;

	  iter = g_list_nth(*pad,
			    i);
	  current = iter->data;
	}
	
	ags_simple_file_read_pad(simple_file, child, &current);
	list = g_list_prepend(list, current);

	i++;
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
  AgsFileIdRef *file_id_ref;
  
  xmlNode *child;

  GList *list, *list_start;
    
  xmlChar *str;

  guint nth_pad;
  
  if(*pad != NULL){
    gobject = *pad;

    nth_pad = gobject->channel->pad;
  }else{
    file_id_ref = ags_simple_file_find_id_ref_by_node(simple_file,
						      node->parent->parent);
    machine = file_id_ref->ref;
    
    if(!AGS_IS_MACHINE(machine)){
      return;
    }
    
    /* get nth pad */
    nth_pad = 0;
    str = xmlGetProp(node,
		     "nth-pad\0");

    if(str != NULL){
      nth_pad = g_ascii_strtoull(str,
				 NULL,
				 10);
    }

    /* retrieve pad */
    gobject = NULL;
    str = xmlGetProp(node->parent,
		     "is-output\0");

    list_start = NULL;
    
    if(!g_strcmp0(str,
		  "false\0")){
      if(machine->input != NULL){
	list_start = gtk_container_get_children(machine->input);
      }
    }else{
      if(machine->output != NULL){
	list_start = gtk_container_get_children(machine->output);
      }
    }

    list = g_list_nth(list_start,
		      nth_pad);

    if(list != NULL){
      gobject = AGS_PAD(list->data);
    }

    if(list_start != NULL){
      g_list_free(list_start);
    }
  }
  
  ags_simple_file_add_id_ref(simple_file,
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
				 g_strcmp0(str,
					   "false\0"));

    str = xmlGetProp(file_launch->node,
		     "mute\0");
    
    gtk_toggle_button_set_active(pad->mute,
				 g_strcmp0(str,
					   "false\0"));

    str = xmlGetProp(file_launch->node,
		     "solo\0");
    
    gtk_toggle_button_set_active(pad->solo,
				 g_strcmp0(str,
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
					   &property_start);

	property = property_start;

	while(property != NULL){
	  g_object_set_property(pad,
				((GParameter *) property->data)->name,
				&(((GParameter *) property->data)->value));
	  
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

  guint i;
  
  child = node->children;
  list = NULL;

  i = 0;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-line\0",
		     12)){
	current = NULL;

	if(*line != NULL){
	  GList *iter;

	  iter = g_list_nth(*line,
			    i);
	  current = iter->data;
	}
	
	ags_simple_file_read_line(simple_file, child, &current);
	list = g_list_prepend(list, current);

	i++;
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
  GObject *gobject;

  AgsFileLaunch *file_launch;
  AgsFileIdRef *file_id_ref;
  
  xmlNode *child;

  xmlChar *str;
  
  guint nth_line;

  auto void ags_simple_file_read_oscillator(AgsSimpleFile *simple_file, xmlNode *node, AgsOscillator *oscillator);
  auto void ags_simple_file_read_line_member(AgsSimpleFile *simple_file, xmlNode *node, AgsLineMember *line_member);

  void ags_simple_file_read_oscillator(AgsSimpleFile *simple_file, xmlNode *node, AgsOscillator *oscillator){
    xmlChar *str;

    gdouble val;
    guint nth;

    str = xmlGetProp(node,
		     "wave\0");

    if(str != NULL){
      
      nth = g_ascii_strtoull(str,
			     NULL,
			     10);
      gtk_combo_box_set_active(oscillator->wave,
			       nth);
    }

    str = xmlGetProp(node,
		     "attack\0");

    if(str != NULL){
      val = g_ascii_strtod(str,
			   NULL);
      gtk_adjustment_set_value(oscillator->attack->adjustment,
			       val);
    }

    str = xmlGetProp(node,
		     "frequency\0");

    if(str != NULL){
      val = g_ascii_strtod(str,
			   NULL);
      gtk_adjustment_set_value(oscillator->frequency->adjustment,
			       val);
    }

    str = xmlGetProp(node,
		     "length\0");

    if(str != NULL){
      val = g_ascii_strtod(str,
			   NULL);
      gtk_adjustment_set_value(oscillator->frame_count->adjustment,
			       val);
    }

    str = xmlGetProp(node,
		     "phase\0");

    if(str != NULL){
      val = g_ascii_strtod(str,
			   NULL);
      gtk_adjustment_set_value(oscillator->phase->adjustment,
			       val);
    }

    str = xmlGetProp(node,
		     "volume\0");

    if(str != NULL){
      val = g_ascii_strtod(str,
			   NULL);
      gtk_adjustment_set_value(oscillator->volume->adjustment,
			      val);
    }
  }
  
  void ags_simple_file_read_line_member(AgsSimpleFile *simple_file, xmlNode *node, AgsLineMember *line_member){
    GtkWidget *child_widget;

    xmlChar *str;
    
    gdouble val;

    line_member->flags |= AGS_LINE_MEMBER_APPLY_INITIAL;
    
    str = xmlGetProp(node,
		     "control-type\0");

    if(str != NULL){
      g_object_set(line_member,
		   "widget-type\0", g_type_from_name(str),
		   NULL);
    }

    child_widget = gtk_bin_get_child(line_member);
    
    /* apply value */
    str = xmlGetProp(node,
		     "value\0");

    if(str != NULL){
       if(AGS_IS_DIAL(child_widget)){
	val = g_ascii_strtod(str,
			     NULL);
	gtk_adjustment_set_value(AGS_DIAL(child_widget)->adjustment,
				 val);
      }else if(GTK_IS_RANGE(child_widget)){
	val = g_ascii_strtod(str,
			     NULL);    
      
	gtk_adjustment_set_value(GTK_RANGE(child_widget)->adjustment,
				 val);
      }else if(GTK_IS_SPIN_BUTTON(child_widget)){
	val = g_ascii_strtod(str,
			     NULL);    
      
	gtk_adjustment_set_value(GTK_SPIN_BUTTON(child_widget)->adjustment,
				 val);
      }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
	gtk_toggle_button_set_active(child_widget,
				     ((g_strcmp0(str, "false\0")) ? TRUE: FALSE));
      }else{
	g_warning("ags_simple_file_read_line() - unknown line member type\0");
      }
    }
  }
  
  if(*line != NULL){
    gobject = *line;

    nth_line = AGS_LINE(gobject)->channel->line;
  }else{
    GList *list_start, *list;
    
    file_id_ref = ags_simple_file_find_id_ref_by_node(simple_file,
						      node->parent->parent);
    pad = file_id_ref->ref;
    
    if(!AGS_IS_PAD(pad)){
      pad = NULL;
    }
        
    /* get nth-line */
    nth_line = 0;
    str = xmlGetProp(node,
		     "nth-line\0");

    if(str != NULL){
      nth_line = g_ascii_strtoull(str,
				  NULL,
				  10);
    }

    /* retrieve line or channel */
    gobject = NULL;

    if(pad != NULL){
      list_start = gtk_container_get_children(pad->expander_set);

      list = list_start;

      while(list != NULL){
	if(AGS_IS_LINE(list->data) &&
	   AGS_LINE(list->data)->channel->line == nth_line){
	  gobject = list->data;
	  
	  break;
	}

	list = list->next;
      }

      if(list_start != NULL){
	g_list_free(list_start);

      }
    }else{
      AgsMachine *machine;

      gboolean is_output;
      
      file_id_ref = ags_simple_file_find_id_ref_by_node(simple_file,
							node->parent->parent->parent->parent);
      machine = file_id_ref->ref;
    
      if(!AGS_IS_MACHINE(machine)){
	return;
      }

      is_output = TRUE;
      str = xmlGetProp(node->parent->parent->parent,
		       "is-output\0");

      if(str != NULL &&
	 !g_strcmp0(str,
		    "false\0")){
	is_output = FALSE;
      }

      if(is_output){
	gobject = ags_channel_nth(machine->audio->output,
				  nth_line);
      }else{
	gobject = ags_channel_nth(machine->audio->input,
				  nth_line);
      }
    }
  }

  ags_simple_file_add_id_ref(simple_file,
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
		     19) &&
	 AGS_IS_LINE(gobject)){
	xmlNode *effect_list_child;

	GList *mapped_filename, *mapped_effect;
	
	/* effect list children */
	effect_list_child = child->children;

	mapped_filename = NULL;
	mapped_effect = NULL;
	
	while(effect_list_child != NULL){
	  if(effect_list_child->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(effect_list_child->name,
			   (xmlChar *) "ags-sf-effect\0",
			   14)){
	      xmlNode *effect_child;

	      xmlChar *filename, *effect;

	      filename = xmlGetProp(effect_list_child,
				    "filename\0");
	      
	      effect = xmlGetProp(effect_list_child,
				  "effect\0");
	      
	      if(filename != NULL &&
		 effect != NULL){
		if(g_list_find_custom(mapped_filename,
				      filename,
				      g_strcmp0) == NULL ||
		   g_list_find_custom(mapped_effect,
				      effect,
				      g_strcmp0) == NULL){
		  mapped_filename = g_list_prepend(mapped_filename,
						   filename);
		  mapped_effect = g_list_prepend(mapped_effect,
						 effect);

		  if(AGS_IS_LINE(gobject)){
		    ags_channel_add_effect(AGS_LINE(gobject)->channel,
					   filename,
					   effect);
		  }else if(AGS_IS_CHANNEL(gobject)){
		    ags_channel_add_effect(gobject,
					   filename,
					   effect);
		  }
		}
	      }
	      
	      /* effect list children */
	      effect_child = effect_list_child->children;

	      while(effect_child != NULL){
		if(effect_child->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(effect_child->name,
				 (xmlChar *) "ags-sf-control\0",
				 15)){
		    AgsLineMember *line_member;

		    GList *list_start, *list;
		    
		    xmlChar *specifier;

		    specifier = xmlGetProp(effect_child,
					   "specifier\0");
		    		       
		    list =
		      list_start = gtk_container_get_children(AGS_LINE(gobject)->expander->table);

		    while(list != NULL){
		      if(AGS_IS_LINE_MEMBER(list->data)){
			line_member = AGS_LINE_MEMBER(list->data);
			
			if(((filename == NULL && effect == NULL) ||
			    (strlen(filename) == 0 && strlen(effect) == 0) ||
			     (!g_strcmp0(line_member->filename,
					 filename) &&
			      !g_strcmp0(line_member->effect,
					 effect))) &&
			   !g_strcmp0(line_member->specifier,
				      specifier)){
			  ags_simple_file_read_line_member(simple_file,
							   effect_child,
							   line_member);
			    
			  break;
			}
		      }
			
		      list = list->next;		    
		    }

		    if(list_start != NULL){
		      g_list_free(list_start);
		    }
		  }
		}

		effect_child = effect_child->next;
	      }
	    }
	  }

	  effect_list_child = effect_list_child->next;
	}
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-oscillator\0",
			   15)){
	ags_simple_file_read_oscillator(simple_file, child, AGS_SYNTH_INPUT_LINE(gobject)->oscillator);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-property-list\0",
			   14)){
	GList *property_start, *property;

	property_start = NULL;
	ags_simple_file_read_property_list(file_launch->file,
					   child,
					   &property_start);

	property = property_start;

	while(property != NULL){
	  g_object_set_property(pad,
				((GParameter *) property->data)->name,
				&(((GParameter *) property->data)->value));
	  
	  property = property->next;
	}
	
	g_list_free_full(property_start,
			 g_free);
      }
    }

    child = child->next;
  }

  /* launch AgsLine */
  if(AGS_IS_LINE(gobject)){
    file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
						 "file\0", simple_file,
						 "node\0", node,
						 NULL);
    g_signal_connect(G_OBJECT(file_launch), "start\0",
		     G_CALLBACK(ags_simple_file_read_line_launch), gobject);
    ags_simple_file_add_launch(simple_file,
			       (GObject *) file_launch);
  }else if(AGS_IS_CHANNEL(gobject)){
    file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
						 "file\0", simple_file,
						 "node\0", node,
						 NULL);
    g_signal_connect(G_OBJECT(file_launch), "start\0",
		     G_CALLBACK(ags_simple_file_read_channel_line_launch), gobject);
    ags_simple_file_add_launch(simple_file,
			       (GObject *) file_launch);
  }else{
    g_warning("ags_simple_file_read_line() - failed\0");
  }
}

void
ags_simple_file_read_line_launch(AgsFileLaunch *file_launch,
				 AgsLine *line)
{
  AgsMachine *machine;
  AgsChannel *channel, *link;
  
  xmlNode *child;

  GList *xpath_result;

  xmlChar *str;
  
  guint nth_line;
  gboolean is_output;

  machine = gtk_widget_get_ancestor(line,
				    AGS_TYPE_MACHINE);
  
  /* link or file */
  is_output = TRUE;
  str = xmlGetProp(file_launch->node->parent->parent->parent,
		   "is-output\0");

  if(str != NULL &&
     !g_strcmp0(str,
		"false\0")){
    is_output = FALSE;
  }

  nth_line = 0;
  str = xmlGetProp(file_launch->node,
		   "nth-line\0");

  if(str != NULL){
    nth_line = g_ascii_strtoull(str,
				NULL,
				10);
  }
  
  if(is_output){
    channel = ags_channel_nth(machine->audio->output,
			      nth_line);
  }else{
    channel = ags_channel_nth(machine->audio->input,
			      nth_line);
  }

  str = xmlGetProp(file_launch->node,
		   "filename\0");
    
  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "file://\0",
			  7)){
    AgsAudioFile *audio_file;
    AgsAudioFileLink *file_link;
    
    GList *audio_signal_list;
    
    gchar *filename;

    guint file_channel;

    /* filename */
    filename = g_strdup(&(str[7]));

    /* audio channel to read */
    file_channel = 0;

    str = xmlGetProp(file_launch->node,
		     "file-channel\0");

    if(str != NULL){
      file_channel = g_ascii_strtoull(str,
				      NULL,
				      10);
    }

    /* read audio signal */
    audio_file = ags_audio_file_new(filename,
				    machine->audio->soundcard,
				    file_channel, 1);

    ags_audio_file_open(audio_file);
    ags_audio_file_read_audio_signal(audio_file);

    /* add audio signal */
    audio_signal_list = audio_file->audio_signal;

    file_link = g_object_new(AGS_TYPE_AUDIO_FILE_LINK,
			     "filename\0", filename,
			     "audio-channel\0", file_channel,
			     NULL);
    g_object_set(G_OBJECT(channel),
		 "file-link", file_link,
		 NULL);

    AGS_AUDIO_SIGNAL(audio_signal_list->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;

    ags_recycling_add_audio_signal(channel->first_recycling,
				   audio_signal_list->data);
  }else{
    str = xmlGetProp(file_launch->node,
		     "link\0");
    xpath_result = NULL;
    
    if(str != NULL){
      xpath_result = ags_simple_file_find_id_ref_by_xpath(file_launch->file,
							  str);
    }
    
    while(xpath_result != NULL){
      AgsFileIdRef *file_id_ref;

      file_id_ref = xpath_result->data;

      if(AGS_IS_CHANNEL(file_id_ref->ref)){      
	GError *error;
    
	link = file_id_ref->ref;
	
	error = NULL;
	ags_channel_set_link(channel,
			     link,
			     &error);
	
	if(error != NULL){
	  g_warning("ags_simple_file_read_line_launch() - %s", error->message);
	}

	break;
      }else if(AGS_IS_LINE(file_id_ref->ref)){
	GError *error;
    
	link = AGS_LINE(file_id_ref->ref)->channel;
	
	error = NULL;
	ags_channel_set_link(channel,
			     link,
			     &error);

	if(error != NULL){
	  g_warning("ags_simple_file_read_line_launch() - %s", error->message);
	}

	break;
      }
      
      xpath_result = xpath_result->next;
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
					   &property_start);

	property = property_start;

	while(property != NULL){
	  g_object_set_property(line,
				((GParameter *) property->data)->name,
				&(((GParameter *) property->data)->value));
	  
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
ags_simple_file_read_channel_line_launch(AgsFileLaunch *file_launch,
					 AgsChannel *channel)
{
  AgsChannel *link;
  
  xmlNode *child;

  GList *xpath_result;

  xmlChar *str;

  str = xmlGetProp(file_launch->node,
		   "filename\0");
    
  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "file://\0",
			  7)){
    AgsAudioFile *audio_file;
    AgsAudioFileLink *file_link;
    
    GList *audio_signal_list;
    
    gchar *filename;

    guint file_channel;

    /* filename */
    filename = g_strdup(&(str[7]));

    /* audio channel to read */
    file_channel = 0;

    str = xmlGetProp(file_launch->node,
		     "file-channel\0");

    if(str != NULL){
      file_channel = g_ascii_strtoull(str,
				      NULL,
				      10);
    }

    /* read audio signal */
    audio_file = ags_audio_file_new(filename,
				    channel->soundcard,
				    file_channel, 1);

    ags_audio_file_open(audio_file);
    ags_audio_file_read_audio_signal(audio_file);

    /* add audio signal */
    audio_signal_list = audio_file->audio_signal;

    file_link = g_object_new(AGS_TYPE_AUDIO_FILE_LINK,
			     "filename\0", filename,
			     "audio-channel\0", file_channel,
			     NULL);
    g_object_set(G_OBJECT(channel),
		 "file-link", file_link,
		 NULL);

    AGS_AUDIO_SIGNAL(audio_signal_list->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;

    ags_recycling_add_audio_signal(channel->first_recycling,
				   audio_signal_list->data);
  }else{
    str = xmlGetProp(file_launch->node,
		     "link\0");
    xpath_result = NULL;
    
    if(str != NULL){
      xpath_result = ags_simple_file_find_id_ref_by_xpath(file_launch->file,
							  str);
    }
    
    while(xpath_result != NULL){
      AgsFileIdRef *file_id_ref;

      file_id_ref = xpath_result->data;

      if(AGS_IS_CHANNEL(file_id_ref->ref)){      
	GError *error;
    
	link = file_id_ref->ref;
	
	error = NULL;
	ags_channel_set_link(channel,
			     link,
			     &error);
	
	if(error != NULL){
	  g_warning("ags_simple_file_read_line_launch() - %s", error->message);
	}

	break;
      }else if(AGS_IS_LINE(file_id_ref->ref)){
	GError *error;
    
	link = AGS_LINE(file_id_ref->ref)->channel;
	
	error = NULL;
	ags_channel_set_link(channel,
			     link,
			     &error);

	if(error != NULL){
	  g_warning("ags_simple_file_read_line_launch() - %s", error->message);
	}

	break;
      }
      
      xpath_result = xpath_result->next;
    }
  }
}

void
ags_simple_file_read_effect_pad_list(AgsSimpleFile *simple_file, xmlNode *node, GList **effect_pad)
{
  AgsEffectPad *current;
  GList *list;
  
  xmlNode *child;

  child = node->children;
  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-effect-pad\0",
		     11)){
	current = NULL;
	ags_simple_file_read_effect_pad(simple_file, child, &current);
	list = g_list_prepend(list, current);
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *effect_pad = list;
}

void
ags_simple_file_read_effect_pad(AgsSimpleFile *simple_file, xmlNode *node, AgsEffectPad **effect_pad)
{
  AgsEffectPad *gobject;

  AgsFileLaunch *file_launch;

  xmlNode *child;
  
  if(*effect_pad != NULL){
    gobject = *effect_pad;
  }else{
    return;
  }

  /* children */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-effect-line-list\0",
		     24)){
	GList *list;

	list = NULL;
	ags_simple_file_read_effect_line_list(simple_file, child, &list);

	g_list_free(list);
      }
    }

    child = child->next;
  }

  /* launch AgsEffectPad */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "node\0", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start\0",
		   G_CALLBACK(ags_simple_file_read_effect_pad_launch), gobject);
  ags_simple_file_add_launch(simple_file,
			     (GObject *) file_launch);
}

void
ags_simple_file_read_effect_pad_launch(AgsFileLaunch *file_launch,
				       AgsEffectPad *effect_pad)
{
  /* empty */
}

void
ags_simple_file_read_effect_line_list(AgsSimpleFile *simple_file, xmlNode *node, GList **effect_line)
{
  AgsEffectLine *current;
  
  xmlNode *child;

  GList *list;
  
  guint i;

  child = node->children;
  list = NULL;

  i = 0;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-effect-line\0",
		     11)){
	current = NULL;

	if(*effect_line != NULL){
	  GList *iter;

	  iter = g_list_nth(*effect_line,
			    i);
	  current = iter->data;
	}
	
	ags_simple_file_read_effect_line(simple_file, child, &current);
	list = g_list_prepend(list, current);

	i++;
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *effect_line = list;
}

void
ags_simple_file_read_effect_line(AgsSimpleFile *simple_file, xmlNode *node, AgsEffectLine **effect_line)
{
  AgsEffectLine *gobject;

  AgsFileLaunch *file_launch;

  xmlNode *child;

  xmlChar *str;
  
  if(*effect_line != NULL){
    gobject = *effect_line;
  }else{
    return;
  }

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
	      
	      xmlChar *filename, *effect;

	      filename = xmlGetProp(effect_list_child,
				    "filename\0");
	      
	      effect = xmlGetProp(effect_list_child,
				  "effect\0");
	      
	      /* effect list children */
	      effect_child = child->children;

	      while(effect_child != NULL){
		if(effect_child->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(effect_child->name,
				 (xmlChar *) "ags-sf-control\0",
				 15)){
		    AgsLineMember *line_member;
		    GtkWidget *child_widget;

		    GList *list_start, *list;
		    
		    xmlChar *specifier;

		    gdouble val;

		    specifier = xmlGetProp(effect_child,
					   "specifier\0");

		    str = xmlGetProp(effect_child,
				     "value\0");

		    if(str != NULL){
		      val = g_ascii_strtod(str,
					   NULL);
		      
		      list =
			list_start = gtk_container_get_children(gobject->table);

		      while(list != NULL){
			if(AGS_IS_LINE_MEMBER(list->data)){
			  line_member = AGS_LINE_MEMBER(list->data);
			
			  if(!g_strcmp0(line_member->filename,
					filename) &&
			     !g_strcmp0(line_member->effect,
					effect) &&
			     !g_strcmp0(line_member->specifier,
					specifier)){
			    child_widget = gtk_bin_get_child(line_member);

			    if(GTK_IS_RANGE(child_widget)){
			      gtk_adjustment_set_value(GTK_RANGE(child_widget)->adjustment,
						       val);
			    }else if(GTK_IS_SPIN_BUTTON(child_widget)){
			      gtk_adjustment_set_value(GTK_SPIN_BUTTON(child_widget)->adjustment,
						       val);
			    }else if(AGS_IS_DIAL(child_widget)){
			      gtk_adjustment_set_value(AGS_DIAL(child_widget)->adjustment,
						       val);
			      ags_dial_draw(child_widget);
			    }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
			      gtk_toggle_button_set_active(child_widget,
							   ((val != 0.0) ? TRUE: FALSE));
			    }else{
			      g_warning("ags_simple_file_read_effect_line() - unknown line member type\0");
			    }
			  
			    break;
			  }
			}

			list = list->next;
		      }
		    
		      g_list_free(list_start);
		    }
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

  /* launch AgsEffectLine */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "node\0", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start\0",
		   G_CALLBACK(ags_simple_file_read_effect_line_launch), gobject);
  ags_simple_file_add_launch(simple_file,
			     (GObject *) file_launch);
}

void
ags_simple_file_read_effect_line_launch(AgsFileLaunch *file_launch,
					AgsEffectLine *effect_line)
{
  /* empty */
}

void
ags_simple_file_read_editor(AgsSimpleFile *simple_file, xmlNode *node, AgsEditor **editor)
{
  AgsEditor *gobject;
  
  AgsFileLaunch *file_launch;

  if(*editor != NULL){
    gobject = *editor;
  }else{
    return;
  }
  
  /* launch AgsLine */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "file\0", simple_file,
					       "node\0", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start\0",
		   G_CALLBACK(ags_simple_file_read_editor_launch), gobject);
  ags_simple_file_add_launch(simple_file,
			     (GObject *) file_launch);
}

void
ags_simple_file_read_editor_launch(AgsFileLaunch *file_launch,
				   AgsEditor *editor)
{
  xmlNode *child;
  
  xmlChar *str;

  str = xmlGetProp(file_launch->node,
		   "zoom\0");

  if(str != NULL){
    GtkTreeModel *model;
    GtkTreeIter iter;

    gchar *value;

    model = gtk_combo_box_get_model(editor->toolbar->zoom);
    
    if(gtk_tree_model_get_iter_first(model, &iter)){
      do{
	gtk_tree_model_get(model, &iter,
			   0, &value,
			   -1);

	if(!g_strcmp0(str,
		      value)){
	  gtk_combo_box_set_active_iter((GtkComboBox *) editor->toolbar->zoom,
					&iter);
	  break;
	}
      }while(gtk_tree_model_iter_next(model,
				      &iter));
    }
  }

  /* children */
  child = file_launch->node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-property-list\0",
		     21)){
	GList *list_start, *list;
	GList *property_start, *property;

	property_start = NULL;
	ags_simple_file_read_property_list(file_launch->file,
					   child,
					   &property_start);

	property = property_start;

	while(property != NULL){
  	  if(!g_strcmp0(((GParameter *) property->data)->name,
			"machine\0")){
	    AgsMachine *machine;

	    GList *file_id_ref;
	    GList *list_start, *list;

	    str = g_value_get_string(&(((GParameter *) property->data)->value));

	    if(str != NULL){
	      file_id_ref = ags_simple_file_find_id_ref_by_xpath(file_launch->file,
								 str);
	      machine = AGS_FILE_ID_REF(file_id_ref->data)->ref;
	    
	      ags_machine_selector_add_index(editor->machine_selector);

	      list_start = gtk_container_get_children(editor->machine_selector);
	      list = g_list_last(list_start);

	      gtk_button_clicked(list->data);
	      ags_machine_selector_link_index(editor->machine_selector,
					      machine);
	      
	      g_list_free(list_start);
	    }
	  }else{
	    g_object_set_property(editor,
				  ((GParameter *) property->data)->name,
				  &(((GParameter *) property->data)->value));
	  }
	  
	  property = property->next;
	}

	list_start = gtk_container_get_children(editor->machine_selector);
	list = list_start->next;

	if(list != NULL){
	  gtk_button_clicked(list->data);
	}
	
	g_list_free(list_start);
	g_list_free_full(property_start,
			 g_free);
      }
    }

    child = child->next;
  }
}

void
ags_simple_file_read_automation_editor(AgsSimpleFile *simple_file, xmlNode *node, AgsAutomationEditor **automation_editor)
{
  AgsAutomationEditor *gobject;
  
  AgsFileLaunch *file_launch;

  if(*automation_editor != NULL){
    gobject = *automation_editor;
  }else{
    return;
  }

  /* launch AgsLine */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "file\0", simple_file,
					       "node\0", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start\0",
		   G_CALLBACK(ags_simple_file_read_automation_editor_launch), gobject);
  ags_simple_file_add_launch(simple_file,
			     (GObject *) file_launch);
}

void
ags_simple_file_read_automation_editor_launch(AgsFileLaunch *file_launch,
					      AgsAutomationEditor *automation_editor)
{
  xmlNode *child;
  
  xmlChar *str;

  str = xmlGetProp(file_launch->node,
		   "zoom\0");
  
  if(str != NULL){
    GtkTreeModel *model;
    GtkTreeIter iter;

    gchar *value;

    model = gtk_combo_box_get_model(automation_editor->automation_toolbar->zoom);
    
    if(gtk_tree_model_get_iter_first(model, &iter)){
      do{
	gtk_tree_model_get(model, &iter,
			   0, &value,
			   -1);

	if(!g_strcmp0(str,
		      value)){
	  gtk_combo_box_set_active_iter((GtkComboBox *) automation_editor->automation_toolbar->zoom,
					&iter);
	  break;
	}
      }while(gtk_tree_model_iter_next(model,
				      &iter));
    }
  }

  /* children */
  child = file_launch->node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-property-list\0",
		     21)){
	GList *list_start, *list;
	GList *property_start, *property;

	property_start = NULL;
	ags_simple_file_read_property_list(file_launch->file,
					   child,
					   &property_start);

	property = property_start;

	while(property != NULL){
	  if(!g_strcmp0(((GParameter *) property->data)->name,
			"machine\0")){
	    AgsMachine *machine;

	    xmlNode *machine_child;
	    
	    GList *file_id_ref;
	    GList *list_start, *list;

	    gchar *str;
	    
	    str = g_value_get_string(&(((GParameter *) property->data)->value));
	    
	    if(str != NULL){
	      file_id_ref = ags_simple_file_find_id_ref_by_xpath(file_launch->file,
								 str);
	      machine = AGS_FILE_ID_REF(file_id_ref->data)->ref;

	      ags_machine_selector_add_index(automation_editor->machine_selector);

	      list_start = gtk_container_get_children(automation_editor->machine_selector);
	      list = g_list_last(list_start);

	      gtk_button_clicked(list->data);
	      ags_machine_selector_link_index(automation_editor->machine_selector,
					      machine);

	      /* apply automation ports */
	      machine_child = AGS_FILE_ID_REF(file_id_ref->data)->node->children;

	      while(machine_child != NULL){
		if(machine_child->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(machine_child->name,
				 "ags-sf-automation-port-list\0",
				 27)){
		    xmlNode *automation_port;

		    automation_port = machine_child->children;

		    while(automation_port != NULL){
		      if(automation_port->type == XML_ELEMENT_NODE){
			if(!xmlStrncmp(automation_port->name,
				       "ags-sf-automation-port\0",
				       22)){
			  gchar *specifier;

			  specifier = xmlGetProp(automation_port,
						 "specifier\0");

			  if(specifier != NULL){
			    GtkTreeModel *model;
			    GtkTreeIter iter;

			    gchar *value;
			  
			    model = gtk_combo_box_get_model(automation_editor->automation_toolbar->port);
			  
			    if(gtk_tree_model_get_iter_first(model, &iter)){
			      do{
				gtk_tree_model_get(model, &iter,
						   1, &value,
						   -1);

				if(!g_strcmp0(specifier,
					      value)){
				  gtk_combo_box_set_active_iter(automation_editor->automation_toolbar->port,
								&iter);
				}
			      }while(gtk_tree_model_iter_next(model,
							      &iter));
			    }
			  }
			}
		      }

		      automation_port = automation_port->next;
		    }
		  }
		}
	      
		machine_child = machine_child->next;
	      }
	    
	      g_list_free(list_start);
	    }
	  }else{
	    g_object_set_property(automation_editor,
				  ((GParameter *) property->data)->name,
				  &(((GParameter *) property->data)->value));
	  }
	  
	  property = property->next;
	}

	list_start = gtk_container_get_children(automation_editor->machine_selector);
	list = list_start->next;

	if(list != NULL){
	  gtk_button_clicked(list->data);
	}
	
	g_list_free(list_start);
	g_list_free_full(property_start,
			 g_free);
      }
    }

    child = child->next;
  }
}

void
ags_simple_file_read_notation_list(AgsSimpleFile *simple_file, xmlNode *node, GList **notation)
{
  AgsNotation *current;

  xmlNode *child;

  GList *list;

  guint i;

  child = node->children;
  list = NULL;

  i = 0;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-notation\0",
		     11)){
	current = NULL;

	if(*notation != NULL){
	  GList *iter;

	  iter = g_list_nth(*notation,
			    i);
	  current = iter->data;
	}
	
	ags_simple_file_read_notation(simple_file, child, &current);
	list = g_list_prepend(list, current);

	i++;
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *notation = list;
}

void
ags_simple_file_read_notation(AgsSimpleFile *simple_file, xmlNode *node, AgsNotation **notation)
{
  AgsNotation *gobject;
  AgsNote *note;

  xmlNode *child;
  
  xmlChar *str;
  
  guint audio_channel;

  if(*notation != NULL){
    gobject = *notation;

    audio_channel = gobject->audio_channel;
  }else{
    AgsMachine *machine;
    
    AgsFileIdRef *file_id_ref;
    
    file_id_ref = ags_simple_file_find_id_ref_by_node(simple_file,
						      node->parent->parent);
    machine = file_id_ref->ref;

    if(!AGS_IS_MACHINE(machine)){
      return;
    }
    
    audio_channel = 0;
    str = xmlGetProp(node,
		     "channel\0");

    if(str != NULL){
      audio_channel = g_ascii_strtoull(str,
				       NULL,
				       10);
    }
    
    gobject = g_object_new(AGS_TYPE_NOTATION,
			   "audio\0", machine->audio,
			   "audio-channel\0", audio_channel,
			   NULL);

    *notation = gobject;
  }

  /* children */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-sf-note\0",
		     12)){
	note = ags_note_new();

	/* position and offset */
	str = xmlGetProp(child,
			 "x0\0");

	if(str != NULL){
	  note->x[0] = g_ascii_strtoull(str,
				       NULL,
				       10);
	}
	
	str = xmlGetProp(child,
			 "x1\0");

	if(str != NULL){
	  note->x[1] = g_ascii_strtoull(str,
				       NULL,
				       10);
	}

	str = xmlGetProp(child,
			 "y\0");

	if(str != NULL){
	  note->y = g_ascii_strtoull(str,
				     NULL,
				     10);
	}

	/* envelope */
	//FIXME:JK: missing imaginary part
	str = xmlGetProp(child,
			 "attack\0");

	if(str != NULL){
	  ags_complex_set(note->attack,
			  g_ascii_strtod(str,
					 NULL));
	}

	str = xmlGetProp(child,
			 "decay\0");

	if(str != NULL){
	  ags_complex_set(note->decay,
			  g_ascii_strtod(str,
					 NULL));
	}

	str = xmlGetProp(child,
			 "sustain\0");

	if(str != NULL){
	  ags_complex_set(note->sustain,
			  g_ascii_strtod(str,
					 NULL));
	}

	str = xmlGetProp(child,
			 "release\0");

	if(str != NULL){
	  ags_complex_set(note->release,
			  g_ascii_strtod(str,
					 NULL));
	}

	str = xmlGetProp(child,
			 "ratio\0");

	if(str != NULL){
	  ags_complex_set(note->ratio,
			  g_ascii_strtod(str,
					 NULL));
	}

	
	/* add */
	ags_notation_add_note(gobject,
			      note,
			      FALSE);
      }
    }

    child = child->next;
  }
}

void
ags_simple_file_read_automation_list(AgsSimpleFile *simple_file, xmlNode *node, GList **automation)
{
  AgsAutomation *current;
  
  xmlNode *child;

  GList *list;
  
  guint i;
  
  child = node->children;
  list = NULL;

  i = 0;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-automation\0",
		     11)){
	current = NULL;

	if(*automation != NULL){
	  GList *iter;

	  iter = g_list_nth(*automation,
			    i);
	  current = iter->data;
	}
	
	ags_simple_file_read_automation(simple_file, child, &current);
	list = g_list_prepend(list, current);

	i++;
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *automation = list;
}

void
ags_simple_file_read_automation(AgsSimpleFile *simple_file, xmlNode *node, AgsAutomation **automation)
{
  AgsMachine *machine;

  AgsAutomation *gobject;
  AgsAcceleration *acceleration;

  AgsFileIdRef *file_id_ref;
  
  GType channel_type;

  xmlNode *child;
  
  xmlChar *control_name;
  xmlChar *str;
  
  guint line;
  
  file_id_ref = ags_simple_file_find_id_ref_by_node(simple_file,
						    node->parent->parent);
  machine = file_id_ref->ref;
  
  if(*automation != NULL){
    gobject = *automation;

    line = gobject->line;
    channel_type = gobject->channel_type;
    control_name = gobject->control_name;
  }else{
    line = 0;
    str = xmlGetProp(node,
		     "line\0");

    if(str != NULL){
      line = g_ascii_strtoull(str,
			      NULL,
			      10);
    }

    str = xmlGetProp(node,
		     "channel-type\0");
    channel_type = g_type_from_name(str);
    
    control_name = xmlGetProp(node,
			      "control-name\0");

    gobject = ags_automation_new(machine->audio,
				 line,
				 channel_type,
				 control_name);
    
    *automation = gobject;
  }

  /* children */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-sf-acceleration\0",
		     12)){
	acceleration = ags_acceleration_new();

	/* position and offset */
	str = xmlGetProp(child,
			 "x\0");

	if(str != NULL){
	  acceleration->x = g_ascii_strtoull(str,
					     NULL,
					     10);
	}
	
	str = xmlGetProp(child,
			 "y\0");

	if(str != NULL){
	  acceleration->y = g_ascii_strtod(str,
					   NULL);
	}

	if((acceleration->x == 0 &&
	    acceleration->y == 0.0) ||
	   (acceleration->x == AGS_AUTOMATION_DEFAULT_LENGTH &&
	    acceleration->y == 0.0)){
	  /* no need for default */
	  g_object_unref(acceleration);
	}else{
	  /* add */
	  ags_automation_add_acceleration(gobject,
					  acceleration,
					  FALSE);
	}
      }
    }

    child = child->next;
  }
}

xmlNode*
ags_simple_file_write_config(AgsSimpleFile *simple_file, xmlNode *parent, AgsConfig *ags_config)
{
  xmlNode *node;
  xmlNode *cdata;

  gchar *id;
  char *buffer;
  gsize buffer_length;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-sf-config\0");

  ags_simple_file_add_id_ref(simple_file,
			     g_object_new(AGS_TYPE_FILE_ID_REF,
					  "application-context\0", simple_file->application_context,
					  "file\0", simple_file,
					  "node\0", node,
					  "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
					  "reference\0", ags_config,
					  NULL));

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_VERSION_PROP,
	     AGS_CONFIG(ags_config)->version);

  xmlNewProp(node,
	     AGS_FILE_BUILD_ID_PROP,
	     AGS_CONFIG(ags_config)->build_id);

  xmlAddChild(parent,
	      node);

  /* cdata */
  ags_config_to_data(ags_config,
		     &buffer,
		     &buffer_length);

  cdata = xmlNewCDataBlock(simple_file->doc,
			   buffer,
			   buffer_length);

  /* add to parent */
  xmlAddChild(node,
	      cdata);

  return(node);
}

xmlNode*
ags_simple_file_write_property_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *property)
{
  xmlNode *node;
  xmlNode *child;
  
  node = xmlNewNode(NULL,
		    "ags-sf-property-list");

  while(property != NULL){
    ags_simple_file_write_property(simple_file,
				   node,
				   property->data);

    property = property->next;
  }

  /* add to parent */
  xmlAddChild(parent,
	      node);

  return(node);
}

xmlNode*
ags_simple_file_write_property(AgsSimpleFile *simple_file, xmlNode *parent, GParameter *property)
{
  xmlNode *node;
  
  xmlChar *type_name;
  xmlChar *val;
  
  if(G_VALUE_HOLDS_BOOLEAN(&(property->value))){
    type_name = g_type_name(G_TYPE_BOOLEAN);

    if(g_value_get_boolean(&(property->value))){
      val = g_strdup("true\0");
    }else{
      val = g_strdup("false\0");
    }
  }else if(G_VALUE_HOLDS_UINT(&(property->value))){
    type_name = g_type_name(G_TYPE_UINT);

    val = g_strdup_printf("%u\0",
			  g_value_get_uint(&(property->value)));
  }else if(G_VALUE_HOLDS_INT(&(property->value))){
    type_name = g_type_name(G_TYPE_INT);

    val = g_strdup_printf("%d\0",
			  g_value_get_int(&(property->value)));
  }else if(G_VALUE_HOLDS_DOUBLE(&(property->value))){
    type_name = g_type_name(G_TYPE_DOUBLE);

    val = g_strdup_printf("%f\0",
			  g_value_get_double(&(property->value)));
  }else{
    g_warning("ags_simple_file_write_property() - unsupported type\0");
    
    return;
  }

  node = xmlNewNode(NULL,
		    "ags-sf-property\0");
  
  xmlNewProp(node,
	     "type\0",
	     type_name);

  xmlNewProp(node,
	     "name\0",
	     property->name);

  xmlNewProp(node,
	     "value\0",
	     val);
  
  /* add to parent */
  xmlAddChild(parent,
	      node);

  return(node);
}

xmlNode*
ags_simple_file_write_window(AgsSimpleFile *simple_file, xmlNode *parent, AgsWindow *window)
{
  xmlNode *node;

  GList *list;
  
  node = xmlNewNode(NULL,
		    "ags-sf-window\0");

  xmlNewProp(node,
	     "filename\0",
	     simple_file->filename);

  xmlNewProp(node,
	     "bpm\0",
	     g_strdup_printf("%f\0",
			     window->navigation->bpm->adjustment->value));

  xmlNewProp(node,
	     "loop\0",
	     ((gtk_toggle_button_get_active(window->navigation->loop)) ? g_strdup("true\0"): g_strdup("false\0")));

  xmlNewProp(node,
	     "loop-start\0",
	     g_strdup_printf("%f\0",
			     window->navigation->loop_left_tact->adjustment->value));

  xmlNewProp(node,
	     "loop-end\0",
	     g_strdup_printf("%f\0",
			     window->navigation->loop_right_tact->adjustment->value));

  /* children */
  list = gtk_container_get_children(window->machines);
  ags_simple_file_write_machine_list(simple_file,
				     node,
				     list);
  g_list_free(list);

  ags_simple_file_write_editor(simple_file,
			       node,
			       window->editor);

  ags_simple_file_write_automation_editor(simple_file,
					  node,
					  window->automation_window->automation_editor);

  /* add to parent */
  xmlAddChild(parent,
	      node);

  return(node);
}

xmlNode*
ags_simple_file_write_machine_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *machine)
{
  xmlNode *node;
  
  node = xmlNewNode(NULL,
		    "ags-sf-machine-list\0");

  while(machine != NULL){
    ags_simple_file_write_machine(simple_file,
				  node,
				  machine->data);

    machine = machine->next;
  }

  /* add to parent */
  xmlAddChild(parent,
	      node);

  return(node);
}

xmlNode*
ags_simple_file_write_machine(AgsSimpleFile *simple_file, xmlNode *parent, AgsMachine *machine)
{  
  xmlNode *node;
  xmlNode *pad_list;    

  GList *list;

  gchar *id;
  xmlChar *str;
  
  auto gboolean ags_simple_file_write_machine_inline_pad(AgsSimpleFile *simple_file, xmlNode *parent, AgsChannel *channel);
  auto xmlNode* ags_simple_file_write_control(AgsSimpleFile *simple_file, xmlNode *parent, AgsBulkMember *bulk_member);
  auto xmlNode* ags_simple_file_write_effect_list(AgsSimpleFile *simple_file, xmlNode *parent, AgsEffectBulk *effect_bulk);
  auto xmlNode* ags_simple_file_write_automation_port(AgsSimpleFile *simple_file, xmlNode *parent, gchar **automation_port);
  
  gboolean ags_simple_file_write_machine_inline_pad(AgsSimpleFile *simple_file, xmlNode *parent, AgsChannel *channel){
    AgsChannel *next_pad;

    xmlNode *pad;
    xmlNode *line_list;
    xmlNode *line;

    gboolean found_pad_content, found_line_content;
    
    if(channel == NULL){
      return;
    }

    found_pad_content = FALSE;
    
    while(channel != NULL){
      
      next_pad = channel->next_pad;

      pad = xmlNewNode(NULL,
		       "ags-sf-pad\0");

      xmlNewProp(pad,
		 "nth-pad\0",
		 g_strdup_printf("%d\0",
				 channel->pad));

      line_list = xmlNewNode(NULL,
			     "ags-sf-line-list\0");
      found_line_content = FALSE;

      while(channel != next_pad){
	gchar *id;
	
	id = ags_id_generator_create_uuid();
	  
	line = xmlNewNode(NULL,
			  "ags-sf-line\0");

	xmlNewProp(line,
		   "id\0",
		   id);

	if(channel->link != NULL){
	  ags_simple_file_add_id_ref(simple_file,
				     g_object_new(AGS_TYPE_FILE_ID_REF,
						  "application-context\0", simple_file->application_context,
						  "file\0", simple_file,
						  "node\0", line,
						  "reference\0", channel,
						  NULL));

	  found_line_content = TRUE;
	}

	xmlNewProp(line,
		   "nth-line\0",
		   g_strdup_printf("%d\0",
				   channel->line));

	/* link */
	if(channel->link != NULL){
	  AgsFileLookup *file_lookup;

	  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
						       "file\0", simple_file,
						       "node\0", line,
						       "reference\0", channel,
						       NULL);
	  ags_simple_file_add_lookup(simple_file, (GObject *) file_lookup);
	  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
			   G_CALLBACK(ags_simple_file_write_line_resolve_link), channel);
	}

	/* add to parent */
	xmlAddChild(line_list,
		    line);

	/* iterate */
	channel = channel->next;
      }

      if(found_line_content){
	found_pad_content = TRUE;
	
	/* add to parent */
	xmlAddChild(pad,
		    line_list);
      
	/* add to parent */
	xmlAddChild(parent,
		    pad);
      }else{
	xmlFreeNode(line_list);
	xmlFreeNode(pad);
      }
    }

    if(found_pad_content){
      return(TRUE);
    }else{
      return(FALSE);
    }
  }

  xmlNode* ags_simple_file_write_control(AgsSimpleFile *simple_file, xmlNode *parent, AgsBulkMember *bulk_member){
    GtkWidget *child_widget;
    
    xmlNode *control_node;
    
    /* control node */
    child_widget = gtk_bin_get_child(bulk_member);
    
    if(GTK_IS_TOGGLE_BUTTON(child_widget)){
      control_node = xmlNewNode(NULL,
				"ags-sf-control\0");
      
      xmlNewProp(control_node,
		 "control-type\0",
		 G_OBJECT_TYPE_NAME(child_widget));
      
      xmlNewProp(control_node,
		 "value\0",
		 ((gtk_toggle_button_get_active(child_widget)) ? g_strdup("true\0"): g_strdup("false\0")));
    }else if(AGS_IS_DIAL(child_widget)){
      control_node = xmlNewNode(NULL,
				"ags-sf-control\0");

      xmlNewProp(control_node,
		 "control-type\0",
		 G_OBJECT_TYPE_NAME(child_widget));
      
      xmlNewProp(control_node,
		 "value\0",
		 g_strdup_printf("%f\0", AGS_DIAL(child_widget)->adjustment->value));
    }else{
      g_warning("ags_file_write_effect_list() - unknown child of AgsBulkMember type\0");

      return;
    }

    xmlNewProp(control_node,
	       "specifier\0",
	       bulk_member->specifier);

    xmlAddChild(parent,
		control_node);

    return(control_node);
  }
  
  xmlNode* ags_simple_file_write_effect_list(AgsSimpleFile *simple_file, xmlNode *parent, AgsEffectBulk *effect_bulk){
    xmlNode *effect_list_node;
    xmlNode *effect_node;
    
    GList *filename;
    GList *effect;
    GList *list_start, *list;
    
    effect_list_node = NULL;
    effect_node = NULL;
    
    filename = NULL;
    effect = NULL;
    
    list =
      list_start = gtk_container_get_children(effect_bulk->table);
    
    while(list != NULL){
      if(AGS_IS_BULK_MEMBER(list->data)){
	if(g_list_find_custom(filename,
			      AGS_BULK_MEMBER(list->data)->filename,
			      g_strcmp0) == NULL ||
	   g_list_find_custom(effect,
			      AGS_BULK_MEMBER(list->data)->effect,
			      g_strcmp0) == NULL){
	  GtkWidget *child_widget;

	  if(effect_list_node == NULL){
	    effect_list_node = xmlNewNode(NULL,
					  "ags-sf-effect-list");
	  }
	  
	  effect_node = xmlNewNode(NULL,
				   "ags-sf-effect\0");

	  xmlNewProp(effect_node,
		     "filename\0",
		     AGS_BULK_MEMBER(list->data)->filename);
	  filename = g_list_prepend(filename,
				    AGS_BULK_MEMBER(list->data)->filename);

	  xmlNewProp(effect_node,
		     "effect\0",
		     AGS_BULK_MEMBER(list->data)->effect);
	  effect = g_list_prepend(effect,
				  AGS_BULK_MEMBER(list->data)->effect);
	  
	  ags_simple_file_write_control(simple_file, effect_node, list->data);
	  
	  /* add to parent */
	  xmlAddChild(effect_list_node,
		      effect_node);
	}else{
	  ags_simple_file_write_control(simple_file, effect_node, list->data);
	}
      }

      list = list->next;
    }

    g_list_free(list_start);

    /* add to parent */
    if(effect_list_node != NULL){
      xmlAddChild(parent,
		  effect_list_node);
    }

    return(effect_list_node);
  }

  xmlNode* ags_simple_file_write_automation_port(AgsSimpleFile *simple_file, xmlNode *parent, gchar **automation_port){
    xmlNode *node, *child;    

    node = NULL;
    
    if(automation_port != NULL && *automation_port != NULL){
      node = xmlNewNode(NULL,
			"ags-sf-automation-port-list\0");
      
      for(; *automation_port != NULL; automation_port++){
	child = xmlNewNode(NULL,
			   "ags-sf-automation-port");
	xmlNewProp(child,
		   "specifier\0",
		   *automation_port);
	
	xmlAddChild(node,
		    child);
      }
    }
    
    /* add to parent */
    if(node != NULL){
      xmlAddChild(parent,
		  node);
    }
    
    return(node);
  }

  /* node and uuid */
  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-sf-machine\0");
  
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_TYPE_PROP,
	     (xmlChar *) G_OBJECT_TYPE_NAME(machine));
  
  ags_simple_file_add_id_ref(simple_file,
			     g_object_new(AGS_TYPE_FILE_ID_REF,
					  "application-context\0", simple_file->application_context,
					  "file\0", simple_file,
					  "node\0", node,
					  "reference\0", machine,
					  NULL));
  
  /* device */
  if(machine->audio != NULL &&
     machine->audio->soundcard != NULL){
    gchar *device;

    device = ags_soundcard_get_device(AGS_SOUNDCARD(machine->audio->soundcard));
    
    if(device != NULL){
      xmlNewProp(node,
		 (xmlChar *) "soundcard-device\0",
		 (xmlChar *) g_strdup(device));
    }
  }
  
  /* bank and mapping */
  xmlNewProp(node,
	     (xmlChar *) "bank_0",
	     (xmlChar *) g_strdup_printf("%d\0", machine->bank_0));

  xmlNewProp(node,
	     (xmlChar *) "bank_1",
	     (xmlChar *) g_strdup_printf("%d\0", machine->bank_1));
  
  if((AGS_AUDIO_REVERSE_MAPPING & (machine->audio->flags)) != 0){
    xmlNewProp(node,
	       "reverse-mapping\0",
	       "true\0");
  }

  /* channels and pads */
  xmlNewProp(node,
	     (xmlChar *) "channels\0",
	     (xmlChar *) g_strdup_printf("%d\0", machine->audio->audio_channels));

  xmlNewProp(node,
	     (xmlChar *) "output-pads\0",
	     (xmlChar *) g_strdup_printf("%d\0", machine->audio->output_pads));

  xmlNewProp(node,
	     (xmlChar *) "input-pads\0",
	     (xmlChar *) g_strdup_printf("%d\0", machine->audio->input_pads));

  /* midi mapping */
  if((AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
    xmlNewProp(node,
	       "audio-start-mapping\0",
	       g_strdup_printf("%d\0", machine->audio->audio_start_mapping));

    xmlNewProp(node,
	       "audio-end-mapping\0",
	       g_strdup_printf("%d\0", machine->audio->audio_end_mapping));

    xmlNewProp(node,
	       "midi-start-mapping\0",
	       g_strdup_printf("%d\0", machine->audio->midi_start_mapping));

    xmlNewProp(node,
	       "midi-end-mapping\0",
	       g_strdup_printf("%d\0", machine->audio->midi_end_mapping));
  }
  
  /* machine specific */
  if(AGS_IS_DRUM(machine)){
    AgsDrum *drum;

    drum = machine;
    
    if(gtk_toggle_button_get_active(drum->loop_button)){
      xmlNewProp(node,
		 "loop\0",
		 "true\0");
    }

    xmlNewProp(node,
	       "length\0",
	       g_strdup_printf("%u\0", (guint) drum->length_spin->adjustment->value));
  }else if(AGS_IS_MATRIX(machine)){
    AgsMatrix *matrix;

    matrix = machine;
    
    if(gtk_toggle_button_get_active(matrix->loop_button)){
      xmlNewProp(node,
		 "loop\0",
		 "true\0");
    }    

    xmlNewProp(node,
	       "length\0",
	       g_strdup_printf("%u\0", (guint) matrix->length_spin->adjustment->value));
  }else if(AGS_IS_FFPLAYER(machine)){
    AgsFFPlayer *ffplayer;

    ffplayer = machine;

    if(ffplayer->ipatch != NULL &&
       ffplayer->ipatch->filename != NULL){
      xmlNewProp(node,
		 "filename\0",
		 ffplayer->ipatch->filename);

      xmlNewProp(node,
		 "preset\0",
		 gtk_combo_box_text_get_active_text(ffplayer->preset));

      xmlNewProp(node,
		 "instrument\0",
		 gtk_combo_box_text_get_active_text(ffplayer->instrument));
    }
  }else if(AGS_IS_LADSPA_BRIDGE(machine)){
    AgsLadspaBridge *ladspa_bridge;

    ladspa_bridge = machine;

    xmlNewProp(node,
	       "plugin-file\0",
	       ladspa_bridge->filename);

    xmlNewProp(node,
	       "effect\0",
	       ladspa_bridge->effect);
  }else if(AGS_IS_DSSI_BRIDGE(machine)){
    AgsDssiBridge *dssi_bridge;

    dssi_bridge = machine;

    xmlNewProp(node,
	       "plugin-file\0",
	       dssi_bridge->filename);

    xmlNewProp(node,
	       "effect\0",
	       dssi_bridge->effect);

    xmlNewProp(node,
	       "program\0",
	       gtk_combo_box_text_get_active_text(dssi_bridge->program));
  }else if(AGS_IS_LV2_BRIDGE(machine)){
    AgsLv2Bridge *lv2_bridge;

    lv2_bridge = machine;

    xmlNewProp(node,
	       "plugin-file\0",
	       lv2_bridge->filename);

    xmlNewProp(node,
	       "effect\0",
	       lv2_bridge->effect);
  }
  
  /* input */
  if(machine->input != NULL){
    list = gtk_container_get_children(machine->input);
  
    pad_list =  ags_simple_file_write_pad_list(simple_file,
					       node,
					       list);
    xmlNewProp(pad_list,
	       "is-output\0",
		 g_strdup("false\0"));
    
    g_list_free(list);
  }else{
    AgsChannel *channel;
    
    channel = machine->audio->input;

    if(channel != NULL){      
      pad_list = xmlNewNode(NULL,
			    "ags-sf-pad-list\0");
      xmlNewProp(pad_list,
		 "is-output\0",
		 g_strdup("false\0"));

      if(ags_simple_file_write_machine_inline_pad(simple_file,
						  pad_list,
						  channel)){
	/* add to parent */
	xmlAddChild(node,
		    pad_list);
      }else{
	xmlFreeNode(pad_list);
      }
    }
  }
  
  /* output */
  if(machine->output != NULL){
    list = gtk_container_get_children(machine->output);
  
    pad_list = ags_simple_file_write_pad_list(simple_file,
					      node,
					      list);
    xmlNewProp(pad_list,
	       "is-output\0",
	       g_strdup("true\0"));
    
    g_list_free(list);
  }else{
    AgsChannel *channel;
    
    channel = machine->audio->output;

    if(channel != NULL){
      pad_list = xmlNewNode(NULL,
			    "ags-sf-pad-list\0");
      xmlNewProp(pad_list,
		 "is-output\0",
		 g_strdup("true\0"));

      if(ags_simple_file_write_machine_inline_pad(simple_file,
						  pad_list,
						  channel)){            
	/* add to parent */
	xmlAddChild(node,
		    pad_list);
      }else{
	xmlFreeNode(pad_list);
      }
    }
  }

  /* effect list */
  if(machine->bridge != NULL){
    if(AGS_EFFECT_BRIDGE(machine->bridge)->bulk_input != NULL){
      xmlNode *child;
      
      child = ags_simple_file_write_effect_list(simple_file, node, AGS_EFFECT_BRIDGE(machine->bridge)->bulk_input);

      if(child != NULL){
	xmlNewProp(child,
		   "is-output\0",
		   "false\0");
      }
    }

    if(AGS_EFFECT_BRIDGE(machine->bridge)->bulk_output != NULL){
      xmlNode *child;

      child = ags_simple_file_write_effect_list(simple_file, node, AGS_EFFECT_BRIDGE(machine->bridge)->bulk_output);

      if(child != NULL){
	xmlNewProp(child,
		   "is-output\0",
		   "true\0");
      }
    }

    if(AGS_EFFECT_BRIDGE(machine->bridge)->input != NULL){
      xmlNode *child;
      
      GList *list;

      list = gtk_container_get_children(AGS_EFFECT_BRIDGE(machine->bridge)->input);
      
      child = ags_simple_file_write_effect_pad_list(simple_file,
						    node,
						    list);
      xmlNewProp(child,
		 "is-output\0",
		 "false\0");

      g_list_free(list);
    }

    if(AGS_EFFECT_BRIDGE(machine->bridge)->output != NULL){
      xmlNode *child;
      
      GList *list;

      list = gtk_container_get_children(AGS_EFFECT_BRIDGE(machine->bridge)->output);
      
      child = ags_simple_file_write_effect_pad_list(simple_file,
						    node,
						    list);
      xmlNewProp(child,
		 "is-output\0",
		 "true\0");
      
      g_list_free(list);
    }
  }

  /* pattern list */
  if(machine->audio->input != NULL){
    AgsChannel *channel;

    xmlNode *pattern_list_node;
    
    channel = machine->audio->input;
    pattern_list_node = NULL;
    
    while(channel != NULL){
      if(channel->pattern != NULL){
	AgsPattern *pattern;

	guint i, j, k;
	
	pattern = channel->pattern->data;

	for(i = 0; i < pattern->dim[0]; i++){
	  for(j = 0; j < pattern->dim[1]; j++){
	    if(!ags_pattern_is_empty(pattern,
				     i,
				     j)){
	      xmlNode *pattern_node;

	      xmlChar *content;
	      
	      if(pattern_list_node == NULL){
		pattern_list_node = xmlNewNode(NULL,
					       "ags-sf-pattern-list\0");
	      }

	      pattern_node = xmlNewNode(NULL,
					"ags-sf-pattern\0");

	      /* content */
	      content = malloc((pattern->dim[2] + 1) * sizeof(xmlChar));
	      content[pattern->dim[2]] = '\0';
	      
	      for(k = 0; k < pattern->dim[2]; k++){
		if(ags_pattern_get_bit(pattern,
				       i,
				       j,
				       k)){
		  content[k] = '1';
		}else{
		  content[k] = '0';
		}
	      }

	      xmlNodeAddContent(pattern_node,
				content);

	      /* attributes */
	      xmlNewProp(pattern_node,
			 "nth-line\0",
			 g_strdup_printf("%d\0",
					 channel->line));

	      xmlNewProp(pattern_node,
			 "bank-0\0",
			 g_strdup_printf("%d\0",
					 i));

	      xmlNewProp(pattern_node,
			 "bank-1\0",
			 g_strdup_printf("%d\0",
					 j));

	      /* add child */
	      xmlAddChild(pattern_list_node,
			  pattern_node);
	    }
	  }
	}
      }

      channel = channel->next;
    }

    if(pattern_list_node != NULL){
      xmlAddChild(node,
		  pattern_list_node);
    }
  }

  /* notation and automation list */
  if(machine->automation_port != NULL){
    ags_simple_file_write_automation_port(simple_file,
					 node,
					 machine->automation_port);
  }
  
  if(machine->audio->notation != NULL){
    ags_simple_file_write_notation_list(simple_file,
					node,
					machine->audio->notation);
  }

  if(machine->audio->automation != NULL){
    ags_simple_file_write_automation_list(simple_file,
					  node,
					  machine->audio->automation);
  }
  
  /* add to parent */
  xmlAddChild(parent,
	      node);

  return(node);
}

xmlNode*
ags_simple_file_write_pad_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *pad)
{
  xmlNode *node;

  gboolean found_content;

  found_content = FALSE;
  
  node = xmlNewNode(NULL,
		    "ags-sf-pad-list\0");

  while(pad != NULL){
    if(ags_simple_file_write_pad(simple_file,
				 node,
				 pad->data) != NULL){
      found_content = TRUE;
    }

    pad = pad->next;
  }

  if(found_content){
    /* add to parent */
    xmlAddChild(parent,
		node);

    return(node);
  }else{
    xmlFreeNode(node);
    
    return(NULL);
  }
}

xmlNode*
ags_simple_file_write_pad(AgsSimpleFile *simple_file, xmlNode *parent, AgsPad *pad)
{
  xmlNode *node;

  GList *list;

  gchar *id;

  gboolean found_content;

  found_content = FALSE;
  
  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-sf-pad\0");
  
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  xmlNewProp(node,
	     "nth-pad\0",
	     g_strdup_printf("%d\0", pad->channel->pad));
  
  /* group */
  if(!gtk_toggle_button_get_active(pad->group)){
    xmlNewProp(node,
	       "group\0",
	       g_strdup("false\0"));
    found_content = TRUE;
  }else{
    xmlNewProp(node,
	       "group\0",
	       g_strdup("true\0"));
  }

  /* mute */
  if(!gtk_toggle_button_get_active(pad->mute)){
    xmlNewProp(node,
	       "mute\0",
	       g_strdup("false\0"));
  }else{
    xmlNewProp(node,
	       "mute\0",
	       g_strdup("true\0"));
    found_content = TRUE;
  }

  /* solo */
  if(!gtk_toggle_button_get_active(pad->solo)){
    xmlNewProp(node,
	       "solo\0",
	       g_strdup("false\0"));
  }else{
    xmlNewProp(node,
	       "solo\0",
	       g_strdup("true\0"));
    found_content = TRUE;
  }

  /* children */
  if(pad->expander_set != NULL){
    list = gtk_container_get_children(pad->expander_set);
    
    if(ags_simple_file_write_line_list(simple_file,
				       node,
				       g_list_reverse(list)) != NULL){
      found_content = TRUE;
    }

    g_list_free(list);
  }

  if(found_content){
    /* add to parent */
    xmlAddChild(parent,
		node);

    return(node);
  }else{
    xmlFreeNode(node);
    
    return(NULL);
  }
}

xmlNode*
ags_simple_file_write_line_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *line)
{
  xmlNode *node;

  gboolean found_content;
  
  node = xmlNewNode(NULL,
		    "ags-sf-line-list\0");
  found_content = FALSE;
  
  while(line != NULL){
    if(ags_simple_file_write_line(simple_file,
				  node,
				  line->data) != NULL){
      found_content = TRUE;
    }

    line = line->next;
  }

  if(found_content){
    /* add to parent */
    xmlAddChild(parent,
		node);

    return(node);
  }else{
    xmlFreeNode(node);
    
    return(NULL);
  }
}

xmlNode*
ags_simple_file_write_line(AgsSimpleFile *simple_file, xmlNode *parent, AgsLine *line)
{
  xmlNode *node;
  xmlNode *effect_list_node;
  xmlNode *effect_node;
    
  GList *filename;
  GList *effect;
  GList *list_start, *list;

  gchar *id;

  gboolean found_content;
  
  auto void ags_simple_file_write_oscillator(AgsSimpleFile *simple_file, xmlNode *parent, AgsOscillator *oscillator);
  auto void ags_simple_file_write_control(AgsSimpleFile *simple_file, xmlNode *parent, AgsLineMember *line_member);

  void ags_simple_file_write_oscillator(AgsSimpleFile *simple_file, xmlNode *parent, AgsOscillator *oscillator){
    xmlNode *node;
    
    node = xmlNewNode(NULL,
		      "ags-oscillator\0");
    
    xmlNewProp(node,
	       "wave\0",
	       g_strdup_printf("%d\0", gtk_combo_box_get_active(oscillator->wave)));

    xmlNewProp(node,
	       "attack\0",
	       g_strdup_printf("%f\0", oscillator->attack->adjustment->value));

    xmlNewProp(node,
	       "length\0",
	       g_strdup_printf("%f\0", oscillator->frame_count->adjustment->value));

    xmlNewProp(node,
	       "frequency\0",
	       g_strdup_printf("%f\0", oscillator->frequency->adjustment->value));

    xmlNewProp(node,
	       "phase\0",
	       g_strdup_printf("%f\0", oscillator->phase->adjustment->value));

    xmlNewProp(node,
	       "volume\0",
	       g_strdup_printf("%f\0", oscillator->volume->adjustment->value));

    xmlAddChild(parent,
		node);
  }
  
  void ags_simple_file_write_control(AgsSimpleFile *simple_file, xmlNode *parent, AgsLineMember *line_member){
    GtkWidget *child_widget;
    
    xmlNode *control_node;
    
    /* control node */
    child_widget = gtk_bin_get_child(line_member);

    if(GTK_IS_TOGGLE_BUTTON(child_widget)){
      control_node = xmlNewNode(NULL,
				"ags-sf-control\0");

      xmlNewProp(control_node,
		 "control-type\0",
		 G_OBJECT_TYPE_NAME(child_widget));
    
      xmlNewProp(control_node,
		 "value\0",
		 ((gtk_toggle_button_get_active(child_widget)) ? g_strdup("true\0"): g_strdup("false\0")));
    }else if(AGS_IS_DIAL(child_widget)){
      control_node = xmlNewNode(NULL,
				"ags-sf-control\0");

      xmlNewProp(control_node,
		 "control-type\0",
		 G_OBJECT_TYPE_NAME(child_widget));

      xmlNewProp(control_node,
		 "value\0",
		 g_strdup_printf("%f\0", AGS_DIAL(child_widget)->adjustment->value));
    }else if(GTK_IS_RANGE(child_widget)){
      control_node = xmlNewNode(NULL,
				"ags-sf-control\0");

      xmlNewProp(control_node,
		 "control-type\0",
		 G_OBJECT_TYPE_NAME(child_widget));

      xmlNewProp(control_node,
		 "value\0",
		 g_strdup_printf("%f\0", GTK_RANGE(child_widget)->adjustment->value));
    }else if(GTK_IS_SPIN_BUTTON(child_widget)){
      control_node = xmlNewNode(NULL,
				"ags-sf-control\0");

      xmlNewProp(control_node,
		 "control-type\0",
		 G_OBJECT_TYPE_NAME(child_widget));

      xmlNewProp(control_node,
		 "value\0",
		 g_strdup_printf("%f\0", GTK_SPIN_BUTTON(child_widget)->adjustment->value));
    }else{
      g_warning("ags_file_write_effect_list() - unknown child of AgsLineMember type\0");

      return;
    }

    xmlNewProp(control_node,
	       "specifier\0",
	       line_member->specifier);

    xmlAddChild(parent,
		control_node);
  }
  
  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-sf-line\0");
  
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  found_content = FALSE;

  if(line->channel->link != NULL){
    ags_simple_file_add_id_ref(simple_file,
			       g_object_new(AGS_TYPE_FILE_ID_REF,
					    "application-context\0", simple_file->application_context,
					    "file\0", simple_file,
					    "node\0", node,
					    "reference\0", line->channel,
					    NULL));

    found_content = TRUE;
  }
  
  xmlNewProp(node,
	     "nth-line\0",
	     g_strdup_printf("%d\0", line->channel->line));
  
  /* group */
  if(!gtk_toggle_button_get_active(line->group)){
    xmlNewProp(node,
	       "is-grouped\0",
	       g_strdup("false\0"));

    found_content = TRUE;
  }else{
    xmlNewProp(node,
	       "is-grouped\0",
	       g_strdup("true\0"));
  }

  /* link or file */
  if(line->channel->link != NULL){
    AgsFileLookup *file_lookup;

    file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
						 "file\0", simple_file,
						 "node\0", node,
						 "reference\0", line,
						 NULL);
    ags_simple_file_add_lookup(simple_file, (GObject *) file_lookup);
    g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		     G_CALLBACK(ags_simple_file_write_line_resolve_link), line->channel);
  }else if(AGS_IS_INPUT(line->channel) &&
	   AGS_INPUT(line->channel)->file_link != NULL &&
	   ags_audio_file_check_suffix(AGS_FILE_LINK(AGS_INPUT(line->channel)->file_link)->filename)){
    xmlNewProp(node,
	       "filename\0",
	       g_strdup_printf("file://%s\0", AGS_FILE_LINK(AGS_INPUT(line->channel)->file_link)->filename));

    xmlNewProp(node,
	       "file-channel\0",
	       g_strdup_printf("%d\0", AGS_AUDIO_FILE_LINK(AGS_INPUT(line->channel)->file_link)->audio_channel));

    found_content = TRUE;
  }

  /* machine specific */
  if(AGS_IS_SYNTH_INPUT_LINE(line)){
    ags_simple_file_write_oscillator(simple_file,
				     node,
				     AGS_SYNTH_INPUT_LINE(line)->oscillator);
    found_content = TRUE;
  }
  
  /* effect list */
  effect_list_node = NULL;
  effect_node = NULL;
    
  filename = NULL;
  effect = NULL;

  list_start = 
    list = g_list_reverse(gtk_container_get_children(line->expander->table));
  
  while(list != NULL){
    if(AGS_IS_LINE_MEMBER(list->data)){
      AgsLineMember *line_member;

      line_member = list->data;

      if(g_list_find(filename,
		     AGS_LINE_MEMBER(list->data)->filename) == NULL ||
	 g_list_find(effect,
		     AGS_LINE_MEMBER(list->data)->effect) == NULL){
	GtkWidget *child_widget;

	if(effect_list_node == NULL){
	  effect_list_node = xmlNewNode(NULL,
					"ags-sf-effect-list");
	}
	  
	effect_node = xmlNewNode(NULL,
				 "ags-sf-effect\0");

	filename = g_list_prepend(filename,
				  AGS_LINE_MEMBER(list->data)->filename);

	effect = g_list_prepend(effect,
				AGS_LINE_MEMBER(list->data)->effect);

	xmlNewProp(effect_node,
		   "filename\0",
		   AGS_LINE_MEMBER(list->data)->filename);

	xmlNewProp(effect_node,
		   "effect\0",
		   AGS_LINE_MEMBER(list->data)->effect);
	
	ags_simple_file_write_control(simple_file, effect_node, list->data);
	  
	/* add to parent */
	xmlAddChild(effect_list_node,
		    effect_node);
      }else{
	ags_simple_file_write_control(simple_file, effect_node, list->data);
      }

      found_content = TRUE;
    }
    
    list = list->next;
  }

  if(list_start != NULL){
    g_list_free(list_start);
  }
  
  /* add to parent */
  if(effect_list_node != NULL){
    xmlAddChild(node,
		effect_list_node);
  }

  if(found_content){
    /* add to parent */
    xmlAddChild(parent,
		node);

    return(node);
  }else{
    xmlFreeNode(node);
    
    return(NULL);
  }
}

void
ags_simple_file_write_line_resolve_link(AgsFileLookup *file_lookup,
					AgsChannel *channel)
{
  GList *list;
  
  xmlChar *xpath;

  list = ags_simple_file_find_id_ref_by_reference(file_lookup->file,
						  channel->link);
  
  if(list != NULL){
    xpath = g_strdup_printf("xpath=//ags-sf-line[@id='%s']",
			    xmlGetProp(AGS_FILE_ID_REF(list->data)->node,
				       "id\0"));
    xmlNewProp(file_lookup->node,
	       "link\0",
	       xpath);
  }
}

xmlNode*
ags_simple_file_write_effect_pad_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *effect_pad)
{
  xmlNode *node;

  gboolean found_content;

  found_content = FALSE;
  
  node = xmlNewNode(NULL,
		    "ags-sf-effect-pad-list\0");

  while(effect_pad != NULL){
    if(ags_simple_file_write_effect_pad(simple_file,
					node,
					effect_pad->data) != NULL){
      found_content = TRUE;
    }

    effect_pad = effect_pad->next;
  }

  if(found_content){
    /* add to parent */
    xmlAddChild(parent,
		node);
    
    return(node);
  }else{
    xmlFreeNode(node);
    
    return(NULL);
  }
}

xmlNode*
ags_simple_file_write_effect_pad(AgsSimpleFile *simple_file, xmlNode *parent, AgsEffectPad *effect_pad)
{
  xmlNode *node;

  GList *list;

  gchar *id;

  gboolean found_content;

  found_content = FALSE;
  
  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-sf-effect-pad\0");
  
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);


  /* children */
  if(effect_pad->table != NULL){
    list = gtk_container_get_children(effect_pad->table);

    if(ags_simple_file_write_effect_line_list(simple_file,
					      node,
					      list) != NULL){
      found_content = TRUE;
    }

    g_list_free(list);
  }

  if(found_content){
    /* add to parent */
    xmlAddChild(parent,
		node);

    return(node);
  }else{
    xmlFreeNode(node);

    return(NULL);
  }
}

xmlNode*
ags_simple_file_write_effect_line_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *effect_line)
{
  xmlNode *node;

  gboolean found_content;

  found_content = FALSE;
  
  node = xmlNewNode(NULL,
		    "ags-sf-effect-line-list\0");

  while(effect_line != NULL){
    if(ags_simple_file_write_effect_line(simple_file,
					 node,
					 effect_line->data) != NULL){
      found_content = TRUE;
    }
    
    effect_line = effect_line->next;
  }
  
  if(found_content){
    /* add to parent */
    xmlAddChild(parent,
		node);
    
    return(node);
  }else{
    xmlFreeNode(node);
    
    return(NULL);
  }
}

xmlNode*
ags_simple_file_write_effect_line(AgsSimpleFile *simple_file, xmlNode *parent, AgsEffectLine *effect_line)
{
  xmlNode *node;
  xmlNode *effect_list_node;
  xmlNode *effect_node;
    
  GList *filename;
  GList *effect;
  GList *list_start, *list;

  gchar *id;

  gboolean found_content;
  
  auto void ags_simple_file_write_control(AgsSimpleFile *simple_file, xmlNode *parent, AgsLineMember *line_member);
  
  void ags_simple_file_write_control(AgsSimpleFile *simple_file, xmlNode *parent, AgsLineMember *line_member){
    GtkWidget *child_widget;
    
    xmlNode *control_node;
    
    /* control node */
    child_widget = gtk_bin_get_child(line_member);
    
    if(GTK_IS_TOGGLE_BUTTON(child_widget)){
      control_node = xmlNewNode(NULL,
				"ags-sf-control\0");

      xmlNewProp(control_node,
		 "value\0",
		 ((gtk_toggle_button_get_active(child_widget)) ? g_strdup("true\0"): g_strdup("false\0")));
    }else if(AGS_IS_DIAL(child_widget)){
      control_node = xmlNewNode(NULL,
				"ags-sf-control\0");

      xmlNewProp(control_node,
		 "value\0",
		 g_strdup_printf("%f\0", AGS_DIAL(child_widget)->adjustment->value));
    }else if(GTK_IS_RANGE(child_widget)){
      control_node = xmlNewNode(NULL,
				"ags-sf-control\0");

      xmlNewProp(control_node,
		 "value\0",
		 g_strdup_printf("%f\0", GTK_RANGE(child_widget)->adjustment->value));
    }else if(GTK_IS_SPIN_BUTTON(child_widget)){
      control_node = xmlNewNode(NULL,
				"ags-sf-control\0");

      xmlNewProp(control_node,
		 "value\0",
		 g_strdup_printf("%f\0", GTK_SPIN_BUTTON(child_widget)->adjustment->value));
    }else{
      g_warning("ags_file_write_effect_list() - unknown child of AgsLineMember type\0");

      return;
    }

    xmlNewProp(control_node,
	       "specifier\0",
	       line_member->specifier);

    xmlAddChild(parent,
		control_node);
  }

  found_content = FALSE;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-sf-effect-line\0");
  
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);

  /* effect list */
  effect_list_node = NULL;
  effect_node = NULL;
    
  filename = NULL;
  effect = NULL;

  list_start = 
    list = gtk_container_get_children(effect_line->table);

  while(list != NULL){
    if(AGS_IS_LINE_MEMBER(list->data)){
      AgsLineMember *line_member;

      line_member = list->data;

      if(g_list_find(filename,
		     AGS_LINE_MEMBER(list->data)->filename) == NULL ||
	 g_list_find(effect,
		     AGS_LINE_MEMBER(list->data)->effect) == NULL){
	GtkWidget *child_widget;

	if(effect_list_node == NULL){
	  effect_list_node = xmlNewNode(NULL,
					"ags-sf-effect-list");
	}
	
	effect_node = xmlNewNode(NULL,
				 "ags-sf-effect\0");

	filename = g_list_prepend(filename,
				  AGS_LINE_MEMBER(list->data)->filename);

	effect = g_list_prepend(effect,
				AGS_LINE_MEMBER(list->data)->effect);
	  
	ags_simple_file_write_control(simple_file, effect_node, list->data);
	  
	/* add to parent */
	xmlAddChild(effect_list_node,
		    effect_node);
      }else{
	ags_simple_file_write_control(simple_file, effect_node, list->data);
      }
      
      found_content = TRUE;
    }
    
    list = list->next;
  }
  
  g_list_free(list_start);
  
  /* add to parent */
  if(effect_list_node != NULL){
    xmlAddChild(node,
		effect_list_node);
  }
  
  g_list_free(list_start);

  if(found_content){
    /* add to parent */
    xmlAddChild(parent,
		node);

    return(node);
  }else{
    xmlFreeNode(node);
    
    return(NULL);
  }
}


xmlNode*
ags_simple_file_write_editor(AgsSimpleFile *simple_file, xmlNode *parent, AgsEditor *editor)
{
  AgsFileLookup *file_lookup;

  xmlNode *node;

  node = xmlNewNode(NULL,
		    "ags-sf-editor\0");

  xmlNewProp(node,
	     "zoom\0",
	     gtk_combo_box_text_get_active_text(editor->toolbar->zoom));
  
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", simple_file,
					       "node\0", node,
					       "reference\0", editor,
					       NULL);
  ags_simple_file_add_lookup(simple_file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_simple_file_write_editor_resolve_machine), editor);
  
  /* add to parent */
  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_simple_file_write_editor_resolve_machine(AgsFileLookup *file_lookup,
					     AgsEditor *editor)
{
  xmlNode *node;
  xmlNode *property_list;
  xmlNode *property;
  
  GList *list_start, *list;
  GList *file_id_ref;

  xmlChar *xpath;
  
  node = file_lookup->node;
  property_list = NULL;
  
  list_start = gtk_container_get_children(editor->machine_selector);
  list = list_start->next;

  if(list != NULL){
    property_list = xmlNewNode(NULL,
			       "ags-sf-property-list\0");
  }
  
  while(list != NULL){    
    if(AGS_MACHINE_RADIO_BUTTON(list->data)->machine != NULL){
      property = xmlNewNode(NULL,
			    "ags-sf-property\0");
      
      xmlNewProp(property,
		 "name\0",
		 "machine\0");

      file_id_ref = ags_simple_file_find_id_ref_by_reference(file_lookup->file,
							     AGS_MACHINE_RADIO_BUTTON(list->data)->machine);
      if(file_id_ref != NULL){
	xpath = g_strdup_printf("xpath=//ags-sf-machine[@id='%s']\0",
				xmlGetProp(AGS_FILE_ID_REF(file_id_ref->data)->node,
					   "id\0"));
      }else{
	xpath = g_strdup("(null)\0");
      }
      
      xmlNewProp(property,
		 "value\0",
		 xpath);

      /* add to parent */
      xmlAddChild(property_list,
		  property);
    }else{
      property = xmlNewNode(NULL,
			    "ags-sf-property\0");
      
      xmlNewProp(property,
		 "name\0",
		 "machine\0");

      xmlNewProp(property,
		 "value\0",
		 "(null)");

      /* add to parent */
      xmlAddChild(property_list,
		  property);
    }
    
    list = list->next;
  }

  /* add to parent */
  xmlAddChild(node,
	      property_list);
  
  g_list_free(list_start);
}

xmlNode*
ags_simple_file_write_automation_editor(AgsSimpleFile *simple_file, xmlNode *parent, AgsAutomationEditor *automation_editor)
{
  AgsFileLookup *file_lookup;

  xmlNode *node;
  
  node = xmlNewNode(NULL,
		    "ags-sf-automation-editor\0");

  xmlNewProp(node,
	     "zoom\0",
	     gtk_combo_box_text_get_active_text(automation_editor->automation_toolbar->zoom));

  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", simple_file,
					       "node\0", node,
					       "reference\0", automation_editor,
					       NULL);
  ags_simple_file_add_lookup(simple_file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_simple_file_write_automation_editor_resolve_machine), automation_editor);
  
  /* add to parent */
  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_simple_file_write_automation_editor_resolve_machine(AgsFileLookup *file_lookup,
							AgsAutomationEditor *automation_editor)
{
  xmlNode *node;
  xmlNode *property_list;
  xmlNode *property;
  
  GList *list_start, *list;
  GList *file_id_ref;

  xmlChar *xpath;
  
  node = file_lookup->node;
  property_list = NULL;
  
  list_start = gtk_container_get_children(automation_editor->machine_selector);
  list = list_start->next;

  if(list != NULL){
    property_list = xmlNewNode(NULL,
			       "ags-sf-property-list\0");
  }
  
  while(list != NULL){    
    if(AGS_MACHINE_RADIO_BUTTON(list->data)->machine != NULL){
      property = xmlNewNode(NULL,
			    "ags-sf-property\0");
      
      xmlNewProp(property,
		 "name\0",
		 "machine\0");

      file_id_ref = ags_simple_file_find_id_ref_by_reference(file_lookup->file,
							     AGS_MACHINE_RADIO_BUTTON(list->data)->machine);
      if(file_id_ref != NULL){
	xpath = g_strdup_printf("xpath=//ags-sf-machine[@id='%s']\0",
				xmlGetProp(AGS_FILE_ID_REF(file_id_ref->data)->node,
					   "id\0"));
      }else{
	xpath = g_strdup("(null)\0");
      }
      
      xmlNewProp(property,
		 "value\0",
		 xpath);

      /* add to parent */
      xmlAddChild(property_list,
		  property);
    }else{
      property = xmlNewNode(NULL,
			    "ags-sf-property\0");
      
      xmlNewProp(property,
		 "name\0",
		 "machine\0");

      xmlNewProp(property,
		 "value\0",
		 "(null)");

      /* add to parent */
      xmlAddChild(property_list,
		  property);
    }
    
    list = list->next;
  }

  /* add to parent */
  xmlAddChild(node,
	      property_list);
  
  g_list_free(list_start);
}

xmlNode*
ags_simple_file_write_notation_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *notation)
{
  xmlNode *node;
  
  node = xmlNewNode(NULL,
		    "ags-sf-notation-list\0");

  while(notation != NULL){
    ags_simple_file_write_notation(simple_file,
				   node,
				   notation->data);

    notation = notation->next;
  }

  /* add to parent */
  xmlAddChild(parent,
	      node);

  return(node);
}

xmlNode*
ags_simple_file_write_notation(AgsSimpleFile *simple_file, xmlNode *parent, AgsNotation *notation)
{
  xmlNode *node;
  xmlNode *child;

  GList *list;

  node = xmlNewNode(NULL,
		    "ags-sf-notation\0");

  xmlNewProp(node,
	     "channel\0",
	     g_strdup_printf("%d\0", notation->audio_channel));
  
  list = notation->notes;

  while(list != NULL){
    child = xmlNewNode(NULL,
		       "ags-sf-note\0");
    
    xmlNewProp(child,
	       "x0\0",
	       g_strdup_printf("%d\0",
			       AGS_NOTE(list->data)->x[0]));

    xmlNewProp(child,
	       "x1\0",
	       g_strdup_printf("%d\0",
			       AGS_NOTE(list->data)->x[1]));

    xmlNewProp(child,
	       "y\0",
	       g_strdup_printf("%d\0",
			       AGS_NOTE(list->data)->y));

    //TODO:JK: uncomment me if implemented
    /*
    xmlNewProp(child,
	       "attack\0",
	       g_strdup_printf("%f\0",
			       AGS_NOTE(list->data)->attack));
    
    xmlNewProp(child,
	       "decay\0",
	       g_strdup_printf("%f\0",
			       AGS_NOTE(list->data)->decay));
    
    xmlNewProp(child,
	       "sustain\0",
	       g_strdup_printf("%f\0",
			       AGS_NOTE(list->data)->sustain));
    
    xmlNewProp(child,
	       "release\0",
	       g_strdup_printf("%f\0",
    			       AGS_NOTE(list->data)->release));
			       
    xmlNewProp(child,
	       "ratio\0",
	       g_strdup_printf("%f\0",
			       AGS_NOTE(list->data)->ratio));
    */
    
    /* add to parent */
    xmlAddChild(node,
		child);

    list = list->next;
  }

  /* add to parent */
  xmlAddChild(parent,
	      node);

  return(node);
}

xmlNode*
ags_simple_file_write_automation_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *automation)
{
  xmlNode *node;

  gboolean found_node;
  
  node = xmlNewNode(NULL,
		    "ags-sf-automation-list\0");

  found_node = FALSE;
  
  while(automation != NULL){
    if(ags_simple_file_write_automation(simple_file,
					node,
					automation->data) != NULL){
      found_node = TRUE;
    }

    automation = automation->next;
  }

  if(found_node){
    /* add to parent */
    xmlAddChild(parent,
		node);

    return(node);
  }else{
    xmlFreeNode(node);

    return(NULL);
  }
}

xmlNode*
ags_simple_file_write_automation(AgsSimpleFile *simple_file, xmlNode *parent, AgsAutomation *automation)
{
  xmlNode *node;
  xmlNode *child;
  
  GList *list;

  gboolean found_automation;
  
  node = xmlNewNode(NULL,
		    "ags-sf-automation\0");

  xmlNewProp(node,
	     "line\0",
	     g_strdup_printf("%d\0", automation->line));

  xmlNewProp(node,
	     "channel-type\0",
	     g_strdup(g_type_name(automation->channel_type)));

  xmlNewProp(node,
	     "control-name\0",
	     g_strdup(automation->control_name));

  list = automation->acceleration;
  found_automation = FALSE;
  
  while(list != NULL){
    /* skip default */
    if((AGS_ACCELERATION(list->data)->x == 0 &&
	AGS_ACCELERATION(list->data)->y == 0.0) ||
       (AGS_ACCELERATION(list->data)->x == AGS_AUTOMATION_DEFAULT_LENGTH &&
	AGS_ACCELERATION(list->data)->y == 0.0)){
      list = list->next;

      continue;
    }

    found_automation = TRUE;
    child = xmlNewNode(NULL,
		       "ags-sf-acceleration\0");

    xmlNewProp(child,
	       "x\0",
	       g_strdup_printf("%d\0",
			       AGS_ACCELERATION(list->data)->x));

    xmlNewProp(child,
	       "y\0",
	       g_strdup_printf("%f\0",
			       AGS_ACCELERATION(list->data)->y));
    
    /* add to parent */
    xmlAddChild(node,
		child);

    list = list->next;
  }

  if(found_automation){
    /* add to parent */
    xmlAddChild(parent,
		node);

    return(node);
  }else{
    xmlFreeNode(node);
    
    return(NULL);
  }
}

AgsSimpleFile*
ags_simple_file_new()
{
  AgsSimpleFile *simple_file;

  simple_file = (AgsSimpleFile *) g_object_new(AGS_TYPE_SIMPLE_FILE,
					       NULL);

  return(simple_file);
}
