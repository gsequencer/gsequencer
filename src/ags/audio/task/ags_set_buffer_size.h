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

#ifndef __AGS_SET_BUFFER_SIZE_H__
#define __AGS_SET_BUFFER_SIZE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>

#define AGS_TYPE_SET_BUFFER_SIZE                (ags_set_buffer_size_get_type())
#define AGS_SET_BUFFER_SIZE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SET_BUFFER_SIZE, AgsSetBufferSize))
#define AGS_SET_BUFFER_SIZE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SET_BUFFER_SIZE, AgsSetBufferSizeClass))
#define AGS_IS_SET_BUFFER_SIZE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SET_BUFFER_SIZE))
#define AGS_IS_SET_BUFFER_SIZE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SET_BUFFER_SIZE))
#define AGS_SET_BUFFER_SIZE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SET_BUFFER_SIZE, AgsSetBufferSizeClass))

typedef struct _AgsSetBufferSize AgsSetBufferSize;
typedef struct _AgsSetBufferSizeClass AgsSetBufferSizeClass;

struct _AgsSetBufferSize
{
  AgsTask task;
};

struct _AgsSetBufferSizeClass
{
  AgsTaskClass task;
};

GType ags_set_buffer_size_get_type();

AgsSetBufferSize* ags_set_buffer_size_new();

#endif /*__AGS_SET_BUFFER_SIZE_H__*/
