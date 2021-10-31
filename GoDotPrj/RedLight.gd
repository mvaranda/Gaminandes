extends MeshInstance

const centerMaterials : Array = [
	preload("res://light_off.material"),
	preload("res://light_on.material"),
	 ]

signal fence_signal(is_enter, name, is_light_on, from_left)

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
	state == ST_SHOWING_LIGHT_OFF

func _process(delta):
	if timer_update_and_check(delta) == true:
		timer_set(.5)
		if state == ST_SHOWING_LIGHT_OFF:
			changeMaterial(LIGHT_ON_IDX)
			state = ST_SHOWING_LIGHT_ON
		else:
			changeMaterial(LIGHT_OFF_IDX)
			state = ST_SHOWING_LIGHT_OFF
	pass


func _on_fenceArea_body_entered(body):
	var from_left = false
	var light_on = false

	if body.global_transform.origin.x < global_transform.origin.x:
		from_left = true

	if state == ST_SHOWING_LIGHT_ON:
		light_on = true
	emit_signal("fence_signal", true, get_parent().name, light_on, from_left)


func _on_fenceArea_body_exited(body):
	var from_left = false
	var light_on = false
	
	if state == ST_SHOWING_LIGHT_ON:
		light_on = true
	if body.global_transform.origin.x < global_transform.origin.x:
		from_left = true
		
	emit_signal("fence_signal", false, get_parent().name, light_on, from_left)

