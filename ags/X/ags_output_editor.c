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

#include <ags/X/ags_output_editor.h>
#include <ags/X/ags_output_editor_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

void ags_output_editor_class_init(AgsOutputEditorClass *output_editor);
void ags_output_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_output_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_output_editor_init(AgsOutputEditor *output_editor);
void ags_output_editor_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_output_editor_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_output_editor_connect(AgsConnectable *connectable);
void ags_output_editor_disconnect(AgsConnectable *connectable);
void ags_output_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_output_editor_apply(AgsApplicable *applicable);
void ags_output_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_output_editor
 * @short_description: A composite widget to edit #AgsChannel
 * @title: AgsOutputEditor
 * @section_id:
 * @include: ags/X/ags_output_editor.h
 *
 * #AgsOutputEditor is a composite widget to edit #AgsChannel.
 */

GType
ags_output_editor_get_type(void)
{
  static GType ags_type_output_editor = 0;

  if(!ags_type_output_editor){
    static const GTypeInfo ags_output_editor_info = {
      sizeof (AgsOutputEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_output_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOutputEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_output_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_output_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_output_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_output_editor = g_type_register_static(GTK_TYPE_VBOX,
						    "AgsOutputEditor\0", &ags_output_editor_info,
						    0);

    g_type_add_interface_static(ags_type_output_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_output_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_output_editor);
}

void
ags_output_editor_class_init(AgsOutputEditorClass *output_editor)
{
}

void
ags_output_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_output_editor_connect;
  connectable->disconnect = ags_output_editor_disconnect;
}

void
ags_output_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_output_editor_set_update;
  applicable->apply = ags_output_editor_apply;
  applicable->reset = ags_output_editor_reset;
}

void
ags_output_editor_init(AgsOutputEditor *output_editor)
{
  output_editor->version = AGS_OUTPUT_EDITOR_DEFAULT_VERSION;
  output_editor->build_id = AGS_OUTPUT_EDITOR_DEFAULT_BUILD_ID;

  //TODO:JK: implement me
}

void
ags_output_editor_connect(AgsConnectable *connectable)
{
  AgsOutputEditor *output_editor;

  output_editor = AGS_OUTPUT_EDITOR(connectable);

  if((AGS_OUTPUT_EDITOR_CONNECTED & (output_editor->flags)) != 0){
    return;
  }

  output_editor->flags |= AGS_OUTPUT_EDITOR_CONNECTED;

  //TODO:JK: implement me
}

void
ags_output_editor_disconnect(AgsConnectable *connectable)
{
  AgsOutputEditor *output_editor;

  output_editor = AGS_OUTPUT_EDITOR(connectable);

  if((AGS_OUTPUT_EDITOR_CONNECTED & (output_editor->flags)) == 0){
    return;
  }

  output_editor->flags &= (~AGS_OUTPUT_EDITOR_CONNECTED);

  //TODO:JK: implement me
}

void
ags_output_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_output_editor_apply(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

void
ags_output_editor_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_output_editor_new:
 *
 * Creates an #AgsOutputEditor
 *
 * Returns: a new #AgsOutputEditor
 *
 * Since: 0.7.131
 */
AgsOutputEditor*
ags_output_editor_new()
{
  AgsOutputEditor *output_editor;

  output_editor = (AgsOutputEditor *) g_object_new(AGS_TYPE_OUTPUT_EDITOR,
						   NULL);

  return(output_editor);
}
