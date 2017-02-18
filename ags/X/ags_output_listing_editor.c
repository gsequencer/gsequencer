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

#include <ags/X/ags_output_listing_editor.h>
#include <ags/X/ags_output_listing_editor_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>

#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_pad_editor.h>

void ags_output_listing_editor_class_init(AgsOutputListingEditorClass *output_listing_editor);
void ags_output_listing_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_output_listing_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_output_listing_editor_init(AgsOutputListingEditor *output_listing_editor);
void ags_output_listing_editor_connect(AgsConnectable *connectable);
void ags_output_listing_editor_disconnect(AgsConnectable *connectable);
void ags_output_listing_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_output_listing_editor_apply(AgsApplicable *applicable);
void ags_output_listing_editor_reset(AgsApplicable *applicable);
void ags_output_listing_editor_destroy(GtkObject *object);
void ags_output_listing_editor_show(GtkWidget *widget);

/**
 * SECTION:ags_output_listing_editor
 * @short_description: pack output editors
 * @title: AgsOutputListingEditor
 * @section_id:
 * @include: ags/X/ags_output_listing_editor.h
 *
 * #AgsOutputListingEditor is a composite widget to pack #AgsOutputEditor.
 */

static gpointer ags_output_listing_editor_parent_class = NULL;
static AgsConnectableInterface* ags_output_listing_editor_parent_connectable_interface;

GType
ags_output_listing_editor_get_type(void)
{
  static GType ags_type_output_listing_editor = 0;

  if(!ags_type_output_listing_editor){
    static const GTypeInfo ags_output_listing_editor_info = {
      sizeof (AgsOutputListingEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_output_listing_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOutputListingEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_output_listing_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_output_listing_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_output_listing_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_output_listing_editor = g_type_register_static(AGS_TYPE_PROPERTY_OUTPUT_LISTING_EDITOR,
							    "AgsOutputListingEditor\0",
							    &ags_output_listing_editor_info,
							    0);
    
    g_type_add_interface_static(ags_type_output_listing_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_output_listing_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return(ags_type_output_listing_editor);
}

void
ags_output_listing_editor_class_init(AgsOutputListingEditorClass *output_listing_editor)
{
}

void
ags_output_listing_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_output_listing_editor_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_output_listing_editor_connect;
  connectable->disconnect = ags_output_listing_editor_disconnect;
}

void
ags_output_listing_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_output_listing_editor_set_update;
  applicable->apply = ags_output_listing_editor_apply;
  applicable->reset = ags_output_listing_editor_reset;
}

void
ags_output_listing_editor_init(AgsOutputListingEditor *output_listing_editor)
{
  //TODO:JK: implement me
}

void
ags_output_listing_editor_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_output_listing_editor_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_output_listing_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_output_listing_editor_apply(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

void
ags_output_listing_editor_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_output_listing_editor_add_children:
 * @output_listing_editor: the #AgsOutputListingEditor
 * @audio: the #AgsAudio to use
 * @nth_channel: nth channel to start creation until end
 * @connect: if %TRUE widget is connected and shown
 *
 * Creates new pad editors or destroys them.
 *
 * Since: 0.7.131
 */
void
ags_output_listing_editor_add_children(AgsOutputListingEditor *output_listing_editor,
				       AgsAudio *audio, guint nth_channel,
				       gboolean connect)
{
  //TODO:JK: implement me
}

/**
 * ags_output_listing_editor_new:
 * @channel_type: the channel type to represent
 *
 * Creates an #AgsOutputListingEditor
 *
 * Returns: a new #AgsOutputListingEditor
 *
 * Since: 0.7.131
 */
AgsOutputListingEditor*
ags_output_listing_editor_new(GType channel_type)
{
  AgsOutputListingEditor *output_listing_editor;

  output_listing_editor = (AgsOutputListingEditor *) g_object_new(AGS_TYPE_OUTPUT_LISTING_EDITOR,
								  NULL);
  
  output_listing_editor->channel_type = channel_type;

  return(output_listing_editor);
}
