module(..., package.seeall)


function check_network_dorm_function(app_handle)
  print("check_network_dorm_function\r\n");
  network_state = {};
  network_state.invalid = 0;
  network_state.down = 1;
  network_state.coming_up = 2;
  network_state.up = 4;
  network_state.going_down = 8;
  network_state.resuming = 16;
  network_state.going_null = 32;
  network_state.null = 64;
  local status = network.status(app_handle);
  print("network status = ", status, "\r\n");
  print("enter dormancy now...\r\n");
  network.dorm(app_handle, true);
  vmsleep(1000); 
  status = network.status(app_handle);
  print("network status = ", status, "\r\n");
  print("leave dormancy now...\r\n");
  network.dorm(app_handle, false);
  vmsleep(3000); 
  status = network.status(app_handle);
  print("network status = ", status, "\r\n");
end;

function config_network_common_parameters()
  print("config_network_common_parameters\r\n");
  --The same with AT+CTCPKA
  result = network.set_tcp_ka_param(5, 1);--keep alive parameter, max 5 times, check every 1 minute if socket is idle.
  print("network.set_tcp_ka_param()=", result, "\r\n");
  --The same with AT+CIPCCFG
  result = network.set_tcp_retran_param(10, 10000);--maximum 10 retransmit times, minimum interval is 10 seconds.
  print("network.set_tcp_retran_param()=", result, "\r\n");
  --The same with AT+CIPDNSSET
  result = network.set_dns_timeout_param(0, 30000, 5);--network retry open times = 0(maximum is 3), network open timeout is 30 seconds, maximum DNS query times is 5
  print("network.set_dns_timeout_param()=", result, "\r\n");
end;

function network_open()
  --Following is a sample of changing some common parameters, it is not required.
    config_network_common_parameters();
    print("opening network...\r\n");
    local cid = 1;--0=>use setting of AT+CSOCKSETPN. 1-16=>use self defined cid
    local timeout = 30000;--  '<= 0' means wait for ever; '> 0' is the timeout milliseconds
    app_handle = network.open(cid, timeout);--!!! If the PDP for cid is already opened by other app, this will return a reference to the same PDP context.
    if (not app_handle) then
      print("faield to open network\r\n");
      return app_handle;
    end;
    print("network.open(), app_handle=", app_handle, "\r\n");
    local local_ip_addr = network.local_ip(app_handle);
    print("local ip address is ", local_ip_addr, "\r\n");
    local mtu_value = network.mtu(app_handle);
    print("MTU is ", mtu_value, " bytes\r\n");
    return app_handle;
end;

function network_dns()
  print("resolving DNS address...\r\n");
  local cid = 1;
  ip_address = network.resolve(server_address, cid);
  print("The IP address for ", server_address, " is ", ip_address, "\r\n");
  --check_network_dorm_function(app_handle);
  if(ip_address) then return ip_address
  else return ip_address end
end;

function network_send(socket_fd,MQTT_req)
    --socket.select(socket_fd, SOCK_WRITE_EVENT);
    local timeout = 1500;--  '< 0' means wait for ever; '0' means not wait; '> 0' is the timeout milliseconds
    local err_code, sent_len = socket.send(socket_fd, MQTT_req, timeout);
    if (err_code and (err_code == SOCK_RST_OK)) then

    end;
end;
function network_recv(socket_fd)
  --socket.select(socket_fd, SOCK_READ_EVENT);
  local timeout = 15000;
  local err_code, http_rsp = socket.recv(socket_fd,timeout);--not wait, for old firmware, the timeout should be > 0(for example, timeout=1)
  if (err_code and (err_code == SOCK_RST_OK)) then
        --nx_print.print_payload(http_rsp);
        nx_queue.socket_insert2(http_rsp);
  end;
end;
