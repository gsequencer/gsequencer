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

#ifndef __AGS_SCALE_BOX_H__
#define __AGS_SCALE_BOX_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_SCALE_BOX                (ags_scale_box_get_type())
#define AGS_SCALE_BOX(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SCALE_BOX, AgsScaleBox))
#define AGS_SCALE_BOX_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SCALE_BOX, AgsScaleBoxClass))
#define AGS_IS_SCALE_BOX(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SCALE_BOX))
#define AGS_IS_SCALE_BOX_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SCALE_BOX))
#define AGS_SCALE_BOX_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_SCALE_BOX, AgsScaleBoxClass))

#define AGS_SCALE_BOX_DEFAULT_SPACING (8)

typedef struct _AgsScaleBox AgsScaleBox;
typedef struct _AgsScaleBoxClass AgsScaleBoxClass;

struct _AgsScaleBox
{
  GtkBox box;

  /* private */
  guint scale_count;
  
  GList *scale;
};

struct _AgsScaleBoxClass
{
  GtkBoxClass box;

  void (*child_width_request)(AgsScaleBox *scale_box,
			      GtkWidget *scale,
			      gint width_request);
  void (*child_height_request)(AgsScaleBox *scale_box,
			       GtkWidget *scale,
			       gint height_request);
};

GType ags_scale_box_get_type(void);

guint ags_scale_box_get_scale_count(AgsScaleBox *scale_box);

void ags_scale_box_add(AgsScaleBox *scale_box,
		       GtkWidget *scale);
void ags_scale_box_remove(AgsScaleBox *scale_box,
			  guint position);

void ags_scale_box_child_width_request(AgsScaleBox *scale_box,
				       GtkWidget *scale,
				       gint width_request);
void ags_scale_box_child_height_request(AgsScaleBox *scale_box,
					GtkWidget *scale,
					gint height_request);

GList* ags_scale_box_get_scale(AgsScaleBox *scale_box);

AgsScaleBox* ags_scale_box_new(GtkOrientation orientation);

G_END_DECLS

#endif /*__AGS_SCALE_BOX_H__*/
