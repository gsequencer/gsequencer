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

#ifndef __AGS_SF2_MIDI_LOCALE_LOADER_H__
#define __AGS_SF2_MIDI_LOCALE_LOADER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_sf2_synth_util.h>

#include <ags/audio/file/ags_audio_container.h>

G_BEGIN_DECLS

#define AGS_TYPE_SF2_MIDI_LOCALE_LOADER                (ags_sf2_midi_locale_loader_get_type())
#define AGS_SF2_MIDI_LOCALE_LOADER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SF2_MIDI_LOCALE_LOADER, AgsSF2MidiLocaleLoader))
#define AGS_SF2_MIDI_LOCALE_LOADER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SF2_MIDI_LOCALE_LOADER, AgsSF2MidiLocaleLoaderClass))
#define AGS_IS_SF2_MIDI_LOCALE_LOADER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SF2_MIDI_LOCALE_LOADER))
#define AGS_IS_SF2_MIDI_LOCALE_LOADER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SF2_MIDI_LOCALE_LOADER))
#define AGS_SF2_MIDI_LOCALE_LOADER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_SF2_MIDI_LOCALE_LOADER, AgsSF2MidiLocaleLoaderClass))

#define AGS_SF2_MIDI_LOCALE_LOADER_GET_OBJ_MUTEX(obj) (&(((AgsSF2MidiLocaleLoader *) obj)->obj_mutex))

typedef struct _AgsSF2MidiLocaleLoader AgsSF2MidiLocaleLoader;
typedef struct _AgsSF2MidiLocaleLoaderClass AgsSF2MidiLocaleLoaderClass;

/**
 * AgsSF2MidiLocaleLoaderFlags:
 * @AGS_SF2_MIDI_LOCALE_LOADER_HAS_COMPLETED: has completed
 * @AGS_SF2_MIDI_LOCALE_LOADER_RUN_APPLY_SYNTH: run apply synth
 * 
 * Enum values to configure Soundfont2 MIDI locale loader.
 */
typedef enum{
  AGS_SF2_MIDI_LOCALE_LOADER_HAS_COMPLETED               = 1,
  AGS_SF2_MIDI_LOCALE_LOADER_RUN_APPLY_MIDI_LOCALE       = 1 <<  1,
}AgsSF2MidiLocaleLoaderFlags;

struct _AgsSF2MidiLocaleLoader
{
  GObject gobject;

  guint flags;
  guint connectable_flags;
  
  GRecMutex obj_mutex;

  GThread *thread;

  AgsAudio *audio;

  gchar *filename;

  gint bank;
  gint program;
  
  gdouble base_note;
  guint count;
  
  AgsAudioContainer *audio_container;

  AgsSF2SynthUtil *synth_template;
  AgsSF2SynthUtil *synth;
};

struct _AgsSF2MidiLocaleLoaderClass
{
  GObjectClass gobject;
};

GType ags_sf2_midi_locale_loader_get_type();

/* flags */
gboolean ags_sf2_midi_locale_loader_test_flags(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader, guint flags);
void ags_sf2_midi_locale_loader_set_flags(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader, guint flags);
void ags_sf2_midi_locale_loader_unset_flags(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader, guint flags);

/* properties */
AgsAudio* ags_sf2_midi_locale_loader_get_audio(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader);
void ags_sf2_midi_locale_loader_set_audio(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader,
					  AgsAudio *audio);

gchar* ags_sf2_midi_locale_loader_get_filename(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader);
void ags_sf2_midi_locale_loader_set_filename(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader,
					     gchar *filename);

gint ags_sf2_midi_locale_loader_get_bank(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader);
void ags_sf2_midi_locale_loader_set_bank(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader,
					 gint bank);

gint ags_sf2_midi_locale_loader_get_program(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader);
void ags_sf2_midi_locale_loader_set_program(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader,
					    gint program);

AgsAudioContainer* ags_sf2_midi_locale_loader_get_audio_container(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader);
void ags_sf2_midi_locale_loader_set_audio_container(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader,
						    AgsAudioContainer *audio_container);

AgsSF2SynthUtil* ags_sf2_midi_locale_loader_get_synth_template(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader);
void ags_sf2_midi_locale_loader_set_synth_template(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader,
						   AgsSF2SynthUtil *synth_template);

AgsSF2SynthUtil* ags_sf2_midi_locale_loader_get_synth(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader);
void ags_sf2_midi_locale_loader_set_synth(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader,
					  AgsSF2SynthUtil *synth);

/* thread */
void ags_sf2_midi_locale_loader_start(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader);

/* instantiate */
AgsSF2MidiLocaleLoader* ags_sf2_midi_locale_loader_new(AgsAudio *audio,
						       gchar *filename,
						       gint bank,
						       gint program);

G_END_DECLS

#endif /*__AGS_SF2_MIDI_LOCALE_LOADER_H__*/
