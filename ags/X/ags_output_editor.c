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

#include <ags/X/ags_output_editor.h>
#include <ags/X/ags_output_editor_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_audio_connection.h>
#include <ags/audio/ags_input.h>

#include <ags/audio/task/ags_reset_audio_connection.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_connection_editor.h>
#include <ags/X/ags_line_editor.h>

void ags_output_editor_class_init(AgsOutputEditorClass *output_editor);
void ags_output_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_output_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_output_editor_init(AgsOutputEditor *output_editor);
void ags_output_editor_connect(AgsConnectable *connectable);
void ags_output_editor_disconnect(AgsConnectable *connectable);
void ags_output_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_output_editor_apply(AgsApplicable *applicable);
void ags_output_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_output_editor
 * @short_description: A composite widget to edit #AgsChannel
 * @title: AgsOutputEditor
 * @section_id:
 * @include: ags/X/ags_output_editor.h
 *
 * #AgsOutputEditor is a composite widget to edit #AgsChannel.
 */

static gpointer ags_output_editor_parent_class = NULL;

GType
ags_output_editor_get_type(void)
{
  static GType ags_type_output_editor = 0;

  if(!ags_type_output_editor){
    static const GTypeInfo ags_output_editor_info = {
      sizeof (AgsOutputEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_output_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOutputEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_output_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_output_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_output_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_output_editor = g_type_register_static(GTK_TYPE_HBOX,
						    "AgsOutputEditor\0", &ags_output_editor_info,
						    0);

    g_type_add_interface_static(ags_type_output_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_output_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_output_editor);
}

void
ags_output_editor_class_init(AgsOutputEditorClass *output_editor)
{
  ags_output_editor_parent_class = g_type_class_peek_parent(output_editor);
}

void
ags_output_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_output_editor_connect;
  connectable->disconnect = ags_output_editor_disconnect;
}

void
ags_output_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_output_editor_set_update;
  applicable->apply = ags_output_editor_apply;
  applicable->reset = ags_output_editor_reset;
}

void
ags_output_editor_init(AgsOutputEditor *output_editor)
{
  g_signal_connect_after((GObject *) output_editor, "parent_set\0",
			 G_CALLBACK(ags_output_editor_parent_set_callback), (gpointer) output_editor);

  output_editor->flags = 0;

  output_editor->version = AGS_OUTPUT_EDITOR_DEFAULT_VERSION;
  output_editor->build_id = AGS_OUTPUT_EDITOR_DEFAULT_BUILD_ID;

  /* connecting soundcard */
  output_editor->soundcard = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_pack_start(GTK_BOX(output_editor),
		     GTK_WIDGET(output_editor->soundcard),
		     FALSE, FALSE,
		     0);

  /* connect with line */
  output_editor->audio_channel = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_box_pack_start(GTK_BOX(output_editor),
		     GTK_WIDGET(output_editor->audio_channel),
		     FALSE, FALSE,
		     0);
}

void
ags_output_editor_connect(AgsConnectable *connectable)
{
  AgsOutputEditor *output_editor;

  output_editor = AGS_OUTPUT_EDITOR(connectable);

  if((AGS_OUTPUT_EDITOR_CONNECTED & (output_editor->flags)) != 0){
    return;
  }

  output_editor->flags |= AGS_OUTPUT_EDITOR_CONNECTED;

  g_signal_connect(G_OBJECT(output_editor->soundcard), "changed\0",
		   G_CALLBACK(ags_output_editor_soundcard_callback), output_editor);
}

void
ags_output_editor_disconnect(AgsConnectable *connectable)
{
  AgsOutputEditor *output_editor;

  output_editor = AGS_OUTPUT_EDITOR(connectable);

  if((AGS_OUTPUT_EDITOR_CONNECTED & (output_editor->flags)) == 0){
    return;
  }

  output_editor->flags &= (~AGS_OUTPUT_EDITOR_CONNECTED);

  g_object_disconnect(G_OBJECT(output_editor->soundcard),
		      "changed\0",
		      G_CALLBACK(ags_output_editor_soundcard_callback),
		      output_editor,
		      NULL);
}

void
ags_output_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_output_editor_apply(AgsApplicable *applicable)
{
  AgsOutputEditor *output_editor;
  AgsLineEditor *line_editor;

  GtkTreeIter iter;

  output_editor = AGS_OUTPUT_EDITOR(applicable);

  if(gtk_combo_box_get_active_iter(GTK_COMBO_BOX(output_editor->soundcard),
				   &iter)){
    AgsWindow *window;
    AgsMachine *machine;
    AgsConnectionEditor *connection_editor;

    AgsAudio *audio;
    AgsChannel *channel;

    AgsResetAudioConnection *reset_audio_connection;
    
    AgsMutexManager *mutex_manager;
    AgsAudioLoop *audio_loop;
    AgsTaskThread *task_thread;

    AgsApplicationContext *application_context;
    GObject *soundcard;

    GtkTreeModel *model;

    guint soundcard_channel;
    guint pad, audio_channel;
    
    pthread_mutex_t *application_mutex;
    pthread_mutex_t *channel_mutex;

    connection_editor = AGS_CONNECTION_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(output_editor),
								      AGS_TYPE_CONNECTION_EDITOR));
    machine = connection_editor->machine;
    audio = machine->audio;
    channel = line_editor->channel;
    
    /* get window and application_context  */
    window = (AgsWindow *) gtk_widget_get_toplevel(GTK_WIDGET(machine));
  
    application_context = (AgsApplicationContext *) window->application_context;
    
    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
    
    /* get audio loop */
    pthread_mutex_lock(application_mutex);

    audio_loop = (AgsAudioLoop *) application_context->main_loop;

    pthread_mutex_unlock(application_mutex);

    /* lookup channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     channel);

    pthread_mutex_unlock(application_mutex);

    /* get audio fields */
    pthread_mutex_lock(channel_mutex);

    pad = channel->pad;
    audio_channel = channel->audio_channel;
    
    pthread_mutex_unlock(channel_mutex);

    /* get task and soundcard thread */
    task_thread = (AgsTaskThread *) ags_thread_find_type((AgsThread *) audio_loop,
							 AGS_TYPE_TASK_THREAD);

    /* get mapping and soundcard */
    soundcard_channel = (guint) gtk_spin_button_get_value_as_int(output_editor->audio_channel);

    model = gtk_combo_box_get_model(GTK_COMBO_BOX(output_editor->soundcard));
    gtk_tree_model_get(model,
		       &iter,
		       1, &soundcard,
		       -1);

    /* create task */
    reset_audio_connection = ags_reset_audio_connection_new(soundcard,
							    audio,
							    G_OBJECT_TYPE(channel),
							    pad,
							    audio_channel,
							    soundcard_channel);
    
    ags_task_thread_append_task(task_thread,
				(AgsTask *) reset_audio_connection);
  }
}

void
ags_output_editor_reset(AgsApplicable *applicable)
{
  AgsOutputEditor *output_editor;

  GtkTreeModel *model;

  GtkTreeIter iter;

  output_editor = AGS_OUTPUT_EDITOR(applicable);

  model = gtk_combo_box_get_model(output_editor->soundcard);

  if(gtk_tree_model_get_iter_first(model,
				   &iter)){
    AgsLineEditor *line_editor;
    
    AgsAudio *audio;
    AgsAudioConnection *audio_connection;
    AgsChannel *channel;
    
    GObject *soundcard, *current;

    GList *list;
    
    gint i;
    gboolean found;

    line_editor = AGS_LINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(output_editor),
							  AGS_TYPE_LINE_EDITOR));

    soundcard = NULL;
    audio = NULL;
    channel = line_editor->channel;
    
    if(channel != NULL){
      soundcard = channel->soundcard;
      audio = AGS_AUDIO(channel->audio);
    }
    
    i = 0;
    found = FALSE;

    if(soundcard != NULL){
      do{
	gtk_tree_model_get(model,
			   &iter,
			   1, &current,
			   -1);

	if(soundcard == current){
	  found = TRUE;
	  break;
	}

	i++;
      }while(gtk_tree_model_iter_next(model,
				      &iter));
    }

    if(found &&
       audio != NULL){      
      /* set channel link */
      gtk_combo_box_set_active(output_editor->soundcard,
			       i);

      /*  */
      audio_connection = NULL;
      
      list = audio->audio_connection;
	  
      while((list = ags_audio_connection_find(list,
					      AGS_TYPE_INPUT,
					      channel->pad,
					      channel->audio_channel)) != NULL){
	GObject *data_object;

	g_object_get(G_OBJECT(list->data),
		     "data-object\0", &data_object,
		     NULL);
	
	if(AGS_IS_SOUNDCARD(data_object)){
	  audio_connection = list->data;
	  
	  break;
	}

	list = list->next;
      }

      if(audio_connection != NULL){
	gtk_spin_button_set_value(output_editor->audio_channel,
				  audio_connection->mapped_line);
      }else{
	gtk_spin_button_set_value(output_editor->audio_channel,
				  0);
      }
    }else{
      gtk_combo_box_set_active(output_editor->soundcard,
			       0);
    }
  }
}

/**
 * ags_output_editor_check:
 * @output_editor: the #AgsOutputEditor
 *
 * Checks for possible channels to output. And modifies its ranges.
 * 
 * Since: 0.7.131
 */
void
ags_output_editor_check(AgsOutputEditor *output_editor)
{
  AgsConnectionEditor *connection_editor;
  AgsLineEditor *line_editor;
  
  GtkTreeIter iter;

  connection_editor = AGS_CONNECTION_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(output_editor),
								    AGS_TYPE_CONNECTION_EDITOR));
  line_editor = AGS_LINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(output_editor),
							AGS_TYPE_LINE_EDITOR));

  if(gtk_combo_box_get_active_iter(GTK_COMBO_BOX(output_editor->soundcard),
				   &iter)){    
    GObject *soundcard;
    
    GtkTreeModel *model;

    guint audio_channels;
    
    /* soundcard connection */
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(output_editor->soundcard));
    gtk_tree_model_get(model,
		       &iter,
		       1, &soundcard,
		       -1);

    ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
 			      &audio_channels,
			      NULL,
			      NULL,
			      NULL);

    gtk_spin_button_set_range(output_editor->audio_channel,
			      0.0,
			      audio_channels - 1.0);
  }else{
    gtk_spin_button_set_range(output_editor->audio_channel,
			      -1.0, -1.0);
  }
}

/**
 * ags_output_editor_new:
 *
 * Creates an #AgsOutputEditor
 *
 * Returns: a new #AgsOutputEditor
 *
 * Since: 0.7.131
 */
AgsOutputEditor*
ags_output_editor_new()
{
  AgsOutputEditor *output_editor;

  output_editor = (AgsOutputEditor *) g_object_new(AGS_TYPE_OUTPUT_EDITOR,
						   NULL);

  return(output_editor);
}
