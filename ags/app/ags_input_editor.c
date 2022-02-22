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

#include <ags/app/ags_input_editor.h>
#include <ags/app/ags_input_editor_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_connection_editor.h>
#include <ags/app/ags_line_editor.h>

#include <ags/i18n.h>

void ags_input_editor_class_init(AgsInputEditorClass *input_editor);
void ags_input_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_input_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_input_editor_init(AgsInputEditor *input_editor);

void ags_input_editor_connect(AgsConnectable *connectable);
void ags_input_editor_disconnect(AgsConnectable *connectable);

void ags_input_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_input_editor_apply(AgsApplicable *applicable);
void ags_input_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_input_editor
 * @short_description: A composite widget to edit #AgsChannel
 * @title: AgsInputEditor
 * @section_id:
 * @include: ags/app/ags_input_editor.h
 *
 * #AgsInputEditor is a composite widget to edit #AgsChannel.
 */

static gpointer ags_input_editor_parent_class = NULL;

GType
ags_input_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_input_editor = 0;

    static const GTypeInfo ags_input_editor_info = {
      sizeof (AgsInputEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_input_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsInputEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_input_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_input_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_input_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_input_editor = g_type_register_static(GTK_TYPE_BOX,
						   "AgsInputEditor", &ags_input_editor_info,
						   0);

    g_type_add_interface_static(ags_type_input_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_input_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_input_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_input_editor_class_init(AgsInputEditorClass *input_editor)
{
  ags_input_editor_parent_class = g_type_class_peek_parent(input_editor);
}

void
ags_input_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_input_editor_connect;
  connectable->disconnect = ags_input_editor_disconnect;
}

void
ags_input_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_input_editor_set_update;
  applicable->apply = ags_input_editor_apply;
  applicable->reset = ags_input_editor_reset;
}

void
ags_input_editor_init(AgsInputEditor *input_editor)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(input_editor),
				 GTK_ORIENTATION_HORIZONTAL);
  
  g_signal_connect_after((GObject *) input_editor, "parent_set",
			 G_CALLBACK(ags_input_editor_parent_set_callback), (gpointer) input_editor);

  input_editor->flags = 0;

  input_editor->version = AGS_INPUT_EDITOR_DEFAULT_VERSION;
  input_editor->build_id = AGS_INPUT_EDITOR_DEFAULT_BUILD_ID;

  /* connecting soundcard */
  input_editor->soundcard = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_pack_start(GTK_BOX(input_editor),
		     GTK_WIDGET(input_editor->soundcard),
		     FALSE, FALSE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);

  /* connect with line */
  input_editor->audio_channel = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_box_pack_start(GTK_BOX(input_editor),
		     GTK_WIDGET(input_editor->audio_channel),
		     FALSE, FALSE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);
}

void
ags_input_editor_connect(AgsConnectable *connectable)
{
  AgsInputEditor *input_editor;

  input_editor = AGS_INPUT_EDITOR(connectable);

  if((AGS_INPUT_EDITOR_CONNECTED & (input_editor->flags)) != 0){
    return;
  }

  input_editor->flags |= AGS_INPUT_EDITOR_CONNECTED;

  g_signal_connect(G_OBJECT(input_editor->soundcard), "changed",
		   G_CALLBACK(ags_input_editor_soundcard_callback), input_editor);
}

void
ags_input_editor_disconnect(AgsConnectable *connectable)
{
  AgsInputEditor *input_editor;

  input_editor = AGS_INPUT_EDITOR(connectable);

  if((AGS_INPUT_EDITOR_CONNECTED & (input_editor->flags)) == 0){
    return;
  }

  input_editor->flags &= (~AGS_INPUT_EDITOR_CONNECTED);

  g_object_disconnect(G_OBJECT(input_editor->soundcard),
		      "any_signal::changed",
		      G_CALLBACK(ags_input_editor_soundcard_callback),
		      input_editor,
		      NULL);
}

void
ags_input_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_input_editor_apply(AgsApplicable *applicable)
{
  AgsInputEditor *input_editor;
  AgsLineEditor *line_editor;

  GtkTreeIter iter;

  input_editor = AGS_INPUT_EDITOR(applicable);

  if(gtk_combo_box_get_active_iter(GTK_COMBO_BOX(input_editor->soundcard),
				   &iter)){
    AgsChannel *channel;

    GObject *input_soundcard;

    GtkTreeModel *model;

    guint soundcard_channel;
    guint pad, audio_channel;

    line_editor = AGS_LINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(input_editor),
							  AGS_TYPE_LINE_EDITOR));

    channel = line_editor->channel;

    /* get channel fields */
    g_object_get(channel,
		 "pad", &pad,
		 "audio-channel", &audio_channel,
		 NULL);

    /* get mapping and soundcard */
    soundcard_channel = (guint) gtk_spin_button_get_value_as_int(input_editor->audio_channel);

    model = gtk_combo_box_get_model(GTK_COMBO_BOX(input_editor->soundcard));
    gtk_tree_model_get(model,
		       &iter,
		       1, &input_soundcard,
		       -1);

    /* create task */
    g_object_set(channel,
		 "input-soundcard", input_soundcard,
		 "input-soundcard-channel", soundcard_channel, 
		 NULL);
  }
}

void
ags_input_editor_reset(AgsApplicable *applicable)
{
  AgsInputEditor *input_editor;

  GtkTreeModel *model;

  GtkTreeIter iter;

  input_editor = AGS_INPUT_EDITOR(applicable);

  model = gtk_combo_box_get_model(GTK_COMBO_BOX(input_editor->soundcard));

  if(gtk_tree_model_get_iter_first(model,
				   &iter)){
    AgsLineEditor *line_editor;
    
    AgsAudio *audio;
    AgsChannel *channel;
    
    GObject *input_soundcard, *current;

    gint i;
    gboolean found;

    line_editor = AGS_LINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(input_editor),
							  AGS_TYPE_LINE_EDITOR));

    input_soundcard = NULL;
    audio = NULL;
    channel = line_editor->channel;
    
    if(channel != NULL){
      g_object_get(channel,
		   "audio", &audio,
		   "input-soundcard", &input_soundcard,
		   NULL);
    }
    
    i = 0;
    found = FALSE;

    if(input_soundcard != NULL){
      do{
	gtk_tree_model_get(model,
			   &iter,
			   1, &current,
			   -1);

	if(input_soundcard == current){
	  found = TRUE;
	  break;
	}

	i++;
      }while(gtk_tree_model_iter_next(model,
				      &iter));
    }

    if(found &&
       audio != NULL){
      guint audio_channel;
      
      /* set channel link */
      gtk_combo_box_set_active(GTK_COMBO_BOX(input_editor->soundcard),
			       i);

      g_object_get(channel,
		   "input-soundcard-channel", &audio_channel,
		   NULL);
      
      gtk_spin_button_set_value(input_editor->audio_channel,
				audio_channel);
    }else{
      gtk_combo_box_set_active(GTK_COMBO_BOX(input_editor->soundcard),
			       0);
    }
    
    /* unref */
    if(audio != NULL){
      g_object_unref(audio);
    }

    if(input_soundcard != NULL){
      g_object_unref(input_soundcard);
    }
  }
}

/**
 * ags_input_editor_check:
 * @input_editor: the #AgsInputEditor
 *
 * Checks for possible channels to input. And modifies its ranges.
 * 
 * Since: 3.0.0
 */
void
ags_input_editor_check(AgsInputEditor *input_editor)
{
  GtkTreeIter iter;

  if(gtk_combo_box_get_active_iter(GTK_COMBO_BOX(input_editor->soundcard),
				   &iter)){    
    GObject *input_soundcard;
    
    GtkTreeModel *model;

    guint audio_channels;
    
    /* soundcard connection */
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(input_editor->soundcard));
    gtk_tree_model_get(model,
		       &iter,
		       1, &input_soundcard,
		       -1);

    if(input_soundcard != NULL){
      ags_soundcard_get_presets(AGS_SOUNDCARD(input_soundcard),
				&audio_channels,
				NULL,
				NULL,
				NULL);

      gtk_spin_button_set_range(input_editor->audio_channel,
				0.0,
				audio_channels - 1.0);
    }else{
      gtk_spin_button_set_range(input_editor->audio_channel,
				-1.0, -1.0);
    }
  }else{
    gtk_spin_button_set_range(input_editor->audio_channel,
			      -1.0, -1.0);
  }
}

/**
 * ags_input_editor_new:
 *
 * Create a new instance of #AgsInputEditor
 *
 * Returns: the new #AgsInputEditor
 *
 * Since: 3.0.0
 */
AgsInputEditor*
ags_input_editor_new()
{
  AgsInputEditor *input_editor;

  input_editor = (AgsInputEditor *) g_object_new(AGS_TYPE_INPUT_EDITOR,
						 NULL);

  return(input_editor);
}
