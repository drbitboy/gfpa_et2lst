Minimal InSight SPICE Meta-Kernel (MK) for

  https://github.com/drbitboy/gfpa_et2lst

\begindata

PATH_SYMBOLS = ( 'TOP' )
PATH_VALUES   = ( 'kernels' )

KERNELS_TO_LOAD = (
  '$TOP/spk/mar097s.bsp'
  '$TOP/spk/insight_ls_2016e09o_iau2000_v1.bsp'
  '$TOP/fk/insight_v02.tf'
  '$TOP/sclk/insight_lmst_2016e09o_v1.tsc'
  '$TOP/pck/pck00010.tpc'
  '$TOP/lsk/naif0012.tls'
)
\begintext
