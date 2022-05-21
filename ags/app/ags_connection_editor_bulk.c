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

#include <ags/app/ags_connection_editor_bulk.h>
#include <ags/app/ags_connection_editor_bulk_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_connection_editor.h>

#include <ags/i18n.h>

void ags_connection_editor_bulk_class_init(AgsConnectionEditorBulkClass *connection_editor_bulk);
void ags_connection_editor_bulk_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_connection_editor_bulk_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_connection_editor_bulk_init(AgsConnectionEditorBulk *connection_editor_bulk);
void ags_connection_editor_bulk_dispose(GObject *gobject);

void ags_connection_editor_bulk_connect(AgsConnectable *connectable);
void ags_connection_editor_bulk_disconnect(AgsConnectable *connectable);

void ags_connection_editor_bulk_set_update(AgsApplicable *applicable, gboolean update);
void ags_connection_editor_bulk_apply(AgsApplicable *applicable);
void ags_connection_editor_bulk_reset(AgsApplicable *applicable);

static gpointer ags_connection_editor_bulk_parent_class = NULL;

/**
 * SECTION:ags_connection_editor_bulk
 * @short_description: Edit audio related aspects
 * @title: AgsConnectionEditorBulk
 * @section_id:
 * @include: ags/app/ags_connection_editor_bulk.h
 *
 * #AgsConnectionEditorBulk is a composite widget to edit all aspects of #AgsAudio.
 * It consists of multiple child editor bulks.
 */

GType
ags_connection_editor_bulk_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_connection_editor_bulk = 0;

    static const GTypeInfo ags_connection_editor_bulk_info = {
      sizeof (AgsConnectionEditorBulkClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_connection_editor_bulk_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsConnectionEditorBulk),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_connection_editor_bulk_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_connection_editor_bulk_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_connection_editor_bulk_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_connection_editor_bulk = g_type_register_static(GTK_TYPE_BOX,
							     "AgsConnectionEditorBulk", &ags_connection_editor_bulk_info,
							     0);

    g_type_add_interface_static(ags_type_connection_editor_bulk,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_connection_editor_bulk,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_connection_editor_bulk);
  }

  return g_define_type_id__volatile;
}

void
ags_connection_editor_bulk_class_init(AgsConnectionEditorBulkClass *connection_editor_bulk)
{
  GObjectClass *gobject;

  ags_connection_editor_bulk_parent_class = g_type_class_peek_parent(connection_editor_bulk);

  /* GObjectClass */
  gobject = (GObjectClass *) connection_editor_bulk;

  gobject->dispose = ags_connection_editor_bulk_dispose;
}

void
ags_connection_editor_bulk_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_connection_editor_bulk_connect;
  connectable->disconnect = ags_connection_editor_bulk_disconnect;
}

void
ags_connection_editor_bulk_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_connection_editor_bulk_set_update;
  applicable->apply = ags_connection_editor_bulk_apply;
  applicable->reset = ags_connection_editor_bulk_reset;
}

void
ags_connection_editor_bulk_init(AgsConnectionEditorBulk *connection_editor_bulk)
{
  GtkGrid *grid;
  GtkLabel *label;

  GtkCellRenderer *cell_renderer;
  GtkListStore *model;
  
  gtk_orientable_set_orientation(GTK_ORIENTABLE(connection_editor_bulk),
				 GTK_ORIENTATION_VERTICAL);
  
  connection_editor_bulk->connectable_flags = 0;

  /* output */
  grid =
    connection_editor_bulk->output_grid = gtk_grid_new();
  gtk_widget_set_visible(connection_editor_bulk->output_grid,
			 FALSE);

  gtk_widget_set_valign((GtkWidget *) grid,
			GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) grid,
			GTK_ALIGN_START);

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append((GtkBox *) connection_editor_bulk,
		 (GtkWidget *) grid);

  label = gtk_label_new(i18n("soundcard"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  connection_editor_bulk->output_soundcard = (GtkComboBox *) gtk_combo_box_new();
  gtk_grid_attach(grid,
		  (GtkWidget *) connection_editor_bulk->output_soundcard,
		  1, 0,
		  1, 1);

  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(connection_editor_bulk->output_soundcard),
			     cell_renderer,
			     FALSE); 
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(connection_editor_bulk->output_soundcard),
				 cell_renderer,
				 "text", 0,
				 NULL);

  model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);

  gtk_combo_box_set_model(connection_editor_bulk->output_soundcard,
			  model);
  
  label = gtk_label_new(i18n("first line"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  connection_editor_bulk->output_first_line = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											       0.0,
											       1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) connection_editor_bulk->output_first_line,
		  1, 1,
		  1, 1);

  label = gtk_label_new(i18n("first soundcard line"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  connection_editor_bulk->output_first_soundcard_line = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
													 0.0,
													 1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) connection_editor_bulk->output_first_soundcard_line,
		  1, 2,
		  1, 1);

  label = gtk_label_new(i18n("count"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  connection_editor_bulk->output_count = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											  0.0,
											  1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) connection_editor_bulk->output_count,
		  1, 3,
		  1, 1);

  connection_editor_bulk->remove_bulk = gtk_button_new_from_icon_name("list-remove-symbolic");
  gtk_grid_attach(grid,
		  (GtkWidget *) connection_editor_bulk->remove_bulk,
		  3, 3,
		  1, 1);

  /* input */
  grid =
    connection_editor_bulk->input_grid = gtk_grid_new();
  gtk_widget_set_visible(connection_editor_bulk->input_grid,
			 FALSE);

  gtk_widget_set_valign((GtkWidget *) grid,
			GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) grid,
			GTK_ALIGN_START);

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append((GtkBox *) connection_editor_bulk,
		 (GtkWidget *) grid);

  label = gtk_label_new(i18n("soundcard"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  connection_editor_bulk->input_soundcard = (GtkComboBox *) gtk_combo_box_new();
  gtk_grid_attach(grid,
		  (GtkWidget *) connection_editor_bulk->input_soundcard,
		  1, 0,
		  1, 1);

  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(connection_editor_bulk->input_soundcard),
			     cell_renderer,
			     FALSE); 
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(connection_editor_bulk->input_soundcard),
				 cell_renderer,
				 "text", 0,
				 NULL);

  model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);

  gtk_combo_box_set_model(connection_editor_bulk->input_soundcard,
			  model);

  label = gtk_label_new(i18n("first line"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  connection_editor_bulk->input_first_line = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											      0.0,
											      1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) connection_editor_bulk->input_first_line,
		  1, 1,
		  1, 1);

  label = gtk_label_new(i18n("first soundcard line"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  connection_editor_bulk->input_first_soundcard_line = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
													0.0,
													1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) connection_editor_bulk->input_first_soundcard_line,
		  1, 2,
		  1, 1);

  label = gtk_label_new(i18n("count"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  connection_editor_bulk->input_count = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											 0.0,
											 1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) connection_editor_bulk->input_count,
		  1, 3,
		  1, 1);

  connection_editor_bulk->remove_bulk = gtk_button_new_from_icon_name("list-remove-symbolic");
  gtk_grid_attach(grid,
		  (GtkWidget *) connection_editor_bulk->remove_bulk,
		  3, 3,
		  1, 1);
}

void
ags_connection_editor_bulk_dispose(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_connection_editor_bulk_parent_class)->dispose(gobject);
}

void
ags_connection_editor_bulk_connect(AgsConnectable *connectable)
{
  AgsConnectionEditorBulk *connection_editor_bulk;

  connection_editor_bulk = AGS_CONNECTION_EDITOR_BULK(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (connection_editor_bulk->connectable_flags)) != 0){
    return;
  }

  connection_editor_bulk->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect_after(connection_editor_bulk->output_soundcard, "changed",
			 G_CALLBACK(ags_connection_editor_bulk_output_soundcard_callback), connection_editor_bulk);

  g_signal_connect_after(connection_editor_bulk->input_soundcard, "changed",
			 G_CALLBACK(ags_connection_editor_bulk_input_soundcard_callback), connection_editor_bulk);
  
  g_signal_connect_after(connection_editor_bulk->remove_bulk, "clicked",
			 G_CALLBACK(ags_connection_editor_bulk_remove_bulk_callback), connection_editor_bulk);
}

void
ags_connection_editor_bulk_disconnect(AgsConnectable *connectable)
{
  AgsConnectionEditorBulk *connection_editor_bulk;

  connection_editor_bulk = AGS_CONNECTION_EDITOR_BULK(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (connection_editor_bulk->connectable_flags)) == 0){
    return;
  }
  
  connection_editor_bulk->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(connection_editor_bulk->output_soundcard,
		      "any_signal::changed",
		      G_CALLBACK(ags_connection_editor_bulk_output_soundcard_callback),
		      connection_editor_bulk,
		      NULL);

  g_object_disconnect(connection_editor_bulk->input_soundcard,
		      "any_signal::changed",
		      G_CALLBACK(ags_connection_editor_bulk_input_soundcard_callback),
		      connection_editor_bulk,
		      NULL);

  g_object_disconnect(connection_editor_bulk->remove_bulk,
		      "any_signal::clicked",
		      G_CALLBACK(ags_connection_editor_bulk_remove_bulk_callback),
		      connection_editor_bulk,
		      NULL);
}

void
ags_connection_editor_bulk_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsConnectionEditorBulk *connection_editor_bulk;

  connection_editor_bulk = AGS_CONNECTION_EDITOR_BULK(applicable);

  //TODO:JK: implement me
}

void
ags_connection_editor_bulk_apply(AgsApplicable *applicable)
{
  AgsMachine *machine;
  AgsConnectionEditor *connection_editor;
  AgsConnectionEditorCollection *connection_editor_collection;
  AgsConnectionEditorBulk *connection_editor_bulk;

  GtkTreeModel *model;

  GtkTreeIter iter;
  
  AgsChannel *start_input, *start_output;
  AgsChannel *channel;
  
  AgsApplicationContext *application_context;

  GObject *output_soundcard, *input_soundcard;

  xmlNode *node;

  GList *start_dialog_model, *dialog_model;
  GList *start_list, *list;
  
  guint output_first_line, output_first_soundcard_line;
  guint input_first_line, input_first_soundcard_line;
  guint output_count;
  guint input_count;
  gint i, i_stop;
  
  connection_editor_bulk = AGS_CONNECTION_EDITOR_BULK(applicable);

  connection_editor_collection = (AgsConnectionEditorCollection *) gtk_widget_get_ancestor(connection_editor_bulk,
											   AGS_TYPE_CONNECTION_EDITOR_COLLECTION);

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(connection_editor_bulk,
								      AGS_TYPE_CONNECTION_EDITOR);

  machine = connection_editor->machine;

  /* application context */  
  application_context = ags_application_context_get_instance();

  /* output  */
  model = gtk_combo_box_get_model(connection_editor_bulk->output_soundcard);  

  output_soundcard = NULL;
  
  if(model != NULL &&
     gtk_combo_box_get_active_iter(connection_editor_bulk->output_soundcard,
				   &iter)){
    gtk_tree_model_get(model,
		       &iter,
		       1, &output_soundcard,
		       -1);
  }
  
  output_first_line = (guint) gtk_spin_button_get_value_as_int(connection_editor_bulk->output_first_line);
  
  output_first_soundcard_line = (guint) gtk_spin_button_get_value_as_int(connection_editor_bulk->output_first_soundcard_line);
  
  output_count = (guint) gtk_spin_button_get_value_as_int(connection_editor_bulk->output_count);

  /* input  */
  model = gtk_combo_box_get_model(connection_editor_bulk->input_soundcard);  

  input_soundcard = NULL;

  if(model != NULL &&
     gtk_combo_box_get_active_iter(connection_editor_bulk->input_soundcard,
				   &iter)){
    gtk_tree_model_get(model,
		       &iter,
		       1, &input_soundcard,
		       -1);
  }
  
  input_first_line = (guint) gtk_spin_button_get_value_as_int(connection_editor_bulk->input_first_line);
  
  input_first_soundcard_line = (guint) gtk_spin_button_get_value_as_int(connection_editor_bulk->input_first_soundcard_line);
  
  input_count = (guint) gtk_spin_button_get_value_as_int(connection_editor_bulk->input_count);

  /* apply output */
  start_output = NULL;
  start_input = NULL;

  channel = NULL;  
  
  if(g_type_is_a(connection_editor_collection->channel_type, AGS_TYPE_OUTPUT)){
    start_output = ags_audio_get_output(machine->audio);

    channel = ags_channel_pad_nth(start_output,
				  output_first_line);
  }else{
    start_input = ags_audio_get_input(machine->audio);

    channel = ags_channel_pad_nth(start_input,
				  output_first_line);
  }

  for(i = 0; i < output_count; i++){
    AgsChannel *next_pad;

    ags_channel_set_output_soundcard(channel,
				     output_soundcard);
    ags_channel_set_output_soundcard_channel(channel,
					     output_first_soundcard_line + i);
    
    /* iterate */
    next_pad = ags_channel_next_pad(channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_pad;
  }
  
  if(start_output){
    g_object_unref(start_output);
  }

  if(start_input){
    g_object_unref(start_input);
  }

  /* apply input */
  start_output = NULL;
  start_input = NULL;

  channel = NULL;  
  
  if(g_type_is_a(connection_editor_collection->channel_type, AGS_TYPE_OUTPUT)){
    start_output = ags_audio_get_output(machine->audio);

    channel = ags_channel_pad_nth(start_output,
				  output_first_line);
  }else{
    start_input = ags_audio_get_input(machine->audio);

    channel = ags_channel_pad_nth(start_input,
				  output_first_line);
  }
  
  for(i = 0; i < input_count; i++){
    AgsChannel *next_pad;
    
    ags_channel_set_input_soundcard(channel,
				    input_soundcard);
    ags_channel_set_input_soundcard_channel(channel,
					    input_first_soundcard_line + i);
    
    /* iterate */
    next_pad = ags_channel_next_pad(channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_pad;
  }
  
  if(start_output){
    g_object_unref(start_output);
  }

  if(start_input){
    g_object_unref(start_input);
  }
  
  start_output = NULL;
  start_input = NULL;

  /* find model */
  list = 
    start_list = ags_connection_editor_collection_get_bulk(connection_editor_collection);
  
  i_stop = g_list_index(start_list,
			connection_editor_bulk);
  
  dialog_model =
    start_dialog_model = ags_machine_get_dialog_model(machine);
  
  node = NULL;

  for(i = 0; dialog_model != NULL && i < i_stop; i++){
    if(g_type_is_a(connection_editor_collection->channel_type, AGS_TYPE_OUTPUT)){
      dialog_model = ags_machine_find_dialog_model(machine,
						   dialog_model,
						   "ags-connection-editor-bulk",
						   "direction",
						   "output");
    }else if(g_type_is_a(connection_editor_collection->channel_type, AGS_TYPE_INPUT)){
      dialog_model = ags_machine_find_dialog_model(machine,
						   dialog_model,
						   "ags-connection-editor-bulk",
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

  node = ags_connection_editor_bulk_to_xml_node(connection_editor_bulk);
  ags_machine_add_dialog_model(machine,
			       node);
}

void
ags_connection_editor_bulk_reset(AgsApplicable *applicable)
{
  AgsMachine *machine;
  AgsConnectionEditor *connection_editor;
  AgsConnectionEditorCollection *connection_editor_collection;
  AgsConnectionEditorBulk *connection_editor_bulk;

  GtkTreeModel *model;

  AgsApplicationContext *application_context;
  
  xmlNode *node;
    
  GtkTreeIter iter;

  GList *start_soundcard, *soundcard;
  GList *start_dialog_model, *dialog_model;
  GList *start_list, *list;

  gint i, i_stop;  
    
  connection_editor_bulk = AGS_CONNECTION_EDITOR_BULK(applicable);

  application_context = ags_application_context_get_instance();

  connection_editor_collection = (AgsConnectionEditorCollection *) gtk_widget_get_ancestor(connection_editor_bulk,
											   AGS_TYPE_CONNECTION_EDITOR_COLLECTION);

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(connection_editor_bulk,
								      AGS_TYPE_CONNECTION_EDITOR);

  machine = connection_editor->machine;
  
  start_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

  list = 
    start_list = ags_connection_editor_collection_get_bulk(connection_editor_collection);

  i_stop = g_list_index(start_list,
			connection_editor_bulk);
  
  dialog_model =
    start_dialog_model = ags_machine_get_dialog_model(machine);
  
  node = NULL;

  for(i = 0; dialog_model != NULL && i <= i_stop; i++){
    if(g_type_is_a(connection_editor_collection->channel_type, AGS_TYPE_OUTPUT)){
      dialog_model = ags_machine_find_dialog_model(machine,
						   dialog_model,
						   "ags-connection-editor-bulk",
						   "direction",
						   "output");
    }else{
      dialog_model = ags_machine_find_dialog_model(machine,
						   dialog_model,
						   "ags-connection-editor-bulk",
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
  
  /* output soundcard */
  model = gtk_combo_box_get_model(connection_editor_bulk->output_soundcard);

  if(GTK_IS_LIST_STORE(model)){
    gtk_list_store_clear(GTK_LIST_STORE(model));
  }

  gtk_list_store_append(GTK_LIST_STORE(model), &iter);
  gtk_list_store_set(GTK_LIST_STORE(model), &iter,
		     0, "NULL",
		     1, NULL,
		     -1);

  soundcard = start_soundcard;
  
  while(soundcard != NULL){
    gchar *device;
    gchar *str;

    device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard->data));
    
    str = g_strdup_printf("%s: %s", 
			  G_OBJECT_TYPE_NAME(G_OBJECT(soundcard->data)),
			  ((device != NULL) ? device: "(null)"));

    gtk_list_store_append(GTK_LIST_STORE(model), &iter);

    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
		       0, str,
		       1, soundcard->data,
		       -1);
    
    soundcard = soundcard->next;
  }

  /* input soundcard */
  model = gtk_combo_box_get_model(connection_editor_bulk->input_soundcard);

  if(GTK_IS_LIST_STORE(model)){
    gtk_list_store_clear(GTK_LIST_STORE(model));
  }

  gtk_list_store_append(GTK_LIST_STORE(model), &iter);
  gtk_list_store_set(GTK_LIST_STORE(model), &iter,
		     0, "NULL",
		     1, NULL,
		     -1);

  soundcard = start_soundcard;
  
  while(soundcard != NULL){
    gchar *device;
    gchar *str;

    device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard->data));
    
    str = g_strdup_printf("%s: %s", 
			  G_OBJECT_TYPE_NAME(G_OBJECT(soundcard->data)),
			  ((device != NULL) ? device: "(null)"));

    gtk_list_store_append(GTK_LIST_STORE(model), &iter);

    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
		       0, str,
		       1, soundcard->data,
		       -1);
    
    soundcard = soundcard->next;
  }

  g_list_free_full(start_soundcard,
		   (GDestroyNotify) g_object_unref);
  
  g_list_free(start_dialog_model);
  
  g_list_free(start_list);

  if(node != NULL){
    ags_connection_editor_bulk_from_xml_node(connection_editor_bulk,
					     node);
  }
}

/**
 * ags_connection_editor_bulk_to_xml_node:
 * @connection_editor_bulk: the #AgsConnectionEditorBulk
 * 
 * Serialize @connection_editor_bulk to #xmlNode-struct.
 * 
 * Returns: the serialized #xmlNode-struct
 * 
 * Since: 4.0.0
 */
xmlNode*
ags_connection_editor_bulk_to_xml_node(AgsConnectionEditorBulk *connection_editor_bulk)
{
  AgsConnectionEditor *connection_editor;
  AgsConnectionEditorCollection *connection_editor_collection;

  GtkTreeModel *model;

  xmlNode *node;
  xmlNode *child;
  
  GtkTreeIter iter;

  g_return_val_if_fail(AGS_IS_CONNECTION_EDITOR_BULK(connection_editor_bulk), NULL);

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(connection_editor_bulk,
								      AGS_TYPE_CONNECTION_EDITOR);  

  connection_editor_collection = (AgsConnectionEditorCollection *) gtk_widget_get_ancestor(connection_editor_bulk,
											   AGS_TYPE_CONNECTION_EDITOR_COLLECTION);
  
  node = NULL;
  
  if(g_type_is_a(connection_editor_collection->channel_type, AGS_TYPE_OUTPUT)){
    node = xmlNewNode(NULL,
		      BAD_CAST "ags-connection-editor-bulk");
    
    xmlNewProp(node,
	       "direction",
	       "output");
  }else if(g_type_is_a(connection_editor_collection->channel_type, AGS_TYPE_INPUT)){
    node = xmlNewNode(NULL,
		      BAD_CAST "ags-connection-editor-bulk");

    xmlNewProp(node,
	       "direction",
	       "input");
  }else{
    return(NULL);
  }

  if((AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_OUTPUT & (connection_editor->flags)) != 0){
    gchar *output_soundcard;
    gchar *value;

    /* output soundcard */
    child = xmlNewNode(NULL,
		       "output-soundcard");

    model = gtk_combo_box_get_model(connection_editor_bulk->output_soundcard);

    output_soundcard = NULL;
    
    gtk_combo_box_get_active_iter(connection_editor_bulk->output_soundcard,
				  &iter);
    gtk_tree_model_get(model,
		       &iter,
		       0, &output_soundcard,
		       -1);
  
    xmlNodeAddContent(child,
		      output_soundcard);
  
    xmlAddChild(node,
		child);
    
    /* output first line */
    child = xmlNewNode(NULL,
		       "output-first-line");

    value = g_strdup_printf("%d",
			    gtk_spin_button_get_value_as_int(connection_editor_bulk->output_first_line));
  
    xmlNewProp(child,
	       "value",
	       value);
  
    xmlAddChild(node,
		child);

    /* output first soundcard line */
    child = xmlNewNode(NULL,
		       "output-first-soundcard-line");

    value = g_strdup_printf("%d",
			    gtk_spin_button_get_value_as_int(connection_editor_bulk->output_first_soundcard_line));
  
    xmlNewProp(child,
	       "value",
	       value);
  
    xmlAddChild(node,
		child);

    /* output count */
    child = xmlNewNode(NULL,
		       "output-count");

    value = g_strdup_printf("%d",
			    gtk_spin_button_get_value_as_int(connection_editor_bulk->output_count));
  
    xmlNewProp(child,
	       "value",
	       value);
  
    xmlAddChild(node,
		child);
  }

  if((AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_INPUT & (connection_editor->flags)) != 0){
    gchar *input_soundcard;
    gchar *value;

    /* input soundcard */
    child = xmlNewNode(NULL,
		       "input-soundcard");

    model = gtk_combo_box_get_model(connection_editor_bulk->input_soundcard);

    input_soundcard = NULL;
    
    gtk_combo_box_get_active_iter(connection_editor_bulk->input_soundcard,
				  &iter);
    gtk_tree_model_get(model,
		       &iter,
		       0, &input_soundcard,
		       -1);
  
    xmlNodeAddContent(child,
		      input_soundcard);
  
    xmlAddChild(node,
		child);
    
    /* input first line */
    child = xmlNewNode(NULL,
		       "input-first-line");

    value = g_strdup_printf("%d",
			    gtk_spin_button_get_value_as_int(connection_editor_bulk->input_first_line));
  
    xmlNewProp(child,
	       "value",
	       value);
  
    xmlAddChild(node,
		child);

    /* input first soundcard line */
    child = xmlNewNode(NULL,
		       "input-first-soundcard-line");

    value = g_strdup_printf("%d",
			    gtk_spin_button_get_value_as_int(connection_editor_bulk->input_first_soundcard_line));
  
    xmlNewProp(child,
	       "value",
	       value);
  
    xmlAddChild(node,
		child);

    /* input count */
    child = xmlNewNode(NULL,
		       "input-count");

    value = g_strdup_printf("%d",
			    gtk_spin_button_get_value_as_int(connection_editor_bulk->input_count));
  
    xmlNewProp(child,
	       "value",
	       value);
  
    xmlAddChild(node,
		child);
  }
  
  return(node);
}

/**
 * ags_connection_editor_bulk_from_xml_node:
 * @connection_editor_bulk: the #AgsConnectionEditorBulk
 * @node: the #xmlNode-struct
 * 
 * Parse @node and apply to @connection_editor_bulk.
 * 
 * Since: 4.0.0
 */
void
ags_connection_editor_bulk_from_xml_node(AgsConnectionEditorBulk *connection_editor_bulk,
					 xmlNode *node)
{
  xmlNode *child;

  g_return_if_fail(AGS_IS_CONNECTION_EDITOR_BULK(connection_editor_bulk));
  g_return_if_fail(node != NULL);

  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(BAD_CAST "output-soundcard",
		     child->name,
		     17)){
	GtkTreeModel *model;

	GtkTreeIter iter;
	
	xmlChar *output_soundcard;

	output_soundcard = xmlNodeGetContent(child);

	model = gtk_combo_box_get_model(connection_editor_bulk->output_soundcard);

	if(gtk_tree_model_get_iter_first(model, &iter)){
	  do{
	    gchar *current_output_soundcard;
	    
	    gtk_tree_model_get(model,
			       &iter,
			       0, &current_output_soundcard,
			       -1);
	    
	    if(!g_strcmp0(output_soundcard,
			  current_output_soundcard)){
	      break;
	    }
	  }while(gtk_tree_model_iter_next(model, &iter));
	}

	gtk_combo_box_set_active_iter(connection_editor_bulk->output_soundcard,
				      &iter);
      }else if(!xmlStrncmp(BAD_CAST "output-first-line",
			   child->name,
			   18)){
	xmlChar *output_first_line;

	output_first_line = xmlGetProp(child,
				       BAD_CAST "value");

	gtk_spin_button_set_value(connection_editor_bulk->output_first_line,
				  g_ascii_strtod(output_first_line,
						 NULL));
      }else if(!xmlStrncmp(BAD_CAST "output-first-soundcard-line",
			   child->name,
			   28)){
	xmlChar *output_first_soundcard_line;

	output_first_soundcard_line = xmlGetProp(child,
						 BAD_CAST "value");

	gtk_spin_button_set_value(connection_editor_bulk->output_first_soundcard_line,
				  g_ascii_strtod(output_first_soundcard_line,
						 NULL));
      }else if(!xmlStrncmp(BAD_CAST "output-count",
			   child->name,
			   13)){
	xmlChar *output_count;

	output_count = xmlGetProp(child,
				  BAD_CAST "value");

	gtk_spin_button_set_value(connection_editor_bulk->output_count,
				  g_ascii_strtod(output_count,
						 NULL));
      }else if(!xmlStrncmp(BAD_CAST "input-soundcard",
			   child->name,
			   16)){
	GtkTreeModel *model;

	GtkTreeIter iter;
	
	xmlChar *input_soundcard;

	input_soundcard = xmlNodeGetContent(child);

	model = gtk_combo_box_get_model(connection_editor_bulk->input_soundcard);

	if(gtk_tree_model_get_iter_first(model, &iter)){
	  do{
	    gchar *current_input_soundcard;
	    
	    gtk_tree_model_get(model,
			       &iter,
			       0, &current_input_soundcard,
			       -1);

	    if(!g_strcmp0(input_soundcard,
			  current_input_soundcard)){
	      break;
	    }
	  }while(gtk_tree_model_iter_next(model, &iter));
	}

	gtk_combo_box_set_active_iter(connection_editor_bulk->input_soundcard,
				      &iter);
      }else if(!xmlStrncmp(BAD_CAST "input-first-line",
			   child->name,
			   17)){
	xmlChar *input_first_line;

	input_first_line = xmlGetProp(child,
				      BAD_CAST "value");

	gtk_spin_button_set_value(connection_editor_bulk->input_first_line,
				  g_ascii_strtod(input_first_line,
						 NULL));
      }else if(!xmlStrncmp(BAD_CAST "input-first-soundcard-line",
			   child->name,
			   27)){
	xmlChar *input_first_soundcard_line;

	input_first_soundcard_line = xmlGetProp(child,
						BAD_CAST "value");

	gtk_spin_button_set_value(connection_editor_bulk->input_first_soundcard_line,
				  g_ascii_strtod(input_first_soundcard_line,
						 NULL));
      }else if(!xmlStrncmp(BAD_CAST "input-count",
			   child->name,
			   12)){
	xmlChar *input_count;

	input_count = xmlGetProp(child,
				 BAD_CAST "value");

	gtk_spin_button_set_value(connection_editor_bulk->input_count,
				  g_ascii_strtod(input_count,
						 NULL));
      }
    }

    child = child->next;
  }
}

/**
 * ags_connection_editor_bulk_new:
 *
 * Creates an #AgsConnectionEditorBulk
 *
 * Returns: a new #AgsConnectionEditorBulk
 *
 * Since: 4.0.0
 */
AgsConnectionEditorBulk*
ags_connection_editor_bulk_new()
{
  AgsConnectionEditorBulk *connection_editor_bulk;

  connection_editor_bulk = (AgsConnectionEditorBulk *) g_object_new(AGS_TYPE_CONNECTION_EDITOR_BULK,
								    NULL);

  return(connection_editor_bulk);
}
