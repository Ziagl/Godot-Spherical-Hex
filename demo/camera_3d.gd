extends Camera3D

# Speed of orbit in radians per second
@export var orbit_speed: float = 0.5

# Internal angle tracker
var angle: float = 0.0
var radius: float = 0.0
var height: float = 0.0

func _ready():
	# Calculate initial radius and height from current position
	var pos = global_transform.origin
	radius = sqrt(pos.x * pos.x + pos.z * pos.z)
	height = pos.y
	angle = atan2(pos.z, pos.x)

func _process(delta):
	# Update angle over time
	angle += orbit_speed * delta

	# Compute new position on orbit path
	var new_x = radius * cos(angle)
	var new_z = radius * sin(angle)
	var new_position = Vector3(new_x, height, new_z)

	# Move camera and make it look at origin
	global_transform.origin = new_position
	look_at(Vector3.ZERO, Vector3.UP)
