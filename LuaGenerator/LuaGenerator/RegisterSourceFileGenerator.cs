using CppSharp.AST;
using CppSharp.AST.Extensions;
using CppSharp.Generators;
using CppSharp.Generators.C;
using System;
using System.Collections.Generic;
using Type = CppSharp.AST.Type;

namespace LuaGenerator
{
    class RegisterSourceFileGenerator
    {
        private ASTContext ctx;
        private Class @class;
        private string outputPath;

        public RegisterSourceFileGenerator(ASTContext ctx, Class @class, string outputPath)
        {
            this.outputPath = outputPath;
            this.@class = @class;
            this.ctx = ctx;
        }

        public string getSourceFileContent()
        {
            string ret = @$"
{getIncludeContent()}
{getClassMemberFunctionStructContent()}
{getClassPropertyStructContent()}
{getCreateClassInstanceLuaTableWrapperFunctionContent()}
{getClassConstructorFunctionContent()}
{getRegisterClassFunctionContent()}";
            return ret;
        }

        public string getIncludeContent()
        {
            string className = @class.Name;
            string headerFilePath = @class.TranslationUnit.IncludePath;
            string ret = @$"
// #include ""Class{className}Register.h""

extern ""C""
{{
# include <lua.h>
# include <lauxlib.h>
# include <lualib.h>
}}

#include ""{headerFilePath}""
#include ""RegisterHelper.h""
                ";

            return ret;
        }

        public string getRegisterClassFunctionContent()
        {
            string className = @class.Name;
            string ret = $@"
void registerClass{className}(lua_State *state)
{{
    lua_pushcclosure(state, Class{className}Constructor, 0);
    lua_setglobal(state, ""{className}"");
}}
";
            return ret;
        }

        public string getRegisterClassCallerExternContent()
        {
            string className = @class.Name;
            string ret = $@"extern void registerClass{className}(lua_State * state);";
            return ret;
        }

        public string getRegisterClassCallerContent()
        {
            string className = @class.Name;
            string ret = $@"registerClass{className}(state);";
            return ret;
        }

        public string getClassConstructorFunctionContent()
        {
            string className = @class.Name;
            string content = "";
            foreach (Method method in getSupportContructorMethod())
            {
                int count = method.Parameters.Count;
                if (count == 0)
                {
                    content += $@"
if (numberParameter == 0)
{{
    {className} * instance = new {className}();
    createClass{className}InstanceLuaTableWrapper(state, instance);
    return 1;
}}";
                }
                else
                {
                    string parametersCodeLine = "";
                    string vlist = getVList(method.Parameters.Count, 0);

                    for (int i = 0; i < method.Parameters.Count; i++)
                    {
                        Parameter parameter = method.Parameters[i];
                        parametersCodeLine += getParameterCodeLine(parameter, i) + "\n"; ;
                    }

                    content += $@"
if (numberParameter == {count})
{{
    {parametersCodeLine}
    lua_pop(state, numberParameter);
    {className} * instance = new {className}({vlist});
    createClass{className}InstanceLuaTableWrapper(state, instance);
    return 1;
}}";
                }


            }

            string ret = $@"
int Class{className}Constructor(lua_State *state)
{{
    const int numberParameter = lua_gettop(state);
{content}
    return 0;
}}
";
            return ret;
        }

        public string getCreateClassInstanceLuaTableWrapperFunctionContent()
        {
            string className = @class.Name;
            string functionList = "";

            foreach (Method method in getSupportMemberMethod())
            {
                functionList += $@"
RegisterHelper::addCFunction(state, ""{method.LogicalOriginalName}"", Class{className}MemberFunction::{method.LogicalOriginalName});";
            }

            string ret = $@"
void createClass{className}InstanceLuaTableWrapper(lua_State *state, {className} *instance)
{{
    RegisterHelper::createDefaultClassInstanceLuaTableWrapper<{className}>(state, ""{className}"", instance);
    RegisterHelper::addCFunction(state, ""__index"", Class{className}Property::get);
    RegisterHelper::addCFunction(state, ""__newindex"", Class{className}Property::set);
    {functionList}
}}
";
            return ret;
        }

        public string getClassPropertyStructContent()
        {
            string className = @class.Name;
            string getContent = "";
            string setContent = "";

            foreach (Field field in @class.Fields)
            {
                if (field.Type is PointerType)
                {
                    getContent += $@"
if (name == ""{field.LogicalOriginalName}"")
{{ 
    //void* ptr = reinterpret_cast<void*>(instance->{field.LogicalOriginalName});
    //lua_pushlightuserdata(state, ptr);
    //return 1;
}}";
                    setContent += $@"
if (name == ""{field.LogicalOriginalName}"")
{{ 
    lua_pushstring(state, ""{field.LogicalOriginalName}"");
    lua_pushvalue(state, 3);
    lua_rawset(state, 1);
}}";
                }
                else
                {
                    //TypePrinterResult retType = field.Type.Visit(cppTypePrinter);
                    string typeName = $"{field.Type}";
                    if (typeName == "string")
                    {
                        typeName = "std::string";
                    }
                    string getCodeLine = "";
                    string setCodeLine = "";

                    if (typeName == "float" || typeName == "double")
                    {
                        getCodeLine = $@"lua_pushnumber(state, value);";
                        setCodeLine = $@"{typeName} value = lua_tonumber(state, 3);";
                    }
                    else if (typeName == "std::string")
                    {
                        getCodeLine = $@"lua_pushstring(state, value.c_str());";
                        setCodeLine = $@"{typeName} value = lua_tostring(state, 3);";
                    }
                    else if (typeName == "int")
                    {
                        getCodeLine = $@"lua_pushinteger(state, value);";
                        setCodeLine = $@"{typeName} value = lua_tointeger(state, 3);";
                    }

                    getContent += $@"
if (name == ""{field.LogicalOriginalName}"")
{{ 
    {typeName} value = instance->{field.LogicalOriginalName};
    {getCodeLine}
    return 1;
}}";
                    setContent += $@"
if (name == ""{field.LogicalOriginalName}"")
{{ 
    {setCodeLine}
    instance->{field.LogicalOriginalName} = value;
}}";
                }
            }

            return $@"
struct Class{className}Property
{{
    static int get(lua_State *state)
    {{
        {className} *instance = RegisterHelper::getClassInstance<{className}>(state);
        const std::string name = lua_tostring(state, 2);
        {getContent}
        return 0;
    }}

    static int set(lua_State *state)
    {{
        {className} *instance = RegisterHelper::getClassInstance<{className}>(state);
        const std::string name = lua_tostring(state, 2);
        {setContent}
        return 0;
    }}
}};
";
        }

        public string getClassMemberFunctionStructContent()
        {
            string className = @class.Name;
            string content = "";
            foreach (Method method in getSupportMemberMethod())
            {
                content += MemberFunctionGenerator.get(@class, method);
            }
            return $@"
struct Class{className}MemberFunction
{{
    {content}
}};
";
        }

        private string getParameterCodeLine(Parameter parameter, int index)
        {
            string luaFunction = "";
            if (parameter.Type.ToString() == "double")
            {
                luaFunction = "lua_tonumber";
            }
            else if (parameter.Type.ToString() == "int")
            {
                luaFunction = "lua_tointeger";
            }
            else if (parameter.Type.ToString() == "float")
            {
                luaFunction = "lua_tonumber";
            }
            else if (parameter.Type.ToString() == "string")
            {
                luaFunction = "lua_tostring";
            }
            if (parameter.Type.ToString() == "string")
            {
                return $@"std::{parameter.Type} v{index} = {luaFunction}(state, {index + 1});";
            }
            else
            {
                return $@"{parameter.Type} v{index} = {luaFunction}(state, {index + 1});";
            }
        }

        private string getVList(int count, int offset)
        {
            string vlist = "";
            for (int i = 0; i < count; i++)
            {
                vlist += $"v{i + offset}, ";
            }
            if (count > 0)
            {
                vlist = vlist.Remove(vlist.LastIndexOf(", "));
            }
            return vlist;
        }

        private string getFullClassName()
        {
            string full = @class.Name;
            DeclarationContext declarationContext = @class.Namespace;
            while (declarationContext != null)
            {
                full = $"{declarationContext.Name}::" + full;
                declarationContext = declarationContext.Namespace;
            }
            return full;
        }

        List<Method> getSupportContructorMethod()
        {
            List<Method> methods = new List<Method>();
            foreach (Method constructor in @class.Constructors)
            {
                if (constructor.IsConstructor &&
                    constructor.IsDeleted == false &&
                    constructor.IsCopyConstructor == false &&
                    constructor.IsMoveConstructor == false)
                {
                    methods.Add(constructor);
                }
            }
            return methods;
        }

        private List<Method> getSupportMemberMethod()
        {
            List<Method> methods = new List<Method>();
            foreach (Method memberFunction in @class.Methods)
            {
                if (memberFunction.IsConstructor == false
                    && memberFunction.IsDestructor == false
                    && memberFunction.IsCopyConstructor == false
                    && memberFunction.Kind != CXXMethodKind.Operator)
                {
                    methods.Add(memberFunction);
                }
            }
            return methods;
        }

        public void save()
        {
            System.IO.Directory.CreateDirectory(outputPath);

            string className = @class.Name;
            string fileName = $"Class{className}Register.cpp";
            System.IO.File.WriteAllText(outputPath + "/" + fileName, getSourceFileContent());
        }
    }
}
