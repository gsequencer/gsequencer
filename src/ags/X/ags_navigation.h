/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_NAVIGATION_H__
#define __AGS_NAVIGATION_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_devout.h>

#define AGS_TYPE_NAVIGATION                (ags_navigation_get_type())
#define AGS_NAVIGATION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NAVIGATION, AgsNavigation))
#define AGS_NAVIGATION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NAVIGATION, AgsNavigationClass))
#define AGS_IS_NAVIGATION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_NAVIGATION))
#define AGS_IS_NAVIGATION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_NAVIGATION))
#define AGS_NAVIGATION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_NAVIGATION, AgsNavigationClass))

#define AGS_NAVIGATION_SEEK_STEPS (1.0)
#define AGS_NAVIGATION_REWIND_STEPS (4.0)

typedef struct _AgsNavigation AgsNavigation;
typedef struct _AgsNavigationClass AgsNavigationClass;

struct _AgsNavigation
{
  GtkVBox vbox;

  AgsDevout *devout;

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

  GtkSpinButton *loop_left_tact;
  GtkSpinButton *loop_right_tact;
};

struct _AgsNavigationClass
{
  GtkVBoxClass vbox;
};

GType ags_navigation_get_type(void);

gchar* ags_navigation_tact_to_time_string(gdouble tact);

AgsNavigation* ags_navigation_new();

#endif /*__AGS_NAVIGATION_H__*/
