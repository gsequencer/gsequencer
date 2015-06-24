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

#ifndef __AGS_LINK_COLLECTION_EDITOR_H__
#define __AGS_LINK_COLLECTION_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_LINK_COLLECTION_EDITOR                (ags_link_collection_editor_get_type())
#define AGS_LINK_COLLECTION_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LINK_COLLECTION_EDITOR, AgsLinkCollectionEditor))
#define AGS_LINK_COLLECTION_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LINK_COLLECTION_EDITOR, AgsLinkCollectionEditorClass))
#define AGS_IS_LINK_COLLECTION_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LINK_COLLECTION_EDITOR))
#define AGS_IS_LINK_COLLECTION_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LINK_COLLECTION_EDITOR))
#define AGS_LINK_COLLECTION_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_LINK_COLLECTION_EDITOR, AgsLinkCollectionEditorClass))

typedef struct _AgsLinkCollectionEditor AgsLinkCollectionEditor;
typedef struct _AgsLinkCollectionEditorClass AgsLinkCollectionEditorClass;

struct _AgsLinkCollectionEditor
{
  GtkTable table;

  GType channel_type;

  GtkComboBox *link;
  GtkSpinButton *first_line;
  GtkSpinButton *first_link;
  GtkSpinButton *count;
};

struct _AgsLinkCollectionEditorClass
{
  GtkTableClass table;
};

GType ags_link_collection_editor_get_type();

void ags_link_collection_editor_check_first_line(AgsLinkCollectionEditor *link_collection_editor);

void ags_link_collection_editor_check_first_link(AgsLinkCollectionEditor *link_collection_editor);

void ags_link_collection_editor_check_count(AgsLinkCollectionEditor *link_collection_editor);

AgsLinkCollectionEditor* ags_link_collection_editor_new();

#endif /*__AGS_LINK_COLLECTION_EDITOR_H__*/
