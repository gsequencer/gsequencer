/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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
 * @short_description: interfacing DLS2 related API of libinstpatch
 * @title: AgsIpatchDLS2Reader
 * @section_id:
 * @include: ags/audio/file/ags_ipatch_dls2_reader.h
 *
 * #AgsIpatchDLS2Reader is the base object to ineract with DLS2 related API.
 */

static gpointer ags_ipatch_dls2_reader_parent_class = NULL;

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
   * Since: 3.0.0
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

  ipatch_dls2_reader->flags = 0;
  ipatch_dls2_reader->connectable_flags = 0;

  /* add audio file mutex */
  g_rec_mutex_init(&(ipatch_dls2_reader->obj_mutex));

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

  GRecMutex *ipatch_dls2_reader_mutex;

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(gobject);

  /* get ipatch sample mutex */
  ipatch_dls2_reader_mutex = AGS_IPATCH_DLS2_READER_GET_OBJ_MUTEX(ipatch_dls2_reader);

  switch(prop_id){
  case PROP_IPATCH:
    {
      AgsIpatch *ipatch;

      ipatch = (AgsIpatch *) g_value_get_object(value);

      g_rec_mutex_lock(ipatch_dls2_reader_mutex);

      if(ipatch_dls2_reader->ipatch == ipatch){
	g_rec_mutex_unlock(ipatch_dls2_reader_mutex);
      
	return;
      }
      
      if(ipatch_dls2_reader->ipatch != NULL){
	g_object_unref(ipatch_dls2_reader->ipatch);
      }

      if(ipatch != NULL){
	g_object_ref(ipatch);
      }

      ipatch_dls2_reader->ipatch = ipatch;

      g_rec_mutex_unlock(ipatch_dls2_reader_mutex);
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

  GRecMutex *ipatch_dls2_reader_mutex;

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(gobject);

  /* get ipatch sample mutex */
  ipatch_dls2_reader_mutex = AGS_IPATCH_DLS2_READER_GET_OBJ_MUTEX(ipatch_dls2_reader);

  switch(prop_id){
  case PROP_IPATCH:
    {
      g_rec_mutex_lock(ipatch_dls2_reader_mutex);

      g_value_set_object(value, ipatch_dls2_reader->ipatch);

      g_rec_mutex_unlock(ipatch_dls2_reader_mutex);
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

  GRecMutex *ipatch_dls2_reader_mutex;

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(connectable);

  /* get audio file mutex */
  ipatch_dls2_reader_mutex = AGS_IPATCH_DLS2_READER_GET_OBJ_MUTEX(ipatch_dls2_reader);

  /* get UUID */
  g_rec_mutex_lock(ipatch_dls2_reader_mutex);

  ptr = ipatch_dls2_reader->uuid;

  g_rec_mutex_unlock(ipatch_dls2_reader_mutex);
  
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

  GRecMutex *ipatch_dls2_reader_mutex;

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(connectable);

  /* get ipatch_dls2_reader mutex */
  ipatch_dls2_reader_mutex = AGS_IPATCH_DLS2_READER_GET_OBJ_MUTEX(ipatch_dls2_reader);

  /* check is ready */
  g_rec_mutex_lock(ipatch_dls2_reader_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (ipatch_dls2_reader->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(ipatch_dls2_reader_mutex);
  
  return(is_ready);
}

void
ags_ipatch_dls2_reader_add_to_registry(AgsConnectable *connectable)
{
  AgsIpatchDLS2Reader *ipatch_dls2_reader;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  GRecMutex *ipatch_dls2_reader_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(connectable);

  /* get ipatch dls2 reader mutex */
  ipatch_dls2_reader_mutex = AGS_IPATCH_DLS2_READER_GET_OBJ_MUTEX(ipatch_dls2_reader);

  g_rec_mutex_lock(ipatch_dls2_reader_mutex);

  ipatch_dls2_reader->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(ipatch_dls2_reader_mutex);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(entry->entry,
		       (gpointer) ipatch_dls2_reader);
    ags_registry_add_entry(registry,
			   entry);
  }  
}

void
ags_ipatch_dls2_reader_remove_from_registry(AgsConnectable *connectable)
{
  AgsIpatchDLS2Reader *ipatch_dls2_reader;

  GRecMutex *ipatch_dls2_reader_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(connectable);

  /* get ipatch dls2 reader mutex */
  ipatch_dls2_reader_mutex = AGS_IPATCH_DLS2_READER_GET_OBJ_MUTEX(ipatch_dls2_reader);

  g_rec_mutex_lock(ipatch_dls2_reader_mutex);

  ipatch_dls2_reader->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(ipatch_dls2_reader_mutex);

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

  GRecMutex *ipatch_dls2_reader_mutex;

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(connectable);

  /* get ipatch dls2 reader mutex */
  ipatch_dls2_reader_mutex = AGS_IPATCH_DLS2_READER_GET_OBJ_MUTEX(ipatch_dls2_reader);

  /* check is connected */
  g_rec_mutex_lock(ipatch_dls2_reader_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (ipatch_dls2_reader->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(ipatch_dls2_reader_mutex);
  
  return(is_connected);
}

void
ags_ipatch_dls2_reader_connect(AgsConnectable *connectable)
{
  AgsIpatchDLS2Reader *ipatch_dls2_reader;

  GRecMutex *ipatch_dls2_reader_mutex;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(connectable);

  /* get ipatch dls2 reader mutex */
  ipatch_dls2_reader_mutex = AGS_IPATCH_DLS2_READER_GET_OBJ_MUTEX(ipatch_dls2_reader);

  g_rec_mutex_lock(ipatch_dls2_reader_mutex);

  ipatch_dls2_reader->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(ipatch_dls2_reader_mutex);
}

void
ags_ipatch_dls2_reader_disconnect(AgsConnectable *connectable)
{
  AgsIpatchDLS2Reader *ipatch_dls2_reader;

  GRecMutex *ipatch_dls2_reader_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ipatch_dls2_reader = AGS_IPATCH_DLS2_READER(connectable);

  /* get ipatch dls2 reader mutex */
  ipatch_dls2_reader_mutex = AGS_IPATCH_DLS2_READER_GET_OBJ_MUTEX(ipatch_dls2_reader);

  g_rec_mutex_lock(ipatch_dls2_reader_mutex);

  ipatch_dls2_reader->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(ipatch_dls2_reader_mutex);
}

/**
 * ags_ipatch_dls2_reader_load:
 * @ipatch_dls2_reader: the #AgsIpatchDLS2Reader
 * @handle: the #IpatchFileHandle
 * 
 * Load DLS2 file.
 * 
 * Returns: %TRUE on success, else %FALSE on failure
 * 
 * Since: 3.0.0
 */
gboolean
ags_ipatch_dls2_reader_load(AgsIpatchDLS2Reader *ipatch_dls2_reader,
			    IpatchFileHandle *handle)
{
  IpatchDLSReader *reader;

  GError *error;
  
  GRecMutex *ipatch_dls2_reader_mutex;

  if(!AGS_IS_IPATCH_DLS2_READER(ipatch_dls2_reader)){
    return(FALSE);
  }

  /* get ipatch dls2 reader mutex */
  ipatch_dls2_reader_mutex = AGS_IPATCH_DLS2_READER_GET_OBJ_MUTEX(ipatch_dls2_reader);

  g_rec_mutex_lock(ipatch_dls2_reader_mutex);

  reader = 
    ipatch_dls2_reader->reader = ipatch_dls_reader_new(handle);

  error = NULL;
  ipatch_dls2_reader->base = (IpatchBase *) ipatch_dls_reader_load(reader,
								   &error);

  g_rec_mutex_unlock(ipatch_dls2_reader_mutex);

  if(error != NULL){
    g_warning("%s", error->message);

    g_rec_mutex_lock(ipatch_dls2_reader_mutex);

    ipatch_dls2_reader->error = error;
    
    g_rec_mutex_unlock(ipatch_dls2_reader_mutex);
    
    return(FALSE);
  }

  g_rec_mutex_lock(ipatch_dls2_reader_mutex);
  
  error = NULL;
  ipatch_dls2_reader->dls2 = (IpatchDLS2 *) ipatch_convert_object_to_type((GObject *) handle->file,
									  IPATCH_TYPE_DLS2,
									  &error);

  g_rec_mutex_unlock(ipatch_dls2_reader_mutex);

  if(error != NULL){
    g_warning("%s", error->message);

    g_rec_mutex_lock(ipatch_dls2_reader_mutex);

    ipatch_dls2_reader->error = error;
    
    g_rec_mutex_unlock(ipatch_dls2_reader_mutex);
    
    return(FALSE);
  }

  return(TRUE);
}

/**
 * ags_ipatch_dls2_reader_select_instrument:
 * @ipatch_dls2_reader: the #AgsIpatchDLS2Reader
 * @instrument_index: the instrument index
 * 
 * Select instrument.
 * 
 * Returns: %TRUE on success, else %FALSE on failure
 * 
 * Since: 3.0.0
 */
gboolean
ags_ipatch_dls2_reader_select_instrument(AgsIpatchDLS2Reader *ipatch_dls2_reader,
					 guint instrument_index)
{
  IpatchDLS2 *dls2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;

  IpatchIter instrument_iter;
  
  gboolean success;

  GRecMutex *ipatch_dls2_reader_mutex;

  if(!AGS_IS_IPATCH_DLS2_READER(ipatch_dls2_reader)){
    return(FALSE);
  }

  /* get ipatch dls2 reader mutex */
  ipatch_dls2_reader_mutex = AGS_IPATCH_DLS2_READER_GET_OBJ_MUTEX(ipatch_dls2_reader);

  success = FALSE;

  g_rec_mutex_lock(ipatch_dls2_reader_mutex);

  dls2 = ipatch_dls2_reader->dls2;

  ipatch_list = ipatch_container_get_children((IpatchContainer *) dls2, IPATCH_TYPE_DLS2_INST);

  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &instrument_iter);
    
    if(instrument_index < ipatch_iter_count(&instrument_iter)){
      int bank, program;
	
      success = TRUE;

      ipatch_iter_index(&instrument_iter, instrument_index);
      ipatch_item = (IpatchItem *) ipatch_dls2_inst_get_regions(ipatch_iter_get(&instrument_iter));

      /* selected index and name */
      ipatch_dls2_reader->index_selected[AGS_DLS2_IHDR] = instrument_index;

      ipatch_dls2_reader->index_selected[AGS_DLS2_SHDR] = 0;

      g_free(ipatch_dls2_reader->name_selected[AGS_DLS2_IHDR]);
      ipatch_dls2_inst_get_midi_locale((IpatchDLS2Inst *) ipatch_item,
				       &bank, &program);
      ipatch_dls2_reader->name_selected[AGS_DLS2_IHDR] = g_strdup_printf("bank=%d; program=%d;", bank, program);
      
      g_free(ipatch_dls2_reader->name_selected[AGS_DLS2_SHDR]);
      ipatch_dls2_reader->name_selected[AGS_DLS2_SHDR] = NULL;

      /* container */
      ipatch_dls2_reader->instrument = (IpatchContainer *) ipatch_item;
      
      ipatch_dls2_reader->sample = NULL;
    }
  }
  
  g_rec_mutex_unlock(ipatch_dls2_reader_mutex);

  return(success);
}

/**
 * ags_ipatch_dls2_reader_select_sample:
 * @ipatch_dls2_reader: the #AgsIpatchDLS2Reader
 * @sample_index: the sample index
 * 
 * Select sample.
 * 
 * Returns: %TRUE on success, else %FALSE on failure
 * 
 * Since: 3.0.0
 */
gboolean
ags_ipatch_dls2_reader_select_sample(AgsIpatchDLS2Reader *ipatch_dls2_reader,
				     guint sample_index)
{
  IpatchDLS2 *dls2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;

  IpatchIter sample_iter;
  
  gboolean success;

  GRecMutex *ipatch_dls2_reader_mutex;

  if(!AGS_IS_IPATCH_DLS2_READER(ipatch_dls2_reader)){
    return(FALSE);
  }

  /* get ipatch dls2 reader mutex */
  ipatch_dls2_reader_mutex = AGS_IPATCH_DLS2_READER_GET_OBJ_MUTEX(ipatch_dls2_reader);

  success = FALSE;

  g_rec_mutex_lock(ipatch_dls2_reader_mutex);

  dls2 = ipatch_dls2_reader->dls2;

  ipatch_list = ipatch_dls2_inst_get_regions(ipatch_dls2_reader->instrument);

  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &sample_iter);
    
    if(sample_index < ipatch_iter_count(&sample_iter)){
      success = TRUE;

      ipatch_iter_index(&sample_iter, sample_index);
      ipatch_item = (IpatchItem *) ipatch_dls2_region_get_sample(ipatch_iter_get(&sample_iter));

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
      ipatch_dls2_reader->sample = (IpatchContainer *) ipatch_item;
    }
  }
  
  g_rec_mutex_unlock(ipatch_dls2_reader_mutex);
  
  return(success);
}

/**
 * ags_ipatch_dls2_reader_get_instrument_all:
 * @ipatch_dls2_reader: the #AgsIpatchDLS2Reader
 * 
 * Get instrument all.
 * 
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer full): the string vector containing instrument
 * 
 * Since: 3.0.0
 */
gchar**
ags_ipatch_dls2_reader_get_instrument_all(AgsIpatchDLS2Reader *ipatch_dls2_reader)
{
  IpatchDLS2 *dls2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter instrument_iter;
  
  gchar **instrument;
  
  guint i, i_stop;
  
  GRecMutex *ipatch_dls2_reader_mutex;

  if(!AGS_IS_IPATCH_DLS2_READER(ipatch_dls2_reader)){
    return(NULL);
  }

  /* get ipatch dls2 reader mutex */
  ipatch_dls2_reader_mutex = AGS_IPATCH_DLS2_READER_GET_OBJ_MUTEX(ipatch_dls2_reader);
  
  instrument = NULL;

  g_rec_mutex_lock(ipatch_dls2_reader_mutex);

  dls2 = ipatch_dls2_reader->dls2;
  
  ipatch_list = ipatch_container_get_children((IpatchContainer *) dls2, IPATCH_TYPE_DLS2_INST);
  
  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &instrument_iter);

    i_stop = ipatch_iter_count(&instrument_iter);

    if(i_stop > 0){
      instrument = (gchar **) malloc((i_stop + 1) * sizeof(gchar *));

      ipatch_iter_first(&instrument_iter);
      
      for(i = 0; i < i_stop; i++){
	int bank, program;
	
	ipatch_item = ipatch_iter_get(&instrument_iter);

	ipatch_dls2_inst_get_midi_locale((IpatchDLS2Inst *) ipatch_item,
					 &bank, &program);
	instrument[i] = g_strdup_printf("bank=%d; program=%d;", bank, program);

	/* iterate */
	ipatch_iter_next(&instrument_iter);
      }

      instrument[i] = NULL;
    }
  }
  
  g_rec_mutex_unlock(ipatch_dls2_reader_mutex);
    
  return(instrument);
}

/**
 * ags_ipatch_dls2_reader_get_sample_all:
 * @ipatch_dls2_reader: the #AgsIpatchDLS2Reader
 * 
 * Get sample all.
 * 
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer full): the string vector containing sample
 * 
 * Since: 3.0.0
 */
gchar**
ags_ipatch_dls2_reader_get_sample_all(AgsIpatchDLS2Reader *ipatch_dls2_reader)
{
  IpatchDLS2 *dls2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter sample_iter;
  
  gchar **sample;
  
  guint i, i_stop;

  GRecMutex *ipatch_dls2_reader_mutex;
  
  if(!AGS_IS_IPATCH_DLS2_READER(ipatch_dls2_reader)){
    return(NULL);
  }
  
  /* get ipatch dls2 reader mutex */
  ipatch_dls2_reader_mutex = AGS_IPATCH_DLS2_READER_GET_OBJ_MUTEX(ipatch_dls2_reader);
  
  sample = NULL;

  g_rec_mutex_lock(ipatch_dls2_reader_mutex);

  dls2 = ipatch_dls2_reader->dls2;
  
  ipatch_list = ipatch_container_get_children((IpatchContainer *) dls2, IPATCH_TYPE_DLS2_SAMPLE);
  
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
  
  g_rec_mutex_unlock(ipatch_dls2_reader_mutex);  
  
  return(sample);
}

/**
 * ags_ipatch_dls2_reader_get_sample_by_instrument_index:
 * @ipatch_dls2_reader: the #AgsIpatchDLS2Reader
 * @instrument_index: the instrument index
 * 
 * Get sample by instrument index.
 * 
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer full): the string vector containing sample
 * 
 * Since: 3.0.0
 */
gchar**
ags_ipatch_dls2_reader_get_sample_by_instrument_index(AgsIpatchDLS2Reader *ipatch_dls2_reader,
						      guint instrument_index)
{
  IpatchDLS2 *dls2;
  IpatchContainer *instrument;
  
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter instrument_iter, sample_iter;
 
  gchar **sample;

  guint i, i_stop;
  guint j, j_stop;
  guint count;

  GRecMutex *ipatch_dls2_reader_mutex;

  if(!AGS_IS_IPATCH_DLS2_READER(ipatch_dls2_reader)){
    return(NULL);
  }

  /* get ipatch dls2 reader mutex */
  ipatch_dls2_reader_mutex = AGS_IPATCH_DLS2_READER_GET_OBJ_MUTEX(ipatch_dls2_reader);
  
  sample = NULL;
  
  g_rec_mutex_lock(ipatch_dls2_reader_mutex);
  
  dls2 = ipatch_dls2_reader->dls2;
  
  /* instruments */
  ipatch_list = ipatch_container_get_children((IpatchContainer *) dls2, IPATCH_TYPE_DLS2_INST);
  
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
	    ipatch_item = (IpatchItem *) ipatch_dls2_region_get_sample(ipatch_iter_get(&sample_iter));

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

  g_rec_mutex_unlock(ipatch_dls2_reader_mutex);

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
 * Since: 3.0.0
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
