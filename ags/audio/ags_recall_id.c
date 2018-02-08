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

#include <ags/audio/ags_recall_id.h>

#include <ags/object/ags_connectable.h>

#include <stdlib.h>
#include <stdio.h>

#include <ags/i18n.h>

void ags_recall_id_class_init(AgsRecallIDClass *recall_id);
void ags_recall_id_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_id_init(AgsRecallID *recall_id);
void ags_recall_id_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_recall_id_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_recall_id_connect(AgsConnectable *connectable);
void ags_recall_id_disconnect(AgsConnectable *connectable);
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
  static GType ags_type_recall_id = 0;

  if(!ags_type_recall_id){
    static const GTypeInfo ags_recall_id_info = {
      sizeof (AgsRecallIDClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_id_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallID),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_id_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_id_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_id = g_type_register_static(G_TYPE_OBJECT,
						"AgsRecallID",
						&ags_recall_id_info,
						0);

    g_type_add_interface_static(ags_type_recall_id,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_recall_id);
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
   * Since: 2.0.0
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
ags_recall_id_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_recall_id_connect;
  connectable->disconnect = ags_recall_id_disconnect;
}

void
ags_recall_id_init(AgsRecallID *recall_id)
{
  recall_id->flags = 0;
  recall_id->sound_scope = -1;
  recall_id->staging_flags = 0;
  recall_id->state_flags = 0;
  
  recall_id->recycling_context = NULL;
}

void
ags_recall_id_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsRecallID *recall_id;

  recall_id = AGS_RECALL_ID(gobject);

  switch(prop_id){
  case PROP_RECYCLING_CONTEXT:
    {
      AgsRecyclingContext *recycling_context;

      recycling_context = g_value_get_object(value);

      if(recall_id->recycling_context == recycling_context){
	return;
      }

      if(recall_id->recycling_context != NULL){
	g_object_unref(recall_id->recycling_context);
      }

      if(recycling_context != NULL){
	g_object_ref(recycling_context);
      }

      recall_id->recycling_context = recycling_context;
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

  recall_id = AGS_RECALL_ID(gobject);

  switch(prop_id){
  case PROP_RECYCLING_CONTEXT:
    {
      g_value_set_object(value, recall_id->recycling_context);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_id_connect(AgsConnectable *connectable)
{
  AgsRecallID *recall_id;

  recall_id = AGS_RECALL_ID(connectable);

  if((AGS_RECALL_ID_CONNECTED & (recall_id->flags)) != 0){
    return;
  }

  recall_id->flags |= AGS_RECALL_ID_CONNECTED;
}

void
ags_recall_id_disconnect(AgsConnectable *connectable)
{
  AgsRecallID *recall_id;

  recall_id = AGS_RECALL_ID(connectable);


  if((AGS_RECALL_ID_CONNECTED & (recall_id->flags)) == 0){
    return;
  }

  recall_id->flags &= (~AGS_RECALL_ID_CONNECTED);
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
 * ags_recall_id_set_scope:
 * @recall_id: the #AgsRecallID
 * @sound_scope: the sound scope
 * 
 * Set @sound_scope for @recall_id.
 * 
 * Since: 2.0.0
 */
void
ags_recall_id_set_sound_scope(AgsRecallID *recall_id, gint sound_scope)
{
  if(!AGS_IS_RECALL_ID(recall_id) &&
     ags_recall_id_check_scope(recall_id,
			       -1)){
    return;
  }

  recall_id->sound_scope = sound_scope;
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
 * Since: 2.0.0
 */
gboolean
ags_recall_id_check_sound_scope(AgsRecallID *recall_id, gint sound_scope)
{
  if(!AGS_IS_RECALL_ID(recall_id)){
    return(FALSE);
  }
  
  if(sound_scope < 0){
    switch(recall_id->sound_scope){
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
       sound_scope == recall_id->sound_scope){
      return(TRUE);
    }else{
      return(FALSE);
    }
  }
}

/**
 * ags_recall_id_set_staging_flags:
 * @recall_id: the #AgsRecallID
 * @staging_flags: staging flags to set
 * 
 * Set staging flags.
 * 
 * Since: 2.0.0
 */
void
ags_recall_id_set_staging_flags(AgsRecallID *recall_id, guint staging_flags)
{
  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  recall_id->staging_flags |= staging_flags;
}

/**
 * ags_recall_id_unset_staging_flags:
 * @recall_id: the #AgsRecallID
 * @staging_flags: staging flags to unset
 * 
 * Unset staging flags.
 * 
 * Since: 2.0.0
 */
void
ags_recall_id_unset_staging_flags(AgsRecallID *recall_id, guint staging_flags)
{
  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  recall_id->staging_flags &= (~staging_flags);
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
 * Since: 2.0.0
 */
gboolean
ags_recall_id_check_staging_flags(AgsRecallID *recall_id, guint staging_flags)
{
  if(!AGS_IS_RECALL_ID(recall_id)){
    return(FALSE);
  }

  if((AGS_SOUND_STAGING_CHECK_RT & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_CHECK_RT & (recall_id->staging_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STAGING_RUN_INIT_PRE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (recall_id->staging_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STAGING_RUN_INIT_INTER & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_INIT_INTER & (recall_id->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_RUN_INIT_POST & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_INIT_POST & (recall_id->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_FEED_INPUT_QUEUE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_FEED_INPUT_QUEUE & (recall_id->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_AUTOMATE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_AUTOMATE & (recall_id->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_RUN_PRE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_PRE & (recall_id->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_RUN_INTER & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_INTER & (recall_id->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_RUN_POST & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_POST & (recall_id->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_DO_FEEDBACK & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_DO_FEEDBACK & (recall_id->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE & (recall_id->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_CANCEL & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_CANCEL & (recall_id->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_DONE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_DONE & (recall_id->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_REMOVE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_REMOVE & (recall_id->staging_flags)) == 0){
    return(FALSE);
  }
  
  return(TRUE);
}

/**
 * ags_recall_id_set_state_flags:
 * @recall_id: the #AgsRecallID
 * @state_flags: state flags to set
 * 
 * Set state flags.
 * 
 * Since: 2.0.0
 */
void
ags_recall_id_set_state_flags(AgsRecallID *recall_id, guint state_flags)
{
  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  recall_id->state_flags |= state_flags;
}

/**
 * ags_recall_id_unset_state_flags:
 * @recall_id: the #AgsRecallID
 * @state_flags: state flags to unset
 * 
 * Unset state flags.
 * 
 * Since: 2.0.0
 */
void
ags_recall_id_unset_state_flags(AgsRecallID *recall_id, guint state_flags)
{
  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  recall_id->state_flags &= (~state_flags);
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
 * Since: 2.0.0
 */
gboolean
ags_recall_id_check_state_flags(AgsRecallID *recall_id, guint state_flags)
{
  if(!AGS_IS_RECALL_ID(recall_id)){
    return(FALSE);
  }

  if((AGS_SOUND_STATE_IS_WAITING & (state_flags)) != 0 &&
     (AGS_SOUND_STATE_IS_WAITING & (recall_id->state_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STATE_IS_ACTIVE & (state_flags)) != 0 &&
     (AGS_SOUND_STATE_IS_ACTIVE & (recall_id->state_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STATE_IS_PROCESSING & (state_flags)) != 0 &&
     (AGS_SOUND_STATE_IS_PROCESSING & (recall_id->state_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STATE_IS_TERMINATING & (state_flags)) != 0 &&
     (AGS_SOUND_STATE_IS_TERMINATING & (recall_id->state_flags)) == 0){
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
 * Since: 2.0.0
 */
AgsRecallID*
ags_recall_id_find_recycling_context(GList *recall_id,
				     AgsRecyclingContext *recycling_context)
{
  while(recall_id != NULL){
    if(AGS_RECALL_ID(recall_id->data)->recycling_context == recycling_context){
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
 * Since: 2.0.0
 */
AgsRecallID*
ags_recall_id_find_parent_recycling_context(GList *recall_id,
					    AgsRecyclingContext *parent_recycling_context)
{
  AgsRecallID *recall_id;

  while(recall_id != NULL){
    if(AGS_RECALL_ID(recall_id->data)->recycling_context != NULL &&
       AGS_RECALL_ID(recall_id->data)->recycling_context->parent == parent_recycling_context){
      return(recall_id);
    }

    recall_id = recall_id->next;
  }

  return(NULL);
}

/**
 * ags_recall_id_new:
 * @recycling: the assigned #AgsRecycling
 *
 * Creates a #AgsRecallID, assigned to @recycling
 *
 * Returns: a new #AgsRecallID
 * 
 * Since: 2.0.0
 */
AgsRecallID*
ags_recall_id_new(AgsRecycling *recycling)
{
  AgsRecallID *recall_id;

  recall_id = (AgsRecallID *) g_object_new(AGS_TYPE_RECALL_ID, NULL);

  return(recall_id);
}
