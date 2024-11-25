/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/ags_midi_cc_editor.h>

void ags_midi_cc_editor_class_init(AgsMidiCCEditorClass *midi_cc_editor);
void ags_midi_cc_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_midi_cc_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_midi_cc_editor_init(AgsMidiCCEditor *midi_cc_editor);

gboolean ags_midi_cc_editor_is_connected(AgsConnectable *connectable);
void ags_midi_cc_editor_connect(AgsConnectable *connectable);
void ags_midi_cc_editor_disconnect(AgsConnectable *connectable);

void ags_midi_cc_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_midi_cc_editor_apply(AgsApplicable *applicable);
void ags_midi_cc_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_midi_cc_editor
 * @short_description: edit MIDI control change.
 * @title: AgsMidiCCEditor
 * @section_id:
 * @include: ags/app/ags_midi_cc_editor.h
 *
 * #AgsMidiCCEditor is a composite widget to assign MIDI control change messages to a
 * specific port.
 */

static gpointer ags_midi_cc_editor_parent_class = NULL;

GType
ags_midi_cc_editor_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_midi_cc_editor = 0;

    static const GTypeInfo ags_midi_cc_editor_info = {
      sizeof (AgsMidiCCEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_cc_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiCCEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_cc_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_midi_cc_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_midi_cc_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_midi_cc_editor = g_type_register_static(GTK_TYPE_BOX,
						     "AgsMidiCCEditor", &ags_midi_cc_editor_info,
						     0);

    g_type_add_interface_static(ags_type_midi_cc_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_midi_cc_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_midi_cc_editor);
  }

  return(g_define_type_id__static);
}

void
ags_midi_cc_editor_class_init(AgsMidiCCEditorClass *midi_cc_editor)
{
  ags_midi_cc_editor_parent_class = g_type_class_peek_parent(midi_cc_editor);
}

void
ags_midi_cc_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_midi_cc_editor_is_connected;  
  connectable->connect = ags_midi_cc_editor_connect;
  connectable->disconnect = ags_midi_cc_editor_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_midi_cc_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_midi_cc_editor_set_update;
  applicable->apply = ags_midi_cc_editor_apply;
  applicable->reset = ags_midi_cc_editor_reset;
}

void
ags_midi_cc_editor_init(AgsMidiCCEditor *midi_cc_editor)
{
  //TODO:JK: implement me
}

gboolean
ags_midi_cc_editor_is_connected(AgsConnectable *connectable)
{
  AgsMidiCCEditor *midi_cc_editor;

  gboolean retval;

  midi_cc_editor = AGS_MIDI_CC_EDITOR(connectable);

  retval = ((AGS_CONNECTABLE_CONNECTED & (midi_cc_editor->connectable_flags)) != 0) ? TRUE: FALSE;

  return(retval);
}

void
ags_midi_cc_editor_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_midi_cc_editor_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_midi_cc_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_midi_cc_editor_apply(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

void
ags_midi_cc_editor_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_midi_cc_editor_new:
 *
 * Create a new instance of #AgsMidiCCEditor
 *
 * Returns: the new #AgsMidiCCEditor
 *
 * Since: 6.9.2
 */
AgsMidiCCEditor*
ags_midi_cc_editor_new()
{
  AgsMidiCCEditor *midi_cc_editor;

  midi_cc_editor = (AgsMidiCCEditor *) g_object_new(AGS_TYPE_MIDI_CC_EDITOR,
						    NULL);

  return(midi_cc_editor);
}
