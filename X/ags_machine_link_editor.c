#include "ags_machine_link_editor.h"
#include "ags_machine_link_editor_callbacks.h"

#include "ags_machine.h"
#include "ags_machine_editor.h"

#include "../object/ags_marshal.h"

#include "../audio/ags_channel.h"
#include "../audio/ags_input.h"
#include "../audio/ags_output.h"

GType ags_machine_link_editor_get_type();
void ags_machine_link_editor_class_init(AgsMachineLinkEditorClass *machine_link_editor);
void ags_machine_link_editor_init(AgsMachineLinkEditor *machine_link_editor);
void ags_machine_link_editor_destroy(GtkObject *object);
void ags_machine_link_editor_connect(AgsMachineLinkEditor *machine_link_editor);

void ags_machine_link_editor_real_reset_input(AgsMachineLinkEditor *machine_link_editor);
void ags_machine_link_editor_real_reset_output(AgsMachineLinkEditor *machine_link_editor);

enum{
  RESET_INPUT,
  RESET_OUTPUT,
  LAST_SIGNAL,
};

static guint machine_link_editor_signals[LAST_SIGNAL];

GType
ags_machine_link_editor_get_type()
{
  static GType machine_link_editor_type = 0;

  if (!machine_link_editor_type){
    static const GtkTypeInfo machine_link_editor_info = {
      "AgsMachineLinkEditor\0",
      sizeof(AgsMachineLinkEditor), /* base_init */
      sizeof(AgsMachineLinkEditorClass), /* base_finalize */
      (GtkClassInitFunc) ags_machine_link_editor_class_init,
      (GtkObjectInitFunc) ags_machine_link_editor_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    machine_link_editor_type = gtk_type_unique (GTK_TYPE_VBOX, &machine_link_editor_info);
  }

  return (machine_link_editor_type);
}

void
ags_machine_link_editor_class_init(AgsMachineLinkEditorClass *machine_link_editor)
{
  machine_link_editor->reset_input = ags_machine_link_editor_real_reset_input;
  machine_link_editor->reset_output = ags_machine_link_editor_real_reset_output;

  machine_link_editor_signals[RESET_INPUT] = 
    g_signal_new("reset_input",
		 G_TYPE_FROM_CLASS (machine_link_editor),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsMachineLinkEditorClass, reset_input),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  machine_link_editor_signals[RESET_OUTPUT] = 
    g_signal_new("reset_output",
		 G_TYPE_FROM_CLASS (machine_link_editor),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsMachineLinkEditorClass, reset_output),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_machine_link_editor_init(AgsMachineLinkEditor *machine_link_editor)
{
  GtkLabel *label;
  GtkMenu *menu;
  GtkMenuItem *item;
  GtkTable *table;
  GtkHBox *hbox;

  g_signal_connect_after(G_OBJECT(machine_link_editor), "parent_set\0",
			 G_CALLBACK(ags_machine_link_editor_parent_set_callback), machine_link_editor);

  table = (GtkTable *) gtk_table_new(10, 2, TRUE);
  gtk_box_pack_start((GtkBox *) machine_link_editor, (GtkWidget *) table, FALSE, FALSE, 0);

  /* AgsInput  */
  label = (GtkLabel *) gtk_label_new(g_strdup("input\0"));
  gtk_table_attach(table,
		   (GtkWidget *) label,
		   0, 1,
		   0, 1,
		   GTK_FILL, 0,
		   0, 0);

  /* this AgsInput's first_line */
  label = (GtkLabel *) gtk_label_new(g_strdup("first line\0"));
  gtk_table_attach(table,
		   (GtkWidget *) label,
		   0, 1,
		   1, 2,
		   GTK_FILL, 0,
		   0, 0);

  machine_link_editor->input_first_line = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_table_attach(table,
		   (GtkWidget *) machine_link_editor->input_first_line,
		   1, 2,
		   1, 2,
		   0, 0,
		   0, 0);

  /* link with */
  label = (GtkLabel *) gtk_label_new(g_strdup("link with\0"));
  gtk_table_attach(table,
		   (GtkWidget *) label,
		   0, 1,
		   2, 3,
		   GTK_FILL, 0,
		   0, 0);

  machine_link_editor->input_link = (GtkOptionMenu *) gtk_option_menu_new();
  gtk_table_attach(table,
		   (GtkWidget *) machine_link_editor->input_link,
		   1, 2,
		   2, 3,
		   0, 0,
		   0, 0);

  menu = (GtkMenu *) gtk_menu_new();
  gtk_option_menu_set_menu(machine_link_editor->input_link, (GtkWidget *) menu);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("NULL\0"));
  g_object_set_data((GObject *) item, (char *) g_type_name(AGS_TYPE_MACHINE), NULL);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  gtk_option_menu_set_history(machine_link_editor->input_link, 0);

  /* link input from first channel */
  label = (GtkLabel *) gtk_label_new(g_strdup("from first line\0"));
  gtk_table_attach(table,
		   (GtkWidget *) label,
		   0, 1,
		   3, 4,
		   GTK_FILL, 0,
		   0, 0);

  machine_link_editor->input_first_link = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_table_attach(table,
		   (GtkWidget *) machine_link_editor->input_first_link,
		   1, 2,
		   3, 4,
		   0, 0,
		   0, 0);

  /* n times */
  label = (GtkLabel *) gtk_label_new(g_strdup("n times\0"));
  gtk_table_attach(table,
		   (GtkWidget *) label,
		   0, 1,
		   4, 5,
		   GTK_FILL, 0,
		   0, 0);

  machine_link_editor->input_n = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_table_attach(table,
		   (GtkWidget *) machine_link_editor->input_n,
		   1, 2,
		   4, 5,
		   0, 0,
		   0, 0);

  /* AgsOutput */
  label = (GtkLabel *) gtk_label_new(g_strdup("output\0"));
  gtk_table_attach(table,
		   (GtkWidget *) label,
		   0, 1,
		   5, 6,
		   GTK_FILL, 0,
		   0, 0);

  /* this AgsOutput's first_line */
  label = (GtkLabel *) gtk_label_new(g_strdup("first line\0"));
  gtk_table_attach(table,
		   (GtkWidget *) label,
		   0, 1,
		   6, 7,
		   GTK_FILL, 0,
		   0, 0);

  machine_link_editor->output_first_line = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_table_attach(table,
		   (GtkWidget *) machine_link_editor->output_first_line,
		   1, 2,
		   6, 7,
		   0, 0,
		   0, 0);

  /* link with */
  label = (GtkLabel *) gtk_label_new(g_strdup("link with\0"));
  gtk_table_attach(table,
		   (GtkWidget *) label,
		   0, 1,
		   7, 8,
		   GTK_FILL, 0,
		   0, 0);

  machine_link_editor->output_link = (GtkOptionMenu *) gtk_option_menu_new();
  gtk_table_attach(table,
		   (GtkWidget *) machine_link_editor->output_link,
		   1, 2,
		   7, 8,
		   0, 0,
		   0, 0);

  menu = (GtkMenu *) gtk_menu_new();
  gtk_option_menu_set_menu(machine_link_editor->output_link, (GtkWidget *) menu);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("NULL\0"));
  g_object_set_data((GObject *) item, (char *) g_type_name(AGS_TYPE_MACHINE), NULL);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  gtk_option_menu_set_history(machine_link_editor->output_link, 0);

  /* link output from first channel */
  label = (GtkLabel *) gtk_label_new(g_strdup("from first line\0"));
  gtk_table_attach(table,
		   (GtkWidget *) label,
		   0, 1,
		   8, 9,
		   GTK_FILL, 0,
		   0, 0);

  machine_link_editor->output_first_link = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_table_attach(table,
		   (GtkWidget *) machine_link_editor->output_first_link,
		   1, 2,
		   8, 9,
		   0, 0,
		   0, 0);

  /* n times */
  label = (GtkLabel *) gtk_label_new(g_strdup("n times\0"));
  gtk_table_attach(table,
		   (GtkWidget *) label,
		   0, 1,
		   9, 10,
		   GTK_FILL, 0,
		   0, 0);

  machine_link_editor->output_n = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 0.0, 1.0);
  gtk_table_attach(table,
		   (GtkWidget *) machine_link_editor->output_n,
		   1, 2,
		   9, 10,
		   0, 0,
		   0, 0);

  /* the rest is done parent_set callback */
}

void
ags_machine_link_editor_destroy(GtkObject *object)
{
}

void
ags_machine_link_editor_connect(AgsMachineLinkEditor *machine_link_editor)
{
  AgsMachineEditor *machine_editor;

  /* AgsInput */
  g_signal_connect((GObject *) machine_link_editor->input_first_line, "value_changed\0",
		   G_CALLBACK(ags_machine_link_editor_input_first_line_callback), machine_link_editor);

  g_signal_connect((GObject *) machine_link_editor->input_link, "changed\0",
		   G_CALLBACK(ags_machine_link_editor_input_link_callback), machine_link_editor);

  g_signal_connect((GObject *) machine_link_editor->input_first_link, "value_changed\0",
		   G_CALLBACK(ags_machine_link_editor_input_first_link_callback), machine_link_editor);

  /* AgsOutput */
  g_signal_connect((GObject *) machine_link_editor->output_first_line, "value_changed\0",
		   G_CALLBACK(ags_machine_link_editor_output_first_line_callback), machine_link_editor);

  g_signal_connect((GObject *) machine_link_editor->output_link, "changed\0",
		   G_CALLBACK(ags_machine_link_editor_output_link_callback), machine_link_editor);

  g_signal_connect((GObject *) machine_link_editor->output_first_link, "value_changed\0",
		   G_CALLBACK(ags_machine_link_editor_output_first_link_callback), machine_link_editor);

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_link_editor, AGS_TYPE_MACHINE_EDITOR);

  /* AgsMachineEditor */
  g_signal_connect_after((GObject *) machine_editor, "add_input\0",
			 G_CALLBACK(ags_machine_link_editor_add_input_callback), machine_link_editor);

  g_signal_connect_after((GObject *) machine_editor, "add_output\0",
			 G_CALLBACK(ags_machine_link_editor_add_output_callback), machine_link_editor);

  g_signal_connect_after((GObject *) machine_editor, "remove_input\0",
			 G_CALLBACK(ags_machine_link_editor_remove_input_callback), machine_link_editor);

  g_signal_connect_after((GObject *) machine_editor, "remove_output\0",
			 G_CALLBACK(ags_machine_link_editor_remove_output_callback), machine_link_editor);

  /* AgsMachineResizeEditor */
  g_signal_connect((GObject *) machine_editor->machine_resize_editor, "resize_audio_channels\0",
		   G_CALLBACK(ags_machine_link_editor_resize_audio_channels_callback), machine_link_editor);

  g_signal_connect((GObject *) machine_editor->machine_resize_editor, "resize_input_pads\0",
		   G_CALLBACK(ags_machine_link_editor_resize_input_pads_callback), machine_link_editor);

  g_signal_connect((GObject *) machine_editor->machine_resize_editor, "resize_output_pads\0",
		   G_CALLBACK(ags_machine_link_editor_resize_output_pads_callback), machine_link_editor);
}

void
ags_machine_link_editor_real_reset_input(AgsMachineLinkEditor *machine_link_editor)
{
  AgsMachine *machine_link;
  AgsMachineEditor *machine_editor;
  AgsChannel *channel, *link;
  guint first_line, first_link, n;
  guint i;

  n = (guint) machine_link_editor->input_n->adjustment->value + 1;

  if(n == 0)
    return;

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_link_editor, AGS_TYPE_MACHINE_EDITOR);

  first_line = (guint) machine_link_editor->input_first_line->adjustment->value;
  first_link = (guint) machine_link_editor->input_first_link->adjustment->value;

  channel = ags_channel_nth(machine_editor->machine->audio->input, first_line);

  machine_link = (AgsMachine *) g_object_get_data((GObject *) machine_link_editor->input_link->menu_item, (char *) g_type_name(AGS_TYPE_MACHINE));

  if(machine_link != NULL)
    link = ags_channel_nth(machine_link->audio->output, first_link);
  else
    link = NULL;

  for(i = 0; i < n; i++){
    ags_channel_set_link(channel, link);
    //fprintf(stdout, "%d\n\0", i);

    channel = channel->next;

    if(link != NULL)
      link = link->next;
  }
}

void
ags_machine_link_editor_reset_input(AgsMachineLinkEditor *machine_link_editor)
{
  g_return_if_fail(AGS_IS_MACHINE_LINK_EDITOR(machine_link_editor));
    
  g_object_ref((GObject *) machine_link_editor);
  g_signal_emit(G_OBJECT(machine_link_editor),
		machine_link_editor_signals[RESET_INPUT], 0);
  g_object_unref((GObject *) machine_link_editor);
}

void
ags_machine_link_editor_real_reset_output(AgsMachineLinkEditor *machine_link_editor)
{
  AgsMachine *machine_link;
  AgsMachineEditor *machine_editor;
  AgsChannel *channel, *link;
  guint first_line, first_link, n;
  guint i;

  n = (guint) machine_link_editor->output_n->adjustment->value + 1;

  if(n == 0)
    return;

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_link_editor, AGS_TYPE_MACHINE_EDITOR);

  first_line = (guint) machine_link_editor->output_first_line->adjustment->value;
  first_link = (guint) machine_link_editor->output_first_link->adjustment->value;

  channel = ags_channel_nth(machine_editor->machine->audio->output, first_line);

  machine_link = (AgsMachine *) g_object_get_data((GObject *) machine_link_editor->output_link->menu_item, (char *) g_type_name(AGS_TYPE_MACHINE));

  if(machine_link != NULL)
    link = ags_channel_nth(machine_link->audio->input, first_link);
  else
    link = NULL;

  for(i = 0; i < n; i++){
    ags_channel_set_link(channel, link);
    
    channel = channel->next;

    if(link != NULL)
      link = link->next;
  }
}

void
ags_machine_link_editor_reset_output(AgsMachineLinkEditor *machine_link_editor)
{
  g_return_if_fail(AGS_IS_MACHINE_LINK_EDITOR(machine_link_editor));

  g_object_ref((GObject *) machine_link_editor);
  g_signal_emit(G_OBJECT(machine_link_editor),
		machine_link_editor_signals[RESET_OUTPUT], 0);
  g_object_unref((GObject *) machine_link_editor);
}

void
ags_machine_link_editor_apply(AgsMachineLinkEditor *machine_link_editor,
			      gboolean emit_signal)
{
  if(emit_signal){
    ags_machine_link_editor_reset_input(machine_link_editor);
    ags_machine_link_editor_reset_output(machine_link_editor);
  }else{
    AGS_MACHINE_LINK_EDITOR_GET_CLASS(machine_link_editor)->reset_input(machine_link_editor);
    AGS_MACHINE_LINK_EDITOR_GET_CLASS(machine_link_editor)->reset_output(machine_link_editor);
  }
}

AgsMachineLinkEditor*
ags_machine_link_editor_new()
{
  AgsMachineLinkEditor *machine_link_editor;

  machine_link_editor = (AgsMachineLinkEditor *) g_object_new(AGS_TYPE_MACHINE_LINK_EDITOR, NULL);

  return(machine_link_editor);
}
