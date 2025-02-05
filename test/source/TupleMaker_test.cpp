#include <gtest/gtest.h>

#include "TupleMaker/TupleMaker.hpp"

#include <TFile.h>

TEST(TestsCreate, create_invalid)
{
    ASSERT_THROW(tuma::TupleMaker tmk(nullptr), std::invalid_argument);
}

TEST(TestsCreate, create_valid)
{
    auto tree = std::make_unique<TTree>("T", "Tuple");
    tuma::TupleMaker tmk(tree.get());

    auto t1 = tmk.add_track<tuma::predef::LorentzVectorTrack>("p1");
    auto t2 = tmk.add_track<tuma::predef::LorentzVectorTrack>("p2", 0x1);
    auto t3 = tmk.add_track<tuma::predef::LorentzVectorTrack>("p3", 0x10);

    ASSERT_TRUE(t1);
    ASSERT_TRUE(t2);
    ASSERT_TRUE(t3);
}

TEST(TestsCreate, fill_no_fill)
{
    auto tree = std::make_unique<TTree>("T", "Tuple");
    tuma::TupleMaker tmk(tree.get());

    tmk.add_track<tuma::predef::LorentzVectorTrack>("p1");

    ASSERT_THROW(tmk.check_and_fill(), std::runtime_error);
}

TEST(TestsCreate, fill_no_fill_non_fatal)
{
    auto tree = std::make_unique<TTree>("T", "Tuple");
    tuma::TupleMaker tmk(tree.get());

    tmk.add_track<tuma::predef::LorentzVectorTrack>("p1");

    tmk.make_errors_fatal(false);

    auto res = tmk.check_and_fill();
    ASSERT_FALSE(res);
}

TEST(TestsCreate, fill_fill)
{
    auto tree = std::make_unique<TTree>("T", "Tuple");
    tuma::TupleMaker tmk(tree.get());

    auto t1 = tmk.add_track<tuma::predef::LorentzVectorTrack>("p1");

    TLorentzVector vec1(1, 2, 3, 4);
    t1->fill(&vec1);

    tmk.make_errors_fatal(true);

    auto res = tmk.check_and_fill();
    ASSERT_TRUE(res);
}

TEST(TestsCreate, fill_fill_multiple)
{
    auto tree = std::make_unique<TTree>("T", "Tuple");
    tuma::TupleMaker tmk(tree.get());

    auto t1 = tmk.add_track<tuma::predef::LorentzVectorTrack>("p1");

    t1->use_angles_deg(true);

    TLorentzVector vec1(1, 2, 3, 4);
    t1->fill(&vec1);

    ASSERT_NO_THROW(tmk.check_and_fill());
    ASSERT_THROW(tmk.check_and_fill(), std::runtime_error);

    t1->clear();

    ASSERT_NO_THROW(tmk.check_and_fill());
}

TEST(TestsCreate, angles_use)
{
    auto tree = std::make_unique<TTree>("T", "Tuple");
    tuma::TupleMaker tmk(tree.get());

    auto t1 = tmk.add_track<tuma::predef::LorentzVectorTrack>("p1");
    auto t2 = tmk.add_track<tuma::predef::LorentzVectorTrack>("p2");

    TLorentzVector vec1(1, 2, 3, 4);
    TLorentzVector vec2(1, 2, 3, 4);

    t1->use_angles_deg(true);
    t2->use_angles_deg(true);

    t1->fill(&vec1);
    t2->fill(&vec2);

    tmk.check_and_fill();

    t1->use_angles_deg(false);
    t2->use_angles_deg(true);

    t1->fill(&vec1);
    t2->fill(&vec2);

    tmk.check_and_fill();

    double angle1, angle2;
    tree->GetBranch("fp1Theta")->SetAddress(&angle1);
    tree->GetBranch("fp2Theta")->SetAddress(&angle2);

    tree->GetEntry(0);
    ASSERT_DOUBLE_EQ(angle1, angle2);

    tree->GetEntry(1);
    // ASSERT_DOUBLE_NE(angle1, angle2);
}
