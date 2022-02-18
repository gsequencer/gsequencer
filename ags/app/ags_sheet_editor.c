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

#include <ags/app/ags_sheet_editor.h>

#include <ags/i18n.h>

void ags_sheet_editor_class_init(AgsSheetEditorClass *sheet_editor);
void ags_sheet_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sheet_editor_init(AgsSheetEditor *sheet_editor);
void ags_sheet_editor_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_sheet_editor_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_sheet_editor_connect(AgsConnectable *connectable);
void ags_sheet_editor_disconnect(AgsConnectable *connectable);
void ags_sheet_editor_finalize(GObject *gobject);

void ags_sheet_editor_real_machine_changed(AgsSheetEditor *sheet_editor,
					   AgsMachine *machine);

enum{
  MACHINE_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_SOUNDCARD,
};

static gpointer ags_sheet_editor_parent_class = NULL;
static guint sheet_editor_signals[LAST_SIGNAL];

/**
 * SECTION:ags_sheet_editor
 * @short_description: A composite widget to edit notation
 * @title: AgsSheetEditor
 * @section_id:
 * @include: ags/X/ags_sheet_editor.h
 *
 * #AgsSheetEditor is a composite widget to edit notation. You may select machines
 * or change sheet tool to do notation.
 */

GType
ags_sheet_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sheet_editor = 0;

    static const GTypeInfo ags_sheet_editor_info = {
      sizeof (AgsSheetEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sheet_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSheetEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sheet_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sheet_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_sheet_editor = g_type_register_static(GTK_TYPE_BOX,
						   "AgsSheetEditor", &ags_sheet_editor_info,
						   0);
    
    g_type_add_interface_static(ags_type_sheet_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sheet_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_sheet_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_sheet_editor_connect;
  connectable->disconnect = ags_sheet_editor_disconnect;
}

void
ags_sheet_editor_class_init(AgsSheetEditorClass *sheet_editor)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_sheet_editor_parent_class = g_type_class_peek_parent(sheet_editor);

  /* GObjectClass */
  gobject = (GObjectClass *) sheet_editor;

  gobject->set_property = ags_sheet_editor_set_property;
  gobject->get_property = ags_sheet_editor_get_property;

  gobject->finalize = ags_sheet_editor_finalize;
  
  /* properties */
  /**
   * AgsSheetEditor:soundcard:
   *
   * The assigned #AgsSoundcard acting as default sink.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("assigned soundcard"),
				   i18n_pspec("The soundcard it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /* AgsSheetClass */
  sheet_editor->machine_changed = ags_sheet_editor_real_machine_changed;

  /* signals */
  /**
   * AgsSheet::machine-changed:
   * @sheet: the object to change machine.
   * @machine: the #AgsMachine to set
   *
   * The ::machine-changed signal notifies about changed machine.
   * 
   * Since: 3.0.0
   */
  sheet_editor_signals[MACHINE_CHANGED] =
    g_signal_new("machine-changed",
                 G_TYPE_FROM_CLASS(sheet_editor),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSheetEditorClass, machine_changed),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_sheet_editor_init(AgsSheetEditor *sheet_editor)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(sheet_editor),
				 GTK_ORIENTATION_VERTICAL);  

  //TODO:JK: implement me
}

void
ags_sheet_editor_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsSheetEditor *sheet_editor;

  sheet_editor = AGS_SHEET_EDITOR(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = g_value_get_object(value);

      if(sheet_editor->soundcard == soundcard){
	return;
      }

      if(sheet_editor->soundcard != NULL){
	g_object_unref(sheet_editor->soundcard);
      }
      
      if(soundcard != NULL){
	g_object_ref(soundcard);
      }
      
      sheet_editor->soundcard = soundcard;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_sheet_editor_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsSheetEditor *sheet_editor;

  sheet_editor = AGS_SHEET_EDITOR(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, sheet_editor->soundcard);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_sheet_editor_connect(AgsConnectable *connectable)
{
  AgsSheetEditor *sheet_editor;

  sheet_editor = AGS_SHEET_EDITOR(connectable);

  if((AGS_SHEET_EDITOR_CONNECTED & (sheet_editor->flags)) != 0){
    return;
  }

  sheet_editor->flags |= AGS_SHEET_EDITOR_CONNECTED;  
  
  //  g_signal_connect((GObject *) sheet_editor->machine_selector, "changed",
  //		   G_CALLBACK(ags_sheet_editor_machine_changed_callback), (gpointer) sheet_editor);

  /* toolbar */
  ags_connectable_connect(AGS_CONNECTABLE(sheet_editor->sheet_toolbar));

  /* machine selector */
  ags_connectable_connect(AGS_CONNECTABLE(sheet_editor->machine_selector));

  /* notation page */
  //  ags_connectable_connect(AGS_CONNECTABLE(sheet_editor->notation_page));
}

void
ags_sheet_editor_disconnect(AgsConnectable *connectable)
{
  AgsSheetEditor *sheet_editor;

  sheet_editor = AGS_SHEET_EDITOR(connectable);

  /* sheet toolbar */
  ags_connectable_disconnect(AGS_CONNECTABLE(sheet_editor->sheet_toolbar)); 

  /* machine selector */
  ags_connectable_disconnect(AGS_CONNECTABLE(sheet_editor->machine_selector));

  /* notation page */
  //  ags_connectable_disconnect(AGS_CONNECTABLE(sheet_editor->notation_page));
}

void
ags_sheet_editor_finalize(GObject *gobject)
{
  AgsSheetEditor *sheet_editor;

  sheet_editor = AGS_SHEET_EDITOR(gobject);

  if(sheet_editor->soundcard != NULL){
    g_object_unref(sheet_editor->soundcard);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_sheet_editor_parent_class)->finalize(gobject);
}

void
ags_sheet_editor_real_machine_changed(AgsSheetEditor *sheet_editor,
				      AgsMachine *machine)
{
  //TODO:JK: implement me
}

/**
 * ags_sheet_editor_machine_changed:
 * @sheet_editor: an #AgsSheetEditor
 * @machine: the new #AgsMachine
 *
 * Is emitted as machine changed of sheet_editor.
 *
 * Since: 3.0.0
 */
void
ags_sheet_editor_machine_changed(AgsSheetEditor *sheet_editor,
				 AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_SHEET_EDITOR(sheet_editor));

  g_object_ref((GObject *) sheet_editor);
  g_signal_emit((GObject *) sheet_editor,
		sheet_editor_signals[MACHINE_CHANGED], 0,
		machine);
  g_object_unref((GObject *) sheet_editor);
}

void
ags_sheet_editor_add_note(AgsSheetEditor *sheet_editor,
			  AgsNote *note)
{
  //TODO:JK: implement me
}

void
ags_sheet_editor_delete_note(AgsSheetEditor *sheet_editor,
			     guint x, guint y)
{
  //TODO:JK: implement me
}

void
ags_sheet_editor_select_region(AgsSheetEditor *sheet_editor,
			       guint x0, guint y0,
			       guint x1, guint y1)
{
  //TODO:JK: implement me
}

void
ags_sheet_editor_do_feedback(AgsSheetEditor *sheet_editor)
{
  //TODO:JK: implement me
}

void
ags_sheet_editor_select_all(AgsSheetEditor *sheet_editor)
{
  //TODO:JK: implement me
}

void
ags_sheet_editor_paste(AgsSheetEditor *sheet_editor)
{
  //TODO:JK: implement me
}

void
ags_sheet_editor_copy(AgsSheetEditor *sheet_editor)
{
  //TODO:JK: implement me
}

void
ags_sheet_editor_cut(AgsSheetEditor *sheet_editor)
{
  //TODO:JK: implement me
}


/**
 * ags_sheet_editor_new:
 *
 * Creates an #AgsSheetEditor
 *
 * Returns: a new #AgsSheetEditor
 *
 * Since: 3.0.0
 */
AgsSheetEditor*
ags_sheet_editor_new()
{
  AgsSheetEditor *sheet_editor;

  sheet_editor = (AgsSheetEditor *) g_object_new(AGS_TYPE_SHEET_EDITOR,
						 NULL);

  return(sheet_editor);
}
