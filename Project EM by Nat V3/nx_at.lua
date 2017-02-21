module(..., package.seeall)

function at_cmd()
	
	cmd = "AT+cfun=1\r\n";
	print(cmd);
	sio.clear()  
	sio.send(cmd)
	rsp = sio.recv(5000)
	print(rsp)
	vmsleep(8000);
	
	cmd = "ATE0\r\n"
	print(cmd);
	sio.clear() 
	sio.send(cmd)
	--receive response with 5000 ms time out
	rsp = sio.recv(5000)  
	print(rsp)

	cmd = "ATI\r\n";
	print(cmd);
	--clear sio recv cache
	sio.clear()  
	sio.send(cmd)
	rsp = sio.recv(5000)
	print(rsp)

	cmd = "AT+CREG?\r\n";
	print(cmd);
	sio.clear()  
	sio.send(cmd)
	rsp = sio.recv(5000)
	print(rsp)

	cmd = "AT+CSPN?\r\n";
	print(cmd);
	sio.clear()  
	sio.send(cmd)
	rsp = sio.recv(5000)
	print(rsp)

	cmd = "AT+CSQ\r\n";
	print(cmd);
	sio.clear()  
	sio.send(cmd)
	rsp = sio.recv(5000)
	print(rsp)

	cmd = "AT+COPS?\r\n";
	print(cmd);
	sio.clear()  
	sio.send(cmd)
	rsp = sio.recv(5000)
	print(rsp)


	cmd = "AT+CPIN?\r\n";
	print(cmd);
	sio.clear()  
	sio.send(cmd)
	rsp = sio.recv(5000)
	print(rsp)

	cmd = "AT+CTCPKA=0,120,10\r\n";
	print(cmd);
	sio.clear()  
	sio.send(cmd)
	rsp = sio.recv(5000)
	print(rsp)

	cmd = "AT+CTCPKA?\r\n";
	sio.clear()  
	sio.send(cmd)
	rsp = sio.recv(5000)
	print(rsp)
	
	cmd = "AT+CPSI?\r\n";
	print(cmd);
	sio.clear()  
	sio.send(cmd)
	rsp = sio.recv(5000)
	print(rsp)

	--start init GSM Celler  Location 
	print("=====start init GSM Celler  Location====\r\n");

	cmd = "AT+CASSISTLOCTRYTIMES?\r\n";
	print(cmd);
	sio.clear()  
	sio.send(cmd)
	rsp = sio.recv(5000)
	print(rsp)
	
	
	cmd = "AT+CASSISTLOCFORMAT?\r\n";
	print(cmd);
	sio.clear()  
	sio.send(cmd)
	rsp = sio.recv(5000)
	print(rsp)
	
	
	cmd = "AT+CASSISTLOCMODE?\r\n";
	print(cmd);
	sio.clear()  
	sio.send(cmd)
	rsp = sio.recv(5000)
	print(rsp)

	print("==end init GSM Celler  Location===\r\n");

	print("[GSM INFO] GSM ready...\r\n");
end;

