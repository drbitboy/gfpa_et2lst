#ifndef __CSPPICE_H__
#define __CSPPICE_H__

////////////////////////////////////////////////////////////////////////
// Minimal prototype C++ interface to JPL NAIV CSPICE toolkit
// - I don't like it per se, it was merely an idea
////////////////////////////////////////////////////////////////////////

// Vector and assertion header files
#include <vector>
#include <assert.h>

// CSPICE header file
#include "SpiceUsr.h"

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// General approach
// ----------------
//
//   Interface CSPICE routine xyz_c(in_arg_1, &out_arg_1) via class XYZ:
//
//     class XYZ {
//
//     private:
//
//         // Store output arguments in class, in private
//         SpiceType _out_arg_1;
//
//     public:
//
//         XYZ(in_arg_1) {
//
//             // Call CSPICE routine in constructor to write outputs
//             xyz_c(in_arg_1, &_out_arg_1);
//
//         }
//
//         ~XYZ() { }      // Destructor will typically be empty
//
//         // Use public wrappers to retrieve private output values
//         SpiceType out_arg_1() { return _out_arg_1; } 
//     
//     } // End class XYZ
//
//
// Usage
// -----
//
//     XYZ xyz(in_arg_1,...,in_arg_n);
//
//     SpiceType xyz_result_1 = xyz.out_arg_1();
//
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// Implement (Spice[Double|Char|Int]*) pointers via vectors
// - I.e. std::vector<T>.data() is a (T*) pointer
// - Ugly, but bypasses need for new/delete shenanigans
// - Warning:  this bypasses some vector housekeeping
// - Warning:  this assumes vector<T>(n) constructor allocates memory
typedef std::vector<SpiceDouble> SDVEC;
typedef std::vector<SpiceChar> SCVEC;
typedef std::vector<SpiceInt> SIVEC;

////////////////////////////////////////////////////////////////////////
// Inteface to gipool_c
class GIPOOL {
private:
  // Outputs
  SIVEC _values;
  SpiceInt _n;
  SpiceBoolean _found;

public:

  // Constructor
  GIPOOL(ConstSpiceChar* name
        ,SpiceInt        start
        ,SpiceInt        room
        ,SpiceBoolean    skip_assert=SPICEFALSE  // Just an idea
        ) {

    // Initialize output vector
    _values = SIVEC(room);

    // Call CSPICE routine
    // N.B. _values.data() is (SpiceInt*) pointer to memory reserved for
    //      vector<SpiceInt> _values (in private above)
    // N.B. Writing to _values.data() bypasses vector housekeeping
    gipool_c(name,start,room               // Inputs
            ,&_n,_values.data(),&_found);  // Outputs

    // Optional housekeeping
    if (skip_assert) {
      assert(_found==SPICETRUE);
    }
  }

  // Destructor
  ~GIPOOL(){};

  // Methods to retrieve outputs

  // - (SpiceInt*) pointer _values
  SpiceInt* values() { return _values.data(); }

  // - Number of values returned
  SpiceInt n() { return _n; }

  // - Found boolean
  SpiceBoolean found() { return _found; }

  // Special; may not be useful
  SIVEC& v_values() { return _values; }
};

////////////////////////////////////////////////////////////////////////
// Inteface to gdpool_c
class GDPOOL {
private:
  // Outputs
  SDVEC _values;
  SpiceInt _n;
  SpiceBoolean _found;

public:
  GDPOOL(ConstSpiceChar* name
        ,SpiceInt start
        ,SpiceInt room
        ,SpiceBoolean skip_assert=SPICEFALSE
        ) {
    _values = SDVEC(room);
    gdpool_c(name,start,room,&_n,_values.data(),&_found);
    if (skip_assert) {
      assert(_found==SPICETRUE);
    }
  }
  ~GDPOOL(){};

  SDVEC& v_values() { return _values; }
  SpiceDouble* values() { return _values.data(); }
  SpiceInt n() { return _n; }
  SpiceBoolean found() { return _found; }
};

////////////////////////////////////////////////////////////////////////
// Inteface to bods2c_c
class BODS2C {
private:
  // Outputs
  SpiceInt _code;
  SpiceBoolean _found;

public:
  BODS2C(ConstSpiceChar* name
        ) {
    bods2c_c(name,&_code,&_found);
  }
  ~BODS2C(){};

  SpiceInt code() { return _code; }
  SpiceBoolean found() { return _found; }
};

////////////////////////////////////////////////////////////////////////
// Inteface to repmi_c
class REPMI {
private:
  // Outputs
  SCVEC _out;

public:
  REPMI(ConstSpiceChar* in
       ,ConstSpiceChar* marker
       ,SpiceInt value
       ,SpiceInt lenout
       ) {
    _out = SCVEC(lenout);
    repmi_c(in,marker,value,lenout,_out.data());
  }
  ~REPMI(){};

  SCVEC& v_out() { return _out; }
  SpiceChar* out() { return _out.data(); }
};

////////////////////////////////////////////////////////////////////////
// Interface to SpiceDouble cells
class SDCELL {
private:
  SpiceCell _cell;
  SpiceDouble* _vec;

public:
  SDCELL(SpiceInt size
  ) {
    _vec = new SpiceDouble[SPICE_CELL_CTRLSZ + size];
    _cell.dtype = SPICE_DP;
    _cell.length = 0;
    _cell.size = size;
    _cell.card = 0;
    _cell.isSet = SPICETRUE;
    _cell.adjust = SPICEFALSE;
    _cell.init = SPICEFALSE;
    _cell.base = _vec;
    _cell.data = (_vec + SPICE_CELL_CTRLSZ);
  }
  ~SDCELL() { delete _vec; }
  SpiceCell& cell() { return _cell; }
  SpiceCell* cell_ptr() { return &_cell; }
};

////////////////////////////////////////////////////////////////////////
// Interface to wninsd_c
class WNINSD {
private:
public:
  WNINSD(SpiceDouble left
        ,SpiceDouble right
        ,SDCELL& wn
        ) {
    wninsd_c(left,right,wn.cell_ptr());
  }
};

////////////////////////////////////////////////////////////////////////
// Interface to wncard_c is a simple wrapper
static inline SpiceInt WNCARD(SDCELL& wn) {
  return wncard_c(wn.cell_ptr());
};

////////////////////////////////////////////////////////////////////////
// Interface to gfpa_c
class GFPA {
private:
public:
  GFPA(ConstSpiceChar     * target
      ,ConstSpiceChar     * illmn
      ,ConstSpiceChar     * abcorr
      ,ConstSpiceChar     * obsrvr
      ,ConstSpiceChar     * relate
      ,SpiceDouble          refval
      ,SpiceDouble          adjust
      ,SpiceDouble          step
      ,SpiceInt             nintvls
      ,SDCELL             & cnfine
      ,SDCELL             & result
      ) {
    gfpa_c(target,illmn,abcorr,obsrvr
          ,relate,refval,adjust,step,nintvls
          ,cnfine.cell_ptr(),result.cell_ptr()
          );
  }
};

////////////////////////////////////////////////////////////////////////
// Interface to et2lst_c
class ET2LST {
private:
  SpiceInt _hr;
  SpiceInt _mn;
  SpiceInt _sc;
  SCVEC _time;
  SCVEC _ampm;
public:
  ET2LST(SpiceDouble et
        ,SpiceInt body
        ,SpiceDouble lon
        ,ConstSpiceChar* type
        ,SpiceInt timelen
        ,SpiceInt ampmlen
        ) {
    _time = SCVEC(timelen);
    _ampm = SCVEC(ampmlen);
    et2lst_c(et,body,lon,type,timelen,ampmlen
            ,&_hr,&_mn,&_sc,_time.data(),_ampm.data()
            );
  }
  ~ET2LST(){}
  SpiceInt hr() { return _hr; }
  SpiceInt mn() { return _mn; }
  SpiceInt sc() { return _sc; }
  SCVEC& v_time() { return _time; }
  SpiceChar* time() { return _time.data(); }
  SCVEC& v_ampm() { return _ampm; }
  SpiceChar* ampm() { return _ampm.data(); }
};

////////////////////////////////////////////////////////////////////////
class WNFETD {
// Interface to wnfetd_c
private:
  SpiceDouble _left;
  SpiceDouble _right;
public:
  WNFETD(SDCELL &window
        ,SpiceInt n
        ) {
    wnfetd_c(window.cell_ptr(),n,&_left,&_right);
  }
  ~WNFETD(){}
  SpiceDouble left() { return _left; }
  SpiceDouble right() { return _right; }
};

////////////////////////////////////////////////////////////////////////
SpiceBoolean EQSTR(SCVEC& a,SCVEC& b) {
  return eqstr_c(a.data(),b.data());
}

////////////////////////////////////////////////////////////////////////
// Interface to spkezr_c
class SPKEZR {
private:
  SDVEC _state;
  SpiceDouble _lighttime;
public:
  SPKEZR(ConstSpiceChar* target
        ,SpiceDouble et
        ,ConstSpiceChar* ref
        ,ConstSpiceChar* abcorr
        ,ConstSpiceChar* obs
        ) {
    _state = SDVEC(6);
    spkezr_c(target,et,ref,abcorr,obs,_state.data(),&_lighttime);
  }
  ~SPKEZR(){}
  SDVEC& v_state() { return _state; }
  SpiceDouble* state() { return _state.data(); }
  SpiceDouble lighttime() { return _lighttime; }
  SpiceDouble* pos() { return _state.data(); }
  SpiceDouble* vel() { return _state.data()+3; }
};

////////////////////////////////////////////////////////////////////////
// Interface to vminus_c
class VMINUS {
private:
  SDVEC _vout;
public:
  VMINUS(ConstSpiceDouble* v1
        ) {
    _vout = SDVEC(3);
    vminus_c(v1,_vout.data());
  }
  ~VMINUS(){}
  SDVEC& v_vout() { return _vout; }
  SpiceDouble* vout() { return _vout.data(); }
};

////////////////////////////////////////////////////////////////////////
// Interface to recrad_c
class RECRAD {
private:
  SpiceDouble _range;
  SpiceDouble _ra;
  SpiceDouble _dec;
public:
  RECRAD(ConstSpiceDouble rectan[3]
        ) {
    recrad_c(rectan,&_range,&_ra,&_dec);
  }
  ~RECRAD(){}
  SpiceDouble range() { return _range; }
  SpiceDouble ra() { return _ra; }
  SpiceDouble dec() { return _dec; }
};

#endif // __CSPPICE_H__
