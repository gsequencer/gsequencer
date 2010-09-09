#ifndef __AGS_PATTERN_H__
#define __AGS_PATTERN_H__

#include <glib.h>
#include <glib-object.h>

#include "ags_channel.h"

#define AGS_TYPE_PATTERN                (ags_pattern_get_type())
#define AGS_PATTERN(obj)                ((AgsPattern*) G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_PATTERN, AgsPattern))
#define AGS_PATTERN_CLASS(class)        ((AgsPatternClass*) G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PATTERN, AgsPatternClass))
#define AGS_IS_PATTERN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PATTERN))
#define AGS_IS_PATTERN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PATTERN))
#define AGS_PATTERN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PATTERN, AgsPatternClass))

typedef struct _AgsPattern AgsPattern;
typedef struct _AgsPatternClass AgsPatternClass;

struct _AgsPattern
{
  GObject object;

  guint dim[3];
  guint ***pattern;

  guint64 offset;
};

struct _AgsPatternClass
{
  GObjectClass object;
};

void ags_pattern_connect(AgsPattern *pattern);

AgsPattern* ags_pattern_get_by_offset(GList *list, guint64 offset);

void ags_pattern_set_dim(AgsPattern *pattern, guint dim0, guint dim1, guint lenght);

gboolean ags_pattern_get_bit(AgsPattern *pattern, guint i, guint j, guint bit);
void ags_pattern_toggle_bit(AgsPattern *pattern, guint i, guint j, guint bit);

AgsPattern* ags_pattern_new();

#endif /*__AGS_PATTERN_H__*/
