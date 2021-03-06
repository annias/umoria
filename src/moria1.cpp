// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc code, mainly handles player movement, inventory, etc

#include "headers.h"
#include "externs.h"

// Changes speed of monsters relative to player -RAK-
// Note: When the player is sped up or slowed down, I simply change
// the speed of all the monsters. This greatly simplified the logic.
void change_speed(int num) {
    py.flags.speed += num;
    py.flags.status |= PY_SPEED;

    for (int i = mfptr - 1; i >= MIN_MONIX; i--) {
        m_list[i].cspeed += num;
    }
}

// Player bonuses -RAK-
//
// When an item is worn or taken off, this re-adjusts the player bonuses.
//     Factor =  1 : wear
//     Factor = -1 : removed
//
// Only calculates properties with cumulative effect.  Properties that
// depend on everything being worn are recalculated by calc_bonuses() -CJS-
void py_bonuses(inven_type *t_ptr, int factor) {
    int amount = t_ptr->p1 * factor;
    if (t_ptr->flags & TR_STATS) {
        for (int i = 0; i < 6; i++) {
            if ((1 << i) & t_ptr->flags) {
                bst_stat(i, amount);
            }
        }
    }
    if (TR_SEARCH & t_ptr->flags) {
        py.misc.srh += amount;
        py.misc.fos -= amount;
    }
    if (TR_STEALTH & t_ptr->flags) {
        py.misc.stl += amount;
    }
    if (TR_SPEED & t_ptr->flags) {
        change_speed(-amount);
    }
    if ((TR_BLIND & t_ptr->flags) && (factor > 0)) {
        py.flags.blind += 1000;
    }
    if ((TR_TIMID & t_ptr->flags) && (factor > 0)) {
        py.flags.afraid += 50;
    }
    if (TR_INFRA & t_ptr->flags) {
        py.flags.see_infra += amount;
    }
}

// Recalculate the effect of all the stuff we use. -CJS-
void calc_bonuses() {
    struct player_type::flags *p_ptr = &py.flags;
    struct player_type::misc *m_ptr = &py.misc;

    if (p_ptr->slow_digest) {
        p_ptr->food_digested++;
    }
    if (p_ptr->regenerate) {
        p_ptr->food_digested -= 3;
    }

    p_ptr->see_inv = false;
    p_ptr->teleport = false;
    p_ptr->free_act = false;
    p_ptr->slow_digest = false;
    p_ptr->aggravate = false;
    p_ptr->sustain_str = false;
    p_ptr->sustain_int = false;
    p_ptr->sustain_wis = false;
    p_ptr->sustain_con = false;
    p_ptr->sustain_dex = false;
    p_ptr->sustain_chr = false;
    p_ptr->fire_resist = false;
    p_ptr->acid_resist = false;
    p_ptr->cold_resist = false;
    p_ptr->regenerate = false;
    p_ptr->lght_resist = false;
    p_ptr->ffall = false;

    int old_dis_ac = m_ptr->dis_ac;

    m_ptr->ptohit = (int16_t) tohit_adj();   // Real To Hit
    m_ptr->ptodam = (int16_t) todam_adj();   // Real To Dam
    m_ptr->ptoac = (int16_t) toac_adj();     // Real To AC
    m_ptr->pac = 0;                // Real AC
    m_ptr->dis_th = m_ptr->ptohit; // Display To Hit
    m_ptr->dis_td = m_ptr->ptodam; // Display To Dam
    m_ptr->dis_ac = 0;             // Display AC
    m_ptr->dis_tac = m_ptr->ptoac; // Display To AC

    for (int i = INVEN_WIELD; i < INVEN_LIGHT; i++) {
        inven_type *i_ptr = &inventory[i];
        if (i_ptr->tval != TV_NOTHING) {
            m_ptr->ptohit += i_ptr->tohit;

            // Bows can't damage. -CJS-
            if (i_ptr->tval != TV_BOW) {
                m_ptr->ptodam += i_ptr->todam;
            }

            m_ptr->ptoac += i_ptr->toac;
            m_ptr->pac += i_ptr->ac;
            if (known2_p(i_ptr)) {
                m_ptr->dis_th += i_ptr->tohit;
                if (i_ptr->tval != TV_BOW) {
                    // Bows can't damage. -CJS-
                    m_ptr->dis_td += i_ptr->todam;
                }
                m_ptr->dis_tac += i_ptr->toac;
                m_ptr->dis_ac += i_ptr->ac;
            } else if (!(TR_CURSED & i_ptr->flags)) {
                // Base AC values should always be visible,
                // as long as the item is not cursed.
                m_ptr->dis_ac += i_ptr->ac;
            }
        }
    }
    m_ptr->dis_ac += m_ptr->dis_tac;

    if (weapon_heavy) {
        m_ptr->dis_th += (py.stats.use_stat[A_STR] * 15 - inventory[INVEN_WIELD].weight);
    }

    // Add in temporary spell increases
    if (p_ptr->invuln > 0) {
        m_ptr->pac += 100;
        m_ptr->dis_ac += 100;
    }
    if (p_ptr->blessed > 0) {
        m_ptr->pac += 2;
        m_ptr->dis_ac += 2;
    }
    if (p_ptr->detect_inv > 0) {
        p_ptr->see_inv = true;
    }

    // can't print AC here because might be in a store
    if (old_dis_ac != m_ptr->dis_ac) {
        p_ptr->status |= PY_ARMOR;
    }

    inven_type *i_ptr;

    uint32_t item_flags = 0;
    i_ptr = &inventory[INVEN_WIELD];
    for (int i = INVEN_WIELD; i < INVEN_LIGHT; i++) {
        item_flags |= i_ptr->flags;
        i_ptr++;
    }

    if (TR_SLOW_DIGEST & item_flags) {
        p_ptr->slow_digest = true;
    }
    if (TR_AGGRAVATE & item_flags) {
        p_ptr->aggravate = true;
    }
    if (TR_TELEPORT & item_flags) {
        p_ptr->teleport = true;
    }
    if (TR_REGEN & item_flags) {
        p_ptr->regenerate = true;
    }
    if (TR_RES_FIRE & item_flags) {
        p_ptr->fire_resist = true;
    }
    if (TR_RES_ACID & item_flags) {
        p_ptr->acid_resist = true;
    }
    if (TR_RES_COLD & item_flags) {
        p_ptr->cold_resist = true;
    }
    if (TR_FREE_ACT & item_flags) {
        p_ptr->free_act = true;
    }
    if (TR_SEE_INVIS & item_flags) {
        p_ptr->see_inv = true;
    }
    if (TR_RES_LIGHT & item_flags) {
        p_ptr->lght_resist = true;
    }
    if (TR_FFALL & item_flags) {
        p_ptr->ffall = true;
    }

    i_ptr = &inventory[INVEN_WIELD];
    for (int i = INVEN_WIELD; i < INVEN_LIGHT; i++) {
        if (TR_SUST_STAT & i_ptr->flags) {
            switch (i_ptr->p1) {
            case 1:
                p_ptr->sustain_str = true;
                break;
            case 2:
                p_ptr->sustain_int = true;
                break;
            case 3:
                p_ptr->sustain_wis = true;
                break;
            case 4:
                p_ptr->sustain_con = true;
                break;
            case 5:
                p_ptr->sustain_dex = true;
                break;
            case 6:
                p_ptr->sustain_chr = true;
                break;
            default:
                break;
            }
        }
        i_ptr++;
    }

    if (p_ptr->slow_digest) {
        p_ptr->food_digested--;
    }
    if (p_ptr->regenerate) {
        p_ptr->food_digested += 3;
    }
}

// Displays inventory items from r1 to r2 -RAK-
// Designed to keep the display as far to the right as possible. -CJS-
// The parameter col gives a column at which to start, but if the display
// does not fit, it may be moved left.  The return value is the left edge
// used. If mask is non-zero, then only display those items which have a
// non-zero entry in the mask array.
int show_inven(int r1, int r2, bool weight, int col, char *mask) {
    bigvtype tmp_val;
    vtype out_val[23];

    int len = 79 - col;

    int lim;
    if (weight) {
        lim = 68;
    } else {
        lim = 76;
    }

    // Print the items
    for (int i = r1; i <= r2; i++) {
        if (mask == CNIL || mask[i]) {
            objdes(tmp_val, &inventory[i], true);

            // Truncate if too long.
            tmp_val[lim] = 0;

            (void)sprintf(out_val[i], "%c) %s", 'a' + i, tmp_val);
            int l = (int)strlen(out_val[i]) + 2;
            if (weight) {
                l += 9;
            }
            if (l > len) {
                len = l;
            }
        }
    }

    col = 79 - len;
    if (col < 0) {
        col = 0;
    }

    int current_line = 1;
    for (int i = r1; i <= r2; i++) {
        if (mask == CNIL || mask[i]) {
            // don't need first two spaces if in first column
            if (col == 0) {
                prt(out_val[i], current_line, col);
            } else {
                put_buffer("  ", current_line, col);
                prt(out_val[i], current_line, col + 2);
            }
            if (weight) {
                int total_weight = inventory[i].weight * inventory[i].number;
                (void)sprintf(tmp_val, "%3d.%d lb", (total_weight) / 10, (total_weight) % 10);
                prt(tmp_val, current_line, 71);
            }
            current_line++;
        }
    }
    return col;
}

// Return a string describing how a given equipment item is carried. -CJS-
char *describe_use(int i) {
    const char *p;
    switch (i) {
    case INVEN_WIELD:
        p = "wielding";
        break;
    case INVEN_HEAD:
        p = "wearing on your head";
        break;
    case INVEN_NECK:
        p = "wearing around your neck";
        break;
    case INVEN_BODY:
        p = "wearing on your body";
        break;
    case INVEN_ARM:
        p = "wearing on your arm";
        break;
    case INVEN_HANDS:
        p = "wearing on your hands";
        break;
    case INVEN_RIGHT:
        p = "wearing on your right hand";
        break;
    case INVEN_LEFT:
        p = "wearing on your left hand";
        break;
    case INVEN_FEET:
        p = "wearing on your feet";
        break;
    case INVEN_OUTER:
        p = "wearing about your body";
        break;
    case INVEN_LIGHT:
        p = "using to light the way";
        break;
    case INVEN_AUX:
        p = "holding ready by your side";
        break;
    default:
        p = "carrying in your pack";
        break;
    }
    return (char *)p;
}

// Displays equipment items from r1 to end -RAK-
// Keep display as far right as possible. -CJS-
int show_equip(bool weight, int col) {
    bigvtype prt2;
    vtype out_val[INVEN_ARRAY_SIZE - INVEN_WIELD];

    inven_type *i_ptr;

    int line = 0;
    int len = 79 - col;

    int lim;
    if (weight) {
        lim = 52;
    } else {
        lim = 60;
    }

    // Range of equipment
    for (int i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) {
        i_ptr = &inventory[i];
        if (i_ptr->tval != TV_NOTHING) {
            const char *prt1;

            // Get position
            switch (i) {
            case INVEN_WIELD:
                if (py.stats.use_stat[A_STR] * 15 < i_ptr->weight) {
                    prt1 = "Just lifting";
                } else {
                    prt1 = "Wielding";
                }
                break;
            case INVEN_HEAD:
                prt1 = "On head";
                break;
            case INVEN_NECK:
                prt1 = "Around neck";
                break;
            case INVEN_BODY:
                prt1 = "On body";
                break;
            case INVEN_ARM:
                prt1 = "On arm";
                break;
            case INVEN_HANDS:
                prt1 = "On hands";
                break;
            case INVEN_RIGHT:
                prt1 = "On right hand";
                break;
            case INVEN_LEFT:
                prt1 = "On left hand";
                break;
            case INVEN_FEET:
                prt1 = "On feet";
                break;
            case INVEN_OUTER:
                prt1 = "About body";
                break;
            case INVEN_LIGHT:
                prt1 = "Light source";
                break;
            case INVEN_AUX:
                prt1 = "Spare weapon";
                break;
            default:
                prt1 = "Unknown value";
                break;
            }
            objdes(prt2, &inventory[i], true);

            // Truncate if necessary
            prt2[lim] = 0;

            (void)sprintf(out_val[line], "%c) %-14s: %s", line + 'a', prt1, prt2);
            int l = (int)strlen(out_val[line]) + 2;
            if (weight) {
                l += 9;
            }
            if (l > len) {
                len = l;
            }
            line++;
        }
    }
    col = 79 - len;
    if (col < 0) {
        col = 0;
    }

    line = 0;

    // Range of equipment
    for (int i = INVEN_WIELD; i < INVEN_ARRAY_SIZE; i++) {
        i_ptr = &inventory[i];
        if (i_ptr->tval != TV_NOTHING) {
            // don't need first two spaces when using whole screen
            if (col == 0) {
                prt(out_val[line], line + 1, col);
            } else {
                put_buffer("  ", line + 1, col);
                prt(out_val[line], line + 1, col + 2);
            }
            if (weight) {
                int total_weight = i_ptr->weight * i_ptr->number;
                (void)sprintf(prt2, "%3d.%d lb", (total_weight) / 10, (total_weight) % 10);
                prt(prt2, line + 1, 71);
            }
            line++;
        }
    }
    erase_line(line + 1, col);
    return col;
}

// Remove item from equipment list -RAK-
void takeoff(int item_val, int posn) {
    equip_ctr--;

    inven_type *t_ptr = &inventory[item_val];

    inven_weight -= t_ptr->weight * t_ptr->number;
    py.flags.status |= PY_STR_WGT;

    const char *p;
    if (item_val == INVEN_WIELD || item_val == INVEN_AUX) {
        p = "Was wielding ";
    } else if (item_val == INVEN_LIGHT) {
        p = "Light source was ";
    } else {
        p = "Was wearing ";
    }

    bigvtype out_val, prt2;
    objdes(prt2, t_ptr, true);
    if (posn >= 0) {
        (void)sprintf(out_val, "%s%s (%c)", p, prt2, 'a' + posn);
    } else {
        (void)sprintf(out_val, "%s%s", p, prt2);
    }
    msg_print(out_val);

    // For secondary weapon
    if (item_val != INVEN_AUX) {
        py_bonuses(t_ptr, -1);
    }
    invcopy(t_ptr, OBJ_NOTHING);
}

// Used to verify if this really is the item we wish to -CJS-
// wear or read.
int verify(const char *prompt, int item) {
    bigvtype out_str, object;

    objdes(object, &inventory[item], true);

    // change the period to a question mark
    object[strlen(object) - 1] = '?';

    (void)sprintf(out_str, "%s %s", prompt, object);

    return get_check(out_str);
}

// All inventory commands (wear, exchange, take off, drop, inventory and
// equipment) are handled in an alternative command input mode, which accepts
// any of the inventory commands.
//
// It is intended that this function be called several times in succession,
// as some commands take up a turn, and the rest of moria must proceed in the
// interim. A global variable is provided, doing_inven, which is normally
// zero; however if on return from inven_command it is expected that
// inven_command should be called *again*, (being still in inventory command
// input mode), then doing_inven is set to the inventory command character
// which should be used in the next call to inven_command.
//
// On return, the screen is restored, but not flushed. Provided no flush of
// the screen takes place before the next call to inven_command, the inventory
// command screen is silently redisplayed, and no actual output takes place at
// all. If the screen is flushed before a subsequent call, then the player is
// prompted to see if we should continue. This allows the player to see any
// changes that take place on the screen during inventory command input.
//
// The global variable, screen_change, is cleared by inven_command, and set
// when the screen is flushed. This is the means by which inven_command tell
// if the screen has been flushed.
//
// The display of inventory items is kept to the right of the screen to
// minimize the work done to restore the screen afterwards. -CJS-

// Inventory command screen states.
#define BLANK_SCR 0
#define EQUIP_SCR 1
#define INVEN_SCR 2
#define WEAR_SCR 3
#define HELP_SCR 4
#define WRONG_SCR 5

// Keep track of the state of the inventory screen.
static int scr_state, scr_left, scr_base;
static int wear_low, wear_high;

// Draw the inventory screen.
static void inven_screen(int new_scr) {
    int line = 0;

    if (new_scr != scr_state) {
        scr_state = new_scr;
        switch (new_scr) {
        case BLANK_SCR:
            line = 0;
            break;
        case HELP_SCR:
            if (scr_left > 52) {
                scr_left = 52;
            }
            prt("  ESC: exit", 1, scr_left);
            prt("  w  : wear or wield object", 2, scr_left);
            prt("  t  : take off item", 3, scr_left);
            prt("  d  : drop object", 4, scr_left);
            prt("  x  : exchange weapons", 5, scr_left);
            prt("  i  : inventory of pack", 6, scr_left);
            prt("  e  : list used equipment", 7, scr_left);
            line = 7;
            break;
        case INVEN_SCR:
            scr_left = show_inven(0, inven_ctr - 1, show_weight_flag, scr_left, CNIL);
            line = inven_ctr;
            break;
        case WEAR_SCR:
            scr_left = show_inven(wear_low, wear_high, show_weight_flag, scr_left, CNIL);
            line = wear_high - wear_low + 1;
            break;
        case EQUIP_SCR:
            scr_left = show_equip(show_weight_flag, scr_left);
            line = equip_ctr;
            break;
        }
        if (line >= scr_base) {
            scr_base = line + 1;
            erase_line(scr_base, scr_left);
        } else {
            while (++line <= scr_base) {
                erase_line(line, scr_left);
            }
        }
    }
}

// This does all the work.
void inven_command(char command) {
    bigvtype prt1, prt2;
    int item, tmp;
    inven_type tmp_obj;
    int slot = 0;

    free_turn_flag = true;

    save_screen();

    // Take up where we left off after a previous inventory command. -CJS-
    if (doing_inven) {
        // If the screen has been flushed, we need to redraw. If the command
        // is a simple ' ' to recover the screen, just quit. Otherwise, check
        // and see what the user wants.
        if (screen_change) {
            if (command == ' ' ||
                !get_check("Continuing with inventory command?")) {
                doing_inven = 0;
                return;
            }
            scr_left = 50;
            scr_base = 0;
        }
        tmp = scr_state;
        scr_state = WRONG_SCR;
        inven_screen(tmp);
    } else {
        scr_left = 50;
        scr_base = 0;

        // this forces exit of inven_command() if selecting is not set true
        scr_state = BLANK_SCR;
    }

    bool selecting;

    do {
        if (isupper((int)command)) {
            command = (char) tolower((int)command);
        }

        // Simple command getting and screen selection.
        selecting = false;
        switch (command) {
        case 'i': // Inventory
            if (inven_ctr == 0) {
                msg_print("You are not carrying anything.");
            } else {
                inven_screen(INVEN_SCR);
            }
            break;
        case 'e': // Equipment
            if (equip_ctr == 0) {
                msg_print("You are not using any equipment.");
            } else {
                inven_screen(EQUIP_SCR);
            }
            break;
        case 't': // Take off
            if (equip_ctr == 0) {
                msg_print("You are not using any equipment.");
                // don't print message restarting inven command after taking off
                // something, it is confusing
            } else if (inven_ctr >= INVEN_WIELD && !doing_inven) {
                msg_print("You will have to drop something first.");
            } else {
                if (scr_state != BLANK_SCR) {
                    inven_screen(EQUIP_SCR);
                }
                selecting = true;
            }
            break;
        case 'd': // Drop
            if (inven_ctr == 0 && equip_ctr == 0) {
                msg_print("But you're not carrying anything.");
            } else if (cave[char_row][char_col].tptr != 0) {
                msg_print("There's no room to drop anything here.");
            } else {
                selecting = true;
                if ((scr_state == EQUIP_SCR && equip_ctr > 0) ||
                    inven_ctr == 0) {
                    if (scr_state != BLANK_SCR) {
                        inven_screen(EQUIP_SCR);
                    }
                    command = 'r'; // Remove - or take off and drop.
                } else if (scr_state != BLANK_SCR) {
                    inven_screen(INVEN_SCR);
                }
            }
            break;
        case 'w': // Wear/wield
            for (wear_low = 0;
                 wear_low < inven_ctr && inventory[wear_low].tval > TV_MAX_WEAR;
                 wear_low++) {
                ;
            }
            for (wear_high = wear_low; wear_high < inven_ctr &&
                                       inventory[wear_high].tval >= TV_MIN_WEAR;
                 wear_high++) {
                ;
            }
            wear_high--;
            if (wear_low > wear_high) {
                msg_print("You have nothing to wear or wield.");
            } else {
                if (scr_state != BLANK_SCR && scr_state != INVEN_SCR) {
                    inven_screen(WEAR_SCR);
                }
                selecting = true;
            }
            break;
        case 'x':
            if (inventory[INVEN_WIELD].tval == TV_NOTHING &&
                inventory[INVEN_AUX].tval == TV_NOTHING) {
                msg_print("But you are wielding no weapons.");
            } else if (TR_CURSED & inventory[INVEN_WIELD].flags) {
                objdes(prt1, &inventory[INVEN_WIELD], false);
                (void)sprintf(prt2, "The %s you are wielding appears to be cursed.", prt1);
                msg_print(prt2);
            } else {
                free_turn_flag = false;
                tmp_obj = inventory[INVEN_AUX];
                inventory[INVEN_AUX] = inventory[INVEN_WIELD];
                inventory[INVEN_WIELD] = tmp_obj;
                if (scr_state == EQUIP_SCR) {
                    scr_left = show_equip(show_weight_flag, scr_left);
                }

                py_bonuses(&inventory[INVEN_AUX], -1);  // Subtract bonuses
                py_bonuses(&inventory[INVEN_WIELD], 1); // Add bonuses

                if (inventory[INVEN_WIELD].tval != TV_NOTHING) {
                    (void)strcpy(prt1, "Primary weapon   : ");
                    objdes(prt2, &inventory[INVEN_WIELD], true);
                    msg_print(strcat(prt1, prt2));
                } else {
                    msg_print("No primary weapon.");
                }

                // this is a new weapon, so clear the heavy flag
                weapon_heavy = false;
                check_strength();
            }
            break;
        case ' ': // Dummy command to return again to main prompt.
            break;
        case '?':
            inven_screen(HELP_SCR);
            break;
        default:
            // Nonsense command
            bell();
            break;
        }

        // Clear the doing_inven flag here, instead of at beginning, so that
        // can use it to control when messages above appear.
        doing_inven = 0;

        // Keep looking for objects to drop/wear/take off/throw off
        char which = 'z';
        while (selecting && free_turn_flag) {
            int from, to;
            const char *prompt;
            const char *swap = "";

            if (command == 'w') {
                from = wear_low;
                to = wear_high;
                prompt = "Wear/Wield";
            } else {
                from = 0;
                if (command == 'd') {
                    to = inven_ctr - 1;
                    prompt = "Drop";
                    if (equip_ctr > 0) {
                        swap = ", / for Equip";
                    }
                } else {
                    to = equip_ctr - 1;
                    if (command == 't') {
                        prompt = "Take off";
                    } else { // command == 'r'
                        prompt = "Throw off";
                        if (inven_ctr > 0) {
                            swap = ", / for Inven";
                        }
                    }
                }
            }
            if (from > to) {
                selecting = false;
            } else {
                const char *disp;
                if (scr_state == BLANK_SCR) {
                    disp = ", * to list";
                } else {
                    disp = "";
                }
                (void)sprintf(
                    prt1,
                    "(%c-%c%s%s%s, space to break, ESC to exit) %s which one?",
                    from + 'a', to + 'a', disp, swap,
                    ((command == 'w' || command == 'd') ? ", 0-9" : ""),
                    prompt);

                // Abort everything.
                if (!get_com(prt1, &which)) {
                    selecting = false;
                    which = ESCAPE;
                } else if (which == ' ' || which == '*') {
                    // Draw the screen and maybe exit to main prompt.

                    if (command == 't' || command == 'r') {
                        inven_screen(EQUIP_SCR);
                    } else if (command == 'w' && scr_state != INVEN_SCR) {
                        inven_screen(WEAR_SCR);
                    } else {
                        inven_screen(INVEN_SCR);
                    }
                    if (which == ' ') {
                        selecting = false;
                    }
                } else if (which == '/' && swap[0]) {
                    // Swap screens (for drop)

                    if (command == 'd') {
                        command = 'r';
                    } else {
                        command = 'd';
                    }
                    if (scr_state == EQUIP_SCR) {
                        inven_screen(INVEN_SCR);
                    } else if (scr_state == INVEN_SCR) {
                        inven_screen(EQUIP_SCR);
                    }
                } else {
                    if ((which >= '0') && (which <= '9') && (command != 'r') && (command != 't')) {
                        // look for item whose inscription matches "which"
                        int m;
                        for (m = from;
                             m <= to && ((inventory[m].inscrip[0] != which) ||
                                         (inventory[m].inscrip[1] != '\0'));
                             m++) {
                            ;
                        }
                        if (m <= to) {
                            item = m;
                        } else {
                            item = -1;
                        }
                    } else if ((which >= 'A') && (which <= 'Z')) {
                        item = which - 'A';
                    } else {
                        item = which - 'a';
                    }
                    if (item < from || item > to) {
                        bell();
                    } else { // Found an item!
                        if (command == 'r' || command == 't') {
                            // Get its place in the equipment list.
                            tmp = item;
                            item = 21;
                            do {
                                item++;
                                if (inventory[item].tval != TV_NOTHING) {
                                    tmp--;
                                }
                            } while (tmp >= 0);
                            if (isupper((int)which) && !verify((char *)prompt, item)) {
                                item = -1;
                            } else if (TR_CURSED & inventory[item].flags) {
                                msg_print("Hmmm, it seems to be cursed.");
                                item = -1;
                            } else if (command == 't' && !inven_check_num(&inventory[item])) {
                                if (cave[char_row][char_col].tptr != 0) {
                                    msg_print("You can't carry it.");
                                    item = -1;
                                } else if (get_check("You can't carry it.  Drop it?")) {
                                    command = 'r';
                                } else {
                                    item = -1;
                                }
                            }
                            if (item >= 0) {
                                if (command == 'r') {
                                    inven_drop(item, true);
                                    // As a safety measure, set the player's inven
                                    // weight to 0, when the last object is dropped.
                                    if (inven_ctr == 0 && equip_ctr == 0) {
                                        inven_weight = 0;
                                    }
                                } else {
                                    slot = inven_carry(&inventory[item]);
                                    takeoff(item, slot);
                                }
                                check_strength();
                                free_turn_flag = false;
                                if (command == 'r') {
                                    selecting = false;
                                }
                            }
                        } else if (command == 'w') {
                            // Wearing. Go to a bit of trouble over replacing
                            // existing equipment.
                            if (isupper((int)which) && !verify((char *)prompt, item)) {
                                item = -1;
                            } else {
                                // Slot for equipment
                                switch (inventory[item].tval) {
                                case TV_SLING_AMMO:
                                case TV_BOLT:
                                case TV_ARROW:
                                case TV_BOW:
                                case TV_HAFTED:
                                case TV_POLEARM:
                                case TV_SWORD:
                                case TV_DIGGING:
                                case TV_SPIKE:
                                    slot = INVEN_WIELD;
                                    break;
                                case TV_LIGHT:
                                    slot = INVEN_LIGHT;
                                    break;
                                case TV_BOOTS:
                                    slot = INVEN_FEET;
                                    break;
                                case TV_GLOVES:
                                    slot = INVEN_HANDS;
                                    break;
                                case TV_CLOAK:
                                    slot = INVEN_OUTER;
                                    break;
                                case TV_HELM:
                                    slot = INVEN_HEAD;
                                    break;
                                case TV_SHIELD:
                                    slot = INVEN_ARM;
                                    break;
                                case TV_HARD_ARMOR:
                                case TV_SOFT_ARMOR:
                                    slot = INVEN_BODY;
                                    break;
                                case TV_AMULET:
                                    slot = INVEN_NECK;
                                    break;
                                case TV_RING:
                                    if (inventory[INVEN_RIGHT].tval == TV_NOTHING) {
                                        slot = INVEN_RIGHT;
                                    } else if (inventory[INVEN_LEFT].tval == TV_NOTHING) {
                                        slot = INVEN_LEFT;
                                    } else {
                                        slot = 0;
                                        // Rings. Give choice over where they go.
                                        do {
                                            char query;
                                            if (!get_com("Put ring on which hand (l/r/L/R)?", &query)) {
                                                item = -1;
                                                slot = -1;
                                            } else if (query == 'l') {
                                                slot = INVEN_LEFT;
                                            } else if (query == 'r') {
                                                slot = INVEN_RIGHT;
                                            } else {
                                                if (query == 'L') {
                                                    slot = INVEN_LEFT;
                                                } else if (query == 'R') {
                                                    slot = INVEN_RIGHT;
                                                } else {
                                                    bell();
                                                }
                                                if (slot &&
                                                    !verify("Replace", slot)) {
                                                    slot = 0;
                                                }
                                            }
                                        } while (slot == 0);
                                    }
                                    break;
                                default:
                                    msg_print("IMPOSSIBLE: I don't see how you can use that.");
                                    item = -1;
                                    break;
                                }
                            }
                            if (item >= 0 && inventory[slot].tval != TV_NOTHING) {
                                if (TR_CURSED & inventory[slot].flags) {
                                    objdes(prt1, &inventory[slot], false);
                                    (void)sprintf(prt2, "The %s you are ", prt1);
                                    if (slot == INVEN_HEAD) {
                                        (void)strcat(prt2, "wielding ");
                                    } else {
                                        (void)strcat(prt2, "wearing ");
                                    }
                                    msg_print(strcat(prt2, "appears to be cursed."));
                                    item = -1;
                                } else if (inventory[item].subval == ITEM_GROUP_MIN &&
                                           inventory[item].number > 1 &&
                                           !inven_check_num(&inventory[slot])) {
                                    // this can happen if try to wield a torch,
                                    // and have more than one in inventory
                                    msg_print("You will have to drop something first.");
                                    item = -1;
                                }
                            }
                            if (item >= 0) {
                                // OK. Wear it.
                                free_turn_flag = false;

                                // first remove new item from inventory
                                tmp_obj = inventory[item];
                                inven_type *i_ptr = &tmp_obj;

                                wear_high--;

                                // Fix for torches
                                if (i_ptr->number > 1 &&
                                    i_ptr->subval <= ITEM_SINGLE_STACK_MAX) {
                                    i_ptr->number = 1;
                                    wear_high++;
                                }
                                inven_weight += i_ptr->weight * i_ptr->number;

                                // Subtracts weight
                                inven_destroy(item);

                                // Second, add old item to inv and remove
                                // from equipment list, if necessary.
                                i_ptr = &inventory[slot];
                                if (i_ptr->tval != TV_NOTHING) {
                                    int tmp2 = inven_ctr;
                                    tmp = inven_carry(i_ptr);
                                    // If item removed did not stack with anything
                                    // in inventory, then increment wear_high.
                                    if (inven_ctr != tmp2) {
                                        wear_high++;
                                    }
                                    takeoff(slot, tmp);
                                }

                                // third, wear new item
                                *i_ptr = tmp_obj;
                                equip_ctr++;
                                py_bonuses(i_ptr, 1);

                                const char *string;
                                if (slot == INVEN_WIELD) {
                                    string = "You are wielding";
                                } else if (slot == INVEN_LIGHT) {
                                    string = "Your light source is";
                                } else {
                                    string = "You are wearing";
                                }
                                objdes(prt2, i_ptr, true);
                                // Get the right equipment letter.
                                tmp = INVEN_WIELD;
                                item = 0;
                                while (tmp != slot) {
                                    if (inventory[tmp++].tval != TV_NOTHING) {
                                        item++;
                                    }
                                }

                                (void)sprintf(prt1, "%s %s (%c)", string, prt2, 'a' + item);
                                msg_print(prt1);
                                // this is a new weapon, so clear heavy flag
                                if (slot == INVEN_WIELD) {
                                    weapon_heavy = false;
                                }
                                check_strength();
                                if (i_ptr->flags & TR_CURSED) {
                                    msg_print("Oops! It feels deathly cold!");
                                    add_inscribe(i_ptr, ID_DAMD);
                                    // To force a cost of 0, even if unidentified.
                                    i_ptr->cost = -1;
                                }
                            }
                        } else {
                            // command == 'd'

                            // NOTE: initializing to `ESCAPE` as warnings were being given. -MRC-
                            char query = ESCAPE;

                            if (inventory[item].number > 1) {
                                objdes(prt1, &inventory[item], true);
                                prt1[strlen(prt1) - 1] = '?';
                                (void)sprintf(prt2, "Drop all %s [y/n]", prt1);
                                prt1[strlen(prt1) - 1] = '.';
                                prt(prt2, 0, 0);
                                query = inkey();
                                if (query != 'y' && query != 'n') {
                                    if (query != ESCAPE) {
                                        bell();
                                    }
                                    erase_line(MSG_LINE, 0);
                                    item = -1;
                                }
                            } else if (isupper((int)which) && !verify((char *)prompt, item)) {
                                item = -1;
                            } else {
                                query = 'y';
                            }

                            if (item >= 0) {
                                free_turn_flag = false; // Player turn
                                inven_drop(item, query == 'y');
                                check_strength();
                            }

                            selecting = false;

                            // As a safety measure, set the player's inven weight
                            // to 0, when the last object is dropped.
                            if (inven_ctr == 0 && equip_ctr == 0) {
                                inven_weight = 0;
                            }
                        }
                        if (!free_turn_flag && scr_state == BLANK_SCR) {
                            selecting = false;
                        }
                    }
                }
            }
        }
        if (which == ESCAPE || scr_state == BLANK_SCR) {
            command = ESCAPE;
        } else if (!free_turn_flag) {
            // Save state for recovery if they want to call us again next turn.
            if (selecting) {
                doing_inven = command;
            } else {
                doing_inven = ' '; // A dummy command to recover screen.
                // flush last message before clearing screen_change and exiting
            }
            msg_print(CNIL);
            screen_change = false; // This lets us know if the world changes
            command = ESCAPE;
        } else {
            // Put an appropriate header.
            if (scr_state == INVEN_SCR) {
                if (!show_weight_flag || inven_ctr == 0) {
                    (void)sprintf(prt1, "You are carrying %d.%d pounds. In your pack there is %s",
                                  inven_weight / 10, inven_weight % 10,
                                  (inven_ctr == 0 ? "nothing." : "-"));
                } else {
                    (void)sprintf(prt1, "You are carrying %d.%d pounds. Your capacity is %d.%d pounds. %s",
                                  inven_weight / 10, inven_weight % 10,
                                  weight_limit() / 10, weight_limit() % 10,
                                  "In your pack is -");
                }
                prt(prt1, 0, 0);
            } else if (scr_state == WEAR_SCR) {
                if (wear_high < wear_low) {
                    prt("You have nothing you could wield.", 0, 0);
                } else {
                    prt("You could wield -", 0, 0);
                }
            } else if (scr_state == EQUIP_SCR) {
                if (equip_ctr == 0) {
                    prt("You are not using anything.", 0, 0);
                } else {
                    prt("You are using -", 0, 0);
                }
            } else {
                prt("Allowed commands:", 0, 0);
            }

            erase_line(scr_base, scr_left);
            put_buffer("e/i/t/w/x/d/?/ESC:", scr_base, 60);
            command = inkey();
            erase_line(scr_base, scr_left);
        }
    } while (command != ESCAPE);

    if (scr_state != BLANK_SCR) {
        restore_screen();
    }

    calc_bonuses();
}

// Get the ID of an item and return the CTR value of it -RAK-
int get_item(int *com_val, const char *pmt, int i, int j, char *mask, const char *message) {
    bool test_flag;
    bool full;
    bool item = false;
    bool redraw = false;
    int i_scr = 1;
    *com_val = 0;

    if (j > INVEN_WIELD) {
        full = true;
        if (inven_ctr == 0) {
            i_scr = 0;
            j = equip_ctr - 1;
        } else {
            j = inven_ctr - 1;
        }
    } else {
        full = false;
    }

    if (inven_ctr > 0 || (full && equip_ctr > 0)) {
        do {
            if (redraw) {
                if (i_scr > 0) {
                    (void)show_inven(i, j, false, 80, mask);
                } else {
                    (void)show_equip(false, 80);
                }
            }

            vtype out_val;

            if (full) {
                (void)sprintf(out_val, "(%s: %c-%c,%s%s / for %s, or ESC) %s",
                              (i_scr > 0 ? "Inven" : "Equip"), i + 'a', j + 'a',
                              (i_scr > 0 ? " 0-9," : ""),
                              (redraw ? "" : " * to see,"),
                              (i_scr > 0 ? "Equip" : "Inven"), pmt);
            } else {
                (void)sprintf(out_val, "(Items %c-%c,%s%s ESC to exit) %s",
                              i + 'a', j + 'a', (i_scr > 0 ? " 0-9," : ""),
                              (redraw ? "" : " * for inventory list,"), pmt);
            }

            test_flag = false;
            prt(out_val, 0, 0);

            do {
                char which = inkey();
                switch (which) {
                case ESCAPE:
                    test_flag = true;
                    free_turn_flag = true;
                    i_scr = -1;
                    break;
                case '/':
                    if (full) {
                        if (i_scr > 0) {
                            if (equip_ctr == 0) {
                                prt("But you're not using anything -more-", 0, 0);
                                (void)inkey();
                            } else {
                                i_scr = 0;
                                test_flag = true;
                                if (redraw) {
                                    j = equip_ctr;
                                    while (j < inven_ctr) {
                                        j++;
                                        erase_line(j, 0);
                                    }
                                }
                                j = equip_ctr - 1;
                            }
                            prt(out_val, 0, 0);
                        } else {
                            if (inven_ctr == 0) {
                                prt("But you're not carrying anything -more-", 0, 0);
                                (void)inkey();
                            } else {
                                i_scr = 1;
                                test_flag = true;
                                if (redraw) {
                                    j = inven_ctr;
                                    while (j < equip_ctr) {
                                        j++;
                                        erase_line(j, 0);
                                    }
                                }
                                j = inven_ctr - 1;
                            }
                        }
                    }
                    break;
                case '*':
                    if (!redraw) {
                        test_flag = true;
                        save_screen();
                        redraw = true;
                    }
                    break;
                default:
                    // look for item whose inscription matches "which"
                    if ((which >= '0') && (which <= '9') && (i_scr != 0)) {
                        int m;
                        for (m = i; (m < INVEN_WIELD) && ((inventory[m].inscrip[0] != which) || (inventory[m].inscrip[1] != '\0')); m++) {
                            ;
                        }
                        if (m < INVEN_WIELD) {
                            *com_val = m;
                        } else {
                            *com_val = -1;
                        }
                    } else if (isupper((int)which)) {
                        *com_val = which - 'A';
                    } else {
                        *com_val = which - 'a';
                    }

                    if ((*com_val >= i) && (*com_val <= j) &&
                        (mask == CNIL || mask[*com_val])) {
                        if (i_scr == 0) {
                            i = 21;
                            j = *com_val;
                            do {
                                while (inventory[++i].tval == TV_NOTHING) {
                                    ;
                                }
                                j--;
                            } while (j >= 0);
                            *com_val = i;
                        }
                        if (isupper((int)which) && !verify("Try", *com_val)) {
                            test_flag = true;
                            free_turn_flag = true;
                            i_scr = -1;
                            break;
                        }
                        test_flag = true;
                        item = true;
                        i_scr = -1;
                    } else if (message) {
                        msg_print(message);

                        // Set test_flag to force redraw of the question.
                        test_flag = true;
                    } else {
                        bell();
                    }
                    break;
                }
            } while (!test_flag);
        } while (i_scr >= 0);

        if (redraw) {
            restore_screen();
        }

        erase_line(MSG_LINE, 0);
    } else {
        prt("You are not carrying anything.", 0, 0);
    }

    return item;
}

// I may have written the town level code, but I'm not exactly
// proud of it.   Adding the stores required some real slucky
// hooks which I have not had time to re-think. -RAK-

// Returns true if player has no light -RAK-
bool no_light() {
    cave_type *c_ptr = &cave[char_row][char_col];

    return (!c_ptr->tl && !c_ptr->pl);
}

// map rogue_like direction commands into numbers
static char map_roguedir(char comval) {
    switch (comval) {
    case 'h':
        comval = '4';
        break;
    case 'y':
        comval = '7';
        break;
    case 'k':
        comval = '8';
        break;
    case 'u':
        comval = '9';
        break;
    case 'l':
        comval = '6';
        break;
    case 'n':
        comval = '3';
        break;
    case 'j':
        comval = '2';
        break;
    case 'b':
        comval = '1';
        break;
    case '.':
        comval = '5';
        break;
    }
    return comval;
}

// Prompts for a direction -RAK-
// Direction memory added, for repeated commands.  -CJS
bool get_dir(char *prompt, int *dir) {
    static char prev_dir; // Direction memory. -CJS-

    // used in counted commands. -CJS-
    if (default_dir) {
        *dir = prev_dir;
        return true;
    }

    if (prompt == CNIL) {
        prompt = (char *)"Which direction?";
    }

    for (;;) {
        char command;

        // Don't end a counted command. -CJS-
        int save = command_count;

        if (!get_com(prompt, &command)) {
            free_turn_flag = true;
            return false;
        }

        command_count = save;

        if (rogue_like_commands) {
            command = map_roguedir(command);
        }

        if (command >= '1' && command <= '9' && command != '5') {
            prev_dir = command - '0';
            *dir = prev_dir;
            return true;
        }
        bell();
    }
}

// Similar to get_dir, except that no memory exists, and it is -CJS-
// allowed to enter the null direction.
bool get_alldir(const char *prompt, int *dir) {
    char command;

    for (;;) {
        if (!get_com(prompt, &command)) {
            free_turn_flag = true;
            return false;
        }

        if (rogue_like_commands) {
            command = map_roguedir(command);
        }

        if (command >= '1' && command <= '9') {
            *dir = command - '0';
            return true;
        }

        bell();
    }
}

// Moves creature record from one space to another -RAK-
void move_rec(int y1, int x1, int y2, int x2) {
    // this always works correctly, even if y1==y2 and x1==x2
    int tmp = cave[y1][x1].cptr;
    cave[y1][x1].cptr = 0;
    cave[y2][x2].cptr = (uint8_t) tmp;
}

// Room is lit, make it appear -RAK-
void light_room(int y, int x) {
    int tmp1 = (SCREEN_HEIGHT / 2);
    int tmp2 = (SCREEN_WIDTH / 2);

    int start_row = (y / tmp1) * tmp1;
    int start_col = (x / tmp2) * tmp2;

    int end_row = start_row + tmp1 - 1;
    int end_col = start_col + tmp2 - 1;

    for (int i = start_row; i <= end_row; i++) {
        for (int j = start_col; j <= end_col; j++) {
            cave_type *c_ptr = &cave[i][j];

            if (c_ptr->lr && !c_ptr->pl) {
                c_ptr->pl = true;
                if (c_ptr->fval == DARK_FLOOR) {
                    c_ptr->fval = LIGHT_FLOOR;
                }
                if (!c_ptr->fm && c_ptr->tptr != 0) {
                    int tval = t_list[c_ptr->tptr].tval;
                    if (tval >= TV_MIN_VISIBLE && tval <= TV_MAX_VISIBLE) {
                        c_ptr->fm = true;
                    }
                }
                print(loc_symbol(i, j), i, j);
            }
        }
    }
}

// Lights up given location -RAK-
void lite_spot(int y, int x) {
    if (panel_contains(y, x)) {
        print(loc_symbol(y, x), y, x);
    }
}

// Normal movement
// When FIND_FLAG,  light only permanent features
static void sub1_move_light(int y1, int x1, int y2, int x2) {
    if (light_flag) {
        // Turn off lamp light
        for (int i = y1 - 1; i <= y1 + 1; i++) {
            for (int j = x1 - 1; j <= x1 + 1; j++) {
                cave[i][j].tl = false;
            }
        }
        if (find_flag && !find_prself) {
            light_flag = false;
        }
    } else if (!find_flag || find_prself) {
        light_flag = true;
    }

    for (int i = y2 - 1; i <= y2 + 1; i++) {
        for (int j = x2 - 1; j <= x2 + 1; j++) {
            cave_type *c_ptr = &cave[i][j];

            // only light up if normal movement
            if (light_flag) {
                c_ptr->tl = true;
            }
            if (c_ptr->fval >= MIN_CAVE_WALL) {
                c_ptr->pl = true;
            } else if (!c_ptr->fm && c_ptr->tptr != 0) {
                int tval = t_list[c_ptr->tptr].tval;
                if ((tval >= TV_MIN_VISIBLE) && (tval <= TV_MAX_VISIBLE)) {
                    c_ptr->fm = true;
                }
            }
        }
    }

    int top, left, bottom, right;

    // From uppermost to bottom most lines player was on.
    if (y1 < y2) {
        top = y1 - 1;
        bottom = y2 + 1;
    } else {
        top = y2 - 1;
        bottom = y1 + 1;
    }
    if (x1 < x2) {
        left = x1 - 1;
        right = x2 + 1;
    } else {
        left = x2 - 1;
        right = x1 + 1;
    }
    for (int i = top; i <= bottom; i++) {
        // Leftmost to rightmost do
        for (int j = left; j <= right; j++) {
            print(loc_symbol(i, j), i, j);
        }
    }
}

// When blinded,  move only the player symbol.
// With no light,  movement becomes involved.
static void sub3_move_light(int y1, int x1, int y2, int x2) {
    if (light_flag) {
        for (int i = y1 - 1; i <= y1 + 1; i++) {
            for (int j = x1 - 1; j <= x1 + 1; j++) {
                cave[i][j].tl = false;
                print(loc_symbol(i, j), i, j);
            }
        }
        light_flag = false;
    } else if (!find_flag || find_prself) {
        print(loc_symbol(y1, x1), y1, x1);
    }

    if (!find_flag || find_prself) {
        print('@', y2, x2);
    }
}

// Package for moving the character's light about the screen
// Four cases : Normal, Finding, Blind, and No light -RAK-
void move_light(int y1, int x1, int y2, int x2) {
    if (py.flags.blind > 0 || !player_light) {
        sub3_move_light(y1, x1, y2, x2);
    } else {
        sub1_move_light(y1, x1, y2, x2);
    }
}

// Something happens to disturb the player. -CJS-
// The first arg indicates a major disturbance, which affects search.
// The second arg indicates a light change.
void disturb(int s, int l) {
    command_count = 0;
    if (s && (py.flags.status & PY_SEARCH)) {
        search_off();
    }
    if (py.flags.rest != 0) {
        rest_off();
    }
    if (l || find_flag) {
        find_flag = 0;
        check_view();
    }
    flush();
}

// Search Mode enhancement -RAK-
void search_on() {
    change_speed(1);
    py.flags.status |= PY_SEARCH;
    prt_state();
    prt_speed();
    py.flags.food_digested++;
}

void search_off() {
    check_view();
    change_speed(-1);

    py.flags.status &= ~PY_SEARCH;

    prt_state();
    prt_speed();
    py.flags.food_digested--;
}

// Resting allows a player to safely restore his hp -RAK-
void rest() {
    int rest_num;

    if (command_count > 0) {
        rest_num = command_count;
        command_count = 0;
    } else {
        prt("Rest for how long? ", 0, 0);
        rest_num = 0;

        vtype rest_str;
        if (get_string(rest_str, 0, 19, 5)) {
            if (rest_str[0] == '*') {
                rest_num = -MAX_SHORT;
            } else {
                rest_num = atoi(rest_str);
            }
        }
    }
    // check for reasonable value, must be positive number
    // in range of a short, or must be -MAX_SHORT
    if ((rest_num == -MAX_SHORT) || ((rest_num > 0) && (rest_num < MAX_SHORT))) {
        if (py.flags.status & PY_SEARCH) {
            search_off();
        }
        py.flags.rest = (int16_t) rest_num;
        py.flags.status |= PY_REST;
        prt_state();
        py.flags.food_digested--;
        prt("Press any key to stop resting...", 0, 0);
        put_qio();
    } else {
        if (rest_num != 0) {
            msg_print("Invalid rest count.");
        }
        erase_line(MSG_LINE, 0);
        free_turn_flag = true;
    }
}

void rest_off() {
    py.flags.rest = 0;
    py.flags.status &= ~PY_REST;

    prt_state();

    // flush last message, or delete "press any key" message
    msg_print(CNIL);

    py.flags.food_digested++;
}

// Attacker's level and plusses,  defender's AC -RAK-
bool test_hit(int bth, int level, int pth, int ac, int attack_type) {
    disturb(1, 0);

    // pth could be less than 0 if player wielding weapon too heavy for him
    int i = bth + pth * BTH_PLUS_ADJ + (level * class_level_adj[py.misc.pclass][attack_type]);

    // always miss 1 out of 20, always hit 1 out of 20
    int die = randint(20);

    // normal hit
    return (die != 1 && (die == 20 || (i > 0 && randint(i) > ac)));
}

// Decreases players hit points and sets death flag if necessary -RAK-
void take_hit(int damage, const char *hit_from) {
    if (py.flags.invuln > 0) {
        damage = 0;
    }
    py.misc.chp -= damage;
    if (py.misc.chp < 0) {
        if (!death) {
            death = true;
            (void)strcpy(died_from, hit_from);
            total_winner = false;
        }
        new_level_flag = true;
    } else {
        prt_chp();
    }
}
