import vapoursynth as vs
import havsfunc as haf
import mvsfunc as mvf
import qvs # check out https://github.com/emako/qvs/releases

core = vs.get_core()
src = core.lsmas.LWLibavSource(u'src.mp4')
src = core.vsf.TextSub(src, file=u'src.ass')
#src = core.vsfm.TextSubMod(src, file=u'src.ass')
#src = qvs.TcasSub(src, file=u'src.tcas')
src.set_output()
