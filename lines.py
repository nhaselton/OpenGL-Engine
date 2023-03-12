from pathlib import Path

src = ["OpenGL Engine"]
v=[0,0]
cCount = 0
hCount = 0

vsCount = 0
fsCount = 0

for i in range(len(src)):
    print("\n" + src[i] + ":\n")
    #C FILES
    pathlist = Path(src[i]).rglob('*.cpp')
    for path in pathlist:
         # because path is object not string
        path_in_str = str(path)
        if("lib" not in path_in_str and "glad" not in path_in_str and "imgui" not in path_in_str and "imstb" not in path_in_str):
            lines = sum(1 for line in open(path))
            cCount+= lines;
            v[i] += lines;
            print(str(path) + ": " + str(lines))
    #H FILES
    pathlist = Path(src[i]).rglob('*.h')
    for path in pathlist:
         # because path is object not string
        path_in_str = str(path)
        if("lib" not in path_in_str and "glad" not in path_in_str and "imgui" not in path_in_str and "imstb" not in path_in_str):
            lines = sum(1 for line in open(path))
            hCount+= lines;
            v[i] += lines;
            print(str(path) + ": " + str(lines))


print("\n");

for i in range(len(src)):
    print(src[i] + ": " + str(v[i]))
    
print(".c: " + str(cCount))
print(".h: " + str(hCount) + "\n")

print("C code: " + str(cCount + hCount))

     
