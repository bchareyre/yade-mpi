#!/bin/bash
gmsh -2 cone.geo -o cone.stl
stl2gts -r < cone.stl > cone.gts
