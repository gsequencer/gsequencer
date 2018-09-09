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
static AgsConnectableInterface *ags_ipatch_gig_reader_parent_connectable_interface;

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
  ags_ipatch_gig_reader_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_ipatch_gig_reader_init(AgsIpatchGigReader *ipatch_gig_reader)
{
  guint i;
  
  ipatch_gig_reader->ipatch = NULL;

  /* reader */
  ipatch_gig_reader->reader = NULL;
  ipatch_gig_reader->gig = NULL;
  
  /* selected */
  ipatch_gig_reader->index_selected = (guint *) malloc(3 * sizeof(guint));
  memset(ipatch_gig_reader->index_selected, 0, 3 * sizeof(guint));
  
  ipatch_gig_reader->name_selected = (gchar **) malloc(4 * sizeof(gchar *));

  for(i = 0; i < 4; i++){
    ipatch_gig_reader->name_selected[i] = NULL;
  }

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

  ipatch_gig_reader = AGS_IPATCH_GIG_READER(gobject);

  switch(prop_id){
  case PROP_IPATCH:
    {
      AgsIpatch *ipatch;

      ipatch = (AgsIpatch *) g_value_get_object(value);

      if(ipatch_gig_reader->ipatch == ipatch){
	return;
      }
      
      if(ipatch_gig_reader->ipatch != NULL){
	g_object_unref(ipatch_gig_reader->ipatch);
      }

      if(ipatch != NULL){
	g_object_ref(ipatch);
      }

      ipatch_gig_reader->ipatch = ipatch;
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

  ipatch_gig_reader = AGS_IPATCH_GIG_READER(gobject);

  switch(prop_id){
  case PROP_IPATCH:
    {
      g_value_set_object(value, ipatch_gig_reader->ipatch);
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

  if(ipatch_gig_reader->ipatch != NULL){
    g_object_unref(ipatch_gig_reader->ipatch);
  }

  /* call parent */  
  G_OBJECT_CLASS(ags_ipatch_gig_reader_parent_class)->finalize(gobject);
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
