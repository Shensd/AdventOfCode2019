"use strict";
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (Object.hasOwnProperty.call(mod, k)) result[k] = mod[k];
    result["default"] = mod;
    return result;
};
Object.defineProperty(exports, "__esModule", { value: true });
var fs = __importStar(require("fs"));
function get_file_contents(filepath) {
    var content = fs.readFileSync(filepath, 'utf8');
    return content;
}
function parse_orbit_signature(orbit_string) {
    var tokens = orbit_string.split(")");
    return {
        parent: tokens[0],
        child: tokens[1]
    };
}
function get_orbit_tree(orbit_strings) {
    var orbit_tree = {};
    var string_sigs = orbit_strings.split('\n');
    // filter empty lines
    string_sigs = string_sigs.filter(function (x) { return x.trim() !== ""; });
    string_sigs.forEach(function (orbit_string) {
        var orbit_sig = parse_orbit_signature(orbit_string);
        if (!(orbit_sig.child in orbit_tree)) {
            orbit_tree[orbit_sig.child] = orbit_sig.parent;
        }
    });
    return orbit_tree;
}
function get_orbit_series(planet, orbit_tree) {
    var orbit_series = [];
    var current_planet = planet;
    var parent_orbit = orbit_tree[current_planet];
    while (parent_orbit !== "COM") {
        current_planet = parent_orbit;
        parent_orbit = orbit_tree[current_planet];
        orbit_series.push(current_planet);
    }
    // push COM for completeness sake
    orbit_series.push(parent_orbit);
    return orbit_series;
}
function get_orbit_length(planet, orbit_tree) {
    return get_orbit_series(planet, orbit_tree).length;
}
function get_nearest_ancestor(planet_a, planet_b, orbit_tree) {
    var series_a = get_orbit_series(planet_a, orbit_tree);
    var series_b = get_orbit_series(planet_b, orbit_tree);
    for (var i = 0; i < series_a.length; i++) {
        for (var j = 0; j < series_b.length; j++) {
            if (series_a[i] === series_b[j])
                return series_a[i];
        }
    }
    return "COM";
}
function leaps_between(planet_start, planet_dest, orbit_tree) {
    var orbit_series = get_orbit_series(planet_start, orbit_tree);
    for (var i = 0; i < orbit_series.length; i++) {
        if (orbit_series[i] === planet_dest) {
            return orbit_series.slice(0, i + 1);
        }
    }
    return orbit_series;
}
var FILE_LOCATION = "./input";
function main() {
    var orbit_strings = get_file_contents(FILE_LOCATION);
    var orbit_tree = get_orbit_tree(orbit_strings);
    var total_orbit_length = 0;
    var i = 0;
    for (var planet in orbit_tree) {
        total_orbit_length += get_orbit_length(planet, orbit_tree);
        i += 1;
    }
    console.log("TOTAL ORBIT LENGTH : " + total_orbit_length);
    var me_santa_ancestor = get_nearest_ancestor("YOU", "SAN", orbit_tree);
    var steps_to_dest = leaps_between("YOU", me_santa_ancestor, orbit_tree);
    var steps_to_santa = leaps_between("SAN", me_santa_ancestor, orbit_tree);
    // subtract 2 to account for overlap step, and to count between steps
    var total_steps = steps_to_dest.length + steps_to_santa.length - 2;
    console.log("TOTAL STEPS FROM ME TO SANTA : " + total_steps);
    console.log("WITH ROUTE : " + steps_to_dest.concat(steps_to_santa.reverse().splice(-1)));
}
main();
