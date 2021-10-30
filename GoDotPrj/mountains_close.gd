extends Spatial


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
const NODE_PATH_RED_LIGHT = "/root/RootNode/Levels/level1/lama"
const NODE_PATH_MOUNTAINS_0 = "/root/RootNode/Levels/level1/mountains_closer_0"
const NODE_PATH_MOUNTAINS_1 = "/root/RootNode/Levels/level1/mountains_closer_1"
const NODE_PATH_MOUNTAINS_2 = "/root/RootNode/Levels/level1/mountains_closer_2"

const LAMA_INITIAL_X = 1.883
const SPEED_LOWER_FACTOR = 0.6
const M1_INITIAL_POS = 46
const M2_INITIAL_POS = 93

onready var m0 = get_node(NODE_PATH_MOUNTAINS_0)
onready var m1 = get_node(NODE_PATH_MOUNTAINS_1)
onready var m2 = get_node(NODE_PATH_MOUNTAINS_2)

func process_lama_position_signal(lama_pos):
	var pos = (lama_pos + LAMA_INITIAL_X) * SPEED_LOWER_FACTOR
	var localTranslate = Vector3(pos,0,0)
	m0.transform.origin.x = pos
	m1.transform.origin.x = pos + M1_INITIAL_POS
	m2.transform.origin.x = pos + M2_INITIAL_POS
	
	
# Called when the node enters the scene tree for the first time.
func _ready():
	var n = get_node(NODE_PATH_RED_LIGHT)
	n.connect("lama_position_signal", self, "process_lama_position_signal");
	m0.transform.origin.x = 0
	m1.transform.origin.x = M1_INITIAL_POS
	m2.transform.origin.x = M2_INITIAL_POS



# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
