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
};

static gpointer ags_file_widget_parent_class = NULL;
static guint file_widget_signals[LAST_SIGNAL];

GType
ags_file_widget_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_file_widget);
  }

  return g_define_type_id__volatile;
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
ags_file_widget_factory_setup(GtkListItemFactory *factory, GtkListItem *list_item,
			      AgsFileWidget *file_widget)
{
  GtkLabel *label;

  label = (GtkLabel *) gtk_label_new("");

  gtk_label_set_xalign(label,
		       0.0);
  gtk_widget_set_halign(label,
			GTK_ALIGN_FILL);
  
  gtk_list_item_set_child(list_item,
			  (GtkWidget *) label);
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
  
  label = gtk_list_item_get_child(list_item);

  string_object = GTK_STRING_OBJECT(gtk_list_item_get_item(list_item));
  primary_key = gtk_string_object_get_string(string_object);

  column_name = NULL;
  value = NULL;

  if((!strncmp(primary_key, "..", 3) == FALSE) &&
     (!strncmp(primary_key, ".", 2) == FALSE)){
    filename = g_strdup_printf("%s/%s",
			       file_widget->current_path,
			       primary_key);
  
    retval = stat(filename,
		  &sb);
  
    if(factory == file_widget->filename_factory[0]){
      value = g_strdup_printf("%s",
			      primary_key);
    }else if(factory == file_widget->filename_factory[1]){
      value = g_strdup_printf("%jd",
			      sb.st_size);
    }else if(factory == file_widget->filename_factory[2]){
      gchar *file_type;
      
      switch (sb.st_mode & S_IFMT) {
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
    }else if(factory == file_widget->filename_factory[3]){
      char outstr[200];

      strftime(outstr, sizeof(outstr), "%a, %d %b %Y %T %z", localtime(&sb.st_mtime));
      
      value = g_strdup(outstr);
    }
  }else{
    value = "";
  }

  gtk_label_set_label(label,
		      value);
}

void
ags_file_widget_init(AgsFileWidget *file_widget)
{
  GtkBox *hbox;
  GtkBox *action_hbox;
  GtkLabel *label;
  
  GtkColumnViewColumn *column;  

  GtkStringList *location_string_list;
  GtkStringList *filename_key_string_list;
  GtkStringList *single_filename_string_list;
  GtkStringList *multi_filename_string_list;

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

  file_widget->home_path = NULL;

#if !defined(AGS_W32API)
  uid = getuid();
  pw = getpwuid(uid);

  file_widget->home_path = g_strdup(pw->pw_dir);
#endif
  
  file_widget->sandbox_path = NULL;
  
  file_widget->default_path = NULL;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(file_widget),
				 GTK_ORIENTATION_VERTICAL);
  
  file_widget->vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
					     6);
  gtk_box_append((GtkBox *) file_widget,
		 (GtkWidget *) file_widget->vbox);  

  /* location */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				6);
  gtk_box_append(file_widget->vbox,
		 (GtkWidget *) hbox);

  label = (GtkLabel *) gtk_label_new(i18n("location"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  file_widget->location_entry = (GtkEntry *) gtk_entry_new();
  gtk_box_append(hbox,
		 (GtkWidget *) file_widget->location_entry);

  /* location - combo */
  location_string_list = gtk_string_list_new(location_strv);
  
  file_widget->location_drop_down = (GtkDropDown *) gtk_drop_down_new(G_LIST_MODEL(location_string_list),
								      NULL);
  gtk_box_append(file_widget->vbox,
		 (GtkWidget *) file_widget->location_drop_down);

  /* left, center and right */
  action_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				       6);
  gtk_box_append(file_widget->vbox,
		 (GtkWidget *) action_hbox);

  /* left */
  file_widget->left_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						  6);
  gtk_box_append(action_hbox,
		 (GtkWidget *) file_widget->left_vbox);
  
  file_widget->recently_used = (GtkButton *) gtk_button_new_from_icon_name("document-open-recent");
  gtk_box_append(file_widget->left_vbox,
		 (GtkWidget *) file_widget->recently_used);

  file_widget->location = g_hash_table_new_full(g_direct_hash,
						g_string_equal,
						g_free,
						g_free);
  
  file_widget->start_here = (GtkButton *) gtk_button_new_from_icon_name("start-here");
  gtk_box_append(file_widget->left_vbox,
		 (GtkWidget *) file_widget->start_here);
  
  file_widget->location_separator = (GtkButton *) gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_append(file_widget->left_vbox,
		 (GtkWidget *) file_widget->location_separator);
  
  file_widget->bookmark = g_hash_table_new_full(g_direct_hash,
						g_string_equal,
						g_free,
						g_free);

  /* center */
  file_widget->center_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						    6);
  gtk_box_append(action_hbox,
		 (GtkWidget *) file_widget->center_vbox);

  filename_key_string_list = gtk_string_list_new(filename_keys_strv);
  file_widget->filename_key_selection = gtk_single_selection_new(G_LIST_MODEL(filename_key_string_list));

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
  
  single_filename_string_list = gtk_string_list_new(filename_strv);
  file_widget->filename_single_selection = gtk_single_selection_new(G_LIST_MODEL(single_filename_string_list));

  multi_filename_string_list = gtk_string_list_new(filename_strv);
  file_widget->filename_multi_selection = gtk_single_selection_new(G_LIST_MODEL(multi_filename_string_list));

  file_widget->filename_scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_widget_set_size_request(file_widget->filename_scrolled_window,
			      800,
			      600);
  gtk_box_append(file_widget->center_vbox,
		 (GtkWidget *) file_widget->filename_scrolled_window);
  
  file_widget->filename_view = (GtkListView *) gtk_column_view_new(file_widget->filename_single_selection);
  gtk_scrolled_window_set_child(file_widget->filename_scrolled_window,
				file_widget->filename_view);
  
  for(i = 0; i < 4; i++){
    column = gtk_column_view_column_new(filename_keys_strv[i],
					file_widget->filename_factory[i]);

    gtk_column_view_append_column(GTK_COLUMN_VIEW(file_widget->filename_view),
				  column);
  }
  
  /* right */
  file_widget->right_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						   6);
  gtk_widget_set_visible((GtkWidget *) file_widget->right_vbox,
			 FALSE);
  gtk_box_append(action_hbox,
		 (GtkWidget *) file_widget->right_vbox);

  file_widget->preview = NULL;
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
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_widget_dispose(GObject *gobject)
{  
  /* call parent */
  G_OBJECT_CLASS(ags_file_widget_parent_class)->dispose(gobject);
}

void
ags_file_widget_finalize(GObject *gobject)
{  
  /* call parent */
  G_OBJECT_CLASS(ags_file_widget_parent_class)->finalize(gobject);
}

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

void
ags_file_widget_set_flags(AgsFileWidget *file_widget,
			  guint flags)
{
  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return;
  }

  file_widget->flags |= flags;
}

void
ags_file_widget_unset_flags(AgsFileWidget *file_widget,
			    guint flags)
{
  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return;
  }

  file_widget->flags &= (~flags);
}

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

void
ags_file_widget_set_file_action(AgsFileWidget *file_widget,
				guint file_action)
{
  if(!AGS_IS_FILE_WIDGET(file_widget)){
    return;
  }

  file_widget->file_action = file_action;
}

void
ags_file_widget_add_location(AgsFileWidget *file_widget,
			     gchar *button_action,
			     gchar *button_text)
{
}

void
ags_file_widget_remove_location(AgsFileWidget *file_widget,
				gchar *button_action)
{
}

void
ags_file_widget_add_bookmark(AgsFileWidget *file_widget,
			     gchar *button_action,
			     gchar *button_text)
{
}

void
ags_file_widget_remove_bookmark(AgsFileWidget *file_widget,
				gchar *button_action)
{
}

void
ags_file_widget_refresh(AgsFileWidget *file_widget)
{
  GDir *current_dir;
  
  GtkStringList *single_filename_string_list;
  GtkStringList *multi_filename_string_list;

  GList *start_filename, *filename;
  
  gchar **filename_strv;

  gchar *current_filename;

  guint count;
  guint i, j;
  
  GError *error;
  
  filename_strv = NULL;

  start_filename = NULL;

  error = NULL;
  current_dir = g_dir_open(file_widget->current_path,
			   0,
			   &error);

  if(error != NULL){
    g_error_free(error);
    
    return;
  }

  do{
    current_filename = g_dir_read_name(current_dir);

    if(current_filename != NULL &&
       ((!strncmp(current_filename, ".", 2) == FALSE) &&
	(!strncmp(current_filename, "..", 3) == FALSE))){
      start_filename = g_list_insert_sorted(start_filename,
					    current_filename,
					    g_strcmp0);
    }
  }while(current_filename != NULL);

  filename = start_filename;

  count = g_list_length(start_filename);
  
  filename_strv = (gchar **) g_malloc((count + 3) * sizeof(gchar *));

  filename_strv[0] = g_strdup(".");
  filename_strv[1] = g_strdup("..");

  for(i = 0, j = 0; i < count; i++){
    if(((gchar *) filename->data)[0] != '.' ||
       ags_file_widget_test_flags(file_widget, AGS_FILE_WIDGET_HIDDEN_FILES_VISIBLE)){
      filename_strv[j + 2] = filename->data;
      j++;
    }
    
    filename = filename->next;
  }
  
  filename_strv[j] = NULL;
  
  single_filename_string_list = gtk_string_list_new(filename_strv);
  gtk_single_selection_set_model(file_widget->filename_single_selection,
				 G_LIST_MODEL(single_filename_string_list));

  multi_filename_string_list = gtk_string_list_new(filename_strv);
  gtk_single_selection_set_model(file_widget->filename_multi_selection,
				 G_LIST_MODEL(multi_filename_string_list));

  g_list_free(start_filename);
  //  g_strfreev(filename_strv);
}

void
ags_file_widget_create_dir(AgsFileWidget *file_widget,
			   gchar *dir_path)
{
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
