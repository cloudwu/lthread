local thread = require "thread"

local STEP = 100 -- default value is 1024

local f = thread(function()
	for i=1, 1000 do
		print(i)
	end
	return "end"
end, STEP)

while true do
	local ok, result = f()
	if ok then
		print(result)
		break
	end
	if ok == false then
		-- throw error from thread f
		print("error", result)
		break
	end
	assert(ok == nil)	-- step thread
	print "step"
end
