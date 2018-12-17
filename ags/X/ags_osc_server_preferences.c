/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/X/ags_osc_server_preferences.h>
#include <ags/X/ags_osc_server_preferences_callbacks.h>

#include <ags/libags.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

#include <ags/i18n.h>

void ags_osc_server_preferences_class_init(AgsOscServerPreferencesClass *osc_server_preferences);
void ags_osc_server_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_osc_server_preferences_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_osc_server_preferences_init(AgsOscServerPreferences *osc_server_preferences);

void ags_osc_server_preferences_connect(AgsConnectable *connectable);
void ags_osc_server_preferences_disconnect(AgsConnectable *connectable);

void ags_osc_server_preferences_set_update(AgsApplicable *applicable, gboolean update);
void ags_osc_server_preferences_apply(AgsApplicable *applicable);
void ags_osc_server_preferences_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_osc_server_preferences
 * @short_description: A composite widget to do osc_server related preferences
 * @title: AgsOscServerPreferences
 * @section_id: 
 * @include: ags/X/ags_osc_server_preferences.h
 *
 * #AgsOscServerPreferences enables you to make osc_server related preferences.
 */

static gpointer ags_osc_server_preferences_parent_class = NULL;

GType
ags_osc_server_preferences_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_server_preferences = 0;

    static const GTypeInfo ags_osc_server_preferences_info = {
      sizeof (AgsOscServerPreferencesClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_server_preferences_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscServerPreferences),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_server_preferences_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_osc_server_preferences_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_osc_server_preferences_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_osc_server_preferences = g_type_register_static(GTK_TYPE_VBOX,
							     "AgsOscServerPreferences", &ags_osc_server_preferences_info,
							     0);
    
    g_type_add_interface_static(ags_type_osc_server_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_osc_server_preferences,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_server_preferences);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_server_preferences_class_init(AgsOscServerPreferencesClass *osc_server_preferences)
{
  ags_osc_server_preferences_parent_class = g_type_class_peek_parent(osc_server_preferences);
}

void
ags_osc_server_preferences_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_osc_server_preferences_connect;
  connectable->disconnect = ags_osc_server_preferences_disconnect;
}

void
ags_osc_server_preferences_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_osc_server_preferences_set_update;
  applicable->apply = ags_osc_server_preferences_apply;
  applicable->reset = ags_osc_server_preferences_reset;
}

void
ags_osc_server_preferences_init(AgsOscServerPreferences *osc_server_preferences)
{
  GtkTable *table;
  GtkLabel *label;

  table = (GtkTable *) gtk_table_new(2, 8,
				     FALSE);
  gtk_box_pack_start(GTK_BOX(osc_server_preferences),
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     2);

  osc_server_preferences->auto_start = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("auto-start OSC server"));
  gtk_table_attach(table,
		   GTK_WIDGET(osc_server_preferences->auto_start),
		   0, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  osc_server_preferences->start = (GtkButton *) gtk_button_new_with_label(i18n("start OSC server"));
  gtk_table_attach(table,
		   GTK_WIDGET(osc_server_preferences->start),
		   0, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* listen any address */
  osc_server_preferences->any_address = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("listen on any address"));
  gtk_table_attach(table,
		   GTK_WIDGET(osc_server_preferences->any_address),
		   0, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* enable IP4 address */
  osc_server_preferences->enable_ip4 = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enable IPv4"));
  gtk_table_attach(table,
		   GTK_WIDGET(osc_server_preferences->enable_ip4),
		   0, 2,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* IP4 address */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("IPv4 address"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   4, 5,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  osc_server_preferences->ip4_address = (GtkEntry *) gtk_entry_new();
  gtk_table_attach(table,
		   GTK_WIDGET(osc_server_preferences->ip4_address),
		   1, 2,
		   4, 5,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* enable IP6 address */
  osc_server_preferences->enable_ip6 = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enable IPv6"));
  gtk_table_attach(table,
		   GTK_WIDGET(osc_server_preferences->enable_ip6),
		   0, 2,
		   5, 6,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* IP6 address */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("IPv6 address"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   6, 7,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  osc_server_preferences->ip6_address = (GtkEntry *) gtk_entry_new();
  gtk_table_attach(table,
		   GTK_WIDGET(osc_server_preferences->ip6_address),
		   1, 2,
		   6, 7,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  /* port */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("port"),
				    "xalign", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   7, 8,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  osc_server_preferences->port = (GtkEntry *) gtk_entry_new();
  gtk_table_attach(table,
		   GTK_WIDGET(osc_server_preferences->port),
		   1, 2,
		   7, 8,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_osc_server_preferences_connect(AgsConnectable *connectable)
{
  AgsOscServerPreferences *osc_server_preferences;

  osc_server_preferences = AGS_OSC_SERVER_PREFERENCES(connectable);
}

void
ags_osc_server_preferences_disconnect(AgsConnectable *connectable)
{
  /* empty */
}


void
ags_osc_server_preferences_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_osc_server_preferences_apply(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

void
ags_osc_server_preferences_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_osc_server_preferences_new:
 *
 * Create a new instance of #AgsOscServerPreferences
 *
 * Returns: the new #AgsOscServerPreferences
 *
 * Since: 2.0.0
 */
AgsOscServerPreferences*
ags_osc_server_preferences_new()
{
  AgsOscServerPreferences *osc_server_preferences;

  osc_server_preferences = (AgsOscServerPreferences *) g_object_new(AGS_TYPE_OSC_SERVER_PREFERENCES,
								    NULL);
  
  return(osc_server_preferences);
}
