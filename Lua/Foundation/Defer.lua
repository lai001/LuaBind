return function(call)
    assert(type(call) == "function")
    local a = {
        __gc = function()
            call()
        end
    }
    return setmetatable(a, a)
end