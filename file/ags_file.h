#ifndef __AGS_FILE_H__
#define __AGS_FILE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <libxml/tree.h>

#define AGS_TYPE_FILE                (ags_file_get_type())
#define AGS_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FILE, AgsFile))
#define AGS_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FILE, AgsFileClass))

typedef struct _AgsFile AgsFile;
typedef struct _AgsFileClass AgsFileClass;

typedef enum{
  AGS_FILE_READ,
  AGS_FILE_WRITE,
}AgsFileFlags;

struct _AgsFile
{
  GObject object;

  guint flags;

  char *name;
  char *encoding;
  char *dtd;
  xmlDocPtr doc;
  xmlNodePtr current;

  GtkWidget *window;
};

struct _AgsFileClass
{
  GObjectClass object;
};

void ags_file_write(AgsFile *file);
void ags_file_read(AgsFile *file);

AgsFile* ags_file_new();

#endif /*__AGS_FILE_H__*/
