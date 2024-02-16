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

#ifndef __AGS_FILE_DIALOG_H__
#define __AGS_FILE_DIALOG_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_FILE_DIALOG                (ags_file_dialog_get_type())
#define AGS_FILE_DIALOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FILE_DIALOG, AgsFileDialog))
#define AGS_FILE_DIALOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_FILE_DIALOG, AgsFileDialogClass))
#define AGS_IS_FILE_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FILE_DIALOG))
#define AGS_IS_FILE_DIALOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FILE_DIALOG))
#define AGS_FILE_DIALOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_FILE_DIALOG, AgsFileDialogClass))

typedef struct _AgsFileDialog AgsFileDialog;
typedef struct _AgsFileDialogClass AgsFileDialogClass;

typedef enum{
  AGS_FILE_DIALOG_APP_SANDBOX            = 1,
  AGS_FILE_DIALOG_WITH_MULTI_SELECTION   = 1 <<  1,
  AGS_FILE_DIALOG_WITH_PREVIEW           = 1 <<  2,
}AgsFileDialogFlags;

typedef enum{
  AGS_FILE_DIALOG_OPEN,
  AGS_FILE_DIALOG_SAVE_AS,
}AgsFileDialogFileAction;

struct _AgsFileDialog
{
  GtkWindow window;

  guint flags;
  guint file_action;

  gchar *home_path;
  gchar *sandbox_path;

  gchar *default_path;
  
  GtkBox *vbox;

  GtkEntry *location_entry;

  GtkDropDown *location_drop_down;
  
  GtkBox *left_vbox;
  
  GtkBox *location_box;
  
  GtkButton *recently_used;

  GHashTable *location;
  
  GtkButton *start_here;
  
  GtkSeparator *location_separator;
  
  GHashTable *bookmark;
  
  GtkBox *center_vbox;

  GtkSingleSelection *filename_single_selection;
  GtkMultiSelection *filename_multi_selection;
  
  GtkListView *filename_view;

  GtkBox *right_vbox;

  GtkWidget *preview;
  
  GtkButton *activate_button;
};

struct _AgsFileDialogClass
{
  GtkWindowClass window;

  void (*create_dir)(AgsFileDialog *file_dialog,
		     gchar *dir_path);
};

GType ags_file_dialog_get_type();

gboolean ags_file_dialog_test_flags(AgsFileDialog *file_dialog,
				    guint flags);
void ags_file_dialog_set_flags(AgsFileDialog *file_dialog,
			       guint flags);
void ags_file_dialog_unset_flags(AgsFileDialog *file_dialog,
				 guint flags);

gboolean ags_file_dialog_test_file_action(AgsFileDialog *file_dialog,
					  guint file_action);
void ags_file_dialog_set_file_action(AgsFileDialog *file_dialog,
				     guint file_action);
void ags_file_dialog_unset_file_action(AgsFileDialog *file_dialog,
				       guint file_action);

void ags_file_dialog_add_location(AgsFileDialog *file_dialog,
				  gchar *button_action,
				  gchar *button_text);
void ags_file_dialog_remove_location(AgsFileDialog *file_dialog,
				     gchar *button_action);

void ags_file_dialog_add_bookmark(AgsFileDialog *file_dialog,
				  gchar *button_action,
				  gchar *button_text);
void ags_file_dialog_remove_bookmark(AgsFileDialog *file_dialog,
				     gchar *button_action);

AgsFileDialog* ags_file_dialog_new(GtkWidget *transient_for,
				   gchar *title);

G_END_DECLS

#endif /*__AGS_FILE_DIALOG_H__*/
