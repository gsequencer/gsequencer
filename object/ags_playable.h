#ifndef __AGS_PLAYABLE_H__
#define __AGS_PLAYABLE_H__

#include <glib-object.h>

#define AGS_TYPE_PLAYABLE                    (ags_playable_get_type())
#define AGS_PLAYABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAYABLE, AgsPlayable))
#define AGS_PLAYABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_PLAYABLE, AgsPlayableInterface))
#define AGS_IS_PLAYABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PLAYABLE))
#define AGS_IS_PLAYABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_PLAYABLE))
#define AGS_PLAYABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_PLAYABLE, AgsPlayableInterface))

typedef void AgsPlayable;
typedef struct _AgsPlayableInterface AgsPlayableInterface;

struct _AgsPlayableInterface
{
  GTypeInterface interface;

  gboolean (*open)(AgsPlayable *playable, gchar *name);
  void (*info)(AgsPlayable *playable, guint *channels, guint *frames);
  short* (*read)(AgsPlayable *playable, guint channel);
  void (*close)(AgsPlayable *playable);
};

GType ags_playable_get_type();

gboolean ags_playable_open(AgsPlayable *playable, gchar *name);
void ags_playable_info(AgsPlayable *playable, guint *channels, guint *frames);
short* ags_playable_read(AgsPlayable *playable, guint channel);
void ags_playable_close(AgsPlayable *playable);

#endif /*__AGS_PLAYABLE_H__*/
