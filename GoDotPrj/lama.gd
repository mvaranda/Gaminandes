extends MeshInstance

const SPRITES_FPS = 30.0
const SPRITE_TM = (1 / SPRITES_FPS)
var slow_flag = false
var acc_delta = 0.0

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

const WALK_START_IMG = 462
const WALK_FINAL_IMG = 477
const WALK_NUM_IMGS = (WALK_FINAL_IMG - WALK_START_IMG)
const WALK_FILENAME_PREFIX = "res://sprites/walk1/walk1_0"
const WALK_FILENAME_EXTENSION = ".png"
var images = []
var image_idx = 0

onready var mesh_i = get_node("MeshInstance")

# Get the material in slot 0
onready var material_one = get_surface_material(0)
	
func load_images():
	var name
	for n in range(WALK_START_IMG, WALK_FINAL_IMG + 1):
		name = WALK_FILENAME_PREFIX + String(n) + WALK_FILENAME_EXTENSION
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
	load_images()

func get_next_frame_fwd_idx(delta, num_frames):
	acc_delta += delta
	var nframes = acc_delta * SPRITES_FPS #/ SPRITE_TM
	var idx = 0
	idx = int(current_frame + nframes)
	if idx >= num_frames:
		idx = idx % num_frames
	return idx
	
func get_next_frame_back_idx(delta, num_frames):
	acc_delta += delta
	var nframes = acc_delta / SPRITE_TM
	var idx = 0
	idx = int(current_frame + nframes)
	if idx >= num_frames:
		idx = idx % num_frames
	return num_frames - idx
	
var current_frame = 0
func move_fwd(delta):
	var idx = get_next_frame_fwd_idx(delta, WALK_NUM_IMGS)
#	image_idx += 1
#	if image_idx >= images.size():
#		image_idx = 0
	material_one.albedo_texture = images[idx]
	set_surface_material(0, material_one)
	
func move_back(delta):
	var idx = get_next_frame_back_idx(delta, WALK_NUM_IMGS)
#	image_idx -= 1
#	if image_idx < 0:
#		image_idx = images.size() - 1;
	material_one.albedo_texture = images[idx]
	set_surface_material(0, material_one)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var localTranslate = Vector3(NORMAL_SPEED * delta,0,0)
	#translate(get_transform().basis.xform(localTranslate))
	if move == MOV_FWD:
		#position.x += NORMAL_SPEED * delta;
		#print("translate fwd")
		translate(get_transform().basis.xform(localTranslate))
		move_fwd(delta)
	if move == MOV_BACK:
		#position.x -= NORMAL_SPEED * delta;
		#print("translate back")
		translate(get_transform().basis.xform(-localTranslate))
		move_back(delta)
	
	
