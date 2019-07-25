#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

clang::Rewriter rewriter;
int numFunctions = 0;

class ExampleVisitor : public clang::RecursiveASTVisitor<ExampleVisitor> {
   private:
     clang::ASTContext *astContext;  // used for getting additional AST info

   public:
    explicit ExampleVisitor(clang::CompilerInstance *CI)
        : astContext(&(CI->getASTContext()))  // initialize private members
    {
        rewriter.setSourceMgr(astContext->getSourceManager(),
                              astContext->getLangOpts());
    }

    virtual bool VisitFunctionDecl(clang::FunctionDecl *func) {
        numFunctions++;
        std::string funcName = func->getNameInfo().getName().getAsString();
        if (funcName == "do_math") {
            rewriter.ReplaceText(func->getLocation(), funcName.length(),
                                 "add5");
            llvm::errs() << "** Rewrote function def: " << funcName << "\n";
        }
        return true;
    }

    virtual bool VisitStmt(clang::Stmt *st) {
        if (clang::ReturnStmt * ret = llvm::dyn_cast<clang::ReturnStmt>(st)) {
            rewriter.ReplaceText(ret->getRetValue()->getLocStart(), 6, "val");
            llvm::errs() << "** Rewrote ReturnStmt\n";
        }
        if (clang::CallExpr * call = llvm::dyn_cast<clang::CallExpr>(st)) {
            rewriter.ReplaceText(call->getLocStart(), 7, "add5");
            llvm::errs() << "** Rewrote function call\n";
        }
        return true;
    }

    /*
        virtual bool VisitReturnStmt(ReturnStmt *ret) {
            rewriter.ReplaceText(ret->getRetValue()->getLocStart(), 6, "val");
            errs() << "** Rewrote ReturnStmt\n";
            return true;
        }

        virtual bool VisitCallExpr(CallExpr *call) {
            rewriter.ReplaceText(call->getLocStart(), 7, "add5");
            errs() << "** Rewrote function call\n";
            return true;
        }
    */
};

class ExampleASTConsumer : public clang::ASTConsumer {
   private:
    ExampleVisitor *visitor;  // doesn't have to be private

   public:
    // override the constructor in order to pass CI
    explicit ExampleASTConsumer(clang::CompilerInstance * CI)
        : visitor(new ExampleVisitor(CI))  // initialize the visitor
    {}

    // override this to call our ExampleVisitor on the entire source file
    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        /* we can use ASTContext to get the TranslationUnitDecl, which is
             a single Decl that collectively represents the entire source file
         */
        visitor->TraverseDecl(Context.getTranslationUnitDecl());
    }

    /*
        // override this to call our ExampleVisitor on each top-level Decl
        virtual bool HandleTopLevelDecl(DeclGroupRef DG) {
            // a DeclGroupRef may have multiple Decls, so we iterate through
       each one for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i !=
       e; i++) { Decl *D = *i; visitor->TraverseDecl(D); // recursively visit
       each AST node in Decl "D"
            }
            return true;
        }
    */
};

class ExampleFrontendAction : public clang::ASTFrontendAction {
   public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI,
                                                                  llvm::StringRef file) {
        return std::unique_ptr<clang::ASTConsumer>(
            new ExampleASTConsumer(&CI));  // pass CI pointer to ASTConsumer
    }
};

int main(int argc, const char **argv) {
    llvm::cl::OptionCategory category("LibToolingExample options");
    // parse the command-line args passed to your code
    clang::tooling::CommonOptionsParser op(argc, argv, category);
    // create a new Clang Tool instance (a LibTooling environment)
    clang::tooling::ClangTool Tool(op.getCompilations(), op.getSourcePathList());

    // run the Clang Tool, creating a new FrontendAction (explained below)
    int result =
        Tool.run(clang::tooling::newFrontendActionFactory<ExampleFrontendAction>().get());

    llvm::errs() << "\nFound " << numFunctions << " functions.\n\n";
    // print out the rewritten source code ("rewriter" is a global var.)
    rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID())
        .write(llvm::errs());
    return result;
}
