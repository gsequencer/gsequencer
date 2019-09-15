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

#ifndef __AGS_IPATCH_DLS2_READER_H__
#define __AGS_IPATCH_DLS2_READER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/config.h>

#include <ags/libags.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <libinstpatch/libinstpatch.h>
#endif

#include <ags/audio/file/ags_ipatch.h>

#define AGS_TYPE_IPATCH_DLS2_READER                (ags_ipatch_dls2_reader_get_type())
#define AGS_IPATCH_DLS2_READER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_IPATCH_DLS2_READER, AgsIpatchDLS2Reader))
#define AGS_IPATCH_DLS2_READER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_IPATCH_DLS2_READER, AgsIpatchDLS2ReaderClass))
#define AGS_IS_IPATCH_DLS2_READER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_IPATCH_DLS2_READER))
#define AGS_IS_IPATCH_DLS2_READER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_IPATCH_DLS2_READER))
#define AGS_IPATCH_DLS2_READER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_IPATCH_DLS2_READER, AgsIpatchDLS2ReaderClass))

typedef struct _AgsIpatchDLS2Reader AgsIpatchDLS2Reader;
typedef struct _AgsIpatchDLS2ReaderClass AgsIpatchDLS2ReaderClass;

/**
 * AgsIpatchDLS2ReaderFlags:
 * @AGS_IPATCH_DLS2_READER_ADDED_TO_REGISTRY: the ipatch sample was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_IPATCH_DLS2_READER_CONNECTED: indicates the ipatch sample was connected by calling #AgsConnectable::connect()
 * 
 * Enum values to control the behavior or indicate internal state of #AgsIpatchDLS2Reader by
 * enable/disable as flags.
 */
typedef enum{
  AGS_IPATCH_DLS2_READER_ADDED_TO_REGISTRY    = 1,
  AGS_IPATCH_DLS2_READER_CONNECTED            = 1 <<  1,
}AgsIpatchDLS2ReaderFlags;

/**
 * AgsDLS2Level:
 * @AGS_DLS2_FILENAME: filename
 * @AGS_DLS2_IHDR: instrument header
 * @AGS_DLS2_SHDR: sample header
 * 
 * Enum values to describe the different levels of a DLS2 file.
 */
typedef enum{
  AGS_DLS2_FILENAME = 0,
  AGS_DLS2_IHDR = 1,
  AGS_DLS2_SHDR = 2,
}AgsDLS2Level;

struct _AgsIpatchDLS2Reader
{
  GObject gobject;

  guint flags;

  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  AgsUUID *uuid;

  guint level;
  
  AgsIpatch *ipatch;

  guint *index_selected;
  gchar **name_selected;
  	
#ifdef AGS_WITH_LIBINSTPATCH
  IpatchDLSReader *reader;

  IpatchBase *base;
  IpatchDLS2 *dls2;

  IpatchContainer *instrument;
  IpatchContainer *sample;
#else
  gpointer reader;

  gpointer base;
  gpointer gig;

  gpointer instrument;
  gpointer sample;
#endif

  GError *error;
};

struct _AgsIpatchDLS2ReaderClass
{
  GObjectClass gobject;
};

GType ags_ipatch_dls2_reader_get_type();

pthread_mutex_t* ags_ipatch_dls2_reader_get_class_mutex();

gboolean ags_ipatch_dls2_reader_test_flags(AgsIpatchDLS2Reader *ipatch_dls2_reader, guint flags);
void ags_ipatch_dls2_reader_set_flags(AgsIpatchDLS2Reader *ipatch_dls2_reader, guint flags);
void ags_ipatch_dls2_reader_unset_flags(AgsIpatchDLS2Reader *ipatch_dls2_reader, guint flags);

#ifdef AGS_WITH_LIBINSTPATCH
gboolean ags_ipatch_dls2_reader_load(AgsIpatchDLS2Reader *ipatch_dls2_reader,
				     IpatchFileHandle *handle);
#endif

/* select sample */
gboolean ags_ipatch_dls2_reader_select_instrument(AgsIpatchDLS2Reader *ipatch_dls2_reader,
						  guint instrument_index);
gboolean ags_ipatch_dls2_reader_select_sample(AgsIpatchDLS2Reader *ipatch_dls2_reader,
					      guint sample_index);

/* query */
gchar** ags_ipatch_dls2_reader_get_instrument_all(AgsIpatchDLS2Reader *ipatch_dls2_reader);
gchar** ags_ipatch_dls2_reader_get_sample_all(AgsIpatchDLS2Reader *ipatch_dls2_reader);

gchar** ags_ipatch_dls2_reader_get_sample_by_instrument_index(AgsIpatchDLS2Reader *ipatch_dls2_reader,
							      guint instrument_index);

/* instantiate */
AgsIpatchDLS2Reader* ags_ipatch_dls2_reader_new(AgsIpatch *ipatch);

#endif /*__AGS_IPATCH_DLS2_READER_H__*/
