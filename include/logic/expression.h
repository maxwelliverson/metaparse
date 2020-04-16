//
// Created by maxwell on 2020-04-11.
//

#ifndef METAPARSE_EXPRESSION_H
#define METAPARSE_EXPRESSION_H

#include "boolean.h"
#include "set.h"
#include "variable.h"
#include "meta/concepts.h"

#include <type_traits>

namespace pram
{
    inline namespace logic {
        template <typename T>
        struct NodeConstants;


        class ExprNode;

        class ValueNode;

        class NullNode;

        class TerminalExprNode;

        class UnaryExprNode;

        class BinaryExprNode;

        class TernaryExprNode;

        class VariadicExprNode;

        class RootExprNode;

        template<typename T>
        concept HasCustomRTTI = requires(T &&t){
            t.getKind();
        };
        template<typename T>
        concept Dispatchable = HasCustomRTTI<T> && requires(T &&t){
            t.getKind().dispatch();
        };

        constexpr struct dispatchExpr_fn {
            template<Dispatchable NodeType, typename Func, typename ...Args>
            constexpr decltype(auto) operator()(const NodeType &node, Func &&func, Args &&...args) const {
                return node.getKind().dispatch(node, std::forward<Func>(func), std::forward<Args>(args)...);
            }
        } dispatchExpr;

        constexpr struct recursiveDispatchExpr_fn {
            template<Dispatchable NodeType, typename Func, typename ...Args>
            constexpr decltype(auto) operator()(const NodeType &node, Func &&func, Args &&...args) const {
                return node.getKind().dispatch(node, *this, std::forward<Func>(func), std::forward<Args>(args)...);
            }

            template<typename NodeType, typename Func, typename ...Args>
            constexpr decltype(auto) operator()(const NodeType &node, Func &&func, Args &&...args) const {
                return std::forward<Func>(func)(node, std::forward<Args>(args)...);
            }
        } recursiveDispatchExpr;
        namespace {
            constexpr struct multipleDispatchExprSecond_fn {
                template<typename NodeTypeA, typename Func, typename NodeTypeB, typename... Args>
                constexpr decltype(auto)
                operator()(const NodeTypeA &node_a, Func &&func, const NodeTypeB &node_b, Args &&... args) const {
                    return dispatch(node_b, std::forward<Func>(func), node_a, std::forward<Args>(args)...);
                }
            } multipleDispatchExprSecond;
        }
        constexpr struct multipleDispatchExpr_fn {
            template<typename NodeTypeA, typename Func, typename NodeTypeB, typename ...Args>
            constexpr decltype(auto)
            operator()(const NodeTypeA &node_a, Func &&func, const NodeTypeB &node_b, Args &&...args) const {
                return dispatch(node_a, multipleDispatchExprSecond, node_b, std::forward<Func>(func),
                                std::forward<Args>(args)...);
            }
        } multipleDispatchExpr;

        template<typename ReferenceExprNode>
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
                return a.getKind().getKind() == b.getKind().getKind();
            }
        } sameOpKindAs;

        //Functor Objects for dynamic visiting dispatch
        namespace {
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


        class ExprNode {
        protected:
            class ExprOp {
                friend class ExprNode;
                friend struct NodeConstants<ExprNode>;

            protected:
                enum ExprKind {
                    ROOT,
                    TERMINAL,
                    UNARY,
                    BINARY,
                    TERNARY,
                    VARIADIC
                };
            private:
                const ExprKind exprKind;
            public:
                constexpr ExprOp(ExprKind opKind) : exprKind(opKind) {}

                [[nodiscard]] ExprKind getKind() const {
                    return exprKind;
                }

                template<typename Func, typename ...Args>
                constexpr decltype(auto) dispatch(const ExprNode &node, Func &&func, Args &&...args) const {
                    switch (exprKind) {
                        case ROOT:
                            return std::forward<Func>(func)(reinterpret_cast<const RootExprNode &>(*this),
                                                            std::forward<Args>(args)...);

                        case TERMINAL:
                            return std::forward<Func>(func)(reinterpret_cast<const TerminalExprNode &>(*this),
                                                            std::forward<Args>(args)...);

                        case UNARY:
                            return std::forward<Func>(func)(reinterpret_cast<const UnaryExprNode &>(*this),
                                                            std::forward<Args>(args)...);

                        case BINARY:
                            return std::forward<Func>(func)(reinterpret_cast<const BinaryExprNode &>(*this),
                                                            std::forward<Args>(args)...);

                        case TERNARY:
                            return std::forward<Func>(func)(reinterpret_cast<const TernaryExprNode &>(*this),
                                                            std::forward<Args>(args)...);

                        case VARIADIC:
                            return std::forward<Func>(func)(reinterpret_cast<const VariadicExprNode &>(*this),
                                                            std::forward<Args>(args)...);
                    }
                }
            };
        friend struct NodeConstants<ExprNode>;


        private:

            using ExprNodePtr = std::shared_ptr<ExprNode>;
            using WeakExprNodePtr = std::weak_ptr<ExprNode>;
            using OptExprNodePtr = std::optional<std::shared_ptr<ExprNode>>;

            const ExprOp &exprOpKind;

        protected:
            explicit constexpr ExprNode(const ExprOp &op) : exprOpKind(op) {}

        public:
            [[nodiscard]] constexpr bool sameKindAs(const ExprNode &other) const {
                return recursiveDispatchExpr(other, sameOpKindAs, *this);
            }

            const ExprOp &getKind() const {
                return exprOpKind;
            }
        };

        template <>
        struct NodeConstants<ExprNode>
        {
            constexpr static ExprNode::ExprOp RootExprOp = {ExprNode::ExprOp::ROOT};
            //constexpr static ExprNode RootExpr{RootExprOp};
        };
        constexpr struct RootExprNode : public ExprNode {
            explicit constexpr RootExprNode() : ExprNode(NodeConstants<ExprNode>::RootExprOp) {}
        } RootNode{};


        class TerminalExprNode : public ExprNode {
            friend struct NodeConstants<TerminalExprNode>;
        protected:
            class TerminalExprOp : public ExprOp {
                friend struct NodeConstants<TerminalExprNode>;
            protected:
                enum TerminalKind {
                    VALUE,
                    NIL
                };
            private:
                const TerminalKind terminalKind;

                static ExprKind getBaseKind(TerminalKind) { return TERMINAL; }

            public:
                constexpr TerminalExprOp(TerminalKind terminalKind) :
                        ExprOp(TERMINAL),
                        terminalKind(terminalKind) {}

                [[nodiscard]] TerminalKind getKind() const {
                    return terminalKind;
                }

                template<typename Func, typename ...Args>
                constexpr decltype(auto) dispatch(const TerminalExprNode &node, Func &&func, Args &&...args) const {
                    using Op = TerminalExprNode::TerminalExprOp;

                    switch (terminalKind) {
                        case Op::VALUE:
                            return std::forward<Func>(func)(reinterpret_cast<const ValueNode &>(node),
                                                            std::forward<Args>(args)...);
                        case Op::NIL:
                            return std::forward<Func>(func)(reinterpret_cast<const NullNode &>(node),
                                                            std::forward<Args>(args)...);
                    }
                }
            };

        protected:
            explicit TerminalExprNode(const TerminalExprOp &op) :
                    ExprNode(op) {}

            [[nodiscard]] const TerminalExprOp &getKind() const {
                return reinterpret_cast<const TerminalExprOp &>(ExprNode::getKind());
            }

        public:

        };

        class UnaryExprNode : public ExprNode {
            friend struct NodeConstants<UnaryExprNode>;
            class UnaryExprOp : public ExprOp {
                friend struct NodeConstants<UnaryExprNode>;
                enum {
                    IDENTITY,
                    NOT
                };

            };


        };

        class BinaryExprNode : public ExprNode {
        };

        class TernaryExprNode : public ExprNode {
        };

        class VariadicExprNode : public ExprNode {
        };

    }
}

#endif//METAPARSE_EXPRESSION_H
