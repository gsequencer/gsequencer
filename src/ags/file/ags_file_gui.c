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

#include <ags/main.h>

#include <ags/plugin/ags_plugin_factory.h>

#include <ags/lib/ags_container.h>

#include <ags/object/ags_plugin.h>

#include <ags/util/ags_id_generator.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

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

void ags_file_read_line_member_resolve_port(AgsFileLookup *file_lookup,
					    AgsLineMember *line_member);
void ags_file_write_line_member_resolve_port(AgsFileLookup *file_lookup,
					     AgsLineMember *line_member);

void
ags_file_read_window(AgsFile *file, xmlNode *node, AgsWindow **window)
{
  AgsWindow *gobject;
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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  gobject->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  gobject->name = (gchar *) g_strdup(xmlGetProp(node, "name\0"));

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

	ags_container_add_all(GTK_CONTAINER(gobject->machines),
			      list);
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

xmlNode*
ags_file_write_window(AgsFile *file, xmlNode *parent, AgsWindow *window)
{
  xmlNode *node, *child;
  gchar *id;
  guint i;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-window\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
				   "reference\0", window,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", window->flags));

  xmlNewProp(node,
	     "name\0",
	     g_strdup(window->name));

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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
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

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
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
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", gobject,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_read_machine_resolve_machine_editor), gobject);

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
	GList *pad;

	pad = NULL;

	ags_file_read_pad_list(file,
			       child,
			       &pad);

	if(!xmlStrncmp(xmlGetProp(child,
				  AGS_FILE_SCOPE_PROP),
		       "output\0",
		       6)){
	  ags_container_add_all(gobject->output,
				pad);
	}else{
	  ags_container_add_all(gobject->input,
				pad);
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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
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

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, machine->audio);

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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
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
  
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
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
	     g_strdup_printf("%x\0", pad->flags));

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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
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
	     g_strdup_printf("%x\0", line->flags));

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

    line_member = line_member->next;
    line_member_node = line_member_node->next;
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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
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
  GtkWidget *child_widget;
  xmlNode *child;
  xmlChar *prop, *content;
  gchar *widget_type;
  gchar *task_type;
  static gboolean widget_type_is_registered = FALSE;
  
  if(*line_member == NULL){
    gobject = g_object_new(AGS_TYPE_LINE_MEMBER,
			   NULL);
    *line_member = gobject;
  }else{
    gobject = *line_member;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  if(!widget_type_is_registered){
    ags_main_register_widget_type();

    widget_type_is_registered = TRUE;
  }

  widget_type = (gchar *) xmlGetProp(node, "widget-type\0");
  child_widget = (GtkWidget *) g_object_new(g_type_from_name(widget_type),
					    NULL);

  gtk_container_add(GTK_CONTAINER(gobject),
		    child_widget);

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

  g_object_set(G_OBJECT(line_member),
	       "port\0", (AgsPort *) id_ref->ref,
	       NULL);
}

xmlNode*
ags_file_write_line_member(AgsFile *file, xmlNode *parent, AgsLineMember *line_member)
{
  AgsFileLookup *file_lookup;
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-line-member\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
				   "reference\0", line_member,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", line_member->flags));

  xmlNewProp(node,
	     "widget-type\0",
	     g_type_name(line_member->widget_type));

  if(line_member->task_type != G_TYPE_NONE){
    xmlNewProp(node,
	       AGS_FILE_FLAGS_PROP,
	       g_strdup_printf("%s\0", g_type_name(line_member->task_type)));
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

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, line_member->port);

  id = xmlGetProp(id_ref->node, AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     "port\0",
	     g_strdup_printf("xpath=//ags-port[@id='%s']\0", id));
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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = line_member;

  while(list != NULL){
    ags_file_write_line_member(file, node, AGS_LINE_MEMBER(list->data));

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
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//[@id='%s']\0", id),
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
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_editor(AgsFile *file, xmlNode *parent, AgsEditor *editor)
{
  //TODO:JK: implement me
}

void
ags_file_read_toolbar(AgsFile *file, xmlNode *node, AgsToolbar **toolbar)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_toolbar(AgsFile *file, xmlNode *parent, AgsToolbar *toolbar)
{
  //TODO:JK: implement me
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
ags_file_read_navigation(AgsFile *file, xmlNode *node, AgsNavigation **navigation)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_navigation(AgsFile *file, xmlNode *parent, AgsNavigation *navigation)
{
  //TODO:JK: implement me
}

