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

#include <ags/object/ags_seekable.h>

void ags_seekable_base_init(AgsSeekableInterface *interface);

/**
 * SECTION:ags_seekable
 * @short_description: unique access to recalls
 * @title: AgsSeekable
 * @section_id:
 * @include: ags/object/ags_seekable.h
 *
 * The #AgsSeekable interface gives you the #AgsSeekable::seek() signal
 * what notifies about changed offset of pattern or notation.
 */

GType
ags_seekable_get_type()
{
  static GType ags_type_seekable = 0;

  if(!ags_type_seekable){
    static const GTypeInfo ags_seekable_info = {
      sizeof(AgsSeekableInterface),
      (GBaseInitFunc) ags_seekable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_seekable = g_type_register_static(G_TYPE_INTERFACE,
					       "AgsSeekable\0", &ags_seekable_info,
					       0);
  }

  return(ags_type_seekable);
}

void
ags_seekable_base_init(AgsSeekableInterface *interface)
{
  /* empty */
}

/**
 * ags_seekable_seek:
 * @seekable: the #AgsSeekable interface
 * @steps: the amount of seeking
 * @forward: the direction, %TRUE for moving ahead
 *
 * Seek relatively.
 *
 * Since: 0.5.0
 */
void
ags_seekable_seek(AgsSeekable *seekable, guint steps, gboolean forward)
{
  AgsSeekableInterface *seekable_interface;

  g_return_if_fail(AGS_IS_SEEKABLE(seekable));
  seekable_interface = AGS_SEEKABLE_GET_INTERFACE(seekable);
  g_return_if_fail(seekable_interface->seek);
  seekable_interface->seek(seekable, steps, forward);
}
