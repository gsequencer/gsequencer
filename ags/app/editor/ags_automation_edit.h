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

#ifndef __AGS_AUTOMATION_EDIT_H__
#define __AGS_AUTOMATION_EDIT_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUTOMATION_EDIT                (ags_automation_edit_get_type())
#define AGS_AUTOMATION_EDIT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTOMATION_EDIT, AgsAutomationEdit))
#define AGS_AUTOMATION_EDIT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUTOMATION_EDIT, AgsAutomationEditClass))
#define AGS_IS_AUTOMATION_EDIT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUTOMATION_EDIT))
#define AGS_IS_AUTOMATION_EDIT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUTOMATION_EDIT))
#define AGS_AUTOMATION_EDIT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_AUTOMATION_EDIT, AgsAutomationEditClass))

#define AGS_AUTOMATION_EDIT_DEFAULT_HEIGHT (128)
#define AGS_AUTOMATION_EDIT_DEFAULT_WIDTH (512)

#define AGS_AUTOMATION_EDIT_DEFAULT_POINT_RADIUS (1.75)

#define AGS_AUTOMATION_EDIT_DEFAULT_SCAN_HEIGHT (8)
#define AGS_AUTOMATION_EDIT_DEFAULT_SCAN_WIDTH (64)

#define AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_WIDTH (64)
#define AGS_AUTOMATION_EDIT_DEFAULT_CONTROL_HEIGHT (8)

#define AGS_AUTOMATION_EDIT_DEFAULT_STEP_COUNT (16)

#define AGS_AUTOMATION_EDIT_DEFAULT_CURSOR_POSITION_X (0)
#define AGS_AUTOMATION_EDIT_DEFAULT_CURSOR_POSITION_Y (0.0)

#define AGS_AUTOMATION_EDIT_DEFAULT_SELECTED_ACCELERATION_BORDER (2)

#define AGS_AUTOMATION_EDIT_DEFAULT_FADER_WIDTH (3)

#define AGS_AUTOMATION_EDIT_CURSOR_WIDTH (5)
#define AGS_AUTOMATION_EDIT_CURSOR_HEIGHT (5)

#define AGS_AUTOMATION_EDIT_MIN_ZOOM (1.0 / 16.0)
#define AGS_AUTOMATION_EDIT_MAX_ZOOM (4.0)

#define AGS_AUTOMATION_EDIT_DEFAULT_LOWER (0.0)
#define AGS_AUTOMATION_EDIT_DEFAULT_UPPER (1.0)
#define AGS_AUTOMATION_EDIT_DEFAULT_VALUE (0.0)

#define AGS_AUTOMATION_EDIT_DEFAULT_PADDING (8)
  
typedef struct _AgsAutomationEdit AgsAutomationEdit;
typedef struct _AgsAutomationEditClass AgsAutomationEditClass;

typedef enum{
  AGS_AUTOMATION_EDIT_CONNECTED                   = 1,
  AGS_AUTOMATION_EDIT_AUTO_SCROLL                 = 1 <<  1,
  AGS_AUTOMATION_EDIT_SHOW_RULER                  = 1 <<  2,
  AGS_AUTOMATION_EDIT_SHOW_VSCROLLBAR             = 1 <<  3,
  AGS_AUTOMATION_EDIT_SHOW_HSCROLLBAR             = 1 <<  4,
  AGS_AUTOMATION_EDIT_LOGARITHMIC                 = 1 <<  5,
  AGS_AUTOMATION_EDIT_INTEGER                     = 1 <<  6,
  AGS_AUTOMATION_EDIT_TOGGLED                     = 1 <<  7,
}AgsAutomationEditFlags;

typedef enum{
  AGS_AUTOMATION_EDIT_NO_EDIT_MODE,
  AGS_AUTOMATION_EDIT_POSITION_CURSOR,
  AGS_AUTOMATION_EDIT_ADD_ACCELERATION,
  AGS_AUTOMATION_EDIT_DELETE_ACCELERATION,
  AGS_AUTOMATION_EDIT_SELECT_ACCELERATION,
}AgsAutomationEditMode;

typedef enum{
  AGS_AUTOMATION_EDIT_BUTTON_1            = 1,
}AgsAutomationEditButtonMask;

typedef enum{
  AGS_AUTOMATION_EDIT_KEY_L_CONTROL       = 1,
  AGS_AUTOMATION_EDIT_KEY_R_CONTROL       = 1 <<  1,
  AGS_AUTOMATION_EDIT_KEY_L_SHIFT         = 1 <<  2,
  AGS_AUTOMATION_EDIT_KEY_R_SHIFT         = 1 <<  3,
}AgsAutomationEditKeyMask;

struct _AgsAutomationEdit
{
  GtkGrid grid;

  guint flags;
  guint mode;

  guint button_mask;
  guint key_mask;
  
  guint note_offset;
  guint note_offset_absolute;

  guint point_radius;
  
  guint scan_width;
  guint scan_height;

  guint control_width;
  guint control_height;

  guint step_count;
  
  guint cursor_position_x;
  gdouble cursor_position_y;

  guint selected_acceleration_border;

  guint selection_x0;
  guint selection_x1;
  guint selection_y0;
  guint selection_y1;

  AgsAcceleration *current_acceleration;
  
  AgsRuler *ruler;

  GType channel_type;
  
  gchar *filename;
  gchar *effect;

  gchar *control_specifier;
  gchar *control_name;

  gdouble lower;
  gdouble upper;
  gdouble default_value;
  
  GtkDrawingArea *drawing_area;
  
  GtkScrollbar *vscrollbar;
  GtkScrollbar *hscrollbar;
};

struct _AgsAutomationEditClass
{
  GtkGridClass grid;
};

GType ags_automation_edit_get_type(void);

void ags_automation_edit_reset_vscrollbar(AgsAutomationEdit *automation_edit);
void ags_automation_edit_reset_hscrollbar(AgsAutomationEdit *automation_edit);

void ags_automation_edit_draw_segment(AgsAutomationEdit *automation_edit, cairo_t *cr);
void ags_automation_edit_draw_position(AgsAutomationEdit *automation_edit, cairo_t *cr);

void ags_automation_edit_draw_cursor(AgsAutomationEdit *automation_edit, cairo_t *cr);
void ags_automation_edit_draw_selection(AgsAutomationEdit *automation_edit, cairo_t *cr);

void ags_automation_edit_draw_acceleration(AgsAutomationEdit *automation_edit,
					   AgsAcceleration *acceleration_a, AgsAcceleration *acceleration_b,
					   cairo_t *cr,
					   gdouble opacity);
void ags_automation_edit_draw_automation(AgsAutomationEdit *automation_edit, cairo_t *cr);

void ags_automation_edit_draw(AgsAutomationEdit *automation_edit, cairo_t *cr);

AgsAutomationEdit* ags_automation_edit_new();

G_END_DECLS

#endif /*__AGS_AUTOMATION_EDIT_H__*/
