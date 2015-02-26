#include "../include/switch_sm.hpp"

#include <gtest/gtest.h>

#include <tuple>

struct Func
{
    template <typename TupleElem>
    void operator()(TupleElem)
    {
        TupleElem::execute();        
    }
};

struct TupleElem1 { static int executed; static void execute() {executed++;} }; int TupleElem1::executed;
struct TupleElem2 { static int executed; static void execute() {executed++;} }; int TupleElem2::executed;
struct TupleElem3 { static int executed; static void execute() {executed++;} }; int TupleElem3::executed;
struct TupleElem4 { static int executed; static void execute() {executed++;} }; int TupleElem4::executed;

typedef std::tuple<TupleElem1, TupleElem2, TupleElem3, TupleElem4> TupleList;

TEST(test_tuple_switch, test_tuple_switch) {
    Func func;
    EXPECT_EQ(TupleElem1::executed, 0);
    EXPECT_EQ(TupleElem2::executed, 0);
    EXPECT_EQ(TupleElem3::executed, 0);
    EXPECT_EQ(TupleElem4::executed, 0);
    detail::TupleSwitch<TupleList>(0, func);
    EXPECT_EQ(TupleElem1::executed, 1);
    detail::TupleSwitch<TupleList>(0, func);
    EXPECT_EQ(TupleElem1::executed, 2);
    detail::TupleSwitch<TupleList>(0, func);
    detail::TupleSwitch<TupleList>(0, func);
    EXPECT_EQ(TupleElem2::executed, 0);
    EXPECT_EQ(TupleElem3::executed, 0);
    EXPECT_EQ(TupleElem4::executed, 0);
    EXPECT_EQ(TupleElem1::executed, 4);
    detail::TupleSwitch<TupleList>(1, func);
    EXPECT_EQ(TupleElem2::executed, 1);
    detail::TupleSwitch<TupleList>(2, func);
    EXPECT_EQ(TupleElem3::executed, 1);
    detail::TupleSwitch<TupleList>(3, func);
    EXPECT_EQ(TupleElem4::executed, 1);
}
