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


void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (IS_QK_ONE_SHOT_MOD(keycode) && is_oneshot_layer_active() && record->event.pressed) {
        clear_oneshot_layer_state(ONESHOT_OTHER_KEY_PRESSED);
    }
    return;
}


// --------------------------------------------------------------------------------------------------

#if defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
// This section defines the behavior of the keyboard's rotary encoders.
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(MS_WHLU, MS_WHLD)},
    [1] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(MS_WHLU, MS_WHLD)},
    [2] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(MS_WHLU, MS_WHLD)},
    [3] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(MS_WHLU, MS_WHLD)},
};
#endif // defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)


#ifdef OTHER_KEYMAP_C
#   include OTHER_KEYMAP_C
#endif // OTHER_KEYMAP_C


