##############################################################
#
#               G A M I N A N D E S   G A M E
#
# License: Attribution-NonCommercial-ShareAlike CC BY-NC-SA
# https://creativecommons.org/licenses/by-nc-sa/4.0/
#
# Copyrights: Marcelo Varanda
# Initial date: October 2021
# http://www.varanda.ca
#
##############################################################

extends MeshInstance

const LAMA_INITIAL_POSITION = -1.883
const NODE_PATH_LEVELS = "/root/RootNode/Levels"
const NODE_PATH_LEVEL1 = "/root/RootNode/Levels/level1"

signal lama_position_signal(pos)
signal snaped_signal()
signal score_jump(active_fence)
signal score_shock()

const FENCE_NODE_PATHS_PREFIX = "/root/RootNode/Levels/level1/fence_"
const RED_LIGHT_NODE_NAME = "/red_light"
const NUM_FENCES = 7

var fast_walk = false
var fence_nodes_array = []
var active_fence = 0

var is_inside_fence = false
var is_inside_fence_from_left = false
var is_a_good_jump = false
var was_a_good_jump_on_start = false
var light_node

const SPRITES_FPS = 30.0
var acc_delta = 0.0

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
const ST_SNAPING      = 4
const ST_WAIT_STAND_UP = 5
const ST_WAIT_RESTART  = 6

var state = ST_FREE_MOVING

const NORMAL_FWD_SPEED = 0.8
const FAST_FWD_SPEED = 2.0
const WALK_FPS = 30.0
const WALK_START_IMG = 462
const WALK_FINAL_IMG = 477
const WALK_NUM_IMGS = (WALK_FINAL_IMG - WALK_START_IMG) + 1
const WALK_FILENAME_PREFIX = "res://sprites/walk1/walk1_0"
const WALK_FILENAME_EXTENSION = ".png"
var walk_images = []

const JUMP_FWD_SPEED = 5.0
const JUMP_FPS = 20.0
const JUMP_FILENAME_PREFIX = "res://sprites/good_jump/good_jump_0"
const JUMP_FILENAME_EXTENSION = ".png"
const JUMP_START_IMG =    429 # 423
const JUMP_FINAL_IMG     = 453 # 461 #452
const JUMP_NUM_IMGS = (JUMP_FINAL_IMG - JUMP_START_IMG) + 1
const JUMP_TAKEOFF_IDX = 6
const JUMP_LANDING_IDX = 18
var jump_images = []

const SNAP_FPS = 10.0
const SNAP_START_IMG = 1
const SNAP_FINAL_IMG = 7
const SNAP_NUM_IMGS = (SNAP_FINAL_IMG - SNAP_START_IMG) + 1
const SNAP_FILENAME_PREFIX = "res://sprites/snap/snap_000"
const SNAP_FILENAME_EXTENSION = ".png"
var snap_images = []

const FALL_BACK_SPEED = 2.0
const FALL_FPS = 16.0
const EFALL_FILENAME_PREFIX          = "res://sprites/efall/fall_0"
const FALL_FILENAME_PREFIX           = "res://sprites/fall/fall_0"
const FALL_FILENAME_EXTENSION = ".png"
const FALL_START_IMG          = 759
const FALL_FINAL_IMG          = 769
const FALL_NUM_IMGS = (FALL_FINAL_IMG - FALL_START_IMG) + 1
const FALL_STOP_MOVING_IDX   =  6
var fall_images = []
var efall_images = []

var timer_count = 0.0
const WAIT_TM_STAND_UP = 1.0 #ST_WAIT_STAND_UP

var track_limit = false

onready var mesh_i = get_node("MeshInstance")
onready var mountains_i = get_node("../mountais")

onready var sound_shock = $shock
onready var sound_batida = $batida

var snap_signal_sent = false

# Get the material in slot 0
onready var material_one = get_surface_material(0)

func process_debug_key():
	global_transform.origin.x = 33
	#emit_signal("end_")
	
func load_append_img(img_array, name):
	var img = load(name)
	if img == null:
		print("could not load image " + name)
	img_array.append(load(name))

func load_images():
	var name

	for n in range(WALK_START_IMG, WALK_FINAL_IMG + 1):
		name = WALK_FILENAME_PREFIX + String(n) + WALK_FILENAME_EXTENSION
		load_append_img(walk_images, name)
		
	for n in range(SNAP_START_IMG, SNAP_FINAL_IMG + 1):
		name = SNAP_FILENAME_PREFIX + String(n) + SNAP_FILENAME_EXTENSION
		load_append_img(snap_images, name)
	snap_images.append(walk_images[0]) # final image = first walk
	
	for n in range(JUMP_START_IMG, JUMP_FINAL_IMG + 1):
		name = JUMP_FILENAME_PREFIX + String(n) + JUMP_FILENAME_EXTENSION
		load_append_img(jump_images, name)

	for n in range(FALL_START_IMG, FALL_FINAL_IMG + 1):
		name = FALL_FILENAME_PREFIX + String(n) + FALL_FILENAME_EXTENSION
		load_append_img(fall_images, name)

	for n in range(FALL_START_IMG, FALL_FINAL_IMG + 1):
		name = EFALL_FILENAME_PREFIX + String(n) + FALL_FILENAME_EXTENSION
		load_append_img(efall_images, name)

func start_shock():
	node_levels.g_play_sound(sound_shock, false)
	reset_frame_control()
	state = ST_SHOCKING
	
func start_pull_back():
	node_levels.g_play_sound(sound_batida, false)
	reset_frame_control()
	state = ST_PULLING_BACK

func process_key(val, pressed, shift):
	#print("process_key: got " + val)
	fast_walk = shift
	#print("fast_walk: " + str(fast_walk))
	if pressed == false and val != "key_shift":
		move = MOV_NONE
		#print("released")
	else:
		#print("val = " + val)
		if val ==  "key_right":
			move = MOV_FWD
		elif val ==  "key_left":
			move = MOV_BACK
		elif val ==  "key_up":
			reset_frame_control()
			move = MOV_JUMP
			state = ST_JUMPING
			was_a_good_jump_on_start = is_a_good_jump
		elif val ==  "key_down":
			reset_frame_control()
			move = MOV_DOWN
			state = ST_SNAPING
		elif val ==  "key_shock":
			start_shock()
		elif val ==  "key_collide":
			start_pull_back()
		elif val ==  "key_debug":
			process_debug_key()
		else:
			print("unhanlded key")

func process_fence_signal(is_enter, name, is_light_on, from_left):

	is_inside_fence = is_enter
	is_inside_fence_from_left = from_left
	
	# when coming from right (backing up) we push fwd a bit
	if is_inside_fence: 
		if is_inside_fence_from_left == false and state != ST_JUMPING:
			if is_light_on:
				start_shock()
			else:
				transform.origin.x += .1
				emit_signal("lama_position_signal", transform.origin.x)
		else:
			if state == ST_JUMPING and (was_a_good_jump_on_start == false or is_light_on):
				if is_light_on:
					start_shock()
				else:
					start_pull_back()
		
	
#	var log_txt
#	if is_enter == true:
#		log_txt = "Enter fence " + name
#	else:
#		log_txt = "Exit fence " + name
#
#	if is_light_on == true:
#		log_txt += ", light ON"
#	else:
#		log_txt += ", light OFF"
#	if from_left:
#		log_txt += ", from Left"
#	print(log_txt)
	
func jump_location_signal(name, good_jump_location):
	is_a_good_jump = good_jump_location
	#print("good jump = " + str(good_jump_location))

func get_active_fence():
	for i in range(NUM_FENCES):
		if global_transform.origin.x < fence_nodes_array[i].global_transform.origin.x:
			#print("get_active_fence: " + str(i))
			active_fence = i
			return i
	#print("get_active_fence (last): 0" )
	active_fence = 7
	return 7

func process_end_level_signal():
	print("end level")
	state = ST_WAIT_RESTART
	
func process_set_level_signal(level):
	print("lama process_set_level_signal")
	global_transform.origin.x = LAMA_INITIAL_POSITION
	active_fence = 0
	is_inside_fence = false
	is_inside_fence_from_left = false
	is_a_good_jump = false
	was_a_good_jump_on_start = false
	acc_delta = 0.0
	move = MOV_NONE
	state = ST_FREE_MOVING
	timer_count = 0.0
	track_limit = false
	snap_signal_sent = false
	
var node_levels
func process_limit_signal(is_enter):
	print("track limit")
	track_limit = is_enter
	
# Called when the node enters the scene tree for the first time.
func _ready():
	var n
	node_levels = get_node(NODE_PATH_LEVELS)
	node_levels.connect("key_signal", self, "process_key");
	node_levels.connect("set_level_signal", self, "process_set_level_signal")
	
	n = get_node(NODE_PATH_LEVEL1)	
	n.connect("limit_signal", self, "process_limit_signal");
	n.connect("end_level_signal", self, "process_end_level_signal");
		
	for i in range(NUM_FENCES):
		light_node = get_node(FENCE_NODE_PATHS_PREFIX + str(i) + RED_LIGHT_NODE_NAME)
		fence_nodes_array.append(light_node)
		light_node.connect("fence_signal", self, "process_fence_signal");
		light_node.connect("jump_location_signal", self, "jump_location_signal");
		light_node.set_flash(0, i)

	load_images()
	get_active_fence()
	
		
func get_next_frame_fwd_idx(delta, num_frames, fps, rollover):
	acc_delta += delta
	var nframes = acc_delta * fps
	var idx = 0
	idx = int(current_frame + nframes)
	if idx >= num_frames:
		if rollover == true:
			idx = idx % num_frames
		else:
			idx = num_frames - 1
	return idx

func reset_frame_control():
	current_frame = 0
	acc_delta = 0.0

func get_next_frame_back_idx(delta, num_frames, speed):
	num_frames = num_frames - 1
	acc_delta += delta
	var nframes = acc_delta  * speed #SPRITES_FPS
	var idx = 0
	idx = int(current_frame + nframes)
	if idx >= num_frames:
		idx = idx % num_frames
	return num_frames - idx
	
var current_frame = 0
func move_fwd(delta, imgs, fps, rollover):
	var idx = get_next_frame_fwd_idx(delta, imgs.size(), fps, rollover)
	material_one.albedo_texture = imgs[idx]
	set_surface_material(0, material_one)
	return idx
	
func move_back(delta, imgs, speed):
	var idx = get_next_frame_back_idx(delta, imgs.size(), speed)
	material_one.albedo_texture = imgs[idx]
	set_surface_material(0, material_one)

func _process(delta):
	if state == ST_FREE_MOVING:
		var walk_speed
		var anim_walk_speed
		if fast_walk:
			walk_speed = FAST_FWD_SPEED
			anim_walk_speed = WALK_FPS * 2
		else:
			walk_speed = NORMAL_FWD_SPEED
			anim_walk_speed = WALK_FPS
			
		if move == MOV_FWD:
			if is_inside_fence == false:
				transform.origin.x += walk_speed * delta
				emit_signal("lama_position_signal", transform.origin.x)
			else:
				if light_node.light_is_on:
					start_shock()
					return
			move_fwd(delta, walk_images, anim_walk_speed, true) # animate sprite fwd
			
		if move == MOV_BACK:
			if track_limit == false:
				transform.origin.x -= walk_speed * delta
				emit_signal("lama_position_signal", transform.origin.x)
			move_back(delta, walk_images, anim_walk_speed)
			
	if state == ST_SNAPING:
		var idx = move_fwd(delta, snap_images, SNAP_FPS, false)
		if idx >= SNAP_NUM_IMGS/2:
			if snap_signal_sent == false:
				emit_signal("snaped_signal")
				snap_signal_sent = true
			
		if idx >= SNAP_NUM_IMGS:
			#emit_signal("snaped_signal")
			snap_signal_sent = false
			state = ST_FREE_MOVING
			reset_frame_control()

	if state == ST_JUMPING:
		var idx = move_fwd(delta, jump_images, JUMP_FPS, false)
		if idx >= JUMP_NUM_IMGS - 1:
			state = ST_FREE_MOVING
			reset_frame_control()
			get_active_fence()
			emit_signal("score_jump", active_fence)
			
		elif idx < JUMP_NUM_IMGS - 10:
			transform.origin.x += JUMP_FWD_SPEED * delta
			emit_signal("lama_position_signal", transform.origin.x)
			
	if state == ST_SHOCKING:
		var idx = move_fwd(delta, efall_images, FALL_FPS, false)
		if idx >= FALL_NUM_IMGS - 1:
			state = ST_WAIT_STAND_UP
			timer_count = 0.0
			reset_frame_control()
			get_active_fence()
			emit_signal("score_shock")
			
		elif idx < FALL_STOP_MOVING_IDX:
			transform.origin.x -= JUMP_FWD_SPEED * delta
			emit_signal("lama_position_signal", transform.origin.x)
			

	if state == ST_PULLING_BACK:
		var idx = move_fwd(delta, fall_images, FALL_FPS, false)
		if idx >= FALL_NUM_IMGS - 1:
			state = ST_WAIT_STAND_UP
			timer_count = 0.0
			reset_frame_control()
			get_active_fence()
			
		elif idx < FALL_STOP_MOVING_IDX:
			transform.origin.x -= JUMP_FWD_SPEED * delta
			emit_signal("lama_position_signal", transform.origin.x)
			
	if state == 	ST_WAIT_STAND_UP:
		timer_count += delta
		if timer_count >= WAIT_TM_STAND_UP:
			material_one.albedo_texture = walk_images[0]
			set_surface_material(0, material_one)
			state = ST_FREE_MOVING
			reset_frame_control()
			
	if state == ST_WAIT_RESTART:
		pass


