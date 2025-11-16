fn create_archipelago_menu() -> Ui {
    Ui::new("Archipelago:", List::of(
        UiElement::Input(Input {
            label: Text { text: "Connect (server:port,game,slot[,password])" },
            input: "",
            onclick: fn(input: string) {
                if input.len_utf8() == 0 {
                    return;
                }
                let args = input.split(",");
                let server_and_port = match args.get(0) { Option::Some(s) => s.trim(), Option::None => return };
                let game = match args.get(1) { Option::Some(s) => s.trim(), Option::None => return };
                let slot = match args.get(2) { Option::Some(s) => s.trim(), Option::None => return };
                let password = args.get(3);
                Tas::archipelago_connect(server_and_port, game, slot, password);
                add_component(ARCHIPELAGO_COMPONENT);
                leave_ui();
            },
            onchange: fn(input: string) {},
        }),
        UiElement::Button(UiButton {
            label: Text { text: "Disconnect" },
            onclick: fn(label: Text) {
                remove_component(ARCHIPELAGO_COMPONENT);
                leave_ui();
            },
        }),
        UiElement::Button(UiButton {
            label: Text { text: "Back" },
            onclick: fn(label: Text) { leave_ui(); },
        }),
    ))
}

static mut ARCHIPELAGO_COMPONENT = Component {
    id: ARCHIPELAGO_COMPONENT_ID,
    conflicts_with: List::of(ARCHIPELAGO_COMPONENT_ID, MULTIPLAYER_COMPONENT_ID, NEW_GAME_100_PERCENT_COMPONENT_ID, NEW_GAME_ALL_BUTTONS_COMPONENT_ID, NEW_GAME_NGG_COMPONENT_ID, PRACTICE_COMPONENT_ID, RANDOMIZER_COMPONENT_ID, TAS_COMPONENT_ID, WINDSCREEN_WIPERS_COMPONENT_ID),
    tick_mode: TickMode::DontCare,
    requested_delta_time: Option::None,
    on_tick: update_players,
    on_yield: fn() {},
    draw_hud_text: fn(text: string) -> string {
        return f"{text}\nArchipelago running"
    },
    draw_hud_always: fn() {},
    on_new_game: fn() {
        ARCHIPELAGO_STATE.last_level_unlocked = 1;
        log("Reset last_level_unlocked");
    },
    on_level_change: fn(old: int, new: int) {},
    on_buttons_change: fn(old: int, new: int) {
        // log(f"[AP] # buttons changed: {old} -> {new}");
    },
    on_cubes_change: fn(old: int, new: int) {
        // log(f"[AP] # cubes changed: {old} -> {new}");
    },
    on_platforms_change: fn(old: int, new: int) {
        // log(f"[AP] # platforms changed: {old} -> {new}");
    },
    on_reset: fn(old: int, new: int) {},
    on_element_pressed: fn(index: ElementIndex) {
        // log(f"[AP] Pressed {index.element_type} {index.element_index} in cluster {index.cluster_index}");
        if index.element_type == ElementType::Button {
            log(f"Send location check {10000000 + (index.cluster_index + 1) * 100 + index.element_index + 1} to Archipelago server");
            Tas::archipelago_send_check(10000000 + (index.cluster_index + 1) * 100 + index.element_index + 1);

            if index.cluster_index == 30 { // 31 - 1
                Tas::archipelago_goal();
            }
        }
        if index.element_type == ElementType::Platform {
            log(f"Send location check {10010000 + (index.cluster_index + 1) * 100 + index.element_index + 1} to Archipelago server");
            Tas::archipelago_send_check(10010000 + (index.cluster_index + 1) * 100 + index.element_index + 1);
        }
    },
    on_element_released: fn(index: ElementIndex) {},
    on_key_down: fn(key: KeyCode, is_repeat: bool) {},
    on_key_down_always: fn(key: KeyCode, is_repeat: bool) {},
    on_key_up: fn(key: KeyCode) {},
    on_key_up_always: fn(key: KeyCode) {},
    on_mouse_move: fn(x: int, y: int) {},
    on_component_enter: fn() {},
    on_component_exit: fn() { Tas::archipelago_disconnect(); },
    on_resolution_change: fn() {},
    on_menu_open: fn() {},
};

fn archipelago_disconnected() {
    remove_component(ARCHIPELAGO_COMPONENT);
};
struct ArchipelagoState {
    last_level_unlocked: int,
}
static mut ARCHIPELAGO_STATE = ArchipelagoState {
    last_level_unlocked: 1,
};

// triggers cluster clusterindex
fn archipelago_trigger_cluster(item_index: int){
    let clusterindex = item_index - 10000000;
    if clusterindex < 32 {
        let last_unlocked = ARCHIPELAGO_STATE.last_level_unlocked;
        log(f"Received Trigger cluster {clusterindex} [{last_unlocked}]");
        Tas::set_level(clusterindex - 2);
        if last_unlocked == 7 {
            Tas::trigger_element(ElementIndex { cluster_index: last_unlocked - 1, element_type: ElementType::Button, element_index: 1 });
        }
        if last_unlocked == 10 {
            Tas::trigger_element(ElementIndex { cluster_index: last_unlocked - 1, element_type: ElementType::Button, element_index: 1 });
        }
        if last_unlocked == 18 {
            Tas::trigger_element(ElementIndex { cluster_index: last_unlocked - 1, element_type: ElementType::Button, element_index: 1 });
        }
        if last_unlocked == 26 {
            Tas::trigger_element(ElementIndex { cluster_index: last_unlocked - 1, element_type: ElementType::Button, element_index: 1 });
            Tas::trigger_element(ElementIndex { cluster_index: last_unlocked - 1, element_type: ElementType::Button, element_index: 2 });
        }
        if last_unlocked == 28 {
            Tas::trigger_element(ElementIndex { cluster_index: last_unlocked - 1, element_type: ElementType::Button, element_index: 1 });
        }
        Tas::trigger_element(ElementIndex { cluster_index: last_unlocked - 1, element_type: ElementType::Button, element_index: 0 });
        ARCHIPELAGO_STATE.last_level_unlocked = clusterindex;
    }

    if item_index >= 20000000 {
        if item_index < 30000000 {
            log(f"set_level {item_index - 20000000}");
            Tas::set_level(item_index - 20000000);
        }
    }
    if item_index >= 30000000 {
        log(f"trigger_element {item_index - 30000000} Button 0");
        Tas::trigger_element(ElementIndex { cluster_index: item_index - 30000000, element_type: ElementType::Button, element_index: 0 });
    }
}
