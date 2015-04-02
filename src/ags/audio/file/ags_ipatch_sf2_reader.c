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

#include <ags/audio/file/ags_ipatch_sf2_reader.h>

#include <ags-lib/object/ags_connectable.h>

#include <stdlib.h>

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
      sizeof (AgsIpatchSF2ReaderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ipatch_sf2_reader_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsIpatchSF2Reader),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ipatch_sf2_reader_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ipatch_sf2_reader_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ipatch_sf2_reader = g_type_register_static(G_TYPE_OBJECT,
					     "AgsIpatchSF2Reader\0",
					     &ags_ipatch_sf2_reader_info,
					     0);

    g_type_add_interface_static(ags_type_ipatch_sf2_reader,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_ipatch_sf2_reader);
}

void
ags_ipatch_sf2_reader_class_init(AgsIpatchSF2ReaderClass *ipatch_sf2_reader)
{
  GObjectClass *gobject;

  ags_ipatch_sf2_reader_parent_class = g_type_class_peek_parent(ipatch_sf2_reader);

  gobject = (GObjectClass *) ipatch_sf2_reader;

  gobject->set_property = ags_ipatch_sf2_reader_set_property;
  gobject->get_property = ags_ipatch_sf2_reader_get_property;

  gobject->finalize = ags_ipatch_sf2_reader_finalize;
}

void
ags_ipatch_sf2_reader_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_ipatch_sf2_reader_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_ipatch_sf2_reader_connect;
  connectable->disconnect = ags_ipatch_sf2_reader_disconnect;
}

void
ags_ipatch_sf2_reader_init(AgsIpatchSF2Reader *ipatch_sf2_reader)
{
  ipatch_sf2_reader->ipatch = NULL;

  ipatch_sf2_reader->bank = -1;
  ipatch_sf2_reader->program = -1;

  ipatch_sf2_reader->selected = (gchar **) malloc(4 * sizeof(gchar *));

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

      if(ipatch_sf2_reader->ipatch != NULL){
	g_object_unref(ipatch_sf2_reader->ipatch);
      }

      ipatch_sf2_reader->ipatch = ipatch;
     
      if(ipatch != NULL){
	g_object_ref(ipatch);
	ipatch->reader = (GObject *) ipatch_sf2_reader;
      }
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
    g_value_set_object(value, ipatch_sf2_reader->ipatch);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ipatch_sf2_reader_connect(AgsConnectable *connectable)
{
  ags_ipatch_sf2_reader_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_ipatch_sf2_reader_disconnect(AgsConnectable *connectable)
{
  ags_ipatch_sf2_reader_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_ipatch_sf2_reader_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_ipatch_sf2_reader_parent_class)->finalize(gobject);

  /* empty */
}


/**
 * ags_ipatch_sf2_reader_new:
 *
 * Creates an #AgsIpatchSF2Reader.
 *
 * Returns: an empty #AgsIpatchSF2Reader.
 *
 * Since: 0.4
 */
AgsIpatchSF2Reader*
ags_ipatch_sf2_reader_new()
{
  AgsIpatchSF2Reader *ipatch_sf2_reader;

  ipatch_sf2_reader = (AgsIpatchSF2Reader *) g_object_new(AGS_TYPE_IPATCH_SF2_READER,
							  NULL);

  return(ipatch_sf2_reader);
}
