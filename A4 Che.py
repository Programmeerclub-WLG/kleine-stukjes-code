def read():
	regels=[]
	for a in range(0,10):
		regels.append(list(input()))

regels=[
list('1346242331'), 
list('1243222445'), 
list('1442323455'), 
list('4465555554'), 
list('4351525153'), 
list('2155515533'), 
list('5555555555'), 
list('5256545354'), 
list('1623233266'), 
list('6613412455')
]

color=int(regels[0][0])
count=0

################################################# <junk>
#def controle():
# current=[number_y][number_x]
# if current==color:
#   gotit.append(number_y number_x)
# elif current!=color:
#   current+=1
################################################ </junk>
y_axis=int(0)
x_axis=int(0)


def get_face(y_axis, x_axis):
  
	axes=str(y_axis)+str(x_axis)
#	print("YX:", axes)
#	print("Color:", color)
#	print()
	if int(regels[y_axis][x_axis])==color and axes not in face:
		face.append(axes)

	changed=y_axis+1
	if not changed>9 and int(regels[changed][x_axis])==color and str(changed)+str(x_axis) not in face:
		get_face(changed, x_axis)

	changed=y_axis-1
	if not changed<0 and int(regels[changed][x_axis])==color and str(changed)+str(x_axis) not in face:
		get_face(changed, x_axis)

	changed=x_axis+1
	if not changed>9 and int(regels[y_axis][changed])==color and str(y_axis)+str(changed) not in face:
		get_face(y_axis, changed)

	changed=x_axis-1
	if not changed<0 and int(regels[y_axis][changed])==color and str(y_axis)+str(changed) not in face:
		get_face(y_axis, changed)
		
#	print("face:", face)
	return face


def new_color(current):
	count2=-1
	for a in regels[current]:
		count2+=1
#		print("color:", a)
#		print("y:",current)
#		print("x:", count2)
#		print("current_color:", color)
		if int(a)!=color:
#			print("found")
			return [count2, a]
#	print("not found")
	return [-1, color]


def check_row(current):
	global regels
	global count
	global color
	global face
	new=new_color(current)
	newcoord=new[0]
	newcol=new[1]
	while newcoord!=-1:
		new=new_color(current)
		newcoord=new[0]
		newcol=new[1]
##		print("new:", new)
		if newcoord!=-1:
			count3=-1
			face=[]
#			print()
#			for a in regels:
#				print(str().join(a))
#			print()
			face=get_face(y_axis, x_axis)
#			print("newcolor:", newcol, "("+str(newcoord)+","+str(current)+")")
			for a in face:
				count3+=1
##				print("facepart:", face[count3])
				regels[int(face[count3][0])][int(face[count3][1])]=newcol
				color=int(regels[0][0])
#			print("origcolor:", color)
			count+=1


def main():
#       read()
	current=-1
	for a in regels:
		current+=1
		check_row(current)
#	print()
#	for a in regels:
#		print(str().join(a))
#	print()
	print(count)


main()
