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

#ifndef __AGS_OUTPUT_H__
#define __AGS_OUTPUT_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_id.h>

#define AGS_TYPE_OUTPUT                (ags_output_get_type())
#define AGS_OUTPUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OUTPUT, AgsOutput))
#define AGS_OUTPUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OUTPUT, AgsOutputClass))
#define AGS_IS_OUTPUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_OUTPUT))
#define AGS_IS_OUTPUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OUTPUT))
#define AGS_OUTPUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_OUTPUT, AgsOutputClass))

typedef struct _AgsOutput AgsOutput;
typedef struct _AgsOutputClass AgsOutputClass;

struct _AgsOutput
{
  AgsChannel channel;
};

struct _AgsOutputClass
{
  AgsChannelClass channel;
};

GType ags_output_get_type();

AgsRecycling* ags_output_find_first_input_recycling(AgsOutput *output);
AgsRecycling* ags_output_find_last_input_recycling(AgsOutput *output);

AgsOutput* ags_output_new(GObject *audio);

#endif /*__AGS_OUTPUT_H__*/
