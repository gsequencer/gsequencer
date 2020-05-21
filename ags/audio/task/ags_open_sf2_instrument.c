/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/task/ags_open_sf2_instrument.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>

#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>
#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_audio_container.h>
#include <ags/audio/file/ags_ipatch.h>
#include <ags/audio/file/ags_ipatch_sample.h>
#include <ags/audio/file/ags_ipatch_sf2_reader.h>

#include <ags/config.h>

#include <ags/i18n.h>

void ags_open_sf2_instrument_class_init(AgsOpenSf2InstrumentClass *open_sf2_instrument);
void ags_open_sf2_instrument_init(AgsOpenSf2Instrument *open_sf2_instrument);
void ags_open_sf2_instrument_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_open_sf2_instrument_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_open_sf2_instrument_dispose(GObject *gobject);
void ags_open_sf2_instrument_finalize(GObject *gobject);

void ags_open_sf2_instrument_launch(AgsTask *task);

/**
 * SECTION:ags_open_sf2_instrument
 * @short_description: open Soundfont2 instrument
 * @title: AgsOpenSf2Instrument
 * @section_id:
 * @include: ags/audio/task/ags_open_sf2_instrument.h
 *
 * The #AgsOpenSf2Instrument task opens Soundfont2 instruments.
 */

static gpointer ags_open_sf2_instrument_parent_class = NULL;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_IPATCH,
  PROP_FILENAME,
  PROP_PRESET,
  PROP_INSTRUMENT,
  PROP_START_PAD,
};

GType
ags_open_sf2_instrument_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_open_sf2_instrument = 0;

    static const GTypeInfo ags_open_sf2_instrument_info = {
      sizeof(AgsOpenSf2InstrumentClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_open_sf2_instrument_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsOpenSf2Instrument),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_open_sf2_instrument_init,
    };

    ags_type_open_sf2_instrument = g_type_register_static(AGS_TYPE_TASK,
							  "AgsOpenSf2Instrument",
							  &ags_open_sf2_instrument_info,
							  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_open_sf2_instrument);
  }

  return g_define_type_id__volatile;
}

void
ags_open_sf2_instrument_class_init(AgsOpenSf2InstrumentClass *open_sf2_instrument)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;
  
  ags_open_sf2_instrument_parent_class = g_type_class_peek_parent(open_sf2_instrument);

  /* GObject */
  gobject = (GObjectClass *) open_sf2_instrument;

  gobject->set_property = ags_open_sf2_instrument_set_property;
  gobject->get_property = ags_open_sf2_instrument_get_property;

  gobject->dispose = ags_open_sf2_instrument_dispose;
  gobject->finalize = ags_open_sf2_instrument_finalize;

  /* properties */
  /**
   * AgsOpenSf2Instrument:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 3.0.0
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
   * AgsOpenSf2Instrument:ipatch:
   *
   * The assigned #AgsIpatch
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("ipatch",
				   i18n_pspec("ipatch object"),
				   i18n_pspec("The ipatch object of open sf2 instrument task"),
				   AGS_TYPE_IPATCH,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_IPATCH,
				  param_spec);

  /**
   * AgsOpenSf2Instrument:filename:
   *
   * The assigned filename.
   * 
   * Since: 3.0.0
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
   * AgsOpenSf2Instrument:preset:
   *
   * The assigned preset.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("preset",
				   i18n_pspec("the preset"),
				   i18n_pspec("The preset containing the sf2 instrument"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PRESET,
				  param_spec);

  /**
   * AgsOpenSf2Instrument:instrument:
   *
   * The assigned instrument.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("instrument",
				   i18n_pspec("the instrument"),
				   i18n_pspec("The instrument containing the sf2 instrument"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INSTRUMENT,
				  param_spec);

  /**
   * AgsOpenSf2Instrument:start-pad:
   *
   * The assigned start-pad.
   * 
   * Since: 3.0.0
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
  task = (AgsTaskClass *) open_sf2_instrument;

  task->launch = ags_open_sf2_instrument_launch;
}

void
ags_open_sf2_instrument_init(AgsOpenSf2Instrument *open_sf2_instrument)
{
  open_sf2_instrument->audio = NULL;

  open_sf2_instrument->instrument = NULL;

  open_sf2_instrument->filename = NULL;

  open_sf2_instrument->preset = NULL;
  open_sf2_instrument->instrument = NULL;

  open_sf2_instrument->start_pad = 0;
}

void
ags_open_sf2_instrument_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsOpenSf2Instrument *open_sf2_instrument;

  open_sf2_instrument = AGS_OPEN_SF2_INSTRUMENT(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(open_sf2_instrument->audio == audio){
	return;
      }

      if(open_sf2_instrument->audio != NULL){
	g_object_unref(open_sf2_instrument->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      open_sf2_instrument->audio = audio;
    }
    break;
  case PROP_IPATCH:
    {
      AgsIpatch *ipatch;

      ipatch = (AgsIpatch *) g_value_get_object(value);

      if(open_sf2_instrument->ipatch == ipatch){
	return;
      }

      if(open_sf2_instrument->ipatch != NULL){
	g_object_unref(open_sf2_instrument->ipatch);
      }

      if(ipatch != NULL){
	g_object_ref(ipatch);
      }

      open_sf2_instrument->ipatch = ipatch;
    }
    break;
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(open_sf2_instrument->filename == filename){
	return;
      }

      if(open_sf2_instrument->filename != NULL){
        g_free(open_sf2_instrument->filename);
      }

      open_sf2_instrument->filename = g_strdup(filename);
    }
    break;
  case PROP_PRESET:
    {
      gchar *preset;

      preset = g_value_get_string(value);

      if(open_sf2_instrument->preset == preset){
	return;
      }

      if(open_sf2_instrument->preset != NULL){
        g_free(open_sf2_instrument->preset);
      }

      open_sf2_instrument->preset = g_strdup(preset);
    }
    break;
  case PROP_INSTRUMENT:
    {
      gchar *instrument;

      instrument = g_value_get_string(value);

      if(open_sf2_instrument->instrument == instrument){
	return;
      }

      if(open_sf2_instrument->instrument != NULL){
        g_free(open_sf2_instrument->instrument);
      }

      open_sf2_instrument->instrument = g_strdup(instrument);
    }
    break;
  case PROP_START_PAD:
    {
      open_sf2_instrument->start_pad = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_open_sf2_instrument_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec)
{
  AgsOpenSf2Instrument *open_sf2_instrument;

  open_sf2_instrument = AGS_OPEN_SF2_INSTRUMENT(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, open_sf2_instrument->audio);
    }
    break;
  case PROP_IPATCH:
    {
      g_value_set_object(value, open_sf2_instrument->ipatch);
    }
    break;
  case PROP_FILENAME:
    {
      g_value_set_string(value, open_sf2_instrument->filename);
    }
    break;
  case PROP_PRESET:
    {
      g_value_set_string(value, open_sf2_instrument->preset);
    }
    break;
  case PROP_INSTRUMENT:
    {
      g_value_set_string(value, open_sf2_instrument->instrument);
    }
    break;
  case PROP_START_PAD:
    {
      g_value_set_uint(value, open_sf2_instrument->start_pad);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_open_sf2_instrument_dispose(GObject *gobject)
{
  AgsOpenSf2Instrument *open_sf2_instrument;

  open_sf2_instrument = AGS_OPEN_SF2_INSTRUMENT(gobject);

  if(open_sf2_instrument->audio != NULL){
    g_object_unref(open_sf2_instrument->audio);

    open_sf2_instrument->audio = NULL;
  }

  if(open_sf2_instrument->ipatch != NULL){
    g_object_unref(open_sf2_instrument->ipatch);

    open_sf2_instrument->ipatch = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_open_sf2_instrument_parent_class)->dispose(gobject);
}

void
ags_open_sf2_instrument_finalize(GObject *gobject)
{
  AgsOpenSf2Instrument *open_sf2_instrument;

  open_sf2_instrument = AGS_OPEN_SF2_INSTRUMENT(gobject);

  if(open_sf2_instrument->audio != NULL){
    g_object_unref(open_sf2_instrument->audio);
  }
  
  if(open_sf2_instrument->ipatch != NULL){
    g_object_unref(open_sf2_instrument->ipatch);
  }

  g_free(open_sf2_instrument->filename);
  g_free(open_sf2_instrument->preset);
  g_free(open_sf2_instrument->instrument);

  /* call parent */
  G_OBJECT_CLASS(ags_open_sf2_instrument_parent_class)->finalize(gobject);
}

void
ags_open_sf2_instrument_launch(AgsTask *task)
{
  AgsAudio *audio;
  AgsChannel *start_channel;
  AgsChannel *channel, *next;

  AgsIpatch *ipatch;
  
  AgsOpenSf2Instrument *open_sf2_instrument;

  GObject *output_soundcard;
  
  GList *start_list, *list;

  guint n_pads;
  guint n_audio_channels;
  guint i;
  guint j, j_stop;

  GError *error;
  
  open_sf2_instrument = (AgsOpenSf2Instrument *) task;

  g_return_if_fail(AGS_IS_AUDIO(open_sf2_instrument->audio));
  g_return_if_fail(AGS_IS_IPATCH(open_sf2_instrument->ipatch));
  
  audio = open_sf2_instrument->audio;

  ipatch = open_sf2_instrument->ipatch;
  
  /* read all samples */
  list = 
    start_list = ags_sound_container_get_resource_current(AGS_SOUND_CONTAINER(ipatch));
  
  n_pads = g_list_length(start_list);

  g_object_get(audio,
	       "audio-channels", &n_audio_channels,
	       "output-soundcard", &output_soundcard,
	       NULL);

  while(list != NULL){
    guint current_audio_channels;
    
    ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(list->data),
				   &current_audio_channels,
				   NULL,
				   NULL,
				   NULL);

    if(current_audio_channels > n_audio_channels){
      n_audio_channels = current_audio_channels;
    }
    
    list = list->next;
  }

  /* read */
  if(n_audio_channels > 0 &&
     n_pads > 0){
    /* resize */
    ags_audio_set_audio_channels(audio,
				 n_audio_channels, 0);
    ags_audio_set_pads(audio,
		       AGS_TYPE_INPUT,
		       open_sf2_instrument->start_pad + n_pads, 0);

    /* get some fields */
    g_object_get(audio,
		 "input", &start_channel,
		 NULL);
    
    list = start_list;
    i = 0;
    
    while(list != NULL){
      GList *start_audio_signal, *audio_signal;

      //      g_message("open sf2 sample: %s", ipatch_sf2_sample_get_name(AGS_IPATCH_SAMPLE(list->data)->sample));
      
      channel = ags_channel_pad_nth(start_channel,
				    open_sf2_instrument->start_pad + i);
      next = NULL;
      
      ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(list->data),
				     &j_stop,
				     NULL,
				     NULL,
				     NULL);
      
      audio_signal =
	start_audio_signal = ags_sound_resource_read_audio_signal(AGS_SOUND_RESOURCE(list->data),
								  output_soundcard,
								  -1);
      
      for(j = 0; j < j_stop && audio_signal != NULL; j++){
	AgsRecycling *recycling;

	AgsFileLink *file_link;

	/* reset link */
	error = NULL;
	ags_channel_set_link(channel, NULL,
			     &error);
	
	if(error != NULL){
	  g_warning("%s", error->message);

	  g_error_free(error);
	}

	/* get recycling */
	g_object_get(channel,
		     "first-recycling", &recycling,
		     "file-link", &file_link,
		     NULL);
	g_object_unref(recycling);
	
	/* set filename and channel */
	if(file_link == NULL){
	  file_link = g_object_new(AGS_TYPE_AUDIO_FILE_LINK,
				   NULL);
      
	  g_object_set(channel,
		       "file-link", file_link,
		       NULL);
	}
	  
	g_object_set(file_link,
		     "filename", ipatch->filename,
		     "preset", AGS_IPATCH_SF2_READER(ipatch->reader)->name_selected[AGS_SF2_PHDR],
		     "instrument", AGS_IPATCH_SF2_READER(ipatch->reader)->name_selected[AGS_SF2_IHDR],
		     "sample", ipatch_sf2_sample_get_name(AGS_IPATCH_SAMPLE(list->data)->sample),
		     "audio-channel", j,
		     NULL);	

	g_object_unref(file_link);
	
	/* replace template audio signal */
	AGS_AUDIO_SIGNAL(audio_signal->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_recycling_add_audio_signal(recycling,
				       audio_signal->data);
	
	/* iterate */
	audio_signal = audio_signal->next;

	next = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next;
      }

      if(start_audio_signal == NULL){
	g_object_unref(channel);
      }
      
      if(next != NULL){
	g_object_unref(next);
      }
      
      g_list_free_full(start_audio_signal,
		       g_object_unref);
      
      /* iterate */
      list = list->next;

      i++;
    }    

    /* unref */
    if(start_channel != NULL){
      g_object_unref(start_channel);
    }
    
    g_list_free_full(start_list,
		     g_object_unref);
  }

  g_object_unref(output_soundcard);
}

/**
 * ags_open_sf2_instrument_new:
 * @audio: the #AgsAudio
 * @ipatch: the #AgsIpatch or %NULL
 * @filename: the Soundfont2 file
 * @preset: the preset
 * @instrument: the instrument
 * @start_pad: the pad start
 *
 * Creates an #AgsOpenSf2Instrument.
 *
 * Returns: an new #AgsOpenSf2Instrument.
 *
 * Since: 3.0.0
 */
AgsOpenSf2Instrument*
ags_open_sf2_instrument_new(AgsAudio *audio,
			    AgsIpatch *ipatch,
			    gchar *filename,
			    gchar *preset,
			    gchar *instrument,
			    guint start_pad)
{
  AgsOpenSf2Instrument *open_sf2_instrument;

  open_sf2_instrument = (AgsOpenSf2Instrument *) g_object_new(AGS_TYPE_OPEN_SF2_INSTRUMENT,
							      "audio", audio,
							      "ipatch", ipatch,
							      "filename", filename,
							      "preset", preset,
							      "instrument", instrument,
							      "start-pad", start_pad,
							      NULL);

  return(open_sf2_instrument);
}
