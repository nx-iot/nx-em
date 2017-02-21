require "nx_network"
require "nx_mqtt"
require "nx_gps"
require "nx_at"
require "nx_func"
require "nx_queue"

gpio.setv(43,0);
printdir(1)
vmsleep(10000);
nx_at.at_cmd();
vmsleep(10000);
collectgarbage();

SOCK_TCP = 0;
SOCK_UDP = 1;

SOCKET_EVENT=22
SOCK_WRITE_EVENT = 1
SOCK_READ_EVENT = 2
SOCK_CLOSE_EVENT = 4
SOCK_ACCEPT_EVENT = 8

--const value
GPIO_EVENT  = 0
UART_EVENT  = 1
KEYPAD_EVENT = 2
USB_EVENT = 3
AUDIO_EVENT = 4
TIMER_EVENT = 28
SIO_RCVD_EVENT = 29
USER_EVENT_EXIT=35
LED_EVENT=32

chk_con = 0;
evt_handler={}
result="";
ip_address="";
app_handle=0;
server_address = "mqtt.codeunbug.com";
socket_fd=0;



app_handle=nx_network.network_open()
 print("network.open(), app_handle=", app_handle, "\r\n");
ip_address=nx_network.network_dns()
socket_fd = socket.create(app_handle, SOCK_TCP)

if (not socket_fd) then 
	print("failed to create socket\r\n"); 
elseif (ip_address) then --enable keep alive 
socket.keepalive(socket_fd, true);--this depends on AT+CTCPKA command to set KEEP ALIVE interval 
	print("socket_fd=", socket_fd, "\r\n"); 
end;
print("connecting server...\r\n");
local timeout = 30000;--  '<= 0' means wait for ever; '> 0' is the timeout milliseconds
local connect_result, socket_released = socket.connect(socket_fd, ip_address, 1883, timeout);
	

	nx_gps.S_GPS_Init(); -- init_GPS

	vmstarttimer(0,1000); -- Event write
	vmstarttimer(1,2000); -- Event read
	vmstarttimer(2,15000); -- Event get gps info
	vmstarttimer(3,5000); -- Event get gps base info
	vmstarttimer(4,1000); -- Event get gps base
	--vmstarttimer(5,180000); -- Event batt
	--vmstarttimer(6,60000);
	
	nx_func.register_evt_handler(TIMER_EVENT,nx_func.timer_event_handler);
	nx_func.register_evt_handler(GPIO_EVENT,nx_func.gpio_event_handler);
	nx_func.register_evt_handler(SIO_RCVD_EVENT,nx_func.sio_event_handler);
	nx_func.register_evt_handler(UART_EVENT,nx_func.uart_event_handler);
	nx_func.register_evt_handler(SOCKET_EVENT,nx_func.socket_event_handler);
	
	--socket.select(socket_fd, SOCK_READ_EVENT);
	nx_func.MQTT_Connect(socket_fd); -- MQTT_Connect
	nx_func.MQTT_Sub(socket_fd);	 -- MQTT_Sub

	
	gpio.setv(42,0);
	rst = gpio.setdrt(42,0);
	print(rst,"\r\n");
	rst = gpio.settrigtype(42,0,0);
	print(rst,"\r\n");
	while(true)do
		
		evt, evt_p1,evt_p2,evt_p3,evt_clock = thread.waitevt(15000);
		print("event id : " ,evt,"\r\n");
		if(evt>=0) then
			if(evt == USER_EVENT_EXIT)then
				print("exit main loop\r\n");
				break;
			else
				nx_func.event_handler_proc(evt,evt_p1,evt_p2,evt_p3);
			end;
		end;
		
		
	end;

--**************************** Pull ********************************
		--cmd = "AT+CADC=0\r\n";
		--print(cmd);
		--sio.clear()  
		--sio.send(cmd)
		--rsp = sio.recv(5000)
		--print(rsp)
		--vmsleep(1000);
		--data = tonumber(string.sub(rsp,10,12));
		--print(data,"\r\n","\r\n");
		--if(data >= 9)then
		--	print("Nathapong");
		--	print("\r\n");
			--vmsleep(3000);
		--end;




