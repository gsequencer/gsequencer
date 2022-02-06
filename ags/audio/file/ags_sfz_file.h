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

#ifndef __AGS_SFZ_FILE_H__
#define __AGS_SFZ_FILE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <stdio.h>

G_BEGIN_DECLS

#define AGS_TYPE_SFZ_FILE                (ags_sfz_file_get_type())
#define AGS_SFZ_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SFZ_FILE, AgsSFZFile))
#define AGS_SFZ_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SFZ_FILE, AgsSFZFileClass))
#define AGS_IS_SFZ_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SFZ_FILE))
#define AGS_IS_SFZ_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SFZ_FILE))
#define AGS_SFZ_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SFZ_FILE, AgsSFZFileClass))

#define AGS_SFZ_FILE_GET_OBJ_MUTEX(obj) (&(((AgsSFZFile *) obj)->obj_mutex))

#define AGS_SFZ_FILE_DEFAULT_CHANNELS (2)
#define AGS_SFZ_FILE_LOOP_MAX (4294967296)

#define AGS_SFZ_FILE_READ "r"
#define AGS_SFZ_FILE_WRITE "w"

typedef struct _AgsSFZFile AgsSFZFile;
typedef struct _AgsSFZFileClass AgsSFZFileClass;

/**
 * AgsSFZFileFlags:
 * @AGS_SFZ_FILE_ADDED_TO_REGISTRY: the sfz_file was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_SFZ_FILE_CONNECTED: indicates the sfz_file was connected by calling #AgsConnectable::connect()
 * 
 * Enum values to control the behavior or indicate internal state of #AgsSFZFile by
 * enable/disable as flags.
 */
typedef enum{
  AGS_SFZ_FILE_ADDED_TO_REGISTRY    = 1,
  AGS_SFZ_FILE_CONNECTED            = 1 <<  1,
}AgsSFZFileFlags;

/**
 * AgsSFZLevel:
 * @AGS_SFZ_LEVEL_FILENAME: filename
 * @AGS_SFZ_LEVEL_SAMPLE: sample
 * 
 * Enum values to describe the different levels of a SFZ file.
 */
typedef enum{
  AGS_SFZ_LEVEL_FILENAME = 0,
  AGS_SFZ_LEVEL_SAMPLE   = 1,
}AgsSFZLevel;

struct _AgsSFZFile
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  AgsUUID *uuid;

  GObject *soundcard;
  
  char *filename;
  char *mode;

  FILE *file;

  guint nesting_level;
  
  gchar *level_id;
  guint level_index;

  GObject *reader;
  GObject *writer;

  GList *group;
  GList *region;
  GList *sample;

  guint *index_selected;
  gchar **name_selected;

  gpointer current_sample;

  GList *audio_signal;
};

struct _AgsSFZFileClass
{
  GObjectClass gobject;
};

GType ags_sfz_file_get_type();

gboolean ags_sfz_file_test_flags(AgsSFZFile *sfz_file, guint flags);
void ags_sfz_file_set_flags(AgsSFZFile *sfz_file, guint flags);
void ags_sfz_file_unset_flags(AgsSFZFile *sfz_file, guint flags);

GList* ags_sfz_file_get_group(AgsSFZFile *sfz_file);
void ags_sfz_file_set_group(AgsSFZFile *sfz_file,
			    GList *group);

GList* ags_sfz_file_get_region(AgsSFZFile *sfz_file);
void ags_sfz_file_set_region(AgsSFZFile *sfz_file,
			     GList *region);

GList* ags_sfz_file_get_sample(AgsSFZFile *sfz_file);
void ags_sfz_file_set_sample(AgsSFZFile *sfz_file,
			     GList *sample);

gboolean ags_sfz_file_select_sample(AgsSFZFile *sfz_file,
				    guint sample_index);

void ags_sfz_file_get_range(AgsSFZFile *sfz_file,
			    glong *hikey, glong *lokey);

gboolean ags_sfz_file_check_suffix(gchar *filename);

void ags_sfz_file_parse(AgsSFZFile *sfz_file);

AgsSFZFile* ags_sfz_file_new();

G_END_DECLS

#endif /*__AGS_SFZ_FILE_H__*/
