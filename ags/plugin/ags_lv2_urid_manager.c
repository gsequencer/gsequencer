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

#include <ags/plugin/ags_lv2_urid_manager.h>

#include <ags/object/ags_connectable.h>

void ags_lv2_urid_manager_class_init(AgsLv2UridManagerClass *lv2_urid_manager);
void ags_lv2_urid_manager_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_lv2_urid_manager_init(AgsLv2UridManager *lv2_urid_manager);
void ags_lv2_urid_manager_connect(AgsConnectable *connectable);
void ags_lv2_urid_manager_disconnect(AgsConnectable *connectable);
void ags_lv2_urid_manager_finalize(GObject *gobject);

void ags_lv2_urid_manager_destroy_data(gpointer data);
gboolean ags_lv2_urid_manager_finder(gpointer key, gpointer value, gpointer user_data);

/**
 * SECTION:ags_lv2_urid_manager
 * @short_description: urid 
 * @title: AgsLv2UridManager
 * @section_id:
 * @include: ags/plugin/ags_lv2_urid_manager.h
 *
 * The #AgsLv2UridManager keeps your urid in a hash table where you can lookup your
 * ids.
 */

static gpointer ags_lv2_urid_manager_parent_class = NULL;

AgsLv2UridManager *ags_lv2_urid_manager = NULL;

GType
ags_lv2_urid_manager_get_type()
{
  static GType ags_type_lv2_urid_manager = 0;

  if(!ags_type_lv2_urid_manager){
    const GTypeInfo ags_lv2_urid_manager_info = {
      sizeof (AgsLv2UridManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_urid_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2UridManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_urid_manager_init,
    };

    const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_lv2_urid_manager_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_lv2_urid_manager = g_type_register_static(G_TYPE_OBJECT,
						       "AgsLv2UridManager",
						       &ags_lv2_urid_manager_info,
						       0);

    g_type_add_interface_static(ags_type_lv2_urid_manager,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_lv2_urid_manager);
}

void
ags_lv2_urid_manager_class_init(AgsLv2UridManagerClass *lv2_urid_manager)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_lv2_urid_manager_parent_class = g_type_class_peek_parent(lv2_urid_manager);

  /* GObject */
  gobject = (GObjectClass *) lv2_urid_manager;

  gobject->finalize = ags_lv2_urid_manager_finalize;
}

void
ags_lv2_urid_manager_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_lv2_urid_manager_connect;
  connectable->disconnect = ags_lv2_urid_manager_disconnect;
}

void
ags_lv2_urid_manager_init(AgsLv2UridManager *lv2_urid_manager)
{
  lv2_urid_manager->id_counter = 1;

  lv2_urid_manager->urid = g_hash_table_new_full(g_str_hash, g_str_equal,
						 NULL,
						 (GDestroyNotify) ags_lv2_urid_manager_destroy_data);
}

void
ags_lv2_urid_manager_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_lv2_urid_manager_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_lv2_urid_manager_finalize(GObject *gobject)
{
  AgsLv2UridManager *lv2_urid_manager;

  lv2_urid_manager = AGS_LV2_URID_MANAGER(gobject);

  g_hash_table_destroy(lv2_urid_manager->urid);

  if(lv2_urid_manager == ags_lv2_urid_manager){
    ags_lv2_urid_manager = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_lv2_urid_manager_parent_class)->finalize(gobject);
}

void
ags_lv2_urid_manager_destroy_data(gpointer data)
{
  /* empty */
}

gboolean
ags_lv2_urid_manager_finder(gpointer key, gpointer value, gpointer user_data)
{
  if((uint32_t) g_value_get_uint((GValue *) value) == (uint32_t) g_value_get_uint((GValue *) user_data)){
    return(TRUE);
  }

  return(FALSE);
}

/**
 * ags_lv2_urid_manager_insert:
 * @lv2_urid_manager: the #AgsLv2UridManager
 * @uri: the object as key
 * @id: the id to insert
 *
 * Inserts a id into hash  associated with @urid.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 0.7.9
 */
gboolean
ags_lv2_urid_manager_insert(AgsLv2UridManager *lv2_urid_manager,
			    gchar *uri, GValue *id)
{
  if(lv2_urid_manager == NULL ||
     uri == NULL ||
     id == NULL){
    return(FALSE);
  }

  g_hash_table_insert(lv2_urid_manager->urid,
		      uri, id);

  return(TRUE);
}

/**
 * ags_lv2_urid_manager_remove:
 * @lv2_urid_manager: the #AgsLv2UridManager
 * @uri: the object to remove
 * 
 * Removes an entry associated with @urid.
 *
 * Returns: %TRUE as successfully removed, otherwise %FALSE
 *
 * Since: 0.7.9
 */
gboolean
ags_lv2_urid_manager_remove(AgsLv2UridManager *lv2_urid_manager,
			    gchar *uri)
{
  GValue *id;

  id = g_hash_table_lookup(lv2_urid_manager->urid,
			   uri);

  if(id != NULL){
    g_hash_table_remove(lv2_urid_manager->urid,
			uri);
  }
  
  return(TRUE);
}

/**
 * ags_lv2_urid_manager_lookup:
 * @lv2_urid_manager: the #AgsLv2UridManager
 * @uri: the object to lookup
 *
 * Lookup a id associated with @urid in @lv2_urid_manager
 *
 * Returns: the id on success, else the newly created id
 *
 * Since: 0.7.9
 */
GValue*
ags_lv2_urid_manager_lookup(AgsLv2UridManager *lv2_urid_manager,
			    gchar *uri)
{
  GValue *value;

  value = (GValue *) g_hash_table_lookup(lv2_urid_manager->urid,
					 uri);

  if(value == NULL){
    g_message("new uri %s", uri);
    
    value = g_new0(GValue,
		   1);
    g_value_init(value,
		 G_TYPE_ULONG);

    g_value_set_ulong(value,
		      lv2_urid_manager->id_counter);
    ags_lv2_urid_manager_insert(lv2_urid_manager,
				uri,
				value);

    lv2_urid_manager->id_counter++;
  }
  
  return(value);
}

void
ags_lv2_urid_manager_load_default(AgsLv2UridManager *lv2_urid_manager)
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
		      lv2_urid_manager->id_counter);
    ags_lv2_urid_manager_insert(lv2_urid_manager,
				*str,
				value);

    lv2_urid_manager->id_counter++;
    str++;
  }
}

uint32_t
ags_lv2_urid_manager_map(LV2_URID_Map_Handle handle,
			 char *uri)
{
  GValue *value;
  uint32_t id;

  value = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
				      uri);
  id = g_value_get_ulong(value);
  
  return(id);
}

const char*
ags_lv2_urid_manager_unmap(LV2_URID_Map_Handle handle,
			   uint32_t urid)
{
  AgsLv2UridManager *lv2_urid_manager;
  gchar *retval;
  
  GValue value = {0,};

  lv2_urid_manager = ags_lv2_urid_manager_get_instance();
  
  g_value_init(&value,
	       G_TYPE_ULONG);
  g_value_set_ulong(&value,
		    urid);

  retval = (gchar *) g_hash_table_find(lv2_urid_manager->urid,
				       (GHRFunc) ags_lv2_urid_manager_finder,
				       &value);
  //TODO:JK: unmap
  
  return(retval);
}

/**
 * ags_lv2_urid_manager_get_instance:
 * 
 * Singleton function to optain the id manager instance.
 *
 * Returns: an instance of #AgsLv2UridManager
 *
 * Since: 0.7.9
 */
AgsLv2UridManager*
ags_lv2_urid_manager_get_instance()
{
  if(ags_lv2_urid_manager == NULL){
    ags_lv2_urid_manager = ags_lv2_urid_manager_new();
    
    //    ags_lv2_urid_manager_load_default(ags_lv2_urid_manager);
  }

  return(ags_lv2_urid_manager);
}

/**
 * ags_lv2_urid_manager_new:
 *
 * Instantiate a id manager. 
 *
 * Returns: a new #AgsLv2UridManager
 *
 * Since: 0.7.9
 */
AgsLv2UridManager*
ags_lv2_urid_manager_new()
{
  AgsLv2UridManager *lv2_urid_manager;

  lv2_urid_manager = (AgsLv2UridManager *) g_object_new(AGS_TYPE_LV2_URID_MANAGER,
							NULL);

  return(lv2_urid_manager);
}
