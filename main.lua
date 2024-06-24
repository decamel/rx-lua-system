local logger = Logger.new("lua")

local op = Operator.new("gnss")

op.source = Traffic.tcp()

op.routes = { "DSS/1/1", "DSS/1/2" }

op.next = function(message)
  logger:info("Hello from Lua: " .. message)
end



return op;
