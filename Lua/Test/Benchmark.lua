
local benchmark = Benchmark()
for i = 1, 10000000, 1 do
    local a = 1
    local b = 2
    local c = a + b + a * b
end
print(benchmark())