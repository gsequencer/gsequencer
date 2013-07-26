#ifndef __AGS_REGISTRY_H__
#define __AGS_REGISTRY_H__

#include <xmlrpc.h>
#include <xmlrpc_registry.h>

#define AGS_TYPE_REGISTRY                (ags_registry_get_type())
#define AGS_REGISTRY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_REGISTRY, AgsRegistry))
#define AGS_REGISTRY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_REGISTRY, AgsRegistryClass))
#define AGS_IS_REGISTRY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_REGISTRY))
#define AGS_IS_REGISTRY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_REGISTRY))
#define AGS_REGISTRY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_REGISTRY, AgsRegistryClass))

struct _AgsRegistry
{
  GObject object;
};

struct _AgsRegistryClass
{
  GObjectClass object;
};

GType ags_registry_get_type();

AgsRegistry* ags_registry_new();

#endif /*__AGS_REGISTRY_H__*/
