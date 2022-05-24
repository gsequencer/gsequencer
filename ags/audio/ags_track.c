/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/audio/ags_audio_signal.h>

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

enum{
  PROP_0,
  PROP_X,
  PROP_SMF_BUFFER,
};

static gpointer ags_track_parent_class = NULL;

GType
ags_track_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_track = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_track);
  }

  return g_define_type_id__volatile;
}

GType
ags_track_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_TRACK_IS_SELECTED, "AGS_TRACK_IS_SELECTED", "track-is-selected" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsTrackFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint64("x",
				   i18n_pspec("offset x"),
				   i18n_pspec("The x offset"),
				   0,
				   G_MAXUINT64,
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
   * Since: 3.0.0
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

  /* track mutex */
  g_rec_mutex_init(&(track->obj_mutex));

  /* fields */
  track->x = 0;

  track->smf_buffer = NULL;
  track->allocated_smf_buffer_length = 0;
  track->smf_buffer_length = 0;
}

void
ags_track_set_property(GObject *gobject,
		       guint prop_id,
		       const GValue *value,
		       GParamSpec *param_spec)
{
  AgsTrack *track;

  GRecMutex *track_mutex;

  track = AGS_TRACK(gobject);

  /* get track mutex */
  track_mutex = AGS_TRACK_GET_OBJ_MUTEX(track);

  switch(prop_id){
  case PROP_X:
    {
      g_rec_mutex_lock(track_mutex);

      track->x = g_value_get_uint64(value);

      g_rec_mutex_unlock(track_mutex);
    }
    break;
  case PROP_SMF_BUFFER:
    {
      g_rec_mutex_lock(track_mutex);

      track->smf_buffer = g_value_get_pointer(value);

      g_rec_mutex_unlock(track_mutex);
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

  GRecMutex *track_mutex;

  track = AGS_TRACK(gobject);

  /* get track mutex */
  track_mutex = AGS_TRACK_GET_OBJ_MUTEX(track);

  switch(prop_id){
  case PROP_X:
    {
      g_rec_mutex_lock(track_mutex);

      g_value_set_uint64(value,
			 track->x);

      g_rec_mutex_unlock(track_mutex);
    }
    break;
  case PROP_SMF_BUFFER:
    {
      g_rec_mutex_lock(track_mutex);

      g_value_set_pointer(value,
			  track->smf_buffer);
      
      g_rec_mutex_unlock(track_mutex);
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
 * ags_track_get_obj_mutex:
 * @track: the #AgsTrack
 * 
 * Get object mutex.
 * 
 * Returns: the #GRecMutex to lock @track
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_track_get_obj_mutex(AgsTrack *track)
{
  if(!AGS_IS_TRACK(track)){
    return(NULL);
  }

  return(AGS_TRACK_GET_OBJ_MUTEX(track));
}

/**
 * ags_track_lock:
 * @track: the #AgsTrack
 * 
 * Lock object mutex.
 * 
 * Since: 3.1.0
 */
void
ags_track_lock(AgsTrack *track)
{
  if(!AGS_IS_TRACK(track)){
    return;
  }

  g_rec_mutex_lock(AGS_TRACK_GET_OBJ_MUTEX(track));
}

/**
 * ags_track_unlock:
 * @track: the #AgsTrack
 * 
 * Unlock object mutex.
 * 
 * Since: 3.1.0
 */
void
ags_track_unlock(AgsTrack *track)
{
  if(!AGS_IS_TRACK(track)){
    return;
  }

  g_rec_mutex_unlock(AGS_TRACK_GET_OBJ_MUTEX(track));
}

/**
 * ags_track_test_flags:
 * @track: the #AgsTrack
 * @flags: the flags
 * 
 * Test @flags to be set on @track.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_track_test_flags(AgsTrack *track, guint flags)
{
  gboolean retval;
  
  GRecMutex *track_mutex;

  if(!AGS_IS_TRACK(track)){
    return(FALSE);
  }
      
  /* get track mutex */
  track_mutex = AGS_TRACK_GET_OBJ_MUTEX(track);

  /* test */
  g_rec_mutex_lock(track_mutex);

  retval = (flags & (track->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(track_mutex);

  return(retval);
}

/**
 * ags_track_set_flags:
 * @track: the #AgsTrack
 * @flags: the flags
 * 
 * Set @flags on @track.
 * 
 * Since: 3.0.0
 */
void
ags_track_set_flags(AgsTrack *track, guint flags)
{
  GRecMutex *track_mutex;

  if(!AGS_IS_TRACK(track)){
    return;
  }
      
  /* get track mutex */
  track_mutex = AGS_TRACK_GET_OBJ_MUTEX(track);

  /* set */
  g_rec_mutex_lock(track_mutex);

  track->flags |= flags;
  
  g_rec_mutex_unlock(track_mutex);
}

/**
 * ags_track_unset_flags:
 * @track: the #AgsTrack
 * @flags: the flags
 * 
 * Unset @flags on @track.
 * 
 * Since: 3.0.0
 */
void
ags_track_unset_flags(AgsTrack *track, guint flags)
{
  GRecMutex *track_mutex;

  if(!AGS_IS_TRACK(track)){
    return;
  }
      
  /* get track mutex */
  track_mutex = AGS_TRACK_GET_OBJ_MUTEX(track);

  /* unset */
  g_rec_mutex_lock(track_mutex);

  track->flags &= (~flags);
  
  g_rec_mutex_unlock(track_mutex);
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
 * Since: 3.0.0
 */
gint
ags_track_sort_func(gconstpointer a,
		    gconstpointer b)
{
  guint64 a_x, b_x ;

  if(a == NULL || b == NULL){
    return(0);
  }
    
  g_object_get(a,
	       "x", &a_x ,
	       NULL);
    
  g_object_get(b,
	       "x", &b_x ,
	       NULL);

  if(a_x == b_x){
    return(0);
  }

  if(a_x < b_x){
    return(-1);
  }else{
    return(1);
  }  
}

/**
 * ags_track_get_x:
 * @track: the #AgsTrack
 *
 * Gets x.
 * 
 * Returns: the x
 * 
 * Since: 3.1.0
 */
guint64
ags_track_get_x(AgsTrack *track)
{
  guint64 x;
  
  if(!AGS_IS_TRACK(track)){
    return(0);
  }

  g_object_get(track,
	       "x", &x,
	       NULL);

  return(x);
}

/**
 * ags_track_set_x:
 * @track: the #AgsTrack
 * @x: the x
 *
 * Sets x.
 * 
 * Since: 3.1.0
 */
void
ags_track_set_x(AgsTrack *track, guint64 x)
{
  if(!AGS_IS_TRACK(track)){
    return;
  }

  g_object_set(track,
	       "x", x,
	       NULL);
}

/**
 * ags_track_get_data:
 * @track: the #AgsTrack
 * @smf_buffer_length: (out): the SMF buffer length return location
 *
 * Gets data.
 * 
 * Returns: the data
 * 
 * Since: 3.1.0
 */
gpointer
ags_track_get_smf_buffer(AgsTrack *track,
			 guint *smf_buffer_length)
{
  gpointer smf_buffer;
  
  GRecMutex *track_mutex;

  if(!AGS_IS_TRACK(track)){
    return(NULL);
  }
      
  /* get track mutex */
  track_mutex = AGS_TRACK_GET_OBJ_MUTEX(track);

  /* set format */
  g_rec_mutex_lock(track_mutex);

  smf_buffer = track->smf_buffer;

  if(smf_buffer_length != NULL){
    smf_buffer_length[0] = track->smf_buffer_length;
  }
  
  g_rec_mutex_unlock(track_mutex);

  return(smf_buffer);
}

/**
 * ags_track_duplicate:
 * @track: an #AgsTrack
 * 
 * Duplicate a track.
 *
 * Returns: (transfer full): the duplicated #AgsTrack.
 *
 * Since: 3.0.0
 */
AgsTrack*
ags_track_duplicate(AgsTrack *track)
{
  AgsTrack *track_copy;

  guint copy_mode;

  GRecMutex *track_mutex;

  if(!AGS_IS_TRACK(track)){
    return(NULL);
  }
  
  /* get track mutex */
  track_mutex = AGS_TRACK_GET_OBJ_MUTEX(track);

  /* instantiate track */  
  track_copy = ags_track_new();

  track_copy->flags = 0;

  g_rec_mutex_lock(track_mutex);

  track_copy->x = track->x;

  //  g_object_set(track_copy,
  //	       NULL);

  //TODO:JK: implement me

  g_rec_mutex_unlock(track_mutex);

  return(track_copy);
}

/**
 * ags_track_alloc_smf_buffer:
 * @track: the #AgsTrack
 * @smf_buffer_length: SMF buffer length
 * 
 * Allocate SMF buffer of @track.
 * 
 * Returns: the newly allocated SMF buffer
 * 
 * Since: 3.6.17
 */
gpointer
ags_track_alloc_smf_buffer(AgsTrack *track,
			   guint smf_buffer_length)
{
  guchar *smf_buffer;

  GRecMutex *track_mutex;

  if(!AGS_IS_TRACK(track) ||
     smf_buffer_length == 0){
    return(NULL);
  }
  
  /* get track mutex */
  track_mutex = AGS_TRACK_GET_OBJ_MUTEX(track);
  
  smf_buffer = NULL;

  g_rec_mutex_lock(track_mutex);

  if(track->smf_buffer == NULL){
    smf_buffer =
      track->smf_buffer = (guchar *) g_malloc(smf_buffer_length * sizeof(guchar));

    track->allocated_smf_buffer_length = smf_buffer_length;
  }
  
  g_rec_mutex_lock(track_mutex);

  return(smf_buffer);
}

/**
 * ags_track_realloc_smf_buffer:
 * @track: the #AgsTrack
 * @smf_buffer_length: SMF buffer length
 * 
 * Reallocate SMF buffer of @track.
 * 
 * Returns: the reallocated SMF buffer
 * 
 * Since: 3.6.17
 */
gpointer
ags_track_realloc_smf_buffer(AgsTrack *track,
			     guint smf_buffer_length)
{
  guchar *smf_buffer;

  GRecMutex *track_mutex;

  if(!AGS_IS_TRACK(track) ||
     smf_buffer_length == 0){
    return(NULL);
  }
  
  /* get track mutex */
  track_mutex = AGS_TRACK_GET_OBJ_MUTEX(track);
  
  smf_buffer = NULL;

  g_rec_mutex_lock(track_mutex);

  if(track->smf_buffer != NULL){
    smf_buffer =
      track->smf_buffer = (guchar *) g_realloc(track->smf_buffer,
					       smf_buffer_length * sizeof(guchar));

    track->allocated_smf_buffer_length = smf_buffer_length;
  }
  
  g_rec_mutex_lock(track_mutex);

  return(smf_buffer);
}

/**
 * ags_track_new:
 *
 * Creates a new instance of #AgsTrack.
 *
 * Returns: the new #AgsTrack
 *
 * Since: 3.0.0
 */
AgsTrack*
ags_track_new()
{
  AgsTrack *track;

  track = (AgsTrack *) g_object_new(AGS_TYPE_TRACK,
				    NULL);

  return(track);
}
