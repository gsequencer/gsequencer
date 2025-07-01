/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#ifndef __AGS_RECALL_H__
#define __AGS_RECALL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_dependency.h>

#include <ags/audio/midi/ags_midi_util.h>
#include <ags/audio/midi/ags_midi_ump_util.h>

G_BEGIN_DECLS

#define AGS_TYPE_RECALL                (ags_recall_get_type())
#define AGS_TYPE_RECALL_FLAGS          (ags_recall_flags_get_type())
#define AGS_TYPE_RECALL_NOTIFY_DEPENDENCY_MODE (ags_recall_notify_dependency_mode_get_type())
#define AGS_RECALL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL, AgsRecall))
#define AGS_RECALL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL, AgsRecallClass))
#define AGS_IS_RECALL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RECALL))
#define AGS_IS_RECALL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RECALL))
#define AGS_RECALL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RECALL, AgsRecallClass))

#define AGS_RECALL_HANDLER(handler)    ((AgsRecallHandler *)(handler))

#define AGS_RECALL_GET_OBJ_MUTEX(obj) (&(((AgsRecall *) obj)->obj_mutex))

#define AGS_RECALL_MIDI1_CONTROL_CHANGE(midi1_opcode, midi1_cc) ((midi1_opcode << 8) | (midi1_cc))
#define AGS_RECALL_MIDI2_CONTROL_CHANGE(midi2_opcode, midi1_opcode, midi2_per_note_controller, midi2_cc) ((AgsUmpWord)((midi2_opcode << 24) | (midi1_opcode << 16) | (midi2_per_note_controller << 8) | (midi2_cc)))

#define AGS_RECALL_MIDI1_CC_BANK_SELECT(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x0))
#define AGS_RECALL_MIDI1_CC_MODULATION_WHEEL(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x1))
#define AGS_RECALL_MIDI1_CC_BREATH_CONTROLLER(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x2))
#define AGS_RECALL_MIDI1_CC_FOOT_CONTROLLER(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x4))
#define AGS_RECALL_MIDI1_CC_PORTAMENTO_TIME(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x5))
#define AGS_RECALL_MIDI1_CC_CHANNEL_VOLUME(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x7))
#define AGS_RECALL_MIDI1_CC_BALANCE(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x8))
#define AGS_RECALL_MIDI1_CC_PAN(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0xa))
#define AGS_RECALL_MIDI1_CC_EXPRESSION_CONTROLLER(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0xb))
#define AGS_RECALL_MIDI1_CC_EFFECT_CONTROL_1(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0xc))
#define AGS_RECALL_MIDI1_CC_EFFECT_CONTROL_2(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0xd))
#define AGS_RECALL_MIDI1_CC_GENERAL_PURPOSE_CONTROLLER_1(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x10))
#define AGS_RECALL_MIDI1_CC_GENERAL_PURPOSE_CONTROLLER_2(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x11))
#define AGS_RECALL_MIDI1_CC_GENERAL_PURPOSE_CONTROLLER_3(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x12))
#define AGS_RECALL_MIDI1_CC_GENERAL_PURPOSE_CONTROLLER_4(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xb0 | midi_channel), 0x13))
#define AGS_RECALL_MIDI1_CC_CHANGE_PROGRAM(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xc0 | midi_channel), 0x0))
#define AGS_RECALL_MIDI1_CC_CHANGE_PRESSURE(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xd0 | midi_channel), 0x0))
#define AGS_RECALL_MIDI1_CC_PITCH_BEND(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xe0 | midi_channel), 0x0))
#define AGS_RECALL_MIDI1_CC_SONG_POSITION(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xf2 | midi_channel), 0x0))
#define AGS_RECALL_MIDI1_CC_SONG_SELECT(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xf3 | midi_channel), 0x0))
#define AGS_RECALL_MIDI1_CC_TUNE_REQUEST(midi_channel) (AGS_RECALL_MIDI1_CONTROL_CHANGE((0xf6 | midi_channel), 0x0))

#define AGS_RECALL_MIDI2_CC_MIDI1_BANK_SELECT(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x0, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_MIDI1_MODULATION_WHEEL(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x1, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_MIDI1_BREATH_CONTROLLER(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x2, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_MIDI1_FOOT_CONTROLLER(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x4, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_MIDI1_PORTAMENTO_TIME(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x5, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_MIDI1_CHANNEL_VOLUME(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x7, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_MIDI1_BALANCE(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x8, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_MIDI1_PAN(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0xa, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_MIDI1_EXPRESSION_CONTROLLER(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0xb, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_MIDI1_EFFECT_CONTROL_1(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0xc, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_MIDI1_EFFECT_CONTROL_2(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0xd, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_MIDI1_GENERAL_PURPOSE_CONTROLLER_1(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x10, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_MIDI1_GENERAL_PURPOSE_CONTROLLER_2(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x11, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_MIDI1_GENERAL_PURPOSE_CONTROLLER_3(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x12, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_MIDI1_GENERAL_PURPOSE_CONTROLLER_4(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xb0 | midi_channel), 0x13, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_MIDI1_CHANGE_PROGRAM(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xc0 | midi_channel), 0x0, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_MIDI1_CHANGE_PRESSURE(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xd0 | midi_channel), 0x0, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_MIDI1_PITCH_BEND(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xe0 | midi_channel), 0x0, 0x0, 0))
#define AGS_RECALL_MIDI2_CC_CHANGE_PROGRAM(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xc0 | midi_channel), 0x0, 0))
#define AGS_RECALL_MIDI2_CC_CHANGE_PRESSURE(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xd0 | midi_channel), 0x0, 0))
#define AGS_RECALL_MIDI2_CC_PITCH_BEND(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xe0 | midi_channel), 0x0, 0))
#define AGS_RECALL_MIDI2_CC_PER_NOTE_PITCH_BEND(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0xf0 | midi_group), (0xe0 | midi_channel), midi_note, 0))
#define AGS_RECALL_MIDI2_CC_MODULATION(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 1))
#define AGS_RECALL_MIDI2_CC_BREATH(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 2))
#define AGS_RECALL_MIDI2_CC_PITCH_7_25(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 3))
#define AGS_RECALL_MIDI2_CC_VOLUME(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 7))
#define AGS_RECALL_MIDI2_CC_BALANCE(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 8))
#define AGS_RECALL_MIDI2_CC_PAN(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 10))
#define AGS_RECALL_MIDI2_CC_EXPRESSION(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 11))
#define AGS_RECALL_MIDI2_CC_SOUND_VARIATION(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 70))
#define AGS_RECALL_MIDI2_CC_TIMBRE(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 71))
#define AGS_RECALL_MIDI2_CC_RELEASE(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 72))
#define AGS_RECALL_MIDI2_CC_ATTACK(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 73))
#define AGS_RECALL_MIDI2_CC_BRIGHTNESS(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 74))
#define AGS_RECALL_MIDI2_CC_DECAY(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 75))
#define AGS_RECALL_MIDI2_CC_VIBRATO_RATE(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 76))
#define AGS_RECALL_MIDI2_CC_VIBRATO_DEPTH(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 77))
#define AGS_RECALL_MIDI2_CC_VIBRATO_DELAY(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 78))
#define AGS_RECALL_MIDI2_CC_SOUND_CONTROLLER_10(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 79))
#define AGS_RECALL_MIDI2_CC_EFFECTS_1(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 91))
#define AGS_RECALL_MIDI2_CC_EFFECTS_2(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 92))
#define AGS_RECALL_MIDI2_CC_EFFECTS_3(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 93))
#define AGS_RECALL_MIDI2_CC_EFFECTS_4(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 94))
#define AGS_RECALL_MIDI2_CC_EFFECTS_5(midi_group, midi_channel, midi_note) (AGS_RECALL_MIDI2_CONTROL_CHANGE((0x40 | midi_group), (0xb0 | midi_channel), 0x0, 95))

#define AGS_RECALL_DEFAULT_VERSION "7.0.0"
#define AGS_RECALL_DEFAULT_BUILD_ID "Fri Apr 26 12:33:34 UTC 2024"

typedef struct _AgsRecall AgsRecall;
typedef struct _AgsRecallClass AgsRecallClass;
typedef struct _AgsRecallHandler AgsRecallHandler;

/**
 * AgsRecallFlags:
 * @AGS_RECALL_TEMPLATE: is template
 * @AGS_RECALL_DEFAULT_TEMPLATE: is default template
 * @AGS_RECALL_HAS_OUTPUT_PORT: has output port
 * @AGS_RECALL_BYPASS: don't apply effect processing
 * @AGS_RECALL_INITIAL_RUN: initial run, first attack to audio data
 * @AGS_RECALL_MIDI1: enable MIDI version 1
 * @AGS_RECALL_MIDI1_CONTROL_CHANGE: enable MIDI version 1 control change
 * @AGS_RECALL_MIDI2: enable MIDI version 2
 * @AGS_RECALL_MIDI2_CONTROL_CHANGE: enable MIDI version 2 control change
 * 
 * Enum values to control the behavior or indicate internal state of #AgsRecall by
 * enable/disable as flags.
 */
typedef enum{
  AGS_RECALL_TEMPLATE              = 1,
  AGS_RECALL_DEFAULT_TEMPLATE      = 1 <<  1,
  AGS_RECALL_HAS_OUTPUT_PORT       = 1 <<  2,
  AGS_RECALL_BYPASS                = 1 <<  3,
  AGS_RECALL_INITIAL_RUN           = 1 <<  4,
  AGS_RECALL_MIDI1                 = 1 <<  5,
  AGS_RECALL_MIDI1_CONTROL_CHANGE  = 1 <<  6,
  AGS_RECALL_MIDI2                 = 1 <<  7,
  AGS_RECALL_MIDI2_CONTROL_CHANGE  = 1 <<  8,
}AgsRecallFlags;

/**
 * AgsRecallNotifyDependencyMode:
 * @AGS_RECALL_NOTIFY_RUN: notify dependency as calling run
 * @AGS_RECALL_NOTIFY_AUDIO: notify dependency audio
 * @AGS_RECALL_NOTIFY_AUDIO_RUN: notifiy dependency audio run
 * @AGS_RECALL_NOTIFY_CHANNEL: notifiy dependency channel
 * @AGS_RECALL_NOTIFY_CHANNEL_RUN: notifiy dependency channel run
 * @AGS_RECALL_NOTIFY_RECALL: notifiy dependency recall
 * 
 * Modes to notify of dependencies.
 */
typedef enum{
  AGS_RECALL_NOTIFY_RUN,
  AGS_RECALL_NOTIFY_AUDIO,
  AGS_RECALL_NOTIFY_AUDIO_RUN,
  AGS_RECALL_NOTIFY_CHANNEL,
  AGS_RECALL_NOTIFY_CHANNEL_RUN,
  AGS_RECALL_NOTIFY_RECALL,
}AgsRecallNotifyDependencyMode;

/**
 * AgsRecallMidi1ControlChange:
 * @AGS_RECALL_MIDI1_BANK_SELECT: MIDI version 1 bank select
 * @AGS_RECALL_MIDI1_MODULATION_WHEEL: MIDI version 1 modulation wheel
 * @AGS_RECALL_MIDI1_BREATH_CONTROLLER: MIDI version 1 breath controller
 * @AGS_RECALL_MIDI1_FOOT_CONTROLLER: MIDI version 1 foot controller
 * @AGS_RECALL_MIDI1_PORTAMENTO_TIME: MIDI version 1 portamento time
 * @AGS_RECALL_MIDI1_CHANNEL_VOLUME: MIDI version 1 channel volume
 * @AGS_RECALL_MIDI1_BALANCE: MIDI version 1 balance
 * @AGS_RECALL_MIDI1_PAN: MIDI version 1 PAN
 * @AGS_RECALL_MIDI1_EXPRESSION_CONTROLLER: MIDI version 1 expression controller
 * @AGS_RECALL_MIDI1_EFFECT_CONTROL_1: MIDI version 1 effect control 1
 * @AGS_RECALL_MIDI1_EFFECT_CONTROL_2: MIDI version 1 effect control 2
 * @AGS_RECALL_MIDI1_GENERAL_PURPOSE_CONTROLLER_1: MIDI version 1 general purpose controller 1
 * @AGS_RECALL_MIDI1_GENERAL_PURPOSE_CONTROLLER_2: MIDI version 1 general purpose controller 2
 * @AGS_RECALL_MIDI1_GENERAL_PURPOSE_CONTROLLER_3: MIDI version 1 general purpose controller 3
 * @AGS_RECALL_MIDI1_GENERAL_PURPOSE_CONTROLLER_4: MIDI version 1 general purpose controller 4
 * @AGS_RECALL_MIDI1_CHANGE_PROGRAM: MIDI version 1 change program
 * @AGS_RECALL_MIDI1_CHANGE_PRESSURE: MIDI version 1 change pressure
 * @AGS_RECALL_MIDI1_PITCH_BEND: MIDI version 1 pitch bend
 * @AGS_RECALL_MIDI1_SONG_POSITION: MIDI version 1 song position
 * @AGS_RECALL_MIDI1_SONG_SELECT: MIDI version 1 song select
 * @AGS_RECALL_MIDI1_TUNE_REQUEST: MIDI version 1 tune request
 * 
 * Modes to handle MIDI version 1 control change and others.
 */
typedef enum{
  AGS_RECALL_MIDI1_BANK_SELECT = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xb0, 0x0),
  AGS_RECALL_MIDI1_MODULATION_WHEEL = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xb0, 0x1),
  AGS_RECALL_MIDI1_BREATH_CONTROLLER = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xb0, 0x2),
  AGS_RECALL_MIDI1_FOOT_CONTROLLER = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xb0, 0x4),
  AGS_RECALL_MIDI1_PORTAMENTO_TIME = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xb0, 0x5),
  AGS_RECALL_MIDI1_CHANNEL_VOLUME = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xb0, 0x7),
  AGS_RECALL_MIDI1_BALANCE = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xb0, 0x8),
  AGS_RECALL_MIDI1_PAN = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xb0, 0xa),
  AGS_RECALL_MIDI1_EXPRESSION_CONTROLLER = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xb0, 0xb),
  AGS_RECALL_MIDI1_EFFECT_CONTROL_1 = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xb0, 0xc),
  AGS_RECALL_MIDI1_EFFECT_CONTROL_2 = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xb0, 0xd),
  AGS_RECALL_MIDI1_GENERAL_PURPOSE_CONTROLLER_1 = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xb0, 0x10),
  AGS_RECALL_MIDI1_GENERAL_PURPOSE_CONTROLLER_2 = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xb0, 0x11),
  AGS_RECALL_MIDI1_GENERAL_PURPOSE_CONTROLLER_3 = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xb0, 0x12),
  AGS_RECALL_MIDI1_GENERAL_PURPOSE_CONTROLLER_4 = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xb0, 0x13),
  AGS_RECALL_MIDI1_CHANGE_PROGRAM = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xc0, 0x0),
  AGS_RECALL_MIDI1_CHANGE_PRESSURE = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xd0, 0x0),
  AGS_RECALL_MIDI1_PITCH_BEND = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xe0, 0x0),
  AGS_RECALL_MIDI1_SONG_POSITION = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xf2, 0x0),
  AGS_RECALL_MIDI1_SONG_SELECT = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xf3, 0x0),
  AGS_RECALL_MIDI1_TUNE_REQUEST = AGS_RECALL_MIDI1_CONTROL_CHANGE(0xf6, 0x0),
}AgsRecallMidi1ControlChange;

/**
 * AgsRecallMidi2ControlChange:
 * @AGS_RECALL_MIDI2_MIDI1_BANK_SELECT: MIDI version 2 bank select
 * @AGS_RECALL_MIDI2_MIDI1_MODULATION_WHEEL: MIDI version 2 modulation wheel
 * @AGS_RECALL_MIDI2_MIDI1_BREATH_CONTROLLER: MIDI version 2 breath controller
 * @AGS_RECALL_MIDI2_MIDI1_FOOT_CONTROLLER: MIDI version 2 foot controller
 * @AGS_RECALL_MIDI2_MIDI1_PORTAMENTO_TIME: MIDI version 2 portamento time
 * @AGS_RECALL_MIDI2_MIDI1_CHANNEL_VOLUME: MIDI version 2 channel volume
 * @AGS_RECALL_MIDI2_MIDI1_BALANCE: MIDI version 2 balance
 * @AGS_RECALL_MIDI2_MIDI1_PAN: MIDI version 2 PAN
 * @AGS_RECALL_MIDI2_MIDI1_EXPRESSION_CONTROLLER: MIDI version 2 expression controller
 * @AGS_RECALL_MIDI2_MIDI1_EFFECT_CONTROL_1: MIDI version 2 effect control 1
 * @AGS_RECALL_MIDI2_MIDI1_EFFECT_CONTROL_2: MIDI version 2 effect control 2
 * @AGS_RECALL_MIDI2_MIDI1_GENERAL_PURPOSE_CONTROLLER_1: MIDI version 2 general purpose controller 1
 * @AGS_RECALL_MIDI2_MIDI1_GENERAL_PURPOSE_CONTROLLER_2: MIDI version 2 general purpose controller 2
 * @AGS_RECALL_MIDI2_MIDI1_GENERAL_PURPOSE_CONTROLLER_3: MIDI version 2 general purpose controller 3
 * @AGS_RECALL_MIDI2_MIDI1_GENERAL_PURPOSE_CONTROLLER_4: MIDI version 2 general purpose controller 4
 * @AGS_RECALL_MIDI2_MIDI1_CHANGE_PROGRAM: MIDI version 2 change program
 * @AGS_RECALL_MIDI2_MIDI1_CHANGE_PRESSURE: MIDI version 2 change pressure
 * @AGS_RECALL_MIDI2_MIDI1_PITCH_BEND: MIDI version 2 pitch bend
 * @AGS_RECALL_MIDI2_CHANGE_PROGRAM: MIDI version 2 change program
 * @AGS_RECALL_MIDI2_CHANGE_PRESSURE: MIDI version 2 change pressure
 * @AGS_RECALL_MIDI2_PITCH_BEND: MIDI version 2 pitch bend
 * @AGS_RECALL_MIDI2_MODULATION: MIDI version 2 modulation
 * @AGS_RECALL_MIDI2_BREATH: MIDI version 2 breath
 * @AGS_RECALL_MIDI2_PITCH_7_25: MIDI version 2 pitch 7.25
 * @AGS_RECALL_MIDI2_VOLUME: MIDI version 2 volume
 * @AGS_RECALL_MIDI2_BALANCE: MIDI version 2 balance
 * @AGS_RECALL_MIDI2_PAN: MIDI version 2 pan
 * @AGS_RECALL_MIDI2_EXPRESSION: MIDI version 2 expression
 * @AGS_RECALL_MIDI2_SOUND_VARIATION: MIDI version 2 sound variation
 * @AGS_RECALL_MIDI2_TIMBRE: MIDI version 2 timbre
 * @AGS_RECALL_MIDI2_RELEASE: MIDI version 2 release
 * @AGS_RECALL_MIDI2_ATTACK: MIDI version 2 attack
 * @AGS_RECALL_MIDI2_BRIGHTNESS: MIDI version 2 brightness
 * @AGS_RECALL_MIDI2_DECAY: MIDI version 2 decay
 * @AGS_RECALL_MIDI2_VIBRATO_RATE: MIDI version 2 vibrato rate 
 * @AGS_RECALL_MIDI2_VIBRATO_DEPTH: MIDI version 2 vibrato depth
 * @AGS_RECALL_MIDI2_VIBRATO_DELAY: MIDI version 2 vibrato delay
 * @AGS_RECALL_MIDI2_SOUND_CONTROLLER_10: MIDI version 2 sound controller 10
 * @AGS_RECALL_MIDI2_EFFECTS_1: MIDI version 2 effects 1
 * @AGS_RECALL_MIDI2_EFFECTS_2: MIDI version 2 effects 2
 * @AGS_RECALL_MIDI2_EFFECTS_3: MIDI version 2 effects 3
 * @AGS_RECALL_MIDI2_EFFECTS_4: MIDI version 2 effects 4
 * @AGS_RECALL_MIDI2_EFFECTS_5: MIDI version 2 effects 5
 * 
 * Modes to handle MIDI version 2 control change and others.
 */
typedef enum{
  AGS_RECALL_MIDI2_MIDI1_BANK_SELECT                   = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xb0, 0x0, 0x0, 0),
  AGS_RECALL_MIDI2_MIDI1_MODULATION_WHEEL              = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xb0, 0x1, 0x0, 0),
  AGS_RECALL_MIDI2_MIDI1_BREATH_CONTROLLER             = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xb0, 0x2, 0x0, 0),
  AGS_RECALL_MIDI2_MIDI1_FOOT_CONTROLLER               = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xb0, 0x4, 0x0, 0),
  AGS_RECALL_MIDI2_MIDI1_PORTAMENTO_TIME               = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xb0, 0x5, 0x0, 0),
  AGS_RECALL_MIDI2_MIDI1_CHANNEL_VOLUME                = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xb0, 0x7, 0x0, 0),
  AGS_RECALL_MIDI2_MIDI1_BALANCE                       = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xb0, 0x8, 0x0, 0),
  AGS_RECALL_MIDI2_MIDI1_PAN                           = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xb0, 0xa, 0x0, 0),
  AGS_RECALL_MIDI2_MIDI1_EXPRESSION_CONTROLLER         = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xb0, 0xb, 0x0, 0),
  AGS_RECALL_MIDI2_MIDI1_EFFECT_CONTROL_1              = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xb0, 0xc, 0x0, 0),
  AGS_RECALL_MIDI2_MIDI1_EFFECT_CONTROL_2              = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xb0, 0xd, 0x0, 0),
  AGS_RECALL_MIDI2_MIDI1_GENERAL_PURPOSE_CONTROLLER_1  = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xb0, 0x10, 0x0, 0),
  AGS_RECALL_MIDI2_MIDI1_GENERAL_PURPOSE_CONTROLLER_2  = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xb0, 0x11, 0x0, 0),
  AGS_RECALL_MIDI2_MIDI1_GENERAL_PURPOSE_CONTROLLER_3  = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xb0, 0x12, 0x0, 0),
  AGS_RECALL_MIDI2_MIDI1_GENERAL_PURPOSE_CONTROLLER_4  = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xb0, 0x13, 0x0, 0),
  AGS_RECALL_MIDI2_MIDI1_CHANGE_PROGRAM                = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xc0, 0x0, 0x0, 0),
  AGS_RECALL_MIDI2_MIDI1_CHANGE_PRESSURE               = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xd0, 0x0, 0x0, 0),
  AGS_RECALL_MIDI2_MIDI1_PITCH_BEND                    = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xe0, 0x0, 0x0, 0),
  AGS_RECALL_MIDI2_CHANGE_PROGRAM                      = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xc0, 0xc0, 0x0, 0),
  AGS_RECALL_MIDI2_CHANGE_PRESSURE                     = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xd0, 0xd0, 0x0, 0),
  AGS_RECALL_MIDI2_PITCH_BEND                          = AGS_RECALL_MIDI2_CONTROL_CHANGE(0xe0, 0xe0, 0x0, 0),
  AGS_RECALL_MIDI2_MODULATION                          = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 1),
  AGS_RECALL_MIDI2_BREATH                              = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 2),
  AGS_RECALL_MIDI2_PITCH_7_25                          = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 3),
  AGS_RECALL_MIDI2_VOLUME                              = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 7),
  AGS_RECALL_MIDI2_BALANCE                             = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 8),
  AGS_RECALL_MIDI2_PAN                                 = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 10),
  AGS_RECALL_MIDI2_EXPRESSION                          = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 11),
  AGS_RECALL_MIDI2_SOUND_VARIATION                     = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 70),
  AGS_RECALL_MIDI2_TIMBRE                              = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 71),
  AGS_RECALL_MIDI2_RELEASE                             = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 72),
  AGS_RECALL_MIDI2_ATTACK                              = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 73),
  AGS_RECALL_MIDI2_BRIGHTNESS                          = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 74),
  AGS_RECALL_MIDI2_DECAY                               = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 75),
  AGS_RECALL_MIDI2_VIBRATO_RATE                        = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 76),
  AGS_RECALL_MIDI2_VIBRATO_DEPTH                       = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 77),
  AGS_RECALL_MIDI2_VIBRATO_DELAY                       = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 78),
  AGS_RECALL_MIDI2_SOUND_CONTROLLER_10                 = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 79),
  AGS_RECALL_MIDI2_EFFECTS_1                           = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 91),
  AGS_RECALL_MIDI2_EFFECTS_2                           = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 92),
  AGS_RECALL_MIDI2_EFFECTS_3                           = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 93),
  AGS_RECALL_MIDI2_EFFECTS_4                           = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 94),
  AGS_RECALL_MIDI2_EFFECTS_5                           = AGS_RECALL_MIDI2_CONTROL_CHANGE(0x40, 0xb0, 0x0, 95),
}AgsRecallMidi2ControlChange;

struct _AgsRecall
{
  GObject gobject;

  AgsRecallFlags flags;
  AgsConnectableFlags connectable_flags;
  AgsSoundAbilityFlags ability_flags;
  AgsSoundBehaviourFlags behaviour_flags;
  gint sound_scope;
  AgsSoundStagingFlags staging_flags;
  AgsSoundStateFlags state_flags;
  
  //  gboolean rt_safe; note replace by globals

  GRecMutex obj_mutex;
  
  AgsUUID *uuid;

  gchar *version;
  gchar *build_id;

  gchar *name;

  gchar *filename;
  gchar *effect;
  guint effect_index;
  
  gchar *xml_type;
  
  GObject *recall_container;

  GObject *output_soundcard;
  gint output_soundcard_channel;
  
  GObject *input_soundcard;
  gint input_soundcard_channel;
  
  guint samplerate;
  guint buffer_size;
  AgsSoundcardFormat format;

  guint pad;
  guint audio_channel;

  guint line;

  GList *port;
  GList *automation_port;
  
  AgsRecallID *recall_id;

  GList *recall_dependency;
  GList *recall_handler;  

  AgsRecall *parent;
  
  GType child_type;
  
  guint n_child_params;
  gchar **child_parameter_name;
  GValue *child_value;

  GList *children;  

  AgsMidiUtil *midi_util;  
  AgsMidiUmpUtil *midi_ump_util;

  GHashTable *midi1_cc_to_value;
  GHashTable *midi1_cc_to_port_specifier;

  GHashTable *midi2_cc_to_value;
  GHashTable *midi2_cc_to_port_specifier;

  GHashTable *jack_metadata_to_value;
  GHashTable *jack_metadata_to_port_specifier;
};

struct _AgsRecallClass
{
  GObjectClass gobject;

  void (*resolve_dependency)(AgsRecall *recall);
  void (*check_rt_data)(AgsRecall *recall);
  
  void (*run_init_pre)(AgsRecall *recall);
  void (*run_init_inter)(AgsRecall *recall);
  void (*run_init_post)(AgsRecall *recall);
  
  void (*feed_input_queue)(AgsRecall *recall);
  void (*automate)(AgsRecall *recall);
  
  void (*run_pre)(AgsRecall *recall);
  void (*run_inter)(AgsRecall *recall);
  void (*run_post)(AgsRecall *recall);

  void (*do_feedback)(AgsRecall *recall);
  void (*feed_output_queue)(AgsRecall *recall);
  
  void (*stop_persistent)(AgsRecall *recall);
  void (*cancel)(AgsRecall *recall);
  void (*done)(AgsRecall *recall);

  AgsRecall* (*duplicate)(AgsRecall *recall,
			  AgsRecallID *recall_id,
			  guint *n_params, gchar **parameter_name, GValue *value);

  void (*notify_dependency)(AgsRecall *recall, guint dependency, gboolean increase);

  void (*child_added)(AgsRecall *recall, AgsRecall *child);

  void (*midi1_control_change)(AgsRecall *recall);

  void (*midi2_control_change)(AgsRecall *recall);

  void (*jack_metadata)(AgsRecall *recall);
};

/**
 * AgsRecallHandler:
 * @signal_name: the signal to listen
 * @callback: the callback to use
 * @data: user data to pass
 *
 * A #AgsRecallHandler-struct acts as a callback definition
 */
struct _AgsRecallHandler
{
  const gchar *signal_name;
  GCallback callback;
  GObject *data;
};

GType ags_recall_get_type();

GType ags_recall_flags_get_type();
GType ags_recall_notify_dependency_mode_get_type();

void ags_recall_global_set_omit_event(gboolean omit_event);

gboolean ags_recall_global_get_children_lock_free();
gboolean ags_recall_global_get_omit_event();
gboolean ags_recall_global_get_performance_mode();
gboolean ags_recall_global_get_rt_safe();

GRecMutex* ags_recall_get_obj_mutex(AgsRecall *recall);

gboolean ags_recall_test_flags(AgsRecall *recall, AgsRecallFlags flags);
void ags_recall_set_flags(AgsRecall *recall, AgsRecallFlags flags);
void ags_recall_unset_flags(AgsRecall *recall, AgsRecallFlags flags);

/* ability flags */
gboolean ags_recall_test_ability_flags(AgsRecall *recall, AgsSoundAbilityFlags ability_flags);
void ags_recall_set_ability_flags(AgsRecall *recall, AgsSoundAbilityFlags ability_flags);
void ags_recall_unset_ability_flags(AgsRecall *recall, AgsSoundAbilityFlags ability_flags);

gboolean ags_recall_check_ability_flags(AgsRecall *recall, AgsSoundAbilityFlags ability_flags);
gboolean ags_recall_match_ability_flags_to_scope(AgsRecall *recall, gint sound_scope);

/* behaviour flags */
gboolean ags_recall_test_behaviour_flags(AgsRecall *recall, AgsSoundBehaviourFlags behaviour_flags);
void ags_recall_set_behaviour_flags(AgsRecall *recall, AgsSoundBehaviourFlags behaviour_flags);
void ags_recall_unset_behaviour_flags(AgsRecall *recall, AgsSoundBehaviourFlags behaviour_flags);

gboolean ags_recall_check_behaviour_flags(AgsRecall *recall, AgsSoundBehaviourFlags behaviour_flags);

/* scope */
void ags_recall_set_sound_scope(AgsRecall *recall, gint sound_scope);
gint ags_recall_get_sound_scope(AgsRecall *recall);

gboolean ags_recall_check_sound_scope(AgsRecall *recall, gint sound_scope);

/* staging flags */
gboolean ags_recall_test_staging_flags(AgsRecall *recall, AgsSoundStagingFlags staging_flags);
void ags_recall_set_staging_flags(AgsRecall *recall, AgsSoundStagingFlags staging_flags);
void ags_recall_unset_staging_flags(AgsRecall *recall, AgsSoundStagingFlags staging_flags);

gboolean ags_recall_check_staging_flags(AgsRecall *recall, AgsSoundStagingFlags staging_flags);

/* state flags */
gboolean ags_recall_test_state_flags(AgsRecall *recall, AgsSoundStateFlags state_flags);
void ags_recall_set_state_flags(AgsRecall *recall, AgsSoundStateFlags state_flags);
void ags_recall_unset_state_flags(AgsRecall *recall, AgsSoundStateFlags state_flags);

gboolean ags_recall_check_state_flags(AgsRecall *recall, AgsSoundStateFlags state_flags);

/* fields */
gchar* ags_recall_get_filename(AgsRecall *recall);
void ags_recall_set_filename(AgsRecall *recall,
			     gchar *filename);

gchar* ags_recall_get_effect(AgsRecall *recall);
void ags_recall_set_effect(AgsRecall *recall,
			   gchar *effect);

guint ags_recall_get_effect_index(AgsRecall *recall);
void ags_recall_set_effect_index(AgsRecall *recall,
				 guint effect_index);

GObject* ags_recall_get_recall_container(AgsRecall *recall);
void ags_recall_set_recall_container(AgsRecall *recall,
				     GObject *recall_container);

/* children */
AgsRecallID* ags_recall_get_recall_id(AgsRecall *recall);
void ags_recall_set_recall_id(AgsRecall *recall,
			      AgsRecallID *recall_id);

GList* ags_recall_get_recall_dependency(AgsRecall *recall);
void ags_recall_set_recall_dependency(AgsRecall *recall,
				      GList *recall_dependency);

void ags_recall_add_recall_dependency(AgsRecall *recall,
				      AgsRecallDependency *recall_dependency);
void ags_recall_remove_recall_dependency(AgsRecall *recall,
					 AgsRecallDependency *recall_dependency);

GList* ags_recall_get_port(AgsRecall *recall);
void ags_recall_set_port(AgsRecall *recall, GList *port);

void ags_recall_add_port(AgsRecall *recall,
			 AgsPort *port);
void ags_recall_remove_port(AgsRecall *recall,
			    AgsPort *port);

GList* ags_recall_get_children(AgsRecall *recall);
void ags_recall_set_children(AgsRecall *recall,
			     GList *children);

void ags_recall_add_child(AgsRecall *recall, AgsRecall *child);
void ags_recall_remove_child(AgsRecall *recall, AgsRecall *child);

void ags_recall_handler_free(AgsRecallHandler *recall_handler);
AgsRecallHandler* ags_recall_handler_alloc(const gchar *signal_name,
					   GCallback callback,
					   GObject *data);

void ags_recall_add_recall_handler(AgsRecall *recall,
				   AgsRecallHandler *recall_handler);
void ags_recall_remove_recall_handler(AgsRecall *recall,
				      AgsRecallHandler *recall_handler);

/* soundcard */
GObject* ags_recall_get_output_soundcard(AgsRecall *recall);
void ags_recall_set_output_soundcard(AgsRecall *recall, GObject *output_soundcard);

GObject* ags_recall_get_input_soundcard(AgsRecall *recall);
void ags_recall_set_input_soundcard(AgsRecall *recall, GObject *input_soundcard);

/* presets */
guint ags_recall_get_samplerate(AgsRecall *recall);
void ags_recall_set_samplerate(AgsRecall *recall, guint samplerate);

guint ags_recall_get_buffer_size(AgsRecall *recall);
void ags_recall_set_buffer_size(AgsRecall *recall, guint buffer_size);

AgsSoundcardFormat ags_recall_get_format(AgsRecall *recall);
void ags_recall_set_format(AgsRecall *recall, AgsSoundcardFormat format);

/* MIDI version 1.0 control change */
GHashTable* ags_recall_get_midi1_cc_to_value(AgsRecall *recall);
GHashTable* ags_recall_get_midi1_cc_to_port_specifier(AgsRecall *recall);

/* MIDI version 2.0 control change */
GHashTable* ags_recall_get_midi2_cc_to_value(AgsRecall *recall);
GHashTable* ags_recall_get_midi2_cc_to_port_specifier(AgsRecall *recall);

/* events */
void ags_recall_resolve_dependency(AgsRecall *recall);
void ags_recall_check_rt_data(AgsRecall *recall);

void ags_recall_run_init_pre(AgsRecall *recall);
void ags_recall_run_init_inter(AgsRecall *recall);
void ags_recall_run_init_post(AgsRecall *recall);

void ags_recall_feed_input_queue(AgsRecall *recall);
void ags_recall_automate(AgsRecall *recall);

void ags_recall_run_pre(AgsRecall *recall);
void ags_recall_run_inter(AgsRecall *recall);
void ags_recall_run_post(AgsRecall *recall);

void ags_recall_do_feedback(AgsRecall *recall);
void ags_recall_feed_output_queue(AgsRecall *recall);

void ags_recall_stop_persistent(AgsRecall *recall);
void ags_recall_cancel(AgsRecall *recall);
void ags_recall_done(AgsRecall *recall);

void ags_recall_midi1_control_change(AgsRecall *recall);

void ags_recall_midi2_control_change(AgsRecall *recall);

void ags_recall_jack_metadata(AgsRecall *recall);

AgsRecall* ags_recall_duplicate(AgsRecall *recall,
				AgsRecallID *recall_id,
				guint *n_params, gchar **parameter_name, GValue *value);

void ags_recall_notify_dependency(AgsRecall *recall, guint dependency, gboolean increase);
void ags_recall_child_added(AgsRecall *recall, AgsRecall *child);

/* query */
gboolean ags_recall_is_done(GList *recall, GObject *recycling_context);

GList* ags_recall_get_by_effect(GList *recall, gchar *filename, gchar *effect);
GList* ags_recall_find_recall_id_with_effect(GList *recall, AgsRecallID *recall_id, gchar *filename, gchar *effect);

GList* ags_recall_find_type(GList *recall, GType type);
GList* ags_recall_find_template(GList *recall);
GList* ags_recall_template_find_type(GList *recall, GType type);
GList* ags_recall_template_find_all_type(GList *recall, ...);
GList* ags_recall_find_type_with_recycling_context(GList *recall, GType type, GObject *recycling_context);
GList* ags_recall_find_recycling_context(GList *recall, GObject *recycling_context);
GList* ags_recall_find_provider(GList *recall, GObject *provider);
GList* ags_recall_template_find_provider(GList *recall, GObject *provider);
GList* ags_recall_find_provider_with_recycling_context(GList *recall, GObject *provider, GObject *recycling_context);

/* control */
void ags_recall_lock_port(AgsRecall *recall);
void ags_recall_unlock_port(AgsRecall *recall);

/* instantiate */
AgsRecall* ags_recall_new();

G_END_DECLS

#endif /*__AGS_RECALL_H__*/
