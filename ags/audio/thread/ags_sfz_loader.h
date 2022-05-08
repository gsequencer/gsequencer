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

#ifndef __AGS_SFZ_LOADER_H__
#define __AGS_SFZ_LOADER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>

#include <ags/audio/file/ags_audio_container.h>

G_BEGIN_DECLS

#define AGS_TYPE_SFZ_LOADER                (ags_sfz_loader_get_type())
#define AGS_SFZ_LOADER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SFZ_LOADER, AgsSFZLoader))
#define AGS_SFZ_LOADER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SFZ_LOADER, AgsSFZLoaderClass))
#define AGS_IS_SFZ_LOADER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SFZ_LOADER))
#define AGS_IS_SFZ_LOADER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SFZ_LOADER))
#define AGS_SFZ_LOADER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_SFZ_LOADER, AgsSFZLoaderClass))

#define AGS_SFZ_LOADER_GET_OBJ_MUTEX(obj) (&(((AgsSFZLoader *) obj)->obj_mutex))

typedef struct _AgsSFZLoader AgsSFZLoader;
typedef struct _AgsSFZLoaderClass AgsSFZLoaderClass;

/**
 * AgsSFZLoaderFlags:
 * @AGS_SFZ_LOADER_DO_REPLACE: do replace audio signal
 * @AGS_SFZ_LOADER_HAS_COMPLETED: has completed
 * @AGS_SFZ_LOADER_RUN_APPLY_SYNTH: run apply synth
 * 
 * Enum values to configure SFZ loader.
 */
typedef enum{
  AGS_SFZ_LOADER_DO_REPLACE         = 1,
  AGS_SFZ_LOADER_HAS_COMPLETED      = 1 <<  1,
  AGS_SFZ_LOADER_RUN_APPLY_SYNTH    = 1 <<  2,
}AgsSFZLoaderFlags;

struct _AgsSFZLoader
{
  GObject gobject;

  guint flags;
  guint connectable_flags;
  
  GRecMutex obj_mutex;

  GThread *thread;

  AgsAudio *audio;

  gchar *filename;

  gdouble base_note;
  guint count;
  
  AgsAudioContainer *audio_container;
};

struct _AgsSFZLoaderClass
{
  GObjectClass gobject;
};

GType ags_sfz_loader_get_type();

/* flags */
gboolean ags_sfz_loader_test_flags(AgsSFZLoader *sfz_loader, guint flags);
void ags_sfz_loader_set_flags(AgsSFZLoader *sfz_loader, guint flags);
void ags_sfz_loader_unset_flags(AgsSFZLoader *sfz_loader, guint flags);

/* properties */
AgsAudio* ags_sfz_loader_get_audio(AgsSFZLoader *sfz_loader);
void ags_sfz_loader_set_audio(AgsSFZLoader *sfz_loader,
			      AgsAudio *audio);

gchar* ags_sfz_loader_get_filename(AgsSFZLoader *sfz_loader);
void ags_sfz_loader_set_filename(AgsSFZLoader *sfz_loader,
				 gchar *filename);

AgsAudioContainer* ags_sfz_loader_get_audio_container(AgsSFZLoader *sfz_loader);
void ags_sfz_loader_set_audio_container(AgsSFZLoader *sfz_loader,
					AgsAudioContainer *audio_container);

/* thread */
void ags_sfz_loader_start(AgsSFZLoader *sfz_loader);

/* instantiate */
AgsSFZLoader* ags_sfz_loader_new(AgsAudio *audio,
				 gchar *filename,
				 gboolean do_replace);

G_END_DECLS

#endif /*__AGS_SFZ_LOADER_H__*/
