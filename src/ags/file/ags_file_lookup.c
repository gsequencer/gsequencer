/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/file/ags_file_lookup.h>

void ags_file_lookup_class_init(AgsFileLookupClass *file_lookup);
void ags_file_lookup_init (AgsFileLookup *file_lookup);
void ags_file_lookup_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_file_lookup_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_file_lookup_finalize(GObject *gobject);

enum{
  RESOLVE,
  LAST_SIGNAL,
};

static gpointer ags_file_lookup_parent_class = NULL;
static guint file_lookup_signals[LAST_SIGNAL];

GType
ags_file_lookup_get_type (void)
{
  static GType ags_type_file_lookup = 0;

  if(!ags_type_file_lookup){
    static const GTypeInfo ags_file_lookup_info = {
      sizeof (AgsFileLookupClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_file_lookup_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFileLookup),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_file_lookup_init,
    };

    ags_type_file_lookup = g_type_register_static(G_TYPE_OBJECT,
						  "AgsFileLookup\0",
						  &ags_file_lookup_info,
						  0);
  }

  return (ags_type_file_lookup);
}

void
ags_file_lookup_class_init(AgsFileLookupClass *file_lookup)
{
  GObjectClass *gobject;

  ags_file_lookup_parent_class = g_type_class_peek_parent(file_lookup);

  /* GObjectClass */
  gobject = (GObjectClass *) file_lookup;

  gobject->get_property = ags_file_lookup_get_property;
  gobject->set_property = ags_file_lookup_set_property;

  gobject->finalize = ags_file_lookup_finalize;

  /* AgsFileLookupClass */
  file_lookup->resolve = NULL;

  file_lookup_signals[RESOLVE] =
    g_signal_new("resolve\0",
		 G_TYPE_FROM_CLASS(file_lookup),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileLookupClass, resolve),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_file_lookup_init(AgsFileLookup *file_lookup)
{
  file_lookup->data = NULL;
}

void
ags_file_lookup_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  //TODO:JK: implement me
}

void
ags_file_lookup_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  //TODO:JK: implement me
}

void
ags_file_lookup_finalize(GObject *gobject)
{
}

void
ags_file_lookup_resolve(AgsFileLookup *lookup)
{
  g_return_if_fail(AGS_IS_FILE_LOOKUP(file_lookup));

  g_object_ref((GObject *) file_lookup);
  g_signal_emit(G_OBJECT(file_lookup),
		file_lookup_signals[RESOLVE], 0);
  g_object_unref((GObject *) file_lookup);
}

AgsFileLookup*
ags_file_lookup_new()
{
  AgsFileLookup *file_lookup;

  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       NULL);
  
  return(file_lookup);
}
