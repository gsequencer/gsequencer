/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/ags_editor.h>
#include <ags/X/ags_editor_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_notation.h>

#include <ags/X/ags_window.h>

#include <ags/X/editor/ags_note_edit.h>
#include <ags/X/editor/ags_note_edit_callbacks.h>
#include <ags/X/editor/ags_pattern_edit.h>
#include <ags/X/editor/ags_pattern_edit_callbacks.h>

#include <stdlib.h>
#include <math.h>
#include <cairo.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>

void ags_editor_class_init(AgsEditorClass *editor);
void ags_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_editor_init(AgsEditor *editor);
void ags_editor_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_editor_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_editor_finalize(GObject *gobject);
void ags_editor_connect(AgsConnectable *connectable);
void ags_editor_disconnect(AgsConnectable *connectable);
void ags_editor_destroy(GtkObject *object);
void ags_editor_show(GtkWidget *widget);

void ags_editor_real_audio_channels_changed(AgsEditor *editor, guint audio_channels);
void ags_editor_real_pads_changed(AgsEditor *editor, guint pads);
void ags_editor_real_machine_changed(AgsEditor *editor, AgsMachine *machine);

/**
 * SECTION:ags_editor
 * @short_description: A composite widget to edit notes
 * @title: AgsEditor
 * @section_id:
 * @include: ags/X/ags_editor.h
 *
 * #AgsEditor is a composite widget to edit notes. You may select machines
 * or change editor tool to do notation.
 */

enum{
  MACHINE_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_DEVOUT,
};

static gpointer ags_editor_parent_class = NULL;
static guint editor_signals[LAST_SIGNAL];

GtkStyle *editor_style;

GType
ags_editor_get_type(void)
{
  static GType ags_type_editor = 0;

  if(!ags_type_editor){
    static const GTypeInfo ags_editor_info = {
      sizeof (AgsEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_editor = g_type_register_static(GTK_TYPE_VBOX,
					     "AgsEditor\0", &ags_editor_info,
					     0);
    
    g_type_add_interface_static(ags_type_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_editor);
}

void
ags_editor_class_init(AgsEditorClass *editor)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_editor_parent_class = g_type_class_peek_parent(editor);

  /* GObjectClass */
  gobject = (GObjectClass *) editor;

  gobject->set_property = ags_editor_set_property;
  gobject->get_property = ags_editor_get_property;

  gobject->finalize = ags_editor_finalize;

  /* properties */
  /**
   * AgsEditor:devout:
   *
   * The assigned #AgsDevout acting as default sink.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("devout\0",
				   "assigned devout\0",
				   "The devout it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  /* AgsEditorClass */
  editor->machine_changed = ags_editor_real_machine_changed;

  /* signals */
  /**
   * AgsEditor::machine-changed:
   * @editor: the object to change machine.
   * @machine: the #AgsMachine to set
   *
   * The ::machine-changed signal notifies about changed machine.
   */
  editor_signals[MACHINE_CHANGED] =
    g_signal_new("machine-changed\0",
                 G_TYPE_FROM_CLASS (editor),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsEditorClass, machine_changed),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_editor_connect;
  connectable->disconnect = ags_editor_disconnect;
}

void
ags_editor_init(AgsEditor *editor)
{
  GtkHPaned *paned;
  GtkScrolledWindow *scrolled_window;

  g_signal_connect_after((GObject *) editor, "parent-set\0",
			 G_CALLBACK(ags_editor_parent_set_callback), editor);

  editor->flags = 0;

  editor->version = AGS_EDITOR_DEFAULT_VERSION;
  editor->build_id = AGS_EDITOR_DEFAULT_BUILD_ID;

  editor->devout = NULL;

  editor->toolbar = ags_toolbar_new();
  gtk_box_pack_start((GtkBox *) editor,
		     (GtkWidget *) editor->toolbar,
		     FALSE, FALSE, 0);

  paned = (GtkHPaned *) gtk_hpaned_new();
  gtk_box_pack_start((GtkBox *) editor,
		     (GtkWidget *) paned,
		     TRUE, TRUE, 0);

  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_paned_pack1((GtkPaned *) paned, (GtkWidget *) scrolled_window, FALSE, TRUE);
  //  gtk_widget_set_size_request((GtkWidget *) scrolled_window, 180, -1);

  editor->machine_selector = g_object_new(AGS_TYPE_MACHINE_SELECTOR,
					  "homogeneous\0", FALSE,
					  "spacing\0", 0,
					  NULL);
  gtk_scrolled_window_add_with_viewport(scrolled_window, (GtkWidget *) editor->machine_selector);

  editor->selected_machine = NULL;

  editor->editor_child = NULL;
  editor->table = (GtkTable *) gtk_table_new(4, 3, FALSE);
  gtk_paned_pack2((GtkPaned *) paned, (GtkWidget *) editor->table, TRUE, FALSE);
  
  editor->current_notebook = NULL;
  
  editor->current_meter = NULL;
  editor->current_edit_widget = NULL;

  //TODO:JK: remove me
  /*
  editor->current_meter = g_object_new(AGS_TYPE_METER,
			       NULL);
  gtk_table_attach(table, (GtkWidget *) editor->current_meter,
		   0, 1, 1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  editor->note_edit = g_object_new(AGS_TYPE_NOTE_EDIT,
				   "n-rows\0", 3,
				   "n-columns\0", 3,
				   "homogeneous", FALSE,
				   NULL);
  gtk_table_attach(table, (GtkWidget *) editor->note_edit,
		   1, 2, 1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);
  */
  
  editor->tact_counter = 0;
}

void
ags_editor_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsEditor *editor;

  editor = AGS_EDITOR(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      AgsDevout *devout;

      devout = (AgsDevout *) g_value_get_object(value);

      if(editor->devout == devout)
	return;

      if(devout != NULL)
	g_object_ref(devout);

      editor->devout = devout;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_editor_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsEditor *editor;

  editor = AGS_EDITOR(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    g_value_set_object(value, editor->devout);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_editor_finalize(GObject *gobject)
{
  //TODO:JK: implement me
}

void
ags_editor_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsEditor *editor;
  GtkHPaned *hpaned;

  editor = AGS_EDITOR(connectable);
  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(editor)));

  /*  */
  g_signal_connect(window->devout, "tic\0",
		   ags_editor_tic_callback, editor);

  
  g_signal_connect((GObject *) editor->machine_selector, "changed\0",
		   G_CALLBACK(ags_editor_machine_changed_callback), (gpointer) editor);

  /*  */
  ags_connectable_connect(AGS_CONNECTABLE(editor->toolbar));
  ags_connectable_connect(AGS_CONNECTABLE(editor->machine_selector));
}

void
ags_editor_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

AgsEditorChild*
ags_editor_child_alloc(AgsMachine *machine, AgsNotebook *notebook, AgsMeter *meter, GtkWidget *edit_widget)
{
  AgsEditorChild *editor_child;

  editor_child = (AgsEditorChild *) malloc(sizeof(AgsEditorChild));

  editor_child->machine = machine;
  editor_child->notebook = notebook;
  editor_child->meter = meter;
  editor_child->edit_widget = edit_widget;
  
  return(editor_child);
}

void
ags_editor_real_machine_changed(AgsEditor *editor, AgsMachine *machine)
{
  AgsMachine *machine_old;

  GList *list, *list_start;
  GList *child;
  
  guint pads;
  guint i;
  
  if(editor->selected_machine == machine){
    return;
  }

  machine_old = editor->selected_machine;
  editor->selected_machine = machine;
  
  child = editor->editor_child;

  while(child != NULL){
    if(AGS_EDITOR_CHILD(child->data) == machine){
      break;
    }
    
    child = child->next;
  }

  if(editor->current_notebook != NULL){
    gtk_container_remove(editor->table,
			 editor->current_notebook);
    gtk_container_remove(editor->table,
			 editor->current_meter);
    gtk_container_remove(editor->table,
			 editor->current_edit_widget);
  }
  
  editor->current_notebook = NULL;
  editor->current_meter = NULL;
  editor->current_edit_widget = NULL;

  /* instantiate note edit or pattern edit */
  list_start = 
    list = gtk_container_get_children(editor->machine_selector->popup);
  list = g_list_nth(list,
		    3);

  if(machine == NULL){
    gtk_check_menu_item_set_active(list->data,
				   FALSE);
    g_list_free(list_start);
    
    return;
  }

  gtk_check_menu_item_set_active(list->data,
				 ((AGS_AUDIO_REVERSE_MAPPING & (machine->audio->flags)) ? TRUE: FALSE));
  g_list_free(list_start);
  
  if(child == NULL){
    AgsEditorChild *editor_child;

    editor_child = ags_editor_child_alloc(machine, NULL, NULL, NULL);
    editor->editor_child = g_list_prepend(editor->editor_child,
					  editor_child);
    
    editor_child->notebook = 
      editor->current_notebook = g_object_new(AGS_TYPE_NOTEBOOK,
					      "homogeneous\0", FALSE,
					      "spacing\0", 0,
					      NULL);
    g_object_ref(editor_child->notebook);
    gtk_table_attach(editor->table, (GtkWidget *) editor_child->notebook,
		     0, 3, 0, 1,
		     GTK_FILL|GTK_EXPAND, GTK_FILL,
		     0, 0);

    for(i = 0; i < machine->audio->audio_channels; i++){
      ags_notebook_insert_tab(editor_child->notebook,
			      i);
      AGS_NOTEBOOK_TAB(editor_child->notebook->tabs->data)->notation = g_list_nth(machine->audio->notation,
									   i);
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(editor_child->notebook->tabs->data)->toggle,
				   TRUE);
    }

    gtk_widget_show_all(editor_child->notebook);
    
    pads = machine->audio->input_pads;

    editor_child->meter = 
      editor->current_meter = ags_meter_new();
    g_object_ref(editor_child->meter);
    gtk_table_attach(editor->table, (GtkWidget *) editor_child->meter,
		     0, 1, 1, 2,
		     GTK_FILL, GTK_FILL,
		     0, 0);
    ags_connectable_connect(AGS_CONNECTABLE(editor_child->meter));
    gtk_widget_show_all(editor_child->meter);

    if((AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
      editor_child->edit_widget = 
	editor->current_edit_widget = ags_note_edit_new();
      g_object_ref(editor_child->edit_widget);
      gtk_table_attach(editor->table, (GtkWidget *) editor_child->edit_widget,
		       1, 2, 1, 2,
		       GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		       0, 0);

      ags_connectable_connect(AGS_CONNECTABLE(editor_child->edit_widget));
      gtk_widget_show_all(editor_child->edit_widget);

      ags_note_edit_set_map_height(editor_child->edit_widget,
				   pads * AGS_NOTE_EDIT(editor_child->edit_widget)->control_height);
    }else if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0){
      editor_child->edit_widget =
	editor->current_edit_widget = ags_pattern_edit_new();
      g_object_ref(editor_child->edit_widget);
      gtk_table_attach(editor->table, (GtkWidget *) editor_child->edit_widget,
		       1, 2, 1, 2,
		       GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		       0, 0);

      ags_connectable_connect(AGS_CONNECTABLE(editor_child->edit_widget));
      gtk_widget_show_all(editor_child->edit_widget);

      ags_pattern_edit_set_map_height(editor_child->edit_widget,
				      pads * AGS_PATTERN_EDIT(editor_child->edit_widget)->control_height);
    }
  }else{
    AgsEditorChild *editor_child;

    editor_child = child->data;
    
    gtk_table_attach(editor->table, (GtkWidget *) editor_child->notebook,
		     0, 3, 0, 1,
		     GTK_FILL|GTK_EXPAND, GTK_FILL,
		     0, 0);

    gtk_table_attach(editor->table, (GtkWidget *) editor_child->meter,
		     0, 1, 1, 2,
		     GTK_FILL, GTK_FILL,
		     0, 0);
    gtk_table_attach(editor->table, (GtkWidget *) editor_child->edit_widget,
		     1, 2, 1, 2,
		     GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		     0, 0);
  }
}


/**
 * ags_editor_machine_changed:
 * @editor: an #AgsEditor
 * @machine: the new #AgsMachine
 *
 * Is emitted as machine changed of editor.
 *
 * Since: 0.4
 */
void
ags_editor_machine_changed(AgsEditor *editor, AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_EDITOR(editor));

  g_object_ref((GObject *) editor);
  g_signal_emit((GObject *) editor,
		editor_signals[MACHINE_CHANGED], 0,
		machine);
  g_object_unref((GObject *) editor);
}

/**
 * ags_editor_select_all:
 * @editor: an #AgsEditor
 *
 * Is emitted as machine changed of editor.
 *
 * Since: 0.4.2
 */
void
ags_editor_select_all(AgsEditor *editor)
{
  AgsMachine *machine;
  
  cairo_t *cr;
    
  GList *list_notation;

  gint i;
  gint selected_channel;

  if(editor->selected_machine != NULL && editor->current_edit_widget != NULL){
    machine = editor->selected_machine;

    list_notation = machine->audio->notation;
    i = 0;

    while((selected_channel = ags_notebook_next_active_tab(editor->current_notebook,
							   i)) != -1){
      list_notation = g_list_nth(machine->audio->notation,
				 selected_channel);
      ags_notation_add_all_to_selection(AGS_NOTATION(list_notation->data));

      i++;
    }

    if(AGS_IS_NOTE_EDIT(editor->current_edit_widget)){
      cr = gdk_cairo_create(GTK_WIDGET(AGS_NOTE_EDIT(editor->current_edit_widget)->drawing_area)->window);
      cairo_push_group(cr);
      
      ags_note_edit_draw_segment(editor->current_edit_widget, cr);
      ags_note_edit_draw_notation(editor->current_edit_widget, cr);

      cairo_pop_group_to_source(cr);
      cairo_paint(cr);
    }else if(AGS_IS_PATTERN_EDIT(editor->current_edit_widget)){
      cr = gdk_cairo_create(GTK_WIDGET(AGS_PATTERN_EDIT(editor->current_edit_widget)->drawing_area)->window);
      cairo_push_group(cr);
      
      ags_pattern_edit_draw_segment(editor->current_edit_widget, cr);
      ags_pattern_edit_draw_notation(editor->current_edit_widget, cr);

      cairo_pop_group_to_source(cr);
      cairo_paint(cr);
    }
  }
}

/**
 * ags_editor_paste:
 * @editor: an #AgsEditor
 *
 * Is emitted as machine changed of editor.
 *
 * Since: 0.4.2
 */
void
ags_editor_paste(AgsEditor *editor)
{
  AgsMachine *machine;
  
  AgsNotation *notation;

  xmlDocPtr clipboard;
  xmlNodePtr audio_node, notation_node;
  cairo_t *cr;
  
  gchar *buffer;
  guint position_x, position_y;
  guint last_x;
  gboolean paste_from_position;
  
  void ags_editor_paste_read_notation(){
    xmlXPathContextPtr xpathCtxt;
    xmlXPathObjectPtr xpathObj;
    xmlNodeSetPtr nodes;
    GList *notation_list;

    xpathCtxt = xmlXPathNewContext(clipboard);
    xpathObj = xmlXPathEvalExpression("/audio/notation\0", xpathCtxt);

    if(xpathObj != NULL){
      int i, size;
      guint audio_channel;
      guint current_x;
      
      nodes = xpathObj->nodesetval;
      size = (nodes != NULL) ? nodes->nodeNr: 0;

      for(i = 0; i < size; i++){
	audio_channel = (guint) g_ascii_strtoull(xmlGetProp(nodes->nodeTab[i],
							    "audio-channel\0"),
						 NULL,
						 10);
	notation_list = g_list_nth(machine->audio->notation,
				   audio_channel);

	if(notation_list == NULL){
	  break;
	}
	
	if(paste_from_position){
	  xmlNode *child;

	  guint x_boundary;
	  
	  ags_notation_insert_from_clipboard(AGS_NOTATION(notation_list->data),
					     nodes->nodeTab[i],
					     TRUE, position_x,
					     TRUE, position_y);

	  child = nodes->nodeTab[i]->children;
	  current_x = 0;
	  
	  while(child != NULL){
	    if(child->type == XML_ELEMENT_NODE){
	      if(!xmlStrncmp(child->name,
			     "note\0",
			     5)){
		guint tmp;

		tmp = g_ascii_strtoull(xmlGetProp(child,
						  "x1\0"),
				       NULL,
				       10);

		if(tmp > current_x){
		  current_x = tmp;
		}
	      }
	    }

	    child = child->next;
	  }

	  x_boundary = g_ascii_strtoull(xmlGetProp(nodes->nodeTab[i],
						   "x_boundary\0"),
					NULL,
					10);

	  
	  if(position_x > x_boundary){
	    current_x += (position_x - x_boundary);
	  }else{
	    current_x -= (x_boundary - position_x);
	  }
	  
	  if(current_x > last_x){
	    last_x = current_x;
	  }	
	}else{
	  xmlNode *child;

	  ags_notation_insert_from_clipboard(AGS_NOTATION(notation_list->data),
					     nodes->nodeTab[i],
					     FALSE, 0,
					     FALSE, 0);

	  child = nodes->nodeTab[i]->children;
	  current_x = 0;
	  
	  while(child != NULL){
	    if(child->type == XML_ELEMENT_NODE){
	      if(!xmlStrncmp(child->name,
			     "note\0",
			     5)){
		guint tmp;

		tmp = g_ascii_strtoull(xmlGetProp(child,
						  "x1\0"),
				       NULL,
				       10);

		if(tmp > current_x){
		  current_x = tmp;
		}
	      }
	    }

	    child = child->next;
	  }

	  if(current_x > last_x){
	    last_x = current_x;
	  }
	}
      }

      xmlXPathFreeObject(xpathObj);
    }
  }
  
  if((machine = editor->selected_machine) != NULL && editor->current_edit_widget != NULL){
    /* get clipboard */
    buffer = gtk_clipboard_wait_for_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
    
    if(buffer == NULL)
      return;

    /* get position */
    if(editor->toolbar->selected_edit_mode == editor->toolbar->position){
      last_x = 0;
      paste_from_position = TRUE;

      if(AGS_IS_NOTE_EDIT(editor->current_edit_widget)){
	position_x = AGS_NOTE_EDIT(editor->current_edit_widget)->selected_x;
	position_y = AGS_NOTE_EDIT(editor->current_edit_widget)->selected_y;
      }else if(AGS_IS_PATTERN_EDIT(editor->current_edit_widget)){
	position_x = AGS_PATTERN_EDIT(editor->current_edit_widget)->selected_x;
	position_y = AGS_PATTERN_EDIT(editor->current_edit_widget)->selected_y;
      }
      
#ifdef DEBUG
      printf("pasting at position: [%u,%u]\n\0", position_x, position_y);
#endif
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
	
	ags_editor_paste_read_notation();

	break;
      }
      
      audio_node = audio_node->next;
    }

    xmlFreeDoc(clipboard); 

    if(AGS_IS_NOTE_EDIT(editor->current_edit_widget)){
      cr = gdk_cairo_create(GTK_WIDGET(AGS_NOTE_EDIT(editor->current_edit_widget)->drawing_area)->window);
      cairo_push_group(cr);
      
      ags_note_edit_draw_segment(editor->current_edit_widget, cr);
      ags_note_edit_draw_notation(editor->current_edit_widget, cr);

      if(paste_from_position){
	AGS_NOTE_EDIT(editor->current_edit_widget)->selected_x = (guint) ceil((double) last_x / 16.0) * 16;
	ags_note_edit_draw_position(AGS_NOTE_EDIT(editor->current_edit_widget), cr);
      }
    
      cairo_pop_group_to_source(cr);
      cairo_paint(cr);
    }else if(AGS_IS_PATTERN_EDIT(editor->current_edit_widget)){
      cr = gdk_cairo_create(GTK_WIDGET(AGS_PATTERN_EDIT(editor->current_edit_widget)->drawing_area)->window);
      cairo_push_group(cr);
      
      ags_pattern_edit_draw_segment(editor->current_edit_widget, cr);
      ags_pattern_edit_draw_notation(editor->current_edit_widget, cr);

      if(paste_from_position){
	AGS_PATTERN_EDIT(editor->current_edit_widget)->selected_x = (guint) ceil((double) last_x / 16.0) * 16;
	ags_pattern_edit_draw_position(AGS_PATTERN_EDIT(editor->current_edit_widget), cr);
      }
      
      cairo_pop_group_to_source(cr);
      cairo_paint(cr);
    }
  }
}

/**
 * ags_editor_copy:
 * @editor: an #AgsEditor
 *
 * Is emitted as machine changed of editor.
 *
 * Since: 0.4.2
 */
void
ags_editor_copy(AgsEditor *editor)
{
  AgsMachine *machine;
  
  AgsNotation *notation;

  GList *list_notation;
  xmlDocPtr clipboard;
  xmlNodePtr audio_node, notation_node;

  xmlChar *buffer;
  int size;
  gint i;
  gint selected_channel;

  if(editor->selected_machine != NULL && editor->current_edit_widget != NULL){
    machine = editor->selected_machine;
    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);

    /* create root node */
    audio_node = xmlNewNode(NULL, BAD_CAST "audio\0");
    xmlDocSetRootElement(clipboard, audio_node);

    /* create notation nodes */
    list_notation = machine->audio->notation;
    i = 0;

    while((selected_channel = ags_notebook_next_active_tab(editor->current_notebook,
							   i)) != -1){
      list_notation = g_list_nth(machine->audio->notation,
				 selected_channel);

      notation_node = ags_notation_copy_selection(AGS_NOTATION(list_notation->data));
      
      xmlAddChild(audio_node, notation_node);

      i++;
    }
    
    /* write to clipboard */
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8\0", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   buffer, size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    xmlFreeDoc(clipboard);
  }
}

/**
 * ags_editor_cut:
 * @editor: an #AgsEditor
 *
 * Is emitted as machine changed of editor.
 *
 * Since: 0.4.2
 */
void
ags_editor_cut(AgsEditor *editor)
{
  AgsMachine *machine;
  
  AgsNotation *notation;

  xmlDocPtr clipboard;
  xmlNodePtr audio_node, notation_node;
  cairo_t *cr;

  GList *list_notation;

  xmlChar *buffer;
  int size;
  gint i;
  gint selected_channel;

  if(editor->selected_machine != NULL && editor->current_edit_widget != NULL){
    machine = editor->selected_machine;

    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);

    /* create root node */
    audio_node = xmlNewNode(NULL, BAD_CAST "audio\0");
    xmlDocSetRootElement(clipboard, audio_node);

    /* create notation nodes */
    list_notation = machine->audio->notation;
    i = 0;

    if(AGS_IS_NOTE_EDIT(editor->current_edit_widget)){
      cr = gdk_cairo_create(GTK_WIDGET(AGS_NOTE_EDIT(editor->current_edit_widget)->drawing_area)->window);
    }else if(AGS_IS_PATTERN_EDIT(editor->current_edit_widget)){
      cr = gdk_cairo_create(GTK_WIDGET(AGS_PATTERN_EDIT(editor->current_edit_widget)->drawing_area)->window);
    }

    cairo_push_group(cr);
    
    while((selected_channel = ags_notebook_next_active_tab(editor->current_notebook,
							   i)) != -1){
      list_notation = g_list_nth(machine->audio->notation,
				 selected_channel);

      notation_node = ags_notation_cut_selection(AGS_NOTATION(list_notation->data));

      if(AGS_IS_NOTE_EDIT(editor->current_edit_widget)){
	ags_note_edit_draw_segment(AGS_NOTE_EDIT(editor->current_edit_widget), cr);
	ags_note_edit_draw_notation(AGS_NOTE_EDIT(editor->current_edit_widget), cr);
      }else if(AGS_IS_PATTERN_EDIT(editor->current_edit_widget)){
	ags_pattern_edit_draw_segment(AGS_PATTERN_EDIT(editor->current_edit_widget), cr);
	ags_pattern_edit_draw_notation(AGS_PATTERN_EDIT(editor->current_edit_widget), cr);
      }
      
      i++;
    }

    cairo_pop_group_to_source(cr);
    cairo_paint(cr);
    
    xmlAddChild(audio_node, notation_node);

    /* write to clipboard */
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8\0", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   buffer, size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    xmlFreeDoc(clipboard);
  }
}

void
ags_editor_invert(AgsEditor *editor)
{
  AgsMachine *machine;
  
  AgsNotation *notation;

  cairo_t *cr;

  GList *list_notation;

  int size;
  gint i;
  gint selected_channel;

  auto void ags_editor_invert_notation(AgsNotation *notation);

  void ags_editor_invert_notation(AgsNotation *notation){
    GList *note;
    guint lower, upper;

    note = notation->notes;

    if(note == NULL){
      return;
    }

    /* retrieve upper and lower */
    upper = 0;
    lower = G_MAXUINT;
    
    while(note != NULL){
      if(AGS_NOTE(note->data)->y < lower){
	lower = AGS_NOTE(note->data)->y;
      }

      if(AGS_NOTE(note->data)->y > upper){
	upper = AGS_NOTE(note->data)->y;
      }
      
      note = note->next;
    }

    /* invert */
    note = notation->notes;

    while(note != NULL){
      if((gdouble) AGS_NOTE(note->data)->y < (gdouble) (upper - lower) / 2.0){
	AGS_NOTE(note->data)->y = (upper - (AGS_NOTE(note->data)->y - lower));
      }else if((gdouble) AGS_NOTE(note->data)->y > (gdouble) (upper - lower) / 2.0){
	AGS_NOTE(note->data)->y = (lower + (upper - AGS_NOTE(note->data)->y));
      }
      
      note = note->next;
    }
  }
  
  if(editor->selected_machine != NULL && editor->current_edit_widget != NULL){
    machine = editor->selected_machine;

    /* create notation nodes */
    list_notation = machine->audio->notation;
    i = 0;

    if(AGS_IS_NOTE_EDIT(editor->current_edit_widget)){
      cr = gdk_cairo_create(GTK_WIDGET(AGS_NOTE_EDIT(editor->current_edit_widget)->drawing_area)->window);
    }else if(AGS_IS_PATTERN_EDIT(editor->current_edit_widget)){
      cr = gdk_cairo_create(GTK_WIDGET(AGS_PATTERN_EDIT(editor->current_edit_widget)->drawing_area)->window);
    }

    cairo_push_group(cr);
    
    while((selected_channel = ags_notebook_next_active_tab(editor->current_notebook,
							   i)) != -1){
      list_notation = g_list_nth(machine->audio->notation,
				 selected_channel);

      ags_editor_invert_notation(AGS_NOTATION(list_notation->data));

      if(AGS_IS_NOTE_EDIT(editor->current_edit_widget)){
	ags_note_edit_draw_segment(AGS_NOTE_EDIT(editor->current_edit_widget), cr);
	ags_note_edit_draw_notation(AGS_NOTE_EDIT(editor->current_edit_widget), cr);
      }else if(AGS_IS_PATTERN_EDIT(editor->current_edit_widget)){
	ags_pattern_edit_draw_segment(AGS_PATTERN_EDIT(editor->current_edit_widget), cr);
	ags_pattern_edit_draw_notation(AGS_PATTERN_EDIT(editor->current_edit_widget), cr);
      }
      
      i++;
    }

    cairo_pop_group_to_source(cr);
    cairo_paint(cr);

  }
}

/**
 * ags_editor_new:
 *
 * Creates an #AgsEditor
 *
 * Returns: a new #AgsEditor
 *
 * Since: 0.3
 */
AgsEditor*
ags_editor_new()
{
  AgsEditor *editor;

  editor = (AgsEditor *) g_object_new(AGS_TYPE_EDITOR, NULL);

  return(editor);
}
