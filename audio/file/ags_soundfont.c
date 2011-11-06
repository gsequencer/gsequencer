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

#include <ags/audio/file/ags_soundfont.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_playable.h>

void ags_soundfont_class_init(AgsSoundfontClass *soundfont);
void ags_soundfont_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_soundfont_playable_interface_init(AgsPlayableInterface *playable);
void ags_soundfont_init(AgsSoundfont *soundfont);
void ags_soundfont_connect(AgsConnectable *connectable);
void ags_soundfont_disconnect(AgsConnectable *connectable);
void ags_soundfont_finalize(GObject *gobject);

gboolean ags_soundfont_open(AgsPlayable *playable, gchar *filename);
guint ags_soundfont_level_count(AgsPlayable *playable);
gchar** ags_soundfont_sublevel_names(AgsPlayable *playable);
void ags_soundfont_level_select(AgsPlayable *playable, guint nth_level, gchar *sublevel_name);
void ags_soundfont_iter_start(AgsPlayable *playable);
gboolean ags_soundfont_iter_next(AgsPlayable *playable);
void ags_soundfont_start_sample(AgsPlayable *playable);
void ags_soundfont_next_sample(AgsPlayable *playable);
void ags_soundfont_info(AgsPlayable *playable, guint *channels, guint *frames, guint *loop_start, guint *loop_end);
short* ags_soundfont_read(AgsPlayable *playable, guint channel);
void ags_soundfont_close(AgsPlayable *playable);

gchar** ags_soundfont_get_presets(AgsSoundfont *soundfont);
void ags_soundfont_select_preset(AgsSoundfont *soundfont, gchar *preset_name);
gchar** ags_soundfont_get_instruments(AgsSoundfont *soundfont);
void ags_soundfont_select_instrument(AgsSoundfont *soundfont, gchar *instrument_name);
gchar** ags_soundfont_get_samples(AgsSoundfont *soundfont);
void ags_soundfont_select_sample(AgsSoundfont *soundfont, gchar *sample_name);

static gpointer ags_soundfont_parent_class = NULL;
static AgsConnectableInterface *ags_soundfont_parent_connectable_interface;
static AgsPlayableInterface *ags_soundfont_parent_playable_interface;

typedef struct _AgsSoundfontPresetIO AgsSoundfontPresetIO;

/* sf file chunk IDs */
enum{
  UNKN_ID,
  RIFF_ID,
  LIST_ID,
  SFBK_ID,
  /* info/sample/preset */
  INFO_ID,
  SDTA_ID,
  PDTA_ID,
  /* info ids (1st byte of info strings) */
  IFIL_ID,
  ISNG_ID,
  INAM_ID,
  IROM_ID,
  /* more info ids */
  IVER_ID,
  ICRD_ID,
  IENG_ID,
  IPRD_ID,
  /* and yet more info ids */
  ICOP_ID,
  ICMT_ID,
  ISFT_ID,
  /* sample ids */
  SNAM_ID,
  SMPL_ID,
  /* preset ids */
  PHDR_ID,
  PBAG_ID,
  PMOD_ID,
  PGEN_ID,
  /* instrument ids */
  IHDR_ID,
  IBAG_ID,
  IMOD_ID,
  IGEN_ID,
  /* sample info */
  SHDR_ID
};

struct _AgsSoundfontPresetIO{
  guint16 preset;
  guint16 i_preset_bag;
};

GType
ags_soundfont_get_type()
{
  static GType ags_type_soundfont = 0;

  if(!ags_type_soundfont){
    static const GTypeInfo ags_soundfont_info = {
      sizeof (AgsSoundfontClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_soundfont_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSoundfont),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_soundfont_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_soundfont_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_playable_interface_info = {
      (GInterfaceInitFunc) ags_soundfont_playable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_soundfont = g_type_register_static(G_TYPE_OBJECT,
						"AgsSoundfont\0",
						&ags_soundfont_info,
						0);
    
    g_type_add_interface_static(ags_type_soundfont,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
    
    g_type_add_interface_static(ags_type_soundfont,
				AGS_TYPE_PLAYABLE,
				&ags_playable_interface_info);
  }
  
  return (ags_type_soundfont);
}

void
ags_soundfont_class_init(AgsSoundfontClass *soundfont)
{
  GObjectClass *gobject;

  ags_soundfont_parent_class = g_type_class_peek_parent(soundfont);

  gobject = (GObjectClass *) soundfont;

  gobject->finalize = ags_soundfont_finalize;
}

void
ags_soundfont_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_soundfont_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_soundfont_connect;
  connectable->disconnect = ags_soundfont_disconnect;
}

void
ags_soundfont_playable_interface_init(AgsPlayableInterface *playable)
{
  ags_soundfont_parent_playable_interface = g_type_interface_peek_parent(playable);

  playable->open = ags_soundfont_open;

  playable->level_count = ags_soundfont_level_count;
  playable->sublevel_names = ags_soundfont_sublevel_names;
  playable->level_select = ags_soundfont_level_select;

  playable->iter_start = ags_soundfont_iter_start;
  playable->iter_next = ags_soundfont_iter_next;

  playable->info = ags_soundfont_info;
  playable->read = ags_soundfont_read;

  playable->close = ags_soundfont_close;
}

void
ags_soundfont_init(AgsSoundfont *soundfont)
{
  soundfont->file = NULL;

  soundfont->selected_level = 0;

  soundfont->preset = NULL;

  soundfont->current_preset = NULL;
  soundfont->i_instrument = 0;

  soundfont->current_instrument = NULL;
  soundfont->i_sample = 0;

  soundfont->current_sample = NULL;

  soundfont->audio_file = NULL;
}

void
ags_soundfont_connect(AgsConnectable *connectable)
{
}

void
ags_soundfont_disconnect(AgsConnectable *connectable)
{
}

void
ags_soundfont_finalize(GObject *gobject)
{
}

gboolean
ags_soundfont_open(AgsPlayable *playable, gchar *filename)
{
  AgsSoundfont *soundfont;
  AgsSoundfontPresetIO **preset_io;
  guint32 riff_id;
  size_t sample_position;
  guint16 sample_size;
  auto void ags_soundfont_read_chunk(AgsSoundfont *soundfont, guint32 *id, guint32 *chunk_size);
  auto gboolean ags_soundfont_read_preset(AgsSoundfont *soundfont, AgsSoundfontPresetIO ***ret_preset_io);
  auto gboolean ags_soundfont_read_level1(AgsSoundfont *soundfont, size_t *sample_position, guint16 *sample_size);
  auto void ags_soundfont_read_level2(AgsSoundfont *soundfont);
  auto void ags_soundfont_read_level3(AgsSoundfont *soundfont);
  void ags_soundfont_read_chunk(AgsSoundfont *soundfont, guint32 *id, guint32 *chunk_size){
    FILE *file;
    char *data, *str;
    guint32 tmp;

    file = soundfont->file;

    /* read data */
    data = (char *) malloc(8 * sizeof(char));
    fread(data, sizeof(char), 8, file);

    /* get id */
    if(id != NULL){
      memcpy(&tmp, data, 4 * sizeof(char));
      *id = GUINT32_FROM_LE(tmp);
    }

    /* get chunk_size */
    if(chunk_size != NULL){
      memcpy(&tmp, &(data[4]), 4 * sizeof(char));
      *chunk_size = GUINT32_FROM_LE(tmp);
    }

    /* free data */
    free(data);
  }
  gboolean ags_soundfont_read_preset(AgsSoundfont *soundfont, AgsSoundfontPresetIO ***ret_preset_io){
    FILE *file;
    AgsSoundfontPresetIO **preset_io;
    AgsSoundfontInstrumentIO **instrument_io;
    guint32 id;
    guint32 chunk_size;
    guint i, i_stop;

    file = soundfont->file;

    /* preset header */
    ags_soundfont_read_chunk(soundfont, &id, &chunk_size);

    if(id != PHDR_ID){
      return(FALSE);
    }

    if(chunk_size == 0 ||
       chunk_size % 38 != 0){
      return(FALSE);
    }

    /* alloc memory for preset and preset io */
    i_stop = chunk_size / 38;

    soundfont->preset = (AgsSoundfontPreset **) malloc((i_stop + 1) * sizeof(AgsSoundfontPreset*));
    soundfont->preset[i_stop] = NULL;

    if(ret_preset_io != NULL){
      preset_io = (AgsSoundfontPresetIO **) malloc((i_stop + 1) * sizeof(AgsSoundfontPresetIO*));
      preset_io[i_stop] = NULL;
    }

    /* read presets */
    for(i = 0; i < i_stop; i++){
      char *data;
      guint16 tmp16;

      soundfont->preset[i] = (AgsSoundfontPreset *) malloc(sizeof(AgsSoundfontPreset));

      if(ret_preset_io != NULL){
	preset_io[i] = (AgsSoundfontPresetIO *) malloc(sizeof(AgsSoundfontPresetIO));
      }

      data = (char *) malloc(38 * sizeof(char));
      fread(data, sizeof(char), 38, file);

      soundfont->preset[i]->preset_name = g_strndup(data, 20);

      memcpy(&tmp16, &(data[20]), sizeof(guint16));
      preset_io[i]->preset = GUINT16_FROM_LE(tmp16);

      //skipped bank

      memcpy(&tmp16, &(data[24]), sizeof(guint16));
      preset_io[i]->i_preset_bag = GUINT16_FROM_LE(tmp16);

      //skipped some fields

      free(data);
    }

    /* skip PBAG */
    ags_soundfont_read_chunk(soundfont, &id, &chunk_size);

    if(id != PBAG_ID){
      return(FALSE);
    }

    if(chunk_size == 0 ||
       chunk_size % 4 != 0){
      return(FALSE);
    }

    fseek(file, chunk_size, SEEK_CUR);

    /* skip PMOD */
    ags_soundfont_read_chunk(soundfont, &id, &chunk_size);

    if(id != PMOD_ID){
      return(FALSE);
    }

    fseek(file, chunk_size, SEEK_CUR);    

    /* skip PGEN */
    ags_soundfont_read_chunk(soundfont, &id, &chunk_size);

    if(id != PGEN_ID){
      return(FALSE);
    }

    fseek(file, chunk_size, SEEK_CUR);

    /* read instrument header */
    ags_soundfont_read_chunk(soundfont, &id, &chunk_size);

    if(id != IHDR_ID){
      return(FALSE);
    }




    if(ret_preset_io != NULL){
      *ret_preset_io = preset_io;
    }
  }
  void ags_soundfont_read_id(AgsSoundfont *soundfont, guint32 *id){
    FILE *file;
    char *data, *str;
    guint32 tmp;

    file = soundfont->file;

    /* read data */
    data = (char *) malloc(8 * sizeof(char));
    fread(data, sizeof(char), 8, file);

    /* get id */
    if(id != NULL){
      memcpy(&tmp, data, 4 * sizeof(char));
      *id = GUINT32_FROM_LE(tmp);
    }

    /* free data */
    free(data);
  }
  gboolean ags_soundfont_read_level1(AgsSoundfont *soundfont, size_t *sample_position, guint16 *sample_size){
    FILE *file;
    guint32 info_id, info_list_id;
    guint32 info_chunk_size, info_list_chunk_size;
    guint32 sample_data_id, sample_data_list_id, sample_data_list_chunk_id;
    guint32 sample_data_chunk_size, sample_data_list_chunk_size;
    guint i;

    file = soundfont->file;

    /* load id */
    ags_soundfont_read_chunk(soundfont, &info_id, &info_chunk_size);

    if(info_id != SFBK_ID){
      return(FALSE);
    }

    /* get info listchunk */
    ags_soundfont_read_chunk(soundfont, &info_list_id, &info_list_chunk_size);

    if(info_list_id != LIST_ID){
      return(FALSE);
    }

    ags_soundfont_read_id(soundfont, &info_list_id);

    if(info_list_id != INFO_ID){
      return(FALSE);
    }

    /* process info */
    for(i = 0; i < info_list_chunk_size; ){
      guint32 id;
      guint32 chunk_size;

      ags_soundfont_read_chunk(soundfont, &id, &chunk_size);

      if(id == IFIL_ID){
	guint16 major, minor;
	guint16 tmp;
	char *data;

	if(chunk_size != 4){
	  return(FALSE);
	}

	data = (char *) malloc(4 * sizeof(char));

	fread(data, sizeof(char), 4, file);

	memcpy(&tmp, data, 2);
	major = GUINT16_FROM_LE(tmp);

	memcpy(&tmp, &(data[2]), 2);
	minor = GUINT16_FROM_LE(tmp);

	free(data);

	if(major != 2){
	  return(FALSE);
	}
      }else if(id == IVER_ID){
	if(chunk_size != 4){
	  return(FALSE);
	}

	/* not interested in ROM */
	fseek(file, 4, SEEK_CUR);
      }else if(id == UNKN_ID){
	/* ignored */
	fseek(file, chunk_size, SEEK_CUR);
      }else{
	return(FALSE);
      }

      i += chunk_size;
    }

    /* get sample data listchunk */
    ags_soundfont_read_chunk(soundfont, &sample_data_id, &sample_data_chunk_size);
    
    if(info_list_id != LIST_ID){
      return(FALSE);
    }
    
    ags_soundfont_read_id(soundfont, &sample_data_id);
    
    if(info_list_id != SDTA_ID){
      return(FALSE);
    }
    
    /* get sample data info */
    if(sample_data_chunk_size != 0){
      ags_soundfont_read_chunk(soundfont, &sample_data_list_id, &sample_data_list_chunk_size);

      *sample_position = ftell(file);
      *sample_size = sample_data_list_chunk_size;

      fseek(file, *sample_size, SEEK_CUR);
    }

    /* get presets */
    if(!ags_soundfont_read_preset(soundfont, &preset_io)){
      return(FALSE);
    }

    return(TRUE);
  }
  void ags_soundfont_read_level2(AgsSoundfont *soundfont){
  }
  void ags_soundfont_read_level3(AgsSoundfont *soundfont){
  }

  soundfont = AGS_SOUNDFONT(playable);

  soundfont->file = fopen(filename, "rb\0");

  /* get riff chunk */
  ags_soundfont_read_chunk(soundfont, &riff_id, NULL);

  if(riff_id != RIFF_ID){
    return(FALSE);
  }

  /* read meta info */
  ags_soundfont_read_level1(soundfont, &sample_position, &sample_size);
}

guint
ags_soundfont_level_count(AgsPlayable *playable)
{
  return(3);
}

gchar**
ags_soundfont_sublevel_names(AgsPlayable *playable)
{
  AgsSoundfont *soundfont;

  soundfont = AGS_SOUNDFONT(playable);

  switch(soundfont->selected_level){
  case 0:
    return(ags_soundfont_get_presets(soundfont));
  case 1:
    return(ags_soundfont_get_instruments(soundfont));
  case 2:
    return(ags_soundfont_get_samples(soundfont));
  default:
    return(NULL);
  }
}

void
ags_soundfont_level_select(AgsPlayable *playable, guint nth_level, gchar *sublevel_name)
{
  AgsSoundfont *soundfont;

  soundfont = AGS_SOUNDFONT(playable);


  switch(nth_level){
  case 1:
    {
      ags_soundfont_select_preset(soundfont, sublevel_name);
      soundfont->selected_level = nth_level;
    }
    break;
  case 2:
    {
      ags_soundfont_select_instrument(soundfont, sublevel_name);
      soundfont->selected_level = nth_level;
    }
    break;
  case 3:
    {
      ags_soundfont_select_sample(soundfont, sublevel_name);
      soundfont->selected_level = nth_level;
    }
    break;
  default:
    soundfont->selected_level = 0;
  }
}

void
ags_soundfont_iter_start(AgsPlayable *playable)
{
  AgsSoundfont *soundfont;

  soundfont = AGS_SOUNDFONT(playable);
}

gboolean
ags_soundfont_iter_next(AgsPlayable *playable)
{
  AgsSoundfont *soundfont;

  soundfont = AGS_SOUNDFONT(playable);
}

gchar**
ags_soundfont_get_presets(AgsSoundfont *soundfont)
{
  gchar **names;
  size_t size;

  size = 1;
  names = (gchar **) malloc(size * sizeof(gchar *));

  if(soundfont->preset != NULL){
    for(; soundfont->preset[size - 1] != NULL; size++){

      names = realloc(names, size * sizeof(gchar *));
      names[size - 1] = g_strdup(soundfont->preset[size - 1]->preset_name);
    }
  }

  names[size - 1] = NULL;

  return(names);
}

void
ags_soundfont_select_preset(AgsSoundfont *soundfont, gchar *preset_name)
{
  guint i;

  if(soundfont->preset != NULL){
    for(i = 0; soundfont->preset[i] != NULL; i++){
      if(g_strcmp0(soundfont->preset[i]->preset_name, preset_name) == 0){
	soundfont->current_preset = soundfont->preset[i];

	soundfont->current_instrument = NULL;
	soundfont->i_sample = 0;

	soundfont->current_sample = NULL;

	return;
      }
    }
  }

  /* nothing matched */
  soundfont->current_preset = NULL;
  soundfont->i_instrument = 0;

  soundfont->current_instrument = NULL;
  soundfont->i_sample = 0;

  soundfont->current_sample = NULL;
}

gchar**
ags_soundfont_get_instruments(AgsSoundfont *soundfont)
{
  gchar **names;
  size_t size;

  size = 1;
  names = (gchar **) malloc(size * sizeof(gchar *));

  if(soundfont->current_preset != NULL &&
     soundfont->current_preset->instrument != NULL){
    for(; soundfont->current_preset->instrument[size - 1] != NULL; size++){

      names = realloc(names, size * sizeof(gchar *));
      names[size - 1] = g_strdup(soundfont->current_preset->instrument[size - 1]->instrument_name);
    }
  }


  names[size - 1] = NULL;

  return(names);
}

void
ags_soundfont_select_instrument(AgsSoundfont *soundfont, gchar *instrument_name)
{
  guint i;

  if(soundfont->current_preset != NULL &&
     soundfont->current_preset->instrument != NULL){
    for(i = 0; soundfont->current_preset->instrument[i] != NULL; i++){
      if(g_strcmp0(soundfont->current_preset->instrument[i]->instrument_name, instrument_name) == 0){
	soundfont->i_instrument = i;

	soundfont->current_instrument = soundfont->current_preset->instrument[i];

	soundfont->i_sample = 0;
	
	soundfont->current_sample = NULL;

	return;
      }
    }
  }

  soundfont->i_instrument = 0;

  soundfont->current_instrument = NULL;
  soundfont->i_sample = 0;

  soundfont->current_sample = NULL;
}

gchar**
ags_soundfont_get_samples(AgsSoundfont *soundfont)
{
  gchar **names;
  size_t size;

  size = 1;
  names = (gchar **) malloc(size * sizeof(gchar *));

  if(soundfont->current_instrument != NULL &&
     soundfont->current_instrument->sample != NULL){
    for(; soundfont->current_instrument->sample[size - 1] != NULL; size++){

      names = realloc(names, size * sizeof(gchar *));
      names[size - 1] = g_strdup(soundfont->current_instrument->sample[size - 1]->sample_name);
    }
  }


  names[size - 1] = NULL;

  return(names);
}

void
ags_soundfont_select_sample(AgsSoundfont *soundfont, gchar *sample_name)
{
  guint i;

  if(soundfont->current_instrument != NULL &&
     soundfont->current_instrument->sample != NULL){
    for(i = 0; soundfont->current_instrument->sample[i] != NULL; i++){
      if(g_strcmp0(soundfont->current_instrument->sample[i]->sample_name, sample_name) == 0){
	soundfont->i_sample = i;

	soundfont->current_sample = soundfont->current_instrument->sample[i];

	return;
      }
    }
  }

  soundfont->i_sample = 0;

  soundfont->current_sample = NULL;
}

void
ags_soundfont_start_sample(AgsPlayable *playable)
{
  AgsSoundfont *soundfont;

  soundfont = AGS_SOUNDFONT(playable);
  
  soundfont->current_sample = soundfont->current_instrument->sample[soundfont->i_sample];
}

void
ags_soundfont_next_sample(AgsPlayable *playable)
{
  AgsSoundfont *soundfont;

  soundfont = AGS_SOUNDFONT(playable);
  
  soundfont->current_sample = soundfont->current_instrument->sample[soundfont->current_sample - soundfont->current_instrument->sample[0] + 1];
}

void
ags_soundfont_info(AgsPlayable *playable, guint *channels, guint *frames, guint *loop_start, guint *loop_end)
{
  AgsSoundfont *soundfont;

  soundfont = AGS_SOUNDFONT(playable);

  if((AGS_SOUNDFONT_SAMPLE_MONO & (soundfont->current_sample->flags)) == 0){
    //    if((AGS_SOUNDFONT_SAMPLE_LEFT & (soundfont->current_sample->flags)) != 0){
    //      if((AGS_SOUNDFONT_SAMPLE_RIGHT & (soundfont->current_sample->flags)) != 0){
	*channels = 2;
	//      }else{
	//	*channels = 1;
	//      }
	//    }
  }else{
    *channels = 1;
  }

  *frames = soundfont->current_sample->frames;
  *loop_start = soundfont->current_sample->loop_start;
  *loop_end = soundfont->current_sample->loop_end;
}

short*
ags_soundfont_read(AgsPlayable *playable, guint channel)
{
  short *buffer;
}

void
ags_soundfont_close(AgsPlayable *playable)
{
  AgsSoundfont *soundfont;
  guint i, j, k;

  soundfont = AGS_SOUNDFONT(playable);

  fclose(soundfont->file);
  soundfont->file = NULL;

  soundfont->selected_level = 0;

  if(soundfont->preset != NULL){
    for(i = 0; soundfont->preset[i] != NULL; i++){
      for(j = 0; soundfont->preset[i]->instrument[j] != NULL; j++){
	for(k = 0; soundfont->preset[i]->instrument[j]->sample[k] != NULL; k++){
	  free(soundfont->preset[i]->instrument[j]->sample[k]);
	}
	
	free(soundfont->preset[i]->instrument[j]);
      }

      free(soundfont->preset[i]);
    }

    free(soundfont->preset);
  }

  soundfont->current_preset = NULL;
  soundfont->current_instrument = NULL;
  soundfont->current_sample = NULL;
}

AgsSoundfont*
ags_soundfont_new()
{
  AgsSoundfont *soundfont;

  soundfont = (AgsSoundfont *) g_object_new(AGS_TYPE_SOUNDFONT,
					    NULL);

  return(soundfont);
}
