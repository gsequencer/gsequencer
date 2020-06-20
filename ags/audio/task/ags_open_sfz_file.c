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

#include <ags/audio/task/ags_open_sfz_file.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>

#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>
#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_audio_container.h>
#include <ags/audio/file/ags_sfz_file.h>
#include <ags/audio/file/ags_sfz_group.h>
#include <ags/audio/file/ags_sfz_region.h>
#include <ags/audio/file/ags_sfz_sample.h>

#include <ags/config.h>

#include <ags/i18n.h>

void ags_open_sfz_file_class_init(AgsOpenSFZFileClass *open_sfz_file);
void ags_open_sfz_file_init(AgsOpenSFZFile *open_sfz_file);
void ags_open_sfz_file_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_open_sfz_file_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_open_sfz_file_dispose(GObject *gobject);
void ags_open_sfz_file_finalize(GObject *gobject);

void ags_open_sfz_file_launch(AgsTask *task);


/**
 * SECTION:ags_open_sfz_file
 * @short_description: open SFZ file
 * @title: AgsOpenSFZFile
 * @section_id:
 * @include: ags/audio/task/ags_open_sfz_file.h
 *
 * The #AgsOpenSFZFile task opens SFZ file.
 */

static gpointer ags_open_sfz_file_parent_class = NULL;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_SFZ_FILE,
  PROP_FILENAME,
  PROP_START_PAD,
};

GType
ags_open_sfz_file_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_open_sfz_file = 0;

    static const GTypeInfo ags_open_sfz_file_info = {
      sizeof(AgsOpenSFZFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_open_sfz_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsOpenSFZFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_open_sfz_file_init,
    };

    ags_type_open_sfz_file = g_type_register_static(AGS_TYPE_TASK,
						    "AgsOpenSFZFile",
						    &ags_open_sfz_file_info,
						    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_open_sfz_file);
  }

  return g_define_type_id__volatile;
}

void
ags_open_sfz_file_class_init(AgsOpenSFZFileClass *open_sfz_file)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;
  
  ags_open_sfz_file_parent_class = g_type_class_peek_parent(open_sfz_file);

  /* GObject */
  gobject = (GObjectClass *) open_sfz_file;

  gobject->set_property = ags_open_sfz_file_set_property;
  gobject->get_property = ags_open_sfz_file_get_property;

  gobject->dispose = ags_open_sfz_file_dispose;
  gobject->finalize = ags_open_sfz_file_finalize;

  /* properties */
  /**
   * AgsOpenSFZFile:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 3.4.0
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
   * AgsOpenSFZFile:sfz-file:
   *
   * The assigned #AgsSFZFile
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_object("sfz-file",
				   i18n_pspec("SFZ file object"),
				   i18n_pspec("The SFZ file object of open task"),
				   AGS_TYPE_SFZ_FILE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SFZ_FILE,
				  param_spec);

  /**
   * AgsOpenSFZFile:filename:
   *
   * The assigned filename.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("the filename"),
				   i18n_pspec("The filename"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsOpenSFZFile:start-pad:
   *
   * The assigned start-pad.
   * 
   * Since: 3.4.0
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
  task = (AgsTaskClass *) open_sfz_file;

  task->launch = ags_open_sfz_file_launch;
}

void
ags_open_sfz_file_init(AgsOpenSFZFile *open_sfz_file)
{
  open_sfz_file->audio = NULL;

  open_sfz_file->filename = NULL;

  open_sfz_file->start_pad = 0;
}

void
ags_open_sfz_file_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsOpenSFZFile *open_sfz_file;

  open_sfz_file = AGS_OPEN_SFZ_FILE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
  {
    AgsAudio *audio;

    audio = (AgsAudio *) g_value_get_object(value);

    if(open_sfz_file->audio == audio){
      return;
    }

    if(open_sfz_file->audio != NULL){
      g_object_unref(open_sfz_file->audio);
    }

    if(audio != NULL){
      g_object_ref(audio);
    }

    open_sfz_file->audio = audio;
  }
  break;
  case PROP_SFZ_FILE:
  {
    AgsSFZFile *sfz_file;

    sfz_file = (AgsSFZFile *) g_value_get_object(value);

    if(open_sfz_file->sfz_file == sfz_file){
      return;
    }

    if(open_sfz_file->sfz_file != NULL){
      g_object_unref(open_sfz_file->sfz_file);
    }

    if(sfz_file != NULL){
      g_object_ref(sfz_file);
    }

    open_sfz_file->sfz_file = sfz_file;
  }
  break;
  case PROP_FILENAME:
  {
    gchar *filename;

    filename = g_value_get_string(value);

    if(open_sfz_file->filename == filename){
      return;
    }

    if(open_sfz_file->filename != NULL){
      g_free(open_sfz_file->filename);
    }

    open_sfz_file->filename = g_strdup(filename);
  }
  break;
  case PROP_START_PAD:
  {
    open_sfz_file->start_pad = g_value_get_uint(value);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_open_sfz_file_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsOpenSFZFile *open_sfz_file;

  open_sfz_file = AGS_OPEN_SFZ_FILE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
  {
    g_value_set_object(value, open_sfz_file->audio);
  }
  break;
  case PROP_SFZ_FILE:
  {
    g_value_set_object(value, open_sfz_file->sfz_file);
  }
  break;
  case PROP_FILENAME:
  {
    g_value_set_string(value, open_sfz_file->filename);
  }
  break;
  case PROP_START_PAD:
  {
    g_value_set_uint(value, open_sfz_file->start_pad);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_open_sfz_file_dispose(GObject *gobject)
{
  AgsOpenSFZFile *open_sfz_file;

  open_sfz_file = AGS_OPEN_SFZ_FILE(gobject);

  if(open_sfz_file->audio != NULL){
    g_object_unref(open_sfz_file->audio);

    open_sfz_file->audio = NULL;
  }

  if(open_sfz_file->sfz_file != NULL){
    g_object_unref(open_sfz_file->sfz_file);

    open_sfz_file->sfz_file = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_open_sfz_file_parent_class)->dispose(gobject);
}

void
ags_open_sfz_file_finalize(GObject *gobject)
{
  AgsOpenSFZFile *open_sfz_file;

  open_sfz_file = AGS_OPEN_SFZ_FILE(gobject);

  if(open_sfz_file->audio != NULL){
    g_object_unref(open_sfz_file->audio);
  }
  
  if(open_sfz_file->sfz_file != NULL){
    g_object_unref(open_sfz_file->sfz_file);
  }

  g_free(open_sfz_file->filename);

  /* call parent */
  G_OBJECT_CLASS(ags_open_sfz_file_parent_class)->finalize(gobject);
}

void
ags_open_sfz_file_launch(AgsTask *task)
{
  AgsChannel *start_input, *next_input, *input;
  AgsSFZGroup *group;
  AgsSFZRegion *region;      
  AgsOpenSFZFile *open_sfz_file;
  
  GObject *output_soundcard;

  GList *start_audio_signal, *audio_signal;
  GList *start_list, *list;

  glong key, current_key;
  glong hikey, lokey, current_hikey, current_lokey;
  guint n_pads, current_pads;
  guint n_audio_channels, current_audio_channels;
  int retval;
  guint j_stop;
  guint i, j;
  
  open_sfz_file = AGS_OPEN_SFZ_FILE(task);

  g_return_if_fail(AGS_IS_AUDIO(open_sfz_file->audio));
  g_return_if_fail(AGS_IS_SFZ_FILE(open_sfz_file->sfz_file));

  g_object_get(open_sfz_file->audio,
	       "output-soundcard", &output_soundcard,
	       NULL);

  /* select */
  start_input = NULL;
  
  n_pads = 0;
  n_audio_channels = 0;
  
  g_object_get(open_sfz_file->audio,
	       "input-pads", &n_pads,
	       "audio-channels", &n_audio_channels,
	       NULL);

  hikey = 0;
  lokey = 0;
  
  ags_sfz_file_get_range(AGS_SFZ_FILE(open_sfz_file->sfz_file),
			 &lokey, &hikey);
  
  /* resize */
  g_object_get(open_sfz_file->sfz_file,
	       "sample", &start_list,
	       NULL);

  list = start_list;
  current_audio_channels = n_audio_channels;
  
  while(list != NULL){
    guint tmp;

    ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(list->data),
				   &tmp,
				   NULL,
				   NULL,
				   NULL);

    if(tmp > current_audio_channels){
      current_audio_channels = tmp;
    }
    
    /* iterate */
    list = list->next;
  }
  
  if(current_audio_channels > n_audio_channels){
    n_audio_channels = current_audio_channels;
    
    ags_audio_set_audio_channels(open_sfz_file->audio,
				 n_audio_channels, 0);
  }
  
  current_pads = hikey - lokey + 1;
  
  if(lokey <= hikey &&
     current_pads > n_pads){
    n_pads = current_pads;
    
    ags_audio_set_pads(open_sfz_file->audio,
		       AGS_TYPE_INPUT,
		       n_pads, 0);
  }

  g_object_get(open_sfz_file->audio,
	       "input", &start_input,
	       NULL);
  
  input = start_input;

  audio_signal = 
    start_audio_signal = NULL;
    
  j_stop = n_audio_channels;
  
  for(i = 0, j = 0; input != NULL;){
    AgsRecycling *first_recycling;

    g_object_get(input,
		 "first-recycling", &first_recycling,
		 NULL);

    list = start_list;

    while(list != NULL){
      gchar *str_key, *str_pitch_keycenter;
      gchar *str_lokey, *str_hikey;
      
      gboolean success;
      
      g_object_get(list->data,
		   "group", &group,
		   "region", &region,
		   NULL);

      /* group */
      str_pitch_keycenter = ags_sfz_group_lookup_control(group,
							 "pitch_keycenter");
      
      str_key = ags_sfz_group_lookup_control(group,
					     "key");
      
      str_lokey = ags_sfz_group_lookup_control(group,
					       "lokey");

      str_hikey = ags_sfz_group_lookup_control(group,
					       "hikey");

      lokey = 0;
      
      if(str_lokey != NULL){	
	retval = sscanf(str_lokey, "%lu", &current_lokey);

	if(retval <= 0){
	  retval = ags_diatonic_scale_note_to_midi_key(str_lokey,
						       &current_lokey);

	  if(retval > 0){
	    lokey = current_lokey;
	  }
	}else{
	  lokey = current_lokey;
	}
      }
      
      key = lokey + i;

      success = FALSE;
      
      if(str_pitch_keycenter != NULL){
	retval = sscanf(str_pitch_keycenter, "%lu", &current_key);

	if(retval > 0){
	  if(current_key == key){
	    success = TRUE;
	  }	  
	}else{
	  retval = ags_diatonic_scale_note_to_midi_key(str_pitch_keycenter,
						       &current_key);
	  
	  if(retval > 0){
	    if(current_key == key){
	      success = TRUE;
	    }
	  }
	}	
      }

      if(str_key != NULL){
	retval = sscanf(str_key, "%lu", &current_key);

	if(retval > 0){
	  if(current_key == key){
	    success = TRUE;
	  }	  
	}else{
	  retval = ags_diatonic_scale_note_to_midi_key(str_key,
						       &current_key);

	  if(retval > 0){
	    if(current_key == key){
	      success = TRUE;
	    }
	  }
	}	

	if(success){
	  g_free(str_pitch_keycenter);
	  g_free(str_key);

	  g_free(str_hikey);
	  g_free(str_lokey);

	  break;
	}
      }

      if(str_hikey != NULL && str_lokey != NULL){
	success = FALSE;
	
	retval = sscanf(str_hikey, "%lu", &current_hikey);

	if(retval <= 0){
	  retval = ags_diatonic_scale_note_to_midi_key(str_hikey,
						       &current_hikey);

	  if(retval > 0){
	    if(current_key <= current_hikey){
	      success = TRUE;
	    }	  
	  }
	}else{
	  if(current_key <= current_hikey){
	    success = TRUE;
	  }	  
	}
	
	retval = sscanf(str_lokey, "%lu", &current_lokey);

	if(retval <= 0){
	  retval = ags_diatonic_scale_note_to_midi_key(str_lokey,
						       &current_lokey);

	  if(retval > 0){
	    if(current_key >= current_lokey){
	      success = TRUE;
	    }else{
	      success = FALSE;
	    }	  
	  }else{
	    success = FALSE;
	  }
	}else{
	  if(current_key >= current_lokey){
	    success = TRUE;
	  }else{
	    success = FALSE;
	  }	  
	}

	if(success){
	  hikey = current_hikey;
	  lokey = current_lokey;

	  g_free(str_pitch_keycenter);
	  g_free(str_key);

	  g_free(str_hikey);
	  g_free(str_lokey);

	  break;
	}
      }

      g_free(str_pitch_keycenter);
      g_free(str_key);

      g_free(str_hikey);
      g_free(str_lokey);

      /* region */
      str_pitch_keycenter = ags_sfz_region_lookup_control(region,
							  "pitch_keycenter");
      
      str_key = ags_sfz_region_lookup_control(region,
					      "key");
      
      str_lokey = ags_sfz_region_lookup_control(region,
						"lokey");

      str_hikey = ags_sfz_region_lookup_control(region,
						"hikey");

      lokey = 0;
      
      if(str_lokey != NULL){	
	retval = sscanf(str_lokey, "%lu", &current_lokey);

	if(retval <= 0){
	  retval = ags_diatonic_scale_note_to_midi_key(str_lokey,
						       &current_lokey);

	  if(retval > 0){
	    lokey = current_lokey;
	  }
	}else{
	  lokey = current_lokey;
	}
      }
      
      key = lokey + i;

      success = FALSE;
      
      if(str_pitch_keycenter != NULL){
	retval = sscanf(str_pitch_keycenter, "%lu", &current_key);

	if(retval > 0){
	  if(current_key == key){
	    success = TRUE;
	  }	  
	}else{
	  retval = ags_diatonic_scale_note_to_midi_key(str_pitch_keycenter,
						       &current_key);
	  
	  if(retval > 0){
	    if(current_key == key){
	      success = TRUE;
	    }
	  }
	}	
      }

      if(str_key != NULL){
	retval = sscanf(str_key, "%lu", &current_key);

	if(retval > 0){
	  if(current_key == key){
	    success = TRUE;
	  }	  
	}else{
	  retval = ags_diatonic_scale_note_to_midi_key(str_key,
						       &current_key);

	  if(retval > 0){
	    if(current_key == key){
	      success = TRUE;
	    }
	  }
	}	

	if(success){
	  g_free(str_pitch_keycenter);
	  g_free(str_key);

	  g_free(str_hikey);
	  g_free(str_lokey);

	  break;
	}
      }

      if(str_hikey != NULL && str_lokey != NULL){
	gboolean success;

	success = FALSE;
	
	retval = sscanf(str_hikey, "%lu", &current_hikey);

	if(retval <= 0){
	  retval = ags_diatonic_scale_note_to_midi_key(str_hikey,
						       &current_hikey);

	  if(retval > 0){
	    if(current_key <= current_hikey){
	      success = TRUE;
	    }	  
	  }
	}else{
	  if(current_key <= current_hikey){
	    success = TRUE;
	  }	  
	}
	
	retval = sscanf(str_lokey, "%lu", &current_lokey);

	if(retval <= 0){
	  retval = ags_diatonic_scale_note_to_midi_key(str_lokey,
						       &current_lokey);

	  if(retval > 0){
	    if(current_key >= current_lokey){	      
	      success = TRUE;
	    }else{
	      success = FALSE;
	    }	  
	  }else{
	    success = FALSE;
	  }
	}else{
	  if(current_key >= current_lokey){
	    success = TRUE;
	  }else{
	    success = FALSE;
	  }	  
	}

	if(success){
	  hikey = current_hikey;
	  lokey = current_lokey;

	  g_free(str_pitch_keycenter);
	  g_free(str_key);

	  g_free(str_hikey);
	  g_free(str_lokey);

	  break;
	}
      }

      g_free(str_pitch_keycenter);
      g_free(str_key);

      g_free(str_hikey);
      g_free(str_lokey);
      
      /* iterate */
      list = list->next;
    }
    
    if(j == 0 &&
       list != NULL){
      audio_signal = 
	start_audio_signal = ags_sound_resource_read_audio_signal(AGS_SOUND_RESOURCE(list->data),
								  output_soundcard,
								  -1);
    }

    /* add audio signal as template to recycling */
    audio_signal = g_list_nth(start_audio_signal,
			      j);

    if(audio_signal != NULL){
      AgsAudioSignal *current_audio_signal;

      GList *start_stream, *stream;
      
      gchar *str_key, *str_pitch_keycenter;
      gchar *str_lokey;

      guint samplerate;
      guint buffer_size;
      guint format;
      guint loop_start, loop_end;
      glong pitch_keycenter, current_pitch_keycenter;
      guint x_offset;

      /* key center */
      g_object_get(list->data,
		   "group", &group,
		   "region", &region,
		   NULL);

      key = lokey + i;
      pitch_keycenter = 49;
      
      /* group */
      str_pitch_keycenter = ags_sfz_group_lookup_control(group,
							 "pitch_keycenter");
      
      str_key = ags_sfz_group_lookup_control(group,
					     "key");

      if(str_pitch_keycenter != NULL){
	retval = sscanf(str_pitch_keycenter, "%lu", &current_pitch_keycenter);

	if(retval > 0){
	  pitch_keycenter = current_pitch_keycenter;
	}else{
	  retval = ags_diatonic_scale_note_to_midi_key(str_pitch_keycenter,
						       &current_key);

	  if(retval > 0){
	    pitch_keycenter = current_key;
	  }
	}
      }else if(str_key != NULL){
	retval = sscanf(str_key, "%lu", &current_pitch_keycenter);

	if(retval > 0){
	  pitch_keycenter = current_key;
	}else{
	  retval = ags_diatonic_scale_note_to_midi_key(str_key,
						       &current_key);

	  if(retval > 0){
	    pitch_keycenter = current_key;
	  }
	}	
      }

      g_free(str_pitch_keycenter);
      g_free(str_key);

      /* region */
      str_pitch_keycenter = ags_sfz_region_lookup_control(region,
							  "pitch_keycenter");
      
      str_key = ags_sfz_region_lookup_control(region,
					      "key");

      if(str_pitch_keycenter != NULL){
	retval = sscanf(str_pitch_keycenter, "%lu", &current_pitch_keycenter);

	if(retval > 0){
	  pitch_keycenter = current_pitch_keycenter;
	}else{
	  retval = ags_diatonic_scale_note_to_midi_key(str_pitch_keycenter,
						       &current_key);

	  if(retval > 0){
	    pitch_keycenter = current_key;
	  }
	}		
      }else if(str_key != NULL){
	retval = sscanf(str_key, "%lu", &current_pitch_keycenter);

	if(retval > 0){
	  pitch_keycenter = current_key;
	}else{
	  retval = ags_diatonic_scale_note_to_midi_key(str_key,
						       &current_key);

	  if(retval > 0){
	    pitch_keycenter = current_key;
	  }
	}	
      }

      g_free(str_pitch_keycenter);
      g_free(str_key);
      
      /* loop start/end */
      g_object_get(audio_signal->data,
		   "samplerate", &samplerate,
		   "buffer-size", &buffer_size,
		   "format", &format,
		   "loop-start", &loop_start,
		   "loop-end", &loop_end,
		   NULL);

      /* create audio signal */
      current_audio_signal = ags_audio_signal_new(output_soundcard,
						  (GObject *) first_recycling,
						  NULL);
      current_audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
      
      g_object_set(current_audio_signal,
		   "loop-start", loop_start,
		   "loop-end", loop_end,
		   NULL);

      ags_audio_signal_duplicate_stream(current_audio_signal,
					audio_signal->data);
      
      /* pitch */
      stream =
	start_stream = current_audio_signal->stream;

      x_offset = 0;

      key -= lokey;

      while(stream != NULL){
	switch(format){
	case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  ags_filter_util_pitch_s8((gint8 *) stream->data,
				   buffer_size,
				   samplerate,
				   pitch_keycenter - 48.0,
				   ((gdouble) key - (gdouble) pitch_keycenter) * 100.0);
	}
	break;
	case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  ags_filter_util_pitch_s16((gint16 *) stream->data,
				    buffer_size,
				    samplerate,
				    pitch_keycenter - 48.0,
				    ((gdouble) key - (gdouble) pitch_keycenter) * 100.0);
	}
	break;
	case AGS_SOUNDCARD_SIGNED_24_BIT:
	{
	  ags_filter_util_pitch_s24((gint32 *) stream->data,
				    buffer_size,
				    samplerate,
				    pitch_keycenter - 48.0,
				    ((gdouble) key - (gdouble) pitch_keycenter) * 100.0);
	}
	break;
	case AGS_SOUNDCARD_SIGNED_32_BIT:
	{
	  ags_filter_util_pitch_s32((gint32 *) stream->data,
				    buffer_size,
				    samplerate,
				    pitch_keycenter - 48.0,
				    ((gdouble) key - (gdouble) pitch_keycenter) * 100.0);
	}
	break;
	case AGS_SOUNDCARD_SIGNED_64_BIT:
	{
	  ags_filter_util_pitch_s64((gint64 *) stream->data,
				    buffer_size,
				    samplerate,
				    pitch_keycenter - 48.0,
				    ((gdouble) key - (gdouble) pitch_keycenter) * 100.0);
	}
	break;
	case AGS_SOUNDCARD_FLOAT:
	{
	  ags_filter_util_pitch_float((gfloat *) stream->data,
				      buffer_size,
				      samplerate,
				      pitch_keycenter - 48.0,
				      ((gdouble) key - (gdouble) pitch_keycenter) * 100.0);
	}
	break;
	case AGS_SOUNDCARD_DOUBLE:
	{
	  ags_filter_util_pitch_double((gdouble *) stream->data,
				       buffer_size,
				       samplerate,
				       pitch_keycenter - 48.0,
				       ((gdouble) key - (gdouble) pitch_keycenter) * 100.0);
	}
	break;
	case AGS_SOUNDCARD_COMPLEX:
	{
	  ags_filter_util_pitch_complex((AgsComplex *) stream->data,
					buffer_size,
					samplerate,
					pitch_keycenter - 48.0,
					((gdouble) key - (gdouble) pitch_keycenter) * 100.0);
	}
	break;
	}	  

	/* iterate */
	x_offset += buffer_size;
	
	stream = stream->next;
      }

      /* add audio signal */
      ags_recycling_add_audio_signal(first_recycling,
				     current_audio_signal);
    }else{
      g_message("SFZ audio signal not found");
    }

    g_object_unref(first_recycling);
    
    /* iterate */
    if(j + 1 < j_stop){
      j++;
    }else{
      g_list_free_full(start_audio_signal,
		       g_object_unref);

      audio_signal = 
	start_audio_signal = NULL;

      i++;
      j = 0;

      list = list->next;
    }

    next_input = ags_channel_next(input);

    g_object_unref(input);

    input = next_input;
  }
  
  g_object_unref(output_soundcard);

  g_list_free_full(start_list,
		   g_object_unref);
}

/**
 * ags_open_sfz_file_new:
 * @audio: the #AgsAudio
 * @sfz_file: the #AgsSFZFile or %NULL
 * @filename: the Soundfont2 file
 * @start_pad: the pad start
 *
 * Creates an #AgsOpenSFZFile.
 *
 * Returns: an new #AgsOpenSFZFile.
 *
 * Since: 3.4.0
 */
AgsOpenSFZFile*
ags_open_sfz_file_new(AgsAudio *audio,
		      AgsSFZFile *sfz_file,
		      gchar *filename,
		      guint start_pad)
{
  AgsOpenSFZFile *open_sfz_file;

  open_sfz_file = (AgsOpenSFZFile *) g_object_new(AGS_TYPE_OPEN_SFZ_FILE,
						  "audio", audio,
						  "sfz-file", sfz_file,
						  "filename", filename,
						  "start-pad", start_pad,
						  NULL);

  return(open_sfz_file);
}
