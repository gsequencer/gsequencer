/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_PCM_FILE_DIALOG_H__
#define __AGS_PCM_FILE_DIALOG_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_PCM_FILE_DIALOG                (ags_pcm_file_dialog_get_type())
#define AGS_PCM_FILE_DIALOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PCM_FILE_DIALOG, AgsPCMFileDialog))
#define AGS_PCM_FILE_DIALOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PCM_FILE_DIALOG, AgsPCMFileDialogClass))
#define AGS_IS_PCM_FILE_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PCM_FILE_DIALOG))
#define AGS_IS_PCM_FILE_DIALOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PCM_FILE_DIALOG))
#define AGS_PCM_FILE_DIALOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PCM_FILE_DIALOG, AgsPCMFileDialogClass))

typedef struct _AgsPCMFileDialog AgsPCMFileDialog;
typedef struct _AgsPCMFileDialogClass AgsPCMFileDialogClass;

typedef enum{
  AGS_PCM_FILE_DIALOG_SHOW_AUDIO_CHANNEL        = 1,
  AGS_PCM_FILE_DIALOG_SHOW_NEW_CHANNEL          = 1 <<  1,
  AGS_PCM_FILE_DIALOG_SHOW_EXISTING_CHANNEL     = 1 <<  2,
}AgsPCMFileDialogFlags;

struct _AgsPCMFileDialog
{
  GtkWindow window;

  guint flags;
  
  AgsFileWidget *file_widget;

  GtkLabel *audio_channel_label;
  GtkSpinButton *audio_channel;

  GtkCheckButton *new_channel;
  GtkCheckButton *existing_channel;

  GtkButton *open;
};

struct _AgsPCMFileDialogClass
{
  GtkWindowClass window;

  void (*response)(AgsPCMFileDialog *pcm_file_dialog,
		   gint response_id);
};

GType ags_pcm_file_dialog_get_type(void);

gboolean ags_pcm_file_dialog_test_flags(AgsPCMFileDialog *pcm_file_dialog,
					guint flags);
void ags_pcm_file_dialog_set_flags(AgsPCMFileDialog *pcm_file_dialog,
				   guint flags);
void ags_pcm_file_dialog_unset_flags(AgsPCMFileDialog *pcm_file_dialog,
				     guint flags);

AgsFileWidget* ags_pcm_file_dialog_get_file_widget(AgsPCMFileDialog *pcm_file_dialog);

GtkSpinButton* ags_pcm_file_dialog_get_audio_channel(AgsPCMFileDialog *pcm_file_dialog);

GtkCheckButton* ags_pcm_file_dialog_get_new_channel(AgsPCMFileDialog *pcm_file_dialog);
GtkCheckButton* ags_pcm_file_dialog_get_existing_channel(AgsPCMFileDialog *pcm_file_dialog);

void ags_pcm_file_dialog_response(AgsPCMFileDialog *pcm_file_dialog,
				  gint response_id);

AgsPCMFileDialog* ags_pcm_file_dialog_new(gchar *title,
					  GtkWindow *transient_for);

G_END_DECLS

#endif /*__AGS_PCM_FILE_DIALOG_H__*/
