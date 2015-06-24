/* This file is part of GSequencer.
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

#ifndef __AGS_ADD_POINT_TO_SELECTION_H__
#define __AGS_ADD_POINT_TO_SELECTION_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/audio/ags_notation.h>

#define AGS_TYPE_ADD_POINT_TO_SELECTION                (ags_add_point_to_selection_get_type())
#define AGS_ADD_POINT_TO_SELECTION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ADD_POINT_TO_SELECTION, AgsAddPointToSelection))
#define AGS_ADD_POINT_TO_SELECTION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ADD_POINT_TO_SELECTION, AgsAddPointToSelectionClass))
#define AGS_IS_ADD_POINT_TO_SELECTION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ADD_POINT_TO_SELECTION))
#define AGS_IS_ADD_POINT_TO_SELECTION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ADD_POINT_TO_SELECTION))
#define AGS_ADD_POINT_TO_SELECTION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ADD_POINT_TO_SELECTION, AgsAddPointToSelectionClass))

typedef struct _AgsAddPointToSelection AgsAddPointToSelection;
typedef struct _AgsAddPointToSelectionClass AgsAddPointToSelectionClass;

struct _AgsAddPointToSelection
{
  AgsTask task;

  AgsNotation *notation;

  guint x;
  guint y;

  gboolean replace_current_selection;
};

struct _AgsAddPointToSelectionClass
{
  AgsTaskClass task;
};

GType ags_add_point_to_selection_get_type();

AgsAddPointToSelection* ags_add_point_to_selection_new(AgsNotation *notation,
						       guint x, guint y,
						       gboolean replace_current_selection);

#endif /*__AGS_ADD_POINT_TO_SELECTION_H__*/
