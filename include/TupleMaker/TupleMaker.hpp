#pragma once

#include <TLorentzVector.h>
#include <TMath.h>
#include <TTree.h>

#include <exception>
#include <functional>
#include <memory>

namespace tuma
{

/**
 * Base structure for any track/object should be stored in the tree
 * See tuma:predef:LorentzVectorTrack for example.
 */
struct TrackBase
{
    bool is_ready {false};
};

/**
 * TupleTrack make all magic when registering new leafs for given object.
 */
struct TupleTrack
{
    std::string name;                  /// name of the object, used as aprt of the leaf naming scheme
    std::shared_ptr<void> any_track;   /// store the track for future uses
    std::function<bool&()> get_ready;  /// to return the ready flag

    /*
     * For given tree, create leafs using obj_name, can use extra args.
     *
     * The template parameter T is a class or structure having an recipe for how to fill the tree leafs with data. We
     * don;t know it here. See tuma:predef:LorentzVectorTrack for example and also `examples/` dir for more.
     */
    template<typename T, typename... Ts>
    auto create(TTree* tree, const char* obj_name, Ts... args) -> T*
    {
        name = obj_name;

        T* tt_object = new T();
        tt_object->set_tree(tree, obj_name, args...);
        any_track = std::shared_ptr<void>(tt_object);

        get_ready = [&]() -> bool& { return dynamic_cast<TrackBase*>(static_cast<T*>(any_track.get()))->is_ready; };

        return tt_object;
    }
};

class TupleMaker
{
public:
    TupleMaker(TTree* source)
        : tree {source}
    {
        if (!source) {
            throw std::invalid_argument("Tree must be given");
        }
    }

    template<typename T, typename... Ts>
    auto add_track(const char* obj_name, Ts... args) -> T*
    {
        auto track_ptr = std::make_unique<TupleTrack>();
        auto ret = track_ptr->create<T>(tree, obj_name, args...);

        tracks.push_back(std::move(track_ptr));
        return ret;
    }

    auto reset() -> void
    {
        for (auto& t : tracks) {
            t->get_ready() = false;
        }
    }

    auto check_and_fill() -> bool
    {
        bool check_result {true};

        for (const auto& t : tracks) {
            if (!t->get_ready()) {
                fprintf(stderr, "Branch %s neither filled or cleared\n", t->name.c_str());
                check_result = false;
            }
        }

        if (check_result) {
            tree->Fill();
        } else {
            if (errors_fatal) {
                throw std::runtime_error("Uninitialzied tree leafs");
            }
        }

        reset();

        return check_result;
    }

    constexpr auto make_errors_fatal(bool set_fatal) -> void { errors_fatal = set_fatal; }

private:
    TTree* tree {nullptr};
    std::vector<std::unique_ptr<TupleTrack>> tracks;
    bool errors_fatal {true};
};

template<typename T>
auto create_branch(TTree* source, const char* name, const char* varname, T& var, char type) -> void
{
    const auto str = TString::Format("f%s%s", name, varname);
    const auto str_t = TString::Format("%s/%c", str.Data(), type);
    source->Branch(str.Data(), &var, type);
}

namespace predef
{

class LorentzVectorTrack : public TrackBase
{
public:
    virtual ~LorentzVectorTrack() = default;

    virtual auto use_angles_deg(bool use_deg) -> void { fr2d = use_deg ? TMath::RadToDeg() : 1.0; }

    virtual auto fill(TLorentzVector* track) -> void
    {
        fE = track->E();
        fM = track->M();
        fP = track->P();
        fPx = track->Px();
        fPy = track->Py();
        fPz = track->Pz();
        fTheta = track->Theta() * fr2d;
        fCosTheta = TMath::Cos(track->Theta());
        fPhi = track->Phi() * fr2d;
        fY = track->Rapidity();
        fPt = track->Pt();
        fBeta = track->Beta();

        is_ready = true;
    }

    virtual auto clear() -> void
    {
        fE = 0;
        fM = 0;
        fP = 0;
        fPx = 0;
        fPy = 0;
        fPz = 0;
        fTheta = 0;
        fCosTheta = 0;
        fPhi = 0;
        fY = 0;
        fPt = 0;
        fBeta = 0;

        is_ready = true;
    }

    enum Branches : unsigned int
    {
        bE = 1 << 0,
        bM = 1 << 1,
        bP = 1 << 2,
        bTheta = 1 << 3,
        bCosTheta = 1 << 4,
        bPhi = 1 << 5,
        bY = 1 << 6,
        bPt = 1 << 7,
        bPv = 1 << 8,
        bBeta = 1 << 9,
    };

    virtual auto set_tree(TTree* source, const char* name, UInt_t flags = 0x0) -> bool
    {
        if ((flags & bE) | !flags) {
            create_branch(source, name, "E", fE, 'F');
        }
        if ((flags & bM) | !flags) {
            create_branch(source, name, "M", fM, 'F');
        }
        if ((flags & bP) | !flags) {
            create_branch(source, name, "P", fP, 'F');
        }
        if ((flags & bTheta) | !flags) {
            create_branch(source, name, "Theta", fTheta, 'F');
        }
        if ((flags & bCosTheta) | !flags) {
            create_branch(source, name, "CosTheta", fCosTheta, 'F');
        }
        if ((flags & bPhi) | !flags) {
            create_branch(source, name, "Phi", fPhi, 'F');
        }
        if ((flags & bY) | !flags) {
            create_branch(source, name, "Y", fY, 'F');
        }
        if ((flags & bPt) | !flags) {
            create_branch(source, name, "Pt", fPt, 'F');
        }
        if ((flags & bPv) | !flags) {
            create_branch(source, name, "Px", fPx, 'F');
            create_branch(source, name, "Py", fPy, 'F');
            create_branch(source, name, "Pz", fPz, 'F');
        }
        if ((flags & bBeta) | !flags) {
            create_branch(source, name, "Beta", fBeta, 'F');
        }

        return true;
    }

private:
    Double_t fr2d {1.0};

    Double_t fE {0}, fM {0}, fP {0};
    Double_t fPx {0}, fPy {0}, fPz {0};
    Double_t fTheta {0}, fCosTheta {0}, fPhi {0};
    Double_t fY {0}, fPt {0};
    Double_t fBeta {0};
};
}  // namespace predef

}  // namespace tuma
