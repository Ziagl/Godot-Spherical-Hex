extends Node3D

func _ready():
	var hex_grid = $SphericalHexGrid
	var tile_mesh = preload("res://tile.tscn")
	
	# Configure grid
	hex_grid.radius = 5.0
	hex_grid.resolution = 3
	hex_grid.hex_size = 0.5
	
	# Generate grid
	hex_grid.generate_grid()
	
	# Example: Color random hexes
	#var count = get_tree().get_node_count()
	#print_tree1()
	for hex in get_tree().get_nodes_in_group('hex_tiles'):
		var node = tile_mesh.instantiate()
#		if randf() > 0.5:
#			color_hex(node, Color(1, 0, 0))
		hex.add_child(node)

#func color_hex(mesh, color):
#	var material = StandardMaterial3D.new()
#	material.albedo_color = color
#	mesh.get_node("Mesh").material_override = material

func print_tree1(node: Node = null):
	if node == null:
		node = get_tree().root  # Start from the root node if no node is provided
	var name = node.name
	var groups = node.get_groups()
	print(node.name)  # Print the node's name with indentation
	for child in node.get_children():
		print_tree1(child)  # Recursively print child nodes with increased indentation
