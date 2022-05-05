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

#ifndef __AGS_GSEQUENCER_APPLICATION_H__
#define __AGS_GSEQUENCER_APPLICATION_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_GSEQUENCER_APPLICATION                (ags_gsequencer_application_get_type())
#define AGS_GSEQUENCER_APPLICATION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GSEQUENCER_APPLICATION, AgsGSequencerApplication))
#define AGS_GSEQUENCER_APPLICATION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_GSEQUENCER_APPLICATION, AgsGSequencerApplicationClass))
#define AGS_IS_GSEQUENCER_APPLICATION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_GSEQUENCER_APPLICATION))
#define AGS_IS_GSEQUENCER_APPLICATION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_GSEQUENCER_APPLICATION))
#define AGS_GSEQUENCER_APPLICATION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_GSEQUENCER_APPLICATION, AgsGSequencerApplicationClass))

typedef struct _AgsGSequencerApplication AgsGSequencerApplication;
typedef struct _AgsGSequencerApplicationClass AgsGSequencerApplicationClass;

struct _AgsGSequencerApplication
{
  GtkApplication application;

  GMenu *menubar;

  GMenu *file_menu;

  GMenu *edit_menu;
  GMenu *add_menu;

  GMenu *midi_menu;

  GMenu *help_menu;
};

struct _AgsGSequencerApplicationClass
{
  GtkApplicationClass application;
};

GType ags_gsequencer_application_get_type();

AgsGSequencerApplication* ags_gsequencer_application_new(gchar *application_id,
							 GApplicationFlags flags);

G_END_DECLS

#endif /*__AGS_GSEQUENCER_APPLICATION_H__*/
