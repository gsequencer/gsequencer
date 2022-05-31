/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/editor/ags_preset_editor.h>
#include <ags/app/editor/ags_preset_editor_callbacks.h>

#include <ags/i18n.h>

void ags_preset_editor_class_init(AgsPresetEditorClass *preset_editor);
void ags_preset_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_preset_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_preset_editor_init(AgsPresetEditor *preset_editor);
void ags_preset_editor_connect(AgsConnectable *connectable);
void ags_preset_editor_disconnect(AgsConnectable *connectable);
void ags_preset_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_preset_editor_apply(AgsApplicable *applicable);
void ags_preset_editor_reset(AgsApplicable *applicable);


/**
 * SECTION:ags_preset_editor
 * @short_description: Edit preset of notes
 * @title: AgsPresetEditor
 * @section_id:
 * @include: ags/app/ags_preset_editor.h
 *
 * #AgsPresetEditor is a composite widget to edit preset controls
 * of selected #AgsNote.
 */

static gpointer ags_preset_editor_parent_class = NULL;

GType
ags_preset_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_preset_editor = 0;

    static const GTypeInfo ags_preset_editor_info = {
      sizeof (AgsPresetEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_preset_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPresetEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_preset_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_preset_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_preset_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_preset_editor = g_type_register_static(GTK_TYPE_BOX,
						    "AgsPresetEditor", &ags_preset_editor_info,
						    0);

    g_type_add_interface_static(ags_type_preset_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_preset_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_preset_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_preset_editor_class_init(AgsPresetEditorClass *preset_editor)
{
  ags_preset_editor_parent_class = g_type_class_peek_parent(preset_editor);
}

void
ags_preset_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_preset_editor_connect;
  connectable->disconnect = ags_preset_editor_disconnect;
}

void
ags_preset_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_preset_editor_set_update;
  applicable->apply = ags_preset_editor_apply;
  applicable->reset = ags_preset_editor_reset;
}

void
ags_preset_editor_init(AgsPresetEditor *preset_editor)
{
  //TODO:JK: implement me
}

void
ags_preset_editor_connect(AgsConnectable *connectable)
{
  AgsPresetEditor *preset_editor;

  preset_editor = AGS_PRESET_EDITOR(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (preset_editor->connectable_flags)) != 0){
    return;
  }

  preset_editor->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  //TODO:JK: implement me
}

void
ags_preset_editor_disconnect(AgsConnectable *connectable)
{
  AgsPresetEditor *preset_editor;

  preset_editor = AGS_PRESET_EDITOR(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (preset_editor->connectable_flags)) == 0){
    return;
  }

  preset_editor->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  //TODO:JK: implement me
}

void
ags_preset_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_preset_editor_apply(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

void
ags_preset_editor_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_preset_editor_new:
 *
 * Create a new instance of #AgsPresetEditor
 *
 * Returns: the new #AgsPresetEditor
 *
 * Since: 4.1.0
 */
AgsPresetEditor*
ags_preset_editor_new()
{
  AgsPresetEditor *preset_editor;

  preset_editor = (AgsPresetEditor *) g_object_new(AGS_TYPE_PRESET_EDITOR,
						   NULL);

  return(preset_editor);
}
