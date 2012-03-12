# constructors, constants as static objects
x,y,z,one=Vector3.UnitX,Vector3.UnitY,Vector3.UnitZ,Vector3.One
x2=Vector3(x)
# conversions to sequence types
list(x2)
tuple(x2)
# operations and operators
x+y+z==one
x.dot(y)==0
x.cross(y)==z
# methods
one.norm()

# quaternions
# construction (implicit conversion of 3-tuple or list of length 3 to Vector3)
q1=Quaternion((0,0,1),pi/2)
q2=Quaternion(Vector3(0,0,1),pi/2)
q1==q2
# rotating vector
q1*x==y # almost, due to rounding 
# rotation composition
q1*q1*x 
# inverse rotation
q1.Conjugate()
# convert to axis-angle representation
axis,angle=q1.toAxisAngle()

