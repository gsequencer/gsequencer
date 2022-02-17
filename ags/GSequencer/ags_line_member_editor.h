/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_LINE_MEMBER_EDITOR_H__
#define __AGS_LINE_MEMBER_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/GSequencer/ags_plugin_browser.h>

G_BEGIN_DECLS

#define AGS_TYPE_LINE_MEMBER_EDITOR                (ags_line_member_editor_get_type())
#define AGS_LINE_MEMBER_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LINE_MEMBER_EDITOR, AgsLineMemberEditor))
#define AGS_LINE_MEMBER_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LINE_MEMBER_EDITOR, AgsLineMemberEditorClass))
#define AGS_IS_LINE_MEMBER_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LINE_MEMBER_EDITOR))
#define AGS_IS_LINE_MEMBER_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LINE_MEMBER_EDITOR))
#define AGS_LINE_MEMBER_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LINE_MEMBER_EDITOR, AgsLineMemberEditorClass))

typedef struct _AgsLineMemberEditor AgsLineMemberEditor;
typedef struct _AgsLineMemberEditorClass AgsLineMemberEditorClass;

typedef enum{
  AGS_LINE_MEMBER_EDITOR_CONNECTED    = 1,
}AgsLineMemberEditorFlags;

struct _AgsLineMemberEditor
{
  GtkBox box;

  guint flags;
  
  GtkBox *line_member;

  GtkButton *add;
  GtkMenu *popup;

  GtkButton *remove;

  AgsPluginBrowser *plugin_browser;
};

struct _AgsLineMemberEditorClass
{
  GtkBoxClass box;
};

GType ags_line_member_editor_get_type(void);

AgsLineMemberEditor* ags_line_member_editor_new();

G_END_DECLS

#endif /*__AGS_LINE_MEMBER_EDITOR_H__*/
