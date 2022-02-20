/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/app/ags_output_collection_editor.h>
#include <ags/app/ags_output_collection_editor_callbacks.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_connection_editor.h>

#include <ags/i18n.h>

void ags_output_collection_editor_class_init(AgsOutputCollectionEditorClass *output_collection_editor);
void ags_output_collection_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_output_collection_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_output_collection_editor_init(AgsOutputCollectionEditor *output_collection_editor);
void ags_output_collection_editor_set_property(GObject *gobject,
					       guint prop_id,
					       const GValue *value,
					       GParamSpec *param_spec);
void ags_output_collection_editor_get_property(GObject *gobject,
					       guint prop_id,
					       GValue *value,
					       GParamSpec *param_spec);

void ags_output_collection_editor_connect(AgsConnectable *connectable);
void ags_output_collection_editor_disconnect(AgsConnectable *connectable);

void ags_output_collection_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_output_collection_editor_apply(AgsApplicable *applicable);
void ags_output_collection_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_output_collection_editor
 * @short_description: edit audio connections in bulk mode.
 * @title: AgsOutputCollectionEditor
 * @section_id:
 * @include: ags/app/ags_output_collection_editor.h
 *
 * #AgsOutputCollectionEditor is a composite widget to modify audio connections. A output collection
 * editor should be packed by a #AgsConnectionEditor.
 */

static gpointer ags_output_collection_editor_parent_class = NULL;

enum{
  PROP_0,
  PROP_CHANNEL_TYPE,
};

GType
ags_output_collection_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_output_collection_editor = 0;

    static const GTypeInfo ags_output_collection_editor_info = {
      sizeof (AgsOutputCollectionEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_output_collection_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOutputCollectionEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_output_collection_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_output_collection_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_output_collection_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_output_collection_editor = g_type_register_static(GTK_TYPE_GRID,
							       "AgsOutputCollectionEditor",
							       &ags_output_collection_editor_info,
							       0);
    
    g_type_add_interface_static(ags_type_output_collection_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_output_collection_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_output_collection_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_output_collection_editor_class_init(AgsOutputCollectionEditorClass *output_collection_editor)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_output_collection_editor_parent_class = g_type_class_peek_parent(output_collection_editor);

  /* GObjectClass */
  gobject = (GObjectClass *) output_collection_editor;

  gobject->set_property = ags_output_collection_editor_set_property;
  gobject->get_property = ags_output_collection_editor_get_property;
  
  /* properties */
  /**
   * AgsOutputCollectionEditor:channel-type:
   *
   * The channel type to apply to. Either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_gtype("channel-type",
				  i18n_pspec("assigned channel type"),
				  i18n_pspec("The channel type which this channel output collection editor is assigned with"),
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL_TYPE,
				  param_spec);
}

void
ags_output_collection_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_output_collection_editor_connect;
  connectable->disconnect = ags_output_collection_editor_disconnect;
}

void
ags_output_collection_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_output_collection_editor_set_update;
  applicable->apply = ags_output_collection_editor_apply;
  applicable->reset = ags_output_collection_editor_reset;
}

void
ags_output_collection_editor_init(AgsOutputCollectionEditor *output_collection_editor)
{
  GtkLabel *label;

  output_collection_editor->flags = 0;
    
  g_signal_connect_after(GTK_WIDGET(output_collection_editor), "parent-set",
			 G_CALLBACK(ags_output_collection_editor_parent_set_callback), output_collection_editor);

  output_collection_editor->channel_type = G_TYPE_NONE;

  /* first line */
  label = (GtkLabel *) gtk_label_new(i18n("first line"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_label_set_yalign(label,
		       0.5);
  
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) label,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) label,
			 TRUE);
  
  gtk_grid_attach((GtkGrid *) output_collection_editor,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);

  output_collection_editor->first_line = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0,
											  -1.0,
											  1.0);
  
  gtk_widget_set_valign((GtkWidget *) output_collection_editor->first_line,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) output_collection_editor->first_line,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) output_collection_editor->first_line,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) output_collection_editor->first_line,
			 TRUE);

  gtk_grid_attach((GtkGrid *) output_collection_editor,
		  (GtkWidget *) output_collection_editor->first_line,
		  1, 0,
		  1, 1);

  /* count */
  label = (GtkLabel *) gtk_label_new(i18n("count"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_label_set_yalign(label,
		       0.5);
  
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) label,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) label,
			 TRUE);

  gtk_grid_attach((GtkGrid *) output_collection_editor,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  output_collection_editor->count = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0,
										     -1.0,
										     1.0);
  
  gtk_widget_set_valign((GtkWidget *) output_collection_editor->count,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) output_collection_editor->count,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) output_collection_editor->count,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) output_collection_editor->count,
			 TRUE);

  gtk_grid_attach((GtkGrid *) output_collection_editor,
		  (GtkWidget *) output_collection_editor->count,
		  1, 1,
		  1, 1);

  /* soundcard */
  label = (GtkLabel *) gtk_label_new(i18n("soundcard"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_label_set_yalign(label,
		       0.5);
  
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) label,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) label,
			 TRUE);

  gtk_grid_attach((GtkGrid *) output_collection_editor,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  output_collection_editor->soundcard = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_widget_set_valign((GtkWidget *) output_collection_editor->soundcard,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) output_collection_editor->soundcard,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) output_collection_editor->soundcard,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) output_collection_editor->soundcard,
			 TRUE);

  gtk_grid_attach((GtkGrid *) output_collection_editor,
		  (GtkWidget *) output_collection_editor->soundcard,
		  1, 2,
		  1, 1);

  /* audio channel */
  label = (GtkLabel *) gtk_label_new(i18n("audio channel"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_label_set_yalign(label,
		       0.5);
  
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) label,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) label,
			 TRUE);

  gtk_grid_attach((GtkGrid *) output_collection_editor,
		  (GtkWidget *) label,
		   0, 3,
		   1, 1);

  output_collection_editor->audio_channel = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0,
											     -1.0,
											     1.0);

  gtk_widget_set_valign((GtkWidget *) output_collection_editor->audio_channel,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) output_collection_editor->audio_channel,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) output_collection_editor->audio_channel,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) output_collection_editor->audio_channel,
			 TRUE);
  
  gtk_grid_attach((GtkGrid *) output_collection_editor,
		  (GtkWidget *) output_collection_editor->audio_channel,
		  1, 3,
		  1, 1);
}


void
ags_output_collection_editor_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec)
{
  AgsOutputCollectionEditor *output_collection_editor;

  output_collection_editor = AGS_OUTPUT_COLLECTION_EDITOR(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
    {
      output_collection_editor->channel_type = g_value_get_gtype(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_output_collection_editor_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec)
{
  AgsOutputCollectionEditor *output_collection_editor;

  output_collection_editor = AGS_OUTPUT_COLLECTION_EDITOR(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
    {
      g_value_set_gtype(value, output_collection_editor->channel_type);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_output_collection_editor_connect(AgsConnectable *connectable)
{
  AgsOutputCollectionEditor *output_collection_editor;

  /* AgsOutputCollectionEditor */
  output_collection_editor = AGS_OUTPUT_COLLECTION_EDITOR(connectable);

  if((AGS_OUTPUT_COLLECTION_EDITOR_CONNECTED & (output_collection_editor->flags)) != 0){
    return;
  }

  output_collection_editor->flags |= AGS_OUTPUT_COLLECTION_EDITOR_CONNECTED;
  
  g_signal_connect_after(G_OBJECT(output_collection_editor->soundcard), "changed",
			 G_CALLBACK(ags_output_collection_editor_soundcard_callback), output_collection_editor);
}

void
ags_output_collection_editor_disconnect(AgsConnectable *connectable)
{
  AgsOutputCollectionEditor *output_collection_editor;

  /* AgsOutputCollectionEditor */
  output_collection_editor = AGS_OUTPUT_COLLECTION_EDITOR(connectable);

  if((AGS_OUTPUT_COLLECTION_EDITOR_CONNECTED & (output_collection_editor->flags)) == 0){
    return;
  }

  output_collection_editor->flags &= (~AGS_OUTPUT_COLLECTION_EDITOR_CONNECTED);
  
  g_object_disconnect(G_OBJECT(output_collection_editor->soundcard),
		      "any_signal::changed",
		      G_CALLBACK(ags_output_collection_editor_soundcard_callback),
		      output_collection_editor,
		      NULL);
}

void
ags_output_collection_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_output_collection_editor_apply(AgsApplicable *applicable)
{
  AgsOutputCollectionEditor *output_collection_editor;
  GtkTreeIter iter;

  output_collection_editor = AGS_OUTPUT_COLLECTION_EDITOR(applicable);

  if(gtk_combo_box_get_active_iter(GTK_COMBO_BOX(output_collection_editor->soundcard),
				   &iter)){
    AgsMachine *machine;
    AgsConnectionEditor *connection_editor;

    AgsAudio *audio;
    AgsChannel *start_output, *start_input;
    AgsChannel *channel, *nth_channel;
    
    GObject *output_soundcard;

    GtkTreeModel *model;

    guint first_line, count;
    guint audio_channel;
    guint i;

    connection_editor = AGS_CONNECTION_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(output_collection_editor),
								      AGS_TYPE_CONNECTION_EDITOR));
    machine = connection_editor->machine;
    audio = machine->audio;

    /* get mapping and soundcard */
    first_line = (guint) gtk_spin_button_get_value_as_int(output_collection_editor->first_line);
    count = (guint) gtk_spin_button_get_value_as_int(output_collection_editor->count);

    audio_channel = (guint) gtk_spin_button_get_value_as_int(output_collection_editor->audio_channel);

    model = gtk_combo_box_get_model(GTK_COMBO_BOX(output_collection_editor->soundcard));
    gtk_tree_model_get(model,
		       &iter,
		       1, &output_soundcard,
		       -1);

    /* apply */
    g_object_get(audio,
		 "output", &start_output,
		 "input", &start_input,
		 NULL);
    
    for(i = 0; i < count; i++){
      channel = NULL;
      
      if(g_type_is_a(output_collection_editor->channel_type, AGS_TYPE_OUTPUT)){
	channel = start_output;
      }else if(g_type_is_a(output_collection_editor->channel_type, AGS_TYPE_INPUT)){
	channel = start_input;
      }

      nth_channel = ags_channel_nth(channel,
				    first_line + i);

      g_object_set(nth_channel,
		   "output-soundcard", output_soundcard,
		   "output-soundcard-channel", audio_channel,
		   NULL);

      g_object_unref(nth_channel);
    }

    if(start_output != NULL){
      g_object_unref(start_output);
    }

    if(start_input != NULL){
      g_object_unref(start_input);
    }
  }
}

void
ags_output_collection_editor_reset(AgsApplicable *applicable)
{
  /* empty */
}

/**
 * ags_output_collection_editor_check:
 * @output_collection_editor: the #AgsOutputCollectionEditor
 *
 * Checks for possible channels to output. And modifies its ranges.
 * 
 * Since: 3.0.0
 */
void
ags_output_collection_editor_check(AgsOutputCollectionEditor *output_collection_editor)
{
  AgsConnectionEditor *connection_editor;

  AgsAudio *audio;

  GtkTreeIter iter;
  
  guint count;

  connection_editor = AGS_CONNECTION_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(output_collection_editor),
								    AGS_TYPE_CONNECTION_EDITOR));
  audio = connection_editor->machine->audio;

  /* get audio fields */
  if(g_type_is_a(output_collection_editor->channel_type, AGS_TYPE_INPUT)){
    g_object_get(audio,
		 "input-lines", &count,
		 NULL);
  }else if(g_type_is_a(output_collection_editor->channel_type, AGS_TYPE_OUTPUT)){
    g_object_get(audio,
		 "output-lines", &count,
		 NULL);
  }else{
    count = 0;
  }

  gtk_spin_button_set_range(output_collection_editor->first_line,
			    0.0,
			    count - 1.0);
  gtk_spin_button_set_range(output_collection_editor->count,
			    0.0,
			    count);

  if(gtk_combo_box_get_active_iter(GTK_COMBO_BOX(output_collection_editor->soundcard),
				   &iter)){    
    GObject *output_soundcard;
    
    GtkTreeModel *model;

    guint audio_channels;
    
    /* soundcard connection */
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(output_collection_editor->soundcard));
    gtk_tree_model_get(model,
		       &iter,
		       1, &output_soundcard,
		       -1);

    ags_soundcard_get_presets(AGS_SOUNDCARD(output_soundcard),
			      &audio_channels,
			      NULL,
			      NULL,
			      NULL);

    gtk_spin_button_set_range(output_collection_editor->audio_channel,
			      0.0,
			      audio_channels - 1.0);

    if(audio_channels < count){
      gtk_spin_button_set_range(output_collection_editor->count,
				0.0,
				audio_channels);
    }
  }else{
    gtk_spin_button_set_range(output_collection_editor->audio_channel,
			      -1.0, -1.0);
  }
}

/**
 * ags_output_collection_editor_new:
 * @channel_type: either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT
 *
 * Create a new instance of #AgsOutputCollectionEditor
 *
 * Returns: the new #AgsOutputCollectionEditor
 *
 * Since: 3.0.0
 */
AgsOutputCollectionEditor*
ags_output_collection_editor_new(GType channel_type)
{
  AgsOutputCollectionEditor *output_collection_editor;
  
  output_collection_editor = (AgsOutputCollectionEditor *) g_object_new(AGS_TYPE_OUTPUT_COLLECTION_EDITOR,
									"channel_type", channel_type,
									NULL);
  
  return(output_collection_editor);
}
