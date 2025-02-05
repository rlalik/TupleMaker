#include <TupleMaker/TupleMaker.hpp>

#include <hforwardcand.h>
#include <hparticlecand.h>
#include <hvirtualcand.h>

#include <TFile.h>
#include <TTree.h>

#include <memory>

class VirtualCandTrack : public tuma::predef::LorentzVectorTrack
{
public:
    virtual auto fill(HVirtualCand* track) -> void
    {
        LorentzVectorTrack::fill(track);
        fR = track->getR();
        fZ = track->getZ();
    }

    enum HadesBranches : unsigned int
    {
        bDSinfo = 1 << 10,
        // bdEdx = 1 << 11,
    };

    virtual auto set_tree(TTree* source, const char* name, UInt_t flags = 0x0) -> bool
    {
        bool verbose = false;

        if (!source) {
            return false;
        }

        LorentzVectorTrack::set_tree(source, name, flags);

        if (flags & bDSinfo) {
            tuma::create_branch(source, name, "R", fR, 'F', verbose);
            tuma::create_branch(source, name, "Z", fZ, 'F', verbose);

            // These are only for HParticleCand
            // tuma::create_branch(source, name, "System", fSystem, 'S', verbose);
            // tuma::create_branch(source, name, "Charge", fCharge, 'S', verbose);
            // tuma::create_branch(source, name, "TofRec", fTofRec, 'S', verbose);
            // tuma::create_branch(source, name, "MDCdEdx", fMDCdEdx, 'F');
            // tuma::create_branch(source, name, "TOFdEdx", fTOFdEdx, 'F');
        }

        return true;
    }

private:
    HVirtualCand* track;

    Float_t fR {0}, fZ {0};

    // For HParticleCand only
    // Short_t fSystem {0};
    // Short_t fCharge {0};
    // Short_t fTofRec {0};
    // Float_t fMDCdEdx {0.0};
    // Float_t fTOFdEdx {0.0};
};

auto main() -> int
{
    auto file = std::unique_ptr<TFile>(TFile::Open("Test.root", "RECREATE"));

    auto tree = std::make_unique<TTree>("T", "Tuple");
    tuma::TupleMaker tmk(tree.get());

    HParticleCand vec1;
    HForwardCand vec2;

    // Add track to the tree but only with three branches
    auto t1 = tmk.add_track<VirtualCandTrack>(
        "_p1_",
        tuma::predef::LorentzVectorTrack::bE | tuma::predef::LorentzVectorTrack::bM
            | tuma::predef::LorentzVectorTrack::bPhi | tuma::predef::LorentzVectorTrack::bTheta
            | tuma::predef::LorentzVectorTrack::bCosTheta);
    auto t2 = tmk.add_track<VirtualCandTrack>(
        "_p2_",
        tuma::predef::LorentzVectorTrack::bE | tuma::predef::LorentzVectorTrack::bM
            | tuma::predef::LorentzVectorTrack::bPhi | tuma::predef::LorentzVectorTrack::bTheta
            | tuma::predef::LorentzVectorTrack::bCosTheta);

    t1->use_angles_deg(true);
    t2->use_angles_deg(false);

    const auto n = 3u;
    for (int i = 0; i < n; ++i) {
        tmk.reset();

        vec1.calc4vectorProperties(100 + i);
        vec2.calc4vectorProperties(200 + i);

        t1->fill(&vec1);
        t2->fill(&vec2);

        tmk.check_and_fill();
    }

    tmk.reset();

    vec1.SetE(9999);
    t1->fill(&vec1);  // assure that the track is taken care of (no random values)
    t2->clear();

    tmk.check_and_fill();

    tree->Write();
    file->Write();

    printf("Example tree filled, checking it:\n");

    TBranch* b1 = tree->GetBranch("f_p1_E");
    TBranch* b2 = tree->GetBranch("f_p2_E");

    double e1, e2;
    b1->SetAddress(&e1);
    b2->SetAddress(&e2);

    for (int i = 0; i < n; ++i) {
        tree->Show(i);
        printf("E1 = %f  E2 = %f\n", e1, e2);
    }
}
