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

#include <ags/audio/file/ags_sfz_file.h>

#include <ags/audio/ags_diatonic_scale.h>

#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>
#include <ags/audio/file/ags_sfz_group.h>
#include <ags/audio/file/ags_sfz_region.h>
#include <ags/audio/file/ags_sfz_sample.h>

#include <sys/stat.h>

#if !defined(AGS_W32API)
#include <unistd.h>
#endif

#include <string.h>
#include <strings.h>

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

gchar* ags_sfz_file_parse_skip_comments_and_blanks(gchar *buffer, gsize buffer_length,
						   gchar **iter);

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

static GMutex regex_mutex;

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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * AgsSFZFile:group: (type GList(AgsSFZGroup)) (transfer full)
   *
   * The containing groups.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("group",
				    i18n_pspec("containing group"),
				    i18n_pspec("The containing groups"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GROUP,
				  param_spec);

  /**
   * AgsSFZFile:region: (type GList(AgsSFZRegion)) (transfer full)
   *
   * The containing regions.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("region",
				    i18n_pspec("containing region"),
				    i18n_pspec("The containing regions"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_REGION,
				  param_spec);

  /**
   * AgsSFZFile:sample: (type GList(AgsSFZSample)) (transfer full)
   *
   * The containing samples.
   * 
   * Since: 3.0.0
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
  guint i;
  
  sfz_file->flags = 0;

  /* add audio file mutex */
  g_rec_mutex_init(&(sfz_file->obj_mutex));

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

  GRecMutex *sfz_file_mutex;

  sfz_file = AGS_SFZ_FILE(gobject);

  /* get sfz file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  switch(prop_id){
  case PROP_SOUNDCARD:
  {
    GObject *soundcard;
      
    soundcard = (GObject *) g_value_get_object(value);

    g_rec_mutex_lock(sfz_file_mutex);

    if(soundcard == ((GObject *) sfz_file->soundcard)){
      g_rec_mutex_unlock(sfz_file_mutex);

      return;
    }

    if(sfz_file->soundcard != NULL){
      g_object_unref(sfz_file->soundcard);
    }
      
    if(soundcard != NULL){
      g_object_ref(G_OBJECT(soundcard));
    }
      
    sfz_file->soundcard = (GObject *) soundcard;

    g_rec_mutex_unlock(sfz_file_mutex);
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

    g_rec_mutex_lock(sfz_file_mutex);
      
    sfz_file->mode = mode;

    g_rec_mutex_lock(sfz_file_mutex);
  }
  break;
  case PROP_GROUP:
  {
    GObject *group;

    group = g_value_get_pointer(value);

    g_rec_mutex_lock(sfz_file_mutex);

    if(group == NULL ||
       g_list_find(sfz_file->group, group) != NULL){
      g_rec_mutex_unlock(sfz_file_mutex);

      return;	
    }

    g_object_ref(group);
    sfz_file->group = g_list_prepend(sfz_file->group,
				     group);

    g_rec_mutex_unlock(sfz_file_mutex);
  }
  break;
  case PROP_REGION:
  {
    GObject *region;

    region = g_value_get_pointer(value);

    g_rec_mutex_lock(sfz_file_mutex);

    if(region == NULL ||
       g_list_find(sfz_file->region, region) != NULL){
      g_rec_mutex_unlock(sfz_file_mutex);

      return;	
    }

    g_object_ref(region);
    sfz_file->region = g_list_prepend(sfz_file->region,
				      region);

    g_rec_mutex_unlock(sfz_file_mutex);
  }
  break;
  case PROP_SAMPLE:
  {
    GObject *sample;

    sample = g_value_get_pointer(value);

    g_rec_mutex_lock(sfz_file_mutex);

    if(sample == NULL ||
       g_list_find(sfz_file->sample, sample) != NULL){
      g_rec_mutex_unlock(sfz_file_mutex);

      return;	
    }

    g_object_ref(sample);
    sfz_file->sample = g_list_prepend(sfz_file->sample,
				      sample);

    g_rec_mutex_unlock(sfz_file_mutex);
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

  GRecMutex *sfz_file_mutex;

  sfz_file = AGS_SFZ_FILE(gobject);

  /* get sfz file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  switch(prop_id){
  case PROP_SOUNDCARD:
  {
    g_rec_mutex_lock(sfz_file_mutex);

    g_value_set_object(value, sfz_file->soundcard);

    g_rec_mutex_unlock(sfz_file_mutex);
  }
  break;
  case PROP_FILENAME:
  {
    g_rec_mutex_lock(sfz_file_mutex);

    g_value_set_string(value, sfz_file->filename);

    g_rec_mutex_unlock(sfz_file_mutex);
  }
  break;
  case PROP_MODE:
  {
    g_rec_mutex_lock(sfz_file_mutex);

    g_value_set_string(value, sfz_file->mode);

    g_rec_mutex_unlock(sfz_file_mutex);
  }
  break;
  case PROP_GROUP:
  {
    g_rec_mutex_lock(sfz_file_mutex);

    g_value_set_pointer(value, g_list_copy_deep(sfz_file->group,
						(GCopyFunc) g_object_ref,
						NULL));

    g_rec_mutex_unlock(sfz_file_mutex);
  }
  break;
  case PROP_REGION:
  {
    g_rec_mutex_lock(sfz_file_mutex);

    g_value_set_pointer(value, g_list_copy_deep(sfz_file->region,
						(GCopyFunc) g_object_ref,
						NULL));

    g_rec_mutex_unlock(sfz_file_mutex);
  }
  break;
  case PROP_SAMPLE:
  {
    g_rec_mutex_lock(sfz_file_mutex);

    g_value_set_pointer(value, g_list_copy_deep(sfz_file->sample,
						(GCopyFunc) g_object_ref,
						NULL));

    g_rec_mutex_unlock(sfz_file_mutex);
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

  GRecMutex *sfz_file_mutex;

  sfz_file = AGS_SFZ_FILE(connectable);

  /* get audio file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* get UUID */
  g_rec_mutex_lock(sfz_file_mutex);

  ptr = sfz_file->uuid;

  g_rec_mutex_unlock(sfz_file_mutex);
  
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

  GRecMutex *sfz_file_mutex;

  sfz_file = AGS_SFZ_FILE(connectable);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* check is ready */
  g_rec_mutex_lock(sfz_file_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (sfz_file->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(sfz_file_mutex);
  
  return(is_ready);
}

void
ags_sfz_file_add_to_registry(AgsConnectable *connectable)
{
  AgsSFZFile *sfz_file;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  GRecMutex *sfz_file_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }

  sfz_file = AGS_SFZ_FILE(connectable);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  g_rec_mutex_lock(sfz_file_mutex);

  sfz_file->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(sfz_file_mutex);

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
  AgsSFZFile *sfz_file;

  GRecMutex *sfz_file_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  sfz_file = AGS_SFZ_FILE(connectable);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  g_rec_mutex_lock(sfz_file_mutex);

  sfz_file->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(sfz_file_mutex);

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

  GRecMutex *sfz_file_mutex;

  sfz_file = AGS_SFZ_FILE(connectable);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* check is connected */
  g_rec_mutex_lock(sfz_file_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (sfz_file->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(sfz_file_mutex);
  
  return(is_connected);
}

void
ags_sfz_file_connect(AgsConnectable *connectable)
{
  AgsSFZFile *sfz_file;

  GRecMutex *sfz_file_mutex;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  sfz_file = AGS_SFZ_FILE(connectable);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  g_rec_mutex_lock(sfz_file_mutex);

  sfz_file->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(sfz_file_mutex);
}

void
ags_sfz_file_disconnect(AgsConnectable *connectable)
{
  AgsSFZFile *sfz_file;

  GRecMutex *sfz_file_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  sfz_file = AGS_SFZ_FILE(connectable);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  g_rec_mutex_lock(sfz_file_mutex);

  sfz_file->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(sfz_file_mutex);
}

gboolean
ags_sfz_file_open(AgsSoundContainer *sound_container, gchar *filename)
{
  AgsSFZFile *sfz_file;

  FILE *file;
  
  gchar *old_filename;  

  gboolean retval;

  GRecMutex *sfz_file_mutex;
  
  sfz_file = AGS_SFZ_FILE(sound_container);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* get some fields */
  g_rec_mutex_lock(sfz_file_mutex);

  old_filename = sfz_file->filename;
  
  g_rec_mutex_unlock(sfz_file_mutex);

  /* close current */
  if(old_filename != NULL){
    ags_sound_container_close(sound_container);
    
    g_free(old_filename);
  }

  /* check suffix */
  g_rec_mutex_lock(sfz_file_mutex);

  sfz_file->filename = g_strdup(filename);

  g_rec_mutex_unlock(sfz_file_mutex);
  
  if(!ags_sfz_file_check_suffix(filename)){
    g_message("unsupported suffix");
    
    return(FALSE);
  }

  /* open file */
  file = fopen(filename,
	       "r");

  g_rec_mutex_lock(sfz_file_mutex);

  sfz_file->file = file;

  g_rec_mutex_unlock(sfz_file_mutex);

  if(file == NULL){
    g_message("failed to open file");

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

  GRecMutex *sfz_file_mutex;
  
  sfz_file = AGS_SFZ_FILE(sound_container);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* get nesting level */
  g_rec_mutex_lock(sfz_file_mutex);

  nesting_level = sfz_file->nesting_level;

  g_rec_mutex_unlock(sfz_file_mutex);
  
  return(nesting_level);
}

gchar*
ags_sfz_file_get_level_id(AgsSoundContainer *sound_container)
{
  AgsSFZFile *sfz_file;

  gchar *level_id;

  GRecMutex *sfz_file_mutex;
  
  sfz_file = AGS_SFZ_FILE(sound_container);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* get level id */
  g_rec_mutex_lock(sfz_file_mutex);

  level_id = sfz_file->level_id;

  g_rec_mutex_unlock(sfz_file_mutex);
  
  return(level_id);
}

guint
ags_sfz_file_get_level_index(AgsSoundContainer *sound_container)
{
  AgsSFZFile *sfz_file;

  guint level_index;

  GRecMutex *sfz_file_mutex;
  
  sfz_file = AGS_SFZ_FILE(sound_container);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);
  
  /* get nesting level */
  g_rec_mutex_lock(sfz_file_mutex);

  level_index = sfz_file->level_index;

  g_rec_mutex_unlock(sfz_file_mutex);
  
  return(level_index);
}

gchar**
ags_sfz_file_get_sublevel_name(AgsSoundContainer *sound_container)
{  
  AgsSFZFile *sfz_file;

  guint sublevel;

  GRecMutex *sfz_file_mutex;
  
  sfz_file = AGS_SFZ_FILE(sound_container);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* sublevel */
  sublevel = ags_sound_container_get_nesting_level(AGS_SOUND_CONTAINER(sfz_file));

  switch(sublevel){
  case AGS_SFZ_LEVEL_FILENAME:
  {
    gchar **sublevel_name;
	
    sublevel_name = (gchar **) malloc(2 * sizeof(gchar*));

    g_rec_mutex_lock(sfz_file_mutex);
    
    sublevel_name[0] = g_strdup(sfz_file->filename);

    g_rec_mutex_unlock(sfz_file_mutex);

    sublevel_name[1] = NULL;

    return(sublevel_name);
  }
  case AGS_SFZ_LEVEL_SAMPLE:
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

  GRecMutex *sfz_file_mutex;
  
  if(level_count == 0){
    return(0);
  }

  sfz_file = AGS_SFZ_FILE(sound_container);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* check boundaries */
  if(ags_sound_container_get_nesting_level(AGS_SOUND_CONTAINER(sfz_file)) >= level_count){
    /* level up */
    g_rec_mutex_lock(sfz_file_mutex);

    retval = level_count;

    sfz_file->nesting_level -= level_count;

    g_rec_mutex_unlock(sfz_file_mutex);
  }else{
    /* level up */
    g_rec_mutex_lock(sfz_file_mutex);

    retval = sfz_file->nesting_level;
    
    sfz_file->nesting_level = 0;

    g_rec_mutex_unlock(sfz_file_mutex);
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

  GRecMutex *sfz_file_mutex;
  
  sfz_file = AGS_SFZ_FILE(sound_container);

  /* get sfz_file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  /* sublevel */
  sublevel = ags_sound_container_get_nesting_level(AGS_SOUND_CONTAINER(sfz_file));
  retval = 0;
  
  switch(sublevel){
  case AGS_SFZ_LEVEL_FILENAME:
  {
    if(ags_sfz_file_select_sample(sfz_file, level_index)){
      retval = AGS_SFZ_LEVEL_FILENAME;
    }
  }
  break;
  case AGS_SFZ_LEVEL_SAMPLE:
  {
    retval = AGS_SFZ_LEVEL_SAMPLE;
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

  GRecMutex *sfz_file_mutex;
  
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
 * ags_sfz_file_get_group:
 * @sfz_file: the #AgsSFZFile
 *
 * Get the groups of @sfz_file.
 * 
 * Returns: (element-type AgsAudio.SFZGroup) (transfer full): the #GList-struct containing #AgsSFZGroup
 *
 * Since: 3.17.0
 */
GList*
ags_sfz_file_get_group(AgsSFZFile *sfz_file)
{
  GList *start_group;
  
  if(!AGS_IS_SFZ_FILE(sfz_file)){
    return(NULL);
  }

  start_group = NULL;

  g_object_get(sfz_file,
	       "group", &start_group,
	       NULL);
  
  return(start_group);
}

/**
 * ags_sfz_file_set_group:
 * @sfz_file: the #AgsSFZFile
 * @group: (element-type AgsAudio.SFZGroup) (transfer none): the #GList-struct containing #AgsSFZGroup
 *
 * Set the group field of @sfz_file
 * 
 * Since: 3.17.0
 */
void
ags_sfz_file_set_group(AgsSFZFile *sfz_file,
		       GList *group)
{
  GRecMutex *sfz_file_mutex;

  if(!AGS_IS_SFZ_FILE(sfz_file)){
    return;
  }

  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  g_rec_mutex_lock(sfz_file_mutex);

  g_list_free_full(sfz_file->group,
		   (GDestroyNotify) g_object_unref);
  
  sfz_file->group = g_list_copy_deep(group,
				     (GCopyFunc) g_object_ref,
				      NULL);

  g_rec_mutex_unlock(sfz_file_mutex);
}

/**
 * ags_sfz_file_get_region:
 * @sfz_file: the #AgsSFZFile
 *
 * Get the regions of @sfz_file.
 * 
 * Returns: (element-type AgsAudio.SFZRegion) (transfer full): the #GList-struct containing #AgsSFZRegion
 *
 * Since: 3.17.0
 */
GList*
ags_sfz_file_get_region(AgsSFZFile *sfz_file)
{
  GList *start_region;
  
  if(!AGS_IS_SFZ_FILE(sfz_file)){
    return(NULL);
  }

  start_region = NULL;

  g_object_get(sfz_file,
	       "region", &start_region,
	       NULL);
  
  return(start_region);
}

/**
 * ags_sfz_file_set_region:
 * @sfz_file: the #AgsSFZFile
 * @region: (element-type AgsAudio.SFZRegion) (transfer none): the #GList-struct containing #AgsSFZRegion
 *
 * Set the region field of @sfz_file
 * 
 * Since: 3.17.0
 */
void
ags_sfz_file_set_region(AgsSFZFile *sfz_file,
			GList *region)
{
  GRecMutex *sfz_file_mutex;

  if(!AGS_IS_SFZ_FILE(sfz_file)){
    return;
  }

  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  g_rec_mutex_lock(sfz_file_mutex);

  g_list_free_full(sfz_file->region,
		   (GDestroyNotify) g_object_unref);
  
  sfz_file->region = g_list_copy_deep(region,
				      (GCopyFunc) g_object_ref,
				      NULL);

  g_rec_mutex_unlock(sfz_file_mutex);
}

/**
 * ags_sfz_file_get_sample:
 * @sfz_file: the #AgsSFZFile
 *
 * Get the samples of @sfz_file.
 * 
 * Returns: (element-type AgsAudio.SFZSample) (transfer full): the #GList-struct containing #AgsSFZSample
 *
 * Since: 3.17.0
 */
GList*
ags_sfz_file_get_sample(AgsSFZFile *sfz_file)
{
  GList *start_sample;
  
  if(!AGS_IS_SFZ_FILE(sfz_file)){
    return(NULL);
  }

  start_sample = NULL;

  g_object_get(sfz_file,
	       "sample", &start_sample,
	       NULL);
  
  return(start_sample);
}

/**
 * ags_sfz_file_set_sample:
 * @sfz_file: the #AgsSFZFile
 * @sample: (element-type AgsAudio.SFZSample) (transfer none): the #GList-struct containing #AgsSFZSample
 *
 * Set the sample field of @sfz_file
 * 
 * Since: 3.17.0
 */
void
ags_sfz_file_set_sample(AgsSFZFile *sfz_file,
			GList *sample)
{
  GRecMutex *sfz_file_mutex;

  if(!AGS_IS_SFZ_FILE(sfz_file)){
    return;
  }

  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

  g_rec_mutex_lock(sfz_file_mutex);
  
  g_list_free_full(sfz_file->sample,
		   (GDestroyNotify) g_object_unref);
  
  sfz_file->sample = g_list_copy_deep(sample,
				      (GCopyFunc) g_object_ref,
				      NULL);
  
  g_rec_mutex_unlock(sfz_file_mutex);
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
 * Since: 3.0.0
 */
gboolean
ags_sfz_file_select_sample(AgsSFZFile *sfz_file,
			   guint sample_index)
{
  GList *start_list, *list;

  gboolean success;

  GRecMutex *sfz_file_mutex;
  
  if(!AGS_IS_SFZ_FILE(sfz_file)){
    return(FALSE);
  }

  /* get sfz file mutex */
  sfz_file_mutex = AGS_SFZ_FILE_GET_OBJ_MUTEX(sfz_file);

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
      filename = NULL;
      
      g_object_get(list->data,
		   "filename", &filename,
		   NULL);

      g_rec_mutex_lock(sfz_file_mutex);
      
      sfz_file->index_selected[AGS_SFZ_LEVEL_SAMPLE] = sample_index;

      g_free(sfz_file->name_selected[AGS_SFZ_LEVEL_SAMPLE]);
      
      sfz_file->name_selected[AGS_SFZ_LEVEL_SAMPLE] = filename;

      /* container */
      sfz_file->current_sample = (AgsSFZSample *) list->data;

      g_rec_mutex_unlock(sfz_file_mutex);
    }

    g_list_free_full(start_list,
		     g_object_unref);
  }
  
  return(success);
}

/**
 * ags_sfz_file_get_range:
 * @sfz_file: the #AgsSFZFile
 * @hikey: (out): the return location of key high
 * @lokey: (out): the return location of key low
 * 
 * Get range of @sfz_file, set return location @hikey and @lokey.
 * 
 * Since: 3.0.0
 */
void
ags_sfz_file_get_range(AgsSFZFile *sfz_file,
		       glong *hikey, glong *lokey)
{
  GList *start_group, *group;
  GList *start_region, *region;

  gchar *str;
  
  glong upper, lower;
  glong value;
  int retval;
  
  if(!AGS_IS_SFZ_FILE(sfz_file)){
    return;
  }

  g_object_get(sfz_file,
	       "group", &start_group,
	       "region", &start_region,
	       NULL);

  upper = 0;
  lower = AGS_SFZ_FILE_LOOP_MAX;

  group = start_group;

  while(group != NULL){
    /* hikey */
    str = ags_sfz_group_lookup_control(group->data,
				       "hikey");

    value = 0;
    
    if(str != NULL){
      retval = sscanf(str, "%3ld", &value);

      if(retval <= 0){
	glong tmp;
	guint tmp_retval;
	
	tmp_retval = ags_diatonic_scale_note_to_midi_key(str,
							 &tmp);

	if(retval > 0){
	  value = tmp;
	}
      }
    }

    if(value > upper){
      upper = value;
    }
    
    /* lokey */
    str = ags_sfz_group_lookup_control(group->data,
				       "hikey");

    value = AGS_SFZ_FILE_LOOP_MAX;
    
    if(str != NULL){
      retval = sscanf(str, "%3ld", &value);

      if(retval <= 0){
	glong tmp;
	guint tmp_retval;
	
	tmp_retval = ags_diatonic_scale_note_to_midi_key(str,
							 &tmp);

	if(retval > 0){
	  value = tmp;
	}
      }
    }

    if(value < lower){
      lower = value;
    }

    /* iterate */
    group = group->next;
  }

  region = start_region;

  while(region != NULL){
    /* hikey */
    str = ags_sfz_region_lookup_control(region->data,
					"hikey");

    value = 0;
    
    if(str != NULL){
      retval = sscanf(str, "%3ld", &value);

      if(retval <= 0){
	glong tmp;
	guint tmp_retval;
	
	tmp_retval = ags_diatonic_scale_note_to_midi_key(str,
							 &tmp);

	if(retval > 0){
	  value = tmp;
	}
      }
    }

    if(value > upper){
      upper = value;
    }
    
    /* lokey */
    str = ags_sfz_region_lookup_control(region->data,
					"hikey");

    value = AGS_SFZ_FILE_LOOP_MAX;
    
    if(str != NULL){
      retval = sscanf(str, "%3ld", &value);

      if(retval <= 0){
	glong tmp;
	guint tmp_retval;
	
	tmp_retval = ags_diatonic_scale_note_to_midi_key(str,
							 &tmp);

	if(retval > 0){
	  value = tmp;
	}
      }
    }

    if(value < lower){
      lower = value;
    }

    /* iterate */
    region = region->next;
  }

  /* set return location */
  if(lokey < hikey){
    if(lokey != NULL){
      lokey[0] = lower;
    }

    if(hikey != NULL){
      hikey[0] = upper;
    }
  }else{
    if(lokey != NULL){
      lokey[0] = 49;
    }

    if(hikey != NULL){
      hikey[0] = 49;
    }
  }
}

/**
 * ags_sfz_file_check_suffix:
 * @filename: the filename
 * 
 * Check @filename's suffix to be supported.
 * 
 * Returns: %TRUE if supported, else %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_sfz_file_check_suffix(gchar *filename)
{
  if(g_str_has_suffix(filename, ".sfz")){
    return(TRUE);
  }

  return(FALSE);
}

gchar*
ags_sfz_file_parse_skip_comments_and_blanks(gchar *buffer, gsize buffer_length,
					    gchar **iter)
{
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
  for(; (look_ahead < &(buffer[buffer_length])) && *look_ahead != '\0';){
    /* skip comments */
    if(buffer == look_ahead){
      if(look_ahead + 1 < &(buffer[buffer_length]) && buffer[0] == '/' && buffer[1] == '/'){
	next = strchr(look_ahead, '\n');

	if(next != NULL){
	  look_ahead = next + 1;
	}else{
	  look_ahead = &(buffer[buffer_length]);

	  break;
	}
	  
	continue;
      }
    }else if(buffer[look_ahead - buffer - 1] == '\n' && look_ahead + 1 < &(buffer[buffer_length]) && look_ahead[0] == '/' && look_ahead[1] == '/'){
      next = strchr(look_ahead, '\n');
      
      if(next != NULL){
	look_ahead = next + 1;
      }else{
	look_ahead = &(buffer[buffer_length]);

	break;
      }
	
      continue;
    }

    /* spaces */
    if(!(look_ahead[0] == ' ' || look_ahead[0] == '\t' || look_ahead[0] == '\n' || look_ahead[0] == '\r')){
      break;
    }else{
      look_ahead++;
    }
  }

  return(look_ahead);
}

/**
 * ags_sfz_file_parse:
 * @sfz_file: the #AgsSFZFile
 *
 * Parse @sfz_file.
 *
 * Since: 3.0.0
 */
void
ags_sfz_file_parse(AgsSFZFile *sfz_file)
{
  AgsSFZGroup *current_group;
  AgsSFZRegion *current_region;
  AgsSFZSample *current_sample;
  
  FILE *file;

  struct stat *sb;
  
  GList *start_list, *list;  

  regmatch_t match_arr[2];

  gchar *filename;
  gchar *buffer, *iter;

  size_t n_read;
  gint nth_group;
  gint nth_region;
  gboolean group_active;
  gboolean region_active;
  gboolean sample_active;

  GRecMutex *sfz_file_mutex;

  static regex_t opcode_regex;
  static regex_t opcode_regex_next;

  static const gchar *opcode_pattern = "^([a-zA-Z_]+)\\=";
  static const gchar *opcode_pattern_next = "([a-zA-Z_]+)\\=";

  static const size_t max_matches = 2;
  static gboolean regex_compiled = FALSE;

  if(!AGS_IS_SFZ_FILE(sfz_file)){
    return;
  }

  g_message("SFZ parse");
  
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

  g_free(filename);
  
  /* read SFZ */
  g_rec_mutex_lock(sfz_file_mutex);

  file = sfz_file->file;

  g_rec_mutex_unlock(sfz_file_mutex);

  buffer = (gchar *) malloc((sb->st_size + 1) * sizeof(gchar));

  if(buffer == NULL){
    free(sb);

    return;
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

  g_mutex_lock(&regex_mutex);
  
  if(!regex_compiled){
    regex_compiled = TRUE;
    
    ags_regcomp(&opcode_regex, opcode_pattern, REG_EXTENDED);
    ags_regcomp(&opcode_regex_next, opcode_pattern_next, REG_EXTENDED);
  }

  g_mutex_unlock(&regex_mutex);
  
  do{    
    /* skip blanks and comments */
    iter = ags_sfz_file_parse_skip_comments_and_blanks(buffer, sb->st_size,
						       &iter);

    if(iter >= &(buffer[sb->st_size])){
      break;
    }

    if(!g_ascii_strncasecmp(iter,
			    "<group>",
			    7)){
      g_message("SFZ group");
      
      nth_group++;

      group_active = TRUE;
      region_active = FALSE;
      sample_active = FALSE;

      current_group = ags_sfz_group_new();
      g_object_set(sfz_file,
		   "group", current_group,
		   NULL);

      iter += 7;
    }else if(!g_ascii_strncasecmp(iter,
				  "<region>",
				  8)){
      g_message("SFZ region");
      
      nth_region++;

      region_active = TRUE;
      sample_active = FALSE;

      current_region = ags_sfz_region_new();
      g_object_set(sfz_file,
		   "region", current_region,
		   NULL);

      iter += 8;
    }else if(ags_regexec(&opcode_regex, iter, max_matches, match_arr, 0) == 0){
      gchar *opcode;
      gchar *str;
      gchar *next, *tmp0_next, *tmp1_next;

      iter += match_arr[1].rm_so;
      
      opcode = g_strndup(iter,
			 match_arr[1].rm_eo - match_arr[1].rm_so);

      iter += strlen(opcode) + 1;
      
      if(ags_regexec(&opcode_regex_next, iter, max_matches, match_arr, 0) == 0){
	tmp0_next = strchr(iter, '\n');
	tmp1_next = strchr(iter, '\r');

	if((tmp0_next != NULL || tmp1_next != NULL) &&
	   ((tmp0_next != NULL && tmp0_next < iter + match_arr[1].rm_so) || (tmp1_next != NULL && tmp1_next < iter + match_arr[1].rm_so))){
	  if(tmp0_next != NULL && (tmp1_next == NULL || tmp0_next < tmp1_next)){
	    next = tmp0_next;
	  }else{
	    next = tmp1_next;
	  }
	}else{	
	  next = iter + match_arr[1].rm_so;
	}
      }else{
	tmp0_next = strchr(iter, '\n');
	tmp1_next = strchr(iter, '\r');

	if(tmp0_next != NULL || tmp1_next != NULL){
	  if(tmp0_next != NULL && (tmp1_next == NULL || tmp0_next < tmp1_next)){
	    next = tmp0_next;
	  }else{
	    next = tmp1_next;
	  }
	}else{
	  next = &(buffer[sb->st_size]);
	}
      }

      while(next > iter){
	if((next - 1)[0] == ' '){
	  next--;
	}else{
	  break;
	}
      }

      str = g_strndup(iter,
		      next - iter);

      iter = next;

      g_message("opcode - %s=%s", opcode, str);
      
      if(!g_ascii_strncasecmp(opcode,
			      "sample",
			      6)){
	gchar *filename;
	gchar *tmp;

	gboolean success;
	
	tmp = str;

	while((tmp = strchr(tmp, '\\')) != NULL){
	  tmp[0] = '/';
	  
	  tmp++;
	}
	
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
	g_object_set(current_sample,
		     "group", current_group,
		     "region", current_region,
		     NULL);
	
	success = ags_sound_resource_open(AGS_SOUND_RESOURCE(current_sample),
					  filename);

	if(!success){
	  g_message("failed to open %s", filename);
	}
	
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

	g_free(filename);
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
    }else{
      /* bad byte */
      iter++;
    }
  }while(iter < &(buffer[sb->st_size]));
  
  free(sb);
  free(buffer);

  /* apply loop start/end */
  g_object_get(sfz_file,
	       "sample", &start_list,
	       NULL);

  list = start_list;

  while(list != NULL){
    AgsSFZGroup *group;
    AgsSFZRegion *region;
    AgsSFZSample *sample;
    
    guint loop_start, loop_end;

    sample = (AgsSFZSample *) list->data;

    loop_start = 0;
    loop_end = 0;
    
    g_object_get(sample,
		 "group", &group,
		 "region", &region,
		 NULL);

    /* check group */
    if(group != NULL){
      gchar *str;

      str = ags_sfz_group_lookup_control(group,
					 "loop_start");

      if(str != NULL){
	loop_start = g_ascii_strtoull(str,
				      NULL,
				      10);

	g_free(str);
      }

      str = ags_sfz_group_lookup_control(group,
					 "loop_end");

      if(str != NULL){
	loop_end = g_ascii_strtoull(str,
				    NULL,
				    10);

	g_free(str);
      }
    }

    /* check region */
    if(region != NULL){
      gchar *str;

      str = ags_sfz_region_lookup_control(region,
					 "loop_start");

      if(str != NULL){
	loop_start = g_ascii_strtoull(str,
				      NULL,
				      10);

	g_free(str);
      }

      str = ags_sfz_region_lookup_control(region,
					 "loop_end");

      if(str != NULL){
	loop_end = g_ascii_strtoull(str,
				    NULL,
				    10);

	g_free(str);
      }
    }
    
    g_object_set(sample,
		 "loop-start", loop_start,
		 "loop-end", loop_end,
		 NULL);

    /* iterate */
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

/**
 * ags_sfz_file_new:
 *
 * Creates an #AgsSFZFile.
 *
 * Returns: an empty #AgsSFZFile.
 *
 * Since: 3.0.0
 */
AgsSFZFile*
ags_sfz_file_new()
{
  AgsSFZFile *sfz_file;

  sfz_file = (AgsSFZFile *) g_object_new(AGS_TYPE_SFZ_FILE,
					 NULL);
  
  return(sfz_file);
}
