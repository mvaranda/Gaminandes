extends MeshInstance
			
func process_key(val):
	print("process_key: got " + val)

# Called when the node enters the scene tree for the first time.
func _ready():
	var n = get_node("/root/RootNode/Level_1")
	print(n)
	n.connect("key_signal", self, "process_key");


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
