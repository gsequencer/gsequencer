/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_NAVIGATION_H__
#define __AGS_NAVIGATION_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h> 
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_NAVIGATION                (ags_navigation_get_type())
#define AGS_NAVIGATION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NAVIGATION, AgsNavigation))
#define AGS_NAVIGATION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NAVIGATION, AgsNavigationClass))
#define AGS_IS_NAVIGATION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_NAVIGATION))
#define AGS_IS_NAVIGATION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_NAVIGATION))
#define AGS_NAVIGATION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_NAVIGATION, AgsNavigationClass))

#define AGS_NAVIGATION_DEFAULT_BPM (120.0)
#define AGS_NAVIGATION_DEFAULT_TACT_STEP (1.0)
#define AGS_NAVIGATION_SEEK_STEPS (1.0)
#define AGS_NAVIGATION_REWIND_STEPS (4.0)

#define AGS_NAVIGATION_DURATION_ZERO "0000:00.000"
#define AGS_NAVIGATION_MAX_POSITION_TACT (16 * 1200)
#define AGS_NAVIGATION_MAX_POSITION_TICS (16 * 16 * 1200)

typedef struct _AgsNavigation AgsNavigation;
typedef struct _AgsNavigationClass AgsNavigationClass;

typedef enum{
  AGS_NAVIGATION_BLOCK_TACT   = 1,
  AGS_NAVIGATION_BLOCK_PLAY   = 1 << 1,
  AGS_NAVIGATION_BLOCK_TIC    = 1 << 2,
  AGS_NAVIGATION_BLOCK_BPM    = 1 << 3,
}AgsNavigationFlags;

struct _AgsNavigation
{
  GtkBox box;

  guint flags;
  guint connectable_flags;

  GObject *soundcard;
  gdouble start_tact;
  gdouble note_offset;

  GtkToggleButton *expander;

  GtkSpinButton *bpm;
  gdouble current_bpm;

  GtkToggleButton *rewind;
  GtkButton *previous;
  GtkToggleButton *play;
  GtkButton *stop;
  GtkButton *next;
  GtkToggleButton *forward;

  GtkCheckButton *loop;

  GtkLabel *position_time;
  GtkSpinButton *position_tact;

  GtkLabel *duration_time;
  GtkSpinButton *duration_tact;

  GtkBox *expansion_box;
  
  GtkSpinButton *loop_left_tact;
  GtkSpinButton *loop_right_tact;

  GtkCheckButton *scroll;
  GtkCheckButton *exclude_sequencer;
};

struct _AgsNavigationClass
{
  GtkBoxClass box;

  void (*change_position)(AgsNavigation *navigation,
			  gdouble tact);
};

GType ags_navigation_get_type(void);

gchar* ags_navigation_tact_to_time_string(gdouble tact,
					  gdouble bpm,
					  gdouble delay_factor);

gchar* ags_navigation_relative_tact_to_time_string(gchar *timestr,
						   gdouble delay,
						   gdouble bpm,
						   gdouble delay_factor);

gchar* ags_navigation_absolute_tact_to_time_string(gdouble tact,
						   gdouble bpm,
						   gdouble delay_factor);

void ags_navigation_update_time_string(double tact,
				       gdouble bpm,
				       gdouble delay_factor,
				       gchar *time_string);

void ags_navigation_set_seeking_sensitive(AgsNavigation *navigation,
					  gboolean enabled);

void ags_navigation_change_position(AgsNavigation *navigation,
				    gdouble tact);

gboolean ags_navigation_duration_time_queue_draw_timeout(GtkWidget *widget);

AgsNavigation* ags_navigation_new();

G_END_DECLS

#endif /*__AGS_NAVIGATION_H__*/
