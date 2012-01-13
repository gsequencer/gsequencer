/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/file/ags_ipatch.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_playable.h>

void ags_ipatch_class_init(AgsIpatchClass *ipatch);
void ags_ipatch_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ipatch_playable_interface_init(AgsPlayableInterface *playable);
void ags_ipatch_init(AgsIpatch *ipatch);
void ags_ipatch_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_ipatch_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_ipatch_finalize(GObject *gobject);

void ags_ipatch_connect(AgsConnectable *connectable);
void ags_ipatch_disconnect(AgsConnectable *connectable);

gboolean ags_ipatch_open(AgsPlayable *playable, gchar *name);
guint ags_ipatch_level_count(AgsPlayable *playable);
gchar** ags_ipatch_sublevel_names(AgsPlayable *playable);
void ags_ipatch_iter_start(AgsPlayable *playable);
gboolean ags_ipatch_iter_next(AgsPlayable *playable);
void ags_ipatch_info(AgsPlayable *playable, guint *channels, guint *frames, guint *loop_start, guint *loop_end);
short* ags_ipatch_read(AgsPlayable *playable, guint channel);
void ags_ipatch_close(AgsPlayable *playable);

static gpointer ags_ipatch_parent_class = NULL;
static AgsConnectableInterface *ags_ipatch_parent_connectable_interface;
static AgsPlayableInterface *ags_ipatch_parent_playable_interface;

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_MODE,
};

GType
ags_ipatch_get_type()
{
  static GType ags_type_ipatch = 0;

  if(!ags_type_ipatch){
    static const GTypeInfo ags_ipatch_info = {
      sizeof (AgsIpatchClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ipatch_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsIpatch),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ipatch_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ipatch_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_playable_interface_info = {
      (GInterfaceInitFunc) ags_ipatch_playable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ipatch = g_type_register_static(G_TYPE_OBJECT,
					     "AgsIpatch\0",
					     &ags_ipatch_info,
					     0);

    g_type_add_interface_static(ags_type_ipatch,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ipatch,
				AGS_TYPE_PLAYABLE,
				&ags_playable_interface_info);
  }
  
  return (ags_type_ipatch);
}

void
ags_ipatch_class_init(AgsIpatchClass *ipatch)
{
  GObjectClass *gobject;

  ags_ipatch_parent_class = g_type_class_peek_parent(ipatch);

  gobject = (GObjectClass *) ipatch;

  gobject->set_property = ags_ipatch_set_property;
  gobject->get_property = ags_ipatch_get_property;

  gobject->finalize = ags_ipatch_finalize;
}

void
ags_ipatch_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_ipatch_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_ipatch_connect;
  connectable->disconnect = ags_ipatch_disconnect;
}

void
ags_ipatch_playable_interface_init(AgsPlayableInterface *playable)
{
  ags_ipatch_parent_playable_interface = g_type_interface_peek_parent(playable);

  playable->open = ags_ipatch_open;

  playable->level_count = ags_ipatch_level_count;
  playable->sublevel_names = ags_ipatch_sublevel_names;
  playable->level_select = NULL;

  playable->iter_start = ags_ipatch_iter_start;
  playable->iter_next = ags_ipatch_iter_next;

  playable->info = ags_ipatch_info;
  playable->read = ags_ipatch_read;

  playable->close = ags_ipatch_close;
}

void
ags_ipatch_init(AgsIpatch *ipatch)
{
  ipatch->file = ipatch_file_new();

  ipatch->filename = NULL;
  ipatch->mode = NULL;

  ipatch->handle = NULL;
  ipatch->error = NULL;

  ipatch->container = NULL;
  ipatch->reader = NULL;
}

void
ags_ipatch_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = (gchar *) g_value_get_pointer(value);

      ipatch->filename = filename;
    }
    break;
  case PROP_MODE:
    {
      gchar *mode;
      
      mode = (gchar *) g_value_get_pointer(value);
      
      ipatch->mode = mode;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ipatch_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    g_value_set_pointer(value, ipatch->filename);
    break;
  case PROP_MODE:
    g_value_set_pointer(value, ipatch->mode);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ipatch_connect(AgsConnectable *connectable)
{
  ags_ipatch_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_ipatch_disconnect(AgsConnectable *connectable)
{
  ags_ipatch_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

gboolean
ags_ipatch_open(AgsPlayable *playable, gchar *name)
{
  AgsIpatch *ipatch;
  GError *error;

  ipatch = AGS_IPATCH(playable);

  error = NULL;
  ipatch->handle = ipatch_file_open(ipatch->file,
				    ipatch->filename,
				    ipatch->mode,
				    &error);

  ipatch->container = NULL;

  if(error == NULL){
    return(TRUE);
  }else{
    return(FALSE);
  }
}

guint
ags_ipatch_level_count(AgsPlayable *playable)
{
  /* empty */

  return(0);
}

gchar**
ags_ipatch_sublevel_names(AgsPlayable *playable)
{
  /* empty */

  return(NULL);
}

void
ags_ipatch_iter_start(AgsPlayable *playable)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(playable);

  /* empty */
}

gboolean
ags_ipatch_iter_next(AgsPlayable *playable)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(playable);

  /* empty */
}

void
ags_ipatch_info(AgsPlayable *playable, guint *channels, guint *frames, guint *loop_start, guint *loop_end)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(playable);

  /* empty */
}

short*
ags_ipatch_read(AgsPlayable *playable, guint channel)
{
  AgsIpatch *ipatch;
  short *buffer, *source;
  guint i;

  ipatch = AGS_IPATCH(playable);

  /* empty */
}

void
ags_ipatch_close(AgsPlayable *playable)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(playable);

  /* empty */
}

void
ags_ipatch_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_ipatch_parent_class)->finalize(gobject);

  /* empty */
}

AgsIpatch*
ags_ipatch_new()
{
  AgsIpatch *ipatch;

  ipatch = (AgsIpatch *) g_object_new(AGS_TYPE_IPATCH,
				      NULL);

  return(ipatch);
}
