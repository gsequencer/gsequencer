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

enum{
  PROP_0,
  PROP_X,
  PROP_SMF_BUFFER,
};

static gpointer ags_track_parent_class = NULL;

static pthread_mutex_t ags_track_class_mutex = PTHREAD_MUTEX_INITIALIZER;

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
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  track->flags = 0;

  /* add track mutex */
  track->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  track->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);  

  /* fields */
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

  pthread_mutex_t *track_mutex;

  track = AGS_TRACK(gobject);

  /* get track mutex */
  pthread_mutex_lock(ags_track_get_class_mutex());
  
  track_mutex = track->obj_mutex;
  
  pthread_mutex_unlock(ags_track_get_class_mutex());

  switch(prop_id){
  case PROP_X:
    {
      pthread_mutex_lock(track_mutex);

      track->x = g_value_get_uint64(value);

      pthread_mutex_unlock(track_mutex);
    }
    break;
  case PROP_SMF_BUFFER:
    {
      pthread_mutex_lock(track_mutex);

      track->smf_buffer = g_value_get_pointer(value);

      pthread_mutex_unlock(track_mutex);
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

  pthread_mutex_t *track_mutex;

  track = AGS_TRACK(gobject);

  /* get track mutex */
  pthread_mutex_lock(ags_track_get_class_mutex());
  
  track_mutex = track->obj_mutex;
  
  pthread_mutex_unlock(ags_track_get_class_mutex());

  switch(prop_id){
  case PROP_X:
    {
      pthread_mutex_lock(track_mutex);

      g_value_set_uint64(value,
			 track->x);

      pthread_mutex_unlock(track_mutex);
    }
    break;
  case PROP_SMF_BUFFER:
    {
      pthread_mutex_lock(track_mutex);

      g_value_set_pointer(value,
			  track->smf_buffer);
      
      pthread_mutex_unlock(track_mutex);
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

  pthread_mutex_destroy(track->obj_mutex);
  free(track->obj_mutex);

  pthread_mutexattr_destroy(track->obj_mutexattr);
  free(track->obj_mutexattr);

  if(track->smf_buffer != NULL){
    free(track->smf_buffer);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_track_parent_class)->finalize(gobject);
}

/**
 * ags_track_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_track_get_class_mutex()
{
  return(&ags_track_class_mutex);
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
 * Since: 2.0.0
 */
gboolean
ags_track_test_flags(AgsTrack *track, guint flags)
{
  gboolean retval;
  
  pthread_mutex_t *track_mutex;

  if(!AGS_IS_TRACK(track)){
    return(FALSE);
  }
      
  /* get track mutex */
  pthread_mutex_lock(ags_track_get_class_mutex());
  
  track_mutex = track->obj_mutex;
  
  pthread_mutex_unlock(ags_track_get_class_mutex());

  /* test */
  pthread_mutex_lock(track_mutex);

  retval = (flags & (track->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(track_mutex);

  return(retval);
}

/**
 * ags_track_set_flags:
 * @track: the #AgsTrack
 * @flags: the flags
 * 
 * Set @flags on @track.
 * 
 * Since: 2.0.0
 */
void
ags_track_set_flags(AgsTrack *track, guint flags)
{
  pthread_mutex_t *track_mutex;

  if(!AGS_IS_TRACK(track)){
    return;
  }
      
  /* get track mutex */
  pthread_mutex_lock(ags_track_get_class_mutex());
  
  track_mutex = track->obj_mutex;
  
  pthread_mutex_unlock(ags_track_get_class_mutex());

  /* set */
  pthread_mutex_lock(track_mutex);

  track->flags |= flags;
  
  pthread_mutex_unlock(track_mutex);
}

/**
 * ags_track_unset_flags:
 * @track: the #AgsTrack
 * @flags: the flags
 * 
 * Unset @flags on @track.
 * 
 * Since: 2.0.0
 */
void
ags_track_unset_flags(AgsTrack *track, guint flags)
{
  pthread_mutex_t *track_mutex;

  if(!AGS_IS_TRACK(track)){
    return;
  }
      
  /* get track mutex */
  pthread_mutex_lock(ags_track_get_class_mutex());
  
  track_mutex = track->obj_mutex;
  
  pthread_mutex_unlock(ags_track_get_class_mutex());

  /* unset */
  pthread_mutex_lock(track_mutex);

  track->flags &= (~flags);
  
  pthread_mutex_unlock(track_mutex);
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
 * ags_track_duplicate:
 * @track: an #AgsTrack
 * 
 * Duplicate a track.
 *
 * Returns: the duplicated #AgsTrack.
 *
 * Since: 2.0.0
 */
AgsTrack*
ags_track_duplicate(AgsTrack *track)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_track_new:
 *
 * Creates a new instance of #AgsTrack.
 *
 * Returns: the new #AgsTrack
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
