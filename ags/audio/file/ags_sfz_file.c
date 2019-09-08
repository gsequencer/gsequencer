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

#include <ags/audio/file/ags_sfz_file.h>

#include <ags/libags.h>

#include <ags/audio/file/ags_sfz_group.h>
#include <ags/audio/file/ags_sfz_region.h>
#include <ags/audio/file/ags_sfz_sample.h>

#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>

#include <sys/stat.h>
#include <unistd.h>

#include <ags/i18n.h>

void ags_sfz_file_class_init(AgsSFZFileClass *sfz_file);
void ags_sfz_file_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sfz_file_sound_container_interface_init(AgsSoundContainerInterface *sound_container);
void ags_sfz_file_init(AgsSFZFile *sfz_file);
void ags_sfz_file_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec);
void ags_sfz_file_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec);
void ags_sfz_file_dispose(GObject *gobject);
void ags_sfz_file_finalize(GObject *gobject);

AgsUUID* ags_sfz_file_get_uuid(AgsConnectable *connectable);
gboolean ags_sfz_file_has_resource(AgsConnectable *connectable);
gboolean ags_sfz_file_is_ready(AgsConnectable *connectable);
void ags_sfz_file_add_to_registry(AgsConnectable *connectable);
void ags_sfz_file_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_sfz_file_list_resource(AgsConnectable *connectable);
xmlNode* ags_sfz_file_xml_compose(AgsConnectable *connectable);
void ags_sfz_file_xml_parse(AgsConnectable *connectable,
			    xmlNode *node);
gboolean ags_sfz_file_is_connected(AgsConnectable *connectable);
void ags_sfz_file_connect(AgsConnectable *connectable);
void ags_sfz_file_disconnect(AgsConnectable *connectable);

gboolean ags_sfz_file_open(AgsSoundContainer *sound_container, gchar *filename);
guint ags_sfz_file_get_level_count(AgsSoundContainer *sound_container);
guint ags_sfz_file_get_nesting_level(AgsSoundContainer *sound_container);
gchar* ags_sfz_file_get_level_id(AgsSoundContainer *sound_container);
guint ags_sfz_file_get_level_index(AgsSoundContainer *sound_container);
guint ags_sfz_file_level_up(AgsSoundContainer *sound_container,
			    guint level_count);
guint ags_sfz_file_select_level_by_id(AgsSoundContainer *sound_container,
				      gchar *level_id);
guint ags_sfz_file_select_level_by_index(AgsSoundContainer *sound_container,
					 guint level_index);
gchar** ags_sfz_file_get_sublevel_name(AgsSoundContainer *sound_container);
GList* ags_sfz_file_get_resource_all(AgsSoundContainer *sound_container);
GList* ags_sfz_file_get_resource_by_name(AgsSoundContainer *sound_container,
					 gchar *resource_name);
GList* ags_sfz_file_get_resource_by_index(AgsSoundContainer *sound_container,
					  guint resource_index);
GList* ags_sfz_file_get_resource_current(AgsSoundContainer *sound_container);
void ags_sfz_file_close(AgsSoundContainer *sound_container);

/**
 * SECTION:ags_sfz_file
 * @short_description: SFZ file
 * @title: AgsSFZFile
 * @section_id:
 * @include: ags/audio/file/ags_sfz_file.h
 *
 * #AgsSFZFile is the base object to ineract with SFZ files.
 */
 
enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_FILENAME,
  PROP_MODE,
  PROP_GROUP,
  PROP_REGION,
  PROP_SAMPLE,
};

static gpointer ags_sfz_file_parent_class = NULL;

static pthread_mutex_t ags_sfz_file_class_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t regex_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_sfz_file_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sfz_file = 0;

    static const GTypeInfo ags_sfz_file_info = {
      sizeof (AgsSFZFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sfz_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSFZFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sfz_file_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sfz_file_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_sound_container_interface_info = {
      (GInterfaceInitFunc) ags_sfz_file_sound_container_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_sfz_file = g_type_register_static(G_TYPE_OBJECT,
					       "AgsSFZFile",
					       &ags_sfz_file_info,
					       0);

    g_type_add_interface_static(ags_type_sfz_file,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_sfz_file,
				AGS_TYPE_SOUND_CONTAINER,
				&ags_sound_container_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sfz_file);
  }

  return g_define_type_id__volatile;
}

void
ags_sfz_file_class_init(AgsSFZFileClass *sfz_file)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_sfz_file_parent_class = g_type_class_peek_parent(sfz_file);

  /* GObjectClass */
  gobject = (GObjectClass *) sfz_file;

  gobject->set_property = ags_sfz_file_set_property;
  gobject->get_property = ags_sfz_file_get_property;

  gobject->dispose = ags_sfz_file_dispose;
  gobject->finalize = ags_sfz_file_finalize;

  /* properties */
  /**
   * AgsSFZFile:soundcard:
   *
   * The assigned soundcard.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("soundcard of sfz_file"),
				   i18n_pspec("The soundcard what sfz_file has it's presets"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsSFZFile:filename:
   *
   * The assigned filename.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("the filename"),
				   i18n_pspec("The filename to open"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsSFZFile:mode:
   *
   * The assigned mode.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_string("mode",
				   i18n_pspec("the mode"),
				   i18n_pspec("The mode to open the file"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MODE,
				  param_spec);

  /**
   * AgsSFZFile:group:
   *
   * The containing groups.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_pointer("group",
				    i18n_pspec("containing group"),
				    i18n_pspec("The containing groups"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GROUP,
				  param_spec);

  /**
   * AgsSFZFile:region:
   *
   * The containing regions.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_pointer("region",
				    i18n_pspec("containing region"),
				    i18n_pspec("The containing regions"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_REGION,
				  param_spec);

  /**
   * AgsSFZFile:sample:
   *
   * The containing samples.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_pointer("sample",
				    i18n_pspec("containing sample"),
				    i18n_pspec("The containing samples"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLE,
				  param_spec);
}

void
ags_sfz_file_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_sfz_file_get_uuid;
  connectable->has_resource = ags_sfz_file_has_resource;
  connectable->is_ready = ags_sfz_file_is_ready;

  connectable->add_to_registry = ags_sfz_file_add_to_registry;
  connectable->remove_from_registry = ags_sfz_file_remove_from_registry;

  connectable->list_resource = ags_sfz_file_list_resource;
  connectable->xml_compose = ags_sfz_file_xml_compose;
  connectable->xml_parse = ags_sfz_file_xml_parse;

  connectable->is_connected = ags_sfz_file_is_connected;
  
  connectable->connect = ags_sfz_file_connect;
  connectable->disconnect = ags_sfz_file_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_sfz_file_sound_container_interface_init(AgsSoundContainerInterface *sound_container)
{
  sound_container->open = ags_sfz_file_open;
  
  sound_container->get_level_count = ags_sfz_file_get_level_count;
  sound_container->get_nesting_level = ags_sfz_file_get_nesting_level;
  
  sound_container->get_level_id = ags_sfz_file_get_level_id;
  sound_container->get_level_index = ags_sfz_file_get_level_index;
  
  sound_container->get_sublevel_name = ags_sfz_file_get_sublevel_name;
  
  sound_container->level_up = ags_sfz_file_level_up;
  sound_container->select_level_by_id = ags_sfz_file_select_level_by_id;
  sound_container->select_level_by_index = ags_sfz_file_select_level_by_index;
  
  sound_container->get_resource_all = ags_sfz_file_get_resource_all;
  sound_container->get_resource_by_name = ags_sfz_file_get_resource_by_name;
  sound_container->get_resource_by_index = ags_sfz_file_get_resource_by_index;
  sound_container->get_resource_current = ags_sfz_file_get_resource_current;
  
  sound_container->close = ags_sfz_file_close;
}

void
ags_sfz_file_init(AgsSFZFile *sfz_file)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  sfz_file->flags = 0;

  /* add audio file mutex */
  sfz_file->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  sfz_file->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);  

  /* uuid */
  sfz_file->uuid = ags_uuid_alloc();
  ags_uuid_generate(sfz_file->uuid);

  sfz_file->soundcard = NULL;

  sfz_file->filename = NULL;
  sfz_file->mode = AGS_SFZ_FILE_READ;

  sfz_file->file = NULL;

  sfz_file->nesting_level = 0;

  sfz_file->level_id = NULL;
  sfz_file->level_index = 0;

  sfz_file->reader = NULL;
  sfz_file->writer = NULL;

  sfz_file->group = NULL;
  sfz_file->region = NULL;
  sfz_file->sample = NULL;
  
  /* selected */
  sfz_file->index_selected = (guint *) malloc(2 * sizeof(guint));
  memset(sfz_file->index_selected, 0, 2 * sizeof(guint));
  
  sfz_file->name_selected = (gchar **) malloc(3 * sizeof(gchar *));

  for(i = 0; i < 3; i++){
    sfz_file->name_selected[i] = NULL;
  }

  sfz_file->current_sample = NULL;
  
  sfz_file->audio_signal= NULL;
}

void
ags_sfz_file_set_property(GObject *gobject,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *param_spec)
{
  AgsSFZFile *sfz_file;

  pthread_mutex_t *sfz_file_mutex;

  sfz_file = AGS_SFZ_FILE(gobject);

  /* get audio file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  switch(prop_id){
  case PROP_SOUNDCARD:
  {
    GObject *soundcard;
      
    soundcard = (GObject *) g_value_get_object(value);

    pthread_mutex_lock(sfz_file_mutex);

    if(soundcard == ((GObject *) sfz_file->soundcard)){
      pthread_mutex_unlock(sfz_file_mutex);

      return;
    }

    if(sfz_file->soundcard != NULL){
      g_object_unref(sfz_file->soundcard);
    }
      
    if(soundcard != NULL){
      g_object_ref(G_OBJECT(soundcard));
    }
      
    sfz_file->soundcard = (GObject *) soundcard;

    pthread_mutex_unlock(sfz_file_mutex);
  }
  break;
  case PROP_FILENAME:
  {
    gchar *filename;

    filename = (gchar *) g_value_get_string(value);

    ags_sound_container_open(AGS_SOUND_CONTAINER(sfz_file), filename);
  }
  break;
  case PROP_MODE:
  {
    gchar *mode;
      
    mode = (gchar *) g_value_get_string(value);

    pthread_mutex_lock(sfz_file_mutex);
      
    sfz_file->mode = mode;

    pthread_mutex_lock(sfz_file_mutex);
  }
  break;
  case PROP_GROUP:
  {
    GObject *group;

    group = g_value_get_pointer(value);

    pthread_mutex_lock(sfz_file_mutex);

    if(group == NULL ||
       g_list_find(sfz_file->group, group) != NULL){
      pthread_mutex_unlock(sfz_file_mutex);

      return;	
    }

    g_object_ref(group);
    g_list_prepend(sfz_file->group,
		   group);

    pthread_mutex_unlock(sfz_file_mutex);
  }
  break;
  case PROP_REGION:
  {
    GObject *region;

    region = g_value_get_pointer(value);

    pthread_mutex_lock(sfz_file_mutex);

    if(region == NULL ||
       g_list_find(sfz_file->region, region) != NULL){
      pthread_mutex_unlock(sfz_file_mutex);

      return;	
    }

    g_object_ref(region);
    g_list_prepend(sfz_file->region,
		   region);

    pthread_mutex_unlock(sfz_file_mutex);
  }
  break;
  case PROP_SAMPLE:
  {
    GObject *sample;

    sample = g_value_get_pointer(value);

    pthread_mutex_lock(sfz_file_mutex);

    if(sample == NULL ||
       g_list_find(sfz_file->sample, sample) != NULL){
      pthread_mutex_unlock(sfz_file_mutex);

      return;	
    }

    g_object_ref(sample);
    g_list_prepend(sfz_file->sample,
		   sample);

    pthread_mutex_unlock(sfz_file_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_sfz_file_get_property(GObject *gobject,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *param_spec)
{
  AgsSFZFile *sfz_file;

  pthread_mutex_t *sfz_file_mutex;

  sfz_file = AGS_SFZ_FILE(gobject);

  /* get audio file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  switch(prop_id){
  case PROP_SOUNDCARD:
  {
    pthread_mutex_lock(sfz_file_mutex);

    g_value_set_object(value, sfz_file->soundcard);

    pthread_mutex_unlock(sfz_file_mutex);
  }
  break;
  case PROP_FILENAME:
  {
    pthread_mutex_lock(sfz_file_mutex);

    g_value_set_string(value, sfz_file->filename);

    pthread_mutex_unlock(sfz_file_mutex);
  }
  break;
  case PROP_MODE:
  {
    pthread_mutex_lock(sfz_file_mutex);

    g_value_set_string(value, sfz_file->mode);

    pthread_mutex_unlock(sfz_file_mutex);
  }
  break;
  case PROP_GROUP:
  {
    pthread_mutex_lock(sfz_file_mutex);

    g_value_set_pointer(value, g_list_copy_deep(sfz_file->group,
						(GCopyFunc) g_object_ref,
						NULL));

    pthread_mutex_unlock(sfz_file_mutex);
  }
  break;
  case PROP_REGION:
  {
    pthread_mutex_lock(sfz_file_mutex);

    g_value_set_pointer(value, g_list_copy_deep(sfz_file->region,
						(GCopyFunc) g_object_ref,
						NULL));

    pthread_mutex_unlock(sfz_file_mutex);
  }
  break;
  case PROP_SAMPLE:
  {
    pthread_mutex_lock(sfz_file_mutex);

    g_value_set_pointer(value, g_list_copy_deep(sfz_file->sample,
						(GCopyFunc) g_object_ref,
						NULL));

    pthread_mutex_unlock(sfz_file_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_sfz_file_dispose(GObject *gobject)
{
  AgsSFZFile *sfz_file;

  sfz_file = AGS_SFZ_FILE(gobject);

  if(sfz_file->group != NULL){
    g_list_free_full(sfz_file->group,
		     g_object_unref);

    sfz_file->group = NULL;
  }

  if(sfz_file->region != NULL){
    g_list_free_full(sfz_file->region,
		     g_object_unref);

    sfz_file->region = NULL;
  }

  if(sfz_file->sample != NULL){
    g_list_free_full(sfz_file->sample,
		     g_object_unref);

    sfz_file->sample = NULL;
  }

  /* call parent */  
  G_OBJECT_CLASS(ags_sfz_file_parent_class)->dispose(gobject);
}

void
ags_sfz_file_finalize(GObject *gobject)
{
  AgsSFZFile *sfz_file;

  sfz_file = AGS_SFZ_FILE(gobject);

  pthread_mutex_destroy(sfz_file->obj_mutex);
  free(sfz_file->obj_mutex);

  pthread_mutexattr_destroy(sfz_file->obj_mutexattr);
  free(sfz_file->obj_mutexattr);
  
  if(sfz_file->soundcard != NULL){
    g_object_unref(sfz_file->soundcard);
  }

  g_free(sfz_file->filename);
  g_free(sfz_file->mode);

  if(sfz_file->reader != NULL){
    g_object_unref(sfz_file->reader);
  }

  if(sfz_file->group != NULL){
    g_list_free_full(sfz_file->group,
		     g_object_unref);
  }

  if(sfz_file->region != NULL){
    g_list_free_full(sfz_file->region,
		     g_object_unref);
  }

  if(sfz_file->sample != NULL){
    g_list_free_full(sfz_file->sample,
		     g_object_unref);
  }

  g_list_free_full(sfz_file->audio_signal,
		   g_object_unref);
    
  /* call parent */
  G_OBJECT_CLASS(ags_sfz_file_parent_class)->finalize(gobject);
}

AgsUUID*
ags_sfz_file_get_uuid(AgsConnectable *connectable)
{
  AgsSFZFile *sfz_file;
  
  AgsUUID *ptr;

  pthread_mutex_t *sfz_file_mutex;

  sfz_file = AGS_SFZ_FILE(connectable);

  /* get audio file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* get UUID */
  pthread_mutex_lock(sfz_file_mutex);

  ptr = sfz_file->uuid;

  pthread_mutex_unlock(sfz_file_mutex);
  
  return(ptr);
}

gboolean
ags_sfz_file_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_sfz_file_is_ready(AgsConnectable *connectable)
{
  AgsSFZFile *sfz_file;
  
  gboolean is_ready;

  pthread_mutex_t *sfz_file_mutex;

  sfz_file = AGS_SFZ_FILE(connectable);

  /* get audio file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* check is ready */
  pthread_mutex_lock(sfz_file_mutex);
  
  is_ready = (((AGS_SFZ_FILE_ADDED_TO_REGISTRY & (sfz_file->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(sfz_file_mutex);

  return(is_ready);
}

void
ags_sfz_file_add_to_registry(AgsConnectable *connectable)
{
  AgsSFZFile *sfz_file;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }

  sfz_file = AGS_SFZ_FILE(connectable);

  ags_sfz_file_set_flags(sfz_file, AGS_SFZ_FILE_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(entry->entry,
		       (gpointer) sfz_file);
    ags_registry_add_entry(registry,
			   entry);
  }  
}

void
ags_sfz_file_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_sfz_file_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_sfz_file_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_sfz_file_xml_parse(AgsConnectable *connectable,
		       xmlNode *node)
{
  //TODO:JK: implement me  
}

gboolean
ags_sfz_file_is_connected(AgsConnectable *connectable)
{
  AgsSFZFile *sfz_file;
  
  gboolean is_connected;

  pthread_mutex_t *sfz_file_mutex;

  sfz_file = AGS_SFZ_FILE(connectable);

  /* get audio file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* check is connected */
  pthread_mutex_lock(sfz_file_mutex);

  is_connected = (((AGS_SFZ_FILE_CONNECTED & (sfz_file->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(sfz_file_mutex);

  return(is_connected);
}

void
ags_sfz_file_connect(AgsConnectable *connectable)
{
  AgsSFZFile *sfz_file;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  sfz_file = AGS_SFZ_FILE(connectable);
  
  ags_sfz_file_set_flags(sfz_file, AGS_SFZ_FILE_CONNECTED);
}

void
ags_sfz_file_disconnect(AgsConnectable *connectable)
{
  AgsSFZFile *sfz_file;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  sfz_file = AGS_SFZ_FILE(connectable);

  ags_sfz_file_unset_flags(sfz_file, AGS_SFZ_FILE_CONNECTED);
}

gboolean
ags_sfz_file_open(AgsSoundContainer *sound_container, gchar *filename)
{
  AgsSFZFile *sfz_file;

  FILE *file;
  
  gchar *old_filename;  

  gboolean retval;

  pthread_mutex_t *sfz_file_mutex;
  
  sfz_file = AGS_SFZ_FILE(sound_container);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* get some fields */
  pthread_mutex_lock(sfz_file_mutex);

  old_filename = sfz_file->filename;
  
  pthread_mutex_unlock(sfz_file_mutex);

  /* close current */
  if(old_filename != NULL){
    ags_sound_container_close(sound_container);
    
    g_free(old_filename);
  }

  /* check suffix */
  sfz_file->filename = g_strdup(filename);
  
  if(!ags_sfz_file_check_suffix(filename)){
    return(FALSE);
  }

  /* open file */
  file = fopen(filename,
	       "r");

  pthread_mutex_lock(sfz_file_mutex);

  sfz_file->file = file;

  pthread_mutex_unlock(sfz_file_mutex);

  if(file == NULL){
    return(FALSE);
  }

  /* load samples */
  retval = TRUE;

  ags_sfz_file_parse(sfz_file);
  
  return(retval);
}

guint
ags_sfz_file_get_level_count(AgsSoundContainer *sound_container)
{
  AgsSFZFile *sfz_file;

  sfz_file = AGS_SFZ_FILE(sound_container);

  return(3);
}

guint
ags_sfz_file_get_nesting_level(AgsSoundContainer *sound_container)
{
  AgsSFZFile *sfz_file;

  guint nesting_level;

  pthread_mutex_t *sfz_file_mutex;
  
  sfz_file = AGS_SFZ_FILE(sound_container);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* get nesting level */
  pthread_mutex_lock(sfz_file_mutex);

  nesting_level = sfz_file->nesting_level;

  pthread_mutex_unlock(sfz_file_mutex);
  
  return(nesting_level);
}

gchar*
ags_sfz_file_get_level_id(AgsSoundContainer *sound_container)
{
  AgsSFZFile *sfz_file;

  gchar *level_id;

  pthread_mutex_t *sfz_file_mutex;
  
  sfz_file = AGS_SFZ_FILE(sound_container);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* get level id */
  pthread_mutex_lock(sfz_file_mutex);

  level_id = sfz_file->level_id;

  pthread_mutex_unlock(sfz_file_mutex);
  
  return(level_id);
}

guint
ags_sfz_file_get_level_index(AgsSoundContainer *sound_container)
{
  AgsSFZFile *sfz_file;

  guint level_index;

  pthread_mutex_t *sfz_file_mutex;
  
  sfz_file = AGS_SFZ_FILE(sound_container);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);
  
  /* get nesting level */
  pthread_mutex_lock(sfz_file_mutex);

  level_index = sfz_file->level_index;

  pthread_mutex_unlock(sfz_file_mutex);
  
  return(level_index);
}

gchar**
ags_sfz_file_get_sublevel_name(AgsSoundContainer *sound_container)
{  
  AgsSFZFile *sfz_file;

  guint sublevel;

  pthread_mutex_t *sfz_file_mutex;
  
  sfz_file = AGS_SFZ_FILE(sound_container);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* sublevel */
  sublevel = ags_sound_container_get_nesting_level(AGS_SOUND_CONTAINER(sfz_file));

  switch(sublevel){
  case AGS_SFZ_FILENAME:
  {
    gchar **sublevel_name;
	
    sublevel_name = (gchar **) malloc(2 * sizeof(gchar*));

    sublevel_name[0] = g_strdup(sfz_file->filename);
    sublevel_name[1] = NULL;

    return(sublevel_name);
  }
  case AGS_SFZ_SAMPLE:
  {
    GList *start_list, *list;

    gchar **sublevel_name;

    guint sample_count;
    guint i;

    g_object_get(sfz_file,
		 "sample", &start_list,
		 NULL);

    list = start_list;

    sample_count = g_list_length(start_list);
    
    sublevel_name = (gchar **) malloc((sample_count + 1) * sizeof(gchar*));

    for(i = 0; i < sample_count; i++){
      gchar *str;
      
      g_object_get(list->data,
		   "filename", &str,
		   NULL);
      sublevel_name[i] = str;

      list = list->next;
    }
    
    sublevel_name[i] = NULL;

    g_list_free_full(start_list,
		     g_object_unref);
    
    return(sublevel_name);
  }
  };

  return(NULL);
}

guint
ags_sfz_file_level_up(AgsSoundContainer *sound_container,
		      guint level_count)
{
  AgsSFZFile *sfz_file;
  
  guint retval;

  pthread_mutex_t *sfz_file_mutex;
  
  if(level_count == 0){
    return(0);
  }

  sfz_file = AGS_SFZ_FILE(sound_container);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* check boundaries */
  if(ags_sound_container_get_nesting_level(AGS_SOUND_CONTAINER(sfz_file)) >= level_count){
    /* level up */
    pthread_mutex_lock(sfz_file_mutex);

    retval = level_count;

    sfz_file->nesting_level -= level_count;

    pthread_mutex_unlock(sfz_file_mutex);
  }else{
    /* level up */
    pthread_mutex_lock(sfz_file_mutex);

    retval = sfz_file->nesting_level;
    
    sfz_file->nesting_level = 0;

    pthread_mutex_unlock(sfz_file_mutex);
  }

  return(retval);
}

guint
ags_sfz_file_select_level_by_id(AgsSoundContainer *sound_container,
				gchar *level_id)
{
  return(0);
}

guint
ags_sfz_file_select_level_by_index(AgsSoundContainer *sound_container,
				   guint level_index)
{
  AgsSFZFile *sfz_file;

  guint sublevel;
  guint retval;

  pthread_mutex_t *sfz_file_mutex;
  
  sfz_file = AGS_SFZ_FILE(sound_container);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* sublevel */
  sublevel = ags_sound_container_get_nesting_level(AGS_SOUND_CONTAINER(sfz_file));
  retval = 0;
  
  switch(sublevel){
  case AGS_SFZ_FILENAME:
  {
    if(ags_sfz_file_select_sample(ipatch_sfz_reader, level_index)){
      retval = AGS_SFZ_FILENAME;
    }
  }
  break;
  case AGS_SFZ_SAMPLE:
  {
    retval = AGS_SFZ_SAMPLE;
  }
  break;
  };
  
  return(retval);
}

GList*
ags_sfz_file_get_resource_all(AgsSoundContainer *sound_container)
{
  AgsSFZFile *sfz_file;
  
  GList *resource;

  sfz_file = AGS_SFZ_FILE(sound_container);

  resource = NULL;
  
  //TODO:JK: implement me
  
  return(resource);
}

GList*
ags_sfz_file_get_resource_by_name(AgsSoundContainer *sound_container,
				  gchar *resource_name)
{
  //TODO:JK: implement me

  return(NULL);  
}

GList*
ags_sfz_file_get_resource_by_index(AgsSoundContainer *sound_container,
				   guint resource_index)
{
  //TODO:JK: implement me

  return(NULL);
}

GList*
ags_sfz_file_get_resource_current(AgsSoundContainer *sound_container)
{
  AgsSFZFile *sfz_file;

  GList *sound_resource;

  pthread_mutex_t *sfz_file_mutex;
  
  sfz_file = AGS_SFZ_FILE(sound_container);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* get sound resource */
  sound_resource = NULL;

  g_object_get(sfz_file,
	       "sample", &sound_resource,
	       NULL);
  
  return(sound_resource);
}

void
ags_sfz_file_close(AgsSoundContainer *sound_container)
{
  //TODO:JK: implement me
}
/**
 * ags_sfz_file_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.3.0
 */
pthread_mutex_t*
ags_sfz_file_get_class_mutex()
{
  return(&ags_sfz_file_class_mutex);
}

/**
 * ags_sfz_file_test_flags:
 * @sfz_file: the #AgsSFZFile
 * @flags: the flags
 *
 * Test @flags to be set on @sfz_file.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.3.0
 */
gboolean
ags_sfz_file_test_flags(AgsSFZFile *sfz_file, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *sfz_file_mutex;

  if(!AGS_IS_SFZ_FILE(sfz_file)){
    return(FALSE);
  }

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* test */
  pthread_mutex_lock(sfz_file_mutex);

  retval = (flags & (sfz_file->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(sfz_file_mutex);

  return(retval);
}

/**
 * ags_sfz_file_set_flags:
 * @sfz_file: the #AgsSFZFile
 * @flags: see #AgsSFZFileFlags-enum
 *
 * Enable a feature of @sfz_file.
 *
 * Since: 2.3.0
 */
void
ags_sfz_file_set_flags(AgsSFZFile *sfz_file, guint flags)
{
  pthread_mutex_t *sfz_file_mutex;

  if(!AGS_IS_SFZ_FILE(sfz_file)){
    return;
  }

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(sfz_file_mutex);

  sfz_file->flags |= flags;
  
  pthread_mutex_unlock(sfz_file_mutex);
}
    
/**
 * ags_sfz_file_unset_flags:
 * @sfz_file: the #AgsSFZFile
 * @flags: see #AgsSFZFileFlags-enum
 *
 * Disable a feature of @sfz_file.
 *
 * Since: 2.3.0
 */
void
ags_sfz_file_unset_flags(AgsSFZFile *sfz_file, guint flags)
{  
  pthread_mutex_t *sfz_file_mutex;

  if(!AGS_IS_SFZ_FILE(sfz_file)){
    return;
  }

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(sfz_file_mutex);

  sfz_file->flags &= (~flags);
  
  pthread_mutex_unlock(sfz_file_mutex);
}

/**
 * ags_sfz_file_select_sample:
 * @sfz_file: the #AgsSFZFile
 * @sample_index: the sample index
 * 
 * Select sample.
 * 
 * Returns: %TRUE on success, else %FALSE on failure
 * 
 * Since: 2.3.0
 */
gboolean
ags_sfz_file_select_sample(AgsSFZFile *sfz_file,
			   guint sample_index)
{
  GList *start_list, *list;

  if(!AGS_IS_SFZ_FILE(sfz_file)){
    return(FALSE);
  }

  success = FALSE;

  g_object_get(sfz_file,
	       "sample", &start_list,
	       NULL);

  if(start_list != NULL){
    list = start_list;

    if(sample_index < g_list_length(start_list)){
      gchar *filename;
      
      success = TRUE;

      list = g_list_nth(start_list,
			sample_index);

      /* selected index and name */
      ipatch_sf2_reader->index_selected[AGS_SFZ_SAMPLE] = sample_index;

      g_free(ipatch_sf2_reader->name_selected[AGS_SFZ_SAMPLE]);

      g_object_get(list->data,
		   "filename", &filename,
		   NULL);
      
      ipatch_sf2_reader->name_selected[AGS_SFZ_SAMPLE] = filename;

      /* container */
      ipatch_sf2_reader->current_sample = (AgsSFZSample *) list->data;
    }
  }
  
  return(success);
}

/**
 * ags_sfz_file_check_suffix:
 * @filename: the filename
 * 
 * Check @filename's suffix to be supported.
 * 
 * Returns: %TRUE if supported, else %FALSE
 * 
 * Since: 2.3.0
 */
gboolean
ags_sfz_file_check_suffix(gchar *filename)
{
  if(g_str_has_suffix(filename, ".sfz")){
    return(TRUE);
  }

  return(FALSE);
}

/**
 * ags_sfz_file_parse:
 * @sfz_file: the #AgsSFZFile
 *
 * Parse @sfz_file.
 *
 * Since: 2.3.0
 */
void
ags_sfz_file_parse(AgsSFZFile *sfz_file)
{
  AgsSFZGroup *current_group;
  AgsSFZRegion *current_region;
  AgsSFZSample *current_sample;
  
  FILE *file;

  struct stat *sb;

  regmatch_t match_arr[3];

  gchar *filename;
  gchar *buffer, *iter;

  size_t n_read;
  gint nth_group;
  gint nth_region;
  gboolean group_active;
  gboolean region_active;
  gboolean sample_active;

  pthread_mutex_t *sfz_file_mutex;

  static regex_t opcode_regex;

  static const gchar *opcode_pattern = "^([\w]+)=([\S]+)";

  static const size_t max_matches = 3;
  static gboolean regex_compiled = FALSE;

  auto gchar* ags_sfz_file_parse_skip_comments_and_blanks(gchar **iter);

  gchar* ags_sfz_file_parse_skip_comments_and_blanks(gchar **iter){
    gchar *look_ahead;
    gchar *next;
    
    if(iter == NULL){
      return(NULL);
    }

    look_ahead = *iter;

    if(look_ahead == NULL){
      return(NULL);
    }
    
    /* skip whitespaces and comments */
    for(; (look_ahead < &(buffer[sb->st_size])) && *look_ahead != '\0'; look_ahead++){
      /* skip comments */
      if(buffer == look_ahead){
	if(look_ahead + 1 < &(buffer[sb->st_size]) && buffer[0] == '/' && buffer[1] == '/'){
	  next = index(look_ahead, '\n');

	  if(next != NULL){
	    look_ahead = next + 1;
	  }else{
	    look_ahead = &(buffer[sb->st_size]);

	    break;
	  }
	  
	  continue;
	}
      }else if(buffer[look_ahead - buffer - 1] == '\n' && look_ahead + 1 < &(buffer[sb->st_size]) && look_ahead[0] == '/' && look_ahead[1] == '/'){
	next = index(look_ahead, '\n');
      
	if(next != NULL){
	  look_ahead = next + 1;
	}else{
	  look_ahead = &(buffer[sb->st_size]);

	  break;
	}
	
	continue;
      }

      /* spaces */
      if(!(look_ahead[0] == ' ' || look_ahead[0] == '\t' || look_ahead[0] == '\n')){
	break;
      }
    }

    return(look_ahead);
  }

  if(!AGS_IS_SFZ_FILE(sfz_file)){
    return;
  }

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* stat file */
  g_object_get(sfz_file,
	       "filename", &filename,
	       NULL);
  
  sb = (struct stat *) malloc(sizeof(struct stat));
  stat(filename,
       sb);

  if(sb->st_size == 0){
    g_free(filename);

    free(sb);
    
    return;
  }

  /* read SFZ */
  pthread_mutex_lock(sfz_file_mutex);

  file = sfz_file->file;

  pthread_mutex_unlock(sfz_file_mutex);

  buffer = (gchar *) malloc((sb->st_size + 1) * sizeof(gchar));

  if(buffer == NULL){
    g_free(filename);

    free(sb);

    return(FALSE);
  }
  
  n_read = fread(buffer, sizeof(gchar), sb->st_size, file);

  if(n_read != sb->st_size){
    g_critical("number of read bytes doesn't match buffer size");
  }
  
  buffer[sb->st_size] = '\0';

  iter = buffer;

  current_group = NULL;
  current_region = NULL;
  current_sample = NULL;
  
  nth_group = -1;
  nth_region = -1;

  group_active = FALSE;
  region_active = FALSE;
  sample_active = FALSE;

  pthread_mutex_lock(&regex_mutex);
  
  if(!regex_compiled){
    regex_compiled = TRUE;
    
    ags_regcomp(&opcode_regex, opcode_pattern, REG_EXTENDED);
  }

  pthread_mutex_unlock(&regex_mutex);
  
  do{    
    /* skip blanks and comments */
    iter = ags_sfz_file_parse_skip_comments_and_blanks(&iter);

    if(iter >= &(buffer[sb->st_size])){
      break;
    }

    if(!g_ascii_strncasecmp(iter,
			    "<group>",
			    7)){
      nth_group++;

      group_active = TRUE;
      region_active = FALSE;
      sample_active = FALSE;

      current_group = ags_sfz_group_new();
      g_object_set(sfz_file,
		   "group", current_group,
		   NULL);
    }else if(!g_ascii_strncasecmp(iter,
				  "<region>",
				  8)){
      nth_region++;

      region_active = TRUE;
      sample_active = FALSE;

      current_region = ags_sfz_region_new();
      g_object_set(sfz_file,
		   "region", current_region,
		   NULL);
    }else if(ags_regexec(&opcode_regex, iter, max_matches, match_arr, 0) == 0){
      gchar *opcode;
      gchar *str;

      opcode = g_strndup(match_arr[1].rm_so,
			 match_arr[1].rm_eo - match_arr[1].rm_so);

      str = g_strndup(match_arr[2].rm_so,
		      match_arr[2].rm_eo - match_arr[2].rm_so);
      
      if(!g_ascii_strncasecmp(opcode,
			      "sample",
			      6)){
	gchar *filename;
	
	sample_active = TRUE;
	
	if(g_path_is_absolute(str)){
	  filename = g_strdup(str);
	}else{
	  gchar *path;
	  
	  path = g_path_get_dirname(sfz_file->filename);
	  
	  filename = g_strdup_printf("%s/%s",
				     path,
				     str);
	}
	
	current_sample = ags_sfz_sample_new();
	ags_sound_resource_open(AGS_SOUND_RESOURCE(current_sample),
				filename);
	
	g_object_set(sfz_file,
		     "sample", current_sample,
		     NULL);

	if(region_active){
	  g_object_set(current_region,
		       "sample", current_sample,
		       NULL);
	}else if(group_active){
	  g_object_set(current_group,
		       "sample", current_sample,
 		       NULL);
	}
      }

      if(region_active){
	ags_sfz_region_insert_control(current_region,
				      opcode, str);
      }else if(group_active){
	ags_sfz_group_insert_control(current_group,
				     opcode, str);
      }else{
	g_warning("SFZ neither group nor region defined");
      }
      
      iter += (match_arr[0].rm_eo - match_arr[0].rm_so);
    }else{
      /* bad byte */
      iter++;
    }        
  }while(iter < &(buffer[sb->st_size]));
  
  free(sb);
  free(buffer);
}

/**
 * ags_sfz_file_new:
 *
 * Creates an #AgsSFZFile.
 *
 * Returns: an empty #AgsSFZFile.
 *
 * Since: 2.3.0
 */
AgsSFZFile*
ags_sfz_file_new()
{
  AgsSFZFile *sfz_file;

  sfz_file = (AgsSFZFile *) g_object_new(AGS_TYPE_SFZ_FILE,
					 NULL);
  
  return(sfz_file);
}
