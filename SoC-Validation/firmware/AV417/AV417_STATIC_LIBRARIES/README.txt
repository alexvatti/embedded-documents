
libct.a       - MetaWare C Runtime library
                (Contains Metaware startup code,time functions, string, math operatins etc) 
libcct.a      - MetaWare C++ Runtime libraries
libhlt.a      - Hostlink I/O library (removed to use stubs in libmw.a with
                 -Hhostlib=
libeltx.a     - ThreadX RTOS support for MetaWare thread-safe C/C++ library.
                If you rebuild the libraries, this library is not built by
                default.  Typical build command:
libmqx.a      - MQX RTOS support for MetaWare thread-safe C/C++ library.  
                you rebuild the libraries, this library is not built by
                default.  Typical build command
libms.a       - ARC600/ARC700 Floating point hardware single precision math
                 library (compiled with -Xspfp)
libmsd.a      - ARC600/ARC700 Floating point hardware single and double
                 precision math library (compiled with -Xspfp -Xdpfp)
libmw.a       - MetaWare toolset support libraries (helper functions used
                 by the compiler, etc.)
libaom.a      - Automatic Overlay Manager runtime library



/*************************************
 **steps to generate above libraries**
 *************************************/
1.libct.a
  (Go to the directory MetaWare\arc\lib\src\c\archive\a7_p\lobj_o)(Metaware Installed Directory)
  run gmake -f Makefile
2.libcct.a
  (Go to the directory MetaWare\arc\lib\src\cpp\archive\a7_p\lobj_o)
  run gmake -f Makefile
3.libhlt.a
  (Go to the directory MetaWare\arc\lib\src\hl\archive\a7_p\lobj_o)
  run gmake -f Makefile
4.libeltx.a
  (Go to the directory MetaWare\arc\lib\src\eltx\archive\a7_p\lobj_o)
  run gmake -f Makefile
5.libmqx.a
  (Go to the directory MetaWare\arc\lib\src\mqx\archive\a7_p\lobj_o)
  run gmake -f Makefile
6.libms.a
  (Go to the directory MetaWare\arc\lib\src\m\archive\a7_p\lsp_o)
  run gmake -f Makefile
7.libmw.a
  (Go to the directory E:\MetaWare\arc\lib\src\mw\archive\a7_p\lobj_o)
  run gmake -f Makefile
8.libaom.a
  (Go to the directory MetaWare\arc\lib\src\aom\archive\a7_p\lobj_o)

  run gmake -f Makefile

/***********************************************
 ***************codec Libraries*****************
 ***********************************************/
1.libh264dec.a
  (H264 Decoderlibrary) - contains hardware accelerators(Motion compensation entopy decoding funtions)
2.libh264_encoder_app.a
  (H264 Encoderlibrary) - contains hardware accelarators(Motion Estimation,simd, entropy encoding functions)
3.libmpegdec.a
  (MPEG4 decoder library)
4.libmpeg4_encoder_app.a
  (MPEG4 Encoder library)

/************steps to generate above libraries*****************/

1.libh264dec.a
  (GO to the directory AV417_NEW\standalone_codecs\H264_Decoder)
  run gmake -f Makefile       (For windows)
  run gmake -f Makefile_Linux (For Linux)
2.libh264_encoder_app.a
  (GO to the directory AV417_NEW\standalone_codecs\H264_Encoder)
  run gmake -f Makefile.win32.h264 (For windows)
  run gmake -f Makefile.linux.h264 (For linux)
3.libmpegdec.a
  (GO to the directory AV417_NEW\standalone_codecs\MPEG-4_Decoder)
  run gmake -f Makefile       (For windows)
  run gmake -f Makefile_Linux (For Linux)
4.libmpeg4_encoder_app.a
  (GO to the directory AV417_NEW\standalone_codecs\MPEG-4_ENCODER) 
  run gmake -f Makefile.win32.mpeg4 (For windows)
  run gmake -f Makefile.linux.mpeg4 (For linux)

/*****************************************************************/
