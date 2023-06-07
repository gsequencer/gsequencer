/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/app/file/ags_simple_file.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_gsequencer_application_context.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_meta_data_window.h>
#include <ags/app/ags_machine_util.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line.h>
#include <ags/app/ags_line_member.h>
#include <ags/app/ags_effect_bridge.h>
#include <ags/app/ags_effect_bulk.h>
#include <ags/app/ags_bulk_member.h>
#include <ags/app/ags_effect_pad.h>
#include <ags/app/ags_effect_line.h>

#include <ags/app/editor/ags_machine_selector.h>
#include <ags/app/editor/ags_machine_radio_button.h>

#include <ags/config.h>

#include <ags/app/machine/ags_panel.h>
#include <ags/app/machine/ags_mixer.h>
#include <ags/app/machine/ags_equalizer10.h>
#include <ags/app/machine/ags_spectrometer.h>
#include <ags/app/machine/ags_drum.h>
#include <ags/app/machine/ags_matrix.h>
#include <ags/app/machine/ags_synth.h>
#include <ags/app/machine/ags_synth_input_line.h>
#include <ags/app/machine/ags_fm_synth.h>
#include <ags/app/machine/ags_fm_synth_input_line.h>
#include <ags/app/machine/ags_syncsynth.h>
#include <ags/app/machine/ags_syncsynth_callbacks.h>
#include <ags/app/machine/ags_fm_syncsynth.h>
#include <ags/app/machine/ags_fm_syncsynth_callbacks.h>
#include <ags/app/machine/ags_hybrid_synth.h>
#include <ags/app/machine/ags_hybrid_synth_callbacks.h>
#include <ags/app/machine/ags_hybrid_fm_synth.h>
#include <ags/app/machine/ags_hybrid_fm_synth_callbacks.h>
#include <ags/app/machine/ags_oscillator.h>
#include <ags/app/machine/ags_fm_oscillator.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <ags/app/machine/ags_ffplayer.h>
#include <ags/app/machine/ags_sf2_synth.h>
#endif

#include <ags/app/machine/ags_pitch_sampler.h>
#include <ags/app/machine/ags_sfz_synth.h>

#include <ags/app/machine/ags_audiorec.h>

#include <ags/app/machine/ags_ladspa_bridge.h>
#include <ags/app/machine/ags_dssi_bridge.h>
#include <ags/app/machine/ags_lv2_bridge.h>
#include <ags/app/machine/ags_live_dssi_bridge.h>
#include <ags/app/machine/ags_live_lv2_bridge.h>

#if defined(AGS_WITH_VST3)
#include <ags/app/machine/ags_vst3_bridge.h>
#include <ags/app/machine/ags_live_vst3_bridge.h>
#endif

#ifdef AGS_WITH_LIBINSTPATCH
#include <libinstpatch/libinstpatch.h>
#endif

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

void ags_simple_file_read_change_max_precision(AgsThread *thread,
					       gdouble max_precision);

void ags_simple_file_read_config(AgsSimpleFile *simple_file, xmlNode *node, AgsConfig **config);
void ags_simple_file_read_meta_data_window(AgsSimpleFile *simple_file, xmlNode *node, AgsMetaDataWindow **meta_data_window);
void ags_simple_file_read_property_list(AgsSimpleFile *simple_file, xmlNode *node, GList **property);
void ags_simple_file_read_property(AgsSimpleFile *simple_file, xmlNode *node, GParameter **property);
void ags_simple_file_read_strv(AgsSimpleFile *simple_file, xmlNode *node, gchar ***strv);
void ags_simple_file_read_value(AgsSimpleFile *simple_file, xmlNode *node, GValue **value);
void ags_simple_file_read_window(AgsSimpleFile *simple_file, xmlNode *node, AgsWindow **window);
void ags_simple_file_read_window_launch(AgsFileLaunch *file_launch,
					AgsWindow *window);
void ags_simple_file_read_machine_list(AgsSimpleFile *simple_file, xmlNode *node, GList **machine);
void ags_simple_file_read_machine(AgsSimpleFile *simple_file, xmlNode *node, AgsMachine **machine);
void ags_simple_file_read_machine_resize_audio_channels(AgsMachine *machine,
							guint new_size, guint old_size,
							gpointer data);
void ags_simple_file_read_machine_resize_pads(AgsMachine *machine,
					      GType channel_type,
					      guint new_size, guint old_size,
					      gpointer data);

void ags_simple_file_read_equalizer10_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsEqualizer10 *equalizer10);
void ags_simple_file_read_drum_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsDrum *drum);
void ags_simple_file_read_matrix_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsMatrix *matrix);
void ags_simple_file_read_syncsynth_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsSyncsynth *syncsynth);
void ags_simple_file_read_fm_syncsynth_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsFMSyncsynth *fm_syncsynth);
void ags_simple_file_read_hybrid_synth_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsHybridSynth *hybrid_synth);
void ags_simple_file_read_hybrid_fm_synth_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsHybridFMSynth *hybrid_fm_synth);
void ags_simple_file_read_pitch_sampler_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsPitchSampler *pitch_sampler);
void ags_simple_file_read_sfz_synth_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsSFZSynth *sfz_synth);
#ifdef AGS_WITH_LIBINSTPATCH
void ags_simple_file_read_ffplayer_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsFFPlayer *ffplayer);
void ags_simple_file_read_sf2_synth_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsSF2Synth *sf2_synth);
#endif
void ags_simple_file_read_audiorec_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsAudiorec *audiorec);
void ags_simple_file_read_dssi_bridge_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsDssiBridge *dssi_bridge);
void ags_simple_file_read_live_dssi_bridge_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsLiveDssiBridge *live_dssi_bridge);
void ags_simple_file_read_lv2_bridge_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsLv2Bridge *lv2_bridge);
void ags_simple_file_read_live_lv2_bridge_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsLiveLv2Bridge *live_lv2_bridge);
#if defined(AGS_WITH_VST3)
void ags_simple_file_read_instantiate_vst3_plugin(AgsSimpleFile *simple_file, xmlNode *node, AgsMachine *machine, AgsVst3Plugin *vst3_plugin);

void ags_simple_file_read_vst3_bridge_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsVst3Bridge *vst3_bridge);
void ags_simple_file_read_live_vst3_bridge_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsLiveVst3Bridge *live_vst3_bridge);
#endif
void ags_simple_file_read_effect_bridge_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsEffectBridge *effect_bridge);
void ags_simple_file_read_effect_bulk_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsEffectBulk *effect_bulk);
void ags_simple_file_read_bulk_member_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsBulkMember *bulk_member);

void ags_simple_file_read_machine_launch(AgsFileLaunch *file_launch,
					 AgsMachine *machine);
void ags_simple_file_read_pad_list(AgsSimpleFile *simple_file, xmlNode *node, GList **pad);
void ags_simple_file_read_pad(AgsSimpleFile *simple_file, xmlNode *node, AgsPad **pad);
void ags_simple_file_read_pad_launch(AgsFileLaunch *file_launch,
				     AgsPad *pad);
void ags_simple_file_read_line_list(AgsSimpleFile *simple_file, xmlNode *node, GList **line);

void ags_simple_file_read_line_member(AgsSimpleFile *simple_file, xmlNode *node, AgsLineMember *line_member);  

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
void ags_simple_file_read_oscillator_list(AgsSimpleFile *simple_file, xmlNode *node, GList **oscillator);
void ags_simple_file_read_oscillator(AgsSimpleFile *simple_file, xmlNode *node, AgsOscillator **oscillator);
void ags_simple_file_read_fm_oscillator_list(AgsSimpleFile *simple_file, xmlNode *node, GList **fm_oscillator);
void ags_simple_file_read_fm_oscillator(AgsSimpleFile *simple_file, xmlNode *node, AgsFMOscillator **fm_oscillator);

void ags_simple_file_read_composite_editor(AgsSimpleFile *simple_file, xmlNode *node, AgsCompositeEditor **composite_editor);
void ags_simple_file_read_composite_editor_launch(AgsFileLaunch *file_launch,
						  AgsCompositeEditor *composite_editor);

void ags_simple_file_read_notation_list(AgsSimpleFile *simple_file, xmlNode *node, GList **notation);
void ags_simple_file_read_notation(AgsSimpleFile *simple_file, xmlNode *node, AgsNotation **notation);

void ags_simple_file_read_notation_fixup_1_0_to_1_2(AgsSimpleFile *simple_file, xmlNode *node, GList **notation);

void ags_simple_file_read_notation_list_fixup_1_0_to_1_2(AgsSimpleFile *simple_file, xmlNode *node, GList **notation);

void ags_simple_file_read_automation_list(AgsSimpleFile *simple_file, xmlNode *node, GList **automation);
void ags_simple_file_read_automation(AgsSimpleFile *simple_file, xmlNode *node, AgsAutomation **automation);

void ags_simple_file_read_automation_fixup_1_0_to_1_3(AgsSimpleFile *simple_file, xmlNode *node, GList **automation);

void ags_simple_file_read_automation_list_fixup_1_0_to_1_3(AgsSimpleFile *simple_file, xmlNode *node, GList **automation);
void ags_simple_file_read_preset_list(AgsSimpleFile *simple_file, xmlNode *node, GList **preset);
void ags_simple_file_read_preset(AgsSimpleFile *simple_file, xmlNode *node, AgsPreset **preset);

void ags_simple_file_read_program_list(AgsSimpleFile *simple_file, xmlNode *node, GList **program);
void ags_simple_file_read_program(AgsSimpleFile *simple_file, xmlNode *node, AgsProgram **program);

xmlNode* ags_simple_file_write_config(AgsSimpleFile *simple_file, xmlNode *parent, AgsConfig *config);
xmlNode* ags_simple_file_write_meta_data_window(AgsSimpleFile *simple_file, xmlNode *parent, AgsMetaDataWindow *meta_data_window);
xmlNode* ags_simple_file_write_window(AgsSimpleFile *simple_file, xmlNode *parent, AgsWindow *window);
xmlNode* ags_simple_file_write_property_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *property);
xmlNode* ags_simple_file_write_property(AgsSimpleFile *simple_file, xmlNode *parent, GParameter *property);
xmlNode* ags_simple_file_write_strv(AgsSimpleFile *simple_file, xmlNode *parent, gchar **strv);
xmlNode* ags_simple_file_write_value(AgsSimpleFile *simple_file, xmlNode *parent, GValue *value);
xmlNode* ags_simple_file_write_window(AgsSimpleFile *simple_file, xmlNode *parent, AgsWindow *window);
xmlNode* ags_simple_file_write_machine_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *machine);

gboolean ags_simple_file_write_machine_inline_pad(AgsSimpleFile *simple_file, xmlNode *parent, AgsChannel *channel);
xmlNode* ags_simple_file_write_control(AgsSimpleFile *simple_file, xmlNode *parent, AgsBulkMember *bulk_member);
xmlNode* ags_simple_file_write_effect_list(AgsSimpleFile *simple_file, xmlNode *parent, AgsEffectBulk *effect_bulk);
xmlNode* ags_simple_file_write_automation_port(AgsSimpleFile *simple_file, xmlNode *parent, GList *automation_port);

xmlNode* ags_simple_file_write_machine(AgsSimpleFile *simple_file, xmlNode *parent, AgsMachine *machine);
xmlNode* ags_simple_file_write_pad_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *pad);
xmlNode* ags_simple_file_write_pad(AgsSimpleFile *simple_file, xmlNode *parent, AgsPad *pad);
xmlNode* ags_simple_file_write_line_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *line);

void ags_simple_file_write_line_member_control(AgsSimpleFile *simple_file, xmlNode *parent, AgsLineMember *line_member);

xmlNode* ags_simple_file_write_line(AgsSimpleFile *simple_file, xmlNode *parent, AgsLine *line);
void ags_simple_file_write_line_resolve_link(AgsFileLookup *file_lookup,
					     AgsChannel *channel);
xmlNode* ags_simple_file_write_effect_pad_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *effect_pad);
xmlNode* ags_simple_file_write_effect_pad(AgsSimpleFile *simple_file, xmlNode *parent, AgsEffectPad *effect_pad);
xmlNode* ags_simple_file_write_effect_line_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *effect_line);

void ags_simple_file_write_effect_line_control(AgsSimpleFile *simple_file, xmlNode *parent, AgsLineMember *line_member);

xmlNode* ags_simple_file_write_effect_line(AgsSimpleFile *simple_file, xmlNode *parent, AgsEffectLine *effect_line);
xmlNode* ags_simple_file_write_oscillator_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *oscillator);
xmlNode* ags_simple_file_write_oscillator(AgsSimpleFile *simple_file, xmlNode *parent, AgsOscillator *oscillator);
xmlNode* ags_simple_file_write_fm_oscillator_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *fm_oscillator);
xmlNode* ags_simple_file_write_fm_oscillator(AgsSimpleFile *simple_file, xmlNode *parent, AgsFMOscillator *fm_oscillator);

xmlNode* ags_simple_file_write_composite_editor(AgsSimpleFile *simple_file, xmlNode *parent, AgsCompositeEditor *composite_editor);
void ags_simple_file_write_composite_editor_resolve_machine(AgsFileLookup *file_lookup,
							    AgsCompositeEditor *composite_editor);
xmlNode* ags_simple_file_write_notation_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *notation);
xmlNode* ags_simple_file_write_notation(AgsSimpleFile *simple_file, xmlNode *parent, AgsNotation *notation);
xmlNode* ags_simple_file_write_automation_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *automation);
xmlNode* ags_simple_file_write_automation(AgsSimpleFile *simple_file, xmlNode *parent, AgsAutomation *automation);
xmlNode* ags_simple_file_write_preset_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *preset);
xmlNode* ags_simple_file_write_preset(AgsSimpleFile *simple_file, xmlNode *parent, AgsPreset *preset);
xmlNode* ags_simple_file_write_program_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *program);
xmlNode* ags_simple_file_write_program(AgsSimpleFile *simple_file, xmlNode *parent, AgsProgram *program);

/**
 * SECTION:ags_simple_file
 * @short_description: read/write XML file
 * @title: AgsSimpleFile
 * @section_id:
 * @include: ags/app/file/ags_simple_file.h
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
  PROP_NO_CONFIG,
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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_simple_file = 0;

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
						  "AgsSimpleFile",
						  &ags_simple_file_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_simple_file);
  }

  return g_define_type_id__volatile;
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("filename",
				   "filename to read or write",
				   "The filename to read or write to.",
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("encoding",
				   "encoding to use",
				   "The encoding of the XML document.",
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("audio-format",
				   "audio format to use",
				   "The audio format used to embedded audio.",
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("audio-encoding",
				   "audio encoding to use",
				   "The audio encoding used to embedded audio.",
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("xml-doc",
				    "xml document of file",
				    "The xml document assigned with file",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_XML_DOC,
				  param_spec);

  /**
   * AgsSimpleFile:no-config:
   *
   * The no config option.
   *
   * Since: 3.16.9
   */
  param_spec = g_param_spec_boolean("no-config",
				    "no config",
				    "The no config option",
				    FALSE,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NO_CONFIG,
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
   * Since: 3.0.0
   */
  simple_file_signals[OPEN] =
    g_signal_new("open",
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
   * Since: 3.0.0
   */
  simple_file_signals[OPEN_FROM_DATA] =
    g_signal_new("open-from-data",
		 G_TYPE_FROM_CLASS(simple_file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSimpleFileClass, open_from_data),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__STRING_UINT_POINTER,
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
   * Since: 3.0.0
   */
  simple_file_signals[RW_OPEN] =
    g_signal_new("rw-open",
		 G_TYPE_FROM_CLASS(simple_file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSimpleFileClass, rw_open),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__BOOLEAN_POINTER,
		 G_TYPE_NONE, 2,
		 G_TYPE_BOOLEAN,
		 G_TYPE_POINTER);

  /**
   * AgsSimpleFile::write:
   * @simple_file: the #AgsSimpleFile
   * 
   * Write XML Document to disk.
   *
   * Since: 3.0.0
   */
  simple_file_signals[WRITE] =
    g_signal_new("write",
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
   * Since: 3.0.0
   */
  simple_file_signals[WRITE_RESOLVE] =
    g_signal_new("write-resolve",
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
   * Since: 3.0.0
   */
  simple_file_signals[READ] =
    g_signal_new("read",
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
   * Since: 3.0.0
   */
  simple_file_signals[READ_RESOLVE] =
    g_signal_new("read-resolve",
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
   * Start assigning resources.
   * 
   * Since: 3.0.0
   */
  simple_file_signals[READ_START] =
    g_signal_new("read-start",
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
  return(g_quark_from_static_string("ags-simple-file-error-quark"));
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

  simple_file->no_config = FALSE;
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
  case PROP_NO_CONFIG:
    {
      simple_file->no_config = g_value_get_boolean(value);
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
  case PROP_NO_CONFIG:
    {
      g_value_set_boolean(value, simple_file->no_config);
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
  xmlXPathContext *xpath_context; 
  xmlXPathObject *xpath_object;
  xmlNode **node;

  GList *list;

  gboolean success;
  guint i;

  if(simple_file == NULL || xpath == NULL || !g_str_has_prefix(xpath, "xpath=")){
    g_message("invalid xpath: %s", xpath);

    return(NULL);
  }

  xpath = &(xpath[6]);

  /* Create xpath evaluation context */
  xpath_context = xmlXPathNewContext(simple_file->doc);

  if(xpath_context == NULL) {
    g_warning("Error: unable to create new XPath context");

    return(NULL);
  }

  xpath_context->node = simple_file->root_node;

  /* Evaluate xpath expression */
  xpath_object = xmlXPathEval(BAD_CAST xpath, xpath_context);

  if(xpath_object == NULL) {
    g_warning("Error: unable to evaluate xpath expression \"%s\"", xpath);
    xmlXPathFreeContext(xpath_context); 

    return(NULL);
  }

  node = xpath_object->nodesetval->nodeTab;

  list = NULL;
  success = FALSE;
  
  for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
    if(node[i]->type == XML_ELEMENT_NODE){
      GObject *gobject;

      success = TRUE;
      gobject = ags_simple_file_find_id_ref_by_node(simple_file,
						    node[i]);

      if(gobject != NULL){
	list = g_list_prepend(list,
			      gobject);
      }
    }
  }

  if(!success){
    g_message("no xpath match [%d]: %s", xpath_object->nodesetval->nodeNr, xpath);
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
  if(!AGS_IS_SIMPLE_FILE(simple_file) ||
     file_launch == NULL){
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
  xmlInitParser();

  /* parse the file and get the DOM */
  simple_file->doc = xmlReadFile(simple_file->filename,
				 NULL,
				 XML_PARSE_HUGE);

  if(simple_file->doc == NULL){
    g_warning("ags_simple_file.c - failed to read XML document %s", simple_file->filename);

    if(error != NULL){
      g_set_error(error,
		  AGS_SIMPLE_FILE_ERROR,
		  AGS_SIMPLE_FILE_ERROR_PARSER_FAILURE,
		  "unable to parse document: %s\n",
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
  xmlInitParser();

  simple_file->doc = xmlReadMemory(data, length, simple_file->filename, NULL, 0);

  if(simple_file->doc == NULL){
    g_warning("ags_simple_file.c - failed to read XML document %s", simple_file->filename);

    if(error != NULL){
      g_set_error(error,
		  AGS_SIMPLE_FILE_ERROR,
		  AGS_SIMPLE_FILE_ERROR_PARSER_FAILURE,
		  "unable to parse document from data: %s\n",
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
  xmlInitParser();

  simple_file->out = fopen(simple_file->filename, "w+");

  simple_file->doc = xmlNewDoc(BAD_CAST "1.0");
  simple_file->root_node = xmlNewNode(NULL,
				      BAD_CAST "ags-simple-file");
  xmlNewProp(simple_file->root_node,
	     BAD_CAST "xmlns:agssf",
	     BAD_CAST "http://nongnu.org/gsequencer/ns/ags-simple-file");
  
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
	       "filename", filename,
	       NULL);
  ags_simple_file_open(simple_file,
		       &error);

  if(error != NULL){
    g_warning("%s", error->message);

    g_error_free(error);
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
  //  xmlCleanupParser();

  /*
   * this is to debug memory for regression tests
   */
  //  xmlMemoryDump();

  simple_file->filename = NULL;
}

void
ags_simple_file_real_write(AgsSimpleFile *simple_file)
{
  AgsApplicationContext *application_context;
  AgsConfig *config;

  xmlNode *node;
  
  GList *program;

  gchar *id;

  int size;
  
  if(simple_file == NULL ||
     simple_file->root_node == NULL){
    return;
  }

  application_context = ags_application_context_get_instance();
  config = ags_config_get_instance();
  
  id = ags_id_generator_create_uuid();

  node = simple_file->root_node;
  xmlNewProp(node,
	     BAD_CAST AGS_SIMPLE_FILE_ID_PROP,
	     BAD_CAST id);

  xmlNewProp(node,
	     BAD_CAST AGS_SIMPLE_FILE_VERSION_PROP,
	     BAD_CAST AGS_APPLICATION_CONTEXT(application_context)->version);

  xmlNewProp(node,
	     BAD_CAST AGS_SIMPLE_FILE_BUILD_ID_PROP,
	     BAD_CAST AGS_APPLICATION_CONTEXT(application_context)->build_id);

  /* add to parent */
  ags_simple_file_write_config(simple_file,
			       node,
			       config);  
  
  ags_simple_file_write_meta_data_window(simple_file,
  					 node,
					 (AgsMetaDataWindow *) ags_ui_provider_get_meta_data_window(AGS_UI_PROVIDER(application_context)));
  
  ags_simple_file_write_window(simple_file,
			       node,
			       (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context)));

  program = ags_sound_provider_get_program(AGS_SOUND_PROVIDER(application_context));
  ags_simple_file_write_program_list(simple_file,
				     node,
				     program);

  /* resolve */
  ags_simple_file_write_resolve(simple_file);

  /* 
   * Dumping document to file
   */
  //  xmlSaveFormatFileEnc(simple_file->filename, simple_file->doc, "UTF-8", 1);
  xmlDocDumpFormatMemoryEnc(simple_file->doc, &(simple_file->buffer), &size, simple_file->encoding, TRUE);

  if(simple_file->out != NULL){
    fwrite(simple_file->buffer, size, sizeof(xmlChar), simple_file->out);
    fflush(simple_file->out);
  }
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
  
  application_context = ags_application_context_get_instance();

  root_node = simple_file->root_node;

  if(root_node == NULL){
    return;
  }
  
  /* child elements */
  child = root_node->children;

  /* read config then window */
  ags_application_context_register_types(application_context);

#if 0
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(BAD_CAST "ags-sf-config",
		     child->name,
		     13)){
	//NOTE:JK: no redundant code here
      }
    }

    child = child->next;
  }
#endif
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(BAD_CAST "ags-sf-window",
		     child->name,
		     14)){
	AgsWindow *orig_window, *window;

	window =
	  orig_window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
	ags_simple_file_read_window(simple_file,
				    child,
				    (AgsWindow **) &window);

	if(orig_window != window){
	  ags_ui_provider_set_window(AGS_UI_PROVIDER(application_context),
				     (GtkWidget *) window);
	}
      }else if(!xmlStrncmp(BAD_CAST "ags-sf-meta-list",
			   child->name,
			   17)){
	AgsMetaDataWindow *meta_data_window;

	meta_data_window = ags_ui_provider_get_meta_data_window(AGS_UI_PROVIDER(application_context));

	//FIXME:JK: not supported yet
	ags_simple_file_read_meta_data_window(simple_file,
					      child,
					      (AgsMetaDataWindow **) &meta_data_window);
      }else if(!xmlStrncmp(BAD_CAST "ags-sf-program-list",
			   child->name,
			   23)){
	GList *program;

	program = NULL;
	ags_simple_file_read_program_list(simple_file,
					  child,
					  &program);

	ags_sound_provider_set_program(AGS_SOUND_PROVIDER(application_context),
				       program);
      }
    }

    child = child->next;
  }
  
  /* resolve */
  ags_simple_file_read_resolve(simple_file);
  
  g_message("XML simple file resolved");

  /* connect */  
  ags_connectable_connect(AGS_CONNECTABLE(application_context));

  gtk_widget_show(ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context)));

  g_message("XML simple file connected");

  /* start */
  ags_simple_file_read_start(simple_file);

  /* set file ready */
  ags_ui_provider_set_file_ready(AGS_UI_PROVIDER(application_context),
				 TRUE);
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
ags_simple_file_read_change_max_precision(AgsThread *thread,
					  gdouble max_precision)
{
  AgsThread *child, *next_child;
  
  g_object_set(thread,
	       "max-precision", max_precision,
	       NULL);

  child = ags_thread_children(thread);

  while(child != NULL){
    ags_simple_file_read_change_max_precision(child,
					      max_precision);

    /* iterate */
    next_child = ags_thread_next(child);

    g_object_unref(child);

    child = next_child;
  }
}

void
ags_simple_file_read_config(AgsSimpleFile *simple_file, xmlNode *node, AgsConfig **ags_config)
{
  AgsThread *main_loop;

  AgsApplicationContext *application_context;
  AgsConfig *config;

  char *buffer;
  gchar *id;
  gchar *str;
  
  gsize buffer_length;
  gdouble samplerate;
  guint buffer_size;
  gdouble frequency;
  gdouble gui_scale_factor;

  if(simple_file->no_config){
    g_message("no config");
    
    return;
  }
  
  config = *ags_config;
  config->version = xmlGetProp(node,
			       BAD_CAST AGS_SIMPLE_FILE_VERSION_PROP);

  config->build_id = xmlGetProp(node,
				BAD_CAST AGS_SIMPLE_FILE_BUILD_ID_PROP);

  application_context = ags_application_context_get_instance();
  
  buffer = xmlNodeGetContent(node);
  buffer_length = xmlStrlen(buffer);

  g_message("%s", buffer);
  
  ags_config_load_from_data(config,
			    buffer, buffer_length);

  /* max-precision */
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  str = ags_config_get_value(config,
			     AGS_CONFIG_THREAD,
			     "max-precision");
  
  if(str != NULL){
    gdouble max_precision;
    
    /* change max precision */
    max_precision = g_ascii_strtod(str,
				   NULL);
    
    ags_simple_file_read_change_max_precision(main_loop,
					      max_precision);  
  }

  samplerate = ags_soundcard_helper_config_get_samplerate(config);
  buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  
  frequency = ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;
  ags_main_loop_change_frequency(AGS_MAIN_LOOP(main_loop),
				 frequency);

  g_object_unref(main_loop);

  /* some GUI scaling */
  gui_scale_factor = 1.0;

  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "gui-scale");

  if(str != NULL){
    gui_scale_factor = g_ascii_strtod(str,
				      NULL);

    g_free(str);
  }

  ags_ui_provider_set_gui_scale_factor(AGS_UI_PROVIDER(application_context),
				       gui_scale_factor);
}

void
ags_simple_file_read_meta_data_window(AgsSimpleFile *simple_file, xmlNode *node, AgsMetaDataWindow **meta_data_window)
{
  AgsMetaDataWindow *current;

  xmlNode *child;

  xmlChar *name;
  xmlChar *str;
  
  if(meta_data_window == NULL){
    return;
  }

  current = meta_data_window[0];

  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-meta",
		     12)){
	name = xmlGetProp(child,
			  "name");
	
	str = xmlNodeGetContent(child);
	
	if(!xmlStrncmp(name,
		       (xmlChar *) "author",
		       7)){
	  gtk_editable_set_text(GTK_EDITABLE(current->author),
				str);
	}else if(!xmlStrncmp(name,
			     (xmlChar *) "title",
			     6)){
	  gtk_editable_set_text(GTK_EDITABLE(current->title),
				str);
	}else if(!xmlStrncmp(name,
			     (xmlChar *) "album",
			     6)){
	  gtk_editable_set_text(GTK_EDITABLE(current->album),
				str);
	}else if(!xmlStrncmp(name,
			     (xmlChar *) "release-date",
			     13)){
	  gtk_editable_set_text(GTK_EDITABLE(current->release_date),
				str);
	}else if(!xmlStrncmp(name,
			     (xmlChar *) "copyright",
			     10)){
	  gtk_editable_set_text(GTK_EDITABLE(current->copyright),
				str);
	}else if(!xmlStrncmp(name,
			     (xmlChar *) "license",
			     8)){
	  gtk_editable_set_text(GTK_EDITABLE(current->license),
				str);
	}else if(!xmlStrncmp(name,
			     (xmlChar *) "comment",
			     8)){
	  GtkTextBuffer *text_buffer;
	  
	  text_buffer = gtk_text_view_get_buffer(current->comment);

	  gtk_text_buffer_set_text(text_buffer,
				   str,
				   xmlStrlen(str));
	}

	xmlFree(name);
	xmlFree(str);
      }
    }

    child = child->next;
  }
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
		     (xmlChar *) "ags-sf-property",
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
		   BAD_CAST "name");

  if(str != NULL){
    pointer->name = str;
  }
  
  str = xmlGetProp(node,
		   BAD_CAST "value");

  type = xmlGetProp(node,
		    BAD_CAST "type");

  if(str != NULL){
    if(!g_strcmp0((gchar *) type,
		  "gboolean")){
      g_value_init(&(pointer->value),
		   G_TYPE_BOOLEAN);
      
      if(!g_ascii_strcasecmp((gchar *) str,
			     "false")){
	g_value_set_boolean(&(pointer->value),
			    FALSE);
      }else{
	g_value_set_boolean(&(pointer->value),
			    TRUE);
      }
    }else if(!g_strcmp0((gchar *) type,
			"guint")){
      guint val;

      g_value_init(&(pointer->value),
		   G_TYPE_UINT);
      
      val = g_ascii_strtoull((gchar *) str,
			     NULL,
			     10);

      g_value_set_uint(&(pointer->value),
		       val);
    }else if(!g_strcmp0((gchar *) type,
			"gint")){
      gint val;

      g_value_init(&(pointer->value),
		   G_TYPE_UINT);
      
      val = g_ascii_strtoll((gchar *) str,
			    NULL,
			    10);

      g_value_set_int(&(pointer->value),
		      val);
    }else if(!g_strcmp0((gchar *) type,
			"gdouble")){
      gdouble val;
      
      g_value_init(&(pointer->value),
		   G_TYPE_DOUBLE);

      val = g_ascii_strtod((gchar *) str,
			   NULL);

      g_value_set_double(&(pointer->value),
			 val);
    }else if(!g_strcmp0((gchar *) type,
			"AgsComplex")){
      AgsComplex z;

      ags_complex_set(&z,
		      0.0 + I * 0.0);
      
      g_value_init(&(pointer->value),
		   AGS_TYPE_COMPLEX);

      sscanf((char *) str, "%lf %lf", &(z.real), &(z.imag));

      g_value_set_boxed(&(pointer->value),
			&z);
    }else{
      g_value_init(&(pointer->value),
		   G_TYPE_STRING);

      g_value_set_string(&(pointer->value),
			 g_strdup((gchar *) str));
    }
  }

  if(type != NULL){
    xmlFree(type);
  }

  if(str != NULL){
    xmlFree(str);
  }
}

void
ags_simple_file_read_strv(AgsSimpleFile *simple_file, xmlNode *node, gchar ***strv)
{
  xmlNode *child;
  
  gchar **current;

  guint i;
  
  current = NULL;
  
  child = node->children;

  i = 0;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     BAD_CAST "ags-sf-str",
		     11)){
	if(current == NULL){
	  current = malloc(2 * sizeof(gchar *));
	}else{
	  current = realloc(current,
			    (i + 2) * sizeof(gchar *));
	}

	current[i] = xmlNodeGetContent(child);
	
	i++;
      }
    }

    child = child->next;
  }

  /* set return value */
  strv[0] = current;
}

void
ags_simple_file_read_value(AgsSimpleFile *simple_file, xmlNode *node, GValue **value)
{
  GValue *current;

  gchar *str, *type;
  
  current = value[0];
  
  str = (gchar *) xmlGetProp(node,
			     BAD_CAST "value");

  type = (gchar *) xmlGetProp(node,
			      BAD_CAST "type");

  if(str != NULL){
    if(!g_strcmp0(type,
		  "gboolean")){
      g_value_init(current,
		   G_TYPE_BOOLEAN);
      
      if(!g_ascii_strcasecmp(str,
			     "false")){
	g_value_set_boolean(current,
			    FALSE);
      }else{
	g_value_set_boolean(current,
			    TRUE);
      }
    }else if(!g_strcmp0(type,
			"guint")){
      guint val;

      g_value_init(current,
		   G_TYPE_UINT);
      
      val = g_ascii_strtoull(str,
			     NULL,
			     10);

      g_value_set_uint(current,
		       val);
    }else if(!g_strcmp0(type,
			"gint")){
      gint val;

      g_value_init(current,
		   G_TYPE_UINT);
      
      val = g_ascii_strtoll(str,
			    NULL,
			    10);

      g_value_set_int(current,
		      val);
    }else if(!g_strcmp0(type,
			"gdouble")){
      gdouble val;
      
      g_value_init(current,
		   G_TYPE_DOUBLE);

      val = g_ascii_strtod(str,
			   NULL);

      g_value_set_double(current,
			 val);
    }else if(!g_strcmp0(type,
			"AgsComplex")){
      AgsComplex z;

      ags_complex_set(&z,
		      0.0 + I * 0.0);
      
      g_value_init(current,
		   AGS_TYPE_COMPLEX);

      sscanf(str, "%lf %lf", &(z.real), &(z.imag));
      g_value_set_boxed(current,
			&z);
    }else{
      g_value_init(current,
		   G_TYPE_STRING);

      g_value_set_string(current,
			 g_strdup(str));
    }
  }

  if(type != NULL){
    xmlFree(type);
  }

  if(str != NULL){
    xmlFree(str);
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

  application_context = ags_application_context_get_instance();
  
  if(*window != NULL){
    gobject = *window;
  }else{
    gobject = ags_window_new();
    ags_ui_provider_set_window(AGS_UI_PROVIDER(application_context),
			       (GtkWidget *) gobject);
    
    *window = gobject;
  }

  str = xmlGetProp(node,
		   BAD_CAST "filename");

  if(str != NULL){
    gobject->name = (gchar *) str;

    gtk_window_set_title((GtkWindow *) gobject, g_strconcat("GSequencer - ", gobject->name, NULL));
  }
  
  /* children */  
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-machine-list",
		     20)){
	GList *machine_start;

	machine_start = NULL;
	ags_simple_file_read_machine_list(simple_file,
					  child,
					  &machine_start);
	g_list_free(machine_start);
      }
    }

    child = child->next;
  }

  /* children */  
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-composite-editor",
		     24)){
	ags_simple_file_read_composite_editor(simple_file,
					      child,
					      &(gobject->composite_editor));
      }
    }

    child = child->next;
  }
  
  config = ags_config_get_instance();

  /* presets */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "pcm-channels");

  if(str != NULL){
    pcm_channels = g_ascii_strtoull(str,
				    NULL,
				    10);
    g_free(str);
  }
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate");
  
  if(str != NULL){
    samplerate = g_ascii_strtoull(str,
				  NULL,
				  10);
    free(str);
  }

  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "buffer-size");
  if(str != NULL){
    buffer_size = g_ascii_strtoull(str,
				   NULL,
				   10);
    free(str);
  }

  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "format");
  if(str != NULL){
    format = g_ascii_strtoull(str,
			      NULL,
			      10);
    free(str);
  }
  
  /* launch settings */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "node", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start",
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
		   BAD_CAST "bpm");

  if(str != NULL){
    bpm = g_ascii_strtod(str,
			 NULL);
    gtk_spin_button_set_value(window->navigation->bpm,
			      bpm);

    xmlFree(str);
  }

  /* loop */
  str = xmlGetProp(file_launch->node,
		   "loop");

  if(str != NULL){
    if(!g_ascii_strcasecmp(str,
			   "false")){
      gtk_check_button_set_active((GtkCheckButton *) window->navigation->loop,
				  FALSE);
    }else{
      gtk_check_button_set_active((GtkCheckButton *) window->navigation->loop,
				  TRUE);
    }

    xmlFree(str);
  }
  
  /* loop start */
  str = xmlGetProp(file_launch->node,
		   "loop-start");
  
  if(str != NULL){
    loop_start = g_ascii_strtod(str,
				NULL);
    gtk_spin_button_set_value(window->navigation->loop_left_tact,
			      loop_start);

    xmlFree(str);
  }

  /* loop end */
  str = xmlGetProp(file_launch->node,
		   "loop-end");

  if(str != NULL){
    loop_end = g_ascii_strtod(str,
			      NULL);
    gtk_spin_button_set_value(window->navigation->loop_right_tact,
			      loop_end);

    xmlFree(str);
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
		     (xmlChar *) "ags-sf-machine",
		     11)){
	current = NULL;

	if(*machine != NULL){
	  GList *iter;

	  iter = g_list_nth(*machine,
			    i);

	  if(iter != NULL){
	    current = iter->data;
	  }
	}
	
	ags_simple_file_read_machine(simple_file, child, &current);

	if(current != NULL){
	  list = g_list_prepend(list, current);
	}
	
	i++;
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *machine = list;
}

void
ags_simple_file_read_machine_resize_audio_channels(AgsMachine *machine,
						   guint new_size, guint old_size,
						   gpointer data)
{
  gboolean *resized;

  resized = data;

  resized[0] = TRUE;
}

void
ags_simple_file_read_machine_resize_pads(AgsMachine *machine,
					 GType channel_type,
					 guint new_size, guint old_size,
					 gpointer data)
{
  gboolean *resized;

  resized = data;

  if(channel_type == AGS_TYPE_OUTPUT){
    resized[0] = TRUE;
  }else{
    resized[1] = TRUE;    
  }
}

void
ags_simple_file_read_machine(AgsSimpleFile *simple_file, xmlNode *node, AgsMachine **machine)
{
  AgsWindow *window;
  AgsMachine *gobject;

  AgsConfig *config;

  AgsFileLaunch *file_launch;

  AgsApplicationContext *application_context;
  
  GObject *soundcard;

  xmlNode *child;
  
  GList *start_list, *list;
  GList *output_pad;
  GList *input_pad;

  xmlChar *device;
  xmlChar *type_name;
  xmlChar *audio_name;
  xmlChar *str;
  
  guint audio_channels;
  guint output_pads, input_pads;
  gboolean audio_channel_wait_data[1];
  gboolean wait_audio_channel;
  gboolean wait_data[2];
  gboolean wait_output, wait_input;
  guint i;

  gobject = NULL;
  
  type_name = NULL;

  if(*machine != NULL){
    gobject = *machine;
  }else{
    AgsApplicationContext *application_context;
  
    GList *start_list;

    xmlChar *filename, *effect;


    application_context = ags_application_context_get_instance();

    type_name = xmlGetProp(node,
			   AGS_SIMPLE_FILE_TYPE_PROP);

    filename = xmlGetProp(node,
			  "plugin-file");

    effect = xmlGetProp(node,
			"effect");

    if(effect != NULL &&
       (!g_ascii_strncasecmp(type_name,
			     "AgsLv2Bridge",
			     13) ||
	!g_ascii_strncasecmp(type_name,
			     "AgsLiveLv2Bridge",
			     17))){
      xmlChar *version;
      
      guint major, minor, micro;
      
      /* fixup 3.2.7 */
      version = xmlGetProp(simple_file->root_node,
			   "version");
    
      major = 0;
      minor = 0;
      micro = 0;
    
      if(version != NULL){
	sscanf(version, "%d.%d.%d",
	       &major,
	       &minor,
	       &micro);

	xmlFree(version);
      }

      if(major < 3 ||
	 (major == 3 &&
	  micro < 2) ||
	 (major == 3 &&
	  micro < 2 &&
	  micro < 7)){
	gchar *tmp;

	tmp = NULL;
      
	if(!g_ascii_strncasecmp(effect, "\"\"\"", 3)){
	  tmp = g_strndup(effect + 3,
			  (gsize) (strlen(effect) - 6));
	}else if(!g_ascii_strncasecmp(effect, "\"\"", 2)){
	  tmp = g_strndup(effect + 2,
			  (gsize) (strlen(effect) - 4));
	}else if(!g_ascii_strncasecmp(effect, "'''", 3)){
	  tmp = g_strndup(effect + 3,
			  (gsize) (strlen(effect) - 6));
	}else if(!g_ascii_strncasecmp(effect, "''", 2)){
	  tmp = g_strndup(effect + 2,
			  (gsize) (strlen(effect) - 4));
	}else if(effect[0] == '"'){
	  tmp = g_strndup(effect + 1,
			  (gsize) (strlen(effect) - 2));
	}else if(effect[0] == '\''){
	  tmp = g_strndup(effect + 1,
			  (gsize) (strlen(effect) - 2));
	}

	if(tmp != NULL){
	  xmlFree(effect);

	  effect = xmlStrdup(tmp);
	  g_free(tmp);
	}
      }
    }
    
    if(type_name != NULL){
      gobject = ags_machine_util_new_by_type_name(type_name,
						  filename, effect);
    }    

    *machine = gobject;
    
    /* add to sound provider */
    if(gobject != NULL){
      /* ref audio */
      g_object_ref(gobject->audio);

      start_list = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));
      g_list_foreach(start_list,
		     (GFunc) g_object_unref,
		     NULL);

      g_object_ref(gobject->audio);
      start_list = g_list_append(start_list,
				 gobject->audio);
    
      ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(application_context),
				   start_list);

      /* AgsAudio */
      ags_connectable_connect(AGS_CONNECTABLE(gobject->audio));
    }else{
      GtkDialog *failed_dialog;

      failed_dialog = gtk_message_dialog_new(ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context)),
					     GTK_DIALOG_DESTROY_WITH_PARENT,
					     GTK_MESSAGE_ERROR,
					     GTK_BUTTONS_OK,
					     "failed to instantiate machine %s\nplugin - \nfilename: %s\neffect: %s\n",
					     type_name,
					     filename,
					     effect);

      gtk_widget_show(failed_dialog);

      g_signal_connect(failed_dialog, "response",
		       G_CALLBACK(gtk_window_destroy), NULL);
    }

    if(type_name != NULL){
      xmlFree(type_name);
    }

    if(filename != NULL){
      xmlFree(filename);
    }

    if(effect != NULL){
      xmlFree(effect);
    }
    
    ags_gsequencer_application_context_task_timeout((AgsGSequencerApplicationContext *) application_context);

    ags_ui_provider_check_message(AGS_UI_PROVIDER(application_context));
    ags_ui_provider_clean_message(AGS_UI_PROVIDER(application_context));
  }

  if(gobject == NULL ||
     !AGS_IS_MACHINE(gobject)){
    return;
  }

  application_context = ags_application_context_get_instance();
  
  ags_simple_file_add_id_ref(simple_file,
			     g_object_new(AGS_TYPE_FILE_ID_REF,
					  "file", simple_file,
					  "node", node,
					  "reference", gobject,
					  NULL));
  
  /* retrieve window */  
  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  config = ags_config_get_instance();
  
  /* find soundcard */
  start_list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

  soundcard = NULL;
  if(!(simple_file->no_config)){
    device = xmlGetProp(node,
			"soundcard-device");

    if(device != NULL){
      list = start_list;
    
      for(i = 0; list != NULL; i++){
	str = ags_soundcard_get_device(AGS_SOUNDCARD(list->data));
      
	if(str != NULL){
	  if(!g_ascii_strcasecmp(str,
				 device)){
	    soundcard = list->data;
	  
	    break;
	  }
	}

	/* iterate soundcard */
	list = list->next;
      }

      xmlFree(device);
    }
  }
  
  if(soundcard == NULL &&
     start_list != NULL){
    soundcard = start_list->data;

    //    g_message("soundcard fallback %s", G_OBJECT_TYPE_NAME(soundcard));
  }

  g_list_free_full(start_list,
		   g_object_unref);
  
  g_object_set(gobject->audio,
	       "output-soundcard", soundcard,
	       NULL);

  /* audio name */
  audio_name = xmlGetProp(node,
			  "audio-name");

  if(audio_name != NULL){
    g_object_set(gobject->audio,
		 "audio-name", audio_name,
		 NULL);

    xmlFree(audio_name);
  }
  
  /* machine specific */
  if(AGS_IS_LADSPA_BRIDGE(gobject)){
    //empty
  }else if(AGS_IS_DSSI_BRIDGE(gobject)){
    //empty
  }else if(AGS_IS_LIVE_DSSI_BRIDGE(gobject)){
    //empty
  }else if(AGS_IS_LV2_BRIDGE(gobject)){
    //empty
  }else if(AGS_IS_LIVE_LV2_BRIDGE(gobject)){
    //empty
  }

  /* set name if available */
  str = xmlGetProp(node,
		   "name");

  if(str != NULL){
    g_object_set(gobject,
		 "machine-name", str,
		 NULL);

    xmlFree(str);
  }

  /* reverse mapping */
  str = xmlGetProp(node,
		   "reverse-mapping");
  
  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "true",
			  5)){
    ags_audio_set_behaviour_flags(gobject->audio, (AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING));
  }else{
    ags_audio_unset_behaviour_flags(gobject->audio, (AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING));
  }

  if(str != NULL){
    xmlFree(str);
  }
    
  /* retrieve channel allocation */
  output_pads = gobject->audio->output_pads;
  input_pads = gobject->audio->input_pads;

  audio_channels = gobject->audio->audio_channels;
  
  wait_audio_channel = FALSE;

  audio_channel_wait_data[0] = FALSE;

  wait_output = FALSE;
  wait_input = FALSE;
  
  wait_data[0] = FALSE;
  wait_data[1] = FALSE;

  g_signal_connect_after(gobject, "resize-audio-channels", 
			 G_CALLBACK(ags_simple_file_read_machine_resize_audio_channels), audio_channel_wait_data);
  
  g_signal_connect_after(gobject, "resize-pads", 
			 G_CALLBACK(ags_simple_file_read_machine_resize_pads), wait_data);
  
  str = xmlGetProp(node,
		   "channels");

  if(str != NULL){
    audio_channels = g_ascii_strtoull(str,
				      NULL,
				      10);

    ags_audio_set_audio_channels(gobject->audio,
				 audio_channels, gobject->audio->audio_channels);

    wait_audio_channel = TRUE;
    
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "input-pads");

  if(str != NULL){
    input_pads = g_ascii_strtoull(str,
				  NULL,
				  10);

    ags_audio_set_pads(gobject->audio,
		       AGS_TYPE_INPUT,
		       input_pads, gobject->audio->input_pads);
    wait_input = TRUE;

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "output-pads");

  if(str != NULL){
    output_pads = g_ascii_strtoull(str,
				   NULL,
				   10);
    
    ags_audio_set_pads(gobject->audio,
		       AGS_TYPE_OUTPUT,
		       output_pads, gobject->audio->output_pads);
    wait_output = TRUE;

    xmlFree(str);
  }

  /* dispatch */
#if 0
  while((wait_audio_channel && !audio_channel_wait_data[0]) ||
	(wait_output && !wait_data[0]) ||
	(wait_input && !wait_data[1])){
    usleep(AGS_USEC_PER_SEC / 30);
    g_main_context_iteration(NULL,
			     FALSE);
  }
#endif
  
  g_object_disconnect(gobject,
		      "any_signal::resize-audio-channels", 
		      G_CALLBACK(ags_simple_file_read_machine_resize_audio_channels),
		      audio_channel_wait_data,
		      NULL);

  g_object_disconnect(gobject,
		      "any_signal::resize-pads", 
		      G_CALLBACK(ags_simple_file_read_machine_resize_pads),
		      wait_data,
		      NULL);

  ags_gsequencer_application_context_task_timeout(application_context);

  ags_ui_provider_check_message(AGS_UI_PROVIDER(application_context));
  ags_ui_provider_clean_message(AGS_UI_PROVIDER(application_context));

  /* children */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-pad-list",
		     16)){
	GList *pad;

	pad = NULL;
	ags_simple_file_read_pad_list(simple_file,
				      child,
				      &pad);

	g_list_free(pad);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-effect-pad-list",
			   23)){
	GList *effect_pad;

	effect_pad = NULL;
	ags_simple_file_read_effect_pad_list(simple_file,
					     child,
					     &effect_pad);

	g_list_free(effect_pad);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-effect-list",
			   19)){
	AgsEffectBulk *effect_bulk;

	AgsLadspaManager *ladspa_manager;
	AgsLv2Manager *lv2_manager;

#if defined(AGS_WITH_VST3)
	AgsVst3Manager *vst3_manager;
#endif
	
	xmlNode *effect_list_child;

	gchar *plugin_name;
	xmlChar *filename, *effect;

	gint position;
	gboolean is_ladspa, is_lv2;
#if defined(AGS_WITH_VST3)
	gboolean is_vst3;
#endif
	gboolean is_output;
	guint pads;

	if(AGS_IS_LADSPA_BRIDGE(gobject) ||
	   AGS_IS_DSSI_BRIDGE(gobject) ||
	   AGS_IS_LIVE_DSSI_BRIDGE(gobject) ||
	   AGS_IS_LV2_BRIDGE(gobject) ||
	   AGS_IS_LIVE_LV2_BRIDGE(gobject)
#if defined(AGS_WITH_VST3)
	   || AGS_IS_VST3_BRIDGE(gobject)
	   || AGS_IS_LIVE_VST3_BRIDGE(gobject)
#endif

	   ){
	  child = child->next;
	  
	  continue;
	}
	
	is_output = TRUE;
	str = xmlGetProp(child,
			 "is-output");

	pads = output_pads;
	
	if(str != NULL){
	  if(!g_ascii_strcasecmp(str,
				 "false")){
	    is_output = FALSE;

	    pads = input_pads;
	  }

	  xmlFree(str);
	}

	if(is_output){
	  effect_bulk = (AgsEffectBulk *) AGS_EFFECT_BRIDGE(gobject->bridge)->bulk_output;
	}else{
	  effect_bulk = (AgsEffectBulk *) AGS_EFFECT_BRIDGE(gobject->bridge)->bulk_input;
	}

	plugin_name = NULL;
	
	filename = xmlGetProp(child,
			      "filename");
	effect = xmlGetProp(child,
			    "effect");

	position = 0;

	//NOTE:JK: related to ags-fx-buffer
	if(!is_output){
	  if((AGS_MACHINE_IS_SEQUENCER & (gobject->flags)) != 0 ||
	     (AGS_MACHINE_IS_SYNTHESIZER & (gobject->flags)) != 0 ||
	     (AGS_MACHINE_IS_WAVE_PLAYER & (gobject->flags)) != 0){
	    position = 1;
	  }
	}
	
	ladspa_manager = ags_ladspa_manager_get_instance();
	lv2_manager = ags_lv2_manager_get_instance();
#if defined(AGS_WITH_VST3)
	vst3_manager = ags_vst3_manager_get_instance();
#endif
	
	is_ladspa = (ags_ladspa_manager_find_ladspa_plugin(ladspa_manager, filename, effect) != NULL) ? TRUE: FALSE;
	is_lv2 = (g_strv_contains(lv2_manager->quick_scan_plugin_filename, filename) && g_strv_contains(lv2_manager->quick_scan_plugin_effect, effect)) ? TRUE: FALSE;

#if defined(AGS_WITH_VST3)
	is_vst3 = (ags_vst3_manager_find_vst3_plugin(vst3_manager, filename, effect) != NULL) ? TRUE: FALSE;
#endif
	
	if(is_ladspa){
	  plugin_name = "ags-fx-ladspa";
	}else if(is_lv2){
	  plugin_name = "ags-fx-lv2";
#if defined(AGS_WITH_VST3)
	}else if(is_vst3){
	  plugin_name = "ags-fx-vst3";
#endif
	}
	
	ags_effect_bulk_add_plugin(effect_bulk,
				   NULL,
				   ags_recall_container_new(), ags_recall_container_new(),
				   plugin_name,
				   filename,
				   effect,
				   0, audio_channels,
				   0, pads,
				   position,
				   (AGS_FX_FACTORY_ADD | (is_output ? AGS_FX_FACTORY_OUTPUT: AGS_FX_FACTORY_INPUT)), 0);

	if(filename != NULL){
	  xmlFree(filename);
	}

	if(effect != NULL){
	  xmlFree(effect);
	}
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-oscillator-list",
			   19)){
	if(AGS_IS_SYNCSYNTH(gobject)){
	  GList *oscillator;
	  GList *start_list, *list;

	  guint count;
	  guint i;

	  list = 
	    start_list = ags_syncsynth_get_oscillator(AGS_SYNCSYNTH(gobject));
	  count = g_list_length(start_list);

	  while(list != NULL){
	    ags_syncsynth_remove_oscillator((AgsSyncsynth *) gobject,
					    list->data);

	    /* iterate */
	    list = list->next;
	  }

	  g_list_free(start_list);
	  
	  oscillator = NULL;
	  
	  ags_simple_file_read_oscillator_list(simple_file,
					       child,
					       &oscillator);

	  list =
	    start_list = g_list_reverse(oscillator);
	  
	  while(list != NULL){
	    ags_syncsynth_add_oscillator((AgsSyncsynth *) gobject,
					 list->data);
	    
	    ags_connectable_connect(AGS_CONNECTABLE(list->data));

	    g_signal_connect((GObject *) list->data, "control-changed",
			     G_CALLBACK(ags_syncsynth_oscillator_control_changed_callback), (gpointer) gobject);

	    list = list->next;
	  }
	  
	  g_list_free(start_list);
	}
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-fm-oscillator-list",
			   21)){
	if(AGS_IS_FM_SYNCSYNTH(gobject)){
	  GList *fm_oscillator;
	  GList *start_list, *list;

	  list = 
	    start_list = ags_fm_syncsynth_get_fm_oscillator(AGS_FM_SYNCSYNTH(gobject));

	  while(list != NULL){
	    ags_fm_syncsynth_remove_fm_oscillator((AgsFMSyncsynth *) gobject,
						  list->data);

	    /* iterate */
	    list = list->next;
	  }

	  g_list_free(start_list);
	  
	  fm_oscillator = NULL;
	  
	  ags_simple_file_read_fm_oscillator_list(simple_file,
						  child,
						  &fm_oscillator);

	  list =
	    start_list = g_list_reverse(fm_oscillator);
	  
	  while(list != NULL){
	    ags_fm_syncsynth_add_fm_oscillator((AgsFMSyncsynth *) gobject,
					       list->data);

	    ags_connectable_connect(AGS_CONNECTABLE(list->data));

	    g_signal_connect((GObject *) list->data, "control-changed",
			     G_CALLBACK(ags_fm_syncsynth_fm_oscillator_control_changed_callback), (gpointer) gobject);

	    list = list->next;
	  }
	  
	  g_list_free(start_list);
	}
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-pattern-list",
			   20)){
	xmlNode *pattern_list_child;
	
	
  	/* pattern list children */
	pattern_list_child = child->children;

	while(pattern_list_child != NULL){
	  if(pattern_list_child->type == XML_ELEMENT_NODE){	    
	    if(!xmlStrncmp(pattern_list_child->name,
			   (xmlChar *) "ags-sf-pattern",
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
			       "nth-line");

	      if(str != NULL){
		line = g_ascii_strtoull(str,
					NULL,
					10);

		xmlFree(str);
	      }
	      
	      channel = ags_channel_nth(gobject->audio->input,
					line);

	      /* retrieve bank */
	      bank_0 =
		bank_1 = 0;

	      str = xmlGetProp(pattern_list_child,
			       "bank-0");

	      if(str != NULL){
		bank_0 = g_ascii_strtoull(str,
					  NULL,
					  10);

		xmlFree(str);
	      }

	      str = xmlGetProp(pattern_list_child,
			       "bank-1");

	      if(str != NULL){
		bank_1 = g_ascii_strtoull(str,
					  NULL,
					  10);

		xmlFree(str);
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

	      if(channel != NULL){
		g_object_unref(channel);
	      }
	    }
	  }
	  
	  pattern_list_child = pattern_list_child->next;
	}
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-notation-list",
			   21)){
	gchar *version;

	guint major, minor;
	
	version = xmlGetProp(simple_file->root_node,
			     "version");
	major = 0;
	minor = 0;

	if(version != NULL){
	  sscanf(version, "%d.%d",
		 &major,
		 &minor);

	  xmlFree(version);
	}
	
	if(major == 0 ||
	   (major == 1 && minor < 2)){
	  ags_simple_file_read_notation_list_fixup_1_0_to_1_2(simple_file,
							      child,
							      &(gobject->audio->notation));
	}else{
	  ags_simple_file_read_notation_list(simple_file,
					     child,
					     &(gobject->audio->notation));
	}
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-preset-list",
			   21)){
	GList *preset;

	preset = NULL;
	ags_simple_file_read_preset_list(simple_file,
					 child,
					 &preset);

	g_list_free_full(gobject->audio->preset,
			 g_object_unref);

	gobject->audio->preset = preset;
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-dialog-model-list",
			   25)){
	xmlNode *dialog_model;
	xmlNode *model;

	dialog_model = child->children;

	while(dialog_model != NULL){
	  if(dialog_model->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(dialog_model->name,
			   (xmlChar *) "ags-sf-dialog-model",
			   20)){
	      model = dialog_model->children;

	      while(model != NULL){
		if(model->type == XML_ELEMENT_NODE){
		  xmlNode *copy_model;

		  copy_model = xmlCopyNode(model,
					   1);
		  ags_machine_add_dialog_model(gobject,
		  			       copy_model);
		}
		
		model = model->next;
	      }
	    }
	  }
	  
	  dialog_model = dialog_model->next;
	}
      }
    }

    child = child->next;
  }

  if(AGS_IS_LADSPA_BRIDGE(gobject)){
    //empty
  }else if(AGS_IS_DSSI_BRIDGE(gobject)){
    //empty
  }else if(AGS_IS_LIVE_DSSI_BRIDGE(gobject)){
    //empty
  }else if(AGS_IS_LV2_BRIDGE(gobject)){
    //empty
  }else if(AGS_IS_LIVE_LV2_BRIDGE(gobject)){
    //empty
  }

  /* retrieve midi mapping */
  str = xmlGetProp(node,
		   "audio-start-mapping");

  if(str != NULL){
    g_object_set(gobject->audio,
		 "audio-start-mapping", g_ascii_strtoull(str,
							 NULL,
							 10),
		 NULL);
    
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "audio-end-mapping");

  if(str != NULL){
    g_object_set(gobject->audio,
		 "audio-end-mapping", g_ascii_strtoull(str,
						       NULL,
						       10),
		 NULL);
    
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "midi-start-mapping");

  if(str != NULL){
    g_object_set(gobject->audio,
		 "midi-start-mapping", g_ascii_strtoull(str,
							NULL,
							10),
		 NULL);
    
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "midi-end-mapping");

  if(str != NULL){
    g_object_set(gobject->audio,
		 "midi-end-mapping", g_ascii_strtoull(str,
						      NULL,
						      10),
		 NULL);
    
    xmlFree(str);
  }
  
  /* children */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-automation-list",
		     23)){
	GList *automation;

	gchar *version;

	guint major, minor;
	
	version = xmlGetProp(simple_file->root_node,
			     "version");
	major = 0;
	minor = 0;

	if(version != NULL){
	  sscanf(version, "%d.%d",
		 &major,
		 &minor);
    
	  xmlFree(version);
	}
	
	if(major == 0 ||
	   (major == 1 && minor < 3)){
	  ags_simple_file_read_automation_list_fixup_1_0_to_1_3(simple_file,
								child,
								&(gobject->audio->automation));
	}else{
	  ags_simple_file_read_automation_list(simple_file,
					       child,
					       &(gobject->audio->automation));
	}
      }
    }

    child = child->next;
  }
  
  /* launch AgsMachine */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "file", simple_file,
					       "node", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start",
		   G_CALLBACK(ags_simple_file_read_machine_launch), gobject);
  ags_simple_file_add_launch(simple_file,
			     (GObject *) file_launch);
}

void
ags_simple_file_read_equalizer10_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsEqualizer10 *equalizer10)
{
  xmlNode *child;

  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-sf-equalizer10",
		     19)){
	xmlNode *control_node;

	control_node = child->children;

	while(control_node != NULL){
	  if(control_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(control_node->name,
			   "ags-sf-control",
			   15)){
	      xmlChar *specifier;
	      xmlChar *value;
		
	      gdouble val;
		
	      specifier = xmlGetProp(control_node,
				     "specifier");

	      value = xmlGetProp(control_node,
				 "value");

	      val = 0.0;
		
	      if(value != NULL){
		val = g_strtod(value,
			       NULL);
    
		xmlFree(value);

		if(!xmlStrncmp(specifier,
			       "28 [Hz]",
			       8)){
		  gtk_range_set_value(GTK_RANGE(equalizer10->peak_28hz),
				      val);
		}else if(!xmlStrncmp(specifier,
				     "56 [Hz]",
				     8)){
		  gtk_range_set_value(GTK_RANGE(equalizer10->peak_56hz),
				      val);
		}else if(!xmlStrncmp(specifier,
				     "112 [Hz]",
				     9)){
		  gtk_range_set_value(GTK_RANGE(equalizer10->peak_112hz),
				      val);
		}else if(!xmlStrncmp(specifier,
				     "224 [Hz]",
				     9)){
		  gtk_range_set_value(GTK_RANGE(equalizer10->peak_224hz),
				      val);
		}else if(!xmlStrncmp(specifier,
				     "448 [Hz]",
				     9)){
		  gtk_range_set_value(GTK_RANGE(equalizer10->peak_448hz),
				      val);
		}else if(!xmlStrncmp(specifier,
				     "896 [Hz]",
				     9)){
		  gtk_range_set_value(GTK_RANGE(equalizer10->peak_896hz),
				      val);
		}else if(!xmlStrncmp(specifier,
				     "1792 [Hz]",
				     10)){
		  gtk_range_set_value(GTK_RANGE(equalizer10->peak_1792hz),
				      val);
		}else if(!xmlStrncmp(specifier,
				     "3584 [Hz]",
				     10)){
		  gtk_range_set_value(GTK_RANGE(equalizer10->peak_3584hz),
				      val);
		}else if(!xmlStrncmp(specifier,
				     "7168 [Hz]",
				     10)){
		  gtk_range_set_value(GTK_RANGE(equalizer10->peak_7168hz),
				      val);
		}else if(!xmlStrncmp(specifier,
				     "14336 [Hz]",
				     11)){
		  gtk_range_set_value(GTK_RANGE(equalizer10->peak_14336hz),
				      val);
		}else if(!xmlStrncmp(specifier,
				     "pressure",
				     9)){
		  gtk_range_set_value(GTK_RANGE(equalizer10->pressure),
				      val);
		}
	      }

	      if(specifier != NULL){
		xmlFree(specifier);
	      }
	    }
	  }
	    
	  control_node = control_node->next;
	}
	  
	break;
      }
    }

    child = child->next;
  }
}
  
void
ags_simple_file_read_drum_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsDrum *drum)
{
  xmlChar *str;
    
  guint bank_0, bank_1;
  guint i;
  
  /* bank 0 */
  bank_0 = 0;
  str = xmlGetProp(node,
		   "bank-0");

  if(str != NULL){      
    bank_0 = g_ascii_strtod(str,
			    NULL);

    if(bank_0 < 4){
      gtk_toggle_button_set_active(drum->index0[bank_0],
				   TRUE);
    }

    xmlFree(str);
  }

  /* bank 1 */
  bank_1 = 0;
  str = xmlGetProp(node,
		   "bank-1");

  if(str != NULL){      
    bank_1 = g_ascii_strtod(str,
			    NULL);

    if(bank_1 < 12){
      gtk_toggle_button_set_active(drum->index1[bank_1],
				   TRUE);
    }

    xmlFree(str);
  }

  /* loop */
  str = xmlGetProp(node,
		   "loop");
    
  if(str != NULL){
    if(!g_ascii_strcasecmp(str,
			   "true")){
      gtk_check_button_set_active((GtkCheckButton *) drum->loop_button,
				  TRUE);
    }
      
    xmlFree(str);
  }

  /* length */
  str = xmlGetProp(node,
		   "length");

  if(str != NULL){
    guint length;

    length = g_ascii_strtoull(str,
			      NULL,
			      10);
      
    gtk_spin_button_set_value(drum->length_spin,
			      (gdouble) length);
      
    xmlFree(str);
  }

  /*  */
  if(drum->pattern_box != NULL){
    AgsChannel *start_input;
    
    GList *start_list;
    GList *start_pattern;
    
    drum->pattern_box->flags |= AGS_PATTERN_BOX_BLOCK_PATTERN;

    start_list = ags_pattern_box_get_pad(drum->pattern_box);

    start_input = ags_audio_get_input(AGS_MACHINE(drum)->audio);

    start_pattern = ags_channel_get_pattern(start_input);

    if(start_pattern != NULL){
      for(i = 0; i < 16; i++){
	if(ags_pattern_get_bit(start_pattern->data, bank_0, bank_1, i)){
	  gtk_toggle_button_set_active(g_list_nth_data(start_list, i),
				       TRUE);
	}
      }
    }
        
    g_list_free(start_list);

    if(start_input != NULL){
      g_object_unref(start_input);
    }
    
    g_list_free_full(start_pattern,
		     (GDestroyNotify) g_object_unref);
    
    drum->pattern_box->flags &= (~AGS_PATTERN_BOX_BLOCK_PATTERN);
  }
}
  
void
ags_simple_file_read_matrix_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsMatrix *matrix)
{
  xmlChar *str;
    
  guint bank_0, bank_1;
    
  /* bank 1 */
  bank_1 = 0;
  str = xmlGetProp(node,
		   "bank-1");

  if(str != NULL){      
    bank_1 = g_ascii_strtod(str,
			    NULL);

    if(bank_1 < 9){
      gtk_toggle_button_set_active(matrix->index[bank_1],
				   TRUE);
    }
      
    xmlFree(str);
  }

  /* loop */
  str = xmlGetProp(node,
		   "loop");
    
  if(str != NULL){
    if(!g_ascii_strcasecmp(str,
			   "true")){
      gtk_check_button_set_active((GtkCheckButton *) matrix->loop_button,
				  TRUE);
    }
      
    xmlFree(str);
  }

  /* length */
  str = xmlGetProp(node,
		   "length");

  if(str != NULL){
    guint length;

    length = g_ascii_strtoull(str,
			      NULL,
			      10);
      
    gtk_spin_button_set_value(matrix->length_spin,
			      (gdouble) length);
      
    xmlFree(str);
  }

  /* volume */
  str = xmlGetProp(node,
		   "volume");

  if(str != NULL){
    gdouble volume;

    volume = g_ascii_strtod(str,
			    NULL);
      
    gtk_range_set_value((GtkRange *) matrix->volume,
			volume);
      
    xmlFree(str);
  }
}

void
ags_simple_file_read_synth_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsSynth *synth)
{
  xmlChar *str;
    
  /* base note */
  str = xmlGetProp(node,
		   "base-note");

  if(str != NULL){
    gdouble base_note;

    base_note = g_ascii_strtod(str,
			       NULL);

    if(base_note > AGS_SYNTH_BASE_NOTE_MIN &&
       base_note < AGS_SYNTH_BASE_NOTE_MAX){
      gtk_spin_button_set_value(synth->lower,
				(gdouble) base_note);
    }
      
    xmlFree(str);
  }
}
  
void
ags_simple_file_read_syncsynth_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsSyncsynth *syncsynth)
{
  GList *list, *list_start;

  xmlChar *str;

  gdouble tmp0, tmp1;
  gdouble loop_upper;
    
  /* base note */
  str = xmlGetProp(node,
		   "base-note");

  if(str != NULL){
    gdouble base_note;

    base_note = g_ascii_strtod(str,
			       NULL);

    if(base_note > AGS_SYNCSYNTH_BASE_NOTE_MIN &&
       base_note < AGS_SYNCSYNTH_BASE_NOTE_MAX){
      gtk_spin_button_set_value(syncsynth->lower,
				(gdouble) base_note);
    }
      
    xmlFree(str);
  }

  /* set range of loop start and loop end */
  ags_syncsynth_reset_loop(syncsynth);
    
  /* audio loop start */
  str = xmlGetProp(node,
		   "audio-loop-start");

  if(str != NULL){
    guint audio_loop_start;

    audio_loop_start = g_ascii_strtoull(str,
					NULL,
					10);
      
    gtk_spin_button_set_value(syncsynth->loop_start,
			      (gdouble) audio_loop_start);
      
    xmlFree(str);
  }

  /* audio loop end */
  str = xmlGetProp(node,
		   "audio-loop-end");

  if(str != NULL){
    guint audio_loop_end;

    audio_loop_end = g_ascii_strtoull(str,
				      NULL,
				      10);
      
    gtk_spin_button_set_value(syncsynth->loop_end,
			      (gdouble) audio_loop_end);
      
    xmlFree(str);
  }

  /* volume */
  str = xmlGetProp(node,
		   "volume");

  if(str != NULL){
    gdouble volume;

    volume = g_ascii_strtod(str,
			    NULL);
      
    gtk_range_set_value((GtkRange *) syncsynth->volume,
			volume);
      
    xmlFree(str);
  }
}

void
ags_simple_file_read_fm_synth_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsFMSynth *fm_synth)
{
  xmlChar *str;
    
  /* base note */
  str = xmlGetProp(node,
		   "base-note");

  if(str != NULL){
    gdouble base_note;

    base_note = g_ascii_strtod(str,
			       NULL);

    if(base_note > AGS_FM_SYNTH_BASE_NOTE_MIN &&
       base_note < AGS_FM_SYNTH_BASE_NOTE_MAX){
      gtk_spin_button_set_value(fm_synth->lower,
				(gdouble) base_note);
    }
      
    xmlFree(str);
  }
}

void
ags_simple_file_read_fm_syncsynth_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsFMSyncsynth *fm_syncsynth)
{
  GList *list, *list_start;

  xmlChar *str;

  gdouble tmp0, tmp1;
  gdouble loop_upper;
    
  /* base note */
  str = xmlGetProp(node,
		   "base-note");

  if(str != NULL){
    gdouble base_note;

    base_note = g_ascii_strtod(str,
			       NULL);

    if(base_note > AGS_FM_SYNCSYNTH_BASE_NOTE_MIN &&
       base_note < AGS_FM_SYNCSYNTH_BASE_NOTE_MAX){
      gtk_spin_button_set_value(fm_syncsynth->lower,
				(gdouble) base_note);
    }
      
    xmlFree(str);
  }

  /* set range of loop start and loop end */
  ags_fm_syncsynth_reset_loop(fm_syncsynth);
    
  /* audio loop start */
  str = xmlGetProp(node,
		   "audio-loop-start");

  if(str != NULL){
    guint audio_loop_start;

    audio_loop_start = g_ascii_strtoull(str,
					NULL,
					10);
      
    gtk_spin_button_set_value(fm_syncsynth->loop_start,
			      (gdouble) audio_loop_start);
      
    xmlFree(str);
  }

  /* audio loop end */
  str = xmlGetProp(node,
		   "audio-loop-end");

  if(str != NULL){
    guint audio_loop_end;

    audio_loop_end = g_ascii_strtoull(str,
				      NULL,
				      10);
      
    gtk_spin_button_set_value(fm_syncsynth->loop_end,
			      (gdouble) audio_loop_end);
      
    xmlFree(str);
  }

  /* volume */
  str = xmlGetProp(node,
		   "volume");

  if(str != NULL){
    gdouble volume;

    volume = g_ascii_strtod(str,
			    NULL);
      
    gtk_range_set_value((GtkRange *) fm_syncsynth->volume,
			volume);
      
    xmlFree(str);
  }
}

void
ags_simple_file_read_hybrid_synth_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsHybridSynth *hybrid_synth)
{
  xmlChar *str;

  /* synth-0 */
  str = xmlGetProp(node,
		   "synth-0-oscillator");

  if(str != NULL){
    guint oscillator;

    oscillator = g_ascii_strtoll(str,
				 NULL,
				 10);

    gtk_combo_box_set_active(hybrid_synth->synth_0_oscillator,
			     oscillator);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-octave");

  if(str != NULL){
    gdouble octave;

    octave = g_ascii_strtod(str,
			    NULL);

    ags_dial_set_value(hybrid_synth->synth_0_octave,
		       octave);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-key");

  if(str != NULL){
    gdouble key;

    key = g_ascii_strtod(str,
			 NULL);

    ags_dial_set_value(hybrid_synth->synth_0_key,
		       key);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-phase");

  if(str != NULL){
    gdouble phase;

    phase = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(hybrid_synth->synth_0_phase,
		       phase);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-volume");

  if(str != NULL){
    gdouble volume;

    volume = g_ascii_strtod(str,
			    NULL);

    ags_dial_set_value(hybrid_synth->synth_0_volume,
		       volume);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-sync-enabled");

  if(str != NULL && !g_strcmp0(str, "false") == FALSE){
    gtk_check_button_set_active(hybrid_synth->synth_0_sync_enabled,
				TRUE);
    
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-sync-relative-attack-factor");

  if(str != NULL){
    gdouble sync_relative_attack_factor;

    sync_relative_attack_factor = g_ascii_strtod(str,
						 NULL);

    ags_dial_set_value(hybrid_synth->synth_0_sync_relative_attack_factor,
		       sync_relative_attack_factor);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-sync-attack-0");

  if(str != NULL){
    gdouble sync_attack_0;

    sync_attack_0 = g_ascii_strtod(str,
				   NULL);

    ags_dial_set_value(hybrid_synth->synth_0_sync_attack_0,
		       sync_attack_0);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-sync-phase-0");

  if(str != NULL){
    gdouble sync_phase_0;

    sync_phase_0 = g_ascii_strtod(str,
				  NULL);

    ags_dial_set_value(hybrid_synth->synth_0_sync_phase_0,
		       sync_phase_0);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-sync-attack-1");

  if(str != NULL){
    gdouble sync_attack_1;

    sync_attack_1 = g_ascii_strtod(str,
				   NULL);

    ags_dial_set_value(hybrid_synth->synth_0_sync_attack_1,
		       sync_attack_1);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-sync-phase-1");

  if(str != NULL){
    gdouble sync_phase_1;

    sync_phase_1 = g_ascii_strtod(str,
				  NULL);

    ags_dial_set_value(hybrid_synth->synth_0_sync_phase_1,
		       sync_phase_1);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-sync-attack-2");

  if(str != NULL){
    gdouble sync_attack_2;

    sync_attack_2 = g_ascii_strtod(str,
				   NULL);

    ags_dial_set_value(hybrid_synth->synth_0_sync_attack_2,
		       sync_attack_2);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-sync-phase-2");

  if(str != NULL){
    gdouble sync_phase_2;

    sync_phase_2 = g_ascii_strtod(str,
				  NULL);

    ags_dial_set_value(hybrid_synth->synth_0_sync_phase_2,
		       sync_phase_2);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-sync-lfo-oscillator");

  if(str != NULL){
    guint sync_lfo_oscillator;

    sync_lfo_oscillator = g_ascii_strtoll(str,
					  NULL,
					  10);

    gtk_combo_box_set_active(hybrid_synth->synth_0_sync_lfo_oscillator,
			     sync_lfo_oscillator);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-sync-lfo-frequency");

  if(str != NULL){
    gdouble sync_lfo_frequency;

    sync_lfo_frequency = g_ascii_strtod(str,
					NULL);

    gtk_spin_button_set_value(hybrid_synth->synth_0_sync_lfo_frequency,
			      sync_lfo_frequency);
      
    xmlFree(str);
  }

  /* synth-1 */
  str = xmlGetProp(node,
		   "synth-1-oscillator");

  if(str != NULL){
    guint oscillator;

    oscillator = g_ascii_strtoll(str,
				 NULL,
				 10);

    gtk_combo_box_set_active(hybrid_synth->synth_1_oscillator,
			     oscillator);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-octave");

  if(str != NULL){
    gdouble octave;

    octave = g_ascii_strtod(str,
			    NULL);

    ags_dial_set_value(hybrid_synth->synth_1_octave,
		       octave);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-key");

  if(str != NULL){
    gdouble key;

    key = g_ascii_strtod(str,
			 NULL);

    ags_dial_set_value(hybrid_synth->synth_1_key,
		       key);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-phase");

  if(str != NULL){
    gdouble phase;

    phase = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(hybrid_synth->synth_1_phase,
		       phase);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-volume");

  if(str != NULL){
    gdouble volume;

    volume = g_ascii_strtod(str,
			    NULL);

    ags_dial_set_value(hybrid_synth->synth_1_volume,
		       volume);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-sync-enabled");

  if(str != NULL && !g_strcmp0(str, "false") == FALSE){
    gtk_check_button_set_active(hybrid_synth->synth_1_sync_enabled,
				TRUE);
    
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-sync-relative-attack-factor");

  if(str != NULL){
    gdouble sync_relative_attack_factor;

    sync_relative_attack_factor = g_ascii_strtod(str,
						 NULL);

    ags_dial_set_value(hybrid_synth->synth_1_sync_relative_attack_factor,
		       sync_relative_attack_factor);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-sync-attack-0");

  if(str != NULL){
    gdouble sync_attack_0;

    sync_attack_0 = g_ascii_strtod(str,
				   NULL);

    ags_dial_set_value(hybrid_synth->synth_1_sync_attack_0,
		       sync_attack_0);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-sync-phase-0");

  if(str != NULL){
    gdouble sync_phase_0;

    sync_phase_0 = g_ascii_strtod(str,
				  NULL);

    ags_dial_set_value(hybrid_synth->synth_1_sync_phase_0,
		       sync_phase_0);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-sync-attack-1");

  if(str != NULL){
    gdouble sync_attack_1;

    sync_attack_1 = g_ascii_strtod(str,
				   NULL);

    ags_dial_set_value(hybrid_synth->synth_1_sync_attack_1,
		       sync_attack_1);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-sync-phase-1");

  if(str != NULL){
    gdouble sync_phase_1;

    sync_phase_1 = g_ascii_strtod(str,
				  NULL);

    ags_dial_set_value(hybrid_synth->synth_1_sync_phase_1,
		       sync_phase_1);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-sync-attack-2");

  if(str != NULL){
    gdouble sync_attack_2;

    sync_attack_2 = g_ascii_strtod(str,
				   NULL);

    ags_dial_set_value(hybrid_synth->synth_1_sync_attack_2,
		       sync_attack_2);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-sync-phase-2");

  if(str != NULL){
    gdouble sync_phase_2;

    sync_phase_2 = g_ascii_strtod(str,
				  NULL);

    ags_dial_set_value(hybrid_synth->synth_1_sync_phase_2,
		       sync_phase_2);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-sync-lfo-oscillator");

  if(str != NULL){
    guint sync_lfo_oscillator;

    sync_lfo_oscillator = g_ascii_strtoll(str,
					  NULL,
					  10);

    gtk_combo_box_set_active(hybrid_synth->synth_1_sync_lfo_oscillator,
			     sync_lfo_oscillator);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-sync-lfo-frequency");

  if(str != NULL){
    gdouble sync_lfo_frequency;

    sync_lfo_frequency = g_ascii_strtod(str,
					NULL);

    gtk_spin_button_set_value(hybrid_synth->synth_1_sync_lfo_frequency,
			      sync_lfo_frequency);
      
    xmlFree(str);
  }
  
  /* effects */
  str = xmlGetProp(node,
		   "pitch-tuning");

  if(str != NULL){
    gdouble tuning;

    tuning = g_ascii_strtod(str,
			    NULL);

    ags_dial_set_value(hybrid_synth->pitch_tuning,
		       tuning);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "noise-gain");

  if(str != NULL){
    gdouble gain;

    gain = g_ascii_strtod(str,
			  NULL);

    ags_dial_set_value(hybrid_synth->noise_gain,
		       gain);
      
    xmlFree(str);
  }

  /* low pass */
  str = xmlGetProp(node,
		   "low-pass-enabled");

  if(str != NULL && !g_strcmp0(str, "false") == FALSE){
    gtk_check_button_set_active(hybrid_synth->low_pass_enabled,
				TRUE);
    
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "low-pass-q-lin");

  if(str != NULL){
    gdouble q_lin;

    q_lin = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(hybrid_synth->low_pass_q_lin,
		       q_lin);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "low-pass-filter-gain");

  if(str != NULL){
    gdouble filter_gain;

    filter_gain = g_ascii_strtod(str,
				 NULL);

    ags_dial_set_value(hybrid_synth->low_pass_filter_gain,
		       filter_gain);
      
    xmlFree(str);
  }

  /* high pass */
  str = xmlGetProp(node,
		   "high-pass-enabled");

  if(str != NULL && !g_strcmp0(str, "false") == FALSE){
    gtk_check_button_set_active(hybrid_synth->high_pass_enabled,
				TRUE);
    
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "high-pass-q-lin");

  if(str != NULL){
    gdouble q_lin;

    q_lin = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(hybrid_synth->high_pass_q_lin,
		       q_lin);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "high-pass-filter-gain");

  if(str != NULL){
    gdouble filter_gain;

    filter_gain = g_ascii_strtod(str,
				 NULL);

    ags_dial_set_value(hybrid_synth->high_pass_filter_gain,
		       filter_gain);
      
    xmlFree(str);
  }

  /* chorus */
  str = xmlGetProp(node,
		   "chorus-input-volume");

  if(str != NULL){
    gdouble input_volume;

    input_volume = g_ascii_strtod(str,
				  NULL);

    ags_dial_set_value(hybrid_synth->chorus_input_volume,
		       input_volume);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-output-volume");

  if(str != NULL){
    gdouble output_volume;

    output_volume = g_ascii_strtod(str,
				   NULL);

    ags_dial_set_value(hybrid_synth->chorus_output_volume,
		       output_volume);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-lfo-oscillator");

  if(str != NULL){
    guint lfo_oscillator;

    lfo_oscillator = g_ascii_strtoll(str,
				     NULL,
				     10);

    gtk_combo_box_set_active(hybrid_synth->chorus_lfo_oscillator,
			     lfo_oscillator);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-lfo-frequency");

  if(str != NULL){
    gdouble lfo_frequency;

    lfo_frequency = g_ascii_strtod(str,
				   NULL);

    gtk_spin_button_set_value(hybrid_synth->chorus_lfo_frequency,
			      lfo_frequency);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-depth");

  if(str != NULL){
    gdouble depth;

    depth = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(hybrid_synth->chorus_depth,
		       depth);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-mix");

  if(str != NULL){
    gdouble mix;

    mix = g_ascii_strtod(str,
			 NULL);

    ags_dial_set_value(hybrid_synth->chorus_mix,
		       mix);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-delay");

  if(str != NULL){
    gdouble delay;

    delay = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(hybrid_synth->chorus_delay,
		       delay);
      
    xmlFree(str);
  }
}

void
ags_simple_file_read_hybrid_fm_synth_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsHybridFMSynth *hybrid_fm_synth)
{
  xmlChar *str;

  /* synth-0 */
  str = xmlGetProp(node,
		   "synth-0-oscillator");

  if(str != NULL){
    guint oscillator;

    oscillator = g_ascii_strtoll(str,
				 NULL,
				 10);

    gtk_combo_box_set_active(hybrid_fm_synth->synth_0_oscillator,
			     oscillator);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-octave");

  if(str != NULL){
    gdouble octave;

    octave = g_ascii_strtod(str,
			    NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_0_octave,
		       octave);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-key");

  if(str != NULL){
    gdouble key;

    key = g_ascii_strtod(str,
			 NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_0_key,
		       key);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-phase");

  if(str != NULL){
    gdouble phase;

    phase = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_0_phase,
		       phase);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-volume");

  if(str != NULL){
    gdouble volume;

    volume = g_ascii_strtod(str,
			    NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_0_volume,
		       volume);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-lfo-oscillator");

  if(str != NULL){
    guint lfo_oscillator;

    lfo_oscillator = g_ascii_strtoll(str,
				     NULL,
				     10);

    gtk_combo_box_set_active(hybrid_fm_synth->synth_0_lfo_oscillator,
			     lfo_oscillator);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-lfo-frequency");

  if(str != NULL){
    gdouble lfo_frequency;

    lfo_frequency = g_ascii_strtod(str,
				   NULL);

    gtk_spin_button_set_value(hybrid_fm_synth->synth_0_lfo_frequency,
			      lfo_frequency);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-lfo-depth");

  if(str != NULL){
    gdouble lfo_depth;

    lfo_depth = g_ascii_strtod(str,
			       NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_0_lfo_depth,
		       lfo_depth);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-0-lfo-tuning");

  if(str != NULL){
    gdouble lfo_tuning;

    lfo_tuning = g_ascii_strtod(str,
				NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_0_lfo_tuning,
		       lfo_tuning);
      
    xmlFree(str);
  }

  /* synth-1 */
  str = xmlGetProp(node,
		   "synth-1-oscillator");

  if(str != NULL){
    guint oscillator;

    oscillator = g_ascii_strtoll(str,
				 NULL,
				 10);

    gtk_combo_box_set_active(hybrid_fm_synth->synth_1_oscillator,
			     oscillator);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-octave");

  if(str != NULL){
    gdouble octave;

    octave = g_ascii_strtod(str,
			    NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_1_octave,
		       octave);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-key");

  if(str != NULL){
    gdouble key;

    key = g_ascii_strtod(str,
			 NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_1_key,
		       key);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-phase");

  if(str != NULL){
    gdouble phase;

    phase = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_1_phase,
		       phase);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-volume");

  if(str != NULL){
    gdouble volume;

    volume = g_ascii_strtod(str,
			    NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_1_volume,
		       volume);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-lfo-oscillator");

  if(str != NULL){
    guint lfo_oscillator;

    lfo_oscillator = g_ascii_strtoll(str,
				     NULL,
				     10);

    gtk_combo_box_set_active(hybrid_fm_synth->synth_1_lfo_oscillator,
			     lfo_oscillator);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-lfo-frequency");

  if(str != NULL){
    gdouble lfo_frequency;

    lfo_frequency = g_ascii_strtod(str,
				   NULL);

    gtk_spin_button_set_value(hybrid_fm_synth->synth_1_lfo_frequency,
			      lfo_frequency);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-lfo-depth");

  if(str != NULL){
    gdouble lfo_depth;

    lfo_depth = g_ascii_strtod(str,
			       NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_1_lfo_depth,
		       lfo_depth);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-1-lfo-tuning");

  if(str != NULL){
    gdouble lfo_tuning;

    lfo_tuning = g_ascii_strtod(str,
				NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_1_lfo_tuning,
		       lfo_tuning);
      
    xmlFree(str);
  }

  /* synth-2 */
  str = xmlGetProp(node,
		   "synth-2-oscillator");

  if(str != NULL){
    guint oscillator;

    oscillator = g_ascii_strtoll(str,
				 NULL,
				 10);

    gtk_combo_box_set_active(hybrid_fm_synth->synth_2_oscillator,
			     oscillator);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-2-octave");

  if(str != NULL){
    gdouble octave;

    octave = g_ascii_strtod(str,
			    NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_2_octave,
		       octave);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-2-key");

  if(str != NULL){
    gdouble key;

    key = g_ascii_strtod(str,
			 NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_2_key,
		       key);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-2-phase");

  if(str != NULL){
    gdouble phase;

    phase = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_2_phase,
		       phase);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-2-volume");

  if(str != NULL){
    gdouble volume;

    volume = g_ascii_strtod(str,
			    NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_2_volume,
		       volume);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-2-lfo-oscillator");

  if(str != NULL){
    guint lfo_oscillator;

    lfo_oscillator = g_ascii_strtoll(str,
				     NULL,
				     10);

    gtk_combo_box_set_active(hybrid_fm_synth->synth_2_lfo_oscillator,
			     lfo_oscillator);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-2-lfo-frequency");

  if(str != NULL){
    gdouble lfo_frequency;

    lfo_frequency = g_ascii_strtod(str,
				   NULL);

    gtk_spin_button_set_value(hybrid_fm_synth->synth_2_lfo_frequency,
			      lfo_frequency);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-2-lfo-depth");

  if(str != NULL){
    gdouble lfo_depth;

    lfo_depth = g_ascii_strtod(str,
			       NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_2_lfo_depth,
		       lfo_depth);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-2-lfo-tuning");

  if(str != NULL){
    gdouble lfo_tuning;

    lfo_tuning = g_ascii_strtod(str,
				NULL);

    ags_dial_set_value(hybrid_fm_synth->synth_2_lfo_tuning,
		       lfo_tuning);
      
    xmlFree(str);
  }
  
  /* effects */
  str = xmlGetProp(node,
		   "pitch-tuning");

  if(str != NULL){
    gdouble tuning;

    tuning = g_ascii_strtod(str,
			    NULL);

    ags_dial_set_value(hybrid_fm_synth->pitch_tuning,
		       tuning);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "noise-gain");

  if(str != NULL){
    gdouble gain;

    gain = g_ascii_strtod(str,
			  NULL);

    ags_dial_set_value(hybrid_fm_synth->noise_gain,
		       gain);
      
    xmlFree(str);
  }

  /* low pass */
  str = xmlGetProp(node,
		   "low-pass-enabled");

  if(str != NULL && !g_strcmp0(str, "false") == FALSE){
    gtk_check_button_set_active(hybrid_fm_synth->low_pass_enabled,
				TRUE);
    
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "low-pass-q-lin");

  if(str != NULL){
    gdouble q_lin;

    q_lin = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(hybrid_fm_synth->low_pass_q_lin,
		       q_lin);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "low-pass-filter-gain");

  if(str != NULL){
    gdouble filter_gain;

    filter_gain = g_ascii_strtod(str,
				 NULL);

    ags_dial_set_value(hybrid_fm_synth->low_pass_filter_gain,
		       filter_gain);
      
    xmlFree(str);
  }

  /* high pass */
  str = xmlGetProp(node,
		   "high-pass-enabled");

  if(str != NULL && !g_strcmp0(str, "false") == FALSE){
    gtk_check_button_set_active(hybrid_fm_synth->high_pass_enabled,
				TRUE);
    
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "high-pass-q-lin");

  if(str != NULL){
    gdouble q_lin;

    q_lin = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(hybrid_fm_synth->high_pass_q_lin,
		       q_lin);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "high-pass-filter-gain");

  if(str != NULL){
    gdouble filter_gain;

    filter_gain = g_ascii_strtod(str,
				 NULL);

    ags_dial_set_value(hybrid_fm_synth->high_pass_filter_gain,
		       filter_gain);
      
    xmlFree(str);
  }

  /* chorus */
  str = xmlGetProp(node,
		   "chorus-input-volume");

  if(str != NULL){
    gdouble input_volume;

    input_volume = g_ascii_strtod(str,
				  NULL);

    ags_dial_set_value(hybrid_fm_synth->chorus_input_volume,
		       input_volume);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-output-volume");

  if(str != NULL){
    gdouble output_volume;

    output_volume = g_ascii_strtod(str,
				   NULL);

    ags_dial_set_value(hybrid_fm_synth->chorus_output_volume,
		       output_volume);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-lfo-oscillator");

  if(str != NULL){
    guint lfo_oscillator;

    lfo_oscillator = g_ascii_strtoll(str,
				     NULL,
				     10);

    gtk_combo_box_set_active(hybrid_fm_synth->chorus_lfo_oscillator,
			     lfo_oscillator);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-lfo-frequency");

  if(str != NULL){
    gdouble lfo_frequency;

    lfo_frequency = g_ascii_strtod(str,
				   NULL);

    gtk_spin_button_set_value(hybrid_fm_synth->chorus_lfo_frequency,
			      lfo_frequency);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-depth");

  if(str != NULL){
    gdouble depth;

    depth = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(hybrid_fm_synth->chorus_depth,
		       depth);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-mix");

  if(str != NULL){
    gdouble mix;

    mix = g_ascii_strtod(str,
			 NULL);

    ags_dial_set_value(hybrid_fm_synth->chorus_mix,
		       mix);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-delay");

  if(str != NULL){
    gdouble delay;

    delay = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(hybrid_fm_synth->chorus_delay,
		       delay);
      
    xmlFree(str);
  }
}

void
ags_simple_file_read_pitch_sampler_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsPitchSampler *pitch_sampler)
{
  GtkTreeModel *model;

  GtkTreeIter iter;

  xmlChar *filename;
  xmlChar *enable_synth_generator;
  xmlChar *pitch_type;
  xmlChar *key_count;
  xmlChar *base_note;
  xmlChar *str;
  xmlChar *version;

  gchar *value;
      
  guint major, minor, micro;
      
  /* fixup 3.7.3 */
  version = xmlGetProp(simple_file->root_node,
		       "version");
    
  major = 0;
  minor = 0;
  micro = 0;
    
  if(version != NULL){
    sscanf(version, "%d.%d.%d",
	   &major,
	   &minor,
	   &micro);

    xmlFree(version);
  }

  filename = xmlGetProp(node,
			"filename");

  if(filename != NULL){
    gtk_editable_set_text(GTK_EDITABLE(pitch_sampler->filename),
			  filename);
  }

  enable_synth_generator = xmlGetProp(node,
				      "enable-synth-generator");

  key_count = xmlGetProp(node,
			 "key-count");

  pitch_type = xmlGetProp(node,
			  "pitch-type");
  
  base_note = xmlGetProp(node,
			 "base-note");

  if(enable_synth_generator != NULL &&
     !g_ascii_strncasecmp(enable_synth_generator,
			  "true",
			  5)){
    gtk_check_button_set_active((GtkCheckButton *) pitch_sampler->enable_synth_generator,
				TRUE);
  }

  model = gtk_combo_box_get_model(pitch_sampler->pitch_function);

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gchar *value;
      
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);

      if(!g_strcmp0(pitch_type,
		    value)){
	gtk_combo_box_set_active_iter(pitch_sampler->pitch_function,
				      &iter);
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }
  
  if(base_note != NULL){
    if(major < 3 ||
       (major == 3 &&
	minor < 7) ||
       (major == 3 &&
	minor == 7 &&
	micro < 3)){
      gtk_spin_button_set_value(pitch_sampler->lower,
				g_ascii_strtod(base_note,
					       NULL) - 48.0);
    }else{
      gtk_spin_button_set_value(pitch_sampler->lower,
				g_ascii_strtod(base_note,
					       NULL));
    }
  }

  if(key_count != NULL){
    gtk_spin_button_set_value(pitch_sampler->key_count,
			      g_ascii_strtod(key_count,
					     NULL));
  }

  ags_pitch_sampler_open_filename(pitch_sampler,
				  filename);

  /* aliase */
  str = xmlGetProp(node,
		   "enable-aliase");

  if(str != NULL){
    gtk_check_button_set_active(pitch_sampler->enable_aliase,
				((!g_ascii_strncasecmp(str, "true", 5)) ? TRUE: FALSE));
    
    xmlFree(str);
  }
  
  /* aliase a amount */
  str = xmlGetProp(node,
		   "aliase-a-amount");

  if(str != NULL){
    gdouble a_amount;

    a_amount = g_ascii_strtod(str,
			      NULL);
      
    gtk_adjustment_set_value(pitch_sampler->aliase_a_amount->adjustment,
			     a_amount);
      
    xmlFree(str);
  }

  /* aliase a phase */
  str = xmlGetProp(node,
		   "aliase-a-phase");

  if(str != NULL){
    gdouble a_phase;

    a_phase = g_ascii_strtod(str,
			     NULL);
      
    gtk_adjustment_set_value(pitch_sampler->aliase_a_phase->adjustment,
			     a_phase);
      
    xmlFree(str);
  }
  
  /* aliase b amount */
  str = xmlGetProp(node,
		   "aliase-b-amount");

  if(str != NULL){
    gdouble b_amount;

    b_amount = g_ascii_strtod(str,
			      NULL);
      
    gtk_adjustment_set_value(pitch_sampler->aliase_b_amount->adjustment,
			     b_amount);
      
    xmlFree(str);
  }

  /* aliase b phase */
  str = xmlGetProp(node,
		   "aliase-b-phase");

  if(str != NULL){
    gdouble b_phase;

    b_phase = g_ascii_strtod(str,
			     NULL);
      
    gtk_adjustment_set_value(pitch_sampler->aliase_b_phase->adjustment,
			     b_phase);
      
    xmlFree(str);
  }
  
  /* volume */
  str = xmlGetProp(node,
		   "volume");

  if(str != NULL){
    gdouble volume;

    volume = g_ascii_strtod(str,
			    NULL);
      
    gtk_range_set_value((GtkRange *) pitch_sampler->volume,
			volume);
      
    xmlFree(str);
  }

  /* lfo */
  str = xmlGetProp(node,
		   "enable-lfo");

  if(str != NULL){
    gtk_check_button_set_active(pitch_sampler->enable_lfo,
				((!g_ascii_strncasecmp(str, "true", 5)) ? TRUE: FALSE));
    
    xmlFree(str);
  }

  /* lfo freq */
  str = xmlGetProp(node,
		   "lfo-freq");
    
  if(str != NULL){
    gtk_spin_button_set_value(pitch_sampler->lfo_freq,
			      g_ascii_strtod(str,
					     NULL));
    xmlFree(str);
  }

  /* lfo phase */
  str = xmlGetProp(node,
		   "lfo-phase");
    
  if(str != NULL){
    gtk_spin_button_set_value(pitch_sampler->lfo_phase,
			      g_ascii_strtod(str,
					     NULL));
    xmlFree(str);
  }

  /* lfo depth */
  str = xmlGetProp(node,
		   "lfo-depth");
    
  if(str != NULL){
    gtk_spin_button_set_value(pitch_sampler->lfo_depth,
			      g_ascii_strtod(str,
					     NULL));
    xmlFree(str);
  }

  /* tuning */
  str = xmlGetProp(node,
		   "lfo-tuning");
    
  if(str != NULL){
    gtk_spin_button_set_value(pitch_sampler->lfo_tuning,
			      g_ascii_strtod(str,
					     NULL));
    xmlFree(str);
  }

  if(filename != NULL){      
    xmlFree(filename);
  }

  if(enable_synth_generator != NULL){
    xmlFree(enable_synth_generator);
  }

  if(pitch_type != NULL){
    xmlFree(pitch_type);
  }

  if(key_count != NULL){
    xmlFree(key_count);
  }

  if(base_note != NULL){
    xmlFree(base_note);
  }
}

void
ags_simple_file_read_sfz_synth_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsSFZSynth *sfz_synth)
{
  xmlChar *filename;
  xmlChar *str;

  filename = xmlGetProp(node,
			"filename");

  if(filename != NULL){
    gtk_editable_set_text(GTK_EDITABLE(sfz_synth->filename),
			  filename);
  }
  
  ags_sfz_synth_open_filename(sfz_synth,
			      filename);
  
  /* synth */
  str = xmlGetProp(node,
		   "synth-octave");

  if(str != NULL){
    gdouble octave;

    octave = g_ascii_strtod(str,
			    NULL);

    ags_dial_set_value(sfz_synth->synth_octave,
		       octave);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-key");

  if(str != NULL){
    gdouble key;

    key = g_ascii_strtod(str,
			 NULL);

    ags_dial_set_value(sfz_synth->synth_key,
		       key);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-volume");

  if(str != NULL){
    gdouble volume;

    volume = g_ascii_strtod(str,
			    NULL);

    ags_dial_set_value(sfz_synth->synth_volume,
		       volume);
      
    xmlFree(str);
  }
  
  /* chorus */
  str = xmlGetProp(node,
		   "chorus-input-volume");

  if(str != NULL){
    gdouble input_volume;

    input_volume = g_ascii_strtod(str,
				  NULL);

    ags_dial_set_value(sfz_synth->chorus_input_volume,
		       input_volume);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-output-volume");

  if(str != NULL){
    gdouble output_volume;

    output_volume = g_ascii_strtod(str,
				   NULL);

    ags_dial_set_value(sfz_synth->chorus_output_volume,
		       output_volume);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-lfo-oscillator");

  if(str != NULL){
    guint lfo_oscillator;

    lfo_oscillator = g_ascii_strtoll(str,
				     NULL,
				     10);

    gtk_combo_box_set_active(sfz_synth->chorus_lfo_oscillator,
			     lfo_oscillator);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-lfo-frequency");

  if(str != NULL){
    gdouble lfo_frequency;

    lfo_frequency = g_ascii_strtod(str,
				   NULL);

    gtk_spin_button_set_value(sfz_synth->chorus_lfo_frequency,
			      lfo_frequency);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-depth");

  if(str != NULL){
    gdouble depth;

    depth = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->chorus_depth,
		       depth);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-mix");

  if(str != NULL){
    gdouble mix;

    mix = g_ascii_strtod(str,
			 NULL);

    ags_dial_set_value(sfz_synth->chorus_mix,
		       mix);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-delay");

  if(str != NULL){
    gdouble delay;

    delay = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->chorus_delay,
		       delay);
      
    xmlFree(str);
  }

  /* ext */
  str = xmlGetProp(node,
		   "tremolo-enabled");

  if(str != NULL){
    gboolean enabled;

    enabled = (!g_ascii_strncasecmp(str, AGS_SIMPLE_FILE_TRUE, 5)) ? TRUE: FALSE;

    gtk_check_button_set_active(sfz_synth->tremolo_enabled,
				enabled);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "tremolo-gain");

  if(str != NULL){
    gdouble gain;

    gain = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->tremolo_gain,
		       gain);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "tremolo-lfo-depth");

  if(str != NULL){
    gdouble lfo_depth;

    lfo_depth = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->tremolo_lfo_depth,
		       lfo_depth);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "tremolo-lfo-freq");

  if(str != NULL){
    gdouble lfo_freq;

    lfo_freq = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->tremolo_lfo_freq,
		       lfo_freq);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "tremolo-tuning");

  if(str != NULL){
    gdouble tuning;

    tuning = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->tremolo_tuning,
		       tuning);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "vibrato-enabled");

  if(str != NULL){
    gboolean enabled;

    enabled = (!g_ascii_strncasecmp(str, AGS_SIMPLE_FILE_TRUE, 5)) ? TRUE: FALSE;

    gtk_check_button_set_active(sfz_synth->vibrato_enabled,
				enabled);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "vibrato-gain");

  if(str != NULL){
    gdouble gain;

    gain = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->vibrato_gain,
		       gain);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "vibrato-lfo-depth");

  if(str != NULL){
    gdouble lfo_depth;

    lfo_depth = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->vibrato_lfo_depth,
		       lfo_depth);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "vibrato-lfo-freq");

  if(str != NULL){
    gdouble lfo_freq;

    lfo_freq = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->vibrato_lfo_freq,
		       lfo_freq);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "vibrato-tuning");

  if(str != NULL){
    gdouble tuning;

    tuning = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->vibrato_tuning,
		       tuning);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "wah-wah-enabled");

  if(str != NULL){
    gboolean enabled;

    enabled = (!g_ascii_strncasecmp(str, AGS_SIMPLE_FILE_TRUE, 5)) ? TRUE: FALSE;

    gtk_check_button_set_active(sfz_synth->wah_wah_enabled,
				enabled);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "wah-wah-length");

  if(str != NULL){
    gint length;

    length = (gint) g_ascii_strtoll(str,
				    NULL,
				    10);

    gtk_combo_box_set_active(sfz_synth->wah_wah_enabled,
			     length);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "wah-wah-attack-x");

  if(str != NULL){
    gdouble attack_x;

    attack_x = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->wah_wah_attack_x,
		       attack_x);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "wah-wah-attack-y");

  if(str != NULL){
    gdouble attack_y;

    attack_y = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->wah_wah_attack_y,
		       attack_y);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "wah-wah-decay-x");

  if(str != NULL){
    gdouble decay_x;

    decay_x = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->wah_wah_decay_x,
		       decay_x);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "wah-wah-decay-y");

  if(str != NULL){
    gdouble decay_y;

    decay_y = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->wah_wah_decay_y,
		       decay_y);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "wah-wah-sustain-x");

  if(str != NULL){
    gdouble sustain_x;

    sustain_x = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->wah_wah_sustain_x,
		       sustain_x);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "wah-wah-sustain-y");

  if(str != NULL){
    gdouble sustain_y;

    sustain_y = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->wah_wah_sustain_y,
		       sustain_y);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "wah-wah-release-x");

  if(str != NULL){
    gdouble release_x;

    release_x = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->wah_wah_release_x,
		       release_x);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "wah-wah-release-y");

  if(str != NULL){
    gdouble release_y;

    release_y = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->wah_wah_release_y,
		       release_y);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "wah-wah-ratio");

  if(str != NULL){
    gdouble ratio;

    ratio = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->wah_wah_ratio,
		       ratio);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "wah-wah-lfo-depth");

  if(str != NULL){
    gdouble lfo_depth;

    lfo_depth = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->wah_wah_lfo_depth,
		       lfo_depth);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "wah-wah-lfo-freq");

  if(str != NULL){
    gdouble lfo_freq;

    lfo_freq = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->wah_wah_lfo_freq,
		       lfo_freq);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "wah-wah-tuning");

  if(str != NULL){
    gdouble tuning;

    tuning = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sfz_synth->wah_wah_tuning,
		       tuning);
      
    xmlFree(str);
  }
  
  if(filename != NULL){
    xmlFree(filename);
  }
}

#ifdef AGS_WITH_LIBINSTPATCH
void
ags_simple_file_read_ffplayer_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsFFPlayer *ffplayer)
{
  GtkTreeModel *model;

  GtkTreeIter iter;

  xmlChar *filename, *preset, *instrument;
  xmlChar *enable_synth_generator;
  xmlChar *pitch_type;
  xmlChar *key_count;
  xmlChar *base_note;
  xmlChar *version;
  xmlChar *str;
  
  guint major, minor, micro;
      
  /* fixup 3.7.3 */
  version = xmlGetProp(simple_file->root_node,
		       "version");
    
  major = 0;
  minor = 0;
  micro = 0;
    
  if(version != NULL){
    sscanf(version, "%d.%d.%d",
	   &major,
	   &minor,
	   &micro);

    xmlFree(version);
  }
  
  filename = xmlGetProp(node,
			"filename");

  preset = xmlGetProp(node,
		      "preset");
    
  instrument = xmlGetProp(node,
			  "instrument");

  enable_synth_generator = xmlGetProp(node,
				      "enable-synth-generator");

  pitch_type = xmlGetProp(node,
			  "pitch-type");

  key_count = xmlGetProp(node,
			 "key-count");

  base_note = xmlGetProp(node,
			 "base-note");

  if(enable_synth_generator != NULL &&
     !g_ascii_strncasecmp(enable_synth_generator,
			  "true",
			  5)){
    gtk_check_button_set_active((GtkCheckButton *) ffplayer->enable_synth_generator,
				TRUE);
  }

  model = gtk_combo_box_get_model(ffplayer->pitch_function);

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gchar *value;
      
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);

      if(!g_strcmp0(pitch_type,
		    value)){
	gtk_combo_box_set_active_iter(ffplayer->pitch_function,
				      &iter);
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }

  if(base_note != NULL){
    if(major < 3 ||
       (major == 3 &&
	minor < 7) ||
       (major == 3 &&
	minor == 7 &&
	micro < 3)){
      gtk_spin_button_set_value(ffplayer->lower,
				g_ascii_strtod(base_note,
					       NULL) - 48.0);
    }else{
      gtk_spin_button_set_value(ffplayer->lower,
				g_ascii_strtod(base_note,
					       NULL));
    }
  }

  if(key_count != NULL){
    gtk_spin_button_set_value(ffplayer->key_count,
			      g_ascii_strtod(key_count,
					     NULL));
  }
    
  ffplayer->load_preset = g_strdup(preset);
  ffplayer->load_instrument = g_strdup(instrument);

  ags_ffplayer_open_filename(ffplayer,
			     filename);
  
  /* aliase */
  str = xmlGetProp(node,
		   "enable-aliase");

  if(str != NULL){
    gtk_check_button_set_active(ffplayer->enable_aliase,
				((!g_ascii_strncasecmp(str, "true", 5)) ? TRUE: FALSE));
    
    xmlFree(str);
  }
  
  /* aliase a amount */
  str = xmlGetProp(node,
		   "aliase-a-amount");

  if(str != NULL){
    gdouble a_amount;

    a_amount = g_ascii_strtod(str,
			      NULL);
      
    gtk_adjustment_set_value(ffplayer->aliase_a_amount->adjustment,
			     a_amount);
      
    xmlFree(str);
  }

  /* aliase a phase */
  str = xmlGetProp(node,
		   "aliase-a-phase");

  if(str != NULL){
    gdouble a_phase;

    a_phase = g_ascii_strtod(str,
			     NULL);
      
    gtk_adjustment_set_value(ffplayer->aliase_a_phase->adjustment,
			     a_phase);
      
    xmlFree(str);
  }
  
  /* aliase b amount */
  str = xmlGetProp(node,
		   "aliase-b-amount");

  if(str != NULL){
    gdouble b_amount;

    b_amount = g_ascii_strtod(str,
			      NULL);
      
    gtk_adjustment_set_value(ffplayer->aliase_b_amount->adjustment,
			     b_amount);
      
    xmlFree(str);
  }

  /* aliase b phase */
  str = xmlGetProp(node,
		   "aliase-b-phase");

  if(str != NULL){
    gdouble b_phase;

    b_phase = g_ascii_strtod(str,
			     NULL);
      
    gtk_adjustment_set_value(ffplayer->aliase_b_phase->adjustment,
			     b_phase);
    
    xmlFree(str);
  }
  
  /* volume */
  str = xmlGetProp(node,
		   "volume");

  if(str != NULL){
    gdouble volume;

    volume = g_ascii_strtod(str,
			    NULL);
      
    gtk_range_set_value((GtkRange *) ffplayer->volume,
			volume);
      
    xmlFree(str);
  }

  if(filename != NULL){
    xmlFree(filename);
  }

  if(preset != NULL){
    xmlFree(preset);
  }

  if(instrument != NULL){
    xmlFree(instrument);
  }

  if(enable_synth_generator != NULL){
    xmlFree(enable_synth_generator);
  }

  if(pitch_type != NULL){
    xmlFree(pitch_type);
  }

  if(key_count != NULL){
    xmlFree(key_count);
  }

  if(base_note != NULL){
    xmlFree(base_note);
  }
}

void
ags_simple_file_read_sf2_synth_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsSF2Synth *sf2_synth)
{
  xmlChar *filename;
  xmlChar *bank, *program;
  xmlChar *str;

  filename = xmlGetProp(node,
			"filename");

  bank = xmlGetProp(node,
		    "bank");

  program = xmlGetProp(node,
		       "program");

  if(bank != NULL){
    sf2_synth->load_bank = (gint) g_ascii_strtoll(bank,
						  NULL,
						  10);
  }

  if(bank != NULL &&
     program != NULL){
    sf2_synth->load_program = (gint) g_ascii_strtoll(program,
						     NULL,
						     10);
  }

  if(filename != NULL){
    gtk_editable_set_text(GTK_EDITABLE(sf2_synth->filename),
			  filename);
  }
  
  ags_sf2_synth_open_filename(sf2_synth,
			      filename);
  
  /* synth */
  str = xmlGetProp(node,
		   "synth-octave");

  if(str != NULL){
    gdouble octave;

    octave = g_ascii_strtod(str,
			    NULL);

    ags_dial_set_value(sf2_synth->synth_octave,
		       octave);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-key");

  if(str != NULL){
    gdouble key;

    key = g_ascii_strtod(str,
			 NULL);

    ags_dial_set_value(sf2_synth->synth_key,
		       key);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "synth-volume");

  if(str != NULL){
    gdouble volume;

    volume = g_ascii_strtod(str,
			    NULL);

    ags_dial_set_value(sf2_synth->synth_volume,
		       volume);
      
    xmlFree(str);
  }
  
  /* chorus */
  str = xmlGetProp(node,
		   "chorus-input-volume");

  if(str != NULL){
    gdouble input_volume;

    input_volume = g_ascii_strtod(str,
				  NULL);

    ags_dial_set_value(sf2_synth->chorus_input_volume,
		       input_volume);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-output-volume");

  if(str != NULL){
    gdouble output_volume;

    output_volume = g_ascii_strtod(str,
				   NULL);

    ags_dial_set_value(sf2_synth->chorus_output_volume,
		       output_volume);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-lfo-oscillator");

  if(str != NULL){
    guint lfo_oscillator;

    lfo_oscillator = g_ascii_strtoll(str,
				     NULL,
				     10);

    gtk_combo_box_set_active(sf2_synth->chorus_lfo_oscillator,
			     lfo_oscillator);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-lfo-frequency");

  if(str != NULL){
    gdouble lfo_frequency;

    lfo_frequency = g_ascii_strtod(str,
				   NULL);

    gtk_spin_button_set_value(sf2_synth->chorus_lfo_frequency,
			      lfo_frequency);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-depth");

  if(str != NULL){
    gdouble depth;

    depth = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->chorus_depth,
		       depth);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-mix");

  if(str != NULL){
    gdouble mix;

    mix = g_ascii_strtod(str,
			 NULL);

    ags_dial_set_value(sf2_synth->chorus_mix,
		       mix);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "chorus-delay");

  if(str != NULL){
    gdouble delay;

    delay = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->chorus_delay,
		       delay);
      
    xmlFree(str);
  }

  /* ext */
  str = xmlGetProp(node,
		   "tremolo-enabled");

  if(str != NULL){
    gboolean enabled;

    enabled = (!g_ascii_strncasecmp(str, AGS_SIMPLE_FILE_TRUE, 5)) ? TRUE: FALSE;

    gtk_check_button_set_active(sf2_synth->tremolo_enabled,
				enabled);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "tremolo-gain");

  if(str != NULL){
    gdouble gain;

    gain = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->tremolo_gain,
		       gain);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "tremolo-lfo-depth");

  if(str != NULL){
    gdouble lfo_depth;

    lfo_depth = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->tremolo_lfo_depth,
		       lfo_depth);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "tremolo-lfo-freq");

  if(str != NULL){
    gdouble lfo_freq;

    lfo_freq = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->tremolo_lfo_freq,
		       lfo_freq);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "tremolo-tuning");

  if(str != NULL){
    gdouble tuning;

    tuning = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->tremolo_tuning,
		       tuning);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "vibrato-enabled");

  if(str != NULL){
    gboolean enabled;

    enabled = (!g_ascii_strncasecmp(str, AGS_SIMPLE_FILE_TRUE, 5)) ? TRUE: FALSE;

    gtk_check_button_set_active(sf2_synth->vibrato_enabled,
				enabled);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "vibrato-gain");

  if(str != NULL){
    gdouble gain;

    gain = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->vibrato_gain,
		       gain);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "vibrato-lfo-depth");

  if(str != NULL){
    gdouble lfo_depth;

    lfo_depth = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->vibrato_lfo_depth,
		       lfo_depth);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "vibrato-lfo-freq");

  if(str != NULL){
    gdouble lfo_freq;

    lfo_freq = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->vibrato_lfo_freq,
		       lfo_freq);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "vibrato-tuning");

  if(str != NULL){
    gdouble tuning;

    tuning = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->vibrato_tuning,
		       tuning);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "wah-wah-enabled");

  if(str != NULL){
    gboolean enabled;

    enabled = (!g_ascii_strncasecmp(str, AGS_SIMPLE_FILE_TRUE, 5)) ? TRUE: FALSE;

    gtk_check_button_set_active(sf2_synth->wah_wah_enabled,
				enabled);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "wah-wah-length");

  if(str != NULL){
    gint length;

    length = (gint) g_ascii_strtoll(str,
				    NULL,
				    10);

    gtk_combo_box_set_active(sf2_synth->wah_wah_enabled,
			     length);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "wah-wah-attack-x");

  if(str != NULL){
    gdouble attack_x;

    attack_x = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->wah_wah_attack_x,
		       attack_x);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "wah-wah-attack-y");

  if(str != NULL){
    gdouble attack_y;

    attack_y = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->wah_wah_attack_y,
		       attack_y);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "wah-wah-decay-x");

  if(str != NULL){
    gdouble decay_x;

    decay_x = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->wah_wah_decay_x,
		       decay_x);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "wah-wah-decay-y");

  if(str != NULL){
    gdouble decay_y;

    decay_y = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->wah_wah_decay_y,
		       decay_y);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "wah-wah-sustain-x");

  if(str != NULL){
    gdouble sustain_x;

    sustain_x = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->wah_wah_sustain_x,
		       sustain_x);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "wah-wah-sustain-y");

  if(str != NULL){
    gdouble sustain_y;

    sustain_y = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->wah_wah_sustain_y,
		       sustain_y);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "wah-wah-release-x");

  if(str != NULL){
    gdouble release_x;

    release_x = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->wah_wah_release_x,
		       release_x);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "wah-wah-release-y");

  if(str != NULL){
    gdouble release_y;

    release_y = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->wah_wah_release_y,
		       release_y);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "wah-wah-ratio");

  if(str != NULL){
    gdouble ratio;

    ratio = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->wah_wah_ratio,
		       ratio);
      
    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "wah-wah-lfo-depth");

  if(str != NULL){
    gdouble lfo_depth;

    lfo_depth = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->wah_wah_lfo_depth,
		       lfo_depth);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "wah-wah-lfo-freq");

  if(str != NULL){
    gdouble lfo_freq;

    lfo_freq = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->wah_wah_lfo_freq,
		       lfo_freq);
      
    xmlFree(str);
  }
  
  str = xmlGetProp(node,
		   "wah-wah-tuning");

  if(str != NULL){
    gdouble tuning;

    tuning = g_ascii_strtod(str,
			   NULL);

    ags_dial_set_value(sf2_synth->wah_wah_tuning,
		       tuning);
      
    xmlFree(str);
  }
  
  if(filename != NULL){
    xmlFree(filename);
  }

  if(bank != NULL){
    xmlFree(bank);
  }

  if(program != NULL){
    xmlFree(program);
  }
}
#endif
  
void
ags_simple_file_read_audiorec_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsAudiorec *audiorec)
{
  xmlChar *str;
  gchar *value;

  str = xmlGetProp(node,
		   "filename");
    
  ags_audiorec_open_filename(audiorec,
			     str);

  if(str != NULL){
    gtk_editable_set_text(GTK_EDITABLE(audiorec->filename),
			  str);
  }
  
  if(str != NULL){      
    xmlFree(str);
  }
}
  
void
ags_simple_file_read_dssi_bridge_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsDssiBridge *dssi_bridge)
{
  GtkTreeModel *model;
  GtkTreeIter iter;

  xmlChar *str;
  gchar *value;

  /* program */
  //NOTE:JK: work-around
  gtk_combo_box_set_active((GtkComboBox *) dssi_bridge->program,
			   0);

  model = gtk_combo_box_get_model((GtkComboBox *) dssi_bridge->program);

  str = xmlGetProp(node,
		   "program");

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

  if(str != NULL){      
    xmlFree(str);
  }
}

void
ags_simple_file_read_live_dssi_bridge_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsLiveDssiBridge *live_dssi_bridge)
{
  GtkTreeModel *model;
  GtkTreeIter iter;

  xmlChar *str;
  gchar *value;

  /* program */
  //NOTE:JK: work-around
  gtk_combo_box_set_active((GtkComboBox *) live_dssi_bridge->program,
			   0);

  model = gtk_combo_box_get_model((GtkComboBox *) live_dssi_bridge->program);

  str = xmlGetProp(node,
		   "program");

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);

      if(!g_strcmp0(str,
		    value)){
	gtk_combo_box_set_active_iter((GtkComboBox *) live_dssi_bridge->program,
				      &iter);
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }

  if(str != NULL){      
    xmlFree(str);
  }
}

void
ags_simple_file_read_lv2_bridge_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsLv2Bridge *lv2_bridge)
{
  GtkTreeModel *model;
  GtkTreeIter iter;

  xmlChar *version;
  xmlChar *preset;
  gchar *value;

  guint major, minor, micro;

  if(lv2_bridge->preset == NULL){
    return;
  }
    
  /* program */
  //NOTE:JK: work-around
  gtk_combo_box_set_active((GtkComboBox *) lv2_bridge->preset,
			   0);

  model = gtk_combo_box_get_model((GtkComboBox *) lv2_bridge->preset);

  preset = xmlGetProp(node,
		      "preset");

  /* fixup 3.2.7 */
  version = xmlGetProp(simple_file->root_node,
		       "version");
    
  major = 0;
  minor = 0;
  micro = 0;
    
  if(version != NULL){
    sscanf(version, "%d.%d.%d",
	   &major,
	   &minor,
	   &micro);

    xmlFree(version);
  }

  if(major < 3 ||
     (major == 3 &&
      micro < 2) ||
     (major == 3 &&
      micro < 2 &&
      micro < 7)){
    gchar *tmp;

    tmp = NULL;
      
    if(!g_ascii_strncasecmp(preset, "\"\"\"", 3)){
      tmp = g_strndup(preset + 3,
		      (gsize) (strlen(preset) - 6));
    }else if(!g_ascii_strncasecmp(preset, "\"\"", 2)){
      tmp = g_strndup(preset + 2,
		      (gsize) (strlen(preset) - 4));
    }else if(!g_ascii_strncasecmp(preset, "'''", 3)){
      tmp = g_strndup(preset + 3,
		      (gsize) (strlen(preset) - 6));
    }else if(!g_ascii_strncasecmp(preset, "''", 2)){
      tmp = g_strndup(preset + 2,
		      (gsize) (strlen(preset) - 4));
    }else if(preset[0] == '"'){
      tmp = g_strndup(preset + 1,
		      (gsize) (strlen(preset) - 2));
    }else if(preset[0] == '\''){
      tmp = g_strndup(preset + 1,
		      (gsize) (strlen(preset) - 2));
    }

    if(tmp != NULL){
      xmlFree(preset);

      preset = xmlStrdup(tmp);
      g_free(tmp);
    }
  }

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);

      if(!g_strcmp0(preset,
		    value)){
	gtk_combo_box_set_active_iter((GtkComboBox *) lv2_bridge->preset,
				      &iter);
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }

  if(preset != NULL){      
    xmlFree(preset);
  }
}

void
ags_simple_file_read_live_lv2_bridge_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsLiveLv2Bridge *live_lv2_bridge)
{
  GtkTreeModel *model;
  GtkTreeIter iter;

  xmlChar *version;
  xmlChar *preset;
  gchar *value;

  guint major, minor, micro;

  if(live_lv2_bridge->preset == NULL){
    return;
  }
    
  /* program */
  //NOTE:JK: work-around
  gtk_combo_box_set_active((GtkComboBox *) live_lv2_bridge->preset,
			   0);

  model = gtk_combo_box_get_model((GtkComboBox *) live_lv2_bridge->preset);

  preset = xmlGetProp(node,
		      "preset");

  /* fixup 3.2.7 */
  version = xmlGetProp(simple_file->root_node,
		       "version");
    
  major = 0;
  minor = 0;
  micro = 0;
    
  if(version != NULL){
    sscanf(version, "%d.%d.%d",
	   &major,
	   &minor,
	   &micro);

    xmlFree(version);
  }

  if(major < 3 ||
     (major == 3 &&
      micro < 2) ||
     (major == 3 &&
      micro < 2 &&
      micro < 7)){
    gchar *tmp;

    tmp = NULL;
      
    if(!g_ascii_strncasecmp(preset, "\"\"\"", 3)){
      tmp = g_strndup(preset + 3,
		      (gsize) (strlen(preset) - 6));
    }else if(!g_ascii_strncasecmp(preset, "\"\"", 2)){
      tmp = g_strndup(preset + 2,
		      (gsize) (strlen(preset) - 4));
    }else if(!g_ascii_strncasecmp(preset, "'''", 3)){
      tmp = g_strndup(preset + 3,
		      (gsize) (strlen(preset) - 6));
    }else if(!g_ascii_strncasecmp(preset, "''", 2)){
      tmp = g_strndup(preset + 2,
		      (gsize) (strlen(preset) - 4));
    }else if(preset[0] == '"'){
      tmp = g_strndup(preset + 1,
		      (gsize) (strlen(preset) - 2));
    }else if(preset[0] == '\''){
      tmp = g_strndup(preset + 1,
		      (gsize) (strlen(preset) - 2));
    }

    if(tmp != NULL){
      xmlFree(preset);

      preset = xmlStrdup(tmp);
      g_free(tmp);
    }
  }

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);

      if(!g_strcmp0(preset,
		    value)){
	gtk_combo_box_set_active_iter((GtkComboBox *) live_lv2_bridge->preset,
				      &iter);
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));
  }

  if(preset != NULL){      
    xmlFree(preset);
  }
}

#if defined(AGS_WITH_VST3)
void
ags_simple_file_read_instantiate_vst3_plugin(AgsSimpleFile *simple_file, xmlNode *node, AgsMachine *machine, AgsVst3Plugin *vst3_plugin)
{
  AgsAudio *audio;
  AgsChannel *start_output, *output;
  AgsPlaybackDomain *playback_domain;

  AgsInstantiateVst3Plugin *instantiate_vst3_plugin;

  AgsAudioThread *audio_thread;
  AgsChannelThread *channel_thread;
  
  GList *start_playback, *playback;	
  GList *start_play, *start_recall;
  GList *start_list, *list;

  guint audio_channels;

  audio = machine->audio;
  
  start_output = NULL;
  
  start_play = NULL;
  start_recall = NULL;

  playback_domain = NULL;

  audio_channels = 0;
  
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       "output", &start_output,
	       "play", &start_play,
	       "recall", &start_recall,
	       "playback-domain", &playback_domain,
	       NULL);

  list = 
    start_list = g_list_concat(start_play,
			       start_recall);

  start_playback = NULL;
	
  g_object_get(playback_domain,
	       "output-playback", &start_playback,
	       NULL);
  
  while(list != NULL){
    if(AGS_IS_FX_VST3_AUDIO(list->data)){
      AgsTaskLauncher *task_launcher;      

      if(ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
	/*  */
	if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_PLAYBACK)){
	  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							      AGS_SOUND_SCOPE_PLAYBACK);

	  task_launcher = ags_audio_thread_get_task_launcher(audio_thread);
	
	  instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								     AGS_SOUND_SCOPE_PLAYBACK,
								     -1,
								     FALSE);
	  ags_task_launcher_add_task(task_launcher,
				     instantiate_vst3_plugin);

	  g_object_unref(audio_thread);

	  g_object_unref(task_launcher);
	}
	
	/*  */
	if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_SEQUENCER)){
	  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							      AGS_SOUND_SCOPE_SEQUENCER);

	  task_launcher = ags_audio_thread_get_task_launcher(audio_thread);
	
	  instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								     AGS_SOUND_SCOPE_SEQUENCER,
								     -1,
								     FALSE);
	  ags_task_launcher_add_task(task_launcher,
				     instantiate_vst3_plugin);

	  g_object_unref(audio_thread);

	  g_object_unref(task_launcher);
	}
	
	/*  */
	if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_NOTATION)){
	  audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							      AGS_SOUND_SCOPE_NOTATION);

	  task_launcher = ags_audio_thread_get_task_launcher(audio_thread);
	
	  instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								     AGS_SOUND_SCOPE_NOTATION,
								     -1,
								     FALSE);
	  ags_task_launcher_add_task(task_launcher,
				     instantiate_vst3_plugin);

	  g_object_unref(audio_thread);

	  g_object_unref(task_launcher);
	}
      }else{
	guint i;

	for(i = 0; i < audio_channels; i++){
	  playback = start_playback;

	  output = ags_channel_nth(start_output,
				   i);
	  
	  while(playback != NULL){
	    AgsChannel *channel;
	    
	    gboolean success;

	    channel = NULL;
	    
	    g_object_get(playback->data,
			 "channel", &channel,
			 NULL);

	    success = FALSE;

	    if(channel == output){
	      success = TRUE;
	    }

	    g_object_unref(channel);
	    
	    if(success){
	      break;
	    }

	    playback = playback->next;
	  }
	  
	  /*  */
	  if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_PLAYBACK)){
	    channel_thread = ags_playback_get_channel_thread(playback->data,
							     AGS_SOUND_SCOPE_PLAYBACK);
	
	    task_launcher = ags_channel_thread_get_task_launcher(channel_thread);
	
	    instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								       AGS_SOUND_SCOPE_PLAYBACK,
								       -1,
								       FALSE);
	    ags_task_launcher_add_task(task_launcher,
				       instantiate_vst3_plugin);

	    g_object_unref(channel_thread);

	    g_object_unref(task_launcher);
	  }
	  
	  /*  */
	  if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_SEQUENCER)){
	    channel_thread = ags_playback_get_channel_thread(playback->data,
							     AGS_SOUND_SCOPE_SEQUENCER);

	    task_launcher = ags_channel_thread_get_task_launcher(channel_thread);

	    instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								       AGS_SOUND_SCOPE_SEQUENCER,
								       -1,
								       FALSE);
	    ags_task_launcher_add_task(task_launcher,
				       instantiate_vst3_plugin);

	    g_object_unref(channel_thread);

	    g_object_unref(task_launcher);
	  }
	  
	  /*  */
	  if(ags_audio_test_ability_flags(audio, AGS_SOUND_ABILITY_NOTATION)){
	    channel_thread = ags_playback_get_channel_thread(playback->data,
							     AGS_SOUND_SCOPE_NOTATION);

	    task_launcher = ags_channel_thread_get_task_launcher(channel_thread);

	    instantiate_vst3_plugin =  ags_instantiate_vst3_plugin_new(list->data,
								       AGS_SOUND_SCOPE_NOTATION,
								       -1,
								       FALSE);
	    ags_task_launcher_add_task(task_launcher,
				       instantiate_vst3_plugin);

	    g_object_unref(channel_thread);

	    g_object_unref(task_launcher);
	  }
	  
	  g_object_unref(output);
	}	
      }
    }
    
    list = list->next;
  }
    
  if(playback_domain != NULL){
    g_object_unref(playback_domain);
  }

  g_list_free_full(start_playback,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
}

void
ags_simple_file_read_vst3_bridge_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsVst3Bridge *vst3_bridge)
{
  ags_simple_file_read_instantiate_vst3_plugin(simple_file, node, vst3_bridge, vst3_bridge->vst3_plugin);
}

void
ags_simple_file_read_live_vst3_bridge_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsLiveVst3Bridge *live_vst3_bridge)
{
  ags_simple_file_read_instantiate_vst3_plugin(simple_file, node, live_vst3_bridge, live_vst3_bridge->vst3_plugin);
}
#endif

void
ags_simple_file_read_effect_bridge_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsEffectBridge *effect_bridge)
{
  AgsMachine *machine;
  AgsEffectBulk *effect_bulk;
    
  xmlNode *child;

  xmlChar *str;
    
  gboolean is_output;

  machine = gtk_widget_get_ancestor(effect_bridge,
				    AGS_TYPE_MACHINE);
  
  is_output = TRUE;
  str = xmlGetProp(node,
		   "is-output");
    
  if(str != NULL){
    if(!g_ascii_strcasecmp(str,
			   "false")){
      is_output = FALSE;
    }

    xmlFree(str);
  }

  if(is_output){
    effect_bulk = (AgsEffectBulk *) AGS_EFFECT_BRIDGE(machine->bridge)->bulk_output;
  }else{
    effect_bulk = (AgsEffectBulk *) AGS_EFFECT_BRIDGE(machine->bridge)->bulk_input;
  }
	
  /* effect list children */
  child = node->children;
	
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-effect",
		     14)){
	ags_simple_file_read_effect_bulk_launch(simple_file, child, effect_bulk);
      }
    }

    child = child->next;
  }
}
  
void
ags_simple_file_read_effect_bulk_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsEffectBulk *effect_bulk)
{
  AgsMachine *machine;
  
  xmlNode *child;
    
  xmlChar *version;
  xmlChar *filename, *effect;

  guint major, minor, micro;
  gboolean do_fixup_3_2_7;
  
  machine = gtk_widget_get_ancestor(effect_bulk,
				    AGS_TYPE_MACHINE);
  
  filename = xmlGetProp(node,
			"filename");
	      
  effect = xmlGetProp(node,
		      "effect");

  /* fixup 3.2.7 */
  do_fixup_3_2_7 = FALSE;
  
  version = xmlGetProp(simple_file->root_node,
		       "version");
    
  major = 0;
  minor = 0;
  micro = 0;
    
  if(version != NULL){
    sscanf(version, "%d.%d.%d",
	   &major,
	   &minor,
	   &micro);

    xmlFree(version);
  }

  if(major < 3 ||
     (major == 3 &&
      micro < 2) ||
     (major == 3 &&
      micro < 2 &&
      micro < 7)){
    gchar *tmp;

    do_fixup_3_2_7 = TRUE;

    tmp = NULL;
      
    if(!g_ascii_strncasecmp(effect, "\"\"\"", 3)){
      tmp = g_strndup(effect + 3,
		      (gsize) (strlen(effect) - 6));
    }else if(!g_ascii_strncasecmp(effect, "\"\"", 2)){
      tmp = g_strndup(effect + 2,
		      (gsize) (strlen(effect) - 4));
    }else if(!g_ascii_strncasecmp(effect, "'''", 3)){
      tmp = g_strndup(effect + 3,
		      (gsize) (strlen(effect) - 6));
    }else if(!g_ascii_strncasecmp(effect, "''", 2)){
      tmp = g_strndup(effect + 2,
		      (gsize) (strlen(effect) - 4));
    }else if(effect[0] == '"'){
      tmp = g_strndup(effect + 1,
		      (gsize) (strlen(effect) - 2));
    }else if(effect[0] == '\''){
      tmp = g_strndup(effect + 1,
		      (gsize) (strlen(effect) - 2));
    }

    if(tmp != NULL){
      xmlFree(effect);

      effect = xmlStrdup(tmp);
      g_free(tmp);
    }
  }
  
  /* effect list children */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-control",
		     15)){
	GList *list_start, *list;
		    
	xmlChar *specifier;

	specifier = xmlGetProp(child,
			       "specifier");

	if(do_fixup_3_2_7){
	  gchar *tmp;

	  tmp = NULL;
      
	  if(!g_ascii_strncasecmp(specifier, "\"\"\"", 3)){
	    tmp = g_strndup(specifier + 3,
			    (gsize) (strlen(specifier) - 6));
	  }else if(!g_ascii_strncasecmp(specifier, "\"\"", 2)){
	    tmp = g_strndup(specifier + 2,
			    (gsize) (strlen(specifier) - 4));
	  }else if(!g_ascii_strncasecmp(specifier, "'''", 3)){
	    tmp = g_strndup(specifier + 3,
			    (gsize) (strlen(specifier) - 6));
	  }else if(!g_ascii_strncasecmp(specifier, "''", 2)){
	    tmp = g_strndup(specifier + 2,
			    (gsize) (strlen(specifier) - 4));
	  }else if(specifier[0] == '"'){
	    tmp = g_strndup(specifier + 1,
			    (gsize) (strlen(specifier) - 2));
	  }else if(specifier[0] == '\''){
	    tmp = g_strndup(specifier + 1,
			    (gsize) (strlen(specifier) - 2));
	  }

	  if(tmp != NULL){
	    xmlFree(specifier);

	    specifier = xmlStrdup(tmp);
	    g_free(tmp);
	  }
	}
	
	list_start = ags_effect_bulk_get_bulk_member(effect_bulk);
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

	if(specifier != NULL){
	  xmlFree(specifier);
	}
	  
	g_list_free(list_start);
      }
    }

    child = child->next;
  }

  if(filename != NULL){
    xmlFree(filename);
  }

  if(effect != NULL){
    xmlFree(effect);
  }
}
	
void
ags_simple_file_read_bulk_member_launch(AgsSimpleFile *simple_file, xmlNode *node, AgsBulkMember *bulk_member)
{
  GtkWidget *child_widget;

  xmlChar *str;
  gchar *version;

  gdouble val;
  guint major, minor, micro;

  /* logarithmic port fixup */
  version = xmlGetProp(simple_file->root_node,
		       "version");
  major = 0;
  minor = 0;
  micro = 0;

  if(version != NULL){
    sscanf(version, "%d.%d.%d",
	   &major,
	   &minor,
	   &micro);

    xmlFree(version);
  }

  str = xmlGetProp(node,
		   "value");

  child_widget = ags_bulk_member_get_widget(bulk_member);

  if(GTK_IS_RANGE(child_widget)){    
    if(str != NULL){
      val = g_ascii_strtod(str,
			   NULL);
		      
      if(bulk_member->conversion != NULL &&
	 (major == 1 || 
	  (major == 2 &&
	   minor < 2 ||
	   (minor == 2 &&
	    micro <= 8)))){
	val = ags_conversion_convert(bulk_member->conversion,
				     val,
				     TRUE);
      }

      gtk_range_set_value(GTK_RANGE(child_widget),
			  val);
    }      
  }else if(GTK_IS_SPIN_BUTTON(child_widget)){
    if(str != NULL){
      val = g_ascii_strtod(str,
			   NULL);
		      
      if(bulk_member->conversion != NULL &&
	 (major == 1 || 
	  (major == 2 &&
	   minor < 2 ||
	   (minor == 2 &&
	    micro <= 8)))){
	val = ags_conversion_convert(bulk_member->conversion,
				     val,
				     TRUE);
      }

      gtk_spin_button_set_value(GTK_SPIN_BUTTON(child_widget),
				val);
    }
  }else if(AGS_IS_DIAL(child_widget)){
    if(str != NULL){
      val = g_ascii_strtod(str,
			   NULL);
		      
      if(bulk_member->conversion != NULL &&
	 (major == 1 || 
	  (major == 2 &&
	   minor < 2 ||
	   (minor == 2 &&
	    micro <= 8)))){
	val = ags_conversion_convert(bulk_member->conversion,
				     val,
				     TRUE);
      }

      gtk_adjustment_set_value(AGS_DIAL(child_widget)->adjustment,
			       val);
      gtk_widget_queue_draw((AgsDial *) child_widget);
    }
  }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
    if(str != NULL){
      gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
				   ((!g_ascii_strncasecmp(str, "true", 5)) ? TRUE: FALSE));
    }
  }else if(GTK_IS_CHECK_BUTTON(child_widget)){
    if(str != NULL){
      gtk_check_button_set_active((GtkCheckButton *) child_widget,
				  ((!g_ascii_strncasecmp(str, "true", 5)) ? TRUE: FALSE));
    }
  }else{
    g_warning("ags_simple_file_read_bulk_member_launch() - unknown bulk member type");
  }

  if(str != NULL){
    xmlFree(str);
  }
}  

void
ags_simple_file_read_machine_launch(AgsFileLaunch *file_launch,
				    AgsMachine *machine)
{
  AgsChannel *start_output, *output;
  AgsPlaybackDomain *playback_domain;
  
  AgsAudioLoop *audio_loop;
  AgsAudioThread *audio_thread;
  
  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;
  
  GList *start_list;

  xmlNode *child;

  xmlChar *str;

  gint sound_scope;
  guint audio_channels;
  guint input_pads, output_pads;
  guint i, j;

  application_context = ags_application_context_get_instance();

  /* start threads */
  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  /* get some fields */
  start_output = NULL;

  playback_domain = NULL;
  
  g_object_get(machine->audio,
	       "audio-channels", &audio_channels,
	       "output-pads", &output_pads,
	       "input-pads", &input_pads,
	       "output", &start_output,
	       "playback-domain", &playback_domain,
	       NULL);

  /* add to start queue */
  if(ags_audio_test_ability_flags(machine->audio, AGS_SOUND_ABILITY_PLAYBACK)){
    audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							AGS_SOUND_SCOPE_PLAYBACK);
	
    ags_thread_add_start_queue(audio_loop,
			       audio_thread);
  }
  
  if(ags_audio_test_ability_flags(machine->audio, AGS_SOUND_ABILITY_SEQUENCER)){
    audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							AGS_SOUND_SCOPE_SEQUENCER);
	
    ags_thread_add_start_queue(audio_loop,
			       audio_thread);
  }
  
  if(ags_audio_test_ability_flags(machine->audio, AGS_SOUND_ABILITY_NOTATION)){
    audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							AGS_SOUND_SCOPE_NOTATION);
	
    ags_thread_add_start_queue(audio_loop,
			       audio_thread);
  }

  for(i = 0; i < output_pads; i++){
    for(j = 0; j < audio_channels; j++){
      AgsPlayback *playback;
	
      AgsChannelThread *channel_thread;
	
      output = ags_channel_nth(start_output,
			       i * audio_channels + j);

      playback = NULL;

      g_object_get(output,
		   "playback", &playback,
		   NULL);

      /* add to start queue */
      if(ags_audio_test_ability_flags(machine->audio, AGS_SOUND_ABILITY_PLAYBACK)){
	channel_thread = ags_playback_get_channel_thread(playback,
							 AGS_SOUND_SCOPE_PLAYBACK);
	
	ags_thread_add_start_queue(audio_loop,
				   channel_thread);

	if(channel_thread != NULL){
	  g_object_unref(channel_thread);
	}
      }
	
      if(ags_audio_test_ability_flags(machine->audio, AGS_SOUND_ABILITY_SEQUENCER)){
	channel_thread = ags_playback_get_channel_thread(playback,
							 AGS_SOUND_SCOPE_SEQUENCER);
	
	ags_thread_add_start_queue(audio_loop,
				   channel_thread);

	if(channel_thread != NULL){
	  g_object_unref(channel_thread);
	}
      }
      
      if(ags_audio_test_ability_flags(machine->audio, AGS_SOUND_ABILITY_NOTATION)){
	channel_thread = ags_playback_get_channel_thread(playback,
							 AGS_SOUND_SCOPE_NOTATION);
	
	ags_thread_add_start_queue(audio_loop,
				   channel_thread);

	if(channel_thread != NULL){
	  g_object_unref(channel_thread);
	}
      }
      
      g_object_unref(output);

      g_object_unref(playback);
    }
  }  
  
  if(audio_loop != NULL){
    g_object_unref(audio_loop);
  }
    
  if(playback_domain != NULL){
    g_object_unref(playback_domain);
  }
  
  /* format */
  str = xmlGetProp(file_launch->node,
		   "format");

  if(str != NULL){
    ags_audio_set_format(machine->audio,
			 (AgsSoundcardFormat) g_ascii_strtoull(str, NULL, 10));

    xmlFree(str);
  }

  /* samplerate */
  str = xmlGetProp(file_launch->node,
		   "samplerate");

  if(str != NULL){
    ags_audio_set_samplerate(machine->audio,
			     (guint) g_ascii_strtoull(str, NULL, 10));

    xmlFree(str);
  }

  if(AGS_IS_EQUALIZER10(machine)){
    ags_simple_file_read_equalizer10_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsEqualizer10 *) machine);
  }else if(AGS_IS_DRUM(machine)){
    ags_simple_file_read_drum_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsDrum *) machine);

    ags_machine_reset_pattern_envelope(machine);
  }else if(AGS_IS_MATRIX(machine)){
    ags_simple_file_read_matrix_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsMatrix *) machine);

    ags_machine_reset_pattern_envelope(machine);
  }else if(AGS_IS_SYNTH(machine)){
    ags_simple_file_read_synth_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsSynth *) machine);
  }else if(AGS_IS_SYNCSYNTH(machine)){
    ags_simple_file_read_syncsynth_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsSyncsynth *) machine);
  }else if(AGS_IS_FM_SYNTH(machine)){
    ags_simple_file_read_fm_synth_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsFMSynth *) machine);
  }else if(AGS_IS_FM_SYNCSYNTH(machine)){
    ags_simple_file_read_fm_syncsynth_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsFMSyncsynth *) machine);
  }else if(AGS_IS_HYBRID_SYNTH(machine)){
    ags_simple_file_read_hybrid_synth_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsHybridSynth *) machine);
  }else if(AGS_IS_HYBRID_FM_SYNTH(machine)){
    ags_simple_file_read_hybrid_fm_synth_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsHybridFMSynth *) machine);
  }else if(AGS_IS_PITCH_SAMPLER(machine)){
    ags_simple_file_read_pitch_sampler_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsPitchSampler *) machine);
  }else if(AGS_IS_SFZ_SYNTH(machine)){
    ags_simple_file_read_sfz_synth_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsSFZSynth *) machine);
#ifdef AGS_WITH_LIBINSTPATCH
  }else if(AGS_IS_FFPLAYER(machine)){
    ags_simple_file_read_ffplayer_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsFFPlayer *) machine);
  }else if(AGS_IS_SF2_SYNTH(machine)){
    ags_simple_file_read_sf2_synth_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsSF2Synth *) machine);
#endif
  }else if(AGS_IS_AUDIOREC(machine)){
    ags_simple_file_read_audiorec_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsAudiorec *) machine);
  }else if(AGS_IS_DSSI_BRIDGE(machine)){
    ags_simple_file_read_dssi_bridge_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsDssiBridge *) machine);
  }else if(AGS_IS_LIVE_DSSI_BRIDGE(machine)){
    ags_simple_file_read_live_dssi_bridge_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsLiveDssiBridge *) machine);
  }else if(AGS_IS_LV2_BRIDGE(machine)){
    ags_simple_file_read_lv2_bridge_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsLv2Bridge *) machine);
  }else if(AGS_IS_LIVE_LV2_BRIDGE(machine)){
    ags_simple_file_read_live_lv2_bridge_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsLiveLv2Bridge *) machine);
#if defined(AGS_WITH_VST3)
  }else if(AGS_IS_VST3_BRIDGE(machine)){
    ags_simple_file_read_vst3_bridge_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsVst3Bridge *) machine);
  }else if(AGS_IS_LIVE_VST3_BRIDGE(machine)){
    ags_simple_file_read_live_vst3_bridge_launch((AgsSimpleFile *) file_launch->file, file_launch->node, (AgsLiveVst3Bridge *) machine);
#endif
  }
  
  /* children */
  child = file_launch->node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-property-list",
		     21)){
	GList *property_start, *property;

	property_start = NULL;
	ags_simple_file_read_property_list((AgsSimpleFile *) file_launch->file,
					   child,
					   &property_start);

	property = property_start;

	while(property != NULL){
	  g_object_set_property((GObject *) machine,
				((GParameter *) property->data)->name,
				&(((GParameter *) property->data)->value));
	  
	  property = property->next;
	}
	
	g_list_free_full(property_start,
			 g_free);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-effect-list",
			   19)){
	ags_simple_file_read_effect_bridge_launch((AgsSimpleFile *) file_launch->file, child, (AgsEffectBridge *) machine->bridge);
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
		     (xmlChar *) "ags-sf-pad",
		     11)){
	current = NULL;

	if(*pad != NULL){
	  GList *iter;

	  iter = g_list_nth(*pad,
			    i);

	  if(iter != NULL){
	    current = iter->data;
	  }
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
  gboolean is_output;
  
  if(pad != NULL &&
     pad[0] != NULL){
    gobject = pad[0];

    nth_pad = gobject->channel->pad;
  }else{
    file_id_ref = (AgsFileIdRef *) ags_simple_file_find_id_ref_by_node(simple_file,
								       node->parent->parent);
    machine = file_id_ref->ref;
    
    if(!AGS_IS_MACHINE(machine)){
      return;
    }
    
    /* get nth pad */
    nth_pad = 0;
    str = xmlGetProp(node,
		     "nth-pad");
    
    if(str != NULL){
      nth_pad = g_ascii_strtoull(str,
				 NULL,
				 10);

      xmlFree(str);
    }

    /* retrieve pad */
    gobject = NULL;
    str = xmlGetProp(node->parent,
		     "is-output");

    is_output = TRUE;

    if(!g_ascii_strcasecmp(str,
			   "false")){
      is_output = FALSE;
    }
    
    if(str != NULL){
      xmlFree(str);
    }
    
    list_start = NULL;
    
    if(!is_output){
      if(machine->input_pad != NULL){
	list_start = ags_machine_get_input_pad(machine);
      }
    }else{
      if(machine->output_pad != NULL){
	list_start = ags_machine_get_output_pad(machine);
      }
    }

    list = g_list_nth(list_start,
		      nth_pad);

    if(list != NULL){
      gobject = AGS_PAD(list->data);
    }

    g_list_free(list_start);
  }
  
  ags_simple_file_add_id_ref(simple_file,
			     g_object_new(AGS_TYPE_FILE_ID_REF,
					  "file", simple_file,
					  "node", node,
					  "reference", gobject,
					  NULL));

  /* children */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-line-list",
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

  if(AGS_IS_CONNECTABLE(gobject)){
    ags_connectable_connect(AGS_CONNECTABLE(gobject));
  }
  
  /* launch AgsPad */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "node", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start",
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
		     "group");

    if(str != NULL){
      if(!g_ascii_strcasecmp(str,
			     "false")){
	gtk_toggle_button_set_active(pad->group,
				     FALSE);
      }
      
      xmlFree(str);
    }
    
    str = xmlGetProp(file_launch->node,
		     "mute");

    if(str != NULL){
      if(!g_ascii_strcasecmp(str,
			     "true")){
	gtk_toggle_button_set_active(pad->mute,
				     TRUE);
      }
      
      xmlFree(str);
    }
    
    str = xmlGetProp(file_launch->node,
		     "solo");

    if(str != NULL){
      if(!g_ascii_strcasecmp(str,
			     "true")){
	gtk_toggle_button_set_active(pad->solo,
				     TRUE);
      }
      
      xmlFree(str);
    }
  }
  
  /* children */
  child = file_launch->node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-property-list",
		     21)){
	GList *property_start, *property;

	property_start = NULL;
	ags_simple_file_read_property_list((AgsSimpleFile *) file_launch->file,
					   child,
					   &property_start);

	property = property_start;

	while(property != NULL){
	  g_object_set_property((GObject *) pad,
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
		     (xmlChar *) "ags-sf-line",
		     12)){
	current = NULL;

	if(line[0] != NULL){
	  GList *iter;

	  iter = g_list_nth(line[0],
			    i);
	  
	  if(iter != NULL){
	    current = iter->data;
	  }
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
ags_simple_file_read_line_member(AgsSimpleFile *simple_file, xmlNode *node, AgsLineMember *line_member)
{
  GtkWidget *child_widget;

  xmlChar *str;
    
  gdouble val;

  line_member->flags |= AGS_LINE_MEMBER_APPLY_INITIAL;
    
  str = xmlGetProp(node,
		   "control-type");
  
  if(str != NULL){
    if(!g_ascii_strncasecmp(str,
			    "GtkVScale",
			    11)){
      g_object_set(line_member,
		   "widget-type", GTK_TYPE_SCALE,
		   "widget-orientation", GTK_ORIENTATION_VERTICAL,
		   NULL);
    }else if(!g_ascii_strncasecmp(str,
				  "GtkHScale",
				  11)){
      g_object_set(line_member,
		   "widget-type", GTK_TYPE_SCALE,
		   "widget-orientation", GTK_ORIENTATION_HORIZONTAL,
		   NULL);
    }else{
      g_object_set(line_member,
		   "widget-type", g_type_from_name(str),
		   NULL);
    }
    
    xmlFree(str);
  }

  child_widget = ags_line_member_get_widget(line_member);
    
  /* apply value */
  str = xmlGetProp(node,
		   "value");

  if(str != NULL){
    gchar *version;

    guint major, minor, micro;

    /* logarithmic port fixup */
    version = xmlGetProp(simple_file->root_node,
			 "version");
    major = 0;
    minor = 0;
    micro = 0;

    if(version != NULL){
      sscanf(version, "%d.%d.%d",
	     &major,
	     &minor,
	     &micro);

      xmlFree(version);
    }

    if(AGS_IS_DIAL(child_widget)){
      val = g_ascii_strtod(str,
			   NULL);

      if(line_member->conversion != NULL &&
	 (major == 1 || 
	  (major == 2 &&
	   minor < 2 ||
	   (minor == 2 &&
	    micro <= 8)))){
	val = ags_conversion_convert(line_member->conversion,
				     val,
				     TRUE);
      }
	
      gtk_adjustment_set_value(AGS_DIAL(child_widget)->adjustment,
			       val);
    }else if(GTK_IS_RANGE(child_widget)){
      val = g_ascii_strtod(str,
			   NULL);    
      
      if(line_member->conversion != NULL &&
	 (major == 1 || 
	  (major == 2 &&
	   minor < 2 ||
	   (minor == 2 &&
	    micro <= 8)))){
	val = ags_conversion_convert(line_member->conversion,
				     val,
				     TRUE);
      }

      gtk_range_set_value(GTK_RANGE(child_widget),
			  val);
    }else if(GTK_IS_SPIN_BUTTON(child_widget)){
      val = g_ascii_strtod(str,
			   NULL);    
      
      if(line_member->conversion != NULL &&
	 (major == 1 || 
	  (major == 2 &&
	   minor < 2 ||
	   (minor == 2 &&
	    micro <= 8)))){
	val = ags_conversion_convert(line_member->conversion,
				     val,
				     TRUE);
      }

      gtk_spin_button_set_value(GTK_SPIN_BUTTON(child_widget),
				val);
    }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
      gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
				   ((!g_ascii_strncasecmp(str, "true", 5)) ? TRUE: FALSE));
    }else if(GTK_IS_CHECK_BUTTON(child_widget)){
      gtk_check_button_set_active((GtkCheckButton *) child_widget,
				  ((!g_ascii_strncasecmp(str, "true", 5)) ? TRUE: FALSE));
    }else{
      g_warning("ags_simple_file_read_line() - unknown line member type");
    }

    xmlFree(str);
  }
}

void
ags_simple_file_read_line(AgsSimpleFile *simple_file, xmlNode *node, AgsLine **line)
{
  AgsMachine *machine;
  AgsPad *pad;
  GObject *gobject;

  AgsLv2Manager *lv2_manager;
#if defined(AGS_WITH_VST3)
  AgsVst3Manager *vst3_manager;
#endif
  
  AgsFileLaunch *file_launch;
  AgsFileIdRef *file_id_ref;

  AgsApplicationContext *application_context;
  AgsConfig *config;

  GObject *soundcard;

  xmlNode *child;

  GList *start_list, *list;

  xmlChar *version;
  xmlChar *device;
  xmlChar *str;
  
  guint nth_line;
  guint i;
  guint major, minor, micro;
  gboolean is_output;
  gboolean do_fixup_3_2_7;

  GRecMutex *lv2_manager_mutex;
#if defined(AGS_WITH_VST3)
  GRecMutex *vst3_manager_mutex;
#endif
  
  lv2_manager = ags_lv2_manager_get_instance();

  lv2_manager_mutex = AGS_LV2_MANAGER_GET_OBJ_MUTEX(lv2_manager);

#if defined(AGS_WITH_VST3)
  vst3_manager = ags_vst3_manager_get_instance();

  vst3_manager_mutex = AGS_VST3_MANAGER_GET_OBJ_MUTEX(vst3_manager);
#endif
  
  /* fixup 3.2.7 */
  do_fixup_3_2_7 = FALSE;
  
  version = xmlGetProp(simple_file->root_node,
		       "version");
    
  major = 0;
  minor = 0;
  micro = 0;
    
  if(version != NULL){
    sscanf(version, "%d.%d.%d",
	   &major,
	   &minor,
	   &micro);

    xmlFree(version);
  }

  if(major < 3 ||
     (major == 3 &&
      micro < 2) ||
     (major == 3 &&
      micro < 2 &&
      micro < 7)){
    do_fixup_3_2_7 = TRUE;
  }
  
  machine = NULL;
  pad = NULL;

  file_id_ref = (AgsFileIdRef *) ags_simple_file_find_id_ref_by_node(simple_file,
								     node->parent->parent->parent->parent);
    
  if(AGS_IS_MACHINE(file_id_ref->ref)){
    machine = file_id_ref->ref;
  }
  
  if(line != NULL &&
     line[0] != NULL){
    gobject = G_OBJECT(line[0]);

    nth_line = AGS_LINE(gobject)->channel->line;

    is_output = AGS_IS_OUTPUT(AGS_LINE(gobject)->channel) ? TRUE: FALSE;
  }else{
    GList *list_start, *list;
    
    file_id_ref = (AgsFileIdRef *) ags_simple_file_find_id_ref_by_node(simple_file,
								       node->parent->parent);
    
    if(!AGS_IS_PAD(file_id_ref->ref)){
      pad = NULL;
    }else{
      pad = file_id_ref->ref;
    }
    
    /* get nth-line */
    nth_line = 0;
    str = xmlGetProp(node,
		     "nth-line");

    if(str != NULL){
      nth_line = g_ascii_strtoull(str,
				  NULL,
				  10);

      xmlFree(str);
    }

    /* retrieve line or channel */
    gobject = NULL;

    if(pad != NULL){
      list_start = ags_pad_get_line(pad);

      list = list_start;

      while(list != NULL){
	if(AGS_IS_LINE(list->data) &&
	   AGS_LINE(list->data)->channel->line == nth_line){
	  gobject = list->data;
	  
	  break;
	}

	list = list->next;
      }

      g_list_free(list_start);

      if(gobject == NULL){
	return;
      }
      
      is_output = AGS_IS_OUTPUT(AGS_LINE(gobject)->channel) ? TRUE: FALSE;
    }else{
      //      "./ancestor::*[self::ags-sf-machine][1]"
      
      if(!AGS_IS_MACHINE(machine)){
	return;
      }

      is_output = TRUE;
      str = xmlGetProp(node->parent->parent->parent,
		       "is-output");

      if(str != NULL){
	if(!g_ascii_strcasecmp(str,
			       "false")){
	  is_output = FALSE;
	}

	xmlFree(str);
      }

      if(is_output){
	gobject = (GObject *) ags_channel_nth(machine->audio->output,
					      nth_line);

	if(gobject != NULL){
	  g_object_unref(gobject);
	}
      }else{
	gobject = (GObject *) ags_channel_nth(machine->audio->input,
					      nth_line);

	if(gobject != NULL){
	  g_object_unref(gobject);
	}
      }
    }
  }

  ags_simple_file_add_id_ref(simple_file,
			     g_object_new(AGS_TYPE_FILE_ID_REF,
					  "file", simple_file,
					  "node", node,
					  "reference", gobject,
					  NULL));

  /* device */
  application_context = ags_application_context_get_instance();
  
  config = ags_config_get_instance();
  
  /* find soundcard */
  start_list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  soundcard = NULL;

  if(!(simple_file->no_config)){
    device = xmlGetProp(node,
			"soundcard-device");
    
    if(device != NULL){
      list = start_list;
    
      for(i = 0; list != NULL; i++){
	str = ags_soundcard_get_device(AGS_SOUNDCARD(list->data));
      
	if(str != NULL &&
	   !g_ascii_strcasecmp(str,
			       device)){
	  soundcard = list->data;
	
	  break;
	}

	/* iterate soundcard */
	list = list->next;
      }

      xmlFree(device);
    }
  }
  
  if(soundcard == NULL &&
     start_list != NULL){
    soundcard = start_list->data;

    //    g_message("soundcard fallback %s", G_OBJECT_TYPE_NAME(soundcard));
  }

  g_list_free_full(start_list,
		   g_object_unref);

  if(AGS_IS_LINE(gobject)){
    g_object_set(AGS_LINE(gobject)->channel,
		 "output-soundcard", soundcard,
		 NULL);
  }else if(AGS_IS_CHANNEL(gobject)){
    g_object_set(AGS_CHANNEL(gobject),
		 "output-soundcard", soundcard,
		 NULL);
  }
    
  /* children */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-effect-list",
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
			   (xmlChar *) "ags-sf-effect",
			   14)){
	      xmlNode *effect_child;

	      gchar *plugin_name;
	      xmlChar *filename, *effect;

	      gboolean is_lv2_plugin;
#if defined(AGS_WITH_VST3)
	      gboolean is_vst3_plugin;
#endif
	      plugin_name = NULL;
	      
	      filename = xmlGetProp(effect_list_child,
				    "filename");
	      
	      effect = xmlGetProp(effect_list_child,
				  "effect");

	      g_rec_mutex_lock(lv2_manager_mutex);
	      
	      is_lv2_plugin = ((lv2_manager->quick_scan_plugin_filename != NULL &&
				g_strv_contains(lv2_manager->quick_scan_plugin_filename,
						filename)) ||
			       (lv2_manager->quick_scan_instrument_filename != NULL &&
				g_strv_contains(lv2_manager->quick_scan_instrument_filename,
						filename))) ? TRUE: FALSE;
	      
	      g_rec_mutex_unlock(lv2_manager_mutex);

#if defined(AGS_WITH_VST3)
	      is_vst3_plugin = (ags_vst3_manager_find_vst3_plugin(vst3_manager, filename, effect) != NULL) ? TRUE: FALSE;
#endif

	      if(is_lv2_plugin){
		plugin_name = "ags-fx-lv2";
#if defined(AGS_WITH_VST3)
	      }else if(is_vst3_plugin){
		plugin_name = "ags-fx-vst3";
#endif
	      }else{
		plugin_name = "ags-fx-ladspa";
	      }
	      
	      if(is_lv2_plugin &&
		 do_fixup_3_2_7){
		gchar *tmp;

		tmp = NULL;
      
		if(!g_ascii_strncasecmp(effect, "\"\"\"", 3)){
		  tmp = g_strndup(effect + 3,
				  (gsize) (strlen(effect) - 6));
		}else if(!g_ascii_strncasecmp(effect, "\"\"", 2)){
		  tmp = g_strndup(effect + 2,
				  (gsize) (strlen(effect) - 4));
		}else if(!g_ascii_strncasecmp(effect, "'''", 3)){
		  tmp = g_strndup(effect + 3,
				  (gsize) (strlen(effect) - 6));
		}else if(!g_ascii_strncasecmp(effect, "''", 2)){
		  tmp = g_strndup(effect + 2,
				  (gsize) (strlen(effect) - 4));
		}else if(effect[0] == '"'){
		  tmp = g_strndup(effect + 1,
				  (gsize) (strlen(effect) - 2));
		}else if(effect[0] == '\''){
		  tmp = g_strndup(effect + 1,
				  (gsize) (strlen(effect) - 2));
		}

		if(tmp != NULL){
		  xmlFree(effect);

		  effect = xmlStrdup(tmp);
		  g_free(tmp);
		}
	      }
	      
	      if(is_lv2_plugin){
		AgsLv2Plugin *plugin;
		AgsTurtle *manifest;
		AgsTurtleManager *turtle_manager;
    
		gchar *path;
		gchar *manifest_filename;

		turtle_manager = ags_turtle_manager_get_instance();
    
		path = g_path_get_dirname(filename);

		manifest_filename = g_strdup_printf("%s%c%s",
						    path,
						    G_DIR_SEPARATOR,
						    "manifest.ttl");

		manifest = ags_turtle_manager_find(turtle_manager,
						   manifest_filename);

		if(manifest == NULL){
		  AgsLv2TurtleParser *lv2_turtle_parser;
	
		  AgsTurtle **turtle;

		  guint n_turtle;

		  if(!g_file_test(manifest_filename,
				  G_FILE_TEST_EXISTS)){
		    effect_list_child = effect_list_child->next;
		      
		    continue;
		  }

		  g_message("new turtle [Manifest] - %s", manifest_filename);
	
		  manifest = ags_turtle_new(manifest_filename);
		  ags_turtle_load(manifest,
				  NULL);
		  ags_turtle_manager_add(turtle_manager,
					 (GObject *) manifest);

		  lv2_turtle_parser = ags_lv2_turtle_parser_new(manifest);

		  n_turtle = 1;
		  turtle = (AgsTurtle **) malloc(2 * sizeof(AgsTurtle *));

		  turtle[0] = manifest;
		  turtle[1] = NULL;
	
		  ags_lv2_turtle_parser_parse(lv2_turtle_parser,
					      turtle, n_turtle);
    
		  g_object_run_dispose(lv2_turtle_parser);
		  g_object_unref(lv2_turtle_parser);
	
		  g_object_unref(manifest);
	
		  free(turtle);
		}
    
		g_free(manifest_filename);

		plugin = ags_lv2_manager_find_lv2_plugin_with_fallback(ags_lv2_manager_get_instance(),
								       filename, effect);

		if(plugin == NULL){
		  effect_list_child = effect_list_child->next;

		  if(filename != NULL &&
		     strlen(filename) > 0){
		    g_warning("unable to open plugin %s %s", filename, effect);
		  }
		  
		  continue;
		}
#if defined(AGS_WITH_VST3)
	      }else if(is_vst3_plugin){
		AgsVst3Plugin *plugin;

		plugin = ags_vst3_manager_find_vst3_plugin_with_fallback(ags_vst3_manager_get_instance(),
									 filename, effect);


		if(plugin == NULL){
		  effect_list_child = effect_list_child->next;
		  
		  if(filename != NULL &&
		     strlen(filename) > 0){
		    g_warning("unable to open plugin %s %s", filename, effect);
		  }
		  
		  continue;
		}
#endif
	      }else{
		AgsLadspaPlugin *plugin;
		
		plugin = ags_ladspa_manager_find_ladspa_plugin_with_fallback(ags_ladspa_manager_get_instance(),
									     filename, effect);

		if(plugin == NULL){
		  if(filename != NULL &&
		     strlen(filename) > 0){
		    effect_list_child = effect_list_child->next;
		  
		    g_warning("unable to open plugin %s %s", filename, effect);
		  
		    continue;
		  }
		}
	      }

	      if(filename != NULL &&
		 strlen(filename) > 0 &&
		 effect != NULL &&
		 strlen(effect) > 0){
		if(g_list_find_custom(mapped_filename,
				      filename,
				      (GCompareFunc) g_strcmp0) == NULL ||
		   g_list_find_custom(mapped_effect,
				      effect,
				      (GCompareFunc) g_strcmp0) == NULL){
		  gint position;

		  position = 0;

		  //NOTE:JK: related to ags-fx-buffer
		  if(!is_output){
		    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0 ||
		       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0 ||
		       (AGS_MACHINE_IS_WAVE_PLAYER & (machine->flags)) != 0){
		      position = 1;
		    }
		  }
		  
		  mapped_filename = g_list_prepend(mapped_filename,
						   g_strdup(filename));
		  mapped_effect = g_list_prepend(mapped_effect,
						 g_strdup(effect));

		  if(AGS_IS_LINE(gobject)){
		    ags_line_add_plugin(gobject,
					NULL,
					ags_recall_container_new(), ags_recall_container_new(),
					plugin_name,
					filename,
					effect,
					AGS_LINE(gobject)->channel->audio_channel, AGS_LINE(gobject)->channel->audio_channel + 1,
					AGS_LINE(gobject)->channel->pad, AGS_LINE(gobject)->channel->pad + 1,
					position,
					(AGS_FX_FACTORY_ADD | (is_output ? AGS_FX_FACTORY_OUTPUT: AGS_FX_FACTORY_INPUT)), 0);
		  }else if(AGS_IS_CHANNEL(gobject)){
		    GList *start_recall;

		    start_recall = ags_fx_factory_create(AGS_CHANNEL(gobject)->audio,
							 ags_recall_container_new(), ags_recall_container_new(),
							 plugin_name,
							 filename,
							 effect,
							 AGS_CHANNEL(gobject)->audio_channel, AGS_CHANNEL(gobject)->audio_channel + 1,
							 AGS_CHANNEL(gobject)->pad, AGS_CHANNEL(gobject)->pad + 1,
							 position,
							 (AGS_FX_FACTORY_ADD | (AGS_IS_OUTPUT(gobject) ? AGS_FX_FACTORY_OUTPUT: AGS_FX_FACTORY_INPUT)), 0);

		    /* unref */
		    g_list_free_full(start_recall,
				     (GDestroyNotify) g_object_unref);
		  }
		}
	      }
	      
	      /* effect list children */
	      effect_child = effect_list_child->children;

	      while(effect_child != NULL){
		if(effect_child->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(effect_child->name,
				 (xmlChar *) "ags-sf-control",
				 15)){
		    AgsLineMember *line_member;

		    GList *list_start, *list;
		    
		    xmlChar *specifier;

		    specifier = xmlGetProp(effect_child,
					   "specifier");

		    if(is_lv2_plugin &&
		       do_fixup_3_2_7){
		      gchar *tmp;

		      tmp = NULL;
      
		      if(!g_ascii_strncasecmp(specifier, "\"\"\"", 3)){
			tmp = g_strndup(specifier + 3,
					(gsize) (strlen(specifier) - 6));
		      }else if(!g_ascii_strncasecmp(specifier, "\"\"", 2)){
			tmp = g_strndup(specifier + 2,
					(gsize) (strlen(specifier) - 4));
		      }else if(!g_ascii_strncasecmp(specifier, "'''", 3)){
			tmp = g_strndup(specifier + 3,
					(gsize) (strlen(specifier) - 6));
		      }else if(!g_ascii_strncasecmp(specifier, "''", 2)){
			tmp = g_strndup(specifier + 2,
					(gsize) (strlen(specifier) - 4));
		      }else if(specifier[0] == '"'){
			tmp = g_strndup(specifier + 1,
					(gsize) (strlen(specifier) - 2));
		      }else if(specifier[0] == '\''){
			tmp = g_strndup(specifier + 1,
					(gsize) (strlen(specifier) - 2));
		      }

		      if(tmp != NULL){
			xmlFree(specifier);

			specifier = xmlStrdup(tmp);
			g_free(tmp);
		      }
		    }
		    
		    list =
		      list_start = ags_line_get_line_member(AGS_LINE(gobject));

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

		    if(specifier != NULL){
		      xmlFree(specifier);
		    }

		    if(list_start != NULL){
		      g_list_free(list_start);
		    }
		  }
		}

		effect_child = effect_child->next;
	      }

	      if(filename != NULL){
		xmlFree(filename);
	      }

	      if(effect != NULL){
		xmlFree(effect);
	      }
	    }
	  }

	  effect_list_child = effect_list_child->next;
	}

	g_list_free_full(mapped_filename,
			 g_free);
	g_list_free_full(mapped_effect,
			 g_free);
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-oscillator",
			   15)){	
	if(AGS_IS_SYNTH_INPUT_LINE(gobject)){
	  ags_simple_file_read_oscillator(simple_file, child, &(AGS_SYNTH_INPUT_LINE(gobject)->oscillator));

	  ags_connectable_connect(AGS_CONNECTABLE(AGS_SYNTH_INPUT_LINE(gobject)->oscillator));
	}
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-fm-oscillator",
			   17)){	
	if(AGS_IS_FM_SYNTH_INPUT_LINE(gobject)){
	  ags_simple_file_read_fm_oscillator(simple_file, child, &(AGS_FM_SYNTH_INPUT_LINE(gobject)->fm_oscillator));

	  ags_connectable_connect(AGS_CONNECTABLE(AGS_FM_SYNTH_INPUT_LINE(gobject)->fm_oscillator));
	}
      }else if(!xmlStrncmp(child->name,
			   (xmlChar *) "ags-sf-property-list",
			   14)){
	GList *property_start, *property;

	property_start = NULL;
	ags_simple_file_read_property_list((AgsSimpleFile *) simple_file,
					   child,
					   &property_start);

	property = property_start;

	while(property != NULL){
	  g_object_set_property((GObject *) pad,
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

  //  ags_connectable_connect(AGS_CONNECTABLE(gobject));
  
  /* launch AgsLine */
  if(AGS_IS_LINE(gobject)){
    file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
						 "file", simple_file,
						 "node", node,
						 NULL);
    g_signal_connect(G_OBJECT(file_launch), "start",
		     G_CALLBACK(ags_simple_file_read_line_launch), gobject);
    ags_simple_file_add_launch(simple_file,
			       (GObject *) file_launch);
  }else if(AGS_IS_CHANNEL(gobject)){
    file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
						 "file", simple_file,
						 "node", node,
						 NULL);
    g_signal_connect(G_OBJECT(file_launch), "start",
		     G_CALLBACK(ags_simple_file_read_channel_line_launch), gobject);
    ags_simple_file_add_launch(simple_file,
			       (GObject *) file_launch);
  }else{
    g_warning("ags_simple_file_read_line() - failed");
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

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) line,
						   AGS_TYPE_MACHINE);
  
  /* link or file */
  is_output = TRUE;
  str = xmlGetProp(file_launch->node->parent->parent->parent,
		   "is-output");

  if(str != NULL){
    if(!g_ascii_strcasecmp(str,
			   "false")){
      is_output = FALSE;
    }

    xmlFree(str);
  }

  nth_line = 0;
  str = xmlGetProp(file_launch->node,
		   "nth-line");

  if(str != NULL){
    nth_line = g_ascii_strtoull(str,
				NULL,
				10);

    xmlFree(str);
  }
  
  if(is_output){
    channel = ags_channel_nth(machine->audio->output,
			      nth_line);
  }else{
    channel = ags_channel_nth(machine->audio->input,
			      nth_line);
  }

  str = xmlGetProp(file_launch->node,
		   "filename");
    
  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "file://",
			  7)){
    AgsAudioFile *audio_file;
    AgsAudioFileLink *file_link;
    
    GList *audio_signal_list;
    
    gchar *filename;

    guint file_channel;

    /* filename */
    filename = g_strdup(&(str[7]));
    xmlFree(str);

    g_message("%s", filename);
    
    /* audio channel to read */
    file_channel = 0;

    str = xmlGetProp(file_launch->node,
		     "file-channel");
    
    if(str != NULL){
      file_channel = g_ascii_strtoull(str,
				      NULL,
				      10);
      xmlFree(str);
    }

    /* read audio signal */
    audio_file = ags_audio_file_new(filename,
				    machine->audio->output_soundcard,
				    file_channel);

    ags_audio_file_open(audio_file);
    ags_audio_file_read_audio_signal(audio_file);

    /* add audio signal */
    audio_signal_list = audio_file->audio_signal;

    file_link = g_object_new(AGS_TYPE_AUDIO_FILE_LINK,
			     "filename", filename,
			     "audio-channel", file_channel,
			     NULL);
    g_object_set(G_OBJECT(channel),
		 "file-link", file_link,
		 NULL);

    g_object_unref(file_link);

    if(audio_signal_list != NULL){
      AGS_AUDIO_SIGNAL(audio_signal_list->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
      
      ags_recycling_add_audio_signal(channel->first_recycling,
				     audio_signal_list->data);
    }
  }else{
    if(str != NULL){
      xmlFree(str);
    }
    
    str = xmlGetProp(file_launch->node,
		     "link");
    xpath_result = NULL;
    
    if(str != NULL){
      xpath_result = ags_simple_file_find_id_ref_by_xpath((AgsSimpleFile *) file_launch->file,
							  str);
      xmlFree(str);
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

	  g_error_free(error);
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

	  g_error_free(error);
	}

	break;
      }
      
      xpath_result = xpath_result->next;
    }
  }
  
  /* is-grouped */
  if(AGS_IS_LINE(line)){
    str = xmlGetProp(file_launch->node,
		     "group");

    if(str != NULL){
      if(!g_ascii_strcasecmp(str,
			     "false")){
	gtk_toggle_button_set_active(line->group,
				     FALSE);
      }
      
      xmlFree(str);
    }
  }
  
  /* children */
  child = file_launch->node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-property-list",
		     21)){
	GList *property_start, *property;

	property_start = NULL;
	ags_simple_file_read_property_list((AgsSimpleFile *) file_launch->file,
					   child,
					   &property_start);

	property = property_start;

	while(property != NULL){
	  g_object_set_property((GObject *) line,
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

  if(channel != NULL){
    g_object_unref(channel);
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
		   "filename");
    
  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "file://",
			  7)){
    AgsAudioFile *audio_file;
    AgsAudioFileLink *file_link;
    
    GList *audio_signal_list;
    
    gchar *filename;

    guint file_channel;

    /* filename */
    filename = g_strdup(&(str[7]));
    xmlFree(str);
    
    /* audio channel to read */
    file_channel = 0;

    str = xmlGetProp(file_launch->node,
		     "file-channel");

    if(str != NULL){
      file_channel = g_ascii_strtoull(str,
				      NULL,
				      10);
      xmlFree(str);
    }

    /* read audio signal */
    audio_file = ags_audio_file_new(filename,
				    channel->output_soundcard,
				    file_channel);

    ags_audio_file_open(audio_file);
    ags_audio_file_read_audio_signal(audio_file);

    /* add audio signal */
    audio_signal_list = audio_file->audio_signal;

    file_link = g_object_new(AGS_TYPE_AUDIO_FILE_LINK,
			     "filename", filename,
			     "audio-channel", file_channel,
			     NULL);
    g_object_set(G_OBJECT(channel),
		 "file-link", file_link,
		 NULL);

    g_object_unref(file_link);

    if(audio_signal_list != NULL){
      AGS_AUDIO_SIGNAL(audio_signal_list->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
      
      ags_recycling_add_audio_signal(channel->first_recycling,
				     audio_signal_list->data);
    }
  }else{
    if(str != NULL){
      xmlFree(str);
    }
    
    str = xmlGetProp(file_launch->node,
		     "link");
    xpath_result = NULL;
    
    if(str != NULL){
      xpath_result = ags_simple_file_find_id_ref_by_xpath((AgsSimpleFile *) file_launch->file,
							  str);
      xmlFree(str);
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

	  g_error_free(error);
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

	  g_error_free(error);
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
		     (xmlChar *) "ags-sf-effect-pad",
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
		     (xmlChar *) "ags-sf-effect-line-list",
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
					       "node", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start",
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
		     (xmlChar *) "ags-sf-effect-line",
		     11)){
	current = NULL;

	if(*effect_line != NULL){
	  GList *iter;

	  iter = g_list_nth(*effect_line,
			    i);

	  if(iter != NULL){
	    current = iter->data;
	  }
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
  AgsMachine *machine;
  AgsEffectLine *gobject;

  AgsFileLaunch *file_launch;
  AgsFileIdRef *file_id_ref;

  AgsLv2Manager *lv2_manager;  
#if defined(AGS_WITH_VST3)
  AgsVst3Manager *vst3_manager;  
#endif
  
  xmlNode *child;

  xmlChar *version;
  xmlChar *str;

  guint major, minor, micro;
  gboolean is_output;
  gboolean do_fixup_3_2_7;

  GRecMutex *lv2_manager_mutex;
#if defined(AGS_WITH_VST3)
  GRecMutex *vst3_manager_mutex;
#endif
  
  if(*effect_line != NULL){
    gobject = *effect_line;
  }else{
    return;
  }

  lv2_manager = ags_lv2_manager_get_instance();

  lv2_manager_mutex = AGS_LV2_MANAGER_GET_OBJ_MUTEX(lv2_manager);

#if defined(AGS_WITH_VST3)
  vst3_manager = ags_vst3_manager_get_instance();

  vst3_manager_mutex = AGS_VST3_MANAGER_GET_OBJ_MUTEX(vst3_manager);
#endif
  
  is_output = AGS_IS_OUTPUT(AGS_EFFECT_LINE(gobject)->channel) ? TRUE: FALSE;
  
  /* fixup 3.2.7 */
  do_fixup_3_2_7 = FALSE;
  
  version = xmlGetProp(simple_file->root_node,
		       "version");
    
  major = 0;
  minor = 0;
  micro = 0;
    
  if(version != NULL){
    sscanf(version, "%d.%d.%d",
	   &major,
	   &minor,
	   &micro);

    xmlFree(version);
  }

  if(major < 3 ||
     (major == 3 &&
      micro < 2) ||
     (major == 3 &&
      micro < 2 &&
      micro < 7)){
    do_fixup_3_2_7 = TRUE;
  }

  machine = NULL;

  file_id_ref = (AgsFileIdRef *) ags_simple_file_find_id_ref_by_node(simple_file,
								     node->parent->parent->parent->parent);
    
  if(AGS_IS_MACHINE(file_id_ref->ref)){
    machine = file_id_ref->ref;
  }
  
  /* children */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-effect-list",
		     19)){
	xmlNode *effect_list_child;

	GList *mapped_filename, *mapped_effect;
	
	/* effect list children */
	effect_list_child = child->children;

	mapped_filename = NULL;
	mapped_effect = NULL;

	while(effect_list_child != NULL){
	  if(effect_list_child->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(effect_list_child->name,
			   (xmlChar *) "ags-sf-effect",
			   14)){
	      xmlNode *effect_child;
	      
	      xmlChar *filename, *effect;
	      gchar *plugin_name;

	      gboolean is_lv2_plugin;
#if defined(AGS_WITH_VST3)
	      gboolean is_vst3_plugin;
#endif
	      
	      filename = xmlGetProp(effect_list_child,
				    "filename");
	      
	      effect = xmlGetProp(effect_list_child,
				  "effect");
	      
	      g_rec_mutex_lock(lv2_manager_mutex);
	      
	      is_lv2_plugin = ((lv2_manager->quick_scan_plugin_filename != NULL &&
				g_strv_contains(lv2_manager->quick_scan_plugin_filename,
						filename)) ||
			       (lv2_manager->quick_scan_instrument_filename != NULL &&
				g_strv_contains(lv2_manager->quick_scan_instrument_filename,
						filename))) ? TRUE: FALSE;
	      
	      g_rec_mutex_unlock(lv2_manager_mutex);

#if defined(AGS_WITH_VST3)
	      is_vst3_plugin = (ags_vst3_manager_find_vst3_plugin(vst3_manager, filename, effect) != NULL) ? TRUE: FALSE;
#endif

	      if(is_lv2_plugin){
		plugin_name = "ags-fx-lv2";
#if defined(AGS_WITH_VST3)
	      }else if(is_vst3_plugin){
		plugin_name = "ags-fx-vst3";
#endif
	      }else{
		plugin_name = "ags-fx-ladspa";
	      }

	      if(is_lv2_plugin &&
		 do_fixup_3_2_7){
		gchar *tmp;

		tmp = NULL;
      
		if(!g_ascii_strncasecmp(effect, "\"\"\"", 3)){
		  tmp = g_strndup(effect + 3,
				  (gsize) (strlen(effect) - 6));
		}else if(!g_ascii_strncasecmp(effect, "\"\"", 2)){
		  tmp = g_strndup(effect + 2,
				  (gsize) (strlen(effect) - 4));
		}else if(!g_ascii_strncasecmp(effect, "'''", 3)){
		  tmp = g_strndup(effect + 3,
				  (gsize) (strlen(effect) - 6));
		}else if(!g_ascii_strncasecmp(effect, "''", 2)){
		  tmp = g_strndup(effect + 2,
				  (gsize) (strlen(effect) - 4));
		}else if(effect[0] == '"'){
		  tmp = g_strndup(effect + 1,
				  (gsize) (strlen(effect) - 2));
		}else if(effect[0] == '\''){
		  tmp = g_strndup(effect + 1,
				  (gsize) (strlen(effect) - 2));
		}

		if(tmp != NULL){
		  xmlFree(effect);

		  effect = xmlStrdup(tmp);
		  g_free(tmp);
		}
	      }

	      if(is_lv2_plugin){
		AgsLv2Plugin *plugin;
		AgsTurtle *manifest;
		AgsTurtleManager *turtle_manager;
    
		gchar *path;
		gchar *manifest_filename;

		turtle_manager = ags_turtle_manager_get_instance();
    
		path = g_path_get_dirname(filename);

		manifest_filename = g_strdup_printf("%s%c%s",
						    path,
						    G_DIR_SEPARATOR,
						    "manifest.ttl");

		manifest = ags_turtle_manager_find(turtle_manager,
						   manifest_filename);

		if(manifest == NULL){
		  AgsLv2TurtleParser *lv2_turtle_parser;
	
		  AgsTurtle **turtle;

		  guint n_turtle;

		  if(!g_file_test(manifest_filename,
				  G_FILE_TEST_EXISTS)){
		    effect_list_child = effect_list_child->next;
		      
		    continue;
		  }

		  g_message("new turtle [Manifest] - %s", manifest_filename);
	
		  manifest = ags_turtle_new(manifest_filename);
		  ags_turtle_load(manifest,
				  NULL);
		  ags_turtle_manager_add(turtle_manager,
					 (GObject *) manifest);

		  lv2_turtle_parser = ags_lv2_turtle_parser_new(manifest);

		  n_turtle = 1;
		  turtle = (AgsTurtle **) malloc(2 * sizeof(AgsTurtle *));

		  turtle[0] = manifest;
		  turtle[1] = NULL;
	
		  ags_lv2_turtle_parser_parse(lv2_turtle_parser,
					      turtle, n_turtle);
    
		  g_object_run_dispose(lv2_turtle_parser);
		  g_object_unref(lv2_turtle_parser);
	
		  g_object_unref(manifest);
	
		  free(turtle);
		}
    
		g_free(manifest_filename);

		plugin = ags_lv2_manager_find_lv2_plugin_with_fallback(ags_lv2_manager_get_instance(),
								       filename, effect);

		if(plugin == NULL){
		  effect_list_child = effect_list_child->next;
		  
		  if(filename != NULL &&
		     strlen(filename) > 0){
		    g_warning("unable to open plugin %s %s", filename, effect);
		  }
		  
		  continue;
		}
#if defined(AGS_WITH_VST3)
	      }else if(is_vst3_plugin){
		AgsVst3Plugin *plugin;

		plugin = ags_vst3_manager_find_vst3_plugin_with_fallback(ags_vst3_manager_get_instance(),
									 filename, effect);


		if(plugin == NULL){
		  effect_list_child = effect_list_child->next;
		  
		  if(filename != NULL &&
		     strlen(filename) > 0){
		    g_warning("unable to open plugin %s %s", filename, effect);
		  }
		  
		  continue;
		}
#endif
	      }else{
		AgsLadspaPlugin *plugin;
		
		plugin = ags_ladspa_manager_find_ladspa_plugin_with_fallback(ags_ladspa_manager_get_instance(),
									     filename, effect);

		if(plugin == NULL){
		  if(filename != NULL &&
		     strlen(filename) > 0){
		    effect_list_child = effect_list_child->next;
		  
		    g_warning("unable to open plugin %s %s", filename, effect);
		  
		    continue;
		  }
		}
	      }
	      
	      if(filename != NULL &&
		 strlen(filename) > 0 &&
		 effect != NULL &&
		 strlen(effect) > 0){
		if(g_list_find_custom(mapped_filename,
				      filename,
				      (GCompareFunc) g_strcmp0) == NULL ||
		   g_list_find_custom(mapped_effect,
				      effect,
				      (GCompareFunc) g_strcmp0) == NULL){
		  gint position;

		  position = 0;

		  //NOTE:JK: related to ags-fx-buffer
		  if(!is_output){
		    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0 ||
		       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0 ||
		       (AGS_MACHINE_IS_WAVE_PLAYER & (machine->flags)) != 0){
		      position = 1;
		    }
		  }
		  
		  mapped_filename = g_list_prepend(mapped_filename,
						   g_strdup(filename));
		  mapped_effect = g_list_prepend(mapped_effect,
						 g_strdup(effect));

		  if(AGS_IS_EFFECT_LINE(gobject)){
		    ags_effect_line_add_plugin(gobject,
					       NULL,
					       ags_recall_container_new(), ags_recall_container_new(),
					       plugin_name,
					       filename,
					       effect,
					       AGS_LINE(gobject)->channel->audio_channel, AGS_LINE(gobject)->channel->audio_channel + 1,
					       AGS_LINE(gobject)->channel->pad, AGS_LINE(gobject)->channel->pad + 1,
					       position,
					       (AGS_FX_FACTORY_ADD | (is_output ? AGS_FX_FACTORY_OUTPUT: AGS_FX_FACTORY_INPUT)), 0);
		  }else if(AGS_IS_CHANNEL(gobject)){
		    GList *start_recall;

		    start_recall = ags_fx_factory_create(AGS_CHANNEL(gobject)->audio,
							 ags_recall_container_new(), ags_recall_container_new(),
							 plugin_name,
							 filename,
							 effect,
							 AGS_CHANNEL(gobject)->audio_channel, AGS_CHANNEL(gobject)->audio_channel + 1,
							 AGS_CHANNEL(gobject)->pad, AGS_CHANNEL(gobject)->pad + 1,
							 position,
							 (AGS_FX_FACTORY_ADD | (is_output ? AGS_FX_FACTORY_OUTPUT: AGS_FX_FACTORY_INPUT)), 0);

		    /* unref */
		    g_list_free_full(start_recall,
				     (GDestroyNotify) g_object_unref);
		  }
		}
	      }

	      /* effect list children */
	      effect_child = child->children;

	      while(effect_child != NULL){
		if(effect_child->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(effect_child->name,
				 (xmlChar *) "ags-sf-control",
				 15)){
		    AgsLineMember *line_member;

		    GList *list_start, *list;
		    
		    xmlChar *specifier;

		    specifier = xmlGetProp(effect_child,
					   "specifier");

		    if(is_lv2_plugin &&
		       do_fixup_3_2_7){
		      gchar *tmp;

		      tmp = NULL;
      
		      if(!g_ascii_strncasecmp(specifier, "\"\"\"", 3)){
			tmp = g_strndup(specifier + 3,
					(gsize) (strlen(specifier) - 6));
		      }else if(!g_ascii_strncasecmp(specifier, "\"\"", 2)){
			tmp = g_strndup(specifier + 2,
					(gsize) (strlen(specifier) - 4));
		      }else if(!g_ascii_strncasecmp(specifier, "'''", 3)){
			tmp = g_strndup(specifier + 3,
					(gsize) (strlen(specifier) - 6));
		      }else if(!g_ascii_strncasecmp(specifier, "''", 2)){
			tmp = g_strndup(specifier + 2,
					(gsize) (strlen(specifier) - 4));
		      }else if(specifier[0] == '"'){
			tmp = g_strndup(specifier + 1,
					(gsize) (strlen(specifier) - 2));
		      }else if(specifier[0] == '\''){
			tmp = g_strndup(specifier + 1,
					(gsize) (strlen(specifier) - 2));
		      }

		      if(tmp != NULL){
			xmlFree(specifier);

			specifier = xmlStrdup(tmp);
			g_free(tmp);
		      }
		    }

		    list =
		      list_start = ags_effect_line_get_line_member(gobject);

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

		    if(specifier != NULL){
		      xmlFree(specifier);
		    }
		    
		    if(list_start != NULL){
		      g_list_free(list_start);
		    }
		  }
		}

		effect_child = effect_child->next;
	      }
	      
	      if(filename != NULL){
		xmlFree(filename);
	      }
	      
	      if(effect != NULL){
		xmlFree(effect);
	      }
	    }
	  }
	  
	  effect_list_child = effect_list_child->next;
	}

	g_list_free_full(mapped_filename,
			 g_free);
	g_list_free_full(mapped_effect,
			 g_free);
      }
    }

    child = child->next;
  }

  /* launch AgsEffectLine */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "node", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start",
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
ags_simple_file_read_oscillator_list(AgsSimpleFile *simple_file, xmlNode *node, GList **oscillator)
{
  AgsOscillator *current;
  
  xmlNode *child;

  GList *list;
  
  guint i;

  child = node->children;
  list = NULL;

  i = 0;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-oscillator",
		     14)){
	current = NULL;

	if(*oscillator != NULL){
	  GList *iter;

	  iter = g_list_nth(*oscillator,
			    i);

	  if(iter != NULL){
	    current = iter->data;
	  }
	}
	
	ags_simple_file_read_oscillator(simple_file, child, &current);
	list = g_list_prepend(list, current);

	i++;
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *oscillator = list;
}

void
ags_simple_file_read_oscillator(AgsSimpleFile *simple_file, xmlNode *node, AgsOscillator **oscillator)
{
  AgsOscillator *gobject;
  
  xmlChar *str;
  
  gdouble val;
  guint nth;
  
  if(*oscillator != NULL){
    gobject = AGS_OSCILLATOR(oscillator[0]);
  }else{
    gobject = ags_oscillator_new();

    *oscillator = gobject;
  }

  str = xmlGetProp(node,
		   "wave");

  if(str != NULL){      
    nth = g_ascii_strtoull(str,
			   NULL,
			   10);
    gtk_combo_box_set_active(gobject->wave,
			     nth);

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "attack");

  if(str != NULL){
    val = g_ascii_strtod(str,
			 NULL);
    gtk_spin_button_set_value(gobject->attack,
			      val);

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "frequency");

  if(str != NULL){
    val = g_ascii_strtod(str,
			 NULL);
    gtk_spin_button_set_value(gobject->frequency,
			      val);

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "length");

  if(str != NULL){
    val = g_ascii_strtod(str,
			 NULL);
    gtk_spin_button_set_value(gobject->frame_count,
			      val);

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "phase");

  if(str != NULL){
    val = g_ascii_strtod(str,
			 NULL);
    gtk_spin_button_set_value(gobject->phase,
			      val);

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "volume");

  if(str != NULL){
    val = g_ascii_strtod(str,
			 NULL);
    gtk_spin_button_set_value(gobject->volume,
			      val);

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "sync");

  if(str != NULL &&
     !xmlStrncmp(str,
		 "true",
		 5)){
    gtk_check_button_set_active((GtkCheckButton *) gobject->do_sync,
				TRUE);

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "sync-point");

  if(str != NULL){
    gchar *endptr;

    gdouble current;
    guint i;

    endptr = str;
    
    for(i = 0; endptr[0] != '\0' && i < 2 * gobject->sync_point_count; i++){
      current = g_strtod(endptr,
			 &endptr);

      gtk_spin_button_set_value(gobject->sync_point[i],
				current);

      if(endptr[0] != '\0'){
	endptr++;
      }
    }

    xmlFree(str);
  }
}

void
ags_simple_file_read_fm_oscillator_list(AgsSimpleFile *simple_file, xmlNode *node, GList **fm_oscillator)
{
  AgsFMOscillator *current;
  
  xmlNode *child;

  GList *list;
  
  guint i;

  child = node->children;
  list = NULL;

  i = 0;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-fm-oscillator",
		     14)){
	current = NULL;

	if(*fm_oscillator != NULL){
	  GList *iter;

	  iter = g_list_nth(*fm_oscillator,
			    i);

	  if(iter != NULL){
	    current = iter->data;
	  }
	}
	
	ags_simple_file_read_fm_oscillator(simple_file, child, &current);
	list = g_list_prepend(list, current);

	i++;
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *fm_oscillator = list;
}

void
ags_simple_file_read_fm_oscillator(AgsSimpleFile *simple_file, xmlNode *node, AgsFMOscillator **fm_oscillator)
{
  AgsFMOscillator *gobject;
  
  xmlChar *str;
  
  gdouble val;
  guint nth;
  
  if(*fm_oscillator != NULL){
    gobject = AGS_FM_OSCILLATOR(fm_oscillator[0]);
  }else{
    gobject = ags_fm_oscillator_new();

    *fm_oscillator = gobject;
  }

  str = xmlGetProp(node,
		   "wave");

  if(str != NULL){      
    nth = g_ascii_strtoull(str,
			   NULL,
			   10);
    gtk_combo_box_set_active(gobject->wave,
			     nth);

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "attack");

  if(str != NULL){
    val = g_ascii_strtod(str,
			 NULL);
    gtk_spin_button_set_value(gobject->attack,
			      val);

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "frequency");

  if(str != NULL){
    val = g_ascii_strtod(str,
			 NULL);
    gtk_spin_button_set_value(gobject->frequency,
			      val);

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "length");

  if(str != NULL){
    val = g_ascii_strtod(str,
			 NULL);
    gtk_spin_button_set_value(gobject->frame_count,
			      val);

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "phase");

  if(str != NULL){
    val = g_ascii_strtod(str,
			 NULL);
    gtk_spin_button_set_value(gobject->phase,
			      val);

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "volume");

  if(str != NULL){
    val = g_ascii_strtod(str,
			 NULL);
    gtk_spin_button_set_value(gobject->volume,
			      val);

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "sync");

  if(str != NULL &&
     !xmlStrncmp(str,
		 "true",
		 5)){
    gtk_check_button_set_active((GtkCheckButton *) gobject->do_sync,
				TRUE);

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "sync-point");

  if(str != NULL){
    gchar *endptr;

    gdouble current;
    guint i;

    endptr = str;
    
    for(i = 0; endptr[0] != '\0' && i < 2 * gobject->sync_point_count; i++){
      current = g_strtod(endptr,
			 &endptr);

      gtk_spin_button_set_value(gobject->sync_point[i],
				current);

      if(endptr[0] != '\0'){
	endptr++;
      }
    }

    xmlFree(str);
  }

  /* FM LFO */
  str = xmlGetProp(node,
		   "fm-lfo-wave");

  if(str != NULL){      
    nth = g_ascii_strtoull(str,
			   NULL,
			   10);
    gtk_combo_box_set_active(gobject->fm_lfo_wave,
			     nth);

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "fm-lfo-frequency");

  if(str != NULL){
    val = g_ascii_strtod(str,
			 NULL);
    gtk_spin_button_set_value(gobject->fm_lfo_frequency,
			      val);

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "fm-lfo-depth");

  if(str != NULL){
    val = g_ascii_strtod(str,
			 NULL);
    gtk_spin_button_set_value(gobject->fm_lfo_depth,
			      val);

    xmlFree(str);
  }

  str = xmlGetProp(node,
		   "fm-tuning");

  if(str != NULL){
    val = g_ascii_strtod(str,
			 NULL);
    gtk_spin_button_set_value(gobject->fm_tuning,
			      val);

    xmlFree(str);
  }
}

void
ags_simple_file_read_composite_editor(AgsSimpleFile *simple_file, xmlNode *node, AgsCompositeEditor **composite_editor)
{
  AgsCompositeEditor *gobject;
  
  AgsFileLaunch *file_launch;

  if(*composite_editor != NULL){
    gobject = *composite_editor;
  }else{
    return;
  }

  /* launch AgsLine */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "file", simple_file,
					       "node", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start",
		   G_CALLBACK(ags_simple_file_read_composite_editor_launch), gobject);
  ags_simple_file_add_launch(simple_file,
			     (GObject *) file_launch);
}

void
ags_simple_file_read_composite_editor_launch(AgsFileLaunch *file_launch,
					     AgsCompositeEditor *composite_editor)
{
  xmlNode *child;
  
  xmlChar *str;

  str = xmlGetProp(file_launch->node,
		   "zoom");

  if(str != NULL){
    GtkTreeModel *model;
    GtkTreeIter iter;

    gchar *value;

    model = gtk_combo_box_get_model(GTK_COMBO_BOX(composite_editor->toolbar->zoom));
    
    if(gtk_tree_model_get_iter_first(model, &iter)){
      do{
	gtk_tree_model_get(model, &iter,
			   0, &value,
			   -1);

	if(!g_strcmp0(str,
		      value)){
	  gtk_combo_box_set_active_iter((GtkComboBox *) composite_editor->toolbar->zoom,
					&iter);
	  break;
	}
      }while(gtk_tree_model_iter_next(model,
				      &iter));
    }

    xmlFree(str);
  }

  /* children */
  child = file_launch->node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-property-list",
		     21)){
	GList *start_list, *list;
	GList *start_property, *property;

	start_property = NULL;
	ags_simple_file_read_property_list((AgsSimpleFile *) file_launch->file,
					   child,
					   &start_property);

	property = start_property;

	while(property != NULL){
  	  if(!g_strcmp0(((GParameter *) property->data)->name,
			"machine")){
	    AgsWindow *window;
	    AgsMachine *current_machine;

	    GList *file_id_ref;
	    GList *start_machine, *machine;
	    GList *start_list, *list;

	    gint position;
	    gboolean success;
	    
	    str = g_value_get_string(&(((GParameter *) property->data)->value));

	    if(str != NULL){
	      file_id_ref = ags_simple_file_find_id_ref_by_xpath((AgsSimpleFile *) file_launch->file,
								 str);

	      if(file_id_ref != NULL &&
		 file_id_ref->data != NULL &&
		 AGS_FILE_ID_REF(file_id_ref->data)->ref != NULL){
		window = gtk_widget_get_ancestor(composite_editor,
						 AGS_TYPE_WINDOW);

		current_machine = AGS_FILE_ID_REF(file_id_ref->data)->ref;

		machine = 
		  start_machine = ags_window_get_machine(window);

		start_list = ags_machine_selector_get_machine_radio_button(window->composite_editor->machine_selector);

		position = 0;
		
		while(machine != NULL){
		  if(machine->data == current_machine){
		    break;
		  }
    
		  list = start_list;

		  success = FALSE;

		  while(!success && list != NULL){
		    if(AGS_MACHINE_RADIO_BUTTON(list->data)->machine == current_machine){
		      break;
		    }
      
		    if(AGS_MACHINE_RADIO_BUTTON(list->data)->machine == machine->data){
		      success = TRUE;
		    } 
      
		    list = list->next;
		  }
    
		  if(success){
		    position++;
		  }
    
		  machine = machine->next;
		}

		ags_machine_selector_insert_index(window->composite_editor->machine_selector,
						  position,
						  current_machine);
		
		g_list_free(start_list);
	      }
	    }
	  }else{
	    g_object_set_property((GObject *) composite_editor,
				  ((GParameter *) property->data)->name,
				  &(((GParameter *) property->data)->value));
	  }
	  
	  property = property->next;
	}

	g_list_free_full(start_property,
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
		     (xmlChar *) "ags-sf-notation",
		     11)){
	current = NULL;

	if(*notation != NULL){
	  GList *iter;

	  iter = g_list_nth(*notation,
			    i);

	  if(iter != NULL){
	    current = iter->data;
	  }
	}
	
	ags_simple_file_read_notation(simple_file, child, &current);
	list = ags_notation_add(list,
				current);
	
	i++;
      }
    }

    child = child->next;
  }

  *notation = list;
}

void
ags_simple_file_read_notation_fixup_1_0_to_1_2(AgsSimpleFile *simple_file, xmlNode *node, GList **notation)
{
  AgsMachine *machine;
    
  AgsNotation *gobject;
  AgsNote *note;

  AgsTimestamp *timestamp;
  AgsFileIdRef *file_id_ref;

  xmlNode *child;

  GList *notation_list;
    
  xmlChar *str;
  
  guint audio_channel;
  gboolean found_timestamp;
    
  file_id_ref = (AgsFileIdRef *) ags_simple_file_find_id_ref_by_node(simple_file,
								     node->parent->parent);
  machine = file_id_ref->ref;
    
  audio_channel = 0;
  str = xmlGetProp(node,
		   "channel");

  if(str != NULL){
    audio_channel = g_ascii_strtoull(str,
				     NULL,
				     10);

    xmlFree(str);
  }
    
  /* children */    
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  timestamp->timer.ags_offset.offset = 0;
    
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-sf-note",
		     12)){
	note = ags_note_new();

	/* position and offset */
	str = xmlGetProp(child,
			 "x0");

	if(str != NULL){
	  note->x[0] = g_ascii_strtoull(str,
					NULL,
					10);

	  xmlFree(str);
	}
	
	str = xmlGetProp(child,
			 "x1");

	if(str != NULL){
	  note->x[1] = g_ascii_strtoull(str,
					NULL,
					10);

	  xmlFree(str);
	}

	str = xmlGetProp(child,
			 "y");

	if(str != NULL){
	  note->y = g_ascii_strtoull(str,
				     NULL,
				     10);

	  xmlFree(str);
	}

	/* envelope */
	str = xmlGetProp(child,
			 "envelope");

	if(str != NULL){
	  if(!g_ascii_strncasecmp(str,
				  "true",
				  5)){
	    note->flags |= AGS_NOTE_ENVELOPE;
	  }

	  xmlFree(str);
	}

	str = xmlGetProp(child,
			 "attack");

	if(str != NULL){
	  sscanf(str, "%lf %lf", &(note->attack.real), &(note->attack.imag));

	  xmlFree(str);
	}

	str = xmlGetProp(child,
			 "decay");

	if(str != NULL){
	  sscanf(str, "%lf %lf", &(note->decay.real), &(note->decay.imag));

	  xmlFree(str);
	}

	str = xmlGetProp(child,
			 "sustain");

	if(str != NULL){
	  sscanf(str, "%lf %lf", &(note->sustain.real), &(note->sustain.imag));

	  xmlFree(str);
	}

	str = xmlGetProp(child,
			 "release");

	if(str != NULL){
	  sscanf(str, "%lf %lf", &(note->release.real), &(note->release.imag));

	  xmlFree(str);
	}

	str = xmlGetProp(child,
			 "ratio");

	if(str != NULL){
	  sscanf(str, "%lf %lf", &(note->ratio.real), &(note->ratio.imag));

	  xmlFree(str);
	}

	timestamp->timer.ags_offset.offset = AGS_NOTATION_DEFAULT_OFFSET * floor(note->x[0] / AGS_NOTATION_DEFAULT_OFFSET);

	notation_list = ags_notation_find_near_timestamp(notation[0], audio_channel,
							 timestamp);
	  
	if(notation_list != NULL){
	  gobject = notation_list->data;
	}else{
	  gobject = g_object_new(AGS_TYPE_NOTATION,
				 "audio", machine->audio,
				 "audio-channel", audio_channel,
				 NULL);

	  gobject->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;
	    
	  notation[0] = ags_notation_add(notation[0],
					 gobject);
	}
	  
	/* add */
	ags_notation_add_note(gobject,
			      note,
			      FALSE);
      }
    }

    child = child->next;
  }

  g_object_unref(timestamp);
}

void
ags_simple_file_read_notation_list_fixup_1_0_to_1_2(AgsSimpleFile *simple_file, xmlNode *node, GList **notation)
{
  AgsNotation *current;

  xmlNode *child;

  GList *list;

  guint i;

  if(notation == NULL){
    return;
  }
  
  child = node->children;

  i = 0;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-notation",
		     11)){
	ags_simple_file_read_notation_fixup_1_0_to_1_2(simple_file, child, notation);

	i++;
      }
    }

    child = child->next;
  }
}

void
ags_simple_file_read_notation(AgsSimpleFile *simple_file, xmlNode *node, AgsNotation **notation)
{
  AgsNotation *gobject;
  AgsNote *note;

  xmlNode *child;
  
  xmlChar *str;
  
  guint audio_channel;
  gboolean found_timestamp;
  
  if(*notation != NULL){
    gobject = *notation;

    audio_channel = gobject->audio_channel;
  }else{
    AgsMachine *machine;
    
    AgsFileIdRef *file_id_ref;
    
    file_id_ref = (AgsFileIdRef *) ags_simple_file_find_id_ref_by_node(simple_file,
								       node->parent->parent);
    machine = file_id_ref->ref;

    if(!AGS_IS_MACHINE(machine)){
      return;
    }
    
    audio_channel = 0;
    str = xmlGetProp(node,
		     "channel");

    if(str != NULL){
      audio_channel = g_ascii_strtoull(str,
				       NULL,
				       10);

      xmlFree(str);
    }
    
    gobject = g_object_new(AGS_TYPE_NOTATION,
			   "audio", machine->audio,
			   "audio-channel", audio_channel,
			   NULL);

    *notation = gobject;
  }

  /* children */
  child = node->children;

  found_timestamp = FALSE;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-sf-timestamp",
		     17)){
	found_timestamp = TRUE;
	
	/* offset */
	str = xmlGetProp(child,
			 "offset");

	if(str != NULL){
	  gobject->timestamp->timer.ags_offset.offset = g_ascii_strtoull(str,
									 NULL,
									 10);

	  xmlFree(str);
	}	
      }
    }

    child = child->next;
  }
  
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-sf-note",
		     12)){
	note = ags_note_new();

	/* position and offset */
	str = xmlGetProp(child,
			 "x0");

	if(str != NULL){
	  note->x[0] = g_ascii_strtoull(str,
					NULL,
					10);

	  xmlFree(str);
	}
	
	str = xmlGetProp(child,
			 "x1");

	if(str != NULL){
	  note->x[1] = g_ascii_strtoull(str,
					NULL,
					10);

	  xmlFree(str);
	}

	str = xmlGetProp(child,
			 "y");

	if(str != NULL){
	  note->y = g_ascii_strtoull(str,
				     NULL,
				     10);

	  xmlFree(str);
	}

	/* envelope */
	str = xmlGetProp(child,
			 "envelope");

	if(str != NULL){
	  if(!g_ascii_strncasecmp(str,
				  "true",
				  5)){
	    note->flags |= AGS_NOTE_ENVELOPE;
	  }

	  xmlFree(str);
	}

	str = xmlGetProp(child,
			 "attack");

	if(str != NULL){
	  sscanf(str, "%lf %lf", &(note->attack.real), &(note->attack.imag));

	  xmlFree(str);
	}

	str = xmlGetProp(child,
			 "decay");

	if(str != NULL){
	  sscanf(str, "%lf %lf", &(note->decay.real), &(note->decay.imag));

	  xmlFree(str);
	}

	str = xmlGetProp(child,
			 "sustain");

	if(str != NULL){
	  sscanf(str, "%lf %lf", &(note->sustain.real), &(note->sustain.imag));

	  xmlFree(str);
	}

	str = xmlGetProp(child,
			 "release");

	if(str != NULL){
	  sscanf(str, "%lf %lf", &(note->release.real), &(note->release.imag));

	  xmlFree(str);
	}

	str = xmlGetProp(child,
			 "ratio");

	if(str != NULL){
	  sscanf(str, "%lf %lf", &(note->ratio.real), &(note->ratio.imag));

	  xmlFree(str);
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
		     (xmlChar *) "ags-sf-automation",
		     11)){
	current = NULL;

	if(*automation != NULL){
	  GList *iter;

	  iter = g_list_nth(*automation,
			    i);

	  if(iter != NULL){
	    current = iter->data;
	  }
	}
	
	ags_simple_file_read_automation(simple_file, child, &current);
	list = ags_automation_add(list, current);

	i++;
      }
    }

    child = child->next;
  }

  *automation = list;
}

void
ags_simple_file_read_automation_fixup_1_0_to_1_3(AgsSimpleFile *simple_file, xmlNode *node, GList **automation)
{
  AgsMachine *machine;
    
  AgsAutomation *gobject;
  AgsAcceleration *acceleration;

  AgsTimestamp *timestamp;
  AgsFileIdRef *file_id_ref;

  xmlNode *child;

  GList *automation_list;
    
  xmlChar *str;
  gchar *control_name;
  
  GType channel_type;
  
  guint line;
  gboolean found_timestamp;

  file_id_ref = (AgsFileIdRef *) ags_simple_file_find_id_ref_by_node(simple_file,
								     node->parent->parent);
  machine = file_id_ref->ref;
    
  line = 0;
  str = xmlGetProp(node,
		   "line");

  if(str != NULL){
    line = g_ascii_strtoull(str,
			    NULL,
			    10);

    xmlFree(str);
  }

  channel_type = G_TYPE_NONE;
    
  str = xmlGetProp(node,
		   "channel-type");
    
  if(str != NULL){
    channel_type = g_type_from_name(str);

    xmlFree(str);
  }
    
  control_name = xmlGetProp(node,
			    "control-name");    

  /* children */    
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  timestamp->timer.ags_offset.offset = 0;
    
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-sf-acceleration",
		     12)){
	acceleration = ags_acceleration_new();

	/* position and offset */
	str = xmlGetProp(child,
			 "x");

	if(str != NULL){
	  acceleration->x = g_ascii_strtoull(str,
					     NULL,
					     10);

	  xmlFree(str);
	}

	str = xmlGetProp(child,
			 "y");

	if(str != NULL){
	  acceleration->y = g_ascii_strtoull(str,
					     NULL,
					     10);

	  xmlFree(str);
	}

	timestamp->timer.ags_offset.offset = AGS_AUTOMATION_DEFAULT_OFFSET * floor(acceleration->x / AGS_AUTOMATION_DEFAULT_OFFSET);

	automation_list = ags_automation_find_near_timestamp_extended(automation[0], line,
								      channel_type, control_name,
								      timestamp);
	  
	if(automation_list != NULL){
	  gobject = automation_list->data;

	  channel_type = gobject->channel_type;
	  control_name = gobject->control_name;
	}else{	    
	  gobject = g_object_new(AGS_TYPE_AUTOMATION,
				 "audio", machine->audio,
				 "line", line,
				 "channel-type", channel_type,
				 "control-name", control_name,
				 NULL);

	  gobject->timestamp->timer.ags_offset.offset = timestamp->timer.ags_offset.offset;
	    
	  automation[0] = ags_automation_add(automation[0],
					     gobject);
	}
	  
	/* add */
	ags_automation_add_acceleration(gobject,
					acceleration,
					FALSE);
      }
    }

    child = child->next;
  }

  g_object_unref(timestamp);
}

void
ags_simple_file_read_automation_list_fixup_1_0_to_1_3(AgsSimpleFile *simple_file, xmlNode *node, GList **automation)
{
  AgsAutomation *current;

  xmlNode *child;

  GList *list;

  guint i;

  if(automation == NULL){
    return;
  }
  
  child = node->children;

  i = 0;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-automation",
		     11)){
	ags_simple_file_read_automation_fixup_1_0_to_1_3(simple_file, child, automation);

	i++;
      }
    }

    child = child->next;
  }
}

void
ags_simple_file_read_automation(AgsSimpleFile *simple_file, xmlNode *node, AgsAutomation **automation)
{
  AgsAutomation *gobject;
  AgsAcceleration *acceleration;

  xmlNode *child;
  
  xmlChar *str;
  gchar *control_name;
  
  GType channel_type;

  guint line;
  gboolean found_timestamp;
  
  if(*automation != NULL){
    gobject = *automation;

    line = gobject->line;

    channel_type = gobject->channel_type;
    control_name = gobject->control_name;
  }else{
    AgsMachine *machine;
    
    AgsFileIdRef *file_id_ref;
    
    file_id_ref = (AgsFileIdRef *) ags_simple_file_find_id_ref_by_node(simple_file,
								       node->parent->parent);
    machine = file_id_ref->ref;

    if(!AGS_IS_MACHINE(machine)){
      return;
    }
    
    line = 0;
    str = xmlGetProp(node,
		     "line");

    if(str != NULL){
      line = g_ascii_strtoull(str,
			      NULL,
			      10);

      xmlFree(str);
    }

    channel_type = G_TYPE_NONE;
    
    str = xmlGetProp(node,
		     "channel-type");

    if(str != NULL){
      channel_type = g_type_from_name(str);

      xmlFree(str);
    }
    
    control_name = xmlGetProp(node,
			      "control-name");
    
    gobject = g_object_new(AGS_TYPE_AUTOMATION,
			   "audio", machine->audio,
			   "line", line,
			   "channel-type", channel_type,
			   "control-name", control_name,
			   NULL);

    *automation = gobject;
  }

  /* children */
  child = node->children;

  found_timestamp = FALSE;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-sf-timestamp",
		     17)){
	found_timestamp = TRUE;
	
	/* offset */
	str = xmlGetProp(child,
			 "offset");

	if(str != NULL){
	  gobject->timestamp->timer.ags_offset.offset = g_ascii_strtoull(str,
									 NULL,
									 10);
	  
	  xmlFree(str);
	}
      }
    }

    child = child->next;
  }
  
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-sf-acceleration",
		     12)){
	acceleration = ags_acceleration_new();

	/* position and offset */
	str = xmlGetProp(child,
			 "x");

	if(str != NULL){
	  acceleration->x = g_ascii_strtoull(str,
					     NULL,
					     10);

	  xmlFree(str);
	}	

	str = xmlGetProp(child,
			 "y");

	if(str != NULL){
	  acceleration->y = g_strtod(str,
				     NULL);

	  xmlFree(str);
	}

	/* add */
	ags_automation_add_acceleration(gobject,
					acceleration,
					FALSE);
      }
    }

    child = child->next;
  }
}

void
ags_simple_file_read_preset_list(AgsSimpleFile *simple_file, xmlNode *node, GList **preset)
{
  AgsPreset *current;
  
  xmlNode *child;

  GList *list;
  
  guint i;
  
  child = node->children;
  list = NULL;

  i = 0;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-preset",
		     11)){
	current = NULL;

	if(*preset != NULL){
	  GList *iter;

	  iter = g_list_nth(*preset,
			    i);

	  if(iter != NULL){
	    current = iter->data;
	  }
	}

	ags_simple_file_read_preset(simple_file, child, &current);
	list = g_list_prepend(list, current);
	
	i++;
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *preset = list;
}

void
ags_simple_file_read_preset(AgsSimpleFile *simple_file, xmlNode *node, AgsPreset **preset)
{
  AgsMachine *machine;

  AgsPreset *gobject;

  AgsFileIdRef *file_id_ref;
  AgsFileLaunch *file_launch;

  xmlNode *child;
  
  xmlChar *str;

  guint i;	
  
  file_id_ref = (AgsFileIdRef *) ags_simple_file_find_id_ref_by_node(simple_file,
								     node->parent->parent);
  machine = file_id_ref->ref;
  
  if(*preset != NULL){
    gobject = *preset;
  }else{
    gobject = ags_preset_new();
    
    *preset = gobject;
  }

  /* scope */
  str = xmlGetProp(node,
		   "scope");

  if(str != NULL){
    g_object_set(gobject,
		 "scope", str,
		 NULL);

    xmlFree(str);
  }

  /* preset name */
  str = xmlGetProp(node,
		   "preset-name");

  if(str != NULL){
    g_object_set(gobject,
		 "preset-name", str,
		 NULL);

    xmlFree(str);
  }

  /* audio channel start */
  str = xmlGetProp(node,
		   "audio-channel-start");

  if(str != NULL){
    g_object_set(gobject,
		 "audio-channel-start", g_ascii_strtoull(str,
							 NULL,
							 10),
		 NULL);

    xmlFree(str);
  }

  /* audio channel end */
  str = xmlGetProp(node,
		   "audio-channel-end");

  if(str != NULL){
    g_object_set(gobject,
		 "audio-channel-end", g_ascii_strtoull(str,
						       NULL,
						       10),
		 NULL);

    xmlFree(str);
  }

  /* pad start */
  str = xmlGetProp(node,
		   "pad-start");

  if(str != NULL){
    g_object_set(gobject,
		 "pad-start", g_ascii_strtoull(str,
					       NULL,
					       10),
		 NULL);

    xmlFree(str);
  }

  /* pad end */
  str = xmlGetProp(node,
		   "pad-end");

  if(str != NULL){
    g_object_set(gobject,
		 "pad-end", g_ascii_strtoull(str,
					     NULL,
					     10),
		 NULL);

    xmlFree(str);
  }

  /* x start */
  str = xmlGetProp(node,
		   "x-start");

  if(str != NULL){
    g_object_set(gobject,
		 "x-start", g_ascii_strtoull(str,
					     NULL,
					     10),
		 NULL);

    xmlFree(str);
  }

  /* x end */
  str = xmlGetProp(node,
		   "x-end");

  if(str != NULL){
    g_object_set(gobject,
		 "x-end", g_ascii_strtoull(str,
					   NULL,
					   10),
		 NULL);

    xmlFree(str);
  }

  /* children */
  child = node->children;

  i = 0;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-sf-strv",
		     12)){
	ags_simple_file_read_strv(simple_file, child, &(gobject->parameter_name));
      }else if(!xmlStrncmp(child->name,
			   "ags-sf-value",
			   13)){
	GValue *value;
	
	if(gobject->value == NULL){
	  gobject->value = g_new0(GValue,
				  1);
	}else{
	  gobject->value = g_renew(GValue,
				   gobject->value,
				   i + 1);
	  memset(&(gobject->value[i]), 0, sizeof(GValue));
	}

	//NOTE:JK: pay attention
	value = &(gobject->value[i]);
	ags_simple_file_read_value(simple_file, child, &value);

	i++;
      }
    }

    child = child->next;
  }
}

void
ags_simple_file_read_program_list(AgsSimpleFile *simple_file, xmlNode *node, GList **program)
{
  AgsProgram *current;
  
  xmlNode *child;

  GList *list;
  
  guint i;
  
  child = node->children;
  list = NULL;

  i = 0;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-sf-program",
		     11)){
	current = NULL;

	if(*program != NULL){
	  GList *iter;

	  iter = g_list_nth(*program,
			    i);

	  if(iter != NULL){
	    current = iter->data;
	  }
	}

	ags_simple_file_read_program(simple_file, child, &current);
	list = g_list_prepend(list, current);
	
	i++;
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *program = list;
}

void
ags_simple_file_read_program(AgsSimpleFile *simple_file, xmlNode *node, AgsProgram **program)
{
  AgsProgram *gobject;
  AgsMarker *marker;

  xmlNode *child;
  
  xmlChar *str;
  gchar *control_name;
  
  GType channel_type;

  guint line;
  gboolean found_timestamp;
  
  if(*program != NULL){
    gobject = *program;

    control_name = gobject->control_name;
  }else{    
    control_name = xmlGetProp(node,
			      "control-name");
    
    gobject = g_object_new(AGS_TYPE_PROGRAM,
			   "control-name", control_name,
			   NULL);

    *program = gobject;
  }

  /* children */
  child = node->children;

  found_timestamp = FALSE;
  
  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-sf-timestamp",
		     17)){
	found_timestamp = TRUE;
	
	/* offset */
	str = xmlGetProp(child,
			 "offset");

	if(str != NULL){
	  gobject->timestamp->timer.ags_offset.offset = g_ascii_strtoull(str,
									 NULL,
									 10);
	  
	  xmlFree(str);
	}
      }
    }

    child = child->next;
  }
  
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-sf-marker",
		     14)){
	marker = ags_marker_new();

	/* position and offset */
	str = xmlGetProp(child,
			 "x");

	if(str != NULL){
	  marker->x = g_ascii_strtoull(str,
				       NULL,
				       10);

	  xmlFree(str);
	}	

	str = xmlGetProp(child,
			 "y");

	if(str != NULL){
	  marker->y = g_strtod(str,
			       NULL);

	  xmlFree(str);
	}

	/* add */
	ags_program_add_marker(gobject,
			       marker,
			       FALSE);
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

  gchar *str;
  gchar *id;
  char *buffer;
  gsize buffer_length;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-sf-config");

  /* ID reference */
  str = g_strdup_printf("xpath=//*[@id='%s']", id);
  
  ags_simple_file_add_id_ref(simple_file,
			     g_object_new(AGS_TYPE_FILE_ID_REF,
					  "file", simple_file,
					  "node", node,
					  "xpath", str,
					  "reference", ags_config,
					  NULL));
  
  g_free(str);

  /* properties */
  xmlNewProp(node,
	     AGS_SIMPLE_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_SIMPLE_FILE_VERSION_PROP,
	     AGS_CONFIG(ags_config)->version);

  xmlNewProp(node,
	     AGS_SIMPLE_FILE_BUILD_ID_PROP,
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

  g_free(buffer);
  
  /* add to parent */
  xmlAddChild(node,
	      cdata);

  return(node);
}

xmlNode*
ags_simple_file_write_meta_data_window(AgsSimpleFile *simple_file, xmlNode *parent, AgsMetaDataWindow *meta_data_window)
{
  GtkTextBuffer *text_buffer;
  
  GtkTextIter start;
  GtkTextIter end;

  xmlNode *node;
  xmlNode *child;
  xmlNode *cdata;

  gchar *str;

  node = xmlNewNode(NULL,
		    "ags-sf-meta-list");

  /* author */
  child = xmlNewNode(NULL,
		     "ags-sf-meta");

  xmlNewProp(child,
	     "name",
	     "author");

  str = gtk_editable_get_text(GTK_EDITABLE(meta_data_window->author));

  cdata = xmlNewCDataBlock(simple_file->doc,
			   str,
			   strlen(str));
  
  xmlAddChild(child,
	      cdata);

  xmlAddChild(node,
	      child);

  /* title */
  child = xmlNewNode(NULL,
		     "ags-sf-meta");

  xmlNewProp(child,
	     "name",
	     "title");

  str = gtk_editable_get_text(GTK_EDITABLE(meta_data_window->title));

  cdata = xmlNewCDataBlock(simple_file->doc,
			   str,
			   strlen(str));
  
  xmlAddChild(child,
	      cdata);

  xmlAddChild(node,
	      child);

  /* album */
  child = xmlNewNode(NULL,
		     "ags-sf-meta");

  xmlNewProp(child,
	     "name",
	     "album");

  str = gtk_editable_get_text(GTK_EDITABLE(meta_data_window->album));

  cdata = xmlNewCDataBlock(simple_file->doc,
			   str,
			   strlen(str));
  
  xmlAddChild(child,
	      cdata);

  xmlAddChild(node,
	      child);

  /* release date */
  child = xmlNewNode(NULL,
		     "ags-sf-meta");

  xmlNewProp(child,
	     "name",
	     "release-date");

  str = gtk_editable_get_text(GTK_EDITABLE(meta_data_window->release_date));

  cdata = xmlNewCDataBlock(simple_file->doc,
			   str,
			   strlen(str));
  
  xmlAddChild(child,
	      cdata);

  xmlAddChild(node,
	      child);

  /* copyright */
  child = xmlNewNode(NULL,
		     "ags-sf-meta");

  xmlNewProp(child,
	     "name",
	     "copyright");

  str = gtk_editable_get_text(GTK_EDITABLE(meta_data_window->copyright));

  cdata = xmlNewCDataBlock(simple_file->doc,
			   str,
			   strlen(str));
  
  xmlAddChild(child,
	      cdata);

  xmlAddChild(node,
	      child);

  /* license */
  child = xmlNewNode(NULL,
		     "ags-sf-meta");

  xmlNewProp(child,
	     "name",
	     "license");

  str = gtk_editable_get_text(GTK_EDITABLE(meta_data_window->license));

  cdata = xmlNewCDataBlock(simple_file->doc,
			   str,
			   strlen(str));
  
  xmlAddChild(child,
	      cdata);

  xmlAddChild(node,
	      child);


  /* comment */
  child = xmlNewNode(NULL,
		     "ags-sf-meta");

  xmlNewProp(child,
	     "name",
	     "comment");

  text_buffer = gtk_text_view_get_buffer(meta_data_window->comment);

  gtk_text_buffer_get_bounds(text_buffer,
			     &start,
			     &end);
  
  str = gtk_text_buffer_get_text(text_buffer,
				 &start,
				 &end,
				 TRUE);
  
  cdata = xmlNewCDataBlock(simple_file->doc,
			   str,
			   strlen(str));
  
  xmlAddChild(child,
	      cdata);

  xmlAddChild(node,
	      child);

  /* add to parent */
  xmlAddChild(parent,
	      node);
  
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
      val = g_strdup("true");
    }else{
      val = g_strdup("false");
    }
  }else if(G_VALUE_HOLDS_UINT(&(property->value))){
    type_name = g_type_name(G_TYPE_UINT);

    val = g_strdup_printf("%u",
			  g_value_get_uint(&(property->value)));
  }else if(G_VALUE_HOLDS_INT(&(property->value))){
    type_name = g_type_name(G_TYPE_INT);

    val = g_strdup_printf("%d",
			  g_value_get_int(&(property->value)));
  }else if(G_VALUE_HOLDS_DOUBLE(&(property->value))){
    type_name = g_type_name(G_TYPE_DOUBLE);

    val = g_strdup_printf("%lf",
			  g_value_get_double(&(property->value)));
  }else if(G_VALUE_HOLDS(&(property->value),
			 AGS_TYPE_COMPLEX)){
    AgsComplex *z;
    
    type_name = g_type_name(AGS_TYPE_COMPLEX);

    z = g_value_get_boxed(&(property->value));
    val = g_strdup_printf("%lf %lf",
			  z[0].real, z[0].imag);
  }else{
    g_warning("ags_simple_file_write_property() - unsupported type");
    
    return(NULL);
  }

  node = xmlNewNode(NULL,
		    "ags-sf-property");
  
  xmlNewProp(node,
	     "type",
	     type_name);

  xmlNewProp(node,
	     "name",
	     property->name);

  xmlNewProp(node,
	     "value",
	     val);

  g_free(val);
  
  /* add to parent */
  xmlAddChild(parent,
	      node);

  return(node);
}

xmlNode*
ags_simple_file_write_strv(AgsSimpleFile *simple_file, xmlNode *parent, gchar **strv)
{
  xmlNode *node;
  xmlNode *child;
  
  gchar **current;

  if(strv == NULL){
    return(NULL);
  }
  
  node = xmlNewNode(NULL,
		    "ags-sf-strv");
  
  /* add to parent */
  xmlAddChild(parent,
	      node);

  current = strv;

  while(*current != NULL){
    child = xmlNewNode(NULL,
		       "ags-sf-str");
    
    xmlNodeAddContent(child,
		      *current);
    
    /* add to parent */
    xmlAddChild(node,
		child);

    current++;
  }

  return(node);
}

xmlNode*
ags_simple_file_write_value(AgsSimpleFile *simple_file, xmlNode *parent, GValue *value)
{
  xmlNode *node;
  
  xmlChar *type_name;
  xmlChar *val;
  
  if(G_VALUE_HOLDS_BOOLEAN(value)){
    type_name = g_type_name(G_TYPE_BOOLEAN);

    if(g_value_get_boolean(value)){
      val = g_strdup("true");
    }else{
      val = g_strdup("false");
    }
  }else if(G_VALUE_HOLDS_UINT(value)){
    type_name = g_type_name(G_TYPE_UINT);

    val = g_strdup_printf("%u",
			  g_value_get_uint(value));
  }else if(G_VALUE_HOLDS_INT(value)){
    type_name = g_type_name(G_TYPE_INT);

    val = g_strdup_printf("%d",
			  g_value_get_int(value));
  }else if(G_VALUE_HOLDS_DOUBLE(value)){
    type_name = g_type_name(G_TYPE_DOUBLE);

    val = g_strdup_printf("%lf",
			  g_value_get_double(value));
  }else if(G_VALUE_HOLDS(value,
			 AGS_TYPE_COMPLEX)){
    AgsComplex *z;
    
    type_name = g_type_name(AGS_TYPE_COMPLEX);

    z = g_value_get_boxed(value);
    val = g_strdup_printf("%lf %lf",
			  z[0].real, z[0].imag);
  }else{
    g_warning("ags_simple_file_write_property() - unsupported type");
    
    return(NULL);
  }

  node = xmlNewNode(NULL,
		    "ags-sf-value");
  
  xmlNewProp(node,
	     "type",
	     type_name);

  xmlNewProp(node,
	     "value",
	     val);

  g_free(val);
  
  /* add to parent */
  xmlAddChild(parent,
	      node);

  return(node);
}

xmlNode*
ags_simple_file_write_window(AgsSimpleFile *simple_file, xmlNode *parent, AgsWindow *window)
{
  AgsApplicationContext *application_context;
  
  xmlNode *node;

  GList *list;

  gchar *str;

  application_context = ags_application_context_get_instance();
  
  node = xmlNewNode(NULL,
		    "ags-sf-window");

  /* properties */
  xmlNewProp(node,
	     "filename",
	     simple_file->filename);

  str = g_strdup_printf("%lf",
			gtk_spin_button_get_value(window->navigation->bpm));

  xmlNewProp(node,
	     "bpm",
	     str);

  g_free(str);

  xmlNewProp(node,
	     "loop",
	     ((gtk_check_button_get_active((GtkCheckButton *) window->navigation->loop)) ? AGS_SIMPLE_FILE_TRUE: AGS_SIMPLE_FILE_FALSE));

  str = g_strdup_printf("%lf",
			gtk_spin_button_get_value(window->navigation->loop_left_tact));
  
  xmlNewProp(node,
	     "loop-start",
	     str);

  g_free(str);

  str = g_strdup_printf("%lf",
			gtk_spin_button_get_value(window->navigation->loop_right_tact));
  
  xmlNewProp(node,
	     "loop-end",
	     str);

  g_free(str);

  /* children */
  list = ags_window_get_machine(window);
  ags_simple_file_write_machine_list(simple_file,
				     node,
				     list);
  g_list_free(list);

  ags_simple_file_write_composite_editor(simple_file,
					 node,
					 window->composite_editor);
  
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
		    "ags-sf-machine-list");

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

gboolean
ags_simple_file_write_machine_inline_pad(AgsSimpleFile *simple_file, xmlNode *parent, AgsChannel *channel)
{
  AgsChannel *next_pad;

  xmlNode *pad;
  xmlNode *line_list;
  xmlNode *line;

  gchar *str;
    
  gboolean found_pad_content, found_line_content;
    
  if(channel == NULL){
    return(FALSE);
  }

  found_pad_content = FALSE;
    
  while(channel != NULL){
      
    next_pad = channel->next_pad;

    pad = xmlNewNode(NULL,
		     "ags-sf-pad");

    /* properties */
    str = g_strdup_printf("%d",
			  channel->pad);
      
    xmlNewProp(pad,
	       "nth-pad",
	       str);

    g_free(str);
      
    line_list = xmlNewNode(NULL,
			   "ags-sf-line-list");
    found_line_content = FALSE;

    while(channel != next_pad){
      gchar *id;
	
      line = xmlNewNode(NULL,
			"ags-sf-line");

      /* properties */
      id = ags_id_generator_create_uuid();
	  
      xmlNewProp(line,
		 "id",
		 id);

      if(id != NULL){
	free(id);
      }
	
      if(channel->link != NULL){
	ags_simple_file_add_id_ref(simple_file,
				   g_object_new(AGS_TYPE_FILE_ID_REF,
						"file", simple_file,
						"node", line,
						"reference", channel,
						NULL));

	found_line_content = TRUE;
      }

      str = g_strdup_printf("%d",
			    channel->line);
	
      xmlNewProp(line,
		 "nth-line",
		 str);

      g_free(str);
	
      /* device */
      if(channel != NULL &&
	 channel->output_soundcard != NULL){
	gchar *device;

	device = ags_soundcard_get_device(AGS_SOUNDCARD(channel->output_soundcard));
    
	if(device != NULL){
	  xmlNewProp(line,
		     (xmlChar *) "soundcard-device",
		     (xmlChar *) device);

	  g_free(device);
	}
      }

      /* link */
      if(channel->link != NULL){
	AgsFileLookup *file_lookup;

	file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
						     "file", simple_file,
						     "node", line,
						     "reference", channel,
						     NULL);
	ags_simple_file_add_lookup(simple_file, (GObject *) file_lookup);
	g_signal_connect(G_OBJECT(file_lookup), "resolve",
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

xmlNode*
ags_simple_file_write_control(AgsSimpleFile *simple_file, xmlNode *parent, AgsBulkMember *bulk_member)
{
  GtkWidget *child_widget;
    
  xmlNode *control_node;

  gchar *str;
    
  /* control node */
  child_widget = ags_bulk_member_get_widget(bulk_member);
    
  if(GTK_IS_TOGGLE_BUTTON(child_widget)){
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");
      
    xmlNewProp(control_node,
	       "control-type",
	       G_OBJECT_TYPE_NAME(child_widget));
      
    xmlNewProp(control_node,
	       "value",
	       ((gtk_toggle_button_get_active((GtkToggleButton *) child_widget)) ? AGS_SIMPLE_FILE_TRUE:  AGS_SIMPLE_FILE_FALSE));
  }else if(GTK_IS_CHECK_BUTTON(child_widget)){
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");
      
    xmlNewProp(control_node,
	       "control-type",
	       G_OBJECT_TYPE_NAME(child_widget));
      
    xmlNewProp(control_node,
	       "value",
	       ((gtk_check_button_get_active((GtkCheckButton *) child_widget)) ? AGS_SIMPLE_FILE_TRUE:  AGS_SIMPLE_FILE_FALSE));
  }else if(AGS_IS_DIAL(child_widget)){
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");

    xmlNewProp(control_node,
	       "control-type",
	       G_OBJECT_TYPE_NAME(child_widget));

    str = g_strdup_printf("%lf",
			  gtk_adjustment_get_value(AGS_DIAL(child_widget)->adjustment));
      
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
  }else{
    g_warning("ags_file_write_effect_list() - unknown child of AgsBulkMember type");

    return(NULL);
  }

  xmlNewProp(control_node,
	     "specifier",
	     bulk_member->specifier);

  xmlAddChild(parent,
	      control_node);

  return(control_node);
}
  
xmlNode*
ags_simple_file_write_effect_list(AgsSimpleFile *simple_file, xmlNode *parent, AgsEffectBulk *effect_bulk)
{
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
    list_start = ags_effect_bulk_get_bulk_member(effect_bulk);
    
  while(list != NULL){
    if(AGS_IS_BULK_MEMBER(list->data)){
      if(g_list_find_custom(filename,
			    AGS_BULK_MEMBER(list->data)->filename,
			    (GCompareFunc) g_strcmp0) == NULL ||
	 g_list_find_custom(effect,
			    AGS_BULK_MEMBER(list->data)->effect,
			    (GCompareFunc) g_strcmp0) == NULL){
	GtkWidget *child_widget;

	if(effect_list_node == NULL){
	  effect_list_node = xmlNewNode(NULL,
					"ags-sf-effect-list");
	}
	  
	effect_node = xmlNewNode(NULL,
				 "ags-sf-effect");

	xmlNewProp(effect_node,
		   "filename",
		   AGS_BULK_MEMBER(list->data)->filename);

	filename = g_list_prepend(filename,
				  AGS_BULK_MEMBER(list->data)->filename);

	xmlNewProp(effect_node,
		   "effect",
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

  g_list_free(filename);
  g_list_free(effect);

  /* add to parent */
  if(effect_list_node != NULL){
    xmlAddChild(parent,
		effect_list_node);
  }

  return(effect_list_node);
}

xmlNode*
ags_simple_file_write_automation_port(AgsSimpleFile *simple_file, xmlNode *parent, GList *automation_port)
{
  xmlNode *node, *child;    

  node = NULL;
    
  if(automation_port != NULL){
    node = xmlNewNode(NULL,
		      "ags-sf-automation-port-list");
      
    while(automation_port != NULL){
      gchar *scope;
	
      child = xmlNewNode(NULL,
			 "ags-sf-automation-port");

      scope = NULL;

      if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == G_TYPE_NONE){
	scope = "audio";
      }else if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == AGS_TYPE_OUTPUT){
	scope = "output";
      }else if(AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->channel_type == AGS_TYPE_INPUT){
	scope = "input";
      }
	
      xmlNewProp(child,
		 "scope",
		 scope);

      xmlNewProp(child,
		 "specifier",
		 AGS_MACHINE_AUTOMATION_PORT(automation_port->data)->control_name);
	
      xmlAddChild(node,
		  child);

      automation_port = automation_port->next;
    }
  }
    
  /* add to parent */
  if(node != NULL){
    xmlAddChild(parent,
		node);
  }
    
  return(node);
}

xmlNode*
ags_simple_file_write_machine(AgsSimpleFile *simple_file, xmlNode *parent, AgsMachine *machine)
{
  xmlNode *node;
  xmlNode *pad_list;    

  GList *start_list, *list;

  gchar *id;
  xmlChar *str;    

  /* node and uuid */
  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-sf-machine");
  
  xmlNewProp(node,
	     (xmlChar *) AGS_SIMPLE_FILE_ID_PROP,
	     (xmlChar *) id);

  xmlNewProp(node,
	     (xmlChar *) AGS_SIMPLE_FILE_TYPE_PROP,
	     (xmlChar *) G_OBJECT_TYPE_NAME(machine));

  xmlNewProp(node,
	     (xmlChar *) AGS_SIMPLE_FILE_NAME_PROP,
	     (xmlChar *) machine->machine_name);

  ags_simple_file_add_id_ref(simple_file,
			     g_object_new(AGS_TYPE_FILE_ID_REF,
					  "file", simple_file,
					  "node", node,
					  "reference", machine,
					  NULL));
  
  /* device */
  if(machine->audio != NULL &&
     machine->audio->output_soundcard != NULL){
    gchar *device;

    device = ags_soundcard_get_device(AGS_SOUNDCARD(machine->audio->output_soundcard));
    
    if(device != NULL){
      xmlNewProp(node,
		 (xmlChar *) "soundcard-device",
		 (xmlChar *) device);

      g_free(device);
    }
  }

  /* audio name */
  if(machine->audio != NULL){
    gchar *audio_name;

    g_object_get(machine->audio,
		 "audio-name", &audio_name,
		 NULL);

    if(audio_name != NULL){
      xmlNewProp(node,
		 (xmlChar *) "audio-name",
		 (xmlChar *) audio_name);

      g_free(audio_name);
    }    
  }

  str = g_strdup_printf("%d", machine->audio->format);
  xmlNewProp(node,
	     (xmlChar *) "format",
	     (xmlChar *) str);

  g_free(str);
  
  str = g_strdup_printf("%d", machine->audio->samplerate);
  xmlNewProp(node,
	     (xmlChar *) "samplerate",
	     (xmlChar *) str);

  g_free(str);
    
  /* bank and mapping */
  str =  (xmlChar *) g_strdup_printf("%d",
				     machine->bank_0);
  
  xmlNewProp(node,
	     (xmlChar *) "bank-0",
	     str);

  g_free(str);

  str = (xmlChar *) g_strdup_printf("%d",
				    machine->bank_1);
  
  xmlNewProp(node,
	     (xmlChar *) "bank-1",
	     str);

  g_free(str);
  
  if(ags_audio_test_behaviour_flags(machine->audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
    xmlNewProp(node,
	       "reverse-mapping",
	       "true");
  }

  /* channels and pads */
  str = (xmlChar *) g_strdup_printf("%d",
				    machine->audio->audio_channels);
  
  xmlNewProp(node,
	     (xmlChar *) "channels",
	     str);

  g_free(str);

  str = (xmlChar *) g_strdup_printf("%d",
				    machine->audio->output_pads);
  
  xmlNewProp(node,
	     (xmlChar *) "output-pads",
	     str);

  g_free(str);

  str = (xmlChar *) g_strdup_printf("%d", machine->audio->input_pads);
  
  xmlNewProp(node,
	     (xmlChar *) "input-pads",
	     str);

  g_free(str);
  
  /* midi mapping */
  if((AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
    guint audio_start_mapping, audio_end_mapping;
    guint midi_start_mapping, midi_end_mapping;

    g_object_get(machine->audio,
		 "audio-start-mapping", &audio_start_mapping,
		 "audio-end-mapping", &audio_end_mapping,
		 "midi-start-mapping", &midi_start_mapping,
		 "midi-end-mapping", &midi_end_mapping,
		 NULL);
    
    str = g_strdup_printf("%d",
			  audio_start_mapping);
    
    xmlNewProp(node,
	       "audio-start-mapping",
	       str);

    g_free(str);

    str = g_strdup_printf("%d",
			  audio_end_mapping);
    
    xmlNewProp(node,
	       "audio-end-mapping",
	       str);

    g_free(str);

    str = g_strdup_printf("%d",
			  midi_start_mapping);
    
    xmlNewProp(node,
	       "midi-start-mapping",
	       str);

    str = g_strdup_printf("%d",
			  midi_end_mapping);
    
    xmlNewProp(node,
	       "midi-end-mapping",
	       str);

    g_free(str);
  }

  /* */
  start_list = ags_machine_get_dialog_model(machine);

  if(start_list != NULL){
    xmlNode *dialog_model_list;
    xmlNode *dialog_model;
    
    list = start_list;

    dialog_model_list = xmlNewNode(NULL,
				   "ags-sf-dialog-model-list");
    xmlAddChild(node,
		dialog_model_list);
  
    while(list != NULL){
      dialog_model = xmlNewNode(NULL,
				"ags-sf-dialog-model");
      xmlAddChild(dialog_model_list,
		  dialog_model);

      xmlAddChild(dialog_model,
		  list->data);
      
      list = list->next;
    }

    g_list_free(start_list);
  }
  
  /* machine specific */
  if(AGS_IS_EQUALIZER10(machine)){
    AgsEqualizer10 *equalizer10;

    xmlNode *child;
    xmlNode *control_node;
    
    equalizer10 = (AgsEqualizer10 *) machine;

    child = xmlNewNode(NULL,
		       "ags-sf-equalizer10");
    xmlAddChild(node,
		child);
    
    /* peak 28hz */
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");
    xmlAddChild(child,
		control_node);
    
    xmlNewProp(control_node,
	       "specifier",
	       "28 [Hz]");
    
    xmlNewProp(control_node,
	       "control-type",
	       "GtkScale");

    str = g_strdup_printf("%lf",
			  gtk_range_get_value(GTK_RANGE(equalizer10->peak_28hz)));
    
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
    
    /* peak 56hz */
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");
    xmlAddChild(child,
		control_node);

    xmlNewProp(control_node,
	       "specifier",
	       "56 [Hz]");
    
    xmlNewProp(control_node,
	       "control-type",
	       "GtkScale");

    str = g_strdup_printf("%lf",
			  gtk_range_get_value(GTK_RANGE(equalizer10->peak_56hz)));
    
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
    
    /* peak 112hz */
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");
    xmlAddChild(child,
		control_node);

    xmlNewProp(control_node,
	       "specifier",
	       "112 [Hz]");
    
    xmlNewProp(control_node,
	       "control-type",
	       "GtkScale");

    str = g_strdup_printf("%lf",
			  gtk_range_get_value(GTK_RANGE(equalizer10->peak_112hz)));
    
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
    
    /* peak 224hz */
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");
    xmlAddChild(child,
		control_node);

    xmlNewProp(control_node,
	       "specifier",
	       "224 [Hz]");
    
    xmlNewProp(control_node,
	       "control-type",
	       "GtkScale");

    str = g_strdup_printf("%lf",
			  gtk_range_get_value(GTK_RANGE(equalizer10->peak_224hz)));
    
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
    
    /* peak 448hz */
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");
    xmlAddChild(child,
		control_node);

    xmlNewProp(control_node,
	       "specifier",
	       "448 [Hz]");
    
    xmlNewProp(control_node,
	       "control-type",
	       "GtkScale");

    str = g_strdup_printf("%lf",
			  gtk_range_get_value(GTK_RANGE(equalizer10->peak_448hz)));
    
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
    
    /* peak 896hz */
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");
    xmlAddChild(child,
		control_node);

    xmlNewProp(control_node,
	       "specifier",
	       "896 [Hz]");
    
    xmlNewProp(control_node,
	       "control-type",
	       "GtkScale");

    str = g_strdup_printf("%lf", gtk_range_get_value(GTK_RANGE(equalizer10->peak_896hz)));
    
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
    
    /* peak 1792hz */
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");
    xmlAddChild(child,
		control_node);

    xmlNewProp(control_node,
	       "specifier",
	       "1792 [Hz]");
    
    xmlNewProp(control_node,
	       "control-type",
	       "GtkScale");

    str = g_strdup_printf("%lf",
			  gtk_range_get_value(GTK_RANGE(equalizer10->peak_1792hz)));
    
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
    
    /* peak 3584hz */
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");
    xmlAddChild(child,
		control_node);

    xmlNewProp(control_node,
	       "specifier",
	       "3584 [Hz]");
    
    xmlNewProp(control_node,
	       "control-type",
	       "GtkScale");

    str = g_strdup_printf("%lf",
			  gtk_range_get_value(GTK_RANGE(equalizer10->peak_3584hz)));
    
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
    
    /* peak 7168hz */
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");
    xmlAddChild(child,
		control_node);

    xmlNewProp(control_node,
	       "specifier",
	       "7168 [Hz]");
    
    xmlNewProp(control_node,
	       "control-type",
	       "GtkScale");

    str = g_strdup_printf("%lf",
			  gtk_range_get_value(GTK_RANGE(equalizer10->peak_7168hz)));
    
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
    
    /* peak 14336hz */
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");
    xmlAddChild(child,
		control_node);

    xmlNewProp(control_node,
	       "specifier",
	       "14336 [Hz]");
    
    xmlNewProp(control_node,
	       "control-type",
	       "GtkScale");

    str = g_strdup_printf("%lf", gtk_range_get_value(GTK_RANGE(equalizer10->peak_14336hz)));
    
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
    
    /* pressure */
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");
    xmlAddChild(child,
		control_node);

    xmlNewProp(control_node,
	       "specifier",
	       "pressure");
    
    xmlNewProp(control_node,
	       "control-type",
	       "GtkScale");

    str = g_strdup_printf("%lf",
			  gtk_range_get_value(GTK_RANGE(equalizer10->pressure)));
    
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
  }else if(AGS_IS_DRUM(machine)){
    AgsDrum *drum;

    drum = (AgsDrum *) machine;
    
    if(gtk_check_button_get_active((GtkCheckButton *) drum->loop_button)){
      xmlNewProp(node,
		 "loop",
		 "true");
    }

    str = g_strdup_printf("%u",
			  (guint) gtk_spin_button_get_value(drum->length_spin));
    
    xmlNewProp(node,
	       "length",
	       str);

    g_free(str);
  }else if(AGS_IS_MATRIX(machine)){
    AgsMatrix *matrix;

    matrix = (AgsMatrix *) machine;
    
    if(gtk_check_button_get_active((GtkCheckButton *) matrix->loop_button)){
      xmlNewProp(node,
		 "loop",
		 "true");
    }    

    str = g_strdup_printf("%u", (guint) gtk_spin_button_get_value(matrix->length_spin));

    xmlNewProp(node,
	       "length",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_range_get_value((GtkRange *) matrix->volume));
    
    xmlNewProp(node,
	       "volume",
	       str);

    g_free(str);
  }else if(AGS_IS_SYNTH(machine)){
    AgsSynth *synth;

    synth = (AgsSynth *) machine;

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(synth->lower));
    
    xmlNewProp(node,
	       "base-note",
	       str);

    g_free(str);
  }else if(AGS_IS_FM_SYNTH(machine)){
    AgsFMSynth *fm_synth;

    fm_synth = (AgsFMSynth *) machine;

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(fm_synth->lower));
    
    xmlNewProp(node,
	       "base-note",
	       str);

    g_free(str);
  }else if(AGS_IS_SYNCSYNTH(machine)){
    AgsSyncsynth *syncsynth;

    syncsynth = (AgsSyncsynth *) machine;

    str = g_strdup_printf("%lf", gtk_spin_button_get_value(syncsynth->lower));
    
    xmlNewProp(node,
	       "base-note",
	       str);

    g_free(str);
    
    str = g_strdup_printf("%u",
			  (guint) round(gtk_spin_button_get_value(syncsynth->loop_start)));
    
    xmlNewProp(node,
	       "audio-loop-start",
	       str);

    g_free(str);
    
    str = g_strdup_printf("%u",
			  (guint) round(gtk_spin_button_get_value(syncsynth->loop_end)));
    
    xmlNewProp(node,
	       "audio-loop-end",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_range_get_value((GtkRange *) syncsynth->volume));
    
    xmlNewProp(node,
	       "volume",
	       str);

    g_free(str);
  }else if(AGS_IS_FM_SYNCSYNTH(machine)){
    AgsFMSyncsynth *fm_syncsynth;

    fm_syncsynth = (AgsFMSyncsynth *) machine;

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(fm_syncsynth->lower));
    
    xmlNewProp(node,
	       "base-note",
	       str);

    g_free(str);

    str = g_strdup_printf("%u",
			  (guint) round(gtk_spin_button_get_value(fm_syncsynth->loop_start)));
    
    xmlNewProp(node,
	       "audio-loop-start",
	       str);

    g_free(str);

    str = g_strdup_printf("%u",
			  (guint) round(gtk_spin_button_get_value(fm_syncsynth->loop_end)));
    
    xmlNewProp(node,
	       "audio-loop-end",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_range_get_value((GtkRange *) fm_syncsynth->volume));
    
    xmlNewProp(node,
	       "volume",
	       str);

    g_free(str);
  }else if(AGS_IS_HYBRID_SYNTH(machine)){
    AgsHybridSynth *hybrid_synth;

    hybrid_synth = (AgsHybridSynth *) machine;

    /* synth-0 */
    str = g_strdup_printf("%d", gtk_combo_box_get_active(hybrid_synth->synth_0_oscillator));
    
    xmlNewProp(node,
	       "synth-0-oscillator",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_0_octave));
    
    xmlNewProp(node,
	       "synth-0-octave",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_0_key));
    
    xmlNewProp(node,
	       "synth-0-key",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_0_phase));
    
    xmlNewProp(node,
	       "synth-0-phase",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_0_volume));
    
    xmlNewProp(node,
	       "synth-0-volume",
	       str);

    g_free(str);    

    if(gtk_check_button_get_active(hybrid_synth->synth_0_sync_enabled)){
      xmlNewProp(node,
		 "synth-0-sync-enabled",
		 "true");
    }

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_0_sync_relative_attack_factor));
    
    xmlNewProp(node,
	       "synth-0-sync-relative-attack-factor",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_0_sync_attack_0));
    
    xmlNewProp(node,
	       "synth-0-sync-attack-0",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_0_sync_phase_0));
    
    xmlNewProp(node,
	       "synth-0-sync-phase-0",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_0_sync_attack_1));
    
    xmlNewProp(node,
	       "synth-0-sync-attack-1",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_0_sync_phase_1));
    
    xmlNewProp(node,
	       "synth-0-sync-phase-1",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_0_sync_attack_2));
    
    xmlNewProp(node,
	       "synth-0-sync-attack-2",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_0_sync_phase_2));
    
    xmlNewProp(node,
	       "synth-0-sync-phase-2",
	       str);

    g_free(str);    

    str = g_strdup_printf("%d", gtk_combo_box_get_active(hybrid_synth->synth_0_sync_lfo_oscillator));
    
    xmlNewProp(node,
	       "synth-0-sync-lfo-oscillator",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(hybrid_synth->synth_0_sync_lfo_frequency));
    
    xmlNewProp(node,
	       "synth-0-sync-lfo-frequency",
	       str);

    g_free(str);    

    /* synth-1 */
    str = g_strdup_printf("%d", gtk_combo_box_get_active(hybrid_synth->synth_1_oscillator));
    
    xmlNewProp(node,
	       "synth-1-oscillator",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_1_octave));
    
    xmlNewProp(node,
	       "synth-1-octave",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_1_key));
    
    xmlNewProp(node,
	       "synth-1-key",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_1_phase));
    
    xmlNewProp(node,
	       "synth-1-phase",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_1_volume));
    
    xmlNewProp(node,
	       "synth-1-volume",
	       str);

    g_free(str);    

    if(gtk_check_button_get_active(hybrid_synth->synth_1_sync_enabled)){
      xmlNewProp(node,
		 "synth-1-sync-enabled",
		 "true");
    }

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_1_sync_relative_attack_factor));
    
    xmlNewProp(node,
	       "synth-1-sync-relative-attack-factor",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_1_sync_attack_0));
    
    xmlNewProp(node,
	       "synth-1-sync-attack-0",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_1_sync_phase_0));
    
    xmlNewProp(node,
	       "synth-1-sync-phase-0",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_1_sync_attack_1));
    
    xmlNewProp(node,
	       "synth-1-sync-attack-1",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_1_sync_phase_1));
    
    xmlNewProp(node,
	       "synth-1-sync-phase-1",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_1_sync_attack_2));
    
    xmlNewProp(node,
	       "synth-1-sync-attack-2",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->synth_1_sync_phase_2));
    
    xmlNewProp(node,
	       "synth-1-sync-phase-2",
	       str);

    g_free(str);    

    str = g_strdup_printf("%d", gtk_combo_box_get_active(hybrid_synth->synth_1_sync_lfo_oscillator));
    
    xmlNewProp(node,
	       "synth-1-sync-lfo-oscillator",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(hybrid_synth->synth_1_sync_lfo_frequency));
    
    xmlNewProp(node,
	       "synth-1-sync-lfo-frequency",
	       str);

    g_free(str);    

    /* effects */
    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->pitch_tuning));
    
    xmlNewProp(node,
	       "pitch-tuning",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->noise_gain));
    
    xmlNewProp(node,
	       "noise-gain",
	       str);

    g_free(str);    

    /* low pass */
    if(gtk_check_button_get_active(hybrid_synth->low_pass_enabled)){
      xmlNewProp(node,
		 "low-pass-enabled",
		 "true");
    }

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->low_pass_q_lin));
    
    xmlNewProp(node,
	       "low-pass-q-lin",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->low_pass_filter_gain));
    
    xmlNewProp(node,
	       "low-pass-filter-gain",
	       str);

    g_free(str);

    /* high pass */
    if(gtk_check_button_get_active(hybrid_synth->high_pass_enabled)){
      xmlNewProp(node,
		 "high-pass-enabled",
		 "true");
    }

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->high_pass_q_lin));
    
    xmlNewProp(node,
	       "high-pass-q-lin",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->high_pass_filter_gain));
    
    xmlNewProp(node,
	       "high-pass-filter-gain",
	       str);

    g_free(str);

    /* chorus */
    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->chorus_input_volume));
    
    xmlNewProp(node,
	       "chorus-input-volume",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->chorus_output_volume));
    
    xmlNewProp(node,
	       "chorus-output-volume",
	       str);

    g_free(str);    

    str = g_strdup_printf("%d", gtk_combo_box_get_active(hybrid_synth->chorus_lfo_oscillator));
    
    xmlNewProp(node,
	       "chorus-lfo-oscillator",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(hybrid_synth->chorus_lfo_frequency));
    
    xmlNewProp(node,
	       "chorus-lfo-frequency",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->chorus_depth));
    
    xmlNewProp(node,
	       "chorus-depth",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->chorus_mix));
    
    xmlNewProp(node,
	       "chorus-mix",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_synth->chorus_delay));
    
    xmlNewProp(node,
	       "chorus-delay",
	       str);

    g_free(str);    
  }else if(AGS_IS_HYBRID_FM_SYNTH(machine)){
    AgsHybridFMSynth *hybrid_fm_synth;

    hybrid_fm_synth = (AgsHybridFMSynth *) machine;

    /* synth-0 */
    str = g_strdup_printf("%d", gtk_combo_box_get_active(hybrid_fm_synth->synth_0_oscillator));
    
    xmlNewProp(node,
	       "synth-0-oscillator",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_0_octave));
    
    xmlNewProp(node,
	       "synth-0-octave",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_0_key));
    
    xmlNewProp(node,
	       "synth-0-key",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_0_phase));
    
    xmlNewProp(node,
	       "synth-0-phase",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_0_volume));
    
    xmlNewProp(node,
	       "synth-0-volume",
	       str);

    g_free(str);    

    str = g_strdup_printf("%d",
			  gtk_combo_box_get_active(hybrid_fm_synth->synth_0_lfo_oscillator));
    
    xmlNewProp(node,
	       "synth-0-lfo-oscillator",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(hybrid_fm_synth->synth_0_lfo_frequency));
    
    xmlNewProp(node,
	       "synth-0-lfo-frequency",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_0_lfo_depth));
    
    xmlNewProp(node,
	       "synth-0-lfo_depth",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_0_lfo_tuning));
    
    xmlNewProp(node,
	       "synth-0-lfo-tuning",
	       str);

    g_free(str);    

    /* synth-1 */
    str = g_strdup_printf("%d", gtk_combo_box_get_active(hybrid_fm_synth->synth_1_oscillator));
    
    xmlNewProp(node,
	       "synth-1-oscillator",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_1_octave));
    
    xmlNewProp(node,
	       "synth-1-octave",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_1_key));
    
    xmlNewProp(node,
	       "synth-1-key",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_1_phase));
    
    xmlNewProp(node,
	       "synth-1-phase",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_1_volume));
    
    xmlNewProp(node,
	       "synth-1-volume",
	       str);

    g_free(str);    

    str = g_strdup_printf("%d",
			  gtk_combo_box_get_active(hybrid_fm_synth->synth_1_lfo_oscillator));
    
    xmlNewProp(node,
	       "synth-1-lfo-oscillator",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(hybrid_fm_synth->synth_1_lfo_frequency));
    
    xmlNewProp(node,
	       "synth-1-lfo-frequency",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_1_lfo_depth));
    
    xmlNewProp(node,
	       "synth-1-lfo_depth",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_1_lfo_tuning));
    
    xmlNewProp(node,
	       "synth-1-lfo-tuning",
	       str);

    g_free(str);    

    /* synth-2 */
    str = g_strdup_printf("%d", gtk_combo_box_get_active(hybrid_fm_synth->synth_2_oscillator));
    
    xmlNewProp(node,
	       "synth-2-oscillator",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_2_octave));
    
    xmlNewProp(node,
	       "synth-2-octave",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_2_key));
    
    xmlNewProp(node,
	       "synth-2-key",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_2_phase));
    
    xmlNewProp(node,
	       "synth-2-phase",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_2_volume));
    
    xmlNewProp(node,
	       "synth-2-volume",
	       str);

    g_free(str);    

    str = g_strdup_printf("%d",
			  gtk_combo_box_get_active(hybrid_fm_synth->synth_2_lfo_oscillator));
    
    xmlNewProp(node,
	       "synth-2-lfo-oscillator",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(hybrid_fm_synth->synth_2_lfo_frequency));
    
    xmlNewProp(node,
	       "synth-2-lfo-frequency",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_2_lfo_depth));
    
    xmlNewProp(node,
	       "synth-2-lfo_depth",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->synth_2_lfo_tuning));
    
    xmlNewProp(node,
	       "synth-2-lfo-tuning",
	       str);

    g_free(str);    
    
    /* effects */
    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->pitch_tuning));
    
    xmlNewProp(node,
	       "pitch-tuning",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->noise_gain));
    
    xmlNewProp(node,
	       "noise-gain",
	       str);

    g_free(str);    

    /* low pass */
    if(gtk_check_button_get_active(hybrid_fm_synth->low_pass_enabled)){
      xmlNewProp(node,
		 "low-pass-enabled",
		 "true");
    }

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->low_pass_q_lin));
    
    xmlNewProp(node,
	       "low-pass-q-lin",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->low_pass_filter_gain));
    
    xmlNewProp(node,
	       "low-pass-filter-gain",
	       str);

    g_free(str);

    /* high pass */
    if(gtk_check_button_get_active(hybrid_fm_synth->high_pass_enabled)){
      xmlNewProp(node,
		 "high-pass-enabled",
		 "true");
    }

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->high_pass_q_lin));
    
    xmlNewProp(node,
	       "high-pass-q-lin",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->high_pass_filter_gain));
    
    xmlNewProp(node,
	       "high-pass-filter-gain",
	       str);

    g_free(str);

    /* chorus */
    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->chorus_input_volume));
    
    xmlNewProp(node,
	       "chorus-input-volume",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->chorus_output_volume));
    
    xmlNewProp(node,
	       "chorus-output-volume",
	       str);

    g_free(str);    

    str = g_strdup_printf("%d", gtk_combo_box_get_active(hybrid_fm_synth->chorus_lfo_oscillator));
    
    xmlNewProp(node,
	       "chorus-lfo-oscillator",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(hybrid_fm_synth->chorus_lfo_frequency));
    
    xmlNewProp(node,
	       "chorus-lfo-frequency",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->chorus_depth));
    
    xmlNewProp(node,
	       "chorus-depth",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->chorus_mix));
    
    xmlNewProp(node,
	       "chorus-mix",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(hybrid_fm_synth->chorus_delay));
    
    xmlNewProp(node,
	       "chorus-delay",
	       str);

    g_free(str);    
  }else if(AGS_IS_PITCH_SAMPLER(machine)){
    AgsPitchSampler *pitch_sampler;

    pitch_sampler = (AgsPitchSampler *) machine;

    if(pitch_sampler->audio_container != NULL &&
       pitch_sampler->audio_container->filename != NULL){
      xmlNewProp(node,
		 "filename",
		 pitch_sampler->audio_container->filename);
    }

    if(gtk_check_button_get_active(pitch_sampler->enable_synth_generator)){
      xmlNewProp(node,
		 "enable-synth-generator",
		 "true");
    }

    str = gtk_combo_box_text_get_active_text(pitch_sampler->pitch_function);
      
    xmlNewProp(node,
	       "pitch-type",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(pitch_sampler->lower));
    
    xmlNewProp(node,
	       "base-note",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(pitch_sampler->key_count));
    
    xmlNewProp(node,
	       "key-count",
	       str);

    g_free(str);    

    if(gtk_check_button_get_active(pitch_sampler->enable_aliase)){
      xmlNewProp(node,
		 "enable-aliase",
		 "true");
    }

    str = g_strdup_printf("%lf",
			  gtk_adjustment_get_value(pitch_sampler->aliase_a_amount->adjustment));
    
    xmlNewProp(node,
	       "aliase-a-amount",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_adjustment_get_value(pitch_sampler->aliase_a_phase->adjustment));
    
    xmlNewProp(node,
	       "aliase-a-phase",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_adjustment_get_value(pitch_sampler->aliase_b_amount->adjustment));
    
    xmlNewProp(node,
	       "aliase-b-amount",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_adjustment_get_value(pitch_sampler->aliase_b_phase->adjustment));
    
    xmlNewProp(node,
	       "aliase-b-phase",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_range_get_value((GtkRange *) pitch_sampler->volume));
    
    xmlNewProp(node,
	       "volume",
	       str);

    g_free(str);

    if(gtk_check_button_get_active(pitch_sampler->enable_lfo)){
      xmlNewProp(node,
		 "enable-lfo",
		 "true");
    }

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(pitch_sampler->lfo_freq));
    
    xmlNewProp(node,
	       "lfo-freq",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(pitch_sampler->lfo_phase));
    
    xmlNewProp(node,
	       "lfo-phase",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(pitch_sampler->lfo_depth));
    
    xmlNewProp(node,
	       "lfo-depth",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(pitch_sampler->lfo_tuning));
    
    xmlNewProp(node,
	       "lfo-tuning",
	       str);

    g_free(str);
  }else if(AGS_IS_SFZ_SYNTH(machine)){
    AgsSFZSynth *sfz_synth;
    
    sfz_synth = (AgsSFZSynth *) machine;

    if(sfz_synth->audio_container != NULL){
      xmlNewProp(node,
		 "filename",
		 sfz_synth->audio_container->filename);
    }
    
    /* synth */
    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->synth_octave));
    
    xmlNewProp(node,
	       "synth-octave",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->synth_key));
    
    xmlNewProp(node,
	       "synth-key",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->synth_volume));
    
    xmlNewProp(node,
	       "synth-volume",
	       str);

    g_free(str);    

    /* chorus */
    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->chorus_input_volume));
    
    xmlNewProp(node,
	       "chorus-input-volume",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->chorus_output_volume));
    
    xmlNewProp(node,
	       "chorus-output-volume",
	       str);

    g_free(str);    

    str = g_strdup_printf("%d", gtk_combo_box_get_active(sfz_synth->chorus_lfo_oscillator));
    
    xmlNewProp(node,
	       "chorus-lfo-oscillator",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(sfz_synth->chorus_lfo_frequency));
    
    xmlNewProp(node,
	       "chorus-lfo-frequency",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->chorus_depth));
    
    xmlNewProp(node,
	       "chorus-depth",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->chorus_mix));
    
    xmlNewProp(node,
	       "chorus-mix",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->chorus_delay));
    
    xmlNewProp(node,
	       "chorus-delay",
	       str);

    g_free(str);    

    /* ext */
    str = g_strdup_printf("%s",
			  (sfz_synth->tremolo_enabled ? "true": "false"));
    
    xmlNewProp(node,
	       "tremolo-enabled",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->tremolo_gain));
    
    xmlNewProp(node,
	       "tremolo-gain",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->tremolo_lfo_depth));
    
    xmlNewProp(node,
	       "tremolo-lfo-depth",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->tremolo_lfo_freq));
    
    xmlNewProp(node,
	       "tremolo-lfo-freq",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->tremolo_tuning));
    
    xmlNewProp(node,
	       "tremolo-tuning",
	       str);

    g_free(str);    

    str = g_strdup_printf("%s",
			  (sfz_synth->vibrato_enabled ? "true": "false"));
    
    xmlNewProp(node,
	       "vibrato-enabled",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->vibrato_gain));
    
    xmlNewProp(node,
	       "vibrato-gain",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->vibrato_lfo_depth));
    
    xmlNewProp(node,
	       "vibrato-lfo-depth",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->vibrato_lfo_freq));
    
    xmlNewProp(node,
	       "vibrato-lfo-freq",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->vibrato_tuning));
    
    xmlNewProp(node,
	       "vibrato-tuning",
	       str);

    g_free(str);    

    str = g_strdup_printf("%s",
			  (sfz_synth->wah_wah_enabled ? "true": "false"));
    
    xmlNewProp(node,
	       "wah-wah-enabled",
	       str);

    g_free(str);    

    str = g_strdup_printf("%d",
			  gtk_combo_box_get_active(sfz_synth->wah_wah_length));
    
    xmlNewProp(node,
	       "wah-wah-length",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->wah_wah_attack_x));
    
    xmlNewProp(node,
	       "wah-wah-attack-x",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->wah_wah_attack_y));
    
    xmlNewProp(node,
	       "wah-wah-attack-y",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->wah_wah_attack_x));
    
    xmlNewProp(node,
	       "wah-wah-decay-x",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->wah_wah_decay_y));
    
    xmlNewProp(node,
	       "wah-wah-decay-y",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->wah_wah_sustain_x));
    
    xmlNewProp(node,
	       "wah-wah-sustain-x",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->wah_wah_sustain_y));
    
    xmlNewProp(node,
	       "wah-wah-sustain-y",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->wah_wah_release_x));
    
    xmlNewProp(node,
	       "wah-wah-release-x",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->wah_wah_release_y));
    
    xmlNewProp(node,
	       "wah-wah-release-y",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->wah_wah_ratio));
    
    xmlNewProp(node,
	       "wah-wah-ratio",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->wah_wah_lfo_depth));
    
    xmlNewProp(node,
	       "wah-wah-lfo-depth",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->wah_wah_lfo_freq));
    
    xmlNewProp(node,
	       "wah-wah-lfo-freq",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sfz_synth->wah_wah_tuning));
    
    xmlNewProp(node,
	       "wah-wah-tuning",
	       str);

    g_free(str);    
#ifdef AGS_WITH_LIBINSTPATCH
  }else if(AGS_IS_FFPLAYER(machine)){
    AgsFFPlayer *ffplayer;

    ffplayer = (AgsFFPlayer *) machine;

    if(ffplayer->audio_container != NULL &&
       ffplayer->audio_container->filename != NULL){
      xmlNewProp(node,
		 "filename",
		 ffplayer->audio_container->filename);

      str = gtk_combo_box_text_get_active_text(ffplayer->preset);
      
      xmlNewProp(node,
		 "preset",
		 str);

      g_free(str);

      str = gtk_combo_box_text_get_active_text(ffplayer->instrument);
      
      xmlNewProp(node,
		 "instrument",
		 str);

      g_free(str);
    }

    if(gtk_check_button_get_active(ffplayer->enable_synth_generator)){
      xmlNewProp(node,
		 "enable-synth-generator",
		 "true");
    }

    str = gtk_combo_box_text_get_active_text(ffplayer->pitch_function);
      
    xmlNewProp(node,
	       "pitch-type",
	       str);

    g_free(str);
    
    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(ffplayer->lower));
    
    xmlNewProp(node,
	       "base-note",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(ffplayer->key_count));
    
    xmlNewProp(node,
	       "key-count",
	       str);

    g_free(str);

    if(gtk_check_button_get_active(ffplayer->enable_aliase)){
      xmlNewProp(node,
		 "enable-aliase",
		 "true");
    }

    str = g_strdup_printf("%lf",
			  gtk_adjustment_get_value(ffplayer->aliase_a_amount->adjustment));
    
    xmlNewProp(node,
	       "aliase-a-amount",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_adjustment_get_value(ffplayer->aliase_a_phase->adjustment));
    
    xmlNewProp(node,
	       "aliase-a-phase",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_adjustment_get_value(ffplayer->aliase_b_amount->adjustment));
    
    xmlNewProp(node,
	       "aliase-b-amount",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_adjustment_get_value(ffplayer->aliase_b_phase->adjustment));
    
    xmlNewProp(node,
	       "aliase-b-phase",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_range_get_value((GtkRange *) ffplayer->volume));
    
    xmlNewProp(node,
	       "volume",
	       str);

    g_free(str);
  }else if(AGS_IS_SF2_SYNTH(machine)){
    AgsSF2Synth *sf2_synth;

    gint bank, program;
    
    sf2_synth = (AgsSF2Synth *) machine;

    if(sf2_synth->audio_container != NULL){
      xmlNewProp(node,
		 "filename",
		 sf2_synth->audio_container->filename);

      str = g_strdup_printf("%d",
			    sf2_synth->bank);
    
      xmlNewProp(node,
		 "bank",
		 str);

      g_free(str);

      str = g_strdup_printf("%d",
			    sf2_synth->program);
    
      xmlNewProp(node,
		 "program",
		 str);

      g_free(str);
    }
    
    /* synth */
    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->synth_octave));
    
    xmlNewProp(node,
	       "synth-octave",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->synth_key));
    
    xmlNewProp(node,
	       "synth-key",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->synth_volume));
    
    xmlNewProp(node,
	       "synth-volume",
	       str);

    g_free(str);    

    /* chorus */
    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->chorus_input_volume));
    
    xmlNewProp(node,
	       "chorus-input-volume",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->chorus_output_volume));
    
    xmlNewProp(node,
	       "chorus-output-volume",
	       str);

    g_free(str);    

    str = g_strdup_printf("%d", gtk_combo_box_get_active(sf2_synth->chorus_lfo_oscillator));
    
    xmlNewProp(node,
	       "chorus-lfo-oscillator",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(sf2_synth->chorus_lfo_frequency));
    
    xmlNewProp(node,
	       "chorus-lfo-frequency",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->chorus_depth));
    
    xmlNewProp(node,
	       "chorus-depth",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->chorus_mix));
    
    xmlNewProp(node,
	       "chorus-mix",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->chorus_delay));
    
    xmlNewProp(node,
	       "chorus-delay",
	       str);

    g_free(str);    

    /* ext */
    str = g_strdup_printf("%s",
			  (sf2_synth->tremolo_enabled ? "true": "false"));
    
    xmlNewProp(node,
	       "tremolo-enabled",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->tremolo_gain));
    
    xmlNewProp(node,
	       "tremolo-gain",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->tremolo_lfo_depth));
    
    xmlNewProp(node,
	       "tremolo-lfo-depth",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->tremolo_lfo_freq));
    
    xmlNewProp(node,
	       "tremolo-lfo-freq",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->tremolo_tuning));
    
    xmlNewProp(node,
	       "tremolo-tuning",
	       str);

    g_free(str);    

    str = g_strdup_printf("%s",
			  (sf2_synth->vibrato_enabled ? "true": "false"));
    
    xmlNewProp(node,
	       "vibrato-enabled",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->vibrato_gain));
    
    xmlNewProp(node,
	       "vibrato-gain",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->vibrato_lfo_depth));
    
    xmlNewProp(node,
	       "vibrato-lfo-depth",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->vibrato_lfo_freq));
    
    xmlNewProp(node,
	       "vibrato-lfo-freq",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->vibrato_tuning));
    
    xmlNewProp(node,
	       "vibrato-tuning",
	       str);

    g_free(str);    

    str = g_strdup_printf("%s",
			  (sf2_synth->wah_wah_enabled ? "true": "false"));
    
    xmlNewProp(node,
	       "wah-wah-enabled",
	       str);

    g_free(str);    

    str = g_strdup_printf("%d",
			  gtk_combo_box_get_active(sf2_synth->wah_wah_length));
    
    xmlNewProp(node,
	       "wah-wah-length",
	       str);

    g_free(str);    
    
    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->wah_wah_attack_x));
    
    xmlNewProp(node,
	       "wah-wah-attack-x",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->wah_wah_attack_y));
    
    xmlNewProp(node,
	       "wah-wah-attack-y",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->wah_wah_attack_x));
    
    xmlNewProp(node,
	       "wah-wah-decay-x",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->wah_wah_decay_y));
    
    xmlNewProp(node,
	       "wah-wah-decay-y",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->wah_wah_sustain_x));
    
    xmlNewProp(node,
	       "wah-wah-sustain-x",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->wah_wah_sustain_y));
    
    xmlNewProp(node,
	       "wah-wah-sustain-y",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->wah_wah_release_x));
    
    xmlNewProp(node,
	       "wah-wah-release-x",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->wah_wah_release_y));
    
    xmlNewProp(node,
	       "wah-wah-release-y",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->wah_wah_ratio));
    
    xmlNewProp(node,
	       "wah-wah-ratio",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->wah_wah_lfo_depth));
    
    xmlNewProp(node,
	       "wah-wah-lfo-depth",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->wah_wah_lfo_freq));
    
    xmlNewProp(node,
	       "wah-wah-lfo-freq",
	       str);

    g_free(str);    

    str = g_strdup_printf("%lf",
			  ags_dial_get_value(sf2_synth->wah_wah_tuning));
    
    xmlNewProp(node,
	       "wah-wah-tuning",
	       str);

    g_free(str);    
#endif
  }else if(AGS_IS_AUDIOREC(machine)){
    AgsAudiorec *audiorec;
    
    audiorec = (AgsAudiorec *) machine;

    xmlNewProp(node,
	       "filename",
	       gtk_editable_get_text(GTK_EDITABLE(audiorec->filename)));
  }else if(AGS_IS_LADSPA_BRIDGE(machine)){
    AgsLadspaBridge *ladspa_bridge;

    ladspa_bridge = (AgsLadspaBridge *) machine;

    xmlNewProp(node,
	       "plugin-file",
	       ladspa_bridge->filename);

    xmlNewProp(node,
	       "effect",
	       ladspa_bridge->effect);
  }else if(AGS_IS_DSSI_BRIDGE(machine)){
    AgsDssiBridge *dssi_bridge;

    dssi_bridge = (AgsDssiBridge *) machine;

    xmlNewProp(node,
	       "plugin-file",
	       dssi_bridge->filename);

    xmlNewProp(node,
	       "effect",
	       dssi_bridge->effect);

    str = gtk_combo_box_text_get_active_text(dssi_bridge->program);
    
    xmlNewProp(node,
	       "program",
	       str);

    g_free(str);
  }else if(AGS_IS_LIVE_DSSI_BRIDGE(machine)){
    AgsLiveDssiBridge *live_dssi_bridge;

    live_dssi_bridge = (AgsLiveDssiBridge *) machine;

    xmlNewProp(node,
	       "plugin-file",
	       live_dssi_bridge->filename);

    xmlNewProp(node,
	       "effect",
	       live_dssi_bridge->effect);

    str = gtk_combo_box_text_get_active_text(live_dssi_bridge->program);
    
    xmlNewProp(node,
	       "program",
	       str);

    g_free(str);
  }else if(AGS_IS_LV2_BRIDGE(machine)){
    AgsLv2Bridge *lv2_bridge;

    lv2_bridge = (AgsLv2Bridge *) machine;

    xmlNewProp(node,
	       "plugin-file",
	       lv2_bridge->filename);

    xmlNewProp(node,
	       "effect",
	       lv2_bridge->effect);

    if(lv2_bridge->preset != NULL){
      str = gtk_combo_box_text_get_active_text(lv2_bridge->preset);
      
      xmlNewProp(node,
		 "preset",
		 str);

      g_free(str);
    }
  }else if(AGS_IS_LIVE_LV2_BRIDGE(machine)){
    AgsLiveLv2Bridge *live_lv2_bridge;

    live_lv2_bridge = (AgsLiveLv2Bridge *) machine;

    xmlNewProp(node,
	       "plugin-file",
	       live_lv2_bridge->filename);

    xmlNewProp(node,
	       "effect",
	       live_lv2_bridge->effect);

    if(live_lv2_bridge->preset != NULL){
      str = gtk_combo_box_text_get_active_text(live_lv2_bridge->preset);
      
      xmlNewProp(node,
		 "preset",
		 str);

      g_free(str);
    }
#if defined(AGS_WITH_VST3)
  }else if(AGS_IS_VST3_BRIDGE(machine)){
    AgsVst3Bridge *vst3_bridge;

    vst3_bridge = (AgsVst3Bridge *) machine;

    xmlNewProp(node,
	       "plugin-file",
	       vst3_bridge->filename);

    xmlNewProp(node,
	       "effect",
	       vst3_bridge->effect);
  }else if(AGS_IS_LIVE_VST3_BRIDGE(machine)){
    AgsLiveVst3Bridge *live_vst3_bridge;

    live_vst3_bridge = (AgsLiveVst3Bridge *) machine;

    xmlNewProp(node,
	       "plugin-file",
	       live_vst3_bridge->filename);

    xmlNewProp(node,
	       "effect",
	       live_vst3_bridge->effect);
#endif
  }
  
  /* input */
  if(machine->input_pad != NULL){
    list = ags_machine_get_input_pad(machine);
  
    pad_list =  ags_simple_file_write_pad_list(simple_file,
					       node,
					       list);
    xmlNewProp(pad_list,
	       "is-output",
	       "false");
    
    g_list_free(list);
  }else{
    AgsChannel *channel;
    
    channel = machine->audio->input;

    if(channel != NULL){      
      pad_list = xmlNewNode(NULL,
			    "ags-sf-pad-list");
      xmlNewProp(pad_list,
		 "is-output",
		 "false");

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
  if(machine->output_pad != NULL){
    list = ags_machine_get_output_pad(machine);
  
    pad_list = ags_simple_file_write_pad_list(simple_file,
					      node,
					      list);
    xmlNewProp(pad_list,
	       "is-output",
	       "true");
    
    g_list_free(list);
  }else{
    AgsChannel *channel;
    
    channel = machine->audio->output;

    if(channel != NULL){
      pad_list = xmlNewNode(NULL,
			    "ags-sf-pad-list");
      xmlNewProp(pad_list,
		 "is-output",
		 "true");

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
      
      child = ags_simple_file_write_effect_list(simple_file, node, (AgsEffectBulk *) AGS_EFFECT_BRIDGE(machine->bridge)->bulk_input);

      if(child != NULL){
	xmlNewProp(child,
		   "is-output",
		   "false");
      }
    }

    if(AGS_EFFECT_BRIDGE(machine->bridge)->bulk_output != NULL){
      xmlNode *child;

      child = ags_simple_file_write_effect_list(simple_file, node, (AgsEffectBulk *) AGS_EFFECT_BRIDGE(machine->bridge)->bulk_output);

      if(child != NULL){
	xmlNewProp(child,
		   "is-output",
		   "true");
      }
    }

    if(AGS_EFFECT_BRIDGE(machine->bridge)->input_effect_pad != NULL){
      xmlNode *child;
      
      GList *list;

      list = ags_effect_bridge_get_input_effect_pad(AGS_EFFECT_BRIDGE(machine->bridge));
      
      child = ags_simple_file_write_effect_pad_list(simple_file,
						    node,
						    list);
      xmlNewProp(child,
		 "is-output",
		 "false");

      g_list_free(list);
    }

    if(AGS_EFFECT_BRIDGE(machine->bridge)->output_effect_pad != NULL){
      xmlNode *child;
      
      GList *list;

      list = ags_effect_bridge_get_output_effect_pad(AGS_EFFECT_BRIDGE(machine->bridge));
      
      child = ags_simple_file_write_effect_pad_list(simple_file,
						    node,
						    list);
      xmlNewProp(child,
		 "is-output",
		 "true");
      
      g_list_free(list);
    }
  }

  /* oscillator list */
  if(AGS_IS_SYNCSYNTH(machine)){
    xmlNode *child;
    
    GList *start_oscillator;

    start_oscillator = ags_syncsynth_get_oscillator(AGS_SYNCSYNTH(machine));
    
    child = ags_simple_file_write_oscillator_list(simple_file,
						  node,
						  start_oscillator);
    
    g_list_free(start_oscillator);
  }else if(AGS_IS_FM_SYNCSYNTH(machine)){
    xmlNode *child;
    
    GList *start_fm_oscillator;

    start_fm_oscillator = ags_fm_syncsynth_get_fm_oscillator(AGS_FM_SYNCSYNTH(machine));
    
    child = ags_simple_file_write_fm_oscillator_list(simple_file,
						     node,
						     start_fm_oscillator);
    
    g_list_free(start_fm_oscillator);
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
					       "ags-sf-pattern-list");
	      }

	      pattern_node = xmlNewNode(NULL,
					"ags-sf-pattern");

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

	      free(content);
	      
	      /* attributes */
	      str = g_strdup_printf("%d",
				    channel->line);
	      
	      xmlNewProp(pattern_node,
			 "nth-line",
			 str);

	      g_free(str);

	      str = g_strdup_printf("%d",
				    i);
	      
	      xmlNewProp(pattern_node,
			 "bank-0",
			 str);

	      str = g_strdup_printf("%d",
				    j);
	      
	      xmlNewProp(pattern_node,
			 "bank-1",
			 str);

	      g_free(str);
	      
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
  if(machine->enabled_automation_port != NULL){
    ags_simple_file_write_automation_port(simple_file,
					  node,
					  machine->enabled_automation_port);
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

  if(machine->audio->preset != NULL){
    ags_simple_file_write_preset_list(simple_file,
				      node,
				      machine->audio->preset);
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
		    "ags-sf-pad-list");

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
  gchar *str;
  
  gboolean found_content;

  found_content = FALSE;
  
  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-sf-pad");
  
  xmlNewProp(node,
	     (xmlChar *) AGS_SIMPLE_FILE_ID_PROP,
	     (xmlChar *) id);

  str = g_strdup_printf("%d",
			pad->channel->pad);
  
  xmlNewProp(node,
	     "nth-pad",
	     str);

  g_free(str);
  
  /* group */
  if(!gtk_toggle_button_get_active(pad->group)){
    xmlNewProp(node,
	       "group",
	       "false");
    found_content = TRUE;
  }else{
    xmlNewProp(node,
	       "group",
	       "true");
  }

  /* mute */
  if(!gtk_toggle_button_get_active(pad->mute)){
    xmlNewProp(node,
	       "mute",
	       "false");
  }else{
    xmlNewProp(node,
	       "mute",
	       "true");
    found_content = TRUE;
  }

  /* solo */
  if(!gtk_toggle_button_get_active(pad->solo)){
    xmlNewProp(node,
	       "solo",
	       "false");
  }else{
    xmlNewProp(node,
	       "solo",
	       "true");
    found_content = TRUE;
  }

  /* children */
  if(pad->line != NULL){
    list = ags_pad_get_line(pad);
    
    if(ags_simple_file_write_line_list(simple_file,
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
ags_simple_file_write_line_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *line)
{
  xmlNode *node;

  gboolean found_content;
  
  node = xmlNewNode(NULL,
		    "ags-sf-line-list");
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

void
ags_simple_file_write_line_member_control(AgsSimpleFile *simple_file, xmlNode *parent, AgsLineMember *line_member)
{
  GtkWidget *child_widget;
    
  xmlNode *control_node;

  gchar *str;
    
  /* control node */
  child_widget = ags_line_member_get_widget(line_member);

  if(GTK_IS_TOGGLE_BUTTON(child_widget)){
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");

    xmlNewProp(control_node,
	       "control-type",
	       G_OBJECT_TYPE_NAME(child_widget));
    
    xmlNewProp(control_node,
	       "value",
	       ((gtk_toggle_button_get_active((GtkToggleButton *) child_widget)) ? "true": "false"));
  }else if(GTK_IS_CHECK_BUTTON(child_widget)){
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");

    xmlNewProp(control_node,
	       "control-type",
	       G_OBJECT_TYPE_NAME(child_widget));
    
    xmlNewProp(control_node,
	       "value",
	       ((gtk_check_button_get_active((GtkCheckButton *) child_widget)) ? "true": "false"));
  }else if(AGS_IS_DIAL(child_widget)){
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");

    xmlNewProp(control_node,
	       "control-type",
	       G_OBJECT_TYPE_NAME(child_widget));

    str = g_strdup_printf("%lf",
			  gtk_adjustment_get_value(AGS_DIAL(child_widget)->adjustment));
      
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
  }else if(GTK_IS_RANGE(child_widget)){
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");

    xmlNewProp(control_node,
	       "control-type",
	       G_OBJECT_TYPE_NAME(child_widget));

    str = g_strdup_printf("%lf",
			  gtk_range_get_value(GTK_RANGE(child_widget)));
      
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
  }else if(GTK_IS_SPIN_BUTTON(child_widget)){
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");

    xmlNewProp(control_node,
	       "control-type",
	       G_OBJECT_TYPE_NAME(child_widget));

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(GTK_SPIN_BUTTON(child_widget)));
      
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
  }else{
    g_warning("ags_file_write_effect_list() - unknown child of AgsLineMember type");

    return;
  }

  xmlNewProp(control_node,
	     "specifier",
	     line_member->specifier);

  xmlAddChild(parent,
	      control_node);
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
  gchar *str;
  
  gboolean found_content;
  
  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-sf-line");
  
  xmlNewProp(node,
	     (xmlChar *) AGS_SIMPLE_FILE_ID_PROP,
	     (xmlChar *) id);

  found_content = FALSE;

  if(line->channel->link != NULL){
    ags_simple_file_add_id_ref(simple_file,
			       g_object_new(AGS_TYPE_FILE_ID_REF,
					    "file", simple_file,
					    "node", node,
					    "reference", line->channel,
					    NULL));

    found_content = TRUE;
  }

  str = g_strdup_printf("%d",
			line->channel->line);
  
  xmlNewProp(node,
	     "nth-line",
	     str);

  g_free(str);
  
  /* device */
  if(line->channel != NULL &&
     line->channel->output_soundcard != NULL){
    gchar *device;

    device = ags_soundcard_get_device(AGS_SOUNDCARD(line->channel->output_soundcard));
    
    if(device != NULL){
      xmlNewProp(node,
		 (xmlChar *) "soundcard-device",
		 (xmlChar *) device);

      g_free(device);
    }
  }

  /* group */
  if(!gtk_toggle_button_get_active(line->group)){
    xmlNewProp(node,
	       "is-grouped",
	       "false");

    found_content = TRUE;
  }else{
    xmlNewProp(node,
	       "is-grouped",
	       "true");
  }

  /* link or file */
  if(line->channel->link != NULL){
    AgsFileLookup *file_lookup;

    file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
						 "file", simple_file,
						 "node", node,
						 "reference", line,
						 NULL);
    ags_simple_file_add_lookup(simple_file, (GObject *) file_lookup);
    g_signal_connect(G_OBJECT(file_lookup), "resolve",
		     G_CALLBACK(ags_simple_file_write_line_resolve_link), line->channel);
  }else if(AGS_IS_INPUT(line->channel)){
    AgsFileLink *file_link;

    g_object_get(line->channel,
		 "file-link", &file_link,
		 NULL);
    
    if(file_link != NULL){
      gchar *filename;

      guint audio_channel;

      g_object_get(file_link,
		   "filename", &filename,
		   "audio-channel", &audio_channel,
		   NULL);
      
      if(ags_audio_file_check_suffix(filename)){
	str = g_strdup_printf("file://%s",
			      filename);
    
	xmlNewProp(node,
		   "filename",
		   str);

	g_free(str);

	str = g_strdup_printf("%d",
			      audio_channel);
    
	xmlNewProp(node,
		   "file-channel",
		   str);

	g_free(str);
    
	found_content = TRUE;
      }

      g_free(filename);
    }
  }
  
  /* machine specific */
  if(AGS_IS_SYNTH_INPUT_LINE(line)){
    ags_simple_file_write_oscillator(simple_file,
				     node,
				     AGS_SYNTH_INPUT_LINE(line)->oscillator);
    found_content = TRUE;
  }else if(AGS_IS_FM_SYNTH_INPUT_LINE(line)){
    ags_simple_file_write_fm_oscillator(simple_file,
					node,
					AGS_FM_SYNTH_INPUT_LINE(line)->fm_oscillator);
    found_content = TRUE;
  }
  
  /* effect list */
  effect_list_node = NULL;
  effect_node = NULL;
    
  filename = NULL;
  effect = NULL;

  list_start = 
    list = ags_line_get_line_member(line);
  
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
				 "ags-sf-effect");

	filename = g_list_prepend(filename,
				  AGS_LINE_MEMBER(list->data)->filename);

	effect = g_list_prepend(effect,
				AGS_LINE_MEMBER(list->data)->effect);

	xmlNewProp(effect_node,
		   "filename",
		   AGS_LINE_MEMBER(list->data)->filename);

	xmlNewProp(effect_node,
		   "effect",
		   AGS_LINE_MEMBER(list->data)->effect);
	
	ags_simple_file_write_line_member_control(simple_file, effect_node, list->data);
	  
	/* add to parent */
	xmlAddChild(effect_list_node,
		    effect_node);
      }else{
	ags_simple_file_write_line_member_control(simple_file, effect_node, list->data);
      }

      found_content = TRUE;
    }
    
    list = list->next;
  }

  g_list_free(list_start);

  g_list_free(filename);
  g_list_free(effect);
  
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
  xmlChar *str;

  list = ags_simple_file_find_id_ref_by_reference((AgsSimpleFile *) file_lookup->file,
						  channel->link);
  
  if(list != NULL){
    str = xmlGetProp(AGS_FILE_ID_REF(list->data)->node,
		     "id");
    
    xpath = g_strdup_printf("xpath=//ags-sf-line[@id='%s']",
			    str);
    xmlNewProp(file_lookup->node,
	       "link",
	       xpath);

    g_free(xpath);

    if(str != NULL){
      xmlFree(str);
    }
  }
}

xmlNode*
ags_simple_file_write_effect_pad_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *effect_pad)
{
  xmlNode *node;

  gboolean found_content;

  found_content = FALSE;
  
  node = xmlNewNode(NULL,
		    "ags-sf-effect-pad-list");

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
		    "ags-sf-effect-pad");
  
  xmlNewProp(node,
	     (xmlChar *) AGS_SIMPLE_FILE_ID_PROP,
	     (xmlChar *) id);


  /* children */
  if(effect_pad->effect_line != NULL){
    list = ags_effect_pad_get_effect_line(effect_pad);

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
		    "ags-sf-effect-line-list");

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

void
ags_simple_file_write_effect_line_control(AgsSimpleFile *simple_file, xmlNode *parent, AgsLineMember *line_member)
{
  GtkWidget *child_widget;
    
  xmlNode *control_node;

  gchar *str;
    
  /* control node */
  child_widget = ags_line_member_get_widget(line_member);
    
  if(GTK_IS_TOGGLE_BUTTON(child_widget)){
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");

    xmlNewProp(control_node,
	       "value",
	       ((gtk_toggle_button_get_active((GtkToggleButton *) child_widget)) ? "true": "false"));
  }else if(GTK_IS_CHECK_BUTTON(child_widget)){
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");

    xmlNewProp(control_node,
	       "value",
	       ((gtk_check_button_get_active((GtkCheckButton *) child_widget)) ? "true": "false"));
  }else if(AGS_IS_DIAL(child_widget)){
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");

    str = g_strdup_printf("%lf",
			  gtk_adjustment_get_value(AGS_DIAL(child_widget)->adjustment));
      
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
  }else if(GTK_IS_RANGE(child_widget)){
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");

    str = g_strdup_printf("%lf",
			  gtk_range_get_value(GTK_RANGE(child_widget)));
      
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
  }else if(GTK_IS_SPIN_BUTTON(child_widget)){
    control_node = xmlNewNode(NULL,
			      "ags-sf-control");

    str = g_strdup_printf("%lf",
			  gtk_spin_button_get_value(GTK_SPIN_BUTTON(child_widget)));
      
    xmlNewProp(control_node,
	       "value",
	       str);

    g_free(str);
  }else{
    g_warning("ags_file_write_effect_list() - unknown child of AgsLineMember type");

    return;
  }

  xmlNewProp(control_node,
	     "specifier",
	     line_member->specifier);

  xmlAddChild(parent,
	      control_node);
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
  
  found_content = FALSE;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-sf-effect-line");
  
  xmlNewProp(node,
	     (xmlChar *) AGS_SIMPLE_FILE_ID_PROP,
	     (xmlChar *) id);

  /* effect list */
  effect_list_node = NULL;
  effect_node = NULL;
    
  filename = NULL;
  effect = NULL;

  list_start = 
    list = ags_effect_line_get_line_member(effect_line);

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
				 "ags-sf-effect");

	filename = g_list_prepend(filename,
				  AGS_LINE_MEMBER(list->data)->filename);

	effect = g_list_prepend(effect,
				AGS_LINE_MEMBER(list->data)->effect);
	  
	ags_simple_file_write_effect_line_control(simple_file, effect_node, list->data);
	  
	/* add to parent */
	xmlAddChild(effect_list_node,
		    effect_node);
      }else{
	ags_simple_file_write_effect_line_control(simple_file, effect_node, list->data);
      }
      
      found_content = TRUE;
    }
    
    list = list->next;
  }
  
  g_list_free(list_start);

  g_list_free(filename);
  g_list_free(effect);
  
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
ags_simple_file_write_oscillator_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *oscillator)
{
  xmlNode *node;
    
  node = xmlNewNode(NULL,
		    "ags-oscillator-list");

  while(oscillator != NULL){
    ags_simple_file_write_oscillator(simple_file,
				     node,
				     oscillator->data);
       
    oscillator = oscillator->next;
  }

  xmlAddChild(parent,
	      node);

  return(node);
}

xmlNode*
ags_simple_file_write_oscillator(AgsSimpleFile *simple_file, xmlNode *parent, AgsOscillator *oscillator)
{
  xmlNode *node;

  xmlChar *str, *tmp;

  guint i;
  
  node = xmlNewNode(NULL,
		    "ags-oscillator");

  str = g_strdup_printf("%d",
			gtk_combo_box_get_active(oscillator->wave));
  
  xmlNewProp(node,
	     "wave",
	     str);

  g_free(str);

  str = g_strdup_printf("%lf",
			gtk_spin_button_get_value(oscillator->attack));
  
  xmlNewProp(node,
	     "attack",
	     str);

  g_free(str);

  str = g_strdup_printf("%lf",
			gtk_spin_button_get_value(oscillator->frame_count));
  
  xmlNewProp(node,
	     "length",
	     str);

  g_free(str);

  str = g_strdup_printf("%lf",
			gtk_spin_button_get_value(oscillator->frequency));
  
  xmlNewProp(node,
	     "frequency",
	     str);

  g_free(str);

  str = g_strdup_printf("%lf",
			gtk_spin_button_get_value(oscillator->phase));
  
  xmlNewProp(node,
	     "phase",
	     str);

  g_free(str);

  str = g_strdup_printf("%lf",
			gtk_spin_button_get_value(oscillator->volume));
  
  xmlNewProp(node,
	     "volume",
	     str);

  g_free(str);

  xmlNewProp(node,
	     "sync",
	     (gtk_check_button_get_active((GtkCheckButton *) oscillator->do_sync) ? "true": "false"));

  str = NULL;

  for(i = 0; i < 2 * oscillator->sync_point_count; i++){
    tmp = str;

    if(str != NULL){
      str = g_strdup_printf("%s %lf",
			    str,
			    gtk_spin_button_get_value(oscillator->sync_point[i]));
    }else{
      str = g_strdup_printf("%lf",
			    gtk_spin_button_get_value(oscillator->sync_point[2 * i]));
    }
    
    g_free(tmp);
  }

  xmlNewProp(node,
	     "sync-point",
	     str);

  g_free(str);
  
  xmlAddChild(parent,
	      node);

  return(node);
}

xmlNode*
ags_simple_file_write_fm_oscillator_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *fm_oscillator)
{
  xmlNode *node;
    
  node = xmlNewNode(NULL,
		    "ags-fm-oscillator-list");

  while(fm_oscillator != NULL){
    ags_simple_file_write_fm_oscillator(simple_file,
					node,
					fm_oscillator->data);
       
    fm_oscillator = fm_oscillator->next;
  }

  xmlAddChild(parent,
	      node);

  return(node);
}

xmlNode*
ags_simple_file_write_fm_oscillator(AgsSimpleFile *simple_file, xmlNode *parent, AgsFMOscillator *fm_oscillator)
{
  xmlNode *node;

  xmlChar *str, *tmp;

  guint i;
  
  node = xmlNewNode(NULL,
		    "ags-fm-oscillator");

  str = g_strdup_printf("%d",
			gtk_combo_box_get_active(fm_oscillator->wave));
  
  xmlNewProp(node,
	     "wave",
	     str);

  g_free(str);

  str = g_strdup_printf("%lf",
			gtk_spin_button_get_value(fm_oscillator->attack));
  
  xmlNewProp(node,
	     "attack",
	     str);

  g_free(str);

  str = g_strdup_printf("%lf",
			gtk_spin_button_get_value(fm_oscillator->frame_count));

  xmlNewProp(node,
	     "length",
	     str);

  g_free(str);

  str = g_strdup_printf("%lf",
			gtk_spin_button_get_value(fm_oscillator->frequency));

  xmlNewProp(node,
	     "frequency",
	     str);

  g_free(str);

  str = g_strdup_printf("%lf",
			gtk_spin_button_get_value(fm_oscillator->phase));

  xmlNewProp(node,
	     "phase",
	     str);

  g_free(str);

  str = g_strdup_printf("%lf",
			gtk_spin_button_get_value(fm_oscillator->volume));

  xmlNewProp(node,
	     "volume",
	     str);

  g_free(str);

  xmlNewProp(node,
	     "sync",
	     (gtk_check_button_get_active((GtkCheckButton *) fm_oscillator->do_sync) ? "true": "false"));

  str = NULL;

  for(i = 0; i < 2 * fm_oscillator->sync_point_count; i++){
    tmp = str;

    if(str != NULL){
      str = g_strdup_printf("%s %lf",
			    str,
			    gtk_spin_button_get_value(fm_oscillator->sync_point[i]));
    }else{
      str = g_strdup_printf("%lf",
			    gtk_spin_button_get_value(fm_oscillator->sync_point[2 * i]));
    }
    
    g_free(tmp);
  }
  
  xmlNewProp(node,
	     "sync-point",
	     str);

  g_free(str);

  str = g_strdup_printf("%d", gtk_combo_box_get_active(fm_oscillator->fm_lfo_wave));
  
  xmlNewProp(node,
	     "fm-lfo-wave",
	     str);

  g_free(str);

  str = g_strdup_printf("%lf", gtk_spin_button_get_value(fm_oscillator->fm_lfo_frequency));

  xmlNewProp(node,
	     "fm-lfo-frequency",
	     str);

  g_free(str);

  str = g_strdup_printf("%lf", gtk_spin_button_get_value(fm_oscillator->fm_lfo_depth));

  xmlNewProp(node,
	     "fm-lfo-depth",
	     str);

  g_free(str);

  str = g_strdup_printf("%lf", gtk_spin_button_get_value(fm_oscillator->fm_tuning));

  xmlNewProp(node,
	     "fm-tuning",
	     str);

  g_free(str);
  
  xmlAddChild(parent,
	      node);

  return(node);
}

xmlNode*
ags_simple_file_write_composite_editor(AgsSimpleFile *simple_file, xmlNode *parent, AgsCompositeEditor *composite_editor)
{
  AgsFileLookup *file_lookup;

  xmlNode *node;
  
  node = xmlNewNode(NULL,
		    "ags-sf-composite-editor");

  xmlNewProp(node,
	     "zoom",
	     gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(composite_editor->toolbar->zoom)));

  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file", simple_file,
					       "node", node,
					       "reference", composite_editor,
					       NULL);
  ags_simple_file_add_lookup(simple_file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve",
		   G_CALLBACK(ags_simple_file_write_composite_editor_resolve_machine), composite_editor);
  
  /* add to parent */
  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_simple_file_write_composite_editor_resolve_machine(AgsFileLookup *file_lookup,
						       AgsCompositeEditor *composite_editor)
{
  xmlNode *node;
  xmlNode *property_list;
  xmlNode *property;
  
  GList *start_list, *list;
  GList *file_id_ref;

  xmlChar *xpath;
  xmlChar *str;
  
  node = file_lookup->node;
  property_list = NULL;

  list =   
    start_list = ags_machine_selector_get_machine_radio_button(composite_editor->machine_selector);

  if(list != NULL){
    property_list = xmlNewNode(NULL,
			       "ags-sf-property-list");
  }
  
  while(list != NULL){    
    if(AGS_MACHINE_RADIO_BUTTON(list->data)->machine != NULL){
      property = xmlNewNode(NULL,
			    "ags-sf-property");
      
      xmlNewProp(property,
		 "name",
		 "machine");

      file_id_ref = ags_simple_file_find_id_ref_by_reference((AgsSimpleFile *) file_lookup->file,
							     AGS_MACHINE_RADIO_BUTTON(list->data)->machine);
      if(file_id_ref != NULL){
	str = xmlGetProp(AGS_FILE_ID_REF(file_id_ref->data)->node,
			 "id");
	
	xpath = g_strdup_printf("xpath=//ags-sf-machine[@id='%s']",
				str);

	if(str != NULL){
	  xmlFree(str);
	}
      }else{
	xpath = g_strdup("(null)");
      }
      
      xmlNewProp(property,
		 "value",
		 xpath);

      /* add to parent */
      xmlAddChild(property_list,
		  property);
    }else{
      property = xmlNewNode(NULL,
			    "ags-sf-property");
      
      xmlNewProp(property,
		 "name",
		 "machine");

      xmlNewProp(property,
		 "value",
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
  
  g_list_free(start_list);
}

xmlNode*
ags_simple_file_write_notation_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *notation)
{
  xmlNode *node;
  
  node = xmlNewNode(NULL,
		    "ags-sf-notation-list");

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

  gchar *str;
  
  node = xmlNewNode(NULL,
		    "ags-sf-notation");

  xmlNewProp(node,
	     "channel",
	     g_strdup_printf("%d", notation->audio_channel));

  /* timestamp */
  child = xmlNewNode(NULL,
		     "ags-sf-timestamp");

  str = g_strdup_printf("%lu",
			notation->timestamp->timer.ags_offset.offset);
  
  xmlNewProp(child,
	     "offset",
	     str);

  g_free(str);
  
  /* add to parent */
  xmlAddChild(node,
	      child);

  /* note */
  list = notation->note;

  while(list != NULL){
    child = xmlNewNode(NULL,
		       "ags-sf-note");

    str = g_strdup_printf("%d",
			  AGS_NOTE(list->data)->x[0]);
    
    xmlNewProp(child,
	       "x0",
	       str);

    g_free(str);

    str = g_strdup_printf("%d",
			  AGS_NOTE(list->data)->x[1]);
    
    xmlNewProp(child,
	       "x1",
	       str);

    g_free(str);

    str = g_strdup_printf("%d",
			  AGS_NOTE(list->data)->y);
    
    xmlNewProp(child,
	       "y",
	       str);

    g_free(str);

    if((AGS_NOTE_ENVELOPE & (AGS_NOTE(list->data)->flags)) != 0){
      xmlNewProp(child,
		 "envelope",
		 "true");
    }

    str = g_strdup_printf("%lf %lf",
			  AGS_NOTE(list->data)->attack.real,
			  AGS_NOTE(list->data)->attack.imag);
    
    xmlNewProp(child,
	       "attack",
	       str);

    g_free(str);
    
    str = g_strdup_printf("%lf %lf",
			  AGS_NOTE(list->data)->decay.real,
			  AGS_NOTE(list->data)->decay.imag);
    
    xmlNewProp(child,
	       "decay",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf %lf",
			  AGS_NOTE(list->data)->sustain.real,
			  AGS_NOTE(list->data)->sustain.imag);
    
    xmlNewProp(child,
	       "sustain",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf %lf",
			  AGS_NOTE(list->data)->release.real,
			  AGS_NOTE(list->data)->release.imag);
    
    xmlNewProp(child,
	       "release",
	       str);

    g_free(str);

    str = g_strdup_printf("%lf %lf",
			  AGS_NOTE(list->data)->ratio.real,
			  AGS_NOTE(list->data)->ratio.imag);
    
    xmlNewProp(child,
	       "ratio",
	       str);

    g_free(str);
    
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
  
  node = xmlNewNode(NULL,
		    "ags-sf-automation-list");

  while(automation != NULL){
    ags_simple_file_write_automation(simple_file,
				     node,
				     automation->data);

    automation = automation->next;
  }

  /* add to parent */
  xmlAddChild(parent,
	      node);

  return(node);
}

xmlNode*
ags_simple_file_write_automation(AgsSimpleFile *simple_file, xmlNode *parent, AgsAutomation *automation)
{
  xmlNode *node;
  xmlNode *child;

  GList *list;

  gchar *str;
  
  node = xmlNewNode(NULL,
		    "ags-sf-automation");

  str = g_strdup_printf("%d", automation->line);
  xmlNewProp(node,
	     "line",
	     str);

  g_free(str);

  str = g_strdup_printf("%s", g_type_name(automation->channel_type));
  xmlNewProp(node,
	     "channel-type",
	     str);

  g_free(str);

  str = g_strdup_printf("%s", automation->control_name);
  xmlNewProp(node,
	     "control-name",
	     str);

  g_free(str);

  /* timestamp */
  child = xmlNewNode(NULL,
		     "ags-sf-timestamp");

  xmlNewProp(child,
	     "offset",
	     g_strdup_printf("%lu",
			     automation->timestamp->timer.ags_offset.offset));
    
  /* add to parent */
  xmlAddChild(node,
	      child);

  /* acceleration */
  list = automation->acceleration;

  while(list != NULL){
    child = xmlNewNode(NULL,
		       "ags-sf-acceleration");

    str = g_strdup_printf("%d",
			  AGS_ACCELERATION(list->data)->x);
    
    xmlNewProp(child,
	       "x",
	       str);

    g_free(str);
    
    str = g_strdup_printf("%lf",
			  AGS_ACCELERATION(list->data)->y);
    
    xmlNewProp(child,
	       "y",
	       str);

    g_free(str);

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
ags_simple_file_write_preset_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *preset)
{
  xmlNode *node;

  gboolean found_node;
  
  node = xmlNewNode(NULL,
		    "ags-sf-preset-list");

  found_node = FALSE;
  
  while(preset != NULL){
    if(ags_simple_file_write_preset(simple_file,
				    node,
				    preset->data) != NULL){
      found_node = TRUE;
    }

    preset = preset->next;
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
ags_simple_file_write_preset(AgsSimpleFile *simple_file, xmlNode *parent, AgsPreset *preset)
{
  xmlNode *node;
  xmlNode *child;

  gchar *str;
  
  guint i;
  
  node = xmlNewNode(NULL,
		    "ags-sf-preset");

  /* scope and preset name */
  xmlNewProp(node,
	     "scope",
	     g_strdup(preset->scope));

  xmlNewProp(node,
	     "preset-name",
	     g_strdup(preset->preset_name));

  /* mapping */
  str = g_strdup_printf("%d", preset->audio_channel_start);
  
  xmlNewProp(node,
	     "audio-channel-start",
	     str);

  g_free(str);
  
  str = g_strdup_printf("%d", preset->audio_channel_end);
  
  xmlNewProp(node,
	     "audio-channel-end",
	     str);

  g_free(str);

  str = g_strdup_printf("%d", preset->pad_start);
  
  xmlNewProp(node,
	     "pad-start",
	     str);

  g_free(str);

  str = g_strdup_printf("%d", preset->pad_end);
  
  xmlNewProp(node,
	     "pad-end",
	     str);

  g_free(str);

  str = g_strdup_printf("%d", preset->x_start);
  
  xmlNewProp(node,
	     "x-start",
	     str);

  g_free(str);

  str = g_strdup_printf("%d", preset->x_end);
  
  xmlNewProp(node,
	     "x-end",
	     str);

  g_free(str);

  /* parameter */
  ags_simple_file_write_strv(simple_file, node, preset->parameter_name);
  
  for(i = 0; i < preset->n_params; i++){
    ags_simple_file_write_value(simple_file, node, &(preset->value[i]));
  }
  
  if(preset->n_params > 0){
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
ags_simple_file_write_program_list(AgsSimpleFile *simple_file, xmlNode *parent, GList *program)
{
  xmlNode *node;
  
  node = xmlNewNode(NULL,
		    "ags-sf-program-list");

  while(program != NULL){
    ags_simple_file_write_program(simple_file,
				     node,
				     program->data);

    program = program->next;
  }

  /* add to parent */
  xmlAddChild(parent,
	      node);

  return(node);
}

xmlNode*
ags_simple_file_write_program(AgsSimpleFile *simple_file, xmlNode *parent, AgsProgram *program)
{
  xmlNode *node;
  xmlNode *child;

  GList *list;

  gchar *str;
  
  node = xmlNewNode(NULL,
		    "ags-sf-program");

  xmlNewProp(node,
	     "control-name",
	     g_strdup(program->control_name));

  /* timestamp */
  child = xmlNewNode(NULL,
		     "ags-sf-timestamp");

  xmlNewProp(child,
	     "offset",
	     g_strdup_printf("%lu",
			     program->timestamp->timer.ags_offset.offset));
    
  /* add to parent */
  xmlAddChild(node,
	      child);

  /* marker */
  list = program->marker;

  while(list != NULL){
    child = xmlNewNode(NULL,
		       "ags-sf-marker");

    str = g_strdup_printf("%d",
			  AGS_MARKER(list->data)->x);
    
    xmlNewProp(child,
	       "x",
	       str);

    g_free(str);
    
    str = g_strdup_printf("%lf",
			  AGS_MARKER(list->data)->y);
    
    xmlNewProp(child,
	       "y",
	       str);

    g_free(str);

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

AgsSimpleFile*
ags_simple_file_new()
{
  AgsSimpleFile *simple_file;

  simple_file = (AgsSimpleFile *) g_object_new(AGS_TYPE_SIMPLE_FILE,
					       NULL);

  return(simple_file);
}
