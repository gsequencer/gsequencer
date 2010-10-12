#ifndef __AGS_NAVIGATION_H__
#define __AGS_NAVIGATION_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "../audio/ags_devout.h"

#define AGS_TYPE_NAVIGATION                (ags_navigation_get_type())
#define AGS_NAVIGATION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NAVIGATION, AgsNavigation))
#define AGS_NAVIGATION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NAVIGATION, AgsNavigationClass))
#define AGS_IS_NAVIGATION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_NAVIGATION))
#define AGS_IS_NAVIGATION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_NAVIGATION))
#define AGS_NAVIGATION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_NAVIGATION, AgsNavigationClass))

typedef struct _AgsNavigation AgsNavigation;
typedef struct _AgsNavigationClass AgsNavigationClass;

struct _AgsNavigation
{
  GtkVBox vbox;

  AgsDevout *devout;

  GtkToggleButton *expander;

  GtkSpinButton *bpm;

  GtkToggleButton *rewind;
  GtkButton *previous;
  GtkToggleButton *play;
  GtkButton *stop;
  GtkButton *next;
  GtkToggleButton *forward;

  GtkCheckButton *loop;

  GtkSpinButton *position_min;
  GtkSpinButton *position_sec;

  GtkSpinButton *duration_min;
  GtkSpinButton *duration_sec;

  GtkSpinButton *loop_left_min;
  GtkSpinButton *loop_left_sec;

  GtkSpinButton *loop_right_min;
  GtkSpinButton *loop_right_sec;

  GtkCheckButton *raster;
};

struct _AgsNavigationClass
{
  GtkVBoxClass vbox;
};

GType ags_navigation_get_type(void);

AgsNavigation* ags_navigation_new();

#endif /*__AGS_NAVIGATION_H__*/
