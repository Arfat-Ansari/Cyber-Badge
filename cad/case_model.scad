// Cyber-Badge Custom Upper & Lower Shell Enclosure (OpenSCAD Parametric CAD)
// Dimensions match the interactive HTML CAD Workstation (110mm x 80mm)

$fn = 60;

// Dimensions (mm)
outer_w = 110.0;
outer_h = 80.0;
corner_r = 9.0;
grip_indent = 3.5;

upper_thick = 6.0;
lower_thick = 12.0;
wall_t = 2.4;

screen_w = 38.0;
screen_h = 44.0;
screen_y = 5.0;

btn_radius = 4.5;
action_radius = 5.0;

// Ergonomic 2D console contour with curved palm grips
module ergonomic_profile(w, h, r, indent) {
    hull() {
        translate([-w/2 + r, -h/2 + r, 0]) circle(r=r);
        translate([w/2 - r, -h/2 + r, 0]) circle(r=r);
        translate([-w/2 + r, h/2 - r, 0]) circle(r=r);
        translate([w/2 - r, h/2 - r, 0]) circle(r=r);
    }
}

// 1. UPPER SHELL (FRONT CASING)
module upper_shell() {
    difference() {
        // Main Outer Shell Solid
        linear_extrude(height=upper_thick) {
            ergonomic_profile(outer_w, outer_h, corner_r, grip_indent);
        }
        
        // 1.69" LCD Screen Bezel Cutout
        translate([0, screen_y, -1])
            cube([screen_w, screen_h, upper_thick + 2], center=true);
            
        // Screen Stepped Recess Lip (1.2mm depth)
        translate([0, screen_y, upper_thick - 1.2])
            cube([screen_w + 3, screen_h + 3, 2], center=true);
            
        // 4x D-Pad Circular Holes (Left Side)
        translate([-33, -7 + 11, -1]) cylinder(r=btn_radius, h=upper_thick + 2); // Up
        translate([-33, -7 - 11, -1]) cylinder(r=btn_radius, h=upper_thick + 2); // Down
        translate([-33 - 11, -7, -1]) cylinder(r=btn_radius, h=upper_thick + 2); // Left
        translate([-33 + 11, -7, -1]) cylinder(r=btn_radius, h=upper_thick + 2); // Right
        
        // 2x Action Button Holes A & B (Right Side)
        translate([30, -7, -1]) cylinder(r=action_radius, h=upper_thick + 2); // B
        translate([42, -2, -1]) cylinder(r=action_radius, h=upper_thick + 2); // A
        
        // 2x Pill Buttons (Start / Select)
        translate([-10, -28, -1]) cylinder(r=2.5, h=upper_thick + 2);
        translate([10, -28, -1]) cylinder(r=2.5, h=upper_thick + 2);
        
        // 4x Corner M3 Screw Countersunk Holes
        for (x = [-outer_w/2 + 8, outer_w/2 - 8]) {
            for (y = [-outer_h/2 + 8, outer_h/2 - 8]) {
                translate([x, y, -1]) cylinder(r=1.7, h=upper_thick + 2); // Clearance M3
                translate([x, y, upper_thick - 2]) cylinder(r=3.2, h=3);   // Countersink
            }
        }
    }
}

// 2. LOWER SHELL (BOTTOM CASING)
module lower_shell() {
    difference() {
        // Outer Shell Solid
        linear_extrude(height=lower_thick) {
            ergonomic_profile(outer_w, outer_h, corner_r, grip_indent);
        }
        
        // Inner Cavity (leaving 2.4mm perimeter wall)
        translate([0, 0, wall_t]) {
            linear_extrude(height=lower_thick) {
                ergonomic_profile(outer_w - 2*wall_t, outer_h - 2*wall_t, corner_r - 1.5, grip_indent);
            }
        }
        
        // LiPo Battery Pocket (52 x 32 x 6.5 mm)
        translate([0, -6, wall_t - 0.1])
            cube([52, 32, 6.5], center=true);
            
        // USB-C Charging Port Slot (Bottom Edge)
        translate([0, -outer_h/2, wall_t + 2])
            cube([12.5, 10, 6.5], center=true);
            
        // 4x Bottom Screw Tap Holes
        for (x = [-outer_w/2 + 8, outer_w/2 - 8]) {
            for (y = [-outer_h/2 + 8, outer_h/2 - 8]) {
                translate([x, y, -1]) cylinder(r=1.4, h=lower_thick + 2);
            }
        }
    }
    
    // 4x Internal Corner Standoff Bosses (For M3 Brass Threaded Inserts)
    for (x = [-outer_w/2 + 8, outer_w/2 - 8]) {
        for (y = [-outer_h/2 + 8, outer_h/2 - 8]) {
            difference() {
                translate([x, y, wall_t]) cylinder(r=3.6, h=lower_thick - wall_t);
                translate([x, y, wall_t - 0.1]) cylinder(r=1.9, h=lower_thick); // 3.8mm hole for M3 heat insert
            }
        }
    }
}

// Render Upper on Left, Lower on Right (Ready for 3D Print Layout)
translate([-outer_w/2 - 10, 0, 0]) upper_shell();
translate([outer_w/2 + 10, 0, 0]) lower_shell();
