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

#ifndef __AGS_WINDOW_H__
#define __AGS_WINDOW_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_devout.h>

#include <ags/X/ags_menu_bar.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_editor.h>
#include <ags/X/ags_navigation.h>

#define AGS_TYPE_WINDOW                (ags_window_get_type())
#define AGS_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WINDOW, AgsWindow))
#define AGS_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WINDOW, AgsWindowClass))
#define AGS_IS_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_WINDOW))
#define AGS_IS_WINDOW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_WINDOW))
#define AGS_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_WINDOW, AgsWindowClass))

typedef struct _AgsWindow AgsWindow;
typedef struct _AgsWindowClass AgsWindowClass;
typedef struct _AgsMachineCounter AgsMachineCounter;

struct _AgsWindow
{
  GtkWindow window;

  AgsDevout *devout;

  char *name;

  AgsMenuBar *menu_bar;

  GtkVPaned *paned;

  GtkVBox *machines;
  AgsMachineCounter *counter;
  AgsMachine *selected;

  AgsEditor *editor;
  AgsNavigation *navigation;
};

struct _AgsWindowClass
{
  GtkWindowClass window;
};

struct _AgsMachineCounter
{
  guint everything;

  guint panel;
  guint mixer;
  guint drum;
  guint matrix;
  guint synth;
  guint ffplayer;
};

GType ags_window_get_type(void);

AgsWindow* ags_window_new();

#endif /*__AGS_WINDOW_H__*/
