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

#include <ags/audio/thread/ags_sfz_loader.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_diatonic_scale.h>
#include <ags/audio/ags_filter_util.h>

#include <ags/audio/file/ags_audio_container.h>
#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>
#include <ags/audio/file/ags_sfz_file.h>
#include <ags/audio/file/ags_sfz_group.h>
#include <ags/audio/file/ags_sfz_region.h>

#include <ags/i18n.h>

void ags_sfz_loader_class_init(AgsSFZLoaderClass *sfz_loader);
void ags_sfz_loader_init(AgsSFZLoader *sfz_loader);
void ags_sfz_loader_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_sfz_loader_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_sfz_loader_dispose(GObject *gobject);
void ags_sfz_loader_finalize(GObject *gobject);

void* ags_sfz_loader_run(void *ptr);

/**
 * SECTION:ags_sfz_loader
 * @short_description: load sfz asynchronously
 * @title: AgsSFZLoader
 * @section_id:
 * @include: ags/plugin/ags_sfz_loader.h
 *
 * The #AgsSFZLoader loads sfz audio data asynchronously.
 */

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_FILENAME,
  PROP_AUDIO_CONTAINER,
};

static gpointer ags_sfz_loader_parent_class = NULL;

GType
ags_sfz_loader_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sfz_loader = 0;

    static const GTypeInfo ags_sfz_loader_info = {
      sizeof(AgsSFZLoaderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sfz_loader_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSFZLoader),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sfz_loader_init,
    };

    ags_type_sfz_loader = g_type_register_static(G_TYPE_OBJECT,
						 "AgsSFZLoader",
						 &ags_sfz_loader_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sfz_loader);
  }

  return g_define_type_id__volatile;
}

void
ags_sfz_loader_class_init(AgsSFZLoaderClass *sfz_loader)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_sfz_loader_parent_class = g_type_class_peek_parent(sfz_loader);

  /* GObject */
  gobject = (GObjectClass *) sfz_loader;

  gobject->set_property = ags_sfz_loader_set_property;
  gobject->get_property = ags_sfz_loader_get_property;

  gobject->dispose = ags_sfz_loader_dispose;
  gobject->finalize = ags_sfz_loader_finalize;

  /* properties */
  /**
   * AgsSFZLoader:audio:
   *
   * The assigned audio.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio"),
				   i18n_pspec("The audio"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsSFZLoader:filename:
   *
   * The filename to open.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("filename"),
				   i18n_pspec("The filename"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsSFZLoader:audio-container:
   *
   * The audio container opened.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio-container",
				   i18n_pspec("audio container"),
				   i18n_pspec("The audio container"),
				   AGS_TYPE_AUDIO_CONTAINER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CONTAINER,
				  param_spec);
}

void
ags_sfz_loader_init(AgsSFZLoader *sfz_loader)
{
  sfz_loader->flags = 0;

  /* add base plugin mutex */
  g_rec_mutex_init(&(sfz_loader->obj_mutex)); 

  /* fields */
  sfz_loader->thread = NULL;
  
  sfz_loader->audio = NULL;

  sfz_loader->filename = NULL;

  sfz_loader->audio_container = NULL;
}

void
ags_sfz_loader_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsSFZLoader *sfz_loader;

  GRecMutex *sfz_loader_mutex;

  sfz_loader = AGS_SFZ_LOADER(gobject);

  /* get base plugin mutex */
  sfz_loader_mutex = AGS_SFZ_LOADER_GET_OBJ_MUTEX(sfz_loader);

  switch(prop_id){
  case PROP_AUDIO:
  {
    AgsAudio *audio;

    audio = g_value_get_object(value);
      
    g_rec_mutex_lock(sfz_loader_mutex);

    if(sfz_loader->audio == audio){
      g_rec_mutex_unlock(sfz_loader_mutex);

      return;
    }

    if(sfz_loader->audio != NULL){
      g_object_unref(sfz_loader->audio);
    }

    if(audio != NULL){
      g_object_ref(audio);
    }

    sfz_loader->audio = audio;
      
    g_rec_mutex_unlock(sfz_loader_mutex);
  }
  break;
  case PROP_FILENAME:
  {
    gchar *filename;

    filename = g_value_get_string(value);
      
    g_rec_mutex_lock(sfz_loader_mutex);

    if(sfz_loader->filename == filename){
      g_rec_mutex_unlock(sfz_loader_mutex);

      return;
    }

    if(sfz_loader->filename != NULL){
      g_free(sfz_loader->filename);
    }

    sfz_loader->filename = g_strdup(filename);
      
    g_rec_mutex_unlock(sfz_loader_mutex);
  }
  break;
  case PROP_AUDIO_CONTAINER:
  {
    AgsAudioContainer *audio_container;

    audio_container = g_value_get_object(value);
      
    g_rec_mutex_lock(sfz_loader_mutex);

    if(sfz_loader->audio_container == audio_container){
      g_rec_mutex_unlock(sfz_loader_mutex);

      return;
    }

    if(sfz_loader->audio_container != NULL){
      g_object_unref(sfz_loader->audio_container);
    }

    if(audio_container != NULL){
      g_object_ref(audio_container);
    }

    sfz_loader->audio_container = audio_container;
      
    g_rec_mutex_unlock(sfz_loader_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}
    
void
ags_sfz_loader_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsSFZLoader *sfz_loader;

  GRecMutex *sfz_loader_mutex;

  sfz_loader = AGS_SFZ_LOADER(gobject);

  /* get base plugin mutex */
  sfz_loader_mutex = AGS_SFZ_LOADER_GET_OBJ_MUTEX(sfz_loader);

  switch(prop_id){
  case PROP_AUDIO:
  {
    g_rec_mutex_lock(sfz_loader_mutex);
      
    g_value_set_object(value, sfz_loader->audio);

    g_rec_mutex_unlock(sfz_loader_mutex);
  }
  break;
  case PROP_FILENAME:
  {
    g_rec_mutex_lock(sfz_loader_mutex);
      
    g_value_set_string(value, sfz_loader->filename);

    g_rec_mutex_unlock(sfz_loader_mutex);
  }
  break;
  case PROP_AUDIO_CONTAINER:
  {
    g_rec_mutex_lock(sfz_loader_mutex);
      
    g_value_set_object(value, sfz_loader->audio_container);

    g_rec_mutex_unlock(sfz_loader_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_sfz_loader_dispose(GObject *gobject)
{
  AgsSFZLoader *sfz_loader;

  sfz_loader = AGS_SFZ_LOADER(gobject);

  if(sfz_loader->audio != NULL){
    g_object_unref(sfz_loader->audio);

    sfz_loader->audio = NULL;
  }

  if(sfz_loader->audio_container != NULL){
    g_object_unref(sfz_loader->audio_container);

    sfz_loader->audio_container = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_sfz_loader_parent_class)->dispose(gobject);
}

void
ags_sfz_loader_finalize(GObject *gobject)
{
  AgsSFZLoader *sfz_loader;

  sfz_loader = AGS_SFZ_LOADER(gobject);
  
  /* destroy object mutex */
  if(sfz_loader->audio != NULL){
    g_object_unref(sfz_loader->audio);
  }

  g_free(sfz_loader->filename);

  if(sfz_loader->audio_container != NULL){
    g_object_unref(sfz_loader->audio_container);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_sfz_loader_parent_class)->finalize(gobject);
}

/**
 * ags_sfz_loader_test_flags:
 * @sfz_loader: the #AgsSFZLoader
 * @flags: the flags
 * 
 * Test @flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_sfz_loader_test_flags(AgsSFZLoader *sfz_loader, guint flags)
{
  gboolean retval;
  
  GRecMutex *sfz_loader_mutex;

  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return(FALSE);
  }
  
  /* get sfz loader mutex */
  sfz_loader_mutex = AGS_SFZ_LOADER_GET_OBJ_MUTEX(sfz_loader);

  /* test flags */
  g_rec_mutex_lock(sfz_loader_mutex);

  retval = ((flags & (sfz_loader->flags)) != 0) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(sfz_loader_mutex);

  return(retval);
}

/**
 * ags_sfz_loader_set_flags:
 * @sfz_loader: the #AgsSFZLoader
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 3.0.0
 */
void
ags_sfz_loader_set_flags(AgsSFZLoader *sfz_loader, guint flags)
{
  GRecMutex *sfz_loader_mutex;

  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return;
  }
  
  /* get sfz loader mutex */
  sfz_loader_mutex = AGS_SFZ_LOADER_GET_OBJ_MUTEX(sfz_loader);

  /* set flags */
  g_rec_mutex_lock(sfz_loader_mutex);

  sfz_loader->flags |= flags;
  
  g_rec_mutex_unlock(sfz_loader_mutex);
}

/**
 * ags_sfz_loader_unset_flags:
 * @sfz_loader: the #AgsSFZLoader
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 3.0.0
 */
void
ags_sfz_loader_unset_flags(AgsSFZLoader *sfz_loader, guint flags)
{
  GRecMutex *sfz_loader_mutex;

  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return;
  }
  
  /* get sfz loader mutex */
  sfz_loader_mutex = AGS_SFZ_LOADER_GET_OBJ_MUTEX(sfz_loader);

  /* unset flags */
  g_rec_mutex_lock(sfz_loader_mutex);

  sfz_loader->flags &= (~flags);
  
  g_rec_mutex_unlock(sfz_loader_mutex);
}

void*
ags_sfz_loader_run(void *ptr)
{
  AgsChannel *start_input, *next_input, *input;
  AgsSFZGroup *group;
  AgsSFZRegion *region;      
  AgsSFZLoader *sfz_loader;
  
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
  
  sfz_loader = AGS_SFZ_LOADER(ptr);

  g_object_get(sfz_loader->audio,
	       "output-soundcard", &output_soundcard,
	       NULL);
  
  sfz_loader->audio_container = ags_audio_container_new(sfz_loader->filename,
							NULL,
							NULL,
							NULL,
							output_soundcard,
							-1);
  ags_audio_container_open(sfz_loader->audio_container);

  /* select */
  ags_sound_container_select_level_by_index(AGS_SOUND_CONTAINER(sfz_loader->audio_container->sound_container),
					    0);
  
  g_object_get(sfz_loader->audio,
	       "input", &start_input,
	       "input-pads", &n_pads,
	       "audio-channels", &n_audio_channels,
	       NULL);

  hikey = 0;
  lokey = 0;
  
  ags_sfz_file_get_range(AGS_SFZ_FILE(sfz_loader->audio_container->sound_container),
			 &lokey, &hikey);
  
  /* resize */
  g_object_get(sfz_loader->audio_container->sound_container,
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
    
    ags_audio_set_audio_channels(sfz_loader->audio,
				 n_audio_channels, 0);
  }
  
  current_pads = hikey - lokey + 1;
  
  if(lokey <= hikey &&
     current_pads > n_pads){
    n_pads = current_pads;
    
    ags_audio_set_pads(sfz_loader->audio,
		       AGS_TYPE_INPUT,
		       n_pads, 0);
  }
  
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
  
  ags_sfz_loader_set_flags(sfz_loader,
			   AGS_SFZ_LOADER_HAS_COMPLETED);
  
  g_thread_exit(NULL);

  return(NULL);
}

/**
 * ags_sfz_loader_get_audio:
 * @sfz_loader: the #AgsSFZLoader
 * 
 * Get audio of @sfz_loader.
 * 
 * Returns: the assigned #AgsAudio
 * 
 * Since: 3.2.0
 */
AgsAudio*
ags_sfz_loader_get_audio(AgsSFZLoader *sfz_loader)
{
  AgsAudio *audio;

  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return(NULL);
  }

  g_object_get(sfz_loader,
	       "audio", &audio,
	       NULL);

  return(audio);
}

/**
 * ags_sfz_loader_set_audio:
 * @sfz_loader: the #AgsSFZLoader
 * @audio: the #AgsAudio
 * 
 * Set audio of @sfz_loader.
 * 
 * Since: 3.2.0
 */
void
ags_sfz_loader_set_audio(AgsSFZLoader *sfz_loader,
			 AgsAudio *audio)
{
  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return;
  }

  g_object_set(sfz_loader,
	       "audio", audio,
	       NULL);
}

/**
 * ags_sfz_loader_get_filename:
 * @sfz_loader: the #AgsSFZLoader
 * 
 * Get filename of @sfz_loader.
 * 
 * Returns: the assigned filename
 * 
 * Since: 3.2.0
 */
gchar*
ags_sfz_loader_get_filename(AgsSFZLoader *sfz_loader)
{
  gchar *filename;
  
  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return(NULL);
  }

  g_object_get(sfz_loader,
	       "filename", &filename,
	       NULL);

  return(filename);
}

/**
 * ags_sfz_loader_set_filename:
 * @sfz_loader: the #AgsSFZLoader
 * @filename: the filename
 * 
 * Set filename of @sfz_loader.
 * 
 * Since: 3.2.0
 */
void
ags_sfz_loader_set_filename(AgsSFZLoader *sfz_loader,
			    gchar *filename)
{
  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return;
  }

  g_object_set(sfz_loader,
	       "filename", filename,
	       NULL);
}

/**
 * ags_sfz_loader_get_audio_container:
 * @sfz_loader: the #AgsSFZLoader
 * 
 * Get audio container of @sfz_loader.
 * 
 * Returns: the assigned #AgsAudioContainer
 * 
 * Since: 3.2.0
 */
AgsAudioContainer*
ags_sfz_loader_get_audio_container(AgsSFZLoader *sfz_loader)
{
  AgsAudioContainer *audio_container;
  
  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return(NULL);
  }

  g_object_get(sfz_loader,
	       "audio-container", &audio_container,
	       NULL);

  return(audio_container);
}

/**
 * ags_sfz_loader_set_audio_container:
 * @sfz_loader: the #AgsSFZLoader
 * @audio_container: the #AgsAudioContainer
 * 
 * Set audio container of @sfz_loader.
 * 
 * Since: 3.2.0
 */
void
ags_sfz_loader_set_audio_container(AgsSFZLoader *sfz_loader,
				   AgsAudioContainer *audio_container)
{
  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return;
  }

  g_object_set(sfz_loader,
	       "audio-container", audio_container,
	       NULL);
}

/**
 * ags_sfz_loader_start:
 * @sfz_loader: the #AgsSFZLoader
 * 
 * Start @sfz_loader.
 * 
 * Since: 3.0.0
 */
void
ags_sfz_loader_start(AgsSFZLoader *sfz_loader)
{
  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return;
  }
  
  sfz_loader->thread = g_thread_new("Advanced Gtk+ Sequencer - SFZ loader",
				    ags_sfz_loader_run,
				    sfz_loader);
}

/**
 * ags_sfz_loader_new:
 * @audio: the #AgsAudio
 * @filename: the filename
 * @do_replace: if %TRUE replace @audio's sfz
 *
 * Create a new instance of #AgsSFZLoader.
 *
 * Returns: the new #AgsSFZLoader
 *
 * Since: 3.0.0
 */ 
AgsSFZLoader*
ags_sfz_loader_new(AgsAudio *audio,
		   gchar *filename,
		   gboolean do_replace)
{
  AgsSFZLoader *sfz_loader;

  sfz_loader = (AgsSFZLoader *) g_object_new(AGS_TYPE_SFZ_LOADER,
					     "audio", audio,
					     "filename", filename,
					     NULL);

  if(do_replace){
    ags_sfz_loader_set_flags(sfz_loader,
			     AGS_SFZ_LOADER_DO_REPLACE);
  }
  
  return(sfz_loader);
}
