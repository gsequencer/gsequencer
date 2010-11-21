#ifndef __AGS_TACTABLE_H__
#define __AGS_TACTABLE_H__

#include <glib-object.h>

#define AGS_TYPE_TACTABLE                    (ags_tactable_get_type())
#define AGS_TACTABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TACTABLE, AgsTactable))
#define AGS_TACTABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_TACTABLE, AgsTactableInterface))
#define AGS_IS_TACTABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_TACTABLE))
#define AGS_IS_TACTABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_TACTABLE))
#define AGS_TACTABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_TACTABLE, AgsTactableInterface))

typedef void AgsTactable;
typedef struct _AgsTactableInterface AgsTactableInterface;

struct _AgsTactableInterface
{
  GTypeInterface interface;

  void (*change_bpm)(AgsTactable *tactable, gdouble bpm, gdouble old_bpm);
};

GType ags_tactable_get_type();

void ags_tactable_change_bpm(AgsTactable *tactable, gdouble bpm, gdouble old_bpm);

#endif /*__AGS_TACTABLE_H__*/
