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

#ifndef __AGS_PITCH_SAMPLER_H__
#define __AGS_PITCH_SAMPLER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>

#include <ags/X/machine/ags_pitch_sampler_file.h>

#define AGS_TYPE_PITCH_SAMPLER                (ags_pitch_sampler_get_type())
#define AGS_PITCH_SAMPLER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PITCH_SAMPLER, AgsPitchSampler))
#define AGS_PITCH_SAMPLER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PITCH_SAMPLER, AgsPitchSamplerClass))
#define AGS_IS_PITCH_SAMPLER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PITCH_SAMPLER))
#define AGS_IS_PITCH_SAMPLER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PITCH_SAMPLER))
#define AGS_PITCH_SAMPLER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_PITCH_SAMPLER, AgsPitchSamplerClass))

#define AGS_PITCH_SAMPLER_LFO_FREQ_MIN (4.0)
#define AGS_PITCH_SAMPLER_LFO_FREQ_MAX (8.0)
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

  AgsAudioContainer *audio_container;

  GtkVBox *file;

  GtkEntry *filename;
  GtkButton *open;

  AgsSFZLoader *sfz_loader;

  gint position;
  GtkLabel *loading;

  GtkButton *add;
  GtkButton *remove;

  GtkCheckButton *auto_update;
  GtkButton *update;

  GtkCheckButton *enable_lfo;
    
  GtkSpinButton *lfo_freq;
  GtkSpinButton *lfo_phase;

  GtkSpinButton *lfo_depth;
  GtkSpinButton *lfo_tuning;

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

#endif /*__AGS_PITCH_SAMPLER_H__*/
