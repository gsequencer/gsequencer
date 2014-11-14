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

#include <ags/X/editor/ags_toolbar_callbacks.h>

#include <ags/X/ags_editor.h>
#include <ags/X/editor/ags_note_edit.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <math.h>

void
ags_toolbar_show_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
  ags_toolbar_show(widget);
}

void
ags_toolbar_position_callback(GtkToggleButton *toggle_button, AgsToolbar *toolbar)
{
  AgsEditor *editor;

  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(toolbar), AGS_TYPE_EDITOR);

  if(toggle_button == toolbar->selected_edit_mode){
    GtkWidget *child;
    GList *list;
    GdkRectangle *rectangle;
    gint width, height;
    
    list = gtk_container_get_children(editor->table);

    /* 3rd - 1 */
    child = g_list_nth(editor->table,
		       2)->data;

    /* refresh editor */
    gtk_widget_queue_draw(child);

    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_button_get_active(toggle_button)){
    GtkToggleButton *old_selected_edit_mode;
    cairo_t *cr;
    
    old_selected_edit_mode = toolbar->selected_edit_mode;
    toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);

    /* refresh note_edit */    
    if((AGS_EDITOR_TOOL_NOTE_EDIT & (editor->flags)) != 0){
      cr = gdk_cairo_create(GTK_WIDGET(editor->edit.note_edit->drawing_area)->window);
      ags_note_edit_draw_position(editor->edit.note_edit, cr);
    }else if((AGS_EDITOR_TOOL_PATTERN_EDIT & (editor->flags)) != 0){
      cr = gdk_cairo_create(GTK_WIDGET(editor->edit.pattern_edit->drawing_area)->window);
      ags_pattern_edit_draw_position(editor->edit.pattern_edit, cr);
    }else if((AGS_EDITOR_TOOL_AUTOMATION_EDIT & (editor->flags)) != 0){
      //TODO:JK: implement me
    }
  }
}

void
ags_toolbar_edit_callback(GtkToggleButton *toggle_button, AgsToolbar *toolbar)
{
  if(toggle_button == toolbar->selected_edit_mode){
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_button_get_active(toggle_button)){
    GtkToggleButton *old_selected_edit_mode;
    
    old_selected_edit_mode = toolbar->selected_edit_mode;
    toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_toolbar_clear_callback(GtkToggleButton *toggle_button, AgsToolbar *toolbar)
{
  if(toggle_button == toolbar->selected_edit_mode){
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_button_get_active(toggle_button)){
    GtkToggleButton *old_selected_edit_mode;
    
    old_selected_edit_mode = toolbar->selected_edit_mode;
    toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_toolbar_select_callback(GtkToggleButton *toggle_button, AgsToolbar *toolbar)
{
  if(toggle_button == toolbar->selected_edit_mode){
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_button_get_active(toggle_button)){
    GtkToggleButton *old_selected_edit_mode;
    
    old_selected_edit_mode = toolbar->selected_edit_mode;
    toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);
  }
}

void
ags_toolbar_copy_or_cut_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
  AgsMachine *machine;
  AgsEditor *editor;
  AgsNotation *notation;
  GList *list_notation;
  xmlDocPtr clipboard;
  xmlNodePtr audio_node, notation_node;
  xmlChar *buffer;
  int size;

  /* add notation to root node */
  editor = AGS_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(toolbar), AGS_TYPE_EDITOR));

  if((machine = editor->selected_machine) != NULL){
    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);

    /* create root node */
    audio_node = xmlNewNode(NULL, BAD_CAST "audio\0");
    xmlDocSetRootElement(clipboard, audio_node);

    /* create notation nodes */
    list_notation = machine->audio->notation;

    if(gtk_option_menu_get_history(toolbar->mode) == 0){
      if(editor->notebook->tabs != NULL){
	list_notation = g_list_nth(list_notation,
				   ags_notebook_next_active_tab(editor->notebook,
								0));

	if(widget == (GtkWidget *) toolbar->copy)
	  notation_node = ags_notation_copy_selection(AGS_NOTATION(list_notation->data));
	else
	  notation_node = ags_notation_cut_selection(AGS_NOTATION(list_notation->data));

	xmlAddChild(audio_node, notation_node);
      }
    }else{
      while(list_notation != NULL ){
	if(widget == (GtkWidget *) toolbar->copy)
	  notation_node = ags_notation_copy_selection(AGS_NOTATION(list_notation->data));
	else
	  notation_node = ags_notation_cut_selection(AGS_NOTATION(list_notation->data));

	xmlAddChild(audio_node, notation_node);

	list_notation = list_notation->next;
      }
    }

    /* write to clipboard */
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8\0", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   buffer, size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    xmlFreeDoc(clipboard);
  }
}

void
ags_toolbar_paste_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
  AgsMachine *machine;
  AgsEditor *editor;
  AgsNotation *notation;
  xmlDocPtr clipboard;
  xmlNodePtr audio_node, notation_node;
  gchar *buffer;
  guint position_x, position_y;
  gboolean paste_from_position;
  void ags_toolbar_paste_callback_read_notation(){
    xmlXPathContextPtr xpathCtxt;
    xmlXPathObjectPtr xpathObj;
    xmlNodeSetPtr nodes;
    GList *notation_list;

    xpathCtxt = xmlXPathNewContext(clipboard);

    xpathObj = xmlXPathEvalExpression("/audio/notation\0", xpathCtxt);

    if(xpathObj != NULL){
      int i, size;

      nodes = xpathObj->nodesetval;
      size = (nodes != NULL) ? nodes->nodeNr: 0;

      notation_list = machine->audio->notation;

      for(i = 0; i < size && notation_list != NULL; i++){
	if(paste_from_position){
	  ags_notation_insert_from_clipboard(AGS_NOTATION(notation_list->data),
					     nodes->nodeTab[i],
					     TRUE, position_x,
					     TRUE, position_y);
	}else{
	  ags_notation_insert_from_clipboard(AGS_NOTATION(notation_list->data),
					     nodes->nodeTab[i],
					     FALSE, 0,
					     FALSE, 0);
	}

	notation_list = notation_list->next;
      }

      xmlXPathFreeObject(xpathObj);
    }

    if(xpathCtxt != NULL)
      xmlXPathFreeContext(xpathCtxt);
  }

  /* retrieve AgsEditor */
  editor = AGS_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(toolbar), AGS_TYPE_EDITOR));

  if((machine = editor->selected_machine) != NULL){
    /* get clipboard */
    buffer = gtk_clipboard_wait_for_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
    
    if(buffer == NULL)
      return;

    /* get position */
    if(toolbar->selected_edit_mode == toolbar->position){
      paste_from_position = TRUE;

      if((AGS_EDITOR_TOOL_NOTE_EDIT & (editor->flags)) != 0){
	position_x = editor->edit.note_edit->selected_x;
	position_y = editor->edit.note_edit->selected_y;
      }else if((AGS_EDITOR_TOOL_PATTERN_EDIT & (editor->flags)) != 0){
	position_x = editor->edit.pattern_edit->selected_x;
	position_y = editor->edit.pattern_edit->selected_y;
      }else if((AGS_EDITOR_TOOL_AUTOMATION_EDIT & (editor->flags)) != 0){
	//TODO:JK: implement me
	position_x = 0;
	position_y = 0;
	//	position_x = editor->edit.automation_edit->selected_x;
	//	position_y = editor->edit.automation_edit->selected_y;
      }

      printf("pasting at position: [%u,%u]\n\0", position_x, position_y);
    }else{
      paste_from_position = FALSE;
    }

    /* get xml tree */
    clipboard = xmlReadMemory(buffer, strlen(buffer),
			      NULL, "UTF-8\0",
			      0);
    audio_node = xmlDocGetRootElement(clipboard);
    
    /* iterate xml tree */
    while(audio_node != NULL){
      if(audio_node->type == XML_ELEMENT_NODE && !xmlStrncmp("audio\0", audio_node->name, 6)){
	notation_node = audio_node->children;
	
	ags_toolbar_paste_callback_read_notation();

	break;
      }
      
      audio_node = audio_node->next;
    }

    xmlFreeDoc(clipboard); 
  }
}

void
ags_toolbar_zoom_callback(GtkComboBox *combo_box, AgsToolbar *toolbar)
{
  AgsEditor *editor;
  GtkWidget *widget;
  GtkAdjustment *adjustment;
  double zoom, zoom_old;
  guint history;

  editor = (AgsEditor *) gtk_widget_get_ancestor((GtkWidget *) toolbar, AGS_TYPE_EDITOR);

  history = gtk_combo_box_get_active(combo_box);

  zoom = exp2((double) history - 4.0);
  zoom_old = exp2((double) toolbar->zoom_history - 4.0);

  toolbar->zoom_history = history;

  if((AGS_EDITOR_TOOL_NOTE_EDIT & (editor->flags)) != 0){
    editor->edit.note_edit->flags |= AGS_NOTE_EDIT_RESETING_HORIZONTALLY;
    ags_note_edit_reset_horizontally(editor->edit.note_edit, AGS_NOTE_EDIT_RESET_HSCROLLBAR |
				     AGS_NOTE_EDIT_RESET_WIDTH);
    editor->edit.note_edit->flags &= (~AGS_NOTE_EDIT_RESETING_HORIZONTALLY);
  }else if((AGS_EDITOR_TOOL_PATTERN_EDIT & (editor->flags)) != 0){
    editor->edit.pattern_edit->flags |= AGS_PATTERN_EDIT_RESETING_HORIZONTALLY;
    ags_note_edit_reset_horizontally(editor->edit.pattern_edit, AGS_PATTERN_EDIT_RESET_HSCROLLBAR |
				     AGS_PATTERN_EDIT_RESET_WIDTH);
    editor->edit.note_edit->flags &= (~AGS_NOTE_EDIT_RESETING_HORIZONTALLY);
  }else if((AGS_EDITOR_TOOL_AUTOMATION_EDIT & (editor->flags)) != 0){
    //TODO:JK: implement me
  }
}

void
ags_toolbar_mode_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
  AgsEditor *editor;
  AgsNotebook *notebook;
  GList *list;
  gint history;

  /* retrieve some variables */
  editor = (AgsEditor *) gtk_widget_get_ancestor(GTK_WIDGET(toolbar),
						 AGS_TYPE_EDITOR);

  notebook = editor->notebook;

  /* toggle AgsNotebook */
  history = gtk_option_menu_get_history(toolbar->mode);

  switch(history){
  case 0:
    {
      list = g_list_reverse(notebook->tabs);

      if(list != NULL){
	gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(list->data)->toggle,
				     TRUE);

	list = list->next;
      }

      while(list != NULL){
	gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(list->data)->toggle,
				     FALSE);	

	list = list->next;
      }
    }
    break;
  case 1:
    {
      //NOTE: you're on your own
    }
    break;
  case 2:
    {
      list = g_list_reverse(notebook->tabs);

      while(list != NULL){
	gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(list->data)->toggle,
				     TRUE);

	list = list->next;
      }
    }
    break;
  default:
    g_message("unknown editor mode\0");
  }
}
