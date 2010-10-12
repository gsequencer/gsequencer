#ifndef __AGS_CONNECTABLE_H__
#define __AGS_CONNECTABLE_H__

#include <glib-object.h>

#define AGS_TYPE_CONNECTABLE                    (ags_connectable_get_type())
#define AGS_CONNECTABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONNECTABLE, AgsConnectable))
#define AGS_CONNECTABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_CONNECTABLE, AgsConnectableInterface))
#define AGS_IS_CONNECTABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CONNECTABLE))
#define AGS_IS_CONNECTABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_CONNECTABLE))
#define AGS_CONNECTABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_CONNECTABLE, AgsConnectableInterface))

typedef void AgsConnectable;
typedef struct _AgsConnectableInterface AgsConnectableInterface;

struct _AgsConnectableInterface
{
  GTypeInterface interface;

  void (*connect)(AgsConnectable *connectable);
  void (*disconnect)(AgsConnectable *connectable);
};

GType ags_connectable_get_type();

void ags_connectable_connect(AgsConnectable *connectable);
void ags_connectable_disconnect(AgsConnectable *connectable);

#endif /*__AGS_CONNECTABLE_H__*/
