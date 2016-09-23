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

#include <ags/audio/task/ags_reset_audio_connection.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_connection_editor.h>

void ags_audio_connection_collection_editor_class_init(AgsAudioConnectionCollectionEditorClass *audio_connection_collection_editor);
void ags_audio_connection_collection_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_connection_collection_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_audio_connection_collection_editor_init(AgsAudioConnectionCollectionEditor *audio_connection_collection_editor);
void ags_audio_connection_collection_editor_set_property(GObject *gobject,
							 guint prop_id,
							 const GValue *value,
							 GParamSpec *param_spec);
void ags_audio_connection_collection_editor_get_property(GObject *gobject,
							 guint prop_id,
							 GValue *value,
							 GParamSpec *param_spec);
void ags_audio_connection_collection_editor_connect(AgsConnectable *connectable);
void ags_audio_connection_collection_editor_disconnect(AgsConnectable *connectable);
void ags_audio_connection_collection_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_audio_connection_collection_editor_apply(AgsApplicable *applicable);
void ags_audio_connection_collection_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_audio_connection_collection_editor
 * @short_description: edit audio connections in bulk mode.
 * @title: AgsAudioConnectionCollectionEditor
 * @section_id:
 * @include: ags/X/ags_audio_connection_collection_editor.h
 *
 * #AgsAudioConnectionCollectionEditor is a composite widget to modify audio connections. A audio_connection collection
 * editor should be packed by a #AgsConnectionEditor.
 */

static gpointer ags_audio_connection_collection_editor_parent_class = NULL;

enum{
  PROP_0,
  PROP_CHANNEL_TYPE,
};

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

  gobject->set_property = ags_audio_connection_collection_editor_set_property;
  gobject->get_property = ags_audio_connection_collection_editor_get_property;
  
  /* properties */
  /**
   * AgsAudioConnectionCollectionEditor:channel-type:
   *
   * The channel type to apply to. Either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT.
   * 
   * Since: 0.7.65
   */
  param_spec = g_param_spec_gtype("channel-type\0",
				  "assigned channel type\0",
				  "The channel type which this channel audio_connection collection editor is assigned with\0",
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL_TYPE,
				  param_spec);
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
  
  g_signal_connect_after(GTK_WIDGET(audio_connection_collection_editor), "parent-set\0",
			 G_CALLBACK(ags_audio_connection_collection_editor_parent_set_callback), audio_connection_collection_editor);

  audio_connection_collection_editor->channel_type = G_TYPE_NONE;
  
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

  label = (GtkLabel *) gtk_label_new("pad\0");
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(label));

  audio_connection_collection_editor->pad = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0,
											     -1.0,
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

  label = (GtkLabel *) gtk_label_new("audio channel\0");
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(label));

  audio_connection_collection_editor->audio_channel = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0,
												       -1.0,
												       1.0);
  gtk_table_attach(GTK_TABLE(audio_connection_collection_editor),
		   GTK_WIDGET(audio_connection_collection_editor->audio_channel),
		   1, 2,
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

  label = (GtkLabel *) gtk_label_new("soundcard\0");
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(label));

  audio_connection_collection_editor->soundcard = (GtkComboBoxText *) gtk_combo_box_text_new();
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

  label = (GtkLabel *) gtk_label_new("soundcard audio channel\0");
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(label));

  audio_connection_collection_editor->soundcard_audio_channel = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0,
														 -1.0,
														 1.0);
  gtk_table_attach(GTK_TABLE(audio_connection_collection_editor),
		   GTK_WIDGET(audio_connection_collection_editor->soundcard_audio_channel),
		   1, 2,
		   3, 4,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);
}


void
ags_audio_connection_collection_editor_set_property(GObject *gobject,
						    guint prop_id,
						    const GValue *value,
						    GParamSpec *param_spec)
{
  AgsAudioConnectionCollectionEditor *audio_connection_collection_editor;

  audio_connection_collection_editor = AGS_AUDIO_CONNECTION_COLLECTION_EDITOR(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
    audio_connection_collection_editor->channel_type = g_value_get_gtype(value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_connection_collection_editor_get_property(GObject *gobject,
						    guint prop_id,
						    GValue *value,
						    GParamSpec *param_spec)
{
  AgsAudioConnectionCollectionEditor *audio_connection_collection_editor;

  audio_connection_collection_editor = AGS_AUDIO_CONNECTION_COLLECTION_EDITOR(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
    g_value_set_gtype(value, audio_connection_collection_editor->channel_type);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
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
  AgsAudioConnectionCollectionEditor *audio_connection_collection_editor;
  GtkTreeIter iter;

  audio_connection_collection_editor = AGS_AUDIO_CONNECTION_COLLECTION_EDITOR(applicable);

  if(gtk_combo_box_get_active_iter(GTK_COMBO_BOX(audio_connection_collection_editor->soundcard),
				   &iter)){
    AgsWindow *window;
    AgsMachine *machine;
    AgsConnectionEditor *connection_editor;

    AgsAudio *audio;

    AgsResetAudioConnection *reset_audio_connection;
    
    AgsMutexManager *mutex_manager;
    AgsAudioLoop *audio_loop;
    AgsTaskThread *task_thread;

    AgsApplicationContext *application_context;
    GObject *soundcard;

    GtkTreeModel *model;

    guint pad, audio_channel;
    guint soundcard_audio_channel;

    pthread_mutex_t *application_mutex;
    pthread_mutex_t *audio_mutex;

    connection_editor = AGS_CONNECTION_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(audio_connection_collection_editor),
								      AGS_TYPE_CONNECTION_EDITOR));
    machine = connection_editor->machine;
    audio = machine->audio;

    /* get window and application_context  */
    window = (AgsWindow *) gtk_widget_get_toplevel(GTK_WIDGET(machine));
  
    application_context = (AgsApplicationContext *) window->application_context;
    
    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
    
    /* get audio loop */
    pthread_mutex_lock(application_mutex);

    audio_loop = (AgsAudioLoop *) application_context->main_loop;

    pthread_mutex_unlock(application_mutex);

    /* get task and soundcard thread */
    task_thread = (AgsTaskThread *) ags_thread_find_type((AgsThread *) audio_loop,
							 AGS_TYPE_TASK_THREAD);

    /* lookup audio mutex */
    pthread_mutex_lock(application_mutex);
    
    mutex_manager = ags_mutex_manager_get_instance();
    
    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
    
    pthread_mutex_unlock(application_mutex);

    /* get mapping and soundcard */
    pad = (guint) gtk_spin_button_get_value_as_int(audio_connection_collection_editor->pad);
    audio_channel = (guint) gtk_spin_button_get_value_as_int(audio_connection_collection_editor->audio_channel);

    soundcard_audio_channel = (guint) gtk_spin_button_get_value_as_int(audio_connection_collection_editor->soundcard_audio_channel);

    model = gtk_combo_box_get_model(GTK_COMBO_BOX(audio_connection_collection_editor->soundcard));
    gtk_tree_model_get(model,
		       &iter,
		       1, &soundcard,
		       -1);

    /* create task */
    reset_audio_connection = ags_reset_audio_connection_new(soundcard,
							    audio,
							    audio_connection_collection_editor->channel_type,
							    pad,
							    audio_channel,
							    soundcard_audio_channel);
    ags_task_thread_append_task(task_thread,
				(AgsTask *) reset_audio_connection);
  }
}

void
ags_audio_connection_collection_editor_reset(AgsApplicable *applicable)
{
  AgsAudioConnectionCollectionEditor *audio_connection_collection_editor;

  audio_connection_collection_editor = AGS_AUDIO_CONNECTION_COLLECTION_EDITOR(applicable);

  /* empty */
}

/**
 * ags_audio_connection_collection_editor_check:
 * @audio_connection_collection_editor: the #AgsAudio_ConnectionCollectionEditor
 *
 * Checks for possible channels to audio_connection. And modifies its ranges.
 * 
 * Since: 0.7.65
 */
void
ags_audio_connection_collection_editor_check(AgsAudioConnectionCollectionEditor *audio_connection_collection_editor)
{
  AgsConnectionEditor *connection_editor;

  AgsAudio *audio;
  
  GtkTreeIter iter;

  connection_editor = AGS_CONNECTION_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(audio_connection_collection_editor),
								    AGS_TYPE_CONNECTION_EDITOR));
  audio = connection_editor->machine->audio;

  if(audio_connection_collection_editor->channel_type == AGS_TYPE_INPUT){
    gtk_spin_button_set_range(audio_connection_collection_editor->pad,
			      0.0,
			      audio->input_pads - 1.0);
  }else{
    gtk_spin_button_set_range(audio_connection_collection_editor->pad,
			      0.0,
			      audio->output_pads - 1.0);
  }

  gtk_spin_button_set_range(audio_connection_collection_editor->audio_channel,
			    0.0,
			    audio->audio_channels - 1.0);

  if(gtk_combo_box_get_active_iter(GTK_COMBO_BOX(audio_connection_collection_editor->soundcard),
				   &iter)){    
    GObject *soundcard;
    
    GtkTreeModel *model;

    guint soundcard_audio_channels;
    
    /* soundcard connection */
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(audio_connection_collection_editor->soundcard));
    gtk_tree_model_get(model,
		       &iter,
		       1, &soundcard,
		       -1);

    ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			      &soundcard_audio_channels,
			      NULL,
			      NULL,
			      NULL);

    gtk_spin_button_set_range(audio_connection_collection_editor->soundcard_audio_channel,
			      0.0,
			      soundcard_audio_channels - 1.0);
  }else{
    gtk_spin_button_set_range(audio_connection_collection_editor->soundcard_audio_channel,
			      -1.0, -1.0);
  }
}

/**
 * ags_audio_connection_collection_editor_new:
 * @channel_type: either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT
 *
 * Creates an #AgsAudioConnectionCollectionEditor
 *
 * Returns: a new #AgsAudioConnectionCollectionEditor
 *
 * Since: 0.7.65
 */
AgsAudioConnectionCollectionEditor*
ags_audio_connection_collection_editor_new(GType channel_type)
{
  AgsAudioConnectionCollectionEditor *audio_connection_collection_editor;
  
  audio_connection_collection_editor = (AgsAudioConnectionCollectionEditor *) g_object_new(AGS_TYPE_AUDIO_CONNECTION_COLLECTION_EDITOR,
											   "channel_type\0", channel_type,
											   NULL);
  
  return(audio_connection_collection_editor);
}
