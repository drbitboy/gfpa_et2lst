#include <iomanip>
#include <iostream>
#include "csppice.h"

int
main(int argc, char** argv) {

  // Furnsh kernels from command line arguments
  for (int iarg=1; iarg<argc; ++iarg) {
    furnsh_c(argv[iarg]);
  }

  // Use InSight landing site ID to build LMST SCLK (Sols) KPV; get coeffs
  BODS2C i_site = BODS2C("INSIGHT_LANDING_SITE");
  REPMI sclk_coeffs_name = REPMI("SCLK01_COEFFICIENTS_#","#",-i_site.code(),99);
  GDPOOL sclk_coeffs = GDPOOL(sclk_coeffs_name.out(),0,3);

  // There shuld only be three coefficients
  assert(sclk_coeffs.n()==3);

  // Mars ID (499)
  BODS2C i_mars = BODS2C("MARS");
  assert(i_mars.code()==499);

  // ET for LMST=0; ET seconds per Sol
  SpiceDouble et0 = sclk_coeffs.values()[1];
  SpiceDouble s_per_sol = sclk_coeffs.values()[2];

  // Parameters for GFPA
  // - adjust, step
  SpiceDouble f_adjust = 0.;
  SpiceDouble f_step = s_per_sol / 10.0;

  // - ET range for cnfine:  687 Sols; greater than 1y (Martian)
  SpiceDouble et_lo = et0 + f_step;
  SpiceDouble et_hi = et0 + (s_per_sol * 687);

  // - number of intervals
  SpiceInt nintvls =  2 + ((int)(5 + ((et_hi - et_lo) / f_step)));

  // Initialize window containing start and end ETs
  SDCELL cnfine(2);
  WNINSD(et_lo,et_hi,cnfine);

  // Result window
  SDCELL result(2000);

  // GFPA:  ETs of [Landing site-Mars-Sun] phase angle local maxima
  // - This will be near midnight for each Sol
  GFPA("MARS","SUN","LT+S","INSIGHT_LANDING_SITE","LOCMAX"
      ,0.0,f_adjust,f_step,nintvls,cnfine,result
      );

  // GFPA:  ETs of [Landing site-Mars-Sun] phase angle local maxima
  // - This will be near midnight for each Sol
  GFPA("MARS","SUN","LT+S","INSIGHT_LANDING_SITE","LOCMAX"
      ,0.0,f_adjust,f_step,nintvls,cnfine,result
      );

  // Result window for GFPOSC
  SDCELL rsltps(2000);

  // Midnight longitude of Sun => vector from InSight landing site
  // toward the center of mars
  SpiceDouble mn_lon = RECSPH(SPKEZR("MARS",WNFETD(result,0).left(),"IAU_MARS","NONE","INSIGHT_LANDING_SITE").pos()).lon();

  // GFPOSC:  ETs of Sun with longitude opposite InSight
  GFPOSC("SUN","IAU_MARS","LT+S","MARS","SPHERICAL","LONGITUDE","="
        ,mn_lon,f_adjust,f_step,nintvls,cnfine,rsltps
        );

  // The result count must be the same
  assert(WNCARD(result)==WNCARD(rsltps));

  // Initialize previous LST (Local Solar Time) to nothing, & repeats
  SCVEC last_lst(99,'\0');
  int repeats = 0;

  std::cout << "ltst_count_pairs=list()" << std::endl;
  std::cout << "error_sunposz_sunvelz=list()" << std::endl;

  // Loop over results
  SpiceInt ncard = WNCARD(result); 
  for (SpiceInt icard=0; icard<ncard; ++icard) {

  // Get ET, calculate west longitude (Mars rotation is prograde)
  SpiceDouble et = WNFETD(result,icard).left();
  SpiceDouble wlon = - RECRAD(VMINUS(SPKEZR("MARS",et,"IAU_MARS","LT+S","INSIGHT_LANDING_SITE").pos()).vout()).ra();

  // Hmmmm test:  LT+S and NONE results differ by less than a nanoradian
  SpiceDouble wlon2 = - RECRAD(VMINUS(SPKEZR("MARS",et,"IAU_MARS","NONE","INSIGHT_LANDING_SITE").pos()).vout()).ra();
  assert(((wlon2-wlon)<2e-10) && (wlon<wlon2));

  SpiceDouble et_posc = WNFETD(rsltps,icard).left();
  SPKEZR sunez("SUN",et_posc,"IAU_MARS","LT+S","MARS");

    std::cout
    << "error_sunposz_sunvelz.append("
    << "(" << (et-et_posc) << "," << sunez.pos()[2] << "," << sunez.vel()[2] << ",))"
    << std::endl;

    // Calculate LST wrt that west lon at ET
    ET2LST lst(et,i_mars.code(),wlon,"PLANETOGRAPHIC",99,99);

    if (repeats && EQSTR(lst.v_time(), last_lst)) {
      // Incrmeent repeats if LST is same as last pass through loop
      ++repeats;
    } else {
      // If repeats is zero (first loop pass) or LST is different ...
      if (repeats > 0) {
        // Output last LST along with how many times that LST occured
        std::cout
        << "ltst_count_pairs.append("
        << "('" << last_lst.data() << "'," << repeats << ",))"
        << std::endl;
      }
      // Set repeats to 1, and copy newly calculate LST to last_lst
      repeats = 1;
      SpiceChar* p_time = lst.time();
      for (int ipos=0; ipos<99; ++ipos) {
        last_lst[ipos] = p_time[ipos];
      }
    }
  } // End of loop over results

  // Output remaining LST
  std::cout
  << "ltst_count_pairs.append("
  << "('" << last_lst.data() << "'," << repeats << ",))"
  << std::endl;

  // Output miscellaneous info
  std::cout << std::setprecision(15) << std::endl
  << "et0,s_per_sol,result_count=" <<et0
  << "," << s_per_sol
  << "," << WNCARD(result)
  << std::endl;

  return 0;
}
