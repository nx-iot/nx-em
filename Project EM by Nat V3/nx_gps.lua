module(..., package.seeall)
function show_gps_mode_string()
  mode=gps.gpsgetmode();
  
  if(mode == 1) then
    print("---current mode is standalone!---\r\n");
  elseif(mode == 2) then
    print("---current mode is MSB!---\r\n");
  elseif(mode == 3) then
    print("---current mode is MSA!---\r\n");
  end;  
  return;
end;

function show_gps_ssl_string()
  ssl=gps.gpsgetssl();

  if(ssl == 0) then
    print("---current ssl is enable!---\r\n");
  elseif(ssl == 1) then
    print("---current ssl is disable!---\r\n");
  end;  
  return;
end ;

function show_gps_url_string()
  url=gps.gpsgeturl();
  
  if(url) then
    print("---current url is ", url, "!---\r\n");
  else
    print("---current url is nil!\r\n");
  end;  
  return;
end ;

function A_GPS()
	print("---------------------GPS begin test---------------------\r\n");
	g_mode = gps.gpsgetmode();
	if(g_mode ~= 3) then
		s_mode = gps.gpssetmode(3);
		if(not s_mode) then
			print("---GPS Mode setting error!---\r\n");
			return;
		end;
	end;  
	show_gps_mode_string();

	vmsleep(1000);
	g_url = gps.gpsgeturl();
	equal = string.equal(g_url, "supl.google.com:3425", 1);
	if(not equal) then  
		gs_url = gps.gpsseturl("supl.google.com:3425");
	end;
	show_gps_url_string();

	vmsleep(1000);
	g_ssl = gps.gpsgetssl();
	if(g_ssl ~= 0) then
		s_ssl = gps.gpssetssl(0);
		print(s_ssl,"\r\n");
	end; 
	s_ssl = gps.gpssetssl(0);
	--print(s_ssl,"\r\n");
	show_gps_ssl_string();

	vmsleep(1000);
	start = gps.gpsstart(2)
	if(start == true) then
		print("--------------GPS costart success!--------------\r\n")
	end;
	
end;


function S_GPS_Init()
	g_mode = gps.gpsgetmode();
	if(g_mode ~= 1) then
		s_mode = gps.gpssetmode(1);
		if(not s_mode) then
			print("---GPS Mode setting error!---\r\n");
			return;
		end;
	end;  
	show_gps_mode_string();
	vmsleep(1000);
	start = gps.gpsstart(2)
	if(start == true) then
		print("--------------GPS coldstart success!--------------\r\n")
	end;
end;

function GPS_Get_Base()
	cmd = "AT+CASSISTLOC=1\r\n";
	nx_queue.sio_insert(cmd)
	--sio.send(cmd);
	--rsp = sio.recv(5000);
	--print(rsp);
	--return rsp;
end;

function GPS_Get_Locat_Base()
	cmd = "AT+CPSI?\r\n";
	nx_queue.sio_insert(cmd)
	--sio.send(cmd);
	--rsp = sio.recv(5000)
	--print(rsp)
	--return rsp;
end;

function GPS_INFO()
	cmd = "AT+CGPSINFO\r\n";
	nx_queue.sio_insert(cmd)
	--sio.send(cmd);
	--rsp = sio.recv(5000)
	--print(rsp)
	--return rsp;
end;