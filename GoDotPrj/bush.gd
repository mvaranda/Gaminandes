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

const NODE_NAME_LAMA_HEAD_KINEMATIC = "headLama"
var enabled = false

signal bush_location_signal(name, is_enter)


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

func enable(b):
	if enabled != b or visible != b:
		visible = b
		enabled = b
	
func _on_bush_entered(body):
	if enabled and body.name == NODE_NAME_LAMA_HEAD_KINEMATIC:
		#print("bush Enter " + name + "body: " + body.name)
		emit_signal("bush_location_signal", name, true)


func _on_bush_exit(body):
	if enabled and body.name == NODE_NAME_LAMA_HEAD_KINEMATIC:
		#print("bush Exit " + name + "body: " + body.name)
		emit_signal("bush_location_signal", name, false)

