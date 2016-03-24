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

#ifndef __AGS_TURTLE_MANAGER_H__
#define __AGS_TURTLE_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_TURTLE_MANAGER                (ags_turtle_manager_get_type())
#define AGS_TURTLE_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TURTLE_MANAGER, AgsTurtleManager))
#define AGS_TURTLE_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TURTLE_MANAGER, AgsTurtleManagerClass))
#define AGS_IS_TURTLE_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TURTLE_MANAGER))
#define AGS_IS_TURTLE_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TURTLE_MANAGER))
#define AGS_TURTLE_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_TURTLE_MANAGER, AgsTurtleManagerClass))

typedef struct _AgsTurtleManager AgsTurtleManager;
typedef struct _AgsTurtleManagerClass AgsTurtleManagerClass;

struct _AgsTurtleManager
{
  GObject object;

  GList *turtle;
};

struct _AgsTurtleManagerClass
{
  GObjectClass object;
};

GType ags_turtle_manager_get_type(void);

GObject* ags_turtle_manager_find(AgsTurtleManager *turtle_manager,
				 gchar *filename);
void ags_turtle_manager_add(AgsTurtleManager *turtle_manager,
			    GObject *turtle);

/*  */
AgsTurtleManager* ags_turtle_manager_get_instance();

AgsTurtleManager* ags_turtle_manager_new();

#endif /*__AGS_TURTLE_MANAGER_H__*/
