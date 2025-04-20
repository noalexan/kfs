# Improvement Idea

## Keyboard

### Lock Keys

Actually lock keys are toggle on release, but is not standar, modern keyboard toggle thekey statement
on press, but with the current implrmentation repeat are not handled:

<!-- * [0x3A] = { "Caps Lock",    KEY_PRESS  }, -->
<!-- * [0x45] = { "Num Lock",     KEY_PRESS  }, -->
<!-- * [0x46] = { "Scroll Lock",  KEY_PRESS  }, -->
<!-- * [0xBA] = { "Caps Lock",    KEY_RELEASE }, -->
<!-- * [0xC5] = { "Num Lock",     KEY_RELEASE }, -->
<!-- * [0xC6] = { "Scroll Lock",  KEY_RELEASE }, -->
- Improvment way :

```c
    bool key_down[256] = {0};     //  tableau global ou save l'etat de chaque touche
    bool caps_lock_state = false; // ou num_lock_state, scroll_lock_state, etc.

    void handle_key_event(uint8_t scancode, bool is_press) {
        uint8_t code = scancode & 0x7F;

        if (is_press) {
            if (!key_down[code]) {
                key_down[code] = true;
                // PREMIER PRESS : on toggle le lock
                if (code == CAPS_LOCK_SCANCODE) {
                    caps_lock_state = !caps_lock_state;
                    // (Met à jour l'etat , etc.)
                }
                // ... autres touches
            }
            // sinon, c'est un repeat : on ignore pour les lock keys
        } else {
            key_down[code] = false;
            // On ne fait rien sur release pour les lock keys
        }
    }
```

## TSS

Follow Os dev to implement TSS
