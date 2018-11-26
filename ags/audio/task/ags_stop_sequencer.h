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

#ifndef __AGS_STOP_SEQUENCER_H__
#define __AGS_STOP_SEQUENCER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#define AGS_TYPE_STOP_SEQUENCER                (ags_stop_sequencer_get_type())
#define AGS_STOP_SEQUENCER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_STOP_SEQUENCER, AgsStopSequencer))
#define AGS_STOP_SEQUENCER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_STOP_SEQUENCER, AgsStopSequencerClass))
#define AGS_IS_STOP_SEQUENCER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_STOP_SEQUENCER))
#define AGS_IS_STOP_SEQUENCER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_STOP_SEQUENCER))
#define AGS_STOP_SEQUENCER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_STOP_SEQUENCER, AgsStopSequencerClass))

typedef struct _AgsStopSequencer AgsStopSequencer;
typedef struct _AgsStopSequencerClass AgsStopSequencerClass;

struct _AgsStopSequencer
{
  AgsTask task;

  AgsApplicationContext *application_context;
};

struct _AgsStopSequencerClass
{
  AgsTaskClass task;
};

GType ags_stop_sequencer_get_type();

AgsStopSequencer* ags_stop_sequencer_new(AgsApplicationContext *application_context);

#endif /*__AGS_STOP_SEQUENCER_H__*/
