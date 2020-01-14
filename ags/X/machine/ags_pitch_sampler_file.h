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

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_PITCH_SAMPLER_FILE                (ags_pitch_sampler_file_get_type())
#define AGS_PITCH_SAMPLER_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PITCH_SAMPLER_FILE, AgsPitchSamplerFile))
#define AGS_PITCH_SAMPLER_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PITCH_SAMPLER_FILE, AgsPitchSamplerFileClass))
#define AGS_IS_PITCH_SAMPLER_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PITCH_SAMPLER_FILE))
#define AGS_IS_PITCH_SAMPLER_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PITCH_SAMPLER_FILE))
#define AGS_PITCH_SAMPLER_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_PITCH_SAMPLER_FILE, AgsPitchSamplerFileClass))

typedef struct _AgsPitchSamplerFile AgsPitchSamplerFile;
typedef struct _AgsPitchSamplerFileClass AgsPitchSamplerFileClass;

typedef enum{
  AGS_PITCH_SAMPLER_FILE_CONNECTED   = 1,
}AgsPitchSamplerFileFlags;

struct _AgsPitchSamplerFile
{
  GtkVBox vbox;

  guint flags;
  
  GtkEntry *filename;

  GtkLabel *freq;
  GtkLabel *base_key;

  GtkLabel *loop_start;
  GtkLabel *loop_end;
};

struct _AgsPitchSamplerFileClass
{
  GtkVBoxClass vbox;

  void (*control_changed)(AgsPitchSamplerFile *pitch_sampler_file);
};

GType ags_pitch_sampler_file_get_type(void);

AgsPitchSamplerFile* ags_pitch_sampler_file_new();

G_END_DECLS

#endif /*__AGS_PITCH_SAMPLER_FILE_H__*/
