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

#include <ags/plugin/ags_lv2_uri_map_manager.h>

#include <ags/object/ags_connectable.h>

void ags_lv2_uri_map_manager_class_init(AgsLv2UriMapManagerClass *lv2_uri_map_manager);
void ags_lv2_uri_map_manager_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_lv2_uri_map_manager_init(AgsLv2UriMapManager *lv2_uri_map_manager);
void ags_lv2_uri_map_manager_connect(AgsConnectable *connectable);
void ags_lv2_uri_map_manager_disconnect(AgsConnectable *connectable);
void ags_lv2_uri_map_manager_finalize(GObject *gobject);

void ags_lv2_uri_map_manager_destroy_data(gpointer data);

/**
 * SECTION:ags_lv2_uri_map_manager
 * @short_description: uri map
 * @title: AgsLv2UriMapManager
 * @section_id:
 * @include: ags/thread/ags_lv2_uri_map_manager.h
 *
 * The #AgsLv2UriMapManager keeps your uri in a hash table where you can lookup your
 * ids.
 */

static gpointer ags_lv2_uri_map_manager_parent_class = NULL;

AgsLv2UriMapManager *ags_lv2_uri_map_manager = NULL;

GType
ags_lv2_uri_map_manager_get_type()
{
  static GType ags_type_lv2_uri_map_manager = 0;

  if(!ags_type_lv2_uri_map_manager){
    const GTypeInfo ags_lv2_uri_map_manager_info = {
      sizeof (AgsLv2UriMapManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_uri_map_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2UriMapManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_uri_map_manager_init,
    };

    const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_lv2_uri_map_manager_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_lv2_uri_map_manager = g_type_register_static(G_TYPE_OBJECT,
							  "AgsLv2UriMapManager\0",
							  &ags_lv2_uri_map_manager_info,
							  0);

    g_type_add_interface_static(ags_type_lv2_uri_map_manager,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_lv2_uri_map_manager);
}

void
ags_lv2_uri_map_manager_class_init(AgsLv2UriMapManagerClass *lv2_uri_map_manager)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_lv2_uri_map_manager_parent_class = g_type_class_peek_parent(lv2_uri_map_manager);

  /* GObject */
  gobject = (GObjectClass *) lv2_uri_map_manager;

  gobject->finalize = ags_lv2_uri_map_manager_finalize;
}

void
ags_lv2_uri_map_manager_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_lv2_uri_map_manager_connect;
  connectable->disconnect = ags_lv2_uri_map_manager_disconnect;
}

void
ags_lv2_uri_map_manager_init(AgsLv2UriMapManager *lv2_uri_map_manager)
{
  lv2_uri_map_manager->id_counter = 1;

  lv2_uri_map_manager->uri_map = g_hash_table_new_full(g_str_hash, g_str_equal,
						       NULL,
						       (GDestroyNotify) ags_lv2_uri_map_manager_destroy_data);
}

void
ags_lv2_uri_map_manager_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_lv2_uri_map_manager_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_lv2_uri_map_manager_finalize(GObject *gobject)
{
  AgsLv2UriMapManager *lv2_uri_map_manager;

  lv2_uri_map_manager = AGS_LV2_URI_MAP_MANAGER(gobject);

  g_hash_table_destroy(lv2_uri_map_manager->uri_map);
}

void
ags_lv2_uri_map_manager_destroy_data(gpointer data)
{
  /* empty */
}

/**
 * ags_lv2_uri_map_manager_insert:
 * @lv2_uri_map_manager: the #AgsLv2UriMapManager
 * @uri: the object as key
 * @id: the id to insert
 *
 * Inserts a id into hash map associated with @uri.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 0.7.7
 */
gboolean
ags_lv2_uri_map_manager_insert(AgsLv2UriMapManager *lv2_uri_map_manager,
			       gchar *uri, GValue *id)
{
  return(g_hash_table_insert(lv2_uri_map_manager->uri_map,
			     uri, id));
}

/**
 * ags_lv2_uri_map_manager_remove:
 * @lv2_uri_map_manager: the #AgsLv2UriMapManager
 * @uri: the object to remove
 * 
 * Removes a lock associated with @uri.3
 *
 * Returns: %TRUE as successfully removed, otherwise %FALSE
 *
 * Since: 0.7.7
 */
gboolean
ags_lv2_uri_map_manager_remove(AgsLv2UriMapManager *lv2_uri_map_manager,
			       gchar *uri)
{
  GValue *id;

  id = g_hash_table_lookup(lv2_uri_map_manager->uri_map,
			   uri);

  if(id != NULL){
    g_hash_table_remove(lv2_uri_map_manager->uri_map,
			uri);
  }
  
  return(TRUE);
}

/**
 * ags_lv2_uri_map_manager_lookup:
 * @lv2_uri_map_manager: the #AgsLv2UriMapManager
 * @uri: the object to lookup
 *
 * Lookup a id associated with @uri in @lv2_uri_map_manager
 *
 * Returns: the id on success, else G_MAXUINT32
 *
 * Since: 0.7.7
 */
GValue*
ags_lv2_uri_map_manager_lookup(AgsLv2UriMapManager *lv2_uri_map_manager,
			       gchar *uri)
{
  GValue *value;

  value = (GValue *) g_hash_table_lookup(lv2_uri_map_manager->uri_map,
					 uri);

  if(value == NULL){
    g_message("new uri %s\0", uri);
    
    value = g_new0(GValue,
		   1);
    g_value_init(value,
		 G_TYPE_ULONG);

    g_value_set_ulong(value,
		      lv2_uri_map_manager->id_counter);
    ags_lv2_uri_map_manager_insert(lv2_uri_map_manager,
				   uri,
				   value);

    lv2_uri_map_manager->id_counter++;
  }
  
  return(value);
}

void
ags_lv2_uri_map_manager_load_default(AgsLv2UriMapManager *lv2_uri_map_manager)
{
  GValue *value;
  gchar **str;
  
  static const gchar **default_uri = {
    NULL,
  };

  str = default_uri;

  while(str[0] != NULL){
    value = g_new0(GValue,
		   1);
    g_value_init(value,
		 G_TYPE_ULONG);

    g_value_set_ulong(value,
		      lv2_uri_map_manager->id_counter);
    ags_lv2_uri_map_manager_insert(lv2_uri_map_manager,
				   *str,
				   value);

    lv2_uri_map_manager->id_counter++;
    str++;
  }
}

uint32_t
ags_lv2_uri_map_manager_uri_to_id(LV2_URI_Map_Callback_Data callback_data,
				  char *map,
				  char *uri)
{
  GValue *value;
  uint32_t id;

  if(map != NULL){
    uri = g_strdup_printf("%s<%s>\0",
			  map,
			  uri);
  }
  
  value = ags_lv2_uri_map_manager_lookup(ags_lv2_uri_map_manager_get_instance(),
					 uri);
  id = g_value_get_ulong(value);
  
  return(id);
}

/**
 * ags_lv2_uri_map_manager_get_instance:
 * 
 * Singleton function to optain the id manager instance.
 *
 * Returns: an instance of #AgsLv2UriMapManager
 *
 * Since: 0.7.7
 */
AgsLv2UriMapManager*
ags_lv2_uri_map_manager_get_instance()
{
  if(ags_lv2_uri_map_manager == NULL){
    ags_lv2_uri_map_manager = ags_lv2_uri_map_manager_new();
    
    //    ags_lv2_uri_map_manager_load_default(ags_lv2_uri_map_manager);
  }

  return(ags_lv2_uri_map_manager);
}

/**
 * ags_lv2_uri_map_manager_new:
 *
 * Instantiate a id manager. 
 *
 * Returns: a new #AgsLv2UriMapManager
 *
 * Since: 0.7.7
 */
AgsLv2UriMapManager*
ags_lv2_uri_map_manager_new()
{
  AgsLv2UriMapManager *lv2_uri_map_manager;

  lv2_uri_map_manager = (AgsLv2UriMapManager *) g_object_new(AGS_TYPE_LV2_URI_MAP_MANAGER,
							     NULL);

  return(lv2_uri_map_manager);
}
