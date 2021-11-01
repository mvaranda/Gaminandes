extends Spatial

const NODE_NAME_LAMA_HEAD_KINEMATIC = "headLama"

# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_bush_entered(body):
	if body.name == NODE_NAME_LAMA_HEAD_KINEMATIC:
		print("bush Enter " + name + "body: " + body.name)


func _on_bush_exit(body):
	if body.name == NODE_NAME_LAMA_HEAD_KINEMATIC:
		print("bush Exit " + name + "body: " + body.name)

