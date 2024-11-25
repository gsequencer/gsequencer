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

#include "ags_file_widget.h"

#include <ags/widget/ags_input_dialog.h>

#include <glib.h>
#include <glib/gstdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <gmodule.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <libxml/xlink.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/stat.h>
#include <time.h>

#if !defined(AGS_W32API)
#include <pwd.h>
#endif

#include <ags/i18n.h>

void ags_file_widget_class_init(AgsFileWidgetClass *file_widget);
void ags_file_widget_orientable_interface_init(GtkOrientableIface *orientable);
void ags_file_widget_init(AgsFileWidget *file_widget);
void ags_file_widget_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_file_widget_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_file_widget_dispose(GObject *gobject);
void ags_file_widget_finalize(GObject *gobject);

void ags_file_widget_real_refresh(AgsFileWidget *file_widget);
void ags_file_widget_real_create_dir(AgsFileWidget *file_widget,
				     gchar *dir_path);

void ags_file_widget_factory_setup(GtkListItemFactory *factory, GtkListItem *list_item,
				   AgsFileWidget *file_widget);
void ags_file_widget_primary_key_factory_bind(GtkListItemFactory *factory, GtkListItem *list_item,
					      AgsFileWidget *file_widget);
void ags_file_widget_value_factory_bind(GtkListItemFactory *factory, GtkListItem *list_item,
					AgsFileWidget *file_widget);

void ags_file_widget_location_entry_callback(GtkEntry *location_entry,
					     AgsFileWidget *file_widget);

void ags_file_widget_gesture_click_pressed_callback(GtkGestureClick *event_controller,
						    gint n_press,
						    gdouble x,
						    gdouble y,
						    AgsFileWidget *file_widget);
void ags_file_widget_gesture_click_released_callback(GtkGestureClick *event_controller,
						     gint n_press,
						     gdouble x,
						     gdouble y,
						     AgsFileWidget *file_widget);

void ags_file_widget_filename_activate_callback(GtkListView *list_view,
						guint position,
						AgsFileWidget *file_widget);

void ags_file_widget_location_drop_down_callback(GObject *location,
						 GParamSpec *pspec,
						 AgsFileWidget *file_widget);
void ags_file_widget_location_callback(AgsIconLink *icon_link,
				       AgsFileWidget *file_widget);
void ags_file_widget_bookmark_callback(AgsIconLink *icon_link,
				       AgsFileWidget *file_widget);

void ags_file_widget_rename_response_callback(AgsInputDialog *input_dialog,
					      gint response,
					      AgsFileWidget *file_widget);
void ags_file_widget_mkdir_response_callback(AgsInputDialog *input_dialog,
					     gint response,
					     AgsFileWidget *file_widget);

void ags_file_widget_rename_callback(GAction *action, GVariant *parameter,
				     AgsFileWidget *file_widget);
void ags_file_widget_mkdir_callback(GAction *action, GVariant *parameter,
				    AgsFileWidget *file_widget);
void ags_file_widget_show_hidden_files_callback(GAction *action, GVariant *parameter,
						AgsFileWidget *file_widget);
void ags_file_widget_add_bookmark_callback(GAction *action, GVariant *parameter,
					   AgsFileWidget *file_widget);

/**
 * SECTION:ags_file_widget
 * @short_description: a file widget widget
 * @title: AgsFileWidget
 * @section_id:
 * @include: ags/widget/ags_file_widget.h
 *
 * #AgsFileWidget is a widget to open or save files.
 */

enum{
  REFRESH,
  CREATE_DIR,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_DEFAULT_BUNDLE,
  PROP_HOME_PATH,
  PROP_APP_HOME_PATH,
  PROP_APP_GENERIC_PATH,
  PROP_DEFAULT_PATH,
  PROP_CURRENT_PATH,
  PROP_LOCATION,
  PROP_RECENTLY_USED_FILENAME,
  PROP_BOOKMARK,
  PROP_BOOKMARK_FILENAME,
  PROP_FILE_MAGIC_EXECUTABLE,
};

static gpointer ags_file_widget_parent_class = NULL;
static guint file_widget_signals[LAST_SIGNAL];

GType
ags_file_widget_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_file_widget = 0;

    static const GTypeInfo ags_file_widget_info = {
      sizeof(AgsFileWidgetClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_file_widget_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFileWidget),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_file_widget_init,
    };

    ags_type_file_widget = g_type_register_static(GTK_TYPE_BOX,
						  "AgsFileWidget", &ags_file_widget_info,
						  0);    

    g_once_init_leave(&g_define_type_id__static, ags_type_file_widget);
  }

  return g_define_type_id__static;
}

void
ags_file_widget_class_init(AgsFileWidgetClass *file_widget)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_file_widget_parent_class = g_type_class_peek_parent(file_widget);

  /* GObjectClass */
  gobject = (GObjectClass *) file_widget;

  gobject->set_property = ags_file_widget_set_property;
  gobject->get_property = ags_file_widget_get_property;

  gobject->dispose = ags_file_widget_dispose;
  gobject->finalize = ags_file_widget_finalize;

  /* properties */
  /**
   * AgsFileWidget:default-bundle:
   *
   * The default bundle.
   * 
   * Since: 6.6.0
   */
  param_spec = g_param_spec_string("default-bundle",
				   "default bundle",
				   "The default bundle",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEFAULT_BUNDLE,
				  param_spec);

  /**
   * AgsFileWidget:home-path:
   *
   * The home path.
   * 
   * Since: 6.6.0
   */
  param_spec = g_param_spec_string("home-path",
				   "home path",
				   "The home path",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_HOME_PATH,
				  param_spec);

  /**
   * AgsFileWidget:app-home-path:
   *
   * The app home path.
   * 
   * Since: 6.6.0
   */
  param_spec = g_param_spec_string("app-home-path",
				   "app home path",
				   "The app home path",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APP_HOME_PATH,
				  param_spec);

  /**
   * AgsFileWidget:app-generic-path:
   *
   * The app generic path.
   * 
   * Since: 6.6.0
   */
  param_spec = g_param_spec_string("app-generic-path",
				   "app generic path",
				   "The app generic path",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APP_GENERIC_PATH,
				  param_spec);

  /**
   * AgsFileWidget:default-path:
   *
   * The default path.
   * 
   * Since: 6.6.0
   */
  param_spec = g_param_spec_string("default-path",
				   "default path",
				   "The default path",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEFAULT_PATH,
				  param_spec);
  
  /**
   * AgsFileWidget:current-path:
   *
   * The current path.
   * 
   * Since: 6.6.0
   */
  param_spec = g_param_spec_string("current-path",
				   "current path",
				   "The current path",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CURRENT_PATH,
				  param_spec);

  /**
   * AgsFileWidget:recently-used-filename:
   *
   * The recently used filename.
   * 
   * Since: 6.6.0
   */
  param_spec = g_param_spec_string("recently-used-filename",
				   "recently used filename",
				   "The recently used filename",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECENTLY_USED_FILENAME,
				  param_spec);
  

  /**
   * AgsFileWidget:bookmark-filename:
   *
   * The recently used filename.
   * 
   * Since: 6.6.0
   */
  param_spec = g_param_spec_string("bookmark-filename",
				   "bookmark filename",
				   "The bookmark filename",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BOOKMARK_FILENAME,
				  param_spec);
  

  /**
   * AgsFileWidget:file-magic-executable:
   *
   * The file magic executable.
   * 
   * Since: 6.6.0
   */
  param_spec = g_param_spec_string("file-magic-executable",
				   "file magic executable",
				   "The file magic executable",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE_MAGIC_EXECUTABLE,
				  param_spec);

  /* AgsFileWidget */
  file_widget->refresh = ags_file_widget_real_refresh;

  file_widget->create_dir = ags_file_widget_real_create_dir;
  
  /* signals */
  /**
   * AgsFileWidget::refresh:
   * @file_widget: the #AgsFileWidget
   *
   * The ::refresh signal notifies about filesystem change.
   *
   * Since: 6.6.0
   */
  file_widget_signals[REFRESH] =
    g_signal_new("refresh",
		 G_TYPE_FROM_CLASS(file_widget),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileWidgetClass, refresh),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsFileWidget::create-dir:
   * @file_widget: the #AgsFileWidget
   * @dir_path: the directory path
   *
   * The ::create-dir signal notifies about creating directory.
   *
   * Since: 6.6.0
   */
  file_widget_signals[CREATE_DIR] =
    g_signal_new("create-dir",
		 G_TYPE_FROM_CLASS(file_widget),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileWidgetClass, create_dir),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__STRING,
		 G_TYPE_NONE, 1,
		 G_TYPE_STRING);
}

void
ags_file_widget_init(AgsFileWidget *file_widget)
{
  GtkBox *hbox;
  GtkBox *action_hbox;
  GtkLabel *label;
  
  GtkColumnViewColumn *column;  

  GtkEventController *event_controller;
  
  GtkStringList *location_string_list;
  GtkStringList *filename_key_string_list;
  GtkStringList *single_filename_string_list;
  GtkStringList *multi_filename_string_list;

  GSimpleActionGroup *action_group;
  GSimpleAction *action;
  GMenu *bookmark_menu;
  GMenuItem *menu_item;
  
  gchar *str;
  
#if !defined(AGS_W32API)
  struct passwd *pw;

  uid_t uid;
#endif

  guint i;
  
  const char* location_strv[] = {
    "/",
    NULL,
  };

  const char* filename_keys_strv[] = {
    "file",
    "size",
    "type",
    "date modified",
    NULL,
  };

  const char* filename_strv[] = {
    ".",
    "..",
    NULL,
  };
  
  file_widget->flags = 0;
  file_widget->file_action = AGS_FILE_WIDGET_OPEN;
  file_widget->file_filter = AGS_FILE_WIDGET_FILTER_NONE;

  file_widget->default_bundle = NULL;
  
  file_widget->home_path = NULL;

#if !defined(AGS_W32API)
  uid = getuid();
  pw = getpwuid(uid);

  file_widget->home_path = g_strdup(pw->pw_dir);
#endif
  
  file_widget->sandbox_path = NULL;
  
  file_widget->app_home_path = NULL;  
  file_widget->app_generic_path = NULL;
  
  file_widget->default_path = NULL;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(file_widget),
				 GTK_ORIENTATION_VERTICAL);
  
  file_widget->vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
					     6);

  gtk_widget_set_valign((GtkWidget *) file_widget->vbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) file_widget->vbox,
			GTK_ALIGN_FILL);

  gtk_box_append((GtkBox *) file_widget,
		 (GtkWidget *) file_widget->vbox);  

  /* location */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				6);

  gtk_widget_set_halign((GtkWidget *) hbox,
			GTK_ALIGN_FILL);

  gtk_box_append(file_widget->vbox,
		 (GtkWidget *) hbox);

  label = (GtkLabel *) gtk_label_new(i18n("location"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_box_append(hbox,
		 (GtkWidget *) label);

  file_widget->location_entry = (GtkEntry *) gtk_entry_new();

  gtk_widget_set_halign((GtkWidget *) file_widget->location_entry,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand((GtkWidget *) file_widget->location_entry,
			 TRUE);  
  
  gtk_entry_set_alignment(file_widget->location_entry,
			  0.0);
  
  gtk_box_append(hbox,
		 (GtkWidget *) file_widget->location_entry);

  g_signal_connect_after(file_widget->location_entry, "activate",
  			 G_CALLBACK(ags_file_widget_location_entry_callback), file_widget);

  /* location - combo */
  location_string_list = gtk_string_list_new((const gchar * const *) location_strv);
  
  file_widget->location_drop_down = (GtkDropDown *) gtk_drop_down_new(G_LIST_MODEL(location_string_list),
								      NULL);
  gtk_box_append(file_widget->vbox,
		 (GtkWidget *) file_widget->location_drop_down);

  g_signal_connect_after(file_widget->location_drop_down, "notify::selected-item",
			 G_CALLBACK(ags_file_widget_location_drop_down_callback), file_widget);

  /* action menu button */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				6);

  gtk_widget_set_halign((GtkWidget *) hbox,
			GTK_ALIGN_FILL);
  
  gtk_widget_set_hexpand((GtkWidget *) hbox,
			 TRUE);  

  gtk_box_append(file_widget->vbox,
		 (GtkWidget *) hbox);

  file_widget->action_menu_button = (GtkMenuButton *) gtk_menu_button_new();
  gtk_menu_button_set_icon_name(file_widget->action_menu_button,
				"system-run");

  gtk_widget_set_halign((GtkWidget *) file_widget->action_menu_button,
			GTK_ALIGN_CENTER);

  gtk_widget_set_hexpand((GtkWidget *) file_widget->action_menu_button,
			 TRUE);  

  gtk_box_append(hbox,
		 (GtkWidget *) file_widget->action_menu_button);
  
  file_widget->action_popup = (GMenu *) g_menu_new();

  menu_item = g_menu_item_new(i18n("rename file"),
			      "file_widget.rename");
  g_menu_append_item(file_widget->action_popup,
		     menu_item);

  menu_item = g_menu_item_new(i18n("create directory"),
			      "file_widget.mkdir");
  g_menu_append_item(file_widget->action_popup,
		     menu_item);

  menu_item = g_menu_item_new(i18n("show hidden files"),
			      "file_widget.show_hidden_files");
  g_menu_append_item(file_widget->action_popup,
		     menu_item);

  bookmark_menu = (GMenu *) g_menu_new();
  
  menu_item = g_menu_item_new_section(NULL,
				      G_MENU_MODEL(bookmark_menu));
  g_menu_append_item(file_widget->action_popup,
		     menu_item);

  menu_item = g_menu_item_new(i18n("add bookmark"),
			      "file_widget.add_bookmark");
  g_menu_append_item(bookmark_menu,
		     menu_item);
  
  file_widget->action_popover = (GtkPopoverMenu *) gtk_popover_menu_new_from_model(G_MENU_MODEL(file_widget->action_popup));
  gtk_menu_button_set_popover(file_widget->action_menu_button,
			      (GtkWidget *) file_widget->action_popover);
  
  action_group =
    file_widget->action_group = g_simple_action_group_new();
  gtk_widget_insert_action_group((GtkWidget *) file_widget->action_popover,
				 "file_widget",
				 G_ACTION_GROUP(action_group));

  /* left, center and right */
  action_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				       6);

  gtk_widget_set_valign((GtkWidget *) action_hbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) action_hbox,
			GTK_ALIGN_FILL);

  gtk_box_append(file_widget->vbox,
		 (GtkWidget *) action_hbox);

  /* left */
  file_widget->left_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						  6);
  gtk_box_append(action_hbox,
		 (GtkWidget *) file_widget->left_vbox);

  /* location */
  file_widget->location_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						     6);
  gtk_box_append(file_widget->left_vbox,
		 (GtkWidget *) file_widget->location_box);

  file_widget->location = g_hash_table_new_full((GHashFunc) g_str_hash,
						(GEqualFunc) g_str_equal,
						g_free,
						NULL);

  /* recently used */
  file_widget->recently_used_filename = NULL;

  file_widget->recently_used = (gchar **) g_malloc(AGS_FILE_WIDGET_MAX_RECENTLY_USED * sizeof(gchar *));

  for(i = 0; i < AGS_FILE_WIDGET_MAX_RECENTLY_USED; i++){
    file_widget->recently_used[i] = NULL;
  }
  
  str = g_strdup_printf("<span foreground=\"#0000ff\"><u>%s</u></span>",
			i18n("Recently-used"));
  
  file_widget->recently_used_link = ags_icon_link_new("document-open-recent",
						      AGS_FILE_WIDGET_LOCATION_OPEN_RECENT,
						      str);
  
  g_free(str);
  
  g_hash_table_insert(file_widget->location,
		      g_strdup(AGS_FILE_WIDGET_LOCATION_OPEN_RECENT),
		      file_widget->recently_used_link);
  
  gtk_box_append(file_widget->location_box,
		 (GtkWidget *) file_widget->recently_used_link);

  g_signal_connect(file_widget->recently_used_link, "clicked",
		   G_CALLBACK(ags_file_widget_location_callback), file_widget);

  /* separator */
  file_widget->location_separator = (GtkSeparator *) gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_append(file_widget->left_vbox,
		 (GtkWidget *) file_widget->location_separator);

  /* bookmark */
#if 0
  //NOTE:JK: this is gsequencer specific
  
  if(file_widget->home_path != NULL){
    file_widget->bookmark_filename = g_strdup_printf("%s/.gsequencer/default-bookmarks.xml",
						     file_widget->home_path);
  }else{
    file_widget->bookmark_filename = NULL;
  }
#else
    file_widget->bookmark_filename = NULL;
#endif
  
    file_widget->bookmark = g_hash_table_new_full((GHashFunc) g_direct_hash,
						(GEqualFunc) g_string_equal,
						g_free,
						g_free);

  file_widget->bookmark_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						     6);
  gtk_box_append(file_widget->left_vbox,
		 (GtkWidget *) file_widget->bookmark_box);

  /* center */
  file_widget->center_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						    6);

  gtk_widget_set_valign((GtkWidget *) file_widget->center_vbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) file_widget->center_vbox,
			GTK_ALIGN_FILL);

  gtk_box_append(action_hbox,
		 (GtkWidget *) file_widget->center_vbox);

  file_widget->file_filter_suffix = NULL;

  file_widget->current_file_filter = NULL;
 
  filename_key_string_list = gtk_string_list_new((const gchar * const *) filename_keys_strv);
  file_widget->filename_key_selection = (GtkNoSelection *) gtk_single_selection_new(G_LIST_MODEL(filename_key_string_list));

  for(i = 0; i < 4; i++){
    file_widget->filename_factory[i] = gtk_signal_list_item_factory_new();
  }

  g_signal_connect(file_widget->filename_factory[0], "setup",
		   G_CALLBACK(ags_file_widget_factory_setup), file_widget);

  g_signal_connect(file_widget->filename_factory[0], "bind",
		   G_CALLBACK(ags_file_widget_primary_key_factory_bind), file_widget);

  for(i = 1; i < 4; i++){
    g_signal_connect(file_widget->filename_factory[i], "setup",
		     G_CALLBACK(ags_file_widget_factory_setup), file_widget);
    
    g_signal_connect(file_widget->filename_factory[i], "bind",
		     G_CALLBACK(ags_file_widget_value_factory_bind), file_widget);
  }
  
  single_filename_string_list = gtk_string_list_new((const gchar * const *) filename_strv);
  file_widget->filename_single_selection = gtk_single_selection_new(G_LIST_MODEL(single_filename_string_list));

  multi_filename_string_list = gtk_string_list_new((const gchar * const *) filename_strv);
  file_widget->filename_multi_selection = gtk_multi_selection_new(G_LIST_MODEL(multi_filename_string_list));

  file_widget->filename_scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();

  gtk_widget_set_valign((GtkWidget *) file_widget->filename_scrolled_window,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) file_widget->filename_scrolled_window,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) file_widget->filename_scrolled_window,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) file_widget->filename_scrolled_window,
			 TRUE);
  
  gtk_box_append(file_widget->center_vbox,
		 (GtkWidget *) file_widget->filename_scrolled_window);

  /* filename view */
  file_widget->filename_view = (GtkColumnView *) gtk_column_view_new(GTK_SELECTION_MODEL(file_widget->filename_single_selection));
  gtk_scrolled_window_set_child(file_widget->filename_scrolled_window,
				(GtkWidget *) file_widget->filename_view);
  
  for(i = 0; i < 4; i++){
    column = gtk_column_view_column_new(filename_keys_strv[i],
					file_widget->filename_factory[i]);

    gtk_column_view_append_column(GTK_COLUMN_VIEW(file_widget->filename_view),
				  column);
  }

  g_signal_connect_after(file_widget->filename_view, "activate",
			 G_CALLBACK(ags_file_widget_filename_activate_callback), file_widget);

  event_controller = (GtkEventController *) gtk_gesture_click_new();
  gtk_gesture_single_set_button((GtkGestureSingle *) event_controller,
				GDK_BUTTON_SECONDARY);
  gtk_widget_add_controller((GtkWidget *) file_widget->filename_view,
			    event_controller);

  g_signal_connect_after(event_controller, "pressed",
			 G_CALLBACK(ags_file_widget_gesture_click_pressed_callback), file_widget);

  g_signal_connect_after(event_controller, "released",
			 G_CALLBACK(ags_file_widget_gesture_click_released_callback), file_widget);

  /* rename file */
  action = g_simple_action_new("rename",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_file_widget_rename_callback), file_widget);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* create directory */
  action = g_simple_action_new("mkdir",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_file_widget_mkdir_callback), file_widget);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* show hidden files */
  action = g_simple_action_new("show_hidden_files",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_file_widget_show_hidden_files_callback), file_widget);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  /* add bookmark */
  action = g_simple_action_new("add_bookmark",
			       NULL);
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_file_widget_add_bookmark_callback), file_widget);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
  
  /* right */
  file_widget->right_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						   6);
  gtk_widget_set_visible((GtkWidget *) file_widget->right_vbox,
			 FALSE);
  gtk_box_append(action_hbox,
		 (GtkWidget *) file_widget->right_vbox);

  /* preview */
  file_widget->file_magic_executable = NULL;
  
  file_widget->preview = NULL;

  /* file filter */
  file_widget->file_filter_drop_down = NULL;
}

void
ags_file_widget_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsFileWidget *file_widget;

  file_widget = AGS_FILE_WIDGET(gobject);

  switch(prop_id){
  case PROP_DEFAULT_BUNDLE:
    {
      gchar *default_bundle;

      default_bundle = g_value_get_string(value);

      if(file_widget->default_bundle == default_bundle){
	return;
      }

      g_free(file_widget->default_bundle);

      file_widget->default_bundle = g_strdup(default_bundle);
    }
    break;
  case PROP_HOME_PATH:
    {
      gchar *home_path;

      home_path = g_value_get_string(value);

      if(file_widget->home_path == home_path){
	return;
      }

      g_free(file_widget->home_path);

      file_widget->home_path = g_strdup(home_path);
    }
    break;
  case PROP_APP_HOME_PATH:
    {
      gchar *app_home_path;

      app_home_path = g_value_get_string(value);

      if(file_widget->app_home_path == app_home_path){
	return;
      }

      g_free(file_widget->app_home_path);

      file_widget->app_home_path = g_strdup(app_home_path);
    }
    break;
  case PROP_APP_GENERIC_PATH:
    {
      gchar *app_generic_path;

      app_generic_path = g_value_get_string(value);

      if(file_widget->app_generic_path == app_generic_path){
	return;
      }

      g_free(file_widget->app_generic_path);

      file_widget->app_generic_path = g_strdup(app_generic_path);
    }
    break;
  case PROP_DEFAULT_PATH:
    {
      gchar *default_path;

      default_path = g_value_get_string(value);

      if(file_widget->default_path == default_path){
	return;
      }

      g_free(file_widget->default_path);

      file_widget->default_path = g_strdup(default_path);
    }
    break;
  case PROP_CURRENT_PATH:
    {
      gchar *current_path;

      current_path = g_value_get_string(value);

      if(file_widget->current_path == current_path){
	return;
      }

      g_free(file_widget->current_path);

      file_widget->current_path = g_strdup(current_path);
    }
    break;
  case PROP_RECENTLY_USED_FILENAME:
    {
      gchar *recently_used_filename;

      recently_used_filename = g_value_get_string(value);

      if(file_widget->recently_used_filename == recently_used_filename){
	return;
      }

      g_free(file_widget->recently_used_filename);

      file_widget->recently_used_filename = g_strdup(recently_used_filename);
    }
    break;
  case PROP_BOOKMARK_FILENAME:
    {
      gchar *bookmark_filename;

      bookmark_filename = g_value_get_string(value);

      if(file_widget->bookmark_filename == bookmark_filename){
	return;
      }

      g_free(file_widget->bookmark_filename);

      file_widget->bookmark_filename = g_strdup(bookmark_filename);
    }
    break;
  case PROP_FILE_MAGIC_EXECUTABLE:
    {
      gchar *file_magic_executable;

      file_magic_executable = g_value_get_string(value);

      if(file_widget->file_magic_executable == file_magic_executable){
	return;
      }

      g_free(file_widget->file_magic_executable);

      file_widget->file_magic_executable = g_strdup(file_magic_executable);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_widget_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsFileWidget *file_widget;

  file_widget = AGS_FILE_WIDGET(gobject);

  switch(prop_id){
  case PROP_DEFAULT_BUNDLE:
  {
    g_value_set_string(value, file_widget->default_bundle);
  }
  break;
  case PROP_HOME_PATH:
  {
    g_value_set_string(value, file_widget->home_path);
  }
  break;
  case PROP_APP_HOME_PATH:
  {
    g_value_set_string(value, file_widget->app_home_path);
  }
  break;
  case PROP_APP_GENERIC_PATH:
  {
    g_value_set_string(value, file_widget->app_generic_path);
  }
  break;
  case PROP_DEFAULT_PATH:
  {
    g_value_set_string(value, file_widget->default_path);
  }
  break;
  case PROP_CURRENT_PATH:
  {
    g_value_set_string(value, file_widget->current_path);
  }
  break;
  case PROP_RECENTLY_USED_FILENAME:
  {
    g_value_set_string(value, file_widget->recently_used_filename);
  }
  break;
  case PROP_BOOKMARK_FILENAME:
  {
    g_value_set_string(value, file_widget->bookmark_filename);
  }
  break;
  case PROP_FILE_MAGIC_EXECUTABLE:
  {
    g_value_set_string(value, file_widget->file_magic_executable);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_widget_dispose(GObject *gobject)
{  
  AgsFileWidget *file_widget;
  
  file_widget = AGS_FILE_WIDGET(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_file_widget_parent_class)->dispose(gobject);
}

void
ags_file_widget_finalize(GObject *gobject)
{
  AgsFileWidget *file_widget;
  
  file_widget = AGS_FILE_WIDGET(gobject);

  g_free(file_widget->default_bundle);

  g_free(file_widget->home_path);
  g_free(file_widget->sandbox_path);

  g_free(file_widget->app_home_path);
  g_free(file_widget->app_generic_path);

  g_free(file_widget->default_path);
  
  g_free(file_widget->current_path);

  /* call parent */
  G_OBJECT_CLASS(ags_file_widget_parent_class)->finalize(gobject);
}


void
ags_file_widget_factory_setup(GtkListItemFactory *factory, GtkListItem *list_item,
			      AgsFileWidget *file_widget)
{
  GtkLabel *label;
  
  label = (GtkLabel *) gtk_label_new("");

  gtk_label_set_xalign(label,
		       0.0);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_list_item_set_child(list_item,
			  (GtkWidget *) label);
  gtk_list_item_set_activatable(list_item,
				TRUE);
}

void
ags_file_widget_primary_key_factory_bind(GtkListItemFactory *factory, GtkListItem *list_item,
					 AgsFileWidget *file_widget)
{
  GtkLabel *label;

  GtkStringObject *string_object;
  
  gchar *primary_key;
  
  label = (GtkLabel *) gtk_list_item_get_child(list_item);

  string_object = GTK_STRING_OBJECT(gtk_list_item_get_item(list_item));
  primary_key = gtk_string_object_get_string(string_object);

  gtk_label_set_label(label,
		      primary_key);
}

void
ags_file_widget_value_factory_bind(GtkListItemFactory *factory, GtkListItem *list_item,
				   AgsFileWidget *file_widget)
{
  GtkLabel *label;

  GtkStringObject *string_object;

  gchar *primary_key;
  gchar *column_name, *value;
  gchar *filename;

  struct stat sb;
  
  int retval;
  
  label = (GtkLabel *) gtk_list_item_get_child(list_item);

  string_object = GTK_STRING_OBJECT(gtk_list_item_get_item(list_item));
  primary_key = gtk_string_object_get_string(string_object);

  filename = NULL;
  
  column_name = NULL;
  value = NULL;

  if(!g_strcmp0(file_widget->current_path,
		"recently-used:")){
    filename = g_strdup(primary_key);
    
  }else{
    if((!strncmp(primary_key, "..", 3) == FALSE) &&
       (!strncmp(primary_key, ".", 2) == FALSE)){
      filename = g_strdup_printf("%s/%s",
				 file_widget->current_path,
				 primary_key);
    }
  }

  value = NULL;

  if(filename != NULL &&
     g_file_test(filename, G_FILE_TEST_EXISTS)){
    retval = -1;
    
    if(factory == file_widget->filename_factory[0]){
      value = g_strdup_printf("%s",
			      primary_key);
    }else if(factory == file_widget->filename_factory[1]){
      if(g_access(filename, R_OK) == 0){
	retval = stat(filename,
		      &sb);
      }

      if(retval == 0){
	value = g_strdup_printf("%jd",
				sb.st_size);
      }
    }else if(factory == file_widget->filename_factory[2]){
      gchar *file_type;

      if(g_access(filename, R_OK) == 0){
	retval = stat(filename,
		      &sb);
      }  
      
      if(retval == 0){
	switch(sb.st_mode & S_IFMT){
	case S_IFBLK:
	  {
	    file_type = "block device";
	  }
	  break;
	case S_IFCHR:
	  {
	    file_type = "character device";
	  }
	  break;
	case S_IFDIR:
	  {
	    file_type = "directory";
	  }
	  break;
	case S_IFIFO:
	  {
	    file_type = "FIFO/pipe";
	  }
	  break;
	case S_IFLNK:
	  {
	    file_type = "symlink";
	  }
	  break;
	case S_IFREG:
	  {
	    file_type = "regular file";
	  }
	  break;
	case S_IFSOCK:
	  {
	    file_type = "socket";
	  }
	  break;
	default:
	  {
	    file_type = "unknown?";
	  }
	}

	value = g_strdup_printf("%s",
				file_type);
      }      
    }else if(factory == file_widget->filename_factory[3]){
      char outstr[200];

      if(g_access(filename, R_OK) == 0){
	retval = stat(filename,
		      &sb);
      }
  
      if(retval == 0){
	strftime(outstr, sizeof(outstr), "%a, %d %b %Y %T %z", localtime(&sb.st_mtime));
      
	value = g_strdup(outstr);
      }
    }
  }

  if(value == NULL){
    value = g_strdup("");
  }
  
  gtk_label_set_label(label,
		      value);
}

void
ags_file_widget_location_entry_callback(GtkEntry *location_entry,
					AgsFileWidget *file_widget)
{
  gchar *filename;
  gchar *prev_current_path;
  
  filename = gtk_editable_get_text(GTK_EDITABLE(file_widget->location_entry));

  if(filename != NULL){
    prev_current_path = file_widget->current_path;

    if(g_file_test(filename, G_FILE_TEST_EXISTS) &&
       g_file_test(filename, G_FILE_TEST_IS_DIR)){
      guint length;

      length = strlen(filename);
      
      if(length > 1 &&
	 filename[length - 1] == '/'){
	file_widget->current_path = g_strdup_printf("%s",
						    filename);
      }else{
	file_widget->current_path = g_strndup(filename,
					      length);
      }
      
      ags_file_widget_refresh(file_widget);
    }else{
      if(ags_file_widget_test_file_action(file_widget, AGS_FILE_WIDGET_SAVE_AS)){
	guint length;

	length = strlen(filename);
      
	file_widget->current_path = g_strdup_printf("%s",
						    filename);
      
	ags_file_widget_refresh(file_widget);	
      }else{
	prev_current_path = NULL;
      }
    }

    g_free(prev_current_path);
  }
}

void
ags_file_widget_gesture_click_pressed_callback(GtkGestureClick *event_controller,
					       gint n_press,
					       gdouble x,
					       gdouble y,
					       AgsFileWidget *file_widget)
{
  //empty
}

void
ags_file_widget_gesture_click_released_callback(GtkGestureClick *event_controller,
						gint n_press,
						gdouble x,
						gdouble y,
						AgsFileWidget *file_widget)
{
  //empty
}

void
ags_file_widget_filename_activate_callback(GtkListView *list_view,
					   guint position,
					   AgsFileWidget *file_widget)
{
  gchar *current_path, *filename;
  gchar *prev_current_path;

  if(position == GTK_INVALID_LIST_POSITION){
    return;
  }
  
  filename = NULL;
  
  if(!ags_file_widget_test_flags(file_widget, AGS_FILE_WIDGET_WITH_MULTI_SELECTION)){
    GtkStringObject *string_object;

    string_object = gtk_single_selection_get_selected_item(file_widget->filename_single_selection);
    
    filename = gtk_string_object_get_string(string_object);
  }

  if(ags_file_widget_test_flags(file_widget, AGS_FILE_WIDGET_WITH_MULTI_SELECTION)){
    GtkBitset *bitset;
    GtkStringObject *string_object;

    guint64 length;

    bitset = gtk_selection_model_get_selection(GTK_SELECTION_MODEL(file_widget->filename_multi_selection));

    length = gtk_bitset_get_size(bitset);

    if(length > 0){
      guint position;

      position = gtk_bitset_get_nth(bitset,
				    0);
    
      string_object = g_list_model_get_item(gtk_multi_selection_get_model(file_widget->filename_multi_selection),
					    position);

      filename = gtk_string_object_get_string(string_object);
    }  
  }
  
  if(filename != NULL &&
     !strncmp(filename, ".", 2) == FALSE &&
     !strncmp(filename, "..", 3) == FALSE){
    if(strlen(file_widget->current_path) > 1){
      current_path = g_strdup_printf("%s/%s",
				     file_widget->current_path,
				     filename);
    }else{
      current_path = g_strdup_printf("/%s",
				     filename);
    }
  }else{
    current_path = g_strdup(filename);
  }
  
  prev_current_path = file_widget->current_path;

  if(!(g_file_test(current_path,
		   G_FILE_TEST_IS_DIR) ||
       g_file_test(current_path,
		   G_FILE_TEST_IS_SYMLINK))){
    current_path = NULL;
  }
  
  if(current_path != NULL &&
     (!g_strcmp0(current_path, prev_current_path)) == FALSE &&
     (!strncmp(current_path, ".", 2)) == FALSE &&
     ((!strncmp(current_path, "..", 3)) == FALSE ||
      (!strncmp(current_path, "/", 2)) == FALSE)){
    if(!strncmp(current_path, "..", 3)){
      gchar *iter;
	
      if((iter = strrchr(prev_current_path, '/')) != NULL){
	if(iter != prev_current_path){
	  file_widget->current_path = g_strndup(prev_current_path,
						iter - prev_current_path);
	}else{
	  file_widget->current_path = g_strdup("/");
	}
      }else{
	prev_current_path = NULL;
      }
    }else{
      file_widget->current_path = current_path;
    }
      
    ags_file_widget_refresh(file_widget);
  }else{
    prev_current_path = NULL;
  }

  g_free(prev_current_path);
}

/**
 * ags_file_widget_test_flags:
 * @file_widget: the #AgsFileWidget
 * @flags: the flags
 *
 * Test @flags of @file_widget.
 * 
 * Returns: %TRUE if flags set, otherwise %FALSE
 * 
 * Since: 6.6.0
 */
gboolean
ags_file_widget_test_flags(AgsFileWidget *file_widget,
			   guint flags)
{
  gboolean success;

  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return(FALSE);
  }
  
  success = ((flags & (file_widget->flags)) != 0) ? TRUE: FALSE;
  
  return(success);
}

/**
 * ags_file_widget_set_flags:
 * @file_widget: the #AgsFileWidget
 * @flags: the flags
 *
 * Set @flags of @file_widget.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_set_flags(AgsFileWidget *file_widget,
			  guint flags)
{
  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return;
  }

  if((AGS_FILE_WIDGET_WITH_MULTI_SELECTION & (file_widget->flags)) == 0 &&
     (AGS_FILE_WIDGET_WITH_MULTI_SELECTION & (flags)) != 0){
    gtk_column_view_set_model(file_widget->filename_view,
			      GTK_SELECTION_MODEL(file_widget->filename_multi_selection));
  }
  
  file_widget->flags |= flags;
}

/**
 * ags_file_widget_unset_flags:
 * @file_widget: the #AgsFileWidget
 * @flags: the flags
 *
 * Set @flags of @file_widget.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_unset_flags(AgsFileWidget *file_widget,
			    guint flags)
{
  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return;
  }

  if((AGS_FILE_WIDGET_WITH_MULTI_SELECTION & (file_widget->flags)) != 0 &&
     (AGS_FILE_WIDGET_WITH_MULTI_SELECTION & (flags)) != 0){
    gtk_column_view_set_model(file_widget->filename_view,
			      GTK_SELECTION_MODEL(file_widget->filename_single_selection));
  }

  file_widget->flags &= (~flags);
}

/**
 * ags_file_widget_test_file_action:
 * @file_widget: the #AgsFileWidget
 * @file_action: the file action
 *
 * Test @file_action of @file_widget.
 * 
 * Returns: %TRUE if file action matches, otherwise %FALSE
 * 
 * Since: 6.6.0
 */
gboolean
ags_file_widget_test_file_action(AgsFileWidget *file_widget,
				 guint file_action)
{
  gboolean success;

  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return(FALSE);
  }
  
  success = (file_action == file_widget->file_action) ? TRUE: FALSE;
  
  return(success);
}

/**
 * ags_file_widget_set_file_action:
 * @file_widget: the #AgsFileWidget
 * @file_action: the file action
 *
 * Set @file_action of @file_widget.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_set_file_action(AgsFileWidget *file_widget,
				guint file_action)
{
  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return;
  }

  file_widget->file_action = file_action;
}

/**
 * ags_file_widget_test_file_filter:
 * @file_widget: the #AgsFileWidget
 * @file_filter: the file filter
 *
 * Test @file_filter of @file_widget.
 * 
 * Returns: %TRUE if file filter matches, otherwise %FALSE
 * 
 * Since: 6.6.0
 */
gboolean
ags_file_widget_test_file_filter(AgsFileWidget *file_widget,
				 guint file_filter)
{
  gboolean success;

  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return(FALSE);
  }
  
  success = (file_filter == file_widget->file_filter) ? TRUE: FALSE;
  
  return(success);
}

/**
 * ags_file_widget_set_file_filter:
 * @file_widget: the #AgsFileWidget
 * @file_filter: the file filter
 *
 * Set @file_filter of @file_widget.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_set_file_filter(AgsFileWidget *file_widget,
				guint file_filter)
{
  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return;
  }

  file_widget->file_filter = file_filter;
}

/**
 * ags_file_widget_set_default_bundle:
 * @file_widget: the #AgsFileWidget
 * @default_bundle: the default bundle
 *
 * Set @default_bundle of @file_widget.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_set_default_bundle(AgsFileWidget *file_widget,
				   gchar *default_bundle)
{
  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return;
  }

  g_object_set(file_widget,
	       "default-bundle", default_bundle,
	       NULL);
}

/**
 * ags_file_widget_get_default_bundle:
 * @file_widget: the #AgsFileWidget
 *
 * Get default bundle of @file_widget.
 * 
 * Returns: (transfer full): the default bundle as string
 * 
 * Since: 6.6.0
 */
gchar*
ags_file_widget_get_default_bundle(AgsFileWidget *file_widget)
{
  gchar *default_bundle;

  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return(NULL);
  }

  default_bundle = NULL;

  g_object_get(file_widget,
	       "default-bundle", &default_bundle,
	       NULL);

  return(default_bundle);
}

/**
 * ags_file_widget_set_home_path:
 * @file_widget: the #AgsFileWidget
 * @home_path: the home path
 *
 * Set @home_path of @file_widget.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_set_home_path(AgsFileWidget *file_widget,
			      gchar *home_path)
{
  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return;
  }

  g_object_set(file_widget,
	       "home-path", home_path,
	       NULL);
}

/**
 * ags_file_widget_get_home_path:
 * @file_widget: the #AgsFileWidget
 *
 * Get home path of @file_widget.
 * 
 * Returns: (transfer full): the home path as string
 * 
 * Since: 6.6.0
 */
gchar*
ags_file_widget_get_home_path(AgsFileWidget *file_widget)
{
  gchar *home_path;

  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return(NULL);
  }

  home_path = NULL;

  g_object_get(file_widget,
	       "home-path", &home_path,
	       NULL);

  return(home_path);
}

/**
 * ags_file_widget_set_app_home_path:
 * @file_widget: the #AgsFileWidget
 * @app_home_path: the app home path
 *
 * Set @app_home_path of @file_widget.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_set_app_home_path(AgsFileWidget *file_widget,
				  gchar *app_home_path)
{
  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return;
  }

  g_object_set(file_widget,
	       "app-home-path", app_home_path,
	       NULL);
}

/**
 * ags_file_widget_get_app_home_path:
 * @file_widget: the #AgsFileWidget
 *
 * Get app home path of @file_widget.
 * 
 * Returns: (transfer full): the app home path as string
 * 
 * Since: 6.6.0
 */
gchar*
ags_file_widget_get_app_home_path(AgsFileWidget *file_widget)
{
  gchar *app_home_path;

  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return(NULL);
  }

  app_home_path = NULL;

  g_object_get(file_widget,
	       "app-home-path", &app_home_path,
	       NULL);

  return(app_home_path);
}

/**
 * ags_file_widget_set_app_generic_path:
 * @file_widget: the #AgsFileWidget
 * @app_generic_path: the app generic path
 *
 * Set @app_generic_path of @file_widget.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_set_app_generic_path(AgsFileWidget *file_widget,
				     gchar *app_generic_path)
{
  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return;
  }

  g_object_set(file_widget,
	       "app-generic-path", app_generic_path,
	       NULL);
}

/**
 * ags_file_widget_get_app_generic_path:
 * @file_widget: the #AgsFileWidget
 *
 * Get app generic path of @file_widget.
 * 
 * Returns: (transfer full): the app generic path as string
 * 
 * Since: 6.6.0
 */
gchar*
ags_file_widget_get_app_generic_path(AgsFileWidget *file_widget)
{
  gchar *app_generic_path;

  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return(NULL);
  }

  app_generic_path = NULL;

  g_object_get(file_widget,
	       "app-generic-path", &app_generic_path,
	       NULL);

  return(app_generic_path);
}

/**
 * ags_file_widget_set_default_path:
 * @file_widget: the #AgsFileWidget
 * @default_path: the default path
 *
 * Set @default_path of @file_widget.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_set_default_path(AgsFileWidget *file_widget,
				 gchar *default_path)
{
  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return;
  }

  g_object_set(file_widget,
	       "default-path", default_path,
	       NULL);
}

/**
 * ags_file_widget_get_default_path:
 * @file_widget: the #AgsFileWidget
 *
 * Get default path of @file_widget.
 * 
 * Returns: (transfer full): the default path as string
 * 
 * Since: 6.6.0
 */
gchar*
ags_file_widget_get_default_path(AgsFileWidget *file_widget)
{
  gchar *default_path;

  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return(NULL);
  }

  default_path = NULL;

  g_object_get(file_widget,
	       "default-path", &default_path,
	       NULL);

  return(default_path);
}

/**
 * ags_file_widget_set_current_path:
 * @file_widget: the #AgsFileWidget
 * @current_path: the current path
 *
 * Set @current_path of @file_widget.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_set_current_path(AgsFileWidget *file_widget,
				 gchar *current_path)
{
  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return;
  }

  g_object_set(file_widget,
	       "current-path", current_path,
	       NULL);
}

/**
 * ags_file_widget_get_current_path:
 * @file_widget: the #AgsFileWidget
 *
 * Get current path of @file_widget.
 * 
 * Returns: (transfer full): the current path as string
 * 
 * Since: 6.6.0
 */
gchar*
ags_file_widget_get_current_path(AgsFileWidget *file_widget)
{
  gchar *current_path;

  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return(NULL);
  }

  current_path = NULL;

  g_object_get(file_widget,
	       "current-path", &current_path,
	       NULL);

  return(current_path);
}

/**
 * ags_file_widget_set_recently_used_filename:
 * @file_widget: the #AgsFileWidget
 * @recently_used_filename: the recently used filename
 *
 * Set @recently_used_filename of @file_widget.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_set_recently_used_filename(AgsFileWidget *file_widget,
					   gchar *recently_used_filename)
{
  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return;
  }

  g_object_set(file_widget,
	       "recently-used-filename", recently_used_filename,
	       NULL);
}

/**
 * ags_file_widget_get_recently_used_filename:
 * @file_widget: the #AgsFileWidget
 *
 * Get recently used filename of @file_widget.
 * 
 * Returns: (transfer full): the home path as string
 * 
 * Since: 6.6.0
 */
gchar*
ags_file_widget_get_recently_used_filename(AgsFileWidget *file_widget)
{
  gchar *recently_used_filename;

  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return(NULL);
  }

  recently_used_filename = NULL;

  g_object_get(file_widget,
	       "recently-used-filename", &recently_used_filename,
	       NULL);

  return(recently_used_filename);
}

/**
 * ags_file_widget_set_bookmark_filename:
 * @file_widget: the #AgsFileWidget
 * @bookmark_filename: the bookmark filename
 *
 * Set @bookmark_filename of @file_widget.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_set_bookmark_filename(AgsFileWidget *file_widget,
				      gchar *bookmark_filename)
{
  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return;
  }

  g_object_set(file_widget,
	       "bookmark-filename", bookmark_filename,
	       NULL);
}

/**
 * ags_file_widget_get_bookmark_filename:
 * @file_widget: the #AgsFileWidget
 *
 * Get bookmark filename of @file_widget.
 * 
 * Returns: (transfer full): the bookmark filename as string
 * 
 * Since: 6.6.0
 */
gchar*
ags_file_widget_get_bookmark_filename(AgsFileWidget *file_widget)
{
  gchar *bookmark_filename;

  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return(NULL);
  }

  bookmark_filename = NULL;

  g_object_get(file_widget,
	       "bookmark-filename", &bookmark_filename,
	       NULL);

  return(bookmark_filename);
}

/**
 * ags_file_widget_set_file_magic_executable:
 * @file_widget: the #AgsFileWidget
 * @file_magic_executable: the file magic executable
 *
 * Set @file_magic_executable of @file_widget.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_set_file_magic_executable(AgsFileWidget *file_widget,
					  gchar *file_magic_executable)
{
  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return;
  }

  g_object_set(file_widget,
	       "file-magic-executable", file_magic_executable,
	       NULL);
}

/**
 * ags_file_widget_get_file_magic_executable:
 * @file_widget: the #AgsFileWidget
 *
 * Get file magic executable of @file_widget.
 * 
 * Returns: (transfer full): the file magic executable as string
 * 
 * Since: 6.6.0
 */
gchar*
ags_file_widget_get_file_magic_executable(AgsFileWidget *file_widget)
{
  gchar *file_magic_executable;

  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return(NULL);
  }

  file_magic_executable = NULL;

  g_object_get(file_widget,
	       "file-magic-executable", &file_magic_executable,
	       NULL);

  return(file_magic_executable);
}

void
ags_file_widget_location_drop_down_callback(GObject *location,
					    GParamSpec *pspec,
					    AgsFileWidget *file_widget)
{
  GObject *item;
  
  gchar *current_path;
  gchar *prev_current_path;

  item = gtk_drop_down_get_selected_item((GtkDropDown *) location);

  if(item == NULL ||
     !GTK_IS_STRING_OBJECT(item)){
    return;
  }
  
  current_path = gtk_string_object_get_string((GtkStringObject *) item);

  prev_current_path = file_widget->current_path;

  if(current_path != NULL &&
     (!g_strcmp0(current_path, prev_current_path)) == FALSE){
    file_widget->current_path = g_strdup(current_path);

    ags_file_widget_refresh(file_widget);
  }else{
    prev_current_path = NULL;
  }

  g_free(prev_current_path);
}

void
ags_file_widget_location_callback(AgsIconLink *icon_link,
				  AgsFileWidget *file_widget)
{
  gchar *button_action;
  gchar *current_path;
  gchar *prev_current_path;
  gchar *str;

  current_path = NULL;
  prev_current_path = file_widget->current_path;

  if(g_hash_table_lookup(file_widget->location, AGS_FILE_WIDGET_LOCATION_OPEN_RECENT) == icon_link){
    current_path = g_strdup("recently-used:");
  }else if(g_hash_table_lookup(file_widget->location, AGS_FILE_WIDGET_LOCATION_OPEN_START_HERE) == icon_link){
#if defined(AGS_MACOS_SANDBOX)
    current_path = g_strdup_printf("%s/Music/GSequencer/workspace",
				   file_widget->home_path);
#endif

#if defined(AGS_FLATPAK_SANDBOX)
    if((str = getenv("HOME")) != NULL){
      current_path = g_strdup_printf("%s/workspace",
				     str);
    }else{
      current_path = g_strdup_printf("%s/workspace",
				     file_widget->home_path);
    }
#endif
    
#if defined(AGS_SNAP_SANDBOX)
    if((str = getenv("SNAP_USER_DATA")) != NULL){
      current_path = g_strdup_printf("%s/workspace",
				     str);
    }else{
      current_path = g_strdup_printf("%s/workspace",
				     file_widget->home_path);
    }
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
    current_path = g_strdup_printf("%s/workspace",
				   file_widget->home_path);
#endif    
  }else if(g_hash_table_lookup(file_widget->location, AGS_FILE_WIDGET_LOCATION_OPEN_USER_HOME) == icon_link){
#if defined(AGS_MACOS_SANDBOX)
    current_path = g_strdup(file_widget->home_path);
#endif

#if !defined(AGS_MACOS_SANDBOX)
    if((str = getenv("HOME")) != NULL){
      current_path = g_strdup(str);
    }else{
      current_path = g_strdup(file_widget->home_path);
    }
#endif
  }else if(g_hash_table_lookup(file_widget->location, AGS_FILE_WIDGET_LOCATION_OPEN_USER_DESKTOP) == icon_link){
    if((str = getenv("XDG_DESKTOP_DIR")) != NULL){
      current_path = g_strdup(str);
    }else{
      current_path = g_strdup_printf("%s/Desktop",
				     file_widget->home_path);
    }
  }else if(g_hash_table_lookup(file_widget->location, AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_PICTURES) == icon_link){
    if((str = getenv("XDG_PICTURES_DIR")) != NULL){
      current_path = g_strdup(str);
    }else{
      current_path = g_strdup_printf("%s/Pictures",
				     file_widget->home_path);
    }
  }else if(g_hash_table_lookup(file_widget->location, AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_MUSIC) == icon_link){
    if((str = getenv("XDG_MUSIC_DIR")) != NULL){
      current_path = g_strdup(str);
    }else{
      current_path = g_strdup_printf("%s/Music",
				     file_widget->home_path);
    }
  }else if(g_hash_table_lookup(file_widget->location, AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_VIDEOS) == icon_link){
    if((str = getenv("XDG_VIDEOS_DIR")) != NULL){
      current_path = g_strdup(str);
    }else{
      current_path = g_strdup_printf("%s/Videos",
				     file_widget->home_path);
    }
  }else if(g_hash_table_lookup(file_widget->location, AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOWNLOADS) == icon_link){
    if((str = getenv("XDG_DOWNLOAD_DIR")) != NULL){
      current_path = g_strdup(str);
    }else{
      current_path = g_strdup_printf("%s/Downloads",
				     file_widget->home_path);
    }
  }else if(g_hash_table_lookup(file_widget->location, AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOCUMENTS) == icon_link){
    if((str = getenv("XDG_DOCUMENTS_DIR")) != NULL){
      current_path = g_strdup(str);
    }else{
      current_path = g_strdup_printf("%s/Documents",
				     file_widget->home_path);
    }
  }else if(g_hash_table_lookup(file_widget->location, AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_APP_HOME) == icon_link){
    current_path = g_strdup(file_widget->app_home_path);
  }else if(g_hash_table_lookup(file_widget->location, AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_APP_GENERIC) == icon_link){
    current_path = g_strdup(file_widget->app_generic_path);
  }

  if(current_path != NULL &&
     (!g_strcmp0(current_path, prev_current_path)) == FALSE){
    file_widget->current_path = current_path;

    ags_file_widget_refresh(file_widget);
  }else{
    prev_current_path = NULL;
  }

  g_free(prev_current_path);
}

/**
 * ags_file_widget_get_recently_used:
 * @file_widget: the #AgsFileWidget
 * @strv_length: (out): the string vector length
 *
 * Get recently used from @file_widget.
 *
 * Returns: (transfer full): the recently used string vector
 * 
 * Since: 6.6.0
 */
gchar**
ags_file_widget_get_recently_used(AgsFileWidget *file_widget,
				  guint *strv_length)
{
  g_return_val_if_fail(AGS_IS_FILE_WIDGET(file_widget), NULL);

  if(strv_length != 0){
    strv_length[0] = g_strv_length(file_widget->recently_used);
  }
  
  return(g_strdupv(file_widget->recently_used));
}

/**
 * ags_file_widget_read_recently_used:
 * @file_widget: the #AgsFileWidget
 *
 * Read recently-used from recently-used-filename.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_read_recently_used(AgsFileWidget *file_widget)
{
  xmlDoc *recently_used_doc;
  xmlNode *root_node;
  xmlNode *node;

  gchar **strv;
  
  guint strv_length;
  gboolean is_end;
  guint i, i_stop;
  
  g_return_if_fail(AGS_IS_FILE_WIDGET(file_widget));
  g_return_if_fail(file_widget->recently_used_filename != NULL);

  recently_used_doc = xmlReadFile(file_widget->recently_used_filename,
				  NULL,
				  0);

  if(recently_used_doc == NULL){
    return;
  }
  
  root_node = xmlDocGetRootElement(recently_used_doc);

  if(root_node != NULL &&
     !xmlStrncmp("resources",
		 root_node->name,
		 9)){

    node = root_node->children;

    strv = file_widget->recently_used;

    strv_length = g_strv_length(file_widget->recently_used);

    i = 0;
    i_stop = 10;

    is_end = FALSE;
    
    while(node != NULL){
      if(node->type == XML_ELEMENT_NODE){
	if(!xmlStrncmp("resource",
		       node->name,
		       8)){
	  xmlChar *filename;

	  filename = xmlNodeGetContent(node);

	  if(i < i_stop && i < AGS_FILE_WIDGET_MAX_RECENTLY_USED){
	    if(strv[i] == NULL){
	      is_end = TRUE;
	    }
	    
	    if(!is_end){
	      g_free(strv[i]);
	    }
	    
	    strv[i] = g_strdup(filename);
	  }
	  
	  xmlFree(filename);

	  i++;
	}
      }

      node = node->next;
    }

    strv[i] = NULL;
  }
}

/**
 * ags_file_widget_write_recently_used:
 * @file_widget: the #AgsFileWidget
 *
 * Write recently_useds from recently_used-filename.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_write_recently_used(AgsFileWidget *file_widget)
{
  FILE *out;

  xmlDoc *recently_used_doc;
  xmlNode *root_node;
  xmlNode *node;

  gchar **iter;

  gchar *default_path;
  xmlChar *buffer;

  int size;

  g_return_if_fail(AGS_IS_FILE_WIDGET(file_widget));
  g_return_if_fail(file_widget->recently_used_filename != NULL);
  
  recently_used_doc = xmlNewDoc("1.0");

  root_node = xmlNewNode(NULL,
			 BAD_CAST "resources");

  xmlDocSetRootElement(recently_used_doc,
		       root_node);
  
  iter = file_widget->recently_used;

  if(iter != NULL){
    for(; iter[0] != NULL; iter++){
      node = xmlNewNode(NULL,
			BAD_CAST "resource");

      xmlNodeAddContent(node,
			(gchar *) iter[0]);
      
      xmlAddChild(root_node,
		  node);
    }
  }

  default_path = NULL;
  
  if(strrchr(file_widget->recently_used_filename, '/') != NULL){
    default_path = g_strndup(file_widget->recently_used_filename,
			     strrchr(file_widget->recently_used_filename, '/') - file_widget->recently_used_filename);
  }
  
  g_mkdir_with_parents(default_path,
		       0755);

  out = NULL;
  buffer = NULL;
  
  if(g_file_test(default_path, G_FILE_TEST_EXISTS) &&
     g_file_test(default_path, G_FILE_TEST_IS_DIR)){
    out = fopen(file_widget->recently_used_filename, "w+");
    size = 0;

    if(out != NULL){
      xmlDocDumpFormatMemoryEnc(recently_used_doc, &(buffer), &size, "UTF-8", TRUE);
      
      fwrite(buffer, size, sizeof(xmlChar), out);
      fflush(out);
      fclose(out);

      xmlFree(buffer);
    }
  }

  xmlFreeDoc(recently_used_doc);
}

/**
 * ags_file_widget_get_location:
 * @file_widget: the #AgsFileWidget
 *
 * Get location from @file_widget.
 *
 * Returns: (transfer container): the location #GHashTable
 * 
 * Since: 6.6.0
 */
GHashTable*
ags_file_widget_get_location(AgsFileWidget *file_widget)
{
  GHashTable *location;
  
  g_return_val_if_fail(AGS_IS_FILE_WIDGET(file_widget), NULL);

  location = file_widget->location;

  g_hash_table_ref(location);

  return(location);
}

/**
 * ags_file_widget_add_location:
 * @file_widget: the #AgsFileWidget
 * @button_action: the button action
 * @button_text: the button text
 *
 * Add @button_action to location with @button_text.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_add_location(AgsFileWidget *file_widget,
			     gchar *button_action,
			     gchar *button_text)
{
  if(!AGS_IS_FILE_WIDGET(file_widget) ||
     button_action == NULL){
    return;
  }

  if(g_hash_table_lookup(file_widget->location, button_action) == NULL){
    AgsIconLink *icon_link;

    gchar *str;
    
    icon_link = NULL;
    str = NULL;
    
    if(!strncmp(button_action, AGS_FILE_WIDGET_LOCATION_OPEN_START_HERE, strlen(AGS_FILE_WIDGET_LOCATION_OPEN_START_HERE) + 1)){
      str = g_strdup_printf("<span foreground=\"#0000ff\"><u>%s</u></span>",
			    i18n("Start here"));
      
      icon_link = ags_icon_link_new("folder-start-here",
				    AGS_FILE_WIDGET_LOCATION_OPEN_START_HERE,
				    str);
    }else if(!strncmp(button_action, AGS_FILE_WIDGET_LOCATION_OPEN_USER_HOME, strlen(AGS_FILE_WIDGET_LOCATION_OPEN_USER_HOME) + 1)){
      str = g_strdup_printf("<span foreground=\"#0000ff\"><u>%s</u></span>",
			    i18n("Home"));
      
      icon_link = ags_icon_link_new("user-home",
				    AGS_FILE_WIDGET_LOCATION_OPEN_USER_HOME,
				    str);
    }else if(!strncmp(button_action, AGS_FILE_WIDGET_LOCATION_OPEN_USER_DESKTOP, strlen(AGS_FILE_WIDGET_LOCATION_OPEN_USER_DESKTOP) + 1)){
      str = g_strdup_printf("<span foreground=\"#0000ff\"><u>%s</u></span>",
			    i18n("Desktop"));
      
      icon_link = ags_icon_link_new("user-desktop",
				    AGS_FILE_WIDGET_LOCATION_OPEN_USER_DESKTOP,
				    str);
    }else if(!strncmp(button_action, AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_PICTURES, strlen(AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_PICTURES) + 1)){
      str = g_strdup_printf("<span foreground=\"#0000ff\"><u>%s</u></span>",
			    i18n("Pictures"));
      
      icon_link = ags_icon_link_new("folder-pictures",
				    AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_PICTURES,
				    str);
    }else if(!strncmp(button_action, AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_MUSIC, strlen(AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_MUSIC) + 1)){
      str = g_strdup_printf("<span foreground=\"#0000ff\"><u>%s</u></span>",
			    i18n("Music"));
      
      icon_link = ags_icon_link_new("folder-music",
				    AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_MUSIC,
				    str);      
    }else if(!strncmp(button_action, AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_VIDEOS, strlen(AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_VIDEOS) + 1)){
      str = g_strdup_printf("<span foreground=\"#0000ff\"><u>%s</u></span>",
			    i18n("Videos"));
      
      icon_link = ags_icon_link_new("folder-videos",
				    AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_VIDEOS,
				    str);      
    }else if(!strncmp(button_action, AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOWNLOADS, strlen(AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOWNLOADS) + 1)){
      str = g_strdup_printf("<span foreground=\"#0000ff\"><u>%s</u></span>",
			    i18n("Downloads"));
      
      icon_link = ags_icon_link_new("folder-download",
				    AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOWNLOADS,
				    str);
    }else if(!strncmp(button_action, AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOCUMENTS, strlen(AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOCUMENTS) + 1)){
      str = g_strdup_printf("<span foreground=\"#0000ff\"><u>%s</u></span>",
			    i18n("Documents"));
      
      icon_link = ags_icon_link_new("folder-documents",
				    AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOCUMENTS,
				    str);      
    }else if(!strncmp(button_action, AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_APP_HOME, strlen(AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_APP_HOME) + 1)){
      str = g_strdup_printf("<span foreground=\"#0000ff\"><u>%s</u></span>",
			    i18n("App home"));
      
      icon_link = ags_icon_link_new("folder",
				    AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_APP_HOME,
				    str);
    }else if(!strncmp(button_action, AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_APP_GENERIC, strlen(AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_APP_GENERIC) + 1)){
      str = g_strdup_printf("<span foreground=\"#0000ff\"><u>%s</u></span>",
			    i18n("App generic"));
      
      icon_link = ags_icon_link_new("folder",
				    AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_APP_GENERIC,
				    str);
    }else{
      //      icon_link = ags_icon_link_new("folder");

      return;
    }

    g_free(str);
    
    if(button_text != NULL){
      ags_icon_link_set_link_text(icon_link,
				  button_text);
    }
    
    gtk_box_append(file_widget->location_box,
		   (GtkWidget *) icon_link);
    
    g_hash_table_insert(file_widget->location,
			g_strdup(button_action),
			icon_link);
    
    g_signal_connect(icon_link, "clicked",
		     G_CALLBACK(ags_file_widget_location_callback), file_widget);
  }
}

/**
 * ags_file_widget_remove_location:
 * @file_widget: the #AgsFileWidget
 * @button_action: the button action
 *
 * Remove @button_action from location with @button_text.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_remove_location(AgsFileWidget *file_widget,
				gchar *button_action)
{
  AgsIconLink *icon_link;

  if(!AGS_IS_FILE_WIDGET(file_widget) ||
     button_action == NULL){
    return;
  }

  if((icon_link = g_hash_table_lookup(file_widget->location, button_action)) != NULL){
    gtk_box_remove(file_widget->location_box,
		   (GtkWidget *) icon_link);
    
    g_hash_table_remove(file_widget->location,
			button_action);
    
    g_object_disconnect(icon_link,
			"any_signal::clicked",
			G_CALLBACK(ags_file_widget_location_callback),
			file_widget,
			NULL);
  }
}

void
ags_file_widget_bookmark_callback(AgsIconLink *icon_link,
				  AgsFileWidget *file_widget)
{
  gchar *current_path;
  gchar *prev_current_path;

  current_path = ags_icon_link_get_action(icon_link);

  prev_current_path = file_widget->current_path;

  if(current_path != NULL &&
     (!g_strcmp0(current_path, prev_current_path)) == FALSE){
    file_widget->current_path = g_strdup(current_path);

    ags_file_widget_refresh(file_widget);
  }else{
    prev_current_path = NULL;
  }

  g_free(prev_current_path);
  g_free(current_path);
}

void
ags_file_widget_rename_response_callback(AgsInputDialog *input_dialog,
					 gint response,
					 AgsFileWidget *file_widget)
{
  if(response == GTK_RESPONSE_ACCEPT){
    GtkStringObject *string_object;

    gchar *selected_filename;
    gchar *new_filename;
    gchar *new_path;
    gchar *new_str, *str;

    string_object = gtk_single_selection_get_selected_item(file_widget->filename_single_selection);
  
    selected_filename = gtk_string_object_get_string(string_object);

    new_filename = gtk_editable_get_text(GTK_EDITABLE(input_dialog->string_input));

    new_path = g_strdup(file_widget->current_path);

    str = g_strdup_printf("%s/%s",
			  new_path,
			  selected_filename);
    
    new_str = g_strdup_printf("%s/%s",
			      new_path,
			      new_filename);
    
    g_rename(str,
	     new_str);

    ags_file_widget_refresh(file_widget);

    g_free(new_path);
    
    g_free(str);
    g_free(new_str);
  }

  gtk_window_close((GtkWindow *) input_dialog);
}

void
ags_file_widget_mkdir_response_callback(AgsInputDialog *input_dialog,
					gint response,
					AgsFileWidget *file_widget)
{
  if(response == GTK_RESPONSE_ACCEPT){
    gchar *dir_path;
    gchar *new_path;
    gchar *new_str;

    dir_path = gtk_editable_get_text(GTK_EDITABLE(input_dialog->string_input));
    
    new_path = g_strdup(file_widget->current_path);

    new_str = g_strdup_printf("%s/%s",
			      new_path,
			      dir_path);

    ags_file_widget_create_dir(file_widget,
			       new_str);

    ags_file_widget_refresh(file_widget);

    g_free(new_path);
    
    g_free(new_str);
  }

  gtk_window_close((GtkWindow *) input_dialog);
}

void
ags_file_widget_rename_callback(GAction *action, GVariant *parameter,
				AgsFileWidget *file_widget)
{
  GtkWindow *transient_for;
  AgsInputDialog *input_dialog;

  transient_for = (GtkWindow *) gtk_widget_get_ancestor((GtkWidget *) file_widget,
							GTK_TYPE_WINDOW);
  
  input_dialog = ags_input_dialog_new(i18n("rename file or directory"),
				      transient_for);

  ags_input_dialog_set_flags(input_dialog,
			     AGS_INPUT_DIALOG_SHOW_STRING_INPUT);

  ags_input_dialog_set_text(input_dialog,
			    i18n("enter filename"));

  g_signal_connect(input_dialog, "response",
		   G_CALLBACK(ags_file_widget_rename_response_callback), file_widget);

  gtk_window_set_modal((GtkWindow *) input_dialog,
		       TRUE);
  gtk_window_present((GtkWindow *) input_dialog);
}

void
ags_file_widget_mkdir_callback(GAction *action, GVariant *parameter,
			       AgsFileWidget *file_widget)
{
  GtkWindow *transient_for;
  AgsInputDialog *input_dialog;

  transient_for = (GtkWindow *) gtk_widget_get_ancestor((GtkWidget *) file_widget,
							GTK_TYPE_WINDOW);
  
  input_dialog = ags_input_dialog_new(i18n("create directory"),
				      transient_for);

  ags_input_dialog_set_flags(input_dialog,
			     AGS_INPUT_DIALOG_SHOW_STRING_INPUT);

  ags_input_dialog_set_text(input_dialog,
			    i18n("enter directory"));

  g_signal_connect(input_dialog, "response",
		   G_CALLBACK(ags_file_widget_mkdir_response_callback), file_widget);

  gtk_window_set_modal((GtkWindow *) input_dialog,
		       TRUE);
  gtk_window_present((GtkWindow *) input_dialog);
}

void
ags_file_widget_show_hidden_files_callback(GAction *action, GVariant *parameter,
					   AgsFileWidget *file_widget)
{
  if(!ags_file_widget_test_flags(file_widget, AGS_FILE_WIDGET_HIDDEN_FILES_VISIBLE)){
    ags_file_widget_set_flags(file_widget,
			      AGS_FILE_WIDGET_HIDDEN_FILES_VISIBLE);

    ags_file_widget_refresh(file_widget);
  }else{
    ags_file_widget_unset_flags(file_widget,
				AGS_FILE_WIDGET_HIDDEN_FILES_VISIBLE);

    ags_file_widget_refresh(file_widget);
  }
}

void
ags_file_widget_add_bookmark_callback(GAction *action, GVariant *parameter,
				      AgsFileWidget *file_widget)
{
  GtkStringObject *string_object;

  gchar *new_path;
  gchar *bookmark_location;
  gchar *new_str;
  
  string_object = gtk_single_selection_get_selected_item(file_widget->filename_single_selection);

  bookmark_location = gtk_string_object_get_string(string_object);

  if((!strncmp(bookmark_location, ".", 2)) == FALSE &&
     (!strncmp(bookmark_location, "..", 3)) == FALSE){
    new_path = g_strdup(file_widget->current_path);

    new_str = g_strdup_printf("%s/%s",
			      new_path,
			      bookmark_location);
    
    ags_file_widget_add_bookmark(file_widget,
				 new_str);
  
    g_free(new_path);
  
    g_free(new_str);
  }
}

/**
 * ags_file_widget_get_bookmark:
 * @file_widget: the #AgsFileWidget
 *
 * Get bookmark from @file_widget.
 *
 * Returns: (transfer container): the bookmark #GHashTable
 * 
 * Since: 6.6.0
 */
GHashTable*
ags_file_widget_get_bookmark(AgsFileWidget *file_widget)
{
  GHashTable *bookmark;
  
  g_return_val_if_fail(AGS_IS_FILE_WIDGET(file_widget), NULL);

  bookmark = file_widget->bookmark;

  g_hash_table_ref(bookmark);

  return(bookmark);
}

/**
 * ags_file_widget_add_bookmark:
 * @file_widget: the #AgsFileWidget
 * @bookmark_location: the bookmark location
 *
 * Add @bookmark_location to bookmark with @button_text.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_add_bookmark(AgsFileWidget *file_widget,
			     gchar *bookmark_location)
{
  if(!AGS_IS_FILE_WIDGET(file_widget) ||
     bookmark_location == NULL ||
     !strncmp(bookmark_location, ".", 2) ||
     !strncmp(bookmark_location, "..", 3)){
    return;
  }

  if(g_hash_table_lookup(file_widget->bookmark, bookmark_location) == NULL){
    AgsIconLink *icon_link;

    gchar *str;
    gchar *iter;
    
    icon_link = NULL;
    str = NULL;

    iter = strrchr(bookmark_location, '/');

    if(iter != NULL){
      iter++;
    }else{
      iter = bookmark_location;
    }
    
    str = g_strdup_printf("<span foreground=\"#0000ff\"><u>%s</u></span>",
			  iter);
      
    icon_link = ags_icon_link_new("user-bookmarks",
				  bookmark_location,
				  str);

    g_free(str);
        
    gtk_box_append(file_widget->bookmark_box,
		   (GtkWidget *) icon_link);
    
    g_hash_table_insert(file_widget->bookmark,
			g_strdup(bookmark_location),
			icon_link);
    
    g_signal_connect(icon_link, "clicked",
		     G_CALLBACK(ags_file_widget_bookmark_callback), file_widget);
  }
}

/**
 * ags_file_widget_remove_bookmark:
 * @file_widget: the #AgsFileWidget
 * @bookmark_location: the bookmark location
 *
 * Remove @bookmark_location from bookmark with @button_text.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_remove_bookmark(AgsFileWidget *file_widget,
				gchar *bookmark_location)
{
  AgsIconLink *icon_link;

  if(!AGS_IS_FILE_WIDGET(file_widget) ||
     bookmark_location == NULL){
    return;
  }

  if((icon_link = g_hash_table_lookup(file_widget->bookmark, bookmark_location)) != NULL){
    gtk_box_remove(file_widget->bookmark_box,
		   (GtkWidget *) icon_link);
    
    g_hash_table_remove(file_widget->location,
			bookmark_location);
    
    g_object_disconnect(icon_link,
			"any_signal::clicked",
			G_CALLBACK(ags_file_widget_bookmark_callback),
			file_widget,
			NULL);
  }
}

/**
 * ags_file_widget_read_bookmark:
 * @file_widget: the #AgsFileWidget
 *
 * Read bookmarks from bookmark-filename.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_read_bookmark(AgsFileWidget *file_widget)
{
  xmlDoc *bookmark_doc;
  xmlNode *root_node;
  xmlNode *node;

  g_return_if_fail(AGS_IS_FILE_WIDGET(file_widget));
  g_return_if_fail(file_widget->bookmark_filename != NULL);

  bookmark_doc = xmlReadFile(file_widget->bookmark_filename,
			     NULL,
			     0);

  if(bookmark_doc == NULL){
    return;
  }
  
  root_node = xmlDocGetRootElement(bookmark_doc);

  if(root_node != NULL &&
     !xmlStrncmp("resources",
		 root_node->name,
		 10)){

    node = root_node->children;
    
    while(node != NULL){
      if(node->type == XML_ELEMENT_NODE){
	if(!xmlStrncmp("resource",
		       node->name,
		       9)){
	  xmlChar *filename;

	  filename = xmlNodeGetContent(node);

	  ags_file_widget_add_bookmark(file_widget,
				       filename);

	  xmlFree(filename);
	}
      }

      node = node->next;
    }
  }
}

/**
 * ags_file_widget_write_bookmark:
 * @file_widget: the #AgsFileWidget
 *
 * Write bookmarks from bookmark-filename.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_write_bookmark(AgsFileWidget *file_widget)
{
  GList *start_bookmark, *bookmark;
  
  FILE *out;

  xmlDoc *bookmark_doc;
  xmlNode *root_node;
  xmlNode *node;

  gchar *default_path;
  xmlChar *buffer;

  int size;

  g_return_if_fail(AGS_IS_FILE_WIDGET(file_widget));
  g_return_if_fail(file_widget->bookmark_filename != NULL);

  bookmark =
    start_bookmark = g_hash_table_get_keys(file_widget->bookmark);
  
  bookmark_doc = xmlNewDoc("1.0");

  root_node = xmlNewNode(NULL,
			 BAD_CAST "resources");

  xmlDocSetRootElement(bookmark_doc,
		       root_node);
  
  while(bookmark != NULL){
    node = xmlNewNode(NULL,
		      BAD_CAST "resource");

    xmlNodeAddContent(node,
		      (gchar *) bookmark->data);
    
    xmlAddChild(root_node,
		node);

    /* iter */
    bookmark = bookmark->next;
  }

  g_list_free(start_bookmark);

  default_path = NULL;
  
  if(strrchr(file_widget->recently_used_filename, '/') != NULL){
    default_path = g_strndup(file_widget->recently_used_filename,
			     strrchr(file_widget->recently_used_filename, '/') - file_widget->recently_used_filename);
  }
  
  g_mkdir_with_parents(default_path,
		       0755);
  
  out = NULL;
  buffer = NULL;

  if(g_file_test(default_path, G_FILE_TEST_EXISTS) &&
     g_file_test(default_path, G_FILE_TEST_IS_DIR)){
    out = fopen(file_widget->bookmark_filename, "w+");
    size = 0;
  
    if(out != NULL){
      xmlDocDumpFormatMemoryEnc(bookmark_doc, &(buffer), &size, "UTF-8", TRUE);

      fwrite(buffer, size, sizeof(xmlChar), out);
      fflush(out);
      fclose(out);

      xmlFree(buffer);
    }
  }

  xmlFreeDoc(bookmark_doc);
}

/**
 * ags_file_widget_get_filename:
 * @file_widget: the #AgsFileWidget
 *
 * Get filename of @file_widget.
 *
 * Returns: (transfer full): the filename as string
 * 
 * Since: 6.6.0
 */
gchar*
ags_file_widget_get_filename(AgsFileWidget *file_widget)
{
  gchar *filename;

  g_return_val_if_fail(AGS_IS_FILE_WIDGET(file_widget), NULL);

  filename = NULL;

  if(ags_file_widget_test_file_action(file_widget, AGS_FILE_WIDGET_SAVE_AS)){
    gchar *str;

    str = gtk_editable_get_text(GTK_EDITABLE(file_widget->location_entry));
    
    filename = g_strdup(str);
  }else{
    /* single selection */
    if(!ags_file_widget_test_flags(file_widget, AGS_FILE_WIDGET_WITH_MULTI_SELECTION)){
      GtkStringObject *string_object;

      gchar *selected_filename;

      string_object = gtk_single_selection_get_selected_item(file_widget->filename_single_selection);
  
      selected_filename = gtk_string_object_get_string(string_object);

      if(file_widget->current_path != NULL &&
	 !g_strcmp0(file_widget->current_path,
		    "recently-used:")){
	filename = g_strdup_printf("%s",
				   selected_filename);
      }else{
	filename = g_strdup_printf("%s/%s",
				   file_widget->current_path,
				   selected_filename);
      }
    }
  
    /* multi selection */
    if(ags_file_widget_test_flags(file_widget, AGS_FILE_WIDGET_WITH_MULTI_SELECTION)){
      GtkBitset *bitset;
      GtkStringObject *string_object;

      gchar *selected_filename;

      guint64 length;

      bitset = gtk_selection_model_get_selection(GTK_SELECTION_MODEL(file_widget->filename_multi_selection));

      length = gtk_bitset_get_size(bitset);

      if(length > 0){
	guint position;

	position = gtk_bitset_get_nth(bitset,
				      0);
    
	string_object = g_list_model_get_item(gtk_multi_selection_get_model(file_widget->filename_multi_selection),
					      position);
  
	selected_filename = gtk_string_object_get_string(string_object);

	if(file_widget->current_path != NULL &&
	   !g_strcmp0(file_widget->current_path,
		      "recently-used:")){
	  filename = g_strdup_printf("%s",
				     selected_filename);
	}else{
	  filename = g_strdup_printf("%s/%s",
				     file_widget->current_path,
				     selected_filename);
	}
      }
    }
  }
  
  return(filename);
}

/**
 * ags_file_widget_get_filenames:
 * @file_widget: the #AgsFileWidget
 *
 * Get filenames of @file_widget.
 *
 * Returns: (element-type utf8) (transfer full): the filename as string
 * 
 * Since: 6.6.0
 */
GSList*
ags_file_widget_get_filenames(AgsFileWidget *file_widget)
{
  GSList *start_filename;
  
  g_return_val_if_fail(AGS_IS_FILE_WIDGET(file_widget), NULL);
  
  start_filename = NULL;

  if(ags_file_widget_test_file_action(file_widget, AGS_FILE_WIDGET_SAVE_AS)){
    gchar *filename;
    gchar *str;

    str = gtk_editable_get_text(GTK_EDITABLE(file_widget->location_entry));
    
    filename = g_strdup(str);

    start_filename = g_slist_prepend(start_filename,
				     filename);
  }else{
    /* single selection */
    if(!ags_file_widget_test_flags(file_widget, AGS_FILE_WIDGET_WITH_MULTI_SELECTION)){
      GtkStringObject *string_object;

      gchar *selected_filename;
      gchar *filename;

      string_object = gtk_single_selection_get_selected_item(file_widget->filename_single_selection);
  
      selected_filename = gtk_string_object_get_string(string_object);

      filename = g_strdup_printf("%s/%s",
				 file_widget->current_path,
				 selected_filename);

      start_filename = g_slist_prepend(start_filename,
				       filename);
    }

    /* multi selection */
    if(ags_file_widget_test_flags(file_widget, AGS_FILE_WIDGET_WITH_MULTI_SELECTION)){
      GtkBitset *bitset;
      GtkStringObject *string_object;

      gchar *selected_filename;
      gchar *filename;

      guint64 length;
      guint64 i;

      bitset = gtk_selection_model_get_selection(GTK_SELECTION_MODEL(file_widget->filename_multi_selection));

      length = gtk_bitset_get_size(bitset);

      for(i = 0; i < length; i++){
	guint position;

	position = gtk_bitset_get_nth(bitset,
				      i);
    
	string_object = g_list_model_get_item(gtk_multi_selection_get_model(file_widget->filename_multi_selection),
					      position);
  
	selected_filename = gtk_string_object_get_string(string_object);

	filename = g_strdup_printf("%s/%s",
				   file_widget->current_path,
				   selected_filename);

	start_filename = g_slist_prepend(start_filename,
					 filename);
      }

      start_filename = g_slist_reverse(start_filename);
    }
  }
  
  return(start_filename);
}

void
ags_file_widget_real_refresh(AgsFileWidget *file_widget)
{
  GDir *current_dir;
  
  GtkStringList *single_filename_string_list;
  GtkStringList *multi_filename_string_list;
  GtkStringList *location_string_list;

  GList *start_filename, *filename;
  GList *start_location, *location;
  
  gchar **filename_strv;
  gchar **location_strv;			  

  gchar *current_filename;

  guint count;
  guint i, j;
  
  GError *error;

  /* filename view */
  current_dir = NULL;
  
  filename_strv = NULL;

  start_filename = NULL;

  if(file_widget->current_path != NULL &&
     g_file_test(file_widget->current_path,
		 G_FILE_TEST_IS_DIR)){
    error = NULL;

    if(g_access(file_widget->current_path, (R_OK | X_OK)) == 0){    
      current_dir = g_dir_open(file_widget->current_path,
			       0,
			       &error);
    }
    
    if(current_dir == NULL ||
       error != NULL){
      gchar *iter, *prev_iter;

      if(current_dir != NULL){
	g_dir_close(current_dir);
      }
      
      if(error != NULL){
	g_error_free(error);
      }
      
      filename_strv = (gchar **) g_malloc(3 * sizeof(gchar *));

      filename_strv[0] = g_strdup(".");
      filename_strv[1] = g_strdup("..");
      filename_strv[2] = NULL;

      location_strv = NULL;

      start_location = NULL;

      iter = file_widget->current_path;

      prev_iter = NULL;
      
      while((iter = strstr(iter, "/")) != NULL){
	if(prev_iter == NULL){
	  start_location = g_list_prepend(start_location,
					  g_strdup("/"));
	}else{
	  start_location = g_list_prepend(start_location,
					  g_strndup(file_widget->current_path,
						    iter - file_widget->current_path));
	}

	prev_iter = iter;
	iter++;
      }

      if(strlen(file_widget->current_path) > 1 &&
	 (!strncmp(file_widget->current_path, "/", 2)) == FALSE){
	start_location = g_list_prepend(start_location,
					g_strdup(file_widget->current_path));
      }
      
      location = start_location;

      count = g_list_length(start_location);
  
      location_strv = (gchar **) g_malloc((count + 1) * sizeof(gchar *));
  
      for(i = 0; location != NULL && i < count;){
	location_strv[i] = g_strdup(location->data);
    
	location = location->next;
	i++;
      }
      
      location_strv[i] = NULL;
    
      g_list_free(start_location);
      
      //  g_strfreev(filename_strv);

      if(!ags_file_widget_test_flags(file_widget, AGS_FILE_WIDGET_WITH_MULTI_SELECTION)){
	single_filename_string_list = gtk_string_list_new((const gchar * const *) filename_strv);
	gtk_single_selection_set_model(file_widget->filename_single_selection,
				       G_LIST_MODEL(single_filename_string_list));
      }
    
      if(ags_file_widget_test_flags(file_widget, AGS_FILE_WIDGET_WITH_MULTI_SELECTION)){
	multi_filename_string_list = gtk_string_list_new((const gchar * const *) filename_strv);
	gtk_multi_selection_set_model(file_widget->filename_multi_selection,
				      G_LIST_MODEL(multi_filename_string_list));
      }

      gtk_editable_set_text(GTK_EDITABLE(file_widget->location_entry),
			    file_widget->current_path);			  

      location_string_list = gtk_string_list_new((const gchar * const *) location_strv);
      gtk_drop_down_set_model(file_widget->location_drop_down,
			      G_LIST_MODEL(location_string_list));

      return;
    }

    do{
      current_filename = g_dir_read_name(current_dir);

      if(current_filename != NULL &&
	 ((!strncmp(current_filename, ".", 2) == FALSE) &&
	  (!strncmp(current_filename, "..", 3) == FALSE))){
	start_filename = g_list_insert_sorted(start_filename,
					      current_filename,
					      (GCompareFunc) g_strcmp0);
      }
    }while(current_filename != NULL);

    filename = start_filename;

    count = g_list_length(start_filename);
  
    filename_strv = (gchar **) g_malloc((count + 3) * sizeof(gchar *));

    j = 0;
    
    filename_strv[0] = g_strdup(".");
    filename_strv[1] = g_strdup("..");

    j = 2;
    
    for(i = 0; i < count; i++){
      if(((gchar *) filename->data)[0] != '.' ||
	 ags_file_widget_test_flags(file_widget, AGS_FILE_WIDGET_HIDDEN_FILES_VISIBLE)){
	filename_strv[j] = g_strdup(filename->data);
	j++;
      }
    
      filename = filename->next;
    }
  
    filename_strv[j] = NULL;
  
    if(!ags_file_widget_test_flags(file_widget, AGS_FILE_WIDGET_WITH_MULTI_SELECTION)){
      single_filename_string_list = gtk_string_list_new((const gchar * const *) filename_strv);
      gtk_single_selection_set_model(file_widget->filename_single_selection,
				     G_LIST_MODEL(single_filename_string_list));
    }
    
    if(ags_file_widget_test_flags(file_widget, AGS_FILE_WIDGET_WITH_MULTI_SELECTION)){
      multi_filename_string_list = gtk_string_list_new((const gchar * const *) filename_strv);
      gtk_multi_selection_set_model(file_widget->filename_multi_selection,
				    G_LIST_MODEL(multi_filename_string_list));
    }
    
    g_list_free(start_filename);

    //  g_strfreev(filename_strv);
  }

  if(current_dir != NULL){
    g_dir_close(current_dir);
  }
  
  /* recently-used */
  if(file_widget->current_path != NULL &&
     !g_strcmp0(file_widget->current_path,
		"recently-used:")){
    count = 0;
    
    filename_strv = ags_file_widget_get_recently_used(file_widget,
						      &count);

    if(!ags_file_widget_test_flags(file_widget, AGS_FILE_WIDGET_WITH_MULTI_SELECTION)){
      single_filename_string_list = gtk_string_list_new((const gchar * const *) filename_strv);
      gtk_single_selection_set_model(file_widget->filename_single_selection,
				     G_LIST_MODEL(single_filename_string_list));
    }
    
    if(ags_file_widget_test_flags(file_widget, AGS_FILE_WIDGET_WITH_MULTI_SELECTION)){
      multi_filename_string_list = gtk_string_list_new((const gchar * const *) filename_strv);
      gtk_multi_selection_set_model(file_widget->filename_multi_selection,
				    G_LIST_MODEL(multi_filename_string_list));
    }
  }
  
  /* location entry */
  if(file_widget->current_path != NULL){
    if(g_file_test(file_widget->current_path,
		   G_FILE_TEST_IS_REGULAR)){
      //TODO:JK: implement me
    }else if(g_file_test(file_widget->current_path,
			 G_FILE_TEST_IS_DIR)){
      gtk_editable_set_text(GTK_EDITABLE(file_widget->location_entry),
			    file_widget->current_path);			  
    }else if(!g_strcmp0(file_widget->current_path,
			"recently-used:")){    
      gtk_editable_set_text(GTK_EDITABLE(file_widget->location_entry),
			    i18n("Recently used:"));    
    }
  }
  
  /* location drop down */
  location_strv = NULL;

  start_location = NULL;
  
  if(file_widget->current_path != NULL){
    if(g_file_test(file_widget->current_path,
		   G_FILE_TEST_IS_DIR)){
      gchar *iter, *prev_iter;

      iter = file_widget->current_path;

      prev_iter = NULL;
      
      while((iter = strstr(iter, "/")) != NULL){
	if(prev_iter == NULL){
	  start_location = g_list_prepend(start_location,
					  g_strdup("/"));
	}else{
	  start_location = g_list_prepend(start_location,
					  g_strndup(file_widget->current_path,
						    iter - file_widget->current_path));
	}

	prev_iter = iter;
	iter++;
      }

      if(strlen(file_widget->current_path) > 1 &&
	 (!strncmp(file_widget->current_path, "/", 2)) == FALSE){
	start_location = g_list_prepend(start_location,
					g_strdup(file_widget->current_path));
      }
      
      location = start_location;

      count = g_list_length(start_location);
  
      location_strv = (gchar **) g_malloc((count + 1) * sizeof(gchar *));
  
      for(i = 0; location != NULL && i < count;){
	location_strv[i] = g_strdup(location->data);
    
	location = location->next;
	i++;
      }
      
      location_strv[i] = NULL;

      location_string_list = gtk_string_list_new((const gchar * const *) location_strv);

      gtk_drop_down_set_model(file_widget->location_drop_down,
			      G_LIST_MODEL(location_string_list));
      
      g_list_free(start_location);
      //  g_strfreev(location_strv);
    }else if(!g_strcmp0(file_widget->current_path,
			"recently-used:")){
      gtk_drop_down_set_model(file_widget->location_drop_down,
			      NULL);
    }
  }  
}

/**
 * ags_file_widget_refresh:
 * @file_widget: the #AgsFileWidget
 *
 * Refresh @file_widget due to current path change.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_refresh(AgsFileWidget *file_widget)
{
  g_return_if_fail(AGS_IS_FILE_WIDGET(file_widget));
  
  g_object_ref((GObject *) file_widget);
  g_signal_emit(G_OBJECT(file_widget),
		file_widget_signals[REFRESH], 0);
  g_object_unref((GObject *) file_widget);
}

void
ags_file_widget_real_create_dir(AgsFileWidget *file_widget,
				gchar *dir_path)
{  
  if(!g_mkdir_with_parents(dir_path,
			   0755)){
    //TODO:JK: implement me
  }
}

/**
 * ags_file_widget_create_dir:
 * @file_widget: the #AgsFileWidget
 * @dir_path: the directory path
 *
 * Create directory @dir_path.
 * 
 * Since: 6.6.0
 */
void
ags_file_widget_create_dir(AgsFileWidget *file_widget,
			   gchar *dir_path)
{
  g_return_if_fail(AGS_IS_FILE_WIDGET(file_widget));
  
  g_object_ref((GObject *) file_widget);
  g_signal_emit(G_OBJECT(file_widget),
		file_widget_signals[CREATE_DIR], 0,
		dir_path);
  g_object_unref((GObject *) file_widget);
}

/**
 * ags_file_widget_new:
 *
 * Creates a new instance of #AgsFileWidget.
 *
 * Returns: the new #AgsFileWidget
 *
 * Since: 6.6.0
 */
AgsFileWidget*
ags_file_widget_new()
{
  AgsFileWidget *file_widget;

  file_widget = (AgsFileWidget *) g_object_new(AGS_TYPE_FILE_WIDGET,
					       NULL);
  
  return(file_widget);
}
