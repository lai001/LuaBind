using CppSharp.AST;
using System;
using System.Collections.Generic;
using System.Text;

namespace LuaGenerator
{
    class MemberFunctionGenerator
    {
        private class ReturnTypeInfo
        {
            public readonly string returnType = "";
            public readonly string @namespace = "";
            public readonly Parameter returnParameter = null;
            public ReturnTypeInfo(Method method)
            {
                if (method.OriginalReturnType.Type is TypedefType)
                {
                    TypedefType typedefType = method.OriginalReturnType.Type as TypedefType;
                    @namespace = typedefType.Declaration.OriginalNamespace.ToString();
                    returnType = method.OriginalReturnType.ToString();
                }
                else
                {
                    returnType = method.ReturnType.ToString();
                }

                for (int i = 0; i < method.Parameters.Count; i++)
                {
                    Parameter parameter = method.Parameters[i];
                    if (parameter.Kind == ParameterKind.IndirectReturnType)
                    {
                        returnParameter = parameter;
                    }
                }
            }
        }

        public static string get(Class @class, Method method)
        {
            string className = @class.Name;
            string content = "";

            List<Parameter> parameters = new List<Parameter>();

            for (int i = 0; i < method.Parameters.Count; i++)
            {
                Parameter parameter = method.Parameters[i];
                if (parameter.Kind != ParameterKind.IndirectReturnType)
                {
                    parameters.Add(parameter);
                }
            }

            for (int i = 0; i < parameters.Count; i++)
            {
                content += getParameterCodeLine(parameters[i], i + 1) + "\n";
            }
            string vlist = getVList(parameters.Count, 1);
            string ret = $@"
static int {method.LogicalOriginalName}(lua_State *state)
{{
    {className} * instance = RegisterHelper::getClassInstance<{className}>(state);
    {content}
    {getCallMemberFunctionCodeLine(method, vlist)}
    {getPushValueCodeLine(method)}
}}
";
            return ret;
        }

        private static string getParameterCodeLine(Parameter parameter, int index)
        {
            System.Diagnostics.Debug.Assert(parameter.Kind != ParameterKind.IndirectReturnType);
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

        private static string getVList(int count, int offset)
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

        private static string getCallMemberFunctionCodeLine(Method method, string vlist)
        {
            ReturnTypeInfo returnTypeInfo = new ReturnTypeInfo(method);
            string returnType = returnTypeInfo.returnType;
            string @namespace = returnTypeInfo.@namespace;
            string codeLine;
            if (returnType == "void")
            {
                codeLine = $@"instance->{method.LogicalOriginalName}({vlist});";
            }
            else
            {
                codeLine = $@"{@namespace}::{returnType} r = instance->{method.LogicalOriginalName}({vlist});";
            }
            return codeLine;
        }

        private static string getPushValueCodeLine(Method method)
        {
            ReturnTypeInfo returnTypeInfo = new ReturnTypeInfo(method);
            string returnType = returnTypeInfo.returnType;
            string @namespace = returnTypeInfo.@namespace;
            string codeLine = "";

            if (returnType == "void")
            {
                codeLine = "return 0;";
            }
            else if (returnType == "string" && @namespace == "std")
            {
                codeLine = $@"
lua_pushstring(state, r.c_str());
return 1;";
            }
            else if (returnType == "int")
            {
                codeLine = $@"
lua_pushinteger(state, r);
return 1;";
            }
            else if (returnType == "float")
            {
                codeLine = $@"
lua_pushnumber(state, r);
return 1;";
            }
            else if (returnType == "double")
            {
                codeLine = $@"
lua_pushnumber(state, r);
return 1;";
            }
            return codeLine;
        }
    }
}
