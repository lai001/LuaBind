using CppSharp.AST;
using System;
using System.Collections.Generic;

namespace LuaGenerator
{
    class LuaClassRegister
    {
        private ASTContext ctx;

        public LuaClassRegister(ASTContext ctx, List<string> classNames)
        {
            string ouputPath = "../Register";
            this.ctx = ctx;
            List<Class> classes = findClasses(classNames);
            List<RegisterSourceFileGenerator> registerSourceFileGenerators = new List<RegisterSourceFileGenerator>();
            RegisterAllClassFileGenerator registerAllClassFileGenerator = new RegisterAllClassFileGenerator(registerSourceFileGenerators, ouputPath);
            foreach (Class @class in classes)
            {
                RegisterSourceFileGenerator registerSourceFileGenerator = new RegisterSourceFileGenerator(ctx, @class, ouputPath);
                registerSourceFileGenerator.save();
                registerSourceFileGenerators.Add(registerSourceFileGenerator);
            }
            registerAllClassFileGenerator.save();
        }

        private List<Class> findClasses(List<string> classNames)
        {
            List<Class> classes = new List<Class>();
            foreach (string className in classNames)
            {
                classes.AddRange(findClassE(ctx, className));
            }
            return classes;
        }

        private List<Class> findClassE(ASTContext ctx, string name)
        {
            List<Class> classes = new List<Class>();
            Action<DeclarationContext> walk = delegate (DeclarationContext declarationContext) { };

            walk = delegate (DeclarationContext declarationContext)
            {
                // Console.WriteLine(declarationContext.Name);

                Class @class = declarationContext.FindClass(name);
                if (@class != null)
                {
                    classes.Add(@class);
                }
                foreach (var item in declarationContext.Namespaces)
                {
                    walk(item);
                }
            };

            foreach (var item in ctx.TranslationUnits)
            {
                if (item.IsSystemHeader == false)
                {
                    walk(item);
                }
            }

            return classes;
        }

    }
}
