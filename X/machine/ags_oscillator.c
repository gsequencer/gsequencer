#include "ags_oscillator.h"
#include "ags_oscillator_callbacks.h"

void ags_oscillator_class_init(AgsOscillatorClass *oscillator);
void ags_oscillator_init(AgsOscillator *oscillator);
void ags_oscillator_connect(AgsOscillator *oscillator);
void ags_oscillator_disconnect(AgsOscillator *oscillator);
void ags_oscillator_destroy(GtkObject *object);
void ags_oscillator_show(GtkWidget *widget);

GType
ags_oscillator_get_type(void)
{
  static GType oscillator_type = 0;

  if (!oscillator_type){
    static const GtkTypeInfo oscillator_info = {
      "AgsOscillator\0",
      sizeof(AgsOscillator), /* base_init */
      sizeof(AgsOscillatorClass), /* base_finalize */
      (GtkClassInitFunc) ags_oscillator_class_init,
      (GtkObjectInitFunc) ags_oscillator_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    oscillator_type = gtk_type_unique (GTK_TYPE_MENU_ITEM, &oscillator_info);
  }

  return (oscillator_type);
}

void
ags_oscillator_class_init(AgsOscillatorClass *oscillator)
{
}

void
ags_oscillator_init(AgsOscillator *oscillator)
{
  GtkTable *table;
  GtkMenu *menu;

  oscillator->frame = (GtkFrame *) gtk_frame_new(NULL);
  gtk_container_add((GtkContainer *) oscillator, (GtkWidget *) oscillator->frame);

  table = (GtkTable *) gtk_table_new(8, 2, FALSE);
  gtk_container_add((GtkContainer *) oscillator->frame, (GtkWidget *) table);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new("wave\0"),
			    0, 1, 0, 1);

  oscillator->wave = (GtkOptionMenu *) gtk_option_menu_new();
  gtk_table_attach_defaults(table,
			    (GtkWidget *) oscillator->wave,
			    1, 2, 0, 1);

  menu = (GtkMenu *) gtk_menu_new();
  gtk_option_menu_set_menu(oscillator->wave, (GtkWidget *) menu);

  gtk_menu_shell_append((GtkMenuShell *) menu,
			gtk_menu_item_new_with_label("sin\0"));

  gtk_menu_shell_append((GtkMenuShell *) menu,
			gtk_menu_item_new_with_label("cos\0"));

  gtk_menu_shell_append((GtkMenuShell *) menu,
			gtk_menu_item_new_with_label("saw\0"));

  gtk_menu_shell_append((GtkMenuShell *) menu,
			gtk_menu_item_new_with_label("square\0"));

  gtk_menu_shell_append((GtkMenuShell *) menu,
			gtk_menu_item_new_with_label("triangle\0"));

  gtk_option_menu_set_history(oscillator->wave, 0);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new("attack\0"),
			    2, 3, 0, 1);
  oscillator->attack = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
  oscillator->attack->adjustment->value = 0.0;
  gtk_table_attach_defaults(table, (GtkWidget *) oscillator->attack, 3, 4, 0, 1);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new("length\0"),
			    4, 5, 0, 1);
  oscillator->length = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
  oscillator->length->adjustment->value = 44100.0 / 27.5;
  gtk_table_attach_defaults(table, (GtkWidget *) oscillator->length, 5, 6, 0, 1);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new("phase\0"),
			    0, 1, 1, 2);
  oscillator->phase = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
  oscillator->phase->adjustment->value = 0.0;
  gtk_table_attach_defaults(table, (GtkWidget *) oscillator->phase, 1, 2, 1, 2);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new("frequency\0"),
			    2, 3, 1, 2);
  oscillator->frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
  oscillator->frequency->adjustment->value = 27.5;
  gtk_table_attach_defaults(table, (GtkWidget *) oscillator->frequency, 3, 4, 1, 2);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new("volume\0"),
			    4, 5, 1, 2);
  oscillator->volume = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1.0, 0.1);
  oscillator->volume->adjustment->value = 0.8;
  gtk_table_attach_defaults(table, (GtkWidget *) oscillator->volume, 5, 6, 1, 2);
}

void
ags_oscillator_connect(AgsOscillator *oscillator)
{
  oscillator->wave_handler = g_signal_connect(G_OBJECT(oscillator->wave), "changed\0",
					      G_CALLBACK(ags_oscillator_wave_callback), oscillator);

  oscillator->attack_handler = g_signal_connect(G_OBJECT(oscillator->attack), "value-changed\0",
						G_CALLBACK(ags_oscillator_attack_callback), oscillator);
  
  oscillator->length_handler = g_signal_connect(G_OBJECT(oscillator->length), "value-changed\0",
						G_CALLBACK(ags_oscillator_length_callback), oscillator);

  oscillator->frequency_handler = g_signal_connect(G_OBJECT(oscillator->frequency), "value-changed\0",
						   G_CALLBACK(ags_oscillator_frequency_callback), oscillator);

  oscillator->phase_handler = g_signal_connect(G_OBJECT(oscillator->phase), "value-changed\0",
					       G_CALLBACK(ags_oscillator_phase_callback), oscillator);

  oscillator->volume_handler = g_signal_connect(G_OBJECT(oscillator->volume), "value-changed\0",
						G_CALLBACK(ags_oscillator_volume_callback), oscillator);
}

void
ags_oscillator_disconnect(AgsOscillator *oscillator)
{
  g_signal_handler_disconnect(G_OBJECT(oscillator->wave), oscillator->wave_handler);
  g_signal_handler_disconnect(G_OBJECT(oscillator->attack), oscillator->attack_handler);
  g_signal_handler_disconnect(G_OBJECT(oscillator->length), oscillator->length_handler);
  g_signal_handler_disconnect(G_OBJECT(oscillator->frequency), oscillator->frequency_handler);
  g_signal_handler_disconnect(G_OBJECT(oscillator->phase), oscillator->phase_handler);
  g_signal_handler_disconnect(G_OBJECT(oscillator->volume), oscillator->volume_handler);
}

void
ags_oscillator_destroy(GtkObject *object)
{
}

void
ags_oscillator_show(GtkWidget *widget)
{
}

AgsOscillator*
ags_oscillator_new()
{
  AgsOscillator *oscillator;

  oscillator = (AgsOscillator *) g_object_new(AGS_TYPE_OSCILLATOR, NULL);

  return(oscillator);
}
