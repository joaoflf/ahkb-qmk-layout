# AHKB - Angry Hacking Keyboard

A QMK keymap for the SplitKB Halcyon Ferris keyboard.

## Setup Instructions

### 1. Install QMK Toolbox

Download and install QMK Toolbox from the [official releases page](https://github.com/qmk/qmk_toolbox/releases).

### 2. Set up QMK Userspace

Clone the SplitKB userspace to your QMK firmware users folder:

```bash
cd qmk_firmware/users/
git clone git@github.com:splitkb/qmk_userspace.git splitkb
```

### 3. Configure Global Userspace Path

Set the global userspace path (you MUST be located in the cloned userspace location for this to work correctly):

```bash
cd qmk_firmware/users/splitkb
qmk config user.overlay_dir="$(realpath .)"
```

### 4. Clone AHKB Keymap

Clone this keymap repository to the appropriate keymaps folder:

```bash
cd qmk_firmware/users/splitkb/keyboards/splitkb/halcyon/ferris/keymaps/
git clone [this-repo-url] ahkb
```

### 5. Compile Firmware

#### For the split with encoder:
```bash
qmk clean && qmk compile -kb splitkb/halcyon/ferris/rev1 -km ahkb -e HLC_ENCODER=1
```

#### For the split with display:
```bash
qmk clean && qmk compile -kb splitkb/halcyon/ferris/rev1 -km ahkb -e HLC_TFT_DISPLAY=1
```

## Layout

This keymap consists of 4 layers designed for efficient typing and programming:

### Layer 0: Base Layout (QWERTY)
```
Q  W  E  R  T     Y  U  I  O  P
A  S  D  F  G     H  J  K  L  ;
Z  X  C  V  B     N  M  ,  .  /
      OSL(1) SFT   SPC OSL(2)
```

### Layer 1: Numbers, Mods & Navigation
```
1  2  3  4  5         6  7  8  9  0
LSF LCT LAL LGU TAB   ←  ↓  ↑  →  BSP
RWD PLY FFD V- V+     CA PGD PGU DEL ENT
      --- SFT         SPC MO(3)
```

### Layer 2: Symbols
```
ESC [ { ( ~     ^ ) } ] `
-   * = _ $     # RGU RAL RCT RSF
+   \ @ ' %     CA \ & " !
    MO(3) SFT   SPC ---
```

### Layer 3: F-Keys & RGB
```
RGB+ RGB> --- SAT+ SAT-   --- --- --- --- ---
LSF  LCT  LAL LGU  F11    F12 RGU RAL RCT RSF
F1   F2   F3  F4   F5     F6  F7  F8  F9  F10
          --- SFT         SPC ---
```

**Key:**
- OSL(n) = One-shot layer n
- MO(n) = Momentary layer n
- OSM = One-shot modifier
- LSF/RSF = Left/Right Shift
- LCT/RCT = Left/Right Control
- LAL/RAL = Left/Right Alt
- LGU/RGU = Left/Right GUI (Cmd)
- CA = Ctrl+A (Select All)
- RGB+/RGB> = RGB Toggle/Next Mode
- SAT+/SAT- = RGB Saturation Up/Down

