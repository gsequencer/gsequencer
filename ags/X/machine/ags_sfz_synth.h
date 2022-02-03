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

#ifndef __AGS_SFZ_SYNTH_H__
#define __AGS_SFZ_SYNTH_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/X/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_SFZ_SYNTH                (ags_sfz_synth_get_type())
#define AGS_SFZ_SYNTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SFZ_SYNTH, AgsSFZSynth))
#define AGS_SFZ_SYNTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SFZ_SYNTH, AgsSFZSynthClass))
#define AGS_IS_SFZ_SYNTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SFZ_SYNTH))
#define AGS_IS_SFZ_SYNTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SFZ_SYNTH))
#define AGS_SFZ_SYNTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SFZ_SYNTH, AgsSFZSynthClass))

#define AGS_SFZ_SYNTH_SAMPLE_HEIGHT_REQUEST (256)
#define AGS_SFZ_SYNTH_SAMPLE_WIDTH_REQUEST (512)

typedef struct _AgsSFZSynth AgsSFZSynth;
typedef struct _AgsSFZSynthClass AgsSFZSynthClass;

struct _AgsSFZSynth
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

  AgsRecallContainer *sfz_synth_play_container;
  AgsRecallContainer *sfz_synth_recall_container;

  AgsRecallContainer *envelope_play_container;
  AgsRecallContainer *envelope_recall_container;
  
  AgsRecallContainer *buffer_play_container;
  AgsRecallContainer *buffer_recall_container;

  GtkEntry *filename;
  GtkButton *open;

  AgsSFZLoader *sfz_loader;
  AgsSFZInstrumentLoader *sfz_instrument_loader;

  gint position;
  GtkSpinner *sfz_loader_spinner;

  GtkTreeView *sample_tree_view;

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

struct _AgsSFZSynthClass
{
  AgsMachineClass machine;
};

GType ags_sfz_synth_get_type(void);

void ags_sfz_synth_open_filename(AgsSFZSynth *sfz_synth,
				 gchar *filename);

gboolean ags_sfz_synth_sfz_loader_completed_timeout(AgsSFZSynth *sfz_synth);

AgsSFZSynth* ags_sfz_synth_new(GObject *soundcard);

G_END_DECLS

#endif /*__AGS_SFZ_SYNTH_H__*/
