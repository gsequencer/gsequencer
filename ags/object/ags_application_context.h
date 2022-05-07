/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
 *
 * This file is part of GSequencer.
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

#ifndef __AGS_APPLICATION_CONTEXT_H__
#define __AGS_APPLICATION_CONTEXT_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#include <ags/lib/ags_uuid.h>

#include <ags/object/ags_config.h>

#include <unistd.h>

G_BEGIN_DECLS

#define AGS_TYPE_APPLICATION_CONTEXT                (ags_application_context_get_type())
#define AGS_TYPE_APPLICATION_CONTEXT_FLAGS          (ags_application_context_flags_get_type())
#define AGS_APPLICATION_CONTEXT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_APPLICATION_CONTEXT, AgsApplicationContext))
#define AGS_APPLICATION_CONTEXT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_APPLICATION_CONTEXT, AgsApplicationContextClass))
#define AGS_IS_APPLICATION_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_APPLICATION_CONTEXT))
#define AGS_IS_APPLICATION_CONTEXT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_APPLICATION_CONTEXT))
#define AGS_APPLICATION_CONTEXT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_APPLICATION_CONTEXT, AgsApplicationContextClass))

#define AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(obj) (&(((AgsApplicationContext *) obj)->obj_mutex))

#ifndef PACKAGE_VERSION
#define AGS_VERSION "3.0.0"
#else
#define AGS_VERSION PACKAGE_VERSION
#endif

#define AGS_BUILD_ID "Wed Nov  6 18:28:15 UTC 2019"

#define AGS_DEFAULT_DIRECTORY ".gsequencer"
#define AGS_DEFAULT_CONFIG "ags.conf"

typedef struct _AgsApplicationContext AgsApplicationContext;
typedef struct _AgsApplicationContextClass AgsApplicationContextClass;

/**
 * AgsApplicationContextFlags:
 * @AGS_APPLICATION_CONTEXT_ADDED_TO_REGISTRY: indicates the application context was added to #AgsRegistry
 * @AGS_APPLICATION_CONTEXT_CONNECTED: indicates the application context was connected by calling #AgsConnectable::connect()
 * @AGS_APPLICATION_CONTEXT_TYPES_REGISTERED: indicates the types have been registered
 * 
 * Enum values to control the behavior or indicate internal state of #AgsApplicationContext by
 * enable/disable as flags.
 */
typedef enum{
  AGS_APPLICATION_CONTEXT_ADDED_TO_REGISTRY       = 1,
  AGS_APPLICATION_CONTEXT_CONNECTED               = 1 <<  1,
  AGS_APPLICATION_CONTEXT_TYPES_REGISTERED        = 1 <<  2,
}AgsApplicationContextFlags;

struct _AgsApplicationContext
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  AgsUUID *uuid;
  
  gchar *version;
  gchar *build_id;

  int argc;
  char **argv;

  volatile int is_ready;
  
  GObject *log;

  gchar *domain;
  
  AgsConfig *config;
  
  GObject *main_loop;
  GObject *task_launcher;
  
  GObject *file;
  GObject *history;
};

struct _AgsApplicationContextClass
{
  GObjectClass gobject;

  void (*load_config)(AgsApplicationContext *application_context);
  
  void (*prepare)(AgsApplicationContext *application_context);
  void (*setup)(AgsApplicationContext *application_context);

  void (*register_types)(AgsApplicationContext *application_context);

  void (*read)(GObject *file, xmlNode *node, GObject **gobject);
  xmlNode* (*write)(GObject *file, xmlNode *parent, GObject *gobject);

  void (*quit)(AgsApplicationContext *application_context);
};

GType ags_application_context_get_type();
GType ags_application_context_flags_get_type();

gboolean ags_application_context_test_flags(AgsApplicationContext *application_context, guint flags);
void ags_application_context_set_flags(AgsApplicationContext *application_context, guint flags);
void ags_application_context_unset_flags(AgsApplicationContext *application_context, guint flags);

void ags_application_context_load_config(AgsApplicationContext *application_context);

void ags_application_context_prepare(AgsApplicationContext *application_context);
void ags_application_context_setup(AgsApplicationContext *application_context);

void ags_application_context_register_types(AgsApplicationContext *application_context);

void ags_application_context_quit(AgsApplicationContext *application_context);

AgsApplicationContext* ags_application_context_get_instance();
AgsApplicationContext* ags_application_context_new(GObject *main_loop,
						   AgsConfig *config);

G_END_DECLS

#endif /*__AGS_APPLICATION_CONTEXT_H__*/
