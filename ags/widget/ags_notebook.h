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

#ifndef __AGS_NOTEBOOK_H__
#define __AGS_NOTEBOOK_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_NOTEBOOK                (ags_notebook_get_type())
#define AGS_NOTEBOOK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NOTEBOOK, AgsNotebook))
#define AGS_NOTEBOOK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NOTEBOOK, AgsNotebookClass))
#define AGS_IS_NOTEBOOK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_NOTEBOOK))
#define AGS_IS_NOTEBOOK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_NOTEBOOK))
#define AGS_NOTEBOOK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_NOTEBOOK, AgsNotebookClass))

#define AGS_NOTEBOOK_TAB_DEFAULT_WIDTH (100)
#define AGS_NOTEBOOK_TAB_DEFAULT_HEIGHT (32)

typedef struct _AgsNotebook AgsNotebook;
typedef struct _AgsNotebookClass AgsNotebookClass;

struct _AgsNotebook
{
  GtkBox box;

  guint tab_width;
  guint tab_height;

  GtkBox *navigation;
  GtkButton *scroll_prev;
  GtkButton *scroll_next;
  
  GtkScrolledWindow *scrolled_window;
  GtkBox *tab_box;

  GList *tab;
};

struct _AgsNotebookClass
{
  GtkBoxClass box;
};

GType ags_notebook_get_type(void);

gint ags_notebook_next_active_tab(AgsNotebook *notebook,
				  gint position);

GList* ags_notebook_get_tab(AgsNotebook *notebook);
void ags_notebook_add_tab(AgsNotebook *notebook,
			  GtkToggleButton *tab);
void ags_notebook_insert_tab(AgsNotebook *notebook,
			     GtkToggleButton *tab,
			     gint position);
void ags_notebook_remove_tab(AgsNotebook *notebook,
			     GtkToggleButton *tab);

AgsNotebook* ags_notebook_new();

G_END_DECLS

#endif /*__AGS_NOTEBOOK_H__*/
