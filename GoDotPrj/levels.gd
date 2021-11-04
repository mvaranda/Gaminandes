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

# To simulate low FPS
const USE_DELAY = false

var mute_all = false
var mute_background = true

const NUM_LEVELS = 3
const NUM_INITIAL_LIFES = 3
const NUM_MAX_LIFES = 6

const ST_PLAY_LEVEL = 0
const ST_WAIT_NEW_LEVEL = 1
const ST_WAIT_NEW_GAME = 2
var state = ST_PLAY_LEVEL

const WAIT_NEW_LEVEL = 2.0
var 	timer_counter = 0.0
var	timer_expire = 0.0

var  is_action_pressed = false
signal key_signal(key, pressed, shift)
signal set_level_signal(level)

const frases : Array = [
	"Excellent",
	"Exceptional",
	"Fabulous",
	"Fantastic",
	"Sensational",
	"Wonderful",
	"Unbelievable",
	"Way to go",
	"Simple superb",
	"Stupendous",
	"Magnificent",
	"Marvelous",
	"Thats great",
	"Good going",
	"Better than ever",
	"You are one of a king",
	"You have mastered it"
]
onready var game_over_panel = get_node("GameOverPanel")
onready var game_over_score = get_node("GameOverPanel/VBoxContainer/ScoreLabel") 
onready var score_label = get_node("ScorePanelContainer/ScoreValue")
onready var level_label = get_node("LevelPanelContainer/LevelValue")
onready var end_level_node = get_node("EndLevelPanelContainer")
onready var level_label_node = get_node("EndLevelPanelContainer/VBoxContainer/level")
onready var motivation_label_node = get_node("EndLevelPanelContainer/VBoxContainer/MotivationFrase")
onready var node_sound_banjo = $sound_banjo
onready var node_sound_thanks = $sound_thanks
onready var node_sound_sax = $sound_sax
onready var node_sound_claps = $sound_claps
onready var node_sound_grand_finale = $sound_grand_finale

onready var lama_node = get_node("level1/lama")
var score = 0
var level = 1
var lifes = NUM_INITIAL_LIFES
var life_icons = []
var life_reward_counter = 0
const LIFE_REWARD_VAL = 3
const LIFE_NODE_NAME_PREFIX = "LamaPanelContainer/LamaIcon_"

func timer_set(time):
	timer_counter = 0.0
	timer_expire = time
	
func timer_update_and_check(delta):
	timer_counter += delta
	if timer_counter > timer_expire:
		return true
	return false

func process_jump_reward ():
	life_reward_counter += 1
	if life_reward_counter >= LIFE_REWARD_VAL:
		if lifes < NUM_MAX_LIFES:
		  lifes += 1
		  update_lifes()
		life_reward_counter = 0

func process_end_level_signal():
	if level < NUM_LEVELS:
		print("next level")
		end_level_node.visible = true
		level_label_node.text = "Level " + str(level) + " completed"
		motivation_label_node.text = frases[randi() % frases.size()]
		timer_set(WAIT_NEW_LEVEL)
		state = ST_WAIT_NEW_LEVEL
		#emit_signal("set_level_signal", level)
		if level == 1:
			g_play_sound(node_sound_sax, false)
		elif level == 2:
			g_play_sound(node_sound_claps, false)
		level += 1
	else:
		g_play_sound(node_sound_grand_finale, false)
		process_game_over()

func process_game_over():
	print("Game over")
	game_over_panel.visible = true
	game_over_score.text = "Score: " + str(score)
	
	
func _ready():
	init_score()
	$level1.connect("score_snap_signal", self, "process_score_snap_signal")
	$level1.connect("end_level_signal", self, "process_end_level_signal")
	lama_node.connect("score_jump", self, "process_score_jump")
	lama_node.connect("score_shock", self, "process_score_shock")
	for i in range(NUM_MAX_LIFES):
		life_icons.append( get_node(LIFE_NODE_NAME_PREFIX + str(i)) )
	update_lifes()

var shift = false
func check_key(event, key, sig):
	if event.is_action_pressed(key):
		if is_action_pressed == false:
			#print("Key " + sig + " pressed")
			emit_signal("key_signal", sig, true, shift)

			is_action_pressed = true
		return true
	if event.is_action_released(key):
		if is_action_pressed == true:
			#print("Key " + sig + " released")
			emit_signal("key_signal", sig, false, false)
			is_action_pressed = false
			shift = false
		return true
	return false

func process_debug_signal():
	process_end_level_signal()

var pressed_x = -1
var pressed_y = -1
func process_mouse(event):
	if event is InputEventMouseButton:
		if event.pressed:
			#print("Mouse pressed at: ", event.position)
			pressed_x = event.position.x
			pressed_y = event.position.y
			print("Mouse pressed x = " + str(pressed_x) + ", y = " + str(pressed_y))
		else:
			print("Mouse released at: ", event.position)
			pressed_x = -1
			pressed_y = -1
			
	if event is InputEventMouseMotion:
		pass
	
func _input(event):
	if event is InputEventMouseButton or event is InputEventMouseMotion:
		process_mouse(event)
		return

	if event.pressed and event.scancode == KEY_SHIFT:
		shift = true
		print("shift")
		emit_signal("key_signal", "key_shift", true, true)
		return

	if event.pressed == false and event.scancode == KEY_SHIFT:
		shift = false
		print("shift release")
		emit_signal("key_signal", "key_shift", false, false)
		return
		
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
		
	if event is InputEventKey and event.pressed and event.scancode == KEY_D:
		emit_signal("key_signal", "key_debug", true, false)
		#process_debug_signal()
		return

################ score functions ##############
const POINTS_FENCE = 100
const POINTS_FENCE_MULTIPLIER = 10
const POINTS_SNAP = 10
const BUSH_COUNTER_INCREMENT = 5

var bush_counter = 0

func init_score():
	score_label.text = str(score)
	level_label.text = str(level)
	
func process_score_snap_signal(bush_idx):
	score += POINTS_SNAP + bush_counter
	bush_counter += BUSH_COUNTER_INCREMENT
	score_label.text = str(score)

func process_score_jump(fence):
	score += POINTS_FENCE + fence * POINTS_FENCE_MULTIPLIER
	score_label.text = str(score)
	process_jump_reward()

func process_score_shock():
	if lifes == 0:
		g_play_sound(node_sound_banjo, false)
		process_game_over()
		$level1/mainAudioStreamPlayer.stop()
		return
	lifes -= 1
	update_lifes()
	
func update_lifes():
	for i in range(NUM_MAX_LIFES):
		if i < lifes:
			life_icons[i].visible = true
		else:
			life_icons[i].visible = false
			
############### Slow Down Test ############
func delay():
	var a = 0
	for i in range(0,1000):
		for ii in range(0,1000):
			a += 1
func _process(delta):
	if USE_DELAY == true:
		delay()
	if state == ST_WAIT_NEW_LEVEL:
		if timer_update_and_check(delta) == true:
			print("timeout")
			emit_signal("set_level_signal", level-1)
			end_level_node.visible = false
			level_label.text = str(level)
			state = ST_PLAY_LEVEL

func g_play_sound(stream, background):
	if mute_all == true:
		return
	if background and mute_background:
		return
	stream.play()

func _on_Button_restart_pressed():
	score = 0
	level = 1
	lifes = NUM_INITIAL_LIFES
	life_reward_counter = 0
	state = ST_PLAY_LEVEL
	update_lifes()
	init_score()
	emit_signal("set_level_signal", 0)
	game_over_panel.visible = false


func _on_setup_button_pressed_():
	$level1/mainAudioStreamPlayer.stop()
	$SetupPanel.visible = true

func _on_setup_close_button_pressed():

	$SetupPanel.visible = false
	mute_all = $SetupPanel/VBoxContainer/CheckBox_mute_all.is_pressed()
	mute_background = $SetupPanel/VBoxContainer/CheckBox_mute_song.is_pressed()
	
	g_play_sound($level1/mainAudioStreamPlayer, true)
