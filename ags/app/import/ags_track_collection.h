/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_TRACK_COLLECTION_H__
#define __AGS_TRACK_COLLECTION_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <libxml/tree.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/import/ags_track_mapper.h>

G_BEGIN_DECLS

#define AGS_TYPE_TRACK_COLLECTION                (ags_track_collection_get_type())
#define AGS_TRACK_COLLECTION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TRACK_COLLECTION, AgsTrackCollection))
#define AGS_TRACK_COLLECTION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TRACK_COLLECTION, AgsTrackCollectionClass))
#define AGS_IS_TRACK_COLLECTION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TRACK_COLLECTION))
#define AGS_IS_TRACK_COLLECTION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TRACK_COLLECTION))
#define AGS_TRACK_COLLECTION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_TRACK_COLLECTION, AgsTrackCollectionClass))

typedef struct _AgsTrackCollection AgsTrackCollection;
typedef struct _AgsTrackCollectionClass AgsTrackCollectionClass;

struct _AgsTrackCollection
{
  GtkBox box;

  xmlDoc *midi_doc;

  guint first_offset;
  gdouble bpm;

  glong tempo;
  glong division;
  
  guint default_length;
  
  GType track_mapper_type;
  
  guint track_mapper_n_properties;

  gchar **track_mapper_strv;
  GValue *track_mapper_value;

  GList *track_mapper;
  
  GtkBox *track_mapper_box;
};

struct _AgsTrackCollectionClass
{
  GtkBoxClass box;
};

GType ags_track_collection_get_type();

GList* ags_track_collection_get_track_mapper(AgsTrackCollection *track_collection);
void ags_track_collection_add_track_mapper(AgsTrackCollection *track_collection,
					   AgsTrackMapper *track_mapper);
void ags_track_collection_remove_track_mapper(AgsTrackCollection *track_collection,
					      AgsTrackMapper *track_mapper);

void ags_track_collection_parse(AgsTrackCollection *track_collection);
void ags_track_collection_add_mapper(AgsTrackCollection *track_collection,
				     xmlNode *track,
				     gchar *instrument, gchar *sequence);

AgsTrackCollection* ags_track_collection_new(GType track_mapper_type,
					     guint track_mapper_n_properties,
					     gchar **track_mapper_strv,
					     GValue *track_mapper_value);

G_END_DECLS

#endif /*__AGS_TRACK_COLLECTION_H__*/


