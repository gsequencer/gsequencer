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

#ifndef __AGS_CLEAR_BUFFER_H__
#define __AGS_CLEAR_BUFFER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#define AGS_TYPE_CLEAR_BUFFER                (ags_clear_buffer_get_type())
#define AGS_CLEAR_BUFFER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CLEAR_BUFFER, AgsClearBuffer))
#define AGS_CLEAR_BUFFER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CLEAR_BUFFER, AgsClearBufferClass))
#define AGS_IS_CLEAR_BUFFER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CLEAR_BUFFER))
#define AGS_IS_CLEAR_BUFFER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CLEAR_BUFFER))
#define AGS_CLEAR_BUFFER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CLEAR_BUFFER, AgsClearBufferClass))

typedef struct _AgsClearBuffer AgsClearBuffer;
typedef struct _AgsClearBufferClass AgsClearBufferClass;

struct _AgsClearBuffer
{
  AgsTask task;

  GObject *device;
};

struct _AgsClearBufferClass
{
  AgsTaskClass task;
};

GType ags_clear_buffer_get_type();

AgsClearBuffer* ags_clear_buffer_new(GObject *device);

#endif /*__AGS_CLEAR_BUFFER_H__*/
