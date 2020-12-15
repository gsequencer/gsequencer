/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_COMPOSITE_EDIT_H__
#define __AGS_COMPOSITE_EDIT_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_COMPOSITE_EDIT                (ags_composite_edit_get_type ())
#define AGS_COMPOSITE_EDIT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COMPOSITE_EDIT, AgsCompositeEdit))
#define AGS_COMPOSITE_EDIT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COMPOSITE_EDIT, AgsCompositeEditClass))
#define AGS_IS_COMPOSITE_EDIT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_COMPOSITE_EDIT))
#define AGS_IS_COMPOSITE_EDIT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_COMPOSITE_EDIT))
#define AGS_COMPOSITE_EDIT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_COMPOSITE_EDIT, AgsCompositeEditClass))

typedef struct _AgsCompositeEdit AgsCompositeEdit;
typedef struct _AgsCompositeEditClass AgsCompositeEditClass;

typedef enum{
  AGS_COMPOSITE_EDIT_ADDED_TO_REGISTRY     = 1,
  AGS_COMPOSITE_EDIT_CONNECTED             = 1 << 1,
}AgsCompositeEditFlags;

typedef enum{
  AGS_COMPOSITE_EDIT_SCROLLBAR_HORIZONTAL     = 1,
  AGS_COMPOSITE_EDIT_SCROLLBAR_VERTICAL       = 1 <<  1,
}AgsCompositeEditScrollbar;

struct _AgsCompositeEdit
{
  GtkBox box;

  guint flags;
  guint scrollbar;

  gchar *version;
  gchar *build_id;

  AgsUUID *uuid;

  GtkBox *edit_box;

  GtkGrid *edit_grid;
  
  GtkWidget *edit_control;  
  GtkWidget *edit;

  gboolean block_vscrollbar;
  GtkScrollbar *vscrollbar;

  gboolean block_hscrollbar;
  GtkScrollbar *hscrollbar;

  GtkWidget *edit_meta;
};

struct _AgsCompositeEditClass
{
  GtkBoxClass box;
};

GType ags_composite_edit_get_type(void);

gboolean ags_composite_edit_test_flags(AgsCompositeEdit *composite_edit, guint flags);
void ags_composite_edit_set_flags(AgsCompositeEdit *composite_edit, guint flags);
void ags_composite_edit_unset_flags(AgsCompositeEdit *composite_edit, guint flags);

/* instantiate */
AgsCompositeEdit* ags_composite_edit_new();

G_END_DECLS

#endif /*__AGS_COMPOSITE_EDIT_H__*/
