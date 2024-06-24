local logger = Logger.new("lua")
local publish = Mqtt.new("events/vessel/heading")

local parser = Nmea.new()

local op = Operator.new("gnss")

op.routes = { "DSS/1/1", "DSS/1/2" }

op.next = function(message)
  local sentence = parse(message)

  local heading = sentence:field(0)

  local payload = Response.new("heading")
  payload.value = heading

  logger:info("Hello from Lua: " .. message)
  publish(payload)
end



return op;
