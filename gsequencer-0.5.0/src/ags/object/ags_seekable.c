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

#include <ags/object/ags_seekable.h>

#include <ags/object/ags_marshal.h>

void ags_seekable_class_init(AgsSeekableInterface *interface);

GType
ags_seekable_get_type()
{
  static GType seekable_type = 0;

  if(!seekable_type){
    seekable_type = g_type_register_static_simple (G_TYPE_INTERFACE,
						   "AgsSeekable\0",
						   sizeof (AgsSeekableInterface),
						   (GClassInitFunc) ags_seekable_class_init,
						   0, NULL, 0);
  }
  
  return seekable_type;
}

void
ags_seekable_class_init(AgsSeekableInterface *interface)
{
  /**
   * AgsSeekable::seek:
   * @seekable: the object
   * @steps: relative seeking
   * @forward: direction to seek, if %TRUE seek forward
   *
   * The ::seek signal notifies about changed position
   * of sequencer.
   */
  g_signal_new("seek\0",
	       G_TYPE_FROM_INTERFACE(interface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsSeekableInterface, seek),
	       NULL, NULL,
	       g_cclosure_user_marshal_VOID__UINT_BOOLEAN,
	       G_TYPE_NONE, 2,
	       G_TYPE_UINT,
	       G_TYPE_BOOLEAN);
}

void
ags_seekable_base_init(AgsSeekableInterface *interface)
{
  /* empty */
}

void
ags_seekable_seek(AgsSeekable *seekable, guint steps, gboolean forward)
{
  AgsSeekableInterface *seekable_interface;

  g_return_if_fail(AGS_IS_SEEKABLE(seekable));
  seekable_interface = AGS_SEEKABLE_GET_INTERFACE(seekable);
  g_return_if_fail(seekable_interface->seek);
  seekable_interface->seek(seekable, steps, forward);
}
