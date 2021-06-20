# Description
This is a simple C program that reads data from a file and outputs a vector field in the form `<x_position> <y_position> <magnetic_induction_x> <magnetic_induction_y>` to a file.
# Instructions
The command prompt format is like this:
`magneticinduction2d.exe <input_file> <output_file>`
The input and output files can be relative or absolute paths.
The input file needs to be in the form:

```
<size of simulation vector field> <vector field precision step> <relative magnetic permittivity of space> <number of conductors>
<x position of conductor 1> <y position of conductor 1> <current intensity of conductor 1>
<x position of conductor 2> <y position of conductor 2> <current intensity of conductor 2>
...
<x position of conductor n> <y position of conductor n> <current intensity of conductor n>
```

The current intensity is taken, by convention, to be positive when the conductor is coming out of the vector field and negative when it is going into the vector field.
Example of input file:
```
30 0.1 1 2
-1.5 0.5 10000
-1 0 -10000
```
The output file can be viewed in gnuplot or similar programs.

In gnuplot you can use the following command to plot the result:
`plot "[Path to output file]" using 1:2:3:4 with vectors head filled`

Additional information is in the .docx document.
