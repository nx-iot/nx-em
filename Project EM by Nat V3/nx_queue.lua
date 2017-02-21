module(..., package.seeall)
require "nx_parser"
first = 0
last = -1
data = {}

first2 = 0
last2 = -1
data2 = {}
i = 0;
function socket_insert(val)
  last = last + 1
  data[last] = val
  print("QUEUE last ",last ,"\r\n");
end

function socket_remove()
    if (first > last) then 
      first = 0
      last = -1
      rval = -1
      --print("Nottt \r\n")
    else
      print("remove: data[first]= ", data[first], " first= ",first ,"\r\n")
      local rval = data[first]
      print("remove: rval= ", rval ,"\r\n")
      nx_network.network_send(socket_fd,rval); -- Set_AT Command
      data[first] = nil        -- to allow garbage collection
      first = first + 1
      print("remove: first= ",first ,"\r\n")
    end
    return rval
end


function socket_insert2(val)
  last2 = last2 + 1
  data2[last2] = val
  print("QUEUE last2 ",last2 ,"\r\n");
end

function socket_remove2()
    if (first2 > last2) then 
      first2 = 0
      last2 = -1
      rval2 = -1
      --print("Nottt2 \r\n")
    else
      print("remove: data2[first2]= ", data2[first2], " first2= ",first2 ,"\r\n")
      local rval2 = data2[first2]
      print("remove: rval2= ", rval2 ,"\r\n")
      --nx_print.print_payload(rval2); -- Get_AT Command
      nx_parser.mqtt_parser(rval2,socket_fd);
      data2[first2] = nil        -- to allow garbage collection
      first2 = first2 + 1
      print("remove: first2= ",first2 ,"\r\n")
    end
    return rval2
end

-- Queue SIO -----------
sio_first = 0
sio_last = -1
sio_data = {}

sio_first2 = 0
sio_last2 = -1
sio_data2 = {}
function sio_insert(val)
  sio_last = sio_last + 1
  sio_data[sio_last] = val
  print("QUEUE sio_last ",sio_last ,"\r\n");
end

function sio_remove()
    if (sio_first > sio_last) then 
      sio_first = 0
      sio_last = -1
      sio_rval = -1
      --print("Nottt \r\n")
    else
      print("remove: sio_data[sio_first]= ", sio_data[sio_first], " sio_first= ",sio_first ,"\r\n")
      local sio_rval = sio_data[sio_first]
      print("remove: sio_rval= ", sio_rval ,"\r\n")
      sio.send(sio_rval); -- Set_AT Command
      sio_data[sio_first] = nil        -- to allow garbage collection
      sio_first = sio_first + 1
      print("remove: sio_first= ",sio_first ,"\r\n")
    end
    return sio_rval
end

function sio_insert2(val)
  sio_last2 = sio_last2 + 1
  sio_data2[sio_last2] = val
  print("QUEUE sio_last2 ",sio_last2 ,"\r\n");
end

function sio_remove2()
    if (sio_first2 > sio_last2) then 
      sio_first2 = 0
      sio_last2 = -1
      sio_rval2 = -1
      --print("Nottt2 \r\n")
    else
      print("remove: sio_data2[sio_first2]= ", sio_data2[sio_first2], " sio_first2= ",sio_first2 ,"\r\n")
      local sio_rval2 = sio_data2[sio_first2]
      print("remove: sio_rval2 = ", sio_rval2 ,"\r\n")
      nx_func.MQTT_Pub(sio_rval2,i);
      i = i+1;
      sio_data2[sio_first2] = nil        -- to allow garbage collection
      sio_first2 = sio_first2 + 1
      print("remove: sio_first2= ",sio_first2 ,"\r\n")
    end
    return sio_rval2
end

--insert("a")
--insert("b")
--insert("c")
--for i,v in pairs(data) do
--    print(i,"  ",v,"\r\n")
--end

--while(first <= last)do
--  local x = remove(q)
--  print("list item= ", x ,"\r\n")
--end; 
