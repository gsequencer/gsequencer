/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/file/ags_ipatch.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_playable.h>

#include <ags/audio/file/ags_ipatch_sf2_reader.h>

void ags_ipatch_class_init(AgsIpatchClass *ipatch);
void ags_ipatch_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ipatch_playable_interface_init(AgsPlayableInterface *playable);
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

void ags_ipatch_connect(AgsConnectable *connectable);
void ags_ipatch_disconnect(AgsConnectable *connectable);

gboolean ags_ipatch_open(AgsPlayable *playable, gchar *name);
guint ags_ipatch_level_count(AgsPlayable *playable);
guint ags_ipatch_nth_level(AgsPlayable *playable);
gchar** ags_ipatch_sublevel_names(AgsPlayable *playable);
void ags_ipatch_level_select(AgsPlayable *playable,
			     guint nth_level, gchar *sublevel_name,
			     GError **error);
void ags_ipatch_iter_start(AgsPlayable *playable);
gboolean ags_ipatch_iter_next(AgsPlayable *playable);
void ags_ipatch_info(AgsPlayable *playable,
		     guint *channels, guint *frames,
		     guint *loop_start, guint *loop_end,
		     GError **error);
signed short* ags_ipatch_read(AgsPlayable *playable, guint channel,
			      GError **error);
void ags_ipatch_close(AgsPlayable *playable);
GList* ags_ipatch_read_audio_signal(AgsPlayable *playable,
				    AgsDevout *devout,
				    guint start_channel, guint channels);

static gpointer ags_ipatch_parent_class = NULL;
static AgsConnectableInterface *ags_ipatch_parent_connectable_interface;
static AgsPlayableInterface *ags_ipatch_parent_playable_interface;

enum{
  PROP_0,
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

    static const GInterfaceInfo ags_playable_interface_info = {
      (GInterfaceInitFunc) ags_ipatch_playable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ipatch = g_type_register_static(G_TYPE_OBJECT,
					     "AgsIpatch\0",
					     &ags_ipatch_info,
					     0);

    g_type_add_interface_static(ags_type_ipatch,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ipatch,
				AGS_TYPE_PLAYABLE,
				&ags_playable_interface_info);
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
  param_spec = g_param_spec_pointer("filename\0",
				   "the filename\0",
				   "The filename to open\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  param_spec = g_param_spec_pointer("mode\0",
				    "the mode\0",
				    "The mode to open the file\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MODE,
				  param_spec);
}

void
ags_ipatch_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_ipatch_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_ipatch_connect;
  connectable->disconnect = ags_ipatch_disconnect;
}

void
ags_ipatch_playable_interface_init(AgsPlayableInterface *playable)
{
  ags_ipatch_parent_playable_interface = g_type_interface_peek_parent(playable);

  playable->open = ags_ipatch_open;

  playable->nth_level = ags_ipatch_nth_level;
  playable->level_count = ags_ipatch_level_count;
  playable->sublevel_names = ags_ipatch_sublevel_names;
  playable->level_select = ags_ipatch_level_select;

  playable->iter_start = ags_ipatch_iter_start;
  playable->iter_next = ags_ipatch_iter_next;

  playable->info = ags_ipatch_info;
  playable->read = ags_ipatch_read;

  playable->close = ags_ipatch_close;
}

void
ags_ipatch_init(AgsIpatch *ipatch)
{
  ipatch->flags = 0;

  ipatch->devout = NULL;
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

  ipatch->nth_level = 0;
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
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = (gchar *) g_value_get_pointer(value);

      ags_playable_open(AGS_PLAYABLE(ipatch), filename);
    }
    break;
  case PROP_MODE:
    {
      gchar *mode;
      
      mode = (gchar *) g_value_get_pointer(value);
      
      ipatch->mode = mode;

      if(ipatch->handle != NULL){
	GError *error;

	error = NULL;

	ipatch_file_default_open_method(ipatch->handle,
					mode,
					&error);

	if(error != NULL){
	  g_error("%s\0", error->message);
	}
      }
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
  case PROP_FILENAME:
    g_value_set_pointer(value, ipatch->filename);
    break;
  case PROP_MODE:
    g_value_set_pointer(value, ipatch->mode);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ipatch_connect(AgsConnectable *connectable)
{
  ags_ipatch_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_ipatch_disconnect(AgsConnectable *connectable)
{
  ags_ipatch_parent_connectable_interface->disconnect(connectable);

  /* empty */
}


gboolean
ags_ipatch_open(AgsPlayable *playable, gchar *filename)
{
  AgsIpatch *ipatch;
  IpatchFileIOFuncs *io_funcs;
  GError *error;

#ifdef AGS_DEBUG
  g_message("open %s with libinstpatch.\0", filename);
#endif

  ipatch = AGS_IPATCH(playable);

  ipatch->filename = filename;

  error = NULL;
  ipatch->handle = ipatch_file_identify_open(ipatch->filename,
					     &error);

  if(error != NULL){
    g_error("%s\0", error->message);
  }

  if(IPATCH_IS_DLS_FILE(ipatch->handle->file)){
    ipatch->flags |= AGS_IPATCH_DLS2;

    //TODO:JK: implement me
  }else if(g_str_has_suffix(filename, ".sf2\0")){
    /*  */
    ipatch->flags |= AGS_IPATCH_SF2;

    /*  */
    ipatch->reader = ags_ipatch_sf2_reader_new();
    AGS_IPATCH_SF2_READER(ipatch->reader)->ipatch = ipatch;

    AGS_IPATCH_SF2_READER(ipatch->reader)->reader = ipatch_sf2_reader_new(ipatch->handle);

    error = NULL;
    ipatch->base = ipatch_sf2_reader_load(AGS_IPATCH_SF2_READER(ipatch->reader)->reader,
					  &error);

    error = NULL;
    AGS_IPATCH_SF2_READER(ipatch->reader)->sf2 = ipatch_convert_object_to_type(ipatch->handle->file,
									       IPATCH_TYPE_SF2,
									       &error);

    if(error != NULL){
      g_error("%s\0", error->message);
    }

    /* load samples */
    ipatch->samples = ipatch_container_get_children(IPATCH_CONTAINER(ipatch->base),
						    IPATCH_TYPE_SF2_SAMPLE);
  }else if(IPATCH_IS_GIG_FILE(ipatch->handle->file)){
    ipatch->flags |= AGS_IPATCH_GIG;

    //TODO:JK: implement me
  }

  if(error == NULL){
    return(TRUE);
  }else{
    return(FALSE);
  }
}

guint
ags_ipatch_nth_level(AgsPlayable *playable)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(playable);

  return(ipatch->nth_level);
}

guint
ags_ipatch_level_count(AgsPlayable *playable)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(playable);

  if((AGS_IPATCH_DLS2 & (ipatch->flags)) != 0){
    return(4);
  }else if((AGS_IPATCH_SF2 & (ipatch->flags)) != 0){
    return(4);
  }else if((AGS_IPATCH_GIG & (ipatch->flags)) != 0){
    return(4);
  }

  return(0);
}

gchar**
ags_ipatch_sublevel_names(AgsPlayable *playable)
{
  AgsIpatch *ipatch;
  AgsIpatchSF2Reader *ipatch_sf2_reader;
  IpatchList *ipatch_list;
  GList *list;
  gchar **names;
  gchar *name;
  guint sublevel;
  guint i;

  ipatch = AGS_IPATCH(playable);
  list = NULL;

  names = (gchar **) malloc(1 * sizeof(gchar*));
  names[0] = NULL;

  if((AGS_IPATCH_SF2 & (ipatch->flags)) != 0){
    ipatch_sf2_reader = AGS_IPATCH_SF2_READER(ipatch->reader);
    
    sublevel = ipatch->nth_level;

    switch(sublevel){
    case AGS_SF2_FILENAME:
      {
	names = realloc(names, 2 * sizeof(char*));

	names[0] = ipatch_sf2_reader->ipatch->filename;
	names[1] = NULL;

	return(names);
      }
    case AGS_SF2_PHDR:
      {
	ipatch_list = ipatch_container_get_children(ipatch_sf2_reader->sf2,
						    IPATCH_TYPE_SF2_PRESET);

	if(ipatch_list != NULL){
	  list = ipatch_list->items;
	}else{
	  return(names);
	}
      }
      break;
    case AGS_SF2_IHDR:
      {
	ipatch_list = ipatch_container_get_children(ipatch_sf2_reader->preset,
						    IPATCH_TYPE_SF2_INST);
      
	if(ipatch_list != NULL){
	  list = ipatch_list->items;
	}else{
	  return(names);
	}
      }
      break;
    case AGS_SF2_SHDR:
      {
	ipatch_list = ipatch_container_get_children(ipatch_sf2_reader->instrument,
						    IPATCH_TYPE_SF2_SAMPLE);
      
	if(ipatch_list != NULL){
	  list = ipatch_list->items;
	}else{
	  return(names);
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
	  names = (gchar **) realloc(names, (i + 2) * sizeof(char*));
	  names[i] = ipatch_sf2_preset_get_name(IPATCH_SF2_PRESET(list->data));
	}
      }
      break;
    case AGS_SF2_IHDR:
      {
	if(IPATCH_IS_SF2_INST(list->data)){
	  names = (gchar **) realloc(names, (i + 2) * sizeof(char*));
	  names[i] = ipatch_sf2_inst_get_name(IPATCH_SF2_INST(list->data));
	}
      }
      break;
    case AGS_SF2_SHDR:
      {
	if(IPATCH_IS_SF2_SAMPLE(list->data)){
	  names = (gchar **) realloc(names, (i + 2) * sizeof(char*));
	  names[i] = ipatch_sf2_sample_get_name(IPATCH_SF2_SAMPLE(list->data));
	}
      }
      break;
    };
    
    list = list->next;
  }
  
  if(i > 0){
    names[i] = NULL;
  }

  return(names);
}

void
ags_ipatch_level_select(AgsPlayable *playable,
			guint nth_level, gchar *sublevel_name,
			GError **error)
{
  AgsIpatch *ipatch;
  AgsIpatchSF2Reader *ipatch_sf2_reader;
  gboolean success;

  ipatch = AGS_IPATCH(playable);

  if((AGS_IPATCH_SF2 & (ipatch->flags)) != 0){
    ipatch_sf2_reader = AGS_IPATCH_SF2_READER(ipatch->reader);

    //TODO:JK: apply mods and gens

    if(sublevel_name == NULL){
      ipatch->nth_level = 0;
      ipatch_sf2_reader->selected[0] = NULL;
    }else{
      IpatchList *ipatch_list;
      GList *list;

      if(nth_level == 0 && !g_strcmp0(ipatch_sf2_reader->ipatch->filename, sublevel_name)){
	ipatch->nth_level = 0;
	ipatch_sf2_reader->selected[0] = sublevel_name;
	return;
      }

      if(nth_level == 1){
	ipatch->nth_level = 1;
	ipatch_sf2_reader->selected[1] = sublevel_name;

	/* preset */
	ipatch_list = ipatch_sf2_get_presets(ipatch_sf2_reader->sf2);
	list = ipatch_list->items;
	
	while(list != NULL){
	  if(!g_strcmp0(IPATCH_SF2_PRESET(list->data)->name, sublevel_name)){
	    ipatch_sf2_reader->preset = IPATCH_SF2_PRESET(list->data);

	    /* some extra code for bank and program */
	    ipatch_sf2_preset_get_midi_locale(IPATCH_SF2_PRESET(list->data),
					      &(ipatch_sf2_reader->bank),
					      &(ipatch_sf2_reader->program));

#ifdef AGS_DEBUG
	    g_message("debug: bank %d program %d\n\0", ipatch_sf2_reader->bank, ipatch_sf2_reader->program);
#endif

	    break;
	  }

	  list = list->next;
	}

	ipatch->iter = list;
      }else{
	gboolean found_first;

	found_first = FALSE;

	if(nth_level == 2){
	  ipatch->nth_level = 2;
	  ipatch_sf2_reader->selected[2] = sublevel_name;

	  /* instrument */
	  ipatch_list = ipatch_container_get_children(ipatch_sf2_reader->preset,
						      IPATCH_TYPE_SF2_INST);
	  list = ipatch_list->items;
	
	  while(list != NULL){
	    if(!g_strcmp0(IPATCH_SF2_INST(list->data)->name, sublevel_name)){
	      ipatch_sf2_reader->instrument = IPATCH_SF2_INST(list->data);
	      break;
	    }

	    list = list->next;
	  }

	  ipatch->iter = list;
	}else if(ipatch->nth_level == 3){
	  ipatch->nth_level = 3;
	  ipatch_sf2_reader->selected[3] = sublevel_name;

	  /* sample */
	  ipatch_list = ipatch_container_get_children(ipatch_sf2_reader->instrument,
						      IPATCH_TYPE_SF2_SAMPLE);
	  list = ipatch_list->items;
	
	  while(list != NULL){
	    if(!strncmp(IPATCH_SF2_SAMPLE(list->data)->name, sublevel_name, 20)){
	      ipatch_sf2_reader->sample = IPATCH_SF2_SAMPLE(list->data);

	      break;
	    }

	    list = list->next;
	  }

	  ipatch->iter = list;
	}else{
	  g_set_error(error,
		      AGS_PLAYABLE_ERROR,
		      AGS_PLAYABLE_ERROR_NO_SUCH_LEVEL,
		      "no level called %s in soundfont2 file: %s\0",
		      sublevel_name, ipatch_sf2_reader->ipatch->filename);
	}
      }
    }
  }
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
		"Not able to go %u steps higher in soundfont2 file: %s\0",
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
  IpatchSample *sample;

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

    g_set_error(error,
		AGS_PLAYABLE_ERROR,
		AGS_PLAYABLE_ERROR_NO_SAMPLE,
		"no sample selected for file: %s\0",
		ipatch->filename);
  }

  sample = IPATCH_SAMPLE(ipatch->iter->data);
  g_object_get(G_OBJECT(sample),
	       "sample-size\0", frames,
	       "loop-start\0", loop_start,
	       "loop-end\0", loop_end,
	       NULL);

  //TODO:JK: verify me
  if(channels != NULL){
    *channels = AGS_IPATCH_DEFAULT_CHANNELS;
  }
}

signed short*
ags_ipatch_read(AgsPlayable *playable, guint channel,
		GError **error)
{
  AgsIpatch *ipatch;
  IpatchSample *sample;
  signed short *buffer, *source;
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
    g_error("%s\0", this_error->message);
  }

  buffer = (signed short *) malloc(channels * frames * sizeof(signed short));
  
  if(ipatch->nth_level == 3){
    sample = NULL;

    if((AGS_IPATCH_DLS2 & (ipatch->flags)) != 0){
      //TODO:JK: implement me
    }else if((AGS_IPATCH_SF2 & (ipatch->flags)) != 0){
      AgsIpatchSF2Reader *reader;

      reader = AGS_IPATCH_SF2_READER(ipatch->reader);

      sample = ipatch_sf2_find_sample(reader->sf2,
				      reader->selected[3],
				      NULL);
    }else if((AGS_IPATCH_GIG & (ipatch->flags)) != 0){
      //TODO:JK: implement me
    }
  }else{
    sample = IPATCH_SAMPLE(ipatch->iter->data);
  }

  this_error = NULL;
  ipatch_sample_read_transform(sample,
			       0,
			       frames,
			       buffer,
			       IPATCH_SAMPLE_16BIT | IPATCH_SAMPLE_MONO | IPATCH_SAMPLE_SIGNED,
			       IPATCH_SAMPLE_UNITY_CHANNEL_MAP,
			       &this_error);
      
  if(this_error != NULL){
    g_error("%s\0", this_error->message);
  }

  return(buffer);
}

void
ags_ipatch_close(AgsPlayable *playable)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(playable);

  /* empty */
}

void
ags_ipatch_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_ipatch_parent_class)->finalize(gobject);

  /* empty */
}

/**
 * ags_ipatch_read_audio_signal:
 * @ipatch an AgsIpatch
 * Returns:
 *
 * Reads an AgsAudioSignal from current sample and iterates to the next sample. Prior,
 * you should have called #ags_playable_iter_start.
 */
GList*
ags_ipatch_read_audio_signal(AgsPlayable *playable,
			     AgsDevout *devout,
			     guint start_channel, guint channels)
{
  AgsIpatch *ipatch;
  GList *list;

  ipatch = AGS_IPATCH(playable);

  ags_playable_iter_next(AGS_PLAYABLE(ipatch->reader));
  list = ags_playable_read_audio_signal(AGS_PLAYABLE(ipatch->reader),
					ipatch->devout,
					0, 2);

  ipatch->audio_signal = list;
}

AgsIpatch*
ags_ipatch_new()
{
  AgsIpatch *ipatch;

  ipatch = (AgsIpatch *) g_object_new(AGS_TYPE_IPATCH,
				      NULL);

  return(ipatch);
}
