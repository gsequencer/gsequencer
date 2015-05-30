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

#include <ags/X/ags_editor.h>
#include <ags/X/ags_editor_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_notation.h>

#include <ags/X/ags_window.h>

#include <math.h>
#include <cairo.h>

void ags_editor_class_init(AgsEditorClass *editor);
void ags_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_editor_init(AgsEditor *editor);
void ags_editor_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_editor_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_editor_finalize(GObject *gobject);
void ags_editor_connect(AgsConnectable *connectable);
void ags_editor_disconnect(AgsConnectable *connectable);
void ags_editor_destroy(GtkObject *object);
void ags_editor_show(GtkWidget *widget);

void ags_editor_real_machine_changed(AgsEditor *editor, AgsMachine *machine);

/**
 * SECTION:ags_editor
 * @short_description: A composite widget to edit notes
 * @title: AgsEditor
 * @section_id:
 * @include: ags/X/ags_editor.h
 *
 * #AgsEditor is a composite widget to edit notes. You may select machines
 * or change editor tool to do notation.
 */

enum{
  MACHINE_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_SOUNDCARD,
};

static gpointer ags_editor_parent_class = NULL;
static guint editor_signals[LAST_SIGNAL];

GtkStyle *editor_style;

GType
ags_editor_get_type(void)
{
  static GType ags_type_editor = 0;

  if(!ags_type_editor){
    static const GTypeInfo ags_editor_info = {
      sizeof (AgsEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_editor = g_type_register_static(GTK_TYPE_VBOX,
					     "AgsEditor\0", &ags_editor_info,
					     0);
    
    g_type_add_interface_static(ags_type_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_editor);
}

void
ags_editor_class_init(AgsEditorClass *editor)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_editor_parent_class = g_type_class_peek_parent(editor);

  /* GObjectClass */
  gobject = (GObjectClass *) editor;

  gobject->set_property = ags_editor_set_property;
  gobject->get_property = ags_editor_get_property;

  gobject->finalize = ags_editor_finalize;

  /* properties */
  /**
   * AgsEditor:soundcard:
   *
   * The assigned #AgsSoundcard acting as default sink.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("soundcard\0",
				   "assigned soundcard\0",
				   "The soundcard it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /* AgsEditorClass */
  editor->machine_changed = ags_editor_real_machine_changed;

  /* signals */
  /**
   * AgsEditor::machine-changed:
   * @editor: the object to change machine.
   * @machine: the #AgsMachine to set
   *
   * The ::machine-changed signal notifies about changed machine.
   */
  editor_signals[MACHINE_CHANGED] =
    g_signal_new("machine-changed\0",
                 G_TYPE_FROM_CLASS (editor),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsEditorClass, machine_changed),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_editor_connect;
  connectable->disconnect = ags_editor_disconnect;
}

void
ags_editor_init(AgsEditor *editor)
{
  GtkHPaned *paned;
  GtkScrolledWindow *scrolled_window;

  g_signal_connect_after((GObject *) editor, "parent-set\0",
			 G_CALLBACK(ags_editor_parent_set_callback), editor);

  editor->flags = 0;

  editor->version = AGS_EDITOR_DEFAULT_VERSION;
  editor->build_id = AGS_EDITOR_DEFAULT_BUILD_ID;

  editor->soundcard = NULL;

  editor->toolbar = ags_toolbar_new();
  gtk_box_pack_start((GtkBox *) editor,
		     (GtkWidget *) editor->toolbar,
		     FALSE, FALSE,
		     0);

  paned = (GtkHPaned *) gtk_hpaned_new();
  gtk_box_pack_start((GtkBox *) editor,
		     (GtkWidget *) paned,
		     TRUE, TRUE,
		     0);

  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_paned_pack1((GtkPaned *) paned, (GtkWidget *) scrolled_window, FALSE, TRUE);
  //  gtk_widget_set_size_request((GtkWidget *) scrolled_window, 180, -1);

  editor->machine_selector = ags_machine_selector_new();
  gtk_scrolled_window_add_with_viewport(scrolled_window,
					(GtkWidget *) editor->machine_selector);

  editor->selected_machine = NULL;

  editor->table = (GtkTable *) gtk_table_new(4, 3, FALSE);
  gtk_paned_pack2((GtkPaned *) paned,
		  (GtkWidget *) editor->table,
		  TRUE, FALSE);
  
  editor->notebook = ags_notebook_new();
  editor->notebook->flags |= AGS_NOTEBOOK_SHOW_AUDIO_CHANNEL;
  gtk_table_attach(editor->table,
		   (GtkWidget *) editor->notebook,
		   0, 3,
		   0, 1,
		   GTK_FILL|GTK_EXPAND,
		   GTK_FILL,
		   0, 0);

  editor->meter.piano = NULL;
  editor->edit.note_edit = NULL;

  editor->tact_counter = 0;
}

void
ags_editor_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsEditor *editor;

  editor = AGS_EDITOR(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      if(editor->soundcard == soundcard)
	return;

      if(soundcard != NULL)
	g_object_ref(soundcard);

      editor->soundcard = soundcard;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_editor_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsEditor *editor;

  editor = AGS_EDITOR(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    g_value_set_object(value, editor->soundcard);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_editor_finalize(GObject *gobject)
{
  //TODO:JK: implement me
}

void
ags_editor_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsEditor *editor;
  GtkHPaned *hpaned;

  editor = AGS_EDITOR(connectable);
  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(editor)));

  /*  */
  g_signal_connect_after((GObject *) window->navigation, "change-position\0",
			 G_CALLBACK(ags_editor_change_position_callback), (gpointer) editor);

  g_signal_connect((GObject *) editor->machine_selector, "changed\0",
		   G_CALLBACK(ags_editor_machine_changed_callback), (gpointer) editor);

  /*  */
  ags_connectable_connect(AGS_CONNECTABLE(editor->toolbar));
  ags_connectable_connect(AGS_CONNECTABLE(editor->machine_selector));
  ags_connectable_connect(AGS_CONNECTABLE(editor->notebook));
}

void
ags_editor_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_editor_real_machine_changed(AgsEditor *editor, AgsMachine *machine)
{
  GtkTable *table;
  guint pads;

  if(editor->selected_machine == machine){
    return;
  }

  editor->selected_machine = machine;

  if(machine == NULL){
    return;
  }

  table = editor->table;

  editor->set_audio_channels_handler = g_signal_connect(machine->audio, "set-audio-channels\0",
							G_CALLBACK(ags_editor_set_audio_channels_callback), editor);
  editor->set_pads_handler = g_signal_connect(machine->audio, "set-pads\0",
					      G_CALLBACK(ags_editor_set_pads_callback), editor);

  if((AGS_AUDIO_NOTATION_DEFAULT & (machine->audio->flags)) != 0){
    pads = machine->audio->input_pads;
  }else{
    pads = machine->audio->output_pads;
  }

  if((AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
    editor->flags |= AGS_EDITOR_TOOL_NOTE_EDIT;

    editor->meter.piano = ags_piano_new();
    gtk_table_attach(editor->table, (GtkWidget *) editor->meter.piano,
		     0, 1, 1, 2,
		     GTK_FILL, GTK_FILL,
		     0, 0);
    ags_connectable_connect(AGS_CONNECTABLE(editor->meter.piano));
    gtk_widget_show_all(editor->meter.piano);

    editor->edit.note_edit = ags_note_edit_new();
    gtk_table_attach(table, (GtkWidget *) editor->edit.note_edit,
		     1, 2, 1, 2,
		     GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		     0, 0);

    g_signal_connect(editor->edit.note_edit->vscrollbar, "value-changed\0",
		     G_CALLBACK(ags_editor_edit_vscrollbar_value_changed_callback), editor);
    ags_connectable_connect(AGS_CONNECTABLE(editor->edit.note_edit));
    gtk_widget_show_all(editor->edit.note_edit);

    ags_note_edit_set_map_height(editor->edit.note_edit,
				 pads * editor->edit.note_edit->control_height);
  }else if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0){
    editor->flags |= AGS_EDITOR_TOOL_PATTERN_EDIT;

    editor->meter.soundset = ags_soundset_new();
    gtk_table_attach(editor->table, (GtkWidget *) editor->meter.soundset,
		     0, 1, 1, 2,
		     GTK_FILL, GTK_FILL,
		     0, 0);
    ags_connectable_connect(AGS_CONNECTABLE(editor->meter.soundset));
    gtk_widget_show_all(editor->meter.soundset);

    editor->edit.pattern_edit = ags_pattern_edit_new();
    gtk_table_attach(table, (GtkWidget *) editor->edit.pattern_edit,
		     1, 2, 1, 2,
		     GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		     0, 0);

    g_signal_connect(editor->edit.pattern_edit->vscrollbar, "value-changed\0",
		     G_CALLBACK(ags_editor_edit_vscrollbar_value_changed_callback), editor);
    ags_connectable_connect(AGS_CONNECTABLE(editor->edit.pattern_edit));
    gtk_widget_show_all(editor->edit.pattern_edit);
    
    ags_pattern_edit_set_map_height(editor->edit.pattern_edit,
 				    pads * editor->edit.pattern_edit->control_height);
  }else{
    /* empty */
  }
}

/**
 * ags_editor_machine_changed:
 * @editor: an #AgsEditor
 * @machine: the new #AgsMachine
 *
 * Is emitted as machine changed of editor.
 *
 * Since: 0.4
 */
void
ags_editor_machine_changed(AgsEditor *editor, AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_EDITOR(editor));

  g_object_ref((GObject *) editor);
  g_signal_emit((GObject *) editor,
		editor_signals[MACHINE_CHANGED], 0,
		machine);
  g_object_unref((GObject *) editor);
}

/**
 * ags_editor_new:
 *
 * Creates an #AgsEditor
 *
 * Returns: a new #AgsEditor
 *
 * Since: 0.3
 */
AgsEditor*
ags_editor_new()
{
  AgsEditor *editor;

  editor = (AgsEditor *) g_object_new(AGS_TYPE_EDITOR, NULL);

  return(editor);
}
