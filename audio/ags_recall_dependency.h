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

#ifndef __AGS_RECALL_DEPENDENCY_H__
#define __AGS_RECALL_DEPENDENCY_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_RECALL_DEPENDENCY                (ags_recall_dependency_get_type ())
#define AGS_RECALL_DEPENDENCY(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), AGS_TYPE_RECALL_DEPENDENCY, AgsRecallDependency))
#define AGS_RECALL_DEPENDENCY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST ((class), AGS_TYPE_RECALL_DEPENDENCY, AgsRecallDependencyClass))
#define AGS_IS_RECALL_DEPENDENCY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECALL_DEPENDENCY))
#define AGS_IS_RECALL_DEPENDENCY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECALL_DEPENDENCY))
#define AGS_RECALL_DEPENDENCY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECALL_DEPENDENCY, AgsRecallDependencyClass))

typedef struct _AgsRecallDependency AgsRecallDependency;
typedef struct _AgsRecallDependencyClass AgsRecallDependencyClass;

typedef enum{
  AGS_RECALL_DEPENDENCY_IS_IN_RECALL_LIST,
  AGS_RECALL_DEPENDENCY_IS_IN_AUDIO,
  AGS_RECALL_DEPENDENCY_IS_IN_OUTPUT,
  AGS_RECALL_DEPENDENCY_IS_IN_INPUT,
}AgsRecallDependencyFlags;

struct _AgsRecallDependency
{
  GObject object;

  guint flags;

  GType recall_type;
  guint template_id;
};

struct _AgsRecallDependencyClass
{
  GObjectClass object;
};

GType ags_recall_dependency_get_type(void);

AgsRecallDependency* ags_recall_dependency_new();

#endif /*__AGS_RECALL_DEPENDENCY_H__*/
