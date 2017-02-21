SOCK_RST_OK = 0
SOCK_RST_TIMEOUT = 1
SOCK_RST_BUSY = 2
SOCK_RST_PARAMETER_WRONG = 3
SOCK_RST_SOCK_FAILED = 4
SOCK_RST_NETWORK_FAILED = 5
module(..., package.seeall)
require "nx_print"

function MQTT_Connect(socket_fd)
	local MQTT_Type = string.char(0x10);
	local Len_P_Name = string.char(0x00,0x06);
	local P_Name = "MQIsdp";
	local P_level = string.char(0x03);
	local C_Flags = string.char(0xC2);
	local TimeOut = string.char(0x00,0xFF);--Time Out = 15s
	local Len_C_Name = string.char(0x00,0x09);
	local C_Name = "Device001";
	local Len_U_Name = string.char(0x00,0x08);
	local U_Name = "rxkyknky";
	local Len_Pass = string.char(0x00,0x0C);
	local Pass_Name = "9-KgEzIl7Uyc";
	local Pro = Len_P_Name..P_Name..P_level..C_Flags..TimeOut..Len_C_Name..C_Name..Len_U_Name..U_Name..Len_Pass..Pass_Name;
	local len = string.char(string.len(Pro));
	local MQTT_req = MQTT_Type..len..Pro;
	local ret = 0;
	print("[INFO] MQTT CPNNECT\r\n");
	nx_print.print_payload(MQTT_req);
	nx_queue.socket_insert(MQTT_req);
	--nx_network.network_send(socket_fd,MQTT_req);
end;

function MQTT_Publish(QOS,Topic,len_Top,Payload,MS_id,socket_fd)
	local MQTT_Type= string.char(0x30);
	if(QOS==0)then
		MQTT_Type = string.char(0x30);
	
	elseif(QOS==1)then
		MQTT_Type = string.char(0x32);
	
	elseif(QOS==2)then
		MQTT_Type = string.char(0x34);
	end;
	local Pro = string.char(0x00,len_Top)..Topic..string.char(0x00,MS_id)..Payload;
	local len = string.char(string.len(Pro)); 
	local MQTT_req = MQTT_Type..len..Pro;
	print("[INFO] MQTT PUBLISH\r\n");
	nx_print.print_payload(MQTT_req);
	nx_queue.socket_insert(MQTT_req);
	--nx_network.network_send(socket_fd,MQTT_req);
end;

function MQTT_Sub(QOS,Topic,len_Top,MS_id,socket_fd)
	local MQTT_Type= string.char(0x82);
	local Pro = string.char(0x00,MS_id)..string.char(0x00,len_Top)..Topic..string.char(QOS);
	local len = string.char(string.len(Pro)); 
	local MQTT_req = MQTT_Type..len..Pro;
	print("[INFO] MQTT SUBSCRIB\r\n");
	nx_print.print_payload(MQTT_req);
	nx_queue.socket_insert(MQTT_req);
	--nx_network.network_send(socket_fd,MQTT_req);
end;

function MQTT_Ping(socket_fd)
--test ping
	local mq_ping = string.char(0xC0,0x00);
	print("[INFO] MQTT PING\r\n");
	nx_print.print_payload(mq_ping);
	nx_queue.socket_insert(mq_ping);
	--nx_network.network_send(socket_fd,mq_ping);
end;
