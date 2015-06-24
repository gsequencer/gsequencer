/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_RECALL_DEPENDENCY_H__
#define __AGS_RECALL_DEPENDENCY_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_id.h>

#define AGS_TYPE_RECALL_DEPENDENCY                (ags_recall_dependency_get_type ())
#define AGS_RECALL_DEPENDENCY(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), AGS_TYPE_RECALL_DEPENDENCY, AgsRecallDependency))
#define AGS_RECALL_DEPENDENCY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST ((class), AGS_TYPE_RECALL_DEPENDENCY, AgsRecallDependencyClass))
#define AGS_IS_RECALL_DEPENDENCY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECALL_DEPENDENCY))
#define AGS_IS_RECALL_DEPENDENCY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECALL_DEPENDENCY))
#define AGS_RECALL_DEPENDENCY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECALL_DEPENDENCY, AgsRecallDependencyClass))

typedef struct _AgsRecallDependency AgsRecallDependency;
typedef struct _AgsRecallDependencyClass AgsRecallDependencyClass;

struct _AgsRecallDependency
{
  GObject object;

  GObject *dependency;
};

struct _AgsRecallDependencyClass
{
  GObjectClass object;
};

GType ags_recall_dependency_get_type(void);

GList* ags_recall_dependency_find_dependency(GList *recall_dependencies, GObject *dependency);
GList* ags_recall_dependency_find_dependency_by_provider(GList *recall_dependencies,
							 GObject *provider);

GObject* ags_recall_dependency_resolve(AgsRecallDependency *recall_dependency, AgsRecallID *recall_id);

AgsRecallDependency* ags_recall_dependency_new(GObject *dependency);

#endif /*__AGS_RECALL_DEPENDENCY_H__*/
