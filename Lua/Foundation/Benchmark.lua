--[[
local benchmark = Benchmark()
benchmark:stop():print()
print(benchmark())
--]]

return function()
    local start = os.clock()
    return function()
        return os.clock() - start
    end
end
