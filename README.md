# aac_enc_dec
aac enc dec by fdk_aac

from:
#https://github.com/mstorsjo/fdk-aac

https://blog.csdn.net/u013692429/article/details/99680818

解码：
http://blog.chinaunix.net/uid-25272011-id-3491488.html
可以将aac  解码成pcm和wav格式


aac enc:
./aac_enc founders.wav out.aac
fdk_aac aac编码时，已经自动添加ADTS.这7个字节在编码前面
