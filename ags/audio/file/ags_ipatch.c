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

#include <ags/audio/file/ags_ipatch.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>
#include <ags/audio/file/ags_ipatch_sf2_reader.h>

#include <ags/i18n.h>

void ags_ipatch_class_init(AgsIpatchClass *ipatch);
void ags_ipatch_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ipatch_sound_container_interface_init(AgsSoundContainerInterface *sound_container);
void ags_ipatch_sound_resource_interface_init(AgsSoundResourceInterface *sound_resource);
void ags_ipatch_init(AgsIpatch *ipatch);
void ags_ipatch_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_ipatch_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_ipatch_finalize(GObject *gobject);

gboolean ags_ipatch_open(AgsSoundContainer *sound_container, gchar *filename);
guint ags_ipatch_get_level_count(AgsSoundContainer *sound_container);
guint ags_ipatch_get_nesting_level(AgsSoundContainer *sound_container);
gchar* ags_ipatch_get_level_id(AgsSoundContainer *sound_container);
guint ags_ipatch_get_level_index(AgsSoundContainer *sound_container);
guint ags_ipatch_level_up(AgsSoundContainer *sound_container,
			  guint level_count);
guint ags_ipatch_select_level_by_id(AgsSoundContainer *sound_container,
				    gchar *level_id);
guint ags_ipatch_select_level_by_index(AgsSoundContainer *sound_container,
				       guint level_index);
gchar** ags_ipatch_get_sublevel_name(AgsSoundContainer *sound_container);
GList* ags_ipatch_get_resource_all(AgsSoundContainer *sound_container);
GList* ags_ipatch_get_resource_by_name(AgsSoundContainer *sound_container,
				       gchar *resource_name);
GList* ags_ipatch_get_resource_by_index(AgsSoundContainer *sound_container,
					guint resource_index);
void ags_ipatch_close(AgsSoundResource *sound_resource);

gboolean ags_ipatch_info(AgsSoundResource *sound_resource,
			 guint *frame_count,
			 guint *loop_start, guint *loop_end);
void ags_ipatch_set_presets(AgsSoundResource *sound_resource,
			    guint channels,
			    guint samplerate,
			    guint buffer_size,
			    guint format);
void ags_ipatch_get_presets(AgsSoundResource *sound_resource,
			    guint *channels,
			    guint *samplerate,
			    guint *buffer_size,
			    guint *format);
guint ags_ipatch_read(AgsSoundResource *sound_resource,
		      void *dbuffer,
		      guint audio_channel,
		      guint frame_count, guint format);
void ags_ipatch_write(AgsSoundResource *sound_resource,
		      void *sbuffer,
		      guint audio_channel,
		      guint frame_count, guint format);
void ags_ipatch_flush(AgsSoundResource *sound_resource);
void ags_ipatch_seek(AgsSoundResource *sound_resource,
		     guint frame_count, gint whence);

/**
 * SECTION:ags_ipatch
 * @short_description: Libinstpatch wrapper
 * @title: AgsIpatch
 * @section_id:
 * @include: ags/audio/file/ags_ipatch.h
 *
 * #AgsIpatch is the base object to ineract with libinstpatch.
 */

static gpointer ags_ipatch_parent_class = NULL;
static AgsConnectableInterface *ags_ipatch_parent_connectable_interface;
static AgsSoundContainerInterface *ags_ipatch_parent_sound_container_interface;
static AgsSoundResourceInterface *ags_ipatch_parent_sound_resource_interface;
 
enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_FILENAME,
  PROP_MODE,
};

GType
ags_ipatch_get_type()
{
  static GType ags_type_ipatch = 0;

  if(!ags_type_ipatch){
    static const GTypeInfo ags_ipatch_info = {
      sizeof (AgsIpatchClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ipatch_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsIpatch),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ipatch_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ipatch_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_sound_container_interface_info = {
      (GInterfaceInitFunc) ags_ipatch_sound_container_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_sound_resource_interface_info = {
      (GInterfaceInitFunc) ags_ipatch_sound_resource_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ipatch = g_type_register_static(G_TYPE_OBJECT,
					     "AgsIpatch",
					     &ags_ipatch_info,
					     0);

    g_type_add_interface_static(ags_type_ipatch,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ipatch,
				AGS_TYPE_SOUND_CONTAINER,
				&ags_sound_container_interface_info);

    g_type_add_interface_static(ags_type_ipatch,
				AGS_TYPE_SOUND_RESOURCE,
				&ags_sound_resource_interface_info);
  }
  
  return (ags_type_ipatch);
}

void
ags_ipatch_class_init(AgsIpatchClass *ipatch)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_ipatch_parent_class = g_type_class_peek_parent(ipatch);

  /* GObjectClass */
  gobject = (GObjectClass *) ipatch;

  gobject->set_property = ags_ipatch_set_property;
  gobject->get_property = ags_ipatch_get_property;

  gobject->finalize = ags_ipatch_finalize;

  /* properties */
  /**
   * AgsIpatch:soundcard:
   *
   * The assigned soundcard.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("soundcard of ipatch"),
				   i18n_pspec("The soundcard what ipatch has it's presets"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsIpatch:filename:
   *
   * The assigned filename.
   * 
   * Since: 2.0.0
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
   * AgsIpatch:mode:
   *
   * The assigned mode.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("mode",
				   i18n_pspec("the mode"),
				   i18n_pspec("The mode to open the file"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MODE,
				  param_spec);
}

void
ags_ipatch_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_ipatch_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_ipatch_sound_container_interface_init(AgsSoundContainerInterface *sound_container)
{
  ags_ipatch_parent_sound_container_interface = g_type_interface_peek_parent(sound_container);

  sound_container->open = ags_ipatch_open;
  
  sound_container->get_level_count = ags_ipatch_get_level_count;
  sound_container->get_nesting_level = ags_ipatch_get_nesting_level;
  
  sound_container->get_level_id = ags_ipatch_get_level_id;
  sound_container->get_level_index = ags_ipatch_get_level_index;
  
  sound_container->get_sublevel_name = ags_ipatch_get_sublevel_name;
  
  sound_container->level_up = ags_ipatch_level_up;
  sound_container->select_level_by_id = ags_ipatch_select_level_by_id;
  sound_container->select_level_by_index = ags_ipatch_select_level_by_index;
  
  sound_container->get_resource_all = ags_ipatch_get_resource_all;
  sound_container->get_resource_by_name = ags_ipatch_get_resource_by_name;
  sound_container->get_resource_by_index = ags_ipatch_get_resource_by_index;

  sound_container->close = ags_ipatch_close;
}

void
ags_ipatch_sound_resource_interface_init(AgsSoundResourceInterface *sound_resource)
{
  ags_ipatch_parent_sound_resource_interface = g_type_interface_peek_parent(sound_resource);

  sound_resource->open = NULL;
  sound_resource->rw_open = NULL;

  sound_resource->load = NULL;

  sound_resource->info = ags_ipatch_info;

  sound_resource->set_presets = ags_ipatch_set_presets;
  sound_resource->get_presets = ags_ipatch_get_presets;
  
  sound_resource->read = ags_ipatch_read;

  sound_resource->write = ags_ipatch_write;
  sound_resource->flush = ags_ipatch_flush;
  
  sound_resource->seek = ags_ipatch_seek;

  sound_resource->close = NULL;
}

void
ags_ipatch_init(AgsIpatch *ipatch)
{
  ipatch->flags = 0;

  ipatch->soundcard = NULL;
  ipatch->audio_signal= NULL;

  ipatch->file = NULL;

  ipatch->filename = NULL;
  ipatch->mode = AGS_IPATCH_READ;

  ipatch->handle = NULL;
  ipatch->error = NULL;

  ipatch->base = NULL;
  ipatch->reader = NULL;

  ipatch->samples = NULL;
  ipatch->iter = NULL;

  ipatch->nesting_level = 0;

  ipatch->level_id = NULL;
  ipatch->level_index = 0;
}

void
ags_ipatch_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;
      
      soundcard = (GObject *) g_value_get_object(value);

      if(soundcard == ((GObject *) ipatch->soundcard)){
	return;
      }

      if(ipatch->soundcard != NULL){
	g_object_unref(ipatch->soundcard);
      }
      
      if(soundcard != NULL){
	g_object_ref(G_OBJECT(soundcard));
      }
      
      ipatch->soundcard = (GObject *) soundcard;
    }
    break;
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = (gchar *) g_value_get_string(value);

      ags_sound_container_open(AGS_SOUND_CONTAINER(sound_container), filename);
    }
    break;
  case PROP_MODE:
    {
      gchar *mode;
      
      mode = (gchar *) g_value_get_string(value);
      
      ipatch->mode = mode;

#ifdef AGS_WITH_LIBINSTPATCH      
      if(ipatch->handle != NULL){
	GError *error;

	error = NULL;

	ipatch_file_default_open_method(ipatch->handle,
					mode,
					&error);

	if(error != NULL){
	  g_warning("%s", error->message);
	}
      }
#endif
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ipatch_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, ipatch->soundcard);
    }
    break;
  case PROP_FILENAME:
    {
      g_value_set_string(value, ipatch->filename);
    }
    break;
  case PROP_MODE:
    {
      g_value_set_string(value, ipatch->mode);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

gboolean
ags_ipatch_open(AgsSoundContainer *sound_container, gchar *filename)
{
  AgsIpatch *ipatch;

#ifdef AGS_WITH_LIBINSTPATCH
  IpatchFileIOFuncs *io_funcs;
#endif

  GError *error;

  ipatch = AGS_IPATCH(sound_container);

  /* close current */
  if(ipatch->filename != NULL){
    ags_sound_container_close(sound_container);
    
    g_free(ipatch->filename);
  }

  /* check suffix */
  ipatch->filename = g_strdup(filename);
  
  if(!ags_ipatch_check_suffix(filename)){
    return(FALSE);
  }
  
  error = NULL;

#ifdef AGS_WITH_LIBINSTPATCH
  ipatch->handle = ipatch_file_identify_open(ipatch->filename,
					     &error);
#endif
  
  if(error != NULL){
    g_warning("%s", error->message);
  }

  if(ipatch->handle == NULL){
    return(FALSE);
  }

  if(IPATCH_IS_DLS_FILE(ipatch->handle->file)){
    ipatch->flags |= AGS_IPATCH_DLS2;

    //TODO:JK: implement me
  }else if(IPATCH_IS_SF2_FILE(ipatch->handle->file)){
    ipatch->flags |= AGS_IPATCH_SF2;

    //TODO:JK: implement me

    /*  */
    ipatch->reader = (GObject *) ags_ipatch_sf2_reader_new();
    AGS_IPATCH_SF2_READER(ipatch->reader)->ipatch = ipatch;

    AGS_IPATCH_SF2_READER(ipatch->reader)->reader = ipatch_sf2_reader_new(ipatch->handle);

    error = NULL;
    ipatch->base = (IpatchBase *) ipatch_sf2_reader_load(AGS_IPATCH_SF2_READER(ipatch->reader)->reader,
							 &error);

    error = NULL;
    AGS_IPATCH_SF2_READER(ipatch->reader)->sf2 = (IpatchSF2 *) ipatch_convert_object_to_type((GObject *) ipatch->handle->file,
											     IPATCH_TYPE_SF2,
											     &error);

    if(error != NULL){
      g_warning("%s", error->message);

      return(FALSE);
    }

    /* load samples */
    ipatch->samples = (IpatchList *) ipatch_container_get_children(IPATCH_CONTAINER(ipatch->base),
								   IPATCH_TYPE_SF2_SAMPLE);
    
    while(g_static_rec_mutex_unlock_full(((IpatchItem *) (ipatch->base))->mutex) != 0);
  }else if(IPATCH_IS_GIG_FILE(ipatch->handle->file)){
    ipatch->flags |= AGS_IPATCH_GIG;

    //TODO:JK: implement me
  }

  return(TRUE);
}

guint
ags_ipatch_get_level_count(AgsSoundContainer *sound_container)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(sound_container);

  if((AGS_IPATCH_DLS2 & (ipatch->flags)) != 0){
    return(3);
  }else if((AGS_IPATCH_SF2 & (ipatch->flags)) != 0){
    return(4);
  }else if((AGS_IPATCH_GIG & (ipatch->flags)) != 0){
    return(3);
  }

  return(0);
}

guint
ags_ipatch_get_nesting_level(AgsSoundContainer *sound_container)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(sound_container);
  
  return(ipatch->nesting_level);
}

gchar*
ags_ipatch_get_level_id(AgsSoundContainer *sound_container)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(sound_container);
  
  return(ipatch->level_id);
}

guint
ags_ipatch_get_level_index(AgsSoundContainer *sound_container)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(sound_container);
  
  return(ipatch->level_index);
}

gchar**
ags_ipatch_get_sublevel_name(AgsSoundContainer *sound_container)
{
  AgsIpatch *ipatch;
  
  AgsIpatch *ipatch;
  AgsIpatchSF2Reader *ipatch_sf2_reader;

#ifdef AGS_WITH_LIBINSTPATCH
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
#endif

  GList *list;

  gchar **sublevel_name;
  gchar *name;

  guint sublevel;
  guint i;

  ipatch = AGS_IPATCH(sound_container);
  
  list = NULL;

  sublevel_name = NULL;
  i  = 0;
  
#ifdef AGS_WITH_LIBINSTPATCH
  if((AGS_IPATCH_SF2 & (ipatch->flags)) != 0){
    ipatch_sf2_reader = AGS_IPATCH_SF2_READER(ipatch->reader);
    
    sublevel = ipatch->nesting_level;

    switch(sublevel){
    case AGS_SF2_FILENAME:
      {
	sublevel_name = (gchar **) malloc(2 * sizeof(gchar*));

	sublevel_name[0] = ipatch_sf2_reader->ipatch->filename;
	sublevel_name[1] = NULL;

	return(sublevel_name);
      }
    case AGS_SF2_PHDR:
      {
	ipatch_list = ipatch_container_get_children(IPATCH_CONTAINER(ipatch_sf2_reader->sf2),
						    IPATCH_TYPE_SF2_PRESET);

	while(g_static_rec_mutex_unlock_full(((IpatchItem *) (ipatch_sf2_reader->sf2))->mutex) != 0);
	
	if(ipatch_list != NULL){
	  list = ipatch_list->items;
	}else{
	  return(sublevel_name);
	}
      }
      break;
    case AGS_SF2_IHDR:
      {
	GList *tmp;

	ipatch_list = ipatch_sf2_preset_get_zones(ipatch_sf2_reader->preset);
	tmp = ipatch_list->items;

	while(tmp != NULL){
	  list = g_list_prepend(list, ipatch_sf2_zone_get_link_item(IPATCH_SF2_ZONE(tmp->data)));

	  tmp = tmp->next;
	}

	if(list != NULL){
	  list = g_list_reverse(list);
	}else{
	  return(sublevel_name);
	}
      }
      break;
    case AGS_SF2_SHDR:
      {
	GList *tmp;

	ipatch_list = ipatch_sf2_preset_get_zones(ipatch_sf2_reader->instrument);
	tmp = ipatch_list->items;
	
	while(tmp != NULL){
	  list = g_list_prepend(list, ipatch_sf2_zone_get_link_item(IPATCH_SF2_ZONE(tmp->data)));

	  tmp = tmp->next;
	}

	if(list != NULL){
	  list = g_list_reverse(list);
	}else{
	  return(sublevel_name);
	}
      }
      break;
    };
  }

  for(i = 0; list != NULL; i++){
    switch(sublevel){
    case AGS_SF2_PHDR:
      {
	if(IPATCH_IS_SF2_PRESET(list->data)){
	  if(sublevel_name == NULL){
	    sublevel_name = (gchar **) malloc(2 * sizeof(gchar *));
	  }else{
	    sublevel_name = (gchar **) realloc(sublevel_name,
					       (i + 2) * sizeof(gchar *));
	  }
	  
	  sublevel_name[i] = ipatch_sf2_preset_get_name(IPATCH_SF2_PRESET(list->data));
	}
      }
      break;
    case AGS_SF2_IHDR:
      {
	if(IPATCH_IS_SF2_INST(list->data)){
	  if(sublevel_name == NULL){
	    sublevel_name = (gchar **) malloc(2 * sizeof(gchar *));
	  }else{
	    sublevel_name = (gchar **) realloc(sublevel_name,
					       (i + 2) * sizeof(gchar *));
	  }

	  sublevel_name[i] = ipatch_sf2_inst_get_name(IPATCH_SF2_INST(list->data));
	}
      }
      break;
    case AGS_SF2_SHDR:
      {
	if(IPATCH_IS_SF2_SAMPLE(list->data)){
	  if(sublevel_name == NULL){
	    sublevel_name = (gchar **) malloc(2 * sizeof(gchar *));
	  }else{
	    sublevel_name = (gchar **) realloc(sublevel_name,
					       (i + 2) * sizeof(gchar *));
	  }

	  sublevel_name[i] = ipatch_sf2_sample_get_name(IPATCH_SF2_SAMPLE(list->data));
	}
      }
      break;
    };
    
    list = list->next;
  }

  if(i > 0){
    sublevel_name[i] = NULL;
  }
#endif
  
  return(sublevel_name);
}

guint
ags_ipatch_level_up(AgsSoundContainer *sound_container,
		    guint level_count)
{
  AgsIpatch *ipatch;
  
  guint retval;
  
  ipatch = AGS_IPATCH(playable);

  if(level_count == 0){
    return(0);
  }

  if(ipatch->nesting_level >= level_count){
    retval = level_count;
    
    ipatch->nesting_level -= level_count;
  }else{
    retval = ipatch->nesting_level;
    
    ipatch->nesting_level = 0;
  }

  return(retval);
}

guint
ags_ipatch_select_level_by_id(AgsSoundContainer *sound_container,
			      gchar *level_id)
{
  AgsIpatch *ipatch;

  gchar **preset, **instrument, **sample;
  gchar **preset_iter, **instrument_iter, **sample_iter;

  gboolean success;

  ipatch = AGS_IPATCH(sound_container);

  /* check filename */
  ipatch->nesting_level = 0;

  if(level_id == NULL){
    return(0);
  }
  
  if(ipatch->filename == NULL){
    return(0);
  }

  success = (!g_strcmp0(ipatch->filename,
			level_id)) ? TRUE: FALSE;
  
  if(success){
    ipatch->level_id = g_strdup(level_id);
    ipatch->level_index = 0;
    
    return(0);
  }

  if(){
  }else if(){
    preset = ags_ipatch_sf2_reader_get_preset_all();
  }else if(){
  }

  /* check preset */
  ipatch->nesting_level = 1;

  preset = 
    sublevel_name = ags_ipatch_get_sublevel_name(sound_container);

  if(sublevel_name == NULL){
    ipatch->nesting_level = 0;
    
    return(0);
  }

  for(iter = sublevel_name; *iter != NULL; iter++){
    if(!g_strcmp0(level_id,
		  *iter)){
      success = TRUE;

      break;
    }
  }

  if(success){
    ipatch->level_id = g_strdup(level_id);
    ipatch->level_index = i;

    g_strfreev(preset);
    
    return(1);
  }
      
  /* check instrument */
  ipatch->nesting_level = 2;

  instrument = 
    sublevel_name = ags_ipatch_get_sublevel_name(sound_container);

  if(sublevel_name == NULL){
    ipatch->nesting_level = 0;

    g_strfreev(preset);
    
    return(0);
  }

  for(iter = sublevel_name; *iter != NULL; iter++){
    if(!g_strcmp0(level_id,
		  *iter)){
      success = TRUE;

      break;
    }
  }

  if(success){
    ipatch->level_id = g_strdup(level_id);
    ipatch->level_index = i;
    
    g_strfreev(preset);
    g_strfreev(instrument);

    return(2);
  }

  /* check preset */
  ipatch->nesting_level = 3;

  sample =
    sublevel_name = ags_ipatch_get_sublevel_name(sound_container);

  if(sublevel_name == NULL){
    ipatch->nesting_level = 0;
    
    return(0);
  }

  for(iter = sublevel_name; *iter != NULL; iter++){
    if(!g_strcmp0(level_id,
		  *iter)){
      success = TRUE;

      break;
    }
  }

  g_strfreev(sublevel_name);

  if(success){
    ipatch->level_id = g_strdup(level_id);
    ipatch->level_index = i;
    
    return(3);
  }

  return(0);
}

guint
ags_ipatch_select_level_by_index(AgsSoundContainer *sound_container,
				 guint level_index)
{
  //TODO:JK: implement me
}

GList*
ags_ipatch_get_resource_all(AgsSoundContainer *sound_container)
{
  //TODO:JK: implement me
}

GList* ags_ipatch_get_resource_by_name(AgsSoundContainer *sound_container,
				       gchar *resource_name)
{
  //TODO:JK: implement me
}

GList* ags_ipatch_get_resource_by_index(AgsSoundContainer *sound_container,
					guint resource_index)
{
  //TODO:JK: implement me
}

void ags_ipatch_close(AgsSoundResource *sound_resource)
{
  //TODO:JK: implement me
}

gboolean ags_ipatch_info(AgsSoundResource *sound_resource,
			 guint *frame_count,
			 guint *loop_start, guint *loop_end)
{
  //TODO:JK: implement me
}

void ags_ipatch_set_presets(AgsSoundResource *sound_resource,
			    guint channels,
			    guint samplerate,
			    guint buffer_size,
			    guint format)
{
  //TODO:JK: implement me
}

void ags_ipatch_get_presets(AgsSoundResource *sound_resource,
			    guint *channels,
			    guint *samplerate,
			    guint *buffer_size,
			    guint *format)
{
  //TODO:JK: implement me
}

guint ags_ipatch_read(AgsSoundResource *sound_resource,
		      void *dbuffer,
		      guint audio_channel,
		      guint frame_count, guint format)
{
  //TODO:JK: implement me
}

void ags_ipatch_write(AgsSoundResource *sound_resource,
		      void *sbuffer,
		      guint audio_channel,
		      guint frame_count, guint format)
{
  //TODO:JK: implement me
}

void ags_ipatch_flush(AgsSoundResource *sound_resource)
{
  //TODO:JK: implement me
}

void ags_ipatch_seek(AgsSoundResource *sound_resource,
		     guint frame_count, gint whence)
{
  //TODO:JK: implement me
}

/**
 * ags_ipatch_check_suffix:
 * @filename: the filename
 * 
 * Check @filename's suffix to be supported.
 * 
 * Returns: %TRUE if supported, else %FALSE
 * 
 * Since: 2.0.0
 */
gboolean
ags_ipatch_check_suffix(gchar *filename)
{
  if(g_str_has_suffix(filename, ".sf2") ||
     g_str_has_suffix(filename, ".dls") ||
     g_str_has_suffix(filename, ".gig")){
    return(TRUE);
  }

  return(FALSE);
}

void
ags_ipatch_level_select(AgsPlayable *playable,
			guint nth_level, gchar *sublevel_name,
			GError **error)
{
  AgsIpatch *ipatch;
  AgsIpatchSF2Reader *ipatch_sf2_reader;
  gboolean success;

  GError *this_error;

  ipatch = AGS_IPATCH(playable);

#ifdef AGS_WITH_LIBINSTPATCH
  if((AGS_IPATCH_SF2 & (ipatch->flags)) != 0){
    ipatch_sf2_reader = AGS_IPATCH_SF2_READER(ipatch->reader);

    //TODO:JK: apply mods and gens

    if(sublevel_name == NULL){
      ipatch->nth_level = 0;
      ipatch_sf2_reader->selected[0] = NULL;
    }else{
      IpatchList *ipatch_list;
      IpatchItem *ipatch_item;
      GList *list;

      if(nth_level == 0 && !g_strcmp0(ipatch_sf2_reader->ipatch->filename, sublevel_name)){
	ipatch->nth_level = 0;

	if(ipatch_sf2_reader->selected[0] != NULL){
	  g_free(ipatch_sf2_reader->selected[0]);
	}
	
	ipatch_sf2_reader->selected[0] = g_strdup(sublevel_name);
	return;
      }

      if(nth_level == 1){
	ipatch->nth_level = 1;

	if(ipatch_sf2_reader->selected[1] != NULL){
	  g_free(ipatch_sf2_reader->selected[1]);
	}
	
	ipatch_sf2_reader->selected[1] = g_strdup(sublevel_name);

	/* preset */
	ipatch_list = ipatch_container_get_children(IPATCH_CONTAINER(ipatch_sf2_reader->sf2),
						    IPATCH_TYPE_SF2_PRESET);

	while(g_static_rec_mutex_unlock_full(((IpatchItem *) (ipatch_sf2_reader->sf2))->mutex) != 0);

	if(ipatch_list == NULL){
	  ipatch->iter = NULL;
	    
	  return;
	}
	  
	list = ipatch_list->items;
	
	while(list != NULL){
	  if(!g_strcmp0(ipatch_sf2_preset_get_name(IPATCH_SF2_PRESET(list->data)), sublevel_name)){
	    /* some extra code for bank and program */
	    //	    ipatch_sf2_preset_get_midi_locale(IPATCH_SF2_PRESET(list->data),
	    //				      &(ipatch_sf2_reader->bank),
	    //				      &(ipatch_sf2_reader->program));

	    //	    g_message("bank %d program %d\n", ipatch_sf2_reader->bank, ipatch_sf2_reader->program);

	    this_error = NULL;
	    ipatch_sf2_reader->preset = (IpatchContainer *) IPATCH_SF2_PRESET(list->data);

	    break;
	  }

	  list = list->next;
	}

	ipatch->iter = list;
      }else{
	if(nth_level == 2){
	  GList *tmp;

	  ipatch->nth_level = 2;

	  if(ipatch_sf2_reader->selected[2] != NULL){
	    g_free(ipatch_sf2_reader->selected[2]);
	  }
	  
	  ipatch_sf2_reader->selected[2] = g_strdup(sublevel_name);

	  /* instrument */
	  ipatch_list = ipatch_sf2_preset_get_zones(ipatch_sf2_reader->preset);

	  if(ipatch_list == NULL){
	    ipatch->iter = NULL;

	    return;
	  }
	  
	  list = NULL;
	  tmp = ipatch_list->items;

	  while(tmp != NULL){
	    list = g_list_prepend(list, ipatch_sf2_zone_get_link_item(IPATCH_SF2_ZONE(tmp->data)));

	    if(!g_strcmp0(IPATCH_SF2_INST(list->data)->name, sublevel_name)){
	      ipatch_sf2_reader->instrument = (IpatchContainer *) IPATCH_SF2_INST(list->data);
	    }

	    tmp = tmp->next;
	  }

	  ipatch->iter = g_list_reverse(list);
	}else if(ipatch->nth_level == 3){
	  GList *tmp;

	  ipatch->nth_level = 3;

	  if(ipatch_sf2_reader->selected[3] != NULL){
	    g_free(ipatch_sf2_reader->selected[3]);
	  }
	  
	  ipatch_sf2_reader->selected[3] = g_strdup(sublevel_name);

	  /* sample */
	  ipatch_list = ipatch_sf2_preset_get_zones(ipatch_sf2_reader->instrument);

	  if(ipatch_list == NULL){
	    ipatch->iter = NULL;

	    return;
	  }
	  
	  list = NULL;
	  tmp = ipatch_list->items;
	
	  while(tmp != NULL){
	    list = g_list_prepend(list, ipatch_sf2_zone_get_link_item(IPATCH_SF2_ZONE(tmp->data)));
	    
	    if(!g_ascii_strcasecmp(IPATCH_SF2_SAMPLE(list->data)->name,
				   sublevel_name)){
	      ipatch_sf2_reader->sample = (IpatchContainer *) IPATCH_SF2_SAMPLE(list->data);
	    }

	    tmp = tmp->next;
	  }

	  ipatch->iter = g_list_reverse(list);
	}else{
	  g_set_error(error,
		      AGS_PLAYABLE_ERROR,
		      AGS_PLAYABLE_ERROR_NO_SUCH_LEVEL,
		      "no level called %s in soundfont2 file: %s",
		      sublevel_name, ipatch_sf2_reader->ipatch->filename);
	}
      }
    }
  }
#endif
}

void
ags_ipatch_level_up(AgsPlayable *playable, guint levels, GError **error)
{
  AgsIpatch *ipatch;
  guint i;

  if(levels == 0)
    return;

  ipatch = AGS_IPATCH(playable);

  if(ipatch->nth_level >= levels){
    ipatch->nth_level -= levels;
  }else{
    g_set_error(error,
		AGS_PLAYABLE_ERROR,
		AGS_PLAYABLE_ERROR_NO_SUCH_LEVEL,
		"Not able to go %u steps higher in soundfont2 file: %s",
		levels, ipatch->filename);
  }
}

void
ags_ipatch_iter_start(AgsPlayable *playable)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(playable);

  if(ipatch->nth_level == 3){
    if((AGS_IPATCH_DLS2 & (ipatch->flags)) != 0){
      //TODO:JK: implement me
    }else if((AGS_IPATCH_SF2 & (ipatch->flags)) != 0){
      //TODO:JK: implement me
    }else if((AGS_IPATCH_GIG & (ipatch->flags)) != 0){
      //TODO:JK: implement me
    }
  }
}

gboolean
ags_ipatch_iter_next(AgsPlayable *playable)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(playable);

  if(ipatch->iter != NULL){
    ipatch->iter = ipatch->iter->next;

    return(TRUE);
  }else{
    return(FALSE);
  }
}

void
ags_ipatch_info(AgsPlayable *playable,
		guint *channels, guint *frames,
		guint *loop_start, guint *loop_end,
		GError **error)
{
  AgsIpatch *ipatch;

#ifdef AGS_WITH_LIBINSTPATCH
  IpatchSample *sample;
#endif
  
  ipatch = AGS_IPATCH(playable);

  if(ipatch->iter == NULL){
    if(channels != NULL){
      *channels = 0;
    }

    if(frames != NULL){
      *frames = 0;
    }
  
    if(loop_start != NULL){
      *loop_start = 0;
    }
  
    if(loop_end != NULL){
      *loop_end = 0;
    }

    if(error != NULL){
      g_set_error(error,
		  AGS_PLAYABLE_ERROR,
		  AGS_PLAYABLE_ERROR_NO_SAMPLE,
		  "no sample selected for file: %s",
		  ipatch->filename);
    }

    return;
  }

#ifdef AGS_WITH_LIBINSTPATCH
  sample = IPATCH_SAMPLE(ipatch->iter->data);
  g_object_get(G_OBJECT(sample),
	       "sample-size", frames,
	       "loop-start", loop_start,
	       "loop-end", loop_end,
	       NULL);
#endif
  
  //TODO:JK: verify me
  if(channels != NULL){
    *channels = AGS_IPATCH_DEFAULT_CHANNELS;
  }
}

guint
ags_ipatch_get_samplerate(AgsPlayable *playable)
{
  AgsIpatch *ipatch;

#ifdef AGS_WITH_LIBINSTPATCH
  IpatchSample *sample;
#endif
  
  guint samplerate;
  
  ipatch = AGS_IPATCH(playable);

  samplerate = 0;
  
#ifdef AGS_WITH_LIBINSTPATCH
  if(ipatch->nth_level == 3){
    if(ipatch->iter != NULL){
      sample = IPATCH_SAMPLE(ipatch->iter->data);
    }else{
      sample = NULL;
    }
  }else{
    sample = NULL;

    if((AGS_IPATCH_DLS2 & (ipatch->flags)) != 0){
      //TODO:JK: implement me
    }else if((AGS_IPATCH_SF2 & (ipatch->flags)) != 0){
      AgsIpatchSF2Reader *reader;

      reader = AGS_IPATCH_SF2_READER(ipatch->reader);
      sample = (IpatchSample *) ipatch_sf2_find_sample(reader->sf2,
						       reader->selected[3],
						       NULL);
    }else if((AGS_IPATCH_GIG & (ipatch->flags)) != 0){
      //TODO:JK: implement me
    }
  }

  g_object_get(sample,
	       "sample-rate", &samplerate,
	       NULL);
#endif
  
  return(samplerate);
}

guint
ags_ipatch_get_format(AgsPlayable *playable)
{
  AgsIpatch *ipatch;

#ifdef AGS_WITH_LIBINSTPATCH
  IpatchSample *sample;
#endif
  
  guint format;
  
  ipatch = AGS_IPATCH(playable);

#ifdef AGS_WITH_LIBINSTPATCH
  sample = NULL;

  if(ipatch->nth_level == 3){
    if(ipatch->iter != NULL){
      sample = IPATCH_SAMPLE(ipatch->iter->data);
    }
  }else{
    if((AGS_IPATCH_DLS2 & (ipatch->flags)) != 0){
      //TODO:JK: implement me
    }else if((AGS_IPATCH_SF2 & (ipatch->flags)) != 0){
      AgsIpatchSF2Reader *reader;

      reader = AGS_IPATCH_SF2_READER(ipatch->reader);
      sample = (IpatchSample *) ipatch_sf2_find_sample(reader->sf2,
						       reader->selected[3],
						       NULL);
    }else if((AGS_IPATCH_GIG & (ipatch->flags)) != 0){
      //TODO:JK: implement me
    }
  }

  format = 0;
  
  if(sample != NULL){
    g_object_get(sample,
		 "sample-format", &format,
		 NULL);
  }

  switch(format){
  case IPATCH_SAMPLE_8BIT:
    return(AGS_AUDIO_BUFFER_UTIL_S8);
  case IPATCH_SAMPLE_16BIT:
    return(AGS_AUDIO_BUFFER_UTIL_S16);
  case IPATCH_SAMPLE_24BIT:
    return(AGS_AUDIO_BUFFER_UTIL_S24);
  case IPATCH_SAMPLE_32BIT:
    return(AGS_AUDIO_BUFFER_UTIL_S32);
  case IPATCH_SAMPLE_FLOAT:
    return(AGS_AUDIO_BUFFER_UTIL_FLOAT);
  case IPATCH_SAMPLE_DOUBLE:
    return(AGS_AUDIO_BUFFER_UTIL_DOUBLE);
  default:
    g_warning("ags_ipatch_get_format() - unknown format");
    return(0);
  }
#else
  return(0);
#endif
}

double*
ags_ipatch_read(AgsPlayable *playable, guint channel,
		GError **error)
{
  AgsIpatch *ipatch;

#ifdef AGS_WITH_LIBINSTPATCH
  IpatchSample *sample;
#endif
  
  double *buffer, *source;
  guint channels, frames;
  guint loop_start, loop_end;
  guint i;

  GError *this_error;

  ipatch = AGS_IPATCH(playable);

  this_error = NULL;
  ags_playable_info(playable,
		    &channels, &frames,
		    &loop_start, &loop_end,
		    &this_error);

  if(this_error != NULL){
    g_warning("%s", this_error->message);
  }

  if(channels == 0 ||
     frames == 0){
    buffer = NULL;
  }else{
    buffer = (double *) malloc(channels * frames * sizeof(double));
  }
  
#ifdef AGS_WITH_LIBINSTPATCH
  if(ipatch->nth_level == 3){
    if(AGS_IPATCH_SF2_READER(ipatch->reader)->sample != NULL){
      sample = AGS_IPATCH_SF2_READER(ipatch->reader)->sample;
    }else{
      if(ipatch->iter != NULL){
	sample = IPATCH_SAMPLE(ipatch->iter->data);
      }else{
	sample = NULL;
      }
    }
  }else{
    sample = NULL;

    if((AGS_IPATCH_DLS2 & (ipatch->flags)) != 0){
      //TODO:JK: implement me
    }else if((AGS_IPATCH_SF2 & (ipatch->flags)) != 0){
      AgsIpatchSF2Reader *reader;

      reader = AGS_IPATCH_SF2_READER(ipatch->reader);

      this_error = NULL;
      sample = (IpatchSample *) ipatch_sf2_find_sample(reader->sf2,
						       reader->selected[3],
						       NULL);
    }else if((AGS_IPATCH_GIG & (ipatch->flags)) != 0){
      //TODO:JK: implement me
    }
  }

  this_error = NULL;
  ipatch_sample_read_transform(sample,
			       0,
			       frames,
			       buffer,
			       IPATCH_SAMPLE_DOUBLE | IPATCH_SAMPLE_MONO,
			       IPATCH_SAMPLE_UNITY_CHANNEL_MAP,
			       &this_error);
      
  if(this_error != NULL){
    g_warning("%s", this_error->message);
  }
#endif
  
  return(buffer);
}

void
ags_ipatch_close(AgsPlayable *playable)
{
  /* empty */
}

void
ags_ipatch_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_ipatch_parent_class)->finalize(gobject);

  /* empty */
}

/**
 * ags_ipatch_new:
 *
 * Creates an #AgsIpatch.
 *
 * Returns: an empty #AgsIpatch.
 *
 * Since: 1.0.0
 */
AgsIpatch*
ags_ipatch_new(gchar *filename,
	       GObject *soundcard)
{
  AgsIpatch *ipatch;

  ipatch = (AgsIpatch *) g_object_new(AGS_TYPE_IPATCH,
				      "filename", filename,
				      "soundcard", soundcard,
				      NULL);

  return(ipatch);
}
