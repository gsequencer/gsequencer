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

#include <ags/audio/ags_notation.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_port.h>

#include <pthread.h>

#include <stdlib.h>

#include <ags/i18n.h>

#include <errno.h>

void ags_notation_class_init(AgsNotationClass *notation);
void ags_notation_init(AgsNotation *notation);
void ags_notation_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec);
void ags_notation_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec);
void ags_notation_dispose(GObject *gobject);
void ags_notation_finalize(GObject *gobject);

void ags_notation_insert_native_piano_from_clipboard(AgsNotation *notation,
						     xmlNode *root_node, char *version,
						     char *base_frequency,
						     char *x_boundary, char *y_boundary,
						     gboolean from_x_offset, guint x_offset,
						     gboolean from_y_offset, guint y_offset,
						     gboolean match_channel, gboolean no_duplicates);

/**
 * SECTION:ags_notation
 * @short_description: Notation class supporting selection and clipboard.
 * @title: AgsNotation
 * @section_id:
 * @include: ags/audio/ags_notation.h
 *
 * #AgsNotation acts as a container of #AgsNote.
 */

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_AUDIO_CHANNEL,
  PROP_TIMESTAMP,
  PROP_NOTE,
};

static gpointer ags_notation_parent_class = NULL;

static pthread_mutex_t ags_notation_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_notation_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_notation = 0;

    static const GTypeInfo ags_notation_info = {
      sizeof(AgsNotationClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_notation_class_init,
      NULL,
      NULL,
      sizeof(AgsNotation),
      0,
      (GInstanceInitFunc) ags_notation_init,
    };

    ags_type_notation = g_type_register_static(G_TYPE_OBJECT,
					       "AgsNotation",
					       &ags_notation_info,
					       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_notation);
  }

  return g_define_type_id__volatile;
}

void 
ags_notation_class_init(AgsNotationClass *notation)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_notation_parent_class = g_type_class_peek_parent(notation);

  gobject = (GObjectClass *) notation;

  gobject->set_property = ags_notation_set_property;
  gobject->get_property = ags_notation_get_property;

  gobject->dispose = ags_notation_dispose;
  gobject->finalize = ags_notation_finalize;

  /* properties */
  /**
   * AgsNotation:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of notation"),
				   i18n_pspec("The audio of notation"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);


  /**
   * AgsNotation:audio-channel:
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
   * AgsNotation:timestamp:
   *
   * The notation's timestamp.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("timestamp",
				   i18n_pspec("timestamp of notation"),
				   i18n_pspec("The timestamp of notation"),
				   AGS_TYPE_TIMESTAMP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMESTAMP,
				  param_spec);
  
  /**
   * AgsNotation:note:
   *
   * The assigned #AgsNote
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("note",
				    i18n_pspec("note of notation"),
				    i18n_pspec("The note of notation"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTE,
				  param_spec);   
}

void
ags_notation_init(AgsNotation *notation)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  notation->flags = 0;

  /* add notation mutex */
  notation->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  notation->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);  

  /* fields */
  notation->audio = NULL;
  notation->audio_channel = 0;

  notation->timestamp = ags_timestamp_new();

  notation->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  notation->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  notation->timestamp->timer.ags_offset.offset = 0;

  g_object_ref(notation->timestamp);

  notation->maximum_note_length = AGS_NOTATION_MAXIMUM_NOTE_LENGTH;

  notation->note = NULL;
  notation->selection = NULL;
}

void
ags_notation_set_property(GObject *gobject,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *param_spec)
{
  AgsNotation *notation;

  pthread_mutex_t *notation_mutex;

  notation = AGS_NOTATION(gobject);

  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      pthread_mutex_lock(notation_mutex);

      if(notation->audio == (GObject *) audio){
	pthread_mutex_unlock(notation_mutex);

	return;
      }

      if(notation->audio != NULL){
	g_object_unref(notation->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      notation->audio = (GObject *) audio;

      pthread_mutex_unlock(notation_mutex);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      guint audio_channel;

      audio_channel = g_value_get_uint(value);

      pthread_mutex_lock(notation_mutex);

      notation->audio_channel = audio_channel;

      pthread_mutex_unlock(notation_mutex);
    }
    break;
  case PROP_TIMESTAMP:
    {
      AgsTimestamp *timestamp;

      timestamp = (AgsTimestamp *) g_value_get_object(value);

      pthread_mutex_lock(notation_mutex);

      if(timestamp == (AgsTimestamp *) notation->timestamp){
	pthread_mutex_unlock(notation_mutex);
	
	return;
      }

      if(notation->timestamp != NULL){
	g_object_unref(G_OBJECT(notation->timestamp));
      }

      if(timestamp != NULL){
	g_object_ref(G_OBJECT(timestamp));
      }

      notation->timestamp = (GObject *) timestamp;

      pthread_mutex_unlock(notation_mutex);
    }
    break;
  case PROP_NOTE:
    {
      AgsNote *note;

      note = (AgsNote *) g_value_get_object(value);

      pthread_mutex_lock(notation_mutex);

      if(note == NULL ||
	 g_list_find(notation->note, note) != NULL){
	pthread_mutex_unlock(notation_mutex);
	
	return;
      }

      pthread_mutex_unlock(notation_mutex);

      ags_notation_add_note(notation,
			    note,
			    FALSE);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_notation_get_property(GObject *gobject,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *param_spec)
{
  AgsNotation *notation;

  pthread_mutex_t *notation_mutex;

  notation = AGS_NOTATION(gobject);

  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  switch(prop_id){
  case PROP_AUDIO:
    {
      pthread_mutex_lock(notation_mutex);

      g_value_set_object(value, notation->audio);

      pthread_mutex_unlock(notation_mutex);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      pthread_mutex_lock(notation_mutex);

      g_value_set_uint(value, notation->audio_channel);

      pthread_mutex_unlock(notation_mutex);
    }
    break;
  case PROP_TIMESTAMP:
    {
      pthread_mutex_lock(notation_mutex);

      g_value_set_object(value, notation->timestamp);

      pthread_mutex_unlock(notation_mutex);
    }
    break;
  case PROP_NOTE:
    {
      pthread_mutex_lock(notation_mutex);

      g_value_set_pointer(value, g_list_copy(notation->note));

      pthread_mutex_unlock(notation_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_notation_dispose(GObject *gobject)
{
  AgsNotation *notation;

  GList *list;
  
  notation = AGS_NOTATION(gobject);

  /* audio */
  if(notation->audio != NULL){
    g_object_unref(notation->audio);

    notation->audio = NULL;
  }

  /* timestamp */
  if(notation->timestamp != NULL){
    g_object_unref(notation->timestamp);

    notation->timestamp = NULL;
  }
    
  /* note and selection */
  list = notation->note;

  while(list != NULL){
    g_object_run_dispose(G_OBJECT(list->data));
    
    list = list->next;
  }
  
  g_list_free_full(notation->note,
		   g_object_unref);
  g_list_free_full(notation->selection,
		   g_object_unref);

  notation->note = NULL;
  notation->selection = NULL;
    
  /* call parent */
  G_OBJECT_CLASS(ags_notation_parent_class)->dispose(gobject);
}

void
ags_notation_finalize(GObject *gobject)
{
  AgsNotation *notation;

  notation = AGS_NOTATION(gobject);

  pthread_mutex_destroy(notation->obj_mutex);
  free(notation->obj_mutex);

  pthread_mutexattr_destroy(notation->obj_mutexattr);
  free(notation->obj_mutexattr);

  /* audio */
  if(notation->audio != NULL){
    g_object_unref(notation->audio);
  }

  /* timestamp */
  if(notation->timestamp != NULL){
    g_object_unref(notation->timestamp);
  }
    
  /* note and selection */
  g_list_free_full(notation->note,
		   g_object_unref);

  g_list_free_full(notation->selection,
		   g_object_unref);
  
  /* call parent */
  G_OBJECT_CLASS(ags_notation_parent_class)->finalize(gobject);
}

/**
 * ags_notation_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_notation_get_class_mutex()
{
  return(&ags_notation_class_mutex);
}

/**
 * ags_notation_test_flags:
 * @notation: the #AgsNotation
 * @flags: the flags
 * 
 * Test @flags to be set on @notation.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 2.0.0
 */
gboolean
ags_notation_test_flags(AgsNotation *notation, guint flags)
{
  gboolean retval;
  
  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation)){
    return(FALSE);
  }
      
  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  /* test */
  pthread_mutex_lock(notation_mutex);

  retval = (flags & (notation->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(notation_mutex);

  return(retval);
}

/**
 * ags_notation_set_flags:
 * @notation: the #AgsNotation
 * @flags: the flags
 * 
 * Set @flags on @notation.
 * 
 * Since: 2.0.0
 */
void
ags_notation_set_flags(AgsNotation *notation, guint flags)
{
  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation)){
    return;
  }
      
  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  /* set */
  pthread_mutex_lock(notation_mutex);

  notation->flags |= flags;
  
  pthread_mutex_unlock(notation_mutex);
}

/**
 * ags_notation_unset_flags:
 * @notation: the #AgsNotation
 * @flags: the flags
 * 
 * Unset @flags on @notation.
 * 
 * Since: 2.0.0
 */
void
ags_notation_unset_flags(AgsNotation *notation, guint flags)
{
  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation)){
    return;
  }
      
  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  /* set */
  pthread_mutex_lock(notation_mutex);

  notation->flags &= (~flags);
  
  pthread_mutex_unlock(notation_mutex);
}

/**
 * ags_notation_find_near_timestamp:
 * @notation: a #GList containing #AgsNotation
 * @audio_channel: the matching audio channel
 * @timestamp: (allow-none): the matching timestamp, or %NULL to match any timestamp
 *
 * Retrieve appropriate notation for timestamp.
 *
 * Returns: Next matching #GList-struct or %NULL if not found
 *
 * Since: 2.0.0
 */
GList*
ags_notation_find_near_timestamp(GList *notation, guint audio_channel,
				 AgsTimestamp *timestamp)
{
  AgsTimestamp *current_timestamp;

  guint current_audio_channel;

  while(notation != NULL){
    g_object_get(notation->data,
		 "audio-channel", &current_audio_channel,
		 NULL);
    
    if(current_audio_channel != audio_channel){
      notation = notation->next;
      
      continue;
    }

    if(timestamp == NULL){
      return(notation);
    }
    
    g_object_get(notation->data,
		 "timestamp", &current_timestamp,
		 NULL);
    
    if(current_timestamp != NULL){
      if(ags_timestamp_test_flags(timestamp,
				  AGS_TIMESTAMP_OFFSET) &&
	 ags_timestamp_test_flags(current_timestamp,
				  AGS_TIMESTAMP_OFFSET)){
	if(ags_timestamp_get_ags_offset(current_timestamp) >= ags_timestamp_get_ags_offset(timestamp) &&
	   ags_timestamp_get_ags_offset(current_timestamp) < ags_timestamp_get_ags_offset(timestamp) + AGS_NOTATION_DEFAULT_OFFSET){
	  return(notation);
	}
      }else if(ags_timestamp_test_flags(timestamp,
					AGS_TIMESTAMP_UNIX) &&
	       ags_timestamp_test_flags(current_timestamp,
					AGS_TIMESTAMP_UNIX)){
	if(ags_timestamp_get_unix_time(current_timestamp) >= ags_timestamp_get_unix_time(timestamp) &&
	   ags_timestamp_get_unix_time(current_timestamp) < ags_timestamp_get_unix_time(timestamp) + AGS_NOTATION_DEFAULT_DURATION){
	  return(notation);
	}
      }
    }
    
    notation = notation->next;
  }

  return(NULL);
}

/**
 * ags_notation_add:
 * @notation: the #GList-struct containing #AgsNotation
 * @new_notation: the #AgsNotation to add
 * 
 * Add @new_notation sorted to @notation
 * 
 * Returns: the new beginning of @notation
 * 
 * Since: 2.0.0
 */
GList*
ags_notation_add(GList *notation,
		 AgsNotation *new_notation)
{
  auto gint ags_notation_add_compare(gconstpointer a,
				     gconstpointer b);
  
  gint ags_notation_add_compare(gconstpointer a,
				gconstpointer b)
  {
    AgsTimestamp *timestamp_a, *timestamp_b;

    g_object_get(a,
		 "timestamp", &timestamp_a,
		 NULL);

    g_object_get(b,
		 "timestamp", &timestamp_b,
		 NULL);
    
    if(ags_timestamp_get_ags_offset(timestamp_a) == ags_timestamp_get_ags_offset(timestamp_b)){
      return(0);
    }else if(ags_timestamp_get_ags_offset(timestamp_a) < ags_timestamp_get_ags_offset(timestamp_b)){
      return(-1);
    }else if(ags_timestamp_get_ags_offset(timestamp_a) > ags_timestamp_get_ags_offset(timestamp_b)){
      return(1);
    }

    return(0);
  }
  
  if(!AGS_IS_NOTATION(new_notation)){
    return(notation);
  }
  
  notation = g_list_insert_sorted(notation,
				  new_notation,
				  ags_notation_add_compare);
  
  return(notation);
}

/**
 * ags_notation_add_note:
 * @notation: the #AgsNotation
 * @note: the #AgsNote to add
 * @use_selection_list: if %TRUE add to selection, else to default notation
 *
 * Adds @note to @notation.
 *
 * Since: 2.0.0
 */
void
ags_notation_add_note(AgsNotation *notation,
		      AgsNote *note,
		      gboolean use_selection_list)
{
  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation) ||
     !AGS_IS_NOTE(note)){
    return;
  }

  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  /* insert sorted */
  g_object_ref(note);
  
  pthread_mutex_lock(notation_mutex);

  if(use_selection_list){
    notation->selection = g_list_insert_sorted(notation->selection,
					       note,
					       (GCompareFunc) ags_note_sort_func);
    ags_note_set_flags(note,
		       AGS_NOTE_IS_SELECTED);
  }else{
    notation->note = g_list_insert_sorted(notation->note,
					  note,
					  (GCompareFunc) ags_note_sort_func);
  }

  pthread_mutex_unlock(notation_mutex);
}

/**
 * ags_notation_remove_note:
 * @notation: the #AgsNotation
 * @note: the #AgsNote to remove
 * @use_selection_list: if %TRUE remove from selection, else from default notation
 *
 * Removes @note from @notation.
 *
 * Since: 2.0.0
 */
void
ags_notation_remove_note(AgsNotation *notation,
			 AgsNote *note,
			 gboolean use_selection_list)
{
  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation) ||
     !AGS_IS_NOTE(note)){
    return;
  }

  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  /* remove if found */
  pthread_mutex_lock(notation_mutex);
  
  if(!use_selection_list){
    if(g_list_find(notation->note,
		   note) != NULL){
      notation->note = g_list_remove(notation->note,
				     note);
      g_object_unref(note);
    }
  }else{
    if(g_list_find(notation->selection,
		   note) != NULL){
      notation->selection = g_list_remove(notation->selection,
					  note);
      g_object_unref(note);
    }
  }

  pthread_mutex_unlock(notation_mutex);
}

/**
 * ags_notation_remove_note_at_position:
 * @notation: the #AgsNotation
 * @x: offset
 * @y: note
 *
 * Removes one #AgsNote of notation.
 *
 * Returns: %TRUE if successfully removed note.
 *
 * Since: 2.0.0
 */
gboolean
ags_notation_remove_note_at_position(AgsNotation *notation,
				     guint x, guint y)
{
  AgsNote *note;
  
  GList *start_list, *list;

  guint current_x0, current_y;
  gboolean retval;

  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation)){
    return(FALSE);
  }

  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  /* find note */
  pthread_mutex_lock(notation_mutex);

  list =
    start_list = ags_list_util_copy_and_ref(notation->note);
  
  pthread_mutex_unlock(notation_mutex);

  note = NULL;

  retval = FALSE;
  
  while(list != NULL){
    g_object_get(list->data,
		 "x0", &current_x0,
		 "y", &current_y,
		 NULL);

    if(current_x0 == x &&
       current_y == y){
      note = list->data;
      
      retval = TRUE;
      
      break;
    }

    if(current_x0 > x){
      break;
    }
    
    list = list->next;
  }

  /* delete link and unref */
  if(retval){
    pthread_mutex_lock(notation_mutex);
    
    notation->note = g_list_remove(notation->note,
				   note);
    g_object_unref(note);

    pthread_mutex_unlock(notation_mutex);
  }

  g_list_free_full(start_list,
		   g_object_unref);

  return(retval);
}

/**
 * ags_notation_get_selection:
 * @notation: the #AgsNotation
 *
 * Retrieve selection.
 *
 * Returns: the selection.
 *
 * Since: 2.0.0
 */
GList*
ags_notation_get_selection(AgsNotation *notation)
{
  GList *selection;

  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation)){
    return(NULL);
  }

  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  /* selection */
  pthread_mutex_lock(notation_mutex);

  selection = notation->selection;
  
  pthread_mutex_unlock(notation_mutex);
  
  return(selection);
}

/**
 * ags_notation_is_note_selected:
 * @notation: the #AgsNotation
 * @note: the #AgsNote to check for
 *
 * Check selection for note.
 *
 * Returns: %TRUE if selected otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_notation_is_note_selected(AgsNotation *notation, AgsNote *note)
{
  GList *selection;

  guint x0;
  guint current_x0;
  gboolean retval;
  
  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation) ||
     !AGS_IS_NOTE(note)){
    return(FALSE);
  }

  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  /* get x */
  g_object_get(note,
	       "x0", &x0,
	       NULL);
  
  /* match note */
  pthread_mutex_lock(notation_mutex);

  selection = notation->selection;
  retval = FALSE;
  
  while(selection != NULL){
    /* get current x */
    g_object_get(selection->data,
		 "x0", &current_x0,
		 NULL);

    if(current_x0 > x0){
      break;
    }
    
    if(selection->data == note){
      retval = TRUE;

      break;
    }

    selection = selection->next;
  }

  pthread_mutex_unlock(notation_mutex);

  return(retval);
}

/**
 * ags_notation_find_point:
 * @notation: the #AgsNotation
 * @x: offset
 * @y: note
 * @use_selection_list: if %TRUE selection is searched
 *
 * Find note by offset and tone.
 *
 * Returns: the matching note.
 *
 * Since: 2.0.0
 */ 
AgsNote*
ags_notation_find_point(AgsNotation *notation,
			guint x, guint y,
			gboolean use_selection_list)
{
  AgsNote *retval;
  
  GList *note;

  guint current_x0, current_x1, current_y;

  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation)){
    return(NULL);
  }

  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  /* find note */
  pthread_mutex_lock(notation_mutex);

  if(use_selection_list){
    note = notation->selection;
  }else{
    note = notation->note;
  }

  retval = NULL;
  
  while(note != NULL){
    g_object_get(note->data,
		 "x0", &current_x0,
		 "x1", &current_x1,
		 "y", &current_y,
		 NULL);
    
    if(current_x0 > x){
      break;
    }

    if(x >= current_x0 &&
       x < current_x1 &&
       current_y == y){
      retval = note->data;

      break;
    }
    
    note = note->next;
  }

  pthread_mutex_unlock(notation_mutex);

  return(retval);
}

/**
 * ags_notation_find_region:
 * @notation: the #AgsNotation
 * @x0: start offset
 * @y0: start tone
 * @x1: end offset
 * @y1: end tone
 * @use_selection_list: if %TRUE selection is searched
 *
 * Find note by offset and tone region.
 *
 * Returns: the matching notes as #GList-struct
 *
 * Since: 2.0.0
 */
GList*
ags_notation_find_region(AgsNotation *notation,
			 guint x0, guint y0,
			 guint x1, guint y1,
			 gboolean use_selection_list)
{
  GList *note;
  GList *region;

  guint current_x0, current_y;
  
  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation)){
    return(NULL);
  }

  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  if(x0 > x1){
    guint tmp;

    tmp = x1;
    x1 = x0;
    x0 = x1;
  }

  if(y0 > y1){
    gdouble tmp_y;

    tmp_y = y0;
    y0 = y1;
    y1 = tmp_y;
  }
  
  /* find note */
  pthread_mutex_lock(notation_mutex);

  if(use_selection_list){
    note = notation->selection;
  }else{
    note = notation->note;
  }

  while(note != NULL){
    g_object_get(note->data,
		 "x0", &current_x0,
		 NULL);
    
    if(current_x0 >= x0){
      break;
    }
    
    note = note->next;
  }

  region = NULL;

  while(note != NULL){
    g_object_get(note->data,
		 "x0", &current_x0,
		 "y", &current_y,
		 NULL);

    if(current_x0 > x1){
      break;
    }

    if(current_y >= y0 && current_y < y1){
      region = g_list_prepend(region,
			      note->data);
    }

    note = note->next;
  }

  pthread_mutex_unlock(notation_mutex);

  region = g_list_reverse(region);

  return(region);
}

/**
 * ags_notation_free_selection:
 * @notation: the #AgsNotation
 *
 * Clear selection.
 *
 * Since: 2.0.0
 */
void
ags_notation_free_selection(AgsNotation *notation)
{
  AgsNote *note;

  GList *list_start, *list;

  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation)){
    return;
  }

  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  /* free selection */
  pthread_mutex_lock(notation_mutex);

  list =
    list_start = notation->selection;
  
  while(list != NULL){
    ags_note_unset_flags(list->data,
				 AGS_NOTE_IS_SELECTED);
    
    list = list->next;
  }

  notation->selection = NULL;

  pthread_mutex_unlock(notation_mutex);
  
  g_list_free_full(list_start,
		   g_object_unref);
}

/**
 * ags_notation_add_point_to_selection:
 * @notation: the #AgsNotation
 * @x: x offset
 * @y: y note tone
 * @replace_current_selection: if %TRUE selection is replaced
 *
 * Select notes at position.
 *
 * Since: 2.0.0
 */ 
void
ags_notation_add_point_to_selection(AgsNotation *notation,
				    guint x, guint y,
				    gboolean replace_current_selection)
{
  AgsNote *note;

  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation)){
    return;
  }

  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  /* find note */
  note = ags_notation_find_point(notation,
				 x, y,
				 FALSE);

  if(note == NULL){
    /* there is nothing to be selected */
    if(replace_current_selection){
      ags_notation_free_selection(notation);
    }
  }else{
    /* add to or replace selection */
    ags_note_set_flags(note, AGS_NOTE_IS_SELECTED);

    if(replace_current_selection){
      GList *list;

      list = g_list_alloc();
      list->data = note;
      g_object_ref(note);
      
      ags_notation_free_selection(notation);

      /* replace */
      pthread_mutex_lock(notation_mutex);

      notation->selection = list;
      
      pthread_mutex_unlock(notation_mutex);
    }else{
      if(!ags_notation_is_note_selected(notation,
					note)){
	/* add */
	ags_notation_add_note(notation,
			      note, TRUE);
      }
    }
  }
}

/**
 * ags_notation_remove_point_from_selection:
 * @notation: the #AgsNotation
 * @x: x offset
 * @y: y note tone
 *
 * Remove notes at position of selection.
 *
 * Since: 2.0.0
 */ 
void
ags_notation_remove_point_from_selection(AgsNotation *notation,
					 guint x, guint y)
{
  AgsNote *note;
  
  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation)){
    return;
  }

  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  /* find point */
  note = ags_notation_find_point(notation,
				 x, y,
				 TRUE);

  if(note != NULL){
    ags_note_unset_flags(note,
			 AGS_NOTE_IS_SELECTED);

    /* remove note from selection */
    pthread_mutex_lock(notation_mutex);
    
    notation->selection = g_list_remove(notation->selection,
					note);
    g_object_unref(note);

    pthread_mutex_unlock(notation_mutex);
  }
}

/**
 * ags_notation_add_region_to_selection:
 * @notation: the #AgsNotation
 * @x0: x start offset
 * @y0: y start tone
 * @x1: x end offset
 * @y1: y end tone
 * @replace_current_selection: if %TRUE selection is replaced
 *
 * Add note within region to selection.
 *
 * Since: 2.0.0
 */
void
ags_notation_add_region_to_selection(AgsNotation *notation,
				     guint x0, guint y0,
				     guint x1, guint y1,
				     gboolean replace_current_selection)
{
  AgsNote *note;

  GList *region, *list;

  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation)){
    return;
  }

  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  /* find region */
  region = ags_notation_find_region(notation,
				    x0, y0,
				    x1, y1,
				    FALSE);

  if(replace_current_selection){
    ags_notation_free_selection(notation);

    list = region;

    while(list != NULL){
      ags_note_set_flags(list->data,
			 AGS_NOTE_IS_SELECTED);
      g_object_ref(list->data);

      list = list->next;
    }

    /* replace */
    pthread_mutex_lock(notation_mutex);
     
    notation->selection = region;

    pthread_mutex_unlock(notation_mutex);
  }else{
    list = region;
    
    while(list != NULL){
      if(!ags_notation_is_note_selected(notation, list->data)){
	/* add */
	ags_notation_add_note(notation,
			      list->data,
			      TRUE);
      }
      
      list = list->next;
    }
    
    g_list_free(region);
  }
}

/**
 * ags_notation_remove_region_from_selection:
 * @notation: the #AgsNotation
 * @x0: x start offset
 * @y0: y start tone
 * @x1: x end offset
 * @y1: y end tone
 *
 * Remove note within region of selection.
 *
 * Since: 2.0.0
 */ 
void
ags_notation_remove_region_from_selection(AgsNotation *notation,
					  guint x0, guint y0,
					  guint x1, guint y1)
{
  AgsNote *note;

  GList *region;
  GList *list;
  
  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation)){
    return;
  }

  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  /* find region */
  region = ags_notation_find_region(notation,
				    x0, y0,
				    x1, y1,
				    TRUE);

  list = region;
  
  while(list != NULL){
    ags_note_unset_flags(list->data,
			 AGS_NOTE_IS_SELECTED);

    /* remove */
    pthread_mutex_lock(notation_mutex);

    notation->selection = g_list_remove(notation->selection,
					list->data);

    pthread_mutex_unlock(notation_mutex);

    g_object_unref(list->data);

    /* iterate */
    list = list->next;
  }

  g_list_free(region);
}

/**
 * ags_notation_add_all_to_selection:
 * @notation: the #AgsNotation
 *
 * Add all note to selection.
 *
 * Since: 2.0.0
 */
void
ags_notation_add_all_to_selection(AgsNotation *notation)
{
  GList *list;
  
  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation)){
    return;
  }

  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  /* select all */
  pthread_mutex_lock(notation_mutex);

  list = notation->note;

  while(list != NULL){
    ags_notation_add_note(notation,
			  list->data, TRUE);
    
    list = list->next;
  }

  pthread_mutex_unlock(notation_mutex);
}

/**
 * ags_notation_copy_selection:
 * @notation: the #AgsNotation
 *
 * Copy selection to clipboard.
 *
 * Returns: the selection as XML.
 *
 * Since: 2.0.0
 */
xmlNode*
ags_notation_copy_selection(AgsNotation *notation)
{
  AgsTimestamp *timestamp;
  
  xmlNode *notation_node, *current_note;
  xmlNode *timestamp_node;

  GList *selection;

  guint current_x0, current_x1, current_y;
  guint x_boundary, y_boundary;

  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation)){
    return;
  }

  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  /* create root node */
  pthread_mutex_lock(notation_mutex);

  notation_node = xmlNewNode(NULL,
			     BAD_CAST "notation");

  xmlNewProp(notation_node,
	     BAD_CAST "program",
	     BAD_CAST "ags");
  xmlNewProp(notation_node,
	     BAD_CAST "type",
	     BAD_CAST (AGS_NOTATION_CLIPBOARD_TYPE));
  xmlNewProp(notation_node,
	     BAD_CAST "version",
	     BAD_CAST (AGS_NOTATION_CLIPBOARD_VERSION));
  xmlNewProp(notation_node,
	     BAD_CAST "format",
	     BAD_CAST (AGS_NOTATION_CLIPBOARD_FORMAT));
  xmlNewProp(notation_node,
	     BAD_CAST "audio-channel",
	     BAD_CAST (g_strdup_printf("%u", notation->audio_channel)));

  /* timestamp */
  timestamp = notation->timestamp;
  
  if(timestamp != NULL){
    timestamp_node = xmlNewNode(NULL,
				BAD_CAST "timestamp");
    xmlAddChild(notation_node,
		timestamp_node);
    
    xmlNewProp(timestamp_node,
	       BAD_CAST "offset",
	       BAD_CAST (g_strdup_printf("%u", ags_timestamp_get_ags_offset(timestamp))));
  }
  
  /* selection */
  selection = notation->selection;

  if(selection != NULL){
    g_object_get(selection->data,
		 "x0", &current_x0,
		 NULL);
    
    x_boundary = current_x0;
    y_boundary = G_MAXUINT;
  }else{
    x_boundary = 0;
    y_boundary = 0;
  }

  while(selection != NULL){
    g_object_get(selection->data,
		 "x0", &current_x0,
		 "x1", &current_x1,
		 "y", &current_y,
		 NULL);

    current_note = xmlNewChild(notation_node,
			       NULL,
			       BAD_CAST "note",
			       NULL);

    xmlNewProp(current_note,
	       BAD_CAST "x",
	       BAD_CAST (g_strdup_printf("%u", current_x0)));
    xmlNewProp(current_note,
	       BAD_CAST "x1",
	       BAD_CAST (g_strdup_printf("%u", current_x1)));
    xmlNewProp(current_note,
	       BAD_CAST "y",
	       BAD_CAST (g_strdup_printf("%u", current_y)));

    if(y_boundary > current_y){
      y_boundary = current_y;
    }

    selection = selection->next;
  }
	       
  pthread_mutex_unlock(notation_mutex);

  xmlNewProp(notation_node,
	     BAD_CAST "x_boundary",
	     BAD_CAST (g_strdup_printf("%u", x_boundary)));
  xmlNewProp(notation_node,
	     BAD_CAST "y_boundary",
	     BAD_CAST (g_strdup_printf("%u", y_boundary)));

  return(notation_node);
}

/**
 * ags_notation_cut_selection:
 * @notation: the #AgsNotation
 *
 * Cut selection to clipboard.
 *
 * Returns: the selection as xmlNode
 *
 * Since: 2.0.0
 */
xmlNode*
ags_notation_cut_selection(AgsNotation *notation)
{
  xmlNode *notation_node;
  
  GList *selection, *note;
  
  pthread_mutex_t *notation_mutex;

  if(!AGS_IS_NOTATION(notation)){
    return;
  }

  /* get notation mutex */
  pthread_mutex_lock(ags_notation_get_class_mutex());
  
  notation_mutex = notation->obj_mutex;
  
  pthread_mutex_unlock(ags_notation_get_class_mutex());

  /* copy selection */
  notation_node = ags_notation_copy_selection(notation);

  /* cut */
  pthread_mutex_lock(notation_mutex);

  selection = notation->selection;

  while(selection != NULL){
    notation->note = g_list_remove(notation->note,
				   selection->data);
    g_object_unref(selection->data);

    selection = selection->next;
  }

  pthread_mutex_unlock(notation_mutex);

  /* free selection */
  ags_notation_free_selection(notation);

  return(notation_node);
}

/**
 * ags_notation_insert_native_piano_from_clipboard:
 * @notation: the #AgsNotation
 * @notation_node: the clipboard XML data
 * @version: clipboard version
 * @base_frequency: lowest frequency of notation
 * @x_boundary: region start offset
 * @y_boundary: region start tone
 * @reset_x_offset: if %TRUE @x_offset used as cursor
 * @x_offset: region start cursor offset
 * @reset_y_offset: if %TRUE @y_offset used as cursor
 * @y_offset: region start cursor tone
 * @match_channel: only paste if channel matches
 * @no_duplicates: only paste if current note doesn't exist
 *
 * Paste previously copied notes. 
 *
 * Since: 2.0.0
 */
void
ags_notation_insert_native_piano_from_clipboard(AgsNotation *notation,
						xmlNode *root_node, char *version,
						char *base_frequency,
						char *x_boundary, char *y_boundary,
						gboolean reset_x_offset, guint x_offset,
						gboolean reset_y_offset, guint y_offset,
						gboolean match_channel, gboolean no_duplicates)
{
  guint current_audio_channel;
  
  gboolean match_timestamp;
  
  auto void ags_notation_insert_native_piano_from_clipboard_version_0_3_12();
  
  void ags_notation_insert_native_piano_from_clipboard_version_0_3_12()
  {
    AgsNote *note;

    AgsTimestamp *timestamp;

    xmlNode *node;

    char *x0, *x1, *y;
    gchar *offset;
    char *endptr;

    guint x_boundary_val, y_boundary_val;
    guint x0_val, x1_val, y_val;
    guint base_x_difference, base_y_difference;
    gboolean subtract_x, subtract_y;

    node = root_node->children;

    /* retrieve x values for resetting */
    if(reset_x_offset){
      if(x_boundary != NULL){
	errno = 0;
	x_boundary_val = strtoul(x_boundary,
				 &endptr,
				 10);

	if(errno == ERANGE){
	  goto dont_reset_x_offset;
	} 
	
	if(x_boundary == endptr){
	  goto dont_reset_x_offset;
	}

	if(x_boundary_val < x_offset){
	  base_x_difference = x_offset - x_boundary_val;
	  subtract_x = FALSE;
	}else{
	  base_x_difference = x_boundary_val - x_offset;
	  subtract_x = TRUE;
	}
      }else{
      dont_reset_x_offset:
	reset_x_offset = FALSE;
      }
    }

    /* retrieve y values for resetting */
    if(reset_y_offset){
      if(y_boundary != NULL){
	errno = 0;
	y_boundary_val = strtoul(y_boundary,
				 &endptr,
				 10);

	if(errno == ERANGE){
	  goto dont_reset_y_offset;
	} 

	if(y_boundary == endptr){
	  goto dont_reset_y_offset;
	}

	if(y_boundary_val < y_offset){
	  base_y_difference = y_offset - y_boundary_val;
	  subtract_y = FALSE;
	}else{
	  base_y_difference = y_boundary_val - y_offset;
	  subtract_y = TRUE;
	}
      }else{
      dont_reset_y_offset:
	reset_y_offset = FALSE;
      }
    }

    /* parse */
    for(; node != NULL; ){
      if(node->type == XML_ELEMENT_NODE){
	if(!xmlStrncmp("note",
		       node->name,
		       5)){
	  /* retrieve x0 offset */
	  x0 = xmlGetProp(node, "x");

	  if(x0 == NULL){
	    node = node->next;
	  
	    continue;
	  }

	  errno = 0;
	  x0_val = strtoul(x0, &endptr, 10);

	  if(errno == ERANGE){
	    node = node->next;
	  
	    continue;
	  } 

	  if(x0 == endptr){
	    node = node->next;
	  
	    continue;
	  }

	  /* retrieve x1 offset */
	  x1 = xmlGetProp(node, "x1");

	  if(x1 == NULL){
	    node = node->next;
	  
	    continue;
	  }

	  errno = 0;
	  x1_val = strtoul(x1, &endptr, 10);

	  if(errno == ERANGE){
	    node = node->next;
	  
	    continue;
	  } 

	  if(x1 == endptr){
	    node = node->next;
	  
	    continue;
	  }

	  /* retrieve y offset */
	  y = xmlGetProp(node, "y");

	  if(y == NULL){
	    node = node->next;
	  
	    continue;
	  }

	  errno = 0;
	  y_val = strtoul(y, &endptr, 10);

	  if(errno == ERANGE){
	    node = node->next;
	  
	    continue;
	  } 

	  if(y == endptr){
	    node = node->next;
	  
	    continue;
	  }

	  /* switch x values if necessary */
	  if(x0_val > x1_val){
	    guint tmp;

	    tmp = x0_val;
	    x0_val = x1_val;
	    x1_val = tmp;
	  }

	  /* calculate new offset */
	  if(reset_x_offset){
	    errno = 0;

	    if(subtract_x){
	      x0_val -= base_x_difference;

	      if(errno != 0){
		node = node->next;
	      
		continue;
	      }

	      x1_val -= base_x_difference;
	    }else{
	      x0_val += base_x_difference;
	      x1_val += base_x_difference;

	      if(errno != 0){
		node = node->next;
	      
		continue;
	      }
	    }
	  }

	  if(reset_y_offset){
	    errno = 0;

	    if(subtract_y){
	      y_val -= base_y_difference;
	    }else{
	      y_val += base_y_difference;
	    }

	    if(errno != 0){
	      node = node->next;
	    
	      continue;
	    }
	  }

	  /* check if max length wasn't exceeded */
	  if(x1_val - x0_val > notation->maximum_note_length){
	    node = node->next;
	  
	    continue;
	  }

	  /* check duplicate */
	  if(no_duplicates &&
	     ags_notation_find_point(notation,
				     x0_val, y_val,
				     FALSE) != NULL){
	    node = node->next;
	  
	    continue;
	  }
	  
	  /* add note */
	  g_object_get(notation,
		       "timestamp", &timestamp,
		       NULL);

	  if(!match_timestamp || 
	     (x0_val >= ags_timestamp_get_ags_offset(timestamp) &&
	      x0_val < ags_timestamp_get_ags_offset(timestamp) + AGS_NOTATION_DEFAULT_OFFSET)){
	    note = ags_note_new();

	    note->x[0] = x0_val;
	    note->x[1] = x1_val;

	    note->y = y_val;

#ifdef AGS_DEBUG
	    g_message("adding note at: [%u,%u|%u]\n", x0_val, x1_val, y_val);
#endif
	    
	    ags_notation_add_note(notation,
				  note,
				  FALSE);
	  }
	}
      }
    
      node = node->next;
    }
  }

  if(!AGS_IS_NOTATION(notation)){
    return;
  }

  match_timestamp = TRUE;
  
  if(!xmlStrncmp("0.3.12", version, 7)){
    ags_notation_insert_native_piano_from_clipboard_version_0_3_12();
  }else if(!xmlStrncmp("0.4.2", version, 6)){
    /* changes contain only for UI relevant new informations */
    ags_notation_insert_native_piano_from_clipboard_version_0_3_12();
  }else if(!xmlStrncmp("1.2.0", version, 6)){
    /* changes contain only optional informations */
    match_timestamp = TRUE;

    g_object_get(notation,
		 "audio-channel", &current_audio_channel,
		 NULL);

    if(match_channel &&
       current_audio_channel != g_ascii_strtoull(xmlGetProp(root_node,
							    "audio-channel"),
						 NULL,
						 10)){
      return;
    }
        
    ags_notation_insert_native_piano_from_clipboard_version_0_3_12();
  }
}

/**
 * ags_notation_insert_from_clipboard:
 * @notation: the #AgsNotation
 * @notation_node: the clipboard XML data
 * @reset_x_offset: if %TRUE @x_offset used as cursor
 * @x_offset: region start cursor offset
 * @reset_y_offset: if %TRUE @y_offset used as cursor
 * @y_offset: region start cursor tone
 *
 * Paste previously copied notes. 
 *
 * Since: 2.0.0
 */
void
ags_notation_insert_from_clipboard(AgsNotation *notation,
				   xmlNode *notation_node,
				   gboolean reset_x_offset, guint x_offset,
				   gboolean reset_y_offset, guint y_offset)
{
  ags_notation_insert_from_clipboard_extended(notation,
					      notation_node,
					      reset_x_offset, x_offset,
					      reset_y_offset, y_offset,
					      FALSE, FALSE);
}

/**
 * ags_notation_insert_from_clipboard_extended:
 * @notation: the #AgsNotation
 * @notation_node: the clipboard XML data
 * @reset_x_offset: if %TRUE @x_offset used as cursor
 * @x_offset: region start cursor offset
 * @reset_y_offset: if %TRUE @y_offset used as cursor
 * @y_offset: region start cursor tone
 * @match_channel: only paste if channel matches
 * @no_duplicates: only paste if current note doesn't exist
 * 
 * Paste previously copied notes. 
 * 
 * Since: 2.0.0
 */
void
ags_notation_insert_from_clipboard_extended(AgsNotation *notation,
					    xmlNode *notation_node,
					    gboolean reset_x_offset, guint x_offset,
					    gboolean reset_y_offset, guint y_offset,
					    gboolean match_channel, gboolean no_duplicates)
{
  char *program, *version, *type, *format;
  char *base_frequency;
  char *x_boundary, *y_boundary;

  if(!AGS_IS_NOTATION(notation)){
    return;
  }
  
  while(notation_node != NULL){
    if(notation_node->type == XML_ELEMENT_NODE && !xmlStrncmp("notation", notation_node->name, 9)){
      break;
    }

    notation_node = notation_node->next;
  }

  if(notation_node != NULL){
    program = xmlGetProp(notation_node, "program");

    if(!xmlStrncmp("ags", program, 4)){
      version = xmlGetProp(notation_node, "version");
      type = xmlGetProp(notation_node, "type");
      format = xmlGetProp(notation_node, "format");

      if(!xmlStrcmp(AGS_NOTATION_CLIPBOARD_FORMAT,
		    format)){
	base_frequency = xmlGetProp(notation_node, "base_frequency");

	x_boundary = xmlGetProp(notation_node, "x_boundary");
	y_boundary = xmlGetProp(notation_node, "y_boundary");

	ags_notation_insert_native_piano_from_clipboard(notation,
							notation_node, version,
							base_frequency,
							x_boundary, y_boundary,
							reset_x_offset, x_offset,
							reset_y_offset, y_offset,
							match_channel, no_duplicates);
      }
    }
  }
}

/**
 * ags_notation_to_raw_midi:
 * @notation: the #AgsNotation
 * @bpm: the source bpm
 * @delay_factor: the source delay factor
 * @nn: numerator
 * @dd: denominator
 * @cc: clocks
 * @bb: beats
 * @tempo: tempo
 * @buffer_length: the return location of buffer length
 * 
 * Convert @notation to raw-midi.
 * 
 * Returns: the raw-midi buffer
 * 
 * Since: 2.0.0
 */
unsigned char*
ags_notation_to_raw_midi(AgsNotation *notation,
			 gdouble bpm, gdouble delay_factor,
			 glong nn, glong dd, glong cc, glong bb,
			 glong tempo,
			 guint *buffer_length)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_notation_from_raw_midi:
 * @raw_midi: the data array
 * @nn: numerator
 * @dd: denominator
 * @cc: clocks
 * @bb: beats
 * @tempo: tempo
 * @bpm: the bpm to use
 * @delay_factor: the segmentation delay factor
 * @buffer_length: the buffer length
 * 
 * Parse @raw_midi data and convert to #AgsNotation.
 * 
 * Returns: the #AgsNotation
 * 
 * Since: 2.0.0
 */
AgsNotation*
ags_notation_from_raw_midi(unsigned char *raw_midi,
			   glong nn, glong dd, glong cc, glong bb,
			   glong tempo,
			   gdouble bpm, gdouble delay_factor,
			   guint buffer_length)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_notation_new:
 * @audio: the assigned #AgsAudio
 * @audio_channel: the audio channel to apply
 *
 * Creates a new instance of #AgsNotation.
 *
 * Returns: the new #AgsNotation
 *
 * Since: 2.0.0
 */
AgsNotation*
ags_notation_new(GObject *audio,
		 guint audio_channel)
{
  AgsNotation *notation;
  
  notation = (AgsNotation *) g_object_new(AGS_TYPE_NOTATION,
					  "audio", audio,
					  "audio-channel", audio_channel,
					  NULL);
  
  return(notation);
}
