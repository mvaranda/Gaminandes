import bpy
from operator import itemgetter, attrgetter

filename = "/Users/mvaranda/Downloads/fences_loc.c"
name_pettern = "cbox_fence"


header = """
//*****************************************
//
//       THIS FILE IS AUTO-GENERATED
// 
//    copyrights: Marcelo Varanda 2014
//
//*****************************************

#ifndef WIN32
extern const float fence_pos_array[] = {
#else
extern float fence_pos_array[] = {
#endif
"""

end = """
	0,
};

//---------------------------------------------
"""

def create_file():
    t=[]
    f = open(filename,"w")
    f.write(header)
    for obj in bpy.data.objects:
        name = obj.name
        if name.find(name_pettern) >= 0:
            x = obj.location.x
            t.append([x,name])
            #f.write("\t" + str(x) + ",\t\t\t// " + name + "\n")
    s = sorted(t, key=itemgetter(0))
    for i in s:
        f.write("\t" + str(i[0]) + ",\t\t\t// " + i[1] + "\n")
    f.write(end)
    f.close()

if __name__ == '__main__':
    create_file()

