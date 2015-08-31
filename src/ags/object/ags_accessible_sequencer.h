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

#ifndef __AGS_ACCESSIBLE_SEQUENCER_H__
#define __AGS_ACCESSIBLE_SEQUENCER_H__

#include <glib-object.h>

#define AGS_TYPE_ACCESSIBLE_SEQUENCER                    (ags_accessible_sequencer_get_type())
#define AGS_ACCESSIBLE_SEQUENCER(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ACCESSIBLE_SEQUENCER, AgsAccessibleSequencer))
#define AGS_ACCESSIBLE_SEQUENCER_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_ACCESSIBLE_SEQUENCER, AgsAccessibleSequencerInterface))
#define AGS_IS_ACCESSIBLE_SEQUENCER(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ACCESSIBLE_SEQUENCER))
#define AGS_IS_ACCESSIBLE_SEQUENCER_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_ACCESSIBLE_SEQUENCER))
#define AGS_ACCESSIBLE_SEQUENCER_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_ACCESSIBLE_SEQUENCER, AgsAccessibleSequencerInterface))

typedef void AgsAccessibleSequencer;
typedef struct _AgsAccessibleSequencerInterface AgsAccessibleSequencerInterface;

struct _AgsAccessibleSequencerInterface
{
  AgsGenericAccessible generic_accessible;

  gboolean (*edit_pattern)(AgsAccessibleSequencer *accessible_sequencer);
  gboolean (*set_sequence_length)(AgsAccessibleSequencer *accessible_sequencer);
};

GType ags_accessible_sequencer_get_type();

#endif /*__AGS_ACCESSIBLE_SEQUENCER_H__*/
