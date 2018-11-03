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

#include <ags/object/ags_seekable.h>

#include <ags/object/ags_marshal.h>

void ags_seekable_class_init(AgsSeekableInterface *interface);

/**
 * SECTION:ags_seekable
 * @short_description: unique access to seekable classes
 * @title: AgsSeekable
 * @section_id: AgsSeekable
 * @include: ags/object/ags_seekable.h
 *
 * The #AgsSeekable interface gives you the #AgsSeekable::seek() signal
 * what notifies about changed offset of pattern or notation.
 */

enum {
  SEEK,
  LAST_SIGNAL,
};

static guint seekable_signals[LAST_SIGNAL];

GType
ags_seekable_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_seekable = 0;

    ags_type_seekable = g_type_register_static_simple(G_TYPE_INTERFACE,
						      "AgsSeekable",
						      sizeof (AgsSeekableInterface),
						      (GClassInitFunc) ags_seekable_class_init,
						      0, NULL, 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_seekable);
  }

  return g_define_type_id__volatile;
}

void
ags_seekable_class_init(AgsSeekableInterface *interface)
{
  /**
   * AgsSeekable::seek:
   * @seekable: the #GObject sub-type implementing #AgsSeekable
   * @offset: the offset
   * @whence: the direction, see #AgsSeekType-enum
   *
   * The ::seek signal notifies about changed position
   * of sequencer.
   * 
   * Since: 2.0.0
   */
  seekable_signals[SEEK] =
    g_signal_new("seek",
		 G_TYPE_FROM_INTERFACE(interface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSeekableInterface, seek),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__INT64_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_INT64,
		 G_TYPE_UINT);
}

void
ags_seekable_base_init(AgsSeekableInterface *interface)
{
  /* empty */
}

/**
 * ags_seekable_seek:
 * @seekable: the #AgsSeekable interface
 * @offset: the offset
 * @whence: the direction, see #AgsSeekType-enum
 *
 * Seek.
 *
 * Since: 2.0.0
 */
void
ags_seekable_seek(AgsSeekable *seekable, gint64 offset, guint whence)
{
  g_signal_emit(seekable, seekable_signals[SEEK], 0,
		offset, whence);
}
