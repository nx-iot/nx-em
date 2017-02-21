module(..., package.seeall)
i=0;
function MQTT_Connect(socket_fd)
	print("MQTT_Connect \r\n");
	nx_mqtt.MQTT_Connect(socket_fd);
	vmsleep(100);
end;

function MQTT_Sub(socket_fd)
	print("MQTT_Sub \r\n");
	nx_mqtt.MQTT_Sub(1,"m001/data/buzzer",16,0,socket_fd);
	vmsleep(100);
end;

function MQTT_Pub(Pay,i)
	print("MQTT_Pub \r\n");
	nx_mqtt.MQTT_Publish(1,"d/o/0",5,Pay,i,socket_fd);
	vmsleep(100);
end;

function register_evt_handler(evt,handler)
	print("register event id : " ,evt,"\r\n");
	evt_handler[evt]=handler;
end;

function default_event_handler(evt_id,evt_p1,evt_p2,evt_p3)
	return true;
end;

function event_handler_proc(evt,evt_p1,evt_p2,evt_p3)
	local idx=nil;
	local handler=nil;
	if(evt<=0)then
		return false;
	end;
	for idx, handler in pairs(evt_handler) do
		if (idx==evt) then
			if(handler) then
				return handler(evt,evt_p1,evt_p2,evt_p3);
			else 
				break;
			end;
		end;
	end;
	return default_event_handler(evt,evt_p1,evt_p2,evt_p3);
end;

function timer_event_handler(evt_id,evt_p1,evt_p2,evt_p3)
	print("timer handler :",evt_id,"\r\n");
	--Event Sent
	if(evt_p1 == 0)then
		--nx_queue.remove();
		socket.select(socket_fd, SOCK_WRITE_EVENT);
	--Event Recv
	elseif (evt_p1==1) then
		socket.select(socket_fd, SOCK_READ_EVENT);
		
	-- get gps
	elseif (evt_p1==2) then
		print("get gps_info ","\r\n");
		nx_gps.GPS_INFO();
		nx_gps.GPS_Get_Base();
		nx_gps.GPS_Get_Locat_Base();
	-- get gps base
	elseif (evt_p1==3) then
		print("nx_queue.sio_remove","\r\n");
		--nx_gps.GPS_Get_Base();
		nx_queue.sio_remove();

	-- get gps base
	elseif (evt_p1==4) then
		print("nx_queue.sio_remove2","\r\n");
		--nx_gps.GPS_Get_Locat_Base();
		nx_queue.sio_remove2();
	
	-- get battery	
	elseif (evt_p1==5) then
		print("get battery ","\r\n");
		cmd = "AT+CBC\r\n";
		sio.send(cmd);
	
	-- get adc
	elseif (evt_p1==6) then
		print("get adc ","\r\n");
		cmd = "AT+CADC=0\r\n";
		sio.send(cmd);
		

	end;

	--vmstarttimer(0);
	--vmstarttimer(1);
	--vmstarttimer(2);
	--vmstarttimer(3);
	--vmstarttimer(4);

	return true;
end;



function gpio_event_handler(evt_id,evt_p1,evt_p2,evt_p3)
	print("gpio handler :",evt_id  ,"\r\n");
	return true;
end;

function sio_event_handler(evt_id,evt_p1,evt_p2,evt_p3)
	print("sio handler :",evt_id ," io : " , evt_p1 ,"\r\n");
	rsp = sio.recv(5000)
	print("sio value : ",rsp);
	if(string.len(rsp) > 10)then
		nx_queue.sio_insert2(rsp)
		--MQTT_Pub(rsp,i);
		--i = i+1;
	end;

	return true;
end;

function uart_event_handler(evt_id,evt_p1,evt_p2,evt_p3)
	print("uart handler :",evt_id,"\r\n");
	return true;
end;


function socket_event_handler(evt_id,evt_p1,evt_p2,evt_p3)
	print("socket handler :",evt_id," ",evt_p1," ",evt_p2," ", evt_p3,"\r\n");


	if(evt_p1==0)then
	   --network event
	   	print("network event\r\n");

	elseif(evt_p1==1)then
	   --socket event
	   if(evt_p3==SOCK_WRITE_EVENT)then
	   		print("SOCK_WRITE_EVENT\r\n")
	   		nx_queue.socket_remove();
			
	   	elseif(evt_p3==SOCK_READ_EVENT)then
	   		print("SOCK_READ_EVENT\r\n");
	   		nx_network.network_recv(evt_p2);
	   		nx_queue.socket_remove2();
	   		local lett = 0;
	   		lett = nx_queue.socket_remove2();
	   		vmsleep(1000);
			if(lett == -1)then
				nx_mqtt.MQTT_Ping(socket_fd);
			end;
			
	   elseif(evt_p3==SOCK_CLOSE_EVENT)then
	   		print("SOCK_CLOSE_EVENT\r\n");
	   	
	   elseif(evt_p3==SOCK_ACCEPT_EVENT) then
	   	 	print("SOCK_ACCEPT_EVENT\r\n");
	   end;
	end;

	return true;
end;