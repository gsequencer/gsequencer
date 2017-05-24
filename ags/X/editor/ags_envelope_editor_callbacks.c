/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/editor/ags_envelope_editor_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

void
ags_envelope_editor_attack_x_callback(GtkButton *button, AgsEnvelopeEditor *envelope_editor)
{
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_attack_y_callback(GtkButton *button, AgsEnvelopeEditor *envelope_editor)
{
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_decay_x_callback(GtkButton *button, AgsEnvelopeEditor *envelope_editor)
{
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_decay_y_callback(GtkButton *button, AgsEnvelopeEditor *envelope_editor)
{
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_sustain_x_callback(GtkButton *button, AgsEnvelopeEditor *envelope_editor)
{
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_sustain_y_callback(GtkButton *button, AgsEnvelopeEditor *envelope_editor)
{
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_release_x_callback(GtkButton *button, AgsEnvelopeEditor *envelope_editor)
{
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_release_y_callback(GtkButton *button, AgsEnvelopeEditor *envelope_editor)
{
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_ratio_callback(GtkButton *button, AgsEnvelopeEditor *envelope_editor)
{
  ags_envelope_editor_plot(envelope_editor);
}
