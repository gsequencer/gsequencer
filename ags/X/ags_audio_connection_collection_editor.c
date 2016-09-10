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

#include <ags/X/ags_audio_connection_collection_editor.h>
#include <ags/X/ags_audio_connection_collection_editor_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_applicable.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_machine_editor.h>

void ags_audio_connection_collection_editor_class_init(AgsAudioConnectionCollectionEditorClass *audio_connection_collection_editor);
void ags_audio_connection_collection_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_connection_collection_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_audio_connection_collection_editor_init(AgsAudioConnectionCollectionEditor *audio_connection_collection_editor);
void ags_audio_connection_collection_editor_connect(AgsConnectable *connectable);
void ags_audio_connection_collection_editor_disconnect(AgsConnectable *connectable);
void ags_audio_connection_collection_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_audio_connection_collection_editor_apply(AgsApplicable *applicable);
void ags_audio_connection_collection_editor_reset(AgsApplicable *applicable);
void ags_audio_connection_collection_editor_show(GtkWidget *widget);

/**
 * SECTION:ags_audio_connection_collection_editor
 * @short_description: Edit audio_connections in bulk mode.
 * @title: AgsAudioConnectionCollectionEditor
 * @section_id:
 * @include: ags/X/ags_audio_connection_collection_editor.h
 *
 * #AgsAudioConnectionCollectionEditor is a composite widget to modify audio_connections in bulk mode. A audio_connection collection
 * editor should be packed by a #AgsMachineEditor.
 */

static gpointer ags_audio_connection_collection_editor_parent_class = NULL;

GType
ags_audio_connection_collection_editor_get_type(void)
{
  static GType ags_type_audio_connection_collection_editor = 0;

  if(!ags_type_audio_connection_collection_editor){
    static const GTypeInfo ags_audio_connection_collection_editor_info = {
      sizeof (AgsAudioConnectionCollectionEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_connection_collection_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioConnectionCollectionEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_connection_collection_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_connection_collection_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_audio_connection_collection_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_connection_collection_editor = g_type_register_static(GTK_TYPE_TABLE,
									 "AgsAudioConnectionCollectionEditor\0",
									 &ags_audio_connection_collection_editor_info,
									 0);
    
    g_type_add_interface_static(ags_type_audio_connection_collection_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_audio_connection_collection_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return(ags_type_audio_connection_collection_editor);
}

void
ags_audio_connection_collection_editor_class_init(AgsAudioConnectionCollectionEditorClass *audio_connection_collection_editor)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_audio_connection_collection_editor_parent_class = g_type_class_peek_parent(audio_connection_collection_editor);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_connection_collection_editor;
}

void
ags_audio_connection_collection_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_audio_connection_collection_editor_connect;
  connectable->disconnect = ags_audio_connection_collection_editor_disconnect;
}

void
ags_audio_connection_collection_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_audio_connection_collection_editor_set_update;
  applicable->apply = ags_audio_connection_collection_editor_apply;
  applicable->reset = ags_audio_connection_collection_editor_reset;
}

void
ags_audio_connection_collection_editor_init(AgsAudioConnectionCollectionEditor *audio_connection_collection_editor)
{
  GtkAlignment *alignment;
  GtkLabel *label;

  gtk_table_resize(GTK_TABLE(audio_connection_collection_editor),
		   4, 2);
  gtk_table_set_row_spacings(GTK_TABLE(audio_connection_collection_editor),
			     4);
  gtk_table_set_col_spacings(GTK_TABLE(audio_connection_collection_editor),
			     2);

  /* pad */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(audio_connection_collection_editor),
		   GTK_WIDGET(alignment),
		   0, 1,
		   0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  label = gtk_label_new("pad\0");
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(label));

  audio_connection_collection_editor->pad = gtk_spin_button_new_with_range(0.0,
									   0.0,
									   1.0);
  gtk_table_attach(GTK_TABLE(audio_connection_collection_editor),
		   GTK_WIDGET(audio_connection_collection_editor->pad),
		   1, 2,
		   0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  /* audio channel */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(audio_connection_collection_editor),
		   GTK_WIDGET(alignment),
		   0, 1,
		   1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  label = gtk_label_new("audio channel\0");
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(label));

  audio_connection_collection_editor->audio_channel = gtk_spin_button_new_with_range(0.0,
										     0.0,
										     1.0);
  gtk_table_attach(GTK_TABLE(audio_connection_collection_editor),
		   GTK_WIDGET(audio_connection_collection_editor->audio_channel),
		   0, 1,
		   1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  /* soundcard */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(audio_connection_collection_editor),
		   GTK_WIDGET(alignment),
		   0, 1,
		   2, 3,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  label = gtk_label_new("soundcard\0");
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(label));

  audio_connection_collection_editor->soundcard = gtk_combo_box_text_new();
  gtk_table_attach(GTK_TABLE(audio_connection_collection_editor),
		   GTK_WIDGET(audio_connection_collection_editor->soundcard),
		   1, 2,
		   2, 3,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  /* soundcard audio channel */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(audio_connection_collection_editor),
		   GTK_WIDGET(alignment),
		   0, 1,
		   3, 4,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  label = gtk_label_new("soundcard audio channel\0");
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(label));

  audio_connection_collection_editor->audio_channel = gtk_spin_button_new_with_range(0.0,
										     0.0,
										     1.0);
  gtk_table_attach(GTK_TABLE(audio_connection_collection_editor),
		   GTK_WIDGET(audio_connection_collection_editor->soundcard_audio_channel),
		   1, 2,
		   3, 4,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);
}

void
ags_audio_connection_collection_editor_connect(AgsConnectable *connectable)
{
  AgsAudioConnectionCollectionEditor *audio_connection_collection_editor;

  /* AgsAudioConnectionCollectionEditor */
  audio_connection_collection_editor = AGS_AUDIO_CONNECTION_COLLECTION_EDITOR(connectable);

  g_signal_connect_after(G_OBJECT(audio_connection_collection_editor->soundcard), "changed\0",
			 G_CALLBACK(ags_audio_connection_collection_editor_soundcard_callback), audio_connection_collection_editor);
}

void
ags_audio_connection_collection_editor_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_audio_connection_collection_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsAudioConnectionCollectionEditor *audio_connection_collection_editor;

  audio_connection_collection_editor = AGS_AUDIO_CONNECTION_COLLECTION_EDITOR(applicable);

  /* empty */
}

void
ags_audio_connection_collection_editor_apply(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

void
ags_audio_connection_collection_editor_reset(AgsApplicable *applicable)
{
  AgsAudioConnectionCollectionEditor *audio_connection_collection_editor;

  audio_connection_collection_editor = AGS_AUDIO_CONNECTION_COLLECTION_EDITOR(applicable);

  /* empty */
}

void
ags_audio_connection_collection_editor_show(GtkWidget *widget)
{
  AgsAudioConnectionCollectionEditor *audio_connection_collection_editor = (AgsAudioConnectionCollectionEditor *) widget;
}

/**
 * ags_audio_connection_collection_editor_new:
 *
 * Creates an #AgsAudioConnectionCollectionEditor
 *
 * Returns: a new #AgsAudioConnectionCollectionEditor
 *
 * Since: 0.7.65
 */
AgsAudioConnectionCollectionEditor*
ags_audio_connection_collection_editor_new()
{
  AgsAudioConnectionCollectionEditor *audio_connection_collection_editor;
  
  audio_connection_collection_editor = (AgsAudioConnectionCollectionEditor *) g_object_new(AGS_TYPE_AUDIO_CONNECTION_COLLECTION_EDITOR,
											   NULL);
  
  return(audio_connection_collection_editor);
}
