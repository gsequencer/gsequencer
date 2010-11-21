#ifndef __AGS_LINE_MEMBER_EDITOR_H__
#define __AGS_LINE_MEMBER_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_line_member_editor.h>

int ags_line_member_editor_destroy_callback(GtkObject *object, AgsLineMemberEditor *line_member_editor);
int ags_line_member_editor_show_callback(GtkWidget *widget, AgsLineMemberEditor *line_member_editor);

#endif /*__AGS_LINE_MEMBER_EDITOR_H__*/
