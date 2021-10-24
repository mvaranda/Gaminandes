extends MeshInstance

const NORMAL_SPEED = 0.5
const MOV_NONE = 0
const MOV_FWD  = 1
const MOV_BACK = 2
const MOV_JUMP = 3
const MOV_DOWN = 4

var move = MOV_NONE

const ST_FREE_MOVING  = 0
const ST_JUMPING      = 1
const ST_PULLING_BACK = 2
const ST_SHOCKING     = 3

var state = ST_FREE_MOVING

const TEXTURE_START_IMG = 462
const TEXTURE_FINAL_IMG = 477
const TEXTURE_FILENAME_PREFIX = "res://sprites/walk1/walk1_0"
const TEXTURE_FILENAME_EXTENSION = ".png"
var images = []
var image_idx = 0

var image = load("res://sprites/walk1/walk1_0462.png")
onready var mesh_i = get_node("MeshInstance")

# Get the material in slot 0
onready var material_one = get_surface_material(0)

# Change the texture
#material_one.albedo_texture = image
# Reassign the material
#mesh.set_surface_material(0, material_one)
	
func load_images():
	var name
	for n in range(TEXTURE_START_IMG, TEXTURE_FINAL_IMG + 1):
		name = TEXTURE_FILENAME_PREFIX + String(n) + TEXTURE_FILENAME_EXTENSION
		print(name)
		images.append(load(name))

func process_key(val, pressed, shift):
	print("process_key: got " + val)
	if pressed == false:
		move = MOV_NONE
		print("released")
	else:
		print("val = " + val)
		if val ==  "key_right":
			move = MOV_FWD
		elif val ==  "key_left":
			move = MOV_BACK
		elif val ==  "key_up":
			move = MOV_JUMP
		elif val ==  "key_down":
			move = MOV_DOWN
		else:
			print("unhanlded key")

# Called when the node enters the scene tree for the first time.
func _ready():
	var n = get_node("/root/RootNode/Level_1")
	print(n)
	n.connect("key_signal", self, "process_key");
	print("image:" + image.to_string())
	load_images()

func move_fwd(delta):
	material_one.albedo_texture = images[image_idx]
	image_idx += 1
	if image_idx >= images.size():
		image_idx = 0
	set_surface_material(0, material_one)
	
func move_back(delta):
	material_one.albedo_texture = images[image_idx]
	image_idx -= 1
	if image_idx < 0:
		image_idx = images.size() - 1;
	set_surface_material(0, material_one)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var localTranslate = Vector3(NORMAL_SPEED * delta,0,0)
	#translate(get_transform().basis.xform(localTranslate))
	if move == MOV_FWD:
		#position.x += NORMAL_SPEED * delta;
		print("translate fwd")
		translate(get_transform().basis.xform(localTranslate))
		move_fwd(delta)
	if move == MOV_BACK:
		#position.x -= NORMAL_SPEED * delta;
		print("translate back")
		translate(get_transform().basis.xform(-localTranslate))
		move_back(delta)
	
	
