/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#ifndef __AGS_UUID__
#define __AGS_UUID__

#include <glib.h>
#include <glib-object.h>

#include <uuid/uuid.h>

#define AGS_TYPE_UUID                (ags_uuid_get_type())

#define AGS_UUID_DEFAULT_LENGTH (16)
#define AGS_UUID_STRING_DEFAULT_LENGTH (36)

typedef uuid_t AgsUUID;

GType ags_uuid_get_type(void);

AgsUUID* ags_uuid_alloc();

gpointer ags_uuid_copy(AgsUUID *ptr);
void ags_uuid_free(AgsUUID *ptr);

void ags_uuid_generate(AgsUUID *ptr);

gint ags_uuid_compare(AgsUUID *a,
		      AgsUUID *b);

gchar* ags_uuid_to_string(AgsUUID *ptr);
AgsUUID* ags_uuid_from_string(gchar *str);

#endif /*__AGS_UUID__*/
