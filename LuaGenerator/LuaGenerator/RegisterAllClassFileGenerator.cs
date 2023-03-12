using CppSharp.AST;
using System;
using System.Collections.Generic;
using System.Text;

namespace LuaGenerator
{
    class RegisterAllClassFileGenerator
    {
        private List<RegisterSourceFileGenerator> registerSourceFileGenerators;

        private string outputPath;

        public RegisterAllClassFileGenerator(List<RegisterSourceFileGenerator> registerSourceFileGenerators, string outputPath)
        {
            this.outputPath = outputPath;
            this.registerSourceFileGenerators = registerSourceFileGenerators;
        }

        private string getHeaderFileName()
        {
            return "AllClassRegister.h";
        }

        private string getSourceFileName()
        {
            return "AllClassRegister.cpp";
        }

        private string getHeaderFileContent()
        {
            string content = @$"#pragma once
extern ""C""
{{
# include <lua.h>
}}
extern ""C""
{{
    void registerAllClass(lua_State * state);
}}
            ";
            return content;
        }

        private string getSourceFileContent()
        {
            string a = "";
            string b = "";

            foreach (RegisterSourceFileGenerator registerSourceFileGenerator in registerSourceFileGenerators)
            {
                a += registerSourceFileGenerator.getRegisterClassCallerExternContent() + "\n";
                b += registerSourceFileGenerator.getRegisterClassCallerContent() + "\n";
            }

            string content = @$"#include ""{getHeaderFileName()}""
{a}
void registerAllClass(lua_State * state)
{{
    {b}
}}
            ";
            return content;
        }

        public void save()
        {
            System.IO.Directory.CreateDirectory(outputPath);

            System.IO.File.WriteAllText(outputPath + "/" + getHeaderFileName(), getHeaderFileContent());
            System.IO.File.WriteAllText(outputPath + "/" + getSourceFileName(), getSourceFileContent());
        }

    }
}
