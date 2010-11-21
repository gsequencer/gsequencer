#ifndef __AGS_SNDFILE_H__
#define __AGS_SNDFILE_H__

#include <glib.h>
#include <glib-object.h>
#include <sndfile.h>

#include <ags/audio/ags_devout.h>

#define AGS_TYPE_SNDFILE                (ags_sndfile_get_type())
#define AGS_SNDFILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SNDFILE, AgsSndfile))
#define AGS_SNDFILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SNDFILE, AgsSndfileClass))
#define AGS_IS_SNDFILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SNDFILE))
#define AGS_IS_SNDFILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SNDFILE))
#define AGS_SNDFILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SNDFILE, AgsSndfileClass))

typedef struct _AgsSndfile AgsSndfile;
typedef struct _AgsSndfileClass AgsSndfileClass;

struct _AgsSndfile
{
  GObject object;

  SF_INFO *info;
  SNDFILE *file;
};

struct _AgsSndfileClass
{
  GObjectClass object;
};

GType ags_sndfile_get_type();

AgsSndfile* ags_sndfile_new();

#endif /*__AGS_SNDFILE_H__*/
