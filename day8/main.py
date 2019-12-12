from PIL import Image

def get_file_content(filename):
    """Get the contents of the given file

    Parameters:
        filename (str): name of file

    Returns:
        str: contents of file
    """
    with open(filename, 'r') as input_file:
        return "".join(input_file.readlines())

def get_layers(pixels, width, height):
    """Get layers of given width and height from a pixel string

    Parameters:
        pixels (str): string of pixels
        width (int): width of layers
        height (int): height of layers

    Returns:
        list(list(str)): list of layers from pixels
    """
    layer_size = width * height
    num_layers = int(len(pixels) / layer_size)

    layers = []

    for i in range(0, num_layers):
        # grab all of the current layer's pixels
        layer_pixels = pixels[i*layer_size:(i*layer_size)+layer_size]
        layers.append([])

        # split layer's pixels into rows
        for j in range(0, height):
            layers[i].append(
                layer_pixels[j*width:(j*width)+width]
            )

    return layers

def get_num_digits_in_layer(layer, digit):
    """Get the number of a given digit in a given layer

    Parameters:
        layer (list(str)): layer from layer set
        digit (int): digit to search for

    Returns:
        int: number of digits in the layer
    """
    instances = 0
    for row in layer:
        instances += row.count(str(digit))
    return instances

def get_least_zeros_layer(layers):
    """Find the layer with the least amount of zeros in the given layer set

    Parameters:
        layers (list(list(str))): a layer set

    Returns:
        list(str): layer with least zeros
    """
    min_zeros = 3000
    min_layer = []

    for layer in layers:
        num_zeros = get_num_digits_in_layer(layer, 0)
        if num_zeros < min_zeros:
            min_zeros = num_zeros
            min_layer = layer

    return min_layer

def get_layer_total(layer):
    """Get the total for the given layer

    Parameters:
        layer (list(str)): single layer from layer set

    Returns:
        int: layer total

    """
    master_row = "".join(layer)
    num_1s = master_row.count("1")
    num_2s = master_row.count("2")

    return num_1s * num_2s;

def get_first_non_transparent_pixel(layers, x, y):
    """From a set of layers, find the first pixel at the coordinate that is not
    transparent, and returns it's color value

    Paramters:
        layers (list(list(str))): a layer set
        x (int): x position
        y (int): y position

    Returns:
        tuple(int, int, int, int): color value of pixel
    """
    colors = {
        "0" : (0, 0, 0, 255),
        "1" : (255, 255, 255, 255),
        "2" : (0, 0, 0, 0)
    }

    for layer in layers:
        if layer[y][x] != "2":
            return colors[layer[y][x]]

    return colors[layer[-1][x]]

def generate_image(layers, width, height, output_location):
    """Generate an image from the given set of layers

    Paramters:
        layers (list(list(string))): a layer set
        width (int): width of output image
        height (int): height of output image
        output_location (str): outfile file name
    """
    img = Image.new("RGBA", (width, height))

    for y in range(0, height):
        for x in range(0, width):
            img.putpixel(
                (x, y), 
                get_first_non_transparent_pixel(layers, x, y)
            )

    img.show()
    img.save(output_location)

INPUT_LOCATION = "./input"

def main():
    width, height = 25, 6

    content = get_file_content(INPUT_LOCATION)
    layers = get_layers(content, width, height)

    min_layer = get_least_zeros_layer(layers)

    print(get_layer_total(min_layer))

    generate_image(layers, width, height, "./out.png")

if __name__ == "__main__":
    main()