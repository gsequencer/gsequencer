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

#include <ags/X/ags_line_member_editor.h>
#include <ags/X/ags_line_member_editor_callbacks.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

void ags_line_member_editor_class_init(AgsLineMemberEditorClass *line_member_editor);
void ags_line_member_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_line_member_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_line_member_editor_init(AgsLineMemberEditor *line_member_editor);
void ags_line_member_editor_connect(AgsConnectable *connectable);
void ags_line_member_editor_disconnect(AgsConnectable *connectable);
void ags_line_member_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_line_member_editor_apply(AgsApplicable *applicable);
void ags_line_member_editor_reset(AgsApplicable *applicable);
void ags_line_member_editor_finalize(GObject *gobject);

GType
ags_line_member_editor_get_type(void)
{
  static GType ags_type_line_member_editor = 0;

  if(!ags_type_line_member_editor){
    static const GTypeInfo ags_line_member_editor_info = {
      sizeof (AgsLineMemberEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_line_member_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLineMemberEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_line_member_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_line_member_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_line_member_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_line_member_editor = g_type_register_static(GTK_TYPE_VBOX,
							 "AgsLineMemberEditor\0", &ags_line_member_editor_info,
							 0);

    g_type_add_interface_static(ags_type_line_member_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
    
    g_type_add_interface_static(ags_type_line_member_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_line_member_editor);
}

void
ags_line_member_editor_class_init(AgsLineMemberEditorClass *line_member_editor)
{
  /* empty */
}

void
ags_line_member_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_line_member_editor_connect;
  connectable->disconnect = ags_line_member_editor_disconnect;
}

void
ags_line_member_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_line_member_editor_set_update;
  applicable->apply = ags_line_member_editor_apply;
  applicable->reset = ags_line_member_editor_reset;
}

void
ags_line_member_editor_init(AgsLineMemberEditor *line_member_editor)
{
  GtkHBox *hbox;

  line_member_editor->line_member = (GtkVBox *) gtk_vbox_new(FALSE, 2);
  gtk_box_pack_start((GtkBox *) line_member_editor,
		     (GtkWidget *) line_member_editor->line_member,
		     FALSE, FALSE,
		     0);

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 2);
  gtk_box_pack_start((GtkBox *) line_member_editor,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  line_member_editor->add = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) line_member_editor->add,
		     FALSE, FALSE,
		     0);

  line_member_editor->remove = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) line_member_editor->remove,
		     FALSE, FALSE,
		     0);

  line_member_editor->ladspa_browser = ags_ladspa_browser_new();
}

void
ags_line_member_editor_connect(AgsConnectable *connectable)
{
  AgsLineMemberEditor *line_member_editor;

  line_member_editor = AGS_LINE_MEMBER_EDITOR(connectable);

  g_signal_connect(G_OBJECT(line_member_editor->add), "clicked\0",
		   G_CALLBACK(ags_line_member_editor_add_callback), line_member_editor);

  g_signal_connect(G_OBJECT(line_member_editor->remove), "clicked\0",
		   G_CALLBACK(ags_line_member_editor_remove_callback), line_member_editor);

  ags_connectable_connect(AGS_CONNECTABLE(line_member_editor->ladspa_browser));
}

void
ags_line_member_editor_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_line_member_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsLineMemberEditor *line_member_editor;

  line_member_editor = AGS_LINE_MEMBER_EDITOR(applicable);

  /* empty */
}

void
ags_line_member_editor_apply(AgsApplicable *applicable)
{
  AgsLineMemberEditor *line_member_editor;

  line_member_editor = AGS_LINE_MEMBER_EDITOR(applicable);

  /* empty */
}

void
ags_line_member_editor_reset(AgsApplicable *applicable)
{
  AgsLineMemberEditor *line_member_editor;

  line_member_editor = AGS_LINE_MEMBER_EDITOR(applicable);

  /* empty */
}

AgsLineMemberEditor*
ags_line_member_editor_new()
{
  AgsLineMemberEditor *line_member_editor;

  line_member_editor = (AgsLineMemberEditor *) g_object_new(AGS_TYPE_LINE_MEMBER_EDITOR,
							    NULL);

  return(line_member_editor);
}
