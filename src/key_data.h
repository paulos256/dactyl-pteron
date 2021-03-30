#pragma once

#include "key.h"
#include "transform.h"

namespace scad {

// Key positioning data and description of layout and grouping of keys.
struct KeyData {
  KeyData(TransformList origin);

  Key key_tab;
  Key key_q;
  Key key_w;
  Key key_e;
  Key key_r;
  Key key_t;

  Key key_caps;
  Key key_a;
  Key key_s;
  Key key_d;
  Key key_f;
  Key key_g;

  Key key_shift;
  Key key_z;
  Key key_x;
  Key key_c;
  Key key_v;
  Key key_b;

  Key key_tilde;
  Key key_slash;
  Key key_left_arrow;
  Key key_right_arrow;

  Key key_thumb1;
  Key key_thumb2;
  Key key_thumb3;
  Key key_thumb4;

  // clang-format off
  KeyGrid grid {{
    { &key_tab,    &key_q,      &key_w,      &key_e,           &key_r,            &key_t},
    { &key_caps,   &key_a,      &key_s,      &key_d,           &key_f,            &key_g},
    { &key_shift,  &key_z,      &key_x,      &key_c,           &key_v,            &key_b},
    { nullptr,     &key_tilde,  &key_slash,  &key_left_arrow,  &key_right_arrow,  nullptr},
  }};
  // clang-format on

  std::vector<Key*> thumb_keys() {
    return { &key_thumb1, &key_thumb2, &key_thumb3, &key_thumb4,  };
  }

  std::vector<Key*> all_keys() {
    std::vector<Key*> keys;
    for (Key* key : thumb_keys()) {
      keys.push_back(key);
    }
    for (Key* key : grid.keys()) {
      keys.push_back(key);
    }
    return keys;
  }
};

}  // namespace scad
