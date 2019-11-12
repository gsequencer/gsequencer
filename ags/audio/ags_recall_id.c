/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/ags_recall_id.h>

#include <stdlib.h>
#include <stdio.h>

#include <ags/i18n.h>

void ags_recall_id_class_init(AgsRecallIDClass *recall_id);
void ags_recall_id_init(AgsRecallID *recall_id);
void ags_recall_id_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_recall_id_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_recall_id_dispose(GObject *gobject);
void ags_recall_id_finalize(GObject *gobject);

/**
 * SECTION:ags_recall_id
 * @short_description: The object specifies run context.
 * @title: AgsRecallID
 * @section_id:
 * @include: ags/audio/ags_recall_id.h
 *
 * #AgsRecallID acts as dynamic context identifier.
 */

enum{
  PROP_0,
  PROP_RECYCLING_CONTEXT,
};

static gpointer ags_recall_id_parent_class = NULL;

GType
ags_recall_id_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_recall_id = 0;

    static const GTypeInfo ags_recall_id_info = {
      sizeof(AgsRecallIDClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_id_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsRecallID),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_id_init,
    };

    ags_type_recall_id = g_type_register_static(G_TYPE_OBJECT,
						"AgsRecallID",
						&ags_recall_id_info,
						0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_recall_id);
  }

  return g_define_type_id__volatile;
}

void
ags_recall_id_class_init(AgsRecallIDClass *recall_id)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_recall_id_parent_class = g_type_class_peek_parent(recall_id);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_id;

  gobject->set_property = ags_recall_id_set_property;
  gobject->get_property = ags_recall_id_get_property;

  gobject->dispose = ags_recall_id_dispose;
  gobject->finalize = ags_recall_id_finalize;

  /* properties */
  /**
   * AgsRecallID:recycling-context:
   *
   * The dynamic run context belonging to.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("recycling-context",
				   i18n_pspec("assigned recycling context"),
				   i18n_pspec("The recycling context it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECYCLING_CONTEXT,
				  param_spec);
}

void
ags_recall_id_init(AgsRecallID *recall_id)
{
  recall_id->flags = 0;
  recall_id->sound_scope = -1;
  recall_id->staging_flags = 0;
  recall_id->state_flags = 0;
  
  /* recall id mutex */
  g_rec_mutex_init(&(recall_id->obj_mutex)); 

  /* recycling context */
  recall_id->recycling_context = NULL;
}

void
ags_recall_id_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsRecallID *recall_id;

  GRecMutex *recall_id_mutex;

  recall_id = AGS_RECALL_ID(gobject);

  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  switch(prop_id){
  case PROP_RECYCLING_CONTEXT:
    {
      AgsRecyclingContext *recycling_context;

      recycling_context = g_value_get_object(value);

      g_rec_mutex_lock(recall_id_mutex);
      
      if(recall_id->recycling_context == recycling_context){
	g_rec_mutex_unlock(recall_id_mutex);

	return;
      }

      if(recall_id->recycling_context != NULL){
	g_object_unref(recall_id->recycling_context);
      }

      if(recycling_context != NULL){
	g_object_ref(recycling_context);
      }

      recall_id->recycling_context = recycling_context;

      g_rec_mutex_unlock(recall_id_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_id_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsRecallID *recall_id;

  GRecMutex *recall_id_mutex;

  recall_id = AGS_RECALL_ID(gobject);

  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  switch(prop_id){
  case PROP_RECYCLING_CONTEXT:
    {
      g_rec_mutex_lock(recall_id_mutex);

      g_value_set_object(value, recall_id->recycling_context);

      g_rec_mutex_unlock(recall_id_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_id_dispose(GObject *gobject)
{
  AgsRecallID *recall_id;

  recall_id = AGS_RECALL_ID(gobject);
  
  if(recall_id->recycling_context != NULL){
    g_object_unref(recall_id->recycling_context);

    recall_id->recycling_context = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_id_parent_class)->dispose(gobject);
}

void
ags_recall_id_finalize(GObject *gobject)
{
  AgsRecallID *recall_id;

  recall_id = AGS_RECALL_ID(gobject);
  
  if(recall_id->recycling_context != NULL){
    g_object_unref(recall_id->recycling_context);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_id_parent_class)->finalize(gobject);
}

/**
 * ags_recall_id_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 3.0.0
 */
GRecMutex*
ags_recall_id_get_class_mutex()
{
  return(&ags_recall_id_class_mutex);
}

/**
 * ags_recall_id_set_scope:
 * @recall_id: the #AgsRecallID
 * @sound_scope: the sound scope
 * 
 * Set @sound_scope for @recall_id.
 * 
 * Since: 3.0.0
 */
void
ags_recall_id_set_sound_scope(AgsRecallID *recall_id, gint sound_scope)
{
  GRecMutex *recall_id_mutex;

  if(!AGS_IS_RECALL_ID(recall_id) ||
     ags_recall_id_check_sound_scope(recall_id,
				     -1)){
    return;
  }

  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* set sound scope */
  g_rec_mutex_lock(recall_id_mutex);

  recall_id->sound_scope = sound_scope;

  g_rec_mutex_unlock(recall_id_mutex);
}

/**
 * ags_recall_id_check_sound_scope:
 * @recall_id: the #AgsRecallID
 * @sound_scope: the sound scope to check or -1 to check all
 * 
 * Check if @sound_scope is set for @recall_id.
 * 
 * Returns: %TRUE if sound scope matches, otherwise  %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_recall_id_check_sound_scope(AgsRecallID *recall_id, gint sound_scope)
{
  gint recall_id_sound_scope;
  
  GRecMutex *recall_id_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return(FALSE);
  }

  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* get sound scope */
  g_rec_mutex_lock(recall_id_mutex);

  recall_id_sound_scope = recall_id->sound_scope;

  g_rec_mutex_unlock(recall_id_mutex);
  
  if(sound_scope < 0){
    switch(recall_id_sound_scope){
    case AGS_SOUND_SCOPE_PLAYBACK:
    case AGS_SOUND_SCOPE_NOTATION:
    case AGS_SOUND_SCOPE_SEQUENCER:
    case AGS_SOUND_SCOPE_WAVE:
    case AGS_SOUND_SCOPE_MIDI:
      return(TRUE);      
    default:
      return(FALSE);
    }
  }else{
    if(sound_scope < AGS_SOUND_SCOPE_LAST &&
       sound_scope == recall_id_sound_scope){
      return(TRUE);
    }else{
      return(FALSE);
    }
  }
}

/**
 * ags_recall_id_test_staging_flags:
 * @recall_id: the #AgsRecallID
 * @staging_flags: the staging flags
 *
 * Test @staging_flags to be set on @recall_id.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_recall_id_test_staging_flags(AgsRecallID *recall_id,
				 guint staging_flags)
{
  gboolean retval;  
  
  GRecMutex *recall_id_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return(FALSE);
  }

  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* test */
  g_rec_mutex_lock(recall_id_mutex);

  retval = (staging_flags & (recall_id->staging_flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(recall_id_mutex);

  return(retval);
}

/**
 * ags_recall_id_set_staging_flags:
 * @recall_id: the #AgsRecallID
 * @staging_flags: staging flags to set
 * 
 * Set staging flags.
 * 
 * Since: 3.0.0
 */
void
ags_recall_id_set_staging_flags(AgsRecallID *recall_id, guint staging_flags)
{
  GRecMutex *recall_id_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* set staging flags */
  g_rec_mutex_lock(recall_id_mutex);

  recall_id->staging_flags |= staging_flags;

  g_rec_mutex_unlock(recall_id_mutex);
}

/**
 * ags_recall_id_unset_staging_flags:
 * @recall_id: the #AgsRecallID
 * @staging_flags: staging flags to unset
 * 
 * Unset staging flags.
 * 
 * Since: 3.0.0
 */
void
ags_recall_id_unset_staging_flags(AgsRecallID *recall_id, guint staging_flags)
{
  GRecMutex *recall_id_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* unset staging flags */
  g_rec_mutex_lock(recall_id_mutex);

  recall_id->staging_flags &= (~staging_flags);

  g_rec_mutex_unlock(recall_id_mutex);
}

/**
 * ags_recall_id_check_staging_flags:
 * @recall_id: the #AgsRecallID
 * @staging_flags: staging flags to check
 * 
 * Check the occurence of @staging_flags in @recall_id.
 * 
 * Returns: %TRUE if all flags matched, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_recall_id_check_staging_flags(AgsRecallID *recall_id, guint staging_flags)
{
  guint recall_id_staging_flags;
  
  GRecMutex *recall_id_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return(FALSE);
  }

  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* get staging flags */
  g_rec_mutex_lock(recall_id_mutex);

  recall_id_staging_flags = recall_id->staging_flags;

  g_rec_mutex_unlock(recall_id_mutex);

  /* check staging flags */
  if((AGS_SOUND_STAGING_CHECK_RT_DATA & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_CHECK_RT_DATA & (recall_id_staging_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STAGING_RUN_INIT_PRE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (recall_id_staging_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STAGING_RUN_INIT_INTER & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_INIT_INTER & (recall_id_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_RUN_INIT_POST & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_INIT_POST & (recall_id_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_FEED_INPUT_QUEUE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_FEED_INPUT_QUEUE & (recall_id_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_AUTOMATE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_AUTOMATE & (recall_id_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_RUN_PRE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_PRE & (recall_id_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_RUN_INTER & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_INTER & (recall_id_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_RUN_POST & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_POST & (recall_id_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_DO_FEEDBACK & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_DO_FEEDBACK & (recall_id_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE & (recall_id_staging_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STAGING_FINI & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_FINI & (recall_id_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_CANCEL & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_CANCEL & (recall_id_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_DONE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_DONE & (recall_id_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_REMOVE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_REMOVE & (recall_id_staging_flags)) == 0){
    return(FALSE);
  }
  
  return(TRUE);
}

/**
 * ags_recall_id_test_state_flags:
 * @recall_id: the #AgsRecallID
 * @state_flags: the state flags
 *
 * Test @state_flags to be set on @recall_id.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_recall_id_test_state_flags(AgsRecallID *recall_id,
			       guint state_flags)
{
  gboolean retval;  
  
  GRecMutex *recall_id_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return(FALSE);
  }

  /* get recall_id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* test */
  g_rec_mutex_lock(recall_id_mutex);

  retval = (state_flags & (recall_id->state_flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(recall_id_mutex);

  return(retval);
}

/**
 * ags_recall_id_set_state_flags:
 * @recall_id: the #AgsRecallID
 * @state_flags: state flags to set
 * 
 * Set state flags.
 * 
 * Since: 3.0.0
 */
void
ags_recall_id_set_state_flags(AgsRecallID *recall_id, guint state_flags)
{
  GRecMutex *recall_id_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* set state flags */
  g_rec_mutex_lock(recall_id_mutex);

  recall_id->state_flags |= state_flags;

  g_rec_mutex_unlock(recall_id_mutex);
}

/**
 * ags_recall_id_unset_state_flags:
 * @recall_id: the #AgsRecallID
 * @state_flags: state flags to unset
 * 
 * Unset state flags.
 * 
 * Since: 3.0.0
 */
void
ags_recall_id_unset_state_flags(AgsRecallID *recall_id, guint state_flags)
{
  GRecMutex *recall_id_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* unset state flags */
  g_rec_mutex_lock(recall_id_mutex);

  recall_id->state_flags &= (~state_flags);

  g_rec_mutex_unlock(recall_id_mutex);
}

/**
 * ags_recall_id_check_state_flags:
 * @recall_id: the #AgsRecallID
 * @state_flags: state flags to check
 * 
 * Check the occurence of @state_flags in @recall_id.
 * 
 * Returns: %TRUE if all flags matched, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_recall_id_check_state_flags(AgsRecallID *recall_id, guint state_flags)
{
  guint recall_id_state_flags;
  
  GRecMutex *recall_id_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return(FALSE);
  }
  
  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* get state flags */
  g_rec_mutex_lock(recall_id_mutex);

  recall_id_state_flags = recall_id->state_flags;
  
  g_rec_mutex_unlock(recall_id_mutex);

  /* check state flags */
  if((AGS_SOUND_STATE_IS_WAITING & (state_flags)) != 0 &&
     (AGS_SOUND_STATE_IS_WAITING & (recall_id_state_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STATE_IS_ACTIVE & (state_flags)) != 0 &&
     (AGS_SOUND_STATE_IS_ACTIVE & (recall_id_state_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STATE_IS_PROCESSING & (state_flags)) != 0 &&
     (AGS_SOUND_STATE_IS_PROCESSING & (recall_id_state_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STATE_IS_TERMINATING & (state_flags)) != 0 &&
     (AGS_SOUND_STATE_IS_TERMINATING & (recall_id_state_flags)) == 0){
    return(FALSE);
  }
  
  return(TRUE);
}

/**
 * ags_recall_id_find_recycling_context:
 * @recall_id: a #GList containing #AgsRecallID
 * @recycling_context: the #AgsRecyclingContext to match
 *
 * Retrieve recall id by recycling context.
 *
 * Returns: Matching recall id.
 * 
 * Since: 3.0.0
 */
AgsRecallID*
ags_recall_id_find_recycling_context(GList *recall_id,
				     AgsRecyclingContext *recycling_context)
{
  while(recall_id != NULL){
    AgsRecyclingContext *current_recycling_context;

    gboolean success;
    
    g_object_get(recall_id->data,
		 "recycling-context", &current_recycling_context,
		 NULL);

    success = (current_recycling_context == recycling_context) ? TRUE: FALSE;

    g_object_unref(current_recycling_context);
    
    if(success){
      return(recall_id->data);
    }

    recall_id = recall_id->next;
  }

  return(NULL);
}

/**
 * ags_recall_id_find_parent_recycling_context:
 * @recall_id: a #GList containing #AgsRecallID
 * @parent_recycling_context: the #AgsRecyclingContext to match
 *
 * Retrieve recall id by recycling context.
 *
 * Returns: Matching recall id.
 * 
 * Since: 3.0.0
 */
AgsRecallID*
ags_recall_id_find_parent_recycling_context(GList *recall_id,
					    AgsRecyclingContext *parent_recycling_context)
{
  while(recall_id != NULL){
    AgsRecyclingContext *current_parent_recycling_context;
    AgsRecyclingContext *current_recycling_context;

    gboolean success;

    success = FALSE;
    
    g_object_get(recall_id->data,
		 "recycling-context", &current_recycling_context,
		 NULL);

    if(current_recycling_context != NULL){
      g_object_get(current_recycling_context,
		   "parent", &current_parent_recycling_context,
		   NULL);
      
      if(current_parent_recycling_context == parent_recycling_context){
	success = TRUE;
      }

      /* unref */
      if(current_parent_recycling_context != NULL){
	g_object_unref(current_parent_recycling_context);
      }
      
      g_object_unref(current_recycling_context);
    }

    if(success){
      return(recall_id->data);
    }
    
    recall_id = recall_id->next;
  }

  return(NULL);
}

/**
 * ags_recall_id_new:
 *
 * Creates a #AgsRecallID
 *
 * Returns: a new #AgsRecallID
 * 
 * Since: 3.0.0
 */
AgsRecallID*
ags_recall_id_new()
{
  AgsRecallID *recall_id;

  recall_id = (AgsRecallID *) g_object_new(AGS_TYPE_RECALL_ID,
					   NULL);

  return(recall_id);
}
