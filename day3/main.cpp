#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>

class Point {
public:
    int x, y;
    Point(int x, int y) : x(x), y(y) {}
    Point(const Point& xy) : x(xy.x), y(xy.y) {}
    void operator=(const Point& xy) {
        x = xy.x;
        y = xy.y;
    }
    inline bool operator==(const Point& point) const {
        return x == point.x && y == point.y;
    }

    std::string to_string(void) const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

class Line {
public:
    const Point& origin;
    const Point& dest;
    Line(const Point& origin, const Point& dest) : origin(origin), dest(dest) {}
    Line(const Line& line) : origin(line.origin), dest(line.dest) {}

    std::string to_string(void) const {
        return origin.to_string() + " --- " + dest.to_string(); 
    }

    int length(void) {
        int len = ( std::max( std::abs(origin.x - dest.x), std::abs(origin.y - dest.y) ) );

        std::cout << origin.to_string() << " --> " << dest.to_string() << ", LEN: " << len << std::endl;

        return len;
    }
};

class Wire {
public:
    std::vector<Point> points;
    int length;

    Wire(std::vector<Point> points) {
        Wire::points = points;
        Wire::length = points.size();
    }

    Line get_line(int index) const {
        if(index == points.size()) index-=1;

        return Line(points[index], points[index+1]);
    }

    int get_wire_length(int index, int offset) {
        int total_len = 0;
        for(int i = 0; i < index; i++) {
            total_len += get_line(i).length();
        }

        std::cout << "LINE INDEX " <<  index << " WITH LEN " << total_len << " WITH OFFSET " << offset << std::endl;

        return total_len + offset;
    }

    bool does_wire_intersect(int x, int y) const {
        for(int i = 0; i < points.size() - 1; i++) {
            const Point* a = &points[i];
            const Point* b = &points[i+1];

            // this is an x crossing
            if(a->x == b->x) {
                if(x <= std::max(a->x, b->x) && x >= std::min(a->x, b->x)) {
                    return true;
                }
            }
            // this is a y crossing
            if(a->y == b->y) {
                if(y <= std::max(a->y, b->y) && y >= std::min(a->y, b->y)) {
                    return true;
                }
            }
        }

        return false;
    }
};

std::vector<Point> convert_directions_to_point_list(std::vector<std::string> directions) {

    std::vector<Point> points;
    Point last_point(0, 0);
    points.push_back(last_point);

    for(int i = 0; i < directions.size(); i++) {

        char direction = directions[i][0];
        int distance = std::stoi(
            directions[i].substr(
                1, 
                directions[i].length()
            )
        );

        Point current_point(points[points.size()-1]);
        switch(direction) {
            case 'U':
                current_point.y += distance;
                break;
            case 'D':
                current_point.y -= distance;
                break;
            case 'L':
                current_point.x -= distance;
                break;
            case 'R':
                current_point.x += distance;
                break;
            default:
                std::cout << "Encountered unexpected direction '" << direction << "'" << std::endl;
                exit(-1);
        }

        points.push_back(current_point);
    }

    return points;
}

std::vector<std::string> get_directions(std::string wire_description) {

    std::stringstream wire_stream(wire_description);
    std::vector<std::string> directions;

    while(!wire_stream.eof()) {
        char buffer[512];
        wire_stream.getline(buffer, 512, ',');
        directions.push_back(buffer);
    }

    return directions;

}

std::pair<Wire, Wire> get_wires_from_file(std::string file_location) {

    std::ifstream input_file(file_location);

    if(!input_file.is_open()) {
        std::cout << "Unable to open input file '" << file_location << "'" << std::endl;
        exit(-1);
    }

    std::vector<std::string> lines;
    unsigned int line_number = 0;
    while(!input_file.eof()) {
        char buffer[2048];
        input_file.getline(buffer, 2048);
        lines.push_back(buffer);
    }
    input_file.close();

    return std::pair<Wire, Wire>(
        Wire(
            convert_directions_to_point_list(get_directions(lines[0]))),
        Wire(
            convert_directions_to_point_list(get_directions(lines[1])))
    );

}

int get_manhatten_distance(const Point& point) {
    return std::abs(point.x) + std::abs(point.y);
}

bool in_range(int child, int parent_a, int parent_b) {
    return (child <= std::max(parent_a, parent_b) && child >= std::min(parent_a, parent_b));
}

bool is_line_vertical(const Line& line) {
    return line.origin.x == line.dest.x;
}

bool is_line_horizontal(const Line& line) {
    return line.origin.y == line.dest.y;
}

Point get_intersection(const Line& a, const Line& b) {
    Line vertical(is_line_vertical(a) ? a : b);
    Line horizontal(is_line_horizontal(a) ? a : b);

    return Point(vertical.origin.x, horizontal.origin.y);
}

bool do_lines_intersect(const Line& line_a, const Line& line_b) {
    Line vert = is_line_vertical(line_a) ? line_a : line_b;
    Line hort = is_line_horizontal(line_a) ? line_a : line_b;

    int x1 = std::min(vert.origin.x, vert.dest.x);
    int y1 = std::min(vert.origin.y, vert.dest.y);
    int x2 = std::max(vert.origin.x, vert.dest.x);
    int y2 = std::max(vert.origin.y, vert.dest.y);

    int x3 = std::min(hort.origin.x, hort.dest.x);
    int y3 = std::min(hort.origin.y, hort.dest.y);
    int x4 = std::max(hort.origin.x, hort.dest.x);
    int y4 = std::max(hort.origin.y, hort.dest.y);

    // return (
    //     (x1 >= x3 && x1 <= x4 && x2 >= x3 && x2 <= x4 && y1 <= y3 && y2 >= y4)
    // );

    Point i = get_intersection(line_a, line_b);

    return (
        i.x >= x3 && i.x <= x4 &&
        i.y >= y1 && i.y <= y2
    );
}

int get_intersection_line_distance(const Line& line, const Point& point) {
    return std::max( std::abs(point.x - line.origin.x), std::abs(point.y - line.origin.y) );
}

#define INPUT_FILE "./test_input"

int main(int argc, char* argv[]) {

    std::pair<Wire, Wire> wires = get_wires_from_file(INPUT_FILE);

    //int line_length = std::min(wires.first.length, wires.second.length);
    int lowest_distance = 2000;
    Point lowest_dist_pos(0,0);

    int lowest_length = 40000;
    Point lowest_len_pos(0, 0);

    for(int i = 0; i < wires.first.length-1; i++) {
        Line a = wires.first.get_line(i);
        for(int j = 0; j < wires.second.length-1; j++) {
            Line b = wires.second.get_line(j);

            if(do_lines_intersect(a, b)) {
                Point intersection = get_intersection(a, b);
                int distance = get_manhatten_distance(intersection);

                int a_len = wires.first.get_wire_length(
                    i, 
                    get_intersection_line_distance(a, intersection));
                int b_len = wires.second.get_wire_length(
                    i, 
                    get_intersection_line_distance(b, intersection));
                int total_len = a_len + b_len;

                std::cout << "FOUND INTERSECT AT " << intersection.to_string() 
                    << ", WITH DISTANCE " << distance 
                    << ", AND LENGTH " << total_len << std::endl;

                if(distance == 0) continue;

                if(distance < lowest_distance) {
                    lowest_dist_pos = intersection;
                    lowest_distance = distance;
                }
                if(total_len < lowest_length) {
                    lowest_len_pos = intersection;
                    lowest_length = total_len;
                }
            }
        }
    }

    std::cout << "CLOSEST POINT  : " << lowest_dist_pos.to_string() << std::endl;
    std::cout << "CLOSEST DIST   : " << lowest_distance << std::endl;

    std::cout << "SHORTEST POINT : " << lowest_len_pos.to_string() << std::endl;
    std::cout << "SHORTEST DIST  : " << lowest_length << std::endl;

    return 0;
}