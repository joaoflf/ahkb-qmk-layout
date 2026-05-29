#include QMK_KEYBOARD_H
#if __has_include("keymap.h")
#   include "keymap.h"
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Layer 0: Base Layout */
    [0] = LAYOUT_ferris_hlc(
        // Left hand
        KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,               KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
        KC_A,    KC_S,    KC_D,    KC_F,    KC_G,               KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN,
        KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,               KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
                                    OSL(1),   KC_LSFT,       KC_SPC, OSL(2),

        //
        KC_NO, KC_NO, KC_NO, KC_NO, KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO,
    ),

        /* Layer 1: Numbers, Mods & Navigation */
    [1] = LAYOUT_ferris_hlc(
        // Left hand
        KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                                   KC_6,    KC_7,    KC_8,    KC_9,    KC_0,
        OSM(MOD_LSFT), OSM(MOD_LCTL), OSM(MOD_LALT), OSM(MOD_LGUI), KC_TAB,         KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_BSPC,
        KC_MRWD, KC_MPLY, KC_MFFD, KC_VOLD, KC_VOLU,                                LCTL(KC_A), KC_PGDN, KC_PGUP, KC_DEL,  KC_ENT,
                                            KC_NO, KC_LSFT,                   KC_SPC, MO(3),
        //
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______


    ),

    /* Layer 2: Symbols */
    [2] = LAYOUT_ferris_hlc(

        KC_ESC, KC_LBRC, LSFT(KC_LBRC), LSFT(KC_9), LSFT(KC_GRV),                     LSFT(KC_6), LSFT(KC_0), LSFT(KC_RBRC) ,KC_RBRC, KC_GRV,
        KC_MINS, LSFT(KC_8), KC_PEQL, LSFT(KC_MINS), LSFT(KC_4),                     LSFT(KC_3), OSM(MOD_RGUI), OSM(MOD_RALT), OSM(MOD_RCTL), OSM(MOD_RSFT),
        LSFT(KC_EQL), LSFT(KC_BSLS), LSFT(KC_2), KC_QUOT, LSFT(KC_5),               LCTL(KC_A), KC_BSLS, LSFT(KC_7), LSFT(KC_QUOT), LSFT(KC_1),
                                                        MO(3), KC_LSFT,       KC_SPC, KC_NO,   // Right thumbs
        //
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

    /* Layer 3: F-Keys */
    [3] = LAYOUT_ferris_hlc(
        // Left hand
        RM_TOGG, RM_NEXT, RM_PREV,  RM_SATU,    RM_SATD,                              RM_HUEU,    RM_HUED,    RM_VALU,    RM_VALD,    RM_SPDU,
        OSM(MOD_LSFT), OSM(MOD_LCTL), OSM(MOD_LALT), OSM(MOD_LGUI), KC_F11,         KC_F12, OSM(MOD_RGUI), OSM(MOD_RALT), OSM(MOD_RCTL), OSM(MOD_RSFT),
        KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                                  KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,
                                        KC_NO, KC_LSFT,                       KC_SPC, KC_NO,
        //
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    )


};


// --- One-shot layer / mod chain handling ---------------------------------
// Goals:
//   * Cmd+S    (tap mode):  OSL(1) → tap F → tap S            → Cmd+S
//   * Cmd+L    (tap mode):  OSL(1) → tap F → tap L            → Cmd+L (no wait)
//   * Cmd+1    (tap mode):  OSL(1) → tap F → tap 1            → Cmd+1 (no wait)
//   * Cmd+Alt+V (hold chord): OSL(2) → hold J + K → press V   → Cmd+Alt+V
//
// Three pieces work together:
//
//   1. Counter + deferred OSL clear: while any OSM is held the OSL stays
//      alive; once everyone is released we arm a short fallback timer so
//      that *eventually* the OSL retires even if nothing else happens.
//
//   2. pre_process_record_user re-resolve: fires the instant a non-OSM
//      key is pressed while the OSL is still alive AND it's either a
//      held chord (OSMs currently down) or the post-tap chain window is
//      armed. Substitutes the base-layer keycode in place when the
//      layer-N keycode is something the user clearly didn't intend
//      (arrows/media/punctuation), and leaves it alone otherwise
//      (digits/letters/symbols/ENT/ESC/TAB/SPC/BSPC).
//
//   3. HOLD_ON_OTHER_KEY_PRESS_PER_KEY for right-hand OSMs only: lets a
//      held J→K→V chord register the modifiers as held mods immediately
//      instead of waiting for the tap term.
#define OSL_CHAIN_WINDOW_MS 100

// Layer 2 is the "chord with base-layer letters" launcher: any key
// pressed during a chord on this layer re-resolves to the base-layer
// letter (Cmd+L, Cmd+V, etc.). Other layers (e.g. layer 1) preserve
// their own keycodes so Cmd+Left, Cmd+1, Cmd+PgDn keep working.
#define OSL_CHORD_LAYER 2

static uint8_t  held_osms       = 0;
static bool     osl_clear_armed = false;
static uint32_t osl_clear_start = 0;

// Position-keyed table of "we substituted the base-layer keycode for this
// key; remember it so the release event can unregister the right keycode".
// 0 means "no substitution". MATRIX_ROWS/MATRIX_COLS come from the keyboard.
static uint16_t remap_keycode[MATRIX_ROWS][MATRIX_COLS] = {{0}};

// Force right-hand OSMs to settle as a hold the moment another key is
// pressed during their tap window, BUT ONLY when at least one other OSM
// is already held. Rationale:
//   * Single mod + letter (J then L): we want J to stay in its tap window
//     so it becomes a sticky LGUI; that arms our tap-mode re-resolve so
//     L can be substituted for the letter at its position.
//   * Multi-mod chord (J + K + V): the *second* held mod (K) needs to
//     settle quickly so its modifier is registered when V arrives.
// Left-hand OSMs keep the default tap-vs-hold behavior.
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    if (IS_QK_ONE_SHOT_MOD(keycode)) {
        if (QK_ONE_SHOT_MOD_GET_MODS(keycode) & 0x10) {
            return held_osms > 0;
        }
    }
    return false;
}

// Decide whether `kc` (the layer-N keycode at this position) should be
// silently replaced with `base` (the layer-0 keycode) when a chord/chain
// re-resolve is taking place.
//
// Rule of thumb:
//   * OSMs anywhere → re-resolve to the base letter (Cmd+S from
//     layer 1's OSM(LCTL), Cmd+L from layer 2's OSM(RCTL)).
//   * Anything else on the *chord layer* (layer 2) → re-resolve. This
//     gives you Cmd+V from KC_QUOT, Cmd+R from LSFT(KC_9), Cmd+Q from
//     KC_ESC, etc. Trade-off: Cmd+@ via held-J + C also re-resolves to
//     Cmd+C, and Cmd+ESC via Q on layer 2 becomes Cmd+Q.
//   * Anything else on *other layers* (e.g. layer 1) → pass through.
//     This preserves Cmd+Left (KC_LEFT), Cmd+1 (KC_1), Cmd+Enter,
//     Cmd+Tab, Cmd+BSPC etc. — they're all on layer 1 in this map.
//   * Letters and digits always pass through; basic-key positions that
//     resolve to non-letter base codes (KC_SCLN, KC_COMM, etc.) are
//     also left alone because they can't become a letter.
static bool should_reresolve_to_base(uint16_t kc, uint16_t base) {
    if (base < KC_A || base > KC_Z) return false;
    if (IS_QK_ONE_SHOT_MOD(kc)) return true;
    if (kc >= KC_A && kc <= KC_Z) return false;
    if (kc >= KC_1 && kc <= KC_0) return false;
    return get_oneshot_layer() == OSL_CHORD_LAYER;
}

static bool try_reresolve(uint16_t keycode, keyrecord_t *record) {
    const uint8_t row = record->event.key.row;
    const uint8_t col = record->event.key.col;
    uint16_t base_keycode = keymap_key_to_keycode(0, record->event.key);
    if (!should_reresolve_to_base(keycode, base_keycode)) return false;

    clear_oneshot_layer_state(ONESHOT_PRESSED | ONESHOT_OTHER_KEY_PRESSED);
    osl_clear_armed = false;
    register_code16(base_keycode);
    remap_keycode[row][col] = base_keycode;
    return true;
}

// We re-resolve in process_record_user (not pre_process) because:
//   * For held chords, pre_process fires on the chord-final key's raw
//     press before action_tapping has settled the held OSMs, so the
//     counter is still 0 and the chord path can't trigger.
//     process_record_user runs after tap-detect has settled the OSMs.
//   * Non-tap-record keys flow through process_record_user essentially
//     immediately on press anyway.
//
// Cases handled here:
//   CASE 1: Tap mode — only the first mod was tapped (osl_clear_armed,
//           counter == 0). Try to re-resolve; should_reresolve_to_base
//           decides per layer what passes through (Cmd+Left, Cmd+1) vs.
//           what becomes a base letter (Cmd+S, Cmd+L).
//   CASE 2: Held chord — at least one OSM is physically held. Re-resolve
//           by default, but treat OSMs on the chord layer that settled
//           as HOLDs (tap.count == 0) as chord members so they keep
//           their modifier role — that's how Cmd+Alt+V's K registers as
//           RALT instead of being substituted for letter K. OSMs that
//           settled as TAPs (tap.count == 1) and OSMs on non-chord
//           layers still re-resolve to their base letter.
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!IS_KEYEVENT(record->event)) return true;

    const uint8_t row = record->event.key.row;
    const uint8_t col = record->event.key.col;
    if (row >= MATRIX_ROWS || col >= MATRIX_COLS) return true;

    if (record->event.pressed) {
        if (!is_oneshot_layer_active()) return true;

        // CASE 1: tap-mode chain window, no mods currently held
        if (osl_clear_armed && held_osms == 0) {
            if (try_reresolve(keycode, record)) return false;
        }

        // CASE 2: held chord
        if (held_osms > 0) {
            bool is_osm = IS_QK_ONE_SHOT_MOD(keycode);
            bool is_chord_member = is_osm
                                && get_oneshot_layer() == OSL_CHORD_LAYER
                                && record->tap.count == 0;
            if (!is_chord_member) {
                if (try_reresolve(keycode, record)) return false;
            }
        }
    } else {
        if (remap_keycode[row][col] != 0) {
            unregister_code16(remap_keycode[row][col]);
            remap_keycode[row][col] = 0;
            return false;
        }
    }

    return true;
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!IS_QK_ONE_SHOT_MOD(keycode)) return;

    if (record->event.pressed) {
        held_osms++;
        osl_clear_armed = false;
    } else {
        if (held_osms > 0) held_osms--;
        if (held_osms == 0 && is_oneshot_layer_active()) {
            osl_clear_armed = true;
            osl_clear_start = timer_read32();
        }
    }
}

void matrix_scan_user(void) {
    if (osl_clear_armed && timer_elapsed32(osl_clear_start) >= OSL_CHAIN_WINDOW_MS) {
        osl_clear_armed = false;
        if (held_osms == 0 && is_oneshot_layer_active()) {
            clear_oneshot_layer_state(ONESHOT_OTHER_KEY_PRESSED);
        }
    }
}


// --------------------------------------------------------------------------------------------------

#if defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
// This section defines the behavior of the keyboard's rotary encoders.
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(MS_WHLU, MS_WHLD)},
    [1] = {ENCODER_CCW_CW(MS_WHLU, MS_WHLD), ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [2] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_BRID, KC_BRIU)},
    [3] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(MS_WHLU, MS_WHLD)},
};
#endif // defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)


#ifdef OTHER_KEYMAP_C
#   include OTHER_KEYMAP_C
#endif // OTHER_KEYMAP_C


