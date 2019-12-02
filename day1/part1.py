"""
Important challenge text:

Fuel required to launch a given module is based on its mass. Specifically, to 
find the fuel required for a module, take its mass, divide by three, round down, 
and subtract 2.

The Fuel Counter-Upper needs to know the total fuel requirement. To find it, 
individually calculate the fuel needed for the mass of each module (your 
puzzle input), then add together all the fuel values.

Mass values were given as a list of numbers.
"""

INPUT_LOCATION = "./inputs"

def get_masses(location):
    """Get the masses listed in the inputs files

    Paramters:
        location (string): location for file containing inputs

    Returns:
        list (string): of individual module masses
    """

    inputs = open(location, "r").readlines()
    # inputs are strings, convert to ints
    inputs = list(map(lambda x: int(x), inputs))

    return inputs

def get_fuel_requirement(mass):
    """Get the fuel requirement of a given mass

    Parameters:
        mass (int): amount of mass to calculate fuel for

    Returns:
        int: amount of required fuel
    """
    # int auto rounds down, as needed in challenge
    return int(mass / 3) - 2

def main():
    module_masses = get_masses(INPUT_LOCATION)
    module_requirements = list(
        map(
            get_fuel_requirement,
            module_masses
        )
    )
    module_sum = sum(module_requirements)
    
    print("TOTAL FUEL NEEDED : %d" % module_sum)

if __name__ == "__main__":
    main()