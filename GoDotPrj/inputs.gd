extends Node

# To simulate low FPS
const USE_DELAY = false

var  is_action_pressed = false
signal key_signal(key, pressed, shift)

func check_key(event, key, sig):
	if event.is_action_pressed(key):
		if is_action_pressed == false:
			#print("Key " + sig + " pressed")
			emit_signal("key_signal", sig, true, false)
			is_action_pressed = true
		return true
	if event.is_action_released(key):
		if is_action_pressed == true:
			#print("Key " + sig + " released")
			emit_signal("key_signal", sig, false, false)
			is_action_pressed = false
		return true
	return false

func _input(event):
	if check_key(event, "ui_right", "key_right"): return
	if check_key(event, "ui_left", "key_left"): return
	if check_key(event, "ui_up", "key_up"): return
	if check_key(event, "ui_down", "key_down"): return
	
	if event is InputEventKey and event.pressed and event.scancode == KEY_S:
		emit_signal("key_signal", "key_shock", true, false)
		return
	if event is InputEventKey and event.pressed and event.scancode == KEY_C:
		emit_signal("key_signal", "key_collide", true, false)
		return

		
func delay():
	var a = 0
	for i in range(0,1000):
		for ii in range(0,1000):
			a += 1
func _process(delta):
	if USE_DELAY == true:
		delay()


