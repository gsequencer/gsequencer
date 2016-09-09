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

#ifndef __AGS_REMOTE_OUTPUT_H__
#define __AGS_REMOTE_OUTPUT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/client/ags_remote_channel.h>

#define AGS_TYPE_REMOTE_OUTPUT                (ags_remote_output_get_type())
#define AGS_REMOTE_OUTPUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_REMOTE_OUTPUT, AgsRemoteOutput))
#define AGS_REMOTE_OUTPUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_REMOTE_OUTPUT, AgsRemoteOutput))
#define AGS_IS_REMOTE_OUTPUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_REMOTE_OUTPUT))
#define AGS_IS_REMOTE_OUTPUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_REMOTE_OUTPUT))
#define AGS_REMOTE_OUTPUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_REMOTE_OUTPUT, AgsRemoteOutputClass))

typedef struct _AgsRemoteOutput AgsRemoteOutput;
typedef struct _AgsRemoteOutputClass AgsRemoteOutputClass;

struct _AgsRemoteOutput
{
  AgsRemoteChannel remote_channel;
};

struct _AgsRemoteOutputClass
{
  AgsRemoteChannelClass remote_channel;
};

GType ags_remote_output_get_type();

AgsRemoteOutput* ags_remote_output_new(GObject *application_context);

#endif /*__AGS_REMOTE_OUTPUT_H__*/
