/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/file/ags_file_gui.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>

#include <ags/main.h>

#include <ags/plugin/ags_plugin_factory.h>

#include <ags/lib/ags_container.h>

#include <ags/object/ags_plugin.h>

#include <ags/util/ags_id_generator.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_launch.h>

#include <ags/X/editor/ags_ruler.h>

#define AGS_FILE_READ_EDITOR_PARAMETER_NAME "ags-file-read-editor-parameter-name\0"

void ags_file_read_window_resolve_devout(AgsFileLookup *file_lookup,
					 AgsWindow *window);
void ags_file_write_window_resolve_devout(AgsFileLookup *file_lookup,
					  AgsWindow *window);

void ags_file_read_machine_resolve_audio(AgsFileLookup *file_lookup,
					 AgsMachine *machine);
void ags_file_read_machine_resolve_machine_editor(AgsFileLookup *file_lookup,
						  AgsMachine *machine);
void ags_file_write_machine_resolve_machine_editor(AgsFileLookup *file_lookup,
						   AgsMachine *machine);
void ags_file_write_machine_resolve_audio(AgsFileLookup *file_lookup,
					  AgsMachine *machine);
void ags_file_read_machine_resolve_rename_dialog(AgsFileLookup *file_lookup,
						 AgsMachine *machine);
void ags_file_write_machine_resolve_rename_dialog(AgsFileLookup *file_lookup,
						  AgsMachine *machine);

void ags_file_read_line_pad_resolve_channel(AgsFileLookup *file_lookup,
					    AgsPad *pad);

void ags_file_read_line_line_resolve_channel(AgsFileLookup *file_lookup,
					     AgsLine *line);

void ags_file_read_line_member_resolve_port(AgsFileLookup *file_lookup,
					    AgsLineMember *line_member);
void ags_file_write_line_member_resolve_port(AgsFileLookup *file_lookup,
					     AgsLineMember *line_member);

void ags_file_read_editor_resolve_parameter(AgsFileLookup *file_lookup,
					    AgsEditor *editor);
void ags_file_read_editor_launch(AgsFileLaunch *file_launch,
				 AgsEditor *editor);

void ags_file_read_machine_selector_resolve_parameter(AgsFileLookup *file_lookup,
						      AgsMachineSelector *machine_selector);

void
ags_file_read_widget(AgsFile *file, xmlNode *node, GtkWidget *widget)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_widget(AgsFile *file, xmlNode *parent, GtkWidget *widget)
{
  //TODO:JK: implement me
}

void
ags_file_read_window(AgsFile *file, xmlNode *node, AgsWindow **window)
{
  AgsWindow *gobject;
  AgsFileLookup *file_lookup;
  xmlNode *child;
  xmlChar *prop, *content;

  if(*window == NULL){
    gobject = g_object_new(AGS_TYPE_WINDOW,
			   NULL);
    *window = gobject;
  }else{
    gobject = *window;
  }

  g_object_set(G_OBJECT(gobject),
	       "ags-main\0", file->ags_main,
	       NULL);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  gobject->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  gobject->name = (gchar *) g_strdup(xmlGetProp(node, "name\0"));

  /* devout */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", gobject,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_read_window_resolve_devout), gobject);

  /* child elements */
  child = node->children;

  while(child != NULL){

    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-menu-bar\0",
		     11)){
	ags_file_read_menu_bar(file,
			       child,
			       &(gobject->menu_bar));
      }else if(!xmlStrncmp(child->name,
			   "ags-machine-counter-list\0",
			   24)){
	ags_file_read_machine_counter_list(file,
					   child,
					   &(gobject->machine_counter));
      }else if(!xmlStrncmp(child->name,
			   "ags-machine-list\0",
			   16)){
	GList *list;

	list = NULL;

	ags_file_read_machine_list(file,
				   child,
				   &list);

	while(list != NULL){
	  gtk_box_pack_start((GtkBox *) gobject->machines,
			     GTK_WIDGET(list->data),
			     FALSE, FALSE, 0);

	  list = list->next;
	}
      }else if(!xmlStrncmp(child->name,
			   "ags-editor\0",
			   10)){
	ags_file_read_editor(file,
			     child,
			     &(gobject->editor));
      }else if(!xmlStrncmp(child->name,
			   "ags-navigation\0",
			   14)){
	ags_file_read_navigation(file,
				 child,
				 &(gobject->navigation));
      }
    }

    child = child->next;
  }
}

void
ags_file_read_window_resolve_devout(AgsFileLookup *file_lookup,
				    AgsWindow *window)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) xmlGetProp(file_lookup->node,
			       "devout\0");

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file, xpath);

  if(id_ref != NULL){
    g_object_set(G_OBJECT(window),
		 "devout\0", (AgsWindow *) id_ref->ref,
		 NULL);
  }
}

xmlNode*
ags_file_write_window(AgsFile *file, xmlNode *parent, AgsWindow *window)
{
  AgsFileLookup *file_lookup;
  xmlNode *node, *child;
  gchar *id;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-window\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", window,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", window->flags));

  xmlNewProp(node,
	     "name\0",
	     g_strdup(window->name));

  /* devout */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", window,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_write_window_resolve_devout), window);

  xmlAddChild(parent,
	      node);

  /* child elements */
  ags_file_write_menu_bar(file,
			  node,
			  window->menu_bar);

  ags_file_write_machine_list(file,
			      node,
			      gtk_container_get_children(GTK_CONTAINER(window->machines)));

  ags_file_write_editor(file,
			node,
			window->editor);

  ags_file_write_navigation(file,
			    node,
			    window->navigation);
}

void
ags_file_write_window_resolve_devout(AgsFileLookup *file_lookup,
				     AgsWindow *window)
{
  AgsFileIdRef *id_ref;
  gchar *id;

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, window->devout);

  id = xmlGetProp(id_ref->node, AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     "devout\0",
	     g_strdup_printf("xpath=//ags-devout[@id='%s']\0", id));
}

void
ags_file_read_menu_bar(AgsFile *file, xmlNode *node, AgsMenuBar **menu_bar)
{
  AgsMenuBar *gobject;
  xmlNode *child;
  xmlChar *prop, *content;

  if(*menu_bar == NULL){
    gobject = g_object_new(AGS_TYPE_MENU_BAR,
			   NULL);
    *menu_bar = gobject;
  }else{
    gobject = *menu_bar;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));
}

xmlNode*
ags_file_write_menu_bar(AgsFile *file, xmlNode *parent, AgsMenuBar *menu_bar)
{
  xmlNode *node, *child;
  gchar *id;
  guint i;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-menu-bar\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", menu_bar,
				   NULL));

  xmlAddChild(parent,
	      node);
}

void
ags_file_read_machine_counter(AgsFile *file, xmlNode *node, AgsMachineCounter **machine_counter)
{
  AgsMachineCounter *ptr;
  xmlNode *child;
  xmlChar *prop, *content;

  if(*machine_counter == NULL){
    ptr = ags_machine_counter_alloc(NULL, NULL,
				    G_TYPE_NONE, 0);
    *machine_counter = ptr;
  }else{
    ptr = *machine_counter;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", ptr,
				   NULL));

  ptr->version = xmlGetProp(node,
			    AGS_FILE_VERSION_PROP);

  ptr->build_id = xmlGetProp(node,
			     AGS_FILE_BUILD_ID_PROP);

  ptr->machine_type = g_type_from_name(xmlGetProp(node,
						  AGS_FILE_TYPE_PROP));

  ptr->counter = g_ascii_strtoull(xmlGetProp(node,
					     "counter\0"),
				  NULL,
				  10);
}

xmlNode*
ags_file_write_machine_counter(AgsFile *file, xmlNode *parent, AgsMachineCounter *machine_counter)
{
  xmlNode *node, *child;
  gchar *id;
  guint i;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-machine-counter\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", machine_counter,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_VERSION_PROP,
	     machine_counter->version);

  xmlNewProp(node,
	     AGS_FILE_BUILD_ID_PROP,
	     machine_counter->build_id);

  xmlNewProp(node,
	     AGS_FILE_TYPE_PROP,
	     g_type_name(machine_counter->machine_type));

  xmlNewProp(node,
	     "counter\0",
	     g_strdup_printf("%d\0", machine_counter->counter));

  xmlAddChild(parent,
	      node);
}

void
ags_file_read_machine_counter_list(AgsFile *file, xmlNode *node, GList **machine_counter)
{
  AgsMachineCounter *current;
  GList *list;
  xmlNode *child;
  xmlChar *id;

  id = xmlGetProp(node, AGS_FILE_ID_PROP);

  child = node->children;
  list = NULL;

  while(child != NULL){
    current = NULL;
    ags_file_read_machine_counter(file, child, &current);

    list = g_list_prepend(list, current);

    child = child->next;
  }

  list = g_list_reverse(list);
  *machine_counter = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_machine_counter_list(AgsFile *file, xmlNode *parent, GList *machine_counter)
{
  AgsMachineCounter *current;
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-machine-counter-list\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = machine_counter;

  while(list != NULL){
    ags_file_write_machine_counter(file, node, AGS_MACHINE_COUNTER(list->data));

    list = list->next;
  }

  return(node);
}

void
ags_file_read_machine(AgsFile *file, xmlNode *node, AgsMachine **machine)
{
  AgsWindow *window;
  AgsMachine *gobject;
  AgsFileLookup *file_lookup;
  GType machine_type;
  xmlNode *child;
  static gboolean machine_type_is_registered = FALSE;

  if(*machine == NULL){
    if(!machine_type_is_registered){
      ags_main_register_machine_type();

      machine_type_is_registered = TRUE;
    }

    machine_type = g_type_from_name(xmlGetProp(node,
					       AGS_FILE_TYPE_PROP));

    gobject = (AgsMachine *) g_object_new(machine_type,
					  NULL);
    *machine = gobject;
  }else{
    gobject = *machine;
  }

  gobject->flags |= AGS_MACHINE_PREMAPPED_RECALL;
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  gobject->version = g_strdup(xmlGetProp(node,
					 AGS_FILE_VERSION_PROP));

  gobject->build_id = g_strdup(xmlGetProp(node,
					  AGS_FILE_BUILD_ID_PROP));

  gobject->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  gobject->file_input_flags = (guint) g_ascii_strtoull(xmlGetProp(node, "file-input-flags\0"),
						       NULL,
						       16);

  gobject->name = g_strdup(xmlGetProp(node,
				      AGS_FILE_NAME_PROP));

  /* audio */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", gobject,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_read_machine_resolve_audio), gobject);

  /* machine-editor */
  //TODO:JK: uncomment me
  //  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
  //					       "file\0", file,
  //					       "node\0", node,
  //					       "reference\0", gobject,
  //					       NULL);
  //  ags_file_add_lookup(file, (GObject *) file_lookup);
  //  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
  //		   G_CALLBACK(ags_file_read_machine_resolve_machine_editor), gobject);

  /* rename-dialog */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", gobject,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_read_machine_resolve_rename_dialog), gobject);

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrcmp(child->name,
		    ags_plugin_get_xml_type(AGS_PLUGIN(gobject)))){
	ags_plugin_read(file,
			child,
			AGS_PLUGIN(gobject));
      }else if(!xmlStrncmp(child->name,
			   "ags-pad-list\0",
			   12)){
	GList *pad, *list;

	pad = NULL;

	ags_file_read_pad_list(file,
			       child,
			       &pad);

	if(!xmlStrncmp(xmlGetProp(child,
				  AGS_FILE_SCOPE_PROP),
		       "output\0",
		       6)){
	  if(!GTK_IS_BOX(gobject->output)){
	    ags_container_add_all(gobject->output,
				  pad);
	  }else{
	    list = pad;

	    while(list != NULL){
	      gtk_box_pack_start(GTK_BOX(gobject->output),
				 GTK_WIDGET(list->data),
				 FALSE, FALSE,
				 0);

	      list = list->next;
	    }
	  }
	}else{
	  if(!GTK_IS_BOX(gobject->input)){
	    ags_container_add_all(gobject->input,
				  pad);
	  }else{
	    list = pad;

	    while(list != NULL){
	      gtk_box_pack_start(GTK_BOX(gobject->input),
				 GTK_WIDGET(list->data),
				 FALSE, FALSE,
				 0);

	      list = list->next;
	    }
	  }
	}

	g_list_free(pad);
      }
    }

    child = child->next;
  }
}

void
ags_file_read_machine_resolve_audio(AgsFileLookup *file_lookup,
				    AgsMachine *machine)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) xmlGetProp(file_lookup->node,
			       "audio\0");

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file, xpath);

  g_object_set(G_OBJECT(machine),
	       "audio\0", (AgsAudio *) id_ref->ref,
	       NULL);

  AGS_AUDIO(id_ref->ref)->machine = machine;
}

void
ags_file_read_machine_resolve_machine_editor(AgsFileLookup *file_lookup,
					     AgsMachine *machine)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) xmlGetProp(file_lookup->node,
			       "machine-editor\0");

  if(xpath == NULL){
    return;
  }

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file, xpath);

  //TODO:JK: use g_object_set
  machine->properties = (GtkDialog *) id_ref->ref;
}

void
ags_file_read_machine_resolve_rename_dialog(AgsFileLookup *file_lookup,
					    AgsMachine *machine)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) xmlGetProp(file_lookup->node,
			       "rename-dialog\0");

  if(xpath == NULL){
    return;
  }

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file, xpath);

  //TODO:JK: use g_object_set
  machine->rename = (GtkDialog *) id_ref->ref;
}

xmlNode*
ags_file_write_machine(AgsFile *file, xmlNode *parent, AgsMachine *machine)
{
  AgsFileLookup *file_lookup;
  xmlNode *node, *child;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-machine\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", machine,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_TYPE_PROP,
	     G_OBJECT_TYPE_NAME(machine));

  xmlNewProp(node,
	     AGS_FILE_VERSION_PROP,
	     machine->version);

  xmlNewProp(node,
	     AGS_FILE_BUILD_ID_PROP,
	     machine->build_id);

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", machine->flags));

  xmlNewProp(node,
	     "file-input-flags\0",
	     g_strdup_printf("%x\0", machine->file_input_flags));

  xmlNewProp(node,
	     AGS_FILE_NAME_PROP,
	     machine->name);

  xmlAddChild(parent,
	      node);  

  /* audio */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", machine,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_write_machine_resolve_audio), machine);

  /* machine-editor */
  //TODO:JK: uncomment me
  //  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
  //					       "file\0", file,
  //					       "node\0", node,
  //					       "reference\0", machine,
  //					       NULL);
  //  ags_file_add_lookup(file, (GObject *) file_lookup);
  //  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
  //		   G_CALLBACK(ags_file_write_machine_resolve_machine_editor), machine);

  /* rename-dialog */
  //TODO:JK: uncomment me
  //  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
  //					       "file\0", file,
  //					       "node\0", node,
  //					       "reference\0", machine,
  //					       NULL);
  //  ags_file_add_lookup(file, (GObject *) file_lookup);
  //  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
  //		   G_CALLBACK(ags_file_write_machine_resolve_rename_dialog), machine);

  /* child elements */
  ags_plugin_write(file,
  		   node,
  		   AGS_PLUGIN(machine));

  if(machine->output != NULL){
    child = ags_file_write_pad_list(file,
				    node,
				    gtk_container_get_children(machine->output));
    xmlNewProp(child,
	       AGS_FILE_SCOPE_PROP,
	       "output\0");
  }
  
  if(machine->input != NULL){
    child = ags_file_write_pad_list(file,
				    node,
				    gtk_container_get_children(machine->input));
    xmlNewProp(child,
	       AGS_FILE_SCOPE_PROP,
	       "input\0");
  }
}

void
ags_file_write_machine_resolve_machine_editor(AgsFileLookup *file_lookup,
					      AgsMachine *machine)
{
  AgsFileIdRef *id_ref;
  gchar *id;

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, machine->properties);

  id = xmlGetProp(id_ref->node, AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     "machine-editor\0",
	     g_strdup_printf("xpath=//ags-machine-editor[@id='%s']\0", id));
}

void
ags_file_write_machine_resolve_rename_dialog(AgsFileLookup *file_lookup,
					     AgsMachine *machine)
{
  AgsFileIdRef *id_ref;
  gchar *id;

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, machine->rename);

  id = xmlGetProp(id_ref->node, AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     "rename-dialog\0",
	     g_strdup_printf("xpath=//ags-dialog[@id='%s']\0", id));
}

void
ags_file_write_machine_resolve_audio(AgsFileLookup *file_lookup,
				     AgsMachine *machine)
{
  AgsFileIdRef *id_ref;
  gchar *id;

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, AGS_AUDIO(machine->audio));

  id = xmlGetProp(id_ref->node, AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     "audio\0",
	     g_strdup_printf("xpath=//ags-audio[@id='%s']\0", id));
}

void
ags_file_read_machine_list(AgsFile *file, xmlNode *node, GList **machine)
{
  AgsMachine *current;
  GList *list;
  xmlNode *child;
  xmlChar *id;

  id = xmlGetProp(node, AGS_FILE_ID_PROP);

  child = node->children;
  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-machine\0",
		     12)){
	current = NULL;
	ags_file_read_machine(file, child, &current);

	list = g_list_prepend(list, current);
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *machine = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_machine_list(AgsFile *file, xmlNode *parent, GList *machine)
{
  AgsMachine *current;
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-machine-list\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = machine;

  while(list != NULL){
    ags_file_write_machine(file, node, AGS_MACHINE(list->data));

    list = list->next;
  }

  return(node);
}

void
ags_file_read_pad(AgsFile *file, xmlNode *node, AgsPad **pad)
{
  AgsPad *gobject;
  AgsFileLookup *file_lookup;
  GType pad_type;
  xmlNode *child;

  if(*pad == NULL){
    pad_type = g_type_from_name(xmlGetProp(node,
					   AGS_FILE_TYPE_PROP));
    
    gobject = (AgsPad *) g_object_new(pad_type,
				      NULL);
    *pad = gobject;
  }else{
    gobject = *pad;
  }

  if(gobject == NULL)
    return;
  
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  gobject->version = g_strdup(xmlGetProp(node,
					 AGS_FILE_VERSION_PROP));

  gobject->build_id = g_strdup(xmlGetProp(node,
					  AGS_FILE_BUILD_ID_PROP));

  gobject->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  /* channel */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", gobject,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
			       G_CALLBACK(ags_file_read_window_resolve_devout), gobject);

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrcmp(child->name,
		    ags_plugin_get_xml_type(AGS_PLUGIN(gobject)))){
	ags_plugin_read(file,
			child,
			AGS_PLUGIN(gobject));
      }else if(!xmlStrncmp(child->name,
			   "ags-line-list\0",
			   13)){
	xmlNode *line_node;
	GList *start, *list;

	list = NULL;

	ags_file_read_line_list(file,
				child,
				&list);
	start = list;

	/* add line to pad */
	line_node = child->children;

	while(line_node != NULL){
	  if(line_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(line_node->name,
			   "ags-line\0",
			   8)){
	      guint x, y;
	      guint width, height;

	      g_object_set(G_OBJECT(list->data),
			   "pad\0", gobject,
			   NULL);

	      x = g_ascii_strtoull(xmlGetProp(line_node,
					      "left-attach\0"),
				   NULL,
				   10);

	      y = g_ascii_strtoull(xmlGetProp(line_node,
					      "top-attach\0"),
				   NULL,
				   10);

	      width = g_ascii_strtoull(xmlGetProp(line_node,
						  "right-attach\0"),
				   NULL,
				   10) - x;

	      height = g_ascii_strtoull(xmlGetProp(line_node,
						   "bottom-attach\0"),
				   NULL,
				   10) - y;

	      ags_expander_set_add(gobject->expander_set,
				   GTK_WIDGET(list->data),
				   x, y,
				   width, height);

	      list = list->next;
	    }
	  }

	  line_node = line_node->next;
	}

	g_list_free(start);
      }
    }

    child = child->next;
  }
}

void
ags_file_read_line_pad_resolve_channel(AgsFileLookup *file_lookup,
				       AgsPad *pad)
{
  AgsFile *file;
  AgsMachine *machine;
  AgsFileIdRef *id_ref;
  xmlNode *node, *audio_node, *channel_node;
  xmlXPathContext *xpath_context;
  xmlXPathObject *xpath_object;
  xmlChar *xpath;
  guint position;
  guint i;

  file = file_lookup->file;

  machine = (AgsMachine *) gtk_widget_get_ancestor(GTK_WIDGET(pad),
						   AGS_TYPE_MACHINE);

  node = file_lookup->node;

  /* retrieve position */
  xpath_context = xmlXPathNewContext(file->doc);
  //  xmlXPathSetContextNode(node->parent,
  //			 xpath_context);
  xpath_context->node = node->parent;

  xpath_object = xmlXPathEval("./ags-pad\0",
			      xpath_context);

  for(i = 0; xpath_object->nodesetval->nodeTab[i] != node && i < xpath_object->nodesetval->nodeMax; i++);

  position = i * machine->audio->audio_channels;

  /*  */
  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, machine->audio);
  audio_node = NULL;

  if(id_ref != NULL){
    audio_node = id_ref->node;
  }

  /*  */
  xpath = g_strdup_printf("./ags-audio[@id='%s']/ags-channel[position='%d']\0",
			  xmlGetProp(audio_node, "id\0"),
			  position);

  xpath_context = xmlXPathNewContext(file->doc);
  xpath_object = xmlXPathEval(xpath,
			      xpath_context);

  /*  */
  if(xpath_object != NULL && xpath_object->nodesetval != NULL){
    g_object_set(G_OBJECT(pad),
		 "channel\0", (AgsChannel *) xpath_object->nodesetval->nodeTab[0],
		 NULL);
  }
}

xmlNode*
ags_file_write_pad(AgsFile *file, xmlNode *parent, AgsPad *pad)
{
  AgsFileLookup *file_lookup;
  AgsExpanderSetChild *expander_set_child;
  xmlNode *node, *child;
  xmlNode *line_node;
  GList *line;
  gchar *id;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-pad\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", pad,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_TYPE_PROP,
	     G_OBJECT_TYPE_NAME(pad));

  xmlNewProp(node,
	     AGS_FILE_VERSION_PROP,
	     pad->version);

  xmlNewProp(node,
	     AGS_FILE_BUILD_ID_PROP,
	     pad->build_id);

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", ((~AGS_PAD_CONNECTED) & (pad->flags))));

  xmlAddChild(parent,
	      node);

  /* child elements */
  ags_plugin_write(file,
  		   node,
  		   AGS_PLUGIN(pad));

  line = gtk_container_get_children(GTK_CONTAINER(pad->expander_set));

  child = ags_file_write_line_list(file,
				   node,
				   line);

  line_node = child->children;

  while(line != NULL){
    expander_set_child = ags_expander_set_child_find(pad->expander_set,
						     line->data);

    xmlNewProp(line_node,
	       "left-attach\0",
	       g_strdup_printf("%d\0", expander_set_child->x));

    xmlNewProp(line_node,
	       "top-attach\0",
	       g_strdup_printf("%d\0", expander_set_child->y));

    xmlNewProp(line_node,
	       "right-attach\0",
	       g_strdup_printf("%d\0", expander_set_child->x + expander_set_child->width));

    xmlNewProp(line_node,
	       "bottom-attach\0",
	       g_strdup_printf("%d\0", expander_set_child->y + expander_set_child->height));
    
    line = line->next;
    line_node = line_node->next;
  }
}

void
ags_file_read_pad_list(AgsFile *file, xmlNode *node, GList **pad)
{
  AgsPad *current;
  GList *list;
  xmlNode *child;
  xmlChar *id;

  id = xmlGetProp(node, AGS_FILE_ID_PROP);

  child = node->children;
  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-pad\0",
		     8)){
	current = NULL;
	ags_file_read_pad(file, child, &current);

	list = g_list_prepend(list, current);
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *pad = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_pad_list(AgsFile *file, xmlNode *parent, GList *pad)
{
  AgsPad *current;
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-pad-list\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = pad;

  while(list != NULL){
    ags_file_write_pad(file, node, AGS_PAD(list->data));

    list = list->next;
  }

  return(node);
}

void
ags_file_read_line(AgsFile *file, xmlNode *node, AgsLine **line)
{
  AgsLine *gobject;
  AgsFileLookup *file_lookup;
  GType line_type;
  xmlNode *child;

  if(*line == NULL){
    line_type = g_type_from_name(xmlGetProp(node,
					    AGS_FILE_TYPE_PROP));
    
    gobject = (AgsLine *) g_object_new(line_type,
				       NULL);
    *line = gobject;
  }else{
    gobject = *line;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  gobject->version = g_strdup(xmlGetProp(node,
					 AGS_FILE_VERSION_PROP));

  gobject->build_id = g_strdup(xmlGetProp(node,
					  AGS_FILE_BUILD_ID_PROP));

  gobject->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);
  gobject->flags |= AGS_LINE_PREMAPPED_RECALL;

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrcmp(child->name,
		    ags_plugin_get_xml_type(AGS_PLUGIN(gobject)))){
	ags_plugin_read(file,
			child,
			AGS_PLUGIN(gobject));
      }else if(!xmlStrncmp(child->name,
			   "ags-line-member-list\0",
			   20)){
	xmlNode *line_member_node;
	GList *start, *list;

	list = NULL;

	ags_file_read_line_member_list(file,
				       child,
				       &list);
	start = list;
	
	/* remove default line members */
	gtk_widget_destroy(GTK_WIDGET(gobject->expander));

	gobject->expander = ags_expander_new(1, 1);
	gtk_table_set_row_spacings(gobject->expander->table,
				   2);
	gtk_table_set_col_spacings(gobject->expander->table,
				   2);
	gtk_box_pack_start(GTK_BOX(gobject),
			   GTK_WIDGET(gobject->expander),
			   TRUE, TRUE,
			   0);

	/* add line member to line */
	line_member_node = child->children;

	while(line_member_node != NULL){
	  if(line_member_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(child->name,
			   "ags-line-member\0",
			   15)){
	      guint x, y;
	      guint width, height;
	      guint control_width, control_height;

	      /* pack */
	      x = g_ascii_strtoull(xmlGetProp(line_member_node,
					      "left-attach\0"),
				   NULL,
				   10);

	      y = g_ascii_strtoull(xmlGetProp(line_member_node,
					      "top-attach\0"),
				   NULL,
				   10);

	      width = g_ascii_strtoull(xmlGetProp(line_member_node,
						  "right-attach\0"),
				       NULL,
				       10) - x;

	      height = g_ascii_strtoull(xmlGetProp(line_member_node,
						   "bottom-attach\0"),
					NULL,
					10) - y;

	      ags_expander_add(gobject->expander,
			       GTK_WIDGET(list->data),
			       x, y,
			       width, height);	      

	      /* set size request */
	      control_width = g_ascii_strtoull(xmlGetProp(line_member_node,
							  "width\0"),
					       NULL,
					       10);

	      control_height = g_ascii_strtoull(xmlGetProp(line_member_node,
							   "height\0"),
						NULL,
						10);

	      gtk_widget_set_size_request(GTK_WIDGET(list->data),
					  control_width, control_height);
	      
	      /* iterate */
	      list = list->next;
	    }
	  }
	  
	  line_member_node = line_member_node->next;
	}

	g_list_free(start);
      }
    }

    child = child->next;
  }
}

xmlNode*
ags_file_write_line(AgsFile *file, xmlNode *parent, AgsLine *line)
{
  AgsFileLookup *file_lookup;
  AgsExpanderChild *expander_child;
  xmlNode *node, *child;
  xmlNode *line_member_node;
  GList *line_member;
  gchar *id;
  guint control_width, control_height;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-line\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", line,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_TYPE_PROP,
	     G_OBJECT_TYPE_NAME(line));

  xmlNewProp(node,
	     AGS_FILE_VERSION_PROP,
	     line->version);

  xmlNewProp(node,
	     AGS_FILE_BUILD_ID_PROP,
	     line->build_id);

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", ((~AGS_LINE_CONNECTED)&(line->flags))));

  xmlAddChild(parent,
	      node);

  /* child elements */
  ags_plugin_write(file,
  		   node,
  		   AGS_PLUGIN(line));

  line_member = gtk_container_get_children(GTK_CONTAINER(line->expander->table));

  child = ags_file_write_line_member_list(file,
					  node,
					  line_member);

  line_member_node = child->children;

  while(line_member != NULL){
    if(AGS_IS_LINE_MEMBER(line_member->data)){
      expander_child = ags_expander_child_find(line->expander,
					       line_member->data);

      xmlNewProp(line_member_node,
		 "left-attach\0",
		 g_strdup_printf("%d\0", expander_child->x));

      xmlNewProp(line_member_node,
		 "top-attach\0",
		 g_strdup_printf("%d\0", expander_child->y));

      xmlNewProp(line_member_node,
		 "right-attach\0",
		 g_strdup_printf("%d\0", expander_child->x + expander_child->width));

      xmlNewProp(line_member_node,
		 "bottom-attach\0",
		 g_strdup_printf("%d\0", expander_child->y + expander_child->height));

      gtk_widget_get_size_request(GTK_BIN(expander_child->child)->child,
				  &control_width, &control_height);

      xmlNewProp(line_member_node,
		 "width\0",
		 g_strdup_printf("%d\0", control_width));

      xmlNewProp(line_member_node,
		 "height\0",
		 g_strdup_printf("%d\0", control_height));

      line_member_node = line_member_node->next;
    }

    line_member = line_member->next;
  }
}

void
ags_file_read_line_list(AgsFile *file, xmlNode *node, GList **line)
{
  AgsLine *current;
  GList *list;
  xmlNode *child;
  xmlChar *id;

  id = xmlGetProp(node, AGS_FILE_ID_PROP);

  child = node->children;
  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-line\0",
		     9)){
	current = NULL;
	ags_file_read_line(file, child, &current);

	list = g_list_prepend(list, current);
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *line = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_line_list(AgsFile *file, xmlNode *parent, GList *line)
{
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-line-list\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = line;

  while(list != NULL){
    ags_file_write_line(file, node, AGS_LINE(list->data));

    list = list->next;
  }

  return(node);
}

void
ags_file_read_line_member(AgsFile *file, xmlNode *node, AgsLineMember **line_member)
{
  AgsFileLookup *file_lookup;
  AgsLineMember *gobject;
  GtkAdjustment *adjustment;
  GtkWidget *child_widget;
  xmlNode *child;
  xmlChar *prop, *content;
  gchar *widget_type;
  gchar *label;
  gchar *task_type;
  guint width, height;
  static gboolean widget_type_is_registered = FALSE;

  if(*line_member == NULL){
    gobject = g_object_new(AGS_TYPE_LINE_MEMBER,
			   NULL);
    *line_member = gobject;
  }else{
    gobject = *line_member;

    if(!AGS_IS_LINE_MEMBER(gobject)){
      return;
    }
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  if(!widget_type_is_registered){
    ags_main_register_widget_type();

    widget_type_is_registered = TRUE;
  }

  widget_type = (gchar *) xmlGetProp(node, "widget-type\0");
  g_object_set(gobject,
	       "widget-type\0", g_type_from_name(widget_type),
	       NULL);
  child_widget = (GtkWidget *) gtk_bin_get_child(gobject);

  /* label */
  label = (gchar *) xmlGetProp(node, "label\0");

  if(label != NULL){
    g_object_set(G_OBJECT(gobject),
		 "widget-label\0", label,
		 NULL);
  }

  /* size */
  width = (guint) g_ascii_strtoull(xmlGetProp(node, "width\0"),
				   NULL,
				   10);

  height = (guint) g_ascii_strtoull(xmlGetProp(node, "height\0"),
				    NULL,
				    10);

  gtk_widget_set_size_request(child_widget,
			      width, height);

  /* check misc */
  if(GTK_IS_MISC(child_widget)){
    guint xalign, yalign;
    guint xpad, ypad;

    xalign = (guint) g_ascii_strtoull(xmlGetProp(node, "xalign\0"),
				      NULL,
				      10);

    yalign = (guint) g_ascii_strtoull(xmlGetProp(node, "yalign\0"),
				      NULL,
				      10);

    xpad = (guint) g_ascii_strtoull(xmlGetProp(node, "xpad\0"),
				    NULL,
				    10);
    
    ypad = (guint) g_ascii_strtoull(xmlGetProp(node, "ypad\0"),
				    NULL,
				    10);
  }

  /* check adjustment and toggle types */
  adjustment = NULL;

  if(GTK_IS_TOGGLE_BUTTON(child_widget)){
    if(!xmlStrncmp(AGS_FILE_TRUE,
		   xmlGetProp(node, "value\0"),
		   5)){
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(child_widget),
				   TRUE);
    }
  }else if(AGS_IS_DIAL(child_widget)){
    AgsDial *dial;
    
    dial = child_widget;
    adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);
    g_object_set(child_widget,
		 "adjustment\0", adjustment,
		 NULL);
    gtk_widget_set_size_request(dial,
				2 * dial->radius + 2 * dial->outline_strength + dial->button_width,
				2 * dial->radius + 2 * dial->outline_strength);
  }else if(GTK_IS_RANGE(child_widget)){
    adjustment = GTK_RANGE(child_widget)->adjustment;

    if(!xmlStrncmp(AGS_FILE_TRUE,
		   xmlGetProp(node, "inverted\0"),
		   9)){
      gtk_range_set_inverted(GTK_RANGE(child_widget),
			     TRUE);
    }
  }else if(AGS_IS_INDICATOR(child_widget)){
    adjustment = gtk_adjustment_new(0.0, 0.0, 10.0, 1.0, 1.0, 10.0);
    g_object_set(child_widget,
		 "adjustment\0", adjustment,
		 NULL);
  }

  //TODO:JK: implement more types

  if(adjustment != NULL){
    gdouble upper, lower;
    gdouble step, page;
    gdouble value;

    step = (gdouble) g_ascii_strtod(xmlGetProp(node, "step\0"),
				    NULL);
    gtk_adjustment_set_step_increment(adjustment,
				      step);
    
    lower = (gdouble) g_ascii_strtod(xmlGetProp(node, "lower\0"),
				     NULL);
    gtk_adjustment_set_lower(adjustment,
			     lower);

    upper = (gdouble) g_ascii_strtod(xmlGetProp(node, "upper\0"),
				     NULL);
    gtk_adjustment_set_upper(adjustment,
			     upper);
    
    //    page = (gdouble) g_ascii_strtod(xmlGetProp(node, "page\0"),
    //				    NULL);
    //    gtk_adjustment_set_page_size(adjustment,
    //				 page);
    
    value = (gdouble) g_ascii_strtod(xmlGetProp(node, "value\0"),
				     NULL);
    gtk_adjustment_set_value(adjustment,
			     value);
  }
  
  /* flags */
  gobject->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  if((task_type = xmlGetProp(node, "task-type\0")) != NULL){
    gobject->task_type = g_type_from_name(task_type);
  }
  
  /* port */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", gobject,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_read_line_member_resolve_port), gobject);
}

void
ags_file_read_line_member_resolve_port(AgsFileLookup *file_lookup,
				       AgsLineMember *line_member)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) xmlGetProp(file_lookup->node,
			       "port\0");

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file, xpath);

  if(id_ref == NULL){
    g_warning("couldn't find port\0");
    return;
  }

  g_object_set(G_OBJECT(line_member),
	       "port\0", (AgsPort *) id_ref->ref,
	       NULL);
}

xmlNode*
ags_file_write_line_member(AgsFile *file, xmlNode *parent, AgsLineMember *line_member)
{
  AgsFileLookup *file_lookup;
  GtkWidget *child_widget;
  GtkAdjustment *adjustment;
  xmlNode *node;
  gchar *id;
  gchar *label;
  
  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-line-member\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", line_member,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", line_member->flags));

  xmlNewProp(node,
	     "widget-type\0",
	     g_strdup_printf("%s\0", g_type_name(line_member->widget_type)));

  child_widget = gtk_bin_get_child(GTK_BIN(line_member));
  
  label = NULL;
  g_object_get(G_OBJECT(child_widget),
	       "label\0", &label,
	       NULL);

  if(label != NULL){
    xmlNewProp(node,
	       "label\0",
	       g_strdup_printf("%s\0", label));
  }

  if(line_member->task_type != G_TYPE_NONE){
    xmlNewProp(node,
	       AGS_FILE_FLAGS_PROP,
	       g_strdup_printf("%s\0", g_type_name(line_member->task_type)));
  }

  /*  */
  if(GTK_IS_MISC(child_widget)){
    gfloat xalign, yalign;
    gint xpad, ypad;
    
    gtk_misc_get_alignment(GTK_MISC(child_widget),
			   &xalign, &yalign);
    xmlNewProp(node,
	       "xalign\0",
	       g_strdup_printf("%d\0", xalign));
    xmlNewProp(node,
	       "yalign\0",
	       g_strdup_printf("%d\0", yalign));
    
    gtk_misc_get_padding(GTK_MISC(child_widget),
			 &xpad, &ypad);
    xmlNewProp(node,
	       "xpad\0",
	       g_strdup_printf("%d\0", xpad));
    xmlNewProp(node,
	       "ypad\0",
	       g_strdup_printf("%d\0", ypad));
    
  }

  /*  */
  adjustment = NULL;

  /*  */
  if(GTK_IS_TOGGLE_BUTTON(child_widget)){
    xmlNewProp(node,
	       "value\0",
	       g_strdup_printf("%s\0", ((gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(child_widget))) ?
					AGS_FILE_TRUE:
					AGS_FILE_FALSE)));
  }else if(AGS_IS_DIAL(child_widget)){
    adjustment = AGS_DIAL(child_widget)->adjustment;

    //TODO:JK: improve dial widget work-around
    
  }else if(GTK_IS_RANGE(child_widget)){
    adjustment = GTK_RANGE(child_widget)->adjustment;

    xmlNewProp(node,
	       "inverted\0",
	       g_strdup_printf("%s\0", (gtk_range_get_inverted(GTK_RANGE(child_widget)) ?
					AGS_FILE_TRUE :
					AGS_FILE_FALSE)));
  }else if(AGS_IS_INDICATOR(child_widget)){
    adjustment = AGS_INDICATOR(child_widget)->adjustment;
  }

  if(adjustment != NULL){
    gdouble upper, lower;
    gdouble page, step;
    gdouble value;

    xmlNewProp(node,
	       "upper\0",
	       g_strdup_printf("%.8f\0", adjustment->upper));
    xmlNewProp(node,
	       "lower\0",
	       g_strdup_printf("%.8f\0", adjustment->lower));
    
    xmlNewProp(node,
	       "page\0",
	       g_strdup_printf("%.8f\0", adjustment->page_size));
    xmlNewProp(node,
	       "step\0",
	       g_strdup_printf("%.8f\0", adjustment->step_increment));
    
    xmlNewProp(node,
	       "value\0",
	       g_strdup_printf("%.8f\0", adjustment->value));
  }

  /* port */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", line_member,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_write_line_member_resolve_port), line_member);

  xmlAddChild(parent,
	      node);
}

void
ags_file_write_line_member_resolve_port(AgsFileLookup *file_lookup,
					AgsLineMember *line_member)
{
  AgsFileIdRef *id_ref;
  gchar *id;

  if(line_member->port == NULL){
    return;
  }

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, line_member->port);

  if(id_ref == NULL){
    return;
  }

  id = xmlGetProp(id_ref->node, AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     "port\0",
	     g_strdup_printf("xpath=//*[@id='%s']\0", id));
}

void
ags_file_read_line_member_list(AgsFile *file, xmlNode *node, GList **line_member)
{
  AgsLineMember *current;
  GList *list;
  xmlNode *child;
  xmlChar *id;

  id = xmlGetProp(node, AGS_FILE_ID_PROP);

  child = node->children;
  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-line-member\0",
		     16)){
	current = NULL;
	ags_file_read_line_member(file, child, &current);

	list = g_list_prepend(list, current);
      }
    }

    child = child->next;
  }

  list = g_list_reverse(list);
  *line_member = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));
}

xmlNode*
ags_file_write_line_member_list(AgsFile *file, xmlNode *parent, GList *line_member)
{
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-line-member-list\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = line_member;

  while(list != NULL){
    if(AGS_IS_LINE_MEMBER(list->data)){
      ags_file_write_line_member(file, node, AGS_LINE_MEMBER(list->data));
    }

    list = list->next;
  }

  return(node);
}

void
ags_file_read_dialog(AgsFile *file, xmlNode *node, GtkDialog **dialog)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_dialog(AgsFile *file, xmlNode *parent, GtkDialog *dialog)
{
  //TODO:JK: implement me
}

void
ags_file_read_dialog_list(AgsFile *file, xmlNode *node, GList **dialog)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_dialog_list(AgsFile *file, xmlNode *parent, GList *dialog)
{
  //TODO:JK: implement me
}

void
ags_file_read_machine_editor(AgsFile *file, xmlNode *node, AgsMachineEditor **machine_editor)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_machine_editor(AgsFile *file, xmlNode *parent, AgsMachineEditor *machine_editor)
{
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-machine-editor\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", machine_editor,
				   NULL));

  //TODO:JK: implement me

  xmlAddChild(parent,
	      node);  
}

void
ags_file_read_machine_editor_list(AgsFile *file, xmlNode *node, GList **machine_editor)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_machine_editor_list(AgsFile *file, xmlNode *parent, GList *machine_editor)
{
  //TODO:JK: implement me
}

void
ags_file_read_pad_editor(AgsFile *file, xmlNode *node, AgsPadEditor **pad_editor)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_pad_editor(AgsFile *file, xmlNode *parent, AgsPadEditor *pad_editor)
{
  //TODO:JK: implement me
}

void
ags_file_read_line_editor(AgsFile *file, xmlNode *node, AgsLineEditor **line_editor)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_line_editor(AgsFile *file, xmlNode *parent, AgsLineEditor *line_editor)
{
  //TODO:JK: implement me
}

void
ags_file_read_link_editor(AgsFile *file, xmlNode *node, AgsLinkEditor **link_editor)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_link_editor(AgsFile *file, xmlNode *parent, AgsLinkEditor *link_editor)
{
  //TODO:JK: implement me
}

void
ags_file_read_line_member_editor(AgsFile *file, xmlNode *node, AgsLineMemberEditor **line_member_editor)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_line_member_editor(AgsFile *file, xmlNode *parent, AgsLineMemberEditor *line_member_editor)
{
  //TODO:JK: implement me
}

void
ags_file_read_link_collection_editor(AgsFile *file, xmlNode *node, AgsLinkCollectionEditor **link_collection_editor)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_link_collection_editor(AgsFile *file, xmlNode *parent, AgsLinkCollectionEditor *link_collection_editor)
{
  //TODO:JK: implement me
}

void
ags_file_read_resize_editor(AgsFile *file, xmlNode *node, AgsResizeEditor **resize_editor)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_resize_editor(AgsFile *file, xmlNode *parent, AgsResizeEditor *resize_editor)
{
  //TODO:JK: implement me
}

void
ags_file_read_editor(AgsFile *file, xmlNode *node, AgsEditor **editor)
{
  AgsEditor *gobject;
  AgsFileLaunch *file_launch;
  xmlNode *child;

  if(*editor == NULL){
    gobject = (AgsEditor *) g_object_new(AGS_TYPE_EDITOR,
					 NULL);
    *editor = gobject;
  }else{
    gobject = *editor;
  }
  
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  gobject->version = g_strdup(xmlGetProp(node,
					 AGS_FILE_VERSION_PROP));

  gobject->build_id = g_strdup(xmlGetProp(node,
					  AGS_FILE_BUILD_ID_PROP));

  gobject->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-toolbar\0",
		     12)){
	ags_file_read_toolbar(file,
			      child,
			      &(gobject->toolbar));
      }else if(!xmlStrncmp(child->name,
			   "ags-machine-selector\0",
			   11)){
	ags_file_read_machine_selector(file,
				       child,
				       &(gobject->machine_selector));
      }else if(!xmlStrncmp(child->name,
			   "ags-notebook\0",
			   13)){
	ags_file_read_notebook(file,
			       child,
			       &(gobject->notebook));
      }
    }

    child = child->next;
  }

  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start\0",
		   G_CALLBACK(ags_file_read_editor_launch), gobject);
  ags_file_add_launch(file,
		      file_launch);
}

void
ags_file_read_editor_resolve_parameter(AgsFileLookup *file_lookup,
				       AgsEditor *editor)
{
  gchar *name;
  GValue *value;

  name = g_object_get_data(G_OBJECT(file_lookup),
			   AGS_FILE_READ_EDITOR_PARAMETER_NAME);
  value = file_lookup->ref;

  g_object_set_property(G_OBJECT(editor),
			name,
			value);
}

void
ags_file_read_editor_launch(AgsFileLaunch *file_launch,
			    AgsEditor *editor)
{
  AgsMachine *machine;
  AgsRuler *ruler;
  GList *list;
  guint tabs, pads;
  guint i;

  machine = editor->selected_machine;

  if(machine == NULL){
    return;
  }

  /* set tabs */
  tabs = machine->audio->audio_channels;

  for(i = 0; i < tabs; i++){
    ruler = ags_ruler_new();
    ags_notebook_add_tab(editor->notebook);

    if(GTK_WIDGET_VISIBLE(GTK_WIDGET(editor->notebook)))
      ags_ruler_connect(ruler);
  }

  list = editor->notebook->tabs;

  while(list != NULL){
    gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(list->data)->toggle,
				 TRUE);

    list = list->next;
  }

  /* map height */
  if((AGS_AUDIO_NOTATION_DEFAULT & (machine->audio->flags)) != 0){
    pads = machine->audio->input_pads;
  }else{
    pads = machine->audio->output_pads;
  }

  editor->note_edit->map_height = pads * editor->note_edit->control_height;

  gtk_widget_show_all(editor->notebook);

  editor->note_edit->flags |= AGS_NOTE_EDIT_RESETING_VERTICALLY;
  ags_note_edit_reset_vertically(editor->note_edit, AGS_NOTE_EDIT_RESET_VSCROLLBAR);
  editor->note_edit->flags &= (~AGS_NOTE_EDIT_RESETING_VERTICALLY);
}

xmlNode*
ags_file_write_editor(AgsFile *file, xmlNode *parent, AgsEditor *editor)
{
  AgsFileIdRef *id_ref;
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-editor\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", editor,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_VERSION_PROP,
	     editor->version);

  xmlNewProp(node,
	     AGS_FILE_BUILD_ID_PROP,
	     editor->build_id);

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", editor->flags));

  xmlAddChild(parent,
	      node);  

  /* child elements */
  ags_file_write_machine_selector(file, node, editor->machine_selector);
  ags_file_write_toolbar(file, node, editor->toolbar);
  ags_file_write_notebook(file, node, editor->notebook);
}

void
ags_file_read_toolbar(AgsFile *file, xmlNode *node, AgsToolbar **toolbar)
{
  AgsToolbar *gobject;
  GtkTreeIter iter;
  GtkTreeModel *model;
  xmlNode *child;
  gchar *value;
  gchar *str;

  if(*toolbar == NULL){
    gobject = (AgsToolbar *) g_object_new(AGS_TYPE_TOOLBAR,
					 NULL);
    *toolbar = gobject;
  }else{
    gobject = *toolbar;
  }
  
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  str = xmlGetProp(node,
		   "edit-mode\0");

  if(!g_strcmp0("position\0",
		str)){
    gtk_button_clicked(gobject->position);
  }else if(!g_strcmp0("edit\0",
		      str)){
    gtk_button_clicked(gobject->edit);
  }else if(!g_strcmp0("clear\0",
		      str)){
    gtk_button_clicked(gobject->clear);
  }else if(!g_strcmp0("select\0",
		      str)){
    gtk_button_clicked(gobject->select);
  }

  /* zoom */
  str = xmlGetProp(node,
		   "zoom\0");

  model = gtk_combo_box_get_model(gobject->zoom);

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);

      if(!g_strcmp0(str,
		    value)){
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));

    gtk_combo_box_set_active_iter(gobject->zoom,
				  &iter);
  }

  /* mode */
  str = xmlGetProp(node,
		   "mode\0");
  
  model = gtk_combo_box_get_model(gobject->mode);
  
  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);

      if(!g_strcmp0(str,
		    value)){
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));

    gtk_combo_box_set_active_iter(gobject->mode,
				  &iter);
  }
}

xmlNode*
ags_file_write_toolbar(AgsFile *file, xmlNode *parent, AgsToolbar *toolbar)
{
  AgsFileIdRef *id_ref;
  xmlNode *node;
  GList *list;
  guint n_properties, n_params;
  gchar *id;
  gint i;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-toolbar\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", toolbar,
				   NULL));

  if(toolbar->selected_edit_mode == toolbar->position){
    xmlNewProp(node,
	       "edit-mode\0",
	       g_strdup("position\0"));
  }else if(toolbar->selected_edit_mode == toolbar->edit){
    xmlNewProp(node,
	       "edit-mode\0",
	       g_strdup("edit\0"));
  }else if(toolbar->selected_edit_mode == toolbar->clear){
    xmlNewProp(node,
	       "edit-mode\0",
	       g_strdup("clear\0"));
  }else if(toolbar->selected_edit_mode == toolbar->select){
    xmlNewProp(node,
	       "edit-mode\0",
	       g_strdup("select\0"));
  }

  xmlNewProp(node,
	     "zoom\0",
	     g_strdup_printf("%s\0", gtk_combo_box_text_get_active_text(toolbar->zoom)));

  xmlNewProp(node,
	     "mode\0",
	     g_strdup_printf("%s\0", gtk_combo_box_text_get_active_text(toolbar->mode)));

  xmlAddChild(parent,
	      node);  
}

void
ags_file_read_machine_selector(AgsFile *file, xmlNode *node, AgsMachineSelector **machine_selector)
{
  AgsMachineSelector *gobject;
  GParameter *parameter;
  xmlNode *child;
  guint n_params;

  if(*machine_selector == NULL){
    gobject = g_object_new(AGS_TYPE_MACHINE_SELECTOR,
			   NULL);

    *machine_selector = gobject;
  }else{
    gobject = *machine_selector;
  }

  /* child elements */
  child = node->children;

  parameter = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-parameter\0",
		     14)){
	AgsFileLookup *file_lookup;
	xmlNode *value_node;
	GList *list;

	ags_file_util_read_parameter(file,
				     child,
				     NULL,
				     &parameter,
				     &n_params,
				     NULL);

	value_node = child->children;

	while(value_node != NULL){
	  if(value_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(value_node->name,
			   "ags-value\0",
			   10)){
	      list = ags_file_lookup_find_by_node(file->lookup,
						  value_node);
	  
	      if(list != NULL){
		file_lookup = AGS_FILE_LOOKUP(list->data);
		g_signal_connect_after(G_OBJECT(file_lookup), "resolve\0",
				       G_CALLBACK(ags_file_read_machine_selector_resolve_parameter), gobject);
	      }
	    }
	  }

	  value_node = value_node->next;
	}
      }
    }

    child = child->next;
  }
}

void
ags_file_read_machine_selector_resolve_parameter(AgsFileLookup *file_lookup,
						 AgsMachineSelector *machine_selector)
{
  AgsEditor *editor;
  GObject *gobject;
  GValue *value;

  value = file_lookup->ref;

  if(G_VALUE_HOLDS(value, G_TYPE_OBJECT)){
    AgsMachineRadioButton *machine_radio_button;

    gobject = g_value_get_object(value);

    if(gobject == NULL){
      return;
    }

    editor = gtk_widget_get_ancestor(machine_selector,
				     AGS_TYPE_EDITOR);

    if(editor->selected_machine == NULL){
      editor->selected_machine = gobject;
    }

    machine_radio_button = g_object_new(AGS_TYPE_MACHINE_RADIO_BUTTON,
					NULL);
    gtk_box_pack_start(GTK_BOX(machine_selector),
		       machine_radio_button,
		       FALSE, FALSE,
		       0);
    g_object_set(machine_radio_button,
		 "machine\0", gobject,
		 NULL);
  }
}

xmlNode*
ags_file_write_machine_selector(AgsFile *file, xmlNode *parent, AgsMachineSelector *machine_selector)
{
  xmlNode *node;
  GParameter *parameter;
  GList *list;
  gchar *id;
  gint n_params;

  auto GParameter* ags_file_write_machine_selector_parameter(GList *list, GParameter *parameter, gchar *prop, gint *n_params);

  GParameter* ags_file_write_machine_selector_parameter(GList *list, GParameter *parameter, gchar *prop, gint *n_params){
    gint i;

    if(n_params == NULL){
      i = 0;
    }else{
      i = *n_params;
    }

    while(list != NULL){
      if(!AGS_IS_MACHINE_RADIO_BUTTON(list->data)){
	list = list->next;
	continue;
      }

      if(parameter == NULL){
	parameter = (GParameter *) malloc(sizeof(GParameter));
      }else{
	parameter = (GParameter *) realloc(parameter,
					   (i + 1) * sizeof(GParameter));
      }

      parameter[i].name = prop;

      memset(&(parameter[i].value), 0, sizeof(GValue));
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value),
			 G_OBJECT(AGS_MACHINE_RADIO_BUTTON(list->data)->machine));

      list = list->next;
      i++;
    }

    if(n_params != NULL){
      *n_params = i;
    }

    return(parameter);
  }

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-machine-selector\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", machine_selector,
				   NULL));

  xmlAddChild(parent,
	      node);

  /* child elements */
  parameter = NULL;
  n_params = 0;

  list = gtk_container_get_children(GTK_CONTAINER(machine_selector));
  list = list->next;
  parameter = ags_file_write_machine_selector_parameter(list, parameter, "machine\0", &n_params);

  ags_file_util_write_parameter(file,
				node,
				ags_id_generator_create_uuid(),
				parameter, n_params);

  return(node);
}

void
ags_file_read_notebook(AgsFile *file, xmlNode *node, AgsNotebook **notebook)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_notebook(AgsFile *file, xmlNode *parent, AgsNotebook *notebook)
{
  //TODO:JK: implement me
}

void
ags_file_read_notebook_tab_list(AgsFile *file, xmlNode *node, GList **notebook_tab_list)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_notebook_tab_list(AgsFile *file, xmlNode *parent, GList *notebook_tab_list)
{
  //TODO:JK: implement me
}

void
ags_file_read_notebook_tab(AgsFile *file, xmlNode *node, AgsNotebookTab **notebook_tab)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_notebook_tab(AgsFile *file, xmlNode *parent, AgsNotebookTab *notebook_tab)
{
  //TODO:JK: implement me
}

void
ags_file_read_navigation(AgsFile *file, xmlNode *node, AgsNavigation **navigation)
{
  AgsNavigation *gobject;
  xmlNode *child;
  xmlChar *str;

  if(*navigation == NULL){
    gobject = g_object_new(AGS_TYPE_NAVIGATION,
			   NULL);

    *navigation = gobject;
  }else{
    gobject = *navigation;
  }

  str = xmlGetProp(node,
		   "expanded\0");

  if(!xmlStrncmp(str,
		 AGS_FILE_TRUE,
		 5)){
    gtk_toggle_button_set_active(gobject->expander,
				 TRUE);
  }

  str = xmlGetProp(node,
		   "bpm\0");
  gtk_spin_button_set_value(gobject->bpm,
			    g_strtod(str,
				     NULL));

  str = xmlGetProp(node,
		   "loop\0");

  if(!xmlStrncmp(str,
		 AGS_FILE_TRUE,
		 5)){
    gtk_toggle_button_set_active(gobject->loop,
				 TRUE);
  }

  str = xmlGetProp(node,
		   "position\0");
  gtk_spin_button_set_value(gobject->position_tact,
			    g_strtod(str,
				     NULL));

  str = xmlGetProp(node,
		   "loop-left\0");
  gtk_spin_button_set_value(gobject->loop_left_tact,
			    g_strtod(str,
				     NULL));

  str = xmlGetProp(node,
		   "loop-right\0");
  gtk_spin_button_set_value(gobject->loop_right_tact,
			    g_strtod(str,
				     NULL));
}

xmlNode*
ags_file_write_navigation(AgsFile *file, xmlNode *parent, AgsNavigation *navigation)
{
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-navigation\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", navigation,
				   NULL));

  xmlNewProp(node,
	     "expanded\0",
	     g_strdup_printf("%s\0", ((gtk_toggle_button_get_active(navigation->expander)) ? AGS_FILE_TRUE: AGS_FILE_FALSE)));
  
  xmlNewProp(node,
	     "bpm\0",
	     g_strdup_printf("%.f\0", gtk_spin_button_get_value(navigation->bpm)));
  
  xmlNewProp(node,
	     "loop\0",
	     g_strdup_printf("%s\0", ((gtk_toggle_button_get_active(navigation->loop)) ? AGS_FILE_TRUE: AGS_FILE_FALSE)));
 
  xmlNewProp(node,
	     "position\0",
	     g_strdup_printf("%.3f\0", gtk_spin_button_get_value(navigation->position_tact)));

  xmlNewProp(node,
	     "loop-left\0",
	     g_strdup_printf("%.3f\0", gtk_spin_button_get_value(navigation->loop_left_tact)));

  xmlNewProp(node,
	     "loop-right\0",
	     g_strdup_printf("%.3f\0", gtk_spin_button_get_value(navigation->loop_right_tact)));

  xmlAddChild(parent,
	      node);  
}

