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

#ifndef __AGS_APPLY_SEQUENCER_LENGTH_H__
#define __AGS_APPLY_SEQUENCER_LENGTH_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>

#define AGS_TYPE_APPLY_SEQUENCER_LENGTH                (ags_apply_sequencer_length_get_type())
#define AGS_APPLY_SEQUENCER_LENGTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_APPLY_SEQUENCER_LENGTH, AgsApplySequencerLength))
#define AGS_APPLY_SEQUENCER_LENGTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_APPLY_SEQUENCER_LENGTH, AgsApplySequencerLengthClass))
#define AGS_IS_APPLY_SEQUENCER_LENGTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_APPLY_SEQUENCER_LENGTH))
#define AGS_IS_APPLY_SEQUENCER_LENGTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_APPLY_SEQUENCER_LENGTH))
#define AGS_APPLY_SEQUENCER_LENGTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_APPLY_SEQUENCER_LENGTH, AgsApplySequencerLengthClass))

typedef struct _AgsApplySequencerLength AgsApplySequencerLength;
typedef struct _AgsApplySequencerLengthClass AgsApplySequencerLengthClass;

struct _AgsApplySequencerLength
{
  AgsTask task;

  GObject *gobject;

  guint length;
};

struct _AgsApplySequencerLengthClass
{
  AgsTaskClass task;
};

GType ags_apply_sequencer_length_get_type();

AgsApplySequencerLength* ags_apply_sequencer_length_new(GObject *gobject,
							guint length);

#endif /*__AGS_APPLY_SEQUENCER_LENGTH_H__*/
