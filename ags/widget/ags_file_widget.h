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

#ifndef __AGS_FILE_WIDGET_H__
#define __AGS_FILE_WIDGET_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/widget/ags_icon_link.h>

G_BEGIN_DECLS

#define AGS_TYPE_FILE_WIDGET                (ags_file_widget_get_type())
#define AGS_FILE_WIDGET(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FILE_WIDGET, AgsFileWidget))
#define AGS_FILE_WIDGET_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_FILE_WIDGET, AgsFileWidgetClass))
#define AGS_IS_FILE_WIDGET(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FILE_WIDGET))
#define AGS_IS_FILE_WIDGET_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FILE_WIDGET))
#define AGS_FILE_WIDGET_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_FILE_WIDGET, AgsFileWidgetClass))

#define AGS_FILE_WIDGET_LOCATION_OPEN_RECENT "document-open-recent"
#define AGS_FILE_WIDGET_LOCATION_OPEN_START_HERE "open-start-here"
#define AGS_FILE_WIDGET_LOCATION_OPEN_USER_HOME "open-user-home"
#define AGS_FILE_WIDGET_LOCATION_OPEN_USER_DESKTOP "open-user-desktop"
#define AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_PICTURES "open-folder-pictures"
#define AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_MUSIC "open-folder-music"
#define AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_VIDEOS "open-folder-videos"
#define AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOWNLOADS "open-folder-downloads"
#define AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOCUMENTS "open-folder-documents"
#define AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_APP_HOME "open-app-home"
#define AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_APP_GENERIC "open-app-generic"

typedef struct _AgsFileWidget AgsFileWidget;
typedef struct _AgsFileWidgetClass AgsFileWidgetClass;

typedef enum{
  AGS_FILE_WIDGET_APP_SANDBOX            = 1,
  AGS_FILE_WIDGET_WITH_MULTI_SELECTION   = 1 <<  1,
  AGS_FILE_WIDGET_WITH_PREVIEW           = 1 <<  2,
  AGS_FILE_WIDGET_HIDDEN_FILES_VISIBLE   = 1 <<  3,
}AgsFileWidgetFlags;

typedef enum{
  AGS_FILE_WIDGET_OPEN,
  AGS_FILE_WIDGET_SAVE_AS,
}AgsFileWidgetFileAction;

struct _AgsFileWidget
{
  GtkBox box;

  guint flags;
  guint file_action;

  gchar *default_bundle;
  
  gchar *home_path;
  gchar *sandbox_path;

  gchar *app_home_path;
  gchar *app_generic_path;
  
  gchar *default_path;

  gchar *current_path;
  
  GSimpleActionGroup *action_group;

  GtkBox *vbox;

  GtkEntry *location_entry;

  GtkMenuButton *action_menu_button;
  GtkPopoverMenu *action_popover;
  GMenu *action_popup;
  
  GtkDropDown *location_drop_down;
  
  GtkBox *left_vbox;
  
  GtkBox *location_box;

  GHashTable *location;  
  
  AgsIconLink *recently_used;
  
  GtkSeparator *location_separator;

  gchar *bookmark_filename;
  
  GHashTable *bookmark;
  
  GtkBox *bookmark_box;

  GtkBox *center_vbox;

  GtkNoSelection *filename_key_selection;
  GtkListItemFactory *filename_factory[4];
  
  GtkSingleSelection *filename_single_selection;
  GtkMultiSelection *filename_multi_selection;

  GtkScrolledWindow *filename_scrolled_window;
  
  GtkColumnView *filename_view;

  GtkBox *right_vbox;

  GtkWidget *preview;
};

struct _AgsFileWidgetClass
{
  GtkBoxClass box;

  void (*refresh)(AgsFileWidget *file_widget);

  void (*create_dir)(AgsFileWidget *file_widget,
		     gchar *dir_path);
};

GType ags_file_widget_get_type();

/* flags */
gboolean ags_file_widget_test_flags(AgsFileWidget *file_widget,
				    guint flags);
void ags_file_widget_set_flags(AgsFileWidget *file_widget,
			       guint flags);
void ags_file_widget_unset_flags(AgsFileWidget *file_widget,
				 guint flags);

/* action */
gboolean ags_file_widget_test_file_action(AgsFileWidget *file_widget,
					  guint file_action);
void ags_file_widget_set_file_action(AgsFileWidget *file_widget,
				     guint file_action);

/* location */
GHashTable* ags_file_widget_get_location(AgsFileWidget *file_widget);

void ags_file_widget_add_location(AgsFileWidget *file_widget,
				  gchar *button_action,
				  gchar *button_text);
void ags_file_widget_remove_location(AgsFileWidget *file_widget,
				     gchar *button_action);

/* bookmarks */
GHashTable* ags_file_widget_get_bookmark(AgsFileWidget *file_widget);

void ags_file_widget_add_bookmark(AgsFileWidget *file_widget,
				  gchar *bookmark_location);
void ags_file_widget_remove_bookmark(AgsFileWidget *file_widget,
				     gchar *bookmark_location);

void ags_file_widget_read_bookmark(AgsFileWidget *file_widget);
void ags_file_widget_write_bookmark(AgsFileWidget *file_widget);

/* methods */
gchar* ags_file_widget_get_filename(AgsFileWidget *file_widget);

GSList* ags_file_widget_get_filenames(AgsFileWidget *file_widget);

/* events */
void ags_file_widget_refresh(AgsFileWidget *file_widget);

void ags_file_widget_create_dir(AgsFileWidget *file_widget,
				gchar *dir_path);

/* instantiate */
AgsFileWidget* ags_file_widget_new();

G_END_DECLS

#endif /*__AGS_FILE_WIDGET_H__*/
