// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Scroll code

#include "headers.h"
#include "externs.h"

// Scrolls for the reading -RAK-
void read_scroll() {
    bool flag;
    int j, k, l, y, x;
    int item_val;
    int tmp[6];

    free_turn_flag = true;

    if (py.flags.blind > 0) {
        msg_print("You can't see to read the scroll.");
    } else if (no_light()) {
        msg_print("You have no light to read by.");
    } else if (py.flags.confused > 0) {
        msg_print("You are too confused to read a scroll.");
    } else if (inven_ctr == 0) {
        msg_print("You are not carrying anything!");
    } else if (!find_range(TV_SCROLL1, TV_SCROLL2, &j, &k)) {
        msg_print("You are not carrying any scrolls!");
    } else if (get_item(&item_val, "Read which scroll?", j, k, CNIL, CNIL)) {
        free_turn_flag = false;

        inven_type *i_ptr = &inventory[item_val];
        uint32_t i = i_ptr->flags;

        bool used_up = true;
        bool ident = false;

        bigvtype out_val, tmp_str;

        while (i != 0) {
            j = bit_pos(&i) + 1;
            if (i_ptr->tval == TV_SCROLL2) {
                j += 32;
            }

            // Scrolls.
            switch (j) {
            case 1:
                i_ptr = &inventory[INVEN_WIELD];
                if (i_ptr->tval != TV_NOTHING) {
                    objdes(tmp_str, i_ptr, false);
                    (void)sprintf(out_val, "Your %s glows faintly!", tmp_str);
                    msg_print(out_val);
                    if (enchant(&i_ptr->tohit, 10)) {
                        i_ptr->flags &= ~TR_CURSED;
                        calc_bonuses();
                    } else {
                        msg_print("The enchantment fails.");
                    }
                    ident = true;
                }
                break;
            case 2:
                i_ptr = &inventory[INVEN_WIELD];
                if (i_ptr->tval != TV_NOTHING) {
                    objdes(tmp_str, i_ptr, false);
                    (void)sprintf(out_val, "Your %s glows faintly!", tmp_str);
                    msg_print(out_val);
                    if ((i_ptr->tval >= TV_HAFTED) &&
                        (i_ptr->tval <= TV_DIGGING)) {
                        j = i_ptr->damage[0] * i_ptr->damage[1];
                    } else {
                        // Bows' and arrows' enchantments should not be
                        // limited by their low base damages
                        j = 10;
                    }
                    if (enchant(&i_ptr->todam, (int16_t) j)) {
                        i_ptr->flags &= ~TR_CURSED;
                        calc_bonuses();
                    } else {
                        msg_print("The enchantment fails.");
                    }
                    ident = true;
                }
                break;
            case 3:
                k = 0;
                l = 0;
                if (inventory[INVEN_BODY].tval != TV_NOTHING) {
                    tmp[k++] = INVEN_BODY;
                }
                if (inventory[INVEN_ARM].tval != TV_NOTHING) {
                    tmp[k++] = INVEN_ARM;
                }
                if (inventory[INVEN_OUTER].tval != TV_NOTHING) {
                    tmp[k++] = INVEN_OUTER;
                }
                if (inventory[INVEN_HANDS].tval != TV_NOTHING) {
                    tmp[k++] = INVEN_HANDS;
                }
                if (inventory[INVEN_HEAD].tval != TV_NOTHING) {
                    tmp[k++] = INVEN_HEAD;
                }
                // also enchant boots
                if (inventory[INVEN_FEET].tval != TV_NOTHING) {
                    tmp[k++] = INVEN_FEET;
                }

                if (k > 0) {
                    l = tmp[randint(k) - 1];
                }

                if (TR_CURSED & inventory[INVEN_BODY].flags) {
                    l = INVEN_BODY;
                } else if (TR_CURSED & inventory[INVEN_ARM].flags) {
                    l = INVEN_ARM;
                } else if (TR_CURSED & inventory[INVEN_OUTER].flags) {
                    l = INVEN_OUTER;
                } else if (TR_CURSED & inventory[INVEN_HEAD].flags) {
                    l = INVEN_HEAD;
                } else if (TR_CURSED & inventory[INVEN_HANDS].flags) {
                    l = INVEN_HANDS;
                } else if (TR_CURSED & inventory[INVEN_FEET].flags) {
                    l = INVEN_FEET;
                }

                if (l > 0) {
                    i_ptr = &inventory[l];
                    objdes(tmp_str, i_ptr, false);
                    (void)sprintf(out_val, "Your %s glows faintly!", tmp_str);
                    msg_print(out_val);
                    if (enchant(&i_ptr->toac, 10)) {
                        i_ptr->flags &= ~TR_CURSED;
                        calc_bonuses();
                    } else {
                        msg_print("The enchantment fails.");
                    }
                    ident = true;
                }
                break;
            case 4:
                msg_print("This is an identify scroll.");
                ident = true;
                used_up = ident_spell();

                // The identify may merge objects, causing the identify scroll
                // to move to a different place.  Check for that here.  It can
                // move arbitrarily far if an identify scroll was used on
                // another identify scroll, but it always moves down.
                while (i_ptr->tval != TV_SCROLL1 || i_ptr->flags != 0x00000008) {
                    item_val--;
                    i_ptr = &inventory[item_val];
                }
                break;
            case 5:
                if (remove_curse()) {
                    msg_print("You feel as if someone is watching over you.");
                    ident = true;
                }
                break;
            case 6:
                ident = light_area(char_row, char_col);
                break;
            case 7:
                for (k = 0; k < randint(3); k++) {
                    y = char_row;
                    x = char_col;
                    ident |= summon_monster(&y, &x, false);
                }
                break;
            case 8:
                teleport(10);
                ident = true;
                break;
            case 9:
                teleport(100);
                ident = true;
                break;
            case 10:
                dun_level += (-3) + 2 * randint(2);
                if (dun_level < 1) {
                    dun_level = 1;
                }
                new_level_flag = true;
                ident = true;
                break;
            case 11:
                if (!py.flags.confuse_monster) {
                    msg_print("Your hands begin to glow.");
                    py.flags.confuse_monster = true;
                    ident = true;
                }
                break;
            case 12:
                ident = true;
                map_area();
                break;
            case 13:
                ident = sleep_monsters1(char_row, char_col);
                break;
            case 14:
                ident = true;
                warding_glyph();
                break;
            case 15:
                ident = detect_treasure();
                break;
            case 16:
                ident = detect_object();
                break;
            case 17:
                ident = detect_trap();
                break;
            case 18:
                ident = detect_sdoor();
                break;
            case 19:
                msg_print("This is a mass genocide scroll.");
                (void)mass_genocide();
                ident = true;
                break;
            case 20:
                ident = detect_invisible();
                break;
            case 21:
                msg_print("There is a high pitched humming noise.");
                (void)aggravate_monster(20);
                ident = true;
                break;
            case 22:
                ident = trap_creation();
                break;
            case 23:
                ident = td_destroy();
                break;
            case 24:
                ident = door_creation();
                break;
            case 25:
                msg_print("This is a Recharge-Item scroll.");
                ident = true;
                used_up = recharge(60);
                break;
            case 26:
                msg_print("This is a genocide scroll.");
                (void)genocide();
                ident = true;
                break;
            case 27:
                ident = unlight_area(char_row, char_col);
                break;
            case 28:
                ident = protect_evil();
                break;
            case 29:
                ident = true;
                create_food();
                break;
            case 30:
                ident = dispel_creature(CD_UNDEAD, 60);
                break;
            case 33:
                i_ptr = &inventory[INVEN_WIELD];
                if (i_ptr->tval != TV_NOTHING) {
                    objdes(tmp_str, i_ptr, false);
                    (void)sprintf(out_val, "Your %s glows brightly!", tmp_str);
                    msg_print(out_val);
                    flag = false;
                    for (k = 0; k < randint(2); k++) {
                        if (enchant(&i_ptr->tohit, 10)) {
                            flag = true;
                        }
                    }
                    if ((i_ptr->tval >= TV_HAFTED) && (i_ptr->tval <= TV_DIGGING)) {
                        j = i_ptr->damage[0] * i_ptr->damage[1];
                    } else {
                        // Bows' and arrows' enchantments should not be limited
                        // by their low base damages
                        j = 10;
                    }
                    for (k = 0; k < randint(2); k++) {
                        if (enchant(&i_ptr->todam, (int16_t) j)) {
                            flag = true;
                        }
                    }
                    if (flag) {
                        i_ptr->flags &= ~TR_CURSED;
                        calc_bonuses();
                    } else {
                        msg_print("The enchantment fails.");
                    }
                    ident = true;
                }
                break;
            case 34:
                i_ptr = &inventory[INVEN_WIELD];
                if (i_ptr->tval != TV_NOTHING) {
                    objdes(tmp_str, i_ptr, false);
                    (void)sprintf(out_val, "Your %s glows black, fades.", tmp_str);
                    msg_print(out_val);
                    unmagic_name(i_ptr);
                    i_ptr->tohit = (int16_t) (-randint(5) - randint(5));
                    i_ptr->todam = (int16_t) (-randint(5) - randint(5));
                    i_ptr->toac = 0;
                    // Must call py_bonuses() before set (clear) flags, and
                    // must call calc_bonuses() after set (clear) flags, so that
                    // all attributes will be properly turned off.
                    py_bonuses(i_ptr, -1);
                    i_ptr->flags = TR_CURSED;
                    calc_bonuses();
                    ident = true;
                }
                break;
            case 35:
                k = 0;
                l = 0;
                if (inventory[INVEN_BODY].tval != TV_NOTHING) {
                    tmp[k++] = INVEN_BODY;
                }
                if (inventory[INVEN_ARM].tval != TV_NOTHING) {
                    tmp[k++] = INVEN_ARM;
                }
                if (inventory[INVEN_OUTER].tval != TV_NOTHING) {
                    tmp[k++] = INVEN_OUTER;
                }
                if (inventory[INVEN_HANDS].tval != TV_NOTHING) {
                    tmp[k++] = INVEN_HANDS;
                }
                if (inventory[INVEN_HEAD].tval != TV_NOTHING) {
                    tmp[k++] = INVEN_HEAD;
                }
                // also enchant boots
                if (inventory[INVEN_FEET].tval != TV_NOTHING) {
                    tmp[k++] = INVEN_FEET;
                }

                if (k > 0) {
                    l = tmp[randint(k) - 1];
                }

                if (TR_CURSED & inventory[INVEN_BODY].flags) {
                    l = INVEN_BODY;
                } else if (TR_CURSED & inventory[INVEN_ARM].flags) {
                    l = INVEN_ARM;
                } else if (TR_CURSED & inventory[INVEN_OUTER].flags) {
                    l = INVEN_OUTER;
                } else if (TR_CURSED & inventory[INVEN_HEAD].flags) {
                    l = INVEN_HEAD;
                } else if (TR_CURSED & inventory[INVEN_HANDS].flags) {
                    l = INVEN_HANDS;
                } else if (TR_CURSED & inventory[INVEN_FEET].flags) {
                    l = INVEN_FEET;
                }

                if (l > 0) {
                    i_ptr = &inventory[l];
                    objdes(tmp_str, i_ptr, false);
                    (void)sprintf(out_val, "Your %s glows brightly!", tmp_str);
                    msg_print(out_val);
                    flag = false;
                    for (k = 0; k < randint(2) + 1; k++) {
                        if (enchant(&i_ptr->toac, 10)) {
                            flag = true;
                        }
                    }
                    if (flag) {
                        i_ptr->flags &= ~TR_CURSED;
                        calc_bonuses();
                    } else {
                        msg_print("The enchantment fails.");
                    }
                    ident = true;
                }
                break;
            case 36:
                if ((inventory[INVEN_BODY].tval != TV_NOTHING) && (randint(4) == 1)) {
                    k = INVEN_BODY;
                } else if ((inventory[INVEN_ARM].tval != TV_NOTHING) && (randint(3) == 1)) {
                    k = INVEN_ARM;
                } else if ((inventory[INVEN_OUTER].tval != TV_NOTHING) && (randint(3) == 1)) {
                    k = INVEN_OUTER;
                } else if ((inventory[INVEN_HEAD].tval != TV_NOTHING) && (randint(3) == 1)) {
                    k = INVEN_HEAD;
                } else if ((inventory[INVEN_HANDS].tval != TV_NOTHING) && (randint(3) == 1)) {
                    k = INVEN_HANDS;
                } else if ((inventory[INVEN_FEET].tval != TV_NOTHING) && (randint(3) == 1)) {
                    k = INVEN_FEET;
                } else if (inventory[INVEN_BODY].tval != TV_NOTHING) {
                    k = INVEN_BODY;
                } else if (inventory[INVEN_ARM].tval != TV_NOTHING) {
                    k = INVEN_ARM;
                } else if (inventory[INVEN_OUTER].tval != TV_NOTHING) {
                    k = INVEN_OUTER;
                } else if (inventory[INVEN_HEAD].tval != TV_NOTHING) {
                    k = INVEN_HEAD;
                } else if (inventory[INVEN_HANDS].tval != TV_NOTHING) {
                    k = INVEN_HANDS;
                } else if (inventory[INVEN_FEET].tval != TV_NOTHING) {
                    k = INVEN_FEET;
                } else {
                    k = 0;
                }

                if (k > 0) {
                    i_ptr = &inventory[k];
                    objdes(tmp_str, i_ptr, false);
                    (void)sprintf(out_val, "Your %s glows black, fades.", tmp_str);
                    msg_print(out_val);
                    unmagic_name(i_ptr);
                    i_ptr->flags = TR_CURSED;
                    i_ptr->tohit = 0;
                    i_ptr->todam = 0;
                    i_ptr->toac = (int16_t) (-randint(5) - randint(5));
                    calc_bonuses();
                    ident = true;
                }
                break;
            case 37:
                ident = false;
                for (k = 0; k < randint(3); k++) {
                    y = char_row;
                    x = char_col;
                    ident |= summon_undead(&y, &x);
                }
                break;
            case 38:
                ident = true;
                bless(randint(12) + 6);
                break;
            case 39:
                ident = true;
                bless(randint(24) + 12);
                break;
            case 40:
                ident = true;
                bless(randint(48) + 24);
                break;
            case 41:
                ident = true;
                if (py.flags.word_recall == 0) {
                    py.flags.word_recall = (int16_t) (25 + randint(30));
                }
                msg_print("The air about you becomes charged.");
                break;
            case 42:
                destroy_area(char_row, char_col);
                ident = true;
                break;
            default:
                msg_print("Internal error in scroll()");
                break;
            }
            // End of Scrolls.
        }

        i_ptr = &inventory[item_val];

        if (ident) {
            if (!known1_p(i_ptr)) {
                struct player_type::misc *m_ptr = &py.misc;

                // round half-way case up
                m_ptr->exp += (i_ptr->level + (m_ptr->lev >> 1)) / m_ptr->lev;
                prt_experience();

                identify(&item_val);

                // NOTE: this is never read after this, so commenting out. -MRC-
                // i_ptr = &inventory[item_val];
            }
        } else if (!known1_p(i_ptr)) {
            sample(i_ptr);
        }
        if (used_up) {
            desc_remain(item_val);
            inven_destroy(item_val);
        }
    }
}
