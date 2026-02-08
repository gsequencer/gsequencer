/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2026 Joël Krähemann
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

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_BOX_TEST                (ags_box_test_get_type())
#define AGS_BOX_TEST(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_BOX_TEST, AgsBoxTest))
#define AGS_BOX_TEST_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_BOX_TEST, AgsBoxTestClass))
#define AGS_IS_BOX_TEST(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_BOX_TEST))
#define AGS_IS_BOX_TEST_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_BOX_TEST))
#define AGS_BOX_TEST_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_BOX_TEST, AgsBoxTestClass))

#define AGS_BOX_TEST_DEFAULT_FONT_SIZE (12)

#define AGS_BOX_TEST_DEFAULT_BUTTON_WIDTH (12)
#define AGS_BOX_TEST_DEFAULT_BUTTON_HEIGHT (8)

#define AGS_BOX_TEST_DEFAULT_MARGIN_LEFT (AGS_BOX_TEST_DEFAULT_MARGIN)
#define AGS_BOX_TEST_DEFAULT_MARGIN_RIGHT (AGS_BOX_TEST_DEFAULT_MARGIN)

#define AGS_BOX_TEST_DEFAULT_HEIGHT (1.25 * AGS_BOX_TEST_DEFAULT_FONT_SIZE)
#define AGS_BOX_TEST_DEFAULT_WIDTH (64.0)

typedef struct _AgsBoxTest AgsBoxTest;
typedef struct _AgsBoxTestClass AgsBoxTestClass;

typedef enum{
  AGS_BOX_TEST_WITH_BUTTONS           = 1,
  AGS_BOX_TEST_SEEMLESS_MODE          = 1 <<  1,
  AGS_BOX_TEST_INVERSE_LIGHT          = 1 <<  2,
}AgsBoxTestFlags;

typedef enum{
  AGS_BOX_TEST_STATE_MOUSE_BUTTON_PRESSED   = 1,
}AgsBoxTestStateFlags;

typedef enum{
  AGS_BOX_TEST_SET_ENABLED,
  AGS_BOX_TEST_UNSET_ENABLED,
}AgsBoxTestAction;

struct _AgsBoxTest
{
  GtkWidget widget;

  AgsBoxTestFlags flags;
  AgsBoxTestStateFlags state_flags;
  
  guint font_size;
  
  gchar *font_name;
  
  GtkAdjustment *adjustment;
};

struct _AgsBoxTestClass
{
  GtkWidgetClass widget;

  void (*value_changed)(AgsBoxTest *box_test);
};

GType ags_box_test_get_type(void);
GType ags_box_test_flags_get_type(void);

/* flags */
gboolean ags_box_test_test_flags(AgsBoxTest *box_test,
				 AgsBoxTestFlags flags);
void ags_box_test_set_flags(AgsBoxTest *box_test,
			    AgsBoxTestFlags flags);
void ags_box_test_unset_flags(AgsBoxTest *box_test,
			      AgsBoxTestFlags flags);

/* getter and setter */
void ags_box_test_set_font_size(AgsBoxTest *box_test,
				guint font_size);
guint ags_box_test_get_font_size(AgsBoxTest *box_test);

void ags_box_test_set_font_name(AgsBoxTest *box_test,
				gchar *font_name);
gchar* ags_box_test_get_font_name(AgsBoxTest *box_test);

void ags_box_test_set_adjustment(AgsBoxTest *box_test,
				 GtkAdjustment *adjustment);
GtkAdjustment* ags_box_test_get_adjustment(AgsBoxTest *box_test);

/* events related */
void ags_box_test_value_changed(AgsBoxTest *box_test);

void ags_box_test_set_value(AgsBoxTest *box_test,
			    gdouble value);

gdouble ags_box_test_get_value(AgsBoxTest *box_test);

void ags_box_test_class_init(AgsBoxTestClass *box_test);
void ags_box_test_accessible_range_interface_init(GtkAccessibleRangeInterface *accessible_range);
void ags_box_test_init(AgsBoxTest *box_test);
void ags_box_test_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec);
void ags_box_test_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec);
void ags_box_test_dispose(GObject *gobject);

void ags_box_test_realize(GtkWidget *widget);
void ags_box_test_unrealize(GtkWidget *widget);

void ags_box_test_measure(GtkWidget *widget,
			  GtkOrientation orientation,
			  int for_size,
			  int *minimum,
			  int *natural,
			  int *minimum_baseline,
			  int *natural_baseline);
void ags_box_test_size_allocate(GtkWidget *widget,
				int width,
				int height,
				int baseline);

gboolean ags_box_test_set_current_value(GtkAccessibleRange *accessible_range,
					gdouble current_value);

void ags_box_test_frame_clock_update_callback(GdkFrameClock *frame_clock,
					      AgsBoxTest *box_test);

void ags_box_test_snapshot(GtkWidget *widget,
			   GtkSnapshot *snapshot);

/* instantiate */
AgsBoxTest* ags_box_test_new();

