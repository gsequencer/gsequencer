/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_resize_editor.h>

#include <ags/object/ags_application_context.h>
#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_applicable.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

#include <ags/audio/task/ags_resize_audio.h>

#include <ags/X/ags_machine_editor.h>

void ags_resize_editor_class_init(AgsResizeEditorClass *resize_editor);
void ags_resize_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_resize_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_resize_editor_init(AgsResizeEditor *resize_editor);
void ags_resize_editor_connect(AgsConnectable *connectable);
void ags_resize_editor_disconnect(AgsConnectable *connectable);
void ags_resize_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_resize_editor_apply(AgsApplicable *applicable);
void ags_resize_editor_reset(AgsApplicable *applicable);
void ags_resize_editor_destroy(GtkObject *object);
void ags_resize_editor_show(GtkWidget *widget);

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

AgsConnectableInterface *ags_resize_editor_parent_connectable_interface;

extern AgsApplicationContext *ags_application_context;

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
						    "AgsResizeEditor\0",
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

  label = (GtkLabel *) gtk_label_new("audio channels\0");
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

  label = (GtkLabel *) gtk_label_new("outputs\0");
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

  label = (GtkLabel *) gtk_label_new("inputs\0");
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
  AgsResizeEditor *resize_editor;

  ags_resize_editor_parent_connectable_interface->connect(connectable);

  /* AgsResizeEditor */
  resize_editor = AGS_RESIZE_EDITOR(connectable);
}

void
ags_resize_editor_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_resize_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsResizeEditor *resize_editor;

  resize_editor = AGS_RESIZE_EDITOR(applicable);
}

void
ags_resize_editor_apply(AgsApplicable *applicable)
{
  AgsMachineEditor *machine_editor;
  AgsResizeEditor *resize_editor;
  AgsAudio *audio;
  AgsResizeAudio *resize_audio;
  AgsThread *main_loop, *current;
  AgsTaskThread *task_thread;

  resize_editor = AGS_RESIZE_EDITOR(applicable);

  if((AGS_PROPERTY_EDITOR_ENABLED & (AGS_PROPERTY_EDITOR(resize_editor)->flags)) == 0)
    return;
  
  machine_editor = AGS_MACHINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(resize_editor),
							      AGS_TYPE_MACHINE_EDITOR));

  audio = machine_editor->machine->audio;

  task_thread = NULL;

  main_loop = ags_application_context->main_loop;
  current = main_loop->children;

  while(current != NULL){
    if(AGS_IS_TASK_THREAD(current)){
      task_thread = (AgsTaskThread *) current;

      break;
    }

    current = current->next;
  }

  /* create task */
  resize_audio = ags_resize_audio_new(audio,
				      (guint) gtk_spin_button_get_value_as_int(resize_editor->output_pads),
				      (guint) gtk_spin_button_get_value_as_int(resize_editor->input_pads),
				      (guint) gtk_spin_button_get_value_as_int(resize_editor->audio_channels));
      
  /* append AgsResizeAudio */
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(resize_audio));
}

void
ags_resize_editor_reset(AgsApplicable *applicable)
{
  AgsMachineEditor *machine_editor;
  AgsResizeEditor *resize_editor;
  AgsAudio *audio;

  resize_editor = AGS_RESIZE_EDITOR(applicable);

  machine_editor = AGS_MACHINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(resize_editor),
							      AGS_TYPE_MACHINE_EDITOR));

  audio = machine_editor->machine->audio;

  gtk_spin_button_set_value(resize_editor->audio_channels,
			    audio->audio_channels);

  gtk_spin_button_set_value(resize_editor->input_pads,
			    audio->input_pads);

  gtk_spin_button_set_value(resize_editor->output_pads,
			    audio->output_pads);
}

void
ags_resize_editor_destroy(GtkObject *object)
{
  AgsResizeEditor *resize_editor;

  resize_editor = (AgsResizeEditor *) object;
}

void
ags_resize_editor_show(GtkWidget *widget)
{
  AgsResizeEditor *resize_editor = (AgsResizeEditor *) widget;
}

/**
 * ags_resize_editor_new:
 *
 * Creates an #AgsResizeEditor
 *
 * Returns: a new #AgsResizeEditor
 *
 * Since: 0.3
 */
AgsResizeEditor*
ags_resize_editor_new()
{
  AgsResizeEditor *resize_editor;

  resize_editor = (AgsResizeEditor *) g_object_new(AGS_TYPE_RESIZE_EDITOR,
						   NULL);
  
  return(resize_editor);
}
