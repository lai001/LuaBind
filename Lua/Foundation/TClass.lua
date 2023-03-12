return function(...)
    local argv = {...}
    local parentClassTemplate
    local selfInitClosure
    local selfClosure
    local SelfClosure
    if type(argv[1]) == "table" then
        parentClassTemplate = argv[1]
        selfInitClosure = argv[2]
        selfClosure = argv[3]
        SelfClosure = argv[4]
    else
        selfInitClosure = argv[1]
        selfClosure = argv[2]
        SelfClosure = argv[3]
    end
    local menberMetatable = {}
    if selfClosure ~= nil then
        selfClosure(menberMetatable)
    end
    if parentClassTemplate ~= nil then
        assert(parentClassTemplate._menberMetatable ~= nil)
        local superMetatable = parentClassTemplate._menberMetatable
        setmetatable(menberMetatable, superMetatable)
    end
    local call
    if type(next(menberMetatable)) ~= "nil" then
        call = function(self, ...)
            local classInstance = {}
            if parentClassTemplate ~= nil then
                classInstance.super = parentClassTemplate(...)
            end
            if selfInitClosure ~= nil then
                selfInitClosure(classInstance, ...)
            end
            setmetatable(classInstance, {
                __index = menberMetatable
            })
            return classInstance
        end
    else
        call = function(self, ...)
            local classInstance = {}
            if parentClassTemplate ~= nil then
                classInstance.super = parentClassTemplate(...)
            end
            if selfInitClosure ~= nil then
                selfInitClosure(classInstance, ...)
            end
            return classInstance
        end
    end
    local classTemplate = {
        __call = call,
        _menberMetatable = menberMetatable
    }
    setmetatable(classTemplate, classTemplate)
    if SelfClosure ~= nil then
        SelfClosure(classTemplate)
    end
    return classTemplate
end