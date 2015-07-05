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

#ifndef __AGS_FREE_SELECTION_H__
#define __AGS_FREE_SELECTION_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>
#include <ags/audio/ags_notation.h>

#define AGS_TYPE_FREE_SELECTION                (ags_free_selection_get_type())
#define AGS_FREE_SELECTION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FREE_SELECTION, AgsFreeSelection))
#define AGS_FREE_SELECTION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FREE_SELECTION, AgsFreeSelectionClass))
#define AGS_IS_FREE_SELECTION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_FREE_SELECTION))
#define AGS_IS_FREE_SELECTION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_FREE_SELECTION))
#define AGS_FREE_SELECTION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_FREE_SELECTION, AgsFreeSelectionClass))

typedef struct _AgsFreeSelection AgsFreeSelection;
typedef struct _AgsFreeSelectionClass AgsFreeSelectionClass;

struct _AgsFreeSelection
{
  AgsTask task;

  AgsNotation *notation;
};

struct _AgsFreeSelectionClass
{
  AgsTaskClass task;
};

GType ags_free_selection_get_type();

AgsFreeSelection* ags_free_selection_new(AgsNotation *notation);

#endif /*__AGS_FREE_SELECTION_H__*/
