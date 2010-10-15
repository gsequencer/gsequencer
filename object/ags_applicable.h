#ifndef __AGS_APPLICABLE_H__
#define __AGS_APPLICABLE_H__

#include <glib-object.h>

#define AGS_TYPE_APPLICABLE                    (ags_applicable_get_type())
#define AGS_APPLICABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_APPLICABLE, AgsApplicable))
#define AGS_APPLICABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_APPLICABLE, AgsApplicableInterface))
#define AGS_IS_APPLICABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_APPLICABLE))
#define AGS_IS_APPLICABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_APPLICABLE))
#define AGS_APPLICABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_APPLICABLE, AgsApplicableInterface))

typedef void AgsApplicable;
typedef struct _AgsApplicableInterface AgsApplicableInterface;

struct _AgsApplicableInterface
{
  GTypeInterface interface;
  
  void (*set_update)(AgsApplicable *applicable, gboolean update);
  void (*apply)(AgsApplicable *applicable);
  void (*reset)(AgsApplicable *applicable);
};

GType ags_applicable_get_type();

void ags_applicable_set_update(AgsApplicable *applicable, gboolean update);
void ags_applicable_apply(AgsApplicable *applicable);
void ags_applicable_reset(AgsApplicable *applicable);

#endif /*__AGS_APPLICABLE_H__*/
