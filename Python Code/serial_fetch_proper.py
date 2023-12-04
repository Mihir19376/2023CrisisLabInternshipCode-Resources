import serial
from time import sleep


def createNewFile(number):
    try:
        f = open("earthquake"+str(number)+".txt", "x")
        f.close()
        return number
    except:
        return createNewFile(number+1)
    
i = ""

if __name__ == '__main__':
    ser = serial.Serial('COM11', 9600, timeout=1)
    ser.reset_output_buffer()
    counter = 0
    while(True):
        v = ser.readline().decode('utf-8').rstrip()
        if len(v) > 0:
            counter = 0
            i += v
            print(i)
        else:
            counter +=1
        sleep(1)
        if(counter > 5):
            break
    


earthquakes = []
counter = -1
counter2=-1
if(i[0:15]=="Start Recieving"):
    i = i[15:len(i)]
data = False
for char in i:
    # '+' seperates each earthquake in the i
    if(char=="+"):
        counter2=-1
        counter+=1
        earthquakes.append([])
    elif(char=="{"):
        data=True
        counter2+=1
        earthquakes[counter].append(char)
    elif(char=="}"):
        data=False
        earthquakes[counter][counter2]+=char
        earthquakes[counter][counter2]+="\n"
    elif data==True:
        earthquakes[counter][counter2]+=char
        

for earthquake in earthquakes:
    print("sjfhvsrfj")
    num = createNewFile(1)
    f = open("earthquake"+str(num)+".txt", "a")
    for line in earthquake:
        f.write(line)
    f.close()