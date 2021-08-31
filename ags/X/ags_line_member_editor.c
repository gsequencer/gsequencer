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

#include <ags/X/ags_line_member_editor.h>
#include <ags/X/ags_line_member_editor_callbacks.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_line_editor.h>

#include <ags/i18n.h>

void ags_line_member_editor_class_init(AgsLineMemberEditorClass *line_member_editor);
void ags_line_member_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_line_member_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_line_member_editor_init(AgsLineMemberEditor *line_member_editor);

void ags_line_member_editor_connect(AgsConnectable *connectable);
void ags_line_member_editor_disconnect(AgsConnectable *connectable);

void ags_line_member_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_line_member_editor_apply(AgsApplicable *applicable);
void ags_line_member_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_line_member_editor
 * @short_description: Edit line member's aspects.
 * @title: AgsLineMemberEditor
 * @section_id:
 * @include: ags/X/ags_line_member_editor.h
 *
 * #AgsLineMemberEditor is a composite widget to modify line member. A line member
 * editor should be packed by a #AgsLineEditor. You may add/remove plugins with this
 * editor.
 */

GType
ags_line_member_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_line_member_editor = 0;

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

    ags_type_line_member_editor = g_type_register_static(GTK_TYPE_BOX,
							 "AgsLineMemberEditor", &ags_line_member_editor_info,
							 0);

    g_type_add_interface_static(ags_type_line_member_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
    
    g_type_add_interface_static(ags_type_line_member_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_line_member_editor);
  }

  return g_define_type_id__volatile;
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
  GtkBox *hbox;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(line_member_editor),
				 GTK_ORIENTATION_VERTICAL);  

  line_member_editor->flags = 0;
  
  line_member_editor->line_member = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
							   0);
  gtk_box_pack_start((GtkBox *) line_member_editor,
		     (GtkWidget *) line_member_editor->line_member,
		     FALSE, FALSE,
		     0);

  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_pack_start((GtkBox *) line_member_editor,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  line_member_editor->add = (GtkButton *) gtk_button_new_from_icon_name("list-add",
									GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) line_member_editor->add,
		     FALSE, FALSE,
		     0);

  line_member_editor->remove = (GtkButton *) gtk_button_new_from_icon_name("list-remove",
									   GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) line_member_editor->remove,
		     FALSE, FALSE,
		     0);

  line_member_editor->plugin_browser = NULL;
}

void
ags_line_member_editor_connect(AgsConnectable *connectable)
{
  AgsLineMemberEditor *line_member_editor;

  line_member_editor = AGS_LINE_MEMBER_EDITOR(connectable);

  if((AGS_LINE_MEMBER_EDITOR_CONNECTED & (line_member_editor->flags)) != 0){
    return;
  }

  line_member_editor->flags |= AGS_LINE_MEMBER_EDITOR_CONNECTED;
  
  g_signal_connect(G_OBJECT(line_member_editor->add), "clicked",
		   G_CALLBACK(ags_line_member_editor_add_callback), line_member_editor);

  g_signal_connect(G_OBJECT(line_member_editor->remove), "clicked",
		   G_CALLBACK(ags_line_member_editor_remove_callback), line_member_editor);
}

void
ags_line_member_editor_disconnect(AgsConnectable *connectable)
{
  AgsLineMemberEditor *line_member_editor;

  line_member_editor = AGS_LINE_MEMBER_EDITOR(connectable);

  if((AGS_LINE_MEMBER_EDITOR_CONNECTED & (line_member_editor->flags)) == 0){
    return;
  }

  line_member_editor->flags &= (~AGS_LINE_MEMBER_EDITOR_CONNECTED);

  g_object_disconnect(G_OBJECT(line_member_editor->add),
		      "any_signal::clicked",
		      G_CALLBACK(ags_line_member_editor_add_callback),
		      line_member_editor,
		      NULL);
  
  g_object_disconnect(G_OBJECT(line_member_editor->remove),
		      "any_signal::clicked",
		      G_CALLBACK(ags_line_member_editor_remove_callback),
		      line_member_editor,
		      NULL);

  if(line_member_editor->plugin_browser != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(line_member_editor->plugin_browser));
    
    g_object_disconnect(G_OBJECT(line_member_editor->plugin_browser),
			"any_signal::response",
			G_CALLBACK(ags_line_member_editor_plugin_browser_response_callback),
			line_member_editor,
			NULL);
  }
}

void
ags_line_member_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_line_member_editor_apply(AgsApplicable *applicable)
{
  /* empty */
}

void
ags_line_member_editor_reset(AgsApplicable *applicable)
{
  AgsLineEditor *line_editor;
  AgsLineMemberEditor *line_member_editor;
  GtkBox *hbox;
  GtkCheckButton *check_button;
  GtkLabel *label;

  GList *start_list, *list;
  GList *start_recall, *recall;

  gchar *str;
  gchar *filename, *effect;

  line_member_editor = AGS_LINE_MEMBER_EDITOR(applicable);

  list = 
    start_list = gtk_container_get_children((GtkContainer *) line_member_editor->line_member);

  while(list != NULL){
    gtk_widget_destroy(list->data);
    
    list = list->next;
  }

  g_list_free((GDestroyNotify) start_list);
  
  line_editor = (AgsLineEditor *) gtk_widget_get_ancestor((GtkWidget *) line_member_editor,
							  AGS_TYPE_LINE_EDITOR);

  g_object_get(line_editor->channel,
	       "play", &start_recall,
	       NULL);

  recall = 
    start_recall = g_list_reverse(start_recall);
  
  while((recall = ags_recall_template_find_all_type(recall,
						    AGS_TYPE_FX_LADSPA_CHANNEL,
						    AGS_TYPE_FX_LV2_CHANNEL,
						    AGS_TYPE_FX_VST3_CHANNEL,
						    G_TYPE_NONE)) != NULL){
    if(ags_recall_test_behaviour_flags(recall->data, AGS_SOUND_BEHAVIOUR_BULK_MODE)){
      recall = recall->next;

      continue;
    }
    
    filename = NULL;
    effect = NULL;    
    
    g_object_get(G_OBJECT(recall->data),
		 "filename", &filename,
		 "effect", &effect,
		 NULL);
    
    hbox = (GtkBox *) gtk_hbox_new(GTK_ORIENTATION_HORIZONTAL,
				   0);
    gtk_box_pack_start(line_member_editor->line_member,
		       (GtkWidget *) hbox,
		       FALSE, FALSE,
		       0);
      
    check_button = (GtkCheckButton *) gtk_check_button_new();
    gtk_box_pack_start(hbox,
		       (GtkWidget *) check_button,
		       FALSE, FALSE,
		       0);
    
    str = g_strdup_printf("%s - %s",
			  filename,
			  effect);
    label = (GtkLabel *) gtk_label_new(str);
    gtk_box_pack_start(hbox,
		       (GtkWidget *) label,
		       FALSE, FALSE,
		       0);
    gtk_widget_show_all((GtkWidget *) hbox);

    g_free(str);
    
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   g_object_unref);
}

/**
 * ags_line_member_editor_new:
 *
 * Create a new instance of #AgsLineMemberEditor
 *
 * Returns: the new #AgsLineMemberEditor
 *
 * Since: 3.0.0
 */
AgsLineMemberEditor*
ags_line_member_editor_new()
{
  AgsLineMemberEditor *line_member_editor;

  line_member_editor = (AgsLineMemberEditor *) g_object_new(AGS_TYPE_LINE_MEMBER_EDITOR,
							    NULL);

  return(line_member_editor);
}
