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

const NODE_NAME_LAMA_HEAD_KINEMATIC = "headLama"
signal fence_signal(is_enter, name, is_light_on, from_left)
signal jump_location_signal(is_enter, name)

const LIGHT_OFF_IDX = 0
const LIGHT_ON_IDX = 1

const ST_SHOWING_LIGHT_OFF = 0
const ST_SHOWING_LIGHT_ON = 1

var state = ST_SHOWING_LIGHT_OFF

#onready var center = $red_light
var 	timer_counter = 0.0
var	timer_expire = 0.0


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
	if timer_update_and_check(delta) == true:
		timer_set(2)
		if state == ST_SHOWING_LIGHT_OFF:
			changeMaterial(LIGHT_ON_IDX)
			state = ST_SHOWING_LIGHT_ON
			light_is_on = true
		else:
			changeMaterial(LIGHT_OFF_IDX)
			state = ST_SHOWING_LIGHT_OFF
			light_is_on = false
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
	print("Exit good jump")
	emit_signal("jump_location_signal", get_parent().name, false)


func _on_GoodJumpArea_body_entered(body):
	if body.name == NODE_NAME_LAMA_HEAD_KINEMATIC:
		#print("Enter good jump " + body.name)
		emit_signal("jump_location_signal", get_parent().name, true)

func _on_GoodJumpArea_body_exited(body):
	if body.name == NODE_NAME_LAMA_HEAD_KINEMATIC:
		#print("Exit good jump " + body.name)
		emit_signal("jump_location_signal", get_parent().name, false)

