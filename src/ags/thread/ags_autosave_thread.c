/* This file is part of GSequencer.
 * 
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/thread/ags_autosave_thread.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_main_loop.h>

#include <ags/audio/ags_devout.h>

#include <sys/types.h>
#include <pwd.h>

void ags_autosave_thread_class_init(AgsAutosaveThreadClass *autosave_thread);
void ags_autosave_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_autosave_thread_main_loop_interface_init(AgsMainLoopInterface *main_loop);
void ags_autosave_thread_init(AgsAutosaveThread *autosave_thread);
void ags_autosave_thread_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_autosave_thread_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_autosave_thread_connect(AgsConnectable *connectable);
void ags_autosave_thread_disconnect(AgsConnectable *connectable);
void ags_autosave_thread_set_tic(AgsMainLoop *main_loop, guint tic);
guint ags_autosave_thread_get_tic(AgsMainLoop *main_loop);
void ags_autosave_thread_set_last_sync(AgsMainLoop *main_loop, guint last_sync);
guint ags_autosave_thread_get_last_sync(AgsMainLoop *main_loop);
void ags_autosave_thread_finalize(GObject *gobject);

void ags_autosave_thread_start(AgsThread *thread);
void ags_autosave_thread_run(AgsThread *thread);

/**
 * SECTION:ags_autosave_thread
 * @short_description: auto safe
 * @title: AgsAutosaveThread
 * @section_id:
 * @include: ags/thread/ags_autosave_thread.h
 *
 * The #AgsAutosaveThread performs auto-safe.
 */

enum{
  PROP_0,
  PROP_AGS_MAIN,
};

static gpointer ags_autosave_thread_parent_class = NULL;
static AgsConnectableInterface *ags_autosave_thread_parent_connectable_interface;

GType
ags_autosave_thread_get_type()
{
  static GType ags_type_autosave_thread = 0;

  if(!ags_type_autosave_thread){
    static const GTypeInfo ags_autosave_thread_info = {
      sizeof (AgsAutosaveThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_autosave_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAutosaveThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_autosave_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_autosave_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_main_loop_interface_info = {
      (GInterfaceInitFunc) ags_autosave_thread_main_loop_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_autosave_thread = g_type_register_static(AGS_TYPE_THREAD,
						      "AgsAutosaveThread\0",
						      &ags_autosave_thread_info,
						      0);
    
    g_type_add_interface_static(ags_type_autosave_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_autosave_thread,
				AGS_TYPE_MAIN_LOOP,
				&ags_main_loop_interface_info);
  }
  
  return (ags_type_autosave_thread);
}

void
ags_autosave_thread_class_init(AgsAutosaveThreadClass *autosave_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;

  ags_autosave_thread_parent_class = g_type_class_peek_parent(autosave_thread);

  /* GObject */
  gobject = (GObjectClass *) autosave_thread;

  gobject->get_property = ags_autosave_thread_get_property;
  gobject->set_property = ags_autosave_thread_set_property;

  gobject->finalize = ags_autosave_thread_finalize;

  /* properties */
  param_spec = g_param_spec_object("ags_main\0",
				   "ags_main to check against\0",
				   "The ags_main to check against serialization.\0",
				   AGS_TYPE_MAIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AGS_MAIN,
				  param_spec);

  /* AgsThread */
  thread = (AgsThreadClass *) autosave_thread;

  thread->start = ags_autosave_thread_start;
  thread->run = ags_autosave_thread_run;
}

void
ags_autosave_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_autosave_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_autosave_thread_connect;
  connectable->disconnect = ags_autosave_thread_disconnect;
}

void
ags_autosave_thread_main_loop_interface_init(AgsMainLoopInterface *main_loop)
{
  main_loop->set_tic = ags_autosave_thread_set_tic;
  main_loop->get_tic = ags_autosave_thread_get_tic;
  main_loop->set_last_sync = ags_autosave_thread_set_last_sync;
  main_loop->get_last_sync = ags_autosave_thread_get_last_sync;
}

void
ags_autosave_thread_init(AgsAutosaveThread *autosave_thread)
{
  AgsThread *thread;

  thread = AGS_THREAD(autosave_thread);

  g_atomic_int_or(&(thread->flags), 
		  AGS_THREAD_READY);

  thread->freq = AGS_AUTOSAVE_THREAD_DEFAULT_JIFFIE;


  g_atomic_int_set(&(autosave_thread->tic), 0);
  g_atomic_int_set(&(autosave_thread->last_sync), 0);

  autosave_thread->ags_main = NULL;
  autosave_thread->counter = 0;
}

void
ags_autosave_thread_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsAutosaveThread *autosave_thread;

  autosave_thread = AGS_AUTOSAVE_THREAD(gobject);

  switch(prop_id){
  case PROP_AGS_MAIN:
    {
      AgsMain *ags_main;

      ags_main = g_value_get_object(value);

      if(autosave_thread->ags_main == ags_main){
	return;
      }

      if(autosave_thread->ags_main != NULL){
	g_object_unref(autosave_thread->ags_main);
      }

      if(ags_main != NULL){
	g_object_ref(ags_main);
      }

      autosave_thread->ags_main = ags_main;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_autosave_thread_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsAutosaveThread *autosave_thread;

  autosave_thread = AGS_AUTOSAVE_THREAD(gobject);

  switch(prop_id){
  case PROP_AGS_MAIN:
    {
      g_value_set_object(value, autosave_thread->ags_main);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_autosave_thread_connect(AgsConnectable *connectable)
{
  ags_autosave_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_autosave_thread_disconnect(AgsConnectable *connectable)
{
  ags_autosave_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_autosave_thread_set_tic(AgsMainLoop *main_loop, guint tic)
{
  g_atomic_int_set(&(AGS_AUTOSAVE_THREAD(main_loop)->tic),
		   tic);
}

guint
ags_autosave_thread_get_tic(AgsMainLoop *main_loop)
{
  return(g_atomic_int_get(&(AGS_AUTOSAVE_THREAD(main_loop)->tic)));
}

void
ags_autosave_thread_set_last_sync(AgsMainLoop *main_loop, guint last_sync)
{
  g_atomic_int_set(&(AGS_AUTOSAVE_THREAD(main_loop)->last_sync),
		   last_sync);
}

guint
ags_autosave_thread_get_last_sync(AgsMainLoop *main_loop)
{
  gint val;

  val = g_atomic_int_get(&(AGS_AUTOSAVE_THREAD(main_loop)->last_sync));

  return(val);
}

void
ags_autosave_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_autosave_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_autosave_thread_start(AgsThread *thread)
{
  AGS_THREAD_CLASS(ags_autosave_thread_parent_class)->start(thread);
}

void
ags_autosave_thread_run(AgsThread *thread)
{
  AgsAutosaveThread *autosave_thread;

  static const struct timespec delay = {
    1,
    0,
  };

  autosave_thread = AGS_AUTOSAVE_THREAD(thread);

  if(autosave_thread->counter != autosave_thread->delay){
    autosave_thread->counter += 1;
  }else{
    AgsFile *file;
    struct passwd *pw;
    uid_t uid;
    gchar *filename;

    autosave_thread->counter = 0;

    uid = getuid();
    pw = getpwuid(uid);

    filename = g_strdup_printf("%s/%s/%d-%s\0",
			       pw->pw_dir,
			       AGS_DEFAULT_DIRECTORY,
			       getpid(),
			       AGS_AUTOSAVE_THREAD_DEFAULT_FILENAME);
    
    file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				    "main\0", autosave_thread->ags_main,
				    "filename\0", filename,
				    NULL);
    ags_file_write_concurrent(file);
    g_object_unref(file);
  }

  nanosleep(&delay, NULL);
}

/**
 * ags_autosave_thread_new:
 * @devout: the #AgsDevout
 * @ags_main: the #AgsMain
 *
 * Create a new #AgsAutosaveThread.
 *
 * Returns: the new #AgsAutosaveThread
 *
 * Since: 0.4
 */
AgsAutosaveThread*
ags_autosave_thread_new(GObject *devout, AgsMain *ags_main)
{
  AgsAutosaveThread *autosave_thread;

  autosave_thread = (AgsAutosaveThread *) g_object_new(AGS_TYPE_AUTOSAVE_THREAD,
						       "devout\0", devout,
						       "ags-main\0", ags_main,
						       NULL);

  return(autosave_thread);
}
