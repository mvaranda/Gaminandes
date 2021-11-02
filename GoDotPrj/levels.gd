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

extends Node

onready var score_label = get_node("ScorePanelContainer/ScoreValue")
onready var level_label = get_node("LevelPanelContainer/LevelValue")
var score = 0
var level = 1

# To simulate low FPS
const USE_DELAY = false

var  is_action_pressed = false
signal key_signal(key, pressed, shift)

func _ready():
	init_score()
	$level1.connect("score_snap_signal", self, "process_score_snap_signal")

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

################ score functions ##############
func init_score():
	score_label.text = str(score)
	level_label.text = str(level)
	
func process_score_snap_signal(bush_idx):
	score += 10 + bush_idx
	score_label.text = str(score)

############### Slow Down Test ############
func delay():
	var a = 0
	for i in range(0,1000):
		for ii in range(0,1000):
			a += 1
func _process(delta):
	if USE_DELAY == true:
		delay()


