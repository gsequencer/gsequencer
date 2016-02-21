/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_FILE_SELECTION_H__
#define __AGS_FILE_SELECTION_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_audio.h>

#include <ags/audio/file/ags_audio_file.h>

#define AGS_TYPE_FILE_SELECTION                (ags_file_selection_get_type())
#define AGS_FILE_SELECTION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FILE_SELECTION, AgsFileSelection))
#define AGS_FILE_SELECTION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FILE_SELECTION, AgsFileSelectionClass))
#define AGS_IS_FILE_SELECTION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_FILE_SELECTION))
#define AGS_IS_FILE_SELECTION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_FILE_SELECTION))
#define AGS_FILE_SELECTION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_FILE_SELECTION, AgsFileSelectionClass))

#define AGS_FILE_SELECTION_ENTRY(strct)        ((AgsFileSelectionEntry *) (strct))

typedef struct _AgsFileSelection AgsFileSelection;
typedef struct _AgsFileSelectionClass AgsFileSelectionClass;
typedef struct _AgsFileSelectionEntry AgsFileSelectionEntry;

typedef enum{
  AGS_FILE_SELECTION_COMPLETED   = 1,
}AgsFileSelectionFlags;

struct _AgsFileSelection
{
  GtkVBox vbox;
  
  guint flags;

  gchar *directory;

  guint entry_count;
  GList *entry;

  GtkLabel *chosed;
  GtkLabel *selected;
};

struct _AgsFileSelectionClass
{
  GtkVBoxClass vbox;

  void (*add_entry)(AgsFileSelection *file_selection, GtkWidget *widget);
  void (*remove_entry)(AgsFileSelection *file_selection, GtkWidget *widget);

  void (*completed)(AgsFileSelection *file_selection);
};

struct _AgsFileSelectionEntry
{
  GtkTable *table;

  GtkWidget *entry;

  gboolean chosed;
  gchar *filename;

  GtkSpinButton *nth_sample;
  GtkSpinButton *nth_channel;
  GtkSpinButton *count;
};

GType ags_file_selection_get_type(void);

AgsFileSelectionEntry* ags_file_selection_entry_alloc();

void ags_file_selection_set_entry(AgsFileSelection *file_selection, GList *entry);

void ags_file_selection_add_entry(AgsFileSelection *file_selection, GtkWidget *widget);
void ags_file_selection_remove_entry(AgsFileSelection *file_selection, GtkWidget *widget);

gboolean ags_file_selection_contains_file(AgsFileSelection *file_selection, gchar *filename);

void ags_file_selection_completed(AgsFileSelection *file_selection);

AgsFileSelection* ags_file_selection_new();

#endif /*__AGS_FILE_SELECTION_H__*/
