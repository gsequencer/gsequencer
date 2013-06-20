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

#include <ags/X/ags_channel_link_collection_editor.h>
#include <ags/X/ags_channel_link_collection_editor_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/audio/ags_input.h>

#include <ags/audio/task/ags_link_channel.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_machine_editor.h>

void ags_channel_link_collection_editor_class_init(AgsChannelLinkCollectionEditorClass *channel_link_collection_editor);
void ags_channel_link_collection_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_channel_link_collection_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_channel_link_collection_editor_init(AgsChannelLinkCollectionEditor *channel_link_collection_editor);
void ags_channel_link_collection_editor_set_property(GObject *gobject,
						     guint prop_id,
						     const GValue *value,
						     GParamSpec *param_spec);
void ags_channel_link_collection_editor_get_property(GObject *gobject,
						     guint prop_id,
						     GValue *value,
						     GParamSpec *param_spec);
void ags_channel_link_collection_editor_connect(AgsConnectable *connectable);
void ags_channel_link_collection_editor_disconnect(AgsConnectable *connectable);
void ags_channel_link_collection_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_channel_link_collection_editor_apply(AgsApplicable *applicable);
void ags_channel_link_collection_editor_reset(AgsApplicable *applicable);
void ags_channel_link_collection_editor_destroy(GtkObject *object);
void ags_channel_link_collection_editor_show(GtkWidget *widget);

enum{
  PROP_0,
  PROP_CHANNEL_TYPE,
};

GType
ags_channel_link_collection_editor_get_type(void)
{
  static GType ags_type_channel_link_collection_editor = 0;

  if(!ags_type_channel_link_collection_editor){
    static const GTypeInfo ags_channel_link_collection_editor_info = {
      sizeof (AgsChannelLinkCollectionEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_channel_link_collection_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsChannelLinkCollectionEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_channel_link_collection_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_channel_link_collection_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_channel_link_collection_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_channel_link_collection_editor = g_type_register_static(GTK_TYPE_TABLE,
								     "AgsChannelLinkCollectionEditor\0",
								     &ags_channel_link_collection_editor_info,
								     0);
    
    g_type_add_interface_static(ags_type_channel_link_collection_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_channel_link_collection_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return(ags_type_channel_link_collection_editor);
}

void
ags_channel_link_collection_editor_class_init(AgsChannelLinkCollectionEditorClass *channel_link_collection_editor)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  gobject = (GObjectClass *) channel_link_collection_editor;

  gobject->set_property = ags_channel_link_collection_editor_set_property;
  gobject->get_property = ags_channel_link_collection_editor_get_property;

  param_spec = g_param_spec_gtype("channel type\0",
				   "assigned channel type\0",
				   "The channel type which this channel link collection editor is assigned with\0",
				   G_TYPE_NONE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL_TYPE,
				  param_spec);
}

void
ags_channel_link_collection_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_channel_link_collection_editor_connect;
  connectable->disconnect = ags_channel_link_collection_editor_disconnect;
}

void
ags_channel_link_collection_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_channel_link_collection_editor_set_update;
  applicable->apply = ags_channel_link_collection_editor_apply;
  applicable->reset = ags_channel_link_collection_editor_reset;
}

void
ags_channel_link_collection_editor_init(AgsChannelLinkCollectionEditor *channel_link_collection_editor)
{
  GtkAlignment *alignment;
  GtkLabel *label;
  GtkCellRenderer *cell_renderer;
  GtkListStore *model;
  GtkTreeIter iter;

  g_signal_connect_after(GTK_WIDGET(channel_link_collection_editor), "parent_set\0",
			 G_CALLBACK(ags_channel_link_collection_editor_parent_set_callback), channel_link_collection_editor);

  gtk_table_resize(GTK_TABLE(channel_link_collection_editor),
		   4, 2);
  gtk_table_set_row_spacings(GTK_TABLE(channel_link_collection_editor),
			     4);
  gtk_table_set_col_spacings(GTK_TABLE(channel_link_collection_editor),
			     2);

  /* link */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(channel_link_collection_editor),
		   GTK_WIDGET(alignment),
		   0, 1,
		   0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  label = (GtkLabel *) gtk_label_new("link\0");
  gtk_container_add(GTK_CONTAINER(alignment), GTK_WIDGET(label));


  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(channel_link_collection_editor),
		   GTK_WIDGET(alignment),
		   1, 2,
		   0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  channel_link_collection_editor->link = (GtkComboBox *) gtk_combo_box_new();
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(channel_link_collection_editor->link));
		    
  model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
		    
  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "NULL\0",
		     1, NULL,
		     -1);
  
  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(channel_link_collection_editor->link),
			     cell_renderer,
			     FALSE); 
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(channel_link_collection_editor->link),
				 cell_renderer,
				 "text\0", 0,
				 NULL);

  gtk_combo_box_set_model(channel_link_collection_editor->link,
			  GTK_TREE_MODEL(model));

  /* first line */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(channel_link_collection_editor),
		   GTK_WIDGET(alignment),
		   0, 1,
		   1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  label = (GtkLabel *) gtk_label_new("first line\0");
  gtk_container_add(GTK_CONTAINER(alignment), GTK_WIDGET(label));

  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(channel_link_collection_editor),
		   GTK_WIDGET(alignment),
		   1, 2,
		   1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);
  
  channel_link_collection_editor->first_line = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(channel_link_collection_editor->first_line));

  /* first link line */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(channel_link_collection_editor),
		   GTK_WIDGET(alignment),
		   0, 1,
		   2, 3,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,

		   0, 0);

  label = (GtkLabel *) gtk_label_new("first link line\0");
  gtk_container_add(GTK_CONTAINER(alignment), GTK_WIDGET(label));

  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(channel_link_collection_editor),
		   GTK_WIDGET(alignment),
		   1, 2,
		   2, 3,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);
  
  channel_link_collection_editor->first_link = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(channel_link_collection_editor->first_link));

  /* count */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(channel_link_collection_editor),
		   GTK_WIDGET(alignment),
		   0, 1,
		   3, 4,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  label = (GtkLabel *) gtk_label_new("count\0");
  gtk_container_add(GTK_CONTAINER(alignment), GTK_WIDGET(label));

  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.5,
						 0.0, 0.0);
  gtk_table_attach(GTK_TABLE(channel_link_collection_editor),
		   GTK_WIDGET(alignment),
		   1, 2,
		   3, 4,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);
  
  channel_link_collection_editor->count = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(channel_link_collection_editor->count));
}

void
ags_channel_link_collection_editor_set_property(GObject *gobject,
						guint prop_id,
						const GValue *value,
						GParamSpec *param_spec)
{
  AgsChannelLinkCollectionEditor *channel_link_collection_editor;

  channel_link_collection_editor = AGS_CHANNEL_LINK_COLLECTION_EDITOR(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
    channel_link_collection_editor->channel_type = g_value_get_gtype(value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_channel_link_collection_editor_get_property(GObject *gobject,
						guint prop_id,
						GValue *value,
						GParamSpec *param_spec)
{
  AgsChannelLinkCollectionEditor *channel_link_collection_editor;

  channel_link_collection_editor = AGS_CHANNEL_LINK_COLLECTION_EDITOR(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
    g_value_set_gtype(value, channel_link_collection_editor->channel_type);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_channel_link_collection_editor_connect(AgsConnectable *connectable)
{
  AgsChannelLinkCollectionEditor *channel_link_collection_editor;
  GList *pad_list;

  /* AgsChannelLinkCollectionEditor */
  channel_link_collection_editor = AGS_CHANNEL_LINK_COLLECTION_EDITOR(connectable);

  g_signal_connect_after(G_OBJECT(channel_link_collection_editor->link), "changed\0",
			 G_CALLBACK(ags_channel_link_collection_editor_link_callback), channel_link_collection_editor);

  g_signal_connect_after(G_OBJECT(channel_link_collection_editor->first_line), "value-changed\0",
			 G_CALLBACK(ags_channel_link_collection_editor_first_line_callback), channel_link_collection_editor);

  g_signal_connect_after(G_OBJECT(channel_link_collection_editor->first_link), "value-changed\0",
			 G_CALLBACK(ags_channel_link_collection_editor_first_link_callback), channel_link_collection_editor);
}

void
ags_channel_link_collection_editor_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_channel_link_collection_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsChannelLinkCollectionEditor *channel_link_collection_editor;

  channel_link_collection_editor = AGS_CHANNEL_LINK_COLLECTION_EDITOR(applicable);

  /* empty */
}

void
ags_channel_link_collection_editor_apply(AgsApplicable *applicable)
{
  AgsChannelLinkCollectionEditor *channel_link_collection_editor;
  GtkTreeIter iter;

  channel_link_collection_editor = AGS_CHANNEL_LINK_COLLECTION_EDITOR(applicable);

  if(gtk_combo_box_get_active_iter(channel_link_collection_editor->link,
				   &iter)){
    AgsMachine *link_machine;
    AgsMachineEditor *machine_editor;
    AgsChannel *channel, *link;
    AgsLinkChannel *link_channel;
    GtkTreeModel *model;
    guint first_line, count;
    guint i;
    GError *error;

    machine_editor = AGS_MACHINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(channel_link_collection_editor),
								AGS_TYPE_MACHINE_EDITOR));

    first_line = (guint) gtk_spin_button_get_value_as_int(channel_link_collection_editor->first_line);

    if(channel_link_collection_editor->channel_type == AGS_TYPE_INPUT)
      channel = ags_channel_nth(machine_editor->machine->audio->input, first_line);
    else
      channel = ags_channel_nth(machine_editor->machine->audio->output, first_line);

    model = gtk_combo_box_get_model(channel_link_collection_editor->link);
    gtk_tree_model_get(model,
		       &iter,
		       1, &link_machine,
		       -1);

    count = (guint) gtk_spin_button_get_value_as_int(channel_link_collection_editor->count);

    error = NULL;

    if(link_machine == NULL){
      for(i = 0; i < count; i++){
	/* create task */
	link_channel = ags_link_channel_new(channel, NULL);

	/* append AgsLinkChannel */
	ags_task_thread_append_task(AGS_DEVOUT(AGS_AUDIO(channel->audio)->devout)->task_thread,
			       AGS_TASK(link_channel));

	channel = channel->next;
      }
    }else{
      guint first_link;

      first_link = (guint) gtk_spin_button_get_value_as_int(channel_link_collection_editor->first_link);


      if(channel_link_collection_editor->channel_type == AGS_TYPE_INPUT)
	link = ags_channel_nth(link_machine->audio->output, first_link);
      else
	link = ags_channel_nth(link_machine->audio->input, first_link);

      for(i = 0; i < count; i++){
	/* create task */
	link_channel = ags_link_channel_new(channel, link);

	/* append AgsLinkChannel */
	ags_task_thread_append_task(AGS_DEVOUT(AGS_AUDIO(channel->audio)->devout)->task_thread,
				    AGS_TASK(link_channel));

	channel = channel->next;
	link = link->next;
      }
    }
  }
}

void
ags_channel_link_collection_editor_reset(AgsApplicable *applicable)
{
  AgsChannelLinkCollectionEditor *channel_link_collection_editor;

  channel_link_collection_editor = AGS_CHANNEL_LINK_COLLECTION_EDITOR(applicable);

  /* empty */
}

void
ags_channel_link_collection_editor_destroy(GtkObject *object)
{
  AgsChannelLinkCollectionEditor *channel_link_collection_editor;

  channel_link_collection_editor = (AgsChannelLinkCollectionEditor *) object;
}

void
ags_channel_link_collection_editor_show(GtkWidget *widget)
{
  AgsChannelLinkCollectionEditor *channel_link_collection_editor = (AgsChannelLinkCollectionEditor *) widget;
}

void
ags_channel_link_collection_editor_check(AgsChannelLinkCollectionEditor *channel_link_collection_editor)
{
  GtkTreeIter iter;

  if(gtk_combo_box_get_active_iter(channel_link_collection_editor->link,
				   &iter)){
    AgsMachine *link_machine;
    AgsMachineEditor *machine_editor;
    GtkTreeModel *model;
    gdouble first_line, first_line_stop, first_line_range;
    gdouble first_link, first_link_stop, first_link_range;
    gdouble max;

    first_line = gtk_spin_button_get_value(channel_link_collection_editor->first_line);
    
    machine_editor = AGS_MACHINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(channel_link_collection_editor),
								AGS_TYPE_MACHINE_EDITOR));

    if(channel_link_collection_editor->channel_type == AGS_TYPE_INPUT)
      first_line_stop = (gdouble) machine_editor->machine->audio->input_lines;
    else
      first_line_stop = (gdouble) machine_editor->machine->audio->output_lines;
    
    
    /* link machine */
    first_link = gtk_spin_button_get_value(channel_link_collection_editor->first_link);

    model = gtk_combo_box_get_model(channel_link_collection_editor->link);
    gtk_tree_model_get(model,
		       &iter,
		       1, &link_machine,
		       -1);

    first_line_range = first_line_stop - first_line;

    if(link_machine != NULL){
      if(channel_link_collection_editor->channel_type == AGS_TYPE_INPUT)
	first_link_stop = (gdouble) link_machine->audio->output_lines;
      else
	first_link_stop = (gdouble) link_machine->audio->input_lines;

      first_link_range = first_link_stop - first_link;

      if(first_line_range > first_link_range)
	max = first_link_range;
      else
	max = first_line_range;
    }else{
      first_link_stop = 0.0;
      max = first_line_range;
    }

    gtk_spin_button_set_range(channel_link_collection_editor->first_line,
			      0.0, first_line_stop - 1.0);

    if(link_machine == NULL)
      gtk_spin_button_set_range(channel_link_collection_editor->first_link,
				0.0, 0.0);
    else
      gtk_spin_button_set_range(channel_link_collection_editor->first_link,
				0.0, first_link_stop - 1.0);

    gtk_spin_button_set_range(channel_link_collection_editor->count,
			      0.0, max);
  }else{
    gtk_spin_button_set_range(channel_link_collection_editor->count,
			      -1.0, -1.0);
  }
}

AgsChannelLinkCollectionEditor*
ags_channel_link_collection_editor_new(GType channel_type)
{
  AgsChannelLinkCollectionEditor *channel_link_collection_editor;

  channel_link_collection_editor = (AgsChannelLinkCollectionEditor *) g_object_new(AGS_TYPE_CHANNEL_LINK_COLLECTION_EDITOR,
										   "channel_type\0", channel_type,
										   NULL);
  
  return(channel_link_collection_editor);
}
