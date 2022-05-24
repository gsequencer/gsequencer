/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_WAVE_LOADER_H__
#define __AGS_WAVE_LOADER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>

#include <ags/audio/file/ags_audio_file.h>

G_BEGIN_DECLS

#define AGS_TYPE_WAVE_LOADER                (ags_wave_loader_get_type())
#define AGS_WAVE_LOADER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WAVE_LOADER, AgsWaveLoader))
#define AGS_WAVE_LOADER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WAVE_LOADER, AgsWaveLoaderClass))
#define AGS_IS_WAVE_LOADER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_WAVE_LOADER))
#define AGS_IS_WAVE_LOADER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_WAVE_LOADER))
#define AGS_WAVE_LOADER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_WAVE_LOADER, AgsWaveLoaderClass))

#define AGS_WAVE_LOADER_GET_OBJ_MUTEX(obj) (&(((AgsWaveLoader *) obj)->obj_mutex))

typedef struct _AgsWaveLoader AgsWaveLoader;
typedef struct _AgsWaveLoaderClass AgsWaveLoaderClass;

typedef enum{
  AGS_WAVE_LOADER_DO_REPLACE      = 1,
  AGS_WAVE_LOADER_HAS_COMPLETED   = 1 <<  1,
}AgsWaveLoaderFlags;

struct _AgsWaveLoader
{
  GObject gobject;

  guint flags;
  guint connectable_flags;
  
  GRecMutex obj_mutex;

  GThread *thread;

  AgsAudio *audio;

  gchar *filename;
  
  AgsAudioFile *audio_file;
};

struct _AgsWaveLoaderClass
{
  GObjectClass gobject;
};

GType ags_wave_loader_get_type();

/* flags */
gboolean ags_wave_loader_test_flags(AgsWaveLoader *wave_loader, guint flags);
void ags_wave_loader_set_flags(AgsWaveLoader *wave_loader, guint flags);
void ags_wave_loader_unset_flags(AgsWaveLoader *wave_loader, guint flags);

/* properties */
AgsAudio* ags_wave_loader_get_audio(AgsWaveLoader *wave_loader);
void ags_wave_loader_set_audio(AgsWaveLoader *wave_loader,
			       AgsAudio *audio);

gchar* ags_wave_loader_get_filename(AgsWaveLoader *wave_loader);
void ags_wave_loader_set_filename(AgsWaveLoader *wave_loader,
				  gchar *filename);

AgsAudioFile* ags_wave_loader_get_audio_file(AgsWaveLoader *wave_loader);
void ags_wave_loader_set_audio_file(AgsWaveLoader *wave_loader,
				    AgsAudioFile *audio_file);

/* thread */
void ags_wave_loader_start(AgsWaveLoader *wave_loader);

/* instantiate */
AgsWaveLoader* ags_wave_loader_new(AgsAudio *audio,
				   gchar *filename,
				   gboolean do_replace);

G_END_DECLS

#endif /*__AGS_WAVE_LOADER_H__*/
