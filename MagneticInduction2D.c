#include <stdio.h>
#include <string.h>
#include <math.h>

/* Program that calculates magnetic induction vector fields from individual conductors
and adds them together to create a final magnetic induction vector field and outputs it to a file.
Version: 1.1
Date: 14/08/2021
*/

struct magneticInductionPoint {
    double xPos, yPos, b, b_x, b_y;
};

struct currentConductor {
    double xPos, yPos, currentIntensity;
};

double m_0 = 4*M_PI*pow(10, -7);

struct magneticInductionPoint calculateInductionPoint(double m_r, struct magneticInductionPoint point, struct currentConductor conductor) {
    double distancb_x, distancb_y, distance;
    distancb_x = fabs(point.xPos-conductor.xPos);
    distancb_y = fabs(point.yPos-conductor.yPos);
    distance = sqrt(distancb_x*distancb_x+distancb_y*distancb_y);
    if(distance <= 0.00001) {
        point.b = 0;
        point.b_x = 0;
        point.b_y = 0;
    return point;
    }
    double alpha = atan(distancb_y/distancb_x);
    point.b = m_0*m_r*conductor.currentIntensity/(2*M_PI*distance);
    if(point.xPos==conductor.xPos && point.yPos>conductor.yPos) {
        point.b_x = -point.b;
        point.b_y = 0;
        return point;
    }
    else if(point.xPos==conductor.xPos && point.yPos<conductor.yPos) {
        point.b_x = point.b;
        point.b_y = 0;
        return point;
    }
    else if(point.xPos<conductor.xPos && point.yPos==conductor.yPos) {
        point.b_x = 0;
        point.b_y = -point.b;
        return point;
    }
    else if(point.xPos>conductor.xPos && point.yPos==conductor.yPos) {
        point.b_x = 0;
        point.b_y = point.b;
        return point;
    }
    point.b_x = point.b*sin(alpha);
    point.b_y = point.b*cos(alpha);
    if(point.xPos<conductor.xPos && point.yPos>conductor.yPos) {
        point.b_x *= -1;
        point.b_y *= -1;
    }
    else if(point.xPos<conductor.xPos && point.yPos<conductor.yPos) {
        point.b_y *= -1;
    }
    else if(point.xPos>conductor.xPos && point.yPos>conductor.yPos) {
        point.b_x *= -1;
    }
    return point; //TODO: Make this be less of a disaster in optimization.
}

struct magneticInductionPoint addMagneticInduction(struct magneticInductionPoint point1, struct magneticInductionPoint point2) {
    struct magneticInductionPoint result;
    result.xPos = point1.xPos;
    result.yPos = point1.yPos;
    result.b_x = point1.b_x+point2.b_x;
    result.b_y = point1.b_y+point2.b_y;
    result.b = sqrt(pow(result.b_x, 2)+pow(result.b_y, 2));
    return result;
};

int main(int argc, char** argv) {
    int size, n;
    double step, m_r;
    FILE *in_file, *out_file;
    if(!strcmp(argv[1], "-h")) {
        printf("You can view help at the GitHub page of this program:\n");
        printf("https://github.com/TheNumber5/magnetic-induction-2d\n");
        printf("Version: v1.1\nDate: 14/08/2021");
        return 0;
    }
    if(!(in_file = fopen(argv[2], "rb"))) {
       perror(argv[2]);
       return 0;
    }
    in_file = fopen(argv[2], "rb");
    out_file = fopen(argv[3], "wb");
    fscanf(in_file, "%i %lf %lf %i", &size, &step, &m_r, &n);
    double currentPositionX = -1*size*step, currentPositionY = size*step;
    struct magneticInductionPoint magneticField1[size*2+1][size*2+1], magneticField2[size*2+1][size*2+1];
    struct magneticInductionPoint auxField[size*2+1][size*2+1];
    struct currentConductor conductors[n];
    for(int i=0; i<n; i++) {
        fscanf(in_file, "%lf %lf %lf", &conductors[i].xPos, &conductors[i].yPos, &conductors[i].currentIntensity);
    }
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            auxField[j][i].xPos = magneticField2[j][i].xPos = magneticField1[j][i].xPos = currentPositionX;
            auxField[j][i].yPos = magneticField2[j][i].yPos = magneticField1[j][i].yPos = currentPositionY;
            currentPositionX += step;
        }
        currentPositionX = -1*size*step;
        currentPositionY -= step;
    }
    if(n==1) {
        for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            auxField[j][i] = calculateInductionPoint(m_r, auxField[j][i], conductors[0]);
        }
    }
    }
    else {
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            magneticField1[j][i] = calculateInductionPoint(m_r, magneticField1[j][i], conductors[0]);
            magneticField2[j][i] = calculateInductionPoint(m_r, magneticField2[j][i], conductors[1]);
        }
    }
    for(int q=2; q<n+1; q++) {
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            auxField[j][i] = addMagneticInduction(magneticField1[j][i], magneticField2[j][i]);
        }
    }
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            magneticField1[j][i].b = auxField[j][i].b;
            magneticField1[j][i].b_x = auxField[j][i].b_x;
            magneticField1[j][i].b_y = auxField[j][i].b_y;
        }
    }
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            magneticField2[j][i] = calculateInductionPoint(m_r, magneticField2[j][i], conductors[q]);
        }
    }
    }
    }
    if(!strcmp(argv[1], "-g")) {
    for(int i=0; i<size*2+1; i++) {
    for(int j=0; j<size*2+1; j++) {
        fprintf(out_file, "%0.2lf %0.2lf %0.12lf\n", auxField[j][i].xPos, auxField[j][i].yPos, auxField[j][i].b);
    }
    }
    printf("Magnetic field gradient written to %s successfully.", argv[3]);
    }
    else if(!strcmp(argv[1], "-vf")) {
    for(int i=0; i<size*2+1; i++) {
    for(int j=0; j<size*2+1; j++) {
        fprintf(out_file, "%0.2lf %0.2lf %0.12lf %0.12lf\n", auxField[j][i].xPos, auxField[j][i].yPos, auxField[j][i].b_x, auxField[j][i].b_y);
    }
    }
    printf("Magnetic field written to %s successfully.", argv[3]);
    }
    else {
        printf("Unknown command.");
    }
    fclose(in_file);
    fclose(out_file);
return 0;
}

