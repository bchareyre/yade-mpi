Point(1) = {-2, -2, -3, 1.0};
Point(2) = {-2, 2, -3, 1.0};
Point(3) = {2, 2, -3, 1.0};
Point(4) = {2, -2, -3, 1.0};
Point(5) = {0, 0, -3, 1.0};
Circle(1) = {2, 5, 1};
Circle(2) = {1, 5, 4};
Circle(3) = {4, 5, 3};
Circle(4) = {3, 5, 2};
Translate {0, 0, 1.5} {
  Duplicata { Line{3, 4, 1, 2}; }
}
Dilate {{0, 0, 1}, 2} {
  Duplicata { Line{2, 1, 4, 3}; }
}
Line(13) = {1, 18};
Line(14) = {6, 4};
Line(15) = {2, 13};
Line(16) = {3, 8};

Line(17) = {1, 19};
Line(18) = {4, 21};
Line(19) = {3, 29};
Line(20) = {2, 24};
Line Loop(21) = {6, -15, -4, 16};
Ruled Surface(22) = {21};
Line Loop(23) = {5, -16, -3, -14};
Ruled Surface(24) = {23};
Line Loop(25) = {8, 14, -2, 13};
Ruled Surface(26) = {25};
Line Loop(27) = {7, -13, -1, 15};
Ruled Surface(28) = {27};
Line Loop(29) = {3, 19, -12, -18};
Ruled Surface(30) = {29};
Line Loop(31) = {18, -9, -17, 2};
Ruled Surface(32) = {31};
Line Loop(33) = {17, -10, -20, 1};
Ruled Surface(34) = {33};
Line Loop(35) = {20, -11, -19, 4};
Ruled Surface(36) = {35};
