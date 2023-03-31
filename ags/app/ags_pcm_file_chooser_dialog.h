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

#ifndef __AGS_PCM_FILE_CHOOSER_DIALOG_H__
#define __AGS_PCM_FILE_CHOOSER_DIALOG_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_PCM_FILE_CHOOSER_DIALOG                (ags_pcm_file_chooser_dialog_get_type())
#define AGS_PCM_FILE_CHOOSER_DIALOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PCM_FILE_CHOOSER_DIALOG, AgsPCMFileChooserDialog))
#define AGS_PCM_FILE_CHOOSER_DIALOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PCM_FILE_CHOOSER_DIALOG, AgsPCMFileChooserDialogClass))
#define AGS_IS_PCM_FILE_CHOOSER_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PCM_FILE_CHOOSER_DIALOG))
#define AGS_IS_PCM_FILE_CHOOSER_DIALOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PCM_FILE_CHOOSER_DIALOG))
#define AGS_PCM_FILE_CHOOSER_DIALOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PCM_FILE_CHOOSER_DIALOG, AgsPCMFileChooserDialogClass))

typedef struct _AgsPCMFileChooserDialog AgsPCMFileChooserDialog;
typedef struct _AgsPCMFileChooserDialogClass AgsPCMFileChooserDialogClass;

typedef enum{
  AGS_PCM_FILE_CHOOSER_DIALOG_SHOW_AUDIO_CHANNEL        = 1,
  AGS_PCM_FILE_CHOOSER_DIALOG_SHOW_NEW_CHANNEL          = 1 <<  1,
  AGS_PCM_FILE_CHOOSER_DIALOG_SHOW_EXISTING_CHANNEL     = 1 <<  2,
}AgsPCMFileChooserDialogFlags;

struct _AgsPCMFileChooserDialog
{
  GtkDialog dialog;

  guint flags;
  
  GtkFileChooserWidget *file_chooser;

  GtkLabel *audio_channel_label;
  GtkSpinButton *audio_channel;

  GtkCheckButton *new_channel;
  GtkCheckButton *existing_channel;
};

struct _AgsPCMFileChooserDialogClass
{
  GtkDialogClass dialog;
};

GType ags_pcm_file_chooser_dialog_get_type(void);

AgsPCMFileChooserDialog* ags_pcm_file_chooser_dialog_new(gchar *title,
							 GtkWindow *transient_for);

G_END_DECLS

#endif /*__AGS_PCM_FILE_CHOOSER_DIALOG_H__*/
