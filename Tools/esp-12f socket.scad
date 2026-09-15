$fn = 24;

// Module parameters (Top surface)
mod_w = 16.0;
mod_l = 24.0;
mod_h = 3.2;
tol   = 0.2;

// --- Wall Thickness Parameters ---
side_wall_margin = 2.4;
bot_wall_margin  = 2.4;

// --- Hole Sizing (Straight Vertical Cylinders) ---
chip_hole_d  = 1.1;
proto_hole_d = 1.1;

// --- Hole Placement & Wall Offsets ---
wall_hole_pullback  = 2.2;
floor_hole_pullback = 0.35;

// --- Wall & Rim Groove Parameters ---
wall_groove_w = 1.2;
rim_groove_d  = 1.0;

// --- Configurable Pry / Extraction Notch Parameters ---
pry_notch_w       = 6.0;
pry_notch_l       = 3.5;
pry_notch_depth   = 1.2;
pry_notch_x       = 6.0;

// --- Configurable Screw & Head Parameters ---
top_boss_extra    = 6.0;
m3_tap_d          = 2.5;
screw_head_d      = 5.6;
screw_head_depth  = 3.0;
pcb_thick         = 0.7;
screw_z_lift      = 0.5;

socket_thick  = 7.0;

// Pocket floor reference
pocket_floor_z   = socket_thick / 2 - mod_h;
wall_inner_x     = (mod_w / 2) + tol;
wall_inner_top_y = (mod_l / 2) + tol;
wall_inner_bot_y = -(mod_l / 2) - tol;

// --- Enlarged & Centered Oval Cutout Parameters ---
oval_w        = 12.6; // Leaves 1.5mm support shelf under pads
oval_l        = 18.0;
oval_d        = oval_w;
oval_straight = oval_l - oval_d;
oval_center_y = -0.5; // Centered to maximize material savings

// ESP-12F Pad Coordinates (Floor Holes)
first_pin_from_top = 8.4;
last_pin_from_bot  = 1.3;
num_side_pins      = 8;

side_pins_span     = mod_l - first_pin_from_top - last_pin_from_bot;
side_pitch_top     = side_pins_span / (num_side_pins - 1);

side_span_x        = 15.0 + (floor_hole_pullback * 2);
bot_span_x         = 9.2;
bot_pitch_top      = bot_span_x / 5;
bot_pin_y          = (-(mod_l / 2) + 0.5) - floor_hole_pullback;

// Protoboard 2.54mm (0.1") Grid Parameters (Wall Holes)
proto_pitch    = 2.54;
proto_span_x   = 7 * proto_pitch;
proto_bot_y    = -(mod_l / 2) - proto_pitch;

top_wall_margin = 1.4;

// Straight vertical through-hole across the full height of the casing
module straight_wall_hole(x, y) {
    translate([x, y, 0])
        cylinder(d = proto_hole_d, h = socket_thick + 2, center = true);
}

// Straight vertical through-hole from pocket floor through the bottom
module straight_floor_hole(x, y) {
    h_len = (pocket_floor_z - (-socket_thick / 2)) + 1.0;
    z_pos = (pocket_floor_z + (-socket_thick / 2)) / 2;
    translate([x, y, z_pos])
        cylinder(d = chip_hole_d, h = h_len, center = true);
}

// Groove on top flat rim of the wall
module rim_top_groove(p_wall_hole, p_inner_edge) {
    z_center = (socket_thick / 2) - (rim_groove_d / 2) + 0.05;
    hull() {
        translate([p_wall_hole[0], p_wall_hole[1], z_center])
            cube([wall_groove_w, wall_groove_w, rim_groove_d + 0.1], center = true);
        translate([p_inner_edge[0], p_inner_edge[1], z_center])
            cube([wall_groove_w, wall_groove_w, rim_groove_d + 0.1], center = true);
    }
}

// Vertical groove down the inner wall face
module wall_vertical_groove(x, y) {
    z_center = (pocket_floor_z + (socket_thick / 2)) / 2;
    h_len    = (socket_thick / 2) - pocket_floor_z + 0.1;
    translate([x, y, z_center])
        cylinder(d = wall_groove_w, h = h_len, center = true);
}

module esp12f_slim_socket() {
    base_x = proto_span_x + (wall_hole_pullback * 2) + proto_hole_d + (side_wall_margin * 2);
    
    top_y  = wall_inner_top_y + top_wall_margin + top_boss_extra;
    bot_y  = (proto_bot_y - wall_hole_pullback) - (proto_hole_d / 2) - bot_wall_margin;
    base_y = top_y - bot_y;
    y_center_offset = (top_y + bot_y) / 2;

    screw_z = pocket_floor_z + pcb_thick + screw_z_lift;

    wall_x_left  = -(proto_span_x / 2) - wall_hole_pullback;
    wall_x_right =  (proto_span_x / 2) + wall_hole_pullback;
    wall_y_bot   = proto_bot_y - wall_hole_pullback;

    difference() {
        // Main solid body
        translate([0, y_center_offset, 0])
            cube([base_x, base_y, socket_thick], center = true);

        // Chip pocket on top
        translate([0, 0, socket_thick/2 - mod_h/2 + 0.1])
            cube([mod_w + tol*2, mod_l + tol*2, mod_h + 0.2], center = true);

        // 1. Side Pin Holes + Grooves
        for (i = [0 : num_side_pins - 1]) {
            y_chip  = (mod_l / 2) - first_pin_from_top - (i * side_pitch_top);
            y_proto = proto_bot_y + ((num_side_pins - i) * proto_pitch);

            straight_floor_hole(-side_span_x / 2, y_chip);
            straight_floor_hole(side_span_x / 2, y_chip);

            straight_wall_hole(wall_x_left, y_proto);
            straight_wall_hole(wall_x_right, y_proto);

            rim_top_groove([wall_x_left, y_proto], [-wall_inner_x, y_chip]);
            rim_top_groove([wall_x_right, y_proto], [wall_inner_x, y_chip]);

            wall_vertical_groove(-wall_inner_x, y_chip);
            wall_vertical_groove(wall_inner_x, y_chip);
        }

        // 2. Bottom Pin Holes + Grooves
        for (j = [0:5]) {
            x_chip  = -(bot_span_x / 2) + (j * bot_pitch_top);
            x_proto = -(5 * proto_pitch / 2) + (j * proto_pitch);

            straight_floor_hole(x_chip, bot_pin_y);

            straight_wall_hole(x_proto, wall_y_bot);

            rim_top_groove([x_proto, wall_y_bot], [x_chip, wall_inner_bot_y]);

            wall_vertical_groove(x_chip, wall_inner_bot_y);
        }

        // Enlarged & Centered Oval Cutout
        hull() {
            translate([0, oval_center_y + (oval_straight / 2), 0])
                cylinder(d = oval_d, h = socket_thick + 2, center = true);
            translate([0, oval_center_y - (oval_straight / 2), 0])
                cylinder(d = oval_d, h = socket_thick + 2, center = true);
        }

        // M3 screw hole through top wall
        translate([0, (wall_inner_top_y + top_y) / 2, screw_z])
            rotate([90, 0, 0])
                cylinder(d = m3_tap_d, h = (top_y - wall_inner_top_y) + 2, center = true);

        // Outside wall counterbore recess for the 5.3mm screw head
        translate([0, top_y - (screw_head_depth / 2) + 0.1, screw_z])
            rotate([90, 0, 0])
                cylinder(d = screw_head_d, h = screw_head_depth + 0.2, center = true);

        // Corner pry notch at top-right edge
        pry_h = (socket_thick / 2) - (pocket_floor_z - pry_notch_depth) + 0.2;
        pry_center_z = (pocket_floor_z - pry_notch_depth) + (pry_h / 2);
        translate([pry_notch_x, wall_inner_top_y, pry_center_z])
            cube([pry_notch_w, pry_notch_l, pry_h], center = true);
    }
}

esp12f_slim_socket();