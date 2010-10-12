#ifndef __AGS_MENU_H__
#define __AGS_MENU_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_table.h"

#define AGS_TYPE_MENU                (ags_menu_get_type())
#define AGS_MENU(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MENU, AgsMenu))
#define AGS_MENU_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MENU, AgsMenuClass))
#define AGS_IS_MENU(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MENU))
#define AGS_IS_MENU_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MENU))
#define AGS_MENU_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MENU, AgsMenuClass))

typedef struct _AgsMenu AgsMenu;
typedef struct _AgsMenuClass AgsMenuClass;

struct _AgsMenu
{
  GtkWindow window;

  AgsTable *table;
};

struct _AgsMenuClass
{
  GtkWindowClass window;
};

GType ags_menu_get_type(void);

AgsMenu* ags_menu_new();

#endif /*__AGS_MENU_H__*/
