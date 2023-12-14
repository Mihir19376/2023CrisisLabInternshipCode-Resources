import serial
from time import sleep

# Creates a file called "earthquake"n".txt" where n is the smallest integer available
def createNewFile(number):
    try:
        f = open("earthquake"+str(number)+".txt", "x")
        f.close()
        return number
    except:
        return createNewFile(number+1)
#First, all the data is stored in this string
i = ""

if __name__ == '__main__':
    #Establish connection with arduino
    while True:
        try:
            arduinoPort = input("What is the arduino port? (eg COM5) ")
            ser = serial.Serial(arduinoPort, 9600, timeout=1)
            ser.reset_output_buffer()
            break
        except:
            print("Something went wrong, are ypou sure that was the right port?")
    
    counter = 0
    ser.flushInput()
    command = input("Enter the client id: ")
    if command:
        command += "\r\n"
        print(command)
        ser.write(command.encode())

    while(True):
        v = ser.readline().decode('utf-8').rstrip()
        if len(v) > 0:
            counter = 0
            i += v
            print(i)
        else:
            counter +=1
        sleep(1)
        #Timeout function
        if(counter > 10):
            break
    

#This converts it a string, into lists, into text files
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
    num = createNewFile(1)
    print("earthquake "+num+" stored")
    f = open("earthquake"+str(num)+".txt", "a")
    for line in earthquake:
        f.write(line)
    f.close()