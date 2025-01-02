#
# WRITE YOUR CODE HERE AND SEND ONLY THIS FILE TO US.
#
# DO NOT DEFINE get_data() here. Check before submitting

import math
import random

from evaluator import *    # get_data() will come from this import




universal_state = get_data()[-1]
M = get_data()[0]
N = get_data()[1]
D = get_data()[2]
K = get_data()[3]
LAMBDA = get_data()[4]
MU = get_data()[5]
universal_state2= get_data()[-1]
people = len(universal_state)


def place_checker(next_coordinate, universal):
	for i in range(0, people):
		if universal[i][0]== next_coordinate:
			return False
	return True

def coordinate_checker(next_coordinate):
	if next_coordinate[0]<N and next_coordinate[0]>=0:
		if next_coordinate[1]<M and next_coordinate[1]>=0:
			return True
		else:
			return False
	else:
		return False







def new_move():
	# write here your definition. You can also define helper functions






	for i in range(0,people):
		paths=["F","FR","R","BR","B","BL","L","FL"]

		b = random.choices(paths, weights=[MU/2, MU/8, (1/2)*(1-MU-MU**2),(2/5)*(MU**2), (MU**2)/5, (2/5)*(MU**2),(1/2)*(1-MU-MU**2),MU/8], k=1)
		xcoordinate = universal_state[i][0][0]
		ycoordinate = universal_state[i][0][1]





		if b == ["B"]:
			Gidecegi_yon = ((universal_state[i][1]-4) %8)

		elif b == ["BL"]:
			Gidecegi_yon = ((universal_state[i][1] -3 ) %8)


		elif b ==["BR"]:
			Gidecegi_yon = ((universal_state[i][1] - 5) %8)


		elif b == ["L"]:
			Gidecegi_yon = ((universal_state[i][1] -2) %8)


		elif b == ["R"]:
			Gidecegi_yon = ((universal_state[i][1] -6) %8)


		elif b == ["FL"]:
			Gidecegi_yon = ((universal_state[i][1] - 1) %8)


		elif b == ["FR"]:
			Gidecegi_yon = ((universal_state[i][1] -7) %8)


		elif b == ["F"]:
			Gidecegi_yon = ((universal_state[i][1] ) %8)

		if Gidecegi_yon==0:
				if place_checker((xcoordinate,ycoordinate+1), universal_state) and coordinate_checker((xcoordinate,ycoordinate+1)):
					ycoordinate = ycoordinate+1
					universal_state[i][0] = (xcoordinate, ycoordinate)
					universal_state[i][1] = Gidecegi_yon
		elif Gidecegi_yon==1:

				if place_checker((xcoordinate-1,ycoordinate+1), universal_state) and coordinate_checker((xcoordinate-1,ycoordinate+1)):
					ycoordinate= ycoordinate+1
					xcoordinate= xcoordinate-1
					universal_state[i][0] = (xcoordinate, ycoordinate)
					universal_state[i][1] = Gidecegi_yon
		elif Gidecegi_yon==2:

				if place_checker((xcoordinate-1,ycoordinate), universal_state) and coordinate_checker((xcoordinate-1,ycoordinate)):
					ycoordinate= ycoordinate+0
					xcoordinate= xcoordinate-1
					universal_state[i][0] = (xcoordinate, ycoordinate)
					universal_state[i][1] = Gidecegi_yon
		elif Gidecegi_yon==3:

				if place_checker((xcoordinate-1,ycoordinate-1), universal_state) and coordinate_checker((xcoordinate-1,ycoordinate-1)):
					ycoordinate= ycoordinate-1
					xcoordinate= xcoordinate-1
					universal_state[i][0] = (xcoordinate, ycoordinate)
					universal_state[i][1] = Gidecegi_yon
		elif Gidecegi_yon==4:

				if place_checker((xcoordinate,ycoordinate-1), universal_state) and coordinate_checker((xcoordinate,ycoordinate-1)):
					ycoordinate= ycoordinate-1
					xcoordinate= xcoordinate+0
					universal_state[i][0] = (xcoordinate, ycoordinate)
					universal_state[i][1] = Gidecegi_yon
		elif Gidecegi_yon==5:

				if place_checker((xcoordinate+1,ycoordinate-1), universal_state) and coordinate_checker((xcoordinate+1,ycoordinate-1)):
					ycoordinate= ycoordinate-1
					xcoordinate= xcoordinate+1
					universal_state[i][0] = (xcoordinate, ycoordinate)
					universal_state[i][1] = Gidecegi_yon
		elif Gidecegi_yon==6:

				if place_checker((xcoordinate+1,ycoordinate), universal_state) and coordinate_checker((xcoordinate+1,ycoordinate)):
					ycoordinate= ycoordinate+0
					xcoordinate= xcoordinate+1
					universal_state[i][0] = (xcoordinate, ycoordinate)
					universal_state[i][1] = Gidecegi_yon
		elif Gidecegi_yon==7:

				if place_checker((xcoordinate+1,ycoordinate+1), universal_state) and coordinate_checker((xcoordinate+1,ycoordinate+1)):
					ycoordinate= ycoordinate+1
					xcoordinate= xcoordinate+1
					universal_state[i][0] = (xcoordinate, ycoordinate)
					universal_state[i][1] = Gidecegi_yon



	for i in range(0,people):
		for j in range(i+1,people):
			x1=universal_state[i][0][0]
			y1=universal_state[i][0][1]
			x2=universal_state[j][0][0]
			y2=universal_state[j][0][1]
			distance = math.sqrt(((x2-x1)**2)+((y2-y1)**2))

			if distance<=D:

				probability= min(1,K/(distance**2))


				if universal_state[i][2]== "masked" and universal_state[j][2]== "notmasked":
					probability = probability/LAMBDA
				if universal_state[i][2]== "notmasked" and universal_state[j][2]== "masked":
					probability = probability/LAMBDA
				if universal_state[i][2] == "masked" and universal_state[j][2] == "masked":
					probability = probability/(LAMBDA**2)
				if (universal_state[i][3]=="infected" and universal_state[j][3]!="infected") or (universal_state[i][3]!="infected" and universal_state[j][3]=="infected"):

					a=random.choices(["infected", "notinfected"], weights=[probability,1-probability], k=1)

					if a==["infected"]:

						if universal_state[i][3]=="infected" and universal_state[j][3]=="notinfected":

							universal_state2[j][3]= a.pop()
						elif universal_state[i][3]=="notinfected" and universal_state[j][3]=="infected":

							universal_state2[i][3]= a.pop()
	for x in range(0, people):
		universal_state[x][3]= universal_state2[x][3]







	return universal_state







