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

#include <ags/audio/task/ags_open_wave.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_wave.h>
#include <ags/audio/ags_buffer.h>

#include <ags/audio/file/ags_sound_resource.h>

#include <ags/config.h>

#include <ags/i18n.h>

void ags_open_wave_class_init(AgsOpenWaveClass *open_wave);
void ags_open_wave_init(AgsOpenWave *open_wave);
void ags_open_wave_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_open_wave_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_open_wave_dispose(GObject *gobject);
void ags_open_wave_finalize(GObject *gobject);

void ags_open_wave_launch(AgsTask *task);

/**
 * SECTION:ags_open_wave
 * @short_description: open wave
 * @title: AgsOpenWave
 * @section_id:
 * @include: ags/audio/task/ags_open_wave.h
 *
 * The #AgsOpenWave task opens audio files as wave.
 */

static gpointer ags_open_wave_parent_class = NULL;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_AUDIO_FILE,
  PROP_FILENAME,
  PROP_START_PAD,
};

GType
ags_open_wave_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_open_wave = 0;

    static const GTypeInfo ags_open_wave_info = {
      sizeof(AgsOpenWaveClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_open_wave_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsOpenWave),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_open_wave_init,
    };

    ags_type_open_wave = g_type_register_static(AGS_TYPE_TASK,
						"AgsOpenWave",
						&ags_open_wave_info,
						0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_open_wave);
  }

  return g_define_type_id__volatile;
}

void
ags_open_wave_class_init(AgsOpenWaveClass *open_wave)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;
  
  ags_open_wave_parent_class = g_type_class_peek_parent(open_wave);

  /* GObject */
  gobject = (GObjectClass *) open_wave;

  gobject->set_property = ags_open_wave_set_property;
  gobject->get_property = ags_open_wave_get_property;

  gobject->dispose = ags_open_wave_dispose;
  gobject->finalize = ags_open_wave_finalize;

  /* properties */
  /**
   * AgsOpenWave:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of open sf2 instrument"),
				   i18n_pspec("The audio of open sf2 instrument task"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);
  
  /**
   * AgsOpenWave:audio-file:
   *
   * The assigned #AgsAudioFile
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("audio-file",
				   i18n_pspec("audio file object"),
				   i18n_pspec("The audio file object to create the wave from"),
				   AGS_TYPE_AUDIO_FILE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_FILE,
				  param_spec);

  /**
   * AgsOpenWave:filename:
   *
   * The assigned filename.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("the filename"),
				   i18n_pspec("The filename containing the sf2 instrument"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsOpenWave:start-pad:
   *
   * The assigned start-pad.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("start-pad",
				 i18n_pspec("the start pad"),
				 i18n_pspec("The start pad"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_START_PAD,
				  param_spec);
  
  /* AgsTask */
  task = (AgsTaskClass *) open_wave;

  task->launch = ags_open_wave_launch;
}

void
ags_open_wave_init(AgsOpenWave *open_wave)
{
  open_wave->audio = NULL;

  open_wave->audio_file = NULL;

  open_wave->filename = NULL;

  open_wave->start_pad = NULL;
}

void
ags_open_wave_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsOpenWave *open_wave;

  open_wave = AGS_OPEN_WAVE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(open_wave->audio == (GObject *) audio){
	return;
      }

      if(open_wave->audio != NULL){
	g_object_unref(open_wave->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      open_wave->audio = (GObject *) audio;
    }
    break;
  case PROP_AUDIO_FILE:
    {
      AgsAudioFile *audio_file;

      audio_file = (AgsAudioFile *) g_value_get_object(value);

      if(open_wave->audio_file == (GObject *) audio_file){
	return;
      }

      if(open_wave->audio_file != NULL){
	g_object_unref(open_wave->audio_file);
      }

      if(audio_file != NULL){
	g_object_ref(audio_file);
      }

      open_wave->audio_file = (GObject *) audio_file;
    }
    break;
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(open_wave->filename == filename){
	return;
      }

      if(open_wave->filename != NULL){
        g_free(open_wave->filename);
      }

      open_wave->filename = g_strdup(filename);
    }
    break;
  case PROP_START_PAD:
    {
      open_wave->start_pad = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_open_wave_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsOpenWave *open_wave;

  open_wave = AGS_OPEN_WAVE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, open_wave->audio);
    }
    break;
  case PROP_AUDIO_FILE:
    {
      g_value_set_object(value, open_wave->audio_file);
    }
    break;
  case PROP_FILENAME:
    {
      g_value_set_string(value, open_wave->filename);
    }
    break;
  case PROP_START_PAD:
    {
      g_value_set_uint(value, open_wave->start_pad);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_open_wave_dispose(GObject *gobject)
{
  AgsOpenWave *open_wave;

  open_wave = AGS_OPEN_WAVE(gobject);

  if(open_wave->audio != NULL){
    g_object_unref(open_wave->audio);

    open_wave->audio = NULL;
  }

  if(open_wave->audio_file != NULL){
    g_object_unref(open_wave->audio_file);

    open_wave->audio_file = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_open_wave_parent_class)->dispose(gobject);
}

void
ags_open_wave_finalize(GObject *gobject)
{
  AgsOpenWave *open_wave;

  open_wave = AGS_OPEN_WAVE(gobject);

  if(open_wave->audio != NULL){
    g_object_unref(open_wave->audio);
  }
  
  if(open_wave->audio_file != NULL){
    g_object_unref(open_wave->audio_file);
  }

  g_free(open_wave->filename);

  /* call parent */
  G_OBJECT_CLASS(ags_open_wave_parent_class)->finalize(gobject);
}

void
ags_open_wave_launch(AgsTask *task)
{  
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;

  AgsAudioFile *audio_file;
  
  AgsOpenWave *open_wave;

  GObject *output_soundcard;
  
  GList *start_list, *list;

  guint n_pads, current_pads;
  guint n_audio_channels, current_audio_channels;
  guint i;

  open_wave = task;

  audio = open_wave->audio;

  audio_file = open_wave->audio_file;

  if(audio_file == NULL){
    //TODO:JK: implement me
  }
  
  /* get some fields */
  g_object_get(audio,
	       "input-pads", &n_pads,
	       "audio-channels", &n_audio_channels,
	       "output-soundcard", &output_soundcard,
	       NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(audio_file->sound_resource),
				 &current_audio_channels,
				 NULL,
				 NULL,
				 NULL);
  
  if(current_audio_channels > n_audio_channels){
    n_audio_channels = current_audio_channels;
  }
  
  if(n_audio_channels > 0){
    GList *start_wave, *wave;

    /* resize */
    ags_audio_set_audio_channels(audio,
				 n_audio_channels, 0);

    if(n_pads < open_wave->start_pad + 1){
      ags_audio_set_pads(audio,
			 AGS_TYPE_INPUT,
			 open_wave->start_pad + 1, 0);
    }
          
    wave =
      start_wave = ags_sound_resource_read_wave(AGS_SOUND_RESOURCE(audio_file->sound_resource),
						output_soundcard,
						-1,
						0,
						0.0, 0);

    while(wave != NULL){
      guint current_line;

      g_object_get(wave->data,
		   "line", &current_line,
		   NULL);
      
      g_object_set(wave->data,
		   "line", (open_wave->start_pad * n_audio_channels) + current_line,
		   NULL);
      
      ags_audio_add_wave(audio,
			 wave->data);
      
      wave = wave->next;
    }

    g_list_free(start_wave);
  }
}

/**
 * ags_open_wave_new:
 * @audio: the #AgsAudio
 * @audio_file: the #AgsAudioFile or %NULL
 * @filename: the Soundfont2 file
 * @start_pad: the pad start
 *
 * Creates an #AgsOpenWave.
 *
 * Returns: an new #AgsOpenWave.
 *
 * Since: 2.0.0
 */
AgsOpenWave*
ags_open_wave_new(AgsAudio *audio,
		  AgsAudioFile *audio_file,
		  gchar *filename,
		  guint start_pad)
{
  AgsOpenWave *open_wave;

  open_wave = (AgsOpenWave *) g_object_new(AGS_TYPE_OPEN_WAVE,
					   "audio", audio,
					   "audio-file", audio_file,
					   "filename", filename,
					   "start-pad", start_pad,
					   NULL);

  return(open_wave);
}
