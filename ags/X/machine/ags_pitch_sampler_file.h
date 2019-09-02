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

#ifndef __AGS_PITCH_SAMPLER_FILE_H__
#define __AGS_PITCH_SAMPLER_FILE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>

#define AGS_TYPE_PITCH_SAMPLER_FILE                (ags_pitch_sampler_file_get_type())
#define AGS_PITCH_SAMPLER_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PITCH_SAMPLER_FILE, AgsPitchSamplerFile))
#define AGS_PITCH_SAMPLER_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PITCH_SAMPLER_FILE, AgsPitchSamplerFileClass))
#define AGS_IS_PITCH_SAMPLER_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PITCH_SAMPLER_FILE))
#define AGS_IS_PITCH_SAMPLER_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PITCH_SAMPLER_FILE))
#define AGS_PITCH_SAMPLER_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_PITCH_SAMPLER_FILE, AgsPitchSamplerFileClass))

typedef struct _AgsPitchSamplerFile AgsPitchSamplerFile;
typedef struct _AgsPitchSamplerFileClass AgsPitchSamplerFileClass;

struct _AgsPitchSamplerFile
{
  GtkVBox vbox;

  GtkEntry *filename;
  GtkButton *open;

  GtkLabel *freq;
  GtkSpinButton *base_key;

  GtkSpinButton *loop_start;
  GtkSpinButton *loop_end;
  
  GtkFileChooserDialog *open_dialog;
};

struct _AgsPitchSamplerFileClass
{
  GtkVBoxClass vbox;
};

GType ags_pitch_sampler_file_get_type(void);

void ags_pitch_sampler_file_open_filename(AgsPitchSamplerFile *pitch_sampler_file,
					  gchar *filename);

AgsPitchSamplerFile* ags_pitch_sampler_file_new();

#endif /*__AGS_PITCH_SAMPLER_FILE_H__*/
