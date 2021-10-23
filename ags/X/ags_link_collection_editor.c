/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/X/ags_link_collection_editor.h>
#include <ags/X/ags_link_collection_editor_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_machine_editor.h>

#include <ags/i18n.h>

void ags_link_collection_editor_class_init(AgsLinkCollectionEditorClass *link_collection_editor);
void ags_link_collection_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_link_collection_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_link_collection_editor_init(AgsLinkCollectionEditor *link_collection_editor);
void ags_link_collection_editor_set_property(GObject *gobject,
					     guint prop_id,
					     const GValue *value,
					     GParamSpec *param_spec);
void ags_link_collection_editor_get_property(GObject *gobject,
					     guint prop_id,
					     GValue *value,
					     GParamSpec *param_spec);

void ags_link_collection_editor_connect(AgsConnectable *connectable);
void ags_link_collection_editor_disconnect(AgsConnectable *connectable);

void ags_link_collection_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_link_collection_editor_apply(AgsApplicable *applicable);
void ags_link_collection_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_link_collection_editor
 * @short_description: edit links in bulk mode.
 * @title: AgsLinkCollectionEditor
 * @section_id:
 * @include: ags/X/ags_link_collection_editor.h
 *
 * #AgsLinkCollectionEditor is a composite widget to modify links in bulk mode. A link collection
 * editor should be packed by a #AgsMachineEditor.
 */

enum{
  PROP_0,
  PROP_CHANNEL_TYPE,
};

static gpointer ags_link_collection_editor_parent_class = NULL;

GType
ags_link_collection_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_link_collection_editor = 0;

    static const GTypeInfo ags_link_collection_editor_info = {
      sizeof (AgsLinkCollectionEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_link_collection_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLinkCollectionEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_link_collection_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_link_collection_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_link_collection_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_link_collection_editor = g_type_register_static(GTK_TYPE_GRID,
							     "AgsLinkCollectionEditor",
							     &ags_link_collection_editor_info,
							     0);
    
    g_type_add_interface_static(ags_type_link_collection_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_link_collection_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_link_collection_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_link_collection_editor_class_init(AgsLinkCollectionEditorClass *link_collection_editor)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_link_collection_editor_parent_class = g_type_class_peek_parent(link_collection_editor);

  /* GObjectClass */
  gobject = (GObjectClass *) link_collection_editor;

  gobject->set_property = ags_link_collection_editor_set_property;
  gobject->get_property = ags_link_collection_editor_get_property;

  /* properties */
  /**
   * AgsLinkCollectionEditor:channel-type:
   *
   * The channel type to apply to. Either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_gtype("channel-type",
				  i18n_pspec("assigned channel type"),
				  i18n_pspec("The channel type which this channel link collection editor is assigned with"),
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL_TYPE,
				  param_spec);
}

void
ags_link_collection_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_link_collection_editor_connect;
  connectable->disconnect = ags_link_collection_editor_disconnect;
}

void
ags_link_collection_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_link_collection_editor_set_update;
  applicable->apply = ags_link_collection_editor_apply;
  applicable->reset = ags_link_collection_editor_reset;
}

void
ags_link_collection_editor_init(AgsLinkCollectionEditor *link_collection_editor)
{
  GtkLabel *label;

  GtkCellRenderer *cell_renderer;

  GtkListStore *model;

  GtkTreeIter iter;

  link_collection_editor->flags = 0;
  
  g_signal_connect_after(GTK_WIDGET(link_collection_editor), "parent_set",
			 G_CALLBACK(ags_link_collection_editor_parent_set_callback), link_collection_editor);

  link_collection_editor->channel_type = G_TYPE_NONE;
  
  /* link */
  label = (GtkLabel *) gtk_label_new(i18n("link"));

  gtk_label_set_yalign(label,
		       0.5);

  gtk_widget_set_valign(label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign(label,
			GTK_ALIGN_START);
  gtk_widget_set_vexpand(label,
			 TRUE);
  gtk_widget_set_hexpand(label,
			 TRUE);
  
  gtk_grid_attach((GtkGrid *) link_collection_editor,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);

  link_collection_editor->link = (GtkComboBox *) gtk_combo_box_new();

  gtk_widget_set_valign(label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign(label,
			GTK_ALIGN_START);
  gtk_widget_set_vexpand(label,
			 TRUE);
  gtk_widget_set_hexpand(label,
			 TRUE);
  
  gtk_grid_attach((GtkGrid *) link_collection_editor,
		  (GtkWidget *) link_collection_editor->link,
		  1, 0,
		  1, 1);
		    
  model = gtk_list_store_new(2,
			     G_TYPE_STRING,
			     G_TYPE_POINTER);
		    
  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "NULL",
		     1, NULL,
		     -1);
  
  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(link_collection_editor->link),
			     cell_renderer,
			     FALSE); 
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(link_collection_editor->link),
				 cell_renderer,
				 "text", 0,
				 NULL);

  gtk_combo_box_set_model(link_collection_editor->link,
			  GTK_TREE_MODEL(model));

  /* first line */
  label = (GtkLabel *) gtk_label_new(i18n("first line"));

  gtk_widget_set_valign(label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign(label,
			GTK_ALIGN_START);
  gtk_widget_set_vexpand(label,
			 TRUE);
  gtk_widget_set_hexpand(label,
			 TRUE);

  gtk_grid_attach((GtkGrid *) link_collection_editor,
		  (GtkWidget *) label,
		   0, 1,
		   1, 1);

  link_collection_editor->first_line = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_grid_attach((GtkGrid *) link_collection_editor,
		  (GtkWidget *) link_collection_editor->first_line,
		  1, 1,
		  1, 1);

  /* first link line */
  label = (GtkLabel *) gtk_label_new(i18n("first link line"));

  gtk_widget_set_valign(label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign(label,
			GTK_ALIGN_START);
  gtk_widget_set_vexpand(label,
			 TRUE);
  gtk_widget_set_hexpand(label,
			 TRUE);

  gtk_grid_attach((GtkGrid *) link_collection_editor,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  link_collection_editor->first_link = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_grid_attach((GtkGrid *) link_collection_editor,
		  (GtkWidget *) link_collection_editor->first_link,
		  1, 2,
		  1, 1);

  /* count */
  label = (GtkLabel *) gtk_label_new(i18n("count"));

  gtk_widget_set_valign(label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign(label,
			GTK_ALIGN_START);
  gtk_widget_set_vexpand(label,
			 TRUE);
  gtk_widget_set_hexpand(label,
			 TRUE);

  gtk_grid_attach((GtkGrid *) link_collection_editor,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);

  link_collection_editor->count = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_grid_attach((GtkGrid *) link_collection_editor,
		  (GtkWidget *) link_collection_editor->count,
		  1, 3,
		  1, 1);
}

void
ags_link_collection_editor_set_property(GObject *gobject,
						guint prop_id,
						const GValue *value,
						GParamSpec *param_spec)
{
  AgsLinkCollectionEditor *link_collection_editor;

  link_collection_editor = AGS_LINK_COLLECTION_EDITOR(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
    link_collection_editor->channel_type = g_value_get_gtype(value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_link_collection_editor_get_property(GObject *gobject,
						guint prop_id,
						GValue *value,
						GParamSpec *param_spec)
{
  AgsLinkCollectionEditor *link_collection_editor;

  link_collection_editor = AGS_LINK_COLLECTION_EDITOR(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
    g_value_set_gtype(value, link_collection_editor->channel_type);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_link_collection_editor_connect(AgsConnectable *connectable)
{
  AgsLinkCollectionEditor *link_collection_editor;

  link_collection_editor = AGS_LINK_COLLECTION_EDITOR(connectable);

  if((AGS_LINK_COLLECTION_EDITOR_CONNECTED & (link_collection_editor->flags)) != 0){
    return;
  }

  link_collection_editor->flags |= AGS_LINK_COLLECTION_EDITOR_CONNECTED;
  
  /* AgsLinkCollectionEditor */
  g_signal_connect_after(G_OBJECT(link_collection_editor->link), "changed",
			 G_CALLBACK(ags_link_collection_editor_link_callback), link_collection_editor);

  g_signal_connect_after(G_OBJECT(link_collection_editor->first_line), "value-changed",
			 G_CALLBACK(ags_link_collection_editor_first_line_callback), link_collection_editor);

  g_signal_connect_after(G_OBJECT(link_collection_editor->first_link), "value-changed",
			 G_CALLBACK(ags_link_collection_editor_first_link_callback), link_collection_editor);
}

void
ags_link_collection_editor_disconnect(AgsConnectable *connectable)
{
  AgsLinkCollectionEditor *link_collection_editor;

  link_collection_editor = AGS_LINK_COLLECTION_EDITOR(connectable);

  if((AGS_LINK_COLLECTION_EDITOR_CONNECTED & (link_collection_editor->flags)) == 0){
    return;
  }

  link_collection_editor->flags &= (~AGS_LINK_COLLECTION_EDITOR_CONNECTED);
  
  /* AgsLinkCollectionEditor */
  g_object_disconnect(G_OBJECT(link_collection_editor->link),
		      "any_signal::changed",
		      G_CALLBACK(ags_link_collection_editor_link_callback),
		      link_collection_editor,
		      NULL);

  g_object_disconnect(G_OBJECT(link_collection_editor->first_line),
		      "any_signal::value-changed",
		      G_CALLBACK(ags_link_collection_editor_first_line_callback),
		      link_collection_editor,
		      NULL);

  g_object_disconnect(G_OBJECT(link_collection_editor->first_link),
		      "any_signal::value-changed",
		      G_CALLBACK(ags_link_collection_editor_first_link_callback),
		      link_collection_editor,
		      NULL);
}

void
ags_link_collection_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_link_collection_editor_apply(AgsApplicable *applicable)
{
  AgsLinkCollectionEditor *link_collection_editor;
  GtkTreeIter iter;

  link_collection_editor = AGS_LINK_COLLECTION_EDITOR(applicable);

  if(gtk_combo_box_get_active_iter(link_collection_editor->link,
				   &iter)){
    AgsMachine *machine, *link_machine;
    AgsMachineEditor *machine_editor;

    GtkTreeModel *model;

    AgsAudio *audio;
    AgsChannel *start_channel, *start_link;
    AgsChannel *channel, *next_channel, *nth_channel;
    AgsChannel *link, *next_link, *nth_link;
    AgsLinkChannel *link_channel;

    AgsApplicationContext *application_context;

    GList *task;

    guint first_line, count;
    guint i;

    machine_editor = AGS_MACHINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(link_collection_editor),
								AGS_TYPE_MACHINE_EDITOR));
    machine = machine_editor->machine;
    audio = machine->audio;

    /* get application_context */
    application_context = ags_application_context_get_instance();

    /* get first line */
    first_line = (guint) gtk_spin_button_get_value_as_int(link_collection_editor->first_line);

    if(g_type_is_a(link_collection_editor->channel_type, AGS_TYPE_INPUT)){
      g_object_get(audio,
		   "input", &start_channel,
		   NULL);
      
      nth_channel = ags_channel_nth(start_channel, first_line);

      channel = nth_channel;      
    }else{
      g_object_get(audio,
		   "output", &start_channel,
		   NULL);

      nth_channel = ags_channel_nth(start_channel, first_line);

      channel = nth_channel;      
    }
    
    model = gtk_combo_box_get_model(link_collection_editor->link);
    gtk_tree_model_get(model,
		       &iter,
		       1, &link_machine,
		       -1);
    
    task = NULL;

    count = (guint) gtk_spin_button_get_value_as_int(link_collection_editor->count);

    if(link_machine == NULL){
      next_channel = NULL;
      
      for(i = 0; i < count; i++){ 
	/* create task */
	link_channel = ags_link_channel_new(channel,
					    NULL);
	task = g_list_prepend(task,
			      link_channel);
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(next_channel != NULL){
	g_object_unref(next_channel);
      }
      
      /* append AgsLinkChannel */
      task = g_list_reverse(task);
      ags_ui_provider_schedule_task_all(AGS_UI_PROVIDER(application_context),
					task);
    }else{
      guint first_link;

      first_link = (guint) gtk_spin_button_get_value_as_int(link_collection_editor->first_link);

      /* get link */
      if(g_type_is_a(link_collection_editor->channel_type, AGS_TYPE_INPUT)){
	g_object_get(link_machine->audio,
		     "output", &start_link,
		     NULL);
      }else{
	g_object_get(link_machine->audio,
		     "input", &start_link,
		     NULL);
      }
      
      nth_link = ags_channel_nth(start_link,
				 first_link);

      link = nth_link;

      next_channel = NULL;
      next_link = NULL;
      
      for(i = 0; i < count; i++){
	/* create task */
	link_channel = ags_link_channel_new(channel,
					    link);
	task = g_list_prepend(task,
			      link_channel);

	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;

	next_link = ags_channel_next(link);

	g_object_unref(link);

	link = next_link;
      }

      task = g_list_reverse(task);
      ags_ui_provider_schedule_task_all(AGS_UI_PROVIDER(application_context),
					task);

      g_object_unref(start_link);

      if(next_channel != NULL){
	g_object_unref(next_channel);
      }

      if(next_link != NULL){
	g_object_unref(next_link);
      }
    }

    g_object_unref(start_channel);
  }
}

void
ags_link_collection_editor_reset(AgsApplicable *applicable)
{
  /* empty */
}

/**
 * ags_link_collection_editor_check:
 * @link_collection_editor: the #AgsLinkCollectionEditor
 *
 * Checks for possible channels to link. And modifies its ranges.
 * 
 * Since: 3.0.0
 */
void
ags_link_collection_editor_check(AgsLinkCollectionEditor *link_collection_editor)
{
  GtkTreeIter iter;

  if(gtk_combo_box_get_active_iter(link_collection_editor->link,
				   &iter)){
    AgsMachine *link_machine;
    AgsMachineEditor *machine_editor;

    GtkTreeModel *model;

    guint first_line, first_line_stop, first_line_range;
    guint first_link, first_link_stop, first_link_range;
    guint max;

    first_line = gtk_spin_button_get_value(link_collection_editor->first_line);
    
    machine_editor = AGS_MACHINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(link_collection_editor),
								AGS_TYPE_MACHINE_EDITOR));

    if(link_collection_editor->channel_type == AGS_TYPE_INPUT){
      g_object_get(machine_editor->machine->audio,
		   "input-lines", &first_line_stop,
		   NULL);
    }else{
      g_object_get(machine_editor->machine->audio,
		   "output-lines", &first_line_stop,
		   NULL);
    }
    
    /* link machine */
    first_link = gtk_spin_button_get_value(link_collection_editor->first_link);

    model = gtk_combo_box_get_model(link_collection_editor->link);
    gtk_tree_model_get(model,
		       &iter,
		       1, &link_machine,
		       -1);

    first_line_range = first_line_stop - first_line;

    if(link_machine != NULL){
      if(link_collection_editor->channel_type == AGS_TYPE_INPUT){
	g_object_get(link_machine->audio,
		     "output-lines", &first_link_stop,
		     NULL);
      }else{
	g_object_get(link_machine->audio,
		     "input-lines", &first_link_stop,
		     NULL);
      }
      
      first_link_range = first_link_stop - first_link;

      if(first_line_range > first_link_range){
	max = first_link_range;
      }else{
	max = first_line_range;
      }
    }else{
      first_link_stop = 0.0;
      max = first_line_range;
    }

    gtk_spin_button_set_range(link_collection_editor->first_line,
			      0.0, first_line_stop - 1.0);

    if(link_machine == NULL){
      gtk_spin_button_set_range(link_collection_editor->first_link,
				0.0, 0.0);
    }else{
      gtk_spin_button_set_range(link_collection_editor->first_link,
				0.0, first_link_stop - 1.0);
    }
    
    gtk_spin_button_set_range(link_collection_editor->count,
			      0.0, max);
  }else{
    gtk_spin_button_set_range(link_collection_editor->count,
			      -1.0, -1.0);
  }
}

/**
 * ags_link_collection_editor_new:
 * @channel_type: either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT
 *
 * Create a new instance of #AgsLinkCollectionEditor
 *
 * Returns: the new #AgsLinkCollectionEditor
 *
 * Since: 3.0.0
 */
AgsLinkCollectionEditor*
ags_link_collection_editor_new(GType channel_type)
{
  AgsLinkCollectionEditor *link_collection_editor;
  
  link_collection_editor = (AgsLinkCollectionEditor *) g_object_new(AGS_TYPE_LINK_COLLECTION_EDITOR,
								    "channel-type", channel_type,
								    NULL);
  
  return(link_collection_editor);
}
