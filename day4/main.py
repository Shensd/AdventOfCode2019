

def get_range_from_file(file_location):
    """Return range of digits from the given file location

    Parameters:
        file_location (str): file path location of file

    Returns:
        tuple (int, int): a tuple containing the min and max range numbers
    """
    with open(file_location, "r") as input_file:
        content = input_file.readlines()
        range_min, range_max = content[0].split("-")
        return (int(range_min), int(range_max))

def is_strictly_increasing(number):
    """Returns true if a given number has only non decreasing digits

    Parameters: 
        number (int): number to test digits of

    Returns:
        bool: true if the digits are only increasing
    """

    digits = list(str(number))
    last_number = int(digits[0])

    for i in range(1, len(digits)):
        if int(digits[i]) < last_number:
            return False
        last_number = int(digits[i])
    
    return True

def has_two_consecutive_digits(number):
    """Returns true if the given number contains a set of two indentical 
    consecutive digits

    Parameters:
        number (int): number to test
    
    Returns:
        bool: true if the number contains a set of identical digits
    """

    digits = list(str(number))

    for i in range(0, len(digits) - 1):
        if digits[i] == digits[i+1]:
            return True

    return False

def has_double_groups_only(number):
    """Returns if the given number contains a set of digits that does not bleed
    over into proximity sets

    Parameters:
        number (int): number to test

    Returns:
        bool: true if a set of self contained digits is found
    """

    digits = list(str(number))

    if not has_two_consecutive_digits(number):
        return False

    # construct consecutive groups of a like digits
    groups = [[digits[0]]]
    i = 1
    while i < len(digits):
        current_digit = digits[i]
        if current_digit == groups[-1][0]:
            groups[-1].append(current_digit)
        else:
            groups.append([current_digit])
    
        i+=1

    # at least one group needs to be exactly two digits long
    return any([ len(group) == 2 for group in groups ])
            

def get_passwords_from_range(range_min, range_max):
    """Get passwords from the given range that are strictly increasing and have
    a set of self similar digits

    Parameters:
        range_min (int): starting value
        range_max (int): ending value

    Returns:
        list (int): list of integers that meet the specifications
    """

    passwords = []

    for i in range(range_min, range_max):
        if is_strictly_increasing(i) and has_two_consecutive_digits(i):
            passwords.append(i)

    return passwords

def get_double_grouped_passwords_from_range(range_min, range_max):
    """Get passwords that are strictly increasing and have a set of self similar
    digits without bleed over within the given range

    Parameters:
        range_min (int): starting value
        range_max (int): ending value

    Returns:
        list (int): list of integers that meet the specifications
    """

    passwords = []

    for i in range(range_min, range_max):
        if is_strictly_increasing(i) and has_double_groups_only(i):
            passwords.append(i)

    return passwords

FILE_LOCATION = "./input"

def main():

    range_min, range_max = get_range_from_file(FILE_LOCATION)
    passwords = get_passwords_from_range(range_min, range_max)
    group_pass = get_double_grouped_passwords_from_range(range_min, range_max)

    print("TOTAL PASSWORDS (PART 1) : %d" % len(passwords))

    print(group_pass)

    print("TOTAL PASSWORDS (PART 2) : %d" % len(group_pass))

if __name__ == "__main__":
    main()