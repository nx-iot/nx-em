module(..., package.seeall)
function print_payload(buf)
      for byte=1, #buf, 16 do
         local chunk = buf:sub(byte, byte+15)
         print(string.format('%08X  ',byte-1))
         chunk:gsub('.', function (c) print(string.format('%02X ',string.byte(c))) end)
         print(string.rep(' ',3*(16-#chunk)))
         print(' ',chunk:gsub('%c','.'),"\n") 
      end
end;