import tcpnonblock
import keyboard
import time
import random
from datetime import datetime

server = tcpnonblock.TCPSocketServer() # Create a Server Object
# Start the Server


# Create a Client Instance for the Server
@server.client_instance
class ClientInstance(tcpnonblock.TCPSocketServerInstance):
    number_id=101  
    count_mssg=1   
    # On Client Connect
    def connect(self):
        dtn = datetime.now().strftime("%H:%M:%S.%m")
        print(dtn,"    ","Client Connected")
        print(dtn,"    ","ADDRESS:",self.addr)
        print("\r")
    
    # On Client Disconnect
    def disconnect(self):
        dtn = datetime.now().strftime("%H:%M:%S.%m")
        print(dtn,"    ","Client Disconnected")
    
    # On Client Message
    def message(self, msg):
        dtn = datetime.now().strftime("%H:%M:%S.%m")     
        if("RPT" in msg):
            #r="+01208.169,+01262.988,-00146.809,0 "
            number_id=self.number_id
            if number_id>998: number_id=100
            self.number_id = number_id + 1

            r="+"+str(number_id)+"08.169,+01262.988,-00146.809,0 "

            number_format = "{:05}".format(self.count_mssg)
            print("\r")
            print(dtn,"  #",number_format,"  ","KEYENCE_Received: ",msg)

            print(dtn,"  #",number_format,"  ","KEYENCE_Response: ",r)
            self.count_mssg = self.count_mssg + 1
            self.send(r) # Echo the Message (Send)
        else:
            print("\r")
            print(dtn,"    ","KEYENCE_Discard: ",msg)
# On Server Start Event
@server.on_start
def start(host, port):
    dtn = datetime.now().strftime("%H:%M:%S.%m")
    print(dtn,"    ","Server Start")
    print(dtn,"    ","IP:",host," - ","PORT:",port)
    print("\r")
# On Server Stop Event
@server.on_stop
def stop():
    dtn = datetime.now().strftime("%H:%M:%S.%m")
    print(dtn,"    ","Server Stop")

host = "192.168.0.103"#"192.168.0.103"
port = 11  # initiate port no above 1024

server.listen(host, port) # Host,Port
server.start()

