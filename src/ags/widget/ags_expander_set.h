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

#ifndef __AGS_EXPANDER_SET_H__
#define __AGS_EXPANDER_SET_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_EXPANDER_SET                (ags_expander_set_get_type())
#define AGS_EXPANDER_SET(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EXPANDER_SET, AgsExpanderSet))
#define AGS_EXPANDER_SET_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EXPANDER_SET, AgsExpanderSetClass))
#define AGS_IS_EXPANDER_SET(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EXPANDER_SET))
#define AGS_IS_EXPANDER_SET_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EXPANDER_SET))
#define AGS_EXPANDER_SET_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EXPANDER_SET, AgsExpanderSetClass))

#define AGS_EXPANDER_SET_CHILD(ptr) ((AgsExpanderSetChild *)(ptr))

typedef struct _AgsExpanderSet AgsExpanderSet;
typedef struct _AgsExpanderSetClass AgsExpanderSetClass;
typedef struct _AgsExpanderSetChild AgsExpanderSetChild;

typedef enum{
  AGS_EXPANDER_SET_DRAW_GHOST            = 1,
  AGS_EXPANDER_SET_DRAW_LEFT_ENDING      = 1 << 1,
  AGS_EXPANDER_SET_DRAW_RIGHT_ENDING     = 1 << 2,
  AGS_EXPANDER_SET_DRAW_LEFT_CONNECTOR   = 1 << 3,
  AGS_EXPANDER_SET_DRAW_RIGHT_CONNECTOR  = 1 << 4,
}AgsExpanderSetFlags;

struct _AgsExpanderSet
{
  GtkTable table;

  guint flags;

  GList *ghost;
  GList *location;
};

struct _AgsExpanderSetClass
{
  GtkTableClass table;
};

struct _AgsExpanderSetChild
{
  GtkWidget *child;

  guint x;
  guint y;

  guint width;
  guint height;
};

GType ags_expander_set_get_type(void);

AgsExpanderSetChild* ags_expander_set_child_alloc(guint x, guint y,
						  guint width, guint height);
AgsExpanderSetChild* ags_expander_set_child_find(AgsExpanderSet *expander_set,
						 GtkWidget *child);

void ags_expander_set_set_flags(AgsExpanderSet *expander_set,
				guint flags);

void ags_expander_set_add(AgsExpanderSet *expander_set,
			  GtkWidget *widget,
			  guint x, guint y,
			  guint width, guint height);
void ags_expander_set_remove(AgsExpanderSet *expander_set,
			     GtkWidget *widget);

AgsExpanderSet* ags_expander_set_new(guint width, guint height);

#endif /*__AGS_EXPANDER_SET_H__*/
