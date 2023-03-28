/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/app/ags_connection_editor_line.h>
#include <ags/app/ags_connection_editor_line_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_connection_editor.h>

#include <ags/i18n.h>

void ags_connection_editor_line_class_init(AgsConnectionEditorLineClass *connection_editor_line);
void ags_connection_editor_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_connection_editor_line_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_connection_editor_line_init(AgsConnectionEditorLine *connection_editor_line);
void ags_connection_editor_line_set_property(GObject *gobject,
					     guint prop_id,
					     const GValue *value,
					     GParamSpec *param_spec);
void ags_connection_editor_line_get_property(GObject *gobject,
					     guint prop_id,
					     GValue *value,
					     GParamSpec *param_spec);

void ags_connection_editor_line_connect(AgsConnectable *connectable);
void ags_connection_editor_line_disconnect(AgsConnectable *connectable);

void ags_connection_editor_line_set_update(AgsApplicable *applicable, gboolean update);
void ags_connection_editor_line_apply(AgsApplicable *applicable);
void ags_connection_editor_line_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_connection_editor_line
 * @short_description: Edit audio related aspects
 * @title: AgsConnectionEditorLine
 * @section_id:
 * @include: ags/app/ags_connection_editor_line.h
 *
 * #AgsConnectionEditorLine is a composite widget to edit all aspects of #AgsAudio.
 * It consists of multiple child editor_lines.
 */

enum{
  PROP_0,
  PROP_CHANNEL,
};

static gpointer ags_connection_editor_line_parent_class = NULL;

GType
ags_connection_editor_line_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_connection_editor_line = 0;

    static const GTypeInfo ags_connection_editor_line_info = {
      sizeof (AgsConnectionEditorLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_connection_editor_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsConnectionEditorLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_connection_editor_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_connection_editor_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_connection_editor_line_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_connection_editor_line = g_type_register_static(GTK_TYPE_BOX,
							     "AgsConnectionEditorLine", &ags_connection_editor_line_info,
							     0);

    g_type_add_interface_static(ags_type_connection_editor_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_connection_editor_line,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_connection_editor_line);
  }

  return g_define_type_id__volatile;
}

void
ags_connection_editor_line_class_init(AgsConnectionEditorLineClass *connection_editor_line)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_connection_editor_line_parent_class = g_type_class_peek_parent(connection_editor_line);

  /* GObjectClass */
  gobject = (GObjectClass *) connection_editor_line;

  gobject->set_property = ags_connection_editor_line_set_property;
  gobject->get_property = ags_connection_editor_line_get_property;

  /* properties */
  /**
   * AgsConnectionEditorLine:channel:
   *
   * The channel.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("channel"),
				   i18n_pspec("The assigned channel"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);
}

void
ags_connection_editor_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_connection_editor_line_connect;
  connectable->disconnect = ags_connection_editor_line_disconnect;
}

void
ags_connection_editor_line_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_connection_editor_line_set_update;
  applicable->apply = ags_connection_editor_line_apply;
  applicable->reset = ags_connection_editor_line_reset;
}

void
ags_connection_editor_line_init(AgsConnectionEditorLine *connection_editor_line)
{
  GtkGrid *grid;
  
  GtkCellRenderer *cell_renderer;
  GtkListStore *model;
  
  gtk_orientable_set_orientation(GTK_ORIENTABLE(connection_editor_line),
				 GTK_ORIENTATION_VERTICAL);

  gtk_box_set_spacing(GTK_BOX(connection_editor_line),
		      AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  connection_editor_line->connectable_flags = 0;

  connection_editor_line->parent_connection_editor_pad = NULL;

  connection_editor_line->channel = NULL;

  grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append((GtkBox *) connection_editor_line,
		 (GtkWidget *) grid);

  connection_editor_line->output_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
							      AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_widget_set_visible((GtkWidget *) connection_editor_line->output_box,
			 FALSE);
  gtk_grid_attach(grid,
		  (GtkWidget *) connection_editor_line->output_box,
		  0, 0,
		  1, 1);

  connection_editor_line->output_label = (GtkLabel *) gtk_label_new(i18n("line"));
  gtk_box_append(connection_editor_line->output_box,
		 (GtkWidget *) connection_editor_line->output_label);
  
  connection_editor_line->output_soundcard = (GtkComboBox *) gtk_combo_box_new();
  gtk_box_append(connection_editor_line->output_box,
		 (GtkWidget *) connection_editor_line->output_soundcard);

  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(connection_editor_line->output_soundcard),
			     cell_renderer,
			     FALSE); 
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(connection_editor_line->output_soundcard),
				 cell_renderer,
				 "text", 0,
				 NULL);

  model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);

  gtk_combo_box_set_model(connection_editor_line->output_soundcard,
			  GTK_TREE_MODEL(model));
  
  connection_editor_line->output_line = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											 0.0,
											 1.0);
  gtk_box_append(connection_editor_line->output_box,
		 (GtkWidget *) connection_editor_line->output_line);
  
  connection_editor_line->input_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
							     AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_widget_set_visible((GtkWidget *) connection_editor_line->input_box,
			 FALSE);
  gtk_grid_attach(grid,
		  (GtkWidget *) connection_editor_line->input_box,
		  0, 1,
		  1, 1);

  connection_editor_line->input_label = (GtkLabel *) gtk_label_new(i18n("line"));
  gtk_box_append(connection_editor_line->input_box,
		 (GtkWidget *) connection_editor_line->input_label);
  
  connection_editor_line->input_soundcard = (GtkComboBox *) gtk_combo_box_new();
  gtk_box_append(connection_editor_line->input_box,
		 (GtkWidget *) connection_editor_line->input_soundcard);

  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(connection_editor_line->input_soundcard),
			     cell_renderer,
			     FALSE); 
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(connection_editor_line->input_soundcard),
				 cell_renderer,
				 "text", 0,
				 NULL);

  model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);

  gtk_combo_box_set_model(connection_editor_line->input_soundcard,
			  GTK_TREE_MODEL(model));

  connection_editor_line->input_line = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											0.0,
											1.0);
  gtk_box_append(connection_editor_line->input_box,
		 (GtkWidget *) connection_editor_line->input_line);
}

void
ags_connection_editor_line_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec)
{
  AgsConnectionEditorLine *connection_editor_line;

  connection_editor_line = AGS_CONNECTION_EDITOR_LINE(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = g_value_get_object(value);

      if(connection_editor_line->channel != NULL){
	g_object_unref(connection_editor_line->channel);
      }

      if(channel != NULL){
	gchar *str;
	
	guint line;
	
	g_object_ref(channel);

	line = ags_channel_get_line(channel);

	str = g_strdup_printf("%s %d",
			      i18n("line"),
			      line);

	gtk_label_set_label(connection_editor_line->output_label,
			    str);

	g_free(str);
      }
    
      connection_editor_line->channel = channel;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_connection_editor_line_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec)
{
  AgsConnectionEditorLine *connection_editor_line;

  connection_editor_line = AGS_CONNECTION_EDITOR_LINE(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value, connection_editor_line->channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_connection_editor_line_connect(AgsConnectable *connectable)
{
  AgsConnectionEditorLine *connection_editor_line;

  connection_editor_line = AGS_CONNECTION_EDITOR_LINE(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (connection_editor_line->connectable_flags)) != 0){
    return;
  }

  connection_editor_line->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect_after(connection_editor_line->output_soundcard, "changed",
			 G_CALLBACK(ags_connection_editor_line_output_soundcard_callback), connection_editor_line);

  g_signal_connect_after(connection_editor_line->input_soundcard, "changed",
			 G_CALLBACK(ags_connection_editor_line_input_soundcard_callback), connection_editor_line);
}

void
ags_connection_editor_line_disconnect(AgsConnectable *connectable)
{
  AgsConnectionEditorLine *connection_editor_line;

  connection_editor_line = AGS_CONNECTION_EDITOR_LINE(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (connection_editor_line->connectable_flags)) == 0){
    return;
  }
  
  connection_editor_line->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(connection_editor_line->output_soundcard,
		      "any_signal::changed",
		      G_CALLBACK(ags_connection_editor_line_output_soundcard_callback),
		      connection_editor_line,
		      NULL);

  g_object_disconnect(connection_editor_line->input_soundcard,
		      "any_signal::changed",
		      G_CALLBACK(ags_connection_editor_line_input_soundcard_callback),
		      connection_editor_line,
		      NULL);
}

void
ags_connection_editor_line_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsConnectionEditorLine *connection_editor_line;

  connection_editor_line = AGS_CONNECTION_EDITOR_LINE(applicable);

  //TODO:JK: implement me
}

void
ags_connection_editor_line_apply(AgsApplicable *applicable)
{
  AgsConnectionEditor *connection_editor;
  AgsConnectionEditorLine *connection_editor_line;

  connection_editor_line = AGS_CONNECTION_EDITOR_LINE(applicable);

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(connection_editor_line->parent_connection_editor_pad,
								      AGS_TYPE_CONNECTION_EDITOR);
  /* output soundcard */
  if((AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_OUTPUT & (connection_editor->flags)) != 0){
    GObject *output_soundcard;
    
    GtkTreeModel *model;

    GtkTreeIter iter;

    guint output_line;

    model = gtk_combo_box_get_model(connection_editor_line->output_soundcard);  

    gtk_combo_box_get_active_iter(connection_editor_line->output_soundcard,
				  &iter);

    output_soundcard = NULL;

    gtk_tree_model_get(model,
		       &iter,
		       1, &output_soundcard,
		       -1);
    output_line = (guint) gtk_spin_button_get_value_as_int(connection_editor_line->output_line);

    ags_channel_set_output_soundcard(connection_editor_line->channel,
				     output_soundcard);
    ags_channel_set_output_soundcard_channel(connection_editor_line->channel,
					     output_line);
  }
  
  /* input soundcard */
  if((AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_INPUT & (connection_editor->flags)) != 0){
    GObject *input_soundcard;
    
    GtkTreeModel *model;

    GtkTreeIter iter;

    guint input_line;

    model = gtk_combo_box_get_model(connection_editor_line->input_soundcard);  

    gtk_combo_box_get_active_iter(connection_editor_line->input_soundcard,
				  &iter);

    input_soundcard = NULL;

    gtk_tree_model_get(model,
		       &iter,
		       1, &input_soundcard,
		       -1);
    input_line = (guint) gtk_spin_button_get_value_as_int(connection_editor_line->input_line);

    ags_channel_set_input_soundcard(connection_editor_line->channel,
				    input_soundcard);
    ags_channel_set_input_soundcard_channel(connection_editor_line->channel,
					    input_line);
  }
}

void
ags_connection_editor_line_reset(AgsApplicable *applicable)
{
  AgsConnectionEditor *connection_editor;
  AgsConnectionEditorLine *connection_editor_line;

  GtkTreeModel *model;

  AgsApplicationContext *application_context;

  GObject *output_soundcard, *input_soundcard;
  
  GList *start_soundcard, *soundcard;
  
  GtkTreeIter iter;

  gint position;
  gboolean success;
  
  connection_editor_line = AGS_CONNECTION_EDITOR_LINE(applicable);

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(connection_editor_line->parent_connection_editor_pad,
								      AGS_TYPE_CONNECTION_EDITOR);

  application_context = ags_application_context_get_instance();

  output_soundcard = ags_channel_get_output_soundcard(connection_editor_line->channel);
  input_soundcard = ags_channel_get_input_soundcard(connection_editor_line->channel);
  
  start_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

  /* output soundcard */
  if((AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_OUTPUT & (connection_editor->flags)) != 0){
    model = gtk_combo_box_get_model(connection_editor_line->output_soundcard);

    if(GTK_IS_LIST_STORE(model)){
      gtk_list_store_clear(GTK_LIST_STORE(model));
    }

    position = 0;

    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
		       0, "NULL",
		       1, NULL,
		       -1);
  
    soundcard = start_soundcard;

    success = FALSE;

    position++;
  
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

      if(soundcard->data == output_soundcard){
	success = TRUE;
      }
    
      if(!success){
	position++;
      }
    
      soundcard = soundcard->next;
    }

    gtk_combo_box_set_active(connection_editor_line->output_soundcard,
			     0);

    if(success){
      guint pcm_channels;
      gint output_soundcard_channel;

      pcm_channels = 0;

      ags_soundcard_get_presets(AGS_SOUNDCARD(output_soundcard),
				&pcm_channels,
				NULL,
				NULL,
				NULL);  

      output_soundcard_channel = ags_channel_get_output_soundcard_channel(connection_editor_line->channel);
    
      gtk_combo_box_set_active(connection_editor_line->output_soundcard,
			       position);

      gtk_spin_button_set_range(connection_editor_line->output_line,
				0.0,
				(gdouble) pcm_channels - 1.0);

      gtk_spin_button_set_value(connection_editor_line->output_line,
				(gdouble) output_soundcard_channel);
    }
  }
  
  /* input soundcard */
  if((AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_INPUT & (connection_editor->flags)) != 0){
    model = gtk_combo_box_get_model(connection_editor_line->input_soundcard);

    if(GTK_IS_LIST_STORE(model)){
      gtk_list_store_clear(GTK_LIST_STORE(model));
    }

    position = 0;

    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
		       0, "NULL",
		       1, NULL,
		       -1);

    soundcard = start_soundcard;

    success = FALSE;

    position++;
  
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

      if(soundcard->data == input_soundcard){
	success = TRUE;
      }
    
      if(!success){
	position++;
      }
    
      soundcard = soundcard->next;
    }
    
    gtk_combo_box_set_active(connection_editor_line->input_soundcard,
			     0);

    if(success){
      guint pcm_channels;
      gint input_soundcard_channel;

      pcm_channels = 0;

      ags_soundcard_get_presets(AGS_SOUNDCARD(input_soundcard),
				&pcm_channels,
				NULL,
				NULL,
				NULL);  

      input_soundcard_channel = ags_channel_get_input_soundcard_channel(connection_editor_line->channel);
    
      gtk_combo_box_set_active(connection_editor_line->input_soundcard,
			       position);

      gtk_spin_button_set_range(connection_editor_line->input_line,
				0.0,
				(gdouble) pcm_channels - 1.0);

      gtk_spin_button_set_value(connection_editor_line->input_line,
				(gdouble) input_soundcard_channel);
    }
  }
  
  g_list_free_full(start_soundcard,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_connection_editor_line_new:
 * @channel_type: the channel type
 *
 * Creates an #AgsConnectionEditorLine
 *
 * Returns: a new #AgsConnectionEditorLine
 *
 * Since: 4.0.0
 */
AgsConnectionEditorLine*
ags_connection_editor_line_new(AgsChannel *channel)
{
  AgsConnectionEditorLine *connection_editor_line;

  connection_editor_line = (AgsConnectionEditorLine *) g_object_new(AGS_TYPE_CONNECTION_EDITOR_LINE,
								    "channel", channel,
								    NULL);

  return(connection_editor_line);
}
