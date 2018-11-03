/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/task/ags_export_output.h>

#include <ags/audio/file/ags_audio_file.h>
#include <ags/audio/file/ags_sndfile.h>

#include <sndfile.h>

#include <ags/i18n.h>

void ags_export_output_class_init(AgsExportOutputClass *export_output);
void ags_export_output_init(AgsExportOutput *export_output);
void ags_export_output_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_export_output_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_export_output_dispose(GObject *gobject);
void ags_export_output_finalize(GObject *gobject);

void ags_export_output_launch(AgsTask *task);

/**
 * SECTION:ags_export_output
 * @short_description: export output task
 * @title: AgsExportOutput
 * @section_id:
 * @include: ags/audio/task/ags_export_output.h
 *
 * The #AgsExportOutput task exports #GObject to file.
 */

static gpointer ags_export_output_parent_class = NULL;

enum{
  PROP_0,
  PROP_EXPORT_THREAD,
  PROP_SOUNDCARD,
  PROP_FILENAME,
  PROP_FORMAT,
  PROP_TIC,
  PROP_LIVE_PERFORMANCE,
};

GType
ags_export_output_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_export_output = 0;

    static const GTypeInfo ags_export_output_info = {
      sizeof(AgsExportOutputClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_export_output_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsExportOutput),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_export_output_init,
    };

    ags_type_export_output = g_type_register_static(AGS_TYPE_TASK,
						    "AgsExportOutput",
						    &ags_export_output_info,
						    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_export_output);
  }

  return g_define_type_id__volatile;
}

void
ags_export_output_class_init(AgsExportOutputClass *export_output)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_export_output_parent_class = g_type_class_peek_parent(export_output);

  /* gobject */
  gobject = (GObjectClass *) export_output;

  gobject->set_property = ags_export_output_set_property;
  gobject->get_property = ags_export_output_get_property;

  gobject->dispose = ags_export_output_dispose;
  gobject->finalize = ags_export_output_finalize;

  /* properties */
  /**
   * AgsExportOutput:export-thread:
   *
   * The assigned #AgsExportThread
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("export-thread",
				   i18n_pspec("export thread of export output"),
				   i18n_pspec("The export thread of export output task"),
				   AGS_TYPE_EXPORT_THREAD,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EXPORT_THREAD,
				  param_spec);

  /**
   * AgsExportOutput:soundcard:
   *
   * The assigned #AgsSoundcard
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("soundcard of export output"),
				   i18n_pspec("The soundcard of export output task"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsExportOutput:filename:
   *
   * The assigned filename.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("the filename"),
				   i18n_pspec("The filename containing the output"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsExportOutput:format:
   *
   * Format to use.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("audio format"),
				 i18n_pspec("The audio format to use"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsExportOutput:tic:
   *
   * Tic offset of output as end tic of it.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("tic",
				 i18n_pspec("tic offset"),
				 i18n_pspec("The tic offset"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIC,
				  param_spec);

  /**
   * AgsExportOutput:live-performance:
   *
   * Do output the audio export live.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_boolean("live-performance",
				     i18n_pspec("export output live"),
				     i18n_pspec("Do export output live"),
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LIVE_PERFORMANCE,
				  param_spec);
  
  /* task */
  task = (AgsTaskClass *) export_output;

  task->launch = ags_export_output_launch;
}

void
ags_export_output_init(AgsExportOutput *export_output)
{
  export_output->export_thread = NULL;
  export_output->soundcard = NULL;

  export_output->filename = NULL;
  export_output->format = 0;

  export_output->tic = 0;
  export_output->live_performance = TRUE;
}

void
ags_export_output_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsExportOutput *export_output;

  export_output = AGS_EXPORT_OUTPUT(gobject);

  switch(prop_id){
  case PROP_EXPORT_THREAD:
    {
      AgsExportThread *export_thread;

      export_thread = (AgsExportThread *) g_value_get_object(value);

      if(export_output->export_thread == (GObject *) export_thread){
	return;
      }

      if(export_output->export_thread != NULL){
	g_object_unref(export_output->export_thread);
      }

      if(export_thread != NULL){
	g_object_ref(export_thread);
      }

      export_output->export_thread = (GObject *) export_thread;
    }
    break;
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      if(export_output->soundcard == (GObject *) soundcard){
	return;
      }

      if(export_output->soundcard != NULL){
	g_object_unref(export_output->soundcard);
      }

      if(soundcard != NULL){
	g_object_ref(soundcard);
      }

      export_output->soundcard = (GObject *) soundcard;
    }
    break;
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(export_output->filename == filename){
	return;
      }

      if(export_output->filename != NULL){
        g_free(export_output->filename);
      }

      export_output->filename = g_strdup(filename);
    }
    break;
  case PROP_FORMAT:
    {
      export_output->format = g_value_get_uint(value);
    }
    break;
  case PROP_TIC:
    {
      export_output->tic = g_value_get_uint(value);
    }
    break;
  case PROP_LIVE_PERFORMANCE:
    {
      export_output->live_performance = g_value_get_boolean(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_export_output_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsExportOutput *export_output;

  export_output = AGS_EXPORT_OUTPUT(gobject);

  switch(prop_id){
  case PROP_EXPORT_THREAD:
    {
      g_value_set_object(value, export_output->export_thread);
    }
    break;
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, export_output->soundcard);
    }
    break;
  case PROP_FILENAME:
    {
      g_value_set_string(value, export_output->filename);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value, export_output->format);
    }
  case PROP_TIC:
    {
      g_value_set_uint(value, export_output->tic);
    }
    break;
  case PROP_LIVE_PERFORMANCE:
    {
      g_value_set_boolean(value, export_output->live_performance);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_export_output_dispose(GObject *gobject)
{
  AgsExportOutput *export_output;

  export_output = AGS_EXPORT_OUTPUT(gobject);

  if(export_output->export_thread != NULL){
    g_object_unref(export_output->export_thread);

    export_output->export_thread = NULL;
  }

  if(export_output->soundcard != NULL){
    g_object_unref(export_output->soundcard);

    export_output->soundcard = NULL;
  }

  if(export_output->filename != NULL){
    g_free(export_output->filename);

    export_output->filename = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_export_output_parent_class)->dispose(gobject);
}

void
ags_export_output_finalize(GObject *gobject)
{
  AgsExportOutput *export_output;

  export_output = AGS_EXPORT_OUTPUT(gobject);

  if(export_output->export_thread != NULL){
    g_object_unref(export_output->export_thread);
  }

  if(export_output->soundcard != NULL){
    g_object_unref(export_output->soundcard);
  }

  if(export_output->filename != NULL){
    g_free(export_output->filename);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_export_output_parent_class)->finalize(gobject);
}

void
ags_export_output_launch(AgsTask *task)
{
  AgsAudioFile *audio_file;

  AgsExportOutput *export_output;

  AgsExportThread *export_thread;

  GObject *soundcard;

  gchar *filename;

  guint pcm_channels;
  guint samplerate;
  guint format;
  guint tic;
  guint val;
  
  export_output = AGS_EXPORT_OUTPUT(task);
  
  soundcard = export_output->soundcard;
  
  export_thread = export_output->export_thread;
  
  filename = export_output->filename;
  
  tic = export_output->tic;

  /* get presets */
  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    &pcm_channels,
			    &samplerate,
			    NULL,
			    &format);

  /* open read/write audio file */
  audio_file = ags_audio_file_new(filename,
				  soundcard,
				  -1);

#ifdef AGS_DEBUG
  g_message("pcm - %d", pcm_channels);
#endif
  
  audio_file->file_audio_channels = pcm_channels;
  audio_file->file_samplerate = (int) samplerate;
  
  ags_audio_file_rw_open(audio_file,
			 TRUE);
  //TODO:JK: more formats

  g_message("export output");
#ifdef AGS_DEBUG
#endif

  /* start export thread */
  g_object_set(G_OBJECT(export_thread),
	       "audio-file", audio_file,
	       "tic", tic,
	       NULL);
}

/**
 * ags_export_output_new:
 * @export_thread: the #AgsExportThread to start
 * @soundcard: the #GObject to export
 * @filename: the filename to save
 * @tic: stream duration in tact
 * @live_performance: if %TRUE export is done during real-time
 *
 * Creates an #AgsExportOutput.
 *
 * Returns: an new #AgsExportOutput.
 *
 * Since: 2.0.0
 */
AgsExportOutput*
ags_export_output_new(AgsExportThread *export_thread,
		      GObject *soundcard,
		      gchar *filename,
		      guint tic,
		      gboolean live_performance)
{
  AgsExportOutput *export_output;

  export_output = (AgsExportOutput *) g_object_new(AGS_TYPE_EXPORT_OUTPUT,
						   "export-thread", export_thread,
						   "soundcard", soundcard,
						   "filename", filename,
						   "tic", tic,
						   "live-performance", live_performance,
						   NULL);

  return(export_output);
}
