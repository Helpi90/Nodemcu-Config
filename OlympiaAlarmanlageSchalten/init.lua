wifi.setmode(wifi.STATION)
wifi.sta.config("SSID","DeinWLANPasswort")
print(wifi.sta.getip())
led0 = 1
led1 = 2
led2 = 3
gpio.mode(led0, gpio.OUTPUT)
gpio.mode(led1, gpio.OUTPUT)
gpio.mode(led2, gpio.OUTPUT)
srv=net.createServer(net.TCP)
srv:listen(80,function(conn)
conn:on("receive", function(client,request)
local buf = "";
local _, _, method, path, vars = string.find(request, "([A-Z]+) (.+)?(.+) HTTP");
if(method == nil)then
_, _, method, path = string.find(request, "([A-Z]+) (.+) HTTP");
end
local _GET = {}
if (vars ~= nil)then
for k, v in string.gmatch(vars, "(%w+)=(%w+)&*") do
_GET[k] = v
end
end
buf = buf.."<HTML><h1> Olympia Protect 9061 WLAN Alarm-Switch</h1>";
buf = buf.."<p>Alarm ausschalten <a href=\"?pin=ON0\"><button>&nbsp&nbsp OK&nbsp&nbsp </button></a></p>";
buf = buf.."<p>Haus-Alarm einschalten <a href=\"?pin=ON1\"><button>&nbsp;&nbspOK </button></a></p>";
buf = buf.."<p>Alarm einschalten <a href=\"?pin=ON2\"><button>&nbsp&nbsp OK&nbsp&nbsp </button></a></p>";
buf = buf.."<p>Lautlos-Alarm einschalten <a href=\"?pin=ON3\"><button>&nbsp&nbsp OK&nbsp&nbsp </button></a></p></HTML>";
local _on,_off = "",""
if(_GET.pin == "ON0")then
gpio.write(led1, gpio.LOW);
tmr.delay(1000000);
gpio.write(led1, gpio.HIGH);
elseif(_GET.pin == "ON1")then
gpio.write(led2, gpio.LOW);
tmr.delay(1000000);
gpio.write(led2, gpio.HIGH);
elseif(_GET.pin == "ON2")then
gpio.write(led0, gpio.LOW);
tmr.delay(1000000);
gpio.write(led0, gpio.HIGH);
elseif(_GET.pin == "ON3")then
gpio.write(led0, gpio.LOW);
gpio.write(led2, gpio.LOW);
tmr.delay(5000000);
gpio.write(led0, gpio.HIGH);
gpio.write(led2, gpio.HIGH);
end
client:send(buf);
client:close();
collectgarbage();
end)
end)
