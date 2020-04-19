//
// Created by maxwell on 2020-04-11.
//

#ifndef METAPARSE_EXPRESSION_H
#define METAPARSE_EXPRESSION_H

#include "boolean.h"
//#include "set.h"
#include "variable.h"
#include "../meta/concepts.h"

#include <type_traits>

namespace pram
{
  inline namespace logic {

    //Outline of class hierarchy
    class ExprNode;
      class RootExprNode;
      class TerminalExprNode;
        class ValueExprNode;
        class NullExprNode;
      class IntermediateExprNode;
        class Operation;
  }

  ENABLE_RTTI(
    ExprNode,
      RTTI_ENUM(Root, Terminal, Intermediate);
      RTTI_DYNAMIC_TYPE(Root, RootExprNode);
      RTTI_DYNAMIC_TYPE(Terminal, TerminalExprNode);
      RTTI_DYNAMIC_TYPE(Intermediate, IntermediateExprNode);
  );

  ENABLE_RTTI(
    TerminalExprNode,
      RTTI_ENUM(Value, Null);
      RTTI_DYNAMIC_TYPE(Value, ValueExprNode);
      RTTI_DYNAMIC_TYPE(Null, NullExprNode);
  );

  ENABLE_RTTI(
    IntermediateExprNode,
      RTTI_ENUM(Operator);
      RTTI_DYNAMIC_TYPE(Operator, logic::Operation);
  );

  inline namespace logic{
    /*constexpr struct dispatchExpr_fn {
      template<meta::Dispatchable NodeType, typename Func, typename... Args>
      constexpr decltype(auto) operator()(const NodeType &node, Func &&func, Args &&... args) const {
        return node.getKind().dispatch(node, std::forward<Func>(func), std::forward<Args>(args)...);
      }
    } dispatchExpr;
    constexpr struct recursiveDispatchExpr_fn {
      template<meta::Dispatchable NodeType, typename Func, typename... Args>
      constexpr decltype(auto) operator()(const NodeType &node, Func &&func, Args &&... args) const {
        return node.getKind().dispatch(node, *this, std::forward<Func>(func), std::forward<Args>(args)...);
      }
      template<typename NodeType, typename Func, typename... Args>
      constexpr decltype(auto) operator()(const NodeType &node, Func &&func, Args &&... args) const {
        return std::forward<Func>(func)(node, std::forward<Args>(args)...);
      }
    } recursiveDispatchExpr;
    namespace {
      constexpr struct multipleDispatchExprSecond_fn {
        template<typename NodeTypeA, typename Func, typename NodeTypeB, typename... Args>
        constexpr decltype(auto) operator()(const NodeTypeA &node_a, Func &&func, const NodeTypeB &node_b, Args &&... args) const {
          return dispatch(node_b, std::forward<Func>(func), node_a, std::forward<Args>(args)...);
        }
      } multipleDispatchExprSecond;
    }
    constexpr struct multipleDispatchExpr_fn {
      template<typename NodeTypeA, typename Func, typename NodeTypeB, typename... Args>
      constexpr decltype(auto) operator()(const NodeTypeA &node_a, Func &&func, const NodeTypeB &node_b, Args &&... args) const {
        return dispatch(node_a, multipleDispatchExprSecond, node_b, std::forward<Func>(func), std::forward<Args>(args)...);
      }
    } multipleDispatchExpr;*/

    /*template<typename ReferenceExprNode>
    struct exprNodeIsA_fn {
      template<meta::base_class_of<ReferenceExprNode> ExprNodeKind>
      constexpr bool operator()(const ExprNodeKind &expr) const {
        return dispatchExpr(expr, *this);
      }
      constexpr bool operator()(const ReferenceExprNode &expr) const {
        return true;
      }
      template<typename ExprNodeKind>
      constexpr bool operator()(const ExprNodeKind &expr) const {
        return false;
      }
    };
    template<typename RefExprNode>
    inline constexpr static exprNodeIsA_fn<RefExprNode> exprNodeIsA{};
    constexpr struct sameOpKindAs_fn {
      template<typename ExprNodeTypeA, typename ExprNodeTypeB>
      constexpr bool operator()(const ExprNodeTypeA &a, const ExprNodeTypeB &b) const {
        return a.getKind() == b.getKind();
      }
    } sameOpKindAs;
*/
    //Functor Objects for dynamic visiting dispatch
    namespace functors {
      constexpr struct {
        template<typename A, typename B>
        constexpr decltype(auto) operator()(const A &a, const B &b) const noexcept(noexcept(a == b)) {
          return a == b;
        }
      } equals;
      constexpr struct {
        template<typename A, typename B>
        constexpr decltype(auto) operator()(const A &a, const B &b) const noexcept(noexcept(a != b)) {
          return a != b;
        }
      } not_equals;
    }



    class ExprNode
    {
  protected:
      using ExprKind = RTTIKind<ExprNode>;

  private:
      using ExprNodePtr = std::shared_ptr<ExprNode>;
      using WeakExprNodePtr = std::weak_ptr<ExprNode>;
      using OptExprNodePtr = std::optional<std::shared_ptr<ExprNode>>;


      const ExprKind exprOpKind;
      const ExprNode* parent;

  public:
      explicit constexpr ExprNode(const ExprNode* parent, ExprKind op) : exprOpKind(op), parent(parent) {}

  

      [[nodiscard]] ExprKind getKind() const {
        return exprOpKind;
      }
    };
    /*const ExprNode::ExprKind ExprNode::RootExpr{ROOT};*/
    class RootExprNode :   public ExprNode {
    public:
      explicit constexpr RootExprNode() : ExprNode(this,ExprKind::Root) {}
    };
    inline constexpr static RootExprNode RootNodeConst{};
    inline constexpr static const RootExprNode* RootNode = &RootNodeConst;
    class TerminalExprNode :          public ExprNode
    {
    protected:
      using TerminalExprKind = RTTIKind<TerminalExprNode>;

    private:
      const TerminalExprKind terminalExprKind;

    public:
      explicit constexpr TerminalExprNode(const ExprNode* parent, TerminalExprKind op) :
        ExprNode(parent, ExprKind::Terminal), terminalExprKind(op) {}

      [[nodiscard]] TerminalExprKind getKind() const {
        return terminalExprKind;
      }
    };
    class IntermediateExprNode :             public ExprNode
    {
    protected:
      using IntermediateExprKind = RTTIKind<IntermediateExprNode>;
    private:
      const IntermediateExprKind intermediateExprKind;

    public:
      explicit constexpr IntermediateExprNode(const ExprNode* parent, IntermediateExprKind op) :
         ExprNode(parent, ExprKind::Intermediate), intermediateExprKind(op) {}

      [[nodiscard]] IntermediateExprKind getKind() const {
        return intermediateExprKind;
      }
    };


    class ValueExprNode :          public TerminalExprNode{
      const Variable* variable;
    public:
      explicit ValueExprNode(const Variable* var, const ExprNode* parent = RootNode) :
        variable(var), TerminalExprNode(parent, TerminalExprKind::Value){}

      [[nodiscard]] const Variable& getValue() const noexcept{
        return *variable;
      }
    };
    class NullExprNode : public TerminalExprNode{
    public:
      explicit constexpr NullExprNode(const ExprNode* parent = RootNode) : TerminalExprNode(parent, TerminalExprKind::Null){}
    };
    inline constexpr static NullExprNode NullNodeConst{};
    inline constexpr static const NullExprNode* NullNode = &NullNodeConst;






  }
}

#endif//METAPARSE_EXPRESSION_H
