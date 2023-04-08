/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_SFZ_INSTRUMENT_LOADER_H__
#define __AGS_SFZ_INSTRUMENT_LOADER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_sfz_synth_util.h>

#include <ags/audio/file/ags_audio_container.h>

G_BEGIN_DECLS

#define AGS_TYPE_SFZ_INSTRUMENT_LOADER                (ags_sfz_instrument_loader_get_type())
#define AGS_SFZ_INSTRUMENT_LOADER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SFZ_INSTRUMENT_LOADER, AgsSFZInstrumentLoader))
#define AGS_SFZ_INSTRUMENT_LOADER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SFZ_INSTRUMENT_LOADER, AgsSFZInstrumentLoaderClass))
#define AGS_IS_SFZ_INSTRUMENT_LOADER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SFZ_INSTRUMENT_LOADER))
#define AGS_IS_SFZ_INSTRUMENT_LOADER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SFZ_INSTRUMENT_LOADER))
#define AGS_SFZ_INSTRUMENT_LOADER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_SFZ_INSTRUMENT_LOADER, AgsSFZInstrumentLoaderClass))

#define AGS_SFZ_INSTRUMENT_LOADER_GET_OBJ_MUTEX(obj) (&(((AgsSFZInstrumentLoader *) obj)->obj_mutex))

typedef struct _AgsSFZInstrumentLoader AgsSFZInstrumentLoader;
typedef struct _AgsSFZInstrumentLoaderClass AgsSFZInstrumentLoaderClass;

/**
 * AgsSFZInstrumentLoaderFlags:
 * @AGS_SFZ_INSTRUMENT_LOADER_HAS_COMPLETED: has completed
 * @AGS_SFZ_INSTRUMENT_LOADER_RUN_APPLY_SYNTH: run apply synth
 * 
 * Enum values to configure Soundfont2 MIDI locale loader.
 */
typedef enum{
  AGS_SFZ_INSTRUMENT_LOADER_HAS_COMPLETED               = 1,
  AGS_SFZ_INSTRUMENT_LOADER_RUN_APPLY_INSTRUMENT       = 1 <<  1,
}AgsSFZInstrumentLoaderFlags;

struct _AgsSFZInstrumentLoader
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

  AgsSFZSynthUtil *synth_template;
  AgsSFZSynthUtil *synth;
};

struct _AgsSFZInstrumentLoaderClass
{
  GObjectClass gobject;
};

GType ags_sfz_instrument_loader_get_type();

/* flags */
gboolean ags_sfz_instrument_loader_test_flags(AgsSFZInstrumentLoader *sfz_instrument_loader, guint flags);
void ags_sfz_instrument_loader_set_flags(AgsSFZInstrumentLoader *sfz_instrument_loader, guint flags);
void ags_sfz_instrument_loader_unset_flags(AgsSFZInstrumentLoader *sfz_instrument_loader, guint flags);

/* properties */
AgsAudio* ags_sfz_instrument_loader_get_audio(AgsSFZInstrumentLoader *sfz_instrument_loader);
void ags_sfz_instrument_loader_set_audio(AgsSFZInstrumentLoader *sfz_instrument_loader,
					 AgsAudio *audio);

gchar* ags_sfz_instrument_loader_get_filename(AgsSFZInstrumentLoader *sfz_instrument_loader);
void ags_sfz_instrument_loader_set_filename(AgsSFZInstrumentLoader *sfz_instrument_loader,
					    gchar *filename);

AgsAudioContainer* ags_sfz_instrument_loader_get_audio_container(AgsSFZInstrumentLoader *sfz_instrument_loader);
void ags_sfz_instrument_loader_set_audio_container(AgsSFZInstrumentLoader *sfz_instrument_loader,
						   AgsAudioContainer *audio_container);

AgsSFZSynthUtil* ags_sfz_instrument_loader_get_synth_template(AgsSFZInstrumentLoader *sfz_instrument_loader);
void ags_sfz_instrument_loader_set_synth_template(AgsSFZInstrumentLoader *sfz_instrument_loader,
						  AgsSFZSynthUtil *synth_template);

AgsSFZSynthUtil* ags_sfz_instrument_loader_get_synth(AgsSFZInstrumentLoader *sfz_instrument_loader);
void ags_sfz_instrument_loader_set_synth(AgsSFZInstrumentLoader *sfz_instrument_loader,
					 AgsSFZSynthUtil *synth);

/* thread */
void ags_sfz_instrument_loader_start(AgsSFZInstrumentLoader *sfz_instrument_loader);

/* instantiate */
AgsSFZInstrumentLoader* ags_sfz_instrument_loader_new(AgsAudio *audio,
						      gchar *filename);

G_END_DECLS

#endif /*__AGS_SFZ_INSTRUMENT_LOADER_H__*/
