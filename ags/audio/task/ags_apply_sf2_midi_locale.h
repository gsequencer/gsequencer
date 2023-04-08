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

#ifndef __AGS_APPLY_SF2_MIDI_LOCALE_H__
#define __AGS_APPLY_SF2_MIDI_LOCALE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_sf2_synth_util.h>

G_BEGIN_DECLS

#define AGS_TYPE_APPLY_SF2_MIDI_LOCALE                (ags_apply_sf2_midi_locale_get_type())
#define AGS_APPLY_SF2_MIDI_LOCALE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_APPLY_SF2_MIDI_LOCALE, AgsApplySF2MidiLocale))
#define AGS_APPLY_SF2_MIDI_LOCALE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_APPLY_SF2_MIDI_LOCALE, AgsApplySF2MidiLocaleClass))
#define AGS_IS_APPLY_SF2_MIDI_LOCALE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_APPLY_SF2_MIDI_LOCALE))
#define AGS_IS_APPLY_SF2_MIDI_LOCALE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_APPLY_SF2_MIDI_LOCALE))
#define AGS_APPLY_SF2_MIDI_LOCALE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_APPLY_SF2_MIDI_LOCALE, AgsApplySF2MidiLocaleClass))

typedef struct _AgsApplySF2MidiLocale AgsApplySF2MidiLocale;
typedef struct _AgsApplySF2MidiLocaleClass AgsApplySF2MidiLocaleClass;

struct _AgsApplySF2MidiLocale
{
  AgsTask task;

  AgsSF2SynthUtil *synth_template;
  AgsSF2SynthUtil *synth;
};

struct _AgsApplySF2MidiLocaleClass
{
  AgsTaskClass task;
};

GType ags_apply_sf2_midi_locale_get_type();

AgsApplySF2MidiLocale* ags_apply_sf2_midi_locale_new(AgsSF2SynthUtil *synth_template,
						     AgsSF2SynthUtil *synth);

G_END_DECLS

#endif /*__AGS_APPLY_SF2_MIDI_LOCALE_H__*/
