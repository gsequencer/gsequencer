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

#include <ags/X/ags_link_editor.h>
#include <ags/X/ags_link_editor_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/file/ags_file_link.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

#include <ags/audio/file/ags_audio_file_link.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/task/ags_link_channel.h>
#include <ags/audio/task/ags_open_single_file.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_line_editor.h>

void ags_link_editor_class_init(AgsLinkEditorClass *link_editor);
void ags_link_editor_init(AgsLinkEditor *link_editor);
void ags_link_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_link_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_link_editor_connect(AgsConnectable *connectable);
void ags_link_editor_disconnect(AgsConnectable *connectable);
void ags_link_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_link_editor_apply(AgsApplicable *applicable);
void ags_link_editor_reset(AgsApplicable *applicable);
void ags_link_editor_destroy(GtkObject *object);
void ags_link_editor_show(GtkWidget *widget);

/**
 * SECTION:ags_link_editor
 * @short_description: Edit links.
 * @title: AgsLinkEditor
 * @section_id:
 * @include: ags/X/ags_link_editor.h
 *
 * #AgsLinkEditor is a composite widget to modify links. A link editor 
 * should be packed by a #AgsLineEditor.
 */

static gpointer ags_link_editor_parent_class = NULL;

GType
ags_link_editor_get_type(void)
{
  static GType ags_type_link_editor = 0;

  if(!ags_type_link_editor){
    static const GTypeInfo ags_link_editor_info = {
      sizeof (AgsLinkEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_link_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLinkEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_link_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_link_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_link_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_link_editor = g_type_register_static(GTK_TYPE_HBOX,
						  "AgsLinkEditor\0", &ags_link_editor_info,
						  0);

    g_type_add_interface_static(ags_type_link_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_link_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_link_editor);
}

void
ags_link_editor_class_init(AgsLinkEditorClass *link_editor)
{
  ags_link_editor_parent_class = g_type_class_peek_parent(link_editor);
}

void
ags_link_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_link_editor_connect;
  connectable->disconnect = ags_link_editor_disconnect;
}

void
ags_link_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_link_editor_set_update;
  applicable->apply = ags_link_editor_apply;
  applicable->reset = ags_link_editor_reset;
}

void
ags_link_editor_init(AgsLinkEditor *link_editor)
{
  GtkCellRenderer *cell_renderer;

  g_signal_connect_after((GObject *) link_editor, "parent_set\0",
			 G_CALLBACK(ags_link_editor_parent_set_callback), (gpointer) link_editor);

  link_editor->flags = 0;

  /* linking machine */
  link_editor->combo = (GtkComboBox *) gtk_combo_box_new();
  gtk_box_pack_start(GTK_BOX(link_editor),
		     GTK_WIDGET(link_editor->combo),
		     FALSE, FALSE, 0);
  
  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(link_editor->combo),
			     cell_renderer,
			     FALSE); 
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(link_editor->combo),
				 cell_renderer,
				 "text\0", 0,
				 NULL);

  /* link with line */
  link_editor->spin_button = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_box_pack_start(GTK_BOX(link_editor),
		     GTK_WIDGET(link_editor->spin_button),
		     FALSE, FALSE, 0);

  link_editor->audio_file = NULL;

  link_editor->file_chooser = NULL;
}

void
ags_link_editor_connect(AgsConnectable *connectable)
{
  AgsLinkEditor *link_editor;

  link_editor = AGS_LINK_EDITOR(connectable);

  /* GtkObject */
  g_signal_connect(G_OBJECT(link_editor), "destroy\0",
		   G_CALLBACK(ags_link_editor_destroy_callback), (gpointer) link_editor);

  /* GtkWidget */
  g_signal_connect(G_OBJECT(link_editor), "show\0",
		   G_CALLBACK(ags_link_editor_show_callback), (gpointer) link_editor);

  /* GtkComboBox */
  g_signal_connect(G_OBJECT(link_editor->combo), "changed\0",
		   G_CALLBACK(ags_link_editor_combo_callback), link_editor);
}

void
ags_link_editor_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_link_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsLinkEditor *link_editor;

  link_editor = AGS_LINK_EDITOR(applicable);

  /* empty */
}

void
ags_link_editor_apply(AgsApplicable *applicable)
{
  AgsLinkEditor *link_editor;
  GtkTreeIter iter;

  link_editor = AGS_LINK_EDITOR(applicable);

  /* reset */
  if(gtk_combo_box_get_active_iter(link_editor->combo,
				   &iter)){
    AgsWindow *window;
    AgsMachine *link_machine, *machine;
    AgsLineEditor *line_editor;

    GObject *soundcard;
    AgsAudio *audio;
    AgsChannel *channel, *link;
    AgsLinkChannel *link_channel;
    GtkTreeModel *model;

    AgsMutexManager *mutex_manager;
    AgsAudioLoop *audio_loop;
    AgsTaskThread *task_thread;

    AgsApplicationContext *application_context;
    
    pthread_mutex_t *application_mutex;
    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *channel_mutex;

    line_editor = AGS_LINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(link_editor),
							  AGS_TYPE_LINE_EDITOR));

    /* get channel */
    channel = line_editor->channel;

    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
    
    /* lookup channel mutex */
    pthread_mutex_lock(application_mutex);
        
    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
    
    pthread_mutex_unlock(application_mutex);
 
    /* get audio */
    pthread_mutex_lock(channel_mutex);

    audio = channel->audio;

    pthread_mutex_unlock(channel_mutex);

    /* lookup audio mutex */
    pthread_mutex_lock(application_mutex);
    
    mutex_manager = ags_mutex_manager_get_instance();
    
    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
    
    pthread_mutex_unlock(application_mutex);

    /* get soundcard and machine */
    pthread_mutex_lock(audio_mutex);

    soundcard = audio->soundcard;
    machine = audio->machine;

    pthread_mutex_unlock(audio_mutex);

    /* get window and application_context  */
    window = (AgsWindow *) gtk_widget_get_toplevel(machine);
  
    application_context = window->application_context;

    /* get audio loop */
    pthread_mutex_lock(application_mutex);

    audio_loop = application_context->main_loop;

    pthread_mutex_unlock(application_mutex);

    /* get task thread */
    task_thread = (AgsTaskThread *) ags_thread_find_type(audio_loop,
							 AGS_TYPE_TASK_THREAD);

    /* get model */
    model = gtk_combo_box_get_model(link_editor->combo);
    gtk_tree_model_get(model,
		       &iter,
		       1, &link_machine,
		       -1);

    if(link_machine == NULL){
      if((AGS_MACHINE_TAKES_FILE_INPUT & (machine->flags)) != 0 &&
	 ((AGS_MACHINE_ACCEPT_WAV & (machine->file_input_flags)) != 0 ||
	  ((AGS_MACHINE_ACCEPT_OGG & (machine->file_input_flags)) != 0)) &&
	 gtk_combo_box_get_active(link_editor->combo) + 1 == gtk_tree_model_iter_n_children(model,
											    NULL)){
	AgsOpenSingleFile *open_single_file;
	gchar *str, *filename;
	
	gtk_tree_model_get(model,
			   &iter,
			   0, &str,
			   -1);
	
	filename = g_strdup(str + 7);
	
	if(g_strcmp0(filename, "\0")){
	  open_single_file = ags_open_single_file_new(channel,
						      soundcard,
						      filename,
						      (guint) gtk_spin_button_get_value_as_int(link_editor->spin_button),
						      1);
	  /* append AgsLinkChannel */
	  ags_task_thread_append_task(task_thread,
				      AGS_TASK(open_single_file));
	}
      }else{
	/* create task */
	link_channel = ags_link_channel_new(channel, NULL);
	
	/* append AgsLinkChannel */
	ags_task_thread_append_task(task_thread,
				    AGS_TASK(link_channel));
      }
    }else{
      guint link_line;
      gboolean is_output;
      
      pthread_mutex_t *link_audio_mutex;

      link_line = (guint) gtk_spin_button_get_value_as_int(link_editor->spin_button);

      pthread_mutex_lock(channel_mutex);
      
      is_output = (AGS_IS_OUTPUT(channel)) ? TRUE: FALSE;
      
      pthread_mutex_unlock(channel_mutex);
      
      /* lookup link's audio mutex */
      pthread_mutex_lock(application_mutex);
      
      mutex_manager = ags_mutex_manager_get_instance();
      
      link_audio_mutex = ags_mutex_manager_lookup(mutex_manager,
						  (GObject *) link_machine->audio);
      
      pthread_mutex_unlock(application_mutex);

      /* get link */
      pthread_mutex_lock(link_audio_mutex);

      if(!is_output){
	link = link_machine->audio->output;
      }else{
	link = link_machine->audio->input;
      }

      pthread_mutex_unlock(link_audio_mutex);
      
      link = ags_channel_nth(link,
			     link_line);
      
      /* create task */
      link_channel = ags_link_channel_new(channel, link);
      
      /* append AgsLinkChannel */
      ags_task_thread_append_task(task_thread,
				  AGS_TASK(link_channel));
    }
  }
}

void
ags_link_editor_reset(AgsApplicable *applicable)
{
  AgsLinkEditor *link_editor;
  GtkTreeModel *model;
  GtkTreeIter iter;

  link_editor = AGS_LINK_EDITOR(applicable);

  model = gtk_combo_box_get_model(link_editor->combo);

  if(gtk_tree_model_get_iter_first(model,
				   &iter)){
    AgsMachine *machine, *link_machine, *link;
    AgsLineEditor *line_editor;

    AgsAudio *audio;
    AgsChannel *channel, *link_channel;

    AgsMutexManager *mutex_manager;

    gint i;
    gboolean found;

    pthread_mutex_t *application_mutex;
    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *channel_mutex;
    pthread_mutex_t *link_channel_mutex;

    line_editor = AGS_LINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(link_editor),
							  AGS_TYPE_LINE_EDITOR));

    channel = line_editor->channel;

    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
    
    /* lookup channel mutex */
    pthread_mutex_lock(application_mutex);
    
    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
    
    pthread_mutex_unlock(application_mutex);
 
    /* get audio and channel's link */
    pthread_mutex_lock(channel_mutex);

    audio = channel->audio;
    link_channel = channel->link;
    
    pthread_mutex_unlock(channel_mutex);

    /* lookup audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
  
    pthread_mutex_unlock(application_mutex);

    /* get machine */
    pthread_mutex_lock(audio_mutex);
  
    machine = (AgsMachine *) audio->machine;

    pthread_mutex_unlock(audio_mutex);

    /*  */
    if(link_channel != NULL){
      AgsAudio *link_audio;
      
      pthread_mutex_t *link_audio_mutex;
      
      /* lookup channel mutex */
      pthread_mutex_lock(application_mutex);
    
      mutex_manager = ags_mutex_manager_get_instance();
    
      link_channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) link_channel);
    
      pthread_mutex_unlock(application_mutex);
 
      /* get audio */
      pthread_mutex_lock(link_channel_mutex);

      link_audio = link_channel->audio;

      pthread_mutex_unlock(link_channel_mutex);

      /* lookup audio mutex */
      pthread_mutex_lock(application_mutex);

      link_audio_mutex = ags_mutex_manager_lookup(mutex_manager,
						  (GObject *) link_audio);
      
      pthread_mutex_unlock(application_mutex);

      /* get machine */
      pthread_mutex_lock(link_audio_mutex);
  
      link_machine = link_audio->machine;

      pthread_mutex_unlock(link_audio_mutex);

    }else{
      link_machine = NULL;
    }
    
    i = 0;
    found = FALSE;

    if(link_machine != NULL){
      do{
	gtk_tree_model_get(model,
			   &iter,
			   1, &link,
			   -1);

	if(link_machine == link){
	  found = TRUE;
	  break;
	}

	i++;
      }while(gtk_tree_model_iter_next(model,
				      &iter));
    }

    gtk_widget_set_sensitive((GtkWidget *) link_editor->spin_button,
			     FALSE);

    if(found){
      /* set channel link */
      gtk_combo_box_set_active(link_editor->combo, i);

      if(link_channel == NULL){
	gtk_spin_button_set_value(link_editor->spin_button, 0);
      }else{
	pthread_mutex_lock(link_channel_mutex);
	
	gtk_spin_button_set_value(link_editor->spin_button, link_channel->line);

	pthread_mutex_unlock(link_channel_mutex);
      }
    }else{
      gtk_combo_box_set_active(link_editor->combo,
			       0);
    }

    /* set file link */
    pthread_mutex_lock(channel_mutex);

    if((AGS_MACHINE_TAKES_FILE_INPUT & (machine->flags)) != 0 &&
     ((AGS_MACHINE_ACCEPT_WAV & (machine->file_input_flags)) != 0 ||
      (AGS_MACHINE_ACCEPT_OGG & (machine->file_input_flags)) != 0) &&
       AGS_IS_INPUT(channel)){
      gtk_tree_model_iter_nth_child(model,
				    &iter,
				    NULL,
				    gtk_tree_model_iter_n_children(model,
								   NULL) - 1);
      
      if(AGS_INPUT(channel)->file_link != NULL){
	/* add file link */
	gtk_list_store_set(model, &iter,
			   0, g_strdup_printf("file://%s\0", AGS_FILE_LINK(AGS_INPUT(channel)->file_link)->filename),
			   1, NULL,
			   -1);

	/* file link set active */
	link_editor->flags |= AGS_LINK_EDITOR_BLOCK_FILE_CHOOSER;

	gtk_combo_box_set_active(link_editor->combo,
				 gtk_tree_model_iter_n_children(model,
								NULL) - 1);

	/* set spin button insensitive */
	gtk_spin_button_set_range(link_editor->spin_button,
				  0.0, 256.0);
	gtk_spin_button_set_value(link_editor->spin_button,
				  AGS_AUDIO_FILE_LINK(AGS_INPUT(channel)->file_link)->audio_channel);
	gtk_widget_set_sensitive((GtkWidget *) link_editor->spin_button,
				 FALSE);

	/* re-enable file chooser */
	link_editor->flags &= (~AGS_LINK_EDITOR_BLOCK_FILE_CHOOSER);
      }else{
	gtk_list_store_set(model, &iter,
			   0, "file://\0",
			   1, NULL,
			   -1);
      }
    }
    
    pthread_mutex_unlock(channel_mutex);
  }
}

void
ags_link_editor_destroy(GtkObject *object)
{
  /* empty */
}

void
ags_link_editor_show(GtkWidget *widget)
{
  /* empty */
}

/**
 * ags_link_editor_new:
 *
 * Creates an #AgsLinkEditor
 *
 * Returns: a new #AgsLinkEditor
 *
 * Since: 0.3
 */
AgsLinkEditor*
ags_link_editor_new()
{
  AgsLinkEditor *link_editor;

  link_editor = (AgsLinkEditor *) g_object_new(AGS_TYPE_LINK_EDITOR,
					       NULL);

  return(link_editor);
}
