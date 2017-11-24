/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_WAVE_TOOLBAR_H__
#define __AGS_WAVE_TOOLBAR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_WAVE_TOOLBAR                (ags_wave_toolbar_get_type())
#define AGS_WAVE_TOOLBAR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WAVE_TOOLBAR, AgsWaveToolbar))
#define AGS_WAVE_TOOLBAR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WAVE_TOOLBAR, AgsWaveToolbarClass))
#define AGS_IS_WAVE_TOOLBAR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_WAVE_TOOLBAR))
#define AGS_IS_WAVE_TOOLBAR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_WAVE_TOOLBAR))
#define AGS_WAVE_TOOLBAR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_WAVE_TOOLBAR, AgsWaveToolbarClass))

typedef struct _AgsWaveToolbar AgsWaveToolbar;
typedef struct _AgsWaveToolbarClass AgsWaveToolbarClass;

typedef enum{
  AGS_WAVE_TOOLBAR_CONNECTED    = 1,
}AgsWaveToolbarFlags;

struct _AgsWaveToolbar
{
  GtkToolbar toolbar;

  guint flags;
  
  GtkToggleButton *selected_edit_mode;
  GtkToggleButton *position;
  GtkToggleButton *select;

  GtkButton *copy;
  GtkButton *cut;
  GtkButton *paste;

  GtkMenuToolButton *menu_tool;
  GtkMenu *tool_popup;

  GtkDialog *select_audio_data;
  GtkDialog *position_wave_cursor;

  guint zoom_history;
  GtkComboBox *zoom;
};

struct _AgsWaveToolbarClass
{
  GtkToolbarClass toolbar;
};

GType ags_wave_toolbar_get_type(void);

GtkMenu* ags_wave_toolbar_tool_popup_new(GtkToolbar *wave_toolbar);

AgsWaveToolbar* ags_wave_toolbar_new();

#endif /*__AGS_WAVE_TOOLBAR_H__*/
