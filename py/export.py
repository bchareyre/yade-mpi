# encoding: utf-8
from yade.wrapper import *

class VTKWriter:
	"""
	USAGE:
	create object vtk_writer = VTKWriter('base_file_name'),
	add to engines PeriodicPythonRunner with command='vtk_writer.snapshot()'
	"""
	def __init__(self,baseName='snapshot',startSnap=0):
		self.snapCount = startSnap
		self.baseName=baseName

	def snapshot(self):
		import xml.dom.minidom
		#import xml.dom.ext # python 2.5 and later		

		positions=[]; radii=[]

		for b in Omega().bodies:
			if b.shape.name=='Sphere':
				positions.append(b.phys['se3'][0])
				radii.append(b.shape['radius'])

		# Document and root element
		doc = xml.dom.minidom.Document()
		root_element = doc.createElementNS("VTK", "VTKFile")
		root_element.setAttribute("type", "UnstructuredGrid")
		root_element.setAttribute("version", "0.1")
		root_element.setAttribute("byte_order", "LittleEndian")
		doc.appendChild(root_element)

		# Unstructured grid element
		unstructuredGrid = doc.createElementNS("VTK", "UnstructuredGrid")
		root_element.appendChild(unstructuredGrid)

		# Piece 0 (only one)
		piece = doc.createElementNS("VTK", "Piece")
		piece.setAttribute("NumberOfPoints", str(len(positions)))
		piece.setAttribute("NumberOfCells", "0")
		unstructuredGrid.appendChild(piece)

		### Points ####
		points = doc.createElementNS("VTK", "Points")
		piece.appendChild(points)

		# Point location data
		point_coords = doc.createElementNS("VTK", "DataArray")
		point_coords.setAttribute("type", "Float32")
		point_coords.setAttribute("format", "ascii")
		point_coords.setAttribute("NumberOfComponents", "3")
		points.appendChild(point_coords)

		string = str()
		for x,y,z in positions:
			string += repr(x) + ' ' + repr(y) + ' ' + repr(z) + ' '
		point_coords_data = doc.createTextNode(string)
		point_coords.appendChild(point_coords_data)

		#### Cells ####
		cells = doc.createElementNS("VTK", "Cells")
		piece.appendChild(cells)

		# Cell locations
		cell_connectivity = doc.createElementNS("VTK", "DataArray")
		cell_connectivity.setAttribute("type", "Int32")
		cell_connectivity.setAttribute("Name", "connectivity")
		cell_connectivity.setAttribute("format", "ascii")		
		cells.appendChild(cell_connectivity)

		# Cell location data
		connectivity = doc.createTextNode("0")
		cell_connectivity.appendChild(connectivity)

		cell_offsets = doc.createElementNS("VTK", "DataArray")
		cell_offsets.setAttribute("type", "Int32")
		cell_offsets.setAttribute("Name", "offsets")
		cell_offsets.setAttribute("format", "ascii")				
		cells.appendChild(cell_offsets)
		offsets = doc.createTextNode("0")
		cell_offsets.appendChild(offsets)

		cell_types = doc.createElementNS("VTK", "DataArray")
		cell_types.setAttribute("type", "UInt8")
		cell_types.setAttribute("Name", "types")
		cell_types.setAttribute("format", "ascii")				
		cells.appendChild(cell_types)
		types = doc.createTextNode("1")
		cell_types.appendChild(types)

		#### Data at Points ####
		point_data = doc.createElementNS("VTK", "PointData")
		piece.appendChild(point_data)

		# Particle radii
		if len(radii) > 0:
			radiiNode = doc.createElementNS("VTK", "DataArray")
			radiiNode.setAttribute("Name", "radii")
			radiiNode.setAttribute("type", "Float32")
			radiiNode.setAttribute("format", "ascii")
			point_data.appendChild(radiiNode)

			string = str()
			for r in radii:
				string += repr(r) + ' '
			radiiData = doc.createTextNode(string)
			radiiNode.appendChild(radiiData)

		#### Cell data (dummy) ####
		cell_data = doc.createElementNS("VTK", "CellData")
		piece.appendChild(cell_data)

		# Write to file and exit
		outFile = open(self.baseName+'%04d'%self.snapCount+'.vtu', 'w')
#		xml.dom.ext.PrettyPrint(doc, file)
		doc.writexml(outFile, newl='\n')
		outFile.close()
		self.snapCount+=1

