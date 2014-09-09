/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/plugin/ags_ladspa_manager.h>

#include <ags/object/ags_marshal.h>

#include <ags/plugin/ags_ladspa_manager.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>

void ags_ladspa_manager_class_init(AgsLadspaManagerClass *ladspa_manager);
void ags_ladspa_manager_init (AgsLadspaManager *ladspa_manager);
void ags_ladspa_manager_finalize(GObject *gobject);

enum{
  ADD,
  CREATE,
  LAST_SIGNAL,
};

static gpointer ags_ladspa_manager_parent_class = NULL;
static guint ladspa_manager_signals[LAST_SIGNAL];

static AgsLadspaManager *ags_ladspa_manager = NULL;
static const gchar *ags_ladspa_default_path = "/usr/lib/ladspa\0";

GType
ags_ladspa_manager_get_type (void)
{
  static GType ags_type_ladspa_manager = 0;

  if(!ags_type_ladspa_manager){
    static const GTypeInfo ags_ladspa_manager_info = {
      sizeof (AgsLadspaManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ladspa_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLadspaManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ladspa_manager_init,
    };

    ags_type_ladspa_manager = g_type_register_static(G_TYPE_OBJECT,
							"AgsLadspaManager\0",
							&ags_ladspa_manager_info,
							0);
  }

  return (ags_type_ladspa_manager);
}

void
ags_ladspa_manager_class_init(AgsLadspaManagerClass *ladspa_manager)
{
  GObjectClass *gobject;

  ags_ladspa_manager_parent_class = g_type_class_peek_parent(ladspa_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) ladspa_manager;

  gobject->finalize = ags_ladspa_manager_finalize;
}

void
ags_ladspa_manager_init(AgsLadspaManager *ladspa_manager)
{
  ladspa_manager->ladspa_plugin = NULL;
}

void
ags_ladspa_manager_finalize(GObject *gobject)
{
  AgsLadspaManager *ladspa_manager;
  GList *ladspa_plugin;

  ladspa_manager = AGS_LADSPA_MANAGER(gobject);

  ladspa_plugin = ladspa_manager->ladspa_plugin;

  g_list_free_full(ladspa_plugin,
		   ags_ladspa_plugin_free);
}

AgsLadspaPlugin*
ags_ladspa_plugin_alloc()
{
  AgsLadspaPlugin *ladspa_plugin;

  ladspa_plugin = (AgsLadspaPlugin *) malloc(sizeof(AgsLadspaPlugin));

  ladspa_plugin->flags = 0;
  ladspa_plugin->filename = NULL;
  ladspa_plugin->plugin_so = NULL;

  return(ladspa_plugin);
}

void
ags_ladspa_plugin_free(AgsLadspaPlugin *ladspa_plugin)
{
  if(ladspa_plugin->plugin_so != NULL){
    dlclose(ladspa_plugin->plugin_so);
  }

  free(ladspa_plugin->filename);
  free(ladspa_plugin);
}

gchar**
ags_ladspa_manager_get_filenames()
{
  AgsLadspaManager *ladspa_manager;
  GList *ladspa_plugin;
  gchar **filenames;
  guint length;
  guint i;

  ladspa_manager = ags_ladspa_manager_get_instance();
  length = g_list_length(ladspa_manager->ladspa_plugin);

  ladspa_plugin = ladspa_manager->ladspa_plugin;
  filenames = (gchar **) malloc((length + 1) * sizeof(gchar *));

  for(i = 0; i < length; i++){
    filenames[i] = AGS_LADSPA_PLUGIN(ladspa_plugin->data)->filename;
    ladspa_plugin = ladspa_plugin->next;
  }

  filenames[i] = NULL;

  return(filenames);
}

AgsLadspaPlugin*
ags_ladspa_manager_find_ladspa_plugin(gchar *filename)
{
  AgsLadspaManager *ladspa_manager;
  AgsLadspaPlugin *ladspa_plugin;
  GList *list;

  ladspa_manager = ags_ladspa_manager_get_instance();

  list = ladspa_manager->ladspa_plugin;

  while(list != NULL){
    ladspa_plugin = AGS_LADSPA_PLUGIN(list->data);
    if(!g_strcmp0(ladspa_plugin->filename,
		  filename)){
      return(ladspa_plugin);
    }

    list = list->next;
  }

  return(NULL);
}

void
ags_ladspa_manager_load_file(gchar *filename)
{
  AgsLadspaManager *ladspa_manager;
  AgsLadspaPlugin *ladspa_plugin;
  gchar *path;

  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  ladspa_manager = ags_ladspa_manager_get_instance();

  pthread_mutex_lock(&(mutex));

  path = g_strdup_printf("%s/%s\0",
			 ags_ladspa_default_path,
			 filename);

  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(filename);
  g_message("loading: %s\0", filename);

  if(ladspa_plugin == NULL){
    ladspa_plugin = ags_ladspa_plugin_alloc();
    ladspa_plugin->filename = filename;
    ladspa_manager->ladspa_plugin = g_list_prepend(ladspa_manager->ladspa_plugin,
						   ladspa_plugin);

    ladspa_plugin->plugin_so = dlopen(path,
				      RTLD_NOW);

    if(ladspa_plugin->plugin_so){
      dlerror();
    }
  }

  pthread_mutex_unlock(&(mutex));

  g_free(path);
}

void
ags_ladspa_manager_load_default_directory()
{
  AgsLadspaManager *ladspa_manager;
  AgsLadspaPlugin *ladspa_plugin;
  GDir *dir;
  gchar *filename;
  GError *error;

  ladspa_manager = ags_ladspa_manager_get_instance();

  error = NULL;
  dir = g_dir_open(ags_ladspa_default_path,
		   0,
		   &error);

  if(error != NULL){
    g_warning(error->message);
  }

  while((filename = g_dir_read_name(dir)) != NULL){
    if(g_str_has_suffix(filename,
			".so\0")){
      ags_ladspa_manager_load_file(filename);
    }
  }
}

AgsLadspaManager*
ags_ladspa_manager_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&(mutex));

  if(ags_ladspa_manager == NULL){
    ags_ladspa_manager = ags_ladspa_manager_new();

    pthread_mutex_unlock(&(mutex));

    ags_ladspa_manager_load_default_directory();
  }else{
    pthread_mutex_unlock(&(mutex));
  }

  return(ags_ladspa_manager);
}

AgsLadspaManager*
ags_ladspa_manager_new()
{
  AgsLadspaManager *ladspa_manager;

  ladspa_manager = (AgsLadspaManager *) g_object_new(AGS_TYPE_LADSPA_MANAGER,
						     NULL);

  return(ladspa_manager);
}
