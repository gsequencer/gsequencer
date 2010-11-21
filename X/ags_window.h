#ifndef __AGS_WINDOW_H__
#define __AGS_WINDOW_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_devout.h>

#include <ags/X/ags_menu_bar.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_editor.h>
#include <ags/X/ags_navigation.h>

#define AGS_TYPE_WINDOW                (ags_window_get_type())
#define AGS_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WINDOW, AgsWindow))
#define AGS_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WINDOW, AgsWindowClass))

typedef struct _AgsWindow AgsWindow;
typedef struct _AgsWindowClass AgsWindowClass;
typedef struct _AgsMachineCounter AgsMachineCounter;

struct _AgsWindow
{
  GtkWindow window;

  AgsDevout *devout;

  char *name;

  AgsMenuBar *menu_bar;

  GtkVPaned *paned;

  GtkVBox *machines;
  AgsMachineCounter *counter;
  AgsMachine *selected;

  AgsEditor *editor;
  AgsNavigation *navigation;
};

struct _AgsWindowClass
{
  GtkWindowClass window;
};

struct _AgsMachineCounter
{
  guint everything;

  guint panel;
  guint mixer;
  guint drum;
  guint matrix;
  guint synth;
  guint ffplayer;
};

GType ags_window_get_type(void);

AgsWindow* ags_window_new();

#endif /*__AGS_WINDOW_H__*/
