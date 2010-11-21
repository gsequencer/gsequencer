#ifndef __AGS_GARBAGE_COLLECTOR_H__
#define __AGS_GARBAGE_COLLECTOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_recycling.h>

#define AGS_TYPE_GARBAGE_COLLECTOR                (ags_garbage_collector_get_type())
#define AGS_GARBAGE_COLLECTOR(obj)                ((AgsGarbage_Collector*) G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_GARBAGE_COLLECTOR, AgsGarbageCollector))
#define AGS_GARBAGE_COLLECTOR_CLASS(class)        ((AgsGarbage_CollectorClass*) G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_GARBAGE_COLLECTOR, AgsGarbageCollectorClass))

typedef struct _AgsGarbageCollector AgsGarbageCollector;
typedef struct _AgsGarbageCollectorClass AgsGarbageCollectorClass;

typedef enum{
  AGS_GARBAGE_COLLECTOR_RUN         = 1, // cancel flag of ags_garbage_collector run
  AGS_GARBAGE_COLLECTOR_CANCEL_RUN  = 1 <<  1, // used by ags_garbage_collector_remove to remove the current (stopped) AgsRecycling while ags_garbage_collector_run is running
}AgsGarbageCollectorFlags;

struct _AgsGarbageCollector
{
  GObject object;

  guint32 flags;

  GObject *devout;

  GList *list; // data of type AgsRecycling
  GList *stopped;
  GList *start;
  GList *end;
};

struct _AgsGarbageCollectorClass
{
  GObjectClass object;
};

GType ags_garbage_collector_get_type();

void ags_garbage_collector_add(AgsGarbageCollector *garbage_collector, AgsRecycling *recycling);
void ags_garbage_collector_remove(AgsGarbageCollector *garbage_collector, AgsRecycling *recycling);

void ags_garbage_collector_run(AgsGarbageCollector *garbage_collector);

AgsGarbageCollector* ags_garbage_collector_new();

#endif /*__AGS_GARBAGE_COLLECTOR_H__*/
