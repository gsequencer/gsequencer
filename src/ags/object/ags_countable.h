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

#ifndef __AGS_COUNTABLE_H__
#define __AGS_COUNTABLE_H__

#include <glib-object.h>

#define AGS_TYPE_COUNTABLE                    (ags_countable_get_type())
#define AGS_COUNTABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COUNTABLE, AgsCountable))
#define AGS_COUNTABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_COUNTABLE, AgsCountableInterface))
#define AGS_IS_COUNTABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_COUNTABLE))
#define AGS_IS_COUNTABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_COUNTABLE))
#define AGS_COUNTABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_COUNTABLE, AgsCountableInterface))

typedef void AgsCountable;
typedef struct _AgsCountableInterface AgsCountableInterface;

struct _AgsCountableInterface
{
  GTypeInterface interface;

  guint (*get_notation_counter)(AgsCountable *countable);
  guint (*get_sequencer_counter)(AgsCountable *countable);
};

GType ags_countable_get_type();

guint ags_countable_get_notation_counter(AgsCountable *countable);
guint ags_countable_get_sequencer_counter(AgsCountable *countable);

#endif /*__AGS_COUNTABLE_H__*/
