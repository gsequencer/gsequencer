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

#ifndef __AGS_SNDFILE_H__
#define __AGS_SNDFILE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <sndfile.h>

#define AGS_TYPE_SNDFILE                (ags_sndfile_get_type())
#define AGS_SNDFILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SNDFILE, AgsSndfile))
#define AGS_SNDFILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SNDFILE, AgsSndfileClass))
#define AGS_IS_SNDFILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SNDFILE))
#define AGS_IS_SNDFILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SNDFILE))
#define AGS_SNDFILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SNDFILE, AgsSndfileClass))

typedef struct _AgsSndfile AgsSndfile;
typedef struct _AgsSndfileClass AgsSndfileClass;

/**
 * AgsSndfileFlags:
 * @AGS_SNDFILE_ADDED_TO_REGISTRY: the sndfile was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_SNDFILE_CONNECTED: indicates the sndfile was connected by calling #AgsConnectable::connect()
 * @AGS_SNDFILE_VIRTUAL: virtual IO
 * 
 * Enum values to control the behavior or indicate internal state of #AgsSndfile by
 * enable/disable as flags.
 */
typedef enum{
  AGS_SNDFILE_ADDED_TO_REGISTRY    = 1,
  AGS_SNDFILE_CONNECTED            = 1 <<  1,
  AGS_SNDFILE_VIRTUAL              = 1 <<  2,
  AGS_SNDFILE_FILL_CACHE           = 1 <<  3,
}AgsSndfileFlags;

struct _AgsSndfile
{
  GObject object;

  guint flags;

  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  AgsUUID *uuid;

  guint audio_channels;
  gint64 *audio_channel_written;
  
  guint buffer_size;
  guint format;

  guint64 offset;
  guint64 buffer_offset;

  void *full_buffer;
  void *buffer;

  guchar *pointer;
  guchar *current;
  gsize length;

  SF_INFO *info;
  SNDFILE *file;
};

struct _AgsSndfileClass
{
  GObjectClass object;
};

GType ags_sndfile_get_type();

pthread_mutex_t* ags_sndfile_get_class_mutex();

gboolean ags_sndfile_test_flags(AgsSndfile *sndfile, guint flags);
void ags_sndfile_set_flags(AgsSndfile *sndfile, guint flags);
void ags_sndfile_unset_flags(AgsSndfile *sndfile, guint flags);

AgsSndfile* ags_sndfile_new();

#endif /*__AGS_SNDFILE_H__*/
