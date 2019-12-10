import * as fs from "fs";

/**
 * Return the contents of the file at the given path
 * 
 * @param filepath path to file
 * @return file contents
 */
function get_file_contents(filepath : string) : string {
    let content : string = fs.readFileSync(filepath, 'utf8');

    return content;
}

interface OrbitSignature {
    parent : string,
    child : string
}

/**
 * Convert a string representing an orbit signature into a OrbitSignature object
 * @param orbit_string stirng representing orbit, eg. A)B
 * @returns OrbitSignature object representing orbit
 */
function parse_orbit_signature(orbit_string : string) : OrbitSignature {
    // orbit delimited by a closing parenthesis
    let tokens : string[] = orbit_string.split(")");
    
    return {
        parent: tokens[0],
        child: tokens[1]
    };
}

interface OrbitTree {
    [index : string] : string;
}

/**
 * Get a dictionary representation of all the orbits given in the string of orbits
 * @param orbit_strings a newline delimited list of orbit strings
 * @returns OrbitTree representing all orbits in file
 */
function get_orbit_tree(orbit_strings : string) : OrbitTree {

    let orbit_tree : OrbitTree = {};
    let string_sigs : string[] = orbit_strings.split('\n');
    // filter empty lines
    string_sigs = string_sigs.filter(
        (x) => x.trim() !== ""
    );

    string_sigs.forEach((orbit_string : string) => {
        let orbit_sig : OrbitSignature = parse_orbit_signature(orbit_string);

        // add child to tree if not present
        if(!(orbit_sig.child in orbit_tree)) {
            orbit_tree[orbit_sig.child] = orbit_sig.parent;
        }
    });

    return orbit_tree;
}

/**
 * Get the indirect orbit steps back to the common orbit point
 * @param planet starting planet
 * @param orbit_tree an orbit tree representing valid orbits
 * @returns list of strings representing planets between planet and common orbit
 */
function get_orbit_series(planet : string, orbit_tree : OrbitTree) : string[] {

    let orbit_series : string[] = [];
    let current_planet : string = planet;
    let parent_orbit : string = orbit_tree[current_planet];

    // loop down until we reach the common orbit
    while(parent_orbit !== "COM") {
        current_planet = parent_orbit;
        parent_orbit = orbit_tree[current_planet];

        orbit_series.push(current_planet);
    }

    // push COM for completeness sake
    orbit_series.push(parent_orbit);

    return orbit_series;
}

/**
 * Returns the number of orbit steps between the given planet and the common
 * orbit point
 * @param planet name of planet 
 * @param orbit_tree an orbit tree representing valid orbits
 * @returns number of orbits between planet and COM
 */
function get_orbit_length(planet : string, orbit_tree : OrbitTree) : number {

    return get_orbit_series(planet, orbit_tree).length;
}

/**
 * Get the nearest ancestor planet between two planets
 * @param planet_a first planet to test
 * @param planet_b second planet to test
 * @param orbit_tree an orbit tree representing valid orbits
 * @return name of planet that is the closest ancestor between two given planets
 */
function get_nearest_ancestor(planet_a : string, planet_b : string, orbit_tree : OrbitTree) : string {

    let series_a : string[] = get_orbit_series(planet_a, orbit_tree);
    let series_b : string[] = get_orbit_series(planet_b, orbit_tree);

    for(let i = 0; i < series_a.length; i++) {
        for(let j = 0; j < series_b.length; j++) {
            if(series_a[i] === series_b[j]) return series_a[i];
        }
    }

    return "COM";
}

/**
 * Returns the steps between a child planet and a parent planet, returns full 
 * steps to common orbit point if destination planet is not an indirect orbit
 * of the staring planet
 * @param planet_start starting planet
 * @param planet_dest destionation planet
 * @param orbit_tree an orbit tree representing valid orbits
 * @returns list of planet names between two given planets
 */
function leaps_between(planet_start : string, planet_dest : string, orbit_tree : OrbitTree) : string[] {

    let orbit_series : string[] = get_orbit_series(planet_start, orbit_tree);
    
    for(let i = 0; i < orbit_series.length; i++) {
        if(orbit_series[i] === planet_dest) {
            return orbit_series.slice(0, i+1);
        }
    }

    return orbit_series;
}

const FILE_LOCATION = "./input";

function main() {

    // get file contents and convert to planet tree
    let orbit_strings : string = get_file_contents(FILE_LOCATION);
    let orbit_tree : OrbitTree = get_orbit_tree(orbit_strings);

    let total_orbit_length : number = 0;
    for(let planet in orbit_tree) {
        total_orbit_length += get_orbit_length(planet, orbit_tree);
    }

    console.log(`TOTAL ORBIT LENGTH : ${total_orbit_length}`);
    
    let me_santa_ancestor : string = get_nearest_ancestor("YOU", "SAN", orbit_tree);
    let steps_to_dest : string[] = leaps_between("YOU", me_santa_ancestor, orbit_tree);
    let steps_to_santa : string[] = leaps_between("SAN", me_santa_ancestor, orbit_tree);

    // subtract 2 to account for overlap step, and to count between steps
    let total_steps = steps_to_dest.length + steps_to_santa.length - 2;

    console.log(`TOTAL STEPS FROM ME TO SANTA : ${total_steps}`);
    console.log(`WITH ROUTE : ${steps_to_dest.concat(steps_to_santa.reverse().splice(-1))}`);
}

main();