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

#include <ags/app/ags_machine_editor_line.h>
#include <ags/app/ags_machine_editor_line_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_machine_editor.h>

#include <ags/i18n.h>

void ags_machine_editor_line_class_init(AgsMachineEditorLineClass *machine_editor_line);
void ags_machine_editor_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_editor_line_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_machine_editor_line_init(AgsMachineEditorLine *machine_editor_line);
void ags_machine_editor_line_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_machine_editor_line_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);

void ags_machine_editor_line_connect(AgsConnectable *connectable);
void ags_machine_editor_line_disconnect(AgsConnectable *connectable);

void ags_machine_editor_line_set_update(AgsApplicable *applicable, gboolean update);
void ags_machine_editor_line_apply(AgsApplicable *applicable);
void ags_machine_editor_line_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_machine_editor_line
 * @short_description: Edit audio related aspects
 * @title: AgsMachineEditorLine
 * @section_id:
 * @include: ags/app/ags_machine_editor_line.h
 *
 * #AgsMachineEditorLine is a composite widget to edit all aspects of #AgsAudio.
 * It consists of multiple child editor_lines.
 */

enum{
  PROP_0,
  PROP_CHANNEL,
};

static gpointer ags_machine_editor_line_parent_class = NULL;

GType
ags_machine_editor_line_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_machine_editor_line = 0;

    static const GTypeInfo ags_machine_editor_line_info = {
      sizeof (AgsMachineEditorLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_editor_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineEditorLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_editor_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_editor_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_machine_editor_line_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine_editor_line = g_type_register_static(GTK_TYPE_BOX,
							  "AgsMachineEditorLine", &ags_machine_editor_line_info,
							  0);

    g_type_add_interface_static(ags_type_machine_editor_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_machine_editor_line,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_machine_editor_line);
  }

  return g_define_type_id__volatile;
}

void
ags_machine_editor_line_class_init(AgsMachineEditorLineClass *machine_editor_line)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_machine_editor_line_parent_class = g_type_class_peek_parent(machine_editor_line);

  /* GObjectClass */
  gobject = (GObjectClass *) machine_editor_line;

  gobject->set_property = ags_machine_editor_line_set_property;
  gobject->get_property = ags_machine_editor_line_get_property;

  /* properties */
  /**
   * AgsMachineEditorLine:channel:
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
ags_machine_editor_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_editor_line_connect;
  connectable->disconnect = ags_machine_editor_line_disconnect;
}

void
ags_machine_editor_line_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_machine_editor_line_set_update;
  applicable->apply = ags_machine_editor_line_apply;
  applicable->reset = ags_machine_editor_line_reset;
}

void
ags_machine_editor_line_init(AgsMachineEditorLine *machine_editor_line)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(machine_editor_line),
				 GTK_ORIENTATION_VERTICAL);

  gtk_box_set_spacing(GTK_BOX(machine_editor_line),
		      AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  machine_editor_line->connectable_flags = 0;

  machine_editor_line->parent_machine_editor_pad = NULL;

  machine_editor_line->channel = NULL;

  machine_editor_line->link_editor = ags_link_editor_new();
  gtk_box_append((GtkBox *) machine_editor_line,
		 (GtkWidget *) machine_editor_line->link_editor);

  machine_editor_line->line_member_editor = ags_line_member_editor_new();
  gtk_box_append((GtkBox *) machine_editor_line,
		 (GtkWidget *) machine_editor_line->line_member_editor);
}

void
ags_machine_editor_line_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsMachineEditorLine *machine_editor_line;

  machine_editor_line = AGS_MACHINE_EDITOR_LINE(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
  {
    AgsChannel *channel;

    channel = g_value_get_object(value);

    if(machine_editor_line->channel != NULL){
      g_object_unref(machine_editor_line->channel);
    }

    if(channel != NULL){
      g_object_ref(channel);
    }
    
    machine_editor_line->channel = channel;
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_editor_line_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec)
{
  AgsMachineEditorLine *machine_editor_line;

  machine_editor_line = AGS_MACHINE_EDITOR_LINE(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
  {
    g_value_set_object(value, machine_editor_line->channel);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_editor_line_connect(AgsConnectable *connectable)
{
  AgsMachineEditorLine *machine_editor_line;

  machine_editor_line = AGS_MACHINE_EDITOR_LINE(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (machine_editor_line->connectable_flags)) != 0){
    return;
  }

  machine_editor_line->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  ags_connectable_connect(AGS_CONNECTABLE(machine_editor_line->link_editor));
  ags_connectable_connect(AGS_CONNECTABLE(machine_editor_line->line_member_editor));
}

void
ags_machine_editor_line_disconnect(AgsConnectable *connectable)
{
  AgsMachineEditorLine *machine_editor_line;

  machine_editor_line = AGS_MACHINE_EDITOR_LINE(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (machine_editor_line->connectable_flags)) == 0){
    return;
  }
  
  machine_editor_line->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  ags_connectable_disconnect(AGS_CONNECTABLE(machine_editor_line->link_editor));
  ags_connectable_disconnect(AGS_CONNECTABLE(machine_editor_line->line_member_editor));
}

void
ags_machine_editor_line_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsMachineEditorLine *machine_editor_line;

  machine_editor_line = AGS_MACHINE_EDITOR_LINE(applicable);

  ags_applicable_set_update(AGS_APPLICABLE(machine_editor_line->link_editor),
			    update);
  ags_applicable_set_update(AGS_APPLICABLE(machine_editor_line->line_member_editor),
			    update);
}

void
ags_machine_editor_line_apply(AgsApplicable *applicable)
{
  AgsMachineEditorLine *machine_editor_line;

  machine_editor_line = AGS_MACHINE_EDITOR_LINE(applicable);

  ags_applicable_apply(AGS_APPLICABLE(machine_editor_line->link_editor));
  ags_applicable_apply(AGS_APPLICABLE(machine_editor_line->line_member_editor));
}

void
ags_machine_editor_line_reset(AgsApplicable *applicable)
{
  AgsMachineEditorLine *machine_editor_line;

  machine_editor_line = AGS_MACHINE_EDITOR_LINE(applicable);
  
  ags_applicable_reset(AGS_APPLICABLE(machine_editor_line->link_editor));
  ags_applicable_reset(AGS_APPLICABLE(machine_editor_line->line_member_editor));
}

/**
 * ags_machine_editor_line_new:
 * @channel_type: the channel type
 *
 * Creates an #AgsMachineEditorLine
 *
 * Returns: a new #AgsMachineEditorLine
 *
 * Since: 4.0.0
 */
AgsMachineEditorLine*
ags_machine_editor_line_new(AgsChannel *channel)
{
  AgsMachineEditorLine *machine_editor_line;

  machine_editor_line = (AgsMachineEditorLine *) g_object_new(AGS_TYPE_MACHINE_EDITOR_LINE,
							      "channel", channel,
							      NULL);

  return(machine_editor_line);
}
