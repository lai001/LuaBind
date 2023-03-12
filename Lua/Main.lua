print("begin--------------------")

for i = 1, 1, 1 do
    local a = A(10012, 200)
    local b = B()
    -- print(inspect(a))
    a.b = b
    print(a.data)
    print(a:mf_const(100, 200))
    print(a:mf1(100, 200))
    print(a:mf2(100, 200))
end

print("end----------------------")
