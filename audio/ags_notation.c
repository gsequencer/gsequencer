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

#include <stdlib.h>

void ags_notation_class_init(AgsNotationClass *notation);
void ags_notation_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_notation_tactable_interface_init(AgsTactableInterface *tactable);
void ags_notation_init(AgsNotation *notation);
void ags_notation_connect(AgsConnectable *connectable);
void ags_notation_disconnect(AgsConnectable *connectable);
void ags_notation_finalize(GObject *object);

void ags_notation_change_bpm(AgsTactable *tactable, gdouble bpm, gdouble old_bpm);

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
  }

  return(ags_type_notation);
}

void 
ags_notation_class_init(AgsNotationClass *notation)
{
  GObjectClass *gobject;

  ags_notation_parent_class = g_type_class_peek_parent(notation);

  gobject = (GObjectClass *) notation;

  gobject->finalize = ags_notation_finalize;
}

void
ags_notation_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_notation_connect;
  connectable->disconnect = ags_notation_disconnect;
}

void
ags_notation_tactable_interface_init(AgsTactableInterface *tactable)
{
  tactable->change_bpm = ags_notation_change_bpm;
}

void
ags_notation_init(AgsNotation *notation)
{
  notation->flags = 0;

  notation->audio = NULL;

  notation->base_frequency = 0;

  notation->tact = g_strdup("1/4\0");
  notation->bpm = 120;

  notation->maximum_note_length = 256;

  notation->notes = NULL;
  notation->pads = 0;

  notation->selection = NULL;
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
ags_notation_finalize(GObject *gobject)
{
  AgsNotation *notation;
  GList *list, *list_next;

  notation = AGS_NOTATION(gobject);

  if(notation->tact != NULL)
    g_free(notation->tact);

  ags_list_free_and_unref_link(notation->notes);

  G_OBJECT_CLASS(ags_notation_parent_class)->finalize(gobject);
}

void
ags_notation_change_bpm(AgsTactable *tactable, gdouble bpm, gdouble old_bpm)
{
  /* empty */
}

void
ags_notation_add_note(AgsNotation *notation, AgsNote *note)
{
  GList *list, *list_new;
  void ags_notation_add_note_add1(){
    list_new = (GList *) malloc(sizeof(GList));
    list_new->data = (gpointer) note;

    if(list->prev == NULL){
      list_new->prev = NULL;
      list_new->next = list;
      list->prev = list_new;
      notation->notes = list_new;
    }else{
      list_new->prev = list->prev;
      list_new->next = list;
      list->prev = list_new;
      list_new->prev->next = list_new;
    }
  }

  list = notation->notes;

  if(list == NULL){
    list_new = g_list_alloc();
    list_new->data = (gpointer) note;
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

gboolean
ags_notation_remove_note_at_position(AgsNotation *notation,
				     guint x, guint y,
				     guint x_offset)
{
  AgsNote *note;
  GList *notes;
  guint x_start;

  notes = notation->notes;

  /* get entry point */
  while(notes != NULL && (note = AGS_NOTE(notes->data))->x[0] < x)
    notes = notes->next;

  /* search in y region for appropriate note */
  if(notes != NULL && note->x[0] == x){
    do{
      if(note->y == y){
	notation->notes = g_list_remove_link(notation->notes, notes);
	free(note);

	return(TRUE);
      }

      notes = notes->next;
    }while(notes != NULL && (note = AGS_NOTE(notes->data))->y <= y);
  }

  /* search backward until x_start */
  if(x_offset < notation->maximum_note_length){
    x_start = 0;
  }else{
    x_start = x_offset - notation->maximum_note_length;
  }

  while(notes != NULL && (note = AGS_NOTE(notes->data))->x[0] >= x_start){
    if(note->y == y){
      do{
	if(note->x[1] > x){
	  notation->notes = g_list_remove_link(notation->notes, notes);
	  free(note);
	
	  return(TRUE);
	}

	notes = notes->prev;
      }while(notes != NULL && (note = AGS_NOTE(notes->data))->y == y);

      continue;
    }

    notes = notes->prev;
  }

  return(FALSE);
}

GList*
ags_notation_get_selection(AgsNotation *notation)
{
  return(notation->selection);
}

gboolean
ags_notation_is_note_selected(AgsNotation *notation, AgsNote *note)
{

  return(FALSE);
}

void
ags_notation_add_point_to_selection(AgsNotation *notation,
				    guint x, guint y,
				    gboolean replace_current_selection)
{
  AgsNote *note;
  GList *notes, *notes_next;

  notes = notation->notes;

  while(notes != NULL && ((AgsNote *) notes->data)->x[0] < x){
    notes = notes->next;
  }

  if(notes == NULL){
    if(replace_current_selection){
      ags_list_free_and_free_link(notation->notes);
      notation->notes = NULL;
    }

    return;
  }

  while((notes_next = notes->next) != NULL && (note = AGS_NOTE(notes_next->data))->x[0] == x){
    if(note->y == y){
      {
	notes = notes_next;
      }while((notes_next = notes->next) != NULL && (note = AGS_NOTE(notes_next->data))->y == y);
    }

    notes = notes_next;
  }
}

void
ags_notation_remove_point_from_selection(AgsNotation *notation,
					 guint x, guint y)
{
}

void
ags_notation_add_region_to_selection(AgsNotation *notation,
				     guint x, guint y,
				     guint width, guint height,
				     gboolean replace_current_selection)
{
}

void
ags_notation_remove_region_from_selection(AgsNotation *notation,
					  guint x, guint y,
					  guint width, guint height)
{
}

AgsNotation*
ags_notation_new()
{
  AgsNotation *notation;

  notation = (AgsNotation *) g_object_new(AGS_TYPE_NOTATION, NULL);

  return(notation);
}
