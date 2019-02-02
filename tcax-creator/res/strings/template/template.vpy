import vapoursynth as vs
import havsfunc as haf
import mvsfunc as mvf

core = vs.get_core()
src = core.lsmas.LWLibavSource(u'src.mp4')
src = core.vsf.TextSub(src, file=u'src.ass')
#src = core.vsfm.TextSubMod(src, file=u'src.ass')
#src = core.tcax.TcasSub(src, file=u'src.tcas')
src.set_output()
