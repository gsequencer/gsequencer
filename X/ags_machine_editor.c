#include "ags_machine_editor.h"
#include "ags_machine_editor_callbacks.h"

#include "ags_pad_editor.h"
#include "ags_line_editor.h"

#include "../audio/ags_output.h"
#include "../audio/ags_input.h"

#include <gtk/gtkvbox.h>

GType ags_machine_editor_get_type(void);
void ags_machine_editor_class_init(AgsMachineEditorClass *machine_editor);
void ags_machine_editor_init(AgsMachineEditor *machine_editor);
void ags_machine_editor_connect(AgsMachineEditor *machine_editor);
void ags_machine_editor_destroy(GtkObject *object);
void ags_machine_editor_show(GtkWidget *widget);

void ags_machine_editor_real_set_machine(AgsMachineEditor *machine_editor,
					 AgsMachine *machine);

void ags_machine_editor_real_add_input(AgsMachineEditor *machine_editor);
void ags_machine_editor_real_add_output(AgsMachineEditor *machine_editor);
void ags_machine_editor_real_remove_input(AgsMachineEditor *machine_editor);
void ags_machine_editor_real_remove_output(AgsMachineEditor *machine_editor);

enum{
  SET_MACHINE,
  ADD_INPUT,
  ADD_OUTPUT,
  REMOVE_INPUT,
  REMOVE_OUTPUT,
  LAST_SIGNAL,
};

static guint machine_editor_signals[LAST_SIGNAL];

GType
ags_machine_editor_get_type(void)
{
  static GType machine_editor_type = 0;

  if (!machine_editor_type){
    static const GtkTypeInfo machine_editor_info = {
      "AgsMachineEditor\0",
      sizeof(AgsMachineEditor), /* base_init */
      sizeof(AgsMachineEditorClass), /* base_finalize */
      (GtkClassInitFunc) ags_machine_editor_class_init,
      (GtkObjectInitFunc) ags_machine_editor_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    machine_editor_type = gtk_type_unique (GTK_TYPE_DIALOG, &machine_editor_info);
  }

  return (machine_editor_type);
}

void
ags_machine_editor_class_init(AgsMachineEditorClass *machine_editor)
{
  machine_editor->set_machine = ags_machine_editor_real_set_machine;

  machine_editor->add_input = ags_machine_editor_real_add_input;
  machine_editor->add_output = ags_machine_editor_real_add_output;

  machine_editor->remove_input = ags_machine_editor_real_remove_input;
  machine_editor->remove_output = ags_machine_editor_real_remove_output;

  machine_editor_signals[SET_MACHINE] =
    g_signal_new("set_machine\0",
		 G_TYPE_FROM_CLASS (machine_editor),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsMachineEditorClass, set_machine),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  machine_editor_signals[ADD_INPUT] =
    g_signal_new("add_input\0",
		 G_TYPE_FROM_CLASS (machine_editor),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsMachineEditorClass, add_input),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  machine_editor_signals[ADD_OUTPUT] =
    g_signal_new("add_output\0",
		 G_TYPE_FROM_CLASS (machine_editor),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsMachineEditorClass, add_output),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  machine_editor_signals[REMOVE_INPUT] =
    g_signal_new("remove_input\0",
		 G_TYPE_FROM_CLASS (machine_editor),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsMachineEditorClass, remove_input),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  machine_editor_signals[REMOVE_OUTPUT] =
    g_signal_new("remove_output\0",
		 G_TYPE_FROM_CLASS (machine_editor),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsMachineEditorClass, remove_output),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_machine_editor_init(AgsMachineEditor *machine_editor)
{
  GtkNotebook *notebook;
  GtkScrolledWindow *scrolled_window;

  gtk_window_set_title((GtkWindow *) machine_editor, g_strdup("properties\0"));

  machine_editor->notebook =
    notebook = (GtkNotebook *) gtk_notebook_new();
  gtk_box_pack_start((GtkBox *) machine_editor->dialog.vbox, (GtkWidget*) notebook, TRUE, TRUE, 0);

  /* AgsOutput */
  machine_editor->output_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_notebook_append_page(notebook, (GtkWidget *) scrolled_window, (GtkWidget *) gtk_label_new(g_strdup("output\0")));

  machine_editor->output_property_editor = ags_machine_property_editor_new(AGS_TYPE_OUTPUT);
  gtk_scrolled_window_add_with_viewport(machine_editor->output_scrolled_window, (GtkWidget *) machine_editor->output_property_editor);

  /* AgsInput */
  machine_editor->input_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_notebook_append_page(notebook, (GtkWidget *) scrolled_window, (GtkWidget *) gtk_label_new(g_strdup("input\0")));

  machine_editor->input_property_editor = ags_machine_property_editor_new(AGS_TYPE_INPUT);
  gtk_scrolled_window_add_with_viewport(machine_editor->input_scrolled_window, (GtkWidget *) machine_editor->input_property_editor);

  /*  */
  machine_editor->machine_link_editor_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_notebook_append_page(notebook, (GtkWidget *) scrolled_window, (GtkWidget *) gtk_label_new(g_strdup("links\0")));

  machine_editor->machine_link_editor = ags_machine_link_editor_new();
  gtk_scrolled_window_add_with_viewport(machine_editor->machine_link_editor_scrolled_window, (GtkWidget *) machine_editor->machine_link_editor);

  /* */
  machine_editor->machine_resize_editor_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_notebook_append_page(notebook, (GtkWidget *) scrolled_window, (GtkWidget *) gtk_label_new(g_strdup("resize\0")));

  machine_editor->machine_resize_editor = ags_machine_resize_editor_new();
  gtk_scrolled_window_add_with_viewport(machine_editor->machine_resize_editor_scrolled_window, (GtkWidget *) machine_editor->machine_resize_editor);

  /* */
  machine_editor->machine_line_member_editor_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_notebook_append_page(notebook, (GtkWidget *) scrolled_window, (GtkWidget *) gtk_label_new(g_strdup("line members\0")));

  machine_editor->machine_line_member_editor = ags_machine_line_member_editor_new();
  gtk_scrolled_window_add_with_viewport(machine_editor->machine_line_member_editor_scrolled_window, (GtkWidget *) machine_editor->machine_line_member_editor);

  /* GtkButton's in GtkDialog->action_area  */
  machine_editor->add = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start((GtkBox *) machine_editor->dialog.action_area, (GtkWidget *) machine_editor->add, FALSE, FALSE, 0);

  machine_editor->remove = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start((GtkBox *) machine_editor->dialog.action_area, (GtkWidget *) machine_editor->remove, FALSE, FALSE, 0);

  machine_editor->apply = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_APPLY);
  gtk_box_pack_start((GtkBox *) machine_editor->dialog.action_area, (GtkWidget *) machine_editor->apply, FALSE, FALSE, 0);

  machine_editor->ok = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_OK);
  gtk_box_pack_start((GtkBox *) machine_editor->dialog.action_area, (GtkWidget *) machine_editor->ok, FALSE, FALSE, 0);

  machine_editor->cancel = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_CANCEL);
  gtk_box_pack_start((GtkBox *) machine_editor->dialog.action_area, (GtkWidget *) machine_editor->cancel, FALSE, FALSE, 0);
}

void
ags_machine_editor_connect(AgsMachineEditor *machine_editor)
{
  g_signal_connect((GObject *) machine_editor, "destroy\0",
		   G_CALLBACK(ags_machine_editor_destroy_callback), (gpointer) machine_editor);

  g_signal_connect((GObject *) machine_editor, "show\0",
		   G_CALLBACK(ags_machine_editor_show_callback), (gpointer) machine_editor);

  g_signal_connect((GtkNotebook *) machine_editor->notebook, "switch-page\0",
		   G_CALLBACK(ags_machine_editor_switch_page_callback), (gpointer) machine_editor);

  ags_machine_property_editor_connect(machine_editor->input_property_editor);
  ags_machine_property_editor_connect(machine_editor->output_property_editor);

  ags_machine_link_editor_connect(machine_editor->machine_link_editor);
  ags_machine_resize_editor_connect(machine_editor->machine_resize_editor);
  ags_machine_line_member_editor_connect(machine_editor->machine_line_member_editor);


  g_signal_connect((GObject *) machine_editor->add, "clicked\0",
		   G_CALLBACK(ags_machine_editor_add_callback), (gpointer) machine_editor);

  g_signal_connect((GObject *) machine_editor->remove, "clicked\0",
		   G_CALLBACK(ags_machine_editor_remove_callback), (gpointer) machine_editor);

  g_signal_connect((GObject *) machine_editor->apply, "clicked\0",
		   G_CALLBACK(ags_machine_editor_apply_callback), (gpointer) machine_editor);

  g_signal_connect((GObject *) machine_editor->ok, "clicked\0",
		   G_CALLBACK(ags_machine_editor_ok_callback), (gpointer) machine_editor);

  g_signal_connect((GObject *) machine_editor->cancel, "clicked\0",
		   G_CALLBACK(ags_machine_editor_cancel_callback), (gpointer) machine_editor);
}

void
ags_machine_editor_destroy(GtkObject *object)
{
}

void
ags_machine_editor_show(GtkWidget *widget)
{
  AgsMachineEditor *machine_editor;

  machine_editor = (AgsMachineEditor *) widget;

  gtk_widget_show((GtkWidget *) machine_editor->notebook);
  gtk_widget_show((GtkWidget *) machine_editor->output_scrolled_window);
  gtk_widget_show((GtkWidget *) machine_editor->output_property_editor);
  gtk_widget_show((GtkWidget *) machine_editor->input_scrolled_window);
  gtk_widget_show((GtkWidget *) machine_editor->input_property_editor);

  gtk_widget_show((GtkWidget *) machine_editor->output_property_editor);
  gtk_widget_show((GtkWidget *) machine_editor->input_property_editor);

  gtk_widget_show_all((GtkWidget *) machine_editor->machine_link_editor_scrolled_window);
  gtk_widget_show_all((GtkWidget *) machine_editor->machine_resize_editor_scrolled_window);
  gtk_widget_show_all((GtkWidget *) machine_editor->machine_line_member_editor_scrolled_window);


  gtk_widget_show_all((GtkWidget *) machine_editor->dialog.action_area);
}

void
ags_machine_editor_real_set_machine(AgsMachineEditor *machine_editor, AgsMachine *machine)
{
  machine_editor->machine = machine;
}

void
ags_machine_editor_set_machine(AgsMachineEditor *machine_editor, AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_MACHINE_EDITOR(machine_editor));

  g_object_ref((GObject *) machine_editor);
  g_signal_emit(G_OBJECT(machine_editor),
		machine_editor_signals[SET_MACHINE], 0,
		machine);
  g_object_unref((GObject *) machine_editor);
}

void
ags_machine_editor_real_add_input(AgsMachineEditor *machine_editor)
{
}

void
ags_machine_editor_add_input(AgsMachineEditor *machine_editor)
{
  g_return_if_fail(AGS_IS_MACHINE_EDITOR(machine_editor));

  g_object_ref((GObject *) machine_editor);
  g_signal_emit(G_OBJECT(machine_editor),
		machine_editor_signals[ADD_INPUT], 0);
  g_object_unref((GObject *) machine_editor);
}

void
ags_machine_editor_real_add_output(AgsMachineEditor *machine_editor)
{
}

void
ags_machine_editor_add_output(AgsMachineEditor *machine_editor)
{
  g_return_if_fail(AGS_IS_MACHINE_EDITOR(machine_editor));

  g_object_ref((GObject *) machine_editor);
  g_signal_emit(G_OBJECT(machine_editor),
		machine_editor_signals[ADD_OUTPUT], 0);
  g_object_unref((GObject *) machine_editor);
}

void
ags_machine_editor_real_remove_input(AgsMachineEditor *machine_editor)
{
}

void
ags_machine_editor_remove_input(AgsMachineEditor *machine_editor)
{
  g_return_if_fail(AGS_IS_MACHINE_EDITOR(machine_editor));

  g_object_ref((GObject *) machine_editor);
  g_signal_emit(G_OBJECT(machine_editor),
		machine_editor_signals[REMOVE_INPUT], 0);
  g_object_unref((GObject *) machine_editor);
}

void
ags_machine_editor_real_remove_output(AgsMachineEditor *machine_editor)
{
}

void
ags_machine_editor_remove_output(AgsMachineEditor *machine_editor)
{
  g_return_if_fail(AGS_IS_MACHINE_EDITOR(machine_editor));

  g_object_ref((GObject *) machine_editor);
  g_signal_emit(G_OBJECT(machine_editor),
		machine_editor_signals[REMOVE_OUTPUT], 0);
  g_object_unref((GObject *) machine_editor);
}

AgsMachineEditor*
ags_machine_editor_new()
{
  AgsMachineEditor *machine_editor;

  machine_editor = (AgsMachineEditor *) g_object_new(AGS_TYPE_MACHINE_EDITOR, NULL);

  return(machine_editor);
}
