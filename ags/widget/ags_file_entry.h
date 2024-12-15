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

#ifndef __AGS_FILE_ENTRY_H__
#define __AGS_FILE_ENTRY_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_FILE_ENTRY                (ags_file_entry_get_type())
#define AGS_FILE_ENTRY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FILE_ENTRY, AgsFileEntry))
#define AGS_FILE_ENTRY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_FILE_ENTRY, AgsFileEntryClass))
#define AGS_IS_FILE_ENTRY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FILE_ENTRY))
#define AGS_IS_FILE_ENTRY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FILE_ENTRY))
#define AGS_FILE_ENTRY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_FILE_ENTRY, AgsFileEntryClass))

#define AGS_FILE_ENTRY_DEFAULT_MAX_FILENAME_LENGTH (8192)

typedef struct _AgsFileEntry AgsFileEntry;
typedef struct _AgsFileEntryClass AgsFileEntryClass;

typedef enum{
  AGS_FILE_ENTRY_AUTO_COMPLETION            = 1,
  AGS_FILE_ENTRY_SELECTION                  = 1 <<  1,
  AGS_FILE_ENTRY_EDIT_DROPDOWN              = 1 <<  2,
}AgsFileEntryFlags;

typedef enum{
  AGS_FILE_ENTRY_KEY_L_CONTROL       = 1,
  AGS_FILE_ENTRY_KEY_R_CONTROL       = 1 <<  1,
  AGS_FILE_ENTRY_KEY_L_SHIFT         = 1 <<  2,
  AGS_FILE_ENTRY_KEY_R_SHIFT         = 1 <<  3,
  AGS_FILE_ENTRY_KEY_L_META          = 1 <<  4,
  AGS_FILE_ENTRY_KEY_R_META          = 1 <<  5,
}AgsFileEntryKeyMask;

struct _AgsFileEntry
{
  GtkBox box;

  AgsFileEntryFlags flags;
  AgsFileEntryKeyMask key_mask; 

  guint current_keyval;
  gint64 keyval_timestamp;

  gchar *im_module;
  GtkIMContext *im_context;
  
  gint cursor_position;
  
  guint max_filename_length;
  
  gchar *filename;
  gchar *auto_completion_filename;

  guint font_size;
    
  gdouble xalign;  

  gchar *selection_offset_x0;
  gchar *selection_offset_x1;

  gdouble click_x;
  gdouble click_y;
  
  gint text_width;
  gint text_height;
  
  gint clip_x0;
  gint clip_x1;
  
  GtkDrawingArea *drawing_area;

  GtkDropDown *edit_drop_down;
};

struct _AgsFileEntryClass
{
  GtkBoxClass box;

  void (*activate)(AgsFileEntry *file_entry);
};

GType ags_file_entry_get_type();

/* flags */
gboolean ags_file_entry_test_flags(AgsFileEntry *file_entry,
				    guint flags);
void ags_file_entry_set_flags(AgsFileEntry *file_entry,
			       guint flags);
void ags_file_entry_unset_flags(AgsFileEntry *file_entry,
				 guint flags);

gchar* ags_file_entry_get_filename(AgsFileEntry *file_entry);
void ags_file_entry_set_filename(AgsFileEntry *file_entry,
				 gchar *filename);

/* instantiate */
AgsFileEntry* ags_file_entry_new();

G_END_DECLS

#endif /*__AGS_FILE_ENTRY_H__*/
