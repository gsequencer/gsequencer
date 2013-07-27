#ifndef __AGS_SERVER_H__
#define __AGS_SERVER_H__

#include <glib.h>
#include <glib-object.h>

#include <xmlrpc.h>
#include <xmlrpc_server.h>

#include <ags/server/ags_registry.h>
#include <ags/server/ags_remote_task.h>

#define AGS_TYPE_SERVER                (ags_server_get_type())
#define AGS_SERVER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SERVER, AgsServer))
#define AGS_SERVER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SERVER, AgsServerClass))
#define AGS_IS_SERVER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SERVER))
#define AGS_IS_SERVER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SERVER))
#define AGS_SERVER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SERVER, AgsServerClass))

typedef struct _AgsServer AgsServer;
typedef struct _AgsServerClass AgsServerClass;

typedef enum{
  AGS_SERVER_STARTED        = 1,
}AgsServerFlags;

struct _AgsServer
{
  GObject object;

  guint flags;

  void *server_info;
  
  GObject *devout;

  AgsRegistry *registry;
  AgsRemoteTask *remote_task;
};

struct _AgsServerClass
{
  GObjectClass object;
};

GType ags_server_get_type();

AgsServer* ags_server_lookup(void *server_info);

xmlrpc_value* ags_server_create_object(xmlrpc_env *env,
				       xmlrpc_value *param_array,
				       void *server_info);

xmlrpc_value* ags_server_object_set_property(xmlrpc_env *env,
					     xmlrpc_value *param_array,
					     void *server_info);

AgsServer* ags_server_new(GObject *devout);

#endif /*__AGS_SERVER_H__*/
