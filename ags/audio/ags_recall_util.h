/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#ifndef __AGS_RECALL_UTIL_H__
#define __AGS_RECALL_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE(midi1_opcode, midi1_cc) ((midi1_opcode << 8) | (midi1_cc))
#define AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE(midi2_opcode, midi1_opcode, midi2_per_note_controller, midi2_cc) ((AgsUmpWord)((midi2_opcode << 24) | (midi1_opcode << 16) | (midi2_per_note_controller << 8) | (midi2_cc)))

#define AGS_RECALL_UTIL_MIDI1_CC_BANK_SELECT(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x0))
#define AGS_RECALL_UTIL_MIDI1_CC_MODULATION_WHEEL(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x1))
#define AGS_RECALL_UTIL_MIDI1_CC_BREATH_CONTROLLER(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x2))
#define AGS_RECALL_UTIL_MIDI1_CC_FOOT_CONTROLLER(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x4))
#define AGS_RECALL_UTIL_MIDI1_CC_PORTAMENTO_TIME(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x5))
#define AGS_RECALL_UTIL_MIDI1_CC_CHANNEL_VOLUME(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x7))
#define AGS_RECALL_UTIL_MIDI1_CC_BALANCE(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x8))
#define AGS_RECALL_UTIL_MIDI1_CC_PAN(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0xa))
#define AGS_RECALL_UTIL_MIDI1_CC_EXPRESSION_CONTROLLER(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0xb))
#define AGS_RECALL_UTIL_MIDI1_CC_24(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x18))
#define AGS_RECALL_UTIL_MIDI1_CC_25(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x19))
#define AGS_RECALL_UTIL_MIDI1_CC_26(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x1a))
#define AGS_RECALL_UTIL_MIDI1_CC_27(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x1b))
#define AGS_RECALL_UTIL_MIDI1_CC_28(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x1c))
#define AGS_RECALL_UTIL_MIDI1_CC_29(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x1d))
#define AGS_RECALL_UTIL_MIDI1_CC_30(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x1e))
#define AGS_RECALL_UTIL_MIDI1_CC_31(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x1f))
#define AGS_RECALL_UTIL_MIDI1_CC_EFFECT_CONTROL_1(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0xc))
#define AGS_RECALL_UTIL_MIDI1_CC_EFFECT_CONTROL_2(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0xd))
#define AGS_RECALL_UTIL_MIDI1_CC_GENERAL_PURPOSE_CONTROLLER_1(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x10))
#define AGS_RECALL_UTIL_MIDI1_CC_GENERAL_PURPOSE_CONTROLLER_2(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x11))
#define AGS_RECALL_UTIL_MIDI1_CC_GENERAL_PURPOSE_CONTROLLER_3(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x12))
#define AGS_RECALL_UTIL_MIDI1_CC_GENERAL_PURPOSE_CONTROLLER_4(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x13))
#define AGS_RECALL_UTIL_MIDI1_CC_CHANGE_PROGRAM(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xc0 | midi_channel), 0x0))
#define AGS_RECALL_UTIL_MIDI1_CC_CHANGE_PRESSURE(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xd0 | midi_channel), 0x0))
#define AGS_RECALL_UTIL_MIDI1_CC_PITCH_BEND(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xe0 | midi_channel), 0x0))
#define AGS_RECALL_UTIL_MIDI1_CC_SONG_POSITION(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xf2 | midi_channel), 0x0))
#define AGS_RECALL_UTIL_MIDI1_CC_SONG_SELECT(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xf3 | midi_channel), 0x0))
#define AGS_RECALL_UTIL_MIDI1_CC_TUNE_REQUEST(midi_channel) (AGS_RECALL_UTIL_MIDI1_CONTROL_CHANGE((0xf6 | midi_channel), 0x0))

#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_BANK_SELECT(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x0, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_MODULATION_WHEEL(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x1, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_BREATH_CONTROLLER(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x2, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_FOOT_CONTROLLER(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x4, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_PORTAMENTO_TIME(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x5, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_CHANNEL_VOLUME(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x7, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_BALANCE(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x8, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_PAN(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0xa, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_EXPRESSION_CONTROLLER(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0xb, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_EFFECT_CONTROL_1(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0xc, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_EFFECT_CONTROL_2(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0xd, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_GENERAL_PURPOSE_CONTROLLER_1(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x10, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_GENERAL_PURPOSE_CONTROLLER_2(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x11, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_GENERAL_PURPOSE_CONTROLLER_3(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x12, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_GENERAL_PURPOSE_CONTROLLER_4(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x13, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_CHANGE_PROGRAM(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xc0 | midi_channel), 0x0, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_CHANGE_PRESSURE(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xd0 | midi_channel), 0x0, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MIDI1_PITCH_BEND(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xe0 | midi_channel), 0x0, 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_CHANGE_PROGRAM(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xc0 | midi_channel), 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_CHANGE_PRESSURE(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xd0 | midi_channel), 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_PITCH_BEND(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xe0 | midi_channel), 0x0, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_PER_NOTE_PITCH_BEND(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0xf0 | midi_group), (0xe0 | midi_channel), midi_note, 0))
#define AGS_RECALL_UTIL_MIDI2_CC_MODULATION(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 1))
#define AGS_RECALL_UTIL_MIDI2_CC_BREATH(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 2))
#define AGS_RECALL_UTIL_MIDI2_CC_PITCH_7_25(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 3))
#define AGS_RECALL_UTIL_MIDI2_CC_VOLUME(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 7))
#define AGS_RECALL_UTIL_MIDI2_CC_BALANCE(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 8))
#define AGS_RECALL_UTIL_MIDI2_CC_PAN(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 10))
#define AGS_RECALL_UTIL_MIDI2_CC_EXPRESSION(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 11))
#define AGS_RECALL_UTIL_MIDI2_CC_24(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 24))
#define AGS_RECALL_UTIL_MIDI2_CC_25(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 25))
#define AGS_RECALL_UTIL_MIDI2_CC_26(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 26))
#define AGS_RECALL_UTIL_MIDI2_CC_27(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 27))
#define AGS_RECALL_UTIL_MIDI2_CC_28(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 28))
#define AGS_RECALL_UTIL_MIDI2_CC_29(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 29))
#define AGS_RECALL_UTIL_MIDI2_CC_30(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 30))
#define AGS_RECALL_UTIL_MIDI2_CC_31(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 31))
#define AGS_RECALL_UTIL_MIDI2_CC_SOUND_VARIATION(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 70))
#define AGS_RECALL_UTIL_MIDI2_CC_TIMBRE(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 71))
#define AGS_RECALL_UTIL_MIDI2_CC_RELEASE(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 72))
#define AGS_RECALL_UTIL_MIDI2_CC_ATTACK(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 73))
#define AGS_RECALL_UTIL_MIDI2_CC_BRIGHTNESS(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 74))
#define AGS_RECALL_UTIL_MIDI2_CC_DECAY(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 75))
#define AGS_RECALL_UTIL_MIDI2_CC_VIBRATO_RATE(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 76))
#define AGS_RECALL_UTIL_MIDI2_CC_VIBRATO_DEPTH(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 77))
#define AGS_RECALL_UTIL_MIDI2_CC_VIBRATO_DELAY(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 78))
#define AGS_RECALL_UTIL_MIDI2_CC_SOUND_CONTROLLER_10(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 79))
#define AGS_RECALL_UTIL_MIDI2_CC_EFFECTS_1(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 91))
#define AGS_RECALL_UTIL_MIDI2_CC_EFFECTS_2(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 92))
#define AGS_RECALL_UTIL_MIDI2_CC_EFFECTS_3(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 93))
#define AGS_RECALL_UTIL_MIDI2_CC_EFFECTS_4(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 94))
#define AGS_RECALL_UTIL_MIDI2_CC_EFFECTS_5(midi_group, midi_channel, midi_note) (AGS_RECALL_UTIL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 95))

G_END_DECLS

#endif /*__AGS_RECALL_UTIL_H__*/
