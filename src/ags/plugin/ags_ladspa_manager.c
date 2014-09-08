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

void ags_ladspa_manager_class_init(AgsLadspaManagerClass *ladspa_manager);
void ags_ladspa_manager_init (AgsLadspaManager *ladspa_manager);
void ags_ladspa_manager_finalize(GObject *gobject);

void ags_ladspa_manager_real_add(AgsLadspaManager *ladspa_manager,
				 GType ladspa_type, gchar *ladspa_name,
				 gchar *version, gchar *build_id,
				 gchar *xml_type,
				 GParameter *control, guint control_count);
AgsLadspa* ags_ladspa_manager_real_create(AgsLadspaManager *ladspa_manager,
					  gchar *ladspa_name, gchar *version, gchar *build_id);

enum{
  ADD,
  CREATE,
  LAST_SIGNAL,
};

static gpointer ags_ladspa_manager_parent_class = NULL;
static guint ladspa_manager_signals[LAST_SIGNAL];

static AgsLadspaManager *ags_ladspa_manager = NULL;

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

  /*  */
  ladspa_manager->add = ags_ladspa_manager_real_add;
  ladspa_manager->create = ags_ladspa_manager_real_create;

  ladspa_manager_signals[ADD] =
    g_signal_new("add\0",
		 G_TYPE_FROM_CLASS (ladspa_manager),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsLadspaManagerClass, add),
		 NULL, NULL,
		 g_cclosure_user_marshal_STRING__ULONG_STRING_STRING_STRING_STRING_UINT_POINTER,
		 G_TYPE_OBJECT, 7,
		 G_TYPE_ULONG,
		 G_TYPE_STRING,
		 G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
		 G_TYPE_UINT, G_TYPE_POINTER);

  ladspa_manager_signals[CREATE] =
    g_signal_new("create\0",
		 G_TYPE_FROM_CLASS (ladspa_manager),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsLadspaManagerClass, create),
		 NULL, NULL,
		 g_cclosure_user_marshal_OBJECT__STRING_STRING_STRING,
		 G_TYPE_OBJECT, 3,
		 G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
}

void
ags_ladspa_manager_init(AgsLadspaManager *ladspa_manager)
{
  ladspa_manager->ladspa = NULL;
}

void
ags_ladspa_manager_finalize(GObject *gobject)
{
  //TODO:JK: implement me
}

void
ags_ladspa_manager_load_file(AgsLadspaManager *ladspa_manager,
			     gchar *filename)
{
  //TODO:JK: implement me
}

void
ags_ladspa_manager_load_default_directory(AgsLadspaManager *ladspa_manager)
{
  //TODO:JK: implement me
}

AgsLadspaManager*
ags_ladspa_manager_get_instance()
{
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&(mutex));

  if(ags_ladspa_manager == NULL){
    ags_ladspa_manager = ags_ladspa_manager_new();
  }

  pthread_mutex_unlock(&(mutex));

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
