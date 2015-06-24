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

#include <ags/X/editor/ags_machine_radio_button.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/X/editor/ags_machine_selector.h>

void ags_machine_radio_button_class_init(AgsMachineRadioButtonClass *machine_radio_button);
void ags_machine_radio_button_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_radio_button_init(AgsMachineRadioButton *machine_radio_button);
void ags_machine_radio_button_set_property(GObject *gobject,
					   guint prop_id,
					   const GValue *value,
					   GParamSpec *param_spec);
void ags_machine_radio_button_get_property(GObject *gobject,
					   guint prop_id,
					   GValue *value,
					   GParamSpec *param_spec);
void ags_machine_radio_button_connect(AgsConnectable *connectable);
void ags_machine_radio_button_disconnect(AgsConnectable *connectable);
void ags_machine_radio_button_finalize(GObject *gobject);

/**
 * SECTION:ags_machine_radio_button
 * @short_description: machine radio buttons
 * @title: AgsMachineRadioButton
 * @section_id:
 * @include: ags/X/editor/ags_machine_radio_button.h
 *
 * The #AgsMachineRadioButton enables you make choice of an #AgsMachine.
 */

enum{
  PROP_0,
  PROP_MACHINE,
};

static gpointer ags_machine_radio_button_parent_class = NULL;

GType
ags_machine_radio_button_get_type(void)
{
  static GType ags_type_machine_radio_button = 0;

  if(!ags_type_machine_radio_button){
    static const GTypeInfo ags_machine_radio_button_info = {
      sizeof (AgsMachineRadioButtonClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_radio_button_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineRadioButton),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_radio_button_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_radio_button_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine_radio_button = g_type_register_static(GTK_TYPE_RADIO_BUTTON,
							   "AgsMachineRadioButton\0", &ags_machine_radio_button_info,
							   0);
    
    g_type_add_interface_static(ags_type_machine_radio_button,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_machine_radio_button);
}

void
ags_machine_radio_button_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_radio_button_connect;
  connectable->disconnect = ags_machine_radio_button_disconnect;
}

void
ags_machine_radio_button_class_init(AgsMachineRadioButtonClass *machine_radio_button)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_machine_radio_button_parent_class = g_type_class_peek_parent(machine_radio_button);

  /* GObjectClass */
  gobject = (GObjectClass *) machine_radio_button;
  
  gobject->set_property = ags_machine_radio_button_set_property;
  gobject->get_property = ags_machine_radio_button_get_property;

  gobject->finalize = ags_machine_radio_button_finalize;

  /* properties */
  param_spec = g_param_spec_object("machine\0",
				   "assigned machine\0",
				   "The machine it is assigned to\0",
				   AGS_TYPE_MACHINE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MACHINE,
				  param_spec);
}

void
ags_machine_radio_button_init(AgsMachineRadioButton *machine_radio_button)
{
  machine_radio_button->machine = NULL;
}

void
ags_machine_radio_button_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec)
{
  AgsMachineRadioButton *machine_radio_button;

  machine_radio_button = AGS_MACHINE_RADIO_BUTTON(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      AgsMachine *machine;
      AgsMachineSelector *machine_selector;

      machine = (AgsMachine *) g_value_get_object(value);
      
      if(machine == machine_radio_button->machine){
	return;
      }

      if(machine_radio_button->machine != NULL){
	g_object_unref(machine_radio_button->machine);
      }

      if(machine != NULL){
	g_object_set(gobject,
		     "label\0", g_strdup_printf("%s: %s\0", G_OBJECT_TYPE_NAME(machine), machine->name),
		     NULL);

	g_object_ref(machine);
      }

      machine_radio_button->machine = machine;
      
      machine_selector = (AgsMachineSelector *) gtk_widget_get_ancestor((GtkWidget *) machine_radio_button,
									AGS_TYPE_MACHINE_SELECTOR);
      ags_machine_selector_changed(machine_selector,
				   machine);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_radio_button_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec)
{
  AgsMachineRadioButton *machine_radio_button;

  machine_radio_button = AGS_MACHINE_RADIO_BUTTON(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    g_value_set_object(value, machine_radio_button->machine);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_radio_button_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_machine_radio_button_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_machine_radio_button_finalize(GObject *gobject)
{
  AgsMachineRadioButton *machine_radio_button;

  machine_radio_button = AGS_MACHINE_RADIO_BUTTON(gobject);

  if(machine_radio_button->machine != NULL){
    g_object_unref(G_OBJECT(machine_radio_button->machine));
  }
}

/**
 * ags_machine_radio_button_new:
 *
 * Create a new #AgsMachineRadioButton.
 *
 * Since: 0.4
 */
AgsMachineRadioButton*
ags_machine_radio_button_new()
{
  AgsMachineRadioButton *machine_radio_button;

  machine_radio_button = (AgsMachineRadioButton *) g_object_new(AGS_TYPE_MACHINE_RADIO_BUTTON,
								NULL);

  return(machine_radio_button);
}
