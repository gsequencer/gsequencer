/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#ifndef __AGS_FILE_LAUNCH_H__
#define __AGS_FILE_LAUNCH_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#define AGS_TYPE_FILE_LAUNCH                (ags_file_launch_get_type())
#define AGS_FILE_LAUNCH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FILE_LAUNCH, AgsFileLaunch))
#define AGS_FILE_LAUNCH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FILE_LAUNCH, AgsFileLaunchClass))
#define AGS_IS_FILE_LAUNCH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FILE_LAUNCH))
#define AGS_IS_FILE_LAUNCH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FILE_LAUNCH))
#define AGS_FILE_LAUNCH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FILE_LAUNCH, AgsFileLaunchClass))

typedef struct _AgsFileLaunch AgsFileLaunch;
typedef struct _AgsFileLaunchClass AgsFileLaunchClass;

struct _AgsFileLaunch
{
  GObject object;

  GObject *ags_main;

  xmlNode *node;
  GObject *file;
};

struct _AgsFileLaunchClass
{
  GObjectClass object;

  void (*start)(AgsFileLaunch *file_launch);
};

GType ags_file_launch_get_type(void);

void ags_file_launch_start(AgsFileLaunch *file_launch);

/* */
AgsFileLaunch* ags_file_launch_new();

#endif /*__AGS_FILE_LAUNCH_H__*/
