/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/ags_notation_editor.h>
#include <ags/X/ags_notation_editor_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

#include <ags/X/ags_window.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <math.h>

#include <ags/config.h>
#include <ags/i18n.h>


void ags_notation_editor_class_init(AgsNotationEditorClass *notation_editor);
void ags_notation_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_notation_editor_init(AgsNotationEditor *notation_editor);
void ags_notation_editor_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_notation_editor_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_notation_editor_connect(AgsConnectable *connectable);
void ags_notation_editor_disconnect(AgsConnectable *connectable);
void ags_notation_editor_finalize(GObject *gobject);

void ags_notation_editor_real_machine_changed(AgsNotationEditor *notation_editor, AgsMachine *machine);

enum{
  MACHINE_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_SOUNDCARD,
};

static gpointer ags_notation_editor_parent_class = NULL;
static guint notation_editor_signals[LAST_SIGNAL];

/**
 * SECTION:ags_notation_editor
 * @short_description: A composite widget to edit notation
 * @title: AgsNotationEditor
 * @section_id:
 * @include: ags/X/ags_notation_editor.h
 *
 * #AgsNotationEditor is a composite widget to edit notation. You may select machines
 * or change editor tool to do notation.
 */

GType
ags_notation_editor_get_type(void)
{
  static GType ags_type_notation_editor = 0;

  if(!ags_type_notation_editor){
    static const GTypeInfo ags_notation_editor_info = {
      sizeof (AgsNotationEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_notation_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsNotationEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_notation_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_notation_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_notation_editor = g_type_register_static(GTK_TYPE_VBOX,
						      "AgsNotationEditor", &ags_notation_editor_info,
						      0);
    
    g_type_add_interface_static(ags_type_notation_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_notation_editor);
}

void
ags_notation_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_notation_editor_connect;
  connectable->disconnect = ags_notation_editor_disconnect;
}

void
ags_notation_editor_class_init(AgsNotationEditorClass *notation_editor)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_notation_editor_parent_class = g_type_class_peek_parent(notation_editor);

  /* GObjectClass */
  gobject = (GObjectClass *) notation_editor;

  gobject->set_property = ags_notation_editor_set_property;
  gobject->get_property = ags_notation_editor_get_property;

  gobject->finalize = ags_notation_editor_finalize;
  
  /* properties */
  /**
   * AgsNotationEditor:soundcard:
   *
   * The assigned #AgsSoundcard acting as default sink.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("assigned soundcard"),
				   i18n_pspec("The soundcard it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /* AgsEditorClass */
  notation_editor->machine_changed = ags_notation_editor_real_machine_changed;

  /* signals */
  /**
   * AgsEditor::machine-changed:
   * @editor: the object to change machine.
   * @machine: the #AgsMachine to set
   *
   * The ::machine-changed signal notifies about changed machine.
   * 
   * Since: 1.0.0
   */
  notation_editor_signals[MACHINE_CHANGED] =
    g_signal_new("machine-changed",
                 G_TYPE_FROM_CLASS(notation_editor),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsNotationEditorClass, machine_changed),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_notation_editor_init(AgsNotationEditor *notation_editor)
{
  GtkHPaned *paned;
  GtkScrolledWindow *scrolled_window;
  GtkTable *table;
  
  notation_editor->flags = 0;

  notation_editor->version = AGS_NOTATION_EDITOR_DEFAULT_VERSION;
  notation_editor->build_id = AGS_NOTATION_EDITOR_DEFAULT_BUILD_ID;

  /* offset */
  notation_editor->tact_counter = 0;
  notation_editor->current_tact = 0.0;

  /* soundcard */
  notation_editor->soundcard = NULL;

  /* notation toolbar */
  notation_editor->notation_toolbar = ags_notation_toolbar_new();
  gtk_box_pack_start((GtkBox *) notation_editor,
		     (GtkWidget *) notation_editor->notation_toolbar,
		     FALSE, FALSE, 0);

  /* machine selector */
  paned = (GtkHPaned *) gtk_hpaned_new();
  gtk_box_pack_start((GtkBox *) notation_editor,
		     (GtkWidget *) paned,
		     TRUE, TRUE, 0);

  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_paned_pack1((GtkPaned *) paned, (GtkWidget *) scrolled_window, FALSE, TRUE);

  notation_editor->machine_selector = g_object_new(AGS_TYPE_MACHINE_SELECTOR,
						   "homogeneous", FALSE,
						   "spacing", 0,
						   NULL);
  notation_editor->machine_selector->flags |= (AGS_MACHINE_SELECTOR_SHOW_REVERSE_MAPPING |
					       AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO |
					       AGS_MACHINE_SELECTOR_NOTATION);
  gtk_label_set_label(notation_editor->machine_selector->label,
		      i18n("notation"));
  
  notation_editor->machine_selector->popup = ags_machine_selector_popup_new(notation_editor->machine_selector);
  g_object_set(notation_editor->machine_selector->menu_button,
	       "menu", notation_editor->machine_selector->popup,
	       NULL);
  
  gtk_scrolled_window_add_with_viewport(scrolled_window,
					(GtkWidget *) notation_editor->machine_selector);

  /* selected machine */
  notation_editor->selected_machine = NULL;

  /* notation edit */
  table = (GtkTable *) gtk_table_new(4, 3,
				     FALSE);
  gtk_paned_pack2((GtkPaned *) paned,
		  (GtkWidget *) table,
		  TRUE, FALSE);

  notation_editor->notebook = g_object_new(AGS_TYPE_NOTEBOOK,
					   "homogeneous", FALSE,
					   "spacing", 0,
					   "prefix", i18n("channel"),
					   NULL);
  gtk_table_attach(table, (GtkWidget *) notation_editor->notebook,
		   0, 3,
		   0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL,
		   0, 0);
}

void
ags_notation_editor_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsNotationEditor *notation_editor;

  notation_editor = AGS_NOTATION_EDITOR(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = g_value_get_object(value);

      if(notation_editor->soundcard == soundcard){
	return;
      }

      if(notation_editor->soundcard != NULL){
	g_object_unref(notation_editor->soundcard);
      }
      
      if(soundcard != NULL){
	g_object_ref(soundcard);
      }
      
      notation_editor->soundcard = soundcard;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_notation_editor_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsNotationEditor *notation_editor;

  notation_editor = AGS_NOTATION_EDITOR(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, notation_editor->soundcard);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_notation_editor_connect(AgsConnectable *connectable)
{
  AgsNotationEditor *notation_editor;

  notation_editor = AGS_NOTATION_EDITOR(connectable);

  if((AGS_NOTATION_EDITOR_CONNECTED & (notation_editor->flags)) != 0){
    return;
  }

  notation_editor->flags |= AGS_NOTATION_EDITOR_CONNECTED;  
  
  g_signal_connect((GObject *) notation_editor->machine_selector, "changed",
		   G_CALLBACK(ags_notation_editor_machine_changed_callback), (gpointer) notation_editor);

  /*  */
  ags_connectable_connect(AGS_CONNECTABLE(notation_editor->notation_toolbar));
  ags_connectable_connect(AGS_CONNECTABLE(notation_editor->machine_selector));
}

void
ags_notation_editor_disconnect(AgsConnectable *connectable)
{
  AgsNotationEditor *notation_editor;

  notation_editor = AGS_NOTATION_EDITOR(connectable);

  ags_connectable_disconnect(AGS_CONNECTABLE(notation_editor->notation_toolbar)); 
  ags_connectable_disconnect(AGS_CONNECTABLE(notation_editor->machine_selector));
}

void
ags_notation_editor_finalize(GObject *gobject)
{
  AgsNotationEditor *notation_editor;

  notation_editor = AGS_NOTATION_EDITOR(gobject);
  
  G_OBJECT_CLASS(ags_notation_editor_parent_class)->finalize(gobject);
}

