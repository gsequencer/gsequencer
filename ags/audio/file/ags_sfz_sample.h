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

#ifndef __AGS_SFZ_SAMPLE_H__
#define __AGS_SFZ_SAMPLE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <sndfile.h>

#define AGS_TYPE_SFZ_SAMPLE                (ags_sfz_sample_get_type())
#define AGS_SFZ_SAMPLE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SFZ_SAMPLE, AgsSFZSample))
#define AGS_SFZ_SAMPLE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SFZ_SAMPLE, AgsSFZSampleClass))
#define AGS_IS_SFZ_SAMPLE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SFZ_SAMPLE))
#define AGS_IS_SFZ_SAMPLE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SFZ_SAMPLE))
#define AGS_SFZ_SAMPLE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_SFZ_SAMPLE, AgsSFZSampleClass))

#define AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(obj) (((AgsSFZSample *) obj)->obj_mutex)

typedef struct _AgsSFZSample AgsSFZSample;
typedef struct _AgsSFZSampleClass AgsSFZSampleClass;

/**
 * AgsSFZSampleFlags:
 * @AGS_SFZ_SAMPLE_ADDED_TO_REGISTRY: the sfz sample was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_SFZ_SAMPLE_CONNECTED: indicates the sfz sample was connected by calling #AgsConnectable::connect()
 * 
 * Enum values to control the behavior or indicate internal state of #AgsSFZSample by
 * enable/disable as flags.
 */
typedef enum{
  AGS_SFZ_SAMPLE_ADDED_TO_REGISTRY    = 1,
  AGS_SFZ_SAMPLE_CONNECTED            = 1 <<  1,
}AgsSFZSampleFlags;

struct _AgsSFZSample
{
  GObject gobject;

  guint flags;

  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  AgsUUID *uuid;

  gchar *filename;
  
  guint audio_channels;
  gint64 *audio_channel_written;

  guint buffer_size;
  guint format;

  guint loop_start;
  guint loop_end;
  
  guint offset;
  guint64 buffer_offset;

  void *full_buffer;
  void *buffer;

  guchar *pointer;
  guchar *current;
  gsize length;
  
  SF_INFO *info;
  SNDFILE *file;

  GObject *group;
  GObject *region;
};

struct _AgsSFZSampleClass
{
  GObjectClass gobject;
};

GType ags_sfz_sample_get_type();

pthread_mutex_t* ags_sfz_sample_get_class_mutex();

gboolean ags_sfz_sample_test_flags(AgsSFZSample *sfz_sample, guint flags);
void ags_sfz_sample_set_flags(AgsSFZSample *sfz_sample, guint flags);
void ags_sfz_sample_unset_flags(AgsSFZSample *sfz_sample, guint flags);

/* instantiate */
AgsSFZSample* ags_sfz_sample_new();

#endif /*__AGS_SFZ_SAMPLE_H__*/
