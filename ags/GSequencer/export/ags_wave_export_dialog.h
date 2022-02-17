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

#ifndef __AGS_WAVE_EXPORT_DIALOG_H__
#define __AGS_WAVE_EXPORT_DIALOG_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/GSequencer/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_WAVE_EXPORT_DIALOG                (ags_wave_export_dialog_get_type())
#define AGS_WAVE_EXPORT_DIALOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WAVE_EXPORT_DIALOG, AgsWaveExportDialog))
#define AGS_WAVE_EXPORT_DIALOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WAVE_EXPORT_DIALOG, AgsWaveExportDialogClass))
#define AGS_IS_WAVE_EXPORT_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_WAVE_EXPORT_DIALOG))
#define AGS_IS_WAVE_EXPORT_DIALOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_WAVE_EXPORT_DIALOG))
#define AGS_WAVE_EXPORT_DIALOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_WAVE_EXPORT_DIALOG, AgsWaveExportDialogClass))
  
#define AGS_WAVE_EXPORT_DIALOG_FORMAT_WAV "wav"
#define AGS_WAVE_EXPORT_DIALOG_FORMAT_FLAC "flac"
#define AGS_WAVE_EXPORT_DIALOG_FORMAT_AIFF "aiff"
#define AGS_WAVE_EXPORT_DIALOG_FORMAT_OGG "ogg"
#define AGS_WAVE_EXPORT_DIALOG_FORMAT_OGG "ogg"
#define AGS_WAVE_EXPORT_DIALOG_FORMAT_MP3 "mp3"
#define AGS_WAVE_EXPORT_DIALOG_FORMAT_AAC "aac"
#define AGS_WAVE_EXPORT_DIALOG_FORMAT_MP4 "mp4"
#define AGS_WAVE_EXPORT_DIALOG_FORMAT_MKV "mkv"
#define AGS_WAVE_EXPORT_DIALOG_FORMAT_WEBM "webm"
#define AGS_WAVE_EXPORT_DIALOG_FORMAT_MPEG "mpg"

typedef struct _AgsWaveExportDialog AgsWaveExportDialog;
typedef struct _AgsWaveExportDialogClass AgsWaveExportDialogClass;

typedef enum{
  AGS_WAVE_EXPORT_DIALOG_CONNECTED     = 1,
}AgsWaveExportDialogFlags;

struct _AgsWaveExportDialog
{
  GtkDialog dialog;

  guint flags;

  AgsMachine *machine;

  GtkEntry *filename;
  GtkButton *file_chooser_button;

  GtkSpinButton *start_tact;
  GtkSpinButton *end_tact;

  GtkLabel *duration;

  GtkComboBoxText *output_format;

  GtkButton *apply;
  GtkButton *ok;
  GtkButton *cancel;
};

struct _AgsWaveExportDialogClass
{
  GtkDialogClass dialog;
};

GType ags_wave_export_dialog_get_type(void);

void ags_wave_export_dialog_update_duration(AgsWaveExportDialog *wave_export_dialog);

AgsWaveExportDialog* ags_wave_export_dialog_new(AgsMachine *machine);

G_END_DECLS

#endif /*__AGS_WAVE_EXPORT_DIALOG_H__*/
