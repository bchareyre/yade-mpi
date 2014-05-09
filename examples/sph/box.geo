acc = 300.0;
hW = 2000.0;
thickW = 50.0;

Point(1) = {0,      300,      0, acc};
Point(2) = {0,      -hW,      0, acc};
Point(3) = {1000.0, -hW,      0, acc};
Point(4) = {2000.0, -hW,      0, acc};
Point(5) = {2300,   -hW+300,  0, acc};
Point(6) = {2300,   -hW,      0, acc};
Point(7) = {3700,   -hW,      0, acc};
Point(8) = {4000,   -hW+300,  0, acc};
Point(9) = {4000,   300,      0, acc};
Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 5};
Line(5) = {5, 6};
Line(6) = {6, 7};
Line(7) = {7, 8};
Line(8) = {8, 9};
Line(9) = {9, 1};
Line Loop(10) = {1, 2, 3, 4, 5, 6, 7, 8, 9};
Plane Surface(11) = {10};
Extrude {0, 0, thickW} {
  Surface{11};
}
