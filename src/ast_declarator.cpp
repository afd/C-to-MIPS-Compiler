#include "ast/ast_declarator.hpp"
#include "ast/ast_variable.hpp"

DirectDeclarator::DirectDeclarator(NodePtr _decl, NodePtr _list)
    : dirDec(_decl), paramList(_list){};
void DirectDeclarator::pyPrint(std::ostream &os) {
  if (dirDec)
    dirDec->pyPrint(os);
  os << "(";
  if (paramList)
    paramList->pyPrint(os);
  os << "):\n";
}
std::string DirectDeclarator::getType() const { return "dirDec"; }
Declaration::Declaration(std::string *_spec, NodePtr _list)
    : decSpec(*_spec), initDecList(_list) {
  delete _spec;
};
void Declaration::pyPrint(std::ostream &os) {
  if (initDecList) {
    initDecList->pyPrint(os);
  }
}
void Declaration::getDeclaredVarNames(std::vector<std::string> &v) const {
  initDecList->getDeclaredVarNames(v);
}
std::string Declaration::getType() const { return "dec"; }

InitDeclarator::InitDeclarator(NodePtr _d, NodePtr _i)
    : declarator(_d), initializer(_i){};
void InitDeclarator::pyPrint(std::ostream &os) {
  // os << declarator << " " << initializer << "\n";
  if (declarator && initializer) {
    declarator->pyPrint(os);
    os << " = ";
    initializer->pyPrint(os);
  } else {
    declarator->pyPrint(os);
    os << " = 0";
  }
}
void InitDeclarator::getDeclaredVarNames(std::vector<std::string> &v) const {
  std::cerr << declarator->getType() << "\n";
  if (declarator->getType() == "var") {
    std::cerr << "entered var \n";
    std::string name = declarator->getName();
    v.push_back(name);
  }
}