#ifndef __AGS_PANEL_H__
#define __AGS_PANEL_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>

#define AGS_TYPE_PANEL                (ags_panel_get_type())
#define AGS_PANEL(obj)                ((AgsPanel*) G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_PANEL, AgsPanel))
#define AGS_PANEL_CLASS(class)        ((AgsPanelClass*) G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PANEL, AgsPanelClass))
#define AGS_IS_PANEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE(obj, AGS_TYPE_PANEL))
#define AGS_IS_PANEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PANEL))
#define AGS_PANEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PANEL, AgsPanelClass))

typedef struct _AgsPanel AgsPanel;
typedef struct _AgsPanelClass AgsPanelClass;

struct _AgsPanel
{
  AgsMachine machine;

  GtkVBox *vbox;
};

struct _AgsPanelClass
{
  AgsMachineClass machine;
};

GType ags_panel_get_type(void);

AgsPanel* ags_panel_new();

#endif /*__AGS_PANEL_H__*/
