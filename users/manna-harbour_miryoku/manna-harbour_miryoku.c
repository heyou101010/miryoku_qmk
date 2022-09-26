// Copyright 2022 Manna Harbour
// https://github.com/manna-harbour/miryoku

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
#define MIRYOKU_X(LAYER, STRING) [U_##LAYER] = U_MACRO_VA_ARGS(MIRYOKU_LAYERMAPPING_##LAYER, MIRYOKU_LAYER_##LAYER),
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};

#if defined (MIRYOKU_KLUDGE_THUMBCOMBOS)
const uint16_t PROGMEM thumbcombos_base_right[] = {LT(U_SYM, KC_ENT), LT(U_NUM, KC_BSPC), COMBO_END};
const uint16_t PROGMEM thumbcombos_base_left[] = {LT(U_NAV, KC_SPC), LT(U_MOUSE, KC_TAB), COMBO_END};
const uint16_t PROGMEM thumbcombos_nav[] = {KC_ENT, KC_BSPC, COMBO_END};
const uint16_t PROGMEM thumbcombos_mouse[] = {KC_BTN2, KC_BTN1, COMBO_END};
const uint16_t PROGMEM thumbcombos_media[] = {KC_MSTP, KC_MPLY, COMBO_END};
const uint16_t PROGMEM thumbcombos_num[] = {KC_0, KC_MINS, COMBO_END};
  #if defined (MIRYOKU_LAYERS_FLIP)
const uint16_t PROGMEM thumbcombos_sym[] = {KC_UNDS, KC_LPRN, COMBO_END};
  #else
const uint16_t PROGMEM thumbcombos_sym[] = {KC_RPRN, KC_UNDS, COMBO_END};
  #endif
const uint16_t PROGMEM thumbcombos_fun[] = {KC_SPC, KC_TAB, COMBO_END};
combo_t key_combos[COMBO_COUNT] = {
  COMBO(thumbcombos_base_right, LT(U_FUN, KC_DEL)),
  COMBO(thumbcombos_base_left, LT(U_MEDIA, KC_ESC)),
  COMBO(thumbcombos_nav, KC_DEL),
  COMBO(thumbcombos_mouse, KC_BTN3),
  COMBO(thumbcombos_media, KC_MUTE),
  COMBO(thumbcombos_num, KC_DOT),
  #if defined (MIRYOKU_LAYERS_FLIP)
  COMBO(thumbcombos_sym, KC_RPRN),
  #else
  COMBO(thumbcombos_sym, KC_LPRN),
  #endif
  COMBO(thumbcombos_fun, KC_APP)
};
#endif

/**************  CUSTOM CODE ***************/

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
  switch(keycode){
    case KC_F13: tap_code16(MAGIC_TOGGLE_CTL_GUI); return false;
    case KC_F14: tap_code16(CAPS_WORD); return false;
    default: return true;
  }  
}

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
  #define MIRYOKU_X(LAYER, STRING) [U_##LAYER] = { ENCODER_CCW_CW(KC_MS_WH_DOWN, KC_MS_WH_UP), ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
  MIRYOKU_LAYER_LIST
  #undef MIRYOKU_X
};

  #if defined(SWAP_HANDS_ENABLE)
  const uint8_t PROGMEM encoder_hand_swap_config[NUM_ENCODERS] = { 1, 0 };
  #endif
#endif

#if defined(OLED_ENABLE)
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
  if (!is_keyboard_master()) {
    return OLED_ROTATION_180;  // flips the display 180 degrees if offhand
  }
  return OLED_ROTATION_270;
}

void oled_render_layer_state(void) {
  uint8_t layer_num = get_highest_layer(layer_state);
  
  char layer [10];
  sprintf(layer, "L%d:", layer_num);
  oled_write_ln(layer, false);
  
  switch (layer_num) {
    #define MIRYOKU_X(LAYER, STRING) \
    case U_##LAYER: { char str_##LAYER [4]; sprintf(str_##LAYER, "%-5s", STRING); oled_write(str_##LAYER, false); } break;
    MIRYOKU_LAYER_LIST
    #undef MIRYOKU_X
  }
}

void oled_render_led_state(void) {
  led_t led_state = host_keyboard_led_state(); 

  oled_write_P(led_state.caps_lock ? PSTR("CAPS ") : PSTR("     "), false);
  oled_write_P(led_state.scroll_lock ? PSTR("SCRL ") : PSTR("     "), false);
  oled_write_P(PSTR("\n"), false);
}

void oled_render_mod_state(void) {
  uint8_t mod_state = get_mods();

  oled_write_P((mod_state & MOD_MASK_SHIFT) ? PSTR("S") : PSTR(" "), false);
  oled_write_P((mod_state & MOD_MASK_CTRL) ? PSTR("C") : PSTR(" "), false);
  oled_write_P((mod_state & MOD_MASK_ALT) ? PSTR("A") : PSTR(" "), false);
  oled_write_P((mod_state & MOD_MASK_GUI) ? PSTR("G") : PSTR(" "), false);
  oled_write_P(PSTR("\n"), false);
}

void oled_render_bootmagic_status(void) {
    /* Show Ctrl-Gui Swap options */
    static const char PROGMEM win_logo[2][3] = {
        {0x97, 0x98, 0},
        {0xb7, 0xb8, 0},
    };
    static const char PROGMEM mac_logo[2][3] = {
        {0x95, 0x96, 0},
        {0xb5, 0xb6, 0},
    };


    oled_write_P(PSTR("BTMGK\n"), false);

    oled_write_P(PSTR(" "), !keymap_config.swap_lctl_lgui);
    oled_write_P(win_logo[0], !keymap_config.swap_lctl_lgui);
    oled_write_P(PSTR(" "), !keymap_config.swap_lctl_lgui);
    oled_write_P(PSTR("\n"), false);
    oled_write_P(PSTR(" "), !keymap_config.swap_lctl_lgui);
    oled_write_P(win_logo[1], !keymap_config.swap_lctl_lgui);
    oled_write_P(PSTR(" "), !keymap_config.swap_lctl_lgui);
    oled_write_P(PSTR("\n"), false);
    oled_write_P(PSTR(" "), keymap_config.swap_lctl_lgui);
    oled_write_P(mac_logo[0], keymap_config.swap_lctl_lgui);
    oled_write_P(PSTR(" "), keymap_config.swap_lctl_lgui);
    oled_write_P(PSTR("\n"), false);
    oled_write_P(PSTR(" "), keymap_config.swap_lctl_lgui);
    oled_write_P(mac_logo[1], keymap_config.swap_lctl_lgui);
    oled_write_P(PSTR(" "), keymap_config.swap_lctl_lgui);
    oled_write_P(PSTR("\n"), false);
}
 
void oled_render_logo(void) {
  static const char PROGMEM crkbd_logo[] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4,
    0
    };

  oled_write_P(crkbd_logo, false);
}

bool oled_task_user(void) {
  if (is_keyboard_master()) {
    oled_render_layer_state();
    oled_render_mod_state();
    oled_render_led_state();
    oled_render_bootmagic_status();
  } else {
    oled_render_logo();
  }
  return false;
}

#endif // OLED_ENABLE
