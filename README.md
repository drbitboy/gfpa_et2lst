# gfpa_et2lst
Compare results of SPICE toolkit routine GFPA against ET2LST 

## Overview of CSPPICE app ge.cpp, using prototype C++ interface to CSPICE)

GFPA finds ETs of local maxima of the phase angle from [InSight landing site (observer)], to [Mars (target)], to [Sun (illuminator)].

ET2LST calculates (Mars-) Local True Solar Times (LTST; also LST), wrt the longitude of InSight at those ETs.

The [ge] executable outputs the HH:MM:SS LST of the times, compressing contiguous repeat values into one line, along with the count of the repeat values.  The output looks like this:

    00:00:01 63
    00:00:00 31
    23:59:59 32
    23:59:58 49
    23:59:57 55
    [...]

The maximum phase angles should, in principle, coincide more or less exactly with LST midnights i.e. LST=00:00:00 (or perhaps 23:59:59), but in practice they are only close; the LSTs of are midnights +/= about 3s (Martian).

I suspect the difference has to do with aberration correction, but I have not been able to prove it.

Another possibility is that a phase angle maximum occurs at a slightly different point than LST midnight because the sub-solar __LAT__itude is always changing (except at solstices), and LST is essentially a measurement of longitude only that does not take latitude into account.

Brian Carcich, 2019-08-14
