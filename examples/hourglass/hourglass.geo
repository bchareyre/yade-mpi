acc = 0.003;
r0 = 0.0025;
r1 = 0.015; h1 = 0.020;
r2 = 0.017; h2 = 0.035;
Point(1) = {0.0, 0.0, 0.0, acc};
Point(2) = {r0, 0.0, 0.0, acc};
Point(3) = {r1, 0.0, h1,  acc};
Point(4) = {r2, 0.0, h2,  acc};
Spline(1) = {2, 3, 4};
Translate {0, 0, h2} {
  Duplicata { Point{1}; }
}
Extrude {{0, 0, 1}, {0, 0, 0}, Pi/2} {
  Line{1};
}
Rotate {{0, 0, 1}, {0, 0, 0}, Pi/2} {
  Duplicata { Surface{5}; }
}
Rotate {{0, 0, 1}, {0, 0, 0}, Pi/2} {
  Duplicata { Surface{6}; }
}
Rotate {{0, 0, 1}, {0, 0, 0}, Pi/2} {
  Duplicata { Surface{11}; }
}
Rotate {{0, 1, 0}, {0, 0, 0}, Pi} {
  Duplicata { Surface{5, 16, 11, 6}; }
}
Line Loop(39) = {4, 8, 13, 18};
Plane Surface(40) = {39};
Line Loop(41) = {38, 33, 28, 23};
Plane Surface(42) = {41};
