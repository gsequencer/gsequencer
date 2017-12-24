/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/util/ags_destroy_util.h>

/**
 * SECTION:ags_destroy_util
 * @short_description: destroy util
 * @title: AgsDestroyUtil
 * @section_id:
 * @include: ags/util/ags_destroy_util.h
 * 
 * Destroying items utility.
 */

/**
 * ags_destroy_util_dispose_and_unref:
 * @gobject: the #GObject to destroy
 * 
 * Run dispose and unref @gobject.
 * 
 * Since: 1.0.0
 */
void
ags_destroy_util_dispose_and_unref(GObject *gobject)
{
  if(gobject == NULL ||
     !G_IS_OBJECT(gobject)){
    return;
  }

  g_object_run_dispose(gobject);
  g_object_unref(gobject);
}
