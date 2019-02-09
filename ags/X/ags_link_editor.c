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

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_line_editor.h>

#include <ags/X/thread/ags_gui_thread.h>

void ags_link_editor_class_init(AgsLinkEditorClass *link_editor);
void ags_link_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_link_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_link_editor_init(AgsLinkEditor *link_editor);

void ags_link_editor_connect(AgsConnectable *connectable);
void ags_link_editor_disconnect(AgsConnectable *connectable);

void ags_link_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_link_editor_apply(AgsApplicable *applicable);
void ags_link_editor_reset(AgsApplicable *applicable);

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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_link_editor = 0;

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
						  "AgsLinkEditor", &ags_link_editor_info,
						  0);

    g_type_add_interface_static(ags_type_link_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_link_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_link_editor);
  }

  return g_define_type_id__volatile;
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

  g_signal_connect_after((GObject *) link_editor, "parent_set",
			 G_CALLBACK(ags_link_editor_parent_set_callback), (gpointer) link_editor);

  link_editor->flags = 0;

  /* linking machine */
  link_editor->combo = (GtkComboBox *) gtk_combo_box_new();
  gtk_box_pack_start(GTK_BOX(link_editor),
		     GTK_WIDGET(link_editor->combo),
		     FALSE, FALSE,
		     0);
  
  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(link_editor->combo),
			     cell_renderer,
			     FALSE); 
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(link_editor->combo),
				 cell_renderer,
				 "text", 0,
				 NULL);

  /* link with line */
  link_editor->spin_button = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_box_pack_start(GTK_BOX(link_editor),
		     GTK_WIDGET(link_editor->spin_button),
		     FALSE, FALSE,
		     0);

  link_editor->audio_file = NULL;

  link_editor->file_chooser = NULL;
}

void
ags_link_editor_connect(AgsConnectable *connectable)
{
  AgsLinkEditor *link_editor;

  link_editor = AGS_LINK_EDITOR(connectable);

  if((AGS_LINK_EDITOR_CONNECTED & (link_editor->flags)) != 0){
    return;
  }

  link_editor->flags |= AGS_LINK_EDITOR_CONNECTED;
  
  /* GtkComboBox */
  g_signal_connect(G_OBJECT(link_editor->combo), "changed",
		   G_CALLBACK(ags_link_editor_combo_callback), link_editor);
}

void
ags_link_editor_disconnect(AgsConnectable *connectable)
{
  AgsLinkEditor *link_editor;

  link_editor = AGS_LINK_EDITOR(connectable);

  if((AGS_LINK_EDITOR_CONNECTED & (link_editor->flags)) == 0){
    return;
  }

  link_editor->flags &= (~AGS_LINK_EDITOR_CONNECTED);

  /* GtkComboBox */
  g_object_disconnect(G_OBJECT(link_editor->combo),
		      "any_signal::changed",
		      G_CALLBACK(ags_link_editor_combo_callback),
		      link_editor,
		      NULL);
}

void
ags_link_editor_set_update(AgsApplicable *applicable, gboolean update)
{
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
    AgsMachineEditor *machine_editor;
    AgsLineEditor *line_editor;

    GtkTreeModel *model;

    AgsAudio *audio;
    AgsChannel *channel, *link;
    AgsLinkChannel *link_channel;

    GObject *output_soundcard;

    AgsThread *gui_thread;

    AgsApplicationContext *application_context;
    
    line_editor = AGS_LINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(link_editor),
							  AGS_TYPE_LINE_EDITOR));

    machine_editor = AGS_MACHINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(line_editor),
								AGS_TYPE_MACHINE_EDITOR));

    machine = machine_editor->machine;

    window = AGS_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(machine),
						AGS_TYPE_WINDOW));
    
    /* get channel */
    channel = line_editor->channel;
 
    /* get audio */
    g_object_get(channel,
		 "audio", &audio,
		 "output-soundcard", &output_soundcard,
		 NULL);

    /* application context */  
    application_context = (AgsApplicationContext *) window->application_context;

    /* get task thread */
    gui_thread = ags_ui_provider_get_gui_thread(AGS_UI_PROVIDER(application_context));

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
	
	if(g_strcmp0(filename, "")){
	  open_single_file = ags_open_single_file_new(channel,
						      filename,
						      (guint) gtk_spin_button_get_value_as_int(link_editor->spin_button));
	  /* append AgsLinkChannel */
	  ags_gui_thread_schedule_task((AgsGuiThread *) gui_thread,
				       (GObject *) open_single_file);
	}
      }else{
	/* create task */
	link_channel = ags_link_channel_new(channel,
					    NULL);
	
	/* append AgsLinkChannel */
	ags_gui_thread_schedule_task((AgsGuiThread *) gui_thread,
				     (GObject *) link_channel);
      }
    }else{
      guint link_line;

      gboolean is_output;
      
      link_line = (guint) gtk_spin_button_get_value_as_int(link_editor->spin_button);
      
      is_output = (AGS_IS_OUTPUT(channel)) ? TRUE: FALSE;

      /* get link */
      if(!is_output){
	g_object_get(link_machine->audio,
		     "output", &link,
		     NULL);
	g_object_unref(link);
      }else{
	g_object_get(link_machine->audio,
		     "input", &link,
		     NULL);
	g_object_unref(link);
      }
      
      link = ags_channel_nth(link,
			     link_line);
      
      /* create task */
      link_channel = ags_link_channel_new(channel,
					  link);
      
      /* append AgsLinkChannel */
      ags_gui_thread_schedule_task((AgsGuiThread *) gui_thread,
				   (GObject *) link_channel);
    }

    g_object_unref(audio);
  
    g_object_unref(output_soundcard);
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
    AgsMachineEditor *machine_editor;
    AgsLineEditor *line_editor;

    AgsAudio *audio;
    AgsChannel *channel, *link_channel;

    gint i;
    gboolean found;

    line_editor = AGS_LINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(link_editor),
							  AGS_TYPE_LINE_EDITOR));

    machine_editor = AGS_MACHINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(line_editor),
								AGS_TYPE_MACHINE_EDITOR));

    machine = machine_editor->machine;

    channel = line_editor->channel;
 
    /* get audio and channel's link */
    g_object_get(channel,
		 "audio", &audio,
		 "link", &link_channel,
		 NULL);
    g_object_unref(audio);

    if(link_channel != NULL){
      g_object_unref(link_channel);
    }
    
    /*  */
    if(link_channel != NULL){
      AgsAudio *link_audio;

      /* get audio */
      g_object_get(link_channel,
		   "audio", &link_audio,
		   NULL);
      g_object_unref(link_audio);

      /* get machine */
      //FIXME:JK: don't access AgsAudio to obtain widget
      link_machine = (AgsMachine *) link_audio->machine_widget;
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
	gtk_spin_button_set_value(link_editor->spin_button, link_channel->line);
      }
    }else{
      gtk_combo_box_set_active(link_editor->combo,
			       0);
    }

    /* set file link */
    if((AGS_MACHINE_TAKES_FILE_INPUT & (machine->flags)) != 0 &&
     ((AGS_MACHINE_ACCEPT_WAV & (machine->file_input_flags)) != 0 ||
      (AGS_MACHINE_ACCEPT_OGG & (machine->file_input_flags)) != 0) &&
       AGS_IS_INPUT(channel)){
      AgsFileLink *file_link;
      
      gtk_tree_model_iter_nth_child(model,
				    &iter,
				    NULL,
				    gtk_tree_model_iter_n_children(model,
								   NULL) - 1);

      g_object_get(channel,
		   "file-link", &file_link,
		   NULL);
      
      if(file_link != NULL){
	gchar *filename;

	guint audio_channel;

	pthread_mutex_t *file_link_mutex;

	/* get file link mutex */
	pthread_mutex_lock(ags_file_link_get_class_mutex());
	
	file_link_mutex = file_link->obj_mutex;

	pthread_mutex_unlock(ags_file_link_get_class_mutex());

	/* get some fields */
	pthread_mutex_lock(file_link_mutex);
	
	filename = g_strdup(file_link->filename);

	audio_channel = AGS_AUDIO_FILE_LINK(file_link)->audio_channel;
	
	pthread_mutex_unlock(file_link_mutex);

	/* add file link */
	gtk_list_store_set(GTK_LIST_STORE(model), &iter,
			   0, g_strdup_printf("file://%s", filename),
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
				  audio_channel);
	gtk_widget_set_sensitive((GtkWidget *) link_editor->spin_button,
				 FALSE);

	/* re-enable file chooser */
	link_editor->flags &= (~AGS_LINK_EDITOR_BLOCK_FILE_CHOOSER);

	g_free(filename);

	g_object_unref(file_link);
      }else{
	gtk_list_store_set(GTK_LIST_STORE(model), &iter,
			   0, "file://",
			   1, NULL,
			   -1);
      }
    }
  }
}

/**
 * ags_link_editor_new:
 *
 * Create a new instance of #AgsLinkEditor
 *
 * Returns: the new #AgsLinkEditor
 *
 * Since: 2.0.0
 */
AgsLinkEditor*
ags_link_editor_new()
{
  AgsLinkEditor *link_editor;

  link_editor = (AgsLinkEditor *) g_object_new(AGS_TYPE_LINK_EDITOR,
					       NULL);

  return(link_editor);
}
