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

#ifndef __AGS_NOTEBOOK_H__
#define __AGS_NOTEBOOK_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_NOTEBOOK                (ags_notebook_get_type())
#define AGS_NOTEBOOK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NOTEBOOK, AgsNotebook))
#define AGS_NOTEBOOK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NOTEBOOK, AgsNotebookClass))
#define AGS_IS_NOTEBOOK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_NOTEBOOK))
#define AGS_IS_NOTEBOOK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_NOTEBOOK))
#define AGS_NOTEBOOK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_NOTEBOOK, AgsNotebookClass))

#define AGS_NOTEBOOK_TAB(x) ((AgsNotebookTab *)(x))

typedef struct _AgsNotebook AgsNotebook;
typedef struct _AgsNotebookClass AgsNotebookClass;
typedef struct _AgsNotebookTab AgsNotebookTab;

typedef enum{
  AGS_NOTEBOOK_TAB_VISIBLE           = 1,
  AGS_NOTEBOOK_TAB_MODE_NORMAL       = 1 << 1,
  AGS_NOTEBOOK_TAB_MODE_OVERLAY      = 1 << 2,
}AgsNotebookTabFlags;

struct _AgsNotebook
{
  GtkVBox vbox;

  guint flags;

  GtkHBox *hbox;

  GList *tabs;
  GtkWidget *child;
};

struct _AgsNotebookClass
{
  GtkVBoxClass vbox;
};

struct _AgsNotebookTab
{
  guint flags;

  GtkToggleButton *toggle;
  GObject *notation;
};

GType ags_notebook_get_type(void);

gint ags_notebook_tab_index(AgsNotebook *notebook,
			    GObject *notation);
gint ags_notebook_next_active_tab(AgsNotebook *notebook,
				  gint position);

gint ags_notebook_add_tab(AgsNotebook *notebook);
void ags_notebook_insert_tab(AgsNotebook *notebook,
			     gint position);
void ags_notebook_remove_tab(AgsNotebook *notebook,
			     gint nth);

void ags_notebook_add_child(AgsNotebook *notebook,
			    GtkWidget *child);
void ags_notebook_remove_child(AgsNotebook *notebook,
			       GtkWidget *child);

AgsNotebook* ags_notebook_new();

#endif /*__AGS_NOTEBOOK_H__*/
