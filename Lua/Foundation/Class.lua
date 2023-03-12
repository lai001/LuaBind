--[[
local TestClass = Class(function(self, ...)
    local arg = {...}
    self.id = arg[1]
    self.name = arg[2]
    local privateMember = 0

    local function privateFunction()
        privateMember = privateMember + 1
    end

    function self:getPrivateMember()
        return privateMember
    end

    function self:incPrivateMember()
        privateFunction()
    end

    function self:print()
        print("TestClass", "id: ", self.id, "name: ", self.name, "privateMember: ", privateMember)
    end
end)

local SubTestClass = Class(TestClass, function(self, ...)
    function self:incID()
        self.id = self.id + 1
    end

    function self:print()
        self.super:print()
        print("SubTestClass...")
    end
end, function(self)
    local privateStaticMember = 0

    function self:setStaticMember(value)
        privateStaticMember = value
    end

    function self:getStaticMember(value)
        return privateStaticMember
    end
end)

local SubTestClass2 = Class(SubTestClass, function(self)

end)

local instance1 = TestClass(1, "name1")
local instance2 = TestClass(2, "name2")
local instance3 = SubTestClass(3, "name3")
local instance4 = SubTestClass(4, "name4")
local instance5 = SubTestClass2(5, "name5")

assert(instance1.id == 1)
assert(instance2.name == "name2")
assert(instance3.id == 3)
assert(instance4.name == "name4")
assert(instance5.name == "name5")
instance4:incID()
assert(instance4.id == 5)
instance1:incPrivateMember()
assert(instance1:getPrivateMember() == 1)
instance5:incID()
assert(instance5.id == 6)
instance5:incPrivateMember()
assert(instance5:getPrivateMember() == 1)
--]]

return function(...)
    local arg = {...}
    local parentClass = nil
    local createClass = nil
    local static = nil
    if type(arg[1]) == "table" then
        parentClass = arg[1]
        createClass = arg[2]
        if arg[3] ~= nil and type(arg[3]) == "function" then
            static = arg[3]
        end
    elseif type(arg[1]) == "function" then
        createClass = arg[1]
        if arg[2] ~= nil and type(arg[2]) == "function" then
            static = arg[2]
        end
    else
        assert(false)
    end
    if parentClass == nil then
        local classTemplate = {
            __call = function(self, ...)
                local class = {}
                createClass(class, ...)
                return class
            end
        }
        if static ~= nil then
            static(classTemplate)
        end
        return setmetatable(classTemplate, classTemplate)
    else
        local class = {
            __call = function(self, ...)
                local class = {}
                createClass(class, ...)
                class.super = parentClass(...)
                return setmetatable(class, {
                    __index = class.super
                })
            end
        }
        if static ~= nil then
            static(class)
        end
        return setmetatable(class, class)
    end
end
