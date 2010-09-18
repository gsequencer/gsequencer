#ifndef __AGS_RUN_CONNECTABLE_H__
#define __AGS_RUN_CONNECTABLE_H__

#include <glib-object.h>

#define AGS_TYPE_RUN_CONNECTABLE                    (ags_run_connectable_get_type())
#define AGS_RUN_CONNECTABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RUN_CONNECTABLE, AgsRunConnectable))
#define AGS_RUN_CONNECTABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_RUN_CONNECTABLE, AgsRunConnectableInterface))
#define AGS_IS_RUN_CONNECTABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RUN_CONNECTABLE))
#define AGS_IS_RUN_CONNECTABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_RUN_CONNECTABLE))
#define AGS_RUN_CONNECTABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_RUN_CONNECTABLE, AgsRunConnectableInterface))

typedef void AgsRunConnectable;
typedef struct _AgsRunConnectableInterface AgsRunConnectableInterface;

struct _AgsRunConnectableInterface
{
  GTypeInterface interface;

  void (*connect)(AgsRunConnectable *connectable);
  void (*disconnect)(AgsRunConnectable *connectable);
};

void ags_run_connectable_connect(AgsRunConnectable *connectable);
void ags_run_connectable_disconnect(AgsRunConnectable *connectable);

#endif /*__AGS_RUN_CONNECTABLE_H__*/
