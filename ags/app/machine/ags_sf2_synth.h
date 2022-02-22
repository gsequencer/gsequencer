/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_SF2_SYNTH_H__
#define __AGS_SF2_SYNTH_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_SF2_SYNTH                (ags_sf2_synth_get_type())
#define AGS_SF2_SYNTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SF2_SYNTH, AgsSF2Synth))
#define AGS_SF2_SYNTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SF2_SYNTH, AgsSF2SynthClass))
#define AGS_IS_SF2_SYNTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SF2_SYNTH))
#define AGS_IS_SF2_SYNTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SF2_SYNTH))
#define AGS_SF2_SYNTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SF2_SYNTH, AgsSF2SynthClass))

#define AGS_SF2_SYNTH_BANK_HEIGHT_REQUEST (256)
#define AGS_SF2_SYNTH_BANK_WIDTH_REQUEST (256)
  
#define AGS_SF2_SYNTH_PROGRAM_HEIGHT_REQUEST (256)
#define AGS_SF2_SYNTH_PROGRAM_WIDTH_REQUEST (512)

#define AGS_SF2_SYNTH_INPUT_LINE(ptr) ((AgsSF2SynthInputLine *)(ptr))

typedef struct _AgsSF2Synth AgsSF2Synth;
typedef struct _AgsSF2SynthInputLine AgsSF2SynthInputLine;
typedef struct _AgsSF2SynthClass AgsSF2SynthClass;

typedef enum{
  AGS_SF2_SYNTH_NO_LOAD          = 1,
}AgsSF2SynthFlags;

struct _AgsSF2Synth
{
  AgsMachine machine;

  guint flags;

  gchar *name;
  gchar *xml_type;

  guint mapped_output_audio_channel;
  guint mapped_input_audio_channel;

  guint mapped_input_pad;
  guint mapped_output_pad;

  AgsRecallContainer *playback_play_container;
  AgsRecallContainer *playback_recall_container;

  AgsRecallContainer *sf2_synth_play_container;
  AgsRecallContainer *sf2_synth_recall_container;

  AgsRecallContainer *envelope_play_container;
  AgsRecallContainer *envelope_recall_container;
  
  AgsRecallContainer *buffer_play_container;
  AgsRecallContainer *buffer_recall_container;

  AgsAudioContainer *audio_container;

  GtkEntry *filename;
  GtkButton *open;

  AgsSF2Loader *sf2_loader;
  AgsSF2MidiLocaleLoader *sf2_midi_locale_loader;

  gint load_bank;
  gint load_program;

  gint position;
  GtkSpinner *sf2_loader_spinner;  

  gint bank;
  gint program;
  
  GtkTreeView *bank_tree_view;
  GtkTreeView *program_tree_view;

  AgsDial *synth_octave;
  AgsDial *synth_key;

  AgsDial *synth_volume;

  GtkCheckButton *chorus_enabled;

  AgsDial *chorus_input_volume;
  AgsDial *chorus_output_volume;

  GtkComboBox *chorus_lfo_oscillator;  
  GtkSpinButton *chorus_lfo_frequency;

  AgsDial *chorus_depth;
  AgsDial *chorus_mix;
  AgsDial *chorus_delay;
    
  GtkWidget *open_dialog;
};

struct _AgsSF2SynthClass
{
  AgsMachineClass machine;
};

struct _AgsSF2SynthInputLine
{
  guint pad;
  guint audio_channel;

  guint line;
  
  gboolean mapped_recall;
};

GType ags_sf2_synth_get_type(void);

void ags_sf2_synth_open_filename(AgsSF2Synth *sf2_synth,
				 gchar *filename);

void ags_sf2_synth_load_bank(AgsSF2Synth *sf2_synth,
			     gint bank);

void ags_sf2_synth_load_midi_locale(AgsSF2Synth *sf2_synth,
				    gint bank,
				    gint program);

gboolean ags_sf2_synth_sf2_loader_completed_timeout(AgsSF2Synth *sf2_synth);

AgsSF2Synth* ags_sf2_synth_new(GObject *soundcard);

G_END_DECLS

#endif /*__AGS_SF2_SYNTH_H__*/
