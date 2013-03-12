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
void ags_ipatch_info(AgsPlayable *playable,
		     guint *channels, guint *frames,
		     guint *loop_start, guint *loop_end,
		     GError **error);
short* ags_ipatch_read(AgsPlayable *playable, guint channel,
		       GError **error);
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
					     "AgsIpatch",
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
  GParamSpec *param_spec;

  ags_ipatch_parent_class = g_type_class_peek_parent(ipatch);

  /* GObjectClass */
  gobject = (GObjectClass *) ipatch;

  gobject->set_property = ags_ipatch_set_property;
  gobject->get_property = ags_ipatch_get_property;

  gobject->finalize = ags_ipatch_finalize;

  /* properties */
  param_spec = g_param_spec_pointer("filename",
				   "the filename",
				   "The filename to open",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  param_spec = g_param_spec_pointer("mode",
				    "the mode",
				    "The mode to open the file",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MODE,
				  param_spec);
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
  ipatch->devout = NULL;
  ipatch->audio_signal= NULL;

  ipatch->file = ipatch_file_new();

  ipatch->filename = NULL;
  ipatch->mode = AGS_IPATCH_READ;

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

      if(ipatch->handle != NULL){
	GError *error;

	error = NULL;

	ipatch_file_default_open_method(ipatch->handle,
					mode,
					&error);

	if(error != NULL){
	  g_error("%s", error->message);
	}
      }
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
ags_ipatch_open(AgsPlayable *playable, gchar *filename)
{
  AgsIpatch *ipatch;
  IpatchFileIOFuncs *io_funcs;
  GError *error;

  ipatch = AGS_IPATCH(playable);

  /*
  io_funcs = (IpatchFileIOFuncs *) g_new(IpatchFileIOFuncs, 1);
  io_funcs->open = ipatch_file_default_open_method;
  io_funcs->close = ipatch_file_default_close_method;
  io_funcs->read = ipatch_file_default_read_method;
  io_funcs->write = ipatch_file_default_write_method;
  io_funcs->seek = ipatch_file_default_seek_method;
  io_funcs->getfd = ipatch_file_default_getfd_method;
  io_funcs->get_size = ipatch_file_default_get_size_method;

  ipatch_file_set_iofuncs_static(ipatch->file,
				 io_funcs);

  */
  error = NULL;
  ipatch->handle = ipatch_file_identify_open(ipatch->filename,
					     &error);

  if(error != NULL){
    g_error("%s", error->message);
  }

  //  ipatch->container = NULL;

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
ags_ipatch_info(AgsPlayable *playable,
		guint *channels, guint *frames,
		guint *loop_start, guint *loop_end,
		GError **error)
{
  AgsIpatch *ipatch;

  ipatch = AGS_IPATCH(playable);

  /* empty */
}

short*
ags_ipatch_read(AgsPlayable *playable, guint channel,
		GError **error)
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

/**
 * ags_ipatch_read_audio_signal:
 * @ipatch an AgsIpatch
 * Returns:
 *
 * Reads an AgsAudioSignal from current sample and iterates to the next sample. Prior,
 * you should have called #ags_playable_iter_start.
 */
void
ags_ipatch_read_audio_signal(AgsIpatch *ipatch)
{
  GList *list;

  ags_playable_iter_next(AGS_PLAYABLE(ipatch->reader));
  list = ags_playable_read_audio_signal(AGS_PLAYABLE(ipatch->reader),
					ipatch->devout,
					0, 2);

  ipatch->audio_signal = list;
}

gboolean
ags_iofuncs_open(IpatchFileHandle *handle, const char *mode, GError **err)
{
}

void
ags_iofuncs_close(IpatchFileHandle *handle)
{
}

GIOStatus
ags_iofuncs_read(IpatchFileHandle *handle, gpointer buf, guint size,
		 guint *bytes_read, GError **err)
{
}

GIOStatus
ags_iofuncs_write(IpatchFileHandle *handle, gconstpointer buf, guint size,
		  GError **err)
{
}

GIOStatus
ags_iofuncs_seek(IpatchFileHandle *handle, int offset, GSeekType type, GError **err)
{
}

int
ags_iofuncs_getfd(IpatchFileHandle *handle)
{
}

int
ags_iofuncs_get_size(IpatchFile *file, GError **err)
{
}

AgsIpatch*
ags_ipatch_new()
{
  AgsIpatch *ipatch;

  ipatch = (AgsIpatch *) g_object_new(AGS_TYPE_IPATCH,
				      NULL);

  return(ipatch);
}
