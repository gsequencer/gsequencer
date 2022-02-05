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

#include <ags/audio/file/ags_sfz_group.h>

#include <ags/audio/file/ags_sfz_region.h>
#include <ags/audio/file/ags_sfz_sample.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_sfz_group_class_init(AgsSFZGroupClass *sfz_group);
void ags_sfz_group_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sfz_group_init(AgsSFZGroup *sfz_group);
void ags_sfz_group_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_sfz_group_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_sfz_group_dispose(GObject *gobject);
void ags_sfz_group_finalize(GObject *gobject);

AgsUUID* ags_sfz_group_get_uuid(AgsConnectable *connectable);
gboolean ags_sfz_group_has_resource(AgsConnectable *connectable);
gboolean ags_sfz_group_is_ready(AgsConnectable *connectable);
void ags_sfz_group_add_to_registry(AgsConnectable *connectable);
void ags_sfz_group_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_sfz_group_list_resource(AgsConnectable *connectable);
xmlNode* ags_sfz_group_xml_compose(AgsConnectable *connectable);
void ags_sfz_group_xml_parse(AgsConnectable *connectable,
			     xmlNode *node);
gboolean ags_sfz_group_is_connected(AgsConnectable *connectable);
void ags_sfz_group_connect(AgsConnectable *connectable);
void ags_sfz_group_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_sfz_group
 * @short_description: interfacing SFZ groups
 * @title: AgsSFZGroup
 * @section_id:
 * @include: ags/audio/file/ags_sfz_group.h
 *
 * #AgsSFZGroup is the base object to ineract with SFZ groups.
 */

enum{
  PROP_0,
  PROP_NTH_GROUP,
  PROP_REGION,
  PROP_SAMPLE,
};

static gpointer ags_sfz_group_parent_class = NULL;

GType
ags_sfz_group_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sfz_group = 0;

    static const GTypeInfo ags_sfz_group_info = {
      sizeof(AgsSFZGroupClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sfz_group_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSFZGroup),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sfz_group_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sfz_group_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_sfz_group = g_type_register_static(G_TYPE_OBJECT,
						"AgsSFZGroup",
						&ags_sfz_group_info,
						0);

    g_type_add_interface_static(ags_type_sfz_group,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sfz_group);
  }

  return g_define_type_id__volatile;
}

void
ags_sfz_group_class_init(AgsSFZGroupClass *sfz_group)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_sfz_group_parent_class = g_type_class_peek_parent(sfz_group);

  gobject = (GObjectClass *) sfz_group;

  gobject->set_property = ags_sfz_group_set_property;
  gobject->get_property = ags_sfz_group_get_property;

  gobject->dispose = ags_sfz_group_dispose;
  gobject->finalize = ags_sfz_group_finalize;

  /* properties */
  /**
   * AgsSFZGroup:nth-group:
   *
   * The nth group.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_int("nth-group",
				i18n_pspec("nth group"),
				i18n_pspec("The nth group it belongs to"),
				-1,
				G_MAXINT32,
				-1,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NTH_GROUP,
				  param_spec);

  /**
   * AgsSFZGroup:region: (type GList(AgsSFZRegion)) (transfer full)
   *
   * The region assigned with.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("region",
				    i18n_pspec("assigned region"),
				    i18n_pspec("The region it is assigned with"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_REGION,
				  param_spec);

  /**
   * AgsSFZGroup:sample:
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
ags_sfz_group_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_sfz_group_get_uuid;
  connectable->has_resource = ags_sfz_group_has_resource;
  connectable->is_ready = ags_sfz_group_is_ready;

  connectable->add_to_registry = ags_sfz_group_add_to_registry;
  connectable->remove_from_registry = ags_sfz_group_remove_from_registry;

  connectable->list_resource = ags_sfz_group_list_resource;
  connectable->xml_compose = ags_sfz_group_xml_compose;
  connectable->xml_parse = ags_sfz_group_xml_parse;

  connectable->is_connected = ags_sfz_group_is_connected;
  
  connectable->connect = ags_sfz_group_connect;
  connectable->disconnect = ags_sfz_group_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_sfz_group_init(AgsSFZGroup *sfz_group)
{
  AgsConfig *config;

  sfz_group->flags = 0;

  /* add audio file mutex */
  g_rec_mutex_init(&(sfz_group->obj_mutex)); 

  /* uuid */
  sfz_group->uuid = ags_uuid_alloc();
  ags_uuid_generate(sfz_group->uuid);

  sfz_group->nth_group = -1;

  sfz_group->region = NULL;
  sfz_group->sample = NULL;

  sfz_group->control = g_hash_table_new_full(g_str_hash, g_str_equal,
					     NULL,
					     NULL);
}

void
ags_sfz_group_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsSFZGroup *sfz_group;

  GRecMutex *sfz_group_mutex;

  sfz_group = AGS_SFZ_GROUP(gobject);

  /* get sfz group mutex */
  sfz_group_mutex = AGS_SFZ_GROUP_GET_OBJ_MUTEX(sfz_group);

  switch(prop_id){
  case PROP_NTH_GROUP:
  {
    gint nth_group;

    nth_group = g_value_get_int(value);

    g_rec_mutex_lock(sfz_group_mutex);

    if(nth_group == sfz_group->nth_group){
      g_rec_mutex_unlock(sfz_group_mutex);

      return;	
    }
    
    sfz_group->nth_group = nth_group;

    g_rec_mutex_unlock(sfz_group_mutex);
  }
  break;
  case PROP_REGION:
  {
    GObject *region;

    region = g_value_get_pointer(value);

    g_rec_mutex_lock(sfz_group_mutex);

    if(region == NULL ||
       g_list_find(sfz_group->region, region) != NULL){
      g_rec_mutex_unlock(sfz_group_mutex);

      return;	
    }

    g_object_ref(region);
    sfz_group->region = g_list_prepend(sfz_group->region,
				       region);

    g_rec_mutex_unlock(sfz_group_mutex);
  }
  break;
  case PROP_SAMPLE:
  {
    GObject *sample;

    sample = g_value_get_object(value);

    g_rec_mutex_lock(sfz_group_mutex);

    if(sfz_group->sample == sample){
      g_rec_mutex_unlock(sfz_group_mutex);

      return;	
    }

    if(sfz_group->sample != NULL){
      g_object_unref(sfz_group->sample);
    }

    if(sample != NULL){
      g_object_ref(sample);
    }
    
    sfz_group->sample = sample;

    g_rec_mutex_unlock(sfz_group_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_sfz_group_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsSFZGroup *sfz_group;

  GRecMutex *sfz_group_mutex;

  sfz_group = (AgsSFZGroup *) gobject;

  /* get sfz group mutex */
  sfz_group_mutex = AGS_SFZ_GROUP_GET_OBJ_MUTEX(sfz_group);
  
  switch(prop_id){
  case PROP_NTH_GROUP:
  {
    g_rec_mutex_lock(sfz_group_mutex);

    g_value_set_int(value, sfz_group->nth_group);

    g_rec_mutex_unlock(sfz_group_mutex);
  }
  break;
  case PROP_REGION:
  {
    g_rec_mutex_lock(sfz_group_mutex);

    g_value_set_pointer(value, g_list_copy_deep(sfz_group->region,
						(GCopyFunc) g_object_ref,
						NULL));

    g_rec_mutex_unlock(sfz_group_mutex);
  }
  break;
  case PROP_SAMPLE:
  {
    g_rec_mutex_lock(sfz_group_mutex);

    g_value_set_object(value, sfz_group->sample);

    g_rec_mutex_unlock(sfz_group_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_sfz_group_dispose(GObject *gobject)
{
  AgsSFZGroup *sfz_group;

  sfz_group = AGS_SFZ_GROUP(gobject);

  if(sfz_group->sample != NULL){
    g_object_unref(sfz_group->sample);

    sfz_group->sample = NULL;
  }

  if(sfz_group->region != NULL){
    g_list_free_full(sfz_group->region,
		     g_object_unref);

    sfz_group->region = NULL;
  }

  /* call parent */  
  G_OBJECT_CLASS(ags_sfz_group_parent_class)->dispose(gobject);
}

void
ags_sfz_group_finalize(GObject *gobject)
{
  AgsSFZGroup *sfz_group;

  sfz_group = AGS_SFZ_GROUP(gobject);

  if(sfz_group->sample != NULL){
    g_object_unref(sfz_group->sample);
  }

  if(sfz_group->region != NULL){
    g_list_free_full(sfz_group->region,
		     g_object_unref);
  }
  
  /* call parent */  
  G_OBJECT_CLASS(ags_sfz_group_parent_class)->finalize(gobject);
}

AgsUUID*
ags_sfz_group_get_uuid(AgsConnectable *connectable)
{
  AgsSFZGroup *sfz_group;
  
  AgsUUID *ptr;

  GRecMutex *sfz_group_mutex;

  sfz_group = AGS_SFZ_GROUP(connectable);

  /* get audio file mutex */
  sfz_group_mutex = AGS_SFZ_GROUP_GET_OBJ_MUTEX(sfz_group);

  /* get UUID */
  g_rec_mutex_lock(sfz_group_mutex);

  ptr = sfz_group->uuid;

  g_rec_mutex_unlock(sfz_group_mutex);
  
  return(ptr);
}

gboolean
ags_sfz_group_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_sfz_group_is_ready(AgsConnectable *connectable)
{
  AgsSFZGroup *sfz_group;
  
  gboolean is_ready;

  sfz_group = AGS_SFZ_GROUP(connectable);

  /* check is ready */
  is_ready = ags_sfz_group_test_flags(sfz_group, AGS_SFZ_GROUP_ADDED_TO_REGISTRY);

  return(is_ready);
}

void
ags_sfz_group_add_to_registry(AgsConnectable *connectable)
{
  AgsSFZGroup *sfz_group;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }

  sfz_group = AGS_SFZ_GROUP(connectable);

  ags_sfz_group_set_flags(sfz_group, AGS_SFZ_GROUP_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(entry->entry,
		       (gpointer) sfz_group);
    ags_registry_add_entry(registry,
			   entry);
  }  
}

void
ags_sfz_group_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_sfz_group_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_sfz_group_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_sfz_group_xml_parse(AgsConnectable *connectable,
			xmlNode *node)
{
  //TODO:JK: implement me  
}

gboolean
ags_sfz_group_is_connected(AgsConnectable *connectable)
{
  AgsSFZGroup *sfz_group;
  
  gboolean is_connected;

  sfz_group = AGS_SFZ_GROUP(connectable);

  /* check is connected */
  is_connected = ags_sfz_group_test_flags(sfz_group, AGS_SFZ_GROUP_CONNECTED);

  return(is_connected);
}

void
ags_sfz_group_connect(AgsConnectable *connectable)
{
  AgsSFZGroup *sfz_group;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  sfz_group = AGS_SFZ_GROUP(connectable);
  
  ags_sfz_group_set_flags(sfz_group, AGS_SFZ_GROUP_CONNECTED);
}

void
ags_sfz_group_disconnect(AgsConnectable *connectable)
{
  AgsSFZGroup *sfz_group;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  sfz_group = AGS_SFZ_GROUP(connectable);

  ags_sfz_group_unset_flags(sfz_group, AGS_SFZ_GROUP_CONNECTED);
}

/**
 * ags_sfz_group_test_flags:
 * @sfz_group: the #AgsSFZGroup
 * @flags: the flags
 *
 * Test @flags to be set on @sfz_group.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_sfz_group_test_flags(AgsSFZGroup *sfz_group, guint flags)
{
  gboolean retval;  
  
  GRecMutex *sfz_group_mutex;

  if(!AGS_IS_SFZ_GROUP(sfz_group)){
    return(FALSE);
  }

  /* get sfz_group mutex */
  sfz_group_mutex = AGS_SFZ_GROUP_GET_OBJ_MUTEX(sfz_group);

  /* test */
  g_rec_mutex_lock(sfz_group_mutex);

  retval = (flags & (sfz_group->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(sfz_group_mutex);

  return(retval);
}

/**
 * ags_sfz_group_set_flags:
 * @sfz_group: the #AgsSFZGroup
 * @flags: see #AgsSFZGroupFlags-enum
 *
 * Enable a feature of @sfz_group.
 *
 * Since: 3.0.0
 */
void
ags_sfz_group_set_flags(AgsSFZGroup *sfz_group, guint flags)
{
  GRecMutex *sfz_group_mutex;

  if(!AGS_IS_SFZ_GROUP(sfz_group)){
    return;
  }

  /* get sfz_group mutex */
  sfz_group_mutex = AGS_SFZ_GROUP_GET_OBJ_MUTEX(sfz_group);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(sfz_group_mutex);

  sfz_group->flags |= flags;
  
  g_rec_mutex_unlock(sfz_group_mutex);
}
    
/**
 * ags_sfz_group_unset_flags:
 * @sfz_group: the #AgsSFZGroup
 * @flags: see #AgsSFZGroupFlags-enum
 *
 * Disable a feature of @sfz_group.
 *
 * Since: 3.0.0
 */
void
ags_sfz_group_unset_flags(AgsSFZGroup *sfz_group, guint flags)
{  
  GRecMutex *sfz_group_mutex;

  if(!AGS_IS_SFZ_GROUP(sfz_group)){
    return;
  }

  /* get sfz_group mutex */
  sfz_group_mutex = AGS_SFZ_GROUP_GET_OBJ_MUTEX(sfz_group);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(sfz_group_mutex);

  sfz_group->flags &= (~flags);
  
  g_rec_mutex_unlock(sfz_group_mutex);
}

/**
 * ags_sfz_group_get_region:
 * @sfz_group: the #AgsSFZGroup
 *
 * Get the regions of @sfz_group.
 * 
 * Returns: (element-type AgsAudio.SFZRegion) (transfer full): the #GList-struct containing #AgsSFZRegion
 *
 * Since: 3.17.0
 */
GList*
ags_sfz_group_get_region(AgsSFZGroup *sfz_group)
{
  GList *start_region;
  
  if(!AGS_IS_SFZ_GROUP(sfz_group)){
    return(NULL);
  }

  start_region = NULL;

  g_object_get(sfz_group,
	       "region", &start_region,
	       NULL);
  
  return(start_region);
}

/**
 * ags_sfz_group_set_region:
 * @sfz_group: the #AgsSFZGroup
 * @region: (element-type AgsAudio.SFZRegion) (transfer none): the #GList-struct containing #AgsSFZRegion
 *
 * Set the region field of @sfz_group
 * 
 * Since: 3.17.0
 */
void
ags_sfz_group_set_region(AgsSFZGroup *sfz_group,
			 GList *region)
{
  GRecMutex *sfz_group_mutex;

  if(!AGS_IS_SFZ_GROUP(sfz_group)){
    return;
  }

  sfz_group_mutex = AGS_SFZ_GROUP_GET_OBJ_MUTEX(sfz_group);

  g_rec_mutex_lock(sfz_group_mutex);
  
  g_list_free_full(sfz_group->region,
		   (GDestroyNotify) g_object_unref);
  
  sfz_group->region = g_list_copy_deep(region,
				       (GCopyFunc) g_object_ref,
				       NULL);
  
  g_rec_mutex_unlock(sfz_group_mutex);
}

/**
 * ags_sfz_group_get_sample:
 * @sfz_group: the #AgsSFZGroup
 *
 * Get the samples of @sfz_group.
 * 
 * Returns: (transfer full): the #AgsSFZSample
 *
 * Since: 3.17.0
 */
GObject*
ags_sfz_group_get_sample(AgsSFZGroup *sfz_group)
{
  GObject *sample;
  
  if(!AGS_IS_SFZ_GROUP(sfz_group)){
    return(NULL);
  }

  sample = NULL;

  g_object_get(sfz_group,
	       "sample", &sample,
	       NULL);
  
  return(sample);
}

/**
 * ags_sfz_group_set_sample:
 * @sfz_group: the #AgsSFZGroup
 * @sample: (transfer none): the #AgsSFZSample
 *
 * Set the sample field of @sfz_group
 * 
 * Since: 3.17.0
 */
void
ags_sfz_group_set_sample(AgsSFZGroup *sfz_group,
			 GObject *sample)
{
  if(!AGS_IS_SFZ_GROUP(sfz_group)){
    return;
  }

  g_object_set(sfz_group,
	       "sample", sample,
	       NULL);
}

/**
 * ags_sfz_group_get_control:
 * @sfz_group: the #AgsSFZGroup
 *
 * Get all control of @sfz_group.
 * 
 * Returns: (transfer container): the #GList-struct containing controls as string
 *
 * Since: 3.17.0
 */
GList*
ags_sfz_group_get_control(AgsSFZGroup *sfz_group)
{
  GList *start_list;
  
  GRecMutex *sfz_group_mutex;

  if(!AGS_IS_SFZ_GROUP(sfz_group)){
    return(NULL);
  }

  /* get sfz_group mutex */
  sfz_group_mutex = AGS_SFZ_GROUP_GET_OBJ_MUTEX(sfz_group);
  
  /* get */
  g_rec_mutex_lock(sfz_group_mutex);

  start_list = g_hash_table_get_keys(sfz_group->control);
  
  g_rec_mutex_unlock(sfz_group_mutex);

  return(start_list);
}

/**
 * ags_sfz_group_insert_control:
 * @sfz_group: the #AgsSFZGroup
 * @key: the key
 * @value: the value
 *
 * Insert control specified by @key and @value to @sfz_group.
 *
 * Since: 3.0.0
 */
void
ags_sfz_group_insert_control(AgsSFZGroup *sfz_group,
			     gchar *key,
			     gchar *value)
{
  GRecMutex *sfz_group_mutex;

  if(!AGS_IS_SFZ_GROUP(sfz_group)){
    return;
  }

  /* get sfz_group mutex */
  sfz_group_mutex = AGS_SFZ_GROUP_GET_OBJ_MUTEX(sfz_group);
  
  /* insert */
  g_rec_mutex_lock(sfz_group_mutex);
  
  g_hash_table_insert(sfz_group->control,
		      key,
		      value);

  g_rec_mutex_unlock(sfz_group_mutex);
}

/**
 * ags_sfz_group_lookup_control:
 * @sfz_group: the #AgsSFZGroup
 * @key: the key
 *
 * Lookup control specified by @key of @sfz_group.
 *
 * Returns: (transfer full): the matching value of key
 * 
 * Since: 3.0.0
 */
gchar*
ags_sfz_group_lookup_control(AgsSFZGroup *sfz_group,
			     gchar *key)
{
  gchar *value;
  
  GRecMutex *sfz_group_mutex;

  if(!AGS_IS_SFZ_GROUP(sfz_group)){
    return(NULL);
  }

  /* get sfz_group mutex */
  sfz_group_mutex = AGS_SFZ_GROUP_GET_OBJ_MUTEX(sfz_group);
  
  /* lookup */
  g_rec_mutex_lock(sfz_group_mutex);
  
  value = g_hash_table_lookup(sfz_group->control,
			      key);

  value = g_strdup(value);
  
  g_rec_mutex_unlock(sfz_group_mutex);

  return(value);
}

/**
 * ags_sfz_group_new:
 *
 * Creates a new instance of #AgsSFZGroup.
 *
 * Returns: the new #AgsSFZGroup.
 *
 * Since: 3.0.0
 */
AgsSFZGroup*
ags_sfz_group_new()
{
  AgsSFZGroup *sfz_group;

  sfz_group = (AgsSFZGroup *) g_object_new(AGS_TYPE_SFZ_GROUP,
					   NULL);

  return(sfz_group);
}
