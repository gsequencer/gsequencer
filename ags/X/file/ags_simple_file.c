/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/file/ags_simple_file.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_marshal.h>

#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_launch.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

void ags_simple_file_class_init(AgsSimpleFileClass *simple_file);
void ags_simple_file_init(AgsSimpleFile *simple_file);
void ags_simple_file_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_simple_file_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_simple_file_finalize(GObject *gobject);

void ags_simple_file_real_open(AgsSimpleFile *simple_file,
			       GError **error);
void ags_simple_file_real_open_from_data(AgsSimpleFile *simple_file,
					 gchar *data, guint length,
					 GError **error);
void ags_simple_file_real_rw_open(AgsSimpleFile *simple_file,
				  gboolean create,
				  GError **error);

void ags_simple_file_real_write(AgsSimpleFile *simple_file);
void ags_simple_file_real_write_concurrent(AgsSimpleFile *simple_file);
void ags_simple_file_real_write_resolve(AgsSimpleFile *simple_file);

void ags_simple_file_real_read(AgsSimpleFile *simple_file);
void ags_simple_file_real_read_resolve(AgsSimpleFile *simple_file);
void ags_simple_file_real_read_start(AgsSimpleFile *simple_file);

/**
 * SECTION:ags_file
 * @short_description: read/write XML file
 * @title: AgsFile
 * @section_id:
 * @include: ags/file/ags_file.h
 *
 * The #AgsFile is an object to read or write files using XML. It
 * is the persisting layer of Advanced Gtk+ Sequencer.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_ENCODING,
  PROP_AUDIO_FORMAT,
  PROP_AUDIO_ENCODING,
  PROP_XML_DOC,
  PROP_APPLICATION_CONTEXT,
};

enum{
  OPEN,
  OPEN_FROM_DATA,
  RW_OPEN,
  WRITE,
  WRITE_CONCURRENT,
  WRITE_RESOLVE,
  READ,
  READ_RESOLVE,
  READ_START,
  LAST_SIGNAL,
};

static gpointer ags_file_parent_class = NULL;
static guint file_signals[LAST_SIGNAL] = { 0 };

GType
ags_simple_file_get_type(void)
{
  //TODO:JK: implement me
}

void
ags_simple_file_class_init(AgsSimpleFileClass *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_init(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  //TODO:JK: implement me
}

void
ags_simple_file_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  //TODO:JK: implement me
}

void
ags_simple_file_finalize(GObject *gobject)
{
  //TODO:JK: implement me
}

gchar*
ags_simple_file_str2md5(gchar *content, guint content_length)
{
  //TODO:JK: implement me
}

void
ags_simple_file_add_id_ref(AgsSimpleFile *simple_file, GObject *id_ref)
{
  //TODO:JK: implement me
}

GObject*
ags_simple_file_find_id_ref_by_node(AgsSimpleFile *simple_file, xmlNode *node)
{
  //TODO:JK: implement me
}

GObject*
ags_simple_file_find_id_ref_by_xpath(AgsSimpleFile *simple_file, gchar *xpath)
{
  //TODO:JK: implement me
}

GObject*
ags_simple_file_find_id_ref_by_reference(AgsSimpleFile *simple_file, gpointer ref)
{
  //TODO:JK: implement me
}

void
ags_simple_file_add_lookup(AgsSimpleFile *simple_file, GObject *file_lookup)
{
  //TODO:JK: implement me
}

void
ags_simple_file_add_launch(AgsSimpleFile *simple_file, GObject *file_launch)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_open(AgsSimpleFile *simple_file,
			       GError **error)
{
  //TODO:JK: implement me
}

void
ags_simple_file_open(AgsSimpleFile *simple_file,
			  GError **error)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_open_from_data(AgsSimpleFile *simple_file,
					 gchar *data, guint length,
					 GError **error)
{
  //TODO:JK: implement me
}

void
ags_simple_file_open_from_data(AgsSimpleFile *simple_file,
				    gchar *data, guint length,
				    GError **error)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_rw_open(AgsSimpleFile *simple_file,
				  gboolean create,
				  GError **error)
{
  //TODO:JK: implement me
}

void
ags_simple_file_rw_open(AgsSimpleFile *simple_file,
			     gboolean create,
			     GError **error)
{
  //TODO:JK: implement me
}

void
ags_simple_file_open_filename(AgsSimpleFile *simple_file,
				   gchar *filename)
{
  //TODO:JK: implement me
}

void
ags_simple_file_close(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_write(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}


void
ags_simple_file_write(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_write_concurrent(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_write_concurrent(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_write_resolve(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_write_resolve(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_read(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_read_resolve(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_resolve(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_real_read_start(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_start(AgsSimpleFile *simple_file)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_config(AgsSimpleFile *simple_file, xmlNode *node, GObject **ags_config)
{
  //TODO:JK: implement me
}

void
ags_simple_file_write_config(AgsSimpleFile *simple_file, xmlNode *parent, GObject *ags_config)
{
  //TODO:JK: implement me
}

void
ags_simple_file_read_application_context(AgsSimpleFile *simple_file, xmlNode *node, GObject **application_context)
{
  //TODO:JK: implement me
}

void
ags_simple_file_write_application_context(AgsSimpleFile *simple_file, xmlNode *parent, GObject *application_context)
{
  //TODO:JK: implement me
}

AgsSimpleFile*
ags_simple_file_new()
{
  //TODO:JK: implement me
}

