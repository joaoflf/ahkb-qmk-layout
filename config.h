#define ENCODER_RESOLUTION 2

// Enable per-key control over the "settle as hold when another key is pressed"
// behavior. We use it in keymap.c::get_hold_on_other_key_press() to give
// right-hand OSMs immediate hold semantics so chord-style use (e.g. holding
// J + K then pressing V) registers the modifiers in time, while left-hand
// OSMs keep the default tap-vs-hold behavior so quick Cmd+S still works.
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY
