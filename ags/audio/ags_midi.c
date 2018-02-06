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

#include <ags/audio/ags_midi.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>

#include <ags/i18n.h>

#include <errno.h>

void ags_midi_class_init(AgsMidiClass *midi);
void ags_midi_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_midi_init(AgsMidi *midi);
void ags_midi_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_midi_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);
void ags_midi_connect(AgsConnectable *connectable);
void ags_midi_disconnect(AgsConnectable *connectable);
void ags_midi_dispose(GObject *gobject);
void ags_midi_finalize(GObject *gobject);

/**
 * SECTION:ags_midi
 * @short_description: Midi class supporting selection and clipboard.
 * @title: AgsMidi
 * @section_id:
 * @include: ags/audio/ags_midi.h
 *
 * #AgsMidi acts as a container of #AgsTrack.
 */

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_AUDIO_CHANNEL,
  PROP_TRACK,
  PROP_TIMESTAMP,
};

static gpointer ags_midi_parent_class = NULL;

GType
ags_midi_get_type()
{
  static GType ags_type_midi = 0;

  if(!ags_type_midi){
    static const GTypeInfo ags_midi_info = {
      sizeof(AgsMidiClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_midi_class_init,
      NULL,
      NULL,
      sizeof(AgsMidi),
      0,
      (GInstanceInitFunc) ags_midi_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_midi_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_midi = g_type_register_static(G_TYPE_OBJECT,
					   "AgsMidi",
					   &ags_midi_info,
					   0);

    g_type_add_interface_static(ags_type_midi,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_midi);
}

void 
ags_midi_class_init(AgsMidiClass *midi)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_midi_parent_class = g_type_class_peek_parent(midi);

  gobject = (GObjectClass *) midi;

  gobject->set_property = ags_midi_set_property;
  gobject->get_property = ags_midi_get_property;

  gobject->dispose = ags_midi_dispose;
  gobject->finalize = ags_midi_finalize;

  /* properties */
  /**
   * AgsMidi:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of midi"),
				   i18n_pspec("The audio of midi"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsMidi:audio-channel:
   *
   * The effect's audio-channel.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_uint("audio-channel",
				  i18n_pspec("audio-channel of effect"),
				  i18n_pspec("The numerical audio-channel of effect"),
				  0,
				  65535,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);  

  /**
   * AgsMidi:track:
   *
   * The assigned #AgsTrack
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("track",
				   i18n_pspec("track of midi"),
				   i18n_pspec("The track of midi"),
				   AGS_TYPE_TRACK,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TRACK,
				  param_spec);
  
  /**
   * AgsPattern:timestamp:
   *
   * The pattern's timestamp.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("timestamp",
				   i18n_pspec("timestamp of pattern"),
				   i18n_pspec("The timestamp of pattern"),
				   AGS_TYPE_TIMESTAMP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMESTAMP,
				  param_spec);
}

void
ags_midi_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_midi_connect;
  connectable->disconnect = ags_midi_disconnect;
}

void
ags_midi_init(AgsMidi *midi)
{
  midi->flags = 0;

  midi->timestamp = ags_timestamp_new();

  midi->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  midi->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  midi->timestamp->timer.ags_offset.offset = 0;

  g_object_ref(midi->timestamp);
  
  midi->audio_channel = 0;
  midi->audio = NULL;
  
  midi->track = NULL;

  midi->loop_start = 0.0;
  midi->loop_end = 0.0;
  midi->offset = 0.0;

  midi->selection = NULL;
}

void
ags_midi_connect(AgsConnectable *connectable)
{
  AgsMidi *midi;

  GList *list;
  
  midi = AGS_MIDI(connectable);

  if((AGS_MIDI_CONNECTED & (midi->flags)) != 0){
    return;
  }

  midi->flags |= AGS_MIDI_CONNECTED;

  /* track */
  list = midi->track;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_midi_disconnect(AgsConnectable *connectable)
{
  AgsMidi *midi;

  GList *list;

  midi = AGS_MIDI(connectable);

  if((AGS_MIDI_CONNECTED & (midi->flags)) == 0){
    return;
  }

  midi->flags &= (~AGS_MIDI_CONNECTED);

  /* track */
  list = midi->track;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_midi_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  AgsMidi *midi;

  midi = AGS_MIDI(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(midi->audio == (GObject *) audio){
	return;
      }

      if(midi->audio != NULL){
	g_object_unref(midi->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      midi->audio = (GObject *) audio;
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      guint audio_channel;

      audio_channel = g_value_get_uint(value);

      midi->audio_channel = audio_channel;
    }
    break;
  case PROP_TRACK:
    {
      AgsTrack *track;

      track = (AgsTrack *) g_value_get_object(value);

      if(track == NULL ||
	 g_list_find(midi->track, track) != NULL){
	return;
      }

      ags_midi_add_track(midi,
			  track,
			  FALSE);
    }
    break;
  case PROP_TIMESTAMP:
    {
      AgsTimestamp *timestamp;

      timestamp = (AgsTimestamp *) g_value_get_object(value);

      if(timestamp == (AgsTimestamp *) midi->timestamp){
	return;
      }

      if(midi->timestamp != NULL){
	g_object_unref(G_OBJECT(midi->timestamp));
      }

      if(timestamp != NULL){
	g_object_ref(G_OBJECT(timestamp));
      }

      midi->timestamp = (GObject *) timestamp;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_get_property(GObject *gobject,
		      guint prop_id,
		      GValue *value,
		      GParamSpec *param_spec)
{
  AgsMidi *midi;

  midi = AGS_MIDI(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, midi->audio);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      g_value_set_uint(value, midi->audio_channel);
    }
    break;
  case PROP_TIMESTAMP:
    {
      g_value_set_object(value, midi->timestamp);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_dispose(GObject *gobject)
{
  AgsMidi *midi;

  GList *list;
  
  midi = AGS_MIDI(gobject);

  /* timestamp */
  if(midi->timestamp != NULL){
    g_object_unref(midi->timestamp);

    midi->timestamp = NULL;
  }

  /* audio */
  if(midi->audio != NULL){
    g_object_unref(midi->audio);

    midi->audio = NULL;
  }

    
  /* track and selection */
  list = midi->track;

  while(list != NULL){
    g_object_run_dispose(G_OBJECT(list->data));
    
    list = list->next;
  }
  
  g_list_free_full(midi->track,
		   g_object_unref);

  g_list_free(midi->selection);

  midi->track = NULL;
  midi->selection = NULL;
    
  /* call parent */
  G_OBJECT_CLASS(ags_midi_parent_class)->dispose(gobject);
}

void
ags_midi_finalize(GObject *gobject)
{
  AgsMidi *midi;

  midi = AGS_MIDI(gobject);

  /* timestamp */
  if(midi->timestamp != NULL){
    g_object_unref(midi->timestamp);
  }

  /* audio */
  if(midi->audio != NULL){
    g_object_unref(midi->audio);
  }
    
  /* track and selection */
  g_list_free_full(midi->track,
		   g_object_unref);

  g_list_free(midi->selection);
  
  /* call parent */
  G_OBJECT_CLASS(ags_midi_parent_class)->finalize(gobject);
}

/**
 * ags_midi_find_near_timestamp:
 * @midi: a #GList containing #AgsMidi
 * @audio_channel: the matching audio channel
 * @timestamp: (allow-none): the matching timestamp, or %NULL to match any timestamp
 *
 * Retrieve appropriate midi for timestamp.
 *
 * Returns: Next match.
 *
 * Since: 2.0.0
 */
GList*
ags_midi_find_near_timestamp(GList *midi, guint audio_channel,
			     AgsTimestamp *timestamp)
{
  AgsTimestamp *current_timestamp;

  while(midi != NULL){
    if(AGS_MIDI(midi->data)->audio_channel != audio_channel){
      midi = midi->next;
      
      continue;
    }

    if(timestamp == NULL){
      return(midi);
    }
    
    current_timestamp = (AgsTimestamp *) AGS_MIDI(midi->data)->timestamp;

    if(current_timestamp != NULL){
      if((AGS_TIMESTAMP_UNIX & (timestamp->flags)) != 0 &&
	 (AGS_TIMESTAMP_UNIX & (current_timestamp->flags)) != 0){
	if(current_timestamp->timer.unix_time.time_val >= timestamp->timer.unix_time.time_val &&
	   current_timestamp->timer.unix_time.time_val < timestamp->timer.unix_time.time_val + AGS_MIDI_DEFAULT_DURATION){
	  return(midi);
	}
      }else if((AGS_TIMESTAMP_OFFSET & (timestamp->flags)) != 0 &&
	       (AGS_TIMESTAMP_OFFSET & (current_timestamp->flags)) != 0){
	if(current_timestamp->timer.ags_offset.offset >= timestamp->timer.ags_offset.offset &&
	   current_timestamp->timer.ags_offset.offset < timestamp->timer.ags_offset.offset + AGS_MIDI_DEFAULT_OFFSET){
	  return(midi);
	}
      }
    }
    
    midi = midi->next;
  }
  
  return(NULL);
}

/**
 * ags_midi_add:
 * @midi: the #GList-struct containing #AgsMidi
 * @new_midi: the midi to add
 * 
 * Add @new_midi sorted to @midi
 * 
 * Returns: the new beginning of @midi
 * 
 * Since: 2.0.0
 */
GList*
ags_midi_add(GList *midi,
	     AgsMidi *new_midi)
{
  auto gint ags_midi_add_compare(gconstpointer a,
				 gconstpointer b);
  
  gint ags_midi_add_compare(gconstpointer a,
			    gconstpointer b)
  {
    if(AGS_MIDI(a)->timestamp->timer.ags_offset.offset == AGS_MIDI(b)->timestamp->timer.ags_offset.offset){
      return(0);
    }else if(AGS_MIDI(a)->timestamp->timer.ags_offset.offset < AGS_MIDI(b)->timestamp->timer.ags_offset.offset){
      return(-1);
    }else if(AGS_MIDI(a)->timestamp->timer.ags_offset.offset > AGS_MIDI(b)->timestamp->timer.ags_offset.offset){
      return(1);
    }

    return(0);
  }
  
  if(!AGS_IS_MIDI(new_midi) ||
     !AGS_IS_TIMESTAMP(new_midi->timestamp)){
    return(midi);
  }
  
  midi = g_list_insert_sorted(midi,
			      new_midi,
			      ags_midi_add_compare);
  
  return(midi);
}

/**
 * ags_midi_add_track:
 * @midi: an #AgsMidi
 * @track: the #AgsTrack to add
 * @use_selection_list: if %TRUE add to selection, else to default midi
 *
 * Adds a track to midi.
 *
 * Since: 2.0.0
 */
void
ags_midi_add_track(AgsMidi *midi,
		    AgsTrack *track,
		    gboolean use_selection_list)
{
  if(!AGS_IS_MIDI(midi) ||
     !AGS_IS_TRACK(track)){
    return;
  }

  g_object_ref(track);
  
  if(use_selection_list){
    midi->selection = g_list_insert_sorted(midi->selection,
					   track,
					   (GCompareFunc) ags_track_sort_func);
  }else{
    midi->track = g_list_insert_sorted(midi->track,
					track,
					(GCompareFunc) ags_track_sort_func);
  }
}

/**
 * ags_midi_remove_track:
 * @midi: an #AgsMidi
 * @track: the #AgsTrack to remove
 * @use_selection_list: if %TRUE remove from selection, else from default midi
 *
 * Removes a track from midi.
 *
 * Since: 2.0.0
 */
void
ags_midi_remove_track(AgsMidi *midi,
		       AgsTrack *track,
		       gboolean use_selection_list)
{
  if(!AGS_IS_MIDI(midi) ||
     !AGS_IS_TRACK(track)){
    return;
  }
  
  if(!use_selection_list){
    midi->track = g_list_remove(midi->track,
				 track);
  }else{
    midi->selection = g_list_remove(midi->selection,
				    track);
  }
}

/**
 * ags_midi_new:
 * @audio: the assigned #AgsAudio
 * @audio_channel: the audio channel to be used
 *
 * Creates a #AgsMidi, assigned to @audio_channel.
 *
 * Returns: a new #AgsMidi
 *
 * Since: 2.0.0
 */
AgsMidi*
ags_midi_new(GObject *audio,
	     guint audio_channel)
{
  AgsMidi *midi;
  
  midi = (AgsMidi *) g_object_new(AGS_TYPE_MIDI,
				  "audio", audio,
				  "audio-channel", audio_channel,
				  NULL);
  
  return(midi);
}
