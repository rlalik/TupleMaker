#include <TupleMaker/TupleMaker.hpp>

#include <TFile.h>
#include <TLorentzVector.h>
#include <TTree.h>

#include <memory>

auto main() -> int
{
    auto file = std::unique_ptr<TFile>(TFile::Open("Test.root", "RECREATE"));

    auto tree = std::make_unique<TTree>("T", "Tuple");
    tuma::TupleMaker tmk(tree.get());

    TLorentzVector vec1(1, 2, 3, 4), vec2(1, 2, 3, 4);

    vec1.Print();
    vec2.Print();

    auto t1 = tmk.add_track<tuma::predef::LorentzVectorTrack>("p1");
    auto t2 = tmk.add_track<tuma::predef::LorentzVectorTrack>(
        "p2",
        tuma::predef::LorentzVectorTrack::bE | tuma::predef::LorentzVectorTrack::bM
            | tuma::predef::LorentzVectorTrack::bPhi | tuma::predef::LorentzVectorTrack::bTheta);

    t1->use_angles_deg(true);
    t2->use_angles_deg(false);

    const auto n = 3u;
    for (unsigned int i = 0; i < n; ++i) {
        tmk.reset();

        vec1.SetE(100 + i);
        vec2.SetE(200 + i);

        t1->fill(&vec1);
        t2->fill(&vec2);

        tmk.check_and_fill();
    }

    tmk.reset();

    vec1.SetE(9999);
    t1->fill(&vec1);
    t2->clear();  // assure that the track is taken care of (no random values)

    tmk.check_and_fill();

    tree->Write();
    file->Write();

    printf("Example tree filled, checking it:\n");

    TBranch* b1 = tree->GetBranch("fp1E");
    TBranch* b2 = tree->GetBranch("fp2E");

    double e1, e2;
    b1->SetAddress(&e1);
    b2->SetAddress(&e2);

    for (unsigned int i = 0; i < tree->GetEntriesFast(); ++i) {
        tree->Show(i);
        printf("E1 = %f  E2 = %f\n", e1, e2);
    }
}
