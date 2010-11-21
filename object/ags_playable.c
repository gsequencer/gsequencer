#include <ags/object/ags_playable.h>

void ags_playable_base_init(AgsPlayableInterface *interface);

GType
ags_playable_get_type()
{
  static GType ags_type_playable = 0;

  if(!ags_type_playable){
    static const GTypeInfo ags_playable_info = {
      sizeof(AgsPlayableInterface),
      (GBaseInitFunc) ags_playable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_playable = g_type_register_static(G_TYPE_INTERFACE,
					       "AgsPlayable\0", &ags_playable_info,
					       0);
  }

  return(ags_type_playable);
}

void
ags_playable_base_init(AgsPlayableInterface *interface)
{
  /* empty */
}

gboolean
ags_playable_open(AgsPlayable *playable, gchar *name)
{
  AgsPlayableInterface *playable_interface;
  gboolean ret_val;

  g_return_val_if_fail(AGS_IS_PLAYABLE(playable), FALSE);
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_val_if_fail(playable_interface->open, FALSE);
  ret_val = playable_interface->open(playable, name);

  return(ret_val);
}

void
ags_playable_info(AgsPlayable *playable, guint *channels, guint *frames)
{
  AgsPlayableInterface *playable_interface;

  g_return_if_fail(AGS_IS_PLAYABLE(playable));
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_if_fail(playable_interface->info);
  playable_interface->info(playable, channels, frames);
}

short*
ags_playable_read(AgsPlayable *playable, guint channel)
{
  AgsPlayableInterface *playable_interface;

  g_return_if_fail(AGS_IS_PLAYABLE(playable));
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_if_fail(playable_interface->read);
  playable_interface->read(playable, channel);
}

void
ags_playable_close(AgsPlayable *playable)
{
  AgsPlayableInterface *playable_interface;

  g_return_if_fail(AGS_IS_PLAYABLE(playable));
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_if_fail(playable_interface->close);
  playable_interface->close(playable);
}

