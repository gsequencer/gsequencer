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

#include <ags/app/editor/ags_preset_editor.h>
#include <ags/app/editor/ags_preset_editor_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_preset_dialog.h>

#include <libxml/tree.h>

#include <ags/i18n.h>

void ags_preset_editor_class_init(AgsPresetEditorClass *preset_editor);
void ags_preset_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_preset_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_preset_editor_init(AgsPresetEditor *preset_editor);
void ags_preset_editor_connect(AgsConnectable *connectable);
void ags_preset_editor_disconnect(AgsConnectable *connectable);
void ags_preset_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_preset_editor_apply(AgsApplicable *applicable);
void ags_preset_editor_reset(AgsApplicable *applicable);


/**
 * SECTION:ags_preset_editor
 * @short_description: Edit preset of notes
 * @title: AgsPresetEditor
 * @section_id:
 * @include: ags/app/ags_preset_editor.h
 *
 * #AgsPresetEditor is a composite widget to edit preset controls
 * of selected #AgsNote.
 */

static gpointer ags_preset_editor_parent_class = NULL;

GType
ags_preset_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_preset_editor = 0;

    static const GTypeInfo ags_preset_editor_info = {
      sizeof (AgsPresetEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_preset_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPresetEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_preset_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_preset_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_preset_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_preset_editor = g_type_register_static(GTK_TYPE_BOX,
						    "AgsPresetEditor", &ags_preset_editor_info,
						    0);

    g_type_add_interface_static(ags_type_preset_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_preset_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_preset_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_preset_editor_class_init(AgsPresetEditorClass *preset_editor)
{
  ags_preset_editor_parent_class = g_type_class_peek_parent(preset_editor);
}

void
ags_preset_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_preset_editor_connect;
  connectable->disconnect = ags_preset_editor_disconnect;
}

void
ags_preset_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_preset_editor_set_update;
  applicable->apply = ags_preset_editor_apply;
  applicable->reset = ags_preset_editor_reset;
}

void
ags_preset_editor_init(AgsPresetEditor *preset_editor)
{
  GtkBox *hbox;
  GtkLabel *label;
  GtkTreeViewColumn *context_column;
  GtkTreeViewColumn *line_column;
  GtkTreeViewColumn *control_column;
  GtkTreeViewColumn *range_column;
  GtkTreeViewColumn *value_column;
  GtkScrolledWindow *scrolled_window;

  GtkCellRenderer *context_renderer;
  GtkCellRenderer *line_renderer;
  GtkCellRenderer *control_renderer;
  GtkCellRenderer *range_renderer;
  GtkCellRenderer *value_renderer;

  GtkListStore *preset_list_store;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(preset_editor),
				 GTK_ORIENTATION_VERTICAL);

  gtk_box_set_spacing(preset_editor,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  preset_editor->flags = 0;
  preset_editor->connectable_flags = 0;
  
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append((GtkBox *) preset_editor,
		 (GtkWidget *) hbox);

  label = (GtkLabel *) gtk_label_new(i18n("filename"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  preset_editor->filename = (GtkEntry *) gtk_entry_new();
  gtk_box_append(hbox,
		 (GtkWidget *) preset_editor->filename);

  preset_editor->load = (GtkButton *) gtk_button_new_from_icon_name("system-run");

  gtk_widget_set_has_tooltip(preset_editor->load,
			     TRUE);
  gtk_widget_set_tooltip_text(preset_editor->load,
			      i18n("load controls from current"));
  
  gtk_box_append(hbox,
		 (GtkWidget *) preset_editor->load);

  preset_editor->open_preset = (GtkButton *) gtk_button_new_from_icon_name("document-open");

  gtk_widget_set_has_tooltip(preset_editor->open_preset,
			     TRUE);
  gtk_widget_set_tooltip_text(preset_editor->open_preset,
			      i18n("open preset"));

  gtk_box_append(hbox,
		 (GtkWidget *) preset_editor->open_preset);

  preset_editor->save_preset = (GtkButton *) gtk_button_new_from_icon_name("document-save");

  gtk_widget_set_has_tooltip(preset_editor->save_preset,
			     TRUE);
  gtk_widget_set_tooltip_text(preset_editor->save_preset,
			      i18n("save preset"));

  gtk_box_append(hbox,
		 (GtkWidget *) preset_editor->save_preset);

  preset_editor->apply_preset = (GtkButton *) gtk_button_new_from_icon_name("task-due");

  gtk_widget_set_has_tooltip(preset_editor->apply_preset,
			     TRUE);
  gtk_widget_set_tooltip_text(preset_editor->apply_preset,
			      i18n("apply preset"));

  gtk_box_append(hbox,
		 (GtkWidget *) preset_editor->apply_preset);

  /* preset */
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_widget_set_size_request((GtkWidget *) scrolled_window,
			      AGS_PRESET_EDITOR_TREE_VIEW_WIDTH_REQUEST,
			      AGS_PRESET_EDITOR_TREE_VIEW_HEIGHT_REQUEST);
  gtk_scrolled_window_set_policy(scrolled_window,
				 GTK_POLICY_AUTOMATIC,
				 GTK_POLICY_ALWAYS);
  gtk_box_append((GtkBox *) preset_editor,
		 (GtkWidget *) scrolled_window);

  preset_editor->preset_tree_view = (GtkTreeView *) gtk_tree_view_new();
  gtk_tree_view_set_activate_on_single_click(preset_editor->preset_tree_view,
					     TRUE);
  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) preset_editor->preset_tree_view);
    
  gtk_widget_set_size_request((GtkWidget *) preset_editor->preset_tree_view,
			      AGS_PRESET_EDITOR_TREE_VIEW_WIDTH_REQUEST,
			      AGS_PRESET_EDITOR_TREE_VIEW_HEIGHT_REQUEST);

  context_renderer = gtk_cell_renderer_text_new();

  context_column = gtk_tree_view_column_new_with_attributes(i18n("context"),
							    context_renderer,
							    "text", 0,
							    NULL);
  gtk_tree_view_append_column(preset_editor->preset_tree_view,
			      context_column);

  line_renderer = gtk_cell_renderer_text_new();

  line_column = gtk_tree_view_column_new_with_attributes(i18n("line"),
							  line_renderer,
							  "text", 1,
							  NULL);
  gtk_tree_view_append_column(preset_editor->preset_tree_view,
			      line_column);

  control_renderer = gtk_cell_renderer_text_new();

  control_column = gtk_tree_view_column_new_with_attributes(i18n("control"),
							    control_renderer,
							    "text", 2,
							    NULL);
  gtk_tree_view_append_column(preset_editor->preset_tree_view,
			      control_column);
  
  range_renderer = gtk_cell_renderer_text_new();

  range_column = gtk_tree_view_column_new_with_attributes(i18n("range"),
							  range_renderer,
							  "text", 3,
							  NULL);
  gtk_tree_view_append_column(preset_editor->preset_tree_view,
			      range_column);
  
  value_renderer = gtk_cell_renderer_text_new();
  g_object_set(value_renderer,
	       "editable", TRUE,
	       NULL);
  
  value_column = gtk_tree_view_column_new_with_attributes(i18n("value"),
							  value_renderer,
							  "text", 4,
							  NULL);
  gtk_tree_view_append_column(preset_editor->preset_tree_view,
			      value_column);
  
  preset_list_store = gtk_list_store_new(6,
					 G_TYPE_STRING,
					 G_TYPE_STRING,
					 G_TYPE_STRING,
					 G_TYPE_STRING,
					 G_TYPE_STRING,
					 G_TYPE_GTYPE);
  
  gtk_tree_view_set_model(preset_editor->preset_tree_view,
			  GTK_TREE_MODEL(preset_list_store));  
}

void
ags_preset_editor_connect(AgsConnectable *connectable)
{
  AgsPresetEditor *preset_editor;

  preset_editor = AGS_PRESET_EDITOR(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (preset_editor->connectable_flags)) != 0){
    return;
  }

  preset_editor->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect(preset_editor->load, "clicked",
		   G_CALLBACK(ags_preset_editor_load_callback), preset_editor);

  g_signal_connect(preset_editor->save_preset, "clicked",
		   G_CALLBACK(ags_preset_editor_save_preset_callback), preset_editor);

  g_signal_connect(preset_editor->open_preset, "clicked",
		   G_CALLBACK(ags_preset_editor_open_preset_callback), preset_editor);

  g_signal_connect(preset_editor->apply_preset, "clicked",
		   G_CALLBACK(ags_preset_editor_apply_preset_callback), preset_editor);
}

void
ags_preset_editor_disconnect(AgsConnectable *connectable)
{
  AgsPresetEditor *preset_editor;

  preset_editor = AGS_PRESET_EDITOR(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (preset_editor->connectable_flags)) == 0){
    return;
  }

  preset_editor->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(preset_editor->load,
		      "any_signal::clicked",
		      G_CALLBACK(ags_preset_editor_load_callback),
		      preset_editor,
		      NULL);

  g_object_disconnect(preset_editor->save_preset,
		      "any_signal::clicked",
		      G_CALLBACK(ags_preset_editor_save_preset_callback),
		      preset_editor,
		      NULL);

  g_object_disconnect(preset_editor->open_preset,
		      "any_signal::clicked",
		      G_CALLBACK(ags_preset_editor_open_preset_callback),
		      preset_editor,
		      NULL);

  g_object_disconnect(preset_editor->apply_preset,
		      "any_signal::clicked",
		      G_CALLBACK(ags_preset_editor_apply_preset_callback),
		      preset_editor,
		      NULL);
}

void
ags_preset_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_preset_editor_apply(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

void
ags_preset_editor_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_preset_editor_open_preset:
 * @preset_editor: the #AgsPresetEditor
 * @filename: the filename
 *
 * Open @filename and fill @preset_editor
 *
 * Since: 4.1.0
 */
void
ags_preset_editor_open_preset(AgsPresetEditor *preset_editor,
			      gchar *filename)
{
  AgsMachine *machine;
  AgsPresetDialog *preset_dialog;

  xmlDoc *doc;
  xmlNode *root_node;
  
  GtkListStore *list_store;
  GtkTreeIter iter;

  AgsChannel *start_channel;
  AgsChannel *channel;

  GList *start_port, *port;

  g_return_if_fail(AGS_IS_PRESET_EDITOR(preset_editor));
  
  preset_dialog = (AgsPresetDialog *) gtk_widget_get_ancestor((GtkWidget *) preset_editor,
							      AGS_TYPE_PRESET_DIALOG);
  machine = preset_dialog->machine;

  if(machine == NULL){
    gtk_tree_view_set_model(preset_editor->preset_tree_view,
			    NULL);
    
    return;
  }

  doc = xmlReadFile(filename,
		    NULL,
		    XML_PARSE_NOENT);

  root_node = xmlDocGetRootElement(doc);

  list_store = gtk_list_store_new(6,
				  G_TYPE_STRING,
				  G_TYPE_STRING,
				  G_TYPE_STRING,
				  G_TYPE_STRING,
				  G_TYPE_STRING,
				  G_TYPE_GTYPE);

  gtk_tree_view_set_model(preset_editor->preset_tree_view,
			  GTK_TREE_MODEL(list_store));

  start_port = NULL;
  
  if(!xmlStrncmp(BAD_CAST "ags-preset-editor",
		 root_node->name,
		 28)){
    xmlNode *child;

    child = root_node->children;

    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!xmlStrncmp(BAD_CAST "ags-preset-editor-preset-value",
		       child->name,
		       31)){
	  GType value_type;
	  
	  xmlChar *context;
	  xmlChar *line;
	  xmlChar *specifier;
	  gchar *range;
	  xmlChar *value;

	  range = NULL;
	  
	  context = xmlGetProp(child,
			       "context");

	  line = xmlGetProp(child,
			    "line");

	  specifier = xmlGetProp(child,
				 "specifier");

	  value = xmlGetProp(child,
			     "value");

	  /* get start port */
	  if(!xmlStrncmp(BAD_CAST "audio",
			 context,
			 6)){
	    GList *tmp_port;
	    
	    port =
	      tmp_port = ags_audio_collect_all_audio_ports(machine->audio);

	    while(tmp_port != NULL){
	      AgsPluginPort *plugin_port;

	      if(!g_strcmp0(AGS_PORT(tmp_port->data)->specifier,
			    specifier) &&
		 g_list_find(start_port, tmp_port->data) == NULL){
		start_port = g_list_prepend(start_port,
					    tmp_port->data);
		g_object_ref(tmp_port->data);
		
		value_type = G_TYPE_NONE;

		plugin_port = NULL;
		
		g_object_get(tmp_port->data,
			     "plugin-port", &plugin_port,
			     NULL);

		if(plugin_port != NULL){
		  /* create list store entry */
		  if(G_VALUE_HOLDS_FLOAT(plugin_port->default_value)){
		    range = g_strdup_printf("%f - %f",
					    g_value_get_float(plugin_port->lower_value), g_value_get_float(plugin_port->upper_value));

		    value_type = G_TYPE_FLOAT;
		  }else if(G_VALUE_HOLDS_DOUBLE(plugin_port->default_value)){
		    range = g_strdup_printf("%f - %f",
					    g_value_get_double(plugin_port->lower_value), g_value_get_double(plugin_port->upper_value));

		    value_type = G_TYPE_DOUBLE;
		  }
		}

		if(range == NULL){
		  range = g_strdup(i18n("not available"));
		}
	  
		gtk_list_store_append(list_store, &iter);
		gtk_list_store_set(list_store, &iter,
				   0, g_strdup(context),
				   1, g_strdup("0"),
				   2, g_strdup(specifier),
				   3, range,
				   4, g_strdup_printf("%f",
						      g_strtod(value,
							       NULL)),
				   5, value_type,
				   -1);
	      }
	      
	      /* iterate */
	      tmp_port = tmp_port->next;
	    }
	      
	    g_list_free_full(port,
			     g_object_unref);
	  }else if(!xmlStrncmp(BAD_CAST "output",
			       context,
			       7)){
	    GList *tmp_port;

	    guint current_line;

	    current_line = g_ascii_strtoull(line,
					    NULL,
					    10);
	    
	    start_channel = ags_audio_get_output(machine->audio);

	    channel = ags_channel_nth(start_channel,
				      current_line);

	    /* output */
	    port =
	      tmp_port = ags_channel_collect_all_channel_ports(channel);

	    while(tmp_port != NULL){
	      AgsPluginPort *plugin_port;
    
	      if(!g_strcmp0(AGS_PORT(tmp_port->data)->specifier,
			    specifier) &&
		 g_list_find(start_port, tmp_port->data) == NULL){    
		start_port = g_list_prepend(start_port,
					    tmp_port->data);
		g_object_ref(tmp_port->data);

		value_type = G_TYPE_NONE;

		plugin_port = NULL;
		
		g_object_get(tmp_port->data,
			     "plugin-port", &plugin_port,
			     NULL);

		if(plugin_port != NULL){
		  /* create list store entry */
		  if(G_VALUE_HOLDS_FLOAT(plugin_port->default_value)){
		    range = g_strdup_printf("%f - %f",
					    g_value_get_float(plugin_port->lower_value), g_value_get_float(plugin_port->upper_value));

		    value_type = G_TYPE_FLOAT;
		  }else if(G_VALUE_HOLDS_DOUBLE(plugin_port->default_value)){
		    range = g_strdup_printf("%f - %f",
					    g_value_get_double(plugin_port->lower_value), g_value_get_double(plugin_port->upper_value));

		    value_type = G_TYPE_DOUBLE;
		  }
		}

		if(range == NULL){
		  range = g_strdup(i18n("not available"));
		}
	  
		gtk_list_store_append(list_store, &iter);
		gtk_list_store_set(list_store, &iter,
				   0, g_strdup(context),
				   1, g_strdup(line),
				   2, g_strdup(specifier),
				   3, range,
				   4, g_strdup_printf("%f",
						      g_strtod(value,
							       NULL)),
				   5, value_type,
				   -1);
	      }
		
	      /* iterate */
	      tmp_port = tmp_port->next;
	    }
	      
	    g_list_free_full(port,
			     g_object_unref);
	    
	    /* unref */
	    if(start_channel != NULL){
	      g_object_unref(start_channel);
	    }	      
	  }else if(!xmlStrncmp(BAD_CAST "input",
			       context,
			       6)){
	    GList *tmp_port;

	    guint current_line;

	    current_line = g_ascii_strtoull(line,
					    NULL,
					    10);
	    
	    start_channel = ags_audio_get_input(machine->audio);

	    channel = ags_channel_nth(start_channel,
				      current_line);

	    /* input */
	    port =
	      tmp_port = ags_channel_collect_all_channel_ports(channel);

	    while(tmp_port != NULL){
	      AgsPluginPort *plugin_port;

	      if(!g_strcmp0(AGS_PORT(tmp_port->data)->specifier,
			    specifier) &&
		 g_list_find(start_port, tmp_port->data) == NULL){    
		start_port = g_list_prepend(start_port,
					    tmp_port->data);
		g_object_ref(tmp_port->data);

		value_type = G_TYPE_NONE;

		plugin_port = NULL;
		
		g_object_get(tmp_port->data,
			     "plugin-port", &plugin_port,
			     NULL);

		if(plugin_port != NULL){
		  /* create list store entry */
		  if(G_VALUE_HOLDS_FLOAT(plugin_port->default_value)){
		    range = g_strdup_printf("%f - %f",
					    g_value_get_float(plugin_port->lower_value), g_value_get_float(plugin_port->upper_value));

		    value_type = G_TYPE_FLOAT;
		  }else if(G_VALUE_HOLDS_DOUBLE(plugin_port->default_value)){
		    range = g_strdup_printf("%f - %f",
					    g_value_get_double(plugin_port->lower_value), g_value_get_double(plugin_port->upper_value));

		    value_type = G_TYPE_DOUBLE;
		  }
		}

		if(range == NULL){
		  range = g_strdup(i18n("not available"));
		}
	  
		gtk_list_store_append(list_store, &iter);
		gtk_list_store_set(list_store, &iter,
				   0, g_strdup(context),
				   1, g_strdup(line),
				   2, g_strdup(specifier),
				   3, range,
				   4, g_strdup_printf("%f",
						      g_strtod(value,
							       NULL)),
				   5, value_type,
				   -1);
	      }
		
	      /* iterate */
	      tmp_port = tmp_port->next;
	    }

	    g_list_free_full(port,
			     g_object_unref);
	    
	    /* unref */
	    if(start_channel != NULL){
	      g_object_unref(start_channel);
	    }	    
	  }

	  xmlFree(context);
	  xmlFree(line);
	  xmlFree(specifier);
	  xmlFree(value);
	}
      }
      
      child = child->next;
    }
  }

  g_list_free_full(start_port,
		   g_object_unref);
}

/**
 * ags_preset_editor_save_preset:
 * @preset_editor: the #AgsPresetEditor
 *
 * Save @preset_editor
 *
 * Since: 4.1.0
 */
void
ags_preset_editor_save_preset(AgsPresetEditor *preset_editor)
{
  FILE *out;

  xmlDoc *doc;
  xmlNode *root_node;
  xmlNode *child;

  GtkListStore *list_store;

  GtkTreeIter iter;

  gchar *filename;
  xmlChar *buffer;

  int size;
  
  g_return_if_fail(AGS_IS_PRESET_EDITOR(preset_editor));

  filename = gtk_editable_get_text(GTK_EDITABLE(preset_editor->filename));

  if(filename == NULL  ||
     strlen(filename) <= 0){
    return;
  }
  
  doc = xmlNewDoc(BAD_CAST "1.0");

  root_node = xmlNewNode(NULL,
			 BAD_CAST "ags-preset-editor");
  
  xmlDocSetRootElement(doc, root_node);

  list_store = GTK_LIST_STORE(gtk_tree_view_get_model(preset_editor->preset_tree_view));

  if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_store),
				   &iter)){
    do{
      gchar *context;
      gchar *line;
      gchar *specifier;
      gchar *range;
      gchar *value;

      context = NULL;
      specifier = NULL;
      range = NULL;
      value = NULL;
      
      gtk_tree_model_get(GTK_TREE_MODEL(list_store), &iter,
			 0, &context,
			 1, &line,
			 2, &specifier,
			 3, &range,
			 4, &value,
			 -1);
      
      child = xmlNewNode(NULL,
			 BAD_CAST "ags-preset-editor-preset-value");
      xmlAddChild(root_node,
		  child);

      xmlNewProp(child,
		 BAD_CAST "context",
		 context);

      xmlNewProp(child,
		 BAD_CAST "line",
		 line);

      xmlNewProp(child,
		 BAD_CAST "specifier",
		 specifier);

      xmlNewProp(child,
		 BAD_CAST "range",
		 range);

      xmlNewProp(child,
		 BAD_CAST "value",
		 value);
    }while(gtk_tree_model_iter_next(GTK_TREE_MODEL(list_store),
				    &iter));
  }

  buffer = NULL;

  out = NULL;

  if(!g_file_test(filename, G_FILE_TEST_IS_DIR)){
    out = fopen(filename, "w+");
  }
  
  if(out != NULL){
    buffer = NULL;
    size = 0;
    
    xmlDocDumpFormatMemoryEnc(doc, &buffer, &size, "UTF-8", TRUE);
    
    fwrite(buffer, size, sizeof(xmlChar), out);
    fflush(out);

    fclose(out);
    
    xmlFree(buffer);   
  }

  xmlFreeDoc(doc);
}

/**
 * ags_preset_editor_apply_preset:
 * @preset_editor: the #AgsPresetEditor
 *
 * Save @preset_editor
 *
 * Since: 4.1.0
 */
void
ags_preset_editor_apply_preset(AgsPresetEditor *preset_editor)
{  
  AgsMachine *machine;
  AgsPresetDialog *preset_dialog;

  GtkListStore *list_store;

  AgsChannel *start_channel;
  AgsChannel *channel;

  GList *start_port, *port;

  GtkTreeIter iter;

  g_return_if_fail(AGS_IS_PRESET_EDITOR(preset_editor));

  preset_dialog = (AgsPresetDialog *) gtk_widget_get_ancestor((GtkWidget *) preset_editor,
							      AGS_TYPE_PRESET_DIALOG);
  machine = preset_dialog->machine;

  if(machine == NULL){    
    return;
  }

  list_store = GTK_LIST_STORE(gtk_tree_view_get_model(preset_editor->preset_tree_view));

  if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(list_store),
				   &iter)){
    do{
      gchar *context;
      gchar *line;
      gchar *specifier;
      gchar *range;
      gchar *value;

      context = NULL;
      specifier = NULL;
      range = NULL;
      value = NULL;
      
      gtk_tree_model_get(GTK_TREE_MODEL(list_store), &iter,
			 0, &context,
			 1, &line,
			 2, &specifier,
			 3, &range,
			 4, &value,
			 -1);

      /* get start port */
      if(!xmlStrncmp(BAD_CAST "audio",
		     context,
		     6)){
	start_port = ags_audio_collect_all_audio_ports(machine->audio);
      }else if(!xmlStrncmp(BAD_CAST "output",
			   context,
			   7)){
	start_channel = ags_audio_get_output(machine->audio);

	channel = ags_channel_nth(start_channel,
				  g_ascii_strtoull(line,
						   NULL,
						   10));

	start_port = NULL;

	/* output */
	port = ags_channel_collect_all_channel_ports(channel);
	
	if(start_port != NULL){
	  if(port != NULL){
	    start_port = g_list_concat(start_port,
				       port);
	  }
	}else{
	  start_port = port;
	}

	/* unref */
	if(start_channel != NULL){
	  g_object_unref(start_channel);
	}	      

	if(channel != NULL){
	  g_object_unref(channel);
	}	      
      }else if(!xmlStrncmp(BAD_CAST "input",
			   context,
			   6)){
	start_channel = ags_audio_get_input(machine->audio);

	channel = ags_channel_nth(start_channel,
				  g_ascii_strtoull(line,
						   NULL,
						   10));

	start_port = NULL;

	/* input */
	port = ags_channel_collect_all_channel_ports(channel);
	
	if(start_port != NULL){
	  if(port != NULL){
	    start_port = g_list_concat(start_port,
				       port);
	  }
	}else{
	  start_port = port;
	}

	/* unref */
	if(start_channel != NULL){
	  g_object_unref(start_channel);
	}	      

	if(channel != NULL){
	  g_object_unref(channel);
	}	      
      }

      port = start_port;
      
      while((port = ags_port_find_specifier(port,
					    specifier)) != NULL){
	GValue port_value = G_VALUE_INIT;

	gboolean success;  

	success = FALSE;
	
	if(!(AGS_PORT(port->data)->port_value_is_pointer)){
	  if(AGS_PORT(port->data)->port_value_type == G_TYPE_FLOAT){
	    success = TRUE;
	    
	    g_value_init(&port_value,
			 G_TYPE_FLOAT);

	    g_value_set_float(&port_value,
			      (gfloat) g_strtod(value,
						NULL));
	  }else if(AGS_PORT(port->data)->port_value_type == G_TYPE_DOUBLE){
	    success = TRUE;
	    
	    g_value_init(&port_value,
			 G_TYPE_DOUBLE);

	    g_value_set_double(&port_value,
			       g_strtod(value,
					NULL));
	  }
	}

	if(success){
	  ags_port_safe_write(port->data,
			      &port_value);
	}
      
	port = port->next;
      }
    }while(gtk_tree_model_iter_next(GTK_TREE_MODEL(list_store),
				    &iter));
  }
}

/**
 * ags_preset_editor_new:
 *
 * Create a new instance of #AgsPresetEditor
 *
 * Returns: the new #AgsPresetEditor
 *
 * Since: 4.1.0
 */
AgsPresetEditor*
ags_preset_editor_new()
{
  AgsPresetEditor *preset_editor;

  preset_editor = (AgsPresetEditor *) g_object_new(AGS_TYPE_PRESET_EDITOR,
						   NULL);

  return(preset_editor);
}
