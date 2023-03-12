local table = {}

function table:isnil(value)
    return value == nil
end
function table:isnumber(value)
    return type(value) == "number"
end
function table:isstring(value)
    return type(value) == "string"
end
function table:isfunction(value)
    return type(value) == "function"
end
function table:istable(value)
    return type(value) == "table"
end
function table:isfloat(value)
    if self:isnumber(value) then
        return math.type(value) == "float"
    else
        return false
    end
end
function table:isinteger(value)
    if self:isnumber(value) then
        return math.type(value) == "integer"
    else
        return false
    end
end

return table