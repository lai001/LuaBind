local scriptCurrentDir = debug.getinfo(1, "S").short_src:match("(.*/)"):sub(1, -2)
package.path = package.path .. string.format(";%s/?.lua", scriptCurrentDir)
package.path = package.path .. string.format(";%s/?.lua", scriptCurrentDir .. "/ThirdParty/inspect.lua")

inspect = require("inspect")
require("Foundation.Foundation")
-- require("Test.Class")
-- require("Test.Benchmark")
-- require("Test.Class1")
require("Main")
