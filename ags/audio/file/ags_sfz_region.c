/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/file/ags_sfz_region.h>

#include <ags/audio/file/ags_sfz_group.h>
#include <ags/audio/file/ags_sfz_sample.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_sfz_region_class_init(AgsSFZRegionClass *sfz_region);
void ags_sfz_region_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sfz_region_init(AgsSFZRegion *sfz_region);
void ags_sfz_region_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_sfz_region_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_sfz_region_dispose(GObject *gobject);
void ags_sfz_region_finalize(GObject *gobject);

AgsUUID* ags_sfz_region_get_uuid(AgsConnectable *connectable);
gboolean ags_sfz_region_has_resource(AgsConnectable *connectable);
gboolean ags_sfz_region_is_ready(AgsConnectable *connectable);
void ags_sfz_region_add_to_registry(AgsConnectable *connectable);
void ags_sfz_region_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_sfz_region_list_resource(AgsConnectable *connectable);
xmlNode* ags_sfz_region_xml_compose(AgsConnectable *connectable);
void ags_sfz_region_xml_parse(AgsConnectable *connectable,
			      xmlNode *node);
gboolean ags_sfz_region_is_connected(AgsConnectable *connectable);
void ags_sfz_region_connect(AgsConnectable *connectable);
void ags_sfz_region_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_sfz_region
 * @short_description: interfacing SFZ regions
 * @title: AgsSFZRegion
 * @section_id:
 * @include: ags/audio/file/ags_sfz_region.h
 *
 * #AgsSFZRegion is the base object to ineract with SFZ regions.
 */

enum{
  PROP_0,
  PROP_NTH_REGION,
  PROP_GROUP,
  PROP_SAMPLE,
};

static gpointer ags_sfz_region_parent_class = NULL;

GType
ags_sfz_region_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sfz_region = 0;

    static const GTypeInfo ags_sfz_region_info = {
      sizeof(AgsSFZRegionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sfz_region_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSFZRegion),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sfz_region_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sfz_region_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_sfz_region = g_type_register_static(G_TYPE_OBJECT,
						 "AgsSFZRegion",
						 &ags_sfz_region_info,
						 0);

    g_type_add_interface_static(ags_type_sfz_region,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sfz_region);
  }

  return g_define_type_id__volatile;
}

void
ags_sfz_region_class_init(AgsSFZRegionClass *sfz_region)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_sfz_region_parent_class = g_type_class_peek_parent(sfz_region);

  gobject = (GObjectClass *) sfz_region;

  gobject->set_property = ags_sfz_region_set_property;
  gobject->get_property = ags_sfz_region_get_property;

  gobject->dispose = ags_sfz_region_dispose;
  gobject->finalize = ags_sfz_region_finalize;

  /* properties */
  /**
   * AgsSFZRegion:nth-region:
   *
   * The nth region.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_int("nth-region",
				i18n_pspec("nth region"),
				i18n_pspec("The nth region it belongs to"),
				-1,
				G_MAXINT32,
				-1,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NTH_REGION,
				  param_spec);

  /**
   * AgsSFZRegion:group:
   *
   * The group assigned with.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("group",
				   i18n_pspec("assigned group"),
				   i18n_pspec("The group it is assigned with"),
				   AGS_TYPE_SFZ_GROUP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GROUP,
				  param_spec);

  /**
   * AgsSFZRegion:sample:
   *
   * The sample assigned with.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("sample",
				   i18n_pspec("assigned sample"),
				   i18n_pspec("The sample it is assigned with"),
				   AGS_TYPE_SFZ_SAMPLE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLE,
				  param_spec);
}

void
ags_sfz_region_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_sfz_region_get_uuid;
  connectable->has_resource = ags_sfz_region_has_resource;
  connectable->is_ready = ags_sfz_region_is_ready;

  connectable->add_to_registry = ags_sfz_region_add_to_registry;
  connectable->remove_from_registry = ags_sfz_region_remove_from_registry;

  connectable->list_resource = ags_sfz_region_list_resource;
  connectable->xml_compose = ags_sfz_region_xml_compose;
  connectable->xml_parse = ags_sfz_region_xml_parse;

  connectable->is_connected = ags_sfz_region_is_connected;
  
  connectable->connect = ags_sfz_region_connect;
  connectable->disconnect = ags_sfz_region_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_sfz_region_init(AgsSFZRegion *sfz_region)
{
  AgsConfig *config;

  sfz_region->flags = 0;

  /* add audio file mutex */
  g_rec_mutex_init(&(sfz_region->obj_mutex));

  /* uuid */
  sfz_region->uuid = ags_uuid_alloc();
  ags_uuid_generate(sfz_region->uuid);

  sfz_region->nth_region = -1;

  sfz_region->group = NULL;
  sfz_region->sample = NULL;

  sfz_region->control = g_hash_table_new_full(g_str_hash, g_str_equal,
					      NULL,
					      NULL);
}

void
ags_sfz_region_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsSFZRegion *sfz_region;

  GRecMutex *sfz_region_mutex;

  sfz_region = AGS_SFZ_REGION(gobject);

  /* get sfz region mutex */
  sfz_region_mutex = AGS_SFZ_REGION_GET_OBJ_MUTEX(sfz_region);

  switch(prop_id){
  case PROP_NTH_REGION:
  {
    gint nth_region;

    nth_region = g_value_get_int(value);

    g_rec_mutex_lock(sfz_region_mutex);

    if(nth_region == sfz_region->nth_region){
      g_rec_mutex_unlock(sfz_region_mutex);

      return;	
    }
    
    sfz_region->nth_region = nth_region;

    g_rec_mutex_unlock(sfz_region_mutex);
  }
  break;
  case PROP_GROUP:
  {
    GObject *group;

    group = g_value_get_object(value);

    g_rec_mutex_lock(sfz_region_mutex);

    if(sfz_region->group == group){
      g_rec_mutex_unlock(sfz_region_mutex);

      return;	
    }

    if(sfz_region->group != NULL){
      g_object_unref(sfz_region->group);
    }

    if(group != NULL){
      g_object_ref(group);
    }
    
    sfz_region->group = group;

    g_rec_mutex_unlock(sfz_region_mutex);
  }
  break;
  case PROP_SAMPLE:
  {
    GObject *sample;

    sample = g_value_get_object(value);

    g_rec_mutex_lock(sfz_region_mutex);

    if(sfz_region->sample == sample){
      g_rec_mutex_unlock(sfz_region_mutex);

      return;	
    }

    if(sfz_region->sample != NULL){
      g_object_unref(sfz_region->sample);
    }

    if(sample != NULL){
      g_object_ref(sample);
    }
    
    sfz_region->sample = sample;

    g_rec_mutex_unlock(sfz_region_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_sfz_region_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsSFZRegion *sfz_region;

  GRecMutex *sfz_region_mutex;

  sfz_region = (AgsSFZRegion *) gobject;

  /* get sfz region mutex */
  sfz_region_mutex = AGS_SFZ_REGION_GET_OBJ_MUTEX(sfz_region);
  
  switch(prop_id){
  case PROP_NTH_REGION:
  {
    g_rec_mutex_lock(sfz_region_mutex);

    g_value_set_int(value, sfz_region->nth_region);

    g_rec_mutex_unlock(sfz_region_mutex);
  }
  break;
  case PROP_GROUP:
  {
    g_rec_mutex_lock(sfz_region_mutex);

    g_value_set_object(value, sfz_region->group);

    g_rec_mutex_unlock(sfz_region_mutex);
  }
  break;
  case PROP_SAMPLE:
  {
    g_rec_mutex_lock(sfz_region_mutex);

    g_value_set_object(value, sfz_region->sample);

    g_rec_mutex_unlock(sfz_region_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_sfz_region_dispose(GObject *gobject)
{
  AgsSFZRegion *sfz_region;

  sfz_region = AGS_SFZ_REGION(gobject);

  if(sfz_region->sample != NULL){
    g_object_unref(sfz_region->sample);

    sfz_region->sample = NULL;
  }

  if(sfz_region->group != NULL){
    g_object_unref(sfz_region->group);

    sfz_region->group = NULL;
  }

  /* call parent */  
  G_OBJECT_CLASS(ags_sfz_region_parent_class)->dispose(gobject);
}

void
ags_sfz_region_finalize(GObject *gobject)
{
  AgsSFZRegion *sfz_region;

  sfz_region = AGS_SFZ_REGION(gobject);

  if(sfz_region->sample != NULL){
    g_object_unref(sfz_region->sample);
  }

  if(sfz_region->group != NULL){
    g_object_unref(sfz_region->group);
  }
  
  /* call parent */  
  G_OBJECT_CLASS(ags_sfz_region_parent_class)->finalize(gobject);
}

AgsUUID*
ags_sfz_region_get_uuid(AgsConnectable *connectable)
{
  AgsSFZRegion *sfz_region;
  
  AgsUUID *ptr;

  GRecMutex *sfz_region_mutex;

  sfz_region = AGS_SFZ_REGION(connectable);

  /* get audio file mutex */
  sfz_region_mutex = AGS_SFZ_REGION_GET_OBJ_MUTEX(sfz_region);

  /* get UUID */
  g_rec_mutex_lock(sfz_region_mutex);

  ptr = sfz_region->uuid;

  g_rec_mutex_unlock(sfz_region_mutex);
  
  return(ptr);
}

gboolean
ags_sfz_region_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_sfz_region_is_ready(AgsConnectable *connectable)
{
  AgsSFZRegion *sfz_region;
  
  gboolean is_ready;

  sfz_region = AGS_SFZ_REGION(connectable);

  /* check is ready */  
  is_ready = ags_sfz_region_test_flags(sfz_region, AGS_SFZ_REGION_ADDED_TO_REGISTRY);

  return(is_ready);
}

void
ags_sfz_region_add_to_registry(AgsConnectable *connectable)
{
  AgsSFZRegion *sfz_region;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }

  sfz_region = AGS_SFZ_REGION(connectable);

  ags_sfz_region_set_flags(sfz_region, AGS_SFZ_REGION_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(entry->entry,
		       (gpointer) sfz_region);
    ags_registry_add_entry(registry,
			   entry);
  }  
}

void
ags_sfz_region_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_sfz_region_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_sfz_region_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_sfz_region_xml_parse(AgsConnectable *connectable,
			 xmlNode *node)
{
  //TODO:JK: implement me  
}

gboolean
ags_sfz_region_is_connected(AgsConnectable *connectable)
{
  AgsSFZRegion *sfz_region;
  
  gboolean is_connected;

  sfz_region = AGS_SFZ_REGION(connectable);

  /* check is connected */
  is_connected = ags_sfz_region_test_flags(sfz_region, AGS_SFZ_REGION_CONNECTED);

  return(is_connected);
}

void
ags_sfz_region_connect(AgsConnectable *connectable)
{
  AgsSFZRegion *sfz_region;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  sfz_region = AGS_SFZ_REGION(connectable);
  
  ags_sfz_region_set_flags(sfz_region, AGS_SFZ_REGION_CONNECTED);
}

void
ags_sfz_region_disconnect(AgsConnectable *connectable)
{
  AgsSFZRegion *sfz_region;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  sfz_region = AGS_SFZ_REGION(connectable);

  ags_sfz_region_unset_flags(sfz_region, AGS_SFZ_REGION_CONNECTED);
}

/**
 * ags_sfz_region_test_flags:
 * @sfz_region: the #AgsSFZRegion
 * @flags: the flags
 *
 * Test @flags to be set on @sfz_region.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_sfz_region_test_flags(AgsSFZRegion *sfz_region, guint flags)
{
  gboolean retval;  
  
  GRecMutex *sfz_region_mutex;

  if(!AGS_IS_SFZ_REGION(sfz_region)){
    return(FALSE);
  }

  /* get sfz_region mutex */
  sfz_region_mutex = AGS_SFZ_REGION_GET_OBJ_MUTEX(sfz_region);

  /* test */
  g_rec_mutex_lock(sfz_region_mutex);

  retval = (flags & (sfz_region->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(sfz_region_mutex);

  return(retval);
}

/**
 * ags_sfz_region_set_flags:
 * @sfz_region: the #AgsSFZRegion
 * @flags: see #AgsSFZRegionFlags-enum
 *
 * Enable a feature of @sfz_region.
 *
 * Since: 3.0.0
 */
void
ags_sfz_region_set_flags(AgsSFZRegion *sfz_region, guint flags)
{
  GRecMutex *sfz_region_mutex;

  if(!AGS_IS_SFZ_REGION(sfz_region)){
    return;
  }

  /* get sfz_region mutex */
  sfz_region_mutex = AGS_SFZ_REGION_GET_OBJ_MUTEX(sfz_region);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(sfz_region_mutex);

  sfz_region->flags |= flags;
  
  g_rec_mutex_unlock(sfz_region_mutex);
}
    
/**
 * ags_sfz_region_unset_flags:
 * @sfz_region: the #AgsSFZRegion
 * @flags: see #AgsSFZRegionFlags-enum
 *
 * Disable a feature of @sfz_region.
 *
 * Since: 3.0.0
 */
void
ags_sfz_region_unset_flags(AgsSFZRegion *sfz_region, guint flags)
{  
  GRecMutex *sfz_region_mutex;

  if(!AGS_IS_SFZ_REGION(sfz_region)){
    return;
  }

  /* get sfz_region mutex */
  sfz_region_mutex = AGS_SFZ_REGION_GET_OBJ_MUTEX(sfz_region);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(sfz_region_mutex);

  sfz_region->flags &= (~flags);
  
  g_rec_mutex_unlock(sfz_region_mutex);
}

/**
 * ags_sfz_region_insert_control:
 * @sfz_region: the #AgsSFZRegion
 * @key: the key
 * @value: the value
 *
 * Insert control specified by @key and @value to @sfz_region.
 *
 * Since: 3.0.0
 */
void
ags_sfz_region_insert_control(AgsSFZRegion *sfz_region,
			      gchar *key,
			      gchar *value)
{
  GRecMutex *sfz_region_mutex;

  if(!AGS_IS_SFZ_REGION(sfz_region)){
    return;
  }

  /* get sfz_region mutex */
  sfz_region_mutex = AGS_SFZ_REGION_GET_OBJ_MUTEX(sfz_region);
  
  /* insert */
  g_rec_mutex_lock(sfz_region_mutex);
  
  g_hash_table_insert(sfz_region->control,
		      key,
		      value);

  g_rec_mutex_unlock(sfz_region_mutex);
}

/**
 * ags_sfz_region_lookup_control:
 * @sfz_region: the #AgsSFZRegion
 * @key: the key
 *
 * Lookup control specified by @key of @sfz_region.
 *
 * Since: 3.0.0
 */
gchar*
ags_sfz_region_lookup_control(AgsSFZRegion *sfz_region,
			      gchar *key)
{
  gchar *value;
  
  GRecMutex *sfz_region_mutex;

  if(!AGS_IS_SFZ_REGION(sfz_region)){
    return(NULL);
  }

  /* get sfz_region mutex */
  sfz_region_mutex = AGS_SFZ_REGION_GET_OBJ_MUTEX(sfz_region);
  
  /* lookup */
  g_rec_mutex_lock(sfz_region_mutex);
  
  value = g_hash_table_lookup(sfz_region->control,
			      key);

  value = g_strdup(value);
  
  g_rec_mutex_unlock(sfz_region_mutex);

  return(value);
}

/**
 * ags_sfz_region_new:
 *
 * Creates a new instance of #AgsSFZRegion.
 *
 * Returns: the new #AgsSFZRegion.
 *
 * Since: 3.0.0
 */
AgsSFZRegion*
ags_sfz_region_new()
{
  AgsSFZRegion *sfz_region;

  sfz_region = (AgsSFZRegion *) g_object_new(AGS_TYPE_SFZ_REGION,
					     NULL);

  return(sfz_region);
}
