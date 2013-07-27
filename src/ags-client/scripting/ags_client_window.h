#ifndef __AGS_SCRIPT_EDITOR_H__
#define __AGS_CLIENT_WINDOW_H__

#include <gtk/gtk.h>

#define AGS_TYPE_CLIENT_WINDOW                (ags_client_window_get_type())
#define AGS_CLIENT_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CLIENT_WINDOW, AgsClientWindow))
#define AGS_CLIENT_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_CLIENT_WINDOW, AgsClientWindowClass))
#define AGS_IS_CLIENT_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CLIENT_WINDOW))
#define AGS_IS_CLIENT_WINDOW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CLIENT_WINDOW))
#define AGS_CLIENT_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_CLIENT_WINDOW, AgsClientWindowClass))

typedef struct _AgsClientWindow AgsClientWindow;
typedef struct _AgsClientWindowClass AgsClientWindowClass;

struct _AgsClientWindow
{
  GtkWindow window;
};

struct _AgsClientWindowClass
{
  GtkWindowClass window;
};

GType ags_client_window_get_type();

AgsClientWindow* ags_client_window_new();

#endif /*__AGS_CLIENT_WINDOW_H__*/
