/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/ags_notation.h>

#include <ags/object/ags_connectable.h>

#include <ags/object/ags_tactable.h>
#include <ags/object/ags_portlet.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_timestamp.h>

#include <stdlib.h>
#include <errno.h>

void ags_notation_class_init(AgsNotationClass *notation);
void ags_notation_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_notation_tactable_interface_init(AgsTactableInterface *tactable);
void ags_notation_portlet_interface_init(AgsPortletInterface *portlet);
void ags_notation_init(AgsNotation *notation);
void ags_notation_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec);
void ags_notation_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec);
void ags_notation_connect(AgsConnectable *connectable);
void ags_notation_disconnect(AgsConnectable *connectable);
void ags_notation_finalize(GObject *object);

void ags_notation_change_bpm(AgsTactable *tactable, gdouble bpm);

void ags_notation_set_port(AgsPortlet *portlet, AgsPort *port);
AgsPort* ags_notation_get_port(AgsPortlet *portlet);
GList* ags_notation_list_safe_properties(AgsPortlet *portlet);
void ags_notation_safe_set_property(AgsPortlet *portlet, gchar *property_name, GValue *value);
void ags_notation_safe_get_property(AgsPortlet *portlet, gchar *property_name, GValue *value);

void ags_notation_insert_native_piano_from_clipboard(AgsNotation *notation,
						     xmlNodePtr root_node, char *version,
						     char *base_frequency,
						     char *x_boundary, char *y_boundary,
						     gboolean from_x_offset, guint x_offset,
						     gboolean from_y_offset, guint y_offset);

/**
 * SECTION:ags_notation
 * @short_description: Notation class supporting selection and clipboard.
 * @title: AgsNotation
 * @section_id:
 * @include: ags/audio/ags_notation.h
 *
 * #AgsNotation acts as a container of #AgsNote.
 */

#define AGS_NOTATION_CLIPBOARD_VERSION "0.3.12\0"
#define AGS_NOTATION_CLIPBOARD_TYPE "AgsNotationClipboardXml\0"
#define AGS_NOTATION_CLIPBOARD_FORMAT "AgsNotationNativePiano\0"

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_AUDIO_CHANNEL,
  PROP_PORT,
  PROP_NOTE,
  PROP_CURRENT_NOTES,
  PROP_NEXT_NOTES,
};

static gpointer ags_notation_parent_class = NULL;

GType
ags_notation_get_type()
{
  static GType ags_type_notation = 0;

  if(!ags_type_notation){
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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_notation_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_tactable_interface_info = {
      (GInterfaceInitFunc) ags_notation_tactable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_portlet_interface_info = {
      (GInterfaceInitFunc) ags_notation_portlet_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_notation = g_type_register_static(G_TYPE_OBJECT,
					       "AgsNotation\0",
					       &ags_notation_info,
					       0);

    g_type_add_interface_static(ags_type_notation,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_notation,
				AGS_TYPE_TACTABLE,
				&ags_tactable_interface_info);

    g_type_add_interface_static(ags_type_notation,
				AGS_TYPE_PORTLET,
				&ags_portlet_interface_info);
  }

  return(ags_type_notation);
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

  gobject->finalize = ags_notation_finalize;

  /* properties */
  /**
   * AgsNotation:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("audio\0",
				   "audio of notation\0",
				   "The audio of notation\0",
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
   * Since: 0.4.3
   */
  param_spec =  g_param_spec_ulong("audio-channel\0",
				   "audio-channel of effect\0",
				   "The numerical audio-channel of effect\0",
				   0,
				   65535,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);

  /**
   * AgsNotation:port:
   *
   * The assigned #AgsPort
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("port\0",
				   "port of notation\0",
				   "The port of notation\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT,
				  param_spec);

  /**
   * AgsNotation:note:
   *
   * The assigned #AgsNote
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("note\0",
				   "note of notation\0",
				   "The note of notation\0",
				   AGS_TYPE_NOTE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTE,
				  param_spec);

  /**
   * AgsNotation:current-notes:
   *
   * Offset of current position.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_pointer("current-notes\0",
				    "current notes for offset\0",
				    "The current notes for offset\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CURRENT_NOTES,
				  param_spec);

  /**
   * AgsNotation:next-notes:
   *
   * Offset of next position.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_pointer("next-notes\0",
				    "next notes for offset\0",
				    "The next notes for offset\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NEXT_NOTES,
				  param_spec);
}

void
ags_notation_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_notation_connect;
  connectable->disconnect = ags_notation_disconnect;
}

void
ags_notation_tactable_interface_init(AgsTactableInterface *tactable)
{
  tactable->change_bpm = ags_notation_change_bpm;
}

void
ags_notation_portlet_interface_init(AgsPortletInterface *portlet)
{
  portlet->set_port = ags_notation_set_port;
  portlet->get_port = ags_notation_get_port;
  portlet->list_safe_properties = ags_notation_list_safe_properties;
  portlet->safe_set_property = ags_notation_safe_set_property;
  portlet->safe_get_property = ags_notation_safe_get_property;
}

void
ags_notation_init(AgsNotation *notation)
{
  notation->flags = 0;

  //TODO:JK: define timestamp
  notation->timestamp = NULL;
  
  notation->audio_channel = 0;
  notation->audio = NULL;

  notation->key = g_strdup("violine\0");
  notation->base_frequency = 440.0;

  notation->tact = AGS_NOTATION_MINIMUM_NOTE_LENGTH;
  notation->bpm = 120.0;

  notation->maximum_note_length = AGS_NOTATION_MAXIMUM_NOTE_LENGTH;

  notation->notes = NULL;

  notation->start_loop = 0.0;
  notation->end_loop = 0.0;
  notation->offset = 0.0;

  notation->selection = NULL;

  notation->port = NULL;

  notation->current_notes = NULL;
  notation->next_notes = NULL;
}

void
ags_notation_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_notation_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_notation_set_property(GObject *gobject,
			 guint prop_id,
			 const GValue *value,
			 GParamSpec *param_spec)
{
  AgsNotation *notation;

  notation = AGS_NOTATION(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(notation->audio == audio){
	return;
      }

      if(notation->audio != NULL){
	g_object_unref(notation->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      notation->audio = audio;
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      guint audio_channel;

      audio_channel = g_value_get_uint(value);

      notation->audio_channel = audio_channel;
    }
    break;
  case PROP_PORT:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == (AgsPort *) notation->port){
	return;
      }

      if(notation->port != NULL){
	g_object_unref(G_OBJECT(notation->port));
      }

      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      notation->port = (GObject *) port;
    }
    break;
  case PROP_NOTE:
    {
      AgsNote *note;

      note = (AgsNote *) g_value_get_object(value);

      if(note == NULL ||
	 g_list_find(notation->notes, note) != NULL){
	return;
      }

      ags_notation_add_note(notation,
			    note,
			    FALSE);
    }
    break;
  case PROP_CURRENT_NOTES:
    {
      AgsPort *port;
      GList *current_notes, *list;

      current_notes = (GList *) g_value_get_pointer(value);

      port = AGS_PORT(notation->port);

      pthread_mutex_lock(&(port->mutex));

      if(notation->current_notes != NULL){
	ags_list_free_and_unref_link(notation->current_notes);
      }

      if(current_notes != NULL){
	list = current_notes;

	while(list != NULL){
	  g_object_ref(G_OBJECT(list->data));

	  list = list->next;
	}
      }

      notation->current_notes = current_notes;

      pthread_mutex_unlock(&(port->mutex));
    }
    break;
  case PROP_NEXT_NOTES:
    {
      AgsPort *port;
      GList *next_notes, *list;

      next_notes = (GList *) g_value_get_pointer(value);

      port = AGS_PORT(notation->port);

      pthread_mutex_lock(&(port->mutex));

      if(notation->next_notes != NULL){
	ags_list_free_and_unref_link(notation->next_notes);
      }

      if(next_notes != NULL){
	list = next_notes;

	while(list != NULL){
	  g_object_ref(G_OBJECT(list->data));

	  list = list->next;
	}
      }

      notation->next_notes = next_notes;

      pthread_mutex_unlock(&(port->mutex));
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

  notation = AGS_NOTATION(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    g_value_set_object(value, notation->audio);
    break;
  case PROP_AUDIO_CHANNEL:
    g_value_set_uint(value, notation->audio_channel);
    break;
  case PROP_PORT:
    g_value_set_object(value, notation->port);
    break;
  case PROP_NOTE:
    g_value_set_pointer(value, g_list_copy(notation->port));
    break;
  case PROP_CURRENT_NOTES:
    {
      AgsPort *port;
      GList *start, *list;

      port = AGS_PORT(notation->port);

      pthread_mutex_lock(&(port->mutex));

      start = 
	list = g_list_copy(notation->current_notes);

      while(list != NULL){
	g_object_ref(G_OBJECT(list->data));

	list = list->next;
      }

      pthread_mutex_unlock(&(port->mutex));

      g_value_set_pointer(value, (gpointer) start);
    }
    break;
  case PROP_NEXT_NOTES:
    {
      AgsPort *port;
      GList *start, *list;

      port = AGS_PORT(notation->port);

      pthread_mutex_lock(&(port->mutex));

      start = 
	list = g_list_copy(notation->next_notes);

      while(list != NULL){
	g_object_ref(G_OBJECT(list->data));

	list = list->next;
      }

      pthread_mutex_unlock(&(port->mutex));

      g_value_set_pointer(value, (gpointer) start);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_notation_finalize(GObject *gobject)
{
  AgsNotation *notation;
  GList *list, *list_next;

  notation = AGS_NOTATION(gobject);

  if(notation->audio != NULL){
    g_object_unref(notation->audio);
  }
  
  g_list_free_full(notation->notes,
		   g_object_unref);

  G_OBJECT_CLASS(ags_notation_parent_class)->finalize(gobject);
}

void
ags_notation_change_bpm(AgsTactable *tactable, gdouble bpm)
{
  //TODO:JK: implement me
}

void
ags_notation_set_port(AgsPortlet *portlet, AgsPort *port)
{
  g_object_set(G_OBJECT(portlet),
	       "port\0", port,
	       NULL);
}

AgsPort*
ags_notation_get_port(AgsPortlet *portlet)
{
  AgsPort *port;

  g_object_get(G_OBJECT(portlet),
	       "port\0", &port,
	       NULL);

  return(port);
}

GList*
ags_notation_list_safe_properties(AgsPortlet *portlet)
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  static GList *list = NULL;

  pthread_mutex_lock(&mutex);

  if(list == NULL){
    list = g_list_prepend(list, "current-notes\0");
    list = g_list_prepend(list, "next-notes\0");
  }

  pthread_mutex_unlock(&mutex);

  return(list);
}

void
ags_notation_safe_set_property(AgsPortlet *portlet, gchar *property_name, GValue *value)
{
  //TODO:JK: add check for safe property

  g_object_set_property(G_OBJECT(portlet),
			property_name, value);
}

void
ags_notation_safe_get_property(AgsPortlet *portlet, gchar *property_name, GValue *value)
{
  //TODO:JK: add check for safe property

  g_object_get_property(G_OBJECT(portlet),
			property_name, value);
}

/**
 * ags_notation_find_near_timestamp:
 * @notation: a #GList containing #AgsNotation
 * @audio_channel: the matching audio channel
 * @timestamp: the matching timestamp
 *
 * Retrieve appropriate notation for timestamp.
 *
 * Returns: Next match.
 *
 * Since: 0.4.2
 */
GList*
ags_notation_find_near_timestamp(GList *notation, guint audio_channel,
				 GObject *gobject)
{
  AgsTimestamp *timestamp, *current_timestamp;

  if(gobject == NULL){
    return(NULL);
  }
  
  timestamp = AGS_TIMESTAMP(gobject);

  while(notation != NULL){
    if(AGS_NOTATION(notation->data)->audio_channel != audio_channel){
      notation = notation->next;
      continue;
    }

    current_timestamp = AGS_NOTATION(notation->data)->timestamp;

    if((AGS_TIMESTAMP_UNIX & (timestamp->flags)) != 0){
      if((AGS_TIMESTAMP_UNIX & (current_timestamp->flags)) != 0){
	if(current_timestamp->timer.unix_time.time_val >= timestamp->timer.unix_time.time_val &&
	   current_timestamp->timer.unix_time.time_val < timestamp->timer.unix_time.time_val + AGS_NOTATION_DEFAULT_DURATION){
	  return(notation);
	}
      }
    }

    notation = notation->next;
  }

  return(NULL);
}

/**
 * ags_notation_add_note:
 * @notation: an #AgsNotation
 * @note: the #AgsNote to add
 * @use_selection_list: if %TRUE add to selection, else to default notation
 *
 * Adds a note to notation.
 *
 * Since: 0.4
 */
void
ags_notation_add_note(AgsNotation *notation,
		      AgsNote *note,
		      gboolean use_selection_list)
{
  GList *list, *list_new;

  auto void ags_notation_add_note_add1();
  
  void ags_notation_add_note_add1(){
    list_new = (GList *) malloc(sizeof(GList));
    list_new->data = (gpointer) note;

    if(list->prev == NULL){
      list_new->prev = NULL;
      list_new->next = list;
      list->prev = list_new;

      if(use_selection_list)
	notation->selection = list_new;
      else
	notation->notes = list_new;
    }else{
      list_new->prev = list->prev;
      list_new->next = list;
      list->prev = list_new;
      list_new->prev->next = list_new;
    }
  }

  if(use_selection_list)
    list = notation->selection;
  else
    list = notation->notes;

  if(list == NULL){
    list_new = g_list_alloc();
    list_new->data = (gpointer) note;

    if(use_selection_list)
      notation->selection = list_new;
    else
      notation->notes = list_new;

    return;
  }

  while(list->next != NULL){
    if((AGS_NOTE(list->data))->x[0] >= note->x[0]){
      while(list->next != NULL){
	if((AGS_NOTE(list->data))->x[0] > note->x[0] ||
	   (AGS_NOTE(list->data))->y >= note->y){
	  ags_notation_add_note_add1();

	  return;
	}

	list = list->next;
      }

      break;
    }

    list = list->next;
  }

  if((AGS_NOTE(list->data))->x[0] >= note->x[0])
    ags_notation_add_note_add1();
  else{
    list_new = g_list_alloc();
    list_new->data = (gpointer) note;
    list_new->prev = list;
    list->next = list_new;
  }
}

/**
 * ags_notation_remove_note_at_position:
 * @notation: an #AgsNotation
 * @x: offset
 * @y: note
 *
 * Removes one #AgsNote of notation.
 *
 * Returns: %TRUE if successfully removed note.
 *
 * Since: 0.4
 */
gboolean
ags_notation_remove_note_at_position(AgsNotation *notation,
				     guint x, guint y)
{
  AgsNote *note;
  GList *notes, *notes_end_region, *reverse_start;
  guint x_start, i;

  notes = notation->notes;

  if(notes == NULL){
    return(FALSE);
  }

  /* get entry point */
  while(notes->next != NULL && (note = AGS_NOTE(notes->data))->x[0] < x)
    notes = notes->next;

  notes_end_region = notes;

  /* search in y region for appropriate note */
  if(notes != NULL && (note = AGS_NOTE(notes->data))->x[0] == x){
    do{
      if(note->y == y){
	g_message("remove");
	notation->notes = g_list_delete_link(notation->notes, notes);
	g_object_unref(note);

	return(TRUE);
      } 

      notes = notes->next;
    }while(notes != NULL &&
	   (note = AGS_NOTE(notes->data))->x[0] == x &&
	   note->y <= y);

    notes = notes_end_region->prev;
  }else{
    notes = notes_end_region;
  }

  /* search backward until x_start */
  reverse_start = notes;

  for(i = 0; i < notation->maximum_note_length / notation->tact; i++){
    notes = reverse_start;

    if(x < notation->maximum_note_length){
      x_start = 0;
    }else{
      x_start = x - notation->maximum_note_length;
    }

    while(notes != NULL && (note = AGS_NOTE(notes->data))->x[0] >= x_start){
      if(note->y == y){
	do{
	  if(note->x[0] < x && note->x[1] == x + i){
	    g_message("remove");
	    notation->notes = g_list_delete_link(notation->notes, notes);
	    g_object_unref(note);
	
	    return(TRUE);
	  }

	  notes = notes->prev;
	}while(notes != NULL &&
	       (note = AGS_NOTE(notes->data))->x[0] >= x_start &&
	       note->y == y);

	continue;
      }

      notes = notes->prev;
    }
  }

  return(FALSE);
}

/**
 * ags_notation_get_selection:
 * @notation: the #AgsNotation
 *
 * Retrieve selection.
 *
 * Returns: the selection.
 *
 * Since: 0.4
 */
GList*
ags_notation_get_selection(AgsNotation *notation)
{
  return(notation->selection);
}

/**
 * ags_notation_is_note_selected:
 * @notation: the #AgsNotation
 * @note: the #AgsNote to check for
 *
 * Check selection for note.
 *
 * Returns: %TRUE if selected
 *
 * Since: 0.4
 */
gboolean
ags_notation_is_note_selected(AgsNotation *notation, AgsNote *note)
{
  GList *selection;

  selection = notation->selection;

  while(selection != NULL && AGS_NOTE(selection->data)->x[0] <= note->x[0]){
    if(selection->data == note)
      return(TRUE);

    selection = selection->next;
  }

  return(FALSE);
}

/**
 * ags_notation_find_point:
 * @notation: an #AgsNotation
 * @x: offset
 * @y: note
 * @use_selection_list: if %TRUE selection is searched
 *
 * Find note by offset and tone.
 *
 * Returns: the matching note.
 *
 * Since: 0.4
 */ 
AgsNote*
ags_notation_find_point(AgsNotation *notation,
			guint x, guint y,
			gboolean use_selection_list)
{
  AgsNote *note, *prev_note;
  GList *notes;

  if(use_selection_list){
    notes = notation->selection;
  }else{
    notes = notation->notes;
  }

  while(notes != NULL && AGS_NOTE(notes->data)->x[0] < x){
    notes = notes->next;
  }

  if(notes == NULL)
    return(NULL);

  prev_note = NULL;

  while(notes != NULL && (note = AGS_NOTE(notes->data))->x[0] == x){
    if(note->y == y){
      /* find last match for point */

      do{
	prev_note = note;
	notes = notes->next;
      }while(notes != NULL && (note = AGS_NOTE(notes->data))->x[0] == x && note->y == y);

      break;
    }

    notes = notes->next;
  }

  return(prev_note);
}

/**
 * ags_notation_find_region:
 * @notation: an #AgsNotation
 * @x0: start offset
 * @y0: start tone
 * @x1: end offset
 * @y1: end tone
 * @use_selection:_list if %TRUE selection is searched
 *
 * Find notes by offset and tone region.
 *
 * Returns: the matching notes as #GList.
 *
 * Since: 0.4
 */
GList*
ags_notation_find_region(AgsNotation *notation,
			 guint x0, guint y0,
			 guint x1, guint y1,
			 gboolean use_selection_list)
{
  AgsNote *note;
  GList *notes;
  GList *region;

  if(use_selection_list){
    notes = notation->selection;
  }else{
    notes = notation->notes;
  }

  while(notes != NULL && AGS_NOTE(notes->data)->x[0] < x0){
    notes = notes->next;
  }

  region = NULL;

  while(notes != NULL && (note = AGS_NOTE(notes->data))->x[0] < x1){
    if(note->y >= y0 && note->y < y1){
      region = g_list_prepend(region, note);
    }

    notes = notes->next;
  }

  region = g_list_reverse(region);

  return(region);
}

/**
 * ags_notation_free_selection:
 * @notation: an #AgsNotation
 *
 * Clear selection.
 *
 * Since: 0.4
 */
void
ags_notation_free_selection(AgsNotation *notation)
{
  AgsNote *note;
  GList *list;

  list = notation->selection;
  
  while(list != NULL){
    note = AGS_NOTE(list->data);
    note->flags &= (~AGS_NOTE_IS_SELECTED);
    g_object_unref(G_OBJECT(note));
    
    list = list->next;
  }

  list = notation->selection;
  notation->selection = NULL;
  g_list_free(list);
}

/**
 * ags_notation_add_point_to_selection:
 * @notation: an #AgsNotation
 * @x: offset
 * @y: tone
 * @replace_current_selection: if %TRUE selection is replaced
 *
 * Select notes at position.
 *
 * Since: 0.4
 */ 
void
ags_notation_add_point_to_selection(AgsNotation *notation,
				    guint x, guint y,
				    gboolean replace_current_selection)
{
  AgsNote *note;

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
    note->flags |= AGS_NOTE_IS_SELECTED;
    g_object_ref(note);

    if(replace_current_selection){
      GList *list;

      list = g_list_alloc();
      list->data = note;
      
      ags_notation_free_selection(notation);
      notation->selection = list;
    }else{
      if(!ags_notation_is_note_selected(notation, note)){
	ags_notation_add_note(notation, note, TRUE);
      }
    }
  }
}

/**
 * ags_notation_remove_point_from_selection:
 * @notation: an #AgsNotation
 * @x: offset
 * @y: tone
 *
 * Remove notes at position of selection.
 *
 * Since: 0.4
 */ 
void
ags_notation_remove_point_from_selection(AgsNotation *notation,
					 guint x, guint y)
{
  AgsNote *note;

  note = ags_notation_find_point(notation,
				 x, y,
				 FALSE);

  if(note != NULL){
    note->flags &= (~AGS_NOTE_IS_SELECTED);

    /* remove note from selection */
    notation->selection = g_list_remove(notation->selection, note);

    g_object_unref(note);
  }
}

/**
 * ags_notation_add_region_to_selection:
 * @notation: an #AgsNotation
 * @x0: start offset
 * @y0: start tone
 * @x1: end offset
 * @y1: end tone
 * @replace_current_selection: if %TRUE selection is replaced
 *
 * Select notes within region.
 *
 * Since: 0.4
 */
void
ags_notation_add_region_to_selection(AgsNotation *notation,
				     guint x0, guint y0,
				     guint x1, guint y1,
				     gboolean replace_current_selection)
{
  AgsNote *note;
  GList *region, *list;

  region = ags_notation_find_region(notation,
				    x0, y0,
				    x1, y1,
				    FALSE);

  if(replace_current_selection){
    ags_notation_free_selection(notation);

    list = region;

    while(list != NULL){
      AGS_NOTE(list->data)->flags |= AGS_NOTE_IS_SELECTED;
      g_object_ref(G_OBJECT(list->data));

      list = list->next;
    }

    notation->selection = region;
  }else{
    while(region != NULL){
      note = AGS_NOTE(region->data);

      if(!ags_notation_is_note_selected(notation, note)){
	note->flags |= AGS_NOTE_IS_SELECTED;
	g_object_ref(G_OBJECT(note));
	ags_notation_add_note(notation,
			      note,
			      TRUE);
      }
      
      region = region->next;
    }
    
    g_list_free(region);
  }
}


/**
 * ags_notation_remove_region_from_selection:
 * @notation: an #AgsNotation
 * @x0: start offset
 * @y0: start tone
 * @x1: end offset
 * @y1: end tone
 *
 * Remove notes within region of selection.
 *
 * Since: 0.4
 */ 
void
ags_notation_remove_region_from_selection(AgsNotation *notation,
					  guint x0, guint y0,
					  guint x1, guint y1)
{
  AgsNote *note;
  GList *region;

  region = ags_notation_find_region(notation,
				    x0, y0,
				    x1, y1,
				    TRUE);

  while(region != NULL){
    note = AGS_NOTE(region->data);
    note->flags &= (~AGS_NOTE_IS_SELECTED);

    notation->selection = g_list_remove(notation->selection, note);
    g_object_unref(G_OBJECT(note));

    region = region->next;
  }

  g_list_free(region);
}

/**
 * ags_notation_copy_selection:
 * @notation: an #AgsNotation
 *
 * Copy selection to clipboard.
 *
 * Returns: the selection as XML.
 *
 * Since: 0.4
 */
xmlNode*
ags_notation_copy_selection(AgsNotation *notation)
{
  AgsNote *note;
  xmlNode *notation_node, *current_note;
  GList *selection;
  guint x_boundary, y_boundary;

  selection = notation->selection;

  /* create root node */
  notation_node = xmlNewNode(NULL, BAD_CAST "notation\0");

  xmlNewProp(notation_node, BAD_CAST "program\0", BAD_CAST "ags\0");
  xmlNewProp(notation_node, BAD_CAST "type\0", BAD_CAST AGS_NOTATION_CLIPBOARD_TYPE);
  xmlNewProp(notation_node, BAD_CAST "version\0", BAD_CAST AGS_NOTATION_CLIPBOARD_VERSION);
  xmlNewProp(notation_node, BAD_CAST "format\0", BAD_CAST AGS_NOTATION_CLIPBOARD_FORMAT);
  xmlNewProp(notation_node, BAD_CAST "base_frequency\0", BAD_CAST g_strdup_printf("%u\0", notation->base_frequency));

  selection = notation->selection;

  if(selection != NULL){
    x_boundary = AGS_NOTE(selection->data)->x[0];
    y_boundary = G_MAXUINT;
  }else{
    x_boundary = 0;
    y_boundary = 0;
  }

  while(selection != NULL){
    note = AGS_NOTE(selection->data);
    current_note = xmlNewChild(notation_node, NULL, BAD_CAST "note\0", NULL);

    xmlNewProp(current_note, BAD_CAST "x\0", BAD_CAST g_strdup_printf("%u\0", note->x[0]));
    xmlNewProp(current_note, BAD_CAST "x1\0", BAD_CAST g_strdup_printf("%u\0", note->x[1]));
    xmlNewProp(current_note, BAD_CAST "y\0", BAD_CAST g_strdup_printf("%u\0", note->y));

    if(y_boundary > note->y)
      y_boundary = note->y;

    selection = selection->next;
  }

  xmlNewProp(notation_node, BAD_CAST "x_boundary\0", BAD_CAST g_strdup_printf("%u\0", x_boundary));
  xmlNewProp(notation_node, BAD_CAST "y_boundary\0", BAD_CAST g_strdup_printf("%u\0", y_boundary));

  return(notation_node);
}

/**
 * ags_notation_cut_selection:
 * @notation: an #AgsNotation
 *
 * Cut selection to clipboard.
 *
 * Returns: the selection as XML.
 *
 * Since: 0.4
 */
xmlNode*
ags_notation_cut_selection(AgsNotation *notation)
{
  xmlNode *notation_node;
  GList *selection, *notes;
  
  notation_node = ags_notation_copy_selection(notation);

  selection = notation->selection;
  notes = notation->notes;

  while(selection != NULL){
    notes = g_list_find(notes, selection->data);

    if(notes->prev == NULL){
      notation->notes = g_list_remove_link(notes, notes);
      notes = notation->notes;
    }else{
      GList *next_note;

      next_note = notes->next;
      notes->prev->next = next_note;

      if(next_note != NULL)
	next_note->prev = notes->prev;

      g_list_free1(notes);

      notes = next_note;
    }

    AGS_NOTE(selection->data)->flags &= (~AGS_NOTE_IS_SELECTED);
    g_object_unref(selection->data);

    selection = selection->next;
  }

  ags_notation_free_selection(notation);

  return(notation_node);
}

/**
 * ags_notation_insert_native_piano_from_clipboard:
 * @notation: an #AgsNotation
 * @notation_node: the clipboard XML data
 * @version: clipboard version
 * @base_frequency: lowest frequency of notation
 * @x_boundary: region start offset
 * @y_boundary: region start tone
 * @reset_x_offset: if %TRUE @x_offset used as cursor
 * @x_offset: region start cursor offset
 * @reset_y_offset: if %TRUE @y_offset used as cursor
 * @y_offset: region start cursor tone
 *
 * Paste previously copied notes. 
 *
 * Since: 0.4
 */
void
ags_notation_insert_native_piano_from_clipboard(AgsNotation *notation,
						xmlNode *root_node, char *version,
						char *base_frequency,
						char *x_boundary, char *y_boundary,
						gboolean reset_x_offset, guint x_offset,
						gboolean reset_y_offset, guint y_offset)
{
  void ags_notation_insert_native_piano_from_clipboard_version_0_3_12(){
    AgsNote *note;
    xmlNode *node;
    char *endptr;
    guint x_boundary_val, y_boundary_val;
    char *x0, *x1, *y;
    guint x0_val, x1_val, y_val;
    guint base_x_difference, base_y_difference;
    gboolean subtract_x, subtract_y;

    node = root_node->children;

    /* retrieve x values for resetting */
    if(reset_x_offset){
      if(x_boundary != NULL){
	errno = 0;
	x_boundary_val = strtoul(x_boundary, &endptr, 10);

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
	y_boundary_val = strtoul(y_boundary, &endptr, 10);

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
    
    for(; node != NULL; node = node->next){
      if(node->type == XML_ELEMENT_NODE && !xmlStrncmp("note\0", node->name, 5)){
	/* retrieve x0 offset */
	x0 = xmlGetProp(node, "x\0");

	if(x0 == NULL)
	  continue;

	errno = 0;
	x0_val = strtoul(x0, &endptr, 10);

	if(errno == ERANGE){
	  continue;
	} 

	if(x0 == endptr){
	  continue;
	}

	/* retrieve x1 offset */
	x1 = xmlGetProp(node, "x1\0");

	if(x1 == NULL)
	  continue;

	errno = 0;
	x1_val = strtoul(x1, &endptr, 10);

	if(errno == ERANGE){
	  continue;
	} 

	if(x1 == endptr){
	  continue;
	}

	/* retrieve y offset */
	y = xmlGetProp(node, "y\0");

	if(y == NULL)
	  continue;

	errno = 0;
	y_val = strtoul(y, &endptr, 10);

	if(errno == ERANGE){
	  continue;
	} 

	if(y == endptr){
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

	    if(errno != 0)
	      continue;

	    x1_val -= base_x_difference;
	  }else{
	    x0_val += base_x_difference;
	    x1_val += base_x_difference;

	    if(errno != 0)
	      continue;
	  }
	}

	if(reset_y_offset){
	  errno = 0;

	  if(subtract_y){
	    y_val -= base_y_difference;
	  }else{
	    y_val += base_y_difference;
	  }

	  if(errno != 0)
	    continue;
	}

	/* check if max length wasn't exceeded */
	if(x1_val - x0_val > notation->maximum_note_length)
	  continue;

	/* add note */
	note = ags_note_new();

	note->x[0] = x0_val;
	note->x[1] = x1_val;

	note->y = y_val;

	g_message("adding note at: [%u,%u|%u]\n\0", x0_val, x1_val, y_val);

	ags_notation_add_note(notation,
			      note,
			      FALSE);
      }
    }

  }

  if(!xmlStrncmp("0.3.12\0", version, 7)){
    ags_notation_insert_native_piano_from_clipboard_version_0_3_12();
  }
}

/**
 * ags_notation_insert_from_clipboard:
 * @notation: an #AgsNotation
 * @notation_node: the clipboard XML data
 * @reset_x_offset: if %TRUE @x_offset used as cursor
 * @x_offset: region start cursor offset
 * @reset_y_offset: if %TRUE @y_offset used as cursor
 * @y_offset: region start cursor tone
 *
 * Paste previously copied notes. 
 *
 * Since: 0.4
 */
void
ags_notation_insert_from_clipboard(AgsNotation *notation,
				   xmlNodePtr notation_node,
				   gboolean reset_x_offset, guint x_offset,
				   gboolean reset_y_offset, guint y_offset)
{
  char *program, *version, *type, *format;
  char *base_frequency;
  char *x_boundary, *y_boundary;

  while(notation_node != NULL){
    if(notation_node->type == XML_ELEMENT_NODE && !xmlStrncmp("notation\0", notation_node->name, 9))
      break;

    notation_node = notation_node->next;
  }

  if(notation_node != NULL){
    program = xmlGetProp(notation_node, "program\0");

    if(!xmlStrncmp("ags\0", program, 4)){
      version = xmlGetProp(notation_node, "version\0");
      type = xmlGetProp(notation_node, "type\0");
      format = xmlGetProp(notation_node, "format\0");

      if(!xmlStrncmp("AgsNotationNativePiano\0", format, 22)){
	base_frequency = xmlGetProp(notation_node, "base_frequency\0");

	x_boundary = xmlGetProp(notation_node, "x_boundary\0");
	y_boundary = xmlGetProp(notation_node, "y_boundary\0");

	ags_notation_insert_native_piano_from_clipboard(notation,
							notation_node, version,
							base_frequency,
							x_boundary, y_boundary,
							reset_x_offset, x_offset,
							reset_y_offset, y_offset);
      }
    }
  }
}

GList*
ags_notation_get_current(AgsNotation *notation)
{
  GList *list;

  list = NULL;

  //TODO:JK: get current

  return(list);
}

/**
 * ags_notation_new:
 * @audio: an #AgsAudio
 * @audio_channel: the audio channel to be used
 *
 * Creates a #AgsNotation, assigned to @audio_channel.
 *
 * Returns: a new #AgsNotation
 *
 * Since: 0.4
 */
AgsNotation*
ags_notation_new(GObject *audio,
		 guint audio_channel)
{
  AgsNotation *notation;

  notation = (AgsNotation *) g_object_new(AGS_TYPE_NOTATION,
					  "audio\0", audio,
					  "audio-channel\0", audio_channel,
					  NULL);

  return(notation);
}
