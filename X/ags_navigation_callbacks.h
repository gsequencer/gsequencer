#ifndef __AGS_NAVIGATION_CALLBACKS_H__
#define __AGS_NAVIGATION_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_navigation.h>

void ags_navigation_parent_set_callback(GtkWidget *widget, GtkObject *old_parent,
					gpointer data);
gboolean ags_navigation_destroy_callback(GtkObject *object,
					 gpointer data);
void ags_navigation_show_callback(GtkWidget *widget,
				  gpointer data);

void ags_navigation_expander_callback(GtkWidget *widget,
				      AgsNavigation *navigation);

void ags_navigation_bpm_callback(GtkWidget *widget,
				 AgsNavigation *navigation);

void ags_navigation_rewind_callback(GtkWidget *widget,
				    AgsNavigation *navigation);
void ags_navigation_prev_callback(GtkWidget *widget,
				  AgsNavigation *navigation);
void ags_navigation_play_callback(GtkWidget *widget,
				  AgsNavigation *navigation);
void ags_navigation_stop_callback(GtkWidget *widget,
				  AgsNavigation *navigation);
void ags_navigation_next_callback(GtkWidget *widget,
				  AgsNavigation *navigation);
void ags_navigation_forward_callback(GtkWidget *widget,
				     AgsNavigation *navigation);

void ags_navigation_loop_callback(GtkWidget *widget,
				  AgsNavigation *navigation);

void ags_navigation_position_sec_callback(GtkWidget *widget,
					  AgsNavigation *navigation);
void ags_navigation_position_min_callback(GtkWidget *widget,
					  AgsNavigation *navigation);

void ags_navigation_duration_sec_callback(GtkWidget *widget,
					  AgsNavigation *navigation);
void ags_navigation_duration_min_callback(GtkWidget *widget,
					  AgsNavigation *navigation);

void ags_navigation_loop_left_sec_callback(GtkWidget *widget,
					   AgsNavigation *navigation);
void ags_navigation_loop_left_min_callback(GtkWidget *widget,
					   AgsNavigation *navigation);

void ags_navigation_loop_right_sec_callback(GtkWidget *widget,
					    AgsNavigation *navigation);
void ags_navigation_loop_right_min_callback(GtkWidget *widget,
					    AgsNavigation *navigation);

void ags_navigation_raster_callback(GtkWidget *widget,
				    AgsNavigation *navigation);

#endif /*__AGS_NAVIGATION_CALLBACKS_H__*/
