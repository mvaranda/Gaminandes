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

extends Spatial

var play_song = false
onready var song_caminandes = $mainAudioStreamPlayer
var bush_array = []

signal score_snap_signal(bush)

const NODE_PATH_LAMA = "/root/RootNode/Levels/level1/lama"
const NODE_PATH_MOUNTAINS_0 = "/root/RootNode/Levels/level1/mountains_closer_0"
const NODE_PATH_MOUNTAINS_1 = "/root/RootNode/Levels/level1/mountains_closer_1"
const NODE_PATH_MOUNTAINS_2 = "/root/RootNode/Levels/level1/mountains_closer_2"

var active_bush = -1

const NUM_BUSHES = 38

const LAMA_INITIAL_X = 1.883
const SPEED_LOWER_FACTOR = 0.6
const M1_INITIAL_POS = 46
const M2_INITIAL_POS = 93

onready var m0 = get_node(NODE_PATH_MOUNTAINS_0)
onready var m1 = get_node(NODE_PATH_MOUNTAINS_1)
onready var m2 = get_node(NODE_PATH_MOUNTAINS_2)

onready var sound_snap = $snap

func process_lama_position_signal(lama_pos):
	var pos = (lama_pos + LAMA_INITIAL_X) * SPEED_LOWER_FACTOR
	m0.transform.origin.x = pos
	m1.transform.origin.x = pos + M1_INITIAL_POS
	m2.transform.origin.x = pos + M2_INITIAL_POS

func process_bush_location_signal(name, is_enter):
	var i = name.substr(5).to_int()
	#print("bush " + str(i) + ", enter = " + str(is_enter))
	if is_enter:
		active_bush = i
	else:
		active_bush = -1

func rand_level_1():
	# 1 in 4
	if (randi() % 4) == 0:
		return true
	return false
	
# Called when the node enters the scene tree for the first time.

func process_snaped_signal():
	if active_bush >= 0:
		bush_array[active_bush].enable(false)
		sound_snap.play()
		emit_signal("score_snap_signal", active_bush)
		active_bush = -1
		
func _ready():
	randomize()
	var n = get_node(NODE_PATH_LAMA)
	n.connect("lama_position_signal", self, "process_lama_position_signal");
	n.connect("snaped_signal", self, "process_snaped_signal")
	m0.transform.origin.x = 0
	m1.transform.origin.x = M1_INITIAL_POS
	m2.transform.origin.x = M2_INITIAL_POS
	
	for i in range(NUM_BUSHES):
		n = get_node("bush_" + str(i))
		bush_array.append(n)
		n.enable(rand_level_1())
		n.connect("bush_location_signal", self, "process_bush_location_signal");
		
	
	if play_song:
		song_caminandes.play()
	#$mainAudioStreamPlayer.set_stream_paused(true)
