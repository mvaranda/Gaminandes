extends Node


var  is_action_pressed = false
signal key_signal(key)

func check_key(event, key, sig):
	if event.is_action_pressed(key):
		if is_action_pressed == false:
			#print("Key " + sig + " pressed")
			emit_signal("key_signal", sig + "_pressed")
			is_action_pressed = true
	if event.is_action_released(key):
		if is_action_pressed == true:
			#print("Key " + sig + " released")
			emit_signal("key_signal", sig + "_released")
			is_action_pressed = false

func _input(event):
	check_key(event, "ui_right", "key_right")
	check_key(event, "ui_left", "key_left")
	check_key(event, "ui_up", "key_up")
	check_key(event, "ui_down", "key_down")

