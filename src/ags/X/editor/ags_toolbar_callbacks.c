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

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <math.h>

gboolean
ags_toolbar_destroy_callback(GtkObject *object, AgsToolbar *toolbar)
{
  ags_toolbar_destroy(object);

  return(FALSE);
}

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
    GdkRectangle *rectangle;
    gint width, height;
    
    /* refresh editor */
    gtk_widget_get_size_request(GTK_WIDGET(editor->drawing_area),
				&width,
				&height);
    
    rectangle = g_new(GdkRectangle, 1);
    rectangle->x = 0;
    rectangle->y = 0;
    rectangle->width = width;
    rectangle->height = height;
    
    gdk_window_invalidate_rect(GTK_WIDGET(editor->drawing_area)->window,
			       rectangle,
			       TRUE);
    gdk_window_process_updates(GTK_WIDGET(editor->drawing_area)->window,
			       TRUE);
    
    g_free(rectangle);
    
    if(!gtk_toggle_button_get_active(toggle_button)){
      gtk_toggle_button_set_active(toggle_button, TRUE);
    }
  }else if(gtk_toggle_button_get_active(toggle_button)){
    GtkToggleButton *old_selected_edit_mode;
    cairo_t *cr;
    
    old_selected_edit_mode = toolbar->selected_edit_mode;
    toolbar->selected_edit_mode = toggle_button;
    gtk_toggle_button_set_active(old_selected_edit_mode, FALSE);

    /* refresh editor */
    cr = gdk_cairo_create(GTK_WIDGET(editor->drawing_area)->window);
    
    ags_editor_draw_position(editor, cr);
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

  if(editor->selected != NULL &&
     (machine = AGS_MACHINE(g_object_get_data((GObject *) editor->selected, (char *) g_type_name(AGS_TYPE_MACHINE)))) != NULL){
    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);

    /* create root node */
    audio_node = xmlNewNode(NULL, BAD_CAST "audio");
    xmlDocSetRootElement(clipboard, audio_node);

    /* create notation nodes */
    list_notation = machine->audio->notation;

    if(gtk_option_menu_get_history(toolbar->mode) == 0){
      if(gtk_notebook_get_n_pages((GtkNotebook *) editor->notebook) > 0){
	list_notation = g_list_nth(list_notation, gtk_notebook_get_current_page((GtkNotebook *) editor->notebook));

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
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8", TRUE);
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

    xpathObj = xmlXPathEvalExpression("/audio/notation", xpathCtxt);

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

  if(editor->selected != NULL &&
     (machine = AGS_MACHINE(g_object_get_data((GObject *) editor->selected, (char *) g_type_name(AGS_TYPE_MACHINE)))) != NULL){
    /* get clipboard */
    buffer = gtk_clipboard_wait_for_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
    
    if(buffer == NULL)
      return;

    /* get position */
    if(toolbar->selected_edit_mode == toolbar->position){
      paste_from_position = TRUE;

      position_x = editor->selected_x;
      position_y = editor->selected_y;

      printf("pasting at position: [%u,%u]\n", position_x, position_y);
    }else{
      paste_from_position = FALSE;
    }

    /* get xml tree */
    clipboard = xmlReadMemory(buffer, strlen(buffer),
			      NULL, "UTF-8",
			      0);
    audio_node = xmlDocGetRootElement(clipboard);
    
    /* iterate xml tree */
    while(audio_node != NULL){
      if(audio_node->type == XML_ELEMENT_NODE && !xmlStrncmp("audio", audio_node->name, 6)){
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
ags_toolbar_zoom_callback(GtkOptionMenu *option, AgsToolbar *toolbar)
{
  AgsEditor *editor;
  GtkWidget *widget;
  GtkAdjustment *adjustment;
  double zoom, zoom_old;
  guint history;

  editor = (AgsEditor *) gtk_widget_get_ancestor((GtkWidget *) toolbar, AGS_TYPE_EDITOR);

  editor->flags |= AGS_EDITOR_RESETING_HORIZONTALLY;
  ags_editor_reset_horizontally(editor, AGS_EDITOR_RESET_HSCROLLBAR |
				AGS_EDITOR_RESET_WIDTH);
  editor->flags &= (~AGS_EDITOR_RESETING_HORIZONTALLY);
}

void
ags_toolbar_tact_callback(GtkOptionMenu *option, AgsToolbar *toolbar)
{
  AgsEditor *editor;
  GtkWidget *widget;
  GtkAdjustment *adjustment;
  double tact, tact_old;
  guint history;

  editor = (AgsEditor *) gtk_widget_get_ancestor((GtkWidget *) toolbar, AGS_TYPE_EDITOR);
  widget = (GtkWidget *) editor->drawing_area;

  history = gtk_option_menu_get_history(option);

  tact = exp2((double) history - 4.0);
  tact_old = exp2((double) toolbar->tact_history - 4.0);

  toolbar->tact_history = history;

  editor->flags |= AGS_EDITOR_RESETING_HORIZONTALLY;
  ags_editor_reset_horizontally(editor, AGS_EDITOR_RESET_HSCROLLBAR |
				AGS_EDITOR_RESET_WIDTH);
  editor->flags &= (~AGS_EDITOR_RESETING_HORIZONTALLY);
}

void
ags_toolbar_mode_default_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
}

void
ags_toolbar_mode_group_channels_callback(GtkWidget *widget, AgsToolbar *toolbar)
{
}

