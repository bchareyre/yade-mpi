from yade import pack, export
pred = pack.inAlignedBox((0,0,0),(10,10,10))
O.bodies.append(pack.randomDensePack(pred,radius=1.,rRelFuzz=.5,spheresInCell=500,memoizeDb='/tmp/pack.db'))

export.textExt('/tmp/test.txt',format='x_y_z_r_attrs',attrs=('b.state.pos.norm()','b.state.pos'),comment='dstN dstV_x dstV_y dstV_z')
# text2vtk and text2vtkSection function can be copy-pasted from yade/py/export.py into separate py file to avoid executing yade or to use pure python
export.text2vtk('/tmp/test.txt','/tmp/test.vtk')
export.text2vtkSection('/tmp/test.txt','/tmp/testSection.vtk',point=(5,5,5),normal=(1,1,1))

# now open paraview, click "Tools" menu -> "Python Shell", click "Run Script", choose "pv_section.py" from this directiory
# or just run python pv_section.py
# and enjoy :-)
