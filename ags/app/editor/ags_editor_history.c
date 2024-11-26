/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/editor/ags_editor_history.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_composite_editor.h>

#include <libxml/xlink.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

void ags_editor_history_class_init(AgsEditorHistoryClass *editor_history);
void ags_editor_history_init (AgsEditorHistory *editor_history);
void ags_editor_history_dispose(GObject *gobject);
void ags_editor_history_finalize(GObject *gobject);

/**
 * SECTION:ags_editor_history
 * @short_description: Singleton pattern to organize LADSPA
 * @title: AgsEditorHistory
 * @section_id:
 * @include: ags/plugin/ags_editor_history.h
 *
 * The #AgsEditorHistory loads/unloads LADSPA plugins.
 */

static gpointer ags_editor_history_parent_class = NULL;

AgsEditorHistory *ags_editor_history = NULL;

GType
ags_editor_history_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_editor_history = 0;

    static const GTypeInfo ags_editor_history_info = {
      sizeof (AgsEditorHistoryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_editor_history_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsEditorHistory),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_editor_history_init,
    };

    ags_type_editor_history = g_type_register_static(G_TYPE_OBJECT,
						     "AgsEditorHistory",
						     &ags_editor_history_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_editor_history);
  }

  return g_define_type_id__volatile;
}

void
ags_editor_history_class_init(AgsEditorHistoryClass *editor_history)
{
  GObjectClass *gobject;

  ags_editor_history_parent_class = g_type_class_peek_parent(editor_history);

  /* GObjectClass */
  gobject = (GObjectClass *) editor_history;

  gobject->dispose = ags_editor_history_dispose;
  gobject->finalize = ags_editor_history_finalize;
}

void
ags_editor_history_init(AgsEditorHistory *editor_history)
{
  editor_history->encoding = "UTF-8";
  
  editor_history->journal_position = -1;
  editor_history->edit_position = -1;

  editor_history->journal_entry = NULL;
}

void
ags_editor_history_dispose(GObject *gobject)
{
  AgsEditorHistory *editor_history;

  editor_history = AGS_EDITOR_HISTORY(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_editor_history_parent_class)->dispose(gobject);
}

void
ags_editor_history_finalize(GObject *gobject)
{
  AgsEditorHistory *editor_history;
  
  editor_history = AGS_EDITOR_HISTORY(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_editor_history_parent_class)->finalize(gobject);
}

/**
 * ags_extern_data_alloc:
 *
 * Alloc editor journal.
 *
 * Returns: (transfer full): the newly allocated #AgsExternData-struct
 * 
 * Since: 7.2.0 
 */
AgsExternData*
ags_extern_data_alloc()
{
  AgsExternData *extern_data;
  
  extern_data = (AgsExternData *) g_malloc(sizeof(AgsExternData));

  extern_data->ref_count = 1;

  extern_data->data_uuid = g_uuid_string_random();
  
  extern_data->data_length = 0;
  extern_data->data = NULL;

  return(extern_data);
}

/**
 * ags_extern_data_free:
 * @extern_data: the #AgsExternData-struct
 *
 * Free @extern_data.
 *
 * Since: 7.2.0 
 */
void
ags_extern_data_free(AgsExternData *extern_data)
{
  g_return_if_fail(extern_data != NULL);

  g_free(extern_data->data_uuid);
  
  g_free(extern_data->data);
  
  g_free(extern_data);
}

/**
 * ags_extern_data_free:
 * @a: the #AgsExternData-struct
 * @b: the other #AgsExternData-struct
 *
 * Compare @a to @b.
 *
 * Since: 7.2.0 
 */
gint
ags_extern_data_cmp(AgsExternData *a,
		    AgsExternData *b)
{
  return(g_strcmp0(a->data_uuid, b->data_uuid));
}
			 
/**
 * ags_editor_journal_alloc:
 *
 * Alloc editor journal.
 *
 * Returns: (transfer full): the newly allocated #AgsEditorJournal-struct
 * 
 * Since: 7.2.0 
 */
AgsEditorJournal*
ags_editor_journal_alloc()
{
  AgsEditorJournal *editor_journal;
  
  editor_journal = (AgsEditorJournal *) g_malloc(sizeof(AgsEditorJournal));

  editor_journal->selected_machine_uuid = NULL;
  editor_journal->selected_machine = NULL;

  editor_journal->pad = 0;
  editor_journal->audio_channel = 0;
  editor_journal->line = 0;

  editor_journal->specifier = NULL;

  editor_journal->scope = NULL;
  editor_journal->journal_type = NULL;

  editor_journal->action_uuid = NULL;

  editor_journal->action = NULL;
  editor_journal->detail = NULL;

  editor_journal->data_access_type = NULL;
  editor_journal->content_type = NULL;

  editor_journal->orig_data_offset = 0;
  editor_journal->orig_data_length = 0;
  editor_journal->orig_data = NULL;

  editor_journal->extern_orig_data = NULL;

  editor_journal->new_data_offset = 0;
  editor_journal->new_data_length = 0;
  editor_journal->new_data = NULL;

  editor_journal->extern_new_data = NULL;

  return(editor_journal);
}

/**
 * ags_editor_journal_free:
 * @editor_journal: the #AgsEditorJournal-struct
 *
 * Free @editor_journal.
 *
 * Since: 7.2.0 
 */
void
ags_editor_journal_free(AgsEditorJournal *editor_journal)
{
  g_return_if_fail(editor_journal != NULL);

  g_free(editor_journal->scope);
  g_free(editor_journal->journal_type);
  
  g_free(editor_journal->action);
  g_free(editor_journal->detail);
  
  g_free(editor_journal->data_access_type);
  g_free(editor_journal->content_type);
  
  g_free(editor_journal->orig_data);

  g_free(editor_journal->new_data);

  g_free(editor_journal);
}

/**
 * ags_editor_history_append:
 * @editor_history: the #AgsEditorHistory
 * @editor_journal: the #AgsEditorJournal
 *
 * Append @editor_journal to @editor_history.
 *
 * Since: 7.2.0 
 */
void
ags_editor_history_append(AgsEditorHistory *editor_history,
			  AgsEditorJournal *editor_journal)
{
  g_return_if_fail(AGS_IS_EDITOR_HISTORY(editor_history));
  g_return_if_fail(editor_journal != NULL);
  
  editor_history->journal_entry = g_list_prepend(editor_history->journal_entry,
						 editor_journal);
}

/**
 * ags_editor_journal_undo:
 * @editor_journal: the #AgsEditorJournal
 *
 * Undo @editor_journal.
 *
 * Since: 7.2.0 
 */
void
ags_editor_history_undo(AgsEditorHistory *editor_history)
{
  AgsCompositeEditor *composite_editor;
  
  AgsTimestamp *timestamp;

  AgsApplicationContext *application_context;
  
  AgsEditorJournal *editor_journal;
  AgsEditorJournal *new_editor_journal;

  GList *start_list, *list;

  xmlChar *str;
  gchar *action_uuid;
  
  gint edit_position;
  guint pad;
  guint audio_channel;
  guint line;

  if(editor_history->journal_entry == NULL){
    return;
  }

  application_context = ags_application_context_get_instance();

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));
  
  edit_position = editor_history->edit_position;
  
  if(edit_position == -1){
    edit_position = g_list_length(editor_history->journal_entry);
  }

  action_uuid = NULL;
  
  do{
    edit_position--;

    editor_journal = g_list_nth_data(editor_history->journal_entry,
				     edit_position);

    if(action_uuid == NULL){
      action_uuid = g_strdup(editor_journal->action_uuid);
    }else{
      if(editor_journal == NULL ||
	 (!g_ascii_strncasecmp(editor_journal->action_uuid, action_uuid, AGS_UUID_STRING_DEFAULT_LENGTH)) == FALSE){
	break;
      }
    }
    
    /* change compsited editor machine */
    if(editor_journal->selected_machine != composite_editor->selected_machine){
      ags_composite_editor_machine_changed(composite_editor,
					   (AgsMachine *) editor_journal->selected_machine);
    }
  
    timestamp = ags_timestamp_new();
    ags_timestamp_set_flags(timestamp,
			    AGS_TIMESTAMP_OFFSET);
  
    if(!g_ascii_strncasecmp(editor_journal->scope,
			    AGS_EDITOR_HISTORY_SCOPE_NOTATION,
			    strlen(AGS_EDITOR_HISTORY_SCOPE_NOTATION))){
      if(!g_ascii_strncasecmp(editor_journal->action,
			      AGS_EDITOR_HISTORY_ACTION_NOTE_ADD,
			      strlen(AGS_EDITOR_HISTORY_ACTION_NOTE_ADD))){
	AgsNote *current_note;      
	AgsNote *add_note;

	GList *start_note, *note;	

	guint x0, x1;
	guint y;
      
	ags_timestamp_set_ags_offset(timestamp,
				     (guint64) floor((double) editor_journal->new_data_offset / AGS_NOTATION_DEFAULT_OFFSET) * AGS_NOTATION_DEFAULT_OFFSET);

	start_list = ags_audio_get_notation(AGS_MACHINE(editor_journal->selected_machine)->audio);
      
	audio_channel = editor_journal->audio_channel;
      
	list = ags_notation_find_near_timestamp(start_list, audio_channel,
						timestamp);

	add_note = ags_note_from_string(editor_journal->new_data);

	x0 = ags_note_get_x0(add_note);
	x1 = ags_note_get_x1(add_note);

	y = ags_note_get_y(add_note);

	g_object_unref(add_note);
      
	current_note = ags_notation_find_point(list->data,
					       x0, y,
					       FALSE);
      
	if(current_note != NULL){
	  if(ags_note_get_x0(current_note) == x0 &&
	     ags_note_get_x1(current_note) == x1 &&
	     ags_note_get_y(current_note) == y){
	    ags_notation_remove_note(list->data,
				     current_note,
				     FALSE);
	  }else{
	    start_note = ags_notation_get_note(list->data);

	    note = g_list_find(start_note,
			       current_note);

	    while(note != NULL){
	      /* remove if match */
	      if(ags_note_get_x0(note->data) == x0 &&
		 ags_note_get_x1(note->data) == x1 &&
		 ags_note_get_y(note->data) == y){
		ags_notation_remove_note(list->data,
					 note->data,
					 FALSE);
	      
		break;
	      }

	      /* premature end */
	      if(ags_note_get_x0(note->data) != x0){
		break;
	      }
	    
	      note = note->next;
	    }
	  
	    g_list_free_full(start_note,
			     (GDestroyNotify) g_object_unref);
	  }
	}

	g_list_free_full(start_list,
			 (GDestroyNotify) g_object_unref);
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_NOTE_RESIZE,
				    strlen(AGS_EDITOR_HISTORY_ACTION_NOTE_RESIZE))){
	AgsNote *current_note;      
	AgsNote *new_resize_note;
	AgsNote *orig_resize_note;

	GList *start_note, *note;	

	guint x0, x1;
	guint orig_x1;
	guint y;
      
	ags_timestamp_set_ags_offset(timestamp,
				     (guint64) floor((double) editor_journal->new_data_offset / AGS_NOTATION_DEFAULT_OFFSET) * AGS_NOTATION_DEFAULT_OFFSET);

	start_list = ags_audio_get_notation(AGS_MACHINE(editor_journal->selected_machine)->audio);
      
	audio_channel = editor_journal->audio_channel;
      
	list = ags_notation_find_near_timestamp(start_list, audio_channel,
						timestamp);

	new_resize_note = ags_note_from_string(editor_journal->new_data);

	x0 = ags_note_get_x0(new_resize_note);
	x1 = ags_note_get_x1(new_resize_note);

	y = ags_note_get_y(new_resize_note);

	g_object_unref(new_resize_note);
      
	orig_resize_note = ags_note_from_string(editor_journal->orig_data);

	orig_x1 = ags_note_get_x1(new_resize_note);

	g_object_unref(orig_resize_note);
      
	current_note = ags_notation_find_point(list->data,
					       x0, y,
					       FALSE);
      
	if(current_note != NULL){
	  if(ags_note_get_x0(current_note) == x0 &&
	     ags_note_get_x1(current_note) == x1 &&
	     ags_note_get_y(current_note) == y){
	    ags_note_set_x1(current_note,
			    orig_x1);
	  }else{
	    start_note = ags_notation_get_note(list->data);

	    note = g_list_find(start_note,
			       current_note);

	    while(note != NULL){
	      /* resize if match */
	      if(ags_note_get_x0(note->data) == x0 &&
		 ags_note_get_x1(note->data) == x1 &&
		 ags_note_get_y(note->data) == y){
		ags_note_set_x1(current_note,
				orig_x1);
	      
		break;
	      }

	      /* premature end */
	      if(ags_note_get_x0(note->data) != x0){
		break;
	      }
	    
	      note = note->next;
	    }
	  
	    g_list_free_full(start_note,
			     (GDestroyNotify) g_object_unref);
	  }
	}

	g_list_free_full(start_list,
			 (GDestroyNotify) g_object_unref);
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_NOTE_REMOVE,
				    strlen(AGS_EDITOR_HISTORY_ACTION_NOTE_REMOVE))){
	AgsNote *current_note;      
	AgsNote *remove_note;

	GList *start_note, *note;	
			      
	xmlChar *str;

	guint x0, x1;
	guint y;
      
	ags_timestamp_set_ags_offset(timestamp,
				     (guint64) floor((double) editor_journal->new_data_offset / AGS_NOTATION_DEFAULT_OFFSET) * AGS_NOTATION_DEFAULT_OFFSET);

	start_list = ags_audio_get_notation(AGS_MACHINE(editor_journal->selected_machine)->audio);
      
	audio_channel = editor_journal->audio_channel;
      
	list = ags_notation_find_near_timestamp(start_list, audio_channel,
						timestamp);

	remove_note = ags_note_from_string(editor_journal->new_data);
      
	ags_notation_add_note(list->data,
			      remove_note,
			      FALSE);

	g_list_free_full(start_list,
			 (GDestroyNotify) g_object_unref);
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_NOTE_COPY,
				    strlen(AGS_EDITOR_HISTORY_ACTION_NOTE_COPY))){
	/* write to clipboard */
	gdk_clipboard_set_text(gdk_display_get_clipboard(gdk_display_get_default()),
			       editor_journal->orig_data);
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_NOTE_CUT,
				    strlen(AGS_EDITOR_HISTORY_ACTION_NOTE_CUT))){      
	/* write to clipboard */
	gdk_clipboard_set_text(gdk_display_get_clipboard(gdk_display_get_default()),
			       editor_journal->orig_data);

	ags_composite_editor_set_flags(composite_editor,
				       AGS_COMPOSITE_EDITOR_PRESERVE_HISTORY);
      
	ags_composite_editor_paste(composite_editor);

	ags_composite_editor_unset_flags(composite_editor,
					 AGS_COMPOSITE_EDITOR_PRESERVE_HISTORY);
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_NOTE_PASTE,
				    strlen(AGS_EDITOR_HISTORY_ACTION_NOTE_PASTE))){
	xmlDoc *clipboard;
	xmlNode *audio_node, *notation_list_node, *notation_node;
	xmlNode *child;

	start_list = ags_audio_get_notation(AGS_MACHINE(editor_journal->selected_machine)->audio);

	ags_notation_free_all_selection(start_list);
      
	clipboard = xmlReadMemory(editor_journal->orig_data, editor_journal->orig_data_length,
				  NULL, "UTF-8",
				  0);

	audio_node = xmlDocGetRootElement(clipboard);

	while(audio_node != NULL){
	  if(audio_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(audio_node->name,
			   BAD_CAST "audio",
			   6)){
	      notation_list_node = audio_node->children;

	      while(notation_list_node != NULL){
		if(notation_list_node->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(notation_list_node->name,
				 BAD_CAST "notation-list",
				 14)){
		    notation_node = notation_list_node->children;
	  
		    while(notation_node != NULL){
		      if(notation_node->type == XML_ELEMENT_NODE){
			if(!xmlStrncmp(notation_node->name,
				       BAD_CAST "notation",
				       9)){
			  guint audio_channel;

			  /* audio-channel */
			  str = xmlGetProp(notation_node,
					   BAD_CAST "audio-channel");

			  audio_channel = g_ascii_strtoull(str,
							   NULL,
							   10);

			  xmlFree(str);
			
			  child = notation_node->children;
			
			  while(child != NULL){
			    if(child->type == XML_ELEMENT_NODE){
			      if(!xmlStrncmp(child->name,
					     BAD_CAST "note",
					     5)){
				AgsNote *current_note;      

				GList *start_note, *note;
			      
				guint x0, x1;
				guint y;

				/* x0 */
				str = xmlGetProp(child,
						 BAD_CAST "x0");

				x0 = g_ascii_strtoull(str,
						      NULL,
						      10);
			      
				xmlFree(str);

				/* x1 */
				str = xmlGetProp(child,
						 BAD_CAST "x1");

				x1 = g_ascii_strtoull(str,
						      NULL,
						      10);
			      
				xmlFree(str);

				/* y */
				str = xmlGetProp(child,
						 BAD_CAST "y");

				y = g_ascii_strtoull(str,
						     NULL,
						     10);
			      
				xmlFree(str);

				/*  */
				ags_timestamp_set_ags_offset(timestamp,
							     (guint64) floor((double) x0 / AGS_NOTATION_DEFAULT_OFFSET) * AGS_NOTATION_DEFAULT_OFFSET);

				list = ags_notation_find_near_timestamp(start_list, audio_channel,
									timestamp);

				current_note = ags_notation_find_point(list->data,
								       x0, y,
								       FALSE);
      
				if(current_note != NULL){
				  if(ags_note_get_x0(current_note) == x0 &&
				     ags_note_get_x1(current_note) == x1 &&
				     ags_note_get_y(current_note) == y){
				    ags_notation_add_note(list->data,
							  current_note,
							  TRUE);
				  }else{
				    start_note = ags_notation_get_note(list->data);

				    note = g_list_find(start_note,
						       current_note);

				    while(note != NULL){
				      /* remove if match */
				      if(ags_note_get_x0(note->data) == x0 &&
					 ags_note_get_x1(note->data) == x1 &&
					 ags_note_get_y(note->data) == y){
					ags_notation_add_note(list->data,
							      note->data,
							      TRUE);
	      
					break;
				      }

				      /* premature end */
				      if(ags_note_get_x0(note->data) != x0){
					break;
				      }
	    
				      note = note->next;
				    }
	  
				    g_list_free_full(start_note,
						     (GDestroyNotify) g_object_unref);
				  }
				}
			      }
			    }

			    child = child->next;
			  }
			}
		      }
		    
		      notation_node = notation_node->next;
		    }	  
		  }
		}
	      
		notation_list_node = notation_list_node->next;
	      }    
	    }
	  }
      
	  audio_node = audio_node->next;
	}

	ags_composite_editor_set_flags(composite_editor,
				       AGS_COMPOSITE_EDITOR_PRESERVE_HISTORY);
      
	ags_composite_editor_cut(composite_editor);

	ags_composite_editor_unset_flags(composite_editor,
					 AGS_COMPOSITE_EDITOR_PRESERVE_HISTORY);
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_NOTE_INVERT,
				    strlen(AGS_EDITOR_HISTORY_ACTION_NOTE_INVERT))){
	xmlDoc *selection;
	xmlNode *audio_node, *notation_list_node, *notation_node;
	xmlNode *child;
      
	GList *start_notation, *notation;
      
	start_list = ags_audio_get_notation(AGS_MACHINE(editor_journal->selected_machine)->audio);

	ags_notation_free_all_selection(start_list);

	selection = xmlReadMemory(editor_journal->orig_data, editor_journal->orig_data_length,
				  NULL, "UTF-8",
				  0);

	audio_node = xmlDocGetRootElement(selection);

	while(audio_node != NULL){
	  if(audio_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(audio_node->name,
			   BAD_CAST "audio",
			   6)){
	      notation_list_node = audio_node->children;

	      while(notation_list_node != NULL){
		if(notation_list_node->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(notation_list_node->name,
				 BAD_CAST "notation-list",
				 14)){
		    notation_node = notation_list_node->children;
	  
		    while(notation_node != NULL){
		      if(notation_node->type == XML_ELEMENT_NODE){
			if(!xmlStrncmp(notation_node->name,
				       BAD_CAST "notation",
				       9)){
			  guint audio_channel;

			  /* audio-channel */
			  str = xmlGetProp(notation_node,
					   BAD_CAST "audio-channel");

			  audio_channel = g_ascii_strtoull(str,
							   NULL,
							   10);

			  xmlFree(str);
			
			  child = notation_node->children;
			
			  while(child != NULL){
			    if(child->type == XML_ELEMENT_NODE){
			      if(!xmlStrncmp(child->name,
					     BAD_CAST "note",
					     5)){
				AgsNote *current_note;      

				GList *start_note, *note;
			      
				guint x0, x1;
				guint y;

				/* x0 */
				str = xmlGetProp(child,
						 BAD_CAST "x0");

				x0 = g_ascii_strtoull(str,
						      NULL,
						      10);
			      
				xmlFree(str);

				/* x1 */
				str = xmlGetProp(child,
						 BAD_CAST "x1");

				x1 = g_ascii_strtoull(str,
						      NULL,
						      10);
			      
				xmlFree(str);

				/* y */
				str = xmlGetProp(child,
						 BAD_CAST "y");

				y = g_ascii_strtoull(str,
						     NULL,
						     10);
			      
				xmlFree(str);

				/*  */
				ags_timestamp_set_ags_offset(timestamp,
							     (guint64) floor((double) x0 / AGS_NOTATION_DEFAULT_OFFSET) * AGS_NOTATION_DEFAULT_OFFSET);

				list = ags_notation_find_near_timestamp(start_list, audio_channel,
									timestamp);

				current_note = ags_notation_find_point(list->data,
								       x0, y,
								       FALSE);
      
				if(current_note != NULL){
				  if(ags_note_get_x0(current_note) == x0 &&
				     ags_note_get_x1(current_note) == x1 &&
				     ags_note_get_y(current_note) == y){
				    ags_notation_add_note(list->data,
							  current_note,
							  TRUE);
				  }else{
				    start_note = ags_notation_get_note(list->data);

				    note = g_list_find(start_note,
						       current_note);

				    while(note != NULL){
				      /* remove if match */
				      if(ags_note_get_x0(note->data) == x0 &&
					 ags_note_get_x1(note->data) == x1 &&
					 ags_note_get_y(note->data) == y){
					ags_notation_add_note(list->data,
							      note->data,
							      TRUE);
	      
					break;
				      }

				      /* premature end */
				      if(ags_note_get_x0(note->data) != x0){
					break;
				      }
	    
				      note = note->next;
				    }
	  
				    g_list_free_full(start_note,
						     (GDestroyNotify) g_object_unref);
				  }
				}
			      }
			    }

			    child = child->next;
			  }
			}
		      }
		    
		      notation_node = notation_node->next;
		    }	  
		  }
		}
	      
		notation_list_node = notation_list_node->next;
	      }    
	    }
	  }
      
	  audio_node = audio_node->next;
	}

	ags_composite_editor_set_flags(composite_editor,
				       AGS_COMPOSITE_EDITOR_PRESERVE_HISTORY);
      
	ags_composite_editor_invert(composite_editor);

	ags_composite_editor_unset_flags(composite_editor,
					 AGS_COMPOSITE_EDITOR_PRESERVE_HISTORY);
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_NOTE_SELECT,
				    strlen(AGS_EDITOR_HISTORY_ACTION_NOTE_SELECT))){
	xmlDoc *selection;
	xmlNode *audio_node, *notation_list_node, *notation_node;
	xmlNode *child;
      
	GList *start_notation, *notation;
      
	start_list = ags_audio_get_notation(AGS_MACHINE(editor_journal->selected_machine)->audio);

	ags_notation_free_all_selection(start_list);

	selection = xmlReadMemory(editor_journal->orig_data, editor_journal->orig_data_length,
				  NULL, "UTF-8",
				  0);

	audio_node = xmlDocGetRootElement(selection);

	while(audio_node != NULL){
	  if(audio_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(audio_node->name,
			   BAD_CAST "audio",
			   6)){
	      notation_list_node = audio_node->children;

	      while(notation_list_node != NULL){
		if(notation_list_node->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(notation_list_node->name,
				 BAD_CAST "notation-list",
				 14)){
		    notation_node = notation_list_node->children;
	  
		    while(notation_node != NULL){
		      if(notation_node->type == XML_ELEMENT_NODE){
			if(!xmlStrncmp(notation_node->name,
				       BAD_CAST "notation",
				       9)){
			  guint audio_channel;

			  /* audio-channel */
			  str = xmlGetProp(notation_node,
					   BAD_CAST "audio-channel");

			  audio_channel = g_ascii_strtoull(str,
							   NULL,
							   10);

			  xmlFree(str);
			
			  child = notation_node->children;
			
			  while(child != NULL){
			    if(child->type == XML_ELEMENT_NODE){
			      if(!xmlStrncmp(child->name,
					     BAD_CAST "note",
					     5)){
				AgsNote *current_note;      

				GList *start_note, *note;
			      
				guint x0, x1;
				guint y;

				/* x0 */
				str = xmlGetProp(child,
						 BAD_CAST "x0");

				x0 = g_ascii_strtoull(str,
						      NULL,
						      10);
			      
				xmlFree(str);

				/* x1 */
				str = xmlGetProp(child,
						 BAD_CAST "x1");

				x1 = g_ascii_strtoull(str,
						      NULL,
						      10);
			      
				xmlFree(str);

				/* y */
				str = xmlGetProp(child,
						 BAD_CAST "y");

				y = g_ascii_strtoull(str,
						     NULL,
						     10);
			      
				xmlFree(str);

				/*  */
				ags_timestamp_set_ags_offset(timestamp,
							     (guint64) floor((double) x0 / AGS_NOTATION_DEFAULT_OFFSET) * AGS_NOTATION_DEFAULT_OFFSET);

				list = ags_notation_find_near_timestamp(start_list, audio_channel,
									timestamp);

				current_note = ags_notation_find_point(list->data,
								       x0, y,
								       FALSE);
      
				if(current_note != NULL){
				  if(ags_note_get_x0(current_note) == x0 &&
				     ags_note_get_x1(current_note) == x1 &&
				     ags_note_get_y(current_note) == y){
				    ags_notation_add_note(list->data,
							  current_note,
							  TRUE);
				  }else{
				    start_note = ags_notation_get_note(list->data);

				    note = g_list_find(start_note,
						       current_note);

				    while(note != NULL){
				      /* remove if match */
				      if(ags_note_get_x0(note->data) == x0 &&
					 ags_note_get_x1(note->data) == x1 &&
					 ags_note_get_y(note->data) == y){
					ags_notation_add_note(list->data,
							      note->data,
							      TRUE);
	      
					break;
				      }

				      /* premature end */
				      if(ags_note_get_x0(note->data) != x0){
					break;
				      }
	    
				      note = note->next;
				    }
	  
				    g_list_free_full(start_note,
						     (GDestroyNotify) g_object_unref);
				  }
				}
			      }
			    }

			    child = child->next;
			  }
			}
		      }
		    
		      notation_node = notation_node->next;
		    }	  
		  }
		}
	      
		notation_list_node = notation_list_node->next;
	      }    
	    }
	  }
      
	  audio_node = audio_node->next;
	}

	g_list_free_full(start_list,
			 (GDestroyNotify) g_object_unref);
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_POPOVER_NOTE_SELECT,
				    strlen(AGS_EDITOR_HISTORY_ACTION_POPOVER_NOTE_SELECT))){
	xmlDoc *doc;
	xmlNode *root_node;

	doc = xmlReadMemory(editor_journal->orig_data, editor_journal->orig_data_length,
			    NULL, "UTF-8",
			    0);
	
	root_node = xmlDocGetRootElement(doc);

	ags_connectable_xml_parse(AGS_CONNECTABLE(composite_editor->toolbar->notation_select_note),
				  root_node);
	ags_applicable_apply(AGS_APPLICABLE(composite_editor->toolbar->notation_select_note));
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_POPOVER_NOTE_POSITION,
				    strlen(AGS_EDITOR_HISTORY_ACTION_POPOVER_NOTE_POSITION))){
	xmlDoc *doc;
	xmlNode *root_node;

	doc = xmlReadMemory(editor_journal->orig_data, editor_journal->orig_data_length,
			    NULL, "UTF-8",
			    0);
	
	root_node = xmlDocGetRootElement(doc);

	ags_connectable_xml_parse(AGS_CONNECTABLE(composite_editor->toolbar->notation_position_cursor),
				  root_node);
	ags_applicable_apply(AGS_APPLICABLE(composite_editor->toolbar->notation_position_cursor));
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_POPOVER_NOTE_MOVE,
				    strlen(AGS_EDITOR_HISTORY_ACTION_POPOVER_NOTE_MOVE))){
	xmlDoc *doc;
	xmlNode *root_node;

	doc = xmlReadMemory(editor_journal->orig_data, editor_journal->orig_data_length,
			    NULL, "UTF-8",
			    0);
	
	root_node = xmlDocGetRootElement(doc);

	ags_connectable_xml_parse(AGS_CONNECTABLE(composite_editor->toolbar->notation_move_note),
				  root_node);
	ags_applicable_apply(AGS_APPLICABLE(composite_editor->toolbar->notation_move_note));
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_POPOVER_NOTE_CROP,
				    strlen(AGS_EDITOR_HISTORY_ACTION_POPOVER_NOTE_CROP))){
	xmlDoc *doc;
	xmlNode *root_node;

	doc = xmlReadMemory(editor_journal->orig_data, editor_journal->orig_data_length,
			    NULL, "UTF-8",
			    0);
	
	root_node = xmlDocGetRootElement(doc);

	ags_connectable_xml_parse(AGS_CONNECTABLE(composite_editor->toolbar->notation_crop_note),
				  root_node);
	ags_applicable_apply(AGS_APPLICABLE(composite_editor->toolbar->notation_crop_note));
      }
    }else if(!g_ascii_strncasecmp(editor_journal->scope,
				  AGS_EDITOR_HISTORY_SCOPE_AUTOMATION,
				  strlen(AGS_EDITOR_HISTORY_SCOPE_AUTOMATION))){
      if(!g_ascii_strncasecmp(editor_journal->action,
			      AGS_EDITOR_HISTORY_ACTION_ACCELERATION_ADD,
			      strlen(AGS_EDITOR_HISTORY_ACTION_ACCELERATION_ADD))){
	//TODO:JK: implement me
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_ACCELERATION_RESIZE,
				    strlen(AGS_EDITOR_HISTORY_ACTION_ACCELERATION_RESIZE))){
	//TODO:JK: implement me
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_ACCELERATION_REMOVE,
				    strlen(AGS_EDITOR_HISTORY_ACTION_ACCELERATION_REMOVE))){
	//TODO:JK: implement me
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_ACCELERATION_COPY,
				    strlen(AGS_EDITOR_HISTORY_ACTION_ACCELERATION_COPY))){
	//TODO:JK: implement me
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_ACCELERATION_CUT,
				    strlen(AGS_EDITOR_HISTORY_ACTION_ACCELERATION_CUT))){
	//TODO:JK: implement me
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_ACCELERATION_PASTE,
				    strlen(AGS_EDITOR_HISTORY_ACTION_ACCELERATION_PASTE))){
	//TODO:JK: implement me
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_ACCELERATION_SELECT,
				    strlen(AGS_EDITOR_HISTORY_ACTION_ACCELERATION_SELECT))){
	//TODO:JK: implement me
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_POPOVER_ACCELERATION_POSITION,
				    strlen(AGS_EDITOR_HISTORY_ACTION_POPOVER_ACCELERATION_POSITION))){
	//TODO:JK: implement me
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_POPOVER_ACCELERATION_RAMP,
				    strlen(AGS_EDITOR_HISTORY_ACTION_POPOVER_ACCELERATION_RAMP))){
	//TODO:JK: implement me
      }
    }else if(!g_ascii_strncasecmp(editor_journal->scope,
				  AGS_EDITOR_HISTORY_SCOPE_WAVE,
				  strlen(AGS_EDITOR_HISTORY_SCOPE_WAVE))){
      if(!g_ascii_strncasecmp(editor_journal->action,
			      AGS_EDITOR_HISTORY_ACTION_BUFFER_ADD,
			      strlen(AGS_EDITOR_HISTORY_ACTION_BUFFER_ADD))){
	//TODO:JK: implement me
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_BUFFER_REMOVE,
				    strlen(AGS_EDITOR_HISTORY_ACTION_BUFFER_REMOVE))){
	//TODO:JK: implement me
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_BUFFER_COPY,
				    strlen(AGS_EDITOR_HISTORY_ACTION_BUFFER_COPY))){
	//TODO:JK: implement me
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_BUFFER_CUT,
				    strlen(AGS_EDITOR_HISTORY_ACTION_BUFFER_CUT))){
	//TODO:JK: implement me
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_BUFFER_PASTE,
				    strlen(AGS_EDITOR_HISTORY_ACTION_BUFFER_PASTE))){
	//TODO:JK: implement me
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_BUFFER_SELECT,
				    strlen(AGS_EDITOR_HISTORY_ACTION_BUFFER_SELECT))){
	//TODO:JK: implement me
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_POPOVER_BUFFER_POSITION,
				    strlen(AGS_EDITOR_HISTORY_ACTION_POPOVER_BUFFER_POSITION))){
	//TODO:JK: implement me
      }else if(!g_ascii_strncasecmp(editor_journal->action,
				    AGS_EDITOR_HISTORY_ACTION_POPOVER_BUFFER_TIME_STRETCH,
				    strlen(AGS_EDITOR_HISTORY_ACTION_POPOVER_BUFFER_TIME_STRETCH))){
	//TODO:JK: implement me
      }
    }
  }while(edit_position >= 0);
  
  /* reset position */
  editor_history->edit_position = edit_position;

  g_free(action_uuid);
}

/**
 * ags_editor_journal_redo:
 * @editor_journal: the #AgsEditorJournal
 *
 * Redo @editor_journal.
 *
 * Since: 7.2.0 
 */
void
ags_editor_history_redo(AgsEditorHistory *editor_history)
{
  AgsEditorJournal *editor_journal;
  AgsEditorJournal *new_editor_journal;
  
  gint edit_position;

  if(editor_history->journal_entry == NULL ||
     editor_history->edit_position == -1){
    return;
  }
  
  edit_position = editor_history->edit_position;
  
  edit_position++;

  editor_journal = g_list_nth_data(editor_history->journal_entry,
				   edit_position);

  if(!g_ascii_strncasecmp(editor_journal->scope,
			  AGS_EDITOR_HISTORY_SCOPE_NOTATION,
			  strlen(AGS_EDITOR_HISTORY_SCOPE_NOTATION))){
    if(!g_ascii_strncasecmp(editor_journal->action,
			    AGS_EDITOR_HISTORY_ACTION_NOTE_ADD,
			    strlen(AGS_EDITOR_HISTORY_ACTION_NOTE_ADD))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_NOTE_RESIZE,
				  strlen(AGS_EDITOR_HISTORY_ACTION_NOTE_RESIZE))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_NOTE_REMOVE,
				  strlen(AGS_EDITOR_HISTORY_ACTION_NOTE_REMOVE))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_NOTE_COPY,
				  strlen(AGS_EDITOR_HISTORY_ACTION_NOTE_COPY))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_NOTE_CUT,
				  strlen(AGS_EDITOR_HISTORY_ACTION_NOTE_CUT))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_NOTE_PASTE,
				  strlen(AGS_EDITOR_HISTORY_ACTION_NOTE_PASTE))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_NOTE_INVERT,
				  strlen(AGS_EDITOR_HISTORY_ACTION_NOTE_INVERT))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_NOTE_SELECT,
				  strlen(AGS_EDITOR_HISTORY_ACTION_NOTE_SELECT))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_POPOVER_NOTE_POSITION,
				  strlen(AGS_EDITOR_HISTORY_ACTION_POPOVER_NOTE_POSITION))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_POPOVER_NOTE_MOVE,
				  strlen(AGS_EDITOR_HISTORY_ACTION_POPOVER_NOTE_MOVE))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_POPOVER_NOTE_CROP,
				  strlen(AGS_EDITOR_HISTORY_ACTION_POPOVER_NOTE_CROP))){
      //TODO:JK: implement me
    }
  }else if(!g_ascii_strncasecmp(editor_journal->scope,
				AGS_EDITOR_HISTORY_SCOPE_AUTOMATION,
				strlen(AGS_EDITOR_HISTORY_SCOPE_AUTOMATION))){
    if(!g_ascii_strncasecmp(editor_journal->action,
			    AGS_EDITOR_HISTORY_ACTION_ACCELERATION_ADD,
			    strlen(AGS_EDITOR_HISTORY_ACTION_ACCELERATION_ADD))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_ACCELERATION_RESIZE,
				  strlen(AGS_EDITOR_HISTORY_ACTION_ACCELERATION_RESIZE))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_ACCELERATION_REMOVE,
				  strlen(AGS_EDITOR_HISTORY_ACTION_ACCELERATION_REMOVE))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_ACCELERATION_COPY,
				  strlen(AGS_EDITOR_HISTORY_ACTION_ACCELERATION_COPY))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_ACCELERATION_CUT,
				  strlen(AGS_EDITOR_HISTORY_ACTION_ACCELERATION_CUT))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_ACCELERATION_PASTE,
				  strlen(AGS_EDITOR_HISTORY_ACTION_ACCELERATION_PASTE))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_ACCELERATION_SELECT,
				  strlen(AGS_EDITOR_HISTORY_ACTION_ACCELERATION_SELECT))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_POPOVER_ACCELERATION_POSITION,
				  strlen(AGS_EDITOR_HISTORY_ACTION_POPOVER_ACCELERATION_POSITION))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_POPOVER_ACCELERATION_RAMP,
				  strlen(AGS_EDITOR_HISTORY_ACTION_POPOVER_ACCELERATION_RAMP))){
      //TODO:JK: implement me
    }
  }else if(!g_ascii_strncasecmp(editor_journal->scope,
				AGS_EDITOR_HISTORY_SCOPE_WAVE,
				strlen(AGS_EDITOR_HISTORY_SCOPE_WAVE))){
    if(!g_ascii_strncasecmp(editor_journal->action,
			    AGS_EDITOR_HISTORY_ACTION_BUFFER_ADD,
			    strlen(AGS_EDITOR_HISTORY_ACTION_BUFFER_ADD))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_BUFFER_REMOVE,
				  strlen(AGS_EDITOR_HISTORY_ACTION_BUFFER_REMOVE))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_BUFFER_COPY,
				  strlen(AGS_EDITOR_HISTORY_ACTION_BUFFER_COPY))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_BUFFER_CUT,
				  strlen(AGS_EDITOR_HISTORY_ACTION_BUFFER_CUT))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_BUFFER_PASTE,
				  strlen(AGS_EDITOR_HISTORY_ACTION_BUFFER_PASTE))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_BUFFER_SELECT,
				  strlen(AGS_EDITOR_HISTORY_ACTION_BUFFER_SELECT))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_POPOVER_BUFFER_POSITION,
				  strlen(AGS_EDITOR_HISTORY_ACTION_POPOVER_BUFFER_POSITION))){
      //TODO:JK: implement me
    }else if(!g_ascii_strncasecmp(editor_journal->action,
				  AGS_EDITOR_HISTORY_ACTION_POPOVER_BUFFER_TIME_STRETCH,
				  strlen(AGS_EDITOR_HISTORY_ACTION_POPOVER_BUFFER_TIME_STRETCH))){
      //TODO:JK: implement me
    }
  }

  /* reset position */
  editor_history->edit_position = edit_position;
}

/**
 * ags_editor_history_release_unused:
 * @editor_history: the #AgsEditorHistory
 *
 * Release unused history.
 *
 * Since: 7.2.0 
 */
void
ags_editor_history_release_unused(AgsEditorHistory *editor_history)
{
  //TODO:JK: implement me
}

/**
 * ags_editor_history_export_to_path:
 * @editor_history: the #AgsEditorHistory
 * @location: the location
 *
 * Export to path.
 *
 * Since: 7.2.0 
 */
void
ags_editor_history_export_to_path(AgsEditorHistory *editor_history,
				  gchar *location)
{
  xmlDoc *doc;
  xmlNode *root_node;
  xmlNode *node;
  xmlNode *child;

  GList *data_entry;
  GList *journal_entry;
  
  gchar *filename;

  GError *error;
  
  static const gchar *history_filename = "ags_history.xml";

  filename = g_strdup_printf("%s/%s",
			     location,
			     history_filename);

  doc = xmlNewDoc("1.0");

  root_node = xmlNewNode(NULL, "ags-history");

  xmlDocSetRootElement(doc,
		       root_node);

  /* extern data */
  node = xmlNewNode(NULL, "ags-data-entry-list");
  xmlAddChild(root_node,
	      node);
  
  data_entry = editor_history->data_entry;

  while(data_entry != NULL){
    gchar *extern_data_filename;

    child = xmlNewNode(NULL, "ags-data-entry");

    xmlAddChild(node,
		child);

    xmlNewProp(child,
	       "data-uuid",
	       AGS_EXTERN_DATA(data_entry->data)->data_uuid);

    extern_data_filename = g_strdup_printf("%s/%s",
					   location,
					   AGS_EXTERN_DATA(data_entry->data)->data_uuid);

    xmlNewProp(child,
	       "fileref",
	       AGS_EXTERN_DATA(data_entry->data)->data_uuid);

    error = NULL;
    g_file_set_contents(extern_data_filename,
			AGS_EXTERN_DATA(data_entry->data)->data,
			AGS_EXTERN_DATA(data_entry->data)->data_length,
			&error);

    g_free(extern_data_filename);
    
    data_entry = data_entry->next;
  }
  
  /* editor journal */
  node = xmlNewNode(NULL, "ags-journal-entry-list");
  xmlAddChild(root_node,
	      node);
  
  journal_entry = editor_history->journal_entry;

  while(journal_entry != NULL){
    child = xmlNewNode(NULL, "ags-journal-entry");

    xmlAddChild(node,
		child);

    xmlNewProp(child,
	       "scope",
	       AGS_EDITOR_JOURNAL(journal_entry->data)->scope);

    xmlNewProp(child,
	       "journal-type",
	       AGS_EDITOR_JOURNAL(journal_entry->data)->journal_type);

    xmlNewProp(child,
	       "action",
	       AGS_EDITOR_JOURNAL(journal_entry->data)->action);

    xmlNewProp(child,
	       "detail",
	       AGS_EDITOR_JOURNAL(journal_entry->data)->detail);

    xmlNewProp(child,
	       "data-access-type",
	       AGS_EDITOR_JOURNAL(journal_entry->data)->data_access_type);

    xmlNewProp(child,
	       "content-type",
	       AGS_EDITOR_JOURNAL(journal_entry->data)->content_type);

    if(!g_ascii_strncasecmp(AGS_EDITOR_JOURNAL(journal_entry->data)->data_access_type,
			    AGS_EDITOR_HISTORY_DATA_ACCESS_RAW_AUDIO_BASE64_SERIALIZER,
			    strlen(AGS_EDITOR_HISTORY_DATA_ACCESS_RAW_AUDIO_BASE64_SERIALIZER))){
      xmlNewProp(child,
		 "extern-orig-data",
		 AGS_EDITOR_JOURNAL(journal_entry->data)->extern_orig_data->data_uuid);

      xmlNewProp(child,
		 "extern-new-data",
		 AGS_EDITOR_JOURNAL(journal_entry->data)->extern_new_data->data_uuid);
    }else if(!g_ascii_strncasecmp(AGS_EDITOR_JOURNAL(journal_entry->data)->data_access_type,
				  AGS_EDITOR_HISTORY_DATA_ACCESS_XML_SERIALIZER,
				  strlen(AGS_EDITOR_HISTORY_DATA_ACCESS_XML_SERIALIZER))){
      xmlNode *orig_data;
      xmlNode *new_data;
      xmlNode *cdata;

      /* orig data */
      orig_data = xmlNewNode(NULL, "ags-orig-data");

      xmlAddChild(child,
		  orig_data);
      
      cdata = xmlNewCDataBlock(doc,
			       AGS_EDITOR_JOURNAL(journal_entry->data)->orig_data,
			       AGS_EDITOR_JOURNAL(journal_entry->data)->orig_data_length);

      xmlAddChild(node,
		  cdata);

      /* new data */
      new_data = xmlNewNode(NULL, "ags-new-data");

      xmlAddChild(child,
		  new_data);
      
      cdata = xmlNewCDataBlock(doc,
			       AGS_EDITOR_JOURNAL(journal_entry->data)->new_data,
			       AGS_EDITOR_JOURNAL(journal_entry->data)->new_data_length);

      xmlAddChild(node,
		  cdata);
    }else{
      g_warning("unsupported data access");
    }
    
    journal_entry = journal_entry->next;
  }

  g_free(filename);
}

/**
 * ags_editor_history_import_from_path:
 * @editor_history: the #AgsEditorHistory
 * @location: the location
 *
 * Import from path.
 *
 * Since: 7.2.0 
 */
void
ags_editor_history_import_from_path(AgsEditorHistory *editor_history,
				    gchar *location)
{
  xmlDoc *doc;
  xmlNode *root_node;
  xmlNode *node;
  xmlNode *child;
  AgsExternData *current_data_entry;
  AgsEditorJournal *current_journal_entry;
  
  GList *data_entry;
  GList *journal_entry;

  xmlChar *str;
  gchar *filename;
  gchar *extern_audio_data_filename;

  GError *error;
  
  static const gchar *history_filename = "ags_history.xml";

  filename = g_strdup_printf("%s/%s",
			     location,
			     history_filename);
  
  doc = xmlReadFile(filename, NULL, 0);

  root_node = xmlDocGetRootElement(doc);

  if((!g_ascii_strncasecmp(root_node->name, "ags-history", 12)) == FALSE){
    return;
  }

  /* read extern data */
  node = root_node->children;

  while(node != NULL){
    if(node->type == XML_ELEMENT_NODE){
      if(!g_ascii_strncasecmp(node->name, "ags-data-entry-list", 19)){
	child = node->children;
	
	while(child != NULL){
	  if(child->type == XML_ELEMENT_NODE){
	    if(!g_ascii_strncasecmp(child->name, "ags-data-entry", 14)){
	      xmlNode *data_node;
	      
	      gchar *extern_audio_data_filename;
	      
	      current_data_entry = ags_extern_data_alloc();

	      editor_history->data_entry = g_list_append(editor_history->data_entry,
							 current_data_entry);

	      str = xmlGetProp(child,
			       "data-uuid");

	      current_data_entry->data_uuid = str;

	      str = xmlGetProp(child,
			       "fileref");

	      extern_audio_data_filename = g_strdup_printf("%s/%s",
							   location,
							   str);

	      error = NULL;
	      g_file_get_contents(extern_audio_data_filename,
				  &(current_data_entry->data),
				  &(current_data_entry->data_length),
				  &error);
	    }
	  }
        
	  child = child->next;
	}
      }
    }
    
    node = node->next;
  }

  /* read editor journal */
  node = root_node->children;

  while(node != NULL){
    if(node->type == XML_ELEMENT_NODE){
      if(!g_ascii_strncasecmp(node->name, "ags-journal-entry-list", 22)){
	child = node->children;
	
	while(child != NULL){
	  if(child->type == XML_ELEMENT_NODE){
	    if(!g_ascii_strncasecmp(child->name, "ags-journal-entry", 17)){
	      xmlNode *data_node;
	      GList *tmp;
	      
	      current_journal_entry = ags_editor_journal_alloc();

	      editor_history->journal_entry = g_list_append(editor_history->journal_entry,
							    current_journal_entry);
	      
	      str = xmlGetProp(child,
			       "scope");

	      current_journal_entry->scope = str;
  
	      str = xmlGetProp(child,
			       "journal-type");

	      current_journal_entry->journal_type = str;
  
	      str = xmlGetProp(child,
			       "action");

	      current_journal_entry->action = str;
  
	      str = xmlGetProp(child,
			       "detail");

	      current_journal_entry->detail = str;
  
	      str = xmlGetProp(child,
			       "data-access-type");

	      current_journal_entry->data_access_type = str;
  
	      str = xmlGetProp(child,
			       "content-type");

	      current_journal_entry->content_type = str;

	      str = xmlGetProp(child,
			       "extern-orig-data");

	      tmp = g_list_find_custom(editor_history->data_entry,
				       str,
				       (GCompareFunc) ags_extern_data_cmp);

	      if(tmp != NULL){
		current_journal_entry->extern_orig_data = tmp->data;
	      }
	      
	      data_node = child->children;
	      
	      while(data_node != NULL){
		if(data_node->type == XML_ELEMENT_NODE){
		  if(!g_ascii_strncasecmp(data_node->name, "ags-orig-data", 13)){
		    xmlChar *content;
		      
		    content = xmlNodeGetContent(node);

		    current_journal_entry->orig_data = content;
		    current_journal_entry->orig_data_length = strlen(content);
		  }else if(!g_ascii_strncasecmp(data_node->name, "ags-new-data", 12)){
		    xmlChar *content;
		      
		    content = xmlNodeGetContent(node);

		    current_journal_entry->new_data = content;
		    current_journal_entry->new_data_length = strlen(content);
		  }
		}
    
		data_node = data_node->next;
	      }
	    }
	  }
    
	  child = child->next;
	}
      }
    }
    
    node = node->next;
  }
}

/**
 * ags_editor_history_get_instance:
 *
 * Get instance.
 *
 * Returns: (transfer none): the #AgsEditorHistory
 *
 * Since: 7.2.0
 */
AgsEditorHistory*
ags_editor_history_get_instance()
{
  if(ags_editor_history == NULL){
    ags_editor_history = ags_editor_history_new();
  }

  return(ags_editor_history);
}

/**
 * ags_editor_history_new:
 *
 * Create a new instance of #AgsEditorHistory
 *
 * Returns: the new #AgsEditorHistory
 *
 * Since: 7.2.0
 */
AgsEditorHistory*
ags_editor_history_new()
{
  AgsEditorHistory *editor_history;

  editor_history = (AgsEditorHistory *) g_object_new(AGS_TYPE_EDITOR_HISTORY,
						     NULL);

  return(editor_history);
}

