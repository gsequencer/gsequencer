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

#ifndef __AGS_SF2_LOADER_H__
#define __AGS_SF2_LOADER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>

#include <ags/audio/file/ags_audio_container.h>

G_BEGIN_DECLS

#define AGS_TYPE_SF2_LOADER                (ags_sf2_loader_get_type())
#define AGS_SF2_LOADER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SF2_LOADER, AgsSF2Loader))
#define AGS_SF2_LOADER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SF2_LOADER, AgsSF2LoaderClass))
#define AGS_IS_SF2_LOADER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SF2_LOADER))
#define AGS_IS_SF2_LOADER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SF2_LOADER))
#define AGS_SF2_LOADER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_SF2_LOADER, AgsSF2LoaderClass))

#define AGS_SF2_LOADER_GET_OBJ_MUTEX(obj) (&(((AgsSF2Loader *) obj)->obj_mutex))

typedef struct _AgsSF2Loader AgsSF2Loader;
typedef struct _AgsSF2LoaderClass AgsSF2LoaderClass;

/**
 * AgsSF2LoaderFlags:
 * @AGS_SF2_LOADER_HAS_COMPLETED: has completed
 * 
 * Enum values to configure SF2 loader.
 */
typedef enum{
  AGS_SF2_LOADER_HAS_COMPLETED   = 1,
}AgsSF2LoaderFlags;

struct _AgsSF2Loader
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  GThread *thread;

  AgsAudio *audio;

  gchar *filename;

  gchar *preset;
  gchar *instrument;
  
  AgsAudioContainer *audio_container;
};

struct _AgsSF2LoaderClass
{
  GObjectClass gobject;
};

GType ags_sf2_loader_get_type();

/* flags */
gboolean ags_sf2_loader_test_flags(AgsSF2Loader *sf2_loader, guint flags);
void ags_sf2_loader_set_flags(AgsSF2Loader *sf2_loader, guint flags);
void ags_sf2_loader_unset_flags(AgsSF2Loader *sf2_loader, guint flags);

/* properties */
AgsAudio* ags_sf2_loader_get_audio(AgsSF2Loader *sf2_loader);
void ags_sf2_loader_set_audio(AgsSF2Loader *sf2_loader,
			      AgsAudio *audio);

gchar* ags_sf2_loader_get_filename(AgsSF2Loader *sf2_loader);
void ags_sf2_loader_set_filename(AgsSF2Loader *sf2_loader,
				 gchar *filename);

gchar* ags_sf2_loader_get_preset(AgsSF2Loader *sf2_loader);
void ags_sf2_loader_set_preset(AgsSF2Loader *sf2_loader,
			       gchar *preset);

gchar* ags_sf2_loader_get_instrument(AgsSF2Loader *sf2_loader);
void ags_sf2_loader_set_instrument(AgsSF2Loader *sf2_loader,
				   gchar *instrument);

AgsAudioContainer* ags_sf2_loader_get_audio_container(AgsSF2Loader *sf2_loader);
void ags_sf2_loader_set_audio_container(AgsSF2Loader *sf2_loader,
					AgsAudioContainer *audio_container);

/* thread */
void ags_sf2_loader_start(AgsSF2Loader *sf2_loader);

/* instantiate */
AgsSF2Loader* ags_sf2_loader_new(AgsAudio *audio,
				 gchar *filename,
				 gchar *preset,
				 gchar *instrument);

G_END_DECLS

#endif /*__AGS_SF2_LOADER_H__*/
