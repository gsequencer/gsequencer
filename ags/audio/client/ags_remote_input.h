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

#ifndef __AGS_REMOTE_INPUT_H__
#define __AGS_REMOTE_INPUT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/client/ags_remote_channel.h>

#define AGS_TYPE_REMOTE_INPUT                (ags_remote_input_get_type())
#define AGS_REMOTE_INPUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_REMOTE_INPUT, AgsRemoteInput))
#define AGS_REMOTE_INPUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_REMOTE_INPUT, AgsRemoteInput))
#define AGS_IS_REMOTE_INPUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_REMOTE_INPUT))
#define AGS_IS_REMOTE_INPUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_REMOTE_INPUT))
#define AGS_REMOTE_INPUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_REMOTE_INPUT, AgsRemoteInputClass))

typedef struct _AgsRemoteInput AgsRemoteInput;
typedef struct _AgsRemoteInputClass AgsRemoteInputClass;

struct _AgsRemoteInput
{
  AgsRemoteChannel remote_channel;
};

struct _AgsRemoteInputClass
{
  AgsRemoteChannelClass remote_channel;
};

GType ags_remote_input_get_type();

AgsRemoteInput* ags_remote_input_new(GObject *application_context);

#endif /*__AGS_REMOTE_INPUT_H__*/
