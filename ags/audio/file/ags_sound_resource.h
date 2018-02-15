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

#ifndef __AGS_SOUND_RESOURCE_H__
#define __AGS_SOUND_RESOURCE_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_SOUND_RESOURCE                    (ags_sound_resource_get_type())
#define AGS_SOUND_RESOURCE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SOUND_RESOURCE, AgsSoundResource))
#define AGS_SOUND_RESOURCE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_SOUND_RESOURCE, AgsSoundResourceInterface))
#define AGS_IS_SOUND_RESOURCE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SOUND_RESOURCE))
#define AGS_IS_SOUND_RESOURCE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_SOUND_RESOURCE))
#define AGS_SOUND_RESOURCE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_SOUND_RESOURCE, AgsSoundResourceInterface))

typedef struct _AgsSoundResource AgsSoundResource;
typedef struct _AgsSoundResourceInterface AgsSoundResourceInterface;

struct _AgsSoundResourceInterface
{
  GTypeInterface ginterface;

  gboolean (*open)(AgsSoundResource *sound_resource,
		   gchar *filename);
  gboolean (*rw_open)(AgsSoundResource *sound_resource,
		      gchar *filename,
		      gboolean create);

  void (*load)(AgsSoundResource *sound_resource);
  
  void (*info)(AgsSoundResource *sound_resource,
	       guint *frame_count,
	       guint *loop_start, guint *loop_end);
  
  void (*set_presets)(AgsSoundResource *sound_resource,
		      guint channels,
		      guint samplerate,
		      guint buffer_size,
		      guint format);
  void (*get_presets)(AgsSoundResource *sound_resource,
		      guint *channels,
		      guint *samplerate,
		      guint *buffer_size,
		      guint *format);

  /* read sample data */
  guint (*read)(AgsSoundResource *sound_resource,
		void *dbuffer,
		guint audio_channel,
		guint frame_count, guint format);

  /* write sample data */
  void (*write)(AgsSoundResource *sound_resource,
		void *sbuffer,
		guint audio_channel,
		guint frame_count, guint format);
  void (*flush)(AgsSoundResource *sound_resource);

  /* position */
  void (*seek)(AgsSoundResource *sound_resource,
	       guint frame_count, gint whence);

  /* close */
  void (*close)(AgsSoundResource *sound_resource);
};

GType ags_sound_resource_get_type();

gboolean ags_sound_resource_open(AgsSoundResource *sound_resource,
				 gchar *filename);
gboolean ags_sound_resource_rw_open(AgsSoundResource *sound_resource,
				    gchar *filename,
				    gboolean create);

void ags_sound_resource_load(AgsSoundResource *sound_resource);

void ags_sound_resource_info(AgsSoundResource *sound_resource,
			     guint *frame_count,
			     guint *loop_start, guint *loop_end);
  
void ags_sound_resource_set_presets(AgsSoundResource *sound_resource,
				    guint channels,
				    guint samplerate,
				    guint buffer_size,
				    guint format);
void ags_sound_resource_get_presets(AgsSoundResource *sound_resource,
				    guint *channels,
				    guint *samplerate,
				    guint *buffer_size,
				    guint *format);

/* read sample data */
guint ags_sound_resource_read(AgsSoundResource *sound_resource,
			      void *dbuffer,
			      guint audio_channel,
			      guint frame_count, guint format);

/* write sample data */
void ags_sound_resource_write(AgsSoundResource *sound_resource,
			      void *sbuffer,
			      guint audio_channel,
			      guint frame_count, guint format);
void ags_sound_resource_flush(AgsSoundResource *sound_resource);

/* position */
void ags_sound_resource_seek(AgsSoundResource *sound_resource,
			     guint frame_count, gint whence);

/* close */
void ags_sound_resource_close(AgsSoundResource *sound_resource);

#endif /*__AGS_SOUND_RESOURCE_H__*/
