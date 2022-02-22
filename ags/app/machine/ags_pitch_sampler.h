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

#ifndef __AGS_PITCH_SAMPLER_H__
#define __AGS_PITCH_SAMPLER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

#include <ags/app/machine/ags_pitch_sampler_file.h>

G_BEGIN_DECLS

#define AGS_TYPE_PITCH_SAMPLER                (ags_pitch_sampler_get_type())
#define AGS_PITCH_SAMPLER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PITCH_SAMPLER, AgsPitchSampler))
#define AGS_PITCH_SAMPLER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PITCH_SAMPLER, AgsPitchSamplerClass))
#define AGS_IS_PITCH_SAMPLER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PITCH_SAMPLER))
#define AGS_IS_PITCH_SAMPLER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PITCH_SAMPLER))
#define AGS_PITCH_SAMPLER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_PITCH_SAMPLER, AgsPitchSamplerClass))

#define AGS_PITCH_SAMPLER_LFO_FREQ_MIN (0.0)
#define AGS_PITCH_SAMPLER_LFO_FREQ_MAX (20.0)
#define AGS_PITCH_SAMPLER_DEFAULT_LFO_FREQ (6.0)

typedef struct _AgsPitchSampler AgsPitchSampler;
typedef struct _AgsPitchSamplerClass AgsPitchSamplerClass;

typedef enum{
  AGS_PITCH_SAMPLER_AUTO_UPDATE          = 1,
}AgsPitch_SamplerFlags;

struct _AgsPitchSampler
{
  AgsMachine machine;

  guint flags;

  guint mapped_output_pad;
  guint mapped_input_pad;

  gchar *name;
  gchar *xml_type;

  AgsRecallContainer *playback_play_container;
  AgsRecallContainer *playback_recall_container;

  AgsRecallContainer *notation_play_container;
  AgsRecallContainer *notation_recall_container;

  AgsRecallContainer *lfo_play_container;
  AgsRecallContainer *lfo_recall_container;

  AgsRecallContainer *two_pass_aliase_play_container;
  AgsRecallContainer *two_pass_aliase_recall_container;

  AgsRecallContainer *volume_play_container;
  AgsRecallContainer *volume_recall_container;

  AgsRecallContainer *envelope_play_container;
  AgsRecallContainer *envelope_recall_container;
  
  AgsRecallContainer *buffer_play_container;
  AgsRecallContainer *buffer_recall_container;

  AgsAudioContainer *audio_container;

  GtkBox *file;

  GtkEntry *filename;
  GtkButton *open;

  AgsSFZLoader *sfz_loader;

  gint position;
  GtkSpinner *sfz_loader_spinner;  

  GtkCheckButton *enable_synth_generator;

  GtkComboBox *pitch_function;

  GtkSpinButton *lower;
  GtkSpinButton *key_count;

  GtkButton *update;

  GtkCheckButton *enable_aliase;

  AgsDial *aliase_a_amount;
  AgsDial *aliase_a_phase;

  AgsDial *aliase_b_amount;
  AgsDial *aliase_b_phase;

  GtkCheckButton *enable_lfo;
    
  GtkSpinButton *lfo_freq;
  GtkSpinButton *lfo_phase;

  GtkSpinButton *lfo_depth;
  GtkSpinButton *lfo_tuning;

  GtkScale *volume;

  GtkFileChooserDialog *open_dialog;
};

struct _AgsPitchSamplerClass
{
  AgsMachineClass machine;
};

GType ags_pitch_sampler_get_type(void);

void ags_pitch_sampler_add_file(AgsPitchSampler *pitch_sampler,
				AgsPitchSamplerFile *file);
void ags_pitch_sampler_remove_file(AgsPitchSampler *pitch_sampler,
				   guint nth);

void ags_pitch_sampler_open_filename(AgsPitchSampler *pitch_sampler,
				     gchar *filename);

void ags_pitch_sampler_update(AgsPitchSampler *pitch_sampler);

gboolean ags_pitch_sampler_sfz_loader_completed_timeout(AgsPitchSampler *pitch_sampler);

AgsPitchSampler* ags_pitch_sampler_new(GObject *soundcard);

G_END_DECLS

#endif /*__AGS_PITCH_SAMPLER_H__*/
