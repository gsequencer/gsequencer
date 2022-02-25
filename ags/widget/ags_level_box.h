/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_LEVEL_BOX_H__
#define __AGS_LEVEL_BOX_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_LEVEL_BOX                (ags_level_box_get_type())
#define AGS_LEVEL_BOX(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LEVEL_BOX, AgsLevelBox))
#define AGS_LEVEL_BOX_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LEVEL_BOX, AgsLevelBoxClass))
#define AGS_IS_LEVEL_BOX(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LEVEL_BOX))
#define AGS_IS_LEVEL_BOX_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LEVEL_BOX))
#define AGS_LEVEL_BOX_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_LEVEL_BOX, AgsLevelBoxClass))

#define AGS_LEVEL_BOX_DEFAULT_SPACING (8)

typedef struct _AgsLevelBox AgsLevelBox;
typedef struct _AgsLevelBoxClass AgsLevelBoxClass;

struct _AgsLevelBox
{
  GtkBox box;

  /* private */
  guint level_count;
  
  GList *level;
};

struct _AgsLevelBoxClass
{
  GtkBoxClass box;

  void (*child_width_request)(AgsLevelBox *level_box,
			      GtkWidget *level,
			      gint width_request);
  void (*child_height_request)(AgsLevelBox *level_box,
			       GtkWidget *level,
			       gint height_request);
};

GType ags_level_box_get_type(void);

guint ags_level_box_get_level_count(AgsLevelBox *level_box);

void ags_level_box_add(AgsLevelBox *level_box,
		       GtkWidget *level);
void ags_level_box_remove(AgsLevelBox *level_box,
			  guint nth);

void ags_level_box_child_width_request(AgsLevelBox *level_box,
				       GtkWidget *level,
				       gint width_request);
void ags_level_box_child_height_request(AgsLevelBox *level_box,
					GtkWidget *level,
					gint height_request);

AgsLevelBox* ags_level_box_new(GtkOrientation orientation);

G_END_DECLS

#endif /*__AGS_LEVEL_BOX_H__*/
