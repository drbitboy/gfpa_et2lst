"""
Usage:

  ./ge ismws.tm > insight_midnights_data.py
  python plot_insight_midnights_data.py

"""
  
from insight_midnights_data import *
import matplotlib.pyplot as plt

errs,sunposz,sunvelz=zip(*error_sunposz_sunvelz)

plt.plot(sunvelz,errs,'-r')
plt.plot(sunvelz[0],errs[0],'ob')
plt.plot(sunvelz[-1],errs[-1],'ok')
plt.axhline()
plt.axvline()
plt.xlabel('Sun velocity in IAU_MARS Z, km/s')
plt.ylabel('ET error, GFPA(LOCMAX) - GFPOSC(=LONGITUDE), s')
plt.show()

plt.plot(sunposz,errs,'-r')
plt.plot(sunposz[0],errs[0],'ob')
plt.plot(sunposz[-1],errs[-1],'ok')
plt.axhline()
plt.axvline()
plt.xlabel('Sun position in IAU_MARS Z, km')
plt.ylabel('ET error, GFPA(LOCMAX) - GFPOSC(=LONGITUDE), s')
plt.show()

vxps=[vel*pos for err,vel,pos in error_sunposz_sunvelz]

plt.plot(vxps,errs,'-r')
plt.plot(vxps[0],errs[0],'ob')
plt.plot(vxps[-1],errs[-1],'ok')
plt.axhline()
plt.axvline()
plt.xlabel('([Sun position] x [Sun velocity] in IAU_MARS Z, km**2 s**-1')
plt.ylabel('ET error, GFPA(LOCMAX) - GFPOSC(=LONGITUDE), s')
plt.show()
