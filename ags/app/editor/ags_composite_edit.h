/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#define AGS_COMPOSITE_EDIT_DEFAULT_SEGMENT_WIDTH (64.0)
#define AGS_COMPOSITE_EDIT_DEFAULT_SEGMENT_HEIGHT (128.0)

typedef struct _AgsCompositeEdit AgsCompositeEdit;
typedef struct _AgsCompositeEditClass AgsCompositeEditClass;

typedef enum{
  AGS_COMPOSITE_EDIT_SCROLLBAR_HORIZONTAL     = 1,
  AGS_COMPOSITE_EDIT_SCROLLBAR_VERTICAL       = 1 <<  1,
}AgsCompositeEditScrollbar;

typedef enum{
  AGS_COMPOSITE_EDIT_CHANNEL_SELECTOR_AUDIO_CHANNEL,
  AGS_COMPOSITE_EDIT_CHANNEL_SELECTOR_OUTPUT_LINE,
  AGS_COMPOSITE_EDIT_CHANNEL_SELECTOR_INPUT_LINE,
}AgsCompositeEditChannelSelectorMode;

typedef enum{
  AGS_COMPOSITE_EDIT_PATTERN_MODE,
  AGS_COMPOSITE_EDIT_NOTATION_MODE,
}AgsCompositeEditEditMode;

typedef enum{
  AGS_COMPOSITE_EDIT_PASTE_MATCH_AUDIO_CHANNEL  =  1,
  AGS_COMPOSITE_EDIT_PASTE_MATCH_LINE           =  1 <<  1,
  AGS_COMPOSITE_EDIT_PASTE_NO_DUPLICATES        =  1 <<  2,
}AgsCompositeEditPasteFlags;

struct _AgsCompositeEdit
{
  GtkBox box;

  guint flags;
  guint connectable_flags;
  guint scrollbar;

  gchar *version;
  gchar *build_id;

  AgsUUID *uuid;

  GtkPaned *composite_paned;

  GtkBox *composite_box;

  GtkGrid *composite_grid;

  AgsRuler *ruler;

  guint channel_selector_mode;
  AgsNotebook *channel_selector;

  guint edit_mode;
  guint paste_flags;
  
  GtkBox *edit_box;
  
  GtkWidget *edit_control;
  GtkWidget *edit;

  GtkWidget *focused_edit;

  gboolean block_vscrollbar;
  GtkScrollbar *vscrollbar;

  gboolean block_hscrollbar;
  GtkScrollbar *hscrollbar;

  GtkScrolledWindow *scrolled_edit_meta;
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

gboolean ags_composite_edit_test_scrollbar(AgsCompositeEdit *composite_edit, guint scrollbar);
void ags_composite_edit_set_scrollbar(AgsCompositeEdit *composite_edit, guint scrollbar);
void ags_composite_edit_unset_scrollbar(AgsCompositeEdit *composite_edit, guint scrollbar);

void ags_composite_edit_set_channel_selector_mode(AgsCompositeEdit *composite_edit, guint channel_selector_mode);

void ags_composite_edit_set_edit_mode(AgsCompositeEdit *composite_edit, guint edit_mode);

/* instantiate */
AgsCompositeEdit* ags_composite_edit_new();

G_END_DECLS

#endif /*__AGS_COMPOSITE_EDIT_H__*/
