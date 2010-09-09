#ifndef __AGS_MENU_ITEM_H__
#define __AGS_MENU_ITEM_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_MENU_ITEM                (ags_menu_get_type())
#define AGS_MENU_ITEM(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MENU_ITEM, AgsMenuItem))
#define AGS_MENU_ITEM_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MENU_ITEM, AgsMenuItemClass))
#define AGS_IS_MENU_ITEM(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MENU_ITEM))
#define AGS_IS_MENU_ITEM_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MENU_ITEM))
#define AGS_MENU_ITEM_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MENU_ITEM, AgsMenuItemClass))

typedef struct _AgsMenuItem AgsMenuItem;
typedef struct _AgsMenuItemClass AgsMenuItemClass;

struct _AgsMenuItem
{
  AgsTable table;
};

struct _AgsMenuItemClass
{
  AgsTableClass table;
};

AgsMenuItem* ags_menu_item_new();

#endif /*__AGS_MENU_ITEM_H__*/
