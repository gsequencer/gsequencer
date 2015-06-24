/* This file is part of GSequencer.
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

#ifndef __AGS_MENU_BAR_H__
#define __AGS_MENU_BAR_H__

#include <gtk/gtk.h>

#define AGS_TYPE_MENU_BAR                (ags_menu_bar_get_type ())
#define AGS_MENU_BAR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MENU_BAR, AgsMenuBar))
#define AGS_MENU_BAR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MENU_BAR, AgsMenuBarClass))
#define AGS_IS_MENU_BAR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MENU_BAR))
#define AGS_IS_MENU_BAR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MENU_BAR))
#define AGS_MENU_BAR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_MENU_BAR, AgsMenuBarClass))

typedef struct _AgsMenuBar AgsMenuBar;
typedef struct _AgsMenuBarClass AgsMenuBarClass;

struct _AgsMenuBar
{
  GtkMenuBar menu_bar;

  GtkMenu *file;
  GtkMenu *edit;
  GtkMenu *add;
  GtkMenu *help;
};

struct _AgsMenuBarClass
{
  GtkMenuBarClass menu_bar;
};

GType ags_menu_bar_get_type(void);

AgsMenuBar* ags_menu_bar_new();

GtkMenu* ags_zoom_menu_new();
GtkMenu* ags_tact_menu_new();

GtkComboBox* ags_zoom_combo_box_new();
GtkComboBox* ags_tact_combo_box_new();

#endif /*__AGS_MENU_BAR_H__*/
