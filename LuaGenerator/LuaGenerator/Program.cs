using CppSharp;
using CppSharp.AST;
using CppSharp.Generators;
using System.Collections.Generic;

namespace LuaGenerator
{
    class Program
    {
        private class SampleLibrary : ILibrary
        {

            public void Postprocess(Driver driver, ASTContext ctx)
            {
                LuaClassRegister luaClassRegister = new LuaClassRegister(ctx, new List<string> { "A", "B"});
            }

            public void Preprocess(Driver driver, ASTContext ctx)
            {
            }

            public void Setup(Driver driver)
            {
                string source = System.IO.Path.GetFullPath("../../../Source");
                var options = driver.Options;
                options.GeneratorKind = GeneratorKind.CSharp;
                options.Quiet = true;
                options.DryRun = true;
                var module = options.AddModule("Sample");
                module.IncludeDirs.Add(source);
                module.Headers.Add($@"{source}/A.h");
                module.Headers.Add($@"{source}/B.h");
            }

            public void SetupPasses(Driver driver)
            {
            }
        }

        static void Main(string[] args)
        {
            ConsoleDriver.Run(new SampleLibrary());
        }
    }

}
