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

#include <ags/X/ags_link_editor.h>
#include <ags/X/ags_link_editor_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_applicable.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>

#include <ags/audio/task/ags_link_channel.h>

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

extern pthread_key_t application_context;
AgsApplicationContext *ags_application_context =  pthread_getspecific(application_context);

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

  if(gtk_combo_box_get_active_iter(link_editor->combo,
				   &iter)){
    AgsMachine *link_machine;
    AgsLineEditor *line_editor;
    AgsChannel *channel, *link;
    AgsThread *main_loop, *current;
    AgsTaskThread *task_thread;
    AgsLinkChannel *link_channel;
    GtkTreeModel *model;

    line_editor = AGS_LINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(link_editor),
							  AGS_TYPE_LINE_EDITOR));

    channel = line_editor->channel;
    
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

    model = gtk_combo_box_get_model(link_editor->combo);
    gtk_tree_model_get(model,
		       &iter,
		       1, &link_machine,
		       -1);
    
    if(link_machine == NULL){
      /* create task */
      link_channel = ags_link_channel_new(channel, NULL);
      
      /* append AgsLinkChannel */
      ags_task_thread_append_task(task_thread,
				  AGS_TASK(link_channel));
    }else{
      guint link_line;

      link_line = (guint) gtk_spin_button_get_value_as_int(link_editor->spin_button);

      if(AGS_IS_INPUT(channel))
	link = ags_channel_nth(link_machine->audio->output,
			       link_line);
      else
	link = ags_channel_nth(link_machine->audio->input,
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
    AgsMachine *machine, *link;
    AgsLineEditor *line_editor;
    AgsChannel *channel;
    gint i;
    gboolean found;

    line_editor = AGS_LINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(link_editor),
							  AGS_TYPE_LINE_EDITOR));

    channel = line_editor->channel;

    if(channel->link != NULL)
      machine = AGS_MACHINE(AGS_AUDIO(channel->link->audio)->machine);
    else
      machine = NULL;

    i = 0;
    found = FALSE;

    do{
      gtk_tree_model_get(model,
			 &iter,
			 1, &link,
			 -1);

      if(machine == link){
	found = TRUE;
	break;
      }

      i++;
    }while(gtk_tree_model_iter_next(model,
				    &iter));

    if(found){
      gtk_combo_box_set_active(link_editor->combo, i);

      if(channel->link == NULL)
	gtk_spin_button_set_value(link_editor->spin_button, 0);
      else
	gtk_spin_button_set_value(link_editor->spin_button, channel->link->line);
    }else
      gtk_combo_box_set_active(link_editor->combo, -1);
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
