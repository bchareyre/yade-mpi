acc = 100.0;
wid = 500.0;
len = 1500.0;
h = 300.0;
Point(1) = {0, 0, -wid/5.0, acc};
Point(2) = {-wid, 0, 0, acc};
Point(3) = {wid, 0, 0, acc};
Point(4) = {wid/1.1, 0, -wid/10.0, acc};
Point(5) = {-wid/1.1, 0, -wid/10.0, acc};
Point(6) = {wid/2.0, 0, +wid/10.0, acc};
Point(7) = {-wid/2.0, 0, +wid/10.0, acc};
Point(8) = {-wid, 0, h, acc};
Point(9) = {wid, 0, h, acc};

Spline(1) = {2, 5, 7, 1, 6, 4, 3};
Line(2) = {8, 2};
Line(3) = {3, 9};

Extrude {0, len, 0} {
  Line{2, 1, 3};
}
Line(16) = {8, 9};
Line Loop(17) = {16, -3, -1, -2};
Plane Surface(18) = {17};
