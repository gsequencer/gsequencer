#include "ags_machine_line_member_editor.h"
#include "ags_machine_line_member_editor_callbacks.h"

GType ags_machine_line_member_editor_get_type();
void ags_machine_line_member_editor_class_init(AgsMachineLineMemberEditorClass *machine_line_member_editor);
void ags_machine_line_member_editor_init(AgsMachineLineMemberEditor *machine_line_member_editor);
void ags_machine_line_member_editor_destroy(GtkObject *object);
void ags_machine_line_member_editor_connect(AgsMachineLineMemberEditor *machine_line_member_editor);

GType
ags_machine_line_member_editor_get_type()
{
  static GType machine_line_member_editor_type = 0;

  if (!machine_line_member_editor_type){
    static const GtkTypeInfo machine_line_member_editor_info = {
      "AgsMachineLineMemberEditor\0",
      sizeof(AgsMachineLineMemberEditor), /* base_init */
      sizeof(AgsMachineLineMemberEditorClass), /* base_finalize */
      (GtkClassInitFunc) ags_machine_line_member_editor_class_init,
      (GtkObjectInitFunc) ags_machine_line_member_editor_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    machine_line_member_editor_type = gtk_type_unique (GTK_TYPE_VBOX, &machine_line_member_editor_info);
  }

  return (machine_line_member_editor_type);
}

void
ags_machine_line_member_editor_class_init(AgsMachineLineMemberEditorClass *machine_line_member_editor)
{
}

void
ags_machine_line_member_editor_init(AgsMachineLineMemberEditor *machine_line_member_editor)
{
  GtkRadioButton *radio;
  GtkHBox *hbox;
  GtkLabel *label;
  GtkSpinButton *spin;

  radio = (GtkRadioButton *) gtk_radio_button_new_from_widget(NULL);
  gtk_button_set_label((GtkButton *) radio, g_strdup("output\0"));
  gtk_box_pack_start((GtkBox *) machine_line_member_editor, (GtkWidget *) radio, FALSE, FALSE, 0);

  radio = (GtkRadioButton *) gtk_radio_button_new_from_widget(radio);
  gtk_button_set_label((GtkButton *) radio, g_strdup("input\0"));
  gtk_box_pack_start((GtkBox *) machine_line_member_editor, (GtkWidget *) radio, FALSE, FALSE, 0);

  /* from pad */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) machine_line_member_editor, (GtkWidget *) hbox, FALSE, FALSE, 0);

  label = (GtkLabel *) gtk_label_new(g_strdup("from pad\0"));
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) label, FALSE, FALSE, 0);

  spin = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100.0, 1.0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) spin, FALSE, FALSE, 0);

  /* to pad */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) machine_line_member_editor, (GtkWidget *) hbox, FALSE, FALSE, 0);

  label = (GtkLabel *) gtk_label_new(g_strdup("to pad\0"));
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) label, FALSE, FALSE, 0);

  spin = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100.0, 1.0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) spin, FALSE, FALSE, 0);
}

void
ags_machine_line_member_editor_destroy(GtkObject *object)
{
}

void
ags_machine_line_member_editor_connect(AgsMachineLineMemberEditor *machine_line_member_editor)
{
}

void
ags_machine_line_member_editor_apply(AgsMachineLineMemberEditor *machine_line_member_editor, gboolean emit_signal)
{
}

AgsMachineLineMemberEditor*
ags_machine_line_member_editor_new()
{
  AgsMachineLineMemberEditor *machine_line_member_editor;

  machine_line_member_editor = g_object_new(AGS_TYPE_MACHINE_LINE_MEMBER_EDITOR, NULL);

  return(machine_line_member_editor);
}
