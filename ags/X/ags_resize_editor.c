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

#include <ags/X/ags_resize_editor.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine_editor.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <ags/i18n.h>

void ags_resize_editor_class_init(AgsResizeEditorClass *resize_editor);
void ags_resize_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_resize_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_resize_editor_init(AgsResizeEditor *resize_editor);
void ags_resize_editor_connect(AgsConnectable *connectable);
void ags_resize_editor_disconnect(AgsConnectable *connectable);
void ags_resize_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_resize_editor_apply(AgsApplicable *applicable);
void ags_resize_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_resize_editor
 * @short_description: Resize audio channels or pads. 
 * @title: AgsResizeEditor
 * @section_id:
 * @include: ags/X/ags_resize_editor.h
 *
 * #AgsResizeEditor is a composite widget to resize. A resize editor 
 * should be packed by a #AgsMachineEditor.
 */

static gpointer ags_resize_editor_parent_class = NULL;
AgsConnectableInterface *ags_resize_editor_parent_connectable_interface;

GType
ags_resize_editor_get_type(void)
{
  static GType ags_type_resize_editor = 0;

  if(!ags_type_resize_editor){
    static const GTypeInfo ags_resize_editor_info = {
      sizeof (AgsResizeEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_resize_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsResizeEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_resize_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_resize_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_resize_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_resize_editor = g_type_register_static(AGS_TYPE_PROPERTY_EDITOR,
						    "AgsResizeEditor",
						    &ags_resize_editor_info,
						    0);
    
    g_type_add_interface_static(ags_type_resize_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_resize_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return(ags_type_resize_editor);
}

void
ags_resize_editor_class_init(AgsResizeEditorClass *resize_editor)
{
  ags_resize_editor_parent_class = g_type_class_peek_parent(resize_editor);
}

void
ags_resize_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_resize_editor_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_resize_editor_connect;
  connectable->disconnect = ags_resize_editor_disconnect;
}

void
ags_resize_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_resize_editor_set_update;
  applicable->apply = ags_resize_editor_apply;
  applicable->reset = ags_resize_editor_reset;
}

void
ags_resize_editor_init(AgsResizeEditor *resize_editor)
{
  GtkTable *table;
  GtkAlignment *alignment;
  GtkLabel *label;

  table = (GtkTable *) gtk_table_new(3, 2, FALSE);
  gtk_box_pack_start(GTK_BOX(resize_editor),
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     0);

  /* audio channels */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  label = (GtkLabel *) gtk_label_new(i18n("audio channels"));
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(label));

  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  resize_editor->audio_channels = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1024.0, 1.0);
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(resize_editor->audio_channels));

  /* output pads */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  label = (GtkLabel *) gtk_label_new(i18n("outputs"));
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(label));

  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  resize_editor->output_pads = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1024.0, 1.0);
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(resize_editor->output_pads));

  /* input pads */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  label = (GtkLabel *) gtk_label_new(i18n("inputs"));
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(label));

  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(table,
		   GTK_WIDGET(alignment),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  resize_editor->input_pads = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1024.0, 1.0);
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(resize_editor->input_pads));
}

void
ags_resize_editor_connect(AgsConnectable *connectable)
{
  ags_resize_editor_parent_connectable_interface->connect(connectable);
}

void
ags_resize_editor_disconnect(AgsConnectable *connectable)
{
  ags_resize_editor_parent_connectable_interface->disconnect(connectable);
}

void
ags_resize_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_resize_editor_apply(AgsApplicable *applicable)
{
  AgsWindow *window;
  AgsMachineEditor *machine_editor;
  AgsResizeEditor *resize_editor;
  AgsAudio *audio;
  AgsResizeAudio *resize_audio;
    
  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsGuiThread *gui_thread;
  
  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;
  
  resize_editor = AGS_RESIZE_EDITOR(applicable);

  if((AGS_PROPERTY_EDITOR_ENABLED & (AGS_PROPERTY_EDITOR(resize_editor)->flags)) == 0){
    return;
  }
  
  machine_editor = AGS_MACHINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(resize_editor),
							      AGS_TYPE_MACHINE_EDITOR));

  audio = machine_editor->machine->audio;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get window and application_context  */
  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) machine_editor->machine);
  
  application_context = (AgsApplicationContext *) window->application_context;

  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  gui_thread = (AgsGuiThread *) ags_thread_find_type(main_loop,
						      AGS_TYPE_GUI_THREAD);

  /* create task */
  resize_audio = ags_resize_audio_new(audio,
				      (guint) gtk_spin_button_get_value_as_int(resize_editor->output_pads),
				      (guint) gtk_spin_button_get_value_as_int(resize_editor->input_pads),
				      (guint) gtk_spin_button_get_value_as_int(resize_editor->audio_channels));
      
  /* append AgsResizeAudio */
  ags_gui_thread_schedule_task(gui_thread,
			       resize_audio);
}

void
ags_resize_editor_reset(AgsApplicable *applicable)
{
  AgsMachineEditor *machine_editor;
  AgsResizeEditor *resize_editor;

  AgsAudio *audio;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  resize_editor = AGS_RESIZE_EDITOR(applicable);

  machine_editor = AGS_MACHINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(resize_editor),
							      AGS_TYPE_MACHINE_EDITOR));

  audio = machine_editor->machine->audio;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* lookup audio mutex */
  pthread_mutex_lock(application_mutex);
  
  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) audio);
    
  pthread_mutex_unlock(application_mutex);

  /* reset */
  pthread_mutex_lock(audio_mutex);

  if((AGS_MACHINE_MONO & (machine_editor->machine->mapping_flags)) != 0){
    gtk_spin_button_set_range(resize_editor->audio_channels,
			      0.0, 1.0);
  }

  gtk_spin_button_set_value(resize_editor->audio_channels,
			    audio->audio_channels);

  gtk_spin_button_set_value(resize_editor->input_pads,
			    audio->input_pads);

  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (machine_editor->machine->audio->flags)) == 0){
    gtk_spin_button_set_range(resize_editor->output_pads,
			      0.0, 1.0);
  }
  
  gtk_spin_button_set_value(resize_editor->output_pads,
			    audio->output_pads);

  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_resize_editor_new:
 *
 * Creates an #AgsResizeEditor
 *
 * Returns: a new #AgsResizeEditor
 *
 * Since: 1.0.0
 */
AgsResizeEditor*
ags_resize_editor_new()
{
  AgsResizeEditor *resize_editor;

  resize_editor = (AgsResizeEditor *) g_object_new(AGS_TYPE_RESIZE_EDITOR,
						   NULL);
  
  return(resize_editor);
}
