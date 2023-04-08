/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_UI_ENUMS_H__
#define __AGS_UI_ENUMS_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#define AGS_TYPE_UI_RECALL_FLAGS       (ags_ui_recall_flags_get_type())

G_BEGIN_DECLS

/**
 * AgsUiRecallFlags: 
 * @AGS_UI_RECALL_MAPPED: recall mapped
 * @AGS_UI_RECALL_PREMAPPED: recall premapped
 * 
 * Enum values to specify particular ability. Mark an object to be
 * able to handle matching scope.
 */
typedef enum{
  AGS_UI_RECALL_MAPPED              = 1,
  AGS_UI_RECALL_PREMAPPED           = 1 <<  1,
}AgsUiRecallFlags;

G_END_DECLS

#endif /*__AGS_UI_ENUMS_H__*/
