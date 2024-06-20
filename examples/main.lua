local M = {}

M.name = "some"

local source = Traffic.tcp.new()

source.next = function()
  print("Lua received new connection");
end

return M
