/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/ags_machine_editor_bulk.h>
#include <ags/app/ags_machine_editor_bulk_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_machine_editor.h>

#include <ags/i18n.h>

void ags_machine_editor_bulk_class_init(AgsMachineEditorBulkClass *machine_editor_bulk);
void ags_machine_editor_bulk_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_editor_bulk_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_machine_editor_bulk_init(AgsMachineEditorBulk *machine_editor_bulk);
void ags_machine_editor_bulk_dispose(GObject *gobject);

void ags_machine_editor_bulk_connect(AgsConnectable *connectable);
void ags_machine_editor_bulk_disconnect(AgsConnectable *connectable);

void ags_machine_editor_bulk_set_update(AgsApplicable *applicable, gboolean update);
void ags_machine_editor_bulk_apply(AgsApplicable *applicable);
void ags_machine_editor_bulk_reset(AgsApplicable *applicable);

static gpointer ags_machine_editor_bulk_parent_class = NULL;

/**
 * SECTION:ags_machine_editor_bulk
 * @short_description: Edit audio related aspects
 * @title: AgsMachineEditorBulk
 * @section_id:
 * @include: ags/app/ags_machine_editor_bulk.h
 *
 * #AgsMachineEditorBulk is a composite widget to edit all aspects of #AgsAudio.
 * It consists of multiple child editor bulks.
 */

GType
ags_machine_editor_bulk_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_machine_editor_bulk = 0;

    static const GTypeInfo ags_machine_editor_bulk_info = {
      sizeof (AgsMachineEditorBulkClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_editor_bulk_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineEditorBulk),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_editor_bulk_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_editor_bulk_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_machine_editor_bulk_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine_editor_bulk = g_type_register_static(GTK_TYPE_BOX,
							  "AgsMachineEditorBulk", &ags_machine_editor_bulk_info,
							  0);

    g_type_add_interface_static(ags_type_machine_editor_bulk,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_machine_editor_bulk,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_machine_editor_bulk);
  }

  return g_define_type_id__volatile;
}

void
ags_machine_editor_bulk_class_init(AgsMachineEditorBulkClass *machine_editor_bulk)
{
  GObjectClass *gobject;

  ags_machine_editor_bulk_parent_class = g_type_class_peek_parent(machine_editor_bulk);

  /* GObjectClass */
  gobject = (GObjectClass *) machine_editor_bulk;

  gobject->dispose = ags_machine_editor_bulk_dispose;
}

void
ags_machine_editor_bulk_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_editor_bulk_connect;
  connectable->disconnect = ags_machine_editor_bulk_disconnect;
}

void
ags_machine_editor_bulk_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_machine_editor_bulk_set_update;
  applicable->apply = ags_machine_editor_bulk_apply;
  applicable->reset = ags_machine_editor_bulk_reset;
}

void
ags_machine_editor_bulk_init(AgsMachineEditorBulk *machine_editor_bulk)
{
  GtkGrid *grid;
  GtkLabel *label;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(machine_editor_bulk),
				 GTK_ORIENTATION_VERTICAL);

  gtk_box_set_spacing(machine_editor_bulk,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  machine_editor_bulk->connectable_flags = 0;

  grid = gtk_grid_new();

  gtk_widget_set_valign((GtkWidget *) grid,
			GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) grid,
			GTK_ALIGN_START);

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append((GtkBox *) machine_editor_bulk,
		 (GtkWidget *) grid);

  label = gtk_label_new(i18n("link"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  machine_editor_bulk->link = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_grid_attach(grid,
		  (GtkWidget *) machine_editor_bulk->link,
		  1, 0,
		  1, 1);

  label = gtk_label_new(i18n("first line"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  machine_editor_bulk->first_line = (GtkComboBox *) gtk_spin_button_new_with_range(0.0,
										   0.0,
										   1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) machine_editor_bulk->first_line,
		  1, 1,
		  1, 1);

  label = gtk_label_new(i18n("first link line"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  machine_editor_bulk->first_link_line = (GtkComboBox *) gtk_spin_button_new_with_range(0.0,
											0.0,
											1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) machine_editor_bulk->first_link_line,
		  1, 2,
		  1, 1);

  label = gtk_label_new(i18n("count"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  machine_editor_bulk->count = (GtkComboBox *) gtk_spin_button_new_with_range(0.0,
									      0.0,
									      1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) machine_editor_bulk->count,
		  1, 3,
		  1, 1);

  machine_editor_bulk->remove_bulk = gtk_button_new_from_icon_name("list-remove-symbolic");
  gtk_grid_attach(grid,
		  (GtkWidget *) machine_editor_bulk->remove_bulk,
		  3, 3,
		  1, 1);
}

void
ags_machine_editor_bulk_dispose(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_machine_editor_bulk_parent_class)->dispose(gobject);
}

void
ags_machine_editor_bulk_connect(AgsConnectable *connectable)
{
  AgsMachineEditorBulk *machine_editor_bulk;

  machine_editor_bulk = AGS_MACHINE_EDITOR_BULK(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (machine_editor_bulk->connectable_flags)) != 0){
    return;
  }

  machine_editor_bulk->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect_after(machine_editor_bulk->link, "changed",
			 G_CALLBACK(ags_machine_editor_bulk_link_callback), machine_editor_bulk);

  g_signal_connect(machine_editor_bulk->remove_bulk, "clicked",
		   G_CALLBACK(ags_machine_editor_bulk_remove_bulk_callback), machine_editor_bulk);
}

void
ags_machine_editor_bulk_disconnect(AgsConnectable *connectable)
{
  AgsMachineEditorBulk *machine_editor_bulk;

  machine_editor_bulk = AGS_MACHINE_EDITOR_BULK(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (machine_editor_bulk->connectable_flags)) == 0){
    return;
  }
  
  machine_editor_bulk->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(machine_editor_bulk->link,
		      "any_signal::changed",
		      G_CALLBACK(ags_machine_editor_bulk_link_callback),
		      machine_editor_bulk,
		      NULL);

  g_object_disconnect(machine_editor_bulk->remove_bulk,
		      "any_signal::clicked",
		      G_CALLBACK(ags_machine_editor_bulk_remove_bulk_callback),
		      machine_editor_bulk,
		      NULL);
}

void
ags_machine_editor_bulk_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsMachineEditorBulk *machine_editor_bulk;

  machine_editor_bulk = AGS_MACHINE_EDITOR_BULK(applicable);

  //TODO:JK: implement me
}

void
ags_machine_editor_bulk_apply(AgsApplicable *applicable)
{
  AgsMachine *machine;
  AgsMachine *link_machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorCollection *machine_editor_collection;
  AgsMachineEditorBulk *machine_editor_bulk;

  GtkTreeModel *model;

  GtkTreeIter iter;
  
  AgsChannel *start_input, *start_output;
  AgsChannel *channel;
  AgsChannel *link;
  
  AgsApplicationContext *application_context;

  xmlNode *node;

  GList *start_dialog_model, *dialog_model;
  GList *start_list, *list;
  
  guint first_line, first_link_line;
  guint count;
  gint i, i_stop;
  
  machine_editor_bulk = AGS_MACHINE_EDITOR_BULK(applicable);

  machine_editor_collection = (AgsMachineEditorCollection *) gtk_widget_get_ancestor(machine_editor_bulk,
										     AGS_TYPE_MACHINE_EDITOR_COLLECTION);

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor(machine_editor_bulk,
								AGS_TYPE_MACHINE_EDITOR);

  machine = machine_editor->machine;

  /* application context */  
  application_context = ags_application_context_get_instance();

  model = gtk_combo_box_get_model(machine_editor_bulk->link);  
  
  gtk_combo_box_get_active_iter(machine_editor_bulk->link,
				&iter);

  link_machine = NULL;

  gtk_tree_model_get(model,
		     &iter,
		     1, &link_machine,
		     -1);

  start_output = NULL;
  start_input = NULL;

  channel = NULL;

  link = NULL;
  
  first_line = (guint) gtk_spin_button_get_value_as_int(machine_editor_bulk->first_line);
  
  first_link_line = (guint) gtk_spin_button_get_value_as_int(machine_editor_bulk->first_link_line);
  
  count = (guint) gtk_spin_button_get_value_as_int(machine_editor_bulk->count);

  if(g_type_is_a(machine_editor_collection->channel_type, AGS_TYPE_OUTPUT)){
    start_output = ags_audio_get_output(machine->audio);

    channel = ags_channel_pad_nth(start_output,
				  first_line);
    
    start_input = NULL;
    
    if(link_machine != NULL){
      start_input = ags_audio_get_input(link_machine->audio);
    }

    link = ags_channel_pad_nth(start_input,
			       first_link_line);
  }else{
    start_output = NULL;
    
    if(link_machine != NULL){
      start_output = ags_audio_get_output(link_machine->audio);
    }

    link = ags_channel_pad_nth(start_output,
			       first_link_line);
    
    start_input = ags_audio_get_input(machine->audio);

    channel = ags_channel_pad_nth(start_input,
				  first_line);
  }

  for(i = 0; i < count; i++){
    AgsLinkChannel *link_channel;

    AgsChannel *next_pad;
    
    /* create task */
    link_channel = ags_link_channel_new(channel,
					link);
    
    /* append AgsLinkChannel */
    ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				  (AgsTask *) link_channel);

    /* iterate */
    next_pad = ags_channel_next_pad(channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_pad;

    next_pad = ags_channel_next_pad(link);

    if(link != NULL){
      g_object_unref(link);
    }
    
    link = next_pad;
  }

  list = 
    start_list = ags_machine_editor_collection_get_bulk(machine_editor_collection);

  i_stop = g_list_index(start_list,
			machine_editor_bulk);
  
  dialog_model =
    start_dialog_model = ags_machine_get_dialog_model(machine);
  
  node = NULL;

  for(i = 0; dialog_model != NULL && i < i_stop; i++){
    if(g_type_is_a(machine_editor_collection->channel_type, AGS_TYPE_OUTPUT)){
      dialog_model = ags_machine_find_dialog_model(machine,
						   dialog_model,
						   "ags-machine-editor-bulk",
						   "direction",
						   "output");
    }else if(g_type_is_a(machine_editor_collection->channel_type, AGS_TYPE_INPUT)){
      dialog_model = ags_machine_find_dialog_model(machine,
						   dialog_model,
						   "ags-machine-editor-bulk",
						   "direction",
						   "input");
    }

    if(dialog_model != NULL){
      if(i == i_stop){
	node = dialog_model->data;

	break;      
      }
    
      dialog_model = dialog_model->next;
    }
  }

  g_list_free(start_dialog_model);

  if(node != NULL){
    ags_machine_remove_dialog_model(machine,
				    node);
  }

  node = ags_machine_editor_bulk_to_xml_node(machine_editor_bulk);
  ags_machine_add_dialog_model(machine,
			       node);
  
  if(start_output){
    g_object_unref(start_output);
  }

  if(start_input){
    g_object_unref(start_input);
  }
}

void
ags_machine_editor_bulk_reset(AgsApplicable *applicable)
{
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorCollection *machine_editor_collection;
  AgsMachineEditorBulk *machine_editor_bulk;

  GtkListStore *list_store;
  
  xmlNode *node;

  GList *start_dialog_model, *dialog_model;
  GList *start_list, *list;
  
  gint i, i_stop;
  
  machine_editor_bulk = AGS_MACHINE_EDITOR_BULK(applicable);  

  machine_editor_collection = (AgsMachineEditorCollection *) gtk_widget_get_ancestor(machine_editor_bulk,
										     AGS_TYPE_MACHINE_EDITOR_COLLECTION);

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor(machine_editor_bulk,
								AGS_TYPE_MACHINE_EDITOR);

  machine = machine_editor->machine;
  
  list_store = GTK_LIST_STORE(gtk_combo_box_get_model(machine_editor_bulk->link));
  
  gtk_list_store_clear(list_store);

  list_store = ags_machine_get_possible_links(machine);

  gtk_combo_box_set_model(machine_editor_bulk->link,
			  GTK_TREE_MODEL(list_store));

  list = 
    start_list = ags_machine_editor_collection_get_bulk(machine_editor_collection);

  i_stop = g_list_index(start_list,
			machine_editor_bulk);
  
  dialog_model =
    start_dialog_model = ags_machine_get_dialog_model(machine);
  
  node = NULL;

  for(i = 0; dialog_model != NULL && i <= i_stop; i++){
    if(g_type_is_a(machine_editor_collection->channel_type, AGS_TYPE_OUTPUT)){
      dialog_model = ags_machine_find_dialog_model(machine,
						   dialog_model,
						   "ags-machine-editor-bulk",
						   "direction",
						   "output");
    }else{
      dialog_model = ags_machine_find_dialog_model(machine,
						   dialog_model,
						   "ags-machine-editor-bulk",
						   "direction",
						   "input");
    }

    if(dialog_model != NULL){
      if(i == i_stop){
	node = dialog_model->data;
      }
    
      dialog_model = dialog_model->next;
    }
  }
  
  g_list_free(start_dialog_model);
  
  g_list_free(start_list);

  if(node != NULL){
    ags_machine_editor_bulk_from_xml_node(machine_editor_bulk,
					  node);
  }
}

/**
 * ags_machine_editor_bulk_to_xml_node:
 * @machine_editor_bulk: the #AgsMachineEditorBulk
 * 
 * Serialize @machine_editor_bulk to #xmlNode-struct.
 * 
 * Returns: the serialized #xmlNode-struct
 * 
 * Since: 4.0.0
 */
xmlNode*
ags_machine_editor_bulk_to_xml_node(AgsMachineEditorBulk *machine_editor_bulk)
{
  AgsMachineEditorCollection *machine_editor_collection;

  GtkTreeModel *model;

  xmlNode *node;
  xmlNode *child;

  GtkTreeIter iter;

  gchar *link;
  gchar *value;
  
  g_return_val_if_fail(AGS_IS_MACHINE_EDITOR_BULK(machine_editor_bulk), NULL);

  machine_editor_collection = (AgsMachineEditorCollection *) gtk_widget_get_ancestor(machine_editor_bulk,
										     AGS_TYPE_MACHINE_EDITOR_COLLECTION);

  node = NULL;
  
  if(g_type_is_a(machine_editor_collection->channel_type, AGS_TYPE_OUTPUT)){
    node = xmlNewNode(NULL,
		      BAD_CAST "ags-machine-editor-bulk");
    
    xmlNewProp(node,
	       "direction",
	       "output");
  }else if(g_type_is_a(machine_editor_collection->channel_type, AGS_TYPE_INPUT)){
    node = xmlNewNode(NULL,
		      BAD_CAST "ags-machine-editor-bulk");

    xmlNewProp(node,
	       "direction",
	       "input");
  }else{
    return(NULL);
  }

  /* link */
  child = xmlNewNode(NULL,
		     "link");

  model = gtk_combo_box_get_model(machine_editor_bulk->link);

  link = NULL;
  
  gtk_combo_box_get_active_iter(machine_editor_bulk->link,
				&iter);
  gtk_tree_model_get(model,
		     &iter,
		     0, &link,
		     -1);
  
  xmlNodeAddContent(child,
		    link);
  
  xmlAddChild(node,
	      child);

  /* first line */
  child = xmlNewNode(NULL,
		     "link-line");

  value = g_strdup_printf("%d",
			  gtk_spin_button_get_value_as_int(machine_editor_bulk->first_line));
  
  xmlNewProp(child,
	     "value",
	     value);
  
  xmlAddChild(node,
	      child);

  /* first link line */
  child = xmlNewNode(NULL,
		     "first-link-line");

  value = g_strdup_printf("%d",
			  gtk_spin_button_get_value_as_int(machine_editor_bulk->first_link_line));
  
  xmlNewProp(child,
	     "value",
	     value);
  
  xmlAddChild(node,
	      child);

  /* count */
  child = xmlNewNode(NULL,
		     "count");

  value = g_strdup_printf("%d",
			  gtk_spin_button_get_value_as_int(machine_editor_bulk->count));
  
  xmlNewProp(child,
	     "value",
	     value);
  
  xmlAddChild(node,
	      child);

  return(node);
}

/**
 * ags_machine_editor_bulk_from_xml_node:
 * @machine_editor_bulk: the #AgsMachineEditorBulk
 * @node: the #xmlNode-struct
 * 
 * Parse @node and apply to @machine_editor_bulk.
 * 
 * Since: 4.0.0
 */
void
ags_machine_editor_bulk_from_xml_node(AgsMachineEditorBulk *machine_editor_bulk,
				      xmlNode *node)
{
  xmlNode *child;

  g_return_if_fail(AGS_IS_MACHINE_EDITOR_BULK(machine_editor_bulk));
  g_return_if_fail(node != NULL);

  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(BAD_CAST "link",
		     child->name,
		     5)){
	GtkTreeModel *model;

	GtkTreeIter iter;
	
	xmlChar *link;

	link = xmlNodeGetContent(child);

	model = gtk_combo_box_get_model(machine_editor_bulk->link);

	if(gtk_tree_model_get_iter_first(model, &iter)){
	  do{
	    gchar *current_link;
	    
	    gtk_tree_model_get(model,
			       &iter,
			       0, &current_link,
			       -1);

	    if(!g_strcmp0(link,
			  current_link)){
	      break;
	    }
	  }while(gtk_tree_model_iter_next(model, &iter));
	}

	gtk_combo_box_set_active_iter(machine_editor_bulk->link,
				      &iter);
      }else if(!xmlStrncmp(BAD_CAST "first-line",
			   child->name,
			   12)){
	xmlChar *first_line;

	first_line = xmlGetProp(child,
				BAD_CAST "value");

	gtk_spin_button_set_value(machine_editor_bulk->first_line,
				  g_ascii_strtod(first_line,
						 NULL));
      }else if(!xmlStrncmp(BAD_CAST "first-link-line",
			   child->name,
			   18)){
	xmlChar *first_link_line;

	first_link_line = xmlGetProp(child,
				     BAD_CAST "value");

	gtk_spin_button_set_value(machine_editor_bulk->first_link_line,
				  g_ascii_strtod(first_link_line,
						 NULL));
      }else if(!xmlStrncmp(BAD_CAST "count",
			   child->name,
			   6)){
	xmlChar *count;

	count = xmlGetProp(child,
			   BAD_CAST "value");

	gtk_spin_button_set_value(machine_editor_bulk->count,
				  g_ascii_strtod(count,
						 NULL));
      }
    }

    child = child->next;
  }
}

/**
 * ags_machine_editor_bulk_new:
 *
 * Creates an #AgsMachineEditorBulk
 *
 * Returns: a new #AgsMachineEditorBulk
 *
 * Since: 4.0.0
 */
AgsMachineEditorBulk*
ags_machine_editor_bulk_new()
{
  AgsMachineEditorBulk *machine_editor_bulk;

  machine_editor_bulk = (AgsMachineEditorBulk *) g_object_new(AGS_TYPE_MACHINE_EDITOR_BULK,
							      NULL);

  return(machine_editor_bulk);
}
