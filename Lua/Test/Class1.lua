local Car = TClass(function(self, ...)

end, function(self)
    function self:print()
        print("Car")
    end
end)

local Car1 = TClass(Car, function(self, ...)
    local arg = {...}
    self.id = arg[1]
    self.name = arg[2]

    local function privateMemberFunc()
        print("privateMemberFunc")
    end
    function self:publicMemberFunc()
        print("privateMemberFunc call by publicMemberFunc")
        privateMemberFunc()
    end
end, function(self)
    function self:print()
        self.super:print()
        print("Car1")
    end
end, function(Self)
    local function privateStaticFunc()
        print("privateStaticFunc")
    end

    function Self:publicStaticFunc()
        print("privateStaticFunc call by publicStaticFunc")
        privateStaticFunc()
    end
end)

local benchmark2 = Benchmark()

for i = 1, 1, 1 do
    local car = Car1(5, "name5")
    print(car.id, car.name)
    Car1:publicStaticFunc()
    car:publicMemberFunc()
    car:print()
end

print(benchmark2())
