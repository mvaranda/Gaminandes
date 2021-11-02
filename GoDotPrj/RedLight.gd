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

var light_is_on = false

const centerMaterials : Array = [
	preload("res://light_off.material"),
	preload("res://light_on.material"),
	 ]

const flashLight : Array = [
	# Level 0
	[
		[ 50, 10000],
		[ 1000, 2000],
		[ 1000, 2000],
		[ 2000, 1000],
		[ 2000, 1000],
		[ 1000, 1000],
		[ 500, 500, 500, 1000]
	],
	# level 1
	[
		[250, 250, 250,  1000],
		[250, 250, 250, 250, 250, 1000],
		[1000, 200, 500, 1000],
		[750, 750],
		[750, 750],
		[1000, 200, 500, 1000],
		[ 100, 100, 100, 100, 100, 100, 100, 100, 100, 1000]
	],
	# level 2
	[
		[250, 250, 250, 250, 250, 250, 250, 700],
		[250, 250, 250, 250, 800, 250, 250, 250, 250, 800],
		[500, 500],
		[200, 500],
		[200, 200, 200, 200, 200, 500],
		[200, 200, 200, 200, 200, 200, 200, 500],
		[200, 200, 200, 200, 200, 200, 200, 200, 200, 500]
	]
]

const NODE_NAME_LAMA_HEAD_KINEMATIC = "headLama"
signal fence_signal(is_enter, name, is_light_on, from_left)
signal jump_location_signal(is_enter, name)

const LIGHT_OFF_IDX = 0
const LIGHT_ON_IDX = 1

const ST_SHOWING_LIGHT_OFF = 0
const ST_SHOWING_LIGHT_ON = 1

var state = ST_SHOWING_LIGHT_OFF
var level = 0
var patter = 0
var patter_idx = 0

#onready var center = $red_light
var 	timer_counter = 0.0
var	timer_expire = 0.0

func set_flash(level_val, pattern_val):
	#print("set_flash: level = " + str(level_val) + ", pat = " + str(pattern_val))
	state = ST_SHOWING_LIGHT_OFF
	level = level_val
	patter = pattern_val
	patter_idx = 0
	timer_counter = 0.0
	timer_expire = 0.0

func timer_set(time):
	timer_counter = 0.0
	timer_expire = time
	
func timer_update_and_check(delta):
	timer_counter += delta
	if timer_counter > timer_expire:
		return true
	return false
	
func changeMaterial(mat_idx):
	set_surface_material(0, centerMaterials[mat_idx])
	
func _ready():
	timer_set(2)
	state = ST_SHOWING_LIGHT_OFF

func _process(delta):
	# 	flashLight[level][patter][patter_idx]
	if timer_update_and_check(delta) == true:
		timer_set(flashLight[level][patter][patter_idx] / 1000.0)
		if state == ST_SHOWING_LIGHT_OFF:
			changeMaterial(LIGHT_ON_IDX)
			state = ST_SHOWING_LIGHT_ON
			light_is_on = true
		else:
			changeMaterial(LIGHT_OFF_IDX)
			state = ST_SHOWING_LIGHT_OFF
			light_is_on = false
		patter_idx += 1
		if patter_idx >= flashLight[level][patter].size():
			patter_idx = 0
	pass


func _on_fenceArea_body_entered(body):
	var from_left = false
#	var light_on = false

	if body.global_transform.origin.x < global_transform.origin.x:
		from_left = true

#	if state == ST_SHOWING_LIGHT_ON:
#		light_on = true
		
	emit_signal("fence_signal", true, get_parent().name, light_is_on, from_left)


func _on_fenceArea_body_exited(body):
	var from_left = false
#	var light_on = false
	
#	if state == ST_SHOWING_LIGHT_ON:
#		light_on = true
	if body.global_transform.origin.x < global_transform.origin.x:
		from_left = true
		
	emit_signal("fence_signal", false, get_parent().name, light_is_on, from_left)



func _on_jump_area_entered():
	#print("Enter good jump")
	emit_signal("jump_location_signal", get_parent().name, true)


func _on_jump_area_exited():
	#print("Exit good jump")
	emit_signal("jump_location_signal", get_parent().name, false)


func _on_GoodJumpArea_body_entered(body):
	if body.name == NODE_NAME_LAMA_HEAD_KINEMATIC:
		#print("Enter good jump " + body.name)
		emit_signal("jump_location_signal", get_parent().name, true)

func _on_GoodJumpArea_body_exited(body):
	if body.name == NODE_NAME_LAMA_HEAD_KINEMATIC:
		#print("Exit good jump " + body.name)
		emit_signal("jump_location_signal", get_parent().name, false)

