#ifndef __AGS_CLIENT_H__
#define __AGS_CLIENT_H__

#include <glib.h>
#include <glib-object.h>

#include <xmlrpc.h>
#include <xmlrpc_client.h>

#define AGS_TYPE_CLIENT                (ags_client_get_type())
#define AGS_CLIENT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CLIENT, AgsClient))
#define AGS_CLIENT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_CLIENT, AgsClientClass))
#define AGS_IS_CLIENT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CLIENT))
#define AGS_IS_CLIENT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CLIENT))
#define AGS_CLIENT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_CLIENT, AgsClientClass))

typedef struct _AgsClient AgsClient;
typedef struct _AgsClientClass AgsClientClass;

struct _AgsClient
{
  GObject object;
};

struct _AgsClientClass
{
  GObjectClass object;
};

GType ags_client_get_type();

AgsClient* ags_client_new();

#endif /*__AGS_CLIENT_H__*/
