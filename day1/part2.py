"""
Important challenge text:

Fuel itself requires fuel just like a module - take its mass, divide by three, 
round down, and subtract 2. However, that fuel also requires fuel, and that fuel 
requires fuel, and so on. Any mass that would require negative fuel should 
instead be treated as if it requires zero fuel; the remaining mass, if any, 
is instead handled by wishing really hard, which has no mass and is outside 
the scope of this calculation.

What is the sum of the fuel requirements for all of the modules on your 
spacecraft when also taking into account the mass of the added fuel? (Calculate 
the fuel requirements for each module separately, then add them all up at 
the end.)

Same mass values are used as in part 1.
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

def get_total_fuel_requirement(mass):
    """Get the total fuel requirement of a given mass

    Parameters:
        mass (int): amount of mass to calculate total fuel needed for

    Returns:
        int: total fuel required for the given mass

    """
    step_fuel = get_fuel_requirement(mass)
    total_required = 0

    # add up fuel until required step fuel is less than or equal to zero
    while step_fuel > 0:
        total_required += step_fuel
        step_fuel = get_fuel_requirement(step_fuel)

    return total_required

def main():
    module_masses = get_masses(INPUT_LOCATION)
    module_requirements = list(
        map(
            get_total_fuel_requirement,
            module_masses
        )
    )
    module_sum = sum(module_requirements)
    
    print("TOTAL FUEL NEEDED : %d" % module_sum)

if __name__ == "__main__":
    main()