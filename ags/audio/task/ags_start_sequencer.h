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

#ifndef __AGS_START_SEQUENCER_H__
#define __AGS_START_SEQUENCER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/object/ags_application_context.h>

#define AGS_TYPE_START_SEQUENCER                (ags_start_sequencer_get_type())
#define AGS_START_SEQUENCER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_START_SEQUENCER, AgsStartSequencer))
#define AGS_START_SEQUENCER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_START_SEQUENCER, AgsStartSequencerClass))
#define AGS_IS_START_SEQUENCER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_START_SEQUENCER))
#define AGS_IS_START_SEQUENCER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_START_SEQUENCER))
#define AGS_START_SEQUENCER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_START_SEQUENCER, AgsStartSequencerClass))

typedef struct _AgsStartSequencer AgsStartSequencer;
typedef struct _AgsStartSequencerClass AgsStartSequencerClass;

struct _AgsStartSequencer
{
  AgsTask task;

  AgsApplicationContext *application_context;
};

struct _AgsStartSequencerClass
{
  AgsTaskClass task;
};

GType ags_start_sequencer_get_type();

AgsStartSequencer* ags_start_sequencer_new(AgsApplicationContext *application_context);

#endif /*__AGS_START_SEQUENCER_H__*/

