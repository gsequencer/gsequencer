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
#include <errno.h>

void ags_notation_class_init(AgsNotationClass *notation);
void ags_notation_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_notation_tactable_interface_init(AgsTactableInterface *tactable);
void ags_notation_init(AgsNotation *notation);
void ags_notation_connect(AgsConnectable *connectable);
void ags_notation_disconnect(AgsConnectable *connectable);
void ags_notation_finalize(GObject *object);

void ags_notation_change_bpm(AgsTactable *tactable, gdouble bpm);

void ags_notation_insert_native_piano_from_clipboard(AgsNotation *notation,
						     xmlNodePtr root_node, char *version,
						     char *base_frequency,
						     char *x_boundary, char *y_boundary,
						     gboolean from_x_offset, guint x_offset,
						     gboolean from_y_offset, guint y_offset);

static gpointer ags_notation_parent_class = NULL;

#define AGS_NOTATION_CLIPBOARD_VERSION "0.3.12"
#define AGS_NOTATION_CLIPBOARD_TYPE "AgsNotationClipboardXml"
#define AGS_NOTATION_CLIPBOARD_FORMAT "AgsNotationNativePiano"

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
					       "AgsNotation",
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

  notation->audio_channel = 0;
  notation->audio = NULL;

  notation->base_frequency = 0;

  notation->tact = g_strdup("1/4");
  notation->bpm = 120;

  notation->maximum_note_length = 256;

  notation->notes = NULL;
  notation->start_loop = NULL;
  notation->end_loop = NULL;
  notation->offset = 0;

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
ags_notation_change_bpm(AgsTactable *tactable, gdouble bpm)
{
  /* empty */
}

void
ags_notation_add_note(AgsNotation *notation,
		      AgsNote *note,
		      gboolean use_selection_list)
{
  GList *list, *list_new;
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

gboolean
ags_notation_remove_note_at_position(AgsNotation *notation,
				     guint x, guint y)
{
  AgsNote *note;
  GList *notes;
  guint x_start;

  notes = notation->notes;

  /* get entry point */
  while(notes->next != NULL && (note = AGS_NOTE(notes->data))->x[0] < x)
    notes = notes->next;

  /* search in y region for appropriate note */
  if(notes != NULL && note->x[0] == x){
    do{
      if(note->y == y){
	notation->notes = g_list_delete_link(notation->notes, notes);
	g_object_unref(note);

	return(TRUE);
      } 

      notes = notes->next;
    }while(notes != NULL &&
	   (note = AGS_NOTE(notes->data))->x[0] == x &&
	   note->y <= y);
  }

  /* search backward until x_start */
  if(x < notation->maximum_note_length){
    x_start = 0;
  }else{
    x_start = x - notation->maximum_note_length;
  }

  while(notes != NULL && (note = AGS_NOTE(notes->data))->x[0] >= x_start){
    if(note->y == y){
      do{
	if(note->x[1] > x){
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
  GList *selection;

  selection = notation->selection;

  while(selection != NULL && AGS_NOTE(selection->data)->x[0] <= note->x[0]){
    if(selection->data == note)
      return(TRUE);

    selection = selection->next;
  }

  return(FALSE);
}

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

xmlNodePtr
ags_notation_copy_selection(AgsNotation *notation)
{
  AgsNote *note;
  xmlNodePtr notation_node, current_note;
  GList *selection;
  guint x_boundary, y_boundary;

  selection = notation->selection;

  /* create root node */
  notation_node = xmlNewNode(NULL, BAD_CAST "notation");

  xmlNewProp(notation_node, BAD_CAST "program", BAD_CAST "ags");
  xmlNewProp(notation_node, BAD_CAST "type", BAD_CAST AGS_NOTATION_CLIPBOARD_TYPE);
  xmlNewProp(notation_node, BAD_CAST "version", BAD_CAST AGS_NOTATION_CLIPBOARD_VERSION);
  xmlNewProp(notation_node, BAD_CAST "format", BAD_CAST AGS_NOTATION_CLIPBOARD_FORMAT);
  xmlNewProp(notation_node, BAD_CAST "base_frequency", BAD_CAST g_strdup_printf("%u", notation->base_frequency));

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
    current_note = xmlNewChild(notation_node, NULL, BAD_CAST "note", NULL);

    xmlNewProp(current_note, BAD_CAST "x", BAD_CAST g_strdup_printf("%u", note->x[0]));
    xmlNewProp(current_note, BAD_CAST "x1", BAD_CAST g_strdup_printf("%u", note->x[1]));
    xmlNewProp(current_note, BAD_CAST "y", BAD_CAST g_strdup_printf("%u", note->y));

    if(y_boundary > note->y)
      y_boundary = note->y;

    selection = selection->next;
  }

  xmlNewProp(notation_node, BAD_CAST "x_boundary", BAD_CAST g_strdup_printf("%u", x_boundary));
  xmlNewProp(notation_node, BAD_CAST "y_boundary", BAD_CAST g_strdup_printf("%u", y_boundary));

  return(notation_node);
}

xmlNodePtr
ags_notation_cut_selection(AgsNotation *notation)
{
  xmlNodePtr notation_node;
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

void
ags_notation_insert_native_piano_from_clipboard(AgsNotation *notation,
						xmlNodePtr root_node, char *version,
						char *base_frequency,
						char *x_boundary, char *y_boundary,
						gboolean reset_x_offset, guint x_offset,
						gboolean reset_y_offset, guint y_offset)
{
  void ags_notation_insert_native_piano_from_clipboard_version_0_3_12(){
    AgsNote *note;
    xmlNodePtr node;
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
      if(node->type == XML_ELEMENT_NODE && !xmlStrncmp("note", node->name, 5)){
	/* retrieve x0 offset */
	x0 = xmlGetProp(node, "x");

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
	x1 = xmlGetProp(node, "x1");

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
	y = xmlGetProp(node, "y");

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

	g_message("adding note at: [%u,%u|%u]\n", x0_val, x1_val, y_val);

	ags_notation_add_note(notation,
			      note,
			      FALSE);
      }
    }

  }

  if(!xmlStrncmp("0.3.12", version, 7)){
    ags_notation_insert_native_piano_from_clipboard_version_0_3_12();
  }
}

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
    if(notation_node->type == XML_ELEMENT_NODE && !xmlStrncmp("notation", notation_node->name, 9))
      break;

    notation_node = notation_node->next;
  }

  if(notation_node != NULL){
    program = xmlGetProp(notation_node, "program");

    if(!xmlStrncmp("ags", program, 4)){
      version = xmlGetProp(notation_node, "version");
      type = xmlGetProp(notation_node, "type");
      format = xmlGetProp(notation_node, "format");

      if(!xmlStrncmp("AgsNotationNativePiano", format, 22)){
	base_frequency = xmlGetProp(notation_node, "base_frequency");

	x_boundary = xmlGetProp(notation_node, "x_boundary");
	y_boundary = xmlGetProp(notation_node, "y_boundary");

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

AgsNotation*
ags_notation_new(guint audio_channel)
{
  AgsNotation *notation;

  notation = (AgsNotation *) g_object_new(AGS_TYPE_NOTATION, NULL);

  notation->audio_channel = audio_channel;

  return(notation);
}
