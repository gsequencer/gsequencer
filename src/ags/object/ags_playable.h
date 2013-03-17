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

#ifndef __AGS_PLAYABLE_H__
#define __AGS_PLAYABLE_H__

#include <glib-object.h>

#include <ags/audio/ags_devout.h>

#define AGS_TYPE_PLAYABLE                    (ags_playable_get_type())
#define AGS_PLAYABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAYABLE, AgsPlayable))
#define AGS_PLAYABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_PLAYABLE, AgsPlayableInterface))
#define AGS_IS_PLAYABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PLAYABLE))
#define AGS_IS_PLAYABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_PLAYABLE))
#define AGS_PLAYABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_PLAYABLE, AgsPlayableInterface))

typedef void AgsPlayable;
typedef struct _AgsPlayableInterface AgsPlayableInterface;

#define AGS_PLAYABLE_ERROR (ags_playable_error_quark())

typedef enum{
  AGS_PLAYABLE_ERROR_NO_SUCH_LEVEL,
  AGS_PLAYABLE_ERROR_NO_SAMPLE,
}AgsPlayableError;

struct _AgsPlayableInterface
{
  GTypeInterface interface;

  gboolean (*open)(AgsPlayable *playable, gchar *name);

  /* these functions are especially for soundfonts */
  guint (*level_count)(AgsPlayable *playable);
  guint (*nth_level)(AgsPlayable *playable);
  gchar* (*selected_level)(AgsPlayable *playable);

  gchar** (*sublevel_names)(AgsPlayable *playable);
  void (*level_select)(AgsPlayable *playable,
		       guint nth_level, gchar *sublevel_name,
		       GError **error);
  void (*level_up)(AgsPlayable *playable,
		   guint levels,
		   GError **error);

  void (*iter_start)(AgsPlayable *playable);
  gboolean (*iter_next)(AgsPlayable *playable);

  /* read sample data */
  void (*info)(AgsPlayable *playable,
	       guint *channels, guint *frames,
	       guint *loop_start, guint *loop_end,
	       GError **error);
  signed short* (*read)(AgsPlayable *playable,
			guint channel,
			GError **error);

  void (*close)(AgsPlayable *playable);
};

GType ags_playable_get_type();

GQuark ags_playable_error_quark();

gboolean ags_playable_open(AgsPlayable *playable, gchar *name);

guint ags_playable_level_count(AgsPlayable *playable);
guint ags_playable_nth_level(AgsPlayable *playable);
gchar* ags_playable_selected_level(AgsPlayable *playable);

gchar** ags_playable_sublevel_names(AgsPlayable *playable);
void ags_playable_level_select(AgsPlayable *playable,
			       guint nth_level, gchar *sublevel_name,
			       GError **error);
void ags_playable_level_up(AgsPlayable *playable, guint levels, GError **error);

void ags_playable_iter_start(AgsPlayable *playable);
gboolean ags_playable_iter_next(AgsPlayable *playable);

void ags_playable_info(AgsPlayable *playable,
		       guint *channels, guint *frames,
		       guint *loop_start, guint *loop_end,
		       GError **error);
signed short* ags_playable_read(AgsPlayable *playable,
				guint channel,
				GError **error);

void ags_playable_close(AgsPlayable *playable);

GList* ags_playable_read_audio_signal(AgsPlayable *playable,
				      AgsDevout *devout,
				      guint start_channel, guint channels);

#endif /*__AGS_PLAYABLE_H__*/
