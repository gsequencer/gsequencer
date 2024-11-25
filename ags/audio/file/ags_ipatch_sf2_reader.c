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

#include <ags/audio/file/ags_ipatch_sf2_reader.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_ipatch_sf2_reader_class_init(AgsIpatchSF2ReaderClass *ipatch_sf2_reader);
void ags_ipatch_sf2_reader_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ipatch_sf2_reader_init(AgsIpatchSF2Reader *ipatch_sf2_reader);
void ags_ipatch_sf2_reader_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_ipatch_sf2_reader_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_ipatch_sf2_reader_dispose(GObject *gobject);
void ags_ipatch_sf2_reader_finalize(GObject *gobject);

AgsUUID* ags_ipatch_sf2_reader_get_uuid(AgsConnectable *connectable);
gboolean ags_ipatch_sf2_reader_has_resource(AgsConnectable *connectable);
gboolean ags_ipatch_sf2_reader_is_ready(AgsConnectable *connectable);
void ags_ipatch_sf2_reader_add_to_registry(AgsConnectable *connectable);
void ags_ipatch_sf2_reader_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_ipatch_sf2_reader_list_resource(AgsConnectable *connectable);
xmlNode* ags_ipatch_sf2_reader_xml_compose(AgsConnectable *connectable);
void ags_ipatch_sf2_reader_xml_parse(AgsConnectable *connectable,
				   xmlNode *node);
gboolean ags_ipatch_sf2_reader_is_connected(AgsConnectable *connectable);
void ags_ipatch_sf2_reader_connect(AgsConnectable *connectable);
void ags_ipatch_sf2_reader_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_ipatch_sf2_reader
 * @short_description: interfacing Soundfont2 related API of libinstpatch
 * @title: AgsIpatchSF2Reader
 * @section_id:
 * @include: ags/audio/file/ags_ipatch_sf2_reader.h
 *
 * #AgsIpatchSF2Reader is the base object to ineract with Soundfont2 related API.
 */

static gpointer ags_ipatch_sf2_reader_parent_class = NULL;

enum{
  PROP_0,
  PROP_IPATCH,
};

GType
ags_ipatch_sf2_reader_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_ipatch_sf2_reader = 0;

    static const GTypeInfo ags_ipatch_sf2_reader_info = {
      sizeof(AgsIpatchSF2ReaderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ipatch_sf2_reader_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsIpatchSF2Reader),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ipatch_sf2_reader_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ipatch_sf2_reader_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ipatch_sf2_reader = g_type_register_static(G_TYPE_OBJECT,
							"AgsIpatchSF2Reader",
							&ags_ipatch_sf2_reader_info,
							0);

    g_type_add_interface_static(ags_type_ipatch_sf2_reader,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_ipatch_sf2_reader);
  }

  return g_define_type_id__static;
}

void
ags_ipatch_sf2_reader_class_init(AgsIpatchSF2ReaderClass *ipatch_sf2_reader)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_ipatch_sf2_reader_parent_class = g_type_class_peek_parent(ipatch_sf2_reader);

  gobject = (GObjectClass *) ipatch_sf2_reader;

  gobject->set_property = ags_ipatch_sf2_reader_set_property;
  gobject->get_property = ags_ipatch_sf2_reader_get_property;

  gobject->dispose = ags_ipatch_sf2_reader_dispose;
  gobject->finalize = ags_ipatch_sf2_reader_finalize;

  /* properties */
  /**
   * AgsIpatchSF2Reader:ipatch:
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
ags_ipatch_sf2_reader_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_ipatch_sf2_reader_get_uuid;
  connectable->has_resource = ags_ipatch_sf2_reader_has_resource;
  connectable->is_ready = ags_ipatch_sf2_reader_is_ready;

  connectable->add_to_registry = ags_ipatch_sf2_reader_add_to_registry;
  connectable->remove_from_registry = ags_ipatch_sf2_reader_remove_from_registry;

  connectable->list_resource = ags_ipatch_sf2_reader_list_resource;
  connectable->xml_compose = ags_ipatch_sf2_reader_xml_compose;
  connectable->xml_parse = ags_ipatch_sf2_reader_xml_parse;

  connectable->is_connected = ags_ipatch_sf2_reader_is_connected;
  
  connectable->connect = ags_ipatch_sf2_reader_connect;
  connectable->disconnect = ags_ipatch_sf2_reader_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_ipatch_sf2_reader_init(AgsIpatchSF2Reader *ipatch_sf2_reader)
{
  guint i;

  ipatch_sf2_reader->flags = 0;
  ipatch_sf2_reader->connectable_flags = 0;

  /* add audio file mutex */
  g_rec_mutex_init(&(ipatch_sf2_reader->obj_mutex));

  /* uuid */
  ipatch_sf2_reader->uuid = ags_uuid_alloc();
  ags_uuid_generate(ipatch_sf2_reader->uuid);
  
  ipatch_sf2_reader->level = 0;

  ipatch_sf2_reader->ipatch = NULL;
  
  /* selected */
  ipatch_sf2_reader->index_selected = (guint *) malloc(4 * sizeof(guint));
  memset(ipatch_sf2_reader->index_selected, 0, 4 * sizeof(guint));
  
  ipatch_sf2_reader->name_selected = (gchar **) malloc(5 * sizeof(gchar *));

  for(i = 0; i < 5; i++){
    ipatch_sf2_reader->name_selected[i] = NULL;
  }

  /* reader */
  ipatch_sf2_reader->reader = NULL;
  ipatch_sf2_reader->sf2 = NULL;
  
  ipatch_sf2_reader->preset = NULL;
  ipatch_sf2_reader->instrument = NULL;
  ipatch_sf2_reader->sample = NULL;

  ipatch_sf2_reader->error = NULL;
}

void
ags_ipatch_sf2_reader_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  GRecMutex *ipatch_sf2_reader_mutex;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(gobject);

  /* get ipatch sample mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);

  switch(prop_id){
  case PROP_IPATCH:
    {
      AgsIpatch *ipatch;

      ipatch = (AgsIpatch *) g_value_get_object(value);

      g_rec_mutex_lock(ipatch_sf2_reader_mutex);

      if(ipatch_sf2_reader->ipatch == ipatch){
	g_rec_mutex_unlock(ipatch_sf2_reader_mutex);

	return;
      }
      
      if(ipatch_sf2_reader->ipatch != NULL){
	g_object_unref(ipatch_sf2_reader->ipatch);
      }

      if(ipatch != NULL){
	g_object_ref(ipatch);
      }

      ipatch_sf2_reader->ipatch = ipatch;

      g_rec_mutex_unlock(ipatch_sf2_reader_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ipatch_sf2_reader_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  GRecMutex *ipatch_sf2_reader_mutex;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(gobject);

  /* get ipatch sample mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);

  switch(prop_id){
  case PROP_IPATCH:
    {
      g_rec_mutex_lock(ipatch_sf2_reader_mutex);

      g_value_set_object(value, ipatch_sf2_reader->ipatch);

      g_rec_mutex_unlock(ipatch_sf2_reader_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ipatch_sf2_reader_dispose(GObject *gobject)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(gobject);

  if(ipatch_sf2_reader->ipatch != NULL){
    g_object_unref(ipatch_sf2_reader->ipatch);

    ipatch_sf2_reader->ipatch = NULL;
  }

  /* call parent */  
  G_OBJECT_CLASS(ags_ipatch_sf2_reader_parent_class)->dispose(gobject);
}

void
ags_ipatch_sf2_reader_finalize(GObject *gobject)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(gobject);

  if(ipatch_sf2_reader->ipatch != NULL){
    g_object_unref(ipatch_sf2_reader->ipatch);
  }

  /* call parent */  
  G_OBJECT_CLASS(ags_ipatch_sf2_reader_parent_class)->finalize(gobject);
}

AgsUUID*
ags_ipatch_sf2_reader_get_uuid(AgsConnectable *connectable)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;
  
  AgsUUID *ptr;

  GRecMutex *ipatch_sf2_reader_mutex;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(connectable);

  /* get audio file mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);

  /* get UUID */
  g_rec_mutex_lock(ipatch_sf2_reader_mutex);

  ptr = ipatch_sf2_reader->uuid;

  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);
  
  return(ptr);
}

gboolean
ags_ipatch_sf2_reader_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_ipatch_sf2_reader_is_ready(AgsConnectable *connectable)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;
  
  gboolean is_ready;

  GRecMutex *ipatch_sf2_reader_mutex;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(connectable);

  /* get ipatch_sf2_reader mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);

  /* check is ready */
  g_rec_mutex_lock(ipatch_sf2_reader_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (ipatch_sf2_reader->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);
  
  return(is_ready);
}

void
ags_ipatch_sf2_reader_add_to_registry(AgsConnectable *connectable)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  GRecMutex *ipatch_sf2_reader_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(connectable);

  /* get ipatch sf2 reader mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);

  g_rec_mutex_lock(ipatch_sf2_reader_mutex);

  ipatch_sf2_reader->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(entry->entry,
		       (gpointer) ipatch_sf2_reader);
    ags_registry_add_entry(registry,
			   entry);
  }  
}

void
ags_ipatch_sf2_reader_remove_from_registry(AgsConnectable *connectable)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  GRecMutex *ipatch_sf2_reader_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(connectable);

  /* get ipatch sf2 reader mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);

  g_rec_mutex_lock(ipatch_sf2_reader_mutex);

  ipatch_sf2_reader->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);

  //TODO:JK: implement me
}

xmlNode*
ags_ipatch_sf2_reader_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_ipatch_sf2_reader_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_ipatch_sf2_reader_xml_parse(AgsConnectable *connectable,
			      xmlNode *node)
{
  //TODO:JK: implement me  
}

gboolean
ags_ipatch_sf2_reader_is_connected(AgsConnectable *connectable)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;
  
  gboolean is_connected;

  GRecMutex *ipatch_sf2_reader_mutex;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(connectable);

  /* get ipatch sf2 reader mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);

  /* check is connected */
  g_rec_mutex_lock(ipatch_sf2_reader_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (ipatch_sf2_reader->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);
  
  return(is_connected);
}

void
ags_ipatch_sf2_reader_connect(AgsConnectable *connectable)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  GRecMutex *ipatch_sf2_reader_mutex;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(connectable);

  /* get ipatch sf2 reader mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);

  g_rec_mutex_lock(ipatch_sf2_reader_mutex);

  ipatch_sf2_reader->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);
}

void
ags_ipatch_sf2_reader_disconnect(AgsConnectable *connectable)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  GRecMutex *ipatch_sf2_reader_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(connectable);

  /* get ipatch sf2 reader mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);

  g_rec_mutex_lock(ipatch_sf2_reader_mutex);

  ipatch_sf2_reader->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);
}

/**
 * ags_ipatch_sf2_reader_load:
 * @ipatch_sf2_reader: the #AgsSF2Reader
 * @handle: the #IpatchFileHandle
 * 
 * Load Soundfont2 file.
 * 
 * Returns: %TRUE on success, else %FALSE on failure
 * 
 * Since: 3.0.0
 */
gboolean
ags_ipatch_sf2_reader_load(AgsIpatchSF2Reader *ipatch_sf2_reader,
			   IpatchFileHandle *handle)
{
  IpatchSF2Reader *reader;

  GError *error;
  
  GRecMutex *ipatch_sf2_reader_mutex;

  if(!AGS_IS_IPATCH_SF2_READER(ipatch_sf2_reader)){
    return(FALSE);
  }

  /* get ipatch sf2 reader mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);

  g_rec_mutex_lock(ipatch_sf2_reader_mutex);

  reader = 
    ipatch_sf2_reader->reader = ipatch_sf2_reader_new(handle);

  error = NULL;
  ipatch_sf2_reader->base = (IpatchBase *) ipatch_sf2_reader_load(reader,
								  &error);

  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);

  if(error != NULL){
    g_warning("%s", error->message);

    g_rec_mutex_lock(ipatch_sf2_reader_mutex);

    ipatch_sf2_reader->error = error;
    
    g_rec_mutex_unlock(ipatch_sf2_reader_mutex);
        
    return(FALSE);
  }

  g_rec_mutex_lock(ipatch_sf2_reader_mutex);
  
  error = NULL;
  ipatch_sf2_reader->sf2 = (IpatchSF2 *) ipatch_convert_object_to_type((GObject *) handle->file,
								       IPATCH_TYPE_SF2,
								       &error);

  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);

  if(error != NULL){
    g_warning("%s", error->message);
    
    g_rec_mutex_lock(ipatch_sf2_reader_mutex);

    ipatch_sf2_reader->error = error;
    
    g_rec_mutex_unlock(ipatch_sf2_reader_mutex);
    
    return(FALSE);
  }
  
  return(TRUE);
}

/**
 * ags_ipatch_sf2_reader_select_preset:
 * @ipatch_sf2_reader: the #AgsIpatchSF2Reader
 * @preset_index: the preset index
 * 
 * Select preset.
 * 
 * Returns: %TRUE on success, else %FALSE on failure
 * 
 * Since: 3.0.0
 */
gboolean
ags_ipatch_sf2_reader_select_preset(AgsIpatchSF2Reader *ipatch_sf2_reader,
				    guint preset_index)
{
  IpatchSF2 *sf2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;

  IpatchIter preset_iter;
  
  gboolean success;
  
  GRecMutex *ipatch_sf2_reader_mutex;

  if(!AGS_IS_IPATCH_SF2_READER(ipatch_sf2_reader)){
    return(FALSE);
  }

  /* get ipatch sf2 reader mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);

  success = FALSE;

  g_rec_mutex_lock(ipatch_sf2_reader_mutex);

  sf2 = ipatch_sf2_reader->sf2;
  
  ipatch_list = ipatch_container_get_children((IpatchContainer *) sf2, IPATCH_TYPE_SF2_PRESET);

  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &preset_iter);
    
    if(preset_index < ipatch_iter_count(&preset_iter)){
      success = TRUE;

      ipatch_iter_index(&preset_iter, preset_index);
      ipatch_item = ipatch_iter_get(&preset_iter);

      /* selected index and name */
      ipatch_sf2_reader->index_selected[AGS_SF2_PHDR] = preset_index;

      ipatch_sf2_reader->index_selected[AGS_SF2_IHDR] = 0;

      ipatch_sf2_reader->index_selected[AGS_SF2_SHDR] = 0;

      ipatch_sf2_reader->name_selected[AGS_SF2_PHDR] = g_strdup(ipatch_sf2_preset_get_name(IPATCH_SF2_PRESET(ipatch_item)));

      g_free(ipatch_sf2_reader->name_selected[AGS_SF2_IHDR]);
      ipatch_sf2_reader->name_selected[AGS_SF2_IHDR] = NULL;
      
      g_free(ipatch_sf2_reader->name_selected[AGS_SF2_SHDR]);
      ipatch_sf2_reader->name_selected[AGS_SF2_SHDR] = NULL;

      /* container */
      ipatch_sf2_reader->preset = (IpatchContainer *) ipatch_item;
      
      ipatch_sf2_reader->instrument = NULL;
      ipatch_sf2_reader->sample = NULL;
    }
  }
  
  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);
  
  return(success);
}

/**
 * ags_ipatch_sf2_reader_select_instrument:
 * @ipatch_sf2_reader: the #AgsIpatchSF2Reader
 * @instrument_index: the instrument index
 * 
 * Select instrument.
 * 
 * Returns: %TRUE on success, else %FALSE on failure
 * 
 * Since: 3.0.0
 */
gboolean
ags_ipatch_sf2_reader_select_instrument(AgsIpatchSF2Reader *ipatch_sf2_reader,
					guint instrument_index)
{
  IpatchSF2 *sf2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;

  IpatchIter instrument_iter;
  
  gboolean success;

  GRecMutex *ipatch_sf2_reader_mutex;

  if(!AGS_IS_IPATCH_SF2_READER(ipatch_sf2_reader)){
    return(FALSE);
  }

  /* get ipatch sf2 reader mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);

  success = FALSE;

  g_rec_mutex_lock(ipatch_sf2_reader_mutex);

  sf2 = ipatch_sf2_reader->sf2;

  if(ipatch_sf2_reader->preset == NULL){
    g_rec_mutex_unlock(ipatch_sf2_reader_mutex);

    return(FALSE);
  }

  ipatch_list = ipatch_sf2_preset_get_zones(ipatch_sf2_reader->preset);

  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &instrument_iter);
    
    if(instrument_index < ipatch_iter_count(&instrument_iter)){
      success = TRUE;

      ipatch_iter_index(&instrument_iter, instrument_index);
      ipatch_item = (IpatchItem *) ipatch_sf2_pzone_get_inst(ipatch_iter_get(&instrument_iter));

      /* selected index and name */
      ipatch_sf2_reader->index_selected[AGS_SF2_IHDR] = instrument_index;

      ipatch_sf2_reader->index_selected[AGS_SF2_SHDR] = 0;

      g_free(ipatch_sf2_reader->name_selected[AGS_SF2_IHDR]);
      ipatch_sf2_reader->name_selected[AGS_SF2_IHDR] = g_strdup(ipatch_sf2_inst_get_name(IPATCH_SF2_INST(ipatch_item)));
      
      g_free(ipatch_sf2_reader->name_selected[AGS_SF2_SHDR]);
      ipatch_sf2_reader->name_selected[AGS_SF2_SHDR] = NULL;

      /* container */
      ipatch_sf2_reader->instrument = (IpatchContainer *) ipatch_item;
      
      ipatch_sf2_reader->sample = NULL;
    }
  }
  
  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);
  
  return(success);
}

/**
 * ags_ipatch_sf2_reader_select_sample:
 * @ipatch_sf2_reader: the #AgsIpatchSF2Reader
 * @sample_index: the sample index
 * 
 * Select sample.
 * 
 * Returns: %TRUE on success, else %FALSE on failure
 * 
 * Since: 3.0.0
 */
gboolean
ags_ipatch_sf2_reader_select_sample(AgsIpatchSF2Reader *ipatch_sf2_reader,
				    guint sample_index)
{
  IpatchSF2 *sf2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;

  IpatchIter sample_iter;
  
  gboolean success;

  GRecMutex *ipatch_sf2_reader_mutex;

  if(!AGS_IS_IPATCH_SF2_READER(ipatch_sf2_reader)){
    return(FALSE);
  }

  /* get ipatch sf2 reader mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);

  success = FALSE;

  g_rec_mutex_lock(ipatch_sf2_reader_mutex);

  sf2 = ipatch_sf2_reader->sf2;

  if(ipatch_sf2_reader->instrument == NULL){
    g_rec_mutex_unlock(ipatch_sf2_reader_mutex);
  
    return(FALSE);
  }
  
  ipatch_list = ipatch_sf2_inst_get_zones(ipatch_sf2_reader->instrument);

  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &sample_iter);
    
    if(sample_index < ipatch_iter_count(&sample_iter)){
      success = TRUE;

      ipatch_iter_index(&sample_iter, sample_index);
      ipatch_item = (IpatchItem *) ipatch_sf2_izone_get_sample(ipatch_iter_get(&sample_iter));

      /* selected index and name */
      ipatch_sf2_reader->index_selected[AGS_SF2_SHDR] = sample_index;

      g_free(ipatch_sf2_reader->name_selected[AGS_SF2_SHDR]);
      ipatch_sf2_reader->name_selected[AGS_SF2_SHDR] = g_strdup(ipatch_sf2_sample_get_name((IpatchSF2Sample *) ipatch_item));

      /* container */
      ipatch_sf2_reader->sample = (IpatchContainer *) ipatch_item;
      g_object_ref(ipatch_sf2_reader->sample);
    }
  }
  
  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);
  
  return(success);
}

/**
 * ags_ipatch_sf2_reader_get_preset_all:
 * @ipatch_sf2_reader: the #AgsIpatchSF2Reader
 * 
 * Get all preset names.
 * 
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer full): the string vector cotaining preset names or %NULL on failure
 * 
 * Since: 3.0.0
 */
gchar**
ags_ipatch_sf2_reader_get_preset_all(AgsIpatchSF2Reader *ipatch_sf2_reader)
{
  IpatchSF2 *sf2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter preset_iter;
  
  gchar **preset;
  
  guint i, i_stop;

  GRecMutex *ipatch_sf2_reader_mutex;
  
  if(!AGS_IS_IPATCH_SF2_READER(ipatch_sf2_reader)){
    return(NULL);
  }

  /* get ipatch sf2 reader mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);
  
  preset = NULL;

  g_rec_mutex_lock(ipatch_sf2_reader_mutex);

  sf2 = ipatch_sf2_reader->sf2;

  if(sf2 == NULL){
    g_rec_mutex_unlock(ipatch_sf2_reader_mutex);

    return(NULL);
  }
  
  ipatch_list = ipatch_container_get_children((IpatchContainer *) sf2, IPATCH_TYPE_SF2_PRESET);
  
  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &preset_iter);

    i_stop = ipatch_iter_count(&preset_iter);

    if(i_stop > 0){
      preset = (gchar **) malloc((i_stop + 1) * sizeof(gchar *));

      ipatch_iter_first(&preset_iter);
      
      for(i = 0; i < i_stop; i++){
	ipatch_item = ipatch_iter_get(&preset_iter);
	preset[i] = g_strdup(ipatch_sf2_preset_get_name(IPATCH_SF2_PRESET(ipatch_item)));

	/* iterate */
	ipatch_iter_next(&preset_iter);
      }

      preset[i] = NULL;
    }
  }
  
  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);
  
  return(preset);
}

/**
 * ags_ipatch_sf2_reader_get_instrument_all:
 * @ipatch_sf2_reader: the #AgsIpatchSF2Reader
 * 
 * Get all instrument names.
 * 
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer full): the string vector cotaining instrument names or %NULL on failure
 * 
 * Since: 3.0.0
 */
gchar**
ags_ipatch_sf2_reader_get_instrument_all(AgsIpatchSF2Reader *ipatch_sf2_reader)
{
  IpatchSF2 *sf2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter instrument_iter;
  
  gchar **instrument;
  
  guint i, i_stop;
  
  GRecMutex *ipatch_sf2_reader_mutex;

  if(!AGS_IS_IPATCH_SF2_READER(ipatch_sf2_reader)){
    return(NULL);
  }

  /* get ipatch sf2 reader mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);
  
  instrument = NULL;

  g_rec_mutex_lock(ipatch_sf2_reader_mutex);

  sf2 = ipatch_sf2_reader->sf2;

  if(sf2 == NULL){
    g_rec_mutex_unlock(ipatch_sf2_reader_mutex);

    return(NULL);
  }
  
  ipatch_list = ipatch_container_get_children((IpatchContainer *) sf2, IPATCH_TYPE_SF2_INST);
  
  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &instrument_iter);

    i_stop = ipatch_iter_count(&instrument_iter);

    if(i_stop > 0){
      instrument = (gchar **) malloc((i_stop + 1) * sizeof(gchar *));

      ipatch_iter_first(&instrument_iter);
      
      for(i = 0; i < i_stop; i++){
	ipatch_item = ipatch_iter_get(&instrument_iter);
	instrument[i] = g_strdup(ipatch_sf2_inst_get_name(IPATCH_SF2_INST(ipatch_item)));

	/* iterate */
	ipatch_iter_next(&instrument_iter);
      }

      instrument[i] = NULL;
    }
  }
  
  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);
  
  return(instrument);
}

/**
 * ags_ipatch_sf2_reader_get_sample_all:
 * @ipatch_sf2_reader: the #AgsIpatchSF2Reader
 * 
 * Get all sample names.
 * 
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer full): the string vector cotaining sample names or %NULL on failure
 * 
 * Since: 3.0.0
 */
gchar**
ags_ipatch_sf2_reader_get_sample_all(AgsIpatchSF2Reader *ipatch_sf2_reader)
{
  IpatchSF2 *sf2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter sample_iter;
  
  gchar **sample;
  
  guint i, i_stop;

  GRecMutex *ipatch_sf2_reader_mutex;
  
  if(!AGS_IS_IPATCH_SF2_READER(ipatch_sf2_reader)){
    return(NULL);
  }
  
  /* get ipatch sf2 reader mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);

  sample = NULL;

  g_rec_mutex_lock(ipatch_sf2_reader_mutex);

  sf2 = ipatch_sf2_reader->sf2;

  if(sf2 == NULL){
    g_rec_mutex_unlock(ipatch_sf2_reader_mutex);

    return(NULL);
  }
  
  ipatch_list = ipatch_container_get_children((IpatchContainer *) sf2, IPATCH_TYPE_SF2_SAMPLE);
  
  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &sample_iter);

    i_stop = ipatch_iter_count(&sample_iter);

    if(i_stop > 0){
      sample = (gchar **) malloc((i_stop + 1) * sizeof(gchar *));

      ipatch_iter_first(&sample_iter);
      
      for(i = 0; i < i_stop; i++){
	ipatch_item = ipatch_iter_get(&sample_iter);
	sample[i] = g_strdup(ipatch_sf2_sample_get_name(IPATCH_SF2_SAMPLE(ipatch_item)));

	/* iterate */
	ipatch_iter_next(&sample_iter);
      }

      sample[i] = NULL;
    }
  }
  
  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);
  
  return(sample);
}

/**
 * ags_ipatch_sf2_reader_get_instrument_by_preset_index:
 * @ipatch_sf2_reader: the #AgsIpatchSF2Reader
 * @preset_index: the preset index
 * 
 * Get instrument by preset index.
 * 
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer full): the string vector cotaining instrument names or %NULL on failure
 * 
 * Since: 3.0.0
 */
gchar**
ags_ipatch_sf2_reader_get_instrument_by_preset_index(AgsIpatchSF2Reader *ipatch_sf2_reader,
						     guint preset_index)
{
  IpatchSF2 *sf2;
  IpatchContainer *preset;  
  IpatchContainer *instrument;
  
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter preset_iter, instrument_iter, sample_iter;

  gchar **instrument_strv;

  guint i, i_stop;
  guint j, j_stop;
  guint k, k_stop;
  guint count;

  GRecMutex *ipatch_sf2_reader_mutex;

  if(!AGS_IS_IPATCH_SF2_READER(ipatch_sf2_reader)){
    return(NULL);
  }

  /* get ipatch sf2 reader mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);
  
  instrument_strv = NULL;
  
  g_rec_mutex_lock(ipatch_sf2_reader_mutex);

  sf2 = ipatch_sf2_reader->sf2;

  if(sf2 == NULL){
    g_rec_mutex_unlock(ipatch_sf2_reader_mutex);

    return(NULL);
  }
  
  /* presets */
  ipatch_list = ipatch_container_get_children((IpatchContainer *) sf2, IPATCH_TYPE_SF2_PRESET);
  
  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &preset_iter);

    i_stop = ipatch_iter_count(&preset_iter);

    if(i_stop > 0 && preset_index < i_stop){
      ipatch_iter_first(&preset_iter);
      ipatch_iter_index(&preset_iter, preset_index);

      preset = (IpatchContainer *) ipatch_iter_get(&preset_iter);

      /* instruments */
      ipatch_list = ipatch_sf2_preset_get_zones(preset);

      if(ipatch_list != NULL){
	ipatch_list_init_iter(ipatch_list, &instrument_iter);

	j_stop = ipatch_iter_count(&instrument_iter);
	ipatch_iter_first(&instrument_iter);

	if(j_stop > 0){
	  instrument_strv = (gchar **) malloc((j_stop + 1) * sizeof(gchar *));
	    
	  for(j = 0, count = 0; j < j_stop; j++){
	    instrument = (IpatchContainer *) ipatch_sf2_pzone_get_inst(ipatch_iter_get(&instrument_iter));
	    instrument_strv[count] = g_strdup(ipatch_sf2_inst_get_name((IpatchSF2Inst *) instrument));

	    /* iterate */
	    ipatch_iter_next(&instrument_iter);
	    count++;
	  }

	  instrument_strv[count] = NULL;
	}
      }
    }
  }
  
  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);

  return(instrument_strv);
}

/**
 * ags_ipatch_sf2_reader_get_sample_by_preset_index:
 * @ipatch_sf2_reader: the #AgsIpatchSF2Reader
 * @preset_index: the preset index
 * 
 * Get sample by preset index.
 * 
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer full): the string vector cotaining sample names or %NULL on failure
 * 
 * Since: 3.0.0
 */
gchar**
ags_ipatch_sf2_reader_get_sample_by_preset_index(AgsIpatchSF2Reader *ipatch_sf2_reader,
						 guint preset_index)
{
  IpatchSF2 *sf2;
  IpatchContainer *preset;  
  IpatchContainer *instrument;
  
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter preset_iter, instrument_iter, sample_iter;

  gchar **sample_strv;

  guint i, i_stop;
  guint j, j_stop;
  guint k, k_stop;
  guint count;

  GRecMutex *ipatch_sf2_reader_mutex;

  if(!AGS_IS_IPATCH_SF2_READER(ipatch_sf2_reader)){
    return(NULL);
  }

  /* get ipatch sf2 reader mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);
  
  sample_strv = NULL;
  
  g_rec_mutex_lock(ipatch_sf2_reader_mutex);

  sf2 = ipatch_sf2_reader->sf2;

  if(sf2 == NULL){
    g_rec_mutex_unlock(ipatch_sf2_reader_mutex);

    return(NULL);
  }
  
  /* presets */
  ipatch_list = ipatch_container_get_children((IpatchContainer *) sf2, IPATCH_TYPE_SF2_PRESET);
  
  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &preset_iter);

    i_stop = ipatch_iter_count(&preset_iter);

    if(i_stop > 0){
      ipatch_iter_index(&preset_iter, preset_index);

      preset = (IpatchContainer *) ipatch_iter_get(&preset_iter);

      /* instruments */
      ipatch_list = ipatch_sf2_preset_get_zones(preset);
      
      if(ipatch_list != NULL){
	ipatch_list_init_iter(ipatch_list, &instrument_iter);

	j_stop = ipatch_iter_count(&instrument_iter);
	
	if(j_stop > 0){
	  for(j = 0, count = 0; j < j_stop; j++){
	    instrument = (IpatchContainer *) ipatch_sf2_pzone_get_inst(ipatch_iter_get(&instrument_iter));
	    
	    /* samples */
	    ipatch_list = ipatch_sf2_inst_get_zones(preset);

	    if(ipatch_list != NULL){
	      ipatch_list_init_iter(ipatch_list, &sample_iter);

	      k_stop = ipatch_iter_count(&sample_iter);

	      if(k_stop > 0){
		if(sample_strv == NULL){
		  sample_strv = (gchar **) malloc((k_stop + 1) * sizeof(gchar *));
		}else{
		  sample_strv = (gchar **) realloc(sample_strv,
						   (count + k_stop + 1) * sizeof(gchar *));
		}
	    
		for(k = 0; k < k_stop; k++){
		  ipatch_item = (IpatchItem *) ipatch_sf2_izone_get_sample(ipatch_iter_get(&sample_iter));
		  sample_strv[count] = g_strdup(ipatch_sf2_sample_get_name((IpatchSF2Sample *) ipatch_item));
		    
		  /* iterate */
		  ipatch_iter_next(&sample_iter);
		  count++;
		}
	      }
	    }

	    /* iterate */
	    ipatch_iter_next(&instrument_iter);
	  }
	}
      }

      if(sample_strv != NULL){
	sample_strv[count] = NULL;
      }
    }
  }

  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);

  return(sample_strv);
}

/**
 * ags_ipatch_sf2_reader_get_sample_by_preset_and_instrument_index:
 * @ipatch_sf2_reader: the #AgsIpatchSF2Reader
 * @preset_index: the preset index
 * @instrument_index: the instrument index
 * 
 * Get sample by preset and instrument index.
 * 
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer full): the string vector cotaining sample names or %NULL on failure
 * 
 * Since: 3.0.0
 */
gchar**
ags_ipatch_sf2_reader_get_sample_by_preset_and_instrument_index(AgsIpatchSF2Reader *ipatch_sf2_reader,
								guint preset_index, guint instrument_index)
{
  IpatchSF2 *sf2;
  IpatchContainer *preset;  
  IpatchContainer *instrument;
  
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter preset_iter, instrument_iter, sample_iter;

  gchar **sample;

  guint i, i_stop;
  guint j, j_stop;
  guint k, k_stop;
  guint count;

  GRecMutex *ipatch_sf2_reader_mutex;

  if(!AGS_IS_IPATCH_SF2_READER(ipatch_sf2_reader)){
    return(NULL);
  }
  
  /* get ipatch sf2 reader mutex */
  ipatch_sf2_reader_mutex = AGS_IPATCH_SF2_READER_GET_OBJ_MUTEX(ipatch_sf2_reader);

  sample = NULL;
  
  g_rec_mutex_lock(ipatch_sf2_reader_mutex);

  sf2 = ipatch_sf2_reader->sf2;

  if(sf2 == NULL){
    g_rec_mutex_unlock(ipatch_sf2_reader_mutex);

    return(NULL);
  }
  
  /* presets */
  ipatch_list = ipatch_container_get_children((IpatchContainer *) sf2, IPATCH_TYPE_SF2_PRESET);
  
  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &preset_iter);
    
    i_stop = ipatch_iter_count(&preset_iter);

    if(i_stop > 0 && preset_index < i_stop){
      ipatch_iter_index(&preset_iter, preset_index);

      preset = (IpatchContainer *) ipatch_iter_get(&preset_iter);

      /* instruments */
      ipatch_list = ipatch_sf2_preset_get_zones(preset);
      
      if(ipatch_list != NULL){
	ipatch_list_init_iter(ipatch_list, &instrument_iter);

	j_stop = ipatch_iter_count(&instrument_iter);

	if(j_stop > 0 && instrument_index < j_stop){	  
	  ipatch_iter_index(&instrument_iter, instrument_index);

	  instrument = (IpatchContainer *) ipatch_sf2_pzone_get_inst(ipatch_iter_get(&instrument_iter));

	  /* samples */
	  ipatch_list = ipatch_sf2_inst_get_zones(instrument);

	  if(ipatch_list != NULL){
	    ipatch_list_init_iter(ipatch_list, &sample_iter);

	    k_stop = ipatch_iter_count(&sample_iter);

	    if(k_stop > 0){
	      sample = (gchar **) malloc((k_stop + 1) * sizeof(gchar *));
	    
	      for(k = 0, count = 0; k < k_stop; k++){
		ipatch_item = (IpatchItem *) ipatch_sf2_izone_get_sample(ipatch_iter_get(&sample_iter));
		sample[count] = g_strdup(ipatch_sf2_sample_get_name((IpatchSF2Sample *) ipatch_item));
		    
		/* iterate */
		ipatch_iter_next(&sample_iter);
		count++;
	      }

	      sample[count] = NULL;
	    }
	  }
	}
      }
    }
  }

  g_rec_mutex_unlock(ipatch_sf2_reader_mutex);

  return(sample);
}

/**
 * ags_ipatch_sf2_reader_new:
 * @ipatch: the #AgsIpatch
 *
 * Creates a new instance of #AgsIpatchSF2Reader.
 *
 * Returns: the new #AgsIpatchSF2Reader.
 *
 * Since: 3.0.0
 */
AgsIpatchSF2Reader*
ags_ipatch_sf2_reader_new(AgsIpatch *ipatch)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = (AgsIpatchSF2Reader *) g_object_new(AGS_TYPE_IPATCH_SF2_READER,
							  "ipatch", ipatch,
							  NULL);

  return(ipatch_sf2_reader);
}
