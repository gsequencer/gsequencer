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

enum{
  PROP_0,
  PROP_CHANNEL,
};

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
  GObjectClass *gobject;
  GParamSpec *param_spec;

  gobject = (GObjectClass *) output_editor;

  gobject->set_property = ags_output_editor_set_property;
  gobject->get_property = ags_output_editor_get_property;

  /**
   * AgsOutputEditor:channel:
   *
   * The assigned #AgsChannel to edit.
   * 
   * Since: 0.7.131
   */
  param_spec = g_param_spec_object("channel\0",
				   "assigned channel\0",
				   "The channel to modify the output\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);
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
  output_editor->flags = 0;

  output_editor->version = AGS_OUTPUT_EDITOR_DEFAULT_VERSION;
  output_editor->build_id = AGS_OUTPUT_EDITOR_DEFAULT_BUILD_ID;

  /* connecting soundcard */
  output_editor->soundcard = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_pack_start(GTK_BOX(output_editor),
		     GTK_WIDGET(output_editor->soundcard),
		     FALSE, FALSE,
		     0);

  /* connect with line */
  output_editor->spin_button = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_box_pack_start(GTK_BOX(output_editor),
		     GTK_WIDGET(output_editor->spin_button),
		     FALSE, FALSE, 0);
}

void
ags_output_editor_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsOutputEditor *output_editor;

  output_editor = AGS_OUTPUT_EDITOR(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      ags_output_editor_set_channel(output_editor, channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_output_editor_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsOutputEditor *output_editor;

  output_editor = AGS_OUTPUT_EDITOR(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value, output_editor->channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
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

  g_signal_connect(G_OBJECT(output_editor->soundcard), "changed\0",
		   G_CALLBACK(ags_output_editor_soundcard_callback), output_editor);
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

  g_object_disconnect(G_OBJECT(output_editor->soundcard),
		      "changed\0",
		      G_CALLBACK(ags_output_editor_combo_callback),
		      output_editor,
		      NULL);
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
 * ags_output_editor_set_channel:
 * @output_editor: an #AgsOutputEditor
 * @channel: the new #AgsChannel
 *
 * Is called as channel gets modified.
 *
 * Since: 0.7.131
 */
void
ags_output_editor_set_channel(AgsOutputEditor *output_editor,
			      AgsChannel *channel)
{
  //TODO:JK: implement me
}

/**
 * ags_output_editor_new:
 * @channel: the channel to edit
 *
 * Creates an #AgsOutputEditor
 *
 * Returns: a new #AgsOutputEditor
 *
 * Since: 0.7.131
 */
AgsOutputEditor*
ags_output_editor_new(AgsChannel *channel)
{
  AgsOutputEditor *output_editor;

  output_editor = (AgsOutputEditor *) g_object_new(AGS_TYPE_OUTPUT_EDITOR,
						   "channel\0", channel,
						   NULL);

  return(output_editor);
}
