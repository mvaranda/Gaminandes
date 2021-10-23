extends MeshInstance

const NORMAL_SPEED = 0.5
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

var state = ST_FREE_MOVING
			
func process_key(val, pressed, shift):
	print("process_key: got " + val)
	if pressed == false:
		move = MOV_NONE
		print("released")
	else:
		print("val = " + val)
		if val ==  "key_right":
			move = MOV_FWD
		elif val ==  "key_left":
			move = MOV_BACK
		elif val ==  "key_up":
			move = MOV_JUMP
		elif val ==  "key_down":
			move = MOV_DOWN
		else:
			print("unhanlded key")

# Called when the node enters the scene tree for the first time.
func _ready():
	var n = get_node("/root/RootNode/Level_1")
	print(n)
	n.connect("key_signal", self, "process_key");


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var localTranslate = Vector3(NORMAL_SPEED * delta,0,0)
	#translate(get_transform().basis.xform(localTranslate))
	if move == MOV_FWD:
		#position.x += NORMAL_SPEED * delta;
		print("translate fwd")
		translate(get_transform().basis.xform(localTranslate))
	if move == MOV_BACK:
		#position.x -= NORMAL_SPEED * delta;
		print("translate back")
		translate(get_transform().basis.xform(-localTranslate))
	pass
