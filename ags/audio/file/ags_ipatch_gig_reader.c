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

#include <ags/audio/file/ags_ipatch_gig_reader.h>

#include <ags/libags.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_ipatch_gig_reader_class_init(AgsIpatchGigReaderClass *ipatch_gig_reader);
void ags_ipatch_gig_reader_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ipatch_gig_reader_init(AgsIpatchGigReader *ipatch_gig_reader);
void ags_ipatch_gig_reader_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_ipatch_gig_reader_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_ipatch_gig_reader_dispose(GObject *gobject);
void ags_ipatch_gig_reader_finalize(GObject *gobject);

AgsUUID* ags_ipatch_gig_reader_get_uuid(AgsConnectable *connectable);
gboolean ags_ipatch_gig_reader_has_resource(AgsConnectable *connectable);
gboolean ags_ipatch_gig_reader_is_ready(AgsConnectable *connectable);
void ags_ipatch_gig_reader_add_to_registry(AgsConnectable *connectable);
void ags_ipatch_gig_reader_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_ipatch_gig_reader_list_resource(AgsConnectable *connectable);
xmlNode* ags_ipatch_gig_reader_xml_compose(AgsConnectable *connectable);
void ags_ipatch_gig_reader_xml_parse(AgsConnectable *connectable,
				   xmlNode *node);
gboolean ags_ipatch_gig_reader_is_connected(AgsConnectable *connectable);
void ags_ipatch_gig_reader_connect(AgsConnectable *connectable);
void ags_ipatch_gig_reader_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_ipatch_gig_reader
 * @short_description: interfacing Soundfont2 related API of libinstpatch
 * @title: AgsIpatchGigReader
 * @section_id:
 * @include: ags/audio/file/ags_ipatch_gig_reader.h
 *
 * #AgsIpatchGigReader is the base object to ineract with Soundfont2 related API.
 */

static gpointer ags_ipatch_gig_reader_parent_class = NULL;

static pthread_mutex_t ags_ipatch_gig_reader_class_mutex = PTHREAD_MUTEX_INITIALIZER;

enum{
  PROP_0,
  PROP_IPATCH,
};

GType
ags_ipatch_gig_reader_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ipatch_gig_reader = 0;

    static const GTypeInfo ags_ipatch_gig_reader_info = {
      sizeof(AgsIpatchGigReaderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ipatch_gig_reader_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsIpatchGigReader),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ipatch_gig_reader_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ipatch_gig_reader_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ipatch_gig_reader = g_type_register_static(G_TYPE_OBJECT,
							"AgsIpatchGigReader",
							&ags_ipatch_gig_reader_info,
							0);

    g_type_add_interface_static(ags_type_ipatch_gig_reader,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ipatch_gig_reader);
  }

  return g_define_type_id__volatile;
}

void
ags_ipatch_gig_reader_class_init(AgsIpatchGigReaderClass *ipatch_gig_reader)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_ipatch_gig_reader_parent_class = g_type_class_peek_parent(ipatch_gig_reader);

  gobject = (GObjectClass *) ipatch_gig_reader;

  gobject->set_property = ags_ipatch_gig_reader_set_property;
  gobject->get_property = ags_ipatch_gig_reader_get_property;

  gobject->dispose = ags_ipatch_gig_reader_dispose;
  gobject->finalize = ags_ipatch_gig_reader_finalize;

  /* properties */
  /**
   * AgsIpatchGigReader:ipatch:
   *
   * The assigned #AgsIpatch
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("ipatch",
				   i18n_pspec("the ipatch"),
				   i18n_pspec("The assigned ipatch"),
				   AGS_TYPE_IPATCH,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_IPATCH,
				  param_spec);
}

void
ags_ipatch_gig_reader_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_ipatch_gig_reader_get_uuid;
  connectable->has_resource = ags_ipatch_gig_reader_has_resource;
  connectable->is_ready = ags_ipatch_gig_reader_is_ready;

  connectable->add_to_registry = ags_ipatch_gig_reader_add_to_registry;
  connectable->remove_from_registry = ags_ipatch_gig_reader_remove_from_registry;

  connectable->list_resource = ags_ipatch_gig_reader_list_resource;
  connectable->xml_compose = ags_ipatch_gig_reader_xml_compose;
  connectable->xml_parse = ags_ipatch_gig_reader_xml_parse;

  connectable->is_connected = ags_ipatch_gig_reader_is_connected;
  
  connectable->connect = ags_ipatch_gig_reader_connect;
  connectable->disconnect = ags_ipatch_gig_reader_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_ipatch_gig_reader_init(AgsIpatchGigReader *ipatch_gig_reader)
{
  guint i;
  
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  ipatch_gig_reader->flags = 0;

  /* add audio file mutex */
  ipatch_gig_reader->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  ipatch_gig_reader->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);  

  /* uuid */
  ipatch_gig_reader->uuid = ags_uuid_alloc();
  ags_uuid_generate(ipatch_gig_reader->uuid);

  ipatch_gig_reader->level = 0;
  ipatch_gig_reader->ipatch = NULL;
  
  /* selected */
  ipatch_gig_reader->index_selected = (guint *) malloc(3 * sizeof(guint));
  memset(ipatch_gig_reader->index_selected, 0, 3 * sizeof(guint));
  
  ipatch_gig_reader->name_selected = (gchar **) malloc(4 * sizeof(gchar *));

  for(i = 0; i < 4; i++){
    ipatch_gig_reader->name_selected[i] = NULL;
  }

  /* reader */
  ipatch_gig_reader->reader = NULL;
  ipatch_gig_reader->gig = NULL;

  ipatch_gig_reader->instrument = NULL;
  ipatch_gig_reader->sample = NULL;

  ipatch_gig_reader->error = NULL;
}

void
ags_ipatch_gig_reader_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsIpatchGigReader *ipatch_gig_reader;

  pthread_mutex_t *ipatch_gig_reader_mutex;

  ipatch_gig_reader = AGS_IPATCH_GIG_READER(gobject);

  /* get ipatch sample mutex */
  pthread_mutex_lock(ags_ipatch_gig_reader_get_class_mutex());
  
  ipatch_gig_reader_mutex = ipatch_gig_reader->obj_mutex;
  
  pthread_mutex_unlock(ags_ipatch_gig_reader_get_class_mutex());

  switch(prop_id){
  case PROP_IPATCH:
    {
      AgsIpatch *ipatch;

      ipatch = (AgsIpatch *) g_value_get_object(value);

      pthread_mutex_lock(ipatch_gig_reader_mutex);

      if(ipatch_gig_reader->ipatch == ipatch){
	pthread_mutex_unlock(ipatch_gig_reader_mutex);

	return;
      }
      
      if(ipatch_gig_reader->ipatch != NULL){
	g_object_unref(ipatch_gig_reader->ipatch);
      }

      if(ipatch != NULL){
	g_object_ref(ipatch);
      }

      ipatch_gig_reader->ipatch = ipatch;

      pthread_mutex_unlock(ipatch_gig_reader_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ipatch_gig_reader_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsIpatchGigReader *ipatch_gig_reader;

  pthread_mutex_t *ipatch_gig_reader_mutex;

  ipatch_gig_reader = AGS_IPATCH_GIG_READER(gobject);

  /* get ipatch sample mutex */
  pthread_mutex_lock(ags_ipatch_gig_reader_get_class_mutex());
  
  ipatch_gig_reader_mutex = ipatch_gig_reader->obj_mutex;
  
  pthread_mutex_unlock(ags_ipatch_gig_reader_get_class_mutex());

  switch(prop_id){
  case PROP_IPATCH:
    {
      pthread_mutex_lock(ipatch_gig_reader_mutex);

      g_value_set_object(value, ipatch_gig_reader->ipatch);

      pthread_mutex_unlock(ipatch_gig_reader_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ipatch_gig_reader_dispose(GObject *gobject)
{
  AgsIpatchGigReader *ipatch_gig_reader;

  ipatch_gig_reader = AGS_IPATCH_GIG_READER(gobject);

  if(ipatch_gig_reader->ipatch != NULL){
    g_object_unref(ipatch_gig_reader->ipatch);

    ipatch_gig_reader->ipatch = NULL;
  }

  /* call parent */  
  G_OBJECT_CLASS(ags_ipatch_gig_reader_parent_class)->dispose(gobject);
}

void
ags_ipatch_gig_reader_finalize(GObject *gobject)
{
  AgsIpatchGigReader *ipatch_gig_reader;

  ipatch_gig_reader = AGS_IPATCH_GIG_READER(gobject);

  pthread_mutex_destroy(ipatch_gig_reader->obj_mutex);
  free(ipatch_gig_reader->obj_mutex);

  pthread_mutexattr_destroy(ipatch_gig_reader->obj_mutexattr);
  free(ipatch_gig_reader->obj_mutexattr);

  if(ipatch_gig_reader->ipatch != NULL){
    g_object_unref(ipatch_gig_reader->ipatch);
  }

  /* call parent */  
  G_OBJECT_CLASS(ags_ipatch_gig_reader_parent_class)->finalize(gobject);
}


AgsUUID*
ags_ipatch_gig_reader_get_uuid(AgsConnectable *connectable)
{
  AgsIpatchGigReader *ipatch_gig_reader;
  
  AgsUUID *ptr;

  pthread_mutex_t *ipatch_gig_reader_mutex;

  ipatch_gig_reader = AGS_IPATCH_GIG_READER(connectable);

  /* get audio file mutex */
  pthread_mutex_lock(ags_ipatch_gig_reader_get_class_mutex());
  
  ipatch_gig_reader_mutex = ipatch_gig_reader->obj_mutex;
  
  pthread_mutex_unlock(ags_ipatch_gig_reader_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(ipatch_gig_reader_mutex);

  ptr = ipatch_gig_reader->uuid;

  pthread_mutex_unlock(ipatch_gig_reader_mutex);
  
  return(ptr);
}

gboolean
ags_ipatch_gig_reader_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_ipatch_gig_reader_is_ready(AgsConnectable *connectable)
{
  AgsIpatchGigReader *ipatch_gig_reader;
  
  gboolean is_ready;

  pthread_mutex_t *ipatch_gig_reader_mutex;

  ipatch_gig_reader = AGS_IPATCH_GIG_READER(connectable);

  /* get audio file mutex */
  pthread_mutex_lock(ags_ipatch_gig_reader_get_class_mutex());
  
  ipatch_gig_reader_mutex = ipatch_gig_reader->obj_mutex;
  
  pthread_mutex_unlock(ags_ipatch_gig_reader_get_class_mutex());

  /* check is ready */
  pthread_mutex_lock(ipatch_gig_reader_mutex);
  
  is_ready = (((AGS_IPATCH_GIG_READER_ADDED_TO_REGISTRY & (ipatch_gig_reader->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(ipatch_gig_reader_mutex);

  return(is_ready);
}

void
ags_ipatch_gig_reader_add_to_registry(AgsConnectable *connectable)
{
  AgsIpatchGigReader *ipatch_gig_reader;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }

  ipatch_gig_reader = AGS_IPATCH_GIG_READER(connectable);

  ags_ipatch_gig_reader_set_flags(ipatch_gig_reader, AGS_IPATCH_GIG_READER_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(&(entry->entry),
		       (gpointer) ipatch_gig_reader);
    ags_registry_add_entry(registry,
			   entry);
  }  
}

void
ags_ipatch_gig_reader_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_ipatch_gig_reader_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_ipatch_gig_reader_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_ipatch_gig_reader_xml_parse(AgsConnectable *connectable,
			      xmlNode *node)
{
  //TODO:JK: implement me  
}

gboolean
ags_ipatch_gig_reader_is_connected(AgsConnectable *connectable)
{
  AgsIpatchGigReader *ipatch_gig_reader;
  
  gboolean is_connected;

  pthread_mutex_t *ipatch_gig_reader_mutex;

  ipatch_gig_reader = AGS_IPATCH_GIG_READER(connectable);

  /* get audio file mutex */
  pthread_mutex_lock(ags_ipatch_gig_reader_get_class_mutex());
  
  ipatch_gig_reader_mutex = ipatch_gig_reader->obj_mutex;
  
  pthread_mutex_unlock(ags_ipatch_gig_reader_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(ipatch_gig_reader_mutex);

  is_connected = (((AGS_IPATCH_GIG_READER_CONNECTED & (ipatch_gig_reader->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(ipatch_gig_reader_mutex);

  return(is_connected);
}

void
ags_ipatch_gig_reader_connect(AgsConnectable *connectable)
{
  AgsIpatchGigReader *ipatch_gig_reader;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ipatch_gig_reader = AGS_IPATCH_GIG_READER(connectable);
  
  ags_ipatch_gig_reader_set_flags(ipatch_gig_reader, AGS_IPATCH_GIG_READER_CONNECTED);
}

void
ags_ipatch_gig_reader_disconnect(AgsConnectable *connectable)
{
  AgsIpatchGigReader *ipatch_gig_reader;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ipatch_gig_reader = AGS_IPATCH_GIG_READER(connectable);

  ags_ipatch_gig_reader_unset_flags(ipatch_gig_reader, AGS_IPATCH_GIG_READER_CONNECTED);
}

/**
 * ags_ipatch_gig_reader_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.36
 */
pthread_mutex_t*
ags_ipatch_gig_reader_get_class_mutex()
{
  return(&ags_ipatch_gig_reader_class_mutex);
}

/**
 * ags_ipatch_gig_reader_test_flags:
 * @ipatch_gig_reader: the #AgsIpatchGigReader
 * @flags: the flags
 *
 * Test @flags to be set on @ipatch_gig_reader.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.36
 */
gboolean
ags_ipatch_gig_reader_test_flags(AgsIpatchGigReader *ipatch_gig_reader, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *ipatch_gig_reader_mutex;

  if(!AGS_IS_IPATCH_GIG_READER(ipatch_gig_reader)){
    return(FALSE);
  }

  /* get ipatch_gig_reader mutex */
  pthread_mutex_lock(ags_ipatch_gig_reader_get_class_mutex());
  
  ipatch_gig_reader_mutex = ipatch_gig_reader->obj_mutex;
  
  pthread_mutex_unlock(ags_ipatch_gig_reader_get_class_mutex());

  /* test */
  pthread_mutex_lock(ipatch_gig_reader_mutex);

  retval = (flags & (ipatch_gig_reader->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(ipatch_gig_reader_mutex);

  return(retval);
}

/**
 * ags_ipatch_gig_reader_set_flags:
 * @ipatch_gig_reader: the #AgsIpatchGigReader
 * @flags: see #AgsIpatchGigReaderFlags-enum
 *
 * Enable a feature of @ipatch_gig_reader.
 *
 * Since: 2.0.36
 */
void
ags_ipatch_gig_reader_set_flags(AgsIpatchGigReader *ipatch_gig_reader, guint flags)
{
  pthread_mutex_t *ipatch_gig_reader_mutex;

  if(!AGS_IS_IPATCH_GIG_READER(ipatch_gig_reader)){
    return;
  }

  /* get ipatch_gig_reader mutex */
  pthread_mutex_lock(ags_ipatch_gig_reader_get_class_mutex());
  
  ipatch_gig_reader_mutex = ipatch_gig_reader->obj_mutex;
  
  pthread_mutex_unlock(ags_ipatch_gig_reader_get_class_mutex());

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(ipatch_gig_reader_mutex);

  ipatch_gig_reader->flags |= flags;
  
  pthread_mutex_unlock(ipatch_gig_reader_mutex);
}
    
/**
 * ags_ipatch_gig_reader_unset_flags:
 * @ipatch_gig_reader: the #AgsIpatchGigReader
 * @flags: see #AgsIpatchGigReaderFlags-enum
 *
 * Disable a feature of @ipatch_gig_reader.
 *
 * Since: 2.0.36
 */
void
ags_ipatch_gig_reader_unset_flags(AgsIpatchGigReader *ipatch_gig_reader, guint flags)
{  
  pthread_mutex_t *ipatch_gig_reader_mutex;

  if(!AGS_IS_IPATCH_GIG_READER(ipatch_gig_reader)){
    return;
  }

  /* get ipatch_gig_reader mutex */
  pthread_mutex_lock(ags_ipatch_gig_reader_get_class_mutex());
  
  ipatch_gig_reader_mutex = ipatch_gig_reader->obj_mutex;
  
  pthread_mutex_unlock(ags_ipatch_gig_reader_get_class_mutex());

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(ipatch_gig_reader_mutex);

  ipatch_gig_reader->flags &= (~flags);
  
  pthread_mutex_unlock(ipatch_gig_reader_mutex);
}

gboolean
ags_ipatch_gig_reader_load(AgsIpatchGigReader *ipatch_gig_reader,
			   IpatchFileHandle *handle)
{
  //TODO:JK: implement me

  return(FALSE);
}

gboolean
ags_ipatch_gig_reader_select_instrument(AgsIpatchGigReader *ipatch_gig_reader,
					guint instrument_index)
{
  //TODO:JK: implement me

  return(FALSE);
}

gboolean
ags_ipatch_gig_reader_select_sample(AgsIpatchGigReader *ipatch_gig_reader,
				    guint sample_index)
{
  //TODO:JK: implement me

  return(FALSE);
}

gchar**
ags_ipatch_gig_reader_get_instrument_all(AgsIpatchGigReader *ipatch_gig_reader)
{
  //TODO:JK: implement me

  return(NULL);
}

gchar**
ags_ipatch_gig_reader_get_sample_all(AgsIpatchGigReader *ipatch_gig_reader)
{
  //TODO:JK: implement me

  return(NULL);
}

gchar**
ags_ipatch_gig_reader_get_sample_by_instrument_index(AgsIpatchGigReader *ipatch_gig_reader,
						     guint instrument_index)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_ipatch_gig_reader_new:
 * @ipatch: the #AgsIpatch
 *
 * Creates a new instance of #AgsIpatchGigReader.
 *
 * Returns: the new #AgsIpatchGigReader.
 *
 * Since: 2.0.0
 */
AgsIpatchGigReader*
ags_ipatch_gig_reader_new(AgsIpatch *ipatch)
{
  AgsIpatchGigReader *ipatch_gig_reader;

  ipatch_gig_reader = (AgsIpatchGigReader *) g_object_new(AGS_TYPE_IPATCH_GIG_READER,
							  "ipatch", ipatch,
							  NULL);

  return(ipatch_gig_reader);
}
