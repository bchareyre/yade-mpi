# constructors, constants as static objects
x,y,z,one=Vector3.UNIT_X,Vector3.UNIT_Y,Vector3.UNIT_Z,Vector3.ONE
x2=Vector3(x)
# conversions to sequence types
list(x2)
tuple(x2)
# operations and operators
x+y+z==one
x.Dot(y)==0
x.Cross(y)==z
# methods
one.Length()

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
axis,angle=q1.ToAxisAngle()

