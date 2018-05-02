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

#include <ags/audio/file/ags_ipatch_sf2_reader.h>

#include <ags/libags.h>

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
void ags_ipatch_sf2_reader_finalize(GObject *gobject);

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
static AgsConnectableInterface *ags_ipatch_sf2_reader_parent_connectable_interface;

enum{
  PROP_0,
  PROP_IPATCH,
};

GType
ags_ipatch_sf2_reader_get_type()
{
  static GType ags_type_ipatch_sf2_reader = 0;

  if(!ags_type_ipatch_sf2_reader){
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
  }
  
  return(ags_type_ipatch_sf2_reader);
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
ags_ipatch_sf2_reader_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_ipatch_sf2_reader_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_ipatch_sf2_reader_init(AgsIpatchSF2Reader *ipatch_sf2_reader)
{
  ipatch_sf2_reader->ipatch = NULL;

  ipatch_sf2_reader->bank = -1;
  ipatch_sf2_reader->program = -1;

  ipatch_sf2_reader->selected = (gchar **) malloc(5 * sizeof(gchar *));
  ipatch_sf2_reader->selected[0] = NULL;
  ipatch_sf2_reader->selected[1] = NULL;
  ipatch_sf2_reader->selected[2] = NULL;
  ipatch_sf2_reader->selected[3] = NULL;
  ipatch_sf2_reader->selected[4] = NULL;
  
  ipatch_sf2_reader->preset = NULL;
  ipatch_sf2_reader->instrument = NULL;
  ipatch_sf2_reader->sample = NULL;
}

void
ags_ipatch_sf2_reader_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(gobject);

  switch(prop_id){
  case PROP_IPATCH:
    {
      AgsIpatch *ipatch;

      ipatch = (AgsIpatch *) g_value_get_object(value);

      if(ipatch_sf2_reader->ipatch == ipatch){
	return;
      }
      
      if(ipatch_sf2_reader->ipatch != NULL){
	g_object_unref(ipatch_sf2_reader->ipatch);
      }

      if(ipatch != NULL){
	g_object_ref(ipatch);
      }

      ipatch_sf2_reader->ipatch = ipatch;
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

  ipatch_sf2_reader = AGS_IPATCH_SF2_READER(gobject);

  switch(prop_id){
  case PROP_IPATCH:
    {
      g_value_set_object(value, ipatch_sf2_reader->ipatch);
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

/**
 * ags_ipatch_sf2_reader_get_preset_all:
 * @ipatch_sf2_reader: the #AgsSF2Reader
 * 
 * Get all preset names.
 * 
 * Returns: the string vector cotaining preset names or %NULL on failure
 * 
 * Since: 2.0.0
 */
gchar**
ags_ipatch_sf2_reader_get_preset_all(AgsIpatchSF2Reader *ipatch_sf2_reader)
{
#ifdef AGS_WITH_LIBINSTPATCH
  IpatchSF2 *sf2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter preset_iter;
#endif
  
  gchar **preset;
  
  guint i, i_stop;
  
  if(!AGS_IS_IPATCH_SF2_READER(ipatch_sf2_reader)){
    return(NULL);
  }
  
  preset = NULL;

#ifdef AGS_WITH_LIBINSTPATCH
  sf2 = ipatch_sf2_reader->sf2;
  
  ipatch_list = ipatch_container_get_children(sf2, IPATCH_TYPE_SF2_PRESET);
  
  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &preset_iter);

    i_stop = ipatch_iter_count(&preset_iter);

    if(i_stop > 0){
      preset = (gchar **) malloc((i_stop + 1) * sizeof(gchar *));

      ipatch_iter_first(&preset_iter);
      
      for(i = 0; i < i_stop; i++){
	item = ipatch_iter_get(&preset_iter);
	preset[i] = ipatch_sf2_preset_get_name(IPATCH_SF2_PRESET(item));

	/* iterate */
	ipatch_iter_next(&preset_iter);
      }

      preset[i] = NULL;
    }
  }
  
  //FIXME:JK: still needed?
  while(g_static_rec_mutex_unlock_full(((IpatchItem *) (sf2))->mutex) != 0);
#endif
  
  return(preset);
}

/**
 * ags_ipatch_sf2_reader_get_instrument_all:
 * @ipatch_sf2_reader: the #AgsSF2Reader
 * 
 * Get all instrument names.
 * 
 * Returns: the string vector cotaining instrument names or %NULL on failure
 * 
 * Since: 2.0.0
 */
gchar**
ags_ipatch_sf2_reader_get_instrument_all(AgsIpatchSF2Reader *ipatch_sf2_reader)
{
#ifdef AGS_WITH_LIBINSTPATCH
  IpatchSF2 *sf2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter instrument_iter;
#endif
  
  gchar **instrument;
  
  guint i, i_stop;
  
  if(!AGS_IS_IPATCH_SF2_READER(ipatch_sf2_reader)){
    return(NULL);
  }
  
  instrument = NULL;

#ifdef AGS_WITH_LIBINSTPATCH
  sf2 = ipatch_sf2_reader->sf2;
  
  ipatch_list = ipatch_container_get_children(sf2, IPATCH_TYPE_SF2_INST);
  
  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &instrument_iter);

    i_stop = ipatch_iter_count(&instrument_iter);

    if(i_stop > 0){
      instrument = (gchar **) malloc((i_stop + 1) * sizeof(gchar *));

      ipatch_iter_first(&instrument_iter);
      
      for(i = 0; i < i_stop; i++){
	item = ipatch_iter_get(&instrument_iter);
	instrument[i] = ipatch_sf2_inst_get_name(IPATCH_SF2_INST(item));

	/* iterate */
	ipatch_iter_next(&instrument_iter);
      }

      instrument[i] = NULL;
    }
  }
  
  //FIXME:JK: still needed?
  while(g_static_rec_mutex_unlock_full(((IpatchItem *) (sf2))->mutex) != 0);
#endif
  
  return(instrument);
}

/**
 * ags_ipatch_sf2_reader_get_sample_all:
 * @ipatch_sf2_reader: the #AgsSF2Reader
 * 
 * Get all sample names.
 * 
 * Returns: the string vector cotaining sample names or %NULL on failure
 * 
 * Since: 2.0.0
 */
gchar**
ags_ipatch_sf2_reader_get_sample_all(AgsIpatchSF2Reader *ipatch_sf2_reader)
{
#ifdef AGS_WITH_LIBINSTPATCH
  IpatchSF2 *sf2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter sample_iter;
#endif
  
  gchar **sample;
  
  guint i, i_stop;
  
  if(!AGS_IS_IPATCH_SF2_READER(ipatch_sf2_reader)){
    return(NULL);
  }
  
  sample = NULL;

#ifdef AGS_WITH_LIBINSTPATCH
  sf2 = ipatch_sf2_reader->sf2;
  
  ipatch_list = ipatch_container_get_children(sf2, IPATCH_TYPE_SF2_SAMPLE);
  
  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &sample_iter);

    i_stop = ipatch_iter_count(&sample_iter);

    if(i_stop > 0){
      sample = (gchar **) malloc((i_stop + 1) * sizeof(gchar *));

      ipatch_iter_first(&sample_iter);
      
      for(i = 0; i < i_stop; i++){
	item = ipatch_iter_get(&sample_iter);
	sample[i] = ipatch_sf2_sample_get_name(IPATCH_SF2_SAMPLE(item));

	/* iterate */
	ipatch_iter_next(&sample_iter);
      }

      sample[i] = NULL;
    }
  }
  
  //FIXME:JK: still needed?
  while(g_static_rec_mutex_unlock_full(((IpatchItem *) (sf2))->mutex) != 0);
#endif
  
  return(sample);
}

/**
 * ags_ipatch_sf2_reader_get_instrument_by_preset_index:
 * @ipatch_sf2_reader: the #AgsSF2Reader
 * @preset_index: the preset index
 * 
 * Get instrument by preset index.
 * 
 * Returns: the string vector cotaining instrument names or %NULL on failure
 * 
 * Since: 2.0.0
 */
gchar**
ags_ipatch_sf2_reader_get_instrument_by_preset_index(AgsIpatchSF2Reader *ipatch_sf2_reader,
						     guint preset_index)
{
#ifdef AGS_WITH_LIBINSTPATCH
  IpatchSF2 *sf2;
  IpatchContainer *preset;  
  IpatchContainer *instrument;
  
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter preset_iter, instrument_iter, sample_iter;
#endif

  gchar **instrument;

  guint i, i_stop;
  guint j, j_stop;
  guint k, k_stop;
  guint count;

  if(!AGS_IS_IPATCH_SF2_READER(ipatch_sf2_reader)){
    return(NULL);
  }
  
  instrument = NULL;
  
#ifdef AGS_WITH_LIBINSTPATCH
  sf2 = ipatch_sf2_reader->sf2;
  
  /* presets */
  ipatch_list = ipatch_container_get_children(sf2, IPATCH_TYPE_SF2_PRESET);
  
  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &preset_iter);

    i_stop = ipatch_iter_count(&preset_iter);

    if(i_stop > 0 && preset_index < i_stop){
      ipatch_iter_index(&preset_iter, preset_index);

      preset = ipatch_iter_get(&preset_iter);

      /* instruments */
      ipatch_list = ipatch_sf2_preset_get_zones(preset);

      if(ipatch_list != NULL){
	ipatch_list_init_iter(ipatch_list, &instrument_iter);

	j_stop = ipatch_iter_count(&instrument_iter);

	if(j_stop > 0){
	  instrument = (gchar **) malloc((j_stop + 1) * sizeof(gchar *));
	    
	  for(j = 0, count = 0; j < j_stop; j++){
	    instrument = ipatch_iter_get(&instrument_iter);

	    ipatch_item = ipatch_iter_get(&sample_iter);
	    instrument[count] = ipatch_sf2_inst_get_name(ipatch_item);

	    /* iterate */
	    ipatch_iter_next(&instrument_iter);
	    count++;
	  }

	  instrument[count] = NULL;
	}
      }
    }
  }

  //FIXME:JK: still needed?
  while(g_static_rec_mutex_unlock_full(((IpatchItem *) (sf2))->mutex) != 0);
#endif

  return(instrument);
}

/**
 * ags_ipatch_sf2_reader_get_sample_by_preset_index:
 * @ipatch_sf2_reader: the #AgsSF2Reader
 * @preset_index: the preset index
 * 
 * Get sample by preset index.
 * 
 * Returns: the string vector cotaining sample names or %NULL on failure
 * 
 * Since: 2.0.0
 */
gchar**
ags_ipatch_sf2_reader_get_sample_by_preset_index(AgsIpatchSF2Reader *ipatch_sf2_reader,
						 gchar *preset_index)
{
#ifdef AGS_WITH_LIBINSTPATCH
  IpatchSF2 *sf2;
  IpatchContainer *preset;  
  IpatchContainer *instrument;
  
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter preset_iter, instrument_iter, sample_iter;
#endif

  gchar **sample;

  guint i, i_stop;
  guint j, j_stop;
  guint k, k_stop;
  guint count;

  if(!AGS_IS_IPATCH_SF2_READER(ipatch_sf2_reader)){
    return(NULL);
  }
  
  sample = NULL;
  
#ifdef AGS_WITH_LIBINSTPATCH
  sf2 = ipatch_sf2_reader->sf2;
  
  /* presets */
  ipatch_list = ipatch_container_get_children(sf2, IPATCH_TYPE_SF2_PRESET);
  
  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &preset_iter);

    i_stop = ipatch_iter_count(&preset_iter);

    if(i_stop > 0){
      ipatch_iter_index(&preset_iter, preset_index);

      preset = ipatch_iter_get(&preset_iter);

      /* instruments */
      ipatch_list = ipatch_sf2_preset_get_zones(preset);
      
      if(ipatch_list != NULL){
	ipatch_list_init_iter(ipatch_list, &instrument_iter);

	j_stop = ipatch_iter_count(&instrument_iter);
	
	if(j_stop > 0){
	  for(j = 0, count = 0; j < j_stop; j++){
	    instrument = ipatch_iter_get(&instrument_iter);
	    
	    /* samples */
	    ipatch_list = ipatch_sf2_inst_get_zones(preset);

	    if(ipatch_list != NULL){
	      ipatch_list_init_iter(ipatch_list, &sample_iter);

	      k_stop = ipatch_iter_count(&sample_iter);

	      if(k_stop > 0){
		if(sample == NULL){
		  sample = (gchar **) malloc((k_stop + 1) * sizeof(gchar *));
		}else{
		  sample = (gchar **) realloc(sample,
					      (count + k_stop + 1) * sizeof(gchar *));
		}
	    
		for(k = 0; k < k_stop; k++){
		  ipatch_item = ipatch_iter_get(&sample_iter);
		  sample[count] = ipatch_sf2_sample_get_name(ipatch_item);
		    
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

      if(sample != NULL){
	sample[count] = NULL;
      }
    }
  }

  //FIXME:JK: still needed?
  while(g_static_rec_mutex_unlock_full(((IpatchItem *) (sf2))->mutex) != 0);
#endif

  return(sample);
}

/**
 * ags_ipatch_sf2_reader_get_sample_by_preset_and_instrument_index:
 * @ipatch_sf2_reader: the #AgsSF2Reader
 * @preset_index: the preset index
 * 
 * Get sample by preset and instrument index.
 * 
 * Returns: the string vector cotaining sample names or %NULL on failure
 * 
 * Since: 2.0.0
 */
gchar**
ags_ipatch_sf2_reader_get_sample_by_preset_and_instrument_index(AgsIpatchSF2Reader *ipatch_sf2_reader,
								guint preset_index, guint instrument_index)
{
#ifdef AGS_WITH_LIBINSTPATCH
  IpatchSF2 *sf2;
  IpatchContainer *preset;  
  IpatchContainer *instrument;
  
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
  
  IpatchIter preset_iter, instrument_iter, sample_iter;
#endif

  gchar **sample;

  guint i, i_stop;
  guint j, j_stop;
  guint k, k_stop;
  guint count;

  if(!AGS_IS_IPATCH_SF2_READER(ipatch_sf2_reader)){
    return(NULL);
  }
  
  sample = NULL;
  
#ifdef AGS_WITH_LIBINSTPATCH
  sf2 = ipatch_sf2_reader->sf2;
  
  /* presets */
  ipatch_list = ipatch_container_get_children(sf2, IPATCH_TYPE_SF2_PRESET);
  
  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &preset_iter);
    
    i_stop = ipatch_iter_count(&preset_iter);

    if(i_stop > 0 && preset_index < i_stop){
      ipatch_iter_index(&preset_iter, preset_index);

      preset = ipatch_iter_get(&preset_iter);

      /* instruments */
      ipatch_list = ipatch_sf2_preset_get_zones(preset);
      
      if(ipatch_list != NULL){
	ipatch_list_init_iter(ipatch_list, &instrument_iter);

	j_stop = ipatch_iter_count(&instrument_iter);

	if(j_stop > 0 && instrument_index < j_stop){	  
	  ipatch_iter_index(&instrument_iter, instrument_index);

	  instrument = ipatch_iter_get(&instrument_iter);

	  /* samples */
	  ipatch_list = ipatch_sf2_inst_get_zones(instrument);

	  if(ipatch_list != NULL){
	    ipatch_list_init_iter(ipatch_list, &sample_iter);

	    k_stop = ipatch_iter_count(&sample_iter);

	    if(k_stop > 0){
	      sample = (gchar **) malloc((k_stop + 1) * sizeof(gchar *));
	    
	      for(k = 0, count = 0; k < k_stop; k++){
		ipatch_item = ipatch_iter_get(&sample_iter);
		sample[count] = ipatch_sf2_sample_get_name(ipatch_item);
		    
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

  //FIXME:JK: still needed?
  while(g_static_rec_mutex_unlock_full(((IpatchItem *) (sf2))->mutex) != 0);
#endif

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
 * Since: 2.0.0
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
