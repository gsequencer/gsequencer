/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_BUFFER_H__
#define __AGS_BUFFER_H__

#include <glib.h>
#include <glib-object.h>

#include <alsa/asoundlib.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_BUFFER                (ags_buffer_get_type())
#define AGS_BUFFER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_BUFFER, AgsBuffer))
#define AGS_BUFFER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_BUFFER, AgsBufferClass))
#define AGS_IS_BUFFER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_BUFFER))
#define AGS_IS_BUFFER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_BUFFER))
#define AGS_BUFFER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_BUFFER, AgsBufferClass))

#define AGS_BUFFER_GET_OBJ_MUTEX(obj) (&(((AgsBuffer *) obj)->obj_mutex))

#define AGS_BUFFER_DEFAULT_TICKS_PER_QUARTER_BUFFER (16.0)

typedef struct _AgsBuffer AgsBuffer;
typedef struct _AgsBufferClass AgsBufferClass;

/**
 * AgsBufferFlags:
 * @AGS_BUFFER_IS_SELECTED: is selected
 *
 * Enum values to control the behavior or indicate internal state of #AgsBuffer by
 * enable/disable as flags.
 */
typedef enum{
  AGS_BUFFER_IS_SELECTED     = 1,
}AgsBufferFlags;

struct _AgsBuffer
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  guint64 x;
  
  guint64 selection_x0;
  guint64 selection_x1;

  guint samplerate;
  guint buffer_size;
  AgsSoundcardFormat format;
  
  void *data;
};

struct _AgsBufferClass
{
  GObjectClass gobject;
};

GType ags_buffer_get_type();
GType ags_buffer_flags_get_type();

GRecMutex* ags_buffer_get_obj_mutex(AgsBuffer *buffer);

void ags_buffer_lock(AgsBuffer *buffer);
void ags_buffer_unlock(AgsBuffer *buffer);

gboolean ags_buffer_test_flags(AgsBuffer *buffer, AgsBufferFlags flags);
void ags_buffer_set_flags(AgsBuffer *buffer, AgsBufferFlags flags);
void ags_buffer_unset_flags(AgsBuffer *buffer, AgsBufferFlags flags);

gint ags_buffer_sort_func(gconstpointer a,
			  gconstpointer b);

guint64 ags_buffer_get_x(AgsBuffer *buffer);
void ags_buffer_set_x(AgsBuffer *buffer, guint64 x);

guint ags_buffer_get_samplerate(AgsBuffer *buffer);
void ags_buffer_set_samplerate(AgsBuffer *buffer,
			       guint samplerate);

guint ags_buffer_get_buffer_size(AgsBuffer *buffer);
void ags_buffer_set_buffer_size(AgsBuffer *buffer,
				guint buffer_size);

AgsSoundcardFormat ags_buffer_get_format(AgsBuffer *buffer);
void ags_buffer_set_format(AgsBuffer *buffer,
			   AgsSoundcardFormat format);

gpointer ags_buffer_get_data(AgsBuffer *buffer);

AgsBuffer* ags_buffer_duplicate(AgsBuffer *buffer);

AgsBuffer* ags_buffer_new();

G_END_DECLS

#endif /*__AGS_BUFFER_H__*/
