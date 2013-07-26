#ifndef __AGS_SERVER_H__
#define __AGS_SERVER_H__

#include <xmlrpc.h>
#include <xmlrpc_server.h>

#define AGS_TYPE_SERVER                (ags_server_get_type())
#define AGS_SERVER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SERVER, AgsServer))
#define AGS_SERVER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SERVER, AgsServerClass))
#define AGS_IS_SERVER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SERVER))
#define AGS_IS_SERVER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SERVER))
#define AGS_SERVER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SERVER, AgsServerClass))

typedef enum{
  AGS_SERVER_STARTED        = 1,
}AgsServerFlags;

struct _AgsServer
{
  GObject object;

  guint flags;

  GObject *devout;
};

struct _AgsServerClass
{
  GObjectClass object;
};

GType ags_server_get_type();

AgsServer* ags_server_new(GObject *devout);

#endif /*__AGS_SERVER_H__*/
