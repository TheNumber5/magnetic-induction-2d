#include <stdio.h>
#include <string.h>
#include <math.h>

/* Program that calculates magnetic induction vector fields from individual conductors
and adds them together to create a final magnetic induction vector field and outputs it to a file.
Version: 1.0b
Date: 20/06/2021
*/

struct magnetic_induction_point {
    double x_pos, y_pos, b, b_x, b_y;
};

struct current_conductor {
    double x_pos, y_pos, current_intensity;
};

double m_0 = 4*M_PI*pow(10, -7);

struct magnetic_induction_point calculate_induction_point(double m_r, struct magnetic_induction_point point, struct current_conductor conductor) {
    double distance_x, distance_y, distance;
    distance_x = fabs(point.x_pos-conductor.x_pos);
    distance_y = fabs(point.y_pos-conductor.y_pos);
    distance = sqrt(distance_x*distance_x+distance_y*distance_y);
    if(distance <= 0.00001) {
        point.b = 0;
        point.b_x = 0;
        point.b_y = 0;
    return point;
    }
    double alpha = atan(distance_y/distance_x);
    point.b = m_0*m_r*conductor.current_intensity/(2*M_PI*distance);
    if(point.x_pos==conductor.x_pos && point.y_pos>conductor.y_pos) {
        point.b_x = -point.b;
        point.b_y = 0;
        return point;
    }
    else if(point.x_pos==conductor.x_pos && point.y_pos<conductor.y_pos) {
        point.b_x = point.b;
        point.b_y = 0;
        return point;
    }
    else if(point.x_pos<conductor.x_pos && point.y_pos==conductor.y_pos) {
        point.b_x = 0;
        point.b_y = -point.b;
        return point;
    }
    else if(point.x_pos>conductor.x_pos && point.y_pos==conductor.y_pos) {
        point.b_x = 0;
        point.b_y = point.b;
        return point;
    }
    point.b_x = point.b*sin(alpha);
    point.b_y = point.b*cos(alpha);
    if(point.x_pos<conductor.x_pos && point.y_pos>conductor.y_pos) {
        point.b_x *= -1;
        point.b_y *= -1;
    }
    else if(point.x_pos<conductor.x_pos && point.y_pos<conductor.y_pos) {
        point.b_y *= -1;
    }
    else if(point.x_pos>conductor.x_pos && point.y_pos>conductor.y_pos) {
        point.b_x *= -1;
    }
    return point; //TODO: Make this be less of a disaster in optimization.
}

struct magnetic_induction_point add_magnetic_induction(struct magnetic_induction_point point_1, struct magnetic_induction_point point_2) {
    struct magnetic_induction_point result;
    result.x_pos = point_1.x_pos;
    result.y_pos = point_1.y_pos;
    result.b_x = point_1.b_x+point_2.b_x;
    result.b_y = point_1.b_y+point_2.b_y;
    result.b = sqrt(pow(result.b_x, 2)+pow(result.b_y, 2));
    return result;
};

int main(int argc, char** argv) {
    int size;
    double step, m_r;
    int n;
    FILE *f_in, *f_out;
    if(!(f_in = fopen(argv[1], "rb"))) {
        perror(argv[1]);
        return 0;
    }
    f_in = fopen(argv[1], "rb");
    f_out = fopen(argv[2], "wb");
    fscanf(f_in, "%i %lf %lf %i", &size, &step, &m_r, &n);
    double current_position_x = -1*size*step, current_position_y = size*step;
    struct magnetic_induction_point field[size*2+1][size*2+1][n];
    struct magnetic_induction_point result_field[size*2+1][size*2+1];
    for(int k=0; k<n; k++){
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            field[j][i][k].x_pos = current_position_x;
            field[j][i][k].y_pos = current_position_y;
            current_position_x += step;
        }
        current_position_x = -1*size*step;
        current_position_y -= step;
    }
    current_position_y = size*step;
    }
    struct current_conductor conductors[n];
    for(int i=0; i<n; i++) {
        fscanf(f_in, "%lf %lf %lf", &conductors[i].x_pos, &conductors[i].y_pos, &conductors[i].current_intensity);
    }
    if(n==1) {
    for(int i=0; i<size*2+1; i++) {
    for(int j=0; j<size*2+1; j++) {
        result_field[j][i] = calculate_induction_point(m_r, field[j][i][0], conductors[0]);
        result_field[j][i].x_pos = field[j][i][0].x_pos;
        result_field[j][i].y_pos = field[j][i][0].y_pos;
    }
    }
    }
    else {
    for(int k=0; k<n; k++) {
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            field[j][i][k] = calculate_induction_point(m_r, field[j][i][k], conductors[k]);
        }
    }
    }
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
        result_field[j][i] = add_magnetic_induction(field[j][i][0], field[j][i][1]);
    }
    }
    for(int k=2; k<n; k++) {
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
        result_field[j][i] = add_magnetic_induction(result_field[j][i], field[j][i][k]);
    }
    }
    }
    }
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            fprintf(f_out, "%0.2lf %0.2lf %0.10lf %0.10lf\n", result_field[j][i].x_pos, result_field[j][i].y_pos, result_field[j][i].b_x, result_field[j][i].b_y);
        }
    }
    printf("Vector field written to %s successfully", argv[2]);
    fclose(f_in);
    fclose(f_out);
    return 0;
}
