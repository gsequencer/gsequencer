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

#include <ags/app/editor/ags_machine_radio_button.h>
#include <ags/app/editor/ags_machine_radio_button_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/app/editor/ags_machine_selector.h>

#include <ags/i18n.h>

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
void ags_machine_radio_button_finalize(GObject *gobject);

gboolean ags_machine_radio_button_is_connected(AgsConnectable *connectable);
void ags_machine_radio_button_connect(AgsConnectable *connectable);
void ags_machine_radio_button_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_machine_radio_button
 * @short_description: machine radio buttons
 * @title: AgsMachineRadioButton
 * @section_id:
 * @include: ags/app/editor/ags_machine_radio_button.h
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
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_machine_radio_button = 0;

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

    ags_type_machine_radio_button = g_type_register_static(GTK_TYPE_CHECK_BUTTON,
							   "AgsMachineRadioButton", &ags_machine_radio_button_info,
							   0);
    
    g_type_add_interface_static(ags_type_machine_radio_button,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_machine_radio_button);
  }

  return g_define_type_id__volatile;
}

void
ags_machine_radio_button_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;
  
  connectable->is_connected = ags_machine_radio_button_is_connected;
  connectable->connect = ags_machine_radio_button_connect;
  connectable->disconnect = ags_machine_radio_button_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
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
  /**
   * AgsMachineRadioButton:machine:
   *
   * The assigned #AgsMachine
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("machine",
				   i18n_pspec("assigned machine"),
				   i18n_pspec("The machine it is assigned to"),
				   AGS_TYPE_MACHINE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MACHINE,
				  param_spec);
}

void
ags_machine_radio_button_init(AgsMachineRadioButton *machine_radio_button)
{
  machine_radio_button->connectable_flags = 0;
  
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
	gchar *str;

	str = g_strdup_printf("%s: %s", G_OBJECT_TYPE_NAME(machine), machine->machine_name);	
	g_object_set(gobject,
		     "label", str,
		     NULL);

	g_signal_connect_after(machine, "notify::machine-name",
			       G_CALLBACK(ags_machine_radio_button_notify_machine_name_callback), machine_radio_button);
	g_object_ref(machine);

	g_free(str);
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
    {
      g_value_set_object(value, machine_radio_button->machine);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

gboolean
ags_machine_radio_button_is_connected(AgsConnectable *connectable)
{
  AgsMachineRadioButton *machine_radio_button;
  
  gboolean is_connected;

  machine_radio_button = AGS_MACHINE_RADIO_BUTTON(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (machine_radio_button->connectable_flags)) != 0) ? TRUE: FALSE;
  
  return(is_connected);
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
 * Returns: a new #AgsMachineRadioButton
 *
 * Since: 3.0.0
 */
AgsMachineRadioButton*
ags_machine_radio_button_new()
{
  AgsMachineRadioButton *machine_radio_button;

  machine_radio_button = (AgsMachineRadioButton *) g_object_new(AGS_TYPE_MACHINE_RADIO_BUTTON,
								NULL);

  return(machine_radio_button);
}
