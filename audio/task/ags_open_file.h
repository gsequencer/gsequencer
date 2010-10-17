#ifndef __AGS_OPEN_FILE_H__
#define __AGS_OPEN_FILE_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_task.h"

#define AGS_TYPE_OPEN_FILE                (ags_open_file_get_type())
#define AGS_OPEN_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OPEN_FILE, AgsOpenFile))
#define AGS_OPEN_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OPEN_FILE, AgsOpenFileClass))
#define AGS_IS_OPEN_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_OPEN_FILE))
#define AGS_IS_OPEN_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_OPEN_FILE))
#define AGS_OPEN_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_OPEN_FILE, AgsOpenFileClass))

typedef struct _AgsOpenFile AgsOpenFile;
typedef struct _AgsOpenFileClass AgsOpenFileClass;

struct _AgsOpenFile
{
  AgsTask task;
};

struct _AgsOpenFileClass
{
  AgsTaskClass task;
};

GType ags_open_file_get_type();

AgsOpenFile* ags_open_file_new();

#endif /*__AGS_OPEN_FILE_H__*/
