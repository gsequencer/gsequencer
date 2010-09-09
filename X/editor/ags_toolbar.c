#include "ags_toolbar.h"
#include "ags_toolbar_callbacks.h"
#include "ags_toolbar_mode_stock.h"

#include "../ags_menu_bar.h"

#include <gtk/gtkhbox.h>
#include <gtk/gtkbox.h>
#include <gtk/gtkcontainer.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtktogglebutton.h>
#include <gtk/gtkcheckbutton.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkoptionmenu.h>
#include <gtk/gtkimage.h>
#include <gtk/gtkstock.h>

GType ags_toolbar_get_type(void);
void ags_toolbar_class_init(AgsToolbarClass *toolbar);
void ags_toolbar_init(AgsToolbar *toolbar);
void ags_toolbar_connect(AgsToolbar *toolbar);
void ags_toolbar_destroy(GtkObject *object);
void ags_toolbar_show(GtkWidget *widget);

GType
ags_toolbar_get_type(void)
{
  static GType toolbar_type = 0;

  if (!toolbar_type){
    static const GtkTypeInfo toolbar_info = {
      "AgsToolbar\0",
      sizeof(AgsToolbar), /* base_init */
      sizeof(AgsToolbarClass), /* base_finalize */
      (GtkClassInitFunc) ags_toolbar_class_init,
      (GtkObjectInitFunc) ags_toolbar_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    toolbar_type = gtk_type_unique (GTK_TYPE_TOOLBAR, &toolbar_info);
  }

  return (toolbar_type);
}

void
ags_toolbar_class_init(AgsToolbarClass *toolbar)
{
}

void
ags_toolbar_init(AgsToolbar *toolbar)
{
  GtkLabel *label;
  GtkMenu *menu;
  GtkMenuItem *item;

  toolbar->edit = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
						   "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_EDIT, GTK_ICON_SIZE_LARGE_TOOLBAR),
						   "relief\0", GTK_RELIEF_NONE,
						   NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->edit, "edit notes\0", NULL);

  toolbar->clear = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
						    "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_CLEAR, GTK_ICON_SIZE_LARGE_TOOLBAR),
						    "relief\0", GTK_RELIEF_NONE,
						    NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->clear, "clear notes\0", NULL);

  toolbar->select = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
						     "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_SELECT_ALL, GTK_ICON_SIZE_LARGE_TOOLBAR),
						     "relief\0", GTK_RELIEF_NONE,
						     NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->select, "select notes\0", NULL);

  toolbar->copy = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
					     "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_COPY, GTK_ICON_SIZE_LARGE_TOOLBAR),
					     "relief\0", GTK_RELIEF_NONE,
					     NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->copy, "copy notes\0", NULL);

  toolbar->cut = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
					    "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_CUT, GTK_ICON_SIZE_LARGE_TOOLBAR),
					    "relief\0", GTK_RELIEF_NONE,
					    NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->cut, "cut notes\0", NULL);

  toolbar->paste = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
					      "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_PASTE, GTK_ICON_SIZE_LARGE_TOOLBAR),
					      "relief\0", GTK_RELIEF_NONE,
					      NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->paste, "paste notes\0", NULL);

  label = (GtkLabel *) gtk_label_new("zoom\0");
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) label, NULL, NULL);

  toolbar->zoom_history = 6;
  toolbar->zoom = (GtkOptionMenu *) gtk_option_menu_new();
  gtk_option_menu_set_menu(toolbar->zoom, (GtkWidget *) ags_zoom_menu_new());
  gtk_option_menu_set_history(toolbar->zoom, 6);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->zoom, NULL , NULL);

  label = (GtkLabel *) gtk_label_new("tic\0");
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) label, NULL, NULL);

  toolbar->tic_history = 4;
  toolbar->tic = (GtkOptionMenu *) gtk_option_menu_new();
  gtk_option_menu_set_menu(toolbar->tic, (GtkWidget *) ags_tic_menu_new());
  gtk_option_menu_set_history(toolbar->tic, 4);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->tic, NULL, NULL);

  label = (GtkLabel *) gtk_label_new("mode\0");
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) label, NULL, NULL);

  menu = (GtkMenu *) gtk_menu_new();

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(AGS_TOOLBAR_MODE_DEFAULT);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(AGS_TOOLBAR_MODE_LOCK_CHANNELS);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  toolbar->mode = (GtkOptionMenu *) gtk_option_menu_new();
  gtk_option_menu_set_menu(toolbar->mode, (GtkWidget *) menu);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->mode, NULL, NULL);
}

void
ags_toolbar_connect(AgsToolbar *toolbar)
{
  GList *list;

  g_signal_connect((GObject *) toolbar, "destroy\0",
		   G_CALLBACK(ags_toolbar_destroy_callback), (gpointer) toolbar);

  g_signal_connect((GObject *) toolbar, "show\0",
		   G_CALLBACK(ags_toolbar_show_callback), (gpointer) toolbar);

  g_signal_connect((GObject *) toolbar->edit, "clicked\0",
		   G_CALLBACK(ags_toolbar_edit_callback), (gpointer) toolbar);

  g_signal_connect((GObject *) toolbar->clear, "clicked\0",
		   G_CALLBACK(ags_toolbar_clear_callback), (gpointer) toolbar);

  g_signal_connect((GObject *) toolbar->select, "clicked\0",
		   G_CALLBACK(ags_toolbar_select_callback), (gpointer) toolbar);

  g_signal_connect((GObject *) toolbar->copy, "clicked\0",
		   G_CALLBACK(ags_toolbar_copy_callback), (gpointer) toolbar);

  g_signal_connect((GObject *) toolbar->cut, "clicked\0",
		   G_CALLBACK(ags_toolbar_cut_callback), (gpointer) toolbar);

  g_signal_connect((GObject *) toolbar->paste, "clicked\0",
		   G_CALLBACK(ags_toolbar_paste_callback), (gpointer) toolbar);

  g_signal_connect_after((GObject *) toolbar->zoom, "changed\0",
			 G_CALLBACK(ags_toolbar_zoom_callback), (gpointer) toolbar);

  g_signal_connect_after((GObject *) toolbar->tic, "changed\0",
			 G_CALLBACK(ags_toolbar_tic_callback), (gpointer) toolbar);

  list = gtk_container_get_children((GtkContainer *) gtk_option_menu_get_menu(toolbar->mode));
  g_signal_connect((GObject *) list->data, "activate\0",
		   G_CALLBACK(ags_toolbar_mode_default_callback), (gpointer) toolbar);

  list = list->next;
  g_signal_connect((GObject *) list->data, "activate\0",
		   G_CALLBACK(ags_toolbar_mode_group_channels_callback), (gpointer) toolbar);
}

void
ags_toolbar_destroy(GtkObject *object)
{
}

void
ags_toolbar_show(GtkWidget *widget)
{
  /*
  GList *list;

  list = gtk_container_get_children((GtkContainer *) widget);

  while(list != NULL){
    gtk_widget_show_all((GtkWidget *) widget);

    list = list->next;
  }
  */
}

AgsToolbar*
ags_toolbar_new()
{
  AgsToolbar *toolbar;

  toolbar = (AgsToolbar *) g_object_new(AGS_TYPE_TOOLBAR, NULL);

  return(toolbar);
}
