#include "ags_sndfile.h"

#include "../../object/ags_connectable.h"
#include "../../object/ags_playable.h"

void ags_sndfile_class_init(AgsSndfileClass *sndfile);
void ags_sndfile_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sndfile_playable_interface_init(AgsPlayableInterface *playable);
void ags_sndfile_init(AgsSndfile *sndfile);
void ags_sndfile_connect(AgsConnectable *connectable);
void ags_sndfile_disconnect(AgsConnectable *connectable);
void ags_sndfile_open(AgsPlayable *playable, gchar *name);
void ags_sndfile_info(AgsPlayable *playable, guint *channels, guint *frames);
short* ags_sndfile_read(AgsPlayable *playable, guint channel);
void ags_sndfile_close(AgsPlayable *playable);
void ags_sndfile_finalize(GObject *gobject);

static gpointer ags_sndfile_parent_class = NULL;
static AgsConnectableInterface *ags_sndfile_parent_connectable_interface;
static AgsPlayableInterface *ags_sndfile_parent_playable_interface;

GType
ags_sndfile_get_type()
{
  static GType ags_type_sndfile = 0;

  if(!ags_type_sndfile){
    static const GTypeInfo ags_sndfile_info = {
      sizeof (AgsSndfileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sndfile_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSndfile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sndfile_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sndfile_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_playable_interface_info = {
      (GInterfaceInitFunc) ags_sndfile_playable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_sndfile = g_type_register_static(G_TYPE_OBJECT,
					      "AgsSndfile\0",
					      &ags_sndfile_info,
					      0);

    g_type_add_interface_static(ags_type_sndfile,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_sndfile,
				AGS_TYPE_PLAYABLE,
				&ags_playable_interface_info);
  }
  
  return (ags_type_sndfile);
}

void
ags_sndfile_class_init(AgsSndfileClass *sndfile)
{
  GObjectClass *gobject;

  ags_sndfile_parent_class = g_type_class_peek_parent(sndfile);

  gobject = (GObjectClass *) sndfile;

  gobject->finalize = ags_sndfile_finalize;
}

void
ags_sndfile_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_sndfile_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_sndfile_connect;
  connectable->disconnect = ags_sndfile_disconnect;
}

void
ags_sndfile_playable_interface_init(AgsPlayableInterface *playable)
{
  ags_sndfile_parent_playable_interface = g_type_interface_peek_parent(playable);

  playable->open = ags_sndfile_open;
  playable->info = ags_sndfile_info;
  playable->read = ags_sndfile_read;
  playable->close = ags_sndfile_close;
}

void
ags_sndfile_init(AgsSndfile *sndfile)
{
  sndfile->info = NULL;
}

void
ags_sndfile_connect(AgsConnectable *connectable)
{
  ags_sndfile_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_sndfile_disconnect(AgsConnectable *connectable)
{
  ags_sndfile_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_sndfile_open(AgsPlayable *playable, gchar *name)
{
  AgsSndfile *sndfile;

  sndfile = AGS_SNDFILE(playable);

  sndfile->info = (SF_INFO *) malloc(sizeof(SF_INFO));

  sndfile->file = (SNDFILE *) sf_open(name, SFM_READ, sndfile->info);

  if(sndfile->file == NULL)
    return(FALSE);
  else
    return(TRUE);
}

void
ags_sndfile_info(AgsPlayable *playable, guint *channels, guint *frames)
{
  AgsSndfile *sndfile;

  sndfile = AGS_SNDFILE(playable);

  *channels = sndfile->info->channels;
  *frames = sndfile->info->frames;
}

short*
ags_sndfile_read(AgsPlayable *playable, guint channel)
{
  AgsSndfile *sndfile;
  short *item;
  short *buffer;
  guint i;

  sndfile = AGS_SNDFILE(playable);

  item = (short *) malloc(sndfile->info->channels * sizeof(short));
  buffer = (short *) malloc(sndfile->info->frames * sizeof(short));

  for(i = 0; i < sndfile->info->frames; i++){
    sf_read_short(sndfile->file, item, sndfile->info->channels);
    buffer[i] = item[channel];
    sf_seek(sndfile->file, 1, SEEK_CUR);
  }

  free(item);

  return(buffer);
}

void
ags_sndfile_close(AgsPlayable *playable)
{
  AgsSndfile *sndfile;

  sndfile = AGS_SNDFILE(playable);


  sf_close(sndfile->file);
  free(sndfile->info);
}

void
ags_sndfile_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_sndfile_parent_class)->finalize(gobject);

  /* empty */
}

AgsSndfile*
ags_sndfile_new()
{
  AgsSndfile *sndfile;

  sndfile = (AgsSndfile *) g_object_new(AGS_TYPE_SNDFILE,
					NULL);

  return(sndfile);
}
