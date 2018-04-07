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

#include <ags/audio/ags_track.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_track_util.h>

#include <ags/i18n.h>

#include <stdlib.h>

void ags_track_class_init(AgsTrackClass *track);
void ags_track_init(AgsTrack *track);
void ags_track_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec);
void ags_track_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec);
void ags_track_finalize(GObject *gobject);

/**
 * SECTION:ags_track
 * @short_description: Track class.
 * @title: AgsTrack
 * @section_id:
 * @include: ags/audio/ags_track.h
 *
 * #AgsTrack represents a tone.
 */

static gpointer ags_track_parent_class = NULL;

enum{
  PROP_0,
  PROP_X,
  PROP_SMF_BUFFER,
};

GType
ags_track_get_type()
{
  static GType ags_type_track = 0;

  if(!ags_type_track){
    static const GTypeInfo ags_track_info = {
      sizeof(AgsTrackClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_track_class_init,
      NULL,
      NULL,
      sizeof(AgsTrack),
      0,
      (GInstanceInitFunc) ags_track_init,
    };

    ags_type_track = g_type_register_static(G_TYPE_OBJECT,
					    "AgsTrack",
					    &ags_track_info,
					    0);
  }

  return(ags_type_track);
}

void 
ags_track_class_init(AgsTrackClass *track)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_track_parent_class = g_type_class_peek_parent(track);

  gobject = (GObjectClass *) track;

  gobject->set_property = ags_track_set_property;
  gobject->get_property = ags_track_get_property;

  gobject->finalize = ags_track_finalize;

  /* properties */
  /**
   * AgsTrack:x:
   *
   * Track's x offset.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("x",
				 i18n_pspec("offset x"),
				 i18n_pspec("The x offset"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X,
				  param_spec);

  /**
   * AgsTrack:smf-buffer:
   *
   * Track's SMF buffer.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("smf-buffer",
				    i18n_pspec("SMF buffer"),
				    i18n_pspec("The SMF buffer"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SMF_BUFFER,
				  param_spec);
}

void
ags_track_init(AgsTrack *track)
{  
  track->flags = 0;

  track->x = 0;

  track->smf_buffer = NULL;
}

void
ags_track_set_property(GObject *gobject,
		       guint prop_id,
		       const GValue *value,
		       GParamSpec *param_spec)
{
  AgsTrack *track;

  track = AGS_TRACK(gobject);

  switch(prop_id){
  case PROP_X:
    {
      track->x = g_value_get_uint(value);
    }
    break;
  case PROP_SMF_BUFFER:
    {
      track->smf_buffer = g_value_get_pointer(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_track_get_property(GObject *gobject,
		       guint prop_id,
		       GValue *value,
		       GParamSpec *param_spec)
{
  AgsTrack *track;

  track = AGS_TRACK(gobject);

  switch(prop_id){
  case PROP_X:
    {
      g_value_set_uint(value,
		       track->x);
    }
    break;
  case PROP_SMF_BUFFER:
    {
      g_value_set_pointer(value,
			  track->smf_buffer);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_track_finalize(GObject *gobject)
{
  AgsTrack *track;

  track = AGS_TRACK(gobject);

  if(track->smf_buffer != NULL){
    free(track->smf_buffer);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_track_parent_class)->finalize(gobject);
}

/**
 * ags_track_sort_func:
 * @a: an #AgsTrack
 * @b: an #AgsTrack
 * 
 * Sort tracks.
 * 
 * Returns: 0 if equal, -1 if smaller and 1 if bigger offset
 *
 * Since: 2.0.0
 */
gint
ags_track_sort_func(gconstpointer a,
		    gconstpointer b)
{
  if(a == NULL || b == NULL){
    return(0);
  }
    
  if(AGS_TRACK(a)->x == AGS_TRACK(b)->x){
    return(0);
  }

  if(AGS_TRACK(a)->x < AGS_TRACK(b)->x){
    return(-1);
  }else{
    return(1);
  }  
}

AgsTrack*
ags_track_duplicate(AgsTrack *track)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_track_new:
 *
 * Creates an #AgsTrack
 *
 * Returns: a new #AgsTrack
 *
 * Since: 2.0.0
 */
AgsTrack*
ags_track_new()
{
  AgsTrack *track;

  track = (AgsTrack *) g_object_new(AGS_TYPE_TRACK,
				    NULL);

  return(track);
}
