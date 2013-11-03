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
	       "main\0", file->main,
	       NULL);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
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

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", window,
				   NULL));
  
  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-window\0");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

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
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
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

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", menu_bar,
				   NULL));
  
  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-menu-bar\0");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

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
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
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

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", machine_counter,
				   NULL));
  
  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-machine-counter\0");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

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
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
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

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));

  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-machine-counter-list\0");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

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
  AgsMachine *gobject;
  AgsFileLookup *file_lookup;
  GType machine_type;
  xmlNode *child;

  if(*machine == NULL){
    machine_type = g_type_from_name(xmlGetProp(node,
					       AGS_FILE_TYPE_PROP));

    gobject = (AgsMachine *) g_object_new(machine_type,
					  NULL);
    *machine = gobject;
  }else{
    gobject = *machine;
  }

  g_object_set(G_OBJECT(gobject),
	       "main\0", file->main,
	       NULL);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
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
  //TODO:JK: implement me
}

void
ags_file_read_machine_resolve_machine_editor(AgsFileLookup *file_lookup,
					     AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_file_read_machine_resolve_rename_dialog(AgsFileLookup *file_lookup,
					    AgsMachine *machine)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_machine(AgsFile *file, xmlNode *parent, AgsMachine *machine)
{
  AgsFileLookup *file_lookup;
  xmlNode *node, *child;
  gchar *id;

  id = ags_id_generator_create_uuid();

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", machine,
				   NULL));
  
  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-machine\0");

  xmlNewProp(node,
	     AGS_FILE_TYPE_PROP,
	     G_OBJECT_TYPE_NAME(machine));

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

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
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", machine,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_write_machine_resolve_machine_editor), machine);

  /* rename-dialog */
  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", machine,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_file_write_machine_resolve_rename_dialog), machine);

  /* child elements */
  ags_plugin_write(file,
		   node,
		   AGS_PLUGIN(machine));

  child = ags_file_write_pad_list(file,
				  node,
				  gtk_container_get_children(machine->output));
  xmlNewProp(child,
	     AGS_FILE_SCOPE_PROP,
	     "output\0");

  child = ags_file_write_pad_list(file,
				  node,
				  gtk_container_get_children(machine->input));
  xmlNewProp(child,
	     AGS_FILE_SCOPE_PROP,
	     "input\0");
}

void
ags_file_write_machine_resolve_machine_editor(AgsFileLookup *file_lookup,
					      AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_file_write_machine_resolve_rename_dialog(AgsFileLookup *file_lookup,
					     AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_file_write_machine_resolve_audio(AgsFileLookup *file_lookup,
				     AgsMachine *machine)
{
  //TODO:JK: implement me
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
    current = NULL;
    ags_file_read_machine(file, child, &current);

    list = g_list_prepend(list, current);

    child = child->next;
  }

  list = g_list_reverse(list);
  *machine = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
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

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->main,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=*/[@id='%s']\0", id),
				   "reference\0", list,
				   NULL));

  node = xmlNewNode(AGS_FILE_DEFAULT_NS,
		    "ags-machine-counter-list\0");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

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
ags_file_read_panel(AgsFile *file, xmlNode *node, AgsMachine *panel)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_panel(AgsFile *file, xmlNode *parent, AgsMachine *panel)
{
  //TODO:JK: implement me
}

void
ags_file_read_mixer(AgsFile *file, xmlNode *node, AgsMachine *mixer)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_mixer(AgsFile *file, xmlNode *parent, AgsMachine *mixer)
{
  //TODO:JK: implement me
}

void
ags_file_read_drum(AgsFile *file, xmlNode *node, AgsMachine *drum)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_drum(AgsFile *file, xmlNode *parent, AgsMachine *drum)
{
  //TODO:JK: implement me
}

void
ags_file_read_matrix(AgsFile *file, xmlNode *node, AgsMachine *matrix)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_matrix(AgsFile *file, xmlNode *parent, AgsMachine *matrix)
{
  //TODO:JK: implement me
}

void
ags_file_read_synth(AgsFile *file, xmlNode *node, AgsMachine *synth)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_synth(AgsFile *file, xmlNode *parent, AgsMachine *synth)
{
  //TODO:JK: implement me
}

void
ags_file_read_ffplayer(AgsFile *file, xmlNode *node, AgsMachine *ffplayer)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_ffplayer(AgsFile *file, xmlNode *parent, AgsMachine *ffplayer)
{
  //TODO:JK: implement me
}

void
ags_file_read_pad(AgsFile *file, xmlNode *node, AgsPad **pad)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_pad(AgsFile *file, xmlNode *parent, AgsPad *pad)
{
  //TODO:JK: implement me
}

void
ags_file_read_pad_list(AgsFile *file, xmlNode *node, GList **pad)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_pad_list(AgsFile *file, xmlNode *parent, GList *pad)
{
  //TODO:JK: implement me
}

void
ags_file_read_mixer_input_pad(AgsFile *file, xmlNode *node, AgsPad *mixer_input_pad)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_mixer_input_pad(AgsFile *file, xmlNode *parent, AgsPad *mixer_input_pad)
{
  //TODO:JK: implement me
}

void
ags_file_read_mixer_output_pad(AgsFile *file, xmlNode *node, AgsPad *mixer_output_pad)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_mixer_output_pad(AgsFile *file, xmlNode *parent, AgsPad *mixer_output_pad)
{
  //TODO:JK: implement me
}

void
ags_file_read_drum_input_pad(AgsFile *file, xmlNode *node, AgsPad *drum_input_pad)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_drum_input_pad(AgsFile *file, xmlNode *parent, AgsPad *drum_input_pad)
{
  //TODO:JK: implement me
}

void
ags_file_read_drum_output_pad(AgsFile *file, xmlNode *node, AgsPad *drum_output_pad)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_drum_output_pad(AgsFile *file, xmlNode *parent, AgsPad *drum_output_pad)
{
  //TODO:JK: implement me
}

void
ags_file_read_line(AgsFile *file, xmlNode *node, AgsLine **line)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_line(AgsFile *file, xmlNode *parent, AgsLine *line)
{
  //TODO:JK: implement me
}

void
ags_file_read_line_list(AgsFile *file, xmlNode *node, GList **line)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_line_list(AgsFile *file, xmlNode *parent, GList *line)
{
  //TODO:JK: implement me
}

void
ags_file_read_drum_input_line(AgsFile *file, xmlNode *node, AgsLine *drum_output_line)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_drum_input_line(AgsFile *file, xmlNode *parent, AgsLine *drum_output_line)
{
  //TODO:JK: implement me
}

void
ags_file_read_drum_output_line(AgsFile *file, xmlNode *node, AgsLine *drum_output_line)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_drum_output_line(AgsFile *file, xmlNode *parent, AgsLine *drum_output_line)
{
  //TODO:JK: implement me
}

void
ags_file_read_oscillator(AgsFile *file, xmlNode *node, AgsOscillator *oscillator)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_oscillator(AgsFile *file, xmlNode *parent, AgsOscillator *oscillator)
{
  //TODO:JK: implement me
}

void
ags_file_read_line_member(AgsFile *file, xmlNode *node, AgsLineMember **line_member)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_line_member(AgsFile *file, xmlNode *parent, AgsLineMember *line_member)
{
  //TODO:JK: implement me
}

void
ags_file_read_line_member_list(AgsFile *file, xmlNode *node, GList **line_member)
{
  //TODO:JK: implement me
}

xmlNode*
ags_file_write_line_member_list(AgsFile *file, xmlNode *parent, GList *line_member)
{
  //TODO:JK: implement me
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
  //TODO:JK: implement me
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

