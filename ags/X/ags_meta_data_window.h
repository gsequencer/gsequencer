/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2021 Daniel Maksymow
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


#ifndef AGS_META_DATA_WINDOW_H
#define AGS_META_DATA_WINDOW_H

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_META_DATA_WINDOW (ags_meta_data_window_get_type())

G_DECLARE_FINAL_TYPE(AgsMetaDataWindow, ags_meta_data_window, AGS,
                     META_DATA_WINDOW, GtkWindow)

AgsMetaDataWindow* ags_meta_data_window_new();

G_END_DECLS

#endif
