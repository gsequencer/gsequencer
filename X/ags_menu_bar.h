#ifndef __AGS_MENU_BAR_H__
#define __AGS_MENU_BAR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtk/gtkmenubar.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkmenuitem.h>

#define AGS_TYPE_MENU_BAR                (ags_menu_bar_get_type())
#define AGS_MENU_BAR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MENUBAR, AgsMenuBar))
#define AGS_MENU_BAR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MENUBAR, AgsMenuBarClass))
#define AGS_IS_MENU_BAR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MENU_BAR))
#define AGS_IS_MENU_BAR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MENU_BAR))
#define AGS_MENU_BAR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MENU_BAR, AgsMenuBarClass))

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

AgsMenuBar* ags_menu_bar_new();

GtkMenu* ags_zoom_menu_new();
GtkMenu* ags_tic_menu_new();

#endif /*__AGS_MENU_BAR_H__*/
