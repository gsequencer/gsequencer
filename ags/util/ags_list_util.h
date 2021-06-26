/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_LIST_UTIL_H__
#define __AGS_LIST_UTIL_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AGS_TYPE_LIST_UTIL         (ags_list_util_get_type())

typedef struct _AgsListUtil AgsListUtil;

struct _AgsListUtil
{
  //empty
};

GType ags_list_util_get_type(void);

GList* ags_list_util_find_type(GList *list,
			       GType gtype);

G_END_DECLS

#endif /*__AGS_LIST_UTIL_H__*/
