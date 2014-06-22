/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/ags_config.h>

#include <ags-lib/object/ags_connectable.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void ags_config_class_init(AgsConfigClass *config_class);
void ags_config_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_config_init(AgsConfig *config);
void ags_config_add_to_registry(AgsConnectable *connectable);
void ags_config_remove_from_registry(AgsConnectable *connectable);
gboolean ags_config_is_connected(AgsConnectable *connectable);
void ags_config_connect(AgsConnectable *connectable);
void ags_config_disconnect(AgsConnectable *connectable);
void ags_config_finalize(GObject *gobject);
gchar* ags_config_get_version(AgsConfig *config);
void ags_config_set_version(AgsConfig *config, gchar *version);
gchar* ags_config_get_build_id(AgsConfig *config);
void ags_config_set_build_id(AgsConfig *config, gchar *build_id);

static gpointer ags_config_parent_class = NULL;

GType
ags_config_get_type (void)
{
  static GType ags_type_config = 0;

  if(!ags_type_config){
    static const GTypeInfo ags_config_info = {
      sizeof (AgsConfigClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_config_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsConfig),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_config_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_config_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };


    ags_type_config = g_type_register_static(G_TYPE_OBJECT,
					     "AgsConfig\0",
					     &ags_config_info,
					     0);

    g_type_add_interface_static(ags_type_config,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_config);
}

void
ags_config_class_init(AgsConfigClass *config)
{
  GObjectClass *gobject;

  ags_config_parent_class = g_type_class_peek_parent(config);

  /* GObjectClass */
  gobject = (GObjectClass *) config;

  gobject->finalize = ags_config_finalize;
}

void
ags_config_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->add_to_registry = ags_config_add_to_registry;
  connectable->remove_from_registry = ags_config_remove_from_registry;

  connectable->is_ready = NULL;
  connectable->is_connected = ags_config_is_connected;
  connectable->connect = ags_config_connect;
  connectable->disconnect = ags_config_disconnect;
}

void
ags_config_init(AgsConfig *config)
{
  config->flags = 0;

  config->hash = g_hash_table_new(g_str_hash, g_str_equal);

  //TODO:JK: implement me
}

void
ags_config_add_to_registry(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_config_remove_from_registry(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

gboolean
ags_config_is_connected(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_config_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_config_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_config_finalize(GObject *gobject)
{
  //TODO:JK: implement me

  G_OBJECT_CLASS(ags_config_parent_class)->finalize(gobject);
}

gchar*
ags_config_get_version(AgsConfig *config)
{
  //TODO:JK: implement me
}

void
ags_config_set_version(AgsConfig *config, gchar *version)
{
  //TODO:JK: implement me
}

gchar*
ags_config_get_build_id(AgsConfig *config)
{
  //TODO:JK: implement me
}

void
ags_config_set_build_id(AgsConfig *config, gchar *build_id)
{
  //TODO:JK: implement me
}
void
ags_config_load_defaults(AgsConfig *config)
{
  //TODO:JK: implement me
}

void
ags_config_set(AgsConfig *config, gchar *key, GValue value)
{
  //TODO:JK: implement me
}

void
ags_config_get(AgsConfig *config, gchar *key, GValue *value)
{
  //TODO:JK: implement me
}

AgsConfig*
ags_config_new()
{
  AgsConfig *config;

  config = (AgsConfig *) g_object_new(AGS_TYPE_CONFIG,
				      NULL);

  return(config);
}

