#ifndef __AGS_MENU_BAR_CALLBACKS_H__
#define __AGS_MENU_BAR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>
#include "ags_menu_bar.h"

gboolean ags_menu_bar_destroy_callback(GtkObject *object, AgsMenuBar *menu_bar);
void ags_menu_bar_show_callback(GtkWidget *widget, AgsMenuBar *menu_bar);

void ags_menu_bar_open_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_save_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_save_as_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_quit_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);

void ags_menu_bar_add_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);

void ags_menu_bar_add_panel_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_add_mixer_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_add_drum_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_add_matrix_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_add_synth_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);
void ags_menu_bar_add_ffplayer_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);

void ags_menu_bar_remove_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);

void ags_menu_bar_about_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar);

#endif /*__AGS_MENU_BAR_CALLBACKS_H__*/
