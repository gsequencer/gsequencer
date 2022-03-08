/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_DESK_FILE_CHOOSER_H__
#define __AGS_DESK_FILE_CHOOSER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_DESK_FILE_CHOOSER                (ags_desk_file_chooser_get_type())
#define AGS_DESK_FILE_CHOOSER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DESK_FILE_CHOOSER, AgsDeskFileChooser))
#define AGS_DESK_FILE_CHOOSER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DESK_FILE_CHOOSER, AgsDeskFileChooserClass))
#define AGS_IS_DESK_FILE_CHOOSER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DESK_FILE_CHOOSER))
#define AGS_IS_DESK_FILE_CHOOSER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_DESK_FILE_CHOOSER))
#define AGS_DESK_FILE_CHOOSER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_DESK_FILE_CHOOSER, AgsDeskFileChooserClass))

typedef struct _AgsDeskFileChooser AgsDeskFileChooser;
typedef struct _AgsDeskFileChooserClass AgsDeskFileChooserClass;

struct _AgsDeskFileChooser
{
  GtkGrid grid;
  
  GtkFileChooserWidget *file_chooser;
};

struct _AgsDeskFileChooserClass
{
  GtkGridClass grid;
};

GType ags_desk_file_chooser_get_type(void);

AgsDeskFileChooser* ags_desk_file_chooser_new();

G_END_DECLS

#endif /*__AGS_DESK_FILE_CHOOSER_H__*/
