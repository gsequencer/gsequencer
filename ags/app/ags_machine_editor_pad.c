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

#include <ags/app/ags_machine_editor_pad.h>
#include <ags/app/ags_machine_editor_pad_callbacks.h>

#include <ags/i18n.h>

void ags_machine_editor_pad_class_init(AgsMachineEditorPadClass *machine_editor_pad);
void ags_machine_editor_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_editor_pad_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_machine_editor_pad_init(AgsMachineEditorPad *machine_editor_pad);
void ags_machine_editor_pad_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_machine_editor_pad_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);

void ags_machine_editor_pad_connect(AgsConnectable *connectable);
void ags_machine_editor_pad_disconnect(AgsConnectable *connectable);

void ags_machine_editor_pad_set_update(AgsApplicable *applicable, gboolean update);
void ags_machine_editor_pad_apply(AgsApplicable *applicable);
void ags_machine_editor_pad_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_machine_editor_pad
 * @short_description: Edit audio related aspects
 * @title: AgsMachineEditorPad
 * @section_id:
 * @include: ags/app/ags_machine_editor_pad.h
 *
 * #AgsMachineEditorPad is a composite widget to edit all aspects of #AgsAudio.
 * It consists of multiple child editor_pads.
 */

enum{
  PROP_0,
  PROP_CHANNEL,
};

GType
ags_machine_editor_pad_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_machine_editor_pad = 0;

    static const GTypeInfo ags_machine_editor_pad_info = {
      sizeof (AgsMachineEditorPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_editor_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineEditorPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_editor_pad_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_editor_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_machine_editor_pad_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine_editor_pad = g_type_register_static(GTK_TYPE_BOX,
							 "AgsMachineEditorPad", &ags_machine_editor_pad_info,
							 0);

    g_type_add_interface_static(ags_type_machine_editor_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_machine_editor_pad,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_machine_editor_pad);
  }

  return g_define_type_id__volatile;
}

void
ags_machine_editor_pad_class_init(AgsMachineEditorPadClass *machine_editor_pad)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  /* GObjectClass */
  gobject = (GObjectClass *) machine_editor_pad;

  gobject->set_property = ags_machine_editor_pad_set_property;
  gobject->get_property = ags_machine_editor_pad_get_property;

  /* properties */
  /**
   * AgsMachineEditorPad:channel:
   *
   * The channel.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("channel"),
				   i18n_pspec("The assigned channel"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);
}

void
ags_machine_editor_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_editor_pad_connect;
  connectable->disconnect = ags_machine_editor_pad_disconnect;
}

void
ags_machine_editor_pad_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_machine_editor_pad_set_update;
  applicable->apply = ags_machine_editor_pad_apply;
  applicable->reset = ags_machine_editor_pad_reset;
}

void
ags_machine_editor_pad_init(AgsMachineEditorPad *machine_editor_pad)
{
  //TODO:JK: implement me
}

void
ags_machine_editor_pad_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsMachineEditorPad *machine_editor_pad;

  machine_editor_pad = AGS_MACHINE_EDITOR_PAD(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
  {
    AgsChannel *channel;

    channel = g_value_get_object(value);

    if(machine_editor_pad->channel != NULL){
      g_object_unref(machine_editor_pad->channel);
    }

    if(channel != NULL){
      g_object_ref(channel);
    }
    
    machine_editor_pad->channel = channel;
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_editor_pad_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsMachineEditorPad *machine_editor_pad;

  machine_editor_pad = AGS_MACHINE_EDITOR_PAD(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
  {
    g_value_set_object(value, machine_editor_pad->channel);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_editor_pad_connect(AgsConnectable *connectable)
{
  AgsMachineEditorPad *machine_editor_pad;

  machine_editor_pad = AGS_MACHINE_EDITOR_PAD(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (machine_editor_pad->connectable_flags)) != 0){
    return;
  }

  machine_editor_pad->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  //TODO:JK: implement me
}

void
ags_machine_editor_pad_disconnect(AgsConnectable *connectable)
{
  AgsMachineEditorPad *machine_editor_pad;

  machine_editor_pad = AGS_MACHINE_EDITOR_PAD(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (machine_editor_pad->connectable_flags)) == 0){
    return;
  }
  
  machine_editor_pad->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  //TODO:JK: implement me
}

void
ags_machine_editor_pad_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsMachineEditorPad *machine_editor_pad;

  machine_editor_pad = AGS_MACHINE_EDITOR_PAD(applicable);

  //TODO:JK: implement me
}

void
ags_machine_editor_pad_apply(AgsApplicable *applicable)
{
  AgsMachineEditorPad *machine_editor_pad;

  machine_editor_pad = AGS_MACHINE_EDITOR_PAD(applicable);

  //TODO:JK: implement me
}

void
ags_machine_editor_pad_reset(AgsApplicable *applicable)
{
  AgsMachineEditorPad *machine_editor_pad;

  machine_editor_pad = AGS_MACHINE_EDITOR_PAD(applicable);

  //TODO:JK: implement me
}

/**
 * ags_machine_editor_pad_new:
 * @channel_type: the channel type
 *
 * Creates an #AgsMachineEditorPad
 *
 * Returns: a new #AgsMachineEditorPad
 *
 * Since: 4.0.0
 */
AgsMachineEditorPad*
ags_machine_editor_pad_new(AgsChannel *channel)
{
  AgsMachineEditorPad *machine_editor_pad;

  machine_editor_pad = (AgsMachineEditorPad *) g_object_new(AGS_TYPE_MACHINE_EDITOR_PAD,
							    "channel", channel,
							    NULL);

  return(machine_editor_pad);
}
