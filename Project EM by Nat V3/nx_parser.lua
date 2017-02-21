module(..., package.seeall)
require "nx_print"

function mqtt_parser(http_rsp,socket_fd)
	len = string.len(http_rsp);
	mqtt_data = {};
	c = 0;
	--mqtt_data[1] = http_rsp;
	if(string.len(http_rsp)~=string.byte(http_rsp,2)+2)then
		while(true)do
			if(string.byte(http_rsp,2)==0)then
				m_len = 2;
			else
				m_len = string.byte(http_rsp,2)+2;
			end;
			local data = string.sub(http_rsp,0,m_len);
			nx_print.print_payload(data);
			if(string.byte(data)==0x20)then
				if(string.byte(data,2)==0x02)then
					if(string.byte(data,4)==0x00)then
						print("[MQTT CALLBACK] MQTT Connect OK\r\n");
					end
				end
			end
			if(string.byte(data)==0xD0)then
				print("[MQTT CALLBACK] MQTT PING ACK OK\r\n");
			end
			if(string.byte(data)==0x90)then
				print("[MQTT CALLBACK] MQTT SUBSCRIB ACK OK\r\n");
			end
			if(string.byte(data)==0x40)then
				if(string.byte(data,2)==0x02)then
					print("[MQTT CALLBACK] MQTT PUBLISH ACK OK\r\n");
					print("[MQTT CALLBACK] MQTT ID : ",string.byte(http_rsp,4),"\r\n");
				end
			end
			if(string.byte(data)==0x32)then
				print("[MQTT CALLBACK] MQTT REMODE OK\r\n");
				local num = string.byte(data,2);
				if(string.byte(data,num +2 )==48)then
					print("[MQTT CALLBACK] LED STATUS : OFF\r\n");
				else
					print("[MQTT CALLBACK] LED STATUS : ON\r\n");
				end;
				mqtt_AckPub(http_rsp,socket_fd);
			end;
			if(string.byte(data)==0x31)then
				print("[MQTT CALLBACK] MQTT REMODE OK\r\n");
				local num = string.byte(data,2);
				
				if(string.byte(data,num +2 )==48)then
					print("[MQTT CALLBACK] LED STATUS : OFF\r\n");
				else
					print("[MQTT CALLBACK] LED STATUS : ON\r\n");
				end;
				mqtt_AckPub(data,socket_fd);
			end;
			
			if(string.len(http_rsp)~=string.byte(http_rsp,2)+2)then
				http_rsp = string.sub(http_rsp,m_len+1);
			else
				c=1;
				break;
			end;
		end;
	end;
	
	if(c==0)then
		nx_print.print_payload(http_rsp);
		if(string.byte(http_rsp)==0x20)then
			if(string.byte(http_rsp,2)==0x02)then
				if(string.byte(http_rsp,4)==0x00)then
					print("[MQTT CALLBACK] MQTT Connect OK\r\n");
				end
			end
		end
		if(string.byte(http_rsp)==0xD0)then
			print("[MQTT CALLBACK] MQTT PING ACK OK\r\n");
		end
		if(string.byte(http_rsp)==0x90)then
			print("[MQTT CALLBACK] MQTT SUBSCRIB ACK OK\r\n");
		end
		if(string.byte(http_rsp)==0x40)then
			if(string.byte(http_rsp,2)==0x02)then
				print("[MQTT CALLBACK] MQTT PUBLISH ACK OK\r\n");
				print("[MQTT CALLBACK] MQTT ID : ",string.byte(http_rsp,4),"\r\n");
			end
		end
		if(string.byte(http_rsp)==0x32)then
			print("[MQTT CALLBACK] MQTT REMODE OK\r\n");
			local num = string.byte(http_rsp,2);
			if(string.byte(http_rsp,num +2 )==48)then
				print("[MQTT CALLBACK] LED STATUS : OFF\r\n");
			else
				print("[MQTT CALLBACK] LED STATUS : ON\r\n");
			end;
				mqtt_AckPub(http_rsp,socket_fd);
		end;

		if(string.byte(http_rsp)==0x31)then
			print("[MQTT CALLBACK] MQTT REMODE OK\r\n");
			local num = string.byte(http_rsp,2);
			if(string.byte(http_rsp,num +2 )==48)then
				print("[MQTT CALLBACK] LED STATUS : OFF\r\n");
			else
				print("[MQTT CALLBACK] LED STATUS : ON\r\n");
			end;
			mqtt_AckPub(http_rsp,socket_fd);
		end;
	end;
end;

function mqtt_AckPub(c,socket_fd)
	local n = string.byte(c,4);
	n = n + 5;
	local value = string.char(0x40,0x02,string.byte(c,n),string.byte(c,n+1));
	nx_print.print_payload(value);
	--socket.select(socket_fd, SOCK_READ_EVENT);--care for read event, IMPORTANT!!! this will let socket notify READ event.
	--local timeout = 30000;--  '< 0' means wait for ever; '0' means not wait; '> 0' is the timeout milliseconds
	--local err_code, sent_len = socket.send(socket_fd, value, timeout);
	--print("socket.send ", err_code, ", ", sent_len, "\r\n");
	--local err_code1, CC = socket.recv(socket_fd, 10);
end;
