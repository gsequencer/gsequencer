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

#include <ags/audio/file/ags_ipatch_dls2_reader.h>

#include <ags/libags.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_ipatch_dls2_reader_class_init(AgsIpatchDLS2ReaderClass *ipatch_dls2_reader);
void ags_ipatch_dls2_reader_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ipatch_dls2_reader_init(AgsIpatchDLS2Reader *ipatch_dls2_reader);
void ags_ipatch_dls2_reader_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_ipatch_dls2_reader_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_ipatch_dls2_reader_dispose(GObject *gobject);
void ags_ipatch_dls2_reader_finalize(GObject *gobject);

AgsUUID* ags_ipatch_dls2_reader_get_uuid(AgsConnectable *connectable);
gboolean ags_ipatch_dls2_reader_has_resource(AgsConnectable *connectable);
gboolean ags_ipatch_dls2_reader_is_ready(AgsConnectable *connectable);
void ags_ipatch_dls2_reader_add_to_registry(AgsConnectable *connectable);
void ags_ipatch_dls2_reader_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_ipatch_dls2_reader_list_resource(AgsConnectable *connectable);
xmlNode* ags_ipatch_dls2_reader_xml_compose(AgsConnectable *connectable);
void ags_ipatch_dls2_reader_xml_parse(AgsConnectable *connectable,
				   xmlNode *node);
gboolean ags_ipatch_dls2_reader_is_connected(AgsConnectable *connectable);
void ags_ipatch_dls2_reader_connect(AgsConnectable *connectable);
void ags_ipatch_dls2_reader_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_ipatch_dls2_reader
 * @short_description: interfacing Soundfont2 related API of libinstpatch
 * @title: AgsIpatchDLS2Reader
 * @section_id:
 * @include: ags/audio/file/ags_ipatch_dls2_reader.h
 *
 * #AgsIpatchDLS2Reader is the base object to ineract with Soundfont2 related API.
 */

static gpointer ags_ipatch_dls2_reader_parent_class = NULL;

static pthread_mutex_t ags_ipatch_dls2_reader_class_mutex = PTHREAD_MUTEX_INITIALIZER;

enum{
  PROP_0,
  PROP_IPATCH,
};

GType
ags_ipatch_dls2_reader_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ipatch_dls2_reader = 0;

    static const GTypeInfo ags_ipatch_dls2_reader_info = {
      sizeof(AgsIpatchDLS2ReaderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ipatch_dls2_reader_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsIpatchDLS2Reader),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ipatch_dls2_reader_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ipatch_dls2_reader_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ipatch_dls2_reader = g_type_register_static(G_TYPE_OBJECT,
							 "AgsIpatchDLS2Reader",
							 &ags_ipatch_dls2_reader_info,
							 0);

    g_type_add_interface_static(ags_type_ipatch_dls2_reader,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ipatch_dls2_reader);
  }

  return g_define_type_id__volatile;
}

void
ags_ipatch_dls2_reader_class_init(AgsIpatchDLS2ReaderClass *ipatch_dls2_reader)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_ipatch_dls2_reader_parent_class = g_type_class_peek_parent(ipatch_dls2_reader);

  gobject = (GObjectClass *) ipatch_dls2_reader;

  gobject->set_property = ags_ipatch_dls2_reader_set_property;
  gobject->get_property = ags_ipatch_dls2_reader_get_property;

  gobject->dispose = ags_ipatch_dls2_reader_dispose;
  gobject->finalize = ags_ipatch_dls2_reader_finalize;

  /* properties */
  /**
   * AgsIpatchDLS2Reader:ipatch:
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
ags_ipatch_dls2_reader_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_ipatch_dls2_reader_get_uuid;
  connectable->has_resource = ags_ipatch_dls2_reader_has_resource;
  connectable->is_ready = ags_ipatch_dls2_reader_is_ready;

  connectable->add_to_registry = ags_ipatch_dls2_reader_add_to_registry;
  connectable->remove_from_registry = ags_ipatch_dls2_reader_remove_from_registry;

  connectable->list_resource = ags_ipatch_dls2_reader_list_resource;
  connectable->xml_compose = ags_ipatch_dls2_reader_xml_compose;
  connectable->xml_parse = ags_ipatch_dls2_reader_xml_parse;

  connectable->is_connected = ags_ipatch_dls2_reader_is_connected;
  
  connectable->connect = ags_ipatch_dls2_reader_connect;
  connectable->disconnect = ags_ipatch_dls2_reader_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_ipatch_dls2_reader_init(AgsIpatchDLS2Reader *ipatch_dls2_reader)
{
  guint i;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  ipatch_dls2_reader->flags = 0;

  /* add audio file mutex */
  ipatch_dls2_reader->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  ipatch_dls2_reader->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);  

  /* uuid */
  ipatch_dls2_reader->uuid = ags_uuid_alloc();
  ags_uuid_generate(ipatch_dls2_reader->uuid);

  ipatch_dls2_reader->level = 0;
  
  ipatch_dls2_reader->ipatch = NULL;
  
  /* selected */
  ipatch_dls2_reader->index_selected = (guint *) malloc(3 * sizeof(guint));
  memset(ipatch_dls2_reader->index_selected, 0, 3 * sizeof(guint));
  
  ipatch_dls2_reader->name_selected = (gchar **) malloc(4 * sizeof(gchar *));

  for(i = 0; i < 4; i++){
    ipatch_dls2_reader->name_selected[i] = NULL;
  }
  
  /* reader */
  ipatch_dls2_reader->reader = NULL;

  ipatch_dls2_reader->dls2 = NULL;
  ipatch_dls2_reader->base = NULL;

  ipatch_dls2_reader->instrument = NULL;
  ipatch_dls2_reader->sample = NULL;

  ipatch_dls2_reader->error = NULL;
}

void
ags_ipatch_dls2_reader_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsIpatchDLS2Reader *ipatch_dls2_reader;

  pthread_mutex_t *ipatch_dls2_reader_mutex;

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(gobject);

  /* get ipatch sample mutex */
  pthread_mutex_lock(ags_ipatch_dls2_reader_get_class_mutex());
  
  ipatch_dls2_reader_mutex = ipatch_dls2_reader->obj_mutex;
  
  pthread_mutex_unlock(ags_ipatch_dls2_reader_get_class_mutex());

  switch(prop_id){
  case PROP_IPATCH:
    {
      AgsIpatch *ipatch;

      ipatch = (AgsIpatch *) g_value_get_object(value);

      pthread_mutex_lock(ipatch_dls2_reader_mutex);

      if(ipatch_dls2_reader->ipatch == ipatch){
	pthread_mutex_unlock(ipatch_dls2_reader_mutex);
      
	return;
      }
      
      if(ipatch_dls2_reader->ipatch != NULL){
	g_object_unref(ipatch_dls2_reader->ipatch);
      }

      if(ipatch != NULL){
	g_object_ref(ipatch);
      }

      ipatch_dls2_reader->ipatch = ipatch;

      pthread_mutex_unlock(ipatch_dls2_reader_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ipatch_dls2_reader_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsIpatchDLS2Reader *ipatch_dls2_reader;

  pthread_mutex_t *ipatch_dls2_reader_mutex;

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(gobject);

  /* get ipatch sample mutex */
  pthread_mutex_lock(ags_ipatch_dls2_reader_get_class_mutex());
  
  ipatch_dls2_reader_mutex = ipatch_dls2_reader->obj_mutex;
  
  pthread_mutex_unlock(ags_ipatch_dls2_reader_get_class_mutex());

  switch(prop_id){
  case PROP_IPATCH:
    {
      pthread_mutex_lock(ipatch_dls2_reader_mutex);

      g_value_set_object(value, ipatch_dls2_reader->ipatch);

      pthread_mutex_unlock(ipatch_dls2_reader_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ipatch_dls2_reader_dispose(GObject *gobject)
{
  AgsIpatchDLS2Reader *ipatch_dls2_reader;

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(gobject);

  if(ipatch_dls2_reader->ipatch != NULL){
    g_object_unref(ipatch_dls2_reader->ipatch);

    ipatch_dls2_reader->ipatch = NULL;
  }

  /* call parent */  
  G_OBJECT_CLASS(ags_ipatch_dls2_reader_parent_class)->dispose(gobject);
}

void
ags_ipatch_dls2_reader_finalize(GObject *gobject)
{
  AgsIpatchDLS2Reader *ipatch_dls2_reader;

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(gobject);

  pthread_mutex_destroy(ipatch_dls2_reader->obj_mutex);
  free(ipatch_dls2_reader->obj_mutex);

  pthread_mutexattr_destroy(ipatch_dls2_reader->obj_mutexattr);
  free(ipatch_dls2_reader->obj_mutexattr);

  if(ipatch_dls2_reader->ipatch != NULL){
    g_object_unref(ipatch_dls2_reader->ipatch);
  }

  /* call parent */  
  G_OBJECT_CLASS(ags_ipatch_dls2_reader_parent_class)->finalize(gobject);
}

AgsUUID*
ags_ipatch_dls2_reader_get_uuid(AgsConnectable *connectable)
{
  AgsIpatchDLS2Reader *ipatch_dls2_reader;
  
  AgsUUID *ptr;

  pthread_mutex_t *ipatch_dls2_reader_mutex;

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(connectable);

  /* get audio file mutex */
  pthread_mutex_lock(ags_ipatch_dls2_reader_get_class_mutex());
  
  ipatch_dls2_reader_mutex = ipatch_dls2_reader->obj_mutex;
  
  pthread_mutex_unlock(ags_ipatch_dls2_reader_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(ipatch_dls2_reader_mutex);

  ptr = ipatch_dls2_reader->uuid;

  pthread_mutex_unlock(ipatch_dls2_reader_mutex);
  
  return(ptr);
}

gboolean
ags_ipatch_dls2_reader_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_ipatch_dls2_reader_is_ready(AgsConnectable *connectable)
{
  AgsIpatchDLS2Reader *ipatch_dls2_reader;
  
  gboolean is_ready;

  pthread_mutex_t *ipatch_dls2_reader_mutex;

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(connectable);

  /* get audio file mutex */
  pthread_mutex_lock(ags_ipatch_dls2_reader_get_class_mutex());
  
  ipatch_dls2_reader_mutex = ipatch_dls2_reader->obj_mutex;
  
  pthread_mutex_unlock(ags_ipatch_dls2_reader_get_class_mutex());

  /* check is ready */
  pthread_mutex_lock(ipatch_dls2_reader_mutex);
  
  is_ready = (((AGS_IPATCH_DLS2_READER_ADDED_TO_REGISTRY & (ipatch_dls2_reader->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(ipatch_dls2_reader_mutex);

  return(is_ready);
}

void
ags_ipatch_dls2_reader_add_to_registry(AgsConnectable *connectable)
{
  AgsIpatchDLS2Reader *ipatch_dls2_reader;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(connectable);

  ags_ipatch_dls2_reader_set_flags(ipatch_dls2_reader, AGS_IPATCH_DLS2_READER_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(&(entry->entry),
		       (gpointer) ipatch_dls2_reader);
    ags_registry_add_entry(registry,
			   entry);
  }  
}

void
ags_ipatch_dls2_reader_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_ipatch_dls2_reader_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_ipatch_dls2_reader_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_ipatch_dls2_reader_xml_parse(AgsConnectable *connectable,
			      xmlNode *node)
{
  //TODO:JK: implement me  
}

gboolean
ags_ipatch_dls2_reader_is_connected(AgsConnectable *connectable)
{
  AgsIpatchDLS2Reader *ipatch_dls2_reader;
  
  gboolean is_connected;

  pthread_mutex_t *ipatch_dls2_reader_mutex;

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(connectable);

  /* get audio file mutex */
  pthread_mutex_lock(ags_ipatch_dls2_reader_get_class_mutex());
  
  ipatch_dls2_reader_mutex = ipatch_dls2_reader->obj_mutex;
  
  pthread_mutex_unlock(ags_ipatch_dls2_reader_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(ipatch_dls2_reader_mutex);

  is_connected = (((AGS_IPATCH_DLS2_READER_CONNECTED & (ipatch_dls2_reader->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(ipatch_dls2_reader_mutex);

  return(is_connected);
}

void
ags_ipatch_dls2_reader_connect(AgsConnectable *connectable)
{
  AgsIpatchDLS2Reader *ipatch_dls2_reader;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(connectable);
  
  ags_ipatch_dls2_reader_set_flags(ipatch_dls2_reader, AGS_IPATCH_DLS2_READER_CONNECTED);
}

void
ags_ipatch_dls2_reader_disconnect(AgsConnectable *connectable)
{
  AgsIpatchDLS2Reader *ipatch_dls2_reader;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(connectable);

  ags_ipatch_dls2_reader_unset_flags(ipatch_dls2_reader, AGS_IPATCH_DLS2_READER_CONNECTED);
}

/**
 * ags_ipatch_dls2_reader_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.36
 */
pthread_mutex_t*
ags_ipatch_dls2_reader_get_class_mutex()
{
  return(&ags_ipatch_dls2_reader_class_mutex);
}

/**
 * ags_ipatch_dls2_reader_test_flags:
 * @ipatch_dls2_reader: the #AgsIpatchDLS2Reader
 * @flags: the flags
 *
 * Test @flags to be set on @ipatch_dls2_reader.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.36
 */
gboolean
ags_ipatch_dls2_reader_test_flags(AgsIpatchDLS2Reader *ipatch_dls2_reader, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *ipatch_dls2_reader_mutex;

  if(!AGS_IS_IPATCH_DLS2_READER(ipatch_dls2_reader)){
    return(FALSE);
  }

  /* get ipatch_dls2_reader mutex */
  pthread_mutex_lock(ags_ipatch_dls2_reader_get_class_mutex());
  
  ipatch_dls2_reader_mutex = ipatch_dls2_reader->obj_mutex;
  
  pthread_mutex_unlock(ags_ipatch_dls2_reader_get_class_mutex());

  /* test */
  pthread_mutex_lock(ipatch_dls2_reader_mutex);

  retval = (flags & (ipatch_dls2_reader->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(ipatch_dls2_reader_mutex);

  return(retval);
}

/**
 * ags_ipatch_dls2_reader_set_flags:
 * @ipatch_dls2_reader: the #AgsIpatchDLS2Reader
 * @flags: see #AgsIpatchDLS2ReaderFlags-enum
 *
 * Enable a feature of @ipatch_dls2_reader.
 *
 * Since: 2.0.36
 */
void
ags_ipatch_dls2_reader_set_flags(AgsIpatchDLS2Reader *ipatch_dls2_reader, guint flags)
{
  pthread_mutex_t *ipatch_dls2_reader_mutex;

  if(!AGS_IS_IPATCH_DLS2_READER(ipatch_dls2_reader)){
    return;
  }

  /* get ipatch_dls2_reader mutex */
  pthread_mutex_lock(ags_ipatch_dls2_reader_get_class_mutex());
  
  ipatch_dls2_reader_mutex = ipatch_dls2_reader->obj_mutex;
  
  pthread_mutex_unlock(ags_ipatch_dls2_reader_get_class_mutex());

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(ipatch_dls2_reader_mutex);

  ipatch_dls2_reader->flags |= flags;
  
  pthread_mutex_unlock(ipatch_dls2_reader_mutex);
}
    
/**
 * ags_ipatch_dls2_reader_unset_flags:
 * @ipatch_dls2_reader: the #AgsIpatchDLS2Reader
 * @flags: see #AgsIpatchDLS2ReaderFlags-enum
 *
 * Disable a feature of @ipatch_dls2_reader.
 *
 * Since: 2.0.36
 */
void
ags_ipatch_dls2_reader_unset_flags(AgsIpatchDLS2Reader *ipatch_dls2_reader, guint flags)
{  
  pthread_mutex_t *ipatch_dls2_reader_mutex;

  if(!AGS_IS_IPATCH_DLS2_READER(ipatch_dls2_reader)){
    return;
  }

  /* get ipatch_dls2_reader mutex */
  pthread_mutex_lock(ags_ipatch_dls2_reader_get_class_mutex());
  
  ipatch_dls2_reader_mutex = ipatch_dls2_reader->obj_mutex;
  
  pthread_mutex_unlock(ags_ipatch_dls2_reader_get_class_mutex());

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(ipatch_dls2_reader_mutex);

  ipatch_dls2_reader->flags &= (~flags);
  
  pthread_mutex_unlock(ipatch_dls2_reader_mutex);
}

gboolean
ags_ipatch_dls2_reader_load(AgsIpatchDLS2Reader *ipatch_dls2_reader,
			    IpatchFileHandle *handle)
{
  if(!AGS_IS_IPATCH_DLS2_READER(ipatch_dls2_reader)){
    return(FALSE);
  }

  ipatch_dls2_reader->reader = ipatch_dls_reader_new(handle);

  ipatch_dls2_reader->error = NULL;
  ipatch_dls2_reader->base = (IpatchBase *) ipatch_dls_reader_load(ipatch_dls2_reader->reader,
								   &(ipatch_dls2_reader->error));

  if(ipatch_dls2_reader->error != NULL){
    g_warning("%s", ipatch_dls2_reader->error->message);
    
    return(FALSE);
  }
  
  ipatch_dls2_reader->error = NULL;
  ipatch_dls2_reader->dls2 = (IpatchDLS2 *) ipatch_convert_object_to_type((GObject *) handle->file,
									  IPATCH_TYPE_DLS2,
									  &(ipatch_dls2_reader->error));

  if(ipatch_dls2_reader->error != NULL){
    g_warning("%s", ipatch_dls2_reader->error->message);
    
    return(FALSE);
  }

  while(g_static_rec_mutex_unlock_full(((IpatchItem *) (ipatch_dls2_reader->base))->mutex) != 0);

  return(TRUE);
}

gboolean
ags_ipatch_dls2_reader_select_instrument(AgsIpatchDLS2Reader *ipatch_dls2_reader,
					 guint instrument_index)
{
#ifdef AGS_WITH_LIBINSTPATCH
  IpatchDLS2 *dls2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;

  IpatchIter instrument_iter;
#endif
  
  gboolean success;

  if(!AGS_IS_IPATCH_DLS2_READER(ipatch_dls2_reader)){
    return(FALSE);
  }

  success = FALSE;

#ifdef AGS_WITH_LIBINSTPATCH
  dls2 = ipatch_dls2_reader->dls2;

  ipatch_list = ipatch_container_get_children(dls2, IPATCH_TYPE_DLS2_INST);

  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &instrument_iter);
    
    if(instrument_index < ipatch_iter_count(&instrument_iter)){
      int bank, program;
	
      success = TRUE;

      ipatch_iter_index(&instrument_iter, instrument_index);
      ipatch_item = ipatch_dls2_inst_get_regions(ipatch_iter_get(&instrument_iter));

      /* selected index and name */
      ipatch_dls2_reader->index_selected[AGS_DLS2_IHDR] = instrument_index;

      ipatch_dls2_reader->index_selected[AGS_DLS2_SHDR] = 0;

      g_free(ipatch_dls2_reader->name_selected[AGS_DLS2_IHDR]);
      ipatch_dls2_inst_get_midi_locale(ipatch_item,
				       &bank, &program);
      ipatch_dls2_reader->name_selected[AGS_DLS2_IHDR] = g_strdup_printf("bank=%d; program=%d;", bank, program);
      
      g_free(ipatch_dls2_reader->name_selected[AGS_DLS2_SHDR]);
      ipatch_dls2_reader->name_selected[AGS_DLS2_SHDR] = NULL;

      /* container */
      ipatch_dls2_reader->instrument = ipatch_item;
      
      ipatch_dls2_reader->sample = NULL;
    }
  }
#endif
  
  return(success);
}

gboolean
ags_ipatch_dls2_reader_select_sample(AgsIpatchDLS2Reader *ipatch_dls2_reader,
				     guint sample_index)
{
#ifdef AGS_WITH_LIBINSTPATCH
  IpatchDLS2 *dls2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;

  IpatchIter sample_iter;
#endif
  
  gboolean success;

  if(!AGS_IS_IPATCH_DLS2_READER(ipatch_dls2_reader)){
    return(FALSE);
  }

  success = FALSE;

#ifdef AGS_WITH_LIBINSTPATCH
  dls2 = ipatch_dls2_reader->dls2;

  ipatch_list = ipatch_dls2_inst_get_regions(ipatch_dls2_reader->instrument);

  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &sample_iter);
    
    if(sample_index < ipatch_iter_count(&sample_iter)){
      success = TRUE;

      ipatch_iter_index(&sample_iter, sample_index);
      ipatch_item = ipatch_dls2_region_get_sample(ipatch_iter_get(&sample_iter));

      /* selected index and name */
      ipatch_dls2_reader->index_selected[AGS_DLS2_SHDR] = sample_index;

      g_free(ipatch_dls2_reader->name_selected[AGS_DLS2_SHDR]);

      if(IPATCH_DLS2_SAMPLE(ipatch_item)->dlid != NULL){
	ipatch_dls2_reader->name_selected[AGS_DLS2_SHDR] = g_strdup_printf("id=0x%x%x",
									   IPATCH_DLS2_SAMPLE(ipatch_item)->dlid[1],
									   IPATCH_DLS2_SAMPLE(ipatch_item)->dlid[0]);
      }else{
	ipatch_dls2_reader->name_selected[AGS_DLS2_SHDR] = g_strdup("(null)");
      }
      
      /* container */
      ipatch_dls2_reader->sample = ipatch_item;
    }
  }
#endif
  
  return(success);
}

gchar**
ags_ipatch_dls2_reader_get_instrument_all(AgsIpatchDLS2Reader *ipatch_dls2_reader)
{
#ifdef AGS_WITH_LIBINSTPATCH
  IpatchDLS2 *dls2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter instrument_iter;
#endif
  
  gchar **instrument;
  
  guint i, i_stop;
  
  if(!AGS_IS_IPATCH_DLS2_READER(ipatch_dls2_reader)){
    return(NULL);
  }
  
  instrument = NULL;

#ifdef AGS_WITH_LIBINSTPATCH
  dls2 = ipatch_dls2_reader->dls2;
  
  ipatch_list = ipatch_container_get_children(dls2, IPATCH_TYPE_DLS2_INST);
  
  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &instrument_iter);

    i_stop = ipatch_iter_count(&instrument_iter);

    if(i_stop > 0){
      instrument = (gchar **) malloc((i_stop + 1) * sizeof(gchar *));

      ipatch_iter_first(&instrument_iter);
      
      for(i = 0; i < i_stop; i++){
	int bank, program;
	
	ipatch_item = ipatch_iter_get(&instrument_iter);

	ipatch_dls2_inst_get_midi_locale(ipatch_item,
					 &bank, &program);
	instrument[i] = g_strdup_printf("bank=%d; program=%d;", bank, program);

	/* iterate */
	ipatch_iter_next(&instrument_iter);
      }

      instrument[i] = NULL;
    }
  }
  
  //FIXME:JK: still needed?
  while(g_static_rec_mutex_unlock_full(((IpatchItem *) (dls2))->mutex) != 0);
#endif
  
  return(instrument);
}

gchar**
ags_ipatch_dls2_reader_get_sample_all(AgsIpatchDLS2Reader *ipatch_dls2_reader)
{
#ifdef AGS_WITH_LIBINSTPATCH
  IpatchDLS2 *dls2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter sample_iter;
#endif
  
  gchar **sample;
  
  guint i, i_stop;
  
  if(!AGS_IS_IPATCH_DLS2_READER(ipatch_dls2_reader)){
    return(NULL);
  }
  
  sample = NULL;

#ifdef AGS_WITH_LIBINSTPATCH
  dls2 = ipatch_dls2_reader->dls2;
  
  ipatch_list = ipatch_container_get_children(dls2, IPATCH_TYPE_DLS2_SAMPLE);
  
  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &sample_iter);

    i_stop = ipatch_iter_count(&sample_iter);

    if(i_stop > 0){
      sample = (gchar **) malloc((i_stop + 1) * sizeof(gchar *));

      ipatch_iter_first(&sample_iter);
      
      for(i = 0; i < i_stop; i++){
	ipatch_item = ipatch_iter_get(&sample_iter);

	if(IPATCH_DLS2_SAMPLE(ipatch_item)->dlid != NULL){
	  sample[i] = g_strdup_printf("id=0x%x%x",
				      IPATCH_DLS2_SAMPLE(ipatch_item)->dlid[1],
				      IPATCH_DLS2_SAMPLE(ipatch_item)->dlid[0]);
	}else{
	  sample[i] = g_strdup("(null)");
	}

	/* iterate */
	ipatch_iter_next(&sample_iter);
      }

      sample[i] = NULL;
    }
  }
  
  //FIXME:JK: still needed?
  while(g_static_rec_mutex_unlock_full(((IpatchItem *) (dls2))->mutex) != 0);
#endif
  
  return(sample);
}

gchar**
ags_ipatch_dls2_reader_get_sample_by_instrument_index(AgsIpatchDLS2Reader *ipatch_dls2_reader,
						      guint instrument_index)
{
#ifdef AGS_WITH_LIBINSTPATCH
  IpatchDLS2 *dls2;
  IpatchContainer *instrument;
  
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter instrument_iter, sample_iter;
#endif

  gchar **sample;

  guint i, i_stop;
  guint j, j_stop;
  guint count;

  if(!AGS_IS_IPATCH_DLS2_READER(ipatch_dls2_reader)){
    return(NULL);
  }
  
  sample = NULL;
  
#ifdef AGS_WITH_LIBINSTPATCH
  dls2 = ipatch_dls2_reader->dls2;
  
  /* instruments */
  ipatch_list = ipatch_container_get_children(dls2, IPATCH_TYPE_DLS2_INST);
  
  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &instrument_iter);
    
    i_stop = ipatch_iter_count(&instrument_iter);

    if(i_stop > 0 && instrument_index < i_stop){
      ipatch_iter_index(&instrument_iter, instrument_index);

      instrument = ipatch_iter_get(&instrument_iter);

      /* samples */
      ipatch_list = ipatch_dls2_inst_get_regions(instrument);

      if(ipatch_list != NULL){
	ipatch_list_init_iter(ipatch_list, &sample_iter);

	j_stop = ipatch_iter_count(&sample_iter);

	if(j_stop > 0){
	  sample = (gchar **) malloc((j_stop + 1) * sizeof(gchar *));
	    
	  for(j = 0, count = 0; j < j_stop; j++){
	    ipatch_item = ipatch_dls2_region_get_sample(ipatch_iter_get(&sample_iter));

	    if(IPATCH_DLS2_SAMPLE(ipatch_item)->dlid != NULL){
	      sample[count] = g_strdup_printf("id=0x%x%x",
					  IPATCH_DLS2_SAMPLE(ipatch_item)->dlid[1],
					  IPATCH_DLS2_SAMPLE(ipatch_item)->dlid[0]);
	    }else{
	      sample[count] = g_strdup("(null)");
	    }
	    
	    /* iterate */
	    ipatch_iter_next(&sample_iter);
	    count++;
	  }

	  sample[count] = NULL;
	}
      }
    }
  }

  //FIXME:JK: still needed?
  while(g_static_rec_mutex_unlock_full(((IpatchItem *) (dls2))->mutex) != 0);
#endif

  return(sample);
}

/**
 * ags_ipatch_dls2_reader_new:
 * @ipatch: the #AgsIpatch
 *
 * Creates an #AgsIpatchDLS2Reader.
 *
 * Returns: an empty #AgsIpatchDLS2Reader.
 *
 * Since: 2.0.0
 */
AgsIpatchDLS2Reader*
ags_ipatch_dls2_reader_new(AgsIpatch *ipatch)
{
  AgsIpatchDLS2Reader *ipatch_dls2_reader;

  ipatch_dls2_reader = (AgsIpatchDLS2Reader *) g_object_new(AGS_TYPE_IPATCH_DLS2_READER,
							    "ipatch", ipatch,
							    NULL);

  return(ipatch_dls2_reader);
}
