#ifndef __AGS_OSCILLATOR_H__
#define __AGS_OSCILLATOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_OSCILLATOR                (ags_oscillator_get_type())
#define AGS_OSCILLATOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_OSCILLATOR, AgsOscillator))
#define AGS_OSCILLATOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_OSCILLATOR, AgsOscillatorClass))
#define AGS_IS_OSCILLATOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_OSCILLATOR))
#define AGS_IS_OSCILLATOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_OSCILLATOR))
#define AGS_OSCILLATOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_OSCILLATOR, AgsOscillatorClass))

typedef struct _AgsOscillator AgsOscillator;
typedef struct _AgsOscillatorClass AgsOscillatorClass;

struct _AgsOscillator
{
  GtkMenuItem item;

  GtkFrame *frame;

  GtkOptionMenu *wave;
  gulong wave_handler;

  GtkSpinButton *attack;
  gulong attack_handler;

  GtkSpinButton *length;
  gulong length_handler;

  GtkSpinButton *frequency;
  gulong frequency_handler;

  GtkSpinButton *phase;
  gulong phase_handler;

  GtkSpinButton *volume;
  gulong volume_handler;
};

struct _AgsOscillatorClass
{
  GtkMenuItemClass item;
};

GType ags_oscillator_get_type(void);

AgsOscillator* ags_oscillator_new();

#endif /*__AGS_OSCILLATOR_H__*/
